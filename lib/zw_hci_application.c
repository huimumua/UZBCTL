/**
@file   zw_hci_application.c - Z-wave host controller interface application layer implementation.

        To provide network management and controller functions.

@author David Chow

@version    1.0 21-5-10  Initial release

version: 1.0
comments: Initial release
*/

#include "../include/zw_hci_application.h"
#include "../include/zwave/ZW_SerialAPI.h"

/**
@defgroup Application Application layer APIs
To provide network management and controller functions.
@{
*/


/**
appl_cmd_hdlr - Parse the application command handler command and call back high-level application layer.
@param[in]	appl_ctx		Context
@param[in]	cmd		        The received application command handler command
@return
*/
static void    appl_cmd_hdlr(appl_layer_ctx_t   *appl_ctx, ssn_cmd_resp_t *cmd)
{
    //ZW->HOST: REQ | 0x04 | rxStatus | sourceNode | cmdLength | pCmd[ ]
    if (appl_ctx->application_command_handler_cb && cmd->len >= 3)
    {   //call back high-level application command handler
        appl_ctx->application_command_handler_cb(appl_ctx, cmd->dat_buf[0], cmd->dat_buf[1], cmd->dat_buf[2], &cmd->dat_buf[3]);
    }
}

/**
appl_updt - Parse the application update command and call back high-level application layer.
@param[in]	appl_ctx		Context
@param[in]	cmd		        The received application command handler command
@return
*/
static void    appl_updt(appl_layer_ctx_t   *appl_ctx, ssn_cmd_resp_t *cmd)
{
    //ZW->HOST: REQ | 0x49 | bStatus | bNodeID | bLen | basic | generic | specific | commandclasses[ ]
    if (appl_ctx->application_update_cb)
    {   //call back high-level application update handler
        appl_node_info_t    node_info;
        int                 parse_ok = 0;
        uint16_t            *cmd_cls_sec;
        uint8_t             cmd_cnt_sec;

        if (cmd->len >= 6)
        {   //Contains node info
            node_info.node_id = cmd->dat_buf[1];
            node_info.basic = cmd->dat_buf[3];
            node_info.gen = cmd->dat_buf[4];
            node_info.spec = cmd->dat_buf[5];
            if(util_cmd_cls_parse(&cmd->dat_buf[6], cmd->dat_buf[2] - 3, &node_info.cmd_cls, &node_info.cmd_cnt,
                                  &cmd_cls_sec, &cmd_cnt_sec))
            {
                parse_ok = 1;
            }
            else
            {
                node_info.cmd_cls = NULL;
                node_info.cmd_cnt = 0;
            }

            appl_ctx->application_update_cb(appl_ctx, cmd->dat_buf[0], &node_info);

            if (parse_ok)
            {
                free(node_info.cmd_cls);
                free(cmd_cls_sec);
            }

        }
        else if (cmd->len > 0 && cmd->dat_buf[2] == 0)
        {   //No class info
            memset(&node_info, 0, sizeof(appl_node_info_t));
            node_info.node_id = cmd->dat_buf[1];
            appl_ctx->application_update_cb(appl_ctx, cmd->dat_buf[0], &node_info);
        }
    }
}

/**
appl_unsolicited_cmd_cb - Despatch the unsolicited command received
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received unsolicited command
@param[in]	user_prm    The user specific parameter
@return
*/
static void    appl_unsolicited_cmd_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **user_prm)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    printf("linhui, appl_unsolicited_cmd_cb, cmdid=%02x\n",cmd->cmd_id);
    switch (cmd->cmd_id)
    {
        case    FUNC_ID_APPLICATION_COMMAND_HANDLER:
            appl_cmd_hdlr(appl_ctx, cmd);
            break;

        case    FUNC_ID_ZW_APPLICATION_UPDATE:
            appl_updt(appl_ctx, cmd);
            break;

        default:
            debug_msg_show(appl_ctx->plt_ctx, "Unhandled unsolicited cmd:%02X", (unsigned)cmd->cmd_id);
            break;
    }
}


/**
appl_wait_to_snd - Wait for the condition where sending of command is allowed
@param[in]	appl_ctx		Context
@return  1 on success, 0 on failure
@post   If success, the caller is responsible to unlock the mutex "snd_mtx".
*/
static int32_t    appl_wait_to_snd(appl_layer_ctx_t   *appl_ctx)
{
    int             ret;

    plt_mtx_lck(appl_ctx->snd_mtx);
    ret = 0;
    while ((appl_ctx->wait_cmd_cb || appl_ctx->wait_nm_cb)
           && ret == 0)
        ret = plt_cond_timedwait(appl_ctx->snd_cv, appl_ctx->snd_mtx, APPL_WAIT_SEND_TIMEOUT);

    if (ret == 1)
    {   //Wait timeout
        //debug_msg_show(appl_ctx->plt_ctx, "Cond wait timeout");
    }

    if ((!appl_ctx->wait_cmd_cb) && (!appl_ctx->wait_nm_cb))
        return  1; //Wait success

    plt_mtx_ulck(appl_ctx->snd_mtx);

    return 0;   //Wait failed
}


/**
appl_cb_tmout_cb - Timer callback when no callback received after sending a command.
@param[in] data     Pointer to the appl_layer_ctx_t
@return
*/
static void    appl_cb_tmout_cb(void *data)
{
    appl_layer_ctx_t    *appl_ctx = (appl_layer_ctx_t   *)data;
    tx_cmplt_cb_t       cb;          //transmit completion status callback function
    void                *usr_prm;    //transmit completion status callback user parameter

    //debug_msg_show(appl_ctx->plt_ctx, "appl_cb_tmout_cb: timeout");

    plt_mtx_lck(appl_ctx->snd_mtx);

    //Release the waiting for callback flag
    appl_ctx->wait_cmd_cb = 0;

    //Stop the timer and release timer resource
    plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
    appl_ctx->cb_tmr_ctx = NULL;

    //Save the callback function and parameter before releasing the lock
    cb = appl_ctx->send_data_cb;
    usr_prm = appl_ctx->snd_dat_cb_prm;

    //Wake up any wait thread
    plt_cond_signal(appl_ctx->snd_cv);

    plt_mtx_ulck(appl_ctx->snd_mtx);
    //Call the callback function
    if (cb)
    {
        cb(appl_ctx, TRANSMIT_COMPLETE_NO_CB, usr_prm);
    }
}


