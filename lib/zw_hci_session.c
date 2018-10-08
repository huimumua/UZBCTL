/**
@file   zw_hci_session.c - Z-wave host controller interface session layer implementation.

        To handle commands sending and the corresponding responses and/or callback.

@author David Chow

@version    1.0 14-5-10  Initial release

version: 1.0
comments: Initial release
*/
#include "../include/zw_hci_session.h"

/**
@defgroup Session Session layer APIs
To handle commands sending and the corresponding responses and/or callback.
@{
*/

static void    ssn_resp_tmout_cb(void *data);

/**
ssn_sta_machine - Session layer state-machine
@param[in] ssn_ctx  Context.
@param[in] ssn_evt  The event for the state-machine
@param[in] data     The data associated with the event
@param[in] dat_len  The length of the data
@return
*/
static void    ssn_sta_machine(ssn_layer_ctx_t   *ssn_ctx, ssn_evt_t ssn_evt, uint8_t *data, uint8_t dat_len)
{

    switch (ssn_ctx->ssn_sta)
    {
            //-------------------------------------
        case    SESSION_STATE_IDLE:
            //-------------------------------------
            if (ssn_evt == EVENT_COMMAND_SENT)
            {
                ssn_cmd_attr_t *cmd_attr = (ssn_cmd_attr_t  *)data;
                //Check if callback frame is expected, then update the function id to callback function map
                if ((cmd_attr->cmd_flag & COMMAND_HAS_CALLBACK) != 0)
                {
                    if ((cmd_attr->func_id > 0) && (cmd_attr->func_id <= SESSION_FUNCID_MAX))
                    {
                        ssn_ctx->cb_map[cmd_attr->func_id].cmd_id = cmd_attr->cmd_id;
                        ssn_ctx->cb_map[cmd_attr->func_id].cmd_cb_func = cmd_attr->cmd_cb_func;
                        ssn_ctx->cb_map[cmd_attr->func_id].cmd_cb_prm[0] = cmd_attr->cmd_cb_prm[0];
                        ssn_ctx->cb_map[cmd_attr->func_id].cmd_cb_prm[1] = cmd_attr->cmd_cb_prm[1];
                    }
                }
                //Save the command id and flag
                ssn_ctx->last_cmd_id = cmd_attr->cmd_id;
                ssn_ctx->last_cmd_flag = cmd_attr->cmd_flag;
                ssn_ctx->ssn_sta = SESSION_STATE_COMMAND_SENT;
            }
            break;

            //-------------------------------------
        case SESSION_STATE_COMMAND_SENT:
            //-------------------------------------
            if (ssn_evt == EVENT_RECEIVED_SEND_FRAME_STATUS)
            {
                frm_snd_sts_t *frm_snd_sts = (frm_snd_sts_t *)data;
                if (*frm_snd_sts == FRAME_SEND_OK)
                {
                    if ((ssn_ctx->last_cmd_flag & COMMAND_HAS_RESPONSE) != 0)
                    {
                        //Start wait response timer
                        ssn_ctx->resp_tmr_ctx = plt_tmr_start(ssn_ctx->plt_ctx, ssn_ctx->resp_tmout_ms, ssn_resp_tmout_cb, ssn_ctx);

                        ssn_ctx->ssn_sta = SESSION_STATE_WAIT_RESPONSE;

                        ssn_ctx->snd_frm_sts = FRAME_SEND_OK;
                        plt_sem_post(ssn_ctx->snd_frm_sts_sem);

                        break;
                    }
                }
                //No response frame is expected, go back to idle state
                ssn_ctx->ssn_sta = SESSION_STATE_IDLE;
                ssn_ctx->snd_frm_sts = *frm_snd_sts;
                plt_sem_post(ssn_ctx->snd_frm_sts_sem);

            }
            break;

            //-------------------------------------
        case SESSION_STATE_WAIT_RESPONSE:
            //-------------------------------------
            if (ssn_evt == EVENT_RECEIVED_RESPONSE)
            {
                //Stop the timer and release timer resource
                plt_tmr_stop(ssn_ctx->plt_ctx, ssn_ctx->resp_tmr_ctx);
                ssn_ctx->resp_tmr_ctx = NULL;

                ssn_ctx->cmd_resp = (ssn_cmd_resp_t *)malloc(sizeof(ssn_cmd_resp_t) + dat_len - FRAME_HEADER_LEN);
                if (ssn_ctx->cmd_resp)
                {
                    ssn_ctx->cmd_resp_sts = SESSION_RESPONSE_OK;
                    ssn_ctx->cmd_resp->type = (dat_frm_typ_t)data[SESSION_TYPE_OFFSET];
                    ssn_ctx->cmd_resp->cmd_id = data[SESSION_COMMAND_ID_OFFSET];
                    ssn_ctx->cmd_resp->len = dat_len - FRAME_HEADER_LEN;
                    memcpy(ssn_ctx->cmd_resp->dat_buf, data + SESSION_DATA_OFFSET, ssn_ctx->cmd_resp->len);
                }
                else
                {
                    ssn_ctx->cmd_resp_sts = SESSION_RESPONSE_MEM;
                }
                ssn_ctx->ssn_sta = SESSION_STATE_IDLE;

                plt_sem_post(ssn_ctx->cmd_resp_sts_sem);

            }
            else if (ssn_evt == EVENT_RESPONSE_TIMEOUT)
            {
                ssn_ctx->cmd_resp_sts = SESSION_RESPONSE_TIMEOUT;
                ssn_ctx->ssn_sta = SESSION_STATE_IDLE;
                plt_sem_post(ssn_ctx->cmd_resp_sts_sem);
            }
            break;
    }
}

