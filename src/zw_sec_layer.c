/**
@file   zw_sec_layer.c - Z-wave security layer functions.

@author David Chow

@version    1.0 19-1-11  Initial release

version: 1.0
comments: Initial release
*/
#include <stddef.h>
#include <time.h>
#include "../include/zw_api_pte.h"
#include "../include/zw_security.h"

static int zwsec_tx_sm(zwnet_p  nw, uint8_t nodeid, zwsec_tx_evt_t  evt, void *data);


/**
zwsec_keys_gen - Generate authentication and encryption keys
@param[in]	nw_key	    Network key
@param[out]	auth_key	Authentication key
@param[out]	enc_key	    Encryption key
@return
*/
void zwsec_keys_gen(const uint8_t *nw_key, uint8_t *auth_key, uint8_t *enc_key)
{
    uint8_t vec[16];

    //Generate authentication key
    memset(vec, 0x55, 16);
    zwsec_aes_encrypt(vec, nw_key, auth_key);

    //Generate encryption key
    memset(vec, 0xAA, 16);
    zwsec_aes_encrypt(vec, nw_key, enc_key);
}


/**
zwsec_mac_cal - Calculate Message Authentication Code (MAC) using AES CBC mode
@param[in]	    mac_in	        Input data for the calculation
@param[in]	    auth_key	    Authentication key
@param[out]	    mac_out	        Calculated 8-byte MAC
@return  0 on success; negative error number on failure
*/
int zwsec_mac_cal(zwsec_mac_t *mac_in, uint8_t *auth_key, uint8_t *mac_out)
{
    int     blk_index;
    int     cipher_index;
    int     j;
    uint8_t *input_data;
    uint8_t msg[16];
    uint8_t temp_mac[16];

    input_data = malloc(20 + mac_in->payload_len);

    if (!input_data)
    {
        return ZW_ERR_MEMORY;
    }

    // Generate input
    //memcpy(input_data, mac_in->i_vec, 16);
    input_data[16] = mac_in->sec_hdr;
    input_data[17] = mac_in->sender_nid;
    input_data[18] = mac_in->rcvr_nid;
    input_data[19] = mac_in->payload_len;
    memcpy(input_data + 20, mac_in->payload, mac_in->payload_len);


    // Perform initial hashing
    zwsec_aes_encrypt(mac_in->i_vec, auth_key, temp_mac);
    memset(msg, 0, 16);

    blk_index = 0;

    // XOR temp_mac with any left over data and encrypt

    for (cipher_index = 16; cipher_index < (20 + mac_in->payload_len); cipher_index++)
    {
        msg[blk_index] = input_data[cipher_index];
        blk_index++;
        if (blk_index == 16)
        {
            for (j = 0; j <= 15; j++)
            {
                temp_mac[j] = msg[j] ^ temp_mac[j];
            }
            memset((uint8_t *)msg, 0, 16);
            blk_index = 0;

            zwsec_aes_encrypt(temp_mac, auth_key, temp_mac);
        }
    }

    if (blk_index != 0)
    {
        for (j = 0; j < 16; j++)
        {
            temp_mac[j] = msg[j] ^ temp_mac[j];
        }
        zwsec_aes_encrypt(temp_mac, auth_key, temp_mac);
    }

    //Return lower 8 bytes of the output
    memcpy(mac_out, temp_mac, 8);

    free(input_data);

    return ZW_ERR_NONE;
}


/**
zwsec_spp_gen - Create security payload package (SPP)
@param[in]	    mac_in	        Input data for the calculation
@param[in]	    auth_key	    Authentication key
@param[in]	    enc_key	        Encryption key
@param[out]	    spp	            Generated SPP
@param[out]	    spp_len	        Generated SPP length
@return  0 on success; negative error number on failure
*/
int zwsec_spp_gen(zwsec_mac_t *mac_in, uint8_t *auth_key, uint8_t *enc_key, uint8_t *spp, uint8_t *spp_len)
{
    int result;

    zwsec_aes_ofb(enc_key, mac_in->i_vec, mac_in->payload, mac_in->payload_len);
    result = zwsec_mac_cal(mac_in, auth_key, spp + 10 + mac_in->payload_len + 1);
    if (result == 0)
    {
        spp[0] = COMMAND_CLASS_SECURITY;
        spp[1] = mac_in->sec_hdr;
        memcpy(spp + 2, mac_in->i_vec, 8);//Sender's nonce
        memcpy(spp + 10, mac_in->payload, mac_in->payload_len);//Payload
        spp[10 + mac_in->payload_len] = mac_in->i_vec[8];   //Receiver's nonce identifier
        //Calculate SPP size
        *spp_len = 19 + mac_in->payload_len;
    }

    return result;

}


/**
zwsec_timer_chk - Check whether the timer has expired
@param[in] now     Current time
@param[in] timer   The time of the timer to check
@return 1=timer has expired; otherwise 0
*/
static int    zwsec_timer_chk(uint16_t now, uint16_t timer)
{
    uint16_t    time_diff;
    if (now == timer)
    {
        return 1;
    }

    //Handle wrap over case
    if (now > timer)
    {
        time_diff = now - timer;
        if (time_diff < 0xF000)
        {
            return 1;
        }
    }
    else //now < timer
    {
        time_diff = timer - now;
        if (time_diff >= 0xF000)
        {
            return 1;
        }
    }
    //Not expired
    return 0;
}


/**
zwsec_tmout_cb - Timer tick timeout callback
@param[in] data     Pointer to security layer context
@return
*/
static void    zwsec_tmout_cb(void *data)
{
    zwsec_layer_t   *sec_ctx = (zwsec_layer_t *)data;

    //Increment timer tick
    sec_ctx->tmr_tick++;

    //Check if external nonce has expired
    if (sec_ctx->ext_nonce.valid)
    {
        if (plt_mtx_trylck(sec_ctx->sec_mtx) == 0)//prevent deadlock that causes other timer callbacks blocked
        {
            if (zwsec_timer_chk(sec_ctx->tmr_tick, sec_ctx->ext_nonce.expiry_tick))
            {   //Expired, invalidate it
                sec_ctx->ext_nonce.valid = 0;
            }
            plt_mtx_ulck(sec_ctx->sec_mtx);
        }
    }

    //Increment internal nonce table clean up tick
    sec_ctx->clean_up_tick++;
    if (sec_ctx->clean_up_tick >= INTERNAL_NONCE_TABLE_CHECK)
    {
        int i;

        //Reset clean up tick
        sec_ctx->clean_up_tick = 0;

        //Clean up internal nonce table
        for (i=0; i<INTERNAL_NONCE_TABLE_SIZE; i++)
        {
            if (sec_ctx->inonce_tbl[i].state == INONCE_STA_VALID)
            {
                //Check whether the nonce has expired
                if (zwsec_timer_chk(sec_ctx->tmr_tick, sec_ctx->inonce_tbl[i].expiry_tick))
                {   //Expired, invalidate it
                    sec_ctx->inonce_tbl[i].state = INONCE_STA_INVALID;
                }
            }
        }
    }
}


/**
zwsec_inonce_gen - Generate an internal nonce and store it into the table
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    node_id	    Receiver node id of the generated nonce
@param[in]	    state	    The state (INONCE_STA_VALID or INONCE_STA_INIT) assigned to the generated nonce
@return  an index to the table (0 to INTERNAL_NONCE_TABLE_SIZE-1) on success; otherwise ZW_ERR_XXX
@pre Caller should lock the mutext sec_mtx before calling this function.
*/
int zwsec_inonce_gen(zwsec_layer_t *sec_ctx, uint8_t node_id, uint8_t state)
{
    int i;
    int j;
    int found;

    //Find an empty slot to store the generated nonce
    for (i=0; i<INTERNAL_NONCE_TABLE_SIZE; i++)
    {
        if (sec_ctx->inonce_tbl[i].state == INONCE_STA_INVALID)
        {
            break;
        }
    }

    if (i >= INTERNAL_NONCE_TABLE_SIZE)
    {
        return ZW_ERR_MEMORY;
    }

    while (zwsec_rand_output(&sec_ctx->prng_ctx, 8, sec_ctx->inonce_tbl[i].nonce) == 8)
    {
        //Check whether the first byte of nonce is zero
        if (sec_ctx->inonce_tbl[i].nonce[0] == 0)
        {   //Discard it and get a new nonce again
            continue;
        }

        //Search whether the first byte of nonce is in the table
        found = 0;
        for (j=0; j<INTERNAL_NONCE_TABLE_SIZE; j++)
        {
            if (sec_ctx->inonce_tbl[j].state != INONCE_STA_INVALID)
            {
                if (sec_ctx->inonce_tbl[j].nonce[0] == sec_ctx->inonce_tbl[i].nonce[0])
                {   //The first byte of nonce has already existed, generate another nonce
                    found = 1;
                    break;
                }
            }
        }

        if (found)
        {
            continue;
        }

        sec_ctx->inonce_tbl[i].rcv_nodeid = node_id;
        sec_ctx->inonce_tbl[i].expiry_tick = sec_ctx->tmr_tick + INTERNAL_NONCE_LIFE;
        sec_ctx->inonce_tbl[i].state = state;
        return i;
    }

    return ZW_ERR_FAILED;

}