/**
appl_nm_sts_tmout_cb - Timer callback when no status received after sending a network management command.
@param[in] data     Pointer to the appl_layer_ctx_t
@return
*/
static void    appl_nm_sts_tmout_cb(void *data)
{
    appl_layer_ctx_t    *appl_ctx = (appl_layer_ctx_t   *)data;
    tx_cmplt_cb_t       cb;          //transmit completion status callback function
    void                *usr_prm;    //transmit completion status callback user parameter

    //debug_msg_show(appl_ctx->plt_ctx, "appl_nm_sts_tmout_cb: timeout");

    plt_mtx_lck(appl_ctx->snd_mtx);

    //Release the waiting for callback flag
    appl_ctx->wait_nm_cb = 0;

    //Stop the timer and release timer resource
    plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
    appl_ctx->cb_tmr_ctx = NULL;

    //Save the callback function and parameter before releasing the lock
    cb = appl_ctx->send_data_cb;
    usr_prm = appl_ctx->snd_dat_cb_prm;

    //Wake up any wait thread
    plt_cond_signal(appl_ctx->snd_cv);

    plt_mtx_ulck(appl_ctx->snd_mtx);
    //Call the callback function
    if (cb)
    {
        cb(appl_ctx, TRANSMIT_COMPLETE_NO_CB, usr_prm);
    }
}


/**
application_node_info - Send the application node information to the controller
@param[in]	appl_ctx		Context
@param[in]	node_info       The node information
@param[in]	dev_opt         Device option mask
@return  0 on success, negative error number on failure
*/
int32_t    application_node_info(appl_layer_ctx_t   *appl_ctx, appl_node_info_t *node_info, uint8_t dev_opt)
{
    int32_t result;
    int     i;
    uint8_t *cmd_cls;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 4 + (node_info->cmd_cnt * sizeof(uint16_t));

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION;
    //HOST->ZW: REQ | 0x03 | deviceOptionsMask | generic | specific | parmLength | nodeParm[ ]
    cmd_prm->dat_buf[0] = dev_opt;
    cmd_prm->dat_buf[1] = node_info->gen;
    cmd_prm->dat_buf[2] = node_info->spec;
    cmd_cls = cmd_prm->dat_buf + 4;
    prm_len = 0;
    for (i=0; i<node_info->cmd_cnt; i++)
    {
        if (node_info->cmd_cls[i] & 0xFF00)
        {   //Extended command class
            cmd_cls[prm_len++] = node_info->cmd_cls[i] >> 8;
        }
        cmd_cls[prm_len++] = node_info->cmd_cls[i] & 0x00FF;
    }
    cmd_prm->dat_buf[3] = prm_len;
    cmd_prm->dat_sz = prm_len + 4;


    cmd_prm->cmd_flag = 0;
    cmd_prm->cmd_cb_func = NULL;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_nm_cb - zw_send_data callback for clearing the wait_cmd_cb flag
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd_id		Command id
@return
*/
static void    zw_nm_cb(struct _ssn_layer_ctx *ssn_ctx, uint8_t cmd_id)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;

    switch (cmd_id)
    {
        case FUNC_ID_ZW_SEND_DATA:
            plt_mtx_lck(appl_ctx->snd_mtx);
            if (appl_ctx->wait_cmd_cb)
            {   //Clear flag
                appl_ctx->wait_cmd_cb = 0;
                //Stop the timer and release timer resource
                plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
                appl_ctx->cb_tmr_ctx = NULL;

                //Wake up any wait thread
                plt_cond_signal(appl_ctx->snd_cv);
            }
            plt_mtx_ulck(appl_ctx->snd_mtx);
            break;

        case FUNC_ID_ZW_ASSIGN_RETURN_ROUTE:
        case FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE:
        case FUNC_ID_ZW_DELETE_RETURN_ROUTE:
        case FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE:
            plt_mtx_lck(appl_ctx->snd_mtx);
            if (appl_ctx->wait_nm_cb)
            {
                appl_ctx->wait_nm_cb = 0;
                //Stop the timer and release timer resource
                plt_tmr_stop(appl_ctx->plt_ctx, appl_ctx->cb_tmr_ctx);
                appl_ctx->cb_tmr_ctx = NULL;

                //Wake up any wait thread
                plt_cond_signal(appl_ctx->snd_cv);

            }
            plt_mtx_ulck(appl_ctx->snd_mtx);
            break;

    }

}


/**
zw_send_data_cb - call back high-level application layer to report transmit completion status
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_send_data_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    tx_cmplt_cb_t       cb;          //transmit completion status callback function
    //ZW->HOST: REQ | 0x13 | funcID | txStatus

    //Save the callback function before releasing the lock
    cb = (tx_cmplt_cb_t) param[0];

    if (cmd->len >= 2 && cb)
    {
        cb(appl_ctx, cmd->dat_buf[1], param[1]);
    }
}


/**
zw_send_data - Send data to a node
@param[in]	appl_ctx		Context
@param[in]	prm             The parameters
@param[in]	cb              The callback function on transmit completion
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/
int32_t    zw_send_data(appl_layer_ctx_t   *appl_ctx, appl_snd_data_t  *prm, tx_cmplt_cb_t cb, void *cb_prm)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    /*
    NOTE: Allways use the completeFunc callback to determine when the next frame can be sent. Calling
    the ZW_SendData or ZW_SendDataMulti in a loop without checking the completeFunc callback will
    overflow the transmit queue and eventually fail. The data buffer in the application must not be changed
    before completeFunc callback is received because it’s only the pointer there is passed to the transmit
    queue.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 3 + prm->dat_len;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SEND_DATA;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x13 | nodeID | dataLength | pData[ ] | txOptions | funcID
    cmd_prm->dat_buf[0] = prm->node_id;
    cmd_prm->dat_buf[1] = prm->dat_len;
    memcpy(cmd_prm->dat_buf + 2, prm->dat_buf, prm->dat_len);
    cmd_prm->dat_buf[2 + prm->dat_len] = prm->tx_opt;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_send_data_cb;
    //Save the pointer to callback function and the callback parameter
    cmd_prm->cmd_cb_prm[0] = cb;
    cmd_prm->cmd_cb_prm[1] = cb_prm;
    appl_ctx->send_data_cb = cb;
    appl_ctx->snd_dat_cb_prm = cb_prm;

    /*printf("\n=========before ssn_cmd_snd, start, len:%d =================\n",cmd_prm->dat_sz);
    int y = 0;
    for(y= 0;y<cmd_prm->dat_sz;y++)
    {
        printf(" (%02x) ",cmd_prm->dat_buf[y]);
    }
    printf("\n===================before ssn_cmd_snd end======================\n");*/
    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x13 | RetVal
            if (resp->dat_buf[0] == 0)
                result = APPL_TX_Q_OVERFLOW;
            free(resp);
        }
    }

    if (result == 0)
    {   //Set flag to indicate waiting for callback to prevent the next command from sending
        appl_ctx->wait_cmd_cb = 1;
        // Start the callback timer
        appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_cb_tmout_cb, appl_ctx);

        if (!appl_ctx->cb_tmr_ctx)
        {   //timer not working
            debug_msg_show(appl_ctx->plt_ctx, "Error: appl callback timer not working");
        }

    }

    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_generic_cb - call back high-level application layer to report transmit completion status
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_generic_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    tx_cmplt_cb_t       cb;          //transmit completion status callback function
    //ZW->HOST: REQ | 0xYY | funcID | bStatus

    cb = (tx_cmplt_cb_t)param[0];

    if (cmd->len >= 2 && cb)
        cb(appl_ctx, cmd->dat_buf[1], param[1]);
}