/**
ssn_func_id_gen - generate a function id
@param[in,out]	ssn_ctx		Context
@return Function id
@pre Caller must lock the sta_mach_mtx before calling this function
*/
static uint8_t    ssn_func_id_gen(ssn_layer_ctx_t   *ssn_ctx)
{
    uint8_t func_id;

    if (++(ssn_ctx->last_gen_func_id) <= SESSION_FUNCID_MAX)
    {
        func_id = ssn_ctx->last_gen_func_id;
    }
    else
    {   //Wrap around to 1.
        //Note: Zero is reserved to indicate disable callback
        func_id = ssn_ctx->last_gen_func_id = 1;
    }

    return func_id;
}


/**
ssn_cmd_snd - Send a command
@param[in,out] ssn_ctx      Context.
@param[in] param            The parameters related to the command.
@param[out] cmd_resp        The response to the command.
@return  0 on success, negative error number on failure
@post   It is the responsibility of the caller with COMMAND_HAS_RESPONSE to free the
        memory allocated to the cmd_resp.
*/
int32_t ssn_cmd_snd(ssn_layer_ctx_t   *ssn_ctx, ssn_cmd_snd_param_t   *param,
                    ssn_cmd_resp_t **cmd_resp)
{
    int32_t             ret_val;
    uint8_t             func_id;
    ssn_cmd_attr_t      cmd_attr;

    plt_mtx_lck(ssn_ctx->sta_mach_mtx);

    //Check whether the previous command has completed
    if (ssn_ctx->ssn_sta != SESSION_STATE_IDLE)
    {
        plt_mtx_ulck(ssn_ctx->sta_mach_mtx);
        return SESSION_ERROR_PREVIOUS_COMMAND_UNCOMPLETED;
    }

    //Insert function id if command requires callback
    if ((param->cmd_flag & COMMAND_HAS_CALLBACK) != 0)
    {
        func_id = ssn_func_id_gen(ssn_ctx);
        //append the function id at the end of data buffer
        param->dat_buf[param->dat_sz] = func_id;
        param->dat_sz++;

    }
    /*printf("\n=========before frm_dat_frm_snd,start, len:%d =================\n",param->dat_sz);
    int y = 0;
    for(y= 0;y<param->dat_sz;y++)
    {
        printf(" ->%02x<- ",param->dat_buf[y]);
    }
    printf("\n===================frm_dat_frm_snd message end======================\n");*/
    //Send to frame layer
    ssn_ctx->snd_frm_sts = FRAME_SEND_UNKNOWN;
    ret_val = frm_dat_frm_snd(&ssn_ctx->frm_ctx, REQ, param->cmd_id,
                              param->dat_buf, param->dat_sz);

    if (ret_val != 0)
    {
        plt_mtx_ulck(ssn_ctx->sta_mach_mtx);
        return ret_val;
    }

    //Call the state machine
    cmd_attr.cmd_id = param->cmd_id;
    cmd_attr.cmd_flag = param->cmd_flag;

    if ((param->cmd_flag & COMMAND_HAS_CALLBACK) != 0)
    {
        cmd_attr.func_id = func_id;
        cmd_attr.cmd_cb_func = param->cmd_cb_func;
        cmd_attr.cmd_cb_prm[0] = param->cmd_cb_prm[0];
        cmd_attr.cmd_cb_prm[1] = param->cmd_cb_prm[1];
    }

    if ((param->cmd_flag & COMMAND_HAS_RESPONSE) != 0)
    {
        ssn_ctx->cmd_resp_sts = SESSION_RESPONSE_UNKNOWN;
        ssn_ctx->cmd_resp = NULL;
    }
    ssn_sta_machine(ssn_ctx, EVENT_COMMAND_SENT, (uint8_t *)&cmd_attr, sizeof(ssn_cmd_attr_t));
    plt_mtx_ulck(ssn_ctx->sta_mach_mtx);

    //Wait for send frame status from the frame layer
    plt_sem_wait(ssn_ctx->snd_frm_sts_sem);
    if (ssn_ctx->snd_frm_sts != FRAME_SEND_OK)
    {
        switch (ssn_ctx->snd_frm_sts)
        {
            case FRAME_SEND_UNKNOWN:
                //Shouldn't happen, it means the state-machine is out of sync!
                ret_val = SESSION_ERROR_SYNC;
                break;

            case FRAME_SEND_TIMEOUT://Send frame timeout due to no ACK received
                ret_val = SESSION_ERROR_SND_FRM_TMOUT;
                break;

            case FRAME_SEND_FAIL_CHKSUM:
                ret_val = SESSION_ERROR_SEND_CHKSUM;
                break;

            case FRAME_SEND_FAIL_DROPPED:
                ret_val = SESSION_ERROR_SEND_BUSY;
                break;

            default:
                break;
        }
        return ret_val;
    }


    //Wait for response
    if ((param->cmd_flag & COMMAND_HAS_RESPONSE) != 0)
    {
        plt_sem_wait(ssn_ctx->cmd_resp_sts_sem);
        if (ssn_ctx->cmd_resp_sts != SESSION_RESPONSE_OK)
        {
            switch (ssn_ctx->cmd_resp_sts)
            {
                case SESSION_RESPONSE_UNKNOWN:
                    //Shouldn't happen, it means the state-machine is out of sync!
                    ret_val = SESSION_ERROR_SYNC;
                    break;

                case SESSION_RESPONSE_MEM:
                    ret_val = ZWHCI_ERROR_MEMORY;
                    break;

                case SESSION_RESPONSE_TIMEOUT://Wait for response frame timeout
                    ret_val = SESSION_ERROR_RESP_TMOUT;
                    break;

                default:
                    break;
            }

            return ret_val;
        }
        plt_mtx_lck(ssn_ctx->sta_mach_mtx);
        if (ssn_ctx->cmd_resp->cmd_id == param->cmd_id)
        {
            *cmd_resp = ssn_ctx->cmd_resp;
        }
        else
        {
            ret_val = SESSION_ERROR_INVALID_RESP;
            free(ssn_ctx->cmd_resp);
        }
        plt_mtx_ulck(ssn_ctx->sta_mach_mtx);
        return ret_val;

    }
    return 0;

}