/**
zwsec_inonce_expire_set - Set the expiry time of the internal nonce
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    index	    Index to the internal nonce table which is given by the zwsec_inonce_gen()
@return
*/
void zwsec_inonce_expire_set(zwsec_layer_t *sec_ctx, int index)
{
    sec_ctx->inonce_tbl[index].expiry_tick = sec_ctx->tmr_tick + INTERNAL_NONCE_LIFE;
    sec_ctx->inonce_tbl[index].state = INONCE_STA_VALID;
}


/**
zwsec_inonce_get - Get an internal nonce from the table
@param[in]	sec_ctx	        Security layer context
@param[in]	rcv_nonce_id	Receiver nonce id (the first byte of the nonce)
@return  an index to the table (0 to INTERNAL_NONCE_TABLE_SIZE-1) on success; otherwise ZW_ERR_XXX
*/
int zwsec_inonce_get(zwsec_layer_t *sec_ctx, uint8_t rcv_nonce_id)
{
    int i;

    for (i=0; i<INTERNAL_NONCE_TABLE_SIZE; i++)
    {
        if (sec_ctx->inonce_tbl[i].state == INONCE_STA_VALID)
        {
            if (sec_ctx->inonce_tbl[i].nonce[0] == rcv_nonce_id)
            {   //The first byte of nonce matches the receiver nonce id

                //Check whether the nonce has expired
                if (zwsec_timer_chk(sec_ctx->tmr_tick, sec_ctx->inonce_tbl[i].expiry_tick))
                {
                    return ZW_ERR_EXPIRED;
                }
                return i;
            }
        }
    }

    return ZW_ERR_NONCE_NOT_FOUND;
}


/**
zwsec_inonce_rm - Remove an internal nonce from the table
@param[in]	sec_ctx	        Security layer context
@param[in]	node_id	        Receiver node id of the nonce
@return
@pre Caller should lock the mutext sec_mtx before calling this function.
*/
void zwsec_inonce_rm(zwsec_layer_t *sec_ctx, uint8_t node_id)
{
    int i;

    for (i=0; i<INTERNAL_NONCE_TABLE_SIZE; i++)
    {
        if (sec_ctx->inonce_tbl[i].state != INONCE_STA_INVALID)
        {
            if (sec_ctx->inonce_tbl[i].rcv_nodeid == node_id)
            {   //Found the nonce belongs to the node id, invalidate it
                sec_ctx->inonce_tbl[i].state = INONCE_STA_INVALID;
            }
        }
    }
}


/**
zwsec_enonce_put - Store an external nonce
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    node_id	    Node id of the originator of the external nonce
@param[in]	    ext_nonce	External nonce
@return
*/
void zwsec_enonce_put(zwsec_layer_t *sec_ctx, uint8_t *ext_nonce, uint8_t node_id)
{
    plt_mtx_lck(sec_ctx->sec_mtx);
    //Check for duplicate external nonce
    if (!sec_ctx->ext_nonce.valid)
    {
        if (memcmp(sec_ctx->ext_nonce.nonce, ext_nonce, 8) == 0)
        {
            //Don't save duplicate copy that has already been used once
            plt_mtx_ulck(sec_ctx->sec_mtx);
            return;
        }
    }
    memcpy(sec_ctx->ext_nonce.nonce, ext_nonce, 8);
    sec_ctx->ext_nonce.snd_nodeid = node_id;
    sec_ctx->ext_nonce.expiry_tick = sec_ctx->tmr_tick + EXTERNAL_NONCE_LIFE;
    sec_ctx->ext_nonce.valid = 1;

    plt_mtx_ulck(sec_ctx->sec_mtx);

}


/**
zwsec_enonce_get - Get the external nonce stored previously
@param[in]	sec_ctx	        Security layer context
@param[in]	node_id	        Node id of the originator of the external nonce
@param[out]	ext_nonce	    External nonce
@return  Zero on success; otherwise ZW_ERR_XXX
*/
int zwsec_enonce_get(zwsec_layer_t *sec_ctx, uint8_t node_id, uint8_t *ext_nonce)
{
    plt_mtx_lck(sec_ctx->sec_mtx);
    if (sec_ctx->ext_nonce.valid)
    {
        if(sec_ctx->ext_nonce.snd_nodeid == node_id)
        {
            //Check whether the nonce has expired
            if (zwsec_timer_chk(sec_ctx->tmr_tick, sec_ctx->ext_nonce.expiry_tick) == 0)
            {
                memcpy(ext_nonce, sec_ctx->ext_nonce.nonce, 8);
                //Invalidate the stored external nonce since nonce can only be used once.
                sec_ctx->ext_nonce.valid = 0;

                plt_mtx_ulck(sec_ctx->sec_mtx);
                return ZW_ERR_NONE;

            }
            plt_mtx_ulck(sec_ctx->sec_mtx);
            return ZW_ERR_EXPIRED;
        }
    }

    plt_mtx_ulck(sec_ctx->sec_mtx);
    return ZW_ERR_FAILED;
}


/**
zwsec_send_raw - send command through the Z-Wave network
@param[in]	ifd	        interface
@param[in]	cmd		    command and parameters
@param[in]	len		    length of cmd in bytes
@param[in]	cb		    callback function for transmit status
@param[in]	cb_param    parameter passed to the callback function
@return	ZW_ERR_xxx
*/
static int zwsec_send_raw(zwifd_p ifd, uint8_t *cmd, int len, tx_cmplt_cb_t cb, void *cb_param)
{
    int                 result;
    appl_snd_data_t     *prm;

    prm = (appl_snd_data_t *)calloc(1, sizeof(appl_snd_data_t) + len);
    if (!prm)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwsec_send_raw memory error");
        return ZW_ERR_MEMORY;
    }

    prm->dat_len = len;
    memcpy(prm->dat_buf, cmd, len);
    prm->node_id = ifd->nodeid;
    prm->tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE;

    //Send the command without queuing
    result = zw_send_data(&ifd->net->appl_ctx, prm, cb, cb_param);
    free(prm);
    if (result != 0)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwsec_send_raw with error:%d", result);

        if (result == APPL_ERROR_WAIT_CB)
        {
            return ZW_ERR_BUSY;
        }

        return ZW_ERR_OP_FAILED;
    }
    return ZW_ERR_NONE;
}


/**
zwsec_send - convert an OPP into SPP and send it
@param[in]	ifd	        interface
@param[in]	tx_param    transmission parameter
@return	ZW_ERR_xxx
*/
int zwsec_send(zwifd_p ifd, zwsec_tx_sm_param_t *tx_param)
{
    int             result;
    zwnet_p         nw = ifd->net;
    uint8_t         nodeid = ifd->nodeid;
    zwsec_layer_t   *sec_ctx = nw->sec_ctx;

    //Call tx state-machine
    plt_mtx_lck(sec_ctx->sec_mtx);
    if (sec_ctx->tx_sm_sta != ZWSEC_STA_IDLE)
    {
        if (sec_ctx->tx_sm_q_cnt < MAX_SEC_TX_Q)
        {
            result = util_list_add(sec_ctx->sec_mtx, &sec_ctx->tx_sm_opp_hd,
                                   (uint8_t *)tx_param,
                                   sizeof(zwsec_tx_sm_param_t) + tx_param->opp[0]);

            if (result == 0)
            {
                sec_ctx->tx_sm_q_cnt++;
                result = ZW_ERR_QUEUED;
            }
            plt_mtx_ulck(sec_ctx->sec_mtx);
            return result;
        }
        //Tx queue is full
        plt_mtx_ulck(sec_ctx->sec_mtx);
        return ZW_ERR_QUEUE_FULL;
    }
    //Set the destination node id for the first time
    sec_ctx->tx_sm_nodeid = nodeid;

    result = zwsec_tx_sm(nw, nodeid, EVT_OPP_RCVD, tx_param);
    plt_mtx_ulck(sec_ctx->sec_mtx);
    return result;
}