/**
zw_cmplt_cb - call back high-level application layer to report completion
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_cmplt_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    wr_cmplt_cb_t       cb;          //callback function

    cb = (wr_cmplt_cb_t)param[0];

    if (cmd->len >= 1 && cb)
        cb(appl_ctx, param[1]);
}


/**
zw_assign_return_route - Assign static return routes (up to 4) to a Routing Slave node or Enhanced Slave node.
@param[in]	appl_ctx		Context
@param[in]	src_node_id     The Node ID (1...232) of the routing slave that should get the return routes.
@param[in]	dst_node_id     The Destination node ID (1...232).
@param[in]	cb              The callback function on transmit completion.
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/
int32_t    zw_assign_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t src_node_id, uint8_t dst_node_id, tx_cmplt_cb_t cb, void *cb_prm)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    /*
    Calculate the shortest routes from the Routing Slave node (src_node_id) to the destination node
    (dst_node_id) and transmits the return routes to the Routing Slave node (src_node_id). The destination
    node is part of the return routes assigned to the slave.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 2;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_ASSIGN_RETURN_ROUTE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x46 | bSrcNodeID | bDstNodeID | funcID
    cmd_prm->dat_buf[0] = src_node_id;
    cmd_prm->dat_buf[1] = dst_node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;
    cmd_prm->cmd_cb_prm[1] = cb_prm;

    //Save the callback function and parameter to be used by the appl_nm_sts_tmout_cb()
    appl_ctx->send_data_cb = cb;
    appl_ctx->snd_dat_cb_prm = cb_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x46 | retVal
            if (resp->dat_buf[0] == 0)
                result = APPL_OPER_ALREADY_ACTIVE;//An “assign/delete return route” operation already is active.
            free(resp);
        }
        if (result == 0)
        {
            //Set flag to indicate waiting for callback to prevent the next command from sending
            appl_ctx->wait_nm_cb = 1;
            // Start the network management status callback timer
            appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_nm_sts_tmout_cb, appl_ctx);

        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_delete_return_route - Delete all static return routes from a Routing Slave or Enhanced Slave.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the routing slave node.
@param[in]	cb              The callback function on transmit completion.
@param[in]	cb_prm          The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/
int32_t    zw_delete_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, void *cb_prm)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_DELETE_RETURN_ROUTE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x47 | nodeID | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;
    cmd_prm->cmd_cb_prm[1] = cb_prm;

    //Save the callback function and parameter to be used by the appl_nm_sts_tmout_cb()
    appl_ctx->send_data_cb = cb;
    appl_ctx->snd_dat_cb_prm = cb_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x47 | retVal
            if (resp->dat_buf[0] == 0)
                result = APPL_OPER_ALREADY_ACTIVE;//An “assign/delete return route” operation already is active.
            free(resp);
        }
        if (result == 0)
        {
            //Set flag to indicate waiting for callback to prevent the next command from sending
            appl_ctx->wait_nm_cb = 1;
            // Start the network management status callback timer
            appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_nm_sts_tmout_cb, appl_ctx);

        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_assign_suc_return_route - Notify presence of a SUC/SIS to a Routing Slave or Enhanced Slave.
@param[in]	appl_ctx		Context
@param[in]	slv_node_id     The Node ID (1...232) of the routing slave that should get the return route to the SUC node.
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_assign_suc_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t slv_node_id, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x51 | bSrcNodeID | funcID | funcID
/*
    The extra funcID is added to ensures backward compatible. This parameter has been removed starting
    from dev. kit 4.1x. and onwards and has therefore no meaning anymore.
*/
    cmd_prm->dat_buf[0] = slv_node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    //Save the callback function and parameter to be used by the appl_nm_sts_tmout_cb()
    appl_ctx->send_data_cb = cb;
    //appl_ctx->snd_dat_cb_prm = cb_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x51 | retVal
            if (resp->dat_buf[0] == 0)
                result = APPL_OPER_ALREADY_ACTIVE;//An “assign/delete return route” operation already is active.
            free(resp);
        }
        if (result == 0)
        {
            //Set flag to indicate waiting for callback to prevent the next command from sending
            appl_ctx->wait_nm_cb = 1;
            // Start the network management status callback timer
            appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_nm_sts_tmout_cb, appl_ctx);

        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_delete_suc_return_route - Delete the return route of the SUC node from a Routing Slave node or Enhanced Slave node