/**
ssn_resp_tmout_cb - Timer callback when no response received after sending a command.
@param[in] data Pointer to the ssn_layer_ctx_t
@return
*/
static void    ssn_resp_tmout_cb(void *data)
{
    ssn_layer_ctx_t   *ssn_ctx = (ssn_layer_ctx_t   *)data;
    plt_mtx_lck(ssn_ctx->sta_mach_mtx);

    //Stop the timer and release timer resource
    plt_tmr_stop(ssn_ctx->plt_ctx, ssn_ctx->resp_tmr_ctx);
    ssn_ctx->resp_tmr_ctx = NULL;

    //Call the state machine
    ssn_sta_machine(ssn_ctx, EVENT_RESPONSE_TIMEOUT, NULL, 0);

    plt_mtx_ulck(ssn_ctx->sta_mach_mtx);

}


/**
ssn_rx_frm_cb - Callback from frame layer whenever a complete frame is received
@param[in] frm_ctx      Frame layer context.
@param[in] buf          Buffer that stores the received frame
@param[in] dat_len      Number of bytes received
@return
*/
static void    ssn_rx_frm_cb(struct _frm_layer_ctx *frm_ctx, uint8_t *buf, uint8_t dat_len)
{
    ssn_layer_ctx_t   *ssn_ctx = frm_ctx->ssn_layer_ctx;

    if (buf[SESSION_TYPE_OFFSET] == REQ)
    {
        //
        //Handling of all unsolicited and callback commands
        //
        int32_t     ret_val;      //Return value
        ssn_cb_req_t    cb_req;   //callback request
        ssn_cmd_resp_t  *cb_cmd;  //callback command
        uint8_t   func_id = buf[SESSION_DATA_OFFSET];
        uint8_t   cmd_id = buf[SESSION_COMMAND_ID_OFFSET];

        cb_cmd = (ssn_cmd_resp_t *)malloc(sizeof(ssn_cmd_resp_t) + dat_len - FRAME_HEADER_LEN);
        if (!cb_cmd)
            return;
        cb_cmd->type = (dat_frm_typ_t)buf[SESSION_TYPE_OFFSET];
        cb_cmd->cmd_id = cmd_id;
        cb_cmd->len = dat_len - FRAME_HEADER_LEN;
        memcpy(cb_cmd->dat_buf, buf + SESSION_DATA_OFFSET, cb_cmd->len);

        //Build a callback request for the callback thread to execute
        cb_req.cmd = cb_cmd;

        cb_req.cmd_cb_func = NULL;

        //Check whether the function id and command id matched
        if (dat_len >= (FRAME_HEADER_LEN + 1))
        {
            if ((func_id > 0) && (func_id <= SESSION_FUNCID_MAX))
            {
                if (ssn_ctx->cb_map[func_id].cmd_id == cmd_id)
                {   //Matched. It is a callback function
                    cb_req.cmd_cb_func = ssn_ctx->cb_map[func_id].cmd_cb_func;
                    cb_req.cmd_cb_prm[0] = ssn_ctx->cb_map[func_id].cmd_cb_prm[0];
                    cb_req.cmd_cb_prm[1] = ssn_ctx->cb_map[func_id].cmd_cb_prm[1];
                    if (ssn_ctx->nm_cb)
                    {
                        ssn_ctx->nm_cb(ssn_ctx, cmd_id);
                    }

                }
            }
        }

        if (!cb_req.cmd_cb_func)
        {
            //Function id doesn't match with the command id,
            //call back unsolicited command callback function
            cb_req.cmd_cb_func = ssn_ctx->unsolicited_cmd_cb;
        }

        ret_val = util_list_add(ssn_ctx->cb_thrd_mtx, &ssn_ctx->cb_req_hd,
                                 (uint8_t *)&cb_req, sizeof(ssn_cb_req_t));
        if (ret_val)
        {   //Error
            free(cb_cmd);
            return;
        }
        plt_sem_post(ssn_ctx->cb_thrd_sem);

    }
    else //RES
    {
        //Handling of response
        plt_mtx_lck(ssn_ctx->sta_mach_mtx);
        ssn_sta_machine(ssn_ctx, EVENT_RECEIVED_RESPONSE, buf, dat_len);
        plt_mtx_ulck(ssn_ctx->sta_mach_mtx);
    }

}


