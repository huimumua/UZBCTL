/**
@file   zw_api_base.c - Z-wave High Level API base object, node, endpoint, interface, device implementation.


@author David Chow

@version    1.0 7-7-10  Initial release

version: 1.0
comments: Initial release
*/

#include <ctype.h>
#include "../include/zw_api_pte.h"
#include "../include/zw_security.h"
#include "../include/zw_api_util.h"
#include "../include/zw_poll.h"

//#define  STRESS_TEST_NO_ROUTING         //For stress test without routing

static void zwif_get_desc_dat(zwif_p intf, zwifd_p desc);

/**
@defgroup Base Base object APIs
@{
*/

static const cmd_get_resp_t cmd_get_resp_tbl[] =
{   //Format: command class, get command, report command
    {COMMAND_CLASS_SENSOR_BINARY, SENSOR_BINARY_GET, SENSOR_BINARY_REPORT},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_GET, SENSOR_MULTILEVEL_REPORT},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5, SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5},
    {COMMAND_CLASS_SENSOR_MULTILEVEL, SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5, SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5},
    {COMMAND_CLASS_METER, METER_GET, METER_REPORT},
    {COMMAND_CLASS_METER, METER_SUPPORTED_GET_V2, METER_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_METER_TBL_MONITOR, METER_TBL_TABLE_ID_GET, METER_TBL_TABLE_ID_REPORT},
    {COMMAND_CLASS_METER_PULSE, METER_PULSE_GET, METER_PULSE_REPORT},
    {COMMAND_CLASS_SIMPLE_AV_CONTROL, SIMPLE_AV_CONTROL_GET, SIMPLE_AV_CONTROL_REPORT},
    {COMMAND_CLASS_CONFIGURATION, CONFIGURATION_GET, CONFIGURATION_REPORT},
    {COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_GET, SWITCH_MULTILEVEL_REPORT},
    {COMMAND_CLASS_SWITCH_MULTILEVEL, SWITCH_MULTILEVEL_SUPPORTED_GET_V3, SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3},
    {COMMAND_CLASS_SWITCH_BINARY, SWITCH_BINARY_GET, SWITCH_BINARY_REPORT},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GET, ASSOCIATION_REPORT},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_SPECIFIC_GROUP_GET_V2, ASSOCIATION_SPECIFIC_GROUP_REPORT_V2},
    {COMMAND_CLASS_ASSOCIATION, ASSOCIATION_GROUPINGS_GET, ASSOCIATION_GROUPINGS_REPORT},
    {COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, ASSOCIATION_GET, ASSOCIATION_REPORT},
    {COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, ASSOCIATION_GROUPINGS_GET, ASSOCIATION_GROUPINGS_REPORT},
    {COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION, COMMAND_RECORDS_SUPPORTED_GET, COMMAND_RECORDS_SUPPORTED_REPORT},
    {COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION, COMMAND_CONFIGURATION_GET, COMMAND_CONFIGURATION_REPORT},
    {COMMAND_CLASS_WAKE_UP, WAKE_UP_INTERVAL_GET, WAKE_UP_INTERVAL_REPORT},
    {COMMAND_CLASS_BASIC, BASIC_GET, BASIC_REPORT},
    {COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_OPERATION_GET, DOOR_LOCK_OPERATION_REPORT},
    {COMMAND_CLASS_DOOR_LOCK, DOOR_LOCK_CONFIGURATION_GET, DOOR_LOCK_CONFIGURATION_REPORT},
    {COMMAND_CLASS_USER_CODE, USER_CODE_GET, USER_CODE_REPORT},
    {COMMAND_CLASS_USER_CODE, USERS_NUMBER_GET, USERS_NUMBER_REPORT},
    {COMMAND_CLASS_ALARM, ALARM_GET, ALARM_REPORT},
    {COMMAND_CLASS_ALARM, ALARM_TYPE_SUPPORTED_GET_V2, ALARM_TYPE_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_NOTIFICATION_V3, EVENT_SUPPORTED_GET_V3, EVENT_SUPPORTED_REPORT_V3},
    {COMMAND_CLASS_BATTERY, BATTERY_GET, BATTERY_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_MODE, THERMOSTAT_FAN_MODE_GET, THERMOSTAT_FAN_MODE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_MODE, THERMOSTAT_FAN_MODE_SUPPORTED_GET, THERMOSTAT_FAN_MODE_SUPPORTED_REPORT},
    {COMMAND_CLASS_THERMOSTAT_FAN_STATE, THERMOSTAT_FAN_STATE_GET, THERMOSTAT_FAN_STATE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_GET, THERMOSTAT_MODE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_MODE, THERMOSTAT_MODE_SUPPORTED_GET, THERMOSTAT_MODE_SUPPORTED_REPORT},
    {COMMAND_CLASS_THERMOSTAT_OPERATING_STATE, THERMOSTAT_OPERATING_STATE_GET, THERMOSTAT_OPERATING_STATE_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETBACK, THERMOSTAT_SETBACK_GET, THERMOSTAT_SETBACK_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_GET, THERMOSTAT_SETPOINT_REPORT},
    {COMMAND_CLASS_THERMOSTAT_SETPOINT, THERMOSTAT_SETPOINT_SUPPORTED_GET, THERMOSTAT_SETPOINT_SUPPORTED_REPORT},
    {COMMAND_CLASS_CLOCK, CLOCK_GET, CLOCK_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_GET, SCHEDULE_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_CHANGED_GET, SCHEDULE_CHANGED_REPORT},
    {COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE, SCHEDULE_OVERRIDE_GET, SCHEDULE_OVERRIDE_REPORT},
    {COMMAND_CLASS_PROTECTION, PROTECTION_GET, PROTECTION_REPORT},
    {COMMAND_CLASS_PROTECTION, PROTECTION_SUPPORTED_GET_V2, PROTECTION_SUPPORTED_REPORT_V2},
    {COMMAND_CLASS_PROTECTION, PROTECTION_EC_GET_V2, PROTECTION_EC_REPORT_V2},
    {COMMAND_CLASS_PROTECTION, PROTECTION_TIMEOUT_GET_V2, PROTECTION_TIMEOUT_REPORT_V2},
#ifdef  TEST_EXT_CMD_CLASS
    {COMMAND_CLASS_EXT_TEST, BASIC_GET, BASIC_REPORT},
#endif
    {COMMAND_CLASS_INDICATOR, INDICATOR_GET, INDICATOR_REPORT},
    {COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_MD_GET, FIRMWARE_MD_REPORT},
    {COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_REQUEST_GET, FIRMWARE_UPDATE_MD_REQUEST_REPORT},
    //{COMMAND_CLASS_FIRMWARE_UPDATE_MD, FIRMWARE_UPDATE_MD_GET, FIRMWARE_UPDATE_MD_REPORT}//For controller, it won't send FIRMWARE_UPDATE_MD_GET
	{COMMAND_CLASS_POWERLEVEL, POWERLEVEL_GET, POWERLEVEL_REPORT},
    {COMMAND_CLASS_POWERLEVEL, POWERLEVEL_TEST_NODE_GET, POWERLEVEL_TEST_NODE_REPORT},
    {COMMAND_CLASS_DOOR_LOCK_LOGGING, DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET, DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT},
    {COMMAND_CLASS_DOOR_LOCK_LOGGING, RECORD_GET, RECORD_REPORT}

};

/**
zwobj_add - Add object to list
@param[in,out]	head	list head
@param[in]		obj		object
*/
void zwobj_add(zwobj_p *head, zwobj_p obj)
{
    zwobj_p temp;

    if (*head == NULL)
    {
        obj->next = NULL;
        *head = obj;
        return;
    }

    temp = *head;
    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = obj;
    obj->next = NULL;

}

/**
zwobj_del - Remove object from list
@param[in,out]	head	list head
@param[in]      obj     object
@return		Non-zero on success; otherwise zero if object not found in the list.
*/
int32_t zwobj_del(zwobj_p *head, zwobj_p obj)
{
    zwobj_p temp;

    if (*head == obj)
    {
        *head = (*head)->next;
        free(obj);
        return 1;
    }

    temp = *head;
    while (temp->next)
    {
        if (temp->next == obj)
        {
            temp->next = temp->next->next;
            free(obj);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


/**
zwobj_rplc - Replace an object with a new one
@param[in,out]	head	list head
@param[in]      obj     object to be replaced
@param[in]      new_obj new object
@return		Non-zero on success; otherwise zero if object not found in the list.
@post   The obj will be invalid on success, don't use it hereafter.
*/
int32_t zwobj_rplc(zwobj_p *head, zwobj_p obj, zwobj_p new_obj)
{
    zwobj_p temp;

    if (*head == obj)
    {
        new_obj->ctx = obj->ctx;
        new_obj->next = obj->next;

        *head = new_obj;
        free(obj);
        return 1;
    }

    temp = *head;
    while (temp->next)
    {
        if (temp->next == obj)
        {
            new_obj->ctx = obj->ctx;
            new_obj->next = obj->next;

            temp->next = new_obj;
            free(obj);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/**
@}
@addtogroup Node Node APIs
@{
*/


/**
zwnode_get_net - Get associated network
@param[in]	noded	node
@return		network
*/
zwnet_p zwnode_get_net(zwnoded_p noded)
{
    return noded->net;
}


/**
zwnode_get_next - get next node in network
@param[in]	noded	    The initial node for searching the next node
@param[out]	nxt_node_desc	The next node if found
@return		ZW_ERR_XXX
*/
int zwnode_get_next(zwnoded_p noded, zwnoded_p nxt_noded)
{
    int         result;
    zwnode_p    node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node && node->obj.next)
    {   //Found next node
        node = (zwnode_p)node->obj.next;
        result = zwnode_get_desc(node, nxt_noded);
        plt_mtx_ulck(noded->net->mtx);
        return result;
    }
    plt_mtx_ulck(noded->net->mtx);
    return  ZW_ERR_NODE_NOT_FOUND;
}

/**
zwnode_find - find a node based on node id
@param[in]	first_node	first node in the network
@param[in]	nodeid	    node id
@return		node if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwnode_p zwnode_find(zwnode_p first_node, uint8_t nodeid)
{
    zwnode_p    temp_node;

    temp_node = first_node;
    while (temp_node)
    {
        if (temp_node->nodeid == nodeid)
        {
            return temp_node;
        }
        temp_node = (zwnode_p)temp_node->obj.next;
    }

    return  NULL;
}


/**
zwnode_probe - send a "no operation" command to a node to test if it's reachable
@param[in]	node        node
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@return	ZW_ERR_xxx
*/
int zwnode_probe(zwnode_p node, tx_cmplt_cb_t cb, void *user)
{
    int                 result;
    appl_snd_data_t     prm = {0};

    prm.dat_buf[0] = COMMAND_CLASS_NO_OPERATION;
    prm.dat_len = 1;
    prm.node_id = node->nodeid;
    prm.tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE;

    result = zw_send_data(&node->net->appl_ctx, &prm, cb, user);

    if (result < 0)
    {
        if (result == APPL_ERROR_WAIT_CB)
        {
            result = ZW_ERR_BUSY;
        }
        else
        {
            result = ZW_ERR_OP_FAILED;
        }
    }

    return result;
}


/**
zwnode_set_user - set user specific information
@param[in]	noded	Node
@param[in]	user	user specific information
*/
void zwnode_set_user(zwnoded_p noded, void *user)
{
    zwnode_p node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {   //Found the node
        node->obj.ctx = user;
    }
    plt_mtx_ulck(noded->net->mtx);
}


/**
zwnode_get_user - get user specific information
@param[in]	noded	Node
@return	user specific information if found; else return NULL
*/
void *zwnode_get_user(zwnoded_p noded)
{
    zwnode_p node;
    void *ret_val;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    ret_val = (node)? node->obj.ctx : NULL;

    plt_mtx_ulck(noded->net->mtx);

    return ret_val;
}


/**
zwnode_get_ext_ver - get extended version information
@param[in]	noded	Node
@return	extended version information if the node supports it; else return NULL
@post   Caller has to free the returned extended version information
*/
ext_ver_t *zwnode_get_ext_ver(zwnoded_p noded)
{
    zwnode_p    node;
    ext_ver_t   *ext_ver = NULL;

    if (!noded)
    {
        return NULL;
    }

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node && node->ext_ver)
    {
        ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (node->ext_ver->fw_cnt*2));
        if (ext_ver)
        {
            ext_ver->hw_ver = node->ext_ver->hw_ver;
            ext_ver->fw_cnt = node->ext_ver->fw_cnt;
            memcpy(ext_ver->fw_ver, node->ext_ver->fw_ver, (ext_ver->fw_cnt*2));
        }
    }

    plt_mtx_ulck(noded->net->mtx);

    return ext_ver;
}


/**
zwnode_get_desc - get device descriptor
@param[in]	node	node
@param[out]	desc	descriptor
@return		ZW_ERR_XXX
@pre        Caller must lock the nw->mtx before calling this function.
*/

int zwnode_get_desc(zwnode_p node, zwnoded_p desc)
{
    if (!node->net)
    {
        return ZW_ERR_NET_NOT_FOUND;
    }

    if (node->nodeid == 0)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }

    desc->net = node->net;
    desc->nodeid = node->nodeid;
    desc->alive = node->alive;
    printf("================sec_incl_failed====================> %d\n",node->sec_incl_failed);
    desc->sec_incl_failed = node->sec_incl_failed;
    desc->pid = node->pid;
    desc->type = node->vtype;
    desc->vid = node->vid;
    desc->proto_ver = node->proto_ver;
    desc->app_ver = node->app_ver;
    desc->lib_type = node->lib_type;
    desc->dev_id = node->dev_id;
    desc->category = node->category;
    desc->sleep_cap = node->sleep_cap;
    desc->sensor = (node->security & 0x60)? 1 : 0;
    desc->wkup_intv = node->wkup_intv;

    return ZW_ERR_NONE;
}


/**
zwnode_get_ep - get first/default endpoint in device
@param[in]	noded	    The node
@param[out]	epd	        The default endpoint
@return		ZW_ERR_XXX
*/
int zwnode_get_ep(zwnoded_p noded, zwepd_p epd)
{
    zwnode_p node;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {   //Found node
        zwep_get_desc(&node->ep, epd);
        plt_mtx_ulck(noded->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(noded->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;
}


/**
zwnode_rm - Remove a node from the network.
@param[in]	nw	        The network
@param[in]	node_id     The node id to be removed
*/
void zwnode_rm(zwnet_p nw, uint8_t node_id)
{
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, node_id);
    if (node)
    {
        zwnode_ep_rm_all(node);

        //Remove command queues
        zwnode_cmd_q_rm(node);
        zwnode_wait_cmd_q_rm(node);

        //Remove polling requests
        zwpoll_node_rm(nw, node_id);

        //Stop timers
        plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
        node->mul_cmd_tmr_ctx = NULL;
        plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
        node->wait_tmr_ctx = NULL;

        //Free extended version information
        if (node->ext_ver)
        {
            free(node->ext_ver);
            node->ext_ver = NULL;
        }

        if (nw->ctl.nodeid == node_id)
        {   //Cannot delete the first node, just clear the content
            zwnode_ctl_clr(nw);
        }
        else
            zwobj_del(&nw->ctl.obj.next, &node->obj);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnode_ep_rm_all - Remove all endpoints of a node
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_ep_rm_all(zwnode_p node)
{
    zwep_p ep;

    //Remove the second endpoint onwards
    while ((ep = (zwep_p)node->ep.obj.next) != NULL)
    {
        zwep_intf_rm_all(ep);
        zwobj_del(&node->ep.obj.next, &ep->obj);
    }
    //Remove the first ep
    zwep_intf_rm_all(&node->ep);
}


/**
zwif_dat_rm - Free command class specific data attached to the interface
@param[in]	intf	Interface
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwif_dat_rm(zwif_p intf)
{
    //Free command class specific data
    if (intf->data_cnt > 0)
    {
        free(intf->data);
    }

    //Free temporary data
    if (intf->tmp_data)
    {
        if (intf->cls == COMMAND_CLASS_FIRMWARE_UPDATE_MD)
        {
            if_fw_tmp_dat_t *fw_data;

            fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;

            if (fw_data->fw_file)
            {
                fclose(fw_data->fw_file);
            }

            if (fw_data->restrt_tmr_ctx)
            {
                plt_tmr_stop(&fw_data->nw->plt_ctx, fw_data->restrt_tmr_ctx);
            }
        }
        else if (intf->cls == COMMAND_CLASS_ASSOCIATION_GRP_INFO)
        {
            int                 i;
            if_grp_info_dat_t   *grp_data;

            grp_data = (if_grp_info_dat_t *)intf->tmp_data;

            for (i=0; i<grp_data->valid_grp_cnt; i++)
            {
                free(grp_data->grp_info[i]);
            }
        }
        else if (intf->cls == COMMAND_CLASS_SIMPLE_AV_CONTROL)
        {
            zwif_av_cap_t   *av_cap;

            av_cap = (zwif_av_cap_t *)intf->tmp_data;

            if (av_cap->bit_mask_buf)
            {
                free(av_cap->bit_mask_buf);
            }
        }
        free(intf->tmp_data);
    }
}


/**
zwif_cmd_q_extra_handle - handle additional function calls for the queued command
@param[in]	nw          Network
@param[in]	extra       Pointer to the extra handle parameter
@param[in]	cmd_id      Command id
@param[in]	nodeid      The destination node id
@return
*/
static void zwif_cmd_q_extra_handle(zwnet_p nw, void *extra, uint16_t cmd_id, uint8_t nodeid)
{
    if (cmd_id == ZW_CID_GRP_ADD)
    {
        ret_route_t     *ret_route = (ret_route_t *)extra;
        zwnode_p        node;
        int             result;

        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, nodeid);

        if (!node)
        {
            return;
        }

        node->add_grp_rr = *ret_route;
        plt_mtx_ulck(nw->mtx);
        //Delete all return routes first. Assignment of return routes will be done in callback
        result = zw_delete_return_route(&nw->appl_ctx, nodeid, zwif_grp_cb, (void *)((unsigned)nodeid));

        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zw_delete_return_route with error:%d", result);

        }
    }
}


/**
zwif_cmd_q_xtra_rm - remove xtra command queue
@param[in]	nw		    Network
@param[in]	xtra_lst_hd List head
@return
*/
void zwif_cmd_q_xtra_rm(zwnet_p nw, util_lst_t **xtra_lst_hd)
{
    util_lst_t      *xtra_lst_ent;
    cmd_q_xtra_t    *xtra;

    while ((xtra_lst_ent = util_list_get(nw->mtx, xtra_lst_hd)) != NULL)
    {
        xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
        free(xtra->extra);
        free(xtra_lst_ent);
    }
}


/**
zwif_cmd_q_cb - send command from command queue callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwif_cmd_q_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p         nw = (zwnet_p)appl_ctx->data;
    util_lst_t      *xtra_lst_hd = (util_lst_t  *)user_prm;
    util_lst_t      *xtra_lst_ent;
    cmd_q_xtra_t    *xtra;

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        while ((xtra_lst_ent = util_list_get(nw->mtx, &xtra_lst_hd)) != NULL)
        {
            xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
            zwif_cmd_q_extra_handle(nw, xtra->extra, xtra->cmd_id, xtra->node_id);
            free(xtra->extra);
            free(xtra_lst_ent);
        }

    }
    else
    {
        zwif_cmd_q_xtra_rm(nw, &xtra_lst_hd);
    }
}


/**
zwnode_cmd_q_rm - Remove all commands in command queue
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_cmd_q_rm(zwnode_p node)
{

    zwnode_mul_cmd_rm_q(node->net, &node->mul_cmd_q_hd);

    node->mul_cmd_q_cnt = 0;
}


/**
zwif_wait_cmd_q_cb - send command from command queue callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwif_wait_cmd_q_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    util_lst_t      *xtra_lst_hd = (util_lst_t  *)user_prm;
    cmd_q_xtra_t    *xtra;

    //Call back zwnode_wait_tx_cb
    xtra = (cmd_q_xtra_t *)xtra_lst_hd->wr_buf;
    zwnode_wait_tx_cb(appl_ctx, tx_sts, (void *)((unsigned)xtra->node_id));

    //Call back zwif_cmd_q_cb
    zwif_cmd_q_cb(appl_ctx, tx_sts, user_prm);
}


/**
zwnode_wait_cmd_q_rm - Remove all commands in command queue
@param[in]	node	The node
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwnode_wait_cmd_q_rm(zwnode_p node)
{

    zwnode_mul_cmd_rm_q(node->net, &node->wait_cmd_q_hd);

    node->wait_cmd_q_cnt = 0;
}

/**
zwnode_cmd_q_get - Get command queue associated with the specified node
@param[in]	noded	The node
@param[out]	cmd_q	The command queue
@return     The number of entries in cmd_q
@post       Caller must free the cmd_q after this call if return value > 0
*/
uint8_t zwnode_cmd_q_get(zwnoded_p noded, uint16_t **cmd_q)
{
    mul_cmd_q_ent_t  *cmd;
    util_lst_t  *cmd_lst_ent;   //command list entry
    zwnode_p    node;
    uint16_t    *temp_cmd_q;
    uint8_t     mul_cmd_q_cnt;
    uint8_t     ret_value = 0;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        if (node->mul_cmd_q_cnt == 0)
        {
            plt_mtx_ulck(noded->net->mtx);
            return 0;
        }
        //Allocate memory for the command queue
        temp_cmd_q = (uint16_t *)calloc(node->mul_cmd_q_cnt, sizeof(uint16_t));

        if (!temp_cmd_q)
        {
            plt_mtx_ulck(noded->net->mtx);
            return 0;
        }
        *cmd_q = temp_cmd_q;
        mul_cmd_q_cnt = node->mul_cmd_q_cnt;

        cmd_lst_ent = node->mul_cmd_q_hd;

        while (mul_cmd_q_cnt-- > 0)
        {
            if (cmd_lst_ent)
            {
                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;
                //Only copy the command id if this is the first command class command
                if (cmd->cmd_num == 1)
                {
                    *temp_cmd_q++ = cmd->cmd_id;
                    ret_value++;
                }
            }
            cmd_lst_ent = cmd_lst_ent->next;
        }
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_cmd_q_cancel - Cancel all pending commands in command queue
@param[in]	noded	The node
@return		ZW_ERR_XXX
*/
int zwnode_cmd_q_cancel(zwnoded_p noded)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        zwnode_cmd_q_rm(node);
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_cmd_q_ena_set - Control the command queue
@param[in]	noded	   node
@param[in]	enable	   1=enable command queuing; 0=disable command queuing and
                       delete the commands in the queue.
return      ZW_ERR_XXX
*/
int zwnode_cmd_q_ena_set(zwnoded_p noded, uint8_t enable)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        if (node->sleep_cap)
        {
            node->enable_cmd_q = enable;
            if (enable == 0)
            {
                zwnode_cmd_q_rm(node);
            }
            ret_value = ZW_ERR_NONE;
        }
        else
        {   //Node without sleeping capability is not
            //allowed to modify command queuing control
            ret_value = ZW_ERR_UNSUPPORTED;
        }
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;

}


/**
zwnode_cmd_q_ena_get - Get the setting of command queue control
@param[in]	noded	    node
@param[out]	enable	    Current setting of command queue control;
                        1=enable command queuing; 0=disable command queuing
return      ZW_ERR_XXX
*/
int zwnode_cmd_q_ena_get(zwnoded_p noded, uint8_t *enable)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        *enable = (node->enable_cmd_q)? 1 : 0;
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwnode_mul_cmd_rm_q - Remove and free resources in the multi-command queue
@param[in]	net	        Network
@param[in]	cmd_q_hd	Command queue list head
@return
*/
void zwnode_mul_cmd_rm_q(zwnet_p net, util_lst_t  **cmd_q_hd)
{
    util_lst_t  *cmd_lst_ent;
    mul_cmd_q_ent_t *cmd_ent;

    while ((cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd)) != NULL)
    {
        cmd_ent = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;
        if (cmd_ent->extra)
        {
            free(cmd_ent->extra);
        }
        free(cmd_lst_ent);
    }
}


/**
zwnode_mul_cmd_snd_frm - Send multi command frame
@param[in]	node	    Node
@param[in]	cmd_q_hd	Command queue list head
@param[in]	cmd_buf		Buffer that stores multi command encapsulation
@param[in]	buf_len		Length of cmd_buf in bytes
@param[in]	secure_msg	Flag to indicate whether there is a secure message in the cmd_buf
@param[in]	wait_rpt	Flag to indicate whether to wait for report
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@return		ZW_ERR_xxx.  If return value is ZW_ERR_UNEXPECTED, the command queue
            (cmd_q_hd) has been flushed, and shouldn't be used thereafter.
*/
static int zwnode_mul_cmd_snd_frm(zwnoded_p noded, util_lst_t  **cmd_q_hd, uint8_t *cmd_buf, int buf_len,
                                  int secure_msg, int wait_rpt, tx_cmplt_cb_t cb, void *user)
{
    zwifd_t     multi_cmd_ifd;
    zwif_p      intf;
    zwnode_p    node;
    zwnet_p     net = noded->net;

    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, noded->nodeid);

    if (!node)
    {
        plt_mtx_ulck(net->mtx);
        zwnode_mul_cmd_rm_q(net, cmd_q_hd);
        return ZW_ERR_UNEXPECTED;
    }

    intf = zwif_find_cls(node->ep.intf, COMMAND_CLASS_MULTI_CMD);

    if (!intf)
    {
        plt_mtx_ulck(net->mtx);
        zwnode_mul_cmd_rm_q(net, cmd_q_hd);
        return ZW_ERR_UNEXPECTED;
    }

    zwif_get_desc(intf, &multi_cmd_ifd);

    plt_mtx_ulck(net->mtx);

    if (secure_msg)
    {
        //Modify the multi-cmd interface descriptor to secure type
        multi_cmd_ifd.propty |= IF_PROPTY_SECURE;
    }

    return zwif_exec_ex(&multi_cmd_ifd, cmd_buf, buf_len, cb, user,
                        ZWIF_OPT_SKIP_ALL_IMD | ((wait_rpt)? ZWIF_OPT_WAIT_RPT_SET : 0), NULL);

}


/**
zwnode_mul_cmd_send - Send commands in the multi command buffer
@param[in]	node	    Node
@param[in]	cmd_q_hd	Command queue list head
@param[in]	max_sz	    Maximun frame size of the multi-command encapsulation
@return		ZW_ERR_xxx
*/
static int zwnode_mul_cmd_send(zwnoded_p noded, util_lst_t  **cmd_q_hd, uint8_t max_sz)
{
    zwnet_p         net;
    util_lst_t      *cmd_lst_ent;   //command list entry

    net = noded->net;

    //Adjust maximum frame size if it is greater than MAX_ZWAVE_PKT_SIZE
    if (max_sz > MAX_ZWAVE_PKT_SIZE)
    {
        max_sz = MAX_ZWAVE_PKT_SIZE;
    }

    cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd);

    if (cmd_lst_ent)
    {
        int             ret;
        int             opt;
        tx_cmplt_cb_t   exec_cb;
        void            *exec_cb_prm;
        mul_cmd_q_ent_t *cmd;
        util_lst_t      *xtra_lst_ent;
        cmd_q_xtra_t    *xtra;
        util_lst_t      *xtra_lst_hd = NULL;

        cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

        if (*cmd_q_hd == NULL)
        {   //Only 1 command in the command buffer, send the command without encapsulation

            if (cmd->wait_rpt)
            {   //Highest priority
                exec_cb = zwnode_wait_tx_cb;
                exec_cb_prm = (void *)((unsigned)noded->nodeid);
                opt = ZWIF_OPT_WAIT_RPT_SET;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 wait rpt");
            }
            else if (cmd->extra)
            {
                exec_cb = zwif_cmd_q_cb;
                exec_cb_prm = cmd->extra;
                opt = 0;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 xtra");
            }
            else
            {
                exec_cb = exec_cb_prm = NULL;
                opt = 0;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: 1 normal");
            }

            ret = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, exec_cb, exec_cb_prm,
                               ZWIF_OPT_SKIP_ALL_IMD | opt, NULL);

            if (ret < 0)
            {
                if (cmd->extra)
                    zwif_cmd_q_xtra_rm(net, &cmd->extra);
            }
            free(cmd_lst_ent);
        }
        else
        {   //Multiple commands in the queue, start encapsulate all the commands into multi command encapsulation
            uint8_t     mul_cmd_buf[MAX_ZWAVE_PKT_SIZE];// Multi Command Encapsulation buffer
            uint8_t     mul_cmd_wr_idx;                 // Index to the buffer for writing
            uint8_t     secure_msg = 0;                 // Flag to indicate one of the messages is secure
            uint8_t     wait_rpt = 0;                   // Flag to indicate to wait for report

            //Initialize the multi command buffer
            mul_cmd_buf[0] = COMMAND_CLASS_MULTI_CMD;
            mul_cmd_buf[1] = MULTI_CMD_ENCAP;
            mul_cmd_buf[2] = 0;
            mul_cmd_wr_idx = 3;

            while (cmd_lst_ent)
            {
                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                //Check whether there is enough buffer space
                if ((mul_cmd_wr_idx + cmd->dat_len + 1) <= max_sz)//additional 1 byte for command length field
                {
                    //Enough buffer
                    //Check whether the message is secure
                    if (cmd->ifd.propty & IF_PROPTY_SECURE)
                    {
                        secure_msg = 1;
                    }

                    //Check whether to wait for report
                    if (cmd->wait_rpt)
                    {
                        wait_rpt = 1;
                    }

                    //Queue the command
                    mul_cmd_buf[2]++; //increment number of commands
                    mul_cmd_buf[mul_cmd_wr_idx++] = cmd->dat_len;
                    memcpy(mul_cmd_buf + mul_cmd_wr_idx, cmd->dat_buf, cmd->dat_len);

                    //Adjustment
                    mul_cmd_wr_idx += cmd->dat_len;

                    //Handle extra function needed for this command
                    if (cmd->extra)
                    {
                        if ((xtra_lst_ent = util_list_get(net->mtx, &cmd->extra)) != NULL)
                        {
                            xtra = (cmd_q_xtra_t *)xtra_lst_ent->wr_buf;
                            ret = util_list_add(net->mtx, &xtra_lst_hd, (uint8_t *)xtra, sizeof(cmd_q_xtra_t));
                            if (ret < 0)
                            {
                                free(xtra->extra);
                            }
                            free(xtra_lst_ent);
                        }
                    }
                }
                else
                {   //Command queue is full, send the encapsulated message

                    ret = zwnode_mul_cmd_snd_frm(noded, cmd_q_hd, mul_cmd_buf, mul_cmd_wr_idx, secure_msg, 0, NULL, NULL);
                    //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: send when q full");

                    if (ret == ZW_ERR_UNEXPECTED)
                    {
                        zwif_cmd_q_xtra_rm(net, &xtra_lst_hd);
                        return ret;
                    }

                    //Reset the multi command buffer
                    mul_cmd_buf[2] = 0;
                    mul_cmd_wr_idx = 3;

                    //Save the current command into the multi command encapsulation buffer
                    continue;
                }

                free(cmd_lst_ent);
                //Get next command
                cmd_lst_ent = util_list_get(net->mtx, cmd_q_hd);
            }

            //Send the last multi-command frame
            if (xtra_lst_hd && wait_rpt)
            {
                exec_cb = zwif_wait_cmd_q_cb;
                exec_cb_prm = xtra_lst_hd;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: xtra and wait rpt");
            }
            else if (wait_rpt)
            {
                exec_cb = zwnode_wait_tx_cb;
                exec_cb_prm = (void *)((unsigned)noded->nodeid);
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: wait rpt");
            }
            else if (xtra_lst_hd)
            {
                exec_cb = zwif_cmd_q_cb;
                exec_cb_prm = xtra_lst_hd;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: xtra");
            }
            else
            {
                exec_cb = exec_cb_prm = NULL;
                //debug_zwapi_msg(&nw->plt_ctx, "mul_cmd_send: normal");
            }

            ret = zwnode_mul_cmd_snd_frm(noded, cmd_q_hd, mul_cmd_buf, mul_cmd_wr_idx, secure_msg, wait_rpt,
                                         exec_cb, exec_cb_prm);

            if (ret < 0)
            {
                if (xtra_lst_hd)
                    zwif_cmd_q_xtra_rm(net, &xtra_lst_hd);
            }
        }
        return ret;
    }

    return ZW_ERR_NONE;
}