@param[in]	appl_ctx		Context
@param[in]	slv_node_id     The Node ID (1...232) of the routing slave node.
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_delete_suc_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t slv_node_id, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x55 | nodeID | funcID
    cmd_prm->dat_buf[0] = slv_node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    //Save the callback function and parameter to be used by the appl_nm_sts_tmout_cb()
    appl_ctx->send_data_cb = cb;
    //appl_ctx->snd_dat_cb_prm = cb_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x55 | retVal
            if (resp->dat_buf[0] == 0)
                result = APPL_OPER_ALREADY_ACTIVE;//An “assign/delete return route” operation already is active.
            free(resp);
        }
        if (result == 0)
        {
            //Set flag to indicate waiting for callback to prevent the next command from sending
            appl_ctx->wait_nm_cb = 1;
            // Start the network management status callback timer
            appl_ctx->cb_tmr_ctx = plt_tmr_start(appl_ctx->plt_ctx, appl_ctx->cb_tmout_ms, appl_nm_sts_tmout_cb, appl_ctx);

        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_enable_suc - Enable/disable assignment of the SUC/SIS functionality in the controller.
@param[in]	appl_ctx		Context
@param[in]	state		    TRUE = SUC functionality is enabled.  FALSE = SUC functionality is disabled.
@param[in]	cap		        SUC capabilities that is enabled.
@param[out]	res             Response: non-zero means the SUC functionality was enabled/disabled;
                            zero means attempting to disable a running SUC, not allowed.
@return  0 on command sent successfully to the controller, negative error number on failure
*/
int32_t    zw_enable_suc(appl_layer_ctx_t   *appl_ctx, uint8_t state, uint8_t cap, uint8_t *res)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 2;//total data length

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_ENABLE_SUC;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x52 | state | capabilities
    cmd_prm->dat_buf[0] = state;
    cmd_prm->dat_buf[1] = cap;
    cmd_prm->cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x52 | retVal
            *res = resp->dat_buf[0];
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_request_network_update - Request network topology updates from the SUC/SIS node.
                            Secondary controllers can only use this call when a SUC is present in the network.
                            All controllers can use this call in case a SUC ID Server (SIS) is available.
                            Routing Slaves can only use this call, when a SUC is present in the network.
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure.
*/
int32_t    zw_request_network_update(appl_layer_ctx_t   *appl_ctx, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t));

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_REQUEST_NETWORK_UPDATE;
    //HOST->ZW: REQ | 0x53 | funcID
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {
            //ZW->HOST: RES | 0x53 | retVal
            if (resp->dat_buf[0] == 0)
            {   //The updating process cannot be started, due to requesting controller
                //is the SUC node or the SUC node is unknown.
                result = APPL_OPER_NOT_STARTED;
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_set_suc_node_id - Enable/disable a static/bridge controller to be a SUC/SIS node.
@param[in]	appl_ctx		Context
@param[in]	set_suc_param   The parameters.
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_set_suc_node_id(appl_layer_ctx_t   *appl_ctx, appl_set_suc_t *set_suc_param, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

/*
    A controller can set itself to a SUC/SIS by calling ZW_EnableSUC and ZW_SetSUCNodeID with its own
    node ID.
    In case ZW_SetSUCNodeID is called locally with the controllers own node ID then only the response is
    returned. In case true is returned in the response then it can be interpreted as the command is now
    executed successfully.
*/

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 4;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SET_SUC_NODE_ID;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x54 | nodeID | SUCState | bTxOption | capabilities | funcID
    cmd_prm->dat_buf[0] = set_suc_param->node_id;
    cmd_prm->dat_buf[1] = set_suc_param->suc_sta;
    cmd_prm->dat_buf[2] = set_suc_param->tx_opt;
    cmd_prm->dat_buf[3] = set_suc_param->cap;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x54 | RetVal
            if (resp->dat_buf[0] == 0)
            {   //The process cannot be started, due to the calling controller is not the master or the
                //destination node is not a static/bridge controller.
                result = APPL_OPER_NOT_STARTED;
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_send_suc_id - Transmit SUC node ID from a primary controller or static controller
                 to the controller node ID specified.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the controller to receive the current SUC node ID.
@param[in]	tx_opt          Transmit option flags.
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_send_suc_id(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t  tx_opt, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 2;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SEND_SUC_ID;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x57 | node | txOption | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->dat_buf[1] = tx_opt;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x57 | RetVal
            if (resp->dat_buf[0] == 0)
            {   //The process cannot be started, due to the controller is not a primary controller
                // or static controller.
                result = APPL_OPER_NOT_STARTED;
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}

/**
zw_get_suc_node_id - Get the currently registered SUC node ID.
@param[in]	appl_ctx		Context
@param[out]	suc_node_id     The SUC node id
@return  0 on success, negative error number on failure
*/
int32_t    zw_get_suc_node_id(appl_layer_ctx_t   *appl_ctx, uint8_t *suc_node_id)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_ZW_GET_SUC_NODE_ID;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x56 | SUCNodeID
            *suc_node_id = resp->dat_buf[0];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_is_failed_node - Test if a node ID is stored in the failed node ID list.
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node ID to be checked
@param[out]	res             Response: non-zero means the node ID is in the list of failing nodes;
                            zero means the node ID is not in the list.
@return  0 on command sent successfully to the controller, negative error number on failure
*/
int32_t    zw_is_failed_node(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t *res)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_IS_FAILED_NODE_ID;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x62 | nodeID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x62 | retVal
            *res = resp->dat_buf[0];
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_remove_failed_node_id - Remove a non-responding node from the routing table in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	cb              The callback function on remove process completion.
@param[out]	resp_flg        The bit-mask flag to indicate the reason of the removing operation is not started.
@return  0 on success, negative error number on failure. If APPL_OPER_NOT_STARTED is returned, check the resp_flg output.
@pre    The node must be on the failed node ID list and as an extra precaution also fail to respond before it is removed.
*/
int32_t    zw_remove_failed_node_id(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, uint8_t *resp_flg)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_REMOVE_FAILED_NODE_ID;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x61 | nodeID | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);

    *resp_flg = 0;
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x61 | retVal
            if (resp->dat_buf[0] != ZW_FAILED_NODE_REMOVE_STARTED)
            {
                result = APPL_OPER_NOT_STARTED;//the removing process cannot be started.
                *resp_flg = resp->dat_buf[0];
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_replace_failed_node - Replace a non-responding node with a new one in the requesting controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the failed node to be deleted.
@param[in]	cb              The callback function on replace process completion.
@param[out]	resp_flg        The bit-mask flag to indicate the reason of the replacing operation is not started.
@return  0 on success, negative error number on failure. If APPL_OPER_NOT_STARTED is returned, check the resp_flg output.
*/
int32_t    zw_replace_failed_node(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, uint8_t *resp_flg)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_REPLACE_FAILED_NODE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x63 | nodeID | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);

    *resp_flg = 0;
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x63 | retVal
            if (resp->dat_buf[0] != ZW_FAILED_NODE_REMOVE_STARTED)
            {
                result = APPL_OPER_NOT_STARTED;//the replacing process cannot be started.
                *resp_flg = resp->dat_buf[0];
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_request_node_neighbor_update - Get the neighbors from the specified node. Can only be called by a primary/inclusion controller.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the node that the controller wants to get new neighbors from.
@param[in]	cb              The callback function with the status of request.
@return  0 on success, negative error number on failure.
*/
int32_t    zw_request_node_neighbor_update(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x48 | nodeID | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;
    cmd_prm->cmd_cb_prm[1] = (void *)((unsigned)node_id);

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_replication_send - Send payload (applications specific data) when the controller is in replication mode
@param[in]	appl_ctx		Context
@param[in]	prm             The parameters
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_replication_send(appl_layer_ctx_t   *appl_ctx, appl_snd_data_t  *prm, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 3 + prm->dat_len;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_ZW_REPLICATION_SEND_DATA;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x45 | destNodeID | dataLength | pData[ ] | txOptions | funcID
    cmd_prm->dat_buf[0] = prm->node_id;
    cmd_prm->dat_buf[1] = prm->dat_len;
    memcpy(cmd_prm->dat_buf + 2, prm->dat_buf, prm->dat_len);
    cmd_prm->dat_buf[2 + prm->dat_len] = prm->tx_opt;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x45 | RetVal
            if (resp->dat_buf[0] == 0)
                result = APPL_TX_Q_OVERFLOW;
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}

/**
zw_node_info_cb - call back high-level application layer to report add node status
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_node_info_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    add_node_nw_cb_t    cb;          //Add node to network status callback function

    //ZW->HOST: REQ | 0xYY | funcID | bStatus | bSource | bLen | basic | generic | specific | cmdclasses[ ]
    //Offset:                  0         1         2       3        4       5         6           7

    cb = (add_node_nw_cb_t)param[0];

    if (cmd->len >= 4 && cb)
    {
        appl_node_info_t    node_info = {0};
        int                 parse_ok = 0;
        uint16_t            *cmd_cls_sec;
        uint8_t             cmd_cnt_sec;
        uint8_t             ni_len;     //Node info length: basic + generic + specific + command classes

        ni_len = cmd->dat_buf[3];

        if (ni_len > 0)
        {
            node_info.node_id = cmd->dat_buf[2];

            //Store basic + generic + specific
            if ((ni_len >= 3) && (cmd->len >= 7))
            {
                node_info.basic = cmd->dat_buf[4];
                node_info.gen = cmd->dat_buf[5];
                node_info.spec = cmd->dat_buf[6];
            }

            //Store command classes
            if ((ni_len > 3) && (cmd->len > 7))
            {
                //Check against buffer length
                if ((ni_len + 4) > cmd->len)
                {
                    ni_len = cmd->len - 4;
                }

                if (ni_len > 3)
                {
                    if(util_cmd_cls_parse(&cmd->dat_buf[7], ni_len - 3, &node_info.cmd_cls, &node_info.cmd_cnt,
                                          &cmd_cls_sec, &cmd_cnt_sec))
                    {
                        parse_ok = 1;
                    }
                }
            }
        }

        //printf("linhui zwnet_node_add_cb called at here.\n");
        cb(appl_ctx, cmd->dat_buf[1], cmd->dat_buf[2],
           (ni_len > 0)? &node_info : NULL);

        if (parse_ok)
        {
            free(node_info.cmd_cls);
            free(cmd_cls_sec);
        }
    }


/*    if(cmd->len >0)
    {
        plt_msg_show("linhui zw_add_node_to_network_cb, data size:%u, data:",
                              (unsigned)cmd->len);
        plt_bin_show(cmd->dat_buf, cmd->len);
    }*/

}


/**
zw_add_node_to_network - Add node to a network
@param[in]	appl_ctx		Context
@param[in]	mode            Mode to control the add node process.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int32_t    zw_add_node_to_network(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    /*
    WARNING: It is not allowed to call this function between a
    ADD_NODE_STATUS_ADDING_* and a ADD_NODE_STATUS_PROTOCOL_DONE callback status,
    doing this can result in malfunction of the protocol.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_ADD_NODE_TO_NETWORK;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x4A | mode | funcID
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_node_info_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_remove_node_from_network - Remove node from a network
@param[in]	appl_ctx		Context
@param[in]	mode            Mode to control the remove node process.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int32_t    zw_remove_node_from_network(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    /*
    WARNING: It is not allowed to call zw_remove_node_from_network() between a
    REMOVE_NODE_STATUS_REMOVING_* and a REMOVE_NODE_STATUS_DONE callback status,
    doing this can result in malfunction of the protocol.

    WARNING: It is not allowed to call zw_remove_node_from_network() with REMOVE_NODE_ANY,
    REMOVE_NODE_SLAVE or REMOVE_CONTROLLER from a callback function from
    zw_remove_node_from_network()
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x4B | mode | funcID
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_node_info_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_create_new_primary_ctrl - Facilitate a SUC to add a controller to the Z-Wave network as a replacement
                             for the old primary controller.
@param[in]	appl_ctx		Context
@param[in]	mode            The learn node state.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int32_t    zw_create_new_primary_ctrl(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    /*
    This function has the same functionality as ZW_AddNodeToNetwork(ADD_NODE_CONTROLLER,…)
    except that the new controller will be a primary controller and it can only be called by a SUC. The
    function is not available if the SUC is a node ID server (SIS).

    WARNING: This function should only be used when it is 100% certain that the original primary controller
    is lost or broken and will not return to the network.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_CREATE_NEW_PRIMARY;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x4C | mode | funcID
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_node_info_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_controller_change - Add a controller to the Z-Wave network and transfer the role as
                       primary controller to it. The controller invoking this function will become
                       secondary.
@param[in]	appl_ctx		Context
@param[in]	mode            The learn node state.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int32_t    zw_controller_change(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    /*
    WARNING: It is not allowed to call this function between a
    ADD_NODE_STATUS_ADDING_* and a ADD_NODE_STATUS_PROTOCOL_DONE callback status,
    doing this can result in malfunction of the protocol.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_CONTROLLER_CHANGE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x4D | mode | funcID
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_node_info_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_set_learn_mode_cb - call back high-level application layer to report learn mode status
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_set_learn_mode_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    set_lrn_mod_cb_t    cb;          //Set learn mode status callback function

    //ZW->HOST: REQ | 0x50 | funcID | bStatus | bSource | bLen | pCmd[ ]

    cb = (set_lrn_mod_cb_t)param[0];

    if (cmd->len >= 4 && cb)
    {
        cb(appl_ctx, cmd->dat_buf[1], cmd->dat_buf[2], cmd->dat_buf[3],
           (cmd->dat_buf[3] > 0)? (cmd->dat_buf + 4) : NULL);
    }

}


/**
zw_set_learn_mode - Set learn mode, used to add or remove the controller to/from a Z-Wave network.
@param[in]	appl_ctx		Context
@param[in]	mode            ZW_SET_LEARN_MODE_CLASSIC/ZW_SET_LEARN_MODE_NWI: Start the learn mode on the controller; ZW_SET_LEARN_MODE_DISABLE: Stop the learn mode.
@param[in]	cb              The callback function to report status.
@return  0 on success, negative error number on failure
*/
int32_t    zw_set_learn_mode(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, set_lrn_mod_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    uint8_t prm_len;    //the length of the parameters

    /*
    WARNING: The learn process should not be stopped between the
    LEARN_MODE_STARTED and the LEARN_MODE_DONE status callback.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SET_LEARN_MODE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x50 | mode | funcID
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_set_learn_mode_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;


    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_set_default_cb - call back high-level application layer to report completion
@param[in]	ssn_ctx		Session layer context
@param[in]	cmd		    The received callback command
@param[in]	param		User defined parameters for the callback
@return
*/
static void    zw_set_default_cb(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *cmd, void **param)
{
    appl_layer_ctx_t    *appl_ctx = ssn_ctx->appl_layer_ctx;
    set_deflt_cb_t    cb;          //Set learn mode status callback function

    //ZW->HOST: REQ | 0x42 | funcID

    cb = (set_deflt_cb_t)param[0];

    if (cb)
        cb(appl_ctx);
}


/**
zw_set_default - Set the Controller back to the factory default state
@param[in]	appl_ctx		Context
@param[in]	cb              The callback function to report completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_set_default(appl_layer_ctx_t   *appl_ctx, set_deflt_cb_t cb)
{
    int32_t result;
    ssn_cmd_snd_param_t    *cmd_prm;
    /*
    NOTE: This function should not be used on a secondary controller, use zw_set_learn_mode() instead
    and use the primary controller to remove it from the network.
    */

    /*
    Warning: This function should be used with care as it could render a Z-Wave network unusable if the
    primary controller in an existing network is set back to default.
    */
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;


    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t));

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SET_DEFAULT;
    cmd_prm->dat_sz = 0;
    //HOST->ZW: REQ | 0x42 | funcID
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK;
    cmd_prm->cmd_cb_func = zw_set_default_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;


    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, NULL);
    plt_mtx_ulck(appl_ctx->snd_mtx);

    free(cmd_prm);
    return result;
}


/**
zw_version - Get the Z-Wave basis API library version.
@param[in]	appl_ctx		Context
@param[out]	lib_ver         A null terminated library version string with the format "Z-Wave x.yy".
                            Caller should ensure the supplied buffer is large enough to store the string.
@param[out]	lib_type        The library type
@return  0 on success, negative error number on failure
*/
int32_t    zw_version(appl_layer_ctx_t   *appl_ctx, uint8_t  *lib_ver, uint8_t *lib_type)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_ZW_GET_VERSION;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x15 | buffer (12 bytes) | library type
            memcpy(lib_ver, resp->dat_buf, 12);
            *lib_type = resp->dat_buf[12];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_get_controller_capabilities - Get the controller capabilities.
@param[in]	appl_ctx		Context
@param[out]	cap             The controller capabilities bit mask
@return  0 on success, negative error number on failure
*/
int32_t    zw_get_controller_capabilities(appl_layer_ctx_t   *appl_ctx, uint8_t *cap)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_ZW_GET_CONTROLLER_CAPABILITIES;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x05 | RetVal
            *cap = resp->dat_buf[0];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}

/**
zw_memory_get_id - Get the home and node id.
@param[in]	appl_ctx		Context
@param[out]	home_id         The 4-byte home id.
@param[out]	node_id         The node id
@return  0 on success, negative error number on failure
*/
int32_t    zw_memory_get_id(appl_layer_ctx_t   *appl_ctx, uint32_t  *home_id, uint8_t *node_id)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_MEMORY_GET_ID;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x20 | HomeId(4 bytes) | NodeId
            *home_id = ((uint32_t)resp->dat_buf[0])<<24 | ((uint32_t)resp->dat_buf[1])<<16
                       | ((uint32_t)resp->dat_buf[2])<<8 | ((uint32_t)resp->dat_buf[3]);
            *node_id = resp->dat_buf[4];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}