/**
ssn_snd_frm_sts_cb - Send frame status callback from frame layer
@param[in] frm_ctx  Frame layer context.
@param[in] status   The status of sending a frame.
@return
*/
static void    ssn_snd_frm_sts_cb(struct _frm_layer_ctx *frm_ctx, frm_snd_sts_t status)
{
    ssn_layer_ctx_t   *ssn_ctx = frm_ctx->ssn_layer_ctx;

    plt_mtx_lck(ssn_ctx->sta_mach_mtx);
    ssn_sta_machine(ssn_ctx, EVENT_RECEIVED_SEND_FRAME_STATUS, (uint8_t *)&status, sizeof(frm_snd_sts_t));
    plt_mtx_ulck(ssn_ctx->sta_mach_mtx);
}


/**
ssn_cb_thrd - Thread to serve the callback requests
@param[in]	data		The session context
@return
*/
static void ssn_cb_thrd(void   *data)
{
    ssn_layer_ctx_t   *ssn_ctx = (ssn_layer_ctx_t *)data;
    util_lst_t      *cb_req_lst;

    ssn_ctx->cb_thrd_sts = 1;
    while (1)
    {
        //Wait for callback request
        plt_sem_wait(ssn_ctx->cb_thrd_sem);

        //Check whether to exit the thread
        if (ssn_ctx->cb_thrd_run == 0)
        {
            ssn_ctx->cb_thrd_sts = 0;
            return;
        }

        cb_req_lst = util_list_get(ssn_ctx->cb_thrd_mtx, &ssn_ctx->cb_req_hd);

        //Callback
        if (cb_req_lst)
        {
            ssn_cb_req_t    *cb_req;

            cb_req = (ssn_cb_req_t *)cb_req_lst->wr_buf;

            cb_req->cmd_cb_func(ssn_ctx, cb_req->cmd, cb_req->cmd_cb_prm);
            free(cb_req->cmd);
            free(cb_req_lst);
        }
    }
}