/**
zwsec_get_report - get interface report through report callback
@param[in]	ifd	        interface
@param[in]	param	    Parameter for the report get command
@param[in]	len     	Length of param
@param[in]	get_rpt_cmd Command to get the report
@param[in]	cb		    callback function for transmit status
@param[in]	cb_param    parameter passed to the callback function
@return		ZW_ERR_xxx
*/
static int zwsec_get_report(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd, tx_cmplt_cb_t cb, void *cb_param)
{
    int         retry_cnt;
    int32_t     result;
    uint8_t     cmd[46];

    //Check if the parameter length
    if (len > 46)
    {
        return ZW_ERR_TOO_LARGE;
    }

    cmd[0] = COMMAND_CLASS_SECURITY;
    cmd[1] = get_rpt_cmd;
    if (len > 0)
    {
        memcpy(cmd + 2, param, len);
    }

    retry_cnt = 2;

    while (retry_cnt-- > 0)
    {
        result = zwsec_send_raw(ifd, cmd, len + 2, cb, cb_param);

        if (result >= 0)
        {
            break;
        }

        //Retry if error code is ZW_ERR_BUSY
        if (result != ZW_ERR_BUSY)
        {
            break;
        }
    }

    if (result != 0)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwif_get_report with error:%d", result);
        return ZW_ERR_OP_FAILED;
    }
    return ZW_ERR_NONE;

}


/**
zwsec_tx_sts_cb - transmit complete status callback
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwsec_tx_sts_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    unsigned    nodeid = (unsigned)user_prm;

    //Call tx state-machine
    plt_mtx_lck(nw->sec_ctx->sec_mtx);
    zwsec_tx_sm(nw, nodeid, EVT_TX_STATUS_RCVD, &tx_sts);
    plt_mtx_ulck(nw->sec_ctx->sec_mtx);
}


/**
zwsec_nonce_rpt_hdlr -  Received nonce handler
@param[in]  nw		Network
@param[in]  nodeid	Source node id
@param[in]	nonce	nonce
*/
void zwsec_nonce_rpt_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *nonce)
{
    //Save the external nonce
    zwsec_enonce_put(nw->sec_ctx, nonce, nodeid);

    //Call tx state-machine
    plt_mtx_lck(nw->sec_ctx->sec_mtx);
    zwsec_tx_sm(nw, nodeid, EVT_EXT_NONCE_RCVD, NULL);
    plt_mtx_ulck(nw->sec_ctx->sec_mtx);

}


/**
zwsec_nonce_tmout_cb - Nonce request timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwsec_nonce_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;


    //Stop nonce request timer
    plt_mtx_lck(sec_ctx->sec_mtx);
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
    sec_ctx->tx_sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call tx state-machine
    zwsec_tx_sm(nw, sec_ctx->tx_sm_nodeid, EVT_NONCE_REQ_TIMEOUT, NULL);
    plt_mtx_ulck(sec_ctx->sec_mtx);

}


/**
zwsec_nonce_get - Request an external nonce through report callback
@param[in]	ifd	       interface
@return		ZW_ERR_xxx
*/
static int zwsec_nonce_get(zwifd_p ifd)
{
    int         result;

    //Request for report
    result = zwsec_get_report(ifd, NULL, 0,
                               SECURITY_NONCE_GET, zwsec_tx_sts_cb, (void *)((unsigned)ifd->nodeid));
    return result;
}


/**
zwsec_cmd_sup_get - Request commands the device supports using Security Encapsulation through report callback
@param[in]	ifd	       interface
@return		ZW_ERR_xxx
*/
int zwsec_cmd_sup_get(zwifd_p ifd)
{
    int                 result;
    zwsec_tx_sm_param_t *tx_param;

    //Send command using security message encapsulation
    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 2);
    if (tx_param)
    {
        tx_param->opp[0] = 2; //length
        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
        tx_param->opp[2] = SECURITY_COMMANDS_SUPPORTED_GET;

        tx_param->cb = NULL;
        tx_param->user[0] = NULL;
        tx_param->use_0_nw_key = 0;
        tx_param->nodeid = ifd->nodeid;

        result = zwsec_send(ifd, tx_param);
        free(tx_param);

    }
    else
    {
        result = ZW_ERR_MEMORY;
    }

    return result;
}


/**
zwsec_sm_spp_tx - Convert OPP into SPP before sending it and update the state-machine's state
@param[in]	nw		    Network
@param[in]	ext_nonce	External nonce
@param[in]	opp		    The OPP with first byte indicating the length of the OPP
@param[in]	nodeid		Destination node id
@param[in]	first_frm   Flag to indicate the OPP is the first frame; 1= OPP is first frame, 0= OPP is second frame
@param[in]	use_0_key   Flag to indicate whether to use all-zeroes network key; 1= use, 0= don't use
@return ZW_ERR_XXX
@pre Caller should lock the mutext sec_mtx before calling this function.
*/
static int zwsec_sm_spp_tx(zwnet_p nw, uint8_t *ext_nonce, uint8_t *opp, uint8_t nodeid, uint8_t first_frm, uint8_t use_0_key)
{
    int             inonce_idx;
    int             result;
    zwsec_layer_t   *sec_ctx = nw->sec_ctx;
    uint8_t         spp[MAX_SPP_SIZE];
    zwsec_mac_t     mac_input;
    zwifd_t         ifd;
    uint8_t         len;
    uint8_t         seq_flag;

    ifd.nodeid = nodeid;
    ifd.net = nw;

    //Split the OPP if it is greater than 26 bytes
    if (opp[0] > OPP_SPLIT_TRESHOLD)
    {
        zwnode_p node;

        len = opp[0] - OPP_SPLIT_TRESHOLD;
        if (len > OPP_SPLIT_TRESHOLD || !first_frm)
        {   //Cannot support OPP that doesn't fit into 2 frames
            //due to protocol limitation
            return ZW_ERR_TOO_LARGE;
        }
        //Save the second frame
        memcpy(sec_ctx->tx_sm_2nd_frm + 1, opp + 1 + OPP_SPLIT_TRESHOLD, len);
        sec_ctx->tx_sm_2nd_frm[0] = len;

        //Fill in the sequence flag
        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, nodeid);
        if (!node)
        {
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_NODE_NOT_FOUND;
        }
        node->sec_snd_seq_num++;
        //debug_zwapi_msg(&nw->plt_ctx, "sec send seq number:%u", (node->sec_snd_seq_num & 0x0F));
        seq_flag = FLG_SEQUENCE | (node->sec_snd_seq_num & 0x0F);

        plt_mtx_ulck(nw->mtx);

        //Length of 1st frame data
        len = OPP_SPLIT_TRESHOLD;

    }
    else
    {   //No splitting of OPP
        if (first_frm)
        {
            seq_flag = 0;
        }
        else
        {   //Second frame
            zwnode_p node;

            //Fill in the sequence flag
            plt_mtx_lck(nw->mtx);
            node = zwnode_find(&nw->ctl, nodeid);
            if (!node)
            {
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_NODE_NOT_FOUND;
            }
            seq_flag = FLG_SEQUENCE | FLG_SECOND_FRM | (node->sec_snd_seq_num & 0x0F);

            plt_mtx_ulck(nw->mtx);

        }
        len = opp[0];
        sec_ctx->tx_sm_2nd_frm[0] = 0;//indicate no second frame
    }

    //
    //Create security payload package (SPP)
    //

    //Create sender nonce
    inonce_idx = zwsec_inonce_gen(sec_ctx, nodeid, INONCE_STA_VALID);
    if (inonce_idx < 0)
    {
        return inonce_idx;
    }
    //Create initialization vector
    memcpy(mac_input.i_vec, sec_ctx->inonce_tbl[inonce_idx].nonce, 8);
    memcpy(mac_input.i_vec + 8, ext_nonce, 8);

    //Create security header, use "nonce get" version if there is
    //second frame of split SPP pending to send
    mac_input.sec_hdr = (sec_ctx->tx_sm_2nd_frm[0])?
        SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET : SECURITY_MESSAGE_ENCAPSULATION;

    //Fill in sender and receiver node id
    mac_input.sender_nid = nw->ctl.nodeid;
    mac_input.rcvr_nid = nodeid;

    //Fill in payload
    mac_input.payload_len = len + 1;//Add 1 byte for sequence flag
    mac_input.payload[0] = seq_flag;
    memcpy(mac_input.payload + 1, opp + 1, len);

    if (use_0_key)
    {
        result = zwsec_spp_gen(&mac_input, sec_ctx->auth_0_key, sec_ctx->enc_0_key, spp, &len);
    }
    else
    {
        result = zwsec_spp_gen(&mac_input, sec_ctx->auth_key, sec_ctx->enc_key, spp, &len);
    }

    if (result < 0)
    {
        return result;
    }

    //Send the generated SPP
    result = zwsec_send_raw(&ifd, spp, len, zwsec_tx_sts_cb, (void *)((unsigned)nodeid));

    //Indicate that there is no unprocessed OPP
    sec_ctx->tx_sm_opp[0] = 0;

    //Start nonce request timer
    if (mac_input.sec_hdr == SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET)
    {
        plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
        sec_ctx->tx_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, NONCE_REQ_TIMEOUT, zwsec_nonce_tmout_cb, nw);
    }

    //Update state
    sec_ctx->tx_sm_sta = ZWSEC_STA_WAIT_SPP_TX_STS;

    return result;
}