/**
zw_memory_get_buffer - Read a number of bytes from the non-volatile memory allocated for the application.
@param[in]	appl_ctx		Context
@param[out]	buf             Buffer to store the read data.
@param[in]	ofs             Offset from the non-volatile memory (allocated for the application) to read
@param[in]	len             Length of the data to read
@return  0 on success, negative error number on failure
*/
int32_t    zw_memory_get_buffer(appl_layer_ctx_t   *appl_ctx, uint8_t  *buf,
                                uint16_t ofs, uint8_t len)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 3;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    //HOST->ZW: REQ | 0x23 | offset(2 bytes) | length
    cmd_prm->cmd_id = FUNC_ID_MEMORY_GET_BUFFER;
    cmd_prm->cmd_flag = COMMAND_HAS_RESPONSE;
    cmd_prm->dat_sz = prm_len;
    cmd_prm->dat_buf[0] = (uint8_t)(ofs >> 8);
    cmd_prm->dat_buf[1] = (uint8_t)(ofs & 0x00FF);
    cmd_prm->dat_buf[2] = len;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {
            //ZW->HOST: RES | 0x23 | buffer[]
            memcpy(buf, resp->dat_buf, len);
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);

    return result;
}

/**
zw_memory_put_buffer - Write to the application area of the non-volatile memory.
@param[in]	appl_ctx		Context
@param[in]	buf             Buffer to store the data to be written.
@param[in]	ofs             Offset from the non-volatile memory (allocated for the application) to write
@param[in]	len             Length of the data to  be written
@param[in]	cb              The callback function on write completion.
@param[in]	user_prm        The callback function parameter.
@return  0 on success, negative error number on failure
*/
int32_t    zw_memory_put_buffer(appl_layer_ctx_t   *appl_ctx, uint8_t  *buf, uint16_t ofs,
                                uint16_t len, wr_cmplt_cb_t cb, void *user_prm)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 4 + len;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }


    cmd_prm->cmd_id = FUNC_ID_MEMORY_PUT_BUFFER;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x24 | offset(2bytes) | length(2bytes) | buffer[ ] | funcID
    cmd_prm->dat_buf[0] = (uint8_t)(ofs >> 8);
    cmd_prm->dat_buf[1] = (uint8_t)(ofs & 0x00FF);
    cmd_prm->dat_buf[2] = (uint8_t)(len >> 8);
    cmd_prm->dat_buf[3] = (uint8_t)(len & 0x00FF);
    memcpy(cmd_prm->dat_buf + 4, buf, len);
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_cmplt_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;
    cmd_prm->cmd_cb_prm[1] = (void *)user_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x24 | retVal
            if (resp->dat_buf[0] == FALSE)
                result = ZWHCI_ERROR_MEMORY;//buffer put queue is full.
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}