/**
zwnode_mul_cmd_ctl_set - Turn on/off multi command encapsulation
@param[in]	noded	Node
@param[in]	on	    1=turn on multi command encapsulation;  0=turn off and flush
                    the commands in the buffer.
@return		ZW_ERR_xxx
*/
int zwnode_mul_cmd_ctl_set(zwnoded_p noded, uint8_t on)
{
    int         result;
    zwnet_p     net;
    zwnode_p    node;

    net = noded->net;

    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, noded->nodeid);

    if (node)
    {
        if (node->mul_cmd_cap)
        {
            util_lst_t  *cmd_lst_hd;
            uint8_t     max_sz;

            node->mul_cmd_ctl = on;
            cmd_lst_hd = node->mul_cmd_q_hd;
            max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;

            //Reset the command queue for the node
            node->mul_cmd_q_hd = NULL;
            node->mul_cmd_q_cnt = 0;

            if (on == 0)
            {   //Stop timer
                plt_tmr_stop(&net->plt_ctx, node->mul_cmd_tmr_ctx);
                node->mul_cmd_tmr_ctx = NULL;
            }

            plt_mtx_ulck(net->mtx);

            if (on == 0)
            {   //Stop and flush the command buffer
                zwnode_mul_cmd_send(noded, &cmd_lst_hd, max_sz);
            }
            return ZW_ERR_NONE;
        }
        else
        {
            result = ZW_ERR_UNSUPPORTED;
        }
    }
    else
    {
        result = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(net->mtx);
    return result;
}


/**
zwnode_mul_cmd_ctl_get - Get the current state of multi command encapsulation
@param[in]	noded	    node
@param[out]	on	        state: 1=on; 0=off
return      ZW_ERR_XXX
*/
int zwnode_mul_cmd_ctl_get(zwnoded_p noded, uint8_t *on)
{
    zwnode_p    node;
    int         ret_value;

    plt_mtx_lck(noded->net->mtx);

    node = zwnode_find(&noded->net->ctl, noded->nodeid);

    if (node)
    {
        *on = (node->mul_cmd_ctl)? 1 : 0;
        ret_value = ZW_ERR_NONE;
    }
    else
    {
        ret_value = ZW_ERR_NODE_NOT_FOUND;
    }
    plt_mtx_ulck(noded->net->mtx);
    return ret_value;
}


/**
zwep_nameloc_set - Set endpoint name and location string for device regardless whether the endpoint
                   has Node Naming And Location interface.  If it does, the strings will be sent to
                   the interface physically.
@param[in]	    epd	        The endpoint
@param[in,out]	nameloc	    Input:null terminated name & location string
                            Output:null terminated name & location string with proper UTF-8 delineation
@return		ZW_ERR_XXX
*/
int zwep_nameloc_set(zwepd_p epd, struct _zw_nameloc *nameloc)
{
    zwep_p      ep;
    uint8_t     str_len;    //Character string length

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);

    if (ep)
    {   //Found endpoint
        zwif_p  name_loc_if;
        //Store name
        //Check for valid UTF-8 string
        nameloc->name[ZW_LOC_STR_MAX] = '\0';
        str_len = strlen(nameloc->name);
        str_len = plt_utf8_chk((const uint8_t *)nameloc->name, str_len);

        memcpy(ep->name, nameloc->name, str_len);
        ep->name[str_len] = '\0';
        nameloc->name[str_len] = '\0';

        //Store location
        //Check for valid UTF-8 string
        nameloc->loc[ZW_LOC_STR_MAX] = '\0';
        str_len = strlen(nameloc->loc);
        str_len = plt_utf8_chk((const uint8_t *)nameloc->loc, str_len);

        memcpy(ep->loc, nameloc->loc, str_len);
        ep->loc[str_len] = '\0';
        nameloc->loc[str_len] = '\0';

        //Find COMMAND_CLASS_NODE_NAMING interface to send
        //the updated name and location UTF-8 strings
        name_loc_if = zwif_find_cls(ep->intf, COMMAND_CLASS_NODE_NAMING);
        if (name_loc_if)
        {
            zwifd_t ifd;

            zwif_get_desc(name_loc_if, &ifd);

            zwif_nameloc_set(&ifd, nameloc);
        }

        plt_mtx_ulck(epd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;
}



/**
@}
@addtogroup EP Endpoint APIs
@{
*/


/**
zwep_find - find an endpoint based on endpoint id
@param[in]	first_ep	first endpoint in the node
@param[in]	epid	    endpoint id
@return		endpoint if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwep_p zwep_find(zwep_p first_ep, uint8_t epid)
{
    zwep_p    temp_ep;

    temp_ep = first_ep;
    while (temp_ep)
    {
        if (temp_ep->epid == epid)
        {
            return temp_ep;
        }
        temp_ep = (zwep_p)temp_ep->obj.next;
    }

    return  NULL;
}


/**
zwep_get_node - get associated device
@param[in]	epd	    Endpoint
@param[out]	noded	The node descriptor if found
@return		ZW_ERR_XXX
*/
int zwep_get_node(zwepd_p epd, zwnoded_p noded)
{
    zwnode_p node;

    plt_mtx_lck(epd->net->mtx);

    node = zwnode_find(&epd->net->ctl, epd->nodeid);

    if (node)
    {   //Found node
        zwnode_get_desc(node, noded);
        plt_mtx_ulck(epd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_NODE_NOT_FOUND;
}

/**
zwep_get_ep - Get the endpoint of the specified endpoint descriptor
@param[in]	epd	    Endpoint descriptor
@return		The endpoint if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwep_p zwep_get_ep(zwepd_p epd)
{
    zwnode_p node;

    node = zwnode_find(&epd->net->ctl, epd->nodeid);

    if (node)
    {   //Found node
        return zwep_find(&node->ep, epd->epid);
    }
    return  NULL;
}


/**
zwep_get_if - get first interface in endpoint
@param[in]	epd	    endpoint
@param[out]	ifd	interface
@return		ZW_ERR_xxx
@post   Caller must free ifd->data if ifd->data_cnt > 0
*/
int zwep_get_if(zwepd_p epd, zwifd_p ifd)
{
    zwep_p  ep;
    int     result;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);
    if (ep)
    {   //Found endpoint
        if (ep->intf)
        {
            zwif_get_desc_dat(ep->intf, ifd);
            result = ZW_ERR_NONE;
        }
        else
        {
            result = ZW_ERR_INTF_NOT_FOUND;
        }
    }
    else
    {
        result = ZW_ERR_EP_NOT_FOUND;
    }
    plt_mtx_ulck(epd->net->mtx);
    return  result;
}


/**
zwep_get_next - get next endpoint in node
@param[in]	epd	    The initial endpoint for searching the next endpoint
@param[out]	nxt_epd	The next endpoint if found
@return		ZW_ERR_xxx
*/
int zwep_get_next(zwepd_p epd, zwepd_p nxt_epd)
{
    int     result;
    zwep_p  ep;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);
    if (ep && ep->obj.next)
    {   //Found endpoint
        ep = (zwep_p)ep->obj.next;
        result = zwep_get_desc(ep, nxt_epd);
        plt_mtx_ulck(epd->net->mtx);
        return result;
    }

    plt_mtx_ulck(epd->net->mtx);
    return  ZW_ERR_EP_NOT_FOUND;

}


/**
zwep_set_user - set user specific information
@param[in]	epd	    Endpoint
@param[in]	user	User specific information
*/
void zwep_set_user(zwepd_p epd, void *user)
{
    zwep_p  ep;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);
    if (ep)
    {   //Found endpoint
        ep->obj.ctx = user;
    }

    plt_mtx_ulck(epd->net->mtx);

}

/**
zwep_get_user - get user specific information
@param[in]	epd	    Endpoint
@return	user specific information if found; else return NULL
*/
void *zwep_get_user(zwepd_p epd)
{
    zwep_p  ep;
    void    *ret_val;

    plt_mtx_lck(epd->net->mtx);

    ep = zwep_get_ep(epd);

    ret_val = (ep)? ep->obj.ctx : NULL;

    plt_mtx_ulck(epd->net->mtx);

    return ret_val;
}


/**
zwep_get_desc - get endpoint descriptor
@param[in]	ep		endpoint
@param[out]	desc	descriptor
@return		ZW_ERR_XXX
*/
int zwep_get_desc(zwep_p ep, zwepd_p desc)
{
    desc->generic = ep->generic;
    desc->specific = ep->specific;
    desc->epid = ep->epid;
    desc->net = ep->node->net;
    desc->nodeid = ep->node->nodeid;
#ifdef USE_SAFE_VERSION
    strcpy_s(desc->name, ZW_LOC_STR_MAX + 1, ep->name);
    strcpy_s(desc->loc, ZW_LOC_STR_MAX + 1, ep->loc);
#else
    strcpy(desc->name, ep->name);
    strcpy(desc->loc, ep->loc);
#endif
    desc->zwplus_info = ep->zwplus_info;

    return ZW_ERR_NONE;
}


/**
zwep_intf_rm_all - Remove all interfaces of an endpoint
@param[in]	ep	The endpoint
@pre        Caller must lock the nw->mtx before calling this function.
*/
void zwep_intf_rm_all(zwep_p ep)
{
    while (ep->intf)
    {
        zwif_dat_rm(ep->intf);
        zwobj_del((zwobj_p *)(&ep->intf), &ep->intf->obj);
    }
}