/**
zwsec_sm_start - Start state-machine to send securely
@param[in] nw		Network
@param[in] param	Transmission parameters
@return ZW_ERR_XXX
*/
static int zwsec_sm_start(zwnet_p  nw, zwsec_tx_sm_param_t *param)
{
    zwsec_layer_t   *sec_ctx = nw->sec_ctx;
	int				result;
    //uint8_t         ext_nonce[8];
    zwifd_t         ifd;

    ifd.nodeid = param->nodeid;
    ifd.net = nw;

    //Save the callback function and user context
    sec_ctx->tx_sm_cb = param->cb;
    sec_ctx->tx_sm_user[0] = param->user[0];
    sec_ctx->tx_sm_user[1] = param->user[1];

#if 0   //Disable using external nonce without first getting a nonce using NONCE_GET request
    //Check whether an external nonce is available
    if (zwsec_enonce_get(sec_ctx, param->nodeid, ext_nonce) == 0)
    {
        //Found external nonce for this node.
        //Convert OPP to SPP and send it
        return zwsec_sm_spp_tx(nw, ext_nonce, param->opp, param->nodeid, 1, param->use_0_nw_key);
    }
#endif
    //
    // External nonce unavailable
    //

    //Save OPP
    if (param->opp[0] > (OPP_SPLIT_TRESHOLD*2))
    {   //Cannot support OPP that doesn't fit into 2 frames
        //due to protocol limitation
        return ZW_ERR_TOO_LARGE;
    }

    memcpy(sec_ctx->tx_sm_opp, param->opp, param->opp[0] + 1);

    //Save network key option
    sec_ctx->tx_sm_use_0_key = param->use_0_nw_key;


    //Get external nonce
    result = zwsec_nonce_get(&ifd);
    if (result < 0)
    {
        return result;
    }

//  sec_ctx->enonce_rcvd = 0;

    //Setup nonce request timer
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
    sec_ctx->tx_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, NONCE_REQ_TIMEOUT, zwsec_nonce_tmout_cb, nw);

    //Update state
    sec_ctx->tx_sm_sta = ZWSEC_STA_WAIT_GET_NONCE_TX_STS;

    return result;
}


/**
zwsec_sm_next - Determine the next state of the transmission state machine
@param[in]	nw		Network
@return
*/
static void    zwsec_sm_next(zwnet_p nw)
{
    int                 result;
    util_lst_t          *req_lst;
    zwsec_layer_t       *sec_ctx = nw->sec_ctx;
    zwsec_tx_sm_param_t *tx_param;

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
    sec_ctx->tx_sm_tmr_ctx = NULL;

    while ((req_lst = util_list_get(sec_ctx->sec_mtx, &sec_ctx->tx_sm_opp_hd)) != NULL)
    {
        if (sec_ctx->tx_sm_q_cnt > 0)
        {
            sec_ctx->tx_sm_q_cnt--;
        }

        //There is a pending OPP to send
        tx_param = (zwsec_tx_sm_param_t *)req_lst->wr_buf;

        //Set the destination node id for the first time
        nw->sec_ctx->tx_sm_nodeid = tx_param->nodeid;

        result = zwsec_sm_start(nw, tx_param);
        if (result != 0)
        {
            //Callback to report error
            if (sec_ctx->tx_sm_cb)
            {
                sec_ctx->tx_sm_cb(nw, tx_param->nodeid, result, sec_ctx->tx_sm_user);
            }
        }
        free(req_lst);

        if (result == 0)
        {
            return;
        }
    }

    //No more OPP to send, go back to idle state
    sec_ctx->tx_sm_sta = ZWSEC_STA_IDLE;
}


#ifdef DEBUG_ZWAVE_SECURITY
static const char *sec_tx_state_msg[] = { "idle", "wait Nonce Get tx status", "wait SPP tx status", "wait ext nonce"
};

static const char *sec_tx_evt_msg[] = { "rcv opp", "rcvd ext nonce", "rcvd tx status", "nonce req timeout"
};