/**
zw_serial_api_capabilities - Get the host controller interface (serial api) capabilities.
@param[in]	appl_ctx		Context
@param[out]	cap             The host controller interface (serial api) capabilities.
@return  0 on success, negative error number on failure
*/
int32_t    zw_serial_api_capabilities(appl_layer_ctx_t   *appl_ctx, appl_hci_cap_t  *cap)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_SERIAL_API_GET_CAPABILITIES;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0 && resp->len >= 40)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x07 | SERIAL_APPL_VERSION | SERIAL_APPL_REVISION |
            //SERIALAPI_MANUFACTURER_ID1 | SERIALAPI_MANUFACTURER_ID2 |
            //SERIALAPI_MANUFACTURER_PRODUCT_TYPE1 |
            //SERIALAPI_MANUFACTURER_PRODUCT_TYPE2 |
            //SERIALAPI_MANUFACTURER_PRODUCT_ID1 | SERIALAPI_MANUFACTURER_PRODUCT_ID2 |
            //FUNCID_SUPPORTED_BITMASK[ ]

            cap->version = resp->dat_buf[0];
            cap->revision = resp->dat_buf[1];
            cap->manf_id = ((uint16_t)resp->dat_buf[2])<<8 | ((uint16_t)resp->dat_buf[3]);
            cap->manf_pdt_typ = ((uint16_t)resp->dat_buf[4])<<8 | ((uint16_t)resp->dat_buf[5]);
            cap->manf_pdt_id = ((uint16_t)resp->dat_buf[6])<<8 | ((uint16_t)resp->dat_buf[7]);
            memcpy(cap->func_id, resp->dat_buf + 8, 32);

            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_serial_api_get_init_data - Get the initialization data stored.
@param[in]	appl_ctx		Context
@param[out]	init_data       The initialization data stored.
@return  0 on success, negative error number on failure
*/
int32_t    zw_serial_api_get_init_data(appl_layer_ctx_t   *appl_ctx, appl_init_dat_t  *init_data)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

#ifdef DEBUG_SERIAL_API
    debug_msg_show(appl_ctx->plt_ctx, "*************************");
    debug_msg_show(appl_ctx->plt_ctx, "Num of NAK=%u, CAN=%u", appl_ctx->ssn_ctx.frm_ctx.nak_cnt, appl_ctx->ssn_ctx.frm_ctx.can_cnt);