/**
zwep_count - count the number of real endpoints in a node
@param[in]	node	Node
@return		The number of real endpoints in a node
@pre        Caller must lock the nw->mtx before calling this function.
*/
uint8_t zwep_count(zwnode_p node)
{
    zwep_p      temp_ep;
    uint8_t     ep_cnt;

    if (!node)
    {
        return  0;
    }

    ep_cnt = 0;
    temp_ep = &node->ep;
    while (temp_ep)
    {
        if (temp_ep->epid != 0)
        {
            ep_cnt++;
        }
        temp_ep =  (zwep_p)temp_ep->obj.next;
    }

    return  ep_cnt;
}


/**
@}
@addtogroup If Interface APIs
@{
*/

const char *zwif_tx_cmplt_sts[] =
{   "ok",
    "no ACK before timeout",
    "failed",
    "routing not idle",
    "no route",
    "no callback frame before timeout"
};


/**
zwif_alloc - Allocate memory for the interface and initialize it
@param[in]	cls	        Command class
@param[in]	ver	        Command class version
@param[in]	propty      Command class properties
@param[in]	intf_buf	Buffer that contains the reports command of the
                        specified command class
@param[in]	intf_buf_sz	The size of intf_buf
@return		interface; else return NULL
*/
static zwif_p zwif_alloc(uint16_t cls, uint8_t ver, uint8_t propty,
                         const uint8_t *intf_buf, uint8_t intf_buf_sz)

{
    int32_t i;
    zwif_p  intf;

    intf = (zwif_p)calloc(1, sizeof(zwif_t)
                          + (sizeof(if_rpt_t) * intf_buf_sz));
    if (!intf)
    {
        return NULL;
    }

    intf->cls = cls;
    intf->ver = ver;
    intf->propty = propty;
    intf->rpt_num = intf_buf_sz;

    for (i=0; i < intf_buf_sz; i++)
    {
        intf->rpt[i].rpt_cmd = intf_buf[i];
    }

    return intf;
}


/**
zwif_create - Create interface based on command class
@param[in]	cls	        class
@param[in]	ver	        class version
@param[in]	propty      command class properties
@return		interface; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_create(uint16_t cls, uint8_t ver, uint8_t propty)
{
    switch (cls)
    {
        case COMMAND_CLASS_BASIC:
            {
                static const uint8_t intf_settings[] =
                {
                    BASIC_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_SWITCH_MULTILEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_MULTILEVEL_REPORT,
                    SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3   //version 3
                };
                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 3)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SWITCH_BINARY:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_BINARY_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SWITCH_ALL:
            {
                static const uint8_t intf_settings[] =
                {
                    SWITCH_ALL_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            {
                static const uint8_t intf_settings[] =
                {
                    MANUFACTURER_SPECIFIC_REPORT,
                    DEVICE_SPECIFIC_REPORT_V2   //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 2)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_VERSION:
            {
                static const uint8_t intf_settings[] =
                {
                    VERSION_REPORT,
                    VERSION_COMMAND_CLASS_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_POWERLEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    POWERLEVEL_REPORT,
                    POWERLEVEL_TEST_NODE_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CONTROLLER_REPLICATION:
            {
                return zwif_alloc(cls, ver, propty, NULL, 0);
            }
            break;

        case COMMAND_CLASS_NODE_NAMING:
            {
                static const uint8_t intf_settings[] =
                {
                    NODE_NAMING_NODE_NAME_REPORT,
                    NODE_NAMING_NODE_LOCATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SENSOR_BINARY:
            {
                static const uint8_t intf_settings[] =
                {
                    SENSOR_BINARY_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SENSOR_MULTILEVEL:
            {
                static const uint8_t intf_settings[] =
                {
                    SENSOR_MULTILEVEL_REPORT,
                    SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5, //version 5
                    SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 5)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION:
            {
                static const uint8_t intf_settings[] =
                {
                    ASSOCIATION_REPORT,
                    ASSOCIATION_GROUPINGS_REPORT,
                    ASSOCIATION_SPECIFIC_GROUP_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 2 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
            {
                static const uint8_t intf_settings[] =
                {
                    COMMAND_RECORDS_SUPPORTED_REPORT,
                    COMMAND_CONFIGURATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2:
            {
                static const uint8_t intf_settings[] =
                {
                    MULTI_CHANNEL_ASSOCIATION_REPORT_V2,
                    MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;


        case COMMAND_CLASS_MULTI_CHANNEL_V2:
            {
                if (ver == 1)
                {   //multi-instance command class
                    static const uint8_t intf_settings[] =
                    {
                        MULTI_INSTANCE_REPORT
                    };
                    return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));

                }

                if (ver >= 2)
                {   //multi-channel command class
                    static const uint8_t intf_settings[] =
                    {
                        MULTI_INSTANCE_REPORT,
                        MULTI_CHANNEL_END_POINT_REPORT_V2,
                        MULTI_CHANNEL_CAPABILITY_REPORT_V2,
                        MULTI_CHANNEL_END_POINT_FIND_REPORT_V2
                    };

                    return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
                }
            }
            break;

        case COMMAND_CLASS_WAKE_UP:
            {
                static const uint8_t intf_settings[] =
                {
                    WAKE_UP_INTERVAL_REPORT,
                    WAKE_UP_NOTIFICATION,
                    WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 2 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_REPORT,
                    METER_SUPPORTED_REPORT_V2  //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER_PULSE:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_PULSE_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CONFIGURATION:
            {
                static const uint8_t intf_settings[] =
                {
                    CONFIGURATION_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_METER_TBL_MONITOR:
            {
                static const uint8_t intf_settings[] =
                {
                    METER_TBL_TABLE_POINT_ADM_NO_REPORT,
                    METER_TBL_TABLE_ID_REPORT,
                    METER_TBL_REPORT,
                    METER_TBL_STATUS_SUPPORTED_REPORT,
                    METER_TBL_STATUS_REPORT,
                    METER_TBL_CURRENT_DATA_REPORT,
                    METER_TBL_HISTORICAL_DATA_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SIMPLE_AV_CONTROL:
            {
                static const uint8_t intf_settings[] =
                {
                    SIMPLE_AV_CONTROL_REPORT,
                    SIMPLE_AV_CONTROL_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_SECURITY:
            {
                static const uint8_t intf_settings[] =
                {
                    SECURITY_NONCE_REPORT,
                    SECURITY_SCHEME_REPORT,
                    NETWORK_KEY_VERIFY,
                    SECURITY_COMMANDS_SUPPORTED_REPORT

                };
                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK:
            {
                static const uint8_t intf_settings[] =
                {
                    DOOR_LOCK_OPERATION_REPORT,
                    DOOR_LOCK_CONFIGURATION_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_USER_CODE:
            {
                static const uint8_t intf_settings[] =
                {
                    USER_CODE_REPORT,
                    USERS_NUMBER_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ALARM:
            {
                static const uint8_t intf_settings[] =
                {
                    ALARM_REPORT,
                    ALARM_TYPE_SUPPORTED_REPORT_V2,
                    EVENT_SUPPORTED_REPORT_V3

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver < 3)? ver : 3);
            }
            break;

        case COMMAND_CLASS_BATTERY:
            {
                static const uint8_t intf_settings[] =
                {
                    BATTERY_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_FAN_MODE_REPORT,
                    THERMOSTAT_FAN_MODE_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_FAN_STATE_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_MODE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_MODE_REPORT,
                    THERMOSTAT_MODE_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_OPERATING_STATE_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETBACK:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_SETBACK_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETPOINT:
            {
                static const uint8_t intf_settings[] =
                {
                    THERMOSTAT_SETPOINT_REPORT,
                    THERMOSTAT_SETPOINT_SUPPORTED_REPORT

                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_CLOCK:
            {
                static const uint8_t intf_settings[] =
                {
                    CLOCK_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
            {
                static const uint8_t intf_settings[] =
                {
                    SCHEDULE_REPORT,
                    SCHEDULE_CHANGED_REPORT,
                    SCHEDULE_OVERRIDE_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_PROTECTION:
            {
                static const uint8_t intf_settings[] =
                {
                    PROTECTION_REPORT,
                    PROTECTION_SUPPORTED_REPORT_V2, //version 2
                    PROTECTION_EC_REPORT_V2,        //version 2
                    PROTECTION_TIMEOUT_REPORT_V2    //version 2

                };

                return zwif_alloc(cls, ver, propty, intf_settings, (ver == 1)? 1 : sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_APPLICATION_STATUS:
            {
                static const uint8_t intf_settings[] =
                {
                    APPLICATION_BUSY,
                    APPLICATION_REJECTED_REQUEST
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_INDICATOR:
            {
                static const uint8_t intf_settings[] =
                {
                    INDICATOR_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;

        case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            {
                static const uint8_t intf_settings[] =
                {
                    FIRMWARE_MD_REPORT,
                    FIRMWARE_UPDATE_MD_REQUEST_REPORT,
                    FIRMWARE_UPDATE_MD_STATUS_REPORT,
                    //FIRMWARE_UPDATE_MD_REPORT,
                    FIRMWARE_UPDATE_MD_GET  //Handle firmware data get
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ZWAVEPLUS_INFO:
            {
                static const uint8_t intf_settings[] =
                {
                    ZWAVEPLUS_INFO_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_DEVICE_RESET_LOCALLY:
            {
                static const uint8_t intf_settings[] =
                {
                    DEVICE_RESET_LOCALLY_NOTIFICATION
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                static const uint8_t intf_settings[] =
                {
                    ASSOCIATION_GROUP_NAME_REPORT,
                    ASSOCIATION_GROUP_INFO_REPORT,
                    ASSOCIATION_GROUP_COMMAND_LIST_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK_LOGGING:
            {
                static const uint8_t intf_settings[] =
                {
                    DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT,
                    RECORD_REPORT
                };

                return zwif_alloc(cls, ver, propty, intf_settings, sizeof(intf_settings));
            }
            break;

#ifdef  TEST_EXT_CMD_CLASS
        case COMMAND_CLASS_EXT_TEST:
            {
                static const uint8_t intf_settings[] =
                {
                    BASIC_REPORT
                };
                return zwif_alloc(cls, ver, propty, intf_settings, 1);
            }
            break;
#endif
        default:
            return zwif_alloc(cls, ver, propty, NULL, 0);


    }
    return  NULL;
}


/**
zwif_find_cls - find an interface based on its command class
@param[in]	first_intf	first interface in an endpoint
@param[in]	cls	        the command class to search
@return		interface if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_find_cls(zwif_p first_intf, uint16_t cls)
{
    zwif_p    temp_intf;

    if (!first_intf)
    {
        return  NULL;
    }

    temp_intf = first_intf;
    while (temp_intf)
    {
        if (temp_intf->cls == cls)
        {
            return temp_intf;
        }
        temp_intf =  (zwif_p)temp_intf->obj.next;
    }

    return  NULL;
}


/**
zwif_count - count the number of unsecure interfaces in an endpoint
@param[in]	first_intf	first interface in an endpoint
@return		The number of interfaces in an endpoint
@pre        Caller must lock the nw->mtx before calling this function.
*/
uint8_t zwif_count(zwif_p first_intf)
{
    zwif_p      temp_intf;
    uint8_t     unsecure_cnt;

    if (!first_intf)
    {
        return  0;
    }

    unsecure_cnt = 0;
    temp_intf = first_intf;
    while (temp_intf)
    {
        if (temp_intf->propty & IF_PROPTY_UNSECURE)
        {
            unsecure_cnt++;
        }
        temp_intf =  (zwif_p)temp_intf->obj.next;
    }

    return  unsecure_cnt;
}



/**
zwif_get_if - Get the interface of the specified interface descriptor
@param[in]	ifd	    Interface descriptor
@return		The interface if found; else return NULL
@pre        Caller must lock the nw->mtx before calling this function.
*/
zwif_p zwif_get_if(zwifd_p ifd)
{
    zwnode_p node;
    zwep_p  ep;

    node = zwnode_find(&ifd->net->ctl, ifd->nodeid);

    if (node)
    {   //Found node
        ep = zwep_find(&node->ep, ifd->epid);
        if (ep)
        {   //Found endpoint
            return zwif_find_cls(ep->intf, ifd->cls);
        }
    }
    return  NULL;
}


/**
zwif_get_desc - get interface descriptor without the data portion
@param[in]	intf	interface
@param[out]	desc	descriptor
*/
void zwif_get_desc(zwif_p intf, zwifd_p desc)
{
    desc->cls = intf->cls;
    desc->ver = intf->ver;
    desc->propty = intf->propty;
    desc->epid = intf->ep->epid;
    desc->nodeid = intf->ep->node->nodeid;
    desc->net = intf->ep->node->net;
    //Reset the data portion of the descriptor
    desc->data_cnt = 0;
    desc->data = NULL;
}


/**
zwif_get_desc_dat - get interface descriptor with the data portion
@param[in]	intf	interface
@param[out]	desc	descriptor
@post   Caller must free desc->data if desc->data_cnt > 0
*/
static void zwif_get_desc_dat(zwif_p intf, zwifd_p desc)
{
    zwif_get_desc(intf, desc);
    //Copy command class specific data portion
    if ((intf->data_cnt > 0) && (intf->data_item_sz > 0))
    {
        desc->data = calloc(intf->data_cnt, intf->data_item_sz);
        if (desc->data)
        {
            desc->data_cnt = intf->data_cnt;
            memcpy(desc->data, intf->data, (intf->data_cnt * intf->data_item_sz));
        }
    }
}


/**
zwif_ifd_get - get the interface descriptor of the specified command class
@param[in]	ifd	        interface which has the same endpoint as the cmd_cls_ifd
@param[in]	cmd_cls	    specified command class
@param[out]	cmd_cls_ifd	interface descriptor of the specified command class
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_ifd_get(zwifd_p ifd, uint16_t cmd_cls, zwifd_p cmd_cls_ifd)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    int         result;

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        intf = zwif_find_cls(intf->ep->intf, cmd_cls);

        if (intf)
        {
            zwif_get_desc(intf, cmd_cls_ifd);
            result = ZW_ERR_NONE;
        }
        else
        {
            result = ZW_ERR_CLASS_NOT_FOUND;
        }
    }
    else
    {
        result = ZW_ERR_INTF_NOT_FOUND;
    }

    plt_mtx_ulck(nw->mtx);
    return result;
}


/**
zwif_get_next - get next interface in containing endpoint
@param[in]	ifd	    The initial interface for searching the next interface
@param[out]	nxt_intf_desc	The next interface if found
@return		ZW_ERR_xxx
@post   Caller must free nxt_ifd->data if nxt_ifd->data_cnt > 0
*/
int zwif_get_next(zwifd_p ifd, zwifd_p nxt_ifd)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf && intf->obj.next)
    {   //Found interface
        intf = (zwif_p)intf->obj.next;
        zwif_get_desc_dat(intf, nxt_ifd);
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return  ZW_ERR_INTF_NOT_FOUND;

}


/**
zwif_get_ep - get endpoint containing this interface
@param[in]	ifd		interface
@param[out]	epd		endpoint
@return		ZW_ERR_xxx
*/
int zwif_get_ep(zwifd_p ifd, zwepd_p epd)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf)
    {   //Found interface
        zwep_get_desc(intf->ep, epd);
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return  ZW_ERR_INTF_NOT_FOUND;
}


/**
zwif_set_user - Set user specific information
@param[in]	ifd	    Interface
@param[in]	user	User specific information
*/
void zwif_set_user(zwifd_p ifd, void *user)
{
    zwif_p  intf;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);
    if (intf)
    {   //Found interface
        intf->obj.ctx = user;
    }

    plt_mtx_ulck(ifd->net->mtx);
}


/**
zwif_get_user - get user specific information
@param[in]	ifd	interface
@return	user specific information if found; else return NULL
*/
void *zwif_get_user(zwifd_p ifd)
{
    zwif_p  intf;
    void *ret_val;

    plt_mtx_lck(ifd->net->mtx);

    intf = zwif_get_if(ifd);

    ret_val = (intf)? intf->obj.ctx : NULL;

    plt_mtx_ulck(ifd->net->mtx);

    return ret_val;
}


/**
zwif_set_report - Setup a generic report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
@param[in]	rpt 	    Report command
@return      ZW_ERR_XXX
*/
int zwif_set_report(zwifd_p ifd, void *rpt_cb, uint8_t rpt)
{
    zwif_p  intf;
    int i;

    plt_mtx_lck(ifd->net->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {   //Find the report
        for (i=0; i < intf->rpt_num; i++)
        {
            if (intf->rpt[i].rpt_cmd == rpt)
            {
                //Setup report callback
                printf("linhui, Setup report callback\n");
                intf->rpt[i].rpt_cb = rpt_cb;
                plt_mtx_ulck(ifd->net->mtx);
                return ZW_ERR_NONE;
            }
        }
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_RPT_NOT_FOUND;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return ZW_ERR_INTF_NOT_FOUND;
}


/**
zwif_cmd_q_sec_cb - send command in the command queue using encryption transmission callback
@param[in]	nw		network
@param[in]  nodeid	destination node id of the transmission
@param[in]  status	0= transmission ok; else negative error number
@param[in]	user	user context
*/
void zwif_cmd_q_sec_cb(zwnet_p nw, uint8_t nodeid, int status, void *user)
{
    //Callback
    zwif_tx_sts_cb(&nw->appl_ctx,
                   (status == 0)? TRANSMIT_COMPLETE_OK : TRANSMIT_COMPLETE_FAIL,
                   user);

    if (status != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwif_cmd_q_sec_cb: node id:%u, status:%d",
                        nodeid, status);
    }

}


/**
zwif_cls_cmd_get - parse a command buffer to get its class and command
@param[in]	cmd_buf		Command buffer
@param[in]  len	        The length of command buffer in bytes
@param[out] cmd_cls	    The command class
@param[out]	cmd	        The command
@return     1 if the command buffer is successfully parsed; otherwise return zero
*/
int zwif_cls_cmd_get(uint8_t* cmd_buf, uint8_t len, uint16_t *cmd_cls, uint8_t *cmd)
{
    uint16_t    cls;
    uint8_t     command;

    if (len < 2)
    {
        return 0;
    }

    cls = cmd_buf[0];

    if (cmd_buf[0] >= 0xF1)
    {   //Extended command class
        if (len < 3)
        {
            return 0;
        }

        cls = (cls << 8) | cmd_buf[1];
        command = cmd_buf[2];

    }
    else
    {
        command = cmd_buf[1];
    }

    *cmd_cls = cls;
    *cmd = command;

    return 1;

}


/**
zwnode_get_rpt - Check the input command to find the corresponding REPORT
@param[in]	cmd		    Command
@param[in]  len	        The length of command in bytes
@param[out] cmd_cls	    The command class of the report
@param[out]	rpt	        The report
@return     1 if the corresponding report is found; otherwise return 0
*/
int zwnode_get_rpt(uint8_t* cmd, uint8_t len, uint16_t *cmd_cls, uint8_t *rpt)
{
    int         i;
    int         total_ent;  //total entries in the table
    uint16_t    cls;
    uint8_t     get_cmd;


    if (!zwif_cls_cmd_get(cmd, len, &cls, &get_cmd))
    {
        return 0;
    }

    total_ent = sizeof(cmd_get_resp_tbl)/ sizeof(cmd_get_resp_t);

    for (i=0; i<total_ent; i++)
    {
        if ((cmd_get_resp_tbl[i].cls == cls) && (cmd_get_resp_tbl[i].get_cmd == get_cmd))
        {   //Found
            *cmd_cls = cls;
            *rpt = cmd_get_resp_tbl[i].rpt;
            return 1;
        }
    }
    return 0;
}


/**
zwnode_wait_rpt_chk - Check whether the report is the one which the node is waiting for
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@param[in]	cmd_len	The length of cmd_buf
@return
*/
void zwnode_wait_rpt_chk(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len)
{
    zwnode_p    node = intf->ep->node;
    zwnet_p     nw = node->net;
    uint16_t    cls;
    uint8_t     rpt;

    //Get command class and the report command
    if (!zwif_cls_cmd_get(cmd_buf, cmd_len, &cls, &rpt))
    {
        return;
    }

    plt_mtx_lck(nw->mtx);

    if (node->wait_rpt_flg)
    {
        if ((node->wait_cmd == cls)
            && (node->wait_rpt == rpt))
        {   //The waited report has arrived

            //Stop timer
            plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
            node->wait_tmr_ctx = NULL;

            //Clear the wait report flag
            node->wait_rpt_flg = 0;

            //Submit request to wait thread for execution of queued commands
            util_list_add(nw->mtx, &nw->wait_q_req_hd,
                          &node->nodeid, sizeof(uint8_t));

            //Send the command in command queue
            plt_sem_post(nw->wait_q_sem);

        }
    }

    plt_mtx_ulck(nw->mtx);

}


/**
zwnode_wait_rpt_tmout_cb - Wait report timeout callback
@param[in] data     Pointer to node
@return
*/
static void    zwnode_wait_rpt_tmout_cb(void *data)
{
    zwnode_p    node = (zwnode_p)data;
    zwnet_p     nw = node->net;

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_rpt_tmout_cb");
    plt_mtx_lck(nw->mtx);

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
    node->wait_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Clear the wait report flag
    node->wait_rpt_flg = 0;

    //Submit request to wait thread for execution of queued commands
    util_list_add(nw->mtx, &nw->wait_q_req_hd,
                  &node->nodeid, sizeof(uint8_t));
    plt_mtx_ulck(nw->mtx);
    plt_sem_post(nw->wait_q_sem);

}


/**
zwnode_wait_tx_cb - send command callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwnode_wait_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    unsigned    node_id = (unsigned)user_prm;
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);

    node = zwnode_find(&nw->ctl, node_id);

    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        return;
    }

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_tx_cb: tx_sts:%u", tx_sts);

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        //Restart wait report timer
        plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
        node->wait_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNODE_WAIT_RPT_TMOUT, zwnode_wait_rpt_tmout_cb, node);

    }
    else
    {
        zwnode_wait_rpt_tmout_cb(node);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnode_wait_q_thrd - Thread to serve requests to execute queued commands for nodes that require wakeup beam
@param[in]	data     Network
@return
*/
void zwnode_wait_q_thrd(void   *data)
{
    int             result;
    int             wait_rpt;
    int             retry_cnt;
    int             opt;
    zwnet_p         nw = (zwnet_p)data;
    zwnode_p        node;
    util_lst_t      *req_lst;
    util_lst_t      *cmd_lst_ent;   //command list entry
    mul_cmd_q_ent_t *cmd;
    tx_cmplt_cb_t   exec_cb;
    void            *exec_cb_prm;
    uint16_t        cmd_cls;
    uint8_t         rpt;
    uint8_t         node_id;

    nw->wait_q_thrd_sts = 1;
    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->wait_q_sem);

        //Check whether to exit the thread
        if (nw->wait_q_thrd_run == 0)
        {
            nw->wait_q_thrd_sts = 0;
            return;
        }

        while ((req_lst = util_list_get(nw->mtx, &nw->wait_q_req_hd)) != NULL)
        {
            //Get the node id of the request
            node_id = req_lst->wr_buf[0];

            while (1)
            {   //Send commands for the node
                plt_mtx_lck(nw->mtx);

                node = zwnode_find(&nw->ctl, node_id);

                if (!node || node->wait_rpt_flg)
                {
                    plt_mtx_ulck(nw->mtx);
                    break;
                }

                //No report pending, send commands
                cmd_lst_ent = util_list_get(nw->mtx, &node->wait_cmd_q_hd);

                if (!cmd_lst_ent)
                {
                    plt_mtx_ulck(nw->mtx);
                    break;
                }

                node->wait_cmd_q_cnt--;

                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                //Unlock network mutex before sending the command to prevent deadlock
                plt_mtx_ulck(nw->mtx);

                //Check whether the command is GET command with REPORT as response
                wait_rpt = zwnode_get_rpt(cmd->dat_buf, cmd->dat_len, &cmd_cls, &rpt);

                retry_cnt = 2;

                while (retry_cnt-- > 0)
                {
                    if (cmd->extra)
                    {   //Highest priority
                        exec_cb = zwif_cmd_q_cb;
                        exec_cb_prm = cmd->extra;
                        opt = 0;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: extra");

                    }
                    else if (wait_rpt)
                    {   //Don't need to fill up exec_cb and exec_cb_prm as they will be overwritten
                        exec_cb = exec_cb_prm = NULL;

                        opt = ZWIF_OPT_WAIT_REPORT;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: wait rpt");
                    }
                    else
                    {
                        exec_cb = exec_cb_prm = NULL;
                        opt = 0;
                        //debug_zwapi_msg(&nw->plt_ctx, "wait q thd: normal");
                    }

                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len,
                                          exec_cb, exec_cb_prm,
                                          ZWIF_OPT_SKIP_WK_BEAM | ZWIF_OPT_CMD_ENTRY | opt, cmd);

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

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_q_thrd: send data with error:%d", result);
                    if (cmd->extra)
                    {
                        zwif_cmd_q_xtra_rm(nw, &cmd->extra);
                    }
                    free(cmd_lst_ent);

                    //Continue sending the next command for this node
                    continue;
                }

                free(cmd_lst_ent);//From this point onwards, the cmd variable is invalid

                if (wait_rpt)
                {
                    //Stop sending command for this node
                    break;
                }
            }

            free(req_lst);
        }
    }
}