#endif
/**
zwsec_tx_sm - Security layer transmission state-machine
@param[in] nw		Network
@param[in] nodeid	Destination node id
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return Status of the processing of event
*/
static int zwsec_tx_sm(zwnet_p  nw, uint8_t nodeid, zwsec_tx_evt_t  evt, void *data)
{
    int result;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

#ifdef DEBUG_ZWAVE_SECURITY
    debug_zwapi_msg(&nw->plt_ctx, "zwsec_tx_sm:state %s : %s", sec_tx_state_msg[sec_ctx->tx_sm_sta], sec_tx_evt_msg[evt]);

#endif
    //Don't process event from other node
    if (nodeid != sec_ctx->tx_sm_nodeid)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }

    switch (sec_ctx->tx_sm_sta)
    {
        //------------------------------------------------------------
        case ZWSEC_STA_IDLE:
        //------------------------------------------------------------
            {
                if (evt == EVT_OPP_RCVD)
                {
                    return zwsec_sm_start(nw, (zwsec_tx_sm_param_t *)data);
                }
            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_SPP_TX_STS:
        //------------------------------------------------------------
            {
                if (evt == EVT_TX_STATUS_RCVD)
                {
                    uint8_t *tx_sts = (uint8_t *)data;
                    if (*tx_sts == TRANSMIT_COMPLETE_OK)
                    {
                        if (sec_ctx->tx_sm_2nd_frm[0] > 0)
                        {
                            //Expecting external nonce
                            sec_ctx->tx_sm_sta = ZWSEC_STA_WAIT_NONCE;

                        }
                        else
                        {   //Not expecting external nonce

                            //Stop the nonce request timer
                            plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
                            sec_ctx->tx_sm_tmr_ctx = NULL;

                            //Callback to inform that the transmission is o.k.
                            if (sec_ctx->tx_sm_cb)
                            {
                                sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_NONE, sec_ctx->tx_sm_user);
                            }
                            //Proceed to next state
                            zwsec_sm_next(nw);
                        }
                        return ZW_ERR_NONE;
                    }
                    //
                    //Transmit error
                    //

                    //Callback to report error
                    if (sec_ctx->tx_sm_cb)
                    {
                        sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_TRANSMIT, sec_ctx->tx_sm_user);
                    }
                    //Proceed to next state
                    zwsec_sm_next(nw);

                    return ZW_ERR_TRANSMIT;

                }

                if (evt == EVT_NONCE_REQ_TIMEOUT)
                {
#ifdef INTERNAL_TEST
                    sec_ctx->tx_tmout_cnt++;
#endif
                    //Callback to report error
                    if (sec_ctx->tx_sm_cb)
                    {
                        sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_TIMEOUT, sec_ctx->tx_sm_user);
                    }
                    //Proceed to next state
                    zwsec_sm_next(nw);
                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_GET_NONCE_TX_STS:
        //------------------------------------------------------------
            {
                if (evt == EVT_TX_STATUS_RCVD)
                {
                    uint8_t *tx_sts = (uint8_t *)data;
                    if (*tx_sts == TRANSMIT_COMPLETE_OK)
                    {
                        //Expecting external nonce
                        sec_ctx->tx_sm_sta = ZWSEC_STA_WAIT_NONCE;

                        //Check if enonce has arrived earlier
//                      if (sec_ctx->enonce_rcvd)
//                      {
//                          zwsec_tx_sm(nw, nodeid, EVT_EXT_NONCE_RCVD, NULL);
//                      }

                        return ZW_ERR_NONE;
                    }
                    //
                    //Transmit error
                    //

                    //Callback to report error
                    if (sec_ctx->tx_sm_cb)
                    {
                        sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_TRANSMIT, sec_ctx->tx_sm_user);
                    }
                    //Proceed to next state
                    zwsec_sm_next(nw);

                    return ZW_ERR_TRANSMIT;

                }

                //Handle race condition where EVT_EXT_NONCE_RCVD arrives before EVT_TX_STATUS_RCVD
//              if (evt == EVT_EXT_NONCE_RCVD)
//              {
//                  sec_ctx->enonce_rcvd = 1;
//              }

                if (evt == EVT_NONCE_REQ_TIMEOUT)
                {
                    //Callback to report error
                    if (sec_ctx->tx_sm_cb)
                    {
#ifdef INTERNAL_TEST
                        sec_ctx->tx_tmout_cnt++;
#endif
                        sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_TIMEOUT, sec_ctx->tx_sm_user);
                    }
                    //Proceed to next state
                    zwsec_sm_next(nw);
                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_NONCE:
        //------------------------------------------------------------
            {
                if (evt == EVT_EXT_NONCE_RCVD)
                {
                    uint8_t ext_nonce[8];

                    //Stop the nonce request timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
                    sec_ctx->tx_sm_tmr_ctx = NULL;

                    //Check whether OPP is available or
                    //second frame of OPP is pending to send
                    if ((sec_ctx->tx_sm_opp[0] > 0) || (sec_ctx->tx_sm_2nd_frm[0] > 0))
                    {
                        //Check whether an external nonce is available
                        result = zwsec_enonce_get(sec_ctx, nodeid, ext_nonce);
                        if (result == 0)
                        {
                            //Found external nonce for this node.
                            //Convert OPP to SPP and send it
                            if (sec_ctx->tx_sm_opp[0] > 0)
                            {
                                result = zwsec_sm_spp_tx(nw, ext_nonce, sec_ctx->tx_sm_opp, nodeid, 1, sec_ctx->tx_sm_use_0_key);
                            }
                            else
                            {
                                result = zwsec_sm_spp_tx(nw, ext_nonce, sec_ctx->tx_sm_2nd_frm, nodeid, 0, 0);
                            }
                        }

                        if (result < 0)
                        {
                            //Callback to report error
                            if (sec_ctx->tx_sm_cb)
                            {
                                sec_ctx->tx_sm_cb(nw, nodeid, result, sec_ctx->tx_sm_user);
                            }

                            //Proceed to next state
                            zwsec_sm_next(nw);
                        }
                        return ZW_ERR_NONE;
                    }

                    //No data to send.
                    //Proceed to next state
                    zwsec_sm_next(nw);
                    return ZW_ERR_NONE;
                }

                if (evt == EVT_NONCE_REQ_TIMEOUT)
                {
                    //Callback to report error
                    if (sec_ctx->tx_sm_cb)
                    {
#ifdef INTERNAL_TEST
                        sec_ctx->tx_tmout_cnt++;
#endif
                        sec_ctx->tx_sm_cb(nw, nodeid, ZW_ERR_TIMEOUT, sec_ctx->tx_sm_user);
                    }
                    //Proceed to next state
                    zwsec_sm_next(nw);
                    return ZW_ERR_TIMEOUT;
                }
            }
            break;


    }

    return ZW_ERR_EVENT;

}


/**
zwsec_depacket - Convert a received SPP to OPP
@param[in]	nw		    Network
@param[in]	src_node    Node id of the sender
@param[in]	spp_len     The length of the SPP
@param[in]	spp         Security payload package (SPP)
@param[out] opp         The OPP after the depacketization and decryption; first byte indicating the OPP length
@return 0=complete opp frame is returned through param opp, 1=second frame pending; ZW_ERR_XXX on error.
@post Caller must free the buffer allocated to opp if the return value is 0.
*/
int zwsec_depacket(zwnet_p nw, uint8_t src_node, uint8_t spp_len, uint8_t *spp, uint8_t **opp)
{
    int             result;
    int             idx;
    uint8_t         mac_out[8];
    uint8_t         *output;
    zwsec_mac_t     mac_input;
    zwsec_layer_t   *sec_ctx = nw->sec_ctx;

    //Check max package length
    if (spp_len > MAX_SPP_SIZE)
    {
        return ZW_ERR_TOO_LARGE;
    }

    if (spp_len < 19)
    {
        return ZW_ERR_TOO_SMALL;
    }

#ifdef USE_SENDER_NONCE_AS_ENONNCE
    //Store sender nonce
    zwsec_enonce_put(sec_ctx, spp + 2, src_node);
#endif

    //Get receiver nonce from internal nonce table
    idx = zwsec_inonce_get(sec_ctx, spp[spp_len - 9]);
    if (idx < 0)
    {
        return idx;
    }

    if (sec_ctx->inonce_tbl[idx].rcv_nodeid != src_node)
    {
        //debug_zwapi_msg(&nw->plt_ctx, "inonce node id mismatched");
        return ZW_ERR_NONCE_NOT_FOUND;
    }

    //
    //Calculate Message Authentication Code (MAC)
    //

    //Create initialization vector
    memcpy(mac_input.i_vec, spp + 2, 8);
    memcpy(mac_input.i_vec + 8, sec_ctx->inonce_tbl[idx].nonce, 8);

    mac_input.sec_hdr = spp[1];

    //Delete all the nonces associated with this node
    plt_mtx_lck(nw->sec_ctx->sec_mtx);
    zwsec_inonce_rm(nw->sec_ctx, src_node);
    plt_mtx_ulck(nw->sec_ctx->sec_mtx);

    //Fill in sender and receiver node id
    mac_input.sender_nid = src_node;
    mac_input.rcvr_nid = nw->ctl.nodeid;

    //Fill in payload
    mac_input.payload_len = spp_len - 19;
    memcpy(mac_input.payload, spp + 10, mac_input.payload_len);

    result = zwsec_mac_cal(&mac_input, sec_ctx->auth_key, mac_out);

    if (result < 0)
    {
        return result;
    }

    //Check whether calculated MAC matches the incoming SPP's MAC
    if(memcmp(spp + 10 + mac_input.payload_len + 1, mac_out, 8) != 0)
    {   //Authentication failed
        return ZW_ERR_AUTH;
    }

    //
    //Decrypt payload
    //
    zwsec_aes_ofb(sec_ctx->enc_key, mac_input.i_vec, mac_input.payload, mac_input.payload_len);
/*    printf("\n==================message info==============================\n");
    int i=0;
    for(i = 0; i< (mac_input.payload_len - 1); i++)
    {
        printf("%02x ",mac_input.payload[i]);
    }
    printf("\n==============================================================\n");*/

    //Check if this is a split frame
    if (mac_input.payload[0] & FLG_SEQUENCE)
    {
        zwnode_p node;

        //Fill in the sequence flag
        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, src_node);
        if (!node)
        {
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_NODE_NOT_FOUND;
        }

        //Check if this is the first frame
        if ((mac_input.payload[0] & FLG_SECOND_FRM) == 0)
        {   //First frame

            if (node->sec_rcv_seq_num & SEQ_NUMBER_VALID_BIT)
            {
                //Sequence number must be different from the previous one
                if ((node->sec_rcv_seq_num & 0x0F) == (mac_input.payload[0] & 0x0F))
                {
                    plt_mtx_ulck(nw->mtx);
                    return ZW_ERR_SEQ_NUMBER;
                }
            }
            //Store sequence number
            node->sec_rcv_seq_num = (mac_input.payload[0] & 0x0F) | SEQ_NUMBER_VALID_BIT;

            //Store first frame
            memcpy(node->first_spp_frm, mac_input.payload, mac_input.payload_len);
            node->first_spp_frm_len = mac_input.payload_len;

            plt_mtx_ulck(nw->mtx);
            return 1;
        }
        else //Second frame
        {   //Sequence number must be same as the first frame
            if ((node->sec_rcv_seq_num & 0x0F) != (mac_input.payload[0] & 0x0F))
            {
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_SEQ_NUMBER;
            }

            //Combine first and second frames, removing the sequence number
            output = (uint8_t *)malloc(29 + mac_input.payload_len - 1);
            if (!output)
            {
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_MEMORY;
            }

            output[0] = node->first_spp_frm_len + mac_input.payload_len - 2; //store the length
            memcpy(output + 1, node->first_spp_frm + 1, node->first_spp_frm_len - 1);
            memcpy(output + node->first_spp_frm_len, mac_input.payload + 1, mac_input.payload_len - 1);
            *opp = output;
            /*printf("\n==================end message info==============================\n");
            for(i = 0; i< (29 + mac_input.payload_len - 1); i++)
            {
                printf("%02x ",output[i]);
            }
            printf("\n========================end====================================\n");*/

            plt_mtx_ulck(nw->mtx);
            return 0;

        }
    }

    //Not split frame
    output = (uint8_t *)malloc(mac_input.payload_len);
    if (!output)
    {
        plt_mtx_ulck(nw->mtx);
        return ZW_ERR_MEMORY;
    }
    output[0] = mac_input.payload_len - 1; //length
    memcpy(output + 1, mac_input.payload + 1, mac_input.payload_len - 1);
    *opp = output;
    return 0;

}