#endif
    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_SERIAL_API_GET_INIT_DATA;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0 && resp->len >= 3)
    {
        if (resp)
        {   //(Controller) ZW->HOST: RES | 0x02 | ver | capabilities | 29 | nodes[29] | chip_type | chip_version
            //(Slave) ZW->HOST:      RES | 0x02 | ver | capabilities | 0 | chip_type | chip_version


            init_data->version = resp->dat_buf[0];
            init_data->cap = resp->dat_buf[1];
            if (resp->dat_buf[2] > 0)
                memcpy(init_data->nodes, resp->dat_buf + 3, 29);//controller
            else
                memset(init_data->nodes, 0, 29);//slave

            init_data->chip_typ = resp->dat_buf[3 + resp->dat_buf[2]];
            init_data->chip_ver = resp->dat_buf[4 + resp->dat_buf[2]];

            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}

/**
zw_get_node_protocol_info - Get the Node Information Frame without command
                            classes from the non-volatile memory for a given
                            node ID.
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node id
@param[out]	node_info       The node info frame of 6 bytes
@return  0 on success, negative error number on failure
*/
int32_t    zw_get_node_protocol_info(appl_layer_ctx_t   *appl_ctx,
                                     uint8_t  node_id,
                                     uint8_t *node_info)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));
    //HOST->ZW: REQ | 0x41 | bNodeID
    cmd_prm.cmd_id = FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;
    cmd_prm.dat_sz = 1;
    cmd_prm.dat_buf[0] = node_id;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {
            //ZW->HOST: RES | 0x41 | capability | security | reserved | basic | generic | specific
            memcpy(node_info, resp->dat_buf, resp->len);
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}

/**
zw_get_rf_powerlevel - Get the current power level used in RF transmitting.
@param[in]	appl_ctx		Context
@param[out]	pbyPowerLvl     1 byte power level value
@return  0 on success, negative error number on failure
*/
int32_t    zw_get_rf_powerlevel(appl_layer_ctx_t   *appl_ctx,
                                     uint8_t *pbyPowerLvl)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));

    cmd_prm.cmd_id = FUNC_ID_ZW_RF_POWER_LEVEL_GET;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0 && resp->len >= 1)
    {
        if (resp)
        {   //ZW->HOST: RES | 0xBA | powerlevel
            *pbyPowerLvl = resp->dat_buf[0];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_set_rf_powerlevel - Set the current power level used in RF transmitting.
@param[in]	appl_ctx		Context
@param[in]	byPowerLvl      1 byte power level value to set down
@param[out]	pbyPowerLvlRet  (Optional) The actual power level used
@return  0 on success, negative error number on failure
*/
int32_t    zw_set_rf_powerlevel(appl_layer_ctx_t   *appl_ctx,
                                     uint8_t    byPowerLvl,
									 uint8_t	*pbyPowerLvlRet)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));
	//HOST->ZW: REQ | 0x17 | powerLevel
    cmd_prm.cmd_id = FUNC_ID_ZW_RF_POWER_LEVEL_SET;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;
	cmd_prm.dat_sz = 1;
    cmd_prm.dat_buf[0] = byPowerLvl;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0 && resp->len >= 1)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x17 | retVal
			if(pbyPowerLvlRet)
				*pbyPowerLvlRet = resp->dat_buf[0];
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_send_test_frame - Send a test frame to a node ID without any routing.
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) of the target node which will receive the test frame.
@param[in]	byPowerLvl      The power level the test frame is acknowledged at.
@param[in]	cb              The callback function on test completion.
@param[in]  cb_prm			The parameter to be passed when invoking cb callback function
@return  0 on success, negative error number on failure
*/
int32_t    zw_send_test_frame(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t byPowerLvl, tx_cmplt_cb_t cb, void *cb_prm)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 2;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SEND_TEST_FRAME;
    cmd_prm->dat_sz = prm_len;
	//HOST->ZW: REQ | 0xBE | nodeID| powerlevel | funcID
    cmd_prm->dat_buf[0] = node_id;
	cmd_prm->dat_buf[1] = byPowerLvl;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;
	cmd_prm->cmd_cb_prm[1] = cb_prm;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);

    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: REQ | 0xBE | retVal
            if (resp->dat_buf[0] == 0)
                result = APPL_TX_Q_OVERFLOW;
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_request_node_info - Request the node information frame from a controller based node in the network.
                       The node info is return through application_update callback.
@param[in]	appl_ctx		Context
@param[in]	node_id		    Node id of the request
@return  0 on success, negative error number on failure
*/
int32_t    zw_request_node_info(appl_layer_ctx_t   *appl_ctx, uint8_t  node_id)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm = {0};
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    //HOST->ZW: REQ | 0x60 | NodeID
    cmd_prm.cmd_id = FUNC_ID_ZW_REQUEST_NODE_INFO;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;
    cmd_prm.dat_sz = 1;
    cmd_prm.dat_buf[0] = node_id;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);

    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x60 | RetVal
            if (resp->dat_buf[0] == 0)
                result = APPL_TX_Q_OVERFLOW;
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_set_rf_receive_mode - Power down the RF or set the RF into receive mode
@param[in]	appl_ctx	Context
@param[in]	mode        TRUE = turn on the RF in receive mode; FALSE = power down the RF
@return  0 on success, negative error number on failure
*/
int32_t    zw_set_rf_receive_mode(appl_layer_ctx_t   *appl_ctx, uint8_t mode)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 1;//total data length

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SET_RF_RECEIVE_MODE;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x10 | mode
    cmd_prm->dat_buf[0] = mode;
    cmd_prm->cmd_flag = COMMAND_HAS_RESPONSE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x10 | retVal
            if (resp->dat_buf[0] == FALSE);
                result = APPL_OPER_NOT_STARTED;
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_get_random_word - Generate random words
@param[in]	    appl_ctx		Context
@param[in, out]	count		    Number of random bytes required[in]; number of random bytes generated[out]
@param[out]	    buf             The buffer to store the generated random bytes
@return  0 on success, negative error number on failure
*/
int32_t    zw_get_random_word(appl_layer_ctx_t   *appl_ctx,
                                     uint8_t *count,
                                     uint8_t *buf)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;
    ssn_cmd_resp_t  *resp;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));
    //HOST->ZW: REQ | 0x1C | [noRandomBytes]
    cmd_prm.cmd_id = FUNC_ID_ZW_GET_RANDOM;
    cmd_prm.cmd_flag = COMMAND_HAS_RESPONSE;
    cmd_prm.dat_sz = 1;
    cmd_prm.dat_buf[0] = *count;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {
            //ZW->HOST: RES | 0x1C | randomGenerationSuccess | noRandomBytesGenerated |
            //noRandomGenerated[noRandomBytesGenerated]
            if (resp->dat_buf[0] == TRUE)
            {
                *count = resp->dat_buf[1];
                memcpy(buf, resp->dat_buf + 2, *count);
            }
            else
            {
                *count = 0;
            }
            free(resp);
        }
    }
    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_soft_reset - Software reset Z-Wave module.