/**
ssn_init - Init the session layer.
Should be called once before calling the other session layer functions
@param[in,out]	ssn_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the context after this call
*/
int32_t ssn_init(ssn_layer_ctx_t   *ssn_ctx)
{
    int32_t     ret_val;

    //Init frame layer
    ssn_ctx->frm_ctx.snd_frm_sts_cb = ssn_snd_frm_sts_cb;
    ssn_ctx->frm_ctx.rx_frm_cb = ssn_rx_frm_cb;
    ssn_ctx->frm_ctx.snd_tmout_ms = FRAME_SEND_TIMEOUT_MIN;
    ssn_ctx->frm_ctx.ssn_layer_ctx = ssn_ctx;
    ssn_ctx->frm_ctx.plt_ctx = ssn_ctx->plt_ctx;
    ret_val = frm_init(&ssn_ctx->frm_ctx);

    if (ret_val != 0)
        return ret_val;

    //Init session layer
    ssn_ctx->ssn_sta = SESSION_STATE_IDLE;
    if (ssn_ctx->resp_tmout_ms < SESSION_RESPONSE_TIMEOUT_MIN)
        ssn_ctx->resp_tmout_ms = SESSION_RESPONSE_TIMEOUT_MIN;

    if (!plt_mtx_init(&ssn_ctx->sta_mach_mtx))
        goto l_SESSION_INIT_ERROR;

    if (!plt_sem_init(&ssn_ctx->snd_frm_sts_sem))
        goto l_SESSION_INIT_ERROR1;

    if (!plt_sem_init(&ssn_ctx->cmd_resp_sts_sem))
        goto l_SESSION_INIT_ERROR2;

    if (!plt_mtx_init(&ssn_ctx->cb_thrd_mtx))
        goto l_SESSION_INIT_ERROR3;

    if (!plt_sem_init(&ssn_ctx->cb_thrd_sem))
        goto l_SESSION_INIT_ERROR4;

    ssn_ctx->cb_thrd_run = 1;
    if (plt_thrd_create(ssn_cb_thrd, ssn_ctx) < 0)
        goto l_SESSION_INIT_ERROR5;

    return 0;

l_SESSION_INIT_ERROR5:
    plt_sem_destroy(ssn_ctx->cb_thrd_sem);

l_SESSION_INIT_ERROR4:
    plt_mtx_destroy(ssn_ctx->cb_thrd_mtx);

l_SESSION_INIT_ERROR3:
    plt_sem_destroy(ssn_ctx->cmd_resp_sts_sem);

l_SESSION_INIT_ERROR2:
    plt_sem_destroy(ssn_ctx->snd_frm_sts_sem);

l_SESSION_INIT_ERROR1:
    plt_mtx_destroy(ssn_ctx->sta_mach_mtx);

l_SESSION_INIT_ERROR:
    frm_exit(&ssn_ctx->frm_ctx);

    return INIT_ERROR_SESSION;
}


/**
ssn_exit - Clean up the session layer
@param[in,out]	ssn_ctx		Context
@return
*/
void ssn_exit(ssn_layer_ctx_t   *ssn_ctx)
{
    uint32_t        wait_count;
    util_lst_t      *cb_req_lst;
    ssn_cb_req_t    *cb_req;

    frm_exit(&ssn_ctx->frm_ctx);

    //Stop the thread
    ssn_ctx->cb_thrd_run = 0;
    plt_sem_post(ssn_ctx->cb_thrd_sem);
    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (ssn_ctx->cb_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    //Extra time for the thread to fully exit
    //Needed to avoid program crash if calling init and exit in a very short interval
    plt_sleep(100);

    //Flush lists
    while ((cb_req_lst = util_list_get(ssn_ctx->cb_thrd_mtx, &ssn_ctx->cb_req_hd)) != NULL)
    {
        cb_req = (ssn_cb_req_t *)cb_req_lst->wr_buf;
        free(cb_req->cmd);
        free(cb_req_lst);
    }

    plt_sem_destroy(ssn_ctx->cb_thrd_sem);
    plt_mtx_destroy(ssn_ctx->cb_thrd_mtx);
    plt_sem_destroy(ssn_ctx->cmd_resp_sts_sem);
    plt_sem_destroy(ssn_ctx->snd_frm_sts_sem);
    plt_mtx_destroy(ssn_ctx->sta_mach_mtx);

}

/**
@}
*/