/**
zwsec_incd_tmout_cb - Included node timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwsec_incd_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;


    //Stop nonce request timer
    plt_mtx_lck(sec_ctx->sec_mtx);
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->incd_sm_tmr_ctx);
    sec_ctx->incd_sm_tmr_ctx = NULL;

    //Call the state-machine
    zwsec_incd_node_sm(nw, 0, EVT_INCD_TIMEOUT, NULL);
    plt_mtx_ulck(sec_ctx->sec_mtx);

}


/**
zwsec_incd_node_notify - security layer included node callback
@param[in]	nw		network
@param[in]  status	0= secure included node ok; else negative error number
*/
static void zwsec_incd_node_notify(zwnet_p nw, int status)
{
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

    //Update state to idle
    sec_ctx->incd_sm_sta = ZWSEC_INCD_IDLE;

    //Unlock before callback
    plt_mtx_ulck(sec_ctx->sec_mtx);

    sec_ctx->incd_sm_cb(nw, status, sec_ctx->incd_sm_user);

    //Relock after callback
    plt_mtx_lck(sec_ctx->sec_mtx);

}


/**
zwsec_incd_node_sm - Security layer included node state-machine
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return Status of the processing of event
*/
int zwsec_incd_node_sm(zwnet_p  nw, uint8_t nodeid, zwsec_incd_evt_t  evt, void *data)
{
    int result;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

    switch (sec_ctx->incd_sm_sta)
    {
        //------------------------------------------------------------
        case ZWSEC_INCD_IDLE:
        //------------------------------------------------------------
            {
                if (evt == EVT_UNSECURED_INC_DONE)
                {
                    zwsec_incd_sm_param_t   *param = (zwsec_incd_sm_param_t *)data;

                    //Save the completion callback function and parameter
                    sec_ctx->incd_sm_cb = param->cb;
                    sec_ctx->incd_sm_user = param->user;

                    //Set network key to all-zeroes
                    memset(sec_ctx->nw_key, 0, 16);
                    memcpy(sec_ctx->auth_key, sec_ctx->auth_0_key, 16);
                    memcpy(sec_ctx->enc_key, sec_ctx->enc_0_key, 16);


                    //Setup state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->incd_sm_tmr_ctx);
                    sec_ctx->incd_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_incd_tmout_cb, nw);

                    //Update state
                    sec_ctx->incd_sm_sta = ZWSEC_STA_WAIT_SCH_GET;

                    return ZW_ERR_NONE;
                }
            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_SCH_GET:
        //------------------------------------------------------------
            {
                if (evt == EVT_SCHEME_GET)
                {
                    zwifd_t         ifd;
                    uint8_t         rcv_scheme;    //received scheme from the controller
                    uint8_t         cmd[3];

                    rcv_scheme = *((uint8_t *)data);
                    ifd.nodeid = nodeid;
                    ifd.net = nw;

                    //Stop state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->incd_sm_tmr_ctx);
                    sec_ctx->incd_sm_tmr_ctx = NULL;

                    //Check whether the received scheme supported
                    if ((rcv_scheme & 0x01) != 0)
                    {
                        //Callback to report error
                        zwsec_incd_node_notify(nw, ZW_ERR_SEC_SCHEME);

                        return ZW_ERR_SEC_SCHEME;
                    }

                    //Send scheme report
                    cmd[0] = COMMAND_CLASS_SECURITY;
                    cmd[1] = SECURITY_SCHEME_REPORT;
                    cmd[2] = 0;

                    result = zwsec_send_raw(&ifd, cmd, 3, NULL, NULL);

                    if (result != 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "send sec scheme rpt with error:%d", result);

                        //Callback to report error
                        zwsec_incd_node_notify(nw, ZW_ERR_TRANSMIT);

                        return ZW_ERR_TRANSMIT;
                    }

                    //Restart state timer
                    sec_ctx->incd_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_incd_tmout_cb, nw);

                    //Update state
                    sec_ctx->incd_sm_sta = ZWSEC_STA_WAIT_NW_KEY;

                    return ZW_ERR_NONE;

                }

                if (evt == EVT_INCD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_incd_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }
            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_NW_KEY:
        //------------------------------------------------------------
            {
                if (evt == EVT_KEY_SET)
                {
                    uint8_t             *nw_key = (uint8_t *)data;
                    zwsec_tx_sm_param_t *tx_param;
                    zwifd_t             ifd;

                    ifd.nodeid = nodeid;
                    ifd.net = nw;

                    //Stop state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->incd_sm_tmr_ctx);
                    sec_ctx->incd_sm_tmr_ctx = NULL;

                    //Save the network key
                    memcpy(sec_ctx->nw_key, nw_key, 16);

                    //Generate authentication and encryption keys
                    zwsec_keys_gen(sec_ctx->nw_key, sec_ctx->auth_key, sec_ctx->enc_key);

                    //Send the command using security message encapsulation
                    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 2);
                    if (tx_param)
                    {
                        tx_param->opp[0] = 2; //length
                        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                        tx_param->opp[2] = NETWORK_KEY_VERIFY;

                        tx_param->cb = NULL;
                        tx_param->user[0] = NULL;
                        tx_param->use_0_nw_key = 0;
                        tx_param->nodeid = nodeid;

                        result = zwsec_send(&ifd, tx_param);
                        free(tx_param);

                    }
                    else
                    {
                        result = ZW_ERR_MEMORY;
                    }

                    if (result == 0)
                    {
                        //Restart state timer
                        sec_ctx->incd_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_incd_tmout_cb, nw);

                        //Update state
                        sec_ctx->incd_sm_sta = ZWSEC_STA_WAIT_SCH_INHERIT;

                        return ZW_ERR_NONE;
                    }

                    debug_zwapi_msg(&nw->plt_ctx, "send nw key verify with error:%d", result);
                    //Callback to report error
                    zwsec_incd_node_notify(nw, result);

                    return result;
                }

                if (evt == EVT_INCD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_incd_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_SCH_INHERIT:
        //------------------------------------------------------------
            {
                if (evt == EVT_SCHEME_INHERIT)
                {
                    zwsec_tx_sm_param_t *tx_param;
                    zwifd_t             ifd;
                    uint8_t             inherit_schm;    //inherit scheme from the controller

                    inherit_schm = *((uint8_t *)data);

                    ifd.nodeid = nodeid;
                    ifd.net = nw;

                    //Stop state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->incd_sm_tmr_ctx);
                    sec_ctx->incd_sm_tmr_ctx = NULL;

                    //Check whether the inherit scheme supported
                    if ((inherit_schm & 0x01) != 0)
                    {
                        //Callback to report error
                        zwsec_incd_node_notify(nw, ZW_ERR_SEC_SCHEME);

                        return ZW_ERR_SEC_SCHEME;
                    }

                    //Inherit common supported security schemes
                    sec_ctx->inherit_schm = 0;

                    //Send security scheme report using security message encapsulation
                    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 3);
                    if (tx_param)
                    {
                        tx_param->opp[0] = 3; //length
                        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                        tx_param->opp[2] = SECURITY_SCHEME_REPORT;
                        tx_param->opp[3] = sec_ctx->inherit_schm;

                        tx_param->cb = NULL;
                        tx_param->user[0] = NULL;
                        tx_param->use_0_nw_key = 0;
                        tx_param->nodeid = nodeid;

                        result = zwsec_send(&ifd, tx_param);
                        free(tx_param);

                    }
                    else
                    {
                        result = ZW_ERR_MEMORY;
                    }

                    if (result != 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "send sec scheme report securely with error:%d", result);
                    }

                    //Callback to report completion or error
                    zwsec_incd_node_notify(nw, result);

                    return result;

                }

                if (evt == EVT_INCD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_incd_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

    }

    return ZW_ERR_EVENT;

}