/**
zwnode_cmd_q_thrd - Thread to serve requests to execute queued commands for sleeping nodes
@param[in]	data	Network
@return
*/
void zwnode_cmd_q_thrd(void   *data)
{
    zwnet_p         nw = (zwnet_p)data;
    zwnode_p        node;
    util_lst_t      *req_lst;
    int             result;
    zwnoded_t       noded;
#ifdef SEND_NO_MORE_INFO_AFTER_DATA
    uint8_t         wakeup_resp[] =
    {
        COMMAND_CLASS_WAKE_UP,
        WAKE_UP_NO_MORE_INFORMATION
    };
#endif

    nw->cmd_q_thrd_sts = 1;

    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->cmd_q_sem);

        //Check whether to exit the thread
        if (nw->cmd_q_thrd_run == 0)
        {
            nw->cmd_q_thrd_sts = 0;
            return;
        }

        req_lst = util_list_get(nw->cmd_q_mtx, &nw->cmd_q_req_hd);

        if (req_lst)
        {
            util_lst_t      *cmd_lst_ent;   //command list entry
            cmd_q_req_t     *cmd_q_req;
            mul_cmd_q_ent_t *cmd;

            cmd_q_req = (cmd_q_req_t *)req_lst->wr_buf;

            plt_mtx_lck(nw->mtx);

            node = zwnode_find(&nw->ctl, cmd_q_req->node_id);

            if (!node)
            {   //Node no longer exists, remove its command queue
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_rm_q(nw, &cmd_q_req->req_hd);
                free(req_lst);
                continue;
            }

            if (zwnode_get_desc(node, &noded) < 0)
            {
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_rm_q(nw, &cmd_q_req->req_hd);
                free(req_lst);
                continue;
            }

#ifdef SEND_NO_MORE_INFO_AFTER_DATA
            //Inject WAKE_UP_NO_MORE_INFORMATION command at the end of the command list
            if (!cmd_q_req->wkup_brdcst)
            {
                zwif_p      intf;

                intf = zwif_find_cls(node->ep.intf, COMMAND_CLASS_WAKE_UP);

                if (intf)
                {
                    cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + 2);
                    if (cmd)
                    {
                        zwif_get_desc(intf, &cmd->ifd);
                        cmd->cmd_id = ZW_CID_WKUP_NO_INFO;
                        cmd->cmd_num = 1;
                        memcpy(cmd->dat_buf, wakeup_resp, 2);
                        cmd->dat_len = 2;
                        util_list_add(nw->mtx, &cmd_q_req->req_hd,
                                      (uint8_t *)cmd, sizeof(mul_cmd_q_ent_t) + 2);
                        free(cmd);
                    }
                }
            }
#endif
            //Check whether the node supports multi-command encapsulation
            if (node->mul_cmd_ctl)
            {
                uint8_t     max_sz;

                max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;
                plt_mtx_ulck(nw->mtx);
                zwnode_mul_cmd_send(&noded, &cmd_q_req->req_hd, max_sz);
                free(req_lst);
                continue;
            }

            plt_mtx_ulck(nw->mtx);

            while ((cmd_lst_ent = util_list_get(nw->mtx, &cmd_q_req->req_hd)) != NULL)
            {

                cmd = (mul_cmd_q_ent_t *)cmd_lst_ent->wr_buf;

                if (cmd->extra)
                {
                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, zwif_cmd_q_cb, cmd->extra,
                                          ZWIF_OPT_SKIP_ALL_IMD, NULL);
                    if (result < 0)
                    {
                        zwif_cmd_q_xtra_rm(nw, &cmd->extra);
                    }
                }
                else
                {
                    result = zwif_exec_ex(&cmd->ifd, cmd->dat_buf, cmd->dat_len, NULL, NULL,
                                          ZWIF_OPT_SKIP_ALL_IMD, NULL);

                }
                free(cmd_lst_ent);

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnode_cmd_q_thrd: send data with error:%d", result);
                }
            }
            free(req_lst);
        }
    }
}


/**
zwif_cmd_dequeue - Dequeue the first command in the command list for sleeping node
@param[in]	intf	Interface
@param[in]	rx_sts  Receive status
@return             0 if no command in the command list; else return 1
*/
#ifdef SEND_NO_MORE_INFO_AFTER_DATA
static int zwif_cmd_dequeue(zwif_p intf, uint8_t rx_sts)
#else
static int zwif_cmd_dequeue(zwif_p intf)
#endif
{
    zwnode_p    node;
    int         result;

    node = intf->ep->node;
    plt_mtx_lck(node->net->mtx);

    if (node->mul_cmd_q_hd)
    {
        cmd_q_req_t     cmd_q_req;
#ifdef SEND_NO_MORE_INFO_AFTER_DATA
        cmd_q_req.wkup_brdcst = ((rx_sts & RECEIVE_STATUS_TYPE_MASK) == RECEIVE_STATUS_TYPE_BROAD)? 1 : 0;
#endif
        cmd_q_req.node_id = node->nodeid;
        cmd_q_req.req_hd = node->mul_cmd_q_hd;
        //Submit request to thread for execution of queued commands
        result = util_list_add(node->net->cmd_q_mtx, &node->net->cmd_q_req_hd,
                               (uint8_t *)&cmd_q_req, sizeof(cmd_q_req_t));
        if (result != 0)
        {   //Error, try to send on next wake up notification
            plt_mtx_ulck(node->net->mtx);
            return 0;
        }
        plt_sem_post(node->net->cmd_q_sem);
        //Remove the queue from the node
        node->mul_cmd_q_hd = NULL;
        node->mul_cmd_q_cnt = 0;
        plt_mtx_ulck(node->net->mtx);
        return 1;
    }
    plt_mtx_ulck(node->net->mtx);
    return 0;

}


/**
zwif_fw_restrt_tout_cb - Firmware update target restart timer callback
@param[in] data     Firmware update interface data
@return
*/
static void zwif_fw_restrt_tout_cb(void *data)
{
    if_fw_tmp_dat_t     *fw_data = (if_fw_tmp_dat_t *)data;
    zwnet_p             nw = fw_data->nw;
    zwnet_exec_req_t    req = {0};

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, fw_data->restrt_tmr_ctx);
    fw_data->restrt_tmr_ctx = NULL;

    req.node_id = fw_data->node.nodeid;
    req.action = EXEC_ACT_PROBE_RESTART_NODE;
    plt_mtx_ulck(nw->mtx);

    util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                  (uint8_t *)&req, sizeof(zwnet_exec_req_t));
    plt_sem_post(nw->nw_exec_sem);

}