@param[in]	appl_ctx		Context
@return  0 on success, negative error number on failure
*/
int32_t    zw_soft_reset(appl_layer_ctx_t   *appl_ctx)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm = {0};

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    //HOST->ZW: REQ | 0x08
    cmd_prm.cmd_id = FUNC_ID_SERIAL_API_SOFT_RESET;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, NULL);

    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_send_data_abort - Abort the ongoing transmission.
@param[in]	appl_ctx		Context
@return  0 on success, negative error number on failure
*/
int32_t    zw_send_data_abort(appl_layer_ctx_t   *appl_ctx)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;

    //if (!appl_wait_to_snd(appl_ctx))
        //return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));
    //HOST->ZW: REQ | 0x16
    cmd_prm.cmd_id = FUNC_ID_ZW_SEND_DATA_ABORT;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, NULL);

    //plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_replication_receive_complete - Sends command completed to sending controller to indicate
                                  ready to receive further replication data.
@param[in]	appl_ctx		Context
@return  0 on success, negative error number on failure
*/
int32_t    zw_replication_receive_complete(appl_layer_ctx_t   *appl_ctx)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm;

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    memset(&cmd_prm, 0, sizeof(ssn_cmd_snd_param_t));
    //HOST->ZW: REQ | 0x44
    cmd_prm.cmd_id = FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, NULL);

    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zw_send_node_info - Create and transmit a “Node Information” frame to the node ID specified
@param[in]	appl_ctx		Context
@param[in]	node_id         The Node ID (1...232) or 0xFF (broadcast) of the node to receive the “Node Information” frame.
@param[in]	tx_opt          Transmit option flags.
@param[in]	cb              The callback function on transmit completion.
@return  0 on success, negative error number on failure
*/
int32_t    zw_send_node_info(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t  tx_opt, tx_cmplt_cb_t cb)
{
    int32_t                 result;
    ssn_cmd_snd_param_t     *cmd_prm;
    ssn_cmd_resp_t          *resp;
    uint8_t                 prm_len;    //the length of the parameters

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    prm_len = 2;//total data length excluding function id

    cmd_prm = (ssn_cmd_snd_param_t *)calloc(1, sizeof(ssn_cmd_snd_param_t) + prm_len);

    if (!cmd_prm)
    {
        plt_mtx_ulck(appl_ctx->snd_mtx);
        return  ZWHCI_ERROR_MEMORY;
    }

    cmd_prm->cmd_id = FUNC_ID_ZW_SEND_NODE_INFORMATION;
    cmd_prm->dat_sz = prm_len;
    //HOST->ZW: REQ | 0x12 | destNode | txOption | funcID
    cmd_prm->dat_buf[0] = node_id;
    cmd_prm->dat_buf[1] = tx_opt;
    cmd_prm->cmd_flag = COMMAND_HAS_CALLBACK | COMMAND_HAS_RESPONSE;
    cmd_prm->cmd_cb_func = zw_generic_cb;
    cmd_prm->cmd_cb_prm[0] = (void *)cb;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, cmd_prm, &resp);
    if (result == 0)
    {
        if (resp)
        {   //ZW->HOST: RES | 0x12 | RetVal
            if (resp->dat_buf[0] == 0)
            {   //Frame was not put in the transmit queue
                result = APPL_TX_Q_OVERFLOW;
            }
            free(resp);
        }
    }

    plt_mtx_ulck(appl_ctx->snd_mtx);
    free(cmd_prm);
    return result;
}


/**
zw_flash_auto_prog_mode - Enables the Auto Program Mode and resets the 500 Sereis Z-Wave
                          Single Chip after 7.8ms
@param[in]	appl_ctx    Context
@return  0 on success, negative error number on failure
*/
int    zw_flash_auto_prog_mode(appl_layer_ctx_t *appl_ctx)
{
    int32_t result;
    ssn_cmd_snd_param_t    cmd_prm = {0};

    if (!appl_wait_to_snd(appl_ctx))
        return  APPL_ERROR_WAIT_CB;

    //HOST->ZW: REQ | 0x27
    cmd_prm.cmd_id = FUNC_ID_AUTO_PROGRAMMING;

    result = ssn_cmd_snd(&appl_ctx->ssn_ctx, &cmd_prm, NULL);

    plt_mtx_ulck(appl_ctx->snd_mtx);

    return result;
}


/**
zwhci_init - Init the application layer.
Should be called once before calling the other application layer functions
@param[in,out]	appl_ctx		Context
@param[in]	comm_port_id		Pointer to platform specific serial comm port id/name
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the context after this call
*/
int32_t zwhci_init(appl_layer_ctx_t   *appl_ctx, void   *comm_port_id)
{
    int32_t ret_val;

    //Zeroes the session layer context
    memset(&appl_ctx->ssn_ctx, 0, sizeof(ssn_layer_ctx_t));

    //Save the comm port id
    appl_ctx->ssn_ctx.frm_ctx.tpt_ctx.comm_port_id = comm_port_id;

    //Init session layer
    appl_ctx->ssn_ctx.unsolicited_cmd_cb = appl_unsolicited_cmd_cb;
    appl_ctx->ssn_ctx.resp_tmout_ms = SESSION_RESPONSE_TIMEOUT_MIN;
    appl_ctx->ssn_ctx.appl_layer_ctx = appl_ctx;
    appl_ctx->ssn_ctx.nm_cb = zw_nm_cb;
    appl_ctx->ssn_ctx.plt_ctx = appl_ctx->plt_ctx;

    ret_val = ssn_init(&appl_ctx->ssn_ctx);

    if (ret_val != 0)
        return ret_val;

    //Init application layer
    appl_ctx->wait_cmd_cb = 0;
    appl_ctx->wait_nm_cb = 0;

    if (appl_ctx->cb_tmout_ms < APPL_CB_TMOUT_MIN)
        appl_ctx->cb_tmout_ms = APPL_CB_TMOUT_MIN;

    if (!plt_mtx_init(&appl_ctx->snd_mtx))
    {
        ssn_exit(&appl_ctx->ssn_ctx);
        return INIT_ERROR_APPL;
    }

    if (plt_cond_init(&appl_ctx->snd_cv) == 0)
    {
        ssn_exit(&appl_ctx->ssn_ctx);
        plt_mtx_destroy(appl_ctx->snd_mtx);
        return INIT_ERROR_APPL;
    }

    return 0;

}


/**
zwhci_exit - Clean up the application layer
@param[in,out]	appl_ctx		Context
@return
*/
void zwhci_exit(appl_layer_ctx_t   *appl_ctx)
{
    ssn_exit(&appl_ctx->ssn_ctx);
    plt_mtx_destroy(appl_ctx->snd_mtx);
    plt_cond_destroy(appl_ctx->snd_cv);
}

/**
@}
*/