/**
zwsec_incd_msg_hdlr - Security layer included node message handler
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] cmd_buf  Command buffer
@param[in] cmd_len  Command length
@param[in] cmd_secure  Flag to indicate the command is received securely. 1= secure, 0= non-secure
@return
*/
void zwsec_incd_msg_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t cmd_secure)
{
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

    switch (cmd_buf[1])
    {
        case SECURITY_SCHEME_GET:
            if (cmd_len >= 3)
            {
                plt_mtx_lck(sec_ctx->sec_mtx);
                //Call the state-machine
                zwsec_incd_node_sm(nw, nodeid, EVT_SCHEME_GET, cmd_buf + 2);
                plt_mtx_ulck(sec_ctx->sec_mtx);
            }
            break;

        case NETWORK_KEY_SET:
            if (cmd_secure && (cmd_len >= 18))
            {
                plt_mtx_lck(sec_ctx->sec_mtx);
                //Call the state-machine
                zwsec_incd_node_sm(nw, nodeid, EVT_KEY_SET, cmd_buf + 2);
                plt_mtx_ulck(sec_ctx->sec_mtx);
            }
            break;

        case SECURITY_SCHEME_INHERIT:
            if (cmd_secure && (cmd_len >= 3))
            {
                plt_mtx_lck(sec_ctx->sec_mtx);
                //Call the state-machine
                zwsec_incd_node_sm(nw, nodeid, EVT_SCHEME_INHERIT, cmd_buf + 2);
                plt_mtx_ulck(sec_ctx->sec_mtx);
            }
            break;
    }

}


/**
zwsec_add_tmout_cb - Add node timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwsec_add_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;


    //Stop timer
    plt_mtx_lck(sec_ctx->sec_mtx);
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
    sec_ctx->add_sm_tmr_ctx = NULL;

    //Call the state-machine
    zwsec_add_node_sm(nw, 0, EVT_ADD_TIMEOUT, NULL);
    plt_mtx_ulck(sec_ctx->sec_mtx);

}


/**
zwsec_add_delay_cb - Add node delay timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwsec_add_delay_cb(void *data)
{
    zwnet_p                 nw = (zwnet_p)data;
    zwsec_layer_t           *sec_ctx = nw->sec_ctx;
    zwsec_incd_sm_param_t   param;
    int                     result;

    //Stop timer
    plt_mtx_lck(sec_ctx->sec_mtx);
    plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
    sec_ctx->add_sm_tmr_ctx = NULL;


    //Start the state-machine for secure-inclusion
    param.cb = sec_ctx->add_sm_cb;
    param.user = sec_ctx->add_sm_user;
    param.delay_ms = 0;

    //Call the state-machine
    result = zwsec_add_node_sm(nw, sec_ctx->add_sm_nodeid, EVT_UNSEC_INC_DONE, &param);
    plt_mtx_ulck(sec_ctx->sec_mtx);

    if (result < 0)
    {
        if (sec_ctx->add_sm_cb)
        {
            //Callback to report error
            sec_ctx->add_sm_cb(nw, result, sec_ctx->add_sm_user);
        }
    }
}


/**
zwsec_add_node_notify - security layer add node callback
@param[in]	nw		network
@param[in]  status	0= secure add node ok; else negative error number
*/
static void zwsec_add_node_notify(zwnet_p nw, int status)
{
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

    //Update state to idle
    sec_ctx->add_sm_sta = ZWSEC_ADD_IDLE;

    //Unlock before callback
    plt_mtx_ulck(sec_ctx->sec_mtx);

    sec_ctx->add_sm_cb(nw, status, sec_ctx->add_sm_user);

    //Relock after callback
    plt_mtx_lck(sec_ctx->sec_mtx);

}