/**
zwif_rep_hdlr - Interface report handler
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@param[in]	cmd_len	The length of cmd_buf
@param[in]	rx_sts  Receive status
*/
void zwif_rep_hdlr(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t rx_sts)
{
    printf("linhui, zwif_rep_hdlr \n");
    int         i;
    zwifd_t     ifd;
    void        *report_cb;
    uint16_t    cls;
    uint8_t     rpt_cmd;

    //Check whether there is a callback function for this interface
    if (!zwif_cls_cmd_get(cmd_buf, cmd_len, &cls, &rpt_cmd))
    {
        printf("linhui zwif_cls_cmd_get not for this interface, return\n");
        return;
    }

    report_cb = NULL;

    for (i=0; i<intf->rpt_num; i++)
    {
        if (intf->rpt[i].rpt_cmd == rpt_cmd)
        {
            report_cb = intf->rpt[i].rpt_cb;
            break;
        }
    }

    printf("linhui cmd_buf[0]= %02x,cmd_buf[1]= %02x, rpt_cmd=%02x\n",cmd_buf[0],cmd_buf[1], rpt_cmd);

    if (!report_cb)
    {   //Wake up notification and security command class do not require a user-defined callback,
        //it uses the system callback function.
        if (!((cmd_buf[0] == COMMAND_CLASS_WAKE_UP && cmd_buf[1] == WAKE_UP_NOTIFICATION)
              || (cmd_buf[0] == COMMAND_CLASS_SECURITY)
              || (cmd_buf[0] == COMMAND_CLASS_DEVICE_RESET_LOCALLY)))
        {
            printf("linhui wake up notification return\n");
            return;
        }
    }

    switch (cls)
    {
        case COMMAND_CLASS_SECURITY:
            {
                if (cmd_buf[1] == SECURITY_COMMANDS_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwif_sec_cmd_cls_t  *cmd_cls;
                        uint8_t             *tmp_buf;
                        uint16_t            cmd_cls_len;   //command classes length of this report

                        cmd_cls = &intf->ep->node->net->sec_cmd_cls;

                        cmd_cls_len = cmd_len - 3;

                        if (cmd_cls_len)
                        {
                            //Store the supported command classes
                            if (cmd_cls->cmd_cls_buf)
                            {
                                //Re-allocate memory
                                tmp_buf = realloc(cmd_cls->cmd_cls_buf,
                                                  cmd_cls->buf_len + cmd_cls_len);
                            }
                            else
                            {
                                //Allocate memory
                                tmp_buf = malloc(cmd_cls_len);
                                cmd_cls->buf_len = 0;
                            }

                            if (tmp_buf)
                            {
                                cmd_cls->cmd_cls_buf = tmp_buf;
                                memcpy(cmd_cls->cmd_cls_buf + cmd_cls->buf_len,
                                       cmd_buf + 3, cmd_cls_len);

                                cmd_cls->buf_len += cmd_cls_len;

                            }
                        }

                        //Check whether this is the last report
                        if (cmd_buf[2] == 0)
                        {
                            //Last report
                            zwrep_sup_sec_fn    rpt_cb = (zwrep_sup_sec_fn)report_cb;
                            uint16_t            *cmd_cls_sec;
                            uint16_t            *cmd_class;
                            uint8_t             cmd_cnt_sec;
                            uint8_t             cmd_cnt;

                            if(util_cmd_cls_parse(cmd_cls->cmd_cls_buf, cmd_cls->buf_len, &cmd_class, &cmd_cnt,
                                                  &cmd_cls_sec, &cmd_cnt_sec))
                            {
                                //Callback the registered function
                                printf("report callback called here\n");
                                rpt_cb(intf, cmd_class, cmd_cnt);

                                free(cmd_class);
                                free(cmd_cls_sec);
                            }
                            else if (!cmd_cls->cmd_cls_buf)
                            {   //Empty report (i.e. no command class)

                                //Callback the registered function
                                printf("report callback called here 1\n");
                                rpt_cb(intf, NULL, 0);
                            }

                            //Free memory
                            free(cmd_cls->cmd_cls_buf);
                            cmd_cls->cmd_cls_buf = NULL;

                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_VERSION:
            {
                if (cmd_buf[1] == VERSION_COMMAND_CLASS_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_ver_fn    rpt_cb;
                        rpt_cb = (zwrep_ver_fn)report_cb;
                        //Callback the registered function
                        printf("report callback called here 3\n");
                        rpt_cb(intf, cmd_buf[2], cmd_buf[3]);
                    }
                }
                else if (cmd_buf[1] == VERSION_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwrep_zwver_fn  rpt_cb;
                        ext_ver_t       *ext_ver = NULL;
                        uint16_t        proto_ver;
                        uint16_t        app_ver;

                        rpt_cb = (zwrep_zwver_fn)report_cb;
                        proto_ver = ((uint16_t)cmd_buf[3])<<8 | cmd_buf[4];
                        app_ver = ((uint16_t)cmd_buf[5])<<8 | cmd_buf[6];

                        if ((cmd_len >= 9) && (intf->ver > 1))
                        {   //Version 2
                            uint16_t    fw_ver;
                            uint8_t     fw_cnt;
                            int         i;
                            int         j;


                            fw_cnt = cmd_buf[8];
                            if ((cmd_len - 9) >= (fw_cnt * 2))
                            {
                                ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (fw_cnt*2));
                                if (ext_ver)
                                {
                                    ext_ver->hw_ver = cmd_buf[7];
                                    ext_ver->fw_cnt = fw_cnt;
                                    j=9;
                                    for (i=0; i<fw_cnt; i++)
                                    {
                                        fw_ver = cmd_buf[j++];
                                        fw_ver = (fw_ver << 8) | cmd_buf[j++];
                                        ext_ver->fw_ver[i] = fw_ver;
                                    }
                                }
                            }
                        }

                        //Callback the registered function
                        printf("report callback called here 4\n");
                        rpt_cb(intf, proto_ver, app_ver, cmd_buf[2], ext_ver);
                        free(ext_ver);
                    }
                }
            }
            break;

        case COMMAND_CLASS_BASIC:
            {
                if (cmd_buf[1] == BASIC_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here 5\n");
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

#ifdef  TEST_EXT_CMD_CLASS
        case COMMAND_CLASS_EXT_TEST:
            {
                if (rpt_cmd == BASIC_REPORT)
                {
                    if (cmd_len > 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here 6\n");
                        rpt_cb(&ifd, cmd_buf[3]);
                    }
                }
            }
            break;
#endif
        case COMMAND_CLASS_SWITCH_MULTILEVEL:
            {
                if (cmd_buf[1] == SWITCH_MULTILEVEL_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here swich Multi-level\n");
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_lvl_sup_fn    rpt_cb;
                        rpt_cb = (zwrep_lvl_sup_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here 5\n");
                        rpt_cb(&ifd, cmd_buf[2] & 0x1F, cmd_buf[3] & 0x1F);
                    }
                }
            }
            break;

        case COMMAND_CLASS_SWITCH_BINARY:
            {
                if (cmd_buf[1] == SWITCH_BINARY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_switch_fn    rpt_cb;
                        rpt_cb = (zwrep_switch_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here, swich binary\n");
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_NODE_NAMING:
            {
                if (cmd_buf[1] == NODE_NAMING_NODE_LOCATION_REPORT
                    || cmd_buf[1] == NODE_NAMING_NODE_NAME_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_name_fn   rpt_cb;
                        rpt_cb = (zwrep_name_fn)report_cb;

                        if (cmd_len > (ZW_LOC_STR_MAX + 3))
                        {
                            cmd_len = ZW_LOC_STR_MAX + 3;
                        }
                        //Callback the registered function
                        printf("report callback called here, node Naming\n");
                        rpt_cb(intf, (char *)(cmd_buf + 3), cmd_len - 3, cmd_buf[2] & 0x07);
                    }
                }
            }
            break;

        case COMMAND_CLASS_SENSOR_BINARY:
            {
                if (cmd_buf[1] == SENSOR_BINARY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_bsensor_fn    rpt_cb;
                        rpt_cb = (zwrep_bsensor_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here,sensor binary\n");
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_SENSOR_MULTILEVEL:
            {
                if (cmd_buf[1] == SENSOR_MULTILEVEL_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwsensor_t          snsr_value;
                        zwrep_sensor_fn     rpt_cb;
                        rpt_cb = (zwrep_sensor_fn)report_cb;

                        snsr_value.type = cmd_buf[2];
                        snsr_value.precision = cmd_buf[3] >> 5;
                        snsr_value.unit = (cmd_buf[3] >> 3) & 0x03;
                        snsr_value.size = cmd_buf[3] & 0x07;

                        if (snsr_value.size > 4)
                        {
                            return;
                        }

                        if (cmd_len >= (4 + snsr_value.size))
                        {
                            memcpy(snsr_value.data, cmd_buf + 4, snsr_value.size);

                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            printf("report callback called here, sensor Multi-level,type=%02x,unit=%02x\n",snsr_value.type,snsr_value.unit);
                            printf("sensor velue=%02x,%02x,\n", snsr_value.data[0],snsr_value.data[1]);
                            rpt_cb(&ifd, &snsr_value);
                        }
                    }
                }
                else if (cmd_buf[1] == SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_sensor_sup_fn rpt_cb;
                        uint8_t             snsr_type[248];
                        int                 i;
                        int                 max_snsr_type;
                        uint8_t             type_len;

                        rpt_cb = (zwrep_sensor_sup_fn)report_cb;

                        type_len = 0;
                        max_snsr_type = (cmd_len - 2) * 8;

                        for (i = 0; i < max_snsr_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                snsr_type[type_len++] = i + 1;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here, sensor Multi-level support v5\n");
                        rpt_cb(&ifd, type_len, snsr_type);
                    }
                }
                else if (cmd_buf[1] == SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_sensor_unit_fn    rpt_cb;

                        rpt_cb = (zwrep_sensor_unit_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        printf("report callback called here, snesor Multi-level scale v5\n");
                        rpt_cb(&ifd, cmd_buf[2], cmd_buf[3] & 0x0F);
                    }
                }

            }
            break;

        case COMMAND_CLASS_ASSOCIATION:
            {
                if (cmd_buf[1] == ASSOCIATION_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        int                 i;
                        zwnet_p             nw;
                        zwnode_p            first_node;
                        zwnode_p            node;
                        zwepd_p             ep;
                        uint8_t             ep_cnt;
                        uint8_t             valid_ep_cnt;
                        zwrep_group_fn      rpt_cb;
                        rpt_cb = (zwrep_group_fn)report_cb;

                        //Find all the endpoints in this report
                        ep_cnt = cmd_len - 5;
                        ep = (zwepd_p)calloc(ep_cnt, sizeof(zwepd_t));

                        if (!ep)
                        {
                            break;
                        }

                        nw = intf->ep->node->net;

                        first_node = &nw->ctl;

                        valid_ep_cnt = 0;

                        for (i=0; i<ep_cnt; i++)
                        {
                            node = zwnode_find(first_node, cmd_buf[5 + i]);
                            if (node)
                            {   //Get the virtual endpoint descriptor
                                zwep_get_desc(&node->ep, ep + valid_ep_cnt);
                                valid_ep_cnt++;
                            }

                        }

                        //Check is there a previous report to consolidate
                        if (nw->asc_rpt.ep_cnt > 0)
                        {
                            //Check whether group id and max. node supported match
                            if (nw->asc_rpt.group_id == cmd_buf[2] &&
                                nw->asc_rpt.max_node == cmd_buf[3])
                            {
                                zwepd_p     ep_desc_tmp;
                                ep_desc_tmp = realloc(nw->asc_rpt.ep_buf,
                                                      (valid_ep_cnt + nw->asc_rpt.ep_cnt) * sizeof(zwepd_t));
                                if (ep_desc_tmp)
                                {
                                    nw->asc_rpt.ep_buf = ep_desc_tmp;
                                    //Add enpoints to the descriptor buffer
                                    for (i=0; i<valid_ep_cnt; i++)
                                    {
                                        nw->asc_rpt.ep_buf[nw->asc_rpt.ep_cnt++] = ep[i];
                                    }
                                }
                                free(ep);
                            }
                            else
                            {   //Group id and max. node supported do not match

                                //Free the association report storage
                                free(nw->asc_rpt.ep_buf);

                                //Create new association report storage based on current report
                                nw->asc_rpt.group_id = cmd_buf[2];
                                nw->asc_rpt.max_node = cmd_buf[3];
                                nw->asc_rpt.ep_buf = ep;
                                nw->asc_rpt.ep_cnt = valid_ep_cnt;
                            }
                        }
                        else
                        {   //This is the first report

                            //Create new association report storage based on current report
                            nw->asc_rpt.group_id = cmd_buf[2];
                            nw->asc_rpt.max_node = cmd_buf[3];
                            nw->asc_rpt.ep_buf = ep;
                            nw->asc_rpt.ep_cnt = valid_ep_cnt;

                        }

                        //Check whether this is the final report
                        if (cmd_buf[4] == 0)
                        {
                            zwif_get_desc(intf, &ifd);
                            //Callback
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.ep_cnt, nw->asc_rpt.ep_buf);

                            //Free the association report storage
                            free(nw->asc_rpt.ep_buf);
                            nw->asc_rpt.ep_buf = NULL;
                            nw->asc_rpt.ep_cnt = 0;
                        }
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUPINGS_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_group_sup_fn    rpt_cb;
                        rpt_cb = (zwrep_group_sup_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_SPECIFIC_GROUP_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_group_actv_fn    rpt_cb;
                        rpt_cb = (zwrep_group_actv_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }

            }
            break;

        case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
            {
                if (cmd_buf[1] == COMMAND_RECORDS_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwrep_grp_cmd_sup_fn    rpt_cb;
                        zwgrp_cmd_cap_t         cmd_cap;

                        rpt_cb = (zwrep_grp_cmd_sup_fn)report_cb;
                        cmd_cap.configurable = cmd_buf[2] & 0x01;
                        cmd_cap.config_type = (cmd_buf[2]>>1) & 0x01;
                        cmd_cap.max_len = cmd_buf[2]>>2;
                        cmd_cap.free_rec = (((uint16_t)cmd_buf[3]) << 8) | cmd_buf[4];
                        cmd_cap.total_rec = (((uint16_t)cmd_buf[5]) << 8) | cmd_buf[6];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &cmd_cap);
                    }
                }
                else if (cmd_buf[1] == COMMAND_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwrep_grp_cmd_fn    rpt_cb;
                        rpt_cb = (zwrep_grp_cmd_fn)report_cb;

                        if (cmd_len >= (6 + cmd_buf[5]))
                        {
                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], cmd_buf + 6, cmd_buf[5]);
                        }
                    }
                }
            }
            break;


        case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2:
            {
                if (cmd_buf[1] == MULTI_CHANNEL_ASSOCIATION_REPORT_V2)
                {
                    if (cmd_len >= 5)
                    {
                        int                 i;
                        int                 is_node_have_ep;    //flag to determine whether a node has endpoint
                        int                 is_ep_bit_addr;     //flag to determine whether endpoint is bit-addressable
                        unsigned            allocated_desc;
                        zwnet_p             nw;
                        zwepd_p             ep_desc_array;
                        zwrep_group_fn      rpt_cb;
                        uint8_t             ep_id;
                        uint8_t             node_id;
                        uint8_t             ep_cnt;
                        uint8_t             valid_ep_cnt;

                        rpt_cb = (zwrep_group_fn)report_cb;

                        //Find all the endpoints in this report
                        ep_cnt = cmd_len - 5;
                        ep_desc_array = (zwepd_p)calloc(ep_cnt, sizeof(zwepd_t));

                        if (!ep_desc_array)
                        {
                            break;
                        }
                        allocated_desc = ep_cnt;

                        nw = intf->ep->node->net;

                        valid_ep_cnt = 0;
                        is_node_have_ep = 0;    //init to node without endpoint

                        for (i=0; i<ep_cnt; i++)
                        {
                            //Check for the marker to mark the end of node without endpoints
                            if (!is_node_have_ep
                                && (cmd_buf[5 + i] == MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V2))
                            {
                                is_node_have_ep = 1;
                                continue;
                            }

                            node_id = cmd_buf[5 + i];

                            if (is_node_have_ep)
                            {
                                i++; //point to endpoint id
                                ep_id = cmd_buf[5 + i];

                                //Find the endpoint
                                is_ep_bit_addr = 0;
                                if ((intf->ver >= 2) && (ep_id & 0x80))
                                {
                                    //Bit-address is set:
                                    //Bit 0 is End Point 1, bit 1 is End Point 2 … bit 6 is End Point 7
                                    zwepd_p         ep_desc_tmp;
                                    unsigned        num_of_ep;
                                    int             j;

                                    is_ep_bit_addr = 1;

                                    //Count number of endpoints
                                    num_of_ep = 0;
                                    for (j=0; j<7; j++)
                                    {
                                        if ((ep_id >> j) & 0x01)
                                        {
                                            num_of_ep++;
                                        }
                                    }

                                    //Re-allocate memory
                                    ep_desc_tmp = realloc(ep_desc_array,
                                                          (allocated_desc + num_of_ep) * sizeof(zwepd_t));
                                    if (ep_desc_tmp)
                                    {
                                        allocated_desc += num_of_ep;
                                        ep_desc_array = ep_desc_tmp;
                                        //Add enpoints to the descriptor array
                                        for (j=0; j<7; j++)
                                        {
                                            if ((ep_id >> j) & 0x01)
                                            {
                                                ep_desc_array[valid_ep_cnt].net = nw;
                                                ep_desc_array[valid_ep_cnt].epid = j+1;
                                                ep_desc_array[valid_ep_cnt].nodeid = node_id;
                                                valid_ep_cnt++;
                                            }
                                        }
                                    }
                                }

                                if (!is_ep_bit_addr)
                                {   //Bit address not set
                                    ep_desc_array[valid_ep_cnt].net = nw;
                                    ep_desc_array[valid_ep_cnt].epid = ep_id;
                                    ep_desc_array[valid_ep_cnt].nodeid = node_id;
                                    valid_ep_cnt++;
                                }

                            }
                            else
                            {   //Node without endpoint
                                ep_desc_array[valid_ep_cnt].net = nw;
                                ep_desc_array[valid_ep_cnt].epid = VIRTUAL_EP_ID;
                                ep_desc_array[valid_ep_cnt].nodeid = node_id;
                                valid_ep_cnt++;
                            }
                        }

                        //Check is there a previous report to consolidate
                        if (nw->asc_rpt.ep_cnt > 0)
                        {
                            //Check whether group id and max. node supported match
                            if (nw->asc_rpt.group_id == cmd_buf[2] &&
                                nw->asc_rpt.max_node == cmd_buf[3])
                            {
                                zwepd_p     ep_desc_tmp;
                                ep_desc_tmp = realloc(nw->asc_rpt.ep_buf,
                                                      (valid_ep_cnt + nw->asc_rpt.ep_cnt) * sizeof(zwepd_t));
                                if (ep_desc_tmp)
                                {
                                    nw->asc_rpt.ep_buf = ep_desc_tmp;
                                    //Add enpoints to the descriptor buffer
                                    for (i=0; i<valid_ep_cnt; i++)
                                    {
                                        nw->asc_rpt.ep_buf[nw->asc_rpt.ep_cnt++] = ep_desc_array[i];
                                    }
                                }
                                free(ep_desc_array);
                            }
                            else
                            {   //Group id and max. node supported do not match

                                //Free the association report storage
                                free(nw->asc_rpt.ep_buf);

                                //Create new association report storage based on current report
                                nw->asc_rpt.group_id = cmd_buf[2];
                                nw->asc_rpt.max_node = cmd_buf[3];
                                nw->asc_rpt.ep_buf = ep_desc_array;
                                nw->asc_rpt.ep_cnt = valid_ep_cnt;
                            }
                        }
                        else
                        {   //This is the first report

                            //Create new association report storage based on current report
                            nw->asc_rpt.group_id = cmd_buf[2];
                            nw->asc_rpt.max_node = cmd_buf[3];
                            nw->asc_rpt.ep_buf = ep_desc_array;
                            nw->asc_rpt.ep_cnt = valid_ep_cnt;

                        }

                        //Check whether this is the final report
                        if (cmd_buf[4] == 0)
                        {
                            zwif_get_desc(intf, &ifd);

                            //Check if controller node id presents
                            for (i=0; i<nw->asc_rpt.ep_cnt; i++)
                            {
                                if (nw->asc_rpt.ep_buf[i].nodeid == nw->ctl.nodeid)
                                {   //Set controller's epid to zero
                                    nw->asc_rpt.ep_buf[i].epid = 0;
                                }
                            }

                            //Callback
                            rpt_cb(&ifd, cmd_buf[2], cmd_buf[3], nw->asc_rpt.ep_cnt, nw->asc_rpt.ep_buf);

                            //Free the association report storage
                            free(nw->asc_rpt.ep_buf);
                            nw->asc_rpt.ep_buf = NULL;
                            nw->asc_rpt.ep_cnt = 0;
                        }
                    }
                }
                else if (cmd_buf[1] == MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_group_sup_fn    rpt_cb;
                        rpt_cb = (zwrep_group_sup_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }

            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_V2:
            {
                switch (cmd_buf[1])
                {
                    case MULTI_INSTANCE_REPORT:
                        if (cmd_len >= 4)
                        {
                            zwrep_mul_inst_fn    rpt_cb;
                            rpt_cb = (zwrep_mul_inst_fn)report_cb;
                            //Callback the registered function
                            rpt_cb(intf, cmd_buf[2], cmd_buf[3]);
                        }
                        break;

                    case MULTI_CHANNEL_END_POINT_REPORT_V2:
                        if (cmd_len >= 4)
                        {
                            zwrep_mc_ep_fn    rpt_cb;
                            rpt_cb = (zwrep_mc_ep_fn)report_cb;
                            //Callback the registered function
                            rpt_cb(intf, cmd_buf[3] & 0x7F, cmd_buf[2]);
                        }
                        break;

                    case MULTI_CHANNEL_CAPABILITY_REPORT_V2:
                        if (cmd_len >= 6)
                        {
                            ep_cap_t            ep_cap;
                            zwrep_mc_cap_fn     rpt_cb;
                            uint16_t            *cmd_cls_sec;
                            uint8_t             cmd_cnt_sec;

                            rpt_cb = (zwrep_mc_cap_fn)report_cb;

                            ep_cap.ep = cmd_buf[2] & 0x7F;
                            ep_cap.ep_dynamic = (cmd_buf[2] & 0x80)? 1:0;
                            ep_cap.gen = cmd_buf[3];
                            ep_cap.spec = cmd_buf[4];

                            if (util_cmd_cls_parse(cmd_buf + 5, cmd_len - 5, &ep_cap.cmd_cls, &ep_cap.cmd_cnt, &cmd_cls_sec, &cmd_cnt_sec))
                            {
                                //Callback the registered function
                                rpt_cb(intf, &ep_cap);

                                free(ep_cap.cmd_cls);
                                free(cmd_cls_sec);
                            }
                        }
                        break;

                }

            }
            break;

        case COMMAND_CLASS_WAKE_UP:
            {
                switch (cmd_buf[1])
                {
                    case WAKE_UP_NOTIFICATION:
                        {
                            //Dequeue the commands
#ifdef SEND_NO_MORE_INFO_AFTER_DATA
                            if (zwif_cmd_dequeue(intf, rx_sts) == 0)
#else
                            if (zwif_cmd_dequeue(intf) == 0)
#endif
                            {   //No command in the queue.
                                //Send WAKE_UP_NO_MORE_INFORMATION if the
                                //notification is not a broadcast frame
                                if ((rx_sts & RECEIVE_STATUS_TYPE_MASK) !=
                                    RECEIVE_STATUS_TYPE_BROAD)
                                {
                                    uint8_t     resp[] =
                                    {
                                        COMMAND_CLASS_WAKE_UP,
                                        WAKE_UP_NO_MORE_INFORMATION
                                    };

                                    //Send the response
                                    zwif_get_desc(intf, &ifd);
                                    zwif_exec_ex(&ifd, resp, 2, zwif_tx_sts_cb, NULL,
                                                 ZWIF_OPT_SKIP_MUL_Q, NULL);

                                }
                            }
                        }
                        break;

                    case WAKE_UP_INTERVAL_REPORT:
                        if (cmd_len >= 6)
                        {
                            zwif_wakeup_p      wake_up;
                            zwrep_wakeup_fn    rpt_cb;

                            wake_up = &intf->ep->node->net->wake_up;

                            //Clear the wake up setting
                            memset(wake_up, 0, sizeof(zwif_wakeup_t));

                            wake_up->cur =((uint32_t)(cmd_buf[2])) << 16
                                          | ((uint32_t)(cmd_buf[3])) << 8
                                          | cmd_buf[4];

                            wake_up->node.nodeid = cmd_buf[5];
                            wake_up->node.net = intf->ep->node->net;

                            //Save the interval
                            intf->ep->node->wkup_intv = wake_up->cur;

                            zwif_get_desc(intf, &ifd);
                            if (intf->ver >= 2)
                            {   //Get wake up interval capabilities report

                                //Get from cache first
                                if (intf->data_cnt)
                                {
                                    if_wkup_data_t  *wkup_dat;
                                    wkup_dat = (if_wkup_data_t *)intf->data;

                                    wake_up->min = wkup_dat->min;
                                    wake_up->max = wkup_dat->max;
                                    wake_up->def = wkup_dat->def;
                                    wake_up->interval = wkup_dat->interval;
                                }
                                else
                                {   //Get from the sleeping node
                                    zwif_get_report(&ifd, NULL, 0,
                                                    WAKE_UP_INTERVAL_CAPABILITIES_GET_V2, zwif_tx_sts_cb);
                                    break;
                                }
                            }
                            rpt_cb = (zwrep_wakeup_fn)report_cb;
                            //Callback the registered function
                            rpt_cb(&ifd, wake_up);

                        }
                        break;

                    case WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2:
                        if (cmd_len >= 14)
                        {
                            if_wkup_data_t     *wkup_dat;
                            zwif_wakeup_p      wake_up;
                            zwrep_wakeup_fn    rpt_cb;
                            rpt_cb = (zwrep_wakeup_fn)report_cb;

                            wake_up = &intf->ep->node->net->wake_up;

                            wake_up->min =((uint32_t)(cmd_buf[2])) << 16
                                          | ((uint32_t)(cmd_buf[3])) << 8
                                          | cmd_buf[4];

                            wake_up->max =((uint32_t)(cmd_buf[5])) << 16
                                          | ((uint32_t)(cmd_buf[6])) << 8
                                          | cmd_buf[7];

                            wake_up->def =((uint32_t)(cmd_buf[8])) << 16
                                          | ((uint32_t)(cmd_buf[9])) << 8
                                          | cmd_buf[10];

                            wake_up->interval =((uint32_t)(cmd_buf[11])) << 16
                                               | ((uint32_t)(cmd_buf[12])) << 8
                                               | cmd_buf[13];

                            //Cache the capabilities
                            if (wake_up->max)
                            {
                                //Free any existing interface data
                                if (intf->data_cnt > 0)
                                {
                                    free(intf->data);
                                }

                                intf->data_cnt = 1;
                                intf->data_item_sz = sizeof(if_wkup_data_t);

                                wkup_dat = (if_wkup_data_t *)malloc(intf->data_item_sz);

                                if (wkup_dat)
                                {
                                    wkup_dat->min = wake_up->min;
                                    wkup_dat->max = wake_up->max;
                                    wkup_dat->def = wake_up->def;
                                    wkup_dat->interval = wake_up->interval;

                                    //Assign to interface
                                    intf->data = wkup_dat;
                                }
                                else
                                {
                                    intf->data_cnt = 0;
                                }
                            }

                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, wake_up);
                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_METER_TBL_MONITOR:
            {
                switch (cmd_buf[1])
                {
                    case METER_TBL_TABLE_ID_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwmeter_p   meterd;     //Meter descriptor
                            uint8_t     str_len;    //Character string length


                            meterd = &intf->ep->node->net->meter_desc;

                            //Clear the meter descriptor setting
                            memset(meterd, 0, sizeof(zwmeter_t));

                            //Check whether admin number can be set
                            if (zwif_find_cls(intf->ep->intf, COMMAND_CLASS_METER_TBL_CONFIG))
                            {
                                meterd->caps = ZW_METER_CAP_CFG;
                            }
                            meterd->caps |= ZW_METER_CAP_MON;


                            str_len = cmd_buf[2] & 0x1F;

                            if (str_len > 0)
                            {
                                if (str_len > ZW_ID_STR_MAX)
                                {
                                    str_len = ZW_ID_STR_MAX;
                                }
                                memcpy(meterd->id, cmd_buf + 3, str_len);
                            }

                            //Get Admin number report
                            zwif_get_desc(intf, &ifd);
                            zwif_get_report(&ifd, NULL, 0,
                                            METER_TBL_TABLE_POINT_ADM_NO_GET, zwif_tx_sts_cb);

                        }
                        break;

                    case METER_TBL_TABLE_POINT_ADM_NO_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwmeter_p           meter_desc;
                            zwrep_meterd_fn     rpt_cb;
                            uint8_t             str_len;    //Character string length

                            rpt_cb = (zwrep_meterd_fn)report_cb;

                            meter_desc = &intf->ep->node->net->meter_desc;

                            str_len = cmd_buf[2] & 0x1F;

                            if (str_len > 0)
                            {
                                if (str_len > ZW_ADMIN_STR_MAX)
                                {
                                    str_len = ZW_ADMIN_STR_MAX;
                                }

                                if ((str_len + 3) > cmd_len)
                                {
                                    str_len = cmd_len - 3;
                                }

                                memcpy(meter_desc->admin, cmd_buf + 3, str_len);
                            }
                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, meter_desc);

                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_METER:
            {
                if (cmd_buf[1] == METER_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwmeter_dat_t     meter_value;
                        zwrep_meter_fn    rpt_cb;
                        rpt_cb = (zwrep_meter_fn)report_cb;

                        memset(&meter_value, 0, sizeof(zwmeter_dat_t));

                        meter_value.type = cmd_buf[2] & 0x1F;
                        meter_value.precision = cmd_buf[3] >> 5;
                        meter_value.unit = (cmd_buf[3] >> 3) & 0x03;
                        meter_value.size = cmd_buf[3] & 0x07;

                        if (meter_value.size > 4)
                        {
                            return;
                        }

                        if (cmd_len < (meter_value.size + 4))
                        {
                            break;
                        }
                        memcpy(meter_value.data, cmd_buf + 4, meter_value.size);

                        //Check if version 2 and above meter command class supported
                        if (intf->ver >= 2)
                        {
                            if (intf->ver >= 3)
                            {
                                meter_value.unit += ((cmd_buf[2] & 0x80) >> 5);
                            }

                            meter_value.rate_type = (cmd_buf[2] & 0x60) >> 5;

                            //Check whether delta time field is present
                            if ((cmd_len - 4 - meter_value.size) >= 2)
                            {
                                meter_value.delta_time = (((uint16_t)cmd_buf[4 + meter_value.size]) << 8)
                                                         | (uint16_t)cmd_buf[5 + meter_value.size];
                            }

                            //Check whether previous meter value field is present
                            if (meter_value.delta_time > 0)
                            {
                                if ((cmd_len - 6 - meter_value.size) >= meter_value.size)
                                {
                                    memcpy(meter_value.prv_data, cmd_buf + 6 + meter_value.size, meter_value.size);
                                }
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &meter_value);
                    }
                }
                else if (cmd_buf[1] == METER_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 4)
                    {
                        zwmeter_cap_t           meter_cap;
                        zwrep_meter_sup_fn      rpt_cb;
                        rpt_cb = (zwrep_meter_sup_fn)report_cb;

                        meter_cap.reset_cap = cmd_buf[2] >> 7;
                        meter_cap.type = cmd_buf[2] & 0x1F;
                        meter_cap.unit_sup = cmd_buf[3];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &meter_cap);
                    }
                }

            }
            break;

        case COMMAND_CLASS_METER_PULSE:
            {
                if (cmd_buf[1] == METER_PULSE_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwrep_pulsemeter_fn    rpt_cb;
                        uint32_t               pulse_cnt;

                        rpt_cb = (zwrep_pulsemeter_fn)report_cb;

                        //Convert byte stream into 32-bit unsigned integer
                        pulse_cnt = ((uint32_t)cmd_buf[2]) << 24
                                    | ((uint32_t)cmd_buf[3]) << 16
                                    | ((uint32_t)cmd_buf[4]) << 8
                                    | cmd_buf[5];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, pulse_cnt);
                    }
                }
            }
            break;

        case COMMAND_CLASS_SIMPLE_AV_CONTROL:
            {
                switch (cmd_buf[1])
                {
                    case SIMPLE_AV_CONTROL_REPORT:
                        if (cmd_len >= 3)
                        {
                            uint8_t             rpt_num;
                            zwif_av_cap_t       *av_cap;

                            //Check whether to callback to single ep state-machine
                            if (report_cb == zwnet_av_raw_rpt_cb)
                            {
                                zwrep_av_raw_fn sm_cb = (zwrep_av_raw_fn)report_cb;
                                sm_cb(intf, cmd_len, cmd_buf);
                                break;
                            }

                            av_cap = &intf->ep->node->net->av_cap;

                            //Initialize setting
                            if (av_cap->bit_mask_buf)
                            {
                                free(av_cap->bit_mask_buf);
                            }
                            memset(av_cap, 0, sizeof(zwif_av_cap_t));


                            //Save the total number of reports
                            av_cap->total_rpt = cmd_buf[2];

                            //Get the first report
                            zwif_get_desc(intf, &ifd);
                            rpt_num = 1;
                            zwif_get_report(&ifd, &rpt_num, 1,
                                            SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);

                        }
                        break;

                    case SIMPLE_AV_CONTROL_SUPPORTED_REPORT:
                        if (cmd_len >= 3)
                        {
                            zwif_av_cap_t       *av_cap;
                            uint8_t             rpt_num;        //Report number of this report
                            uint16_t            bit_mask_len;   //Bit mask length of this report
                            uint8_t             *tmp_buf;

                            //Check whether to callback to single ep state-machine
                            if (report_cb == zwnet_av_raw_rpt_cb)
                            {
                                zwrep_av_raw_fn sm_cb = (zwrep_av_raw_fn)report_cb;
                                sm_cb(intf, cmd_len, cmd_buf);
                                break;
                            }

                            av_cap = &intf->ep->node->net->av_cap;

                            rpt_num = cmd_buf[2];
                            bit_mask_len = cmd_len - 3;

                            //Check report number to avoid duplicate
                            if (av_cap->rpt_num >= rpt_num)
                            {
                                break;
                            }

                            //Store the bit-mask
                            if (av_cap->bit_mask_buf)
                            {
                                //Re-allocate memory
                                tmp_buf = realloc(av_cap->bit_mask_buf,
                                                  av_cap->bit_mask_len + bit_mask_len);
                            }
                            else
                            {
                                //Allocate memory
                                tmp_buf = malloc(bit_mask_len);
                            }

                            if (tmp_buf)
                            {
                                av_cap->bit_mask_buf = tmp_buf;
                                memcpy(av_cap->bit_mask_buf + av_cap->bit_mask_len,
                                       cmd_buf + 3, bit_mask_len);

                                av_cap->rpt_num = rpt_num;
                                av_cap->bit_mask_len += bit_mask_len;

                            }

                            zwif_get_desc(intf, &ifd);

                            //Check whether this is the last report
                            if (av_cap->total_rpt == rpt_num)
                            {
                                //Last report
                                zwrep_av_fn         rpt_cb;

                                rpt_cb = (zwrep_av_fn)report_cb;

                                //Callback the registered function
                                rpt_cb(&ifd, av_cap->bit_mask_len, av_cap->bit_mask_buf);

                                //Free memory
                                free(av_cap->bit_mask_buf);
                                av_cap->bit_mask_buf = NULL;

                            }
                            else
                            {
                                //There is at least one report pending, get next report
                                rpt_num = av_cap->rpt_num + 1;
                                zwif_get_report(&ifd, &rpt_num, 1,
                                                SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);
                            }

                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            {
                if (cmd_buf[1] == MANUFACTURER_SPECIFIC_REPORT)
                {
                    if (cmd_len >= 8)
                    {
                        uint16_t        manf_pdt_ids[3];
                        zwrep_manf_fn   rpt_cb;

                        rpt_cb = (zwrep_manf_fn)report_cb;

                        manf_pdt_ids[0] = ((uint16_t)cmd_buf[2]) << 8 | cmd_buf[3];
                        manf_pdt_ids[1] = ((uint16_t)cmd_buf[4]) << 8 | cmd_buf[5];
                        manf_pdt_ids[2] = ((uint16_t)cmd_buf[6]) << 8 | cmd_buf[7];

                        //Callback the registered function
                        rpt_cb(intf, manf_pdt_ids);
                    }
                }
                else if (cmd_buf[1] == DEVICE_SPECIFIC_REPORT_V2)
                {
                    if (cmd_len >= 4)
                    {
                        dev_id_t        dev_id = {0};
                        zwrep_devid_fn  rpt_cb;

                        rpt_cb = (zwrep_devid_fn)report_cb;

                        dev_id.type = cmd_buf[2] & 0x07;
                        dev_id.format = cmd_buf[3] >> 5;
                        dev_id.len = cmd_buf[3] & 0x1F;

                        if (cmd_len >= (4 + dev_id.len))
                        {
                            memcpy(dev_id.dev_id, cmd_buf + 4, dev_id.len);
                            //Callback the registered function
                            rpt_cb(intf, &dev_id);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_CONFIGURATION:
            {
                if (cmd_buf[1] == CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwconfig_t         param;
                        zwrep_config_fn    rpt_cb;
                        rpt_cb = (zwrep_config_fn)report_cb;

                        param.param_num = cmd_buf[2];
                        param.size = cmd_buf[3] & 0x07;

                        if (param.size > 4)
                        {
                            return;
                        }

                        if (cmd_len >= (4 + param.size))
                        {
                            memcpy(param.data, cmd_buf + 4, param.size);

                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, &param);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK:
            {
                if (cmd_buf[1] == DOOR_LOCK_OPERATION_REPORT)
                {
                    if (cmd_len >= 7)
                    {
                        zwdlck_op_t         op_status;
                        zwrep_dlck_op_fn    rpt_cb;
                        rpt_cb = (zwrep_dlck_op_fn)report_cb;

                        op_status.mode = cmd_buf[2];
                        op_status.out_mode = cmd_buf[3] >> 4;
                        op_status.in_mode = cmd_buf[3] & 0x0F;
                        op_status.cond = cmd_buf[4];
                        op_status.tmout_min = cmd_buf[5];
                        op_status.tmout_sec = cmd_buf[6];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &op_status);
                    }
                }
                else if (cmd_buf[1] == DOOR_LOCK_CONFIGURATION_REPORT)
                {
                    if (cmd_len >= 6)
                    {
                        zwdlck_cfg_t        config;
                        zwrep_dlck_cfg_fn   rpt_cb;
                        rpt_cb = (zwrep_dlck_cfg_fn)report_cb;

                        config.type = cmd_buf[2];
                        config.out_sta = cmd_buf[3] >> 4;
                        config.in_sta = cmd_buf[3] & 0x0F;
                        config.tmout_min = cmd_buf[4];
                        config.tmout_sec = cmd_buf[5];

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &config);
                    }
                }
            }
            break;

        case COMMAND_CLASS_USER_CODE:
            {
                if (cmd_buf[1] == USER_CODE_REPORT)
                {
                    if (cmd_len >= 4)//Allow 0 length user code
                    {
                        zwusrcod_t          usr_cod;
                        zwrep_usr_cod_fn    rpt_cb;

                        rpt_cb = (zwrep_usr_cod_fn)report_cb;

                        usr_cod.id = cmd_buf[2];
                        usr_cod.id_sts = cmd_buf[3];
                        usr_cod.code_len = cmd_len - 4;

                        if (usr_cod.code_len > MAX_USRCOD_LENGTH)
                        {   //Maximum allowed user code length exceeded
                            break;
                        }

                        memcpy(usr_cod.code, cmd_buf + 4, usr_cod.code_len);

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &usr_cod);
                    }
                }
                else if (cmd_buf[1] == USERS_NUMBER_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_usr_sup_fn   rpt_cb;
                        rpt_cb = (zwrep_usr_sup_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_ALARM:
            {
                if (cmd_buf[1] == ALARM_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwalrm_p            alrm_info;
                        zwrep_alrm_fn       rpt_cb;
                        rpt_cb = (zwrep_alrm_fn)report_cb;

                        alrm_info = (zwalrm_p)calloc(1, sizeof(zwalrm_t) + cmd_len);

                        if (!alrm_info)
                        {
                            return;
                        }

                        alrm_info->type = cmd_buf[2];
                        alrm_info->level = cmd_buf[3];

                        if (cmd_len == 4)
                        {   //Version 1
                            alrm_info->ex_info = 0;
                        }
                        else if (cmd_len >= 8)
                        {   //version 2 and above
                            uint8_t param_len;

                            alrm_info->ex_info = 1;
                            alrm_info->ex_zensr_nodeid = cmd_buf[4];
                            alrm_info->ex_status = cmd_buf[5];
                            alrm_info->ex_type = cmd_buf[6];
                            alrm_info->ex_event = cmd_buf[7];

                            param_len = cmd_buf[8] & 0x1F;
                            //Check whether event comes with parameter
                            if ((cmd_len >= 9) && (param_len > 0))
                            {
                                if (cmd_len < (9 + param_len))
                                {
                                    free(alrm_info);
                                    return;
                                }

                                //Check for system failure event parameters
                                if ((param_len > 0) && (cmd_buf[6] == ZW_ALRM_SYSTEM)
                                    && ((cmd_buf[7] == ZW_ALRM_EVT_HW_OEM_CODE) || (cmd_buf[7] == ZW_ALRM_EVT_SW_OEM_CODE)))
                                {

                                    memcpy(alrm_info->ex_evt_prm, cmd_buf + 9, param_len);
                                    alrm_info->ex_evt_len = param_len;
                                    alrm_info->ex_evt_type = ZW_ALRM_PARAM_OEM_ERR_CODE;
                                }
                                else if (param_len >= 3)
                                {
                                    if (cmd_buf[9] == COMMAND_CLASS_USER_CODE &&
                                        cmd_buf[10] == USER_CODE_REPORT)
                                    {
                                        //cmd_buf[9] = COMMAND_CLASS_USER_CODE
                                        //cmd_buf[10] = USER_CODE_REPORT
                                        //cmd_buf[11] = User Identifier
                                        //cmd_buf[12] = User ID Status
                                        alrm_info->ex_evt_type = ZW_ALRM_PARAM_USRID;
                                        alrm_info->ex_evt_len = 1;
                                        alrm_info->ex_evt_prm[0] = cmd_buf[11];
                                    }
                                    else if (cmd_buf[9] == COMMAND_CLASS_NODE_NAMING &&
                                             cmd_buf[10] == NODE_NAMING_NODE_LOCATION_REPORT)
                                    {
                                        //cmd_buf[9] = COMMAND_CLASS_NODE_NAMING
                                        //cmd_buf[10] = NODE_NAMING_NODE_LOCATION _REPORT
                                        //cmd_buf[11] = Char. Presentation
                                        //cmd_buf[12] = Node location char 1
                                        uint8_t str_len = param_len - 3;

                                        //Check whether to convert into utf8
                                        if ((cmd_buf[11] & 0x07) == CHAR_PRES_UTF16)
                                        {
                                            uint8_t *utf16_buf;
#ifdef WIN32
                                            char    utf8_str[ZW_LOC_STR_MAX + 1];
#else
                                            uint8_t utf8_str[ZW_LOC_STR_MAX + 1];
#endif
                                            int     result;

                                            utf16_buf = (uint8_t *)calloc(1, str_len + 2);//additional 2 bytes for null characters
                                            if (!utf16_buf)
                                            {
                                                free(alrm_info);
                                                return;
                                            }
                                            memcpy(utf16_buf, cmd_buf + 12, str_len);

                                            //convert into utf8
#ifdef WIN32
                                            result = plt_utf16_to_8((const char *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#else
                                            result = plt_utf16_to_8((const uint16_t *)utf16_buf, utf8_str, ZW_LOC_STR_MAX + 1, 1);
#endif

                                            free(utf16_buf);
                                            if (result == 0)
                                            {
                                                size_t utf8_len;

                                                utf8_len = strlen((const char *)utf8_str);

                                                if (utf8_len > cmd_len)
                                                {   //Not enough memory, have to realloc
                                                    zwalrm_p  tmp_buf;
                                                    tmp_buf = (zwalrm_p) realloc(alrm_info, sizeof(zwalrm_t) + utf8_len);
                                                    if (tmp_buf)
                                                    {
                                                        alrm_info = tmp_buf;
                                                    }
                                                    else
                                                    {
                                                        free(alrm_info);
                                                        return;
                                                    }
                                                }

                                                //Copy node location
                                                memcpy(alrm_info->ex_evt_prm, utf8_str, utf8_len);
                                                alrm_info->ex_evt_prm[utf8_len] = '\0';
                                                alrm_info->ex_evt_len = utf8_len + 1; //include NULL char

                                            }
                                        }
                                        else
                                        {
                                            //Check for valid UTF-8 string
                                            str_len = plt_utf8_chk(cmd_buf + 12, str_len);

                                            //Copy node location to the node
                                            memcpy(alrm_info->ex_evt_prm, cmd_buf + 12, str_len);
                                            alrm_info->ex_evt_len = str_len + 1; //include NULL char

                                        }
                                        alrm_info->ex_evt_type = ZW_ALRM_PARAM_LOC;

                                    }
                                }
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, alrm_info);
                        free(alrm_info);
                    }
                }
                else if (cmd_buf[1] == ALARM_TYPE_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_alrm_sup_fn   rpt_cb;
                        uint8_t             ztype[248];
                        int                 i;
                        int                 max_alrm_type;
                        uint8_t             ztype_len;

                        rpt_cb = (zwrep_alrm_sup_fn)report_cb;

                        ztype_len = 0;
                        max_alrm_type = (cmd_buf[2] & 0x1F) * 8;

                        for (i = 0; i < max_alrm_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 3] >> (i & 0x07)) & 0x01)
                            {
                                ztype[ztype_len++] = i;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] >> 7, ztype_len, ztype);
                    }
                }
                else if (cmd_buf[1] == EVENT_SUPPORTED_REPORT_V3)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_alrm_evt_fn   rpt_cb;
                        uint8_t             sup_evt[248];
                        int                 i;
                        int                 max_evt;
                        uint8_t             evt_len;

                        rpt_cb = (zwrep_alrm_evt_fn)report_cb;

                        evt_len = cmd_buf[3] & 0x1F;

                        if (evt_len && (cmd_len >= (evt_len + 4)))
                        {
                            if (rpt_cb == zwnet_alrm_evt_rpt_cb)
                            {   //Return raw bitmask
                                memcpy(sup_evt, cmd_buf + 4, evt_len);
                            }
                            else
                            {   //Return individual events
                                max_evt = evt_len * 8;
                                evt_len = 0;

                                for (i=0; i < max_evt; i++)
                                {
                                    if ((cmd_buf[(i>>3) + 4] >> (i & 0x07)) & 0x01)
                                    {
                                        sup_evt[evt_len++] = i;
                                    }
                                }
                            }
                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, cmd_buf[2], evt_len, sup_evt);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_BATTERY:
            {
                if (cmd_buf[1] == BATTERY_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
            {
                if (cmd_buf[1] == THERMOSTAT_FAN_MODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_fan_md_fn       rpt_cb;
                        uint8_t                     off;

                        rpt_cb = (zwrep_thrmo_fan_md_fn)report_cb;

                        if (intf->ver >= 3)
                        {
                            off = (cmd_buf[2] & 0x80)? 1 : 0;
                        }
                        else
                        {
                            off = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, off, cmd_buf[2] & 0x0F);
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_FAN_MODE_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_fan_md_sup_fn   rpt_cb;
                        uint8_t             fan_mod[248];
                        int                 i;
                        int                 max_fan_mod;
                        uint8_t             fan_mod_len;
                        uint8_t             off;

                        rpt_cb = (zwrep_thrmo_fan_md_sup_fn)report_cb;

                        fan_mod_len = 0;
                        max_fan_mod = (cmd_len - 2) * 8;

                        for (i = 0; i < max_fan_mod; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                fan_mod[fan_mod_len++] = i;
                            }
                        }

                        off = (intf->ver >= 2)? 1 : 0;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, off, fan_mod_len, fan_mod);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
            {
                if (cmd_buf[1] == THERMOSTAT_FAN_STATE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_fan_sta_fn      rpt_cb;

                        rpt_cb = (zwrep_thrmo_fan_sta_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] & 0x0F);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_MODE:
            {
                if (cmd_buf[1] == THERMOSTAT_MODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_md_fn       rpt_cb;

                        rpt_cb = (zwrep_thrmo_md_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] & 0x1F);
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_MODE_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_md_sup_fn   rpt_cb;
                        uint8_t             sup_mod[248];
                        int                 i;
                        int                 max_sup_mod;
                        uint8_t             sup_mod_len;

                        rpt_cb = (zwrep_thrmo_md_sup_fn)report_cb;

                        sup_mod_len = 0;
                        max_sup_mod = (cmd_len - 2) * 8;

                        for (i = 0; i < max_sup_mod; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                sup_mod[sup_mod_len++] = i;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, sup_mod_len, sup_mod);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
            {
                if (cmd_buf[1] == THERMOSTAT_OPERATING_STATE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_op_sta_fn      rpt_cb;

                        rpt_cb = (zwrep_thrmo_op_sta_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] & 0x0F);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETBACK:
            {
                if (cmd_buf[1] == THERMOSTAT_SETBACK_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_thrmo_setb_fn     rpt_cb;
                        uint8_t                 state;

                        rpt_cb = (zwrep_thrmo_setb_fn)report_cb;

                        if (cmd_buf[3] == 0x79)
                        {
                            state = ZW_THRMO_SETB_STA_FROST_PROCT;
                        }
                        else if (cmd_buf[3] == 0x7A)
                        {
                            state = ZW_THRMO_SETB_STA_ENER_SAVE;
                        }
                        else if ((cmd_buf[3] >= 0x7B) && (cmd_buf[3] < 0x7F))
                        {   //Invalid
                            return;
                        }
                        else if (cmd_buf[3] == 0x7F)
                        {
                            state = ZW_THRMO_SETB_STA_UNUSED;
                        }
                        else
                        {
                            state = ZW_THRMO_SETB_STA_SETB;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2] & 0x03, state, cmd_buf[3]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETPOINT:
            {
                if (cmd_buf[1] == THERMOSTAT_SETPOINT_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwsetp_t                setp_val;
                        zwrep_thrmo_setp_fn     rpt_cb;

                        rpt_cb = (zwrep_thrmo_setp_fn)report_cb;

                        setp_val.type = cmd_buf[2] & 0x0F;
                        setp_val.precision = cmd_buf[3] >> 5;
                        setp_val.unit = (cmd_buf[3] >> 3) & 0x03;
                        setp_val.size = cmd_buf[3] & 0x07;

                        if (setp_val.size > 4)
                        {
                            return;
                        }

                        if (cmd_len >= (4 + setp_val.size))
                        {
                            memcpy(setp_val.data, cmd_buf + 4, setp_val.size);

                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, &setp_val);
                        }
                    }
                }
                else if (cmd_buf[1] == THERMOSTAT_SETPOINT_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_thrmo_setp_sup_fn   rpt_cb;
                        uint8_t             sup_type[248];
                        int                 i;
                        int                 max_sup_type;
                        uint8_t             sup_type_len;

                        rpt_cb = (zwrep_thrmo_setp_sup_fn)report_cb;

                        sup_type_len = 0;
                        max_sup_type = (cmd_len - 2) * 8;

                        //Map type 0 to type 1 for Danfoss Living Connect workaround
                        if (cmd_buf[2] & 0x01)
                        {
                            sup_type[sup_type_len++] = 1;
                        }

                        for (i = 1; i < max_sup_type; i++)
                        {
                            if ((cmd_buf[(i>>3) + 2] >> (i & 0x07)) & 0x01)
                            {
                                sup_type[sup_type_len++] = (i >= 3)? (i + 4) : i;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, sup_type_len, sup_type);
                    }
                }
            }
            break;

        case COMMAND_CLASS_CLOCK:
            {
                if (cmd_buf[1] == CLOCK_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_clock_fn    rpt_cb;
                        rpt_cb = (zwrep_clock_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]>>5, cmd_buf[2] & 0x1F, cmd_buf[3]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
            {
                if (cmd_buf[1] == SCHEDULE_REPORT)
                {
                    if (cmd_len >= 30)
                    {
                        zwrep_clmt_ctl_schd_fn  rpt_cb;
                        zwcc_shed_t             sched;
                        uint8_t                 *cmdptr;
                        uint8_t                 state;


                        sched.weekday = cmd_buf[2] & 0x07;

                        rpt_cb = (zwrep_clmt_ctl_schd_fn)report_cb;

                        cmdptr = cmd_buf + 3;

                        sched.total = 0;

                        for (i=0; i < 9; i++)
                        {
                            if (cmdptr[2] == 0x79)
                            {
                                state = ZW_THRMO_SETB_STA_FROST_PROCT;
                            }
                            else if (cmdptr[2] == 0x7A)
                            {
                                state = ZW_THRMO_SETB_STA_ENER_SAVE;
                            }
                            else if ((cmdptr[2] >= 0x7B) && (cmdptr[2] < 0x7F))
                            {   //Invalid
                                return;
                            }
                            else if (cmdptr[2] == 0x7F)
                            {   //Unused state
                                break;
                            }
                            else
                            {
                                state = ZW_THRMO_SETB_STA_SETB;
                            }

                            sched.swpts[i].state = state;
                            sched.swpts[i].hour = cmdptr[0] & 0x1F;
                            sched.swpts[i].minute = cmdptr[1] & 0x3F;
                            if (state == ZW_THRMO_SETB_STA_SETB)
                            {
                                sched.swpts[i].tenth_deg = cmdptr[2];
                            }

                            //Update switch point counter
                            sched.total++;

                            //Update pointer
                            cmdptr += 3;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &sched);
                    }
                }
                else if (cmd_buf[1] == SCHEDULE_CHANGED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_clmt_ctl_schd_chg_fn    rpt_cb;
                        rpt_cb = (zwrep_clmt_ctl_schd_chg_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }

                }
                else if (cmd_buf[1] == SCHEDULE_OVERRIDE_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_clmt_ctl_schd_ovr_fn    rpt_cb;
                        zwcc_shed_ovr_t               schd_ovr;
                        rpt_cb = (zwrep_clmt_ctl_schd_ovr_fn)report_cb;

                        if (cmd_buf[3] == 0x79)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_FROST_PROCT;
                        }
                        else if (cmd_buf[3] == 0x7A)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_ENER_SAVE;
                        }
                        else if ((cmd_buf[3] >= 0x7B) && (cmd_buf[3] < 0x7F))
                        {   //Invalid
                            return;
                        }
                        else if (cmd_buf[3] == 0x7F)
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_UNUSED;
                        }
                        else
                        {
                            schd_ovr.state = ZW_THRMO_SETB_STA_SETB;
                            schd_ovr.tenth_deg = cmd_buf[3];
                        }

                        schd_ovr.type = cmd_buf[2] & 0x03;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &schd_ovr);
                    }

                }

            }
            break;

        case COMMAND_CLASS_PROTECTION:
            {
                if (cmd_buf[1] == PROTECTION_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_fn     rpt_cb;

                        rpt_cb = (zwrep_prot_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], (cmd_len >= 4)? cmd_buf[3] : ZW_RFPROT_UNPROT);

                    }
                }
                else if (cmd_buf[1] == PROTECTION_SUPPORTED_REPORT_V2)
                {
                    if (cmd_len >= 7)
                    {
                        zwrep_prot_sup_fn   rpt_cb;
                        zwprot_sup_t        sup = {0};
                        int                 i;

                        rpt_cb = (zwrep_prot_sup_fn)report_cb;

                        sup.excl_ctl = (cmd_buf[2] & 0x02)? 1 : 0;
                        sup.tmout = cmd_buf[2] & 0x01;
                        //Init
                        sup.lprot_len = 0;
                        sup.rfprot_len = 0;

                        for (i = 0; i < 16; i++)
                        {
                            if ((cmd_buf[(i>>3) + 3] >> (i & 0x07)) & 0x01)
                            {
                                sup.lprot[sup.lprot_len++] = i;
                            }
                            if ((cmd_buf[(i>>3) + 5] >> (i & 0x07)) & 0x01)
                            {
                                sup.rfprot[sup.rfprot_len++] = i;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &sup);
                    }
                }
                else if (cmd_buf[1] == PROTECTION_EC_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_ec_fn    rpt_cb;
                        rpt_cb = (zwrep_prot_ec_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == PROTECTION_TIMEOUT_REPORT_V2)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_prot_tmout_fn    rpt_cb;
                        rpt_cb = (zwrep_prot_tmout_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }

            }
            break;

        case COMMAND_CLASS_APPLICATION_STATUS:
            {
                if (cmd_buf[1] == APPLICATION_BUSY)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_appl_busy_fn     rpt_cb;

                        rpt_cb = (zwrep_appl_busy_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], cmd_buf[3]);

                    }
                }
                else if (cmd_buf[1] == APPLICATION_REJECTED_REQUEST)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_appl_reject_fn   rpt_cb;

                        rpt_cb = (zwrep_appl_reject_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_INDICATOR:
            {
                if (cmd_buf[1] == INDICATOR_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fn    rpt_cb;
                        rpt_cb = (zwrep_fn)report_cb;
                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
            }
            break;

        case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            {
                if (cmd_buf[1] == FIRMWARE_UPDATE_MD_GET)
                {
                    if (cmd_len >= 5)
                    {
                        zw_fw_tx_fn     fw_tx_fn;
                        uint16_t        frag_num;
                        uint8_t         rpt_cnt;

                        fw_tx_fn = (zw_fw_tx_fn)report_cb;

                        rpt_cnt = cmd_buf[2];
                        frag_num = cmd_buf[3];
                        frag_num = (frag_num << 8) | cmd_buf[4];

                        fw_tx_fn(intf, frag_num, rpt_cnt);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_MD_REPORT)
                {
                    if (cmd_len >= 8)
                    {
                        if_fw_tmp_dat_t     *fw_data;
                        uint16_t            *fw_id;
                        int                 i;
                        int                 j;
                        zwfw_info_t         fw_info;
                        zwrep_fw_info_fn    rpt_cb;
                        rpt_cb = (zwrep_fw_info_fn)report_cb;

                        fw_info.vid = cmd_buf[2];
                        fw_info.vid = (fw_info.vid << 8) | cmd_buf[3];
                        fw_info.zw_fw_id = cmd_buf[4];
                        fw_info.zw_fw_id = (fw_info.zw_fw_id << 8) | cmd_buf[5];
                        fw_info.chksum = cmd_buf[6];
                        fw_info.chksum = (fw_info.chksum << 8) | cmd_buf[7];

                        if (cmd_len == 8)
                        {   //version 1 & 2
                            fw_info.upgrade_flg = ZW_FW_UPGD_YES;
                            fw_info.other_fw_cnt = 0;
                            fw_info.other_fw_id = NULL;
                            fw_info.max_frag_sz = (intf->ver == 1)? ZW_FW_FRAG_SZ_V1 : ZW_FW_FRAG_SZ_V2;
                            fw_info.fixed_frag_sz = 1;
                        }
                        else if (cmd_len >= 12)
                        {   //version 3
                            fw_info.upgrade_flg = cmd_buf[8];
                            fw_info.other_fw_cnt = cmd_buf[9];
                            fw_info.max_frag_sz = cmd_buf[10];
                            fw_info.max_frag_sz = (fw_info.max_frag_sz << 8) | cmd_buf[11];
                            fw_info.fixed_frag_sz = 0;

                            if (fw_info.other_fw_cnt > 0)
                            {
                                if (((cmd_len - 12)/2) == fw_info.other_fw_cnt)
                                {
                                    fw_id = calloc(fw_info.other_fw_cnt, sizeof(uint16_t));

                                    if (!fw_id)
                                    {
                                        return;
                                    }

                                    fw_info.other_fw_id = fw_id;

                                    for (i=0, j=0; i<fw_info.other_fw_cnt; i++)
                                    {
                                        fw_id[i] = cmd_buf[12+(j++)];
                                        fw_id[i] = (fw_id[i] << 8) | cmd_buf[12+(j++)];
                                    }
                                }
                                else
                                {
                                    return;
                                }
                            }
                            else
                            {
                                fw_info.other_fw_id = NULL;
                            }
                        }
                        else
                        {   //error
                            return;
                        }

                        //Save the maximum fragment size
                        if (!intf->tmp_data)
                        {
                            fw_data = (if_fw_tmp_dat_t *)calloc(1, sizeof(if_fw_tmp_dat_t));
                            if (!fw_data)
                            {
                                free(fw_info.other_fw_id);
                                return;
                            }
                            //Assign to interface
                            intf->tmp_data = fw_data;
                        }
                        fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;
                        fw_data->max_frag_sz = fw_info.max_frag_sz;
                        fw_data->fixed_frag_sz = fw_info.fixed_frag_sz;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, &fw_info);
                        free(fw_info.other_fw_id);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_UPDATE_MD_REQUEST_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_fw_updt_sts_fn    rpt_cb;
                        rpt_cb = (zwrep_fw_updt_sts_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == FIRMWARE_UPDATE_MD_STATUS_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        if_fw_tmp_dat_t         *fw_data;
                        zwrep_fw_updt_cmplt_fn  rpt_cb;
                        int                     wait_tm_valid = 0;
                        uint16_t                wait_tm = 0;

                        rpt_cb = (zwrep_fw_updt_cmplt_fn)report_cb;

                        //Stop any further firmware fragment transfer
                        fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;
                        if (fw_data)
                        {
                            fw_data->frag_sz = 0;
                            //Close the firmware file
                            fclose(fw_data->fw_file);
                            fw_data->fw_file = NULL;
                        }

                        if ((intf->ver >=3) && (cmd_buf[2] == ZW_FW_UPDT_CMPLT_OK_RESTART)
                            && (cmd_len >= 5))
                        {
                            wait_tm_valid = 1;
                            wait_tm = cmd_buf[3];
                            wait_tm = (wait_tm << 8) | cmd_buf[4];

                            if (wait_tm == 0)
                            {
                                wait_tm_valid = 0;
                            }
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], wait_tm, wait_tm_valid);

                        //Wait target to restart
                        if (wait_tm_valid && intf->ep->node->restart_cb)
                        {
                            if (fw_data->restrt_tmr_ctx)
                            {
                                plt_tmr_stop(&ifd.net->plt_ctx, fw_data->restrt_tmr_ctx);
                                fw_data->restrt_tmr_ctx = NULL;
                            }

                            fw_data->nw = ifd.net;
                            zwnode_get_desc(intf->ep->node, &fw_data->node);

                            //Start timer
                            fw_data->restrt_tmr_ctx = plt_tmr_start(&ifd.net->plt_ctx, wait_tm*1000, zwif_fw_restrt_tout_cb, fw_data);
                        }
                    }
                }
            }
            break;

        case COMMAND_CLASS_ZWAVEPLUS_INFO:
            {
                if (cmd_buf[1] == ZWAVEPLUS_INFO_REPORT)
                {
                    if (cmd_len >= 5)
                    {
                        zwplus_info_t   info = {0};
                        zwrep_info_fn   rpt_cb;
                        rpt_cb = (zwrep_info_fn)report_cb;

                        info.zwplus_ver = cmd_buf[2];
                        info.role_type = cmd_buf[3];
                        info.node_type = cmd_buf[4];

                        if (cmd_len >= 9)
                        {
                            info.instr_icon = cmd_buf[5];
                            info.instr_icon = (info.instr_icon << 8) | cmd_buf[6];
                            info.usr_icon = cmd_buf[7];
                            info.usr_icon = (info.usr_icon << 8) | cmd_buf[8];
                        }

                        rpt_cb(intf, &info);
                    }
                }
            }
            break;

        case COMMAND_CLASS_DEVICE_RESET_LOCALLY:
            {
                zwnet_p nw = intf->ep->node->net;

                if (cmd_buf[1] == DEVICE_RESET_LOCALLY_NOTIFICATION)
                {   //Send request for failed node removal
                    zwnet_exec_req_t     rm_failed_req = {0};

                    rm_failed_req.node_id = intf->ep->node->nodeid;
                    rm_failed_req.action = EXEC_ACT_PROBE_FAILED_NODE;

                    util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                  (uint8_t *)&rm_failed_req, sizeof(zwnet_exec_req_t));
                    plt_sem_post(nw->nw_exec_sem);
                }
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                if (cmd_buf[1] == ASSOCIATION_GROUP_NAME_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_grp_name_fn     rpt_cb;

                        rpt_cb = (zwrep_grp_name_fn)report_cb;

                        if (cmd_buf[3] > (cmd_len - 4))
                        {
                            return;
                        }

                        rpt_cb(intf, cmd_buf[2], cmd_buf[3], cmd_buf + 4);
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUP_INFO_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_grp_info_fn   rpt_cb;
                        zw_grp_info_ent_t   *grp_info;
                        int                 i;
                        uint8_t             *grp_info_ptr;
                        uint16_t            profile;
                        uint16_t            evt_code;
                        uint8_t             grp_cnt;


                        rpt_cb = (zwrep_grp_info_fn)report_cb;
                        grp_cnt = cmd_buf[2] & 0x3F;

                        if ((grp_cnt * 7) > (cmd_len - 3))
                        {
                            return;
                        }

                        grp_info = (zw_grp_info_ent_t *)calloc(1, (grp_cnt * sizeof(zw_grp_info_ent_t)));

                        if (grp_info)
                        {
                            grp_info_ptr = cmd_buf + 3;

                            for (i=0; i<grp_cnt; i++)
                            {
                                grp_info[i].grp_num =  *grp_info_ptr++;
                                if (*grp_info_ptr++ != 0)
                                {   //Mode not equals to zero, discard the whole report
                                    free(grp_info);
                                    return;
                                }
                                profile = *grp_info_ptr++;
                                profile = (profile << 8) | *grp_info_ptr++;
                                grp_info_ptr++;
                                evt_code = *grp_info_ptr++;
                                evt_code = (evt_code << 8) | *grp_info_ptr++;

                                grp_info[i].profile = profile;
                                grp_info[i].evt_code = evt_code;
                            }
                        }

                        rpt_cb(intf, grp_cnt, (cmd_buf[2] & 0x40)? 1 : 0, grp_info);
                        free(grp_info);
                    }
                }
                else if (cmd_buf[1] == ASSOCIATION_GROUP_COMMAND_LIST_REPORT)
                {
                    if (cmd_len >= 4)
                    {
                        zwrep_grp_cmd_lst_fn    rpt_cb;
                        uint8_t                 lst_len;
                        uint8_t                 cmd_ent_cnt;
                        grp_cmd_ent_t           *cmd_lst;

                        rpt_cb = (zwrep_grp_cmd_lst_fn)report_cb;
                        lst_len = cmd_buf[3];

                        if (lst_len > (cmd_len - 4))
                        {
                            return;
                        }

                        //Allocate memory for the command list
                        cmd_ent_cnt = lst_len >> 1;  //assuming the worst case with each command class equals to 1-byte
                        cmd_lst = (grp_cmd_ent_t *)calloc(1, (cmd_ent_cnt * sizeof(grp_cmd_ent_t)));

                        if (cmd_lst)
                        {
                            int         i;
                            uint8_t     *cmd_lst_ptr;
                            uint16_t    cls;
                            uint8_t     cmd;

                            cmd_lst_ptr = cmd_buf + 4;
                            cmd_ent_cnt = 0;

                            for (i=0; i<lst_len; i++)
                            {
                                if (*cmd_lst_ptr >= 0xF1)
                                {   //Extended command class
                                    if ((i + 2) < lst_len)
                                    {
                                        cls = *cmd_lst_ptr++;
                                        cls = (cls << 8) | (*cmd_lst_ptr++);
                                        cmd = *cmd_lst_ptr++;
                                        i += 2;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                else
                                {   //Normal command class
                                    if ((i + 1) < lst_len)
                                    {
                                        cls = *cmd_lst_ptr++;
                                        cmd = *cmd_lst_ptr++;
                                        i++;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                cmd_lst[cmd_ent_cnt].cls = cls;
                                cmd_lst[cmd_ent_cnt].cmd = cmd;
                                cmd_ent_cnt++;
                            }
                            rpt_cb(intf, cmd_buf[2], cmd_ent_cnt, cmd_lst);
                            free(cmd_lst);
                        }
                    }
                }
            }
            break;

		case COMMAND_CLASS_POWERLEVEL:
            {
                if (cmd_buf[1] == POWERLEVEL_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_power_level_fn       rpt_cb;
                        uint8_t                    byTimeout;

                        rpt_cb = (zwrep_power_level_fn)report_cb;

						if(cmd_len >= 4)
						{
                           byTimeout = cmd_buf[3];
                        }
                        else
                        {
                            byTimeout = 0;
                        }

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], byTimeout);
                    }
                }
                else if (cmd_buf[1] == POWERLEVEL_TEST_NODE_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_power_level_test_fn   rpt_cb;
                        uint8_t             byStatus;
                        uint16_t			wFramCnt;

                        rpt_cb = (zwrep_power_level_test_fn)report_cb;

						if(cmd_len >= 4)
						{
							byStatus = cmd_buf[3];
						}
						else
						{
							byStatus = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES;
						}

                        if(cmd_len >= 6)
						{
							wFramCnt = (cmd_buf[4] << 8) + cmd_buf[5];
						}
						else
						{
							wFramCnt = 0;
						}

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2], byStatus, wFramCnt);
                    }
                }
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK_LOGGING:
            {
                if (cmd_buf[1] == DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT)
                {
                    if (cmd_len >= 3)
                    {
                        zwrep_drlog_sup_fn  rpt_cb;

                        rpt_cb = (zwrep_drlog_sup_fn)report_cb;

                        zwif_get_desc(intf, &ifd);
                        //Callback the registered function
                        rpt_cb(&ifd, cmd_buf[2]);
                    }
                }
                else if (cmd_buf[1] == RECORD_REPORT)
                {
                    if (cmd_len >= 13)
                    {
                        zwrep_drlog_rep_fn  rpt_cb;
                        zwdrlog_rec_t       *rec;
                        uint8_t             usr_code_len;

                        rpt_cb = (zwrep_drlog_rep_fn)report_cb;

                        usr_code_len = cmd_len - 13;

                        rec = (zwdrlog_rec_t *)malloc(sizeof(zwdrlog_rec_t) + usr_code_len);

                        if (rec)
                        {
                            rec->rec_num = cmd_buf[2];
                            rec->year = cmd_buf[3];
                            rec->year = (rec->year << 8) | cmd_buf[4];
                            rec->month = cmd_buf[5];
                            rec->day = cmd_buf[6];
                            rec->hour = cmd_buf[7] & 0x1F;
                            rec->rec_valid = (cmd_buf[7] >> 5);
                            rec->min = cmd_buf[8];
                            rec->second = cmd_buf[9];
                            rec->evt = cmd_buf[10];
                            rec->usr_id = cmd_buf[11];
                            rec->usr_code_len = cmd_buf[12];

                            if (rec->usr_code_len <= usr_code_len)
                            {
                                memcpy(rec->usr_code, cmd_buf + 13, rec->usr_code_len);
                            }
                            else
                            {
                                rec->usr_code_len = 0;
                            }

                            zwif_get_desc(intf, &ifd);
                            //Callback the registered function
                            rpt_cb(&ifd, rec);
                            free(rec);
                        }
                    }
                }
            }
            break;
    }
}


/**
zwif_tx_sts_cb - callback function to display transmit status
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwif_tx_sts_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        printf("Send data completed successfully\n");
    }
    else
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Send data completed with error:%s",
                        (tx_sts < 6)?
                        zwif_tx_cmplt_sts[tx_sts]  : "unknown");
    }
}


/**
zwif_exec_cb - execute action on an interface callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwif_exec_cb(appl_layer_ctx_t    *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    //Callback higher-level application
    if (nw->init.appl_tx)
    {
        nw->init.appl_tx(nw->init.user, tx_sts);
    }
}


/**
zwif_sec_tx_cb - security layer transmission callback
@param[in]	nw		network
@param[in]  nodeid	destination node id of the transmission
@param[in]  status	0= transmission ok; else negative error number
@param[in]	user	user context
*/
void zwif_sec_tx_cb(zwnet_p nw, uint8_t nodeid, int status, void **user)
{
    tx_cmplt_cb_t   cb = (tx_cmplt_cb_t)user[0];

    if (cb)
    {
        cb(&nw->appl_ctx, (status == 0)? TRANSMIT_COMPLETE_OK : TRANSMIT_COMPLETE_FAIL, user[1]);
    }

    if (status != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwif_sec_tx_cb: node id:%u, status:%d",
                        nodeid, status);
    }
}


/**
zwnode_mul_cmd_tmout_cb - Multi command timeout callback
@param[in] data     Pointer to node
@return
*/
static void    zwnode_mul_cmd_tmout_cb(void *data)
{
    int         ret;
    zwnode_p    node = (zwnode_p)data;
    zwnet_p     nw = node->net;
    util_lst_t  *cmd_lst_hd;
    zwnoded_t   noded;
    uint8_t     max_sz;

    plt_mtx_lck(nw->mtx);
    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
    node->mul_cmd_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Send multi command
    cmd_lst_hd = node->mul_cmd_q_hd;
    max_sz = (node->crc_cap)? (MAX_ZWAVE_PKT_SIZE - CRC16_OVERHEAD) : MAX_ZWAVE_PKT_SIZE;

    //Reset the command queue for the node
    node->mul_cmd_q_hd = NULL;
    node->mul_cmd_q_cnt = 0;

    ret = zwnode_get_desc(node, &noded);
    plt_mtx_ulck(nw->mtx);

    if (ret == ZW_ERR_NONE)
    {
        zwnode_mul_cmd_send(&noded, &cmd_lst_hd, max_sz);
    }
}


/**
zwnode_wait_rpt_flg_clr - Reset the wait report flag
@param[in]	net	        Network
@param[in]	nodeid	    Node id
@return
*/
static void zwnode_wait_rpt_flg_clr(zwnet_p net, uint8_t nodeid)
{
    zwnode_p            node;

    //debug_zwapi_msg(&nw->plt_ctx, "zwnode_wait_rpt_flg_clr");
    plt_mtx_lck(net->mtx);

    node = zwnode_find(&net->ctl, nodeid);

    if (!node)
    {
        plt_mtx_ulck(net->mtx);
        return;
    }

    zwnode_wait_rpt_tmout_cb(node);

    plt_mtx_ulck(net->mtx);
}


/**
zwif_multi_cmd_cmp - Compare two multi-command
@param[in]	s1	multi-command 1
@param[in]	s2	multi-command 2
@return     zero if s1 equals to s2; else return non-zero
*/
static int zwif_multi_cmd_cmp(uint8_t *s1, uint8_t *s2)
{
    mul_cmd_q_ent_t  *mul_cmd1 = (mul_cmd_q_ent_t *)s1;
    mul_cmd_q_ent_t  *mul_cmd2 = (mul_cmd_q_ent_t *)s2;

    if ((mul_cmd1->ifd.epid == mul_cmd2->ifd.epid)
        && (mul_cmd1->cmd_id == mul_cmd2->cmd_id)
        && (mul_cmd1->dat_len == mul_cmd2->dat_len))
    {
        return memcmp(mul_cmd1->dat_buf, mul_cmd2->dat_buf, mul_cmd1->dat_len);
    }

    return 1;
}


/**
zwif_exec_ex - execute action on an interface with extra parameters
@param[in]	ifd         interface
@param[in]	cmd_buf		command and parameters
@param[in]	buf_len		length of cmd_buf in bytes
@param[in]	cb		    callback function for transmit status
@param[in]	user	    user parameter of callback function
@param[in]	opt		    option, ZWIF_OPT_XXX
@param[in]	xtra		extra parameter
@return	ZW_ERR_xxx
*/
int zwif_exec_ex(zwifd_p ifd, uint8_t *cmd_buf, int buf_len,
                 tx_cmplt_cb_t cb, void *user, int opt, void *xtra)
{
    int                 result;
    int                 wait_rpt = 0;   //Flag to indicate the command is expecting a response report.
    appl_snd_data_t     *prm;
    uint8_t             *cmd;
    zwnode_p            node;
    zwnet_p             nw = ifd->net;
#ifdef CRC16_ENCAP
    int                 crc16_cap;      //Flag to indicate the node supports CRC-16 encapsulation
    uint8_t             crc16_cmd[MAX_ZWAVE_PKT_SIZE];
#endif
    uint8_t             multi_ch_cmd[MAX_ZWAVE_PKT_SIZE];
    int                 len;
    int                 max_pkt_size = MAX_ZWAVE_PKT_SIZE;

    //Initialize command to the original input command
    cmd = cmd_buf;
    len = buf_len;

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, ifd->nodeid);
    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }
    //-------------------------------------------------
    // Check for node that requires wakeup beam
    //-------------------------------------------------
    if (opt & ZWIF_OPT_WAIT_RPT_SET)
    {   //Caller must set the parameters: cb and user
        wait_rpt = 1;
        //debug_zwapi_msg(&nw->plt_ctx, "ZWIF_OPT_WAIT_RPT_SET");
    }

    if (opt & ZWIF_OPT_WAIT_REPORT)
    {
        if (zwnode_get_rpt(cmd, len, &node->wait_cmd, &node->wait_rpt))
        {   //Caller's parameters: cb and user will be overwritten

            wait_rpt = 1;
            //debug_zwapi_msg(&nw->plt_ctx, "ZWIF_OPT_WAIT_REPORT");

            //Restart wait report timer in GET command transmit status callback
            cb = zwnode_wait_tx_cb;
            user = (void *)((unsigned)node->nodeid);
        }
    }

    if (((opt & ZWIF_OPT_SKIP_WK_BEAM) == 0)
        && (node->security & 0x60))//Check whether the node supports SENSOR_MODE_WAKEUP_1000 or SENSOR_MODE_WAKEUP_250
    {
        int q_cmd = 0;      //Flag to indicate whether to queue the command
        if (node->wait_rpt_flg)
        {   //Queue the command
            q_cmd = 1;
        }
        else
        {
            //No report is pending
            if (node->wait_cmd_q_cnt > 0)
            {
                //There is pending command in the queue, append the command
                q_cmd = 1;
                debug_zwapi_msg(&nw->plt_ctx, "Wait command queue not empty");
            }
        }

        if (q_cmd)
        {
            mul_cmd_q_ent_t  *mul_cmd;

            mul_cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + len);

            if (!mul_cmd)
            {
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_MEMORY;
            }

            mul_cmd->ifd = *ifd;
            if (opt & ZWIF_OPT_POLL)
            {
                mul_cmd->cmd_id = ZW_CID_POLL;
                mul_cmd->cmd_num = 1;
            }
            else
            {
                mul_cmd->cmd_id = node->cmd_id;
                mul_cmd->cmd_num = node->cmd_num;
            }
            mul_cmd->dat_len = len;
            memcpy(mul_cmd->dat_buf, cmd, len);

            //Check for extra parameter
            if ((opt & ZWIF_OPT_Q_EXTRA) && xtra)
            {
                mul_cmd->extra = xtra;
            }

            result = util_list_add_no_dup(nw->mtx, &node->wait_cmd_q_hd,
                                          (uint8_t *)mul_cmd, sizeof(mul_cmd_q_ent_t) + len,
                                          zwif_multi_cmd_cmp);

            if (result == 0)
            {
                node->wait_cmd_q_cnt++;
            }
            else if (result == 1)
            {   //Duplicate command
                if (mul_cmd->extra)
                {
                    zwif_cmd_q_xtra_rm(nw, &mul_cmd->extra);
                }
            }

            if ((opt & ZWIF_OPT_POLL) == 0)
            {
                node->cmd_num = 0;
            }

            //Submit request to wait thread for execution of queued commands
            result = util_list_add(nw->mtx, &nw->wait_q_req_hd,
                                   &node->nodeid, sizeof(uint8_t));
            if (result == 0)
            {
                result = ZW_ERR_QUEUED;
                plt_sem_post(nw->wait_q_sem);
            }
            else
            {
                result = ZW_ERR_MEMORY;
            }

            plt_mtx_ulck(nw->mtx);
            free(mul_cmd);
            return result;

        }

        //Check whether the command is GET command with REPORT as response
        if (zwnode_get_rpt(cmd, len, &node->wait_cmd, &node->wait_rpt))
        {
            wait_rpt = 1;

            //Restart wait report timer in GET command transmit status callback
            cb = (opt & ZWIF_OPT_POLL)? zwpoll_beam_tx_cb : zwnode_wait_tx_cb;
            user = (void *)((unsigned)node->nodeid);
        }
    }

    //-------------------------------------------------
    //Determine the maximum size of input
    //-------------------------------------------------
    if ((opt & ZWIF_OPT_SKIP_CHK) == 0)
    {
        if ((ifd->propty & IF_PROPTY_SECURE) == 0)
        {   //unsecure message
            if (node->crc_cap)
            {   //crc-16 message
                max_pkt_size -= CRC16_OVERHEAD;
            }
        }

        if (node->mul_cmd_ctl)
        {   //multi command message
            max_pkt_size -= 4;
        }

        if (ifd->epid > 0)
        {   //multi channel message
            max_pkt_size -= 4;
        }

        if (len > max_pkt_size)
        {
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_TOO_LARGE;
        }
    }

    //-------------------------------------------------
    // Multi-channel encapsulation
    //-------------------------------------------------

    //Determine whether the destination is an real endpoint
    if (((opt & ZWIF_OPT_SKIP_MUL_CH) == 0)
        && (ifd->epid > 0))
    {
        //Use multi-channel encapsulation command
        uint8_t  mul_ch_ver;

        //Get the multi-channel version
        mul_ch_ver = node->mul_ch_ver;

        multi_ch_cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_V2;
        if (mul_ch_ver == 1)
        {
            multi_ch_cmd[1] = MULTI_INSTANCE_CMD_ENCAP;
            multi_ch_cmd[2] = ifd->epid;
            memcpy(multi_ch_cmd + 3, cmd, len);

        }
        else //version 2
        {
            multi_ch_cmd[1] = MULTI_CHANNEL_CMD_ENCAP_V2;
            //
            // Work-around for Astral 2-gang dimmer
            //
            if ((node->vid == 0x40) && (node->vtype == 0x04) && (node->pid == 0x0102))
            {   //Use destination endpoint id as source endpoint id
                multi_ch_cmd[2] = ifd->epid;//src endpoint
            }
            else
            {
                multi_ch_cmd[2] = 0;//src endpoint
            }
            multi_ch_cmd[3] = ifd->epid;// destination endpoint
            memcpy(multi_ch_cmd + 4, cmd, len);
        }

        //Adjustment
        len += ((mul_ch_ver == 1)? 3 : 4);
        cmd = multi_ch_cmd;
    }

    //----------------------------------------------------------------
    // Command queuing (sleeping node) or Multi command encapsulation
    //----------------------------------------------------------------

    if (((opt & ZWIF_OPT_SKIP_MUL_Q) == 0)
        && (node->mul_cmd_ctl || node->enable_cmd_q))
    {
        int             process_cmd;    //Flag to indicate whether to continue processing the command
        mul_cmd_q_ent_t *wkup_beam_cmd = NULL;

        if ((opt & ZWIF_OPT_CMD_ENTRY) && xtra)
        {
            wkup_beam_cmd = (mul_cmd_q_ent_t *)xtra;
            process_cmd = wkup_beam_cmd->cmd_num;
        }
        else
        {
            process_cmd = (opt & ZWIF_OPT_POLL)? 1 : node->cmd_num;
        }

        if (process_cmd)
        {
            mul_cmd_q_ent_t  *mul_cmd;

            if (node->mul_cmd_ctl && !node->enable_cmd_q)
            {
                //Restart multi command timer
                plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
                node->mul_cmd_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNODE_MUL_CMD_TMOUT, zwnode_mul_cmd_tmout_cb, node);

            }

            mul_cmd = (mul_cmd_q_ent_t *)calloc(1, sizeof(mul_cmd_q_ent_t) + len);

            if (!mul_cmd)
            {
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_MEMORY;
            }

            mul_cmd->ifd = *ifd;
            mul_cmd->dat_len = len;
            memcpy(mul_cmd->dat_buf, cmd, len);

            //Check for command entry in xtra param
            if (wkup_beam_cmd)
            {
                mul_cmd->cmd_id = wkup_beam_cmd->cmd_id;
                mul_cmd->cmd_num = wkup_beam_cmd->cmd_num;
                mul_cmd->extra = wkup_beam_cmd->extra;

            }
            else
            {
                if (opt & ZWIF_OPT_POLL)
                {
                    mul_cmd->cmd_id = ZW_CID_POLL;
                    mul_cmd->cmd_num = 1;
                }
                else
                {
                    mul_cmd->cmd_id = node->cmd_id;
                    mul_cmd->cmd_num = node->cmd_num;
                }

                //Check for extra parameter
                if ((opt & ZWIF_OPT_Q_EXTRA) && xtra)
                {
                    mul_cmd->extra = xtra;
                }

                //Reset the command number
                if ((opt & ZWIF_OPT_POLL) == 0)
                {
                    node->cmd_num = 0;
                }
            }

            //Set wait report flag
            if (wait_rpt)
            {
                mul_cmd->wait_rpt = 1;
            }

            result = util_list_add_no_dup(nw->mtx, &node->mul_cmd_q_hd,
                                          (uint8_t *)mul_cmd, sizeof(mul_cmd_q_ent_t) + len,
                                          zwif_multi_cmd_cmp);

            if (result == 0)
            {
                result = ZW_ERR_QUEUED;
                node->mul_cmd_q_cnt++;

                if (wait_rpt)
                {
                    //Set wait report flag
                    node->wait_rpt_flg = 1;
                }
            }
            else if (result == 1)
            {   //Duplicate command
                result = ZW_ERR_QUEUED;
                if (mul_cmd->extra)
                {
                    zwif_cmd_q_xtra_rm(nw, &mul_cmd->extra);
                }
            }
            else
            {
                result = ZW_ERR_MEMORY;
            }

            plt_mtx_ulck(nw->mtx);
            free(mul_cmd);
            return result;
        }
    }

    //-------------------------------------------------
    //Release network mutex
    //-------------------------------------------------
    if (wait_rpt)
    {
        //Set wait report flag
        node->wait_rpt_flg = 1;
    }

#ifdef CRC16_ENCAP
    crc16_cap = node->crc_cap;
#endif

    plt_mtx_ulck(nw->mtx);


#ifdef  SUPPORT_SECURITY
    //-------------------------------------------------
    // Security message encapsulation
    //-------------------------------------------------
    if (nw->sec_enable && (ifd->propty & IF_PROPTY_SECURE))
    {
        //Send the command without queuing using security message encapsulation
        zwsec_tx_sm_param_t *tx_param;

        tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + len);
        if (tx_param)
        {
            tx_param->opp[0] = len;
            memcpy(tx_param->opp + 1, cmd, len);
            tx_param->cb = zwif_sec_tx_cb;
            tx_param->user[0] = cb;
            tx_param->user[1] = user;
            tx_param->use_0_nw_key = 0;
            tx_param->nodeid = ifd->nodeid;

            result = zwsec_send(ifd, tx_param);
            free(tx_param);
        }
        else
        {
            result = ZW_ERR_MEMORY;
        }

        if (wait_rpt)
        {
            if (result < 0)
            {   //Undo set node->wait_rpt_flg
                zwnode_wait_rpt_flg_clr(nw, ifd->nodeid);
            }
            else
            {
                result = ZW_ERR_QUEUED;
            }
        }

        return result;
    }
#endif


#ifdef CRC16_ENCAP
    //-------------------------------------------------
    // CRC-16 encapsulation
    // Note: CRC-16 encapsulation and security message encapsulation are mutually exclusive
    //-------------------------------------------------
    else if (crc16_cap)
    {
        uint16_t crc;

        //Encapsulate with CRC-16
        crc16_cmd[0] = COMMAND_CLASS_CRC_16_ENCAP;
        crc16_cmd[1] = CRC_16_ENCAP;
        memcpy(crc16_cmd + 2, cmd, len);
        crc = zwutl_crc16_chk(CRC_INIT, crc16_cmd, len + 2);
        crc16_cmd[len + 2] = (crc >> 8);
        crc16_cmd[len + 3] = (crc & 0x00ff);

        //Adjust command buffer pointer
        cmd = crc16_cmd;
        len += CRC16_OVERHEAD;
    }
#endif

    //-------------------------------------------------
    // Send the command through Z-wave
    //-------------------------------------------------
    prm = (appl_snd_data_t *)calloc(1, sizeof(appl_snd_data_t) + len);

    if (!prm)
    {
        if (wait_rpt)
        {   //Undo set node->wait_rpt_flg
            zwnode_wait_rpt_flg_clr(nw, ifd->nodeid);
        }
        return ZW_ERR_MEMORY;
    }

    prm->dat_len = len;
    memcpy(prm->dat_buf, cmd, len);
    prm->node_id = ifd->nodeid;
#ifdef  STRESS_TEST_NO_ROUTING
    prm->tx_opt = /*TRANSMIT_OPTION_LOW_POWER | */TRANSMIT_OPTION_ACK;
#else
    prm->tx_opt = /*TRANSMIT_OPTION_LOW_POWER | */TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE;
#endif
    /*printf("no security, no crc-16, call zw_send_data, prm->tx_opt= %02x\n",prm->tx_opt);
    printf("\n=========before send data, start, len:%d =================\n",len);
    int y = 0;
    for(y= 0;y<len;y++)
    {
        printf(" -%02x- ",prm->dat_buf[y]);
    }
    printf("\n===================before send data message end======================\n");*/

    result = zw_send_data(&nw->appl_ctx, prm, cb, user);


    free(prm);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwif_exec with error:%d", result);
        if (result == APPL_ERROR_WAIT_CB)
        {
            result = ZW_ERR_BUSY;
        }
        else
        {
            result = ZW_ERR_OP_FAILED;
        }

        //For node that requires wakeup beam
        if (wait_rpt)
        {   //Undo set node->wait_rpt_flg
            zwnode_wait_rpt_flg_clr(nw, ifd->nodeid);
        }
    }

    if ((result == 0) && wait_rpt)
    {
        result = ZW_ERR_QUEUED;
    }

    return result;
}


/**
zwif_exec - execute action on an interface
@param[in]	ifdesc      interface
@param[in]	cmd_buf		command and parameters
@param[in]	buf_len		length of cmd in bytes
@param[in]	cb		    callback function for transmit status
@return	ZW_ERR_XXX
*/
int zwif_exec(zwifd_p ifdesc, uint8_t *cmd_buf, int buf_len, tx_cmplt_cb_t cb)
{
    return zwif_exec_ex(ifdesc, cmd_buf, buf_len, cb, NULL, 0, NULL);
}


/**
zwif_get_report - get interface report through report callback
@param[in]	ifd	        interface
@param[in]	param	    Parameter for the report get command
@param[in]	len     	Length of param
@param[in]	get_rpt_cmd Command to get the report
@param[in]	cb		    callback function for transmit status
@return		ZW_ERR_xxx
*/
int zwif_get_report(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd, tx_cmplt_cb_t cb)
{
    int     result;
    int     hdr_len;
    uint8_t cmd[32];

    //Check if the parameter length
    if (len > 29)
    {
        return ZW_ERR_MEMORY;
    }

    //Check for extended command class (2-byte command class)
    if (ifd->cls & 0xFF00)
    {
        hdr_len = 3;
        cmd[0] = ifd->cls >> 8 ;
        cmd[1] = (ifd->cls & 0x00FF);
        cmd[2] = get_rpt_cmd;
    }
    else
    {
        hdr_len = 2;
        cmd[0] = (uint8_t)ifd->cls;
        cmd[1] = get_rpt_cmd;
    }

    if (len > 0)
    {
        memcpy(cmd + hdr_len, param, len);
    }

    result = zwif_exec(ifd, cmd, len + hdr_len, cb);

    if (result < 0)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwif_get_report with error:%d", result);
        return ZW_ERR_OP_FAILED;
    }
    return result;

}


/**
@}
*/