/**
zwsec_add_node_sm - Security layer adding node (node inclusion) state-machine
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return Status of the processing of event
*/
int zwsec_add_node_sm(zwnet_p  nw, uint8_t nodeid, zwsec_add_evt_t  evt, void *data)
{
    int result;
    zwsec_layer_t *sec_ctx = nw->sec_ctx;
    printf("linhui, zwsec_add_node_sm, nodeid=%d, ev=%d, add node status:%d\n",nodeid,evt,sec_ctx->add_sm_sta);

    switch (sec_ctx->add_sm_sta)
    {
        //------------------------------------------------------------
        case ZWSEC_ADD_IDLE:
        //------------------------------------------------------------
            {
                if (evt == EVT_UNSEC_INC_DONE)
                {
                    zwifd_t                 ifd;
                    uint8_t                 cmd[3];
                    zwsec_incd_sm_param_t   *param = (zwsec_incd_sm_param_t *)data;

                    ifd.nodeid = nodeid;
                    ifd.net = nw;

                    //Save the completion callback function and parameter
                    sec_ctx->add_sm_cb = param->cb;
                    sec_ctx->add_sm_user = param->user;

                    //Check if delay is required
                    if (param->delay_ms > 0)
                    {
                        sec_ctx->add_sm_nodeid = nodeid;

                        //Setup delay timer
                        plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
                        sec_ctx->add_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, param->delay_ms, zwsec_add_delay_cb, nw);
                        return ZW_ERR_NONE;
                    }

                    //Send scheme report
                    cmd[0] = COMMAND_CLASS_SECURITY;
                    cmd[1] = SECURITY_SCHEME_GET;
                    cmd[2] = 0;

                    result = zwsec_send_raw(&ifd, cmd, 3, NULL, NULL);

                    if (result != 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "send sec scheme get with error:%d", result);

                        return ZW_ERR_TRANSMIT;
                    }

                    //Setup state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
                    sec_ctx->add_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_add_tmout_cb, nw);

                    //Update state
                    sec_ctx->add_sm_sta = ZWSEC_STA_WAIT_SCHEME;

                    return ZW_ERR_NONE;
                }
            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_SCHEME:
        //------------------------------------------------------------
            {
                if (evt == EVT_SCHEME_RPT)
                {
                    zwsec_tx_sm_param_t *tx_param;
                    zwifd_t             ifd;
                    uint8_t             rcv_scheme;    //received scheme from the added node

                    rcv_scheme = *((uint8_t *)data);
                    ifd.nodeid = nodeid;
                    ifd.net = nw;

                    //Stop state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
                    sec_ctx->add_sm_tmr_ctx = NULL;

                    //Check whether the received scheme supported
                    if ((rcv_scheme & 0x01) != 0)
                    {
                        //Callback to report error
                        zwsec_add_node_notify(nw, ZW_ERR_SEC_SCHEME);

                        return ZW_ERR_SEC_SCHEME;
                    }

                    //Send network key using security message encapsulation with all-zeroes key
                    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 18);
                    if (tx_param)
                    {
                        tx_param->opp[0] = 18; //length
                        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                        tx_param->opp[2] = NETWORK_KEY_SET;
                        memcpy(tx_param->opp + 3, sec_ctx->nw_key, 16);

                        tx_param->cb = NULL;
                        tx_param->user[0] = NULL;
                        tx_param->use_0_nw_key = 1;
                        tx_param->nodeid = nodeid;

                        result = zwsec_send(&ifd, tx_param);
                        free(tx_param);

                    }
                    else
                    {
                        result = ZW_ERR_MEMORY;
                    }

                    if (result == 0)
                    {
                        //Restart state timer
                        sec_ctx->add_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_add_tmout_cb, nw);

                        //Update state
                        sec_ctx->add_sm_sta = ZWSEC_STA_WAIT_VERIFY;

                        return ZW_ERR_NONE;
                    }

                    debug_zwapi_msg(&nw->plt_ctx, "send nw key with error:%d", result);

                    //Callback to report error
                    zwsec_add_node_notify(nw, result);

                    return result;
                }

                if (evt == EVT_ADD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_add_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }
            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_VERIFY:
        //------------------------------------------------------------
            {
                if (evt == EVT_KEY_VERIFY)
                {
                    zwnode_p    node;

                    //Stop state timer
                    plt_tmr_stop(&nw->plt_ctx, sec_ctx->add_sm_tmr_ctx);
                    sec_ctx->add_sm_tmr_ctx = NULL;

                    node = zwnode_find(&nw->ctl, nodeid);

                    if (!node)
                    {
                        //Callback to report error
                        zwsec_add_node_notify(nw, ZW_ERR_NODE_NOT_FOUND);

                        return ZW_ERR_NODE_NOT_FOUND;
                    }

                    //If the included node is a controller, send scheme inherit command
                    if ((node->basic == BASIC_TYPE_CONTROLLER) || (node->basic == BASIC_TYPE_STATIC_CONTROLLER))
                    {
                        zwsec_tx_sm_param_t *tx_param;
                        zwifd_t             ifd;

                        ifd.nodeid = nodeid;
                        ifd.net = nw;

                        //Send the command using security message encapsulation
                        tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 3);
                        if (tx_param)
                        {
                            tx_param->opp[0] = 3; //length
                            tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                            tx_param->opp[2] = SECURITY_SCHEME_INHERIT;
                            tx_param->opp[3] = 0;

                            tx_param->cb = NULL;
                            tx_param->user[0] = NULL;
                            tx_param->use_0_nw_key = 0;
                            tx_param->nodeid = nodeid;

                            result = zwsec_send(&ifd, tx_param);
                            free(tx_param);

                        }
                        else
                        {
                            result = ZW_ERR_MEMORY;
                        }

                        if (result == 0)
                        {
                            //Restart state timer
                            sec_ctx->add_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, INCL_STA_TIMEOUT, zwsec_add_tmout_cb, nw);

                            //Update state
                            sec_ctx->add_sm_sta = ZWSEC_STA_WAIT_INHERIT_RPT;

                            return ZW_ERR_NONE;
                        }

                        debug_zwapi_msg(&nw->plt_ctx, "send scheme inherit with error:%d", result);

                        //Callback to report error
                        zwsec_add_node_notify(nw, result);

                        return result;

                    }
                    //The included node is a slave
                    //Callback to report completion
                    zwsec_add_node_notify(nw, ZW_ERR_NONE);

                    return ZW_ERR_NONE;

                }

                if (evt == EVT_ADD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_add_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

        //------------------------------------------------------------
        case ZWSEC_STA_WAIT_INHERIT_RPT:
        //------------------------------------------------------------
            {
                if (evt == EVT_SCHEME_RPT)
                {
                    uint8_t         rcv_scheme;    //received scheme from the controller

                    rcv_scheme = *((uint8_t *)data);
                    //Check whether the received scheme supported
                    if ((rcv_scheme & 0x01) != 0)
                    {
                        //Callback to report error
                        zwsec_add_node_notify(nw, ZW_ERR_SEC_SCHEME);

                        return ZW_ERR_SEC_SCHEME;
                    }

                    //Callback to report completion
                    zwsec_add_node_notify(nw, ZW_ERR_NONE);

                    return ZW_ERR_NONE;
                }

                if (evt == EVT_ADD_TIMEOUT)
                {
                    //Callback to report error
                    zwsec_add_node_notify(nw, ZW_ERR_TIMEOUT);

                    return ZW_ERR_TIMEOUT;
                }

            }
            break;

    }

    return ZW_ERR_EVENT;
}


/**
zwsec_add_node_msg_hdlr - Security layer add node message handler
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] cmd_buf  Command buffer
@param[in] cmd_len  Command length
@param[in] cmd_secure  Flag to indicate the command is received securely. 1= secure, 0= non-secure
@return
*/
void zwsec_add_node_msg_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t cmd_secure)
{
    zwsec_layer_t *sec_ctx = nw->sec_ctx;

    switch (cmd_buf[1])
    {
        case NETWORK_KEY_VERIFY:
            if (cmd_secure)
            {
                plt_mtx_lck(sec_ctx->sec_mtx);
                //Call the state-machine
                zwsec_add_node_sm(nw, nodeid, EVT_KEY_VERIFY, NULL);
                plt_mtx_ulck(sec_ctx->sec_mtx);
            }
            break;

        case SECURITY_SCHEME_REPORT:
            if (cmd_len >= 3)
            {
                plt_mtx_lck(sec_ctx->sec_mtx);
                //Call the state-machine
                zwsec_add_node_sm(nw, nodeid, EVT_SCHEME_RPT, cmd_buf + 2);
                plt_mtx_ulck(sec_ctx->sec_mtx);
            }
            break;
    }

}


/**
zwsec_init - Initialize the security layer
@param[in]	sec_ctx	    Security layer context
@return  0 on success; negative error number on failure
*/
int zwsec_init(zwsec_layer_t *sec_ctx)
{
	int         result;
    int         retry_count;
    uint8_t     nw_0_key[16];

    //Initialize random number generator
    retry_count = 3;
    while (retry_count-- > 0)
    {
        result = zwsec_rand_init(&sec_ctx->prng_ctx);
        if (result == 0)
        {
            break;
        }
        //Sleep and retry
        plt_sleep(400);
    }

    if (result != 0)
    {
        debug_zwapi_msg(&sec_ctx->net->plt_ctx, "Init random number generator failed:%d", result);
        return result;
    }

    sec_ctx->tx_sm_sta = ZWSEC_STA_IDLE;
    sec_ctx->tmr_tick = 0;
    sec_ctx->tx_sm_q_cnt = 0;
    sec_ctx->clean_up_tick = 0;
    sec_ctx->tx_sm_opp_hd = NULL;
    sec_ctx->incd_sm_sta = ZWSEC_INCD_IDLE;
    sec_ctx->add_sm_sta = ZWSEC_ADD_IDLE;

    if (!plt_mtx_init(&sec_ctx->sec_mtx))
        return ZW_ERR_NO_RES;

    sec_ctx->tick_tmr_ctx = plt_periodic_start(&sec_ctx->net->plt_ctx, SEC_TIMER_TICK, zwsec_tmout_cb, sec_ctx);
    if (!sec_ctx->tick_tmr_ctx)
        goto l_SEC_INIT_ERROR1;

    //Generate network key
    zwsec_rand_output(&sec_ctx->prng_ctx, 16, sec_ctx->nw_key);

    //Generate authentication and encryption keys
    zwsec_keys_gen(sec_ctx->nw_key, sec_ctx->auth_key, sec_ctx->enc_key);

    //Generate authentication and encryption keys for all-zeroes network key
    memset(nw_0_key, 0, 16);
    zwsec_keys_gen(nw_0_key, sec_ctx->auth_0_key, sec_ctx->enc_0_key);

    sec_ctx->init_done = 1;
    return ZW_ERR_NONE;

l_SEC_INIT_ERROR1:
    plt_mtx_destroy(sec_ctx->sec_mtx);
    return ZW_ERR_NO_RES;

}

/**
zwsec_exit - Clean up
@param[in]	sec_ctx	    Security layer context
@return
*/
void zwsec_exit(zwsec_layer_t *sec_ctx)
{

    if (!sec_ctx->init_done)
    {
        return;
    }

    //Flush the transmission queue
    util_list_flush(sec_ctx->sec_mtx, &sec_ctx->tx_sm_opp_hd);

    plt_tmr_stop(&sec_ctx->net->plt_ctx, sec_ctx->tick_tmr_ctx);
    plt_tmr_stop(&sec_ctx->net->plt_ctx, sec_ctx->tx_sm_tmr_ctx);
    plt_mtx_destroy(sec_ctx->sec_mtx);
}

