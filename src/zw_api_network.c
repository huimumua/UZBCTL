/**
@file   zw_api_network.c - Z-wave High Level API network implementation.

@author David Chow

@version    1.0 12-7-10  Initial release

version: 1.0
comments: Initial release
*/


#include "../include/zw_api_pte.h"
#include "../include/zw_security.h"
#include "../include/zw_api_util.h"
#include "../include/zw_poll.h"
#include "../include/zwave/ZW_SerialAPI.h"

#define ZW_LIB_CONTROLLER_STATIC  0x01
#define ZW_LIB_CONTROLLER         0x02
#define ZW_LIB_SLAVE_ENHANCED     0x03
#define ZW_LIB_SLAVE              0x04
#define ZW_LIB_INSTALLER          0x05
#define ZW_LIB_SLAVE_ROUTING      0x06
#define ZW_LIB_CONTROLLER_BRIDGE  0x07
#define ZW_LIB_DUT                0x08

#define ZWNET_MSG_TYPE_SECURE     0x01
#define ZWNET_MSG_TYPE_CRC16      0x02

static int      zwnet_node_info_sm(zwnet_p   nw, zwnet_ni_evt_t  evt, uint8_t *data);
static int      zwnet_updt_sm(zwnet_p   nw, zwnet_nu_evt_t  evt, uint8_t *data);
static int      zwnet_rp_ni_sm(zwnet_p nw, zwnet_ni_evt_t evt, uint8_t *data);
static int      zwnet_ep_info_sm(zwnet_p nw, zwnet_ep_evt_t evt, uint8_t *data);
static int      zwnet_assign_ex(zwnet_p nw, uint8_t node_id, uint8_t role, tx_cmplt_cb_t cb);
static void     zwnet_ep_tmout_cb(void *data);
static int      zwnet_1_ep_info_sm(zwnet_p nw, zwnet_1_ep_evt_t evt, uint8_t *data);

#ifdef CRC16_ENCAP
    #define UNSECURE_CMD_CLS_CNT    3 //Number of command classes which are not supported in secure mode
#else
    #define UNSECURE_CMD_CLS_CNT    2 //Number of command classes which are not supported in secure mode
#endif
static const uint16_t    supp_cmd_class[] =
{
    COMMAND_CLASS_ZWAVEPLUS_INFO,
    COMMAND_CLASS_VERSION,
#ifdef  ZW_STRESS_TEST
    COMMAND_CLASS_MANUFACTURER_PROPRIETARY,//for stress test
#endif
#if defined (TEST_UTF8_NAME_LOC) || defined (TEST_UTF16_NAME_LOC)
    COMMAND_CLASS_NODE_NAMING,
#endif
#ifdef  TEST_EXT_CMD_CLASS
    COMMAND_CLASS_EXT_TEST,
#endif
#ifdef  TEST_SEC_WAKE_UP
    COMMAND_CLASS_WAKE_UP,
    COMMAND_CLASS_SWITCH_MULTILEVEL,
    COMMAND_CLASS_SWITCH_BINARY,
#endif
#ifdef  SUPPORT_SECURITY
    COMMAND_CLASS_SECURITY,
#endif
    COMMAND_CLASS_MANUFACTURER_SPECIFIC,
    COMMAND_CLASS_MULTI_CMD,
	COMMAND_CLASS_POWERLEVEL,
	COMMAND_CLASS_ASSOCIATION,
	COMMAND_CLASS_ASSOCIATION_GRP_INFO,
    //
    // Place command classes which are not supported in secure mode below.
    // Update the constant UNSECURE_CMD_CLS_CNT accordingly.
    //
    COMMAND_CLASS_DEVICE_RESET_LOCALLY
#ifdef CRC16_ENCAP
    ,COMMAND_CLASS_CRC_16_ENCAP
#endif
    ,COMMAND_CLASS_APPLICATION_STATUS

};

/**
@defgroup Net Network APIs
A Z-Wave Network is seen through a local controller node which can manage
control and data in the mesh network.
@{
*/

/**
zwnet_cmd_cls_show - show binary string of command classes
@param[in] nw		    Network
@param[in] cmd_cls_buf  Buffer that stores the command classes
@param[in] cnt          The number of command classes in cmd_cls
@return
*/
void zwnet_cmd_cls_show(zwnet_p nw, uint16_t *cmd_cls_buf, uint8_t cnt)
{
    uint8_t     tmp_cnt;
    char        tmp[8];
    char        hex_str[MAX_BIN_LINE_LEN * 8];

    while (cnt > 0)
    {
        //Split the binary string into multiple of MAX_BIN_LINE_LEN
        if (cnt >= MAX_BIN_LINE_LEN)
        {
            cnt -= MAX_BIN_LINE_LEN;
            tmp_cnt = MAX_BIN_LINE_LEN;
        }
        else
        {
            tmp_cnt = cnt;
            cnt = 0;
        }

        hex_str[0] = '\0';

        //Convert into hex string
        while (tmp_cnt-- > 0)
        {
#ifdef USE_SAFE_VERSION
            sprintf_s(tmp, 8, "%02X, ",(unsigned) *cmd_cls_buf++);
            strcat_s(hex_str, MAX_BIN_LINE_LEN * 8, tmp);
#else
            sprintf(tmp,"%02X, ",(unsigned) *cmd_cls_buf++);
            strcat(hex_str, tmp);
#endif
        }

        debug_zwapi_msg(&nw->plt_ctx, "%s", hex_str);
    }
}


/**
zwnet_node_is_valid - Check whether a node is in the protocol routing table
@param[in]	nw		    Network
@param[in]	node_id	    Node id
@param[out]	res	        Result: Non-zero means the node is in the table; otherwise, return zero
@return		Zero means the res is valid; otherwise ZW_ERR_xxx
*/
static int zwnet_node_is_valid(zwnet_p nw, uint8_t node_id, uint8_t *res)
{
    int32_t result;
    appl_init_dat_t init_dat;

    //Get the initialization data stored in non-volatile memory
    result = zw_serial_api_get_init_data(&nw->appl_ctx, &init_dat);

    if (result == 0)
    {
        int32_t i;

        i = node_id - 1;

        *res = (((init_dat.nodes[i>>3]) >> (i & 7)) & 0x01) ? 1 : 0;
        return ZW_ERR_NONE;

    }
    debug_zwapi_msg(&nw->plt_ctx, "zw_serial_api_get_init_data with error:%d", result);
    return ZW_ERR_NO_RESP;

}


/**
zwnet_cmd_cls_find - Find a command class in the given command class list.
@param[in]	cc_lst	Command class list
@param[in]	cc      Command class to be searched
@param[in]	cc_cnt	Number of command classes in cc_list
@return		Pointer to the found command class; else NULL.
*/
static uint16_t *zwnet_cmd_cls_find(const uint16_t *cc_lst, uint16_t cc, unsigned cc_cnt)
{
    while (cc_cnt-- > 0)
    {
        if (*cc_lst == cc)
        {
            return (uint16_t *)cc_lst;
        }
        cc_lst++;
    }

    return NULL;
}


/**
zwnet_cmd_cls_dedup - Remove duplicate command class in the command class list.
@param[in,out]	cmd_cls_lst	    Command class list
@param[in]	    cmd_cnt	        Number of command classes in command class list
@return		New number of command classes in the command class list
*/
static uint8_t zwnet_cmd_cls_dedup(uint16_t *cmd_cls_lst, uint8_t cmd_cnt)
{
    int         i;
    uint16_t *  new_cmd_cls_lst;
    uint8_t     new_cmd_cnt;

    if (cmd_cnt == 0)
    {
        return 0;
    }

    new_cmd_cls_lst = (uint16_t *)malloc(cmd_cnt * sizeof(uint16_t));

    if (!new_cmd_cls_lst)
    {
        return cmd_cnt;
    }

    new_cmd_cnt = 0;
    for (i=0; i<cmd_cnt; i++)
    {
        if (zwnet_cmd_cls_find(new_cmd_cls_lst, cmd_cls_lst[i], new_cmd_cnt) == NULL)
        {   //Add to the new list if the command class is not found in it
            new_cmd_cls_lst[new_cmd_cnt++] = cmd_cls_lst[i];
        }

    }

    //Copy back to the original buffer of the deduplicated command class list
    memcpy(cmd_cls_lst, new_cmd_cls_lst, new_cmd_cnt * sizeof(uint16_t));

    free(new_cmd_cls_lst);

    return new_cmd_cnt;

}


/**
zwnet_cmd_cls_add - Add a command class to the command class list.
@param[in]	new_cc      New command class to be added to the command class list
@param[in]	src_cc_lst	Source command class list
@param[in]	src_cc_cnt	Number of command classes in source command class list
@param[out]	dst_cc_lst	Destination command class list
@param[out]	dst_cc_cnt	Number of command classes in destination command class list
@return		ZW_ERR_xxx.
@post       The caller must free the allocated memory returned by dst_cc_lst if the new_cc is added. This can be
            checked by comparing dst_cc_lst and src_cc_lst.
*/
static int zwnet_cmd_cls_add(uint16_t new_cc, const uint16_t *src_cc_lst, uint8_t src_cc_cnt,
                             uint16_t **dst_cc_lst, uint8_t *dst_cc_cnt)
{
    //Check whether the command class list contains new_cc
    if ((src_cc_cnt > 0)
        && (zwnet_cmd_cls_find(src_cc_lst, new_cc, src_cc_cnt) == NULL))
    {
        //Add new_cc into the command class list
        *dst_cc_lst = (uint16_t *)malloc((src_cc_cnt + 1) * sizeof(uint16_t));

        if (!*dst_cc_lst)
        {
            return ZW_ERR_MEMORY;
        }

        //Copy old command class list to new command class list with added new_cc
        *dst_cc_lst[0] = new_cc;
        memcpy(*dst_cc_lst + 1, src_cc_lst, src_cc_cnt * sizeof(uint16_t));

        //Adjust the new command class length
        *dst_cc_cnt = src_cc_cnt + 1;

    }
    else
    {   //Found new_cc or no command classes info.
        //Return the original command class list
        *dst_cc_lst = (uint16_t *)src_cc_lst;
        *dst_cc_cnt = src_cc_cnt;
    }
    return ZW_ERR_NONE;

}


/**
zwnet_node_info_update - Update a node in the network structure.
@param[in]	nw		    Network
@param[in]	node_info   Node information such as node id, basic, generic and specific device classes and supported command classes
@return		ZW_ERR_xxx.
*/
static int zwnet_node_info_update(zwnet_p nw, appl_node_info_t *node_info)
{
    unsigned     i;
    int         node_has_changed;
    int         result;
    zwnode_p    node;
    zwif_p      intf;
    uint16_t    *new_cmd_cls;
    uint8_t     new_cmd_cnt;
    uint8_t     ni[8];
    uint8_t     sec_incl_failed = 0;


    result = zwnet_cmd_cls_add(COMMAND_CLASS_BASIC, node_info->cmd_cls, node_info->cmd_cnt,
                               &new_cmd_cls, &new_cmd_cnt);

    if (result < 0)
    {
        return result;
    }

    //Remove any duplicate command classes
    new_cmd_cnt = zwnet_cmd_cls_dedup(new_cmd_cls, new_cmd_cnt);

    node_has_changed = 0;
    plt_mtx_lck(nw->mtx);

    node = zwnode_find(&nw->ctl, node_info->node_id);
    if (node)
    {
        //Update node alive status
        node->alive = 1;

        //Existing node found, check whether there is any changes
        if (new_cmd_cnt == 0)
        {   //No device & command classes info to compare,
            //assume no changes
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_NONE;

        }

        //Check basic, generic, specific device classes
        if (node->basic != node_info->basic)
        {
            node_has_changed = 1;
        }

        if (node->ep.generic != node_info->gen)
        {
            node_has_changed = 1;
        }

        if (node->ep.specific != node_info->spec)
        {
            node_has_changed = 1;
        }

        if (!node_has_changed)
        {   //Check command classes
            for (i = 0; i < new_cmd_cnt; i++)
            {
                if (zwif_find_cls(node->ep.intf, new_cmd_cls[i]) == NULL)
                {
                    node_has_changed = 1;
                    break;
                }
            }
        }

        if (!node_has_changed)
        {
            if (i != zwif_count(node->ep.intf))
            {
                node_has_changed = 1;
            }
        }

        if (node_has_changed)
        {
            //Node security inclusion failed status should be saved and
            //used for the newly created node later.
            printf("============================>sec_incl_failed %d<======1===================\n",node->sec_incl_failed);
            sec_incl_failed = node->sec_incl_failed;

            //Remove the node
            zwnode_rm(nw, node_info->node_id);
        }
        else
        {   //Nothing has changed
            plt_mtx_ulck(nw->mtx);
            if (new_cmd_cls != node_info->cmd_cls)
            {
                free(new_cmd_cls);
            }
            return ZW_ERR_NONE;
        }
    }

    //Add a new node
    node = (zwnode_p)calloc(1, sizeof(zwnode_t));
    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        if (new_cmd_cls != node_info->cmd_cls)
        {
            free(new_cmd_cls);
        }
        return ZW_ERR_MEMORY;
    }

    node->alive = 1;

    printf("============================>sec_incl_failed %d<======2===================\n",sec_incl_failed);
    node->sec_incl_failed = sec_incl_failed;

    //Init all the back links
    node->net = nw;
    node->ep.node = node;

    //Save node id
    node->nodeid = node_info->node_id;

    zwobj_add(&nw->ctl.obj.next, &node->obj);

    //Assign default end point
    node->ep.epid = VIRTUAL_EP_ID;

    //Update basic, generic, specific device classes
    node->basic = node_info->basic;
    node->ep.generic = node_info->gen;
    node->ep.specific = node_info->spec;
    node->ep.intf = NULL;

    //Update node capabilitiy and security
    result = zw_get_node_protocol_info(&nw->appl_ctx, node->nodeid, ni);
    if (result == 0)
    {
        node->capability = ni[0];
        node->security = ni[1];
    }

    //Create unsecure interfaces based on command classes
    for (i = 0; i < new_cmd_cnt; i++)
    {
        intf = zwif_create(new_cmd_cls[i], 1, IF_PROPTY_UNSECURE);

        if (!intf)
        {
            plt_mtx_ulck(nw->mtx);
            if (new_cmd_cls != node_info->cmd_cls)
            {
                free(new_cmd_cls);
            }
            return ZW_ERR_MEMORY;
        }
        //Save back link to end point
        intf->ep = &node->ep;

        //Add interface to the end point
        if (node->ep.intf)
        {   //Second interface onwards
            zwobj_add(&node->ep.intf->obj.next, &intf->obj);
        }
        else
        {   //First interface
            node->ep.intf = intf;
        }
    }
    plt_mtx_ulck(nw->mtx);
    if (new_cmd_cls != node_info->cmd_cls)
    {
        free(new_cmd_cls);
    }
    return ZW_ERR_NONE;

}


/**
zwnet_ep_updt - Update or create end points with the given command class
@param[in]	ep	    The first end point
@param[in]	cmd_cls The command class
@param[in]	inst	The number of instances of a given command class
@param[in]	propty	Multi-instance command class properties
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static int zwnet_ep_updt(zwep_p  ep, uint8_t cmd_cls, uint8_t inst, uint8_t propty)
{
    zwep_p      new_ep;
    zwep_p      tmp_ep;
    zwif_p      intf;
    uint8_t     ver;
    uint8_t     i;

    if (inst == 0)
    {
        return 0;
    }

    tmp_ep = ep;

    //Get the command class version from the virtual end point
    intf = zwif_find_cls(ep->intf, cmd_cls);
    ver = (intf)? intf->ver : 1;

    //Create or update end points
    for (i=0; i < inst; i++)
    {
        if (!tmp_ep->obj.next)
        {   //Create an end point
            new_ep = (zwep_p)calloc(1, sizeof(zwep_t));
            if (!new_ep)
            {
                return ZW_ERR_MEMORY;
            }
            new_ep->generic = ep->generic;
            new_ep->specific = ep->specific;
            new_ep->node = ep->node;
            new_ep->epid = tmp_ep->epid + 1;//increment the end point id from previous end point
            //Add to the end point list
            tmp_ep->obj.next = &new_ep->obj;
            //
            //Create first interface of COMMAND_CLASS_BASIC
            //
            intf = zwif_create(COMMAND_CLASS_BASIC, 1, propty);

            if (!intf)
            {
                return ZW_ERR_MEMORY;
            }
            //Save back link to end point
            intf->ep = new_ep;

            //Add interface to the end point
            new_ep->intf = intf;
        }
        //Work on valid end point
        tmp_ep = (zwep_p)tmp_ep->obj.next;

        //Check whether the interface for the command class has already been created
        if (zwif_find_cls(tmp_ep->intf, cmd_cls) == NULL)
        {
            intf = zwif_create(cmd_cls, ver, propty);

            if (!intf)
            {
                return ZW_ERR_MEMORY;
            }
            //Save back link to end point
            intf->ep = tmp_ep;

            //Add interface to the end point
            if (tmp_ep->intf)
            {   //Second interface onwards
                zwobj_add(&tmp_ep->intf->obj.next, &intf->obj);
            }
            else
            {   //First interface
                tmp_ep->intf = intf;
            }

        }
    }

    return ZW_ERR_NONE;
}


/**
zwnet_ep_rm - Remove an end point
@param[in]	ep	        The end point
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static void zwnet_ep_rm(zwep_p  ep)
{
    zwep_p first_ep;    //The first end point in the list

    first_ep = &ep->node->ep;

    zwep_intf_rm_all(ep);
    if (ep != first_ep)
    {
        //Free the end point object
        zwobj_del(&first_ep->obj.next, &ep->obj);
    }
}


/**
zwnet_ep_add - Add an end point with the given capability
@param[in]	ep	        The first end point in the list
@param[in]	ep_cap	    The end point capability
@param[in]	propty      Multi-channel Command class properties
@return     0 on success, negative error number on failure
@pre        Caller must lock the nw->mtx before calling this function.
*/
static int zwnet_ep_add(zwep_p  ep, ep_cap_t *ep_cap, uint8_t propty)
{
    zwep_p      new_ep;
    zwep_p      old_ep;
    zwif_p      intf;
    int         result;
    int         node_has_changed;
    uint16_t    *new_cmd_cls;
    uint8_t     i;
    uint8_t     cls_ver;    //version of the command class
    uint8_t     new_cmd_cnt;


    //Check whether the end point has already been created
    old_ep = zwep_find(ep, ep_cap->ep);
    if (old_ep)
    {
        if ((old_ep->specific == ep_cap->spec)
            && (old_ep->generic == ep_cap->gen))
        {   //End point matches the capabilities

            //Check for command classes
            node_has_changed = 0;
            for (i = 0; i < ep_cap->cmd_cnt; i++)
            {
                if (zwif_find_cls(old_ep->intf, ep_cap->cmd_cls[i]) == NULL)
                {
                    node_has_changed = 1;
                    break;
                }
            }

            if (!node_has_changed)
            {
                return ZW_ERR_NONE;
            }

        }

        //Delete the obsolete end point
        zwnet_ep_rm(old_ep);

    }

    new_ep = (zwep_p)calloc(1, sizeof(zwep_t));
    if (!new_ep)
    {
        return ZW_ERR_MEMORY;
    }
    new_ep->generic = ep_cap->gen;
    new_ep->specific = ep_cap->spec;
    new_ep->node = ep->node;
    new_ep->epid = ep_cap->ep;

    //Add to the end point list
    zwobj_add(&ep->obj.next, &new_ep->obj);

    //Add COMMAND_CLASS_BASIC to the command class list if there is none in the list
    result = zwnet_cmd_cls_add(COMMAND_CLASS_BASIC, ep_cap->cmd_cls, ep_cap->cmd_cnt,
                               &new_cmd_cls, &new_cmd_cnt);

    if (result < 0)
    {
        return result;
    }

    //Remove any duplicate command classes
    new_cmd_cnt = zwnet_cmd_cls_dedup(new_cmd_cls, new_cmd_cnt);

    //Add command classes and interfaces
    for (i = 0; i < new_cmd_cnt; i++)
    {
        //The version number of the command class should be
        //inherited from the virtual end point
        intf = zwif_find_cls(ep->intf, new_cmd_cls[i]);
        cls_ver = (intf)? intf->ver : 1;

        intf = zwif_create(new_cmd_cls[i], cls_ver, propty);

        if (!intf)
        {
            if (new_cmd_cls != ep_cap->cmd_cls)
            {
                free(new_cmd_cls);
            }
            return ZW_ERR_MEMORY;
        }
        //Save back link to end point
        intf->ep = new_ep;

        //Add interface to the end point
        if (new_ep->intf)
        {   //Second interface onwards
            zwobj_add(&new_ep->intf->obj.next, &intf->obj);
        }
        else
        {   //First interface
            new_ep->intf = intf;
        }
    }

    if (new_cmd_cls != ep_cap->cmd_cls)
    {
        free(new_cmd_cls);
    }
    return ZW_ERR_NONE;
}


/**
zwnet_ni_sec_updt - Update secure command classes to interfaces.
@param[in]	ep		    Endpoint
@param[in]	sec_cls     Buffer that stores the security command classes
@param[in]  cnt         Length of buffer
@return		ZW_ERR_xxx.
@pre        Caller must lock the nw->mtx before calling this function.
*/
static int zwnet_ni_sec_updt(zwep_p ep, uint16_t *sec_cls, uint8_t cnt)
{
    unsigned    i;
    int         skip_sec_rm = 0; //Flag to indicate whether to skip remove secure property from interfaces
    zwif_p      intf;
    uint16_t    *new_sec_cls = NULL;

    if (!sec_cls)
    {
        return ZW_ERR_NONE;
    }

    //Check whether the secure list contains COMMAND_CLASS_BASIC
    if (zwnet_cmd_cls_find(sec_cls, COMMAND_CLASS_BASIC, cnt) == NULL)
    {
        //Add COMMAND_CLASS_BASIC into the secure list
        new_sec_cls = (uint16_t *)malloc((cnt + 1) * sizeof(uint16_t));

        if (!new_sec_cls)
        {
            return ZW_ERR_MEMORY;
        }

        //Copy old secure list to new secure list with added COMMAND_CLASS_BASIC
        new_sec_cls[0] = COMMAND_CLASS_BASIC;
        memcpy(new_sec_cls + 1, sec_cls, cnt * sizeof(uint16_t));

        //Adjust the input parameters
        sec_cls = new_sec_cls;
        cnt++;

    }

    //Check the existence of the interface which corresponds
    //to the command class in the secure list
    for (i = 0; i < cnt; i++)
    {
        intf = zwif_find_cls(ep->intf, sec_cls[i]);

        if (intf)
        {   //Interface has already existed, update its secure property
            intf->propty |= IF_PROPTY_SECURE;
        }
        else
        {   //Create interface for the secure command class
            intf = zwif_create(sec_cls[i], 1, IF_PROPTY_SECURE);

            if (!intf)
            {
                if (new_sec_cls)
                {
                    free(new_sec_cls);
                }
                return ZW_ERR_MEMORY;
            }

            //Save back link to end point
            intf->ep = ep;

            //Add interface to the end point
            if (ep->intf)
            {   //Second interface onwards
                zwobj_add(&ep->intf->obj.next, &intf->obj);
            }
            else
            {   //First interface
                ep->intf = intf;
            }
        }
    }

    //For real endpoint, the interface property depends on node level COMMAND_CLASS_MULTI_CHANNEL_V2 interface property
    if (ep->epid > 0)
    {
        intf = zwif_find_cls(ep->node->ep.intf, COMMAND_CLASS_MULTI_CHANNEL_V2);
        if (intf)
        {
            if ((intf->propty & (IF_PROPTY_SECURE | IF_PROPTY_UNSECURE)) == IF_PROPTY_SECURE)
            {
                //The COMMAND_CLASS_MULTI_CHANNEL_V2 interface property is secure only,
                //all the endpoint interfaces must be accessed through security encapsulation
                skip_sec_rm = 1;
            }
        }
    }


    //Make sure each existing secure interface to match the new secure command class
    if (!skip_sec_rm)
    {
        intf = ep->intf;
        while (intf)
        {
            if (intf->propty & IF_PROPTY_SECURE)
            {   //Secure interface, must find a match
                if (zwnet_cmd_cls_find(sec_cls, intf->cls, i) == NULL)
                {
                    //No match, change the interface to not supporting secure mode
                    //with the exception of basic command class.
                    if (intf->cls != COMMAND_CLASS_BASIC)
                    {
                        intf->propty &= ~IF_PROPTY_SECURE;
                    }
                }
            }
            intf = (zwif_p)intf->obj.next;
        }
    }

    if (new_sec_cls)
    {
        free(new_sec_cls);
    }
    return ZW_ERR_NONE;
}


/**
zwnet_ver_rpt_cb - Version report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the version is referred to
@param[in]	ver	    The version of cls
@return
*/
static void zwnet_ver_rpt_cb(zwif_p intf, uint16_t cls, uint8_t ver)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = cls >> 8;
    data[1] = cls & 0x00FF;
    data[2] = ver;
    memcpy(data + 4, &intf, sizeof(zwif_p));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_VER_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_zwver_rpt_cb - Z-wave library version report callback
@param[in]	intf	    The interface that received the report
@param[in]	proto_ver	Z-Wave Protocol Version
@param[in]	app_ver	    Application Version
@param[in]	lib_type	Z-Wave Library Type
@param[in]	ext_ver	    Extended version information if this parameter is not NULL
@return
*/
static void zwnet_zwver_rpt_cb(zwif_p intf, uint16_t proto_ver, uint16_t app_ver, uint8_t lib_type, ext_ver_t *ext_ver)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = proto_ver >> 8;
    data[1] = proto_ver & 0xFF;
    data[2] = app_ver >> 8;
    data[3] = app_ver & 0xFF;
    data[4] = lib_type;

    memcpy(data + 6, &intf, sizeof(zwif_p));
    memcpy(data + 6 + sizeof(zwif_p), &ext_ver, sizeof(ext_ver_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_ZWVER_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_mul_inst_rpt_cb - Multi instance report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the report is referred to
@param[in]	inst	The number of instances of a given command class
@return
*/
static void zwnet_mul_inst_rpt_cb(zwif_p intf, uint8_t cls, uint8_t inst)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = cls;
    data[1] = inst;
    memcpy(data + 2, &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_MULTI_INSTANCE_REP, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_mul_ch_ep_rpt_cb - Multi channel end point report callback
@param[in]	intf	        The interface that received the report
@param[in]	num_of_ep	    Number of end points embedded in the node. The maximum number of end points is 127.
@param[in]	flag	        Bit-7: 1 if the device has a dynamic number of end points.
                            Bit-6: 1 if all the end points in the node has the same generic and specific command class.
@return
*/
static void zwnet_mul_ch_ep_rpt_cb(zwif_p intf, uint8_t num_of_ep, uint8_t flag)
{
    zwnet_p     nw;
    uint8_t     data[16];

    data[0] = flag;
    data[1] = num_of_ep;
    memcpy(data + 2, &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_EP_NUM_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_cap_rpt_cb - Multi channel capabilitiy report callback
@param[in]	intf	    The interface that received the report
@param[in]	ep_cap	    The end point capability
@return
*/
static void zwnet_cap_rpt_cb(zwif_p intf, ep_cap_t *ep_cap)
{
    zwnet_p     nw;
    uint8_t     data[32];

    memcpy(data, &intf, sizeof(zwif_p));
    memcpy(data + sizeof(zwif_p), &ep_cap, sizeof(ep_cap_t *));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_EP_CAP_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_name_rpt_cb - Node naming report callback
@param[in]	intf	    The interface that received the report
@param[in]	name	    Node name
@param[in]	len	        Length of node name
@param[in]	char_pres   Char presentation
@return
*/
static void zwnet_name_rpt_cb(zwif_p intf, char *name, uint8_t len, uint8_t char_pres)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + ZW_LOC_STR_MAX];

    data[0] = char_pres;
    data[1] = len;
    memcpy(data + 2, name, len);
    memcpy(data + 2 + len, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_NAME_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_loc_rpt_cb - Location report callback
@param[in]	intf	    The interface that received the report
@param[in]	loc	        Location
@param[in]	len	        Length of location
@param[in]	char_pres   Char presentation
@return
*/
static void zwnet_loc_rpt_cb(zwif_p intf, char *loc, uint8_t len, uint8_t char_pres)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + ZW_LOC_STR_MAX];

    data[0] = char_pres;
    data[1] = len;
    memcpy(data + 2, loc, len);
    memcpy(data + 2 + len, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_LOC_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_manf_rpt_cb - manufacturer and product id report callback
@param[in]	intf	        The interface that received the report
@param[in]	manf_pdt_id	    Array that stores manufacturer, product type and product id
@return
*/
static void zwnet_manf_rpt_cb(zwif_p intf, uint16_t *manf_pdt_id)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[16 + 6];

    memcpy(data, manf_pdt_id, 6);
    memcpy(data + 6, &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(intf->ep->node->net, EVT_MANF_PDT_ID_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_dev_id_rpt_cb - device id report callback
@param[in]	intf	    The interface that received the report
@param[in]	dev_id	    Device ID
@return
*/
static void zwnet_dev_id_rpt_cb(zwif_p intf, dev_id_t *dev_id)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[32];

    memcpy(data, &dev_id, sizeof(dev_id_t *));
    memcpy(data + sizeof(dev_id_t *), &intf, sizeof(zwif_p));
    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_DEVICE_ID_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sec_rpt_cb - Supported security commands report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The buffer that store the command classes
@param[in]	cnt	    Command class count
@return
*/
static void zwnet_sec_rpt_cb(zwif_p intf, uint16_t *cls, uint8_t cnt)
{
    zwnet_p     nw;
    uint8_t     data[32];   //Note: for MAC OS X 64-bit, pointer is 8-byte long.

    data[0] = cnt;
    memcpy(data + 2, &cls, sizeof(uint16_t *));
    memcpy(data + 2 + sizeof(uint16_t *), &intf, sizeof(zwif_p));

    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_node_info_sm(nw, EVT_SEC_SUP_REPORT, data);
    zwnet_ep_info_sm(nw, EVT_EP_SEC_SUP_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_node_info_sm(nw, EVT_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_notify_cb - Initialization notify callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_notify_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;
    uint8_t   op;

    plt_mtx_lck(nw->mtx);
    //Stop the timer
    plt_tmr_stop(&nw->plt_ctx, nw->cb_tmr_ctx);
    nw->cb_tmr_ctx = NULL;

    //Reset operation to "no operation"
    op = nw->curr_op;
    nw->curr_op = ZWNET_OP_NONE;

    plt_mtx_ulck(nw->mtx);

    //Invoke notify callback
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, op, OP_DONE);
    }
}


/**
zwnet_notify - Notify user application of current operation status
@param[in]	nw	    Network
@param[in]	op		Network operation ZWNET_OP_XXX
@param[in]	ret		The status of current operation
@return
*/
static void zwnet_notify(zwnet_p nw, uint8_t op, uint16_t sts)
{
    if (nw->init.notify)
    {
        //Unlock network mutex before callback
        plt_mtx_ulck(nw->mtx);
        nw->init.notify(nw->init.user, op, sts);
        //Relock network mutex after callback
        plt_mtx_lck(nw->mtx);
    }
}


/**
zwnet_node_info_cb - Get node info delay timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_node_info_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_node_info_sm(nw, EVT_GET_NODE_INFO, NULL);
    plt_mtx_ulck(nw->mtx);


}


/**
zwnet_sm_next - Start querying information about the next node or
                call the notify callback.
@param[in]	nw		Network
@return
@pre        Caller must lock the nw->mtx before calling this function.
*/
static void    zwnet_sm_next(zwnet_p nw)
{
    int32_t result;

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;    //make sure timer context is null, else restart timer will crash

    //Point to next node
    nw->sm_job.rd_idx++;

    //Invoke notify callback to report the progress of every node
    zwnet_notify(nw, nw->sm_job.op,
                 (((uint16_t)nw->sm_job.num_node)<< 8) | (uint16_t)nw->sm_job.rd_idx);

    //Process the next node
    if (nw->sm_job.rd_idx < nw->sm_job.num_node)
    {
        result = zw_request_node_info(&nw->appl_ctx, nw->sm_job.node_id[nw->sm_job.rd_idx]);
        if (result == 0)
        {
            zwnode_p node;
            //Save the end point for time out event
            node = zwnode_find(&nw->ctl, nw->sm_job.node_id[nw->sm_job.rd_idx]);
            if (node)
            {
                zwep_get_desc(&node->ep, &nw->ni_sm_ep);
            }
            else
            {
                nw->ni_sm_ep.epid = 0;
                nw->ni_sm_ep.nodeid = nw->sm_job.node_id[0];
                nw->ni_sm_ep.net = nw;
            }

            nw->ni_sm_sta = ZWNET_STA_GET_NODE_INFO;

            //Start timer
            nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_NODE_INFO_TMOUT, zwnet_tmout_cb, nw);
            return;
        }
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_next: zw_request_node_info with error:%d", result);
    }

    //Completed job
    nw->ni_sm_sta = ZWNET_STA_IDLE;
    if (nw->sm_job.node_id)
    {
        free(nw->sm_job.node_id);
        nw->sm_job.node_id = NULL;
    }
    nw->sm_job.num_node = 0;

    //Reset operation to "no operation"
    nw->curr_op = ZWNET_OP_NONE;

    //Invoke user callback
    if (nw->sm_job.cb)
    {
        nw->sm_job.cb(nw);
    }

    //Invoke notify callback
    zwnet_notify(nw, nw->sm_job.op, OP_DONE);

}


/**
zwnet_sm_ver_get - Get version of each command class and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	first_cls   First command class to query
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ver_get(zwnet_p nw, zwif_p intf, uint16_t first_cls)
{
    int     result;
    zwifd_t ifd;
    uint8_t param;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the version of first command class
    param = (uint8_t)first_cls;
    result = zwif_get_report(&ifd, &param, 1, VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_ver_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Save the command class in version get
    nw->ni_sm_cls = first_cls;

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_GET_CMD_VER;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    return ZW_ERR_NONE;

}


/**
zwnet_info_rpt_cb - ZWave+ info report callback
@param[in]	intf	    The interface that received the report
@param[in]	info	    Z-Wave+ information
@return
*/
static void zwnet_info_rpt_cb(zwif_p intf, zwplus_info_t *info)
{
    zwnet_p     nw;
    uint8_t     data[32];

    memcpy(data, &intf, sizeof(zwif_p));
    memcpy(data + sizeof(zwif_p), &info, sizeof(zwplus_info_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_INFO_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_1_ep_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_1_ep_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_1_ep_info_sm(nw, EVT_S_EP_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_zwplus_info_get - Get ZWave+ info and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		Zwave+ info interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_zwplus_info_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_info_rpt_cb, ZWAVEPLUS_INFO_REPORT);
    if (result != 0)
    {
        return result;
    }

    result = zwif_get_report(&ifd, NULL, 0, ZWAVEPLUS_INFO_GET, zwif_tx_sts_cb);

    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_zwplus_info_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_ZWPLUS_INFO;
    return ZW_ERR_NONE;
}


/**
zwnet_max_group_rpt_cb - report callback for maximum number of supported groupings
@param[in]	ifd	      interface
@param[in]	max_grp   maximum number of groupings
*/
static void zwnet_max_group_rpt_cb(zwifd_p ifd,  uint8_t max_grp)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = max_grp;
        memcpy(data + 2, &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_CNT_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_max_group_get - Get group count and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_max_group_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_max_group_rpt_cb, ASSOCIATION_GROUPINGS_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             ASSOCIATION_GROUPINGS_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_max_group_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MAX_GROUP;

    return ZW_ERR_NONE;
}


/**
zwnet_max_usr_code_rpt_cb - report callback for maximum number of supported user codes
@param[in]	ifd	      interface
@param[in]	usr_num   number of supported user codes
*/
static void zwnet_max_usr_code_rpt_cb(zwifd_p ifd,  uint8_t usr_num)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = usr_num;
        memcpy(data + 2, &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_USR_CODE_CNT_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_max_usr_code_get - Get maximum user codes count and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_max_usr_code_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_max_usr_code_rpt_cb, USERS_NUMBER_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             USERS_NUMBER_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_max_usr_code_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MAX_USR_CODES;

    return ZW_ERR_NONE;
}


/**
zwnet_meter_sup_rpt_cb - report callback for meter capabilities
@param[in]	ifd	        interface
@param[in]	meter_cap	meter capabilities
*/
static void zwnet_meter_sup_rpt_cb(zwifd_p ifd, zwmeter_cap_p meter_cap)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        memcpy(data, &intf, sizeof(zwif_p));
        memcpy(data + sizeof(zwif_p), &meter_cap, sizeof(zwmeter_cap_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_METER, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_meter_get - Get meter capabilities and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_meter_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check version as this command is only valid for version 2 and above
    if (intf->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_meter_sup_rpt_cb, METER_SUPPORTED_REPORT_V2);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             METER_SUPPORTED_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_METER;

    return ZW_ERR_NONE;
}


/**
zwnet_meter_desc_rpt_cb - report callback for meter descriptor
@param[in]	ifd	        interface
@param[in]	meter	    meter descriptor
*/
static void zwnet_meter_desc_rpt_cb(zwifd_p ifd, zwmeter_p meter)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        memcpy(data, &intf, sizeof(zwif_p));
        memcpy(data + sizeof(zwif_p), &meter, sizeof(zwmeter_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_METER_TBL_DESC, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_meter_desc_get - Get meter descriptor and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_meter_desc_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_meter_desc_rpt_cb, METER_TBL_TABLE_ID_REPORT);

    if (result != 0)
    {
        return result;
    }

    result = zwif_set_report(&ifd, zwnet_meter_desc_rpt_cb, METER_TBL_TABLE_POINT_ADM_NO_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             METER_TBL_TABLE_ID_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_meter_desc_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_METER_DESC;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_fan_md_sup_rpt_cb - Report callback for supported thermostat fan operating modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat fan operating modes (ZW_THRMO_FAN_MD_XXX)
*/
static void zwnet_thrmo_fan_md_sup_rpt_cb(zwifd_p ifd, uint8_t off, uint8_t mode_len, uint8_t *mode)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = off;
        data[1] = mode_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        memcpy(data + 2 + sizeof(zwif_p), &mode, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_THRMO_FAN_MD, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_thrmo_fan_mode_sup_get - Get supported thermostat fan modes and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_fan_mode_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_thrmo_fan_md_sup_rpt_cb, THERMOSTAT_FAN_MODE_SUPPORTED_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_FAN_MODE_SUPPORTED_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_fan_mode_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_FAN_MD;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_md_sup_rpt_cb - Report callback for supported thermostat modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat modes (ZW_THRMO_MD_XXX)
*/
static void zwnet_thrmo_md_sup_rpt_cb(zwifd_p ifd, uint8_t mode_len, uint8_t *mode)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = mode_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        memcpy(data + 2 + sizeof(zwif_p), &mode, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_THRMO_MD, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_thrmo_mode_sup_get - Get supported thermostat modes and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_mode_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_thrmo_md_sup_rpt_cb, THERMOSTAT_MODE_SUPPORTED_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_MODE_SUPPORTED_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_mode_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_MD;

    return ZW_ERR_NONE;
}


/**
zwnet_thrmo_setp_sup_rpt_cb - Report callback for supported thermostat setpoints
@param[in]	ifd	        interface
@param[in]	type_len    size of type buffer
@param[in]	type        buffer to store supported thermostat setpoint types (ZW_THRMO_SETP_TYP_XXX)
*/
static void zwnet_thrmo_setp_sup_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = type_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        memcpy(data + 2 + sizeof(zwif_p), &type, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_THRMO_SETP, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_thrmo_setp_sup_get - Get supported thermostat setpoint types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_thrmo_setp_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_thrmo_setp_sup_rpt_cb, THERMOSTAT_SETPOINT_SUPPORTED_REPORT);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             THERMOSTAT_SETPOINT_SUPPORTED_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_thrmo_setp_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_THRMO_SETP;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_lvl_sup_rpt_cb - multi level switch type callback
@param[in]	ifd	interface
@param[in]	pri_type    primary switch type, SW_TYPE_XX
@param[in]	sec_type    secondary switch type , SW_TYPE_XX.
@return
*/
static void zwnet_sm_lvl_sup_rpt_cb(zwifd_p ifd, uint8_t pri_type, uint8_t sec_type)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = pri_type;
        data[1] = sec_type;
        memcpy(data + 2, &intf, sizeof(zwif_p));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_MUL_SWITCH, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_lvl_sup_get - Get supported switch types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_lvl_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_sm_lvl_sup_rpt_cb, SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             SWITCH_MULTILEVEL_SUPPORTED_GET_V3, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_lvl_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_MUL_SWITCH;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_alrm_sup_rpt_cb - Report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	have_vtype  flag to indicate whether vendor specific alarm type supported. 1=supported; else 0=unsupported
@param[in]	ztype_len   size of ztype buffer
@param[in]	ztype       buffer to store supported Z-wave alarm types (ZW_ALRM_XXX)
*/
static void zwnet_sm_alrm_sup_rpt_cb(zwifd_p ifd, uint8_t have_vtype, uint8_t ztype_len, uint8_t *ztype)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        data[0] = have_vtype;
        data[1] = ztype_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        memcpy(data + 2 + sizeof(zwif_p), &ztype, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_ALARM_TYPE, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_alrm_sup_get - Get supported switch types and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_alrm_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    //Check version as this command is only valid for version 2 and above
    if (intf->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_sm_alrm_sup_rpt_cb, ALARM_TYPE_SUPPORTED_REPORT_V2);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             ALARM_TYPE_SUPPORTED_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_ALARM;

    //Change sub-state
    nw->ni_sm_sub_sta = ALARM_SUBSTA_TYPE;

    return ZW_ERR_NONE;
}



/**
zwnet_alrm_evt_rpt_cb - alarm supported events report callback
@param[in]	ifd	        interface
@param[in]	ztype       Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	evt_len     size of evt buffer
@param[in]	evt         buffer to store supported event bit-masks of the ztype
@return
*/
void zwnet_alrm_evt_rpt_cb(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt_msk)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = ztype;
        data[1] = evt_len;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        memcpy(data + 2 + sizeof(zwif_p), &evt_msk, sizeof(uint8_t *));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_ALARM_EVENT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_alrm_evt_get -  Get the supported alarm events
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more supported events to get; negative error number on failure
*/
static int zwnet_sm_alrm_evt_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_alarm_data_t     *alarm_dat = (if_alarm_data_t *)intf->data;

    if (nw->alrm_rd_idx >= alarm_dat->type_evt_cnt)
    {   //No more supported events to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_alrm_evt_rpt_cb, EVENT_SUPPORTED_REPORT_V3);
    if (result != 0)
    {
        return result;
    }

    //Get the supported events
    result = zwif_get_report(&ifd, &alarm_dat->type_evt[nw->alrm_rd_idx].ztype, 1,
                             EVENT_SUPPORTED_GET_V3, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_alrm_evt_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_prot_sup_rpt_cb - Report callback for supported protection states
@param[in]	ifd	        interface
@param[in]	sup_sta     supported Protection States
*/
static void zwnet_sm_prot_sup_rpt_cb(zwifd_p ifd, zwprot_sup_p sup_sta)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        memcpy(data, &intf, sizeof(zwif_p));
        memcpy(data + sizeof(zwif_p), &sup_sta, sizeof(zwprot_sup_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_PROT, data);
    }
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_prot_sup_get - Get supported protection states and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_prot_sup_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_sm_prot_sup_rpt_cb, PROTECTION_SUPPORTED_REPORT_V2);

    if (result != 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(&ifd, NULL, 0,
                             PROTECTION_SUPPORTED_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_prot_sup_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_PROT;

    return ZW_ERR_NONE;
}


/**
zwnet_add_ctlr_grp - Add controller to a group
@param[in]	nw		    Network
@param[in]	first_intf	First interface of an endpoint
@param[in]	alt_intf	First interface of an alternate endpoint (optional; if not used, it should be NULL)
@param[in]	grp_id		Group id
@return  0 on success, negative error number on failure
*/
static int zwnet_add_ctlr_grp(zwnet_p nw, zwif_p first_intf, zwif_p alt_intf, uint8_t grp_id)
{
    int         mul_ch_assoc_present = 0;   //Flag to indicate whether Multi Channel Association CC presents.
    zwif_p      intf;
    uint8_t     cmd[8];
    zwifd_t     ifd;

    //Find multi-channel association interface first
    intf = zwif_find_cls(first_intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);

    if (intf)
    {
        mul_ch_assoc_present = 1;
    }
    else if ((intf = zwif_find_cls(first_intf, COMMAND_CLASS_ASSOCIATION)) != NULL)
    {
        //Found association command class
    }
    //Try alternate interface
    else if ((intf = zwif_find_cls(alt_intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2)) != NULL)
    {
        mul_ch_assoc_present = 1;
    }
    else if ((intf = zwif_find_cls(alt_intf, COMMAND_CLASS_ASSOCIATION)) != NULL)
    {
    }
    else
    {
        //Not found
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if (mul_ch_assoc_present)
    {
        //
        //All Controller Role types (CSC, SSC, PC, RPC) MUST use the Multi Channel Association CC with
        //Endpoint ID = 1 asserted when performing the association sets. This MUST be done regardless of
        //whether the Controller is a Multi Channel device or not.
        //

        //Prepare the command
        cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2;
        cmd[1] = MULTI_CHANNEL_ASSOCIATION_SET_V2;
        cmd[2] = grp_id;
        cmd[3] = MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2;
        cmd[4] = nw->ctl.nodeid;
        cmd[5] = 1; //endpoint

    }
    else
    {
        //Prepare the command
        cmd[0] = COMMAND_CLASS_ASSOCIATION;
        cmd[1] = ASSOCIATION_SET;
        cmd[2] = grp_id;
        cmd[3] = nw->ctl.nodeid;
    }

    //Send the command
    zwif_get_desc(intf, &ifd);
    return zwif_exec(&ifd, cmd, (mul_ch_assoc_present)? 6 : 4, zwif_tx_sts_cb);
}


/**
zwnet_sm_lifeline_cfg - Configure lifeline
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_lifeline_cfg(zwnet_p nw, zwif_p intf)
{
    int     result;

    result = zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, 1);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_lifeline_cfg: set assoc with error:%d", result);
        return result;
    }

    return ZW_ERR_NONE;
}



/**
zwnet_grp_cmd_lst_rpt_cb - group command list report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	cmd_ent_cnt	    Number of entries in the command list
@param[in]	cmd_lst	        Command list
@return
*/
static void zwnet_grp_cmd_lst_rpt_cb(zwif_p intf, uint8_t grp_id, uint8_t cmd_ent_cnt, grp_cmd_ent_t *cmd_lst)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_id;
    data[1] = cmd_ent_cnt;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &cmd_lst, sizeof(grp_cmd_ent_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_CMD_LST_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_cmd_lst_get -  Get the command lists in the group
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more command list to get; negative error number on failure
*/
static int zwnet_sm_grp_cmd_lst_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;
    uint8_t             param[2];


    if (nw->grp_rd_idx >= grp_data->group_cnt)
    {   //No more command list to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_grp_cmd_lst_rpt_cb, ASSOCIATION_GROUP_COMMAND_LIST_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the command list
    param[0] = 0x80;
    param[1] = ++nw->grp_rd_idx;

    result = zwif_get_report(&ifd, param, 2,
                             ASSOCIATION_GROUP_COMMAND_LIST_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_cmd_lst_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_grp_info_start -  Start get command lists in the first group
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, otherwise negative error number on failure
*/
static int zwnet_sm_grp_info_start(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwif_p              assoc_intf;
    if_grp_info_dat_t   *grp_data;
    uint8_t             grp_cnt;


    //Get maximum supported groupings
    assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_ASSOCIATION);

    if (!assoc_intf)
    {
        assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);
    }

    if (!assoc_intf)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: can't find grouping interface");
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if (assoc_intf->data_cnt == 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: maximum supporting groups is 0");
        return ZW_ERR_VALUE;
    }

    //Get group information interface
    grp_cnt = *((uint8_t *)assoc_intf->data);

    if (grp_cnt == 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_start: can't get maximum supporting groups");
        return ZW_ERR_INTF_NO_DATA;
    }

    grp_data = (if_grp_info_dat_t *)calloc(1, sizeof(if_grp_info_dat_t) + (sizeof(zw_grp_info_p) * grp_cnt));

    if (grp_data)
    {
        if (intf->tmp_data)
        {
            //Free interface specific data
            zwif_dat_rm(intf);
        }

        intf->tmp_data = grp_data;

        grp_data->group_cnt = grp_cnt;

        //Get command list for group #1
        nw->grp_rd_idx = 0;
        result = zwnet_sm_grp_cmd_lst_get(nw, intf);
        if (result == 0)
        {
            //Change state
            nw->single_ep_sta = S_EP_STA_GET_GROUP_INFO;

            //Change sub-state
            nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_CMD_LST;
        }
        return result;
    }

    return ZW_ERR_MEMORY;
}


/**
zwnet_grp_name_rpt_cb - group name report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	name	        Group name
@param[in]	name_len        String length of group name
@return
*/
static void zwnet_grp_name_rpt_cb(zwif_p intf, uint8_t grp_id, uint8_t name_len, uint8_t *name)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_id;
    data[1] = name_len;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &name, sizeof(uint8_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_NAME_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_name_get -  Get the group name
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more group name to get; negative error number on failure
*/
static int zwnet_sm_grp_name_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;
    uint8_t             param;


    if (nw->grp_rd_idx >= grp_data->group_cnt)
    {   //No more group name to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_grp_name_rpt_cb, ASSOCIATION_GROUP_NAME_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the group name
    param = ++nw->grp_rd_idx;

    result = zwif_get_report(&ifd, &param, 1,
                             ASSOCIATION_GROUP_NAME_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_name_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_grp_info_rpt_cb - group info report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_cnt	        Group count
@param[in]	dynamic	        Flag to indicate dynamic info. 1=dynamic; 0=static
@param[in]	grp_info        Group information
@return
*/
static void zwnet_grp_info_rpt_cb(zwif_p intf, uint8_t grp_cnt, uint8_t dynamic, zw_grp_info_ent_t *grp_info)
{
    zwnet_p     nw;
    uint8_t     data[32];

    data[0] = grp_cnt;
    data[1] = dynamic;
    memcpy(data + 2, &intf, sizeof(zwif_p));
    memcpy(data + 2 + sizeof(zwif_p), &grp_info, sizeof(zw_grp_info_ent_t *));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_1_ep_info_sm(nw, EVT_S_EP_GRP_INFO_REPORT, data);
    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_grp_info_get -  Get the group information
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success; negative error number on failure
*/
static int zwnet_sm_grp_info_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    uint8_t             param[2];


    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_grp_info_rpt_cb, ASSOCIATION_GROUP_INFO_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the group information
    param[0] = 0x40;    //enable list mode
    param[1] = 1;       //group id (ignored when list mode is enabled)

    result = zwif_get_report(&ifd, param, 2,
                             ASSOCIATION_GROUP_INFO_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_grp_info_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_name_get - Get the name of the node and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_name_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_name_rpt_cb, NODE_NAMING_NODE_NAME_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the node name
    result = zwif_get_report(&ifd, NULL, 0, NODE_NAMING_NODE_NAME_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_name_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_NODE_NAMELOC;

    return ZW_ERR_NONE;

}


/**
zwnet_sm_manf_get - Get the manufacturer and product id of the node and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_manf_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_manf_rpt_cb, MANUFACTURER_SPECIFIC_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the manufacturer id, product type id and product id
    result = zwif_get_report(&ifd, NULL, 0, MANUFACTURER_SPECIFIC_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_manf_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_GET_MANF_PDT_ID;

    //Change sub-state
    nw->ni_sm_sub_sta = MS_SUBSTA_PDT_ID;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_sm_dev_id_get - Get device id
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	dev_id_type Device id type: DEV_ID_TYPE_XXX
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_dev_id_get(zwnet_p nw, zwif_p intf, uint8_t dev_id_type)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_dev_id_rpt_cb, DEVICE_SPECIFIC_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the device id
    result = zwif_get_report(&ifd, &dev_id_type, 1, DEVICE_SPECIFIC_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_dev_id_get: zwif_get_report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    //Change sub-state
    nw->ni_sm_sub_sta = MS_SUBSTA_DEV_ID_OEM;

    return ZW_ERR_NONE;

}


/**
zwnet_sensor_rpt_cb - sensor reading report callback
@param[in]	ifd	        interface
@param[in]	value		The current value and unit of the sensor.
@return
*/
static void zwnet_sensor_rpt_cb(zwifd_p ifd, zwsensor_t *value)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[16 + 8];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = value->type;
        data[1] = value->unit;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_SENSOR_INFO_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sensor_type_rpt_cb - sensor supported types report callback
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_SENSOR_TYPE_XXX)
@return
*/
static void zwnet_sensor_type_rpt_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = type_len;
        memcpy(data + 1, &type, sizeof(uint8_t *));
        memcpy(data + 1 + sizeof(uint8_t *), &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_SENSOR_TYPE_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_sensor_get - Get the supported sensors and units and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_sensor_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Check version 5 supported
    if (intf->ver < 5)
    {
        //Use sensor multi-level get command

        //Setup report callback
        result = zwif_set_report(&ifd, zwnet_sensor_rpt_cb, SENSOR_MULTILEVEL_REPORT);
        if (result != 0)
        {
            return result;
        }

        //Get the sensor reading
        result = zwif_get_report(&ifd, NULL, 0, SENSOR_MULTILEVEL_GET, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_get: get report with error:%d", result);
            return result;
        }
    }
    else
    {   //Get supported sensor types

        //Setup report callback
        result = zwif_set_report(&ifd, zwnet_sensor_type_rpt_cb, SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5);
        if (result != 0)
        {
            return result;
        }

        //Get supported sensor types report
        result = zwif_get_report(&ifd, NULL, 0, SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5, zwif_tx_sts_cb);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_get: get report with error:%d", result);
            return result;
        }

        //Change sub-state
        nw->ni_sm_sub_sta = SENSOR_SUBSTA_TYPE;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_SENSOR;

    return ZW_ERR_NONE;
}


/**
zwnet_sensor_unit_rpt_cb - sensor supported units report callback
@param[in]	ifd	            interface
@param[in]	sensor_type     sensor type, ZW_SENSOR_TYPE_XXX
@param[in]	sensor_unit_msk bitmask of units supported for the sensor_type, ZW_SENSOR_UNIT_XXX
@return
*/
static void zwnet_sensor_unit_rpt_cb(zwifd_p ifd, uint8_t sensor_type, uint8_t sensor_unit_msk)
{
    zwnet_p     nw = ifd->net;
    zwif_p      intf;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf)
    {
        data[0] = sensor_type;
        data[1] = sensor_unit_msk;
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_SENSOR_UNIT_REPORT, data);
   }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_sensor_unit_get -  Get the supported sensor units
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, 1 on no more supported units to get; negative error number on failure
*/
static int zwnet_sm_sensor_unit_get(zwnet_p nw, zwif_p intf)
{
    int                 result;
    zwifd_t             ifd;
    if_sensor_data_t    *sensor_dat = (if_sensor_data_t *)intf->data;

    if (nw->snsr_rd_idx >= intf->data_cnt)
    {   //No more supported units to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_sensor_unit_rpt_cb, SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5);
    if (result != 0)
    {
        return result;
    }

    //Get the supported units
    result = zwif_get_report(&ifd, &sensor_dat[nw->snsr_rd_idx].sensor_type, 1,
                             SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_sensor_unit_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;

}


/**
zwnet_av_raw_rpt_cb - simple AV raw report callback
@param[in]	intf	        The interface that received the report
@param[in]	cmd_buf	        Command buffer
@param[in]	cmd_len         Length of command buffer
@return
*/
void zwnet_av_raw_rpt_cb(zwif_p intf, uint8_t cmd_len, uint8_t *cmd_buf)
{
    zwnet_p     nw = intf->ep->node->net;
    uint8_t     data[32];

    plt_mtx_lck(nw->mtx);
    if (cmd_buf[1] == SIMPLE_AV_CONTROL_REPORT)
    {
        data[0] = cmd_buf[2];//total number of reports
        memcpy(data + 2, &intf, sizeof(zwif_p));
        zwnet_1_ep_info_sm(nw, EVT_S_EP_AV_REPORT_CNT, data);
    }
    else if (cmd_buf[1] == SIMPLE_AV_CONTROL_SUPPORTED_REPORT)
    {
        data[0] = cmd_buf[2];//report number
        data[1] = cmd_len - 3;//bit mask buffer length
        memcpy(data + 2, &intf, sizeof(zwif_p));
        cmd_buf += 3;
        memcpy(data + 2 + sizeof(zwif_p), &cmd_buf, sizeof(uint8_t *));

        zwnet_1_ep_info_sm(nw, EVT_S_EP_AV_CTL_REPORT, data);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_sm_av_get - Get the supported simple AV control and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_av_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_av_raw_rpt_cb, SIMPLE_AV_CONTROL_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get supported simple AV control report count
    result = zwif_get_report(&ifd, NULL, 0, SIMPLE_AV_CONTROL_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_av_get: get report with error:%d", result);
        return result;
    }

    //Change sub-state
    nw->ni_sm_sub_sta = AV_SUBSTA_RPT_CNT;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    nw->single_ep_sta = S_EP_STA_GET_AV;

    return ZW_ERR_NONE;
}


/**
zwnet_sm_av_ctl_get -  Get the supported simple AV control report
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	av_cap		AV capabilities
@return  0 on success, 1 on no more report to get; negative error number on failure
*/
static int zwnet_sm_av_ctl_get(zwnet_p nw, zwif_p intf, zwif_av_cap_t *av_cap)
{
    int     result;
    zwifd_t ifd;
    uint8_t rpt_num;

    if (av_cap->rpt_num == av_cap->total_rpt)
    {   //No more report to get
        return 1;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_av_raw_rpt_cb, SIMPLE_AV_CONTROL_SUPPORTED_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the next report
    rpt_num = av_cap->rpt_num + 1;
    result = zwif_get_report(&ifd, &rpt_num, 1,
                             SIMPLE_AV_CONTROL_SUPPORTED_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_av_ctl_get: get report with error:%d", result);
        return result;
    }

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_sm_inst_get - Get the number of instances of a given command class
                    and update the state-machine's state
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	cmd_cls		The command class to query for number of supported instances
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_inst_get(zwnet_p nw, zwif_p intf, uint16_t cmd_cls)
{
    int     result;
    zwifd_t ifd;
    uint8_t param;

    //Check for extended command class (2-byte command class)
    if (cmd_cls & 0xFF00)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_inst_get: extended command class UNSUPPORTED");
        return ZW_ERR_FAILED;
    }

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_mul_inst_rpt_cb, MULTI_INSTANCE_REPORT);
    if (result != 0)
    {
        return result;
    }

    //Get the number of instances of first command class
    param = (uint8_t)cmd_cls;
    result = zwif_get_report(&ifd, &param, 1, MULTI_INSTANCE_GET, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_inst_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_MULTI_INSTANCE;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_sm_ch_get - Get the number of end points in a node
@param[in]	nw		    Network
@param[in]	intf		interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ch_get(zwnet_p nw, zwif_p intf)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_mul_ch_ep_rpt_cb, MULTI_CHANNEL_END_POINT_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the number of end points in a node
    result = zwif_get_report(&ifd, NULL, 0, MULTI_CHANNEL_END_POINT_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_ch_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_MULTI_CHANNEL;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_sm_cap_get - Get the capability of an end point
@param[in]	nw		    Network
@param[in]	intf		interface
@param[in]	ep_id		end point id of the interface
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_cap_get(zwnet_p nw, zwif_p intf, uint8_t ep_id)
{
    int     result;
    zwifd_t ifd;

    zwif_get_desc(intf, &ifd);

    //Setup report callback
    result = zwif_set_report(&ifd, zwnet_cap_rpt_cb, MULTI_CHANNEL_CAPABILITY_REPORT_V2);
    if (result != 0)
    {
        return result;
    }

    //Get the number of end points in a node
    result = zwif_get_report(&ifd, &ep_id, 1, MULTI_CHANNEL_CAPABILITY_GET_V2, zwif_tx_sts_cb);
    if (result < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_cap_get: zwif_get_report with error:%d", result);
        return result;
    }
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return ZW_ERR_NONE;
}


/**
zwnet_sm_ep_info_cb - Get endpoint info callback
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static void zwnet_sm_ep_info_cb(zwnet_p nw)
{
    zwnet_node_info_sm(nw, EVT_EP_INFO_DONE, NULL);
}


/**
zwnet_sm_ep_info_get - Get endpoint info by invoking the endpoint info state-machine
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_ep_info_get(zwnet_p nw, zwep_p ep)
{
    int         state;
    ep_sm_job_t sm_job;

    if (!ep)
    {
        return ZW_ERR_EP_NOT_FOUND;
    }

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;

    //Initialize endpoint info state-machine
    nw->ep_sm_sta = EP_STA_IDLE;

    sm_job.cb = zwnet_sm_ep_info_cb;
    sm_job.first_ep = ep;

    state = zwnet_ep_info_sm(nw, EVT_EP_SM_START, (uint8_t *)&sm_job);

    if (state == EP_STA_IDLE)
    {
        return ZW_ERR_FAILED;
    }

    //Change state
    nw->ni_sm_sta = ZWNET_STA_GET_EP_INFO;

    return 0;
}


/**
zwnet_wkup_rpt_cb - wake up capabilities report callback
@param[in]	ifd	    interface
@param[in]	cap		capabilities report, null for notification
@return	    Only apply to notification: 0=no command pending to send; 1=commands pending to send.
*/
static int zwnet_wkup_rpt_cb(zwifd_p ifd, zwif_wakeup_p cap)
{
    zwnet_p     nw;
    zwif_p      intf;
    uint8_t     data[32];   //Note: for MAC OS X 64-bit, pointer is 8-byte long.

    nw = ifd->net;

    plt_mtx_lck(nw->mtx);

    intf = zwif_get_if(ifd);

    if (!intf)
    {
        plt_mtx_ulck(nw->mtx);
        return 0;
    }

    memcpy(data, &cap, sizeof(zwif_wakeup_p));
    memcpy(data + sizeof(zwif_wakeup_p), &intf, sizeof(zwif_p));

    zwnet_node_info_sm(nw, EVT_WKUP_CAP_REP, data);
    plt_mtx_ulck(nw->mtx);
    return 0;
}


/**
zwnet_dev_rec_find - Find device specific configuration record
@param[in]	nw		    Network
@param[in]	vid		    Vendor ID
@param[in]	ptype		Product Type ID
@param[in]	pid		    Product ID
@param[out]	dev_rec     Device record
@return  Non-zero on found; zero on not found
*/
static int zwnet_dev_rec_find(zwnet_p nw, uint16_t vid, uint16_t ptype, uint16_t pid, dev_rec_t *dev_rec)
{
    int i;
    int j;

    //Check whether use user supplied search function
    if (nw->dev_rec_find_fn)
    {
        return nw->dev_rec_find_fn(nw->dev_cfg_ctx, vid, ptype, pid, dev_rec);
    }

    for (j=0; j<4; j++)
    {
        if (nw->dev_cfg_bufs[j].dev_cfg_cnt)
        {
            for (i=0; i<nw->dev_cfg_bufs[j].dev_cfg_cnt; i++)
            {
                if ((nw->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_VID)
                    && (nw->dev_cfg_bufs[j].dev_cfg_buf[i].vid != vid))
                {
                    continue;
                }

                if ((nw->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_PTYPE)
                    && (nw->dev_cfg_bufs[j].dev_cfg_buf[i].ptype != ptype))
                {
                    continue;
                }

                if ((nw->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_PID)
                    && (nw->dev_cfg_bufs[j].dev_cfg_buf[i].pid != pid))
                {
                    continue;
                }

                //Found a match
                *dev_rec = nw->dev_cfg_bufs[j].dev_cfg_buf[i];
                return 1;
            }
        }
    }
    //No match found
    return 0;
}


/**
zwnet_sm_wkup_cfg - Get device's wakeup interval and notification node
@param[in]	nw		    Network
@param[in]	wkup_intf	Wake up interface
@return  0 on successfully sending WAKE_UP_INTERVAL_GET;
         negative error number on failure
*/
static int zwnet_sm_wkup_cfg(zwnet_p nw, zwif_p wkup_intf)
{
    int         result;
    zwifd_t     ifd;

    zwif_get_desc(wkup_intf, &ifd);

    result = zwif_set_report(&ifd, zwnet_wkup_rpt_cb, WAKE_UP_INTERVAL_REPORT);
    if (result != 0)
    {
        return result;
    }

    if (wkup_intf->ver >= 2)
    {
        result = zwif_set_report(&ifd, zwnet_wkup_rpt_cb, WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2);
        if (result != 0)
        {
            return result;
        }
    }

    //Get wake up interval
    zwif_get_report(&ifd, NULL, 0, WAKE_UP_INTERVAL_GET, zwif_tx_sts_cb);

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_WKUP_CFG;

    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
    return 0;

}


/**
zwnet_sm_snsr_cfg - Configure sensor report delivery node and other configuration
@param[in]	nw		    Network
@param[in]	first_intf	First interface of an endpoint
@param[in]	alt_intf	First interface of an alternate endpoint (optional; if not used, it should be NULL)
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_snsr_cfg(zwnet_p nw, zwif_p first_intf, zwif_p alt_intf)
{
    int         result;
    zwnode_p    node;
    zwif_p      intf;
    uint8_t     cmd[8];
    zwifd_t     ifd;
    dev_rec_t   dev_rec = {0};

    node = first_intf->ep->node;

    //Check whether to use default or device specific group id
    if (!zwnet_dev_rec_find(nw, node->vid, node->vtype, node->pid, &dev_rec))
    {
        //Not found, use default group id.
//      dev_rec.grp_id = 1;
//      dev_rec.fld_bitmsk = DEV_REC_MSK_GID;
    }

    //Check for validity of group id value
    if ((dev_rec.fld_bitmsk & DEV_REC_MSK_GID) && dev_rec.grp_id)
    {
        result = zwnet_add_ctlr_grp(nw, first_intf, alt_intf, dev_rec.grp_id);
        if (result < 0)
        {
            return result;
        }
    }

    //Check whether to send configuration parameter
    if (((dev_rec.fld_bitmsk & DEV_REC_CFG_PARAM_MSK) == DEV_REC_CFG_PARAM_MSK)
        && dev_rec.cfg_prm_num)
    {
        intf = zwif_find_cls(first_intf, COMMAND_CLASS_CONFIGURATION);

        if (!intf)
        {   //Try alternate interface
            intf = zwif_find_cls(alt_intf, COMMAND_CLASS_CONFIGURATION);
        }

        if (!intf)
        {
            return ZW_ERR_CLASS_NOT_FOUND;
        }

        switch (dev_rec.cfg_prm_sz)
        {
            case 1:
                cmd[4] = dev_rec.cfg_prm_val & 0xFF;
                break;

            case 2:
                cmd[4] = (dev_rec.cfg_prm_val >> 8) & 0xFF;
                cmd[5] = dev_rec.cfg_prm_val & 0xFF;
                break;

            case 4:
                cmd[4] = (dev_rec.cfg_prm_val >> 24) & 0xFF;
                cmd[5] = (dev_rec.cfg_prm_val >> 16) & 0xFF;
                cmd[6] = (dev_rec.cfg_prm_val >> 8) & 0xFF;
                cmd[7] = dev_rec.cfg_prm_val & 0xFF;
                break;

            default:
                debug_zwapi_msg(&nw->plt_ctx, "Invalid config param size:%u", dev_rec.cfg_prm_sz);
                return ZW_ERR_VALUE;
        }

        //Prepare the command
        cmd[0] = COMMAND_CLASS_CONFIGURATION;
        cmd[1] = CONFIGURATION_SET;
        cmd[2] = dev_rec.cfg_prm_num;
        cmd[3] = dev_rec.cfg_prm_sz;

        //Send the command
        zwif_get_desc(intf, &ifd);
        result = zwif_exec(&ifd, cmd, 4 + dev_rec.cfg_prm_sz, zwif_tx_sts_cb);
        if (result < 0)
        {
            return result;
        }
    }

    return 0;
}


/**
zwnet_rr_sts_cb - callback function to display transmit status
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_rr_sts_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    if (user_prm)
    {
        zwnet_p     nw;
        zwep_p      ep = (zwep_p)user_prm;
        uint8_t     data[32];

        nw = ep->node->net;

        plt_mtx_lck(nw->mtx);
        data[0] = tx_sts;
        memcpy(data + 2, &ep, sizeof(zwep_p));
        zwnet_node_info_sm(nw, EVT_RR_TX_STS, data);
        plt_mtx_ulck(nw->mtx);
    }
}


/**
zwnet_sm_rr - Assign controller node return route and update the state-machine's state
@param[in]	nw		    Network
@param[in]	node		Node
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_rr(zwnet_p nw, zwnode_p node)
{
    int     result;

    result = zw_assign_return_route(&nw->appl_ctx,
                                    node->nodeid,
                                    nw->ctl.nodeid,
                                    zwnet_rr_sts_cb, (void *)&node->ep);
    if (result != 0)
    {
        return result;
    }

    //Update state-machine's state
    nw->ni_sm_sta = ZWNET_STA_ASSIGN_RR;
    //Restart timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);

    return ZW_ERR_NONE;

}


/**
zwnet_sm_others_cb - Get other command classes info callback
@param[in]	nw		    Network
@return
*/
static void zwnet_sm_others_cb(zwnet_p nw)
{
    zwnet_node_info_sm(nw, EVT_GET_OTHERS_DONE, NULL);
}


/**
zwnet_sm_cls_hdlr - Handle command classes in a node
@param[in]	nw		    Network
@param[in]	ep		    end point
@param[in]	act		    the starting point to execute actions
@return
*/
static void zwnet_sm_cls_hdlr(zwnet_p nw, zwep_p ep, zwnet_info_sm_act_t act)
{
    int    result;
    zwif_p intf;

    switch (act)
    {
        //------------------------------------------------------
        case ACT_START:
        //------------------------------------------------------
            //Check whether the node has Wake Up command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_WAKE_UP);
            if (intf)
            {   //Enable command queuing
                zwnode_p node;

                node = ep->node;
                node->enable_cmd_q = 1;
                node->sleep_cap = 1;
                node->cmd_id = 0;
                node->cmd_num = 0;
            }
            else
            {   //Disable command queuing
                zwnode_p node;

                node = ep->node;
                node->enable_cmd_q = 0;
                node->sleep_cap = 0;

                if (node->mul_cmd_q_cnt > 0)
                {
                    //Remove command queue
                    zwnode_cmd_q_rm(node);
                }
            }

            //Check whether the node has Multi Command Encapsulated command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MULTI_CMD);
            if (intf)
            {
                ep->node->mul_cmd_cap = 1;
                ep->node->mul_cmd_ctl = 1;    //turn it on  by default
            }
            else
            {
                ep->node->mul_cmd_cap = 0;
                ep->node->mul_cmd_ctl = 0;
            }

#ifdef CRC16_ENCAP
            //Check whether the node has CRC-16 Encapsulation command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_CRC_16_ENCAP);
            ep->node->crc_cap = (intf)? 1 : 0;
#endif

            //Assign return route to controller
            result = zwnet_sm_rr(nw, ep->node);
            if (result == 0)
            {
                break;
            }

        //------------------------------------------------------
        case ACT_VERSION:
        //------------------------------------------------------
            //Check whether the node has version command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_VERSION);
            if (intf)
            {
                zwif_p    temp_intf;

                //Find the first non-basic/non-extended command class
                temp_intf = ep->intf;
                while (temp_intf)
                {
                    if ((temp_intf->cls != COMMAND_CLASS_BASIC)
                        && ((temp_intf->cls & 0xFF00) == 0))
                    {
                        break;
                    }
                    temp_intf =  (zwif_p)temp_intf->obj.next;
                }

                if (temp_intf)
                {
                    result = zwnet_sm_ver_get(nw, intf, temp_intf->cls);
                    if (result == 0)
                    {
                        break;
                    }
                }
            }

        //------------------------------------------------------
        case ACT_MANF_SPEC:
        //------------------------------------------------------
            //Check whether the node has manufacturer specific command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MANUFACTURER_SPECIFIC);
            if (intf)
            {
                result = zwnet_sm_manf_get(nw, intf);
                if (result == 0)
                {
                    break;
                }
            }

        //------------------------------------------------------
        case ACT_DEVICE_CFG:
        //------------------------------------------------------
            if (nw->sm_job.auto_cfg)
            {
                intf = zwif_find_cls(ep->intf, COMMAND_CLASS_SENSOR_BINARY);
                if (!intf)
                {   //Try multi-level sensor
                    intf = zwif_find_cls(ep->intf, COMMAND_CLASS_SENSOR_MULTILEVEL);
                }

                if (intf)
                {
                    result = zwnet_sm_snsr_cfg(nw, ep->intf, NULL);
                    if (result != 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_snsr_cfg: error %d", result);
                    }
                }
            }

            //Start wakeup interval or sensor configuration
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_WAKE_UP);
            if (intf)
            {
                if (zwnet_sm_wkup_cfg(nw, intf) == 0)
                    break;
            }

        //------------------------------------------------------
        case ACT_OTHERS:
        //------------------------------------------------------
            {
                ep_sm_job_t sm_job;
                //
                //Start processing other command classes by using a single
                //endpoint info state-machine
                //

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Initialize endpoint info state-machine
                //nw->ep_sm_sta = EP_STA_IDLE;

                sm_job.cb = zwnet_sm_others_cb;
                sm_job.first_ep = ep;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Update state-machine's state
                    nw->ni_sm_sta = ZWNET_STA_GET_OTHERS;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_cls_hdlr: single endpoint state-machine in wrong state: %d", nw->single_ep_sta);
                    }
                }
            }

        //------------------------------------------------------
        case ACT_END_POINT:
        //------------------------------------------------------
            //Check whether the node has multi-instance/channel command class
            intf = zwif_find_cls(ep->intf, COMMAND_CLASS_MULTI_INSTANCE);
            if (intf)
            {
                if (intf->ver >= 2)
                {   //multi-channel command class
                    result = zwnet_sm_ch_get(nw, intf);
                    if (result == 0)
                    {
                        break;
                    }
                }
                else
                {   //multi-instance command class
                    result = zwnet_sm_inst_get(nw, intf, ep->intf->cls);
                    if (result == 0)
                    {
                        break;
                    }
                }
            }

        //------------------------------------------------------
        // Process next node
        //------------------------------------------------------
            zwnet_sm_next(nw);

    }
}


/**
zwnet_sm_updt_nameloc - Update name location of an endpoint
@param[in]	data	Character stream with byte 0 = char. presentation, byte 1 = string length
@param[in]	nameloc	Name location character buffer to be updated with UTF-8 string
@return
*/
static void zwnet_sm_updt_nameloc(uint8_t *data, char *nameloc)
{
    int     result;
    uint8_t str_len;    //string length

    str_len = data[1];

    //Check whether to convert into utf8
    if (data[0] == CHAR_PRES_UTF16)
    {
        uint8_t *utf16_buf;
#ifdef WIN32
        char    utf8_str[ZW_LOC_STR_MAX + 1];
#else
        uint8_t utf8_str[ZW_LOC_STR_MAX + 1];
#endif

        utf16_buf = (uint8_t *)calloc(1, str_len + 2);//additional 2 bytes for null characters
        if (utf16_buf)
        {
            memcpy(utf16_buf, data + 2, str_len);

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

                if (utf8_len > ZW_LOC_STR_MAX)
                {
                    utf8_len = ZW_LOC_STR_MAX;
                }

                //Database values to have priority over the values stored in the device
                if (*nameloc == '\0')
                {
                    //Copy node name/loc to the endpoint
                    memcpy(nameloc, utf8_str, utf8_len);
                    nameloc[utf8_len] = '\0';
                }
            }
        }
    }
    else
    {   //UTF-8 format
        //Database values to have priority over the values stored in the device
        if (*nameloc == '\0')
        {
            //Check for valid UTF-8 string
            str_len = plt_utf8_chk(data + 2, str_len);

            //Copy node name/loc to the endpoint
            memcpy(nameloc, data + 2, str_len);
            nameloc[str_len] = '\0';
        }
    }
}


/**
zwnet_sm_ver_rpt_hdlr - Handle command class version report
@param[in]	nw		    Network
@param[in]	ver_intf	Version	interface
@param[in]	ver_rpt_cb	Version	report callback
@param[in]	cmd_cls     Reported command class
@param[in]	cmd_cls_ver Reported command class version
@return  0 on successfully sending next command class query; 1 = no more command class to query; negative on error
*/
static int zwnet_sm_ver_rpt_hdlr(zwnet_p nw, zwif_p *ver_intf, void *ver_rpt_cb, uint16_t cmd_cls, uint8_t cmd_cls_ver)
{
    zwif_p intf;
    zwep_p ep;

    ep = (*ver_intf)->ep;

    intf = zwif_find_cls(ep->intf, cmd_cls);

    if (intf)
    {
        zwif_p  nxt_intf;
        zwif_p  new_intf;
        zwifd_t ver_ifd;

        //Save the next interface
        nxt_intf = (zwif_p)intf->obj.next;

        //Update command class version
        if ((cmd_cls_ver > 0) && (intf->ver != cmd_cls_ver))
        {
            //Replace the old interface with a new interface
            new_intf = zwif_create(cmd_cls, cmd_cls_ver, intf->propty);

            if (new_intf)
            {
                //Save back link to end point
                new_intf->ep = ep;

                //Free interface specific data
                zwif_dat_rm(intf);

                //Replace the old interface in the end point
                zwobj_rplc((zwobj_p *)(&ep->intf), &intf->obj, &new_intf->obj);

                //Check whether the replaced interface is command class version
                if (cmd_cls == COMMAND_CLASS_VERSION)
                {   //Update the version interface
                    *ver_intf = new_intf;
                }
            }
        }

        //Get next command class version

        //Find the next non-basic/non-extended command class
        while (nxt_intf)
        {
            if ((nxt_intf->cls != COMMAND_CLASS_BASIC) &&
                ((nxt_intf->cls & 0xFF00) == 0))
            {
                break;
            }
            nxt_intf =  (zwif_p)nxt_intf->obj.next;
        }

        zwif_get_desc(*ver_intf, &ver_ifd);

        if (nxt_intf)
        {
            uint8_t param;
            int     result;

            //Setup report callback
            zwif_set_report(&ver_ifd, ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);

            param = (uint8_t)nxt_intf->cls;
            result = zwif_get_report(&ver_ifd, &param, 1,
                                     VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

            if (result >= 0)
            {
                //Save the version get command class
                nw->ni_sm_cls = nxt_intf->cls;

                return 0;
            }

            debug_zwapi_msg(&nw->plt_ctx, "zwif_get_report version with error:%d", result);
            return result;
        }
        //No more command class version to query
        return 1;
    }

    return ZW_ERR_INTF_NOT_FOUND;
}


/**
zwnet_node_info_sm - Node info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             The current state of the state-machine.
*/
static int zwnet_node_info_sm(zwnet_p nw, zwnet_ni_evt_t evt, uint8_t *data)
{
    int     curr_sta;
    int32_t result;

    switch (nw->ni_sm_sta)
    {
        //----------------------------------------------------------------
        case ZWNET_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_GET_NODE_INFO)
            {
                sm_job_t  *sm_job = (sm_job_t *)data;

                if (sm_job)
                {
                    //Copy the job
                    nw->sm_job.cb = sm_job->cb;
                    nw->sm_job.num_node = sm_job->num_node;
                    nw->sm_job.op = sm_job->op;
                    nw->sm_job.auto_cfg = sm_job->auto_cfg;
//                  nw->sm_job.cfg_lifeline = sm_job->cfg_lifeline;
                    nw->sm_job.node_id = malloc(sm_job->num_node);
                    if (!nw->sm_job.node_id)
                    {
                        break;
                    }
                    memcpy(nw->sm_job.node_id, sm_job->node_id, sm_job->num_node);
                    nw->sm_job.rd_idx = 0;
                }

                if (nw->sm_job.node_id && nw->sm_job.num_node > 0)
                {
                    result = zw_request_node_info(&nw->appl_ctx, nw->sm_job.node_id[0]);

                    if (result == 0)
                    {
                        zwnode_p node;
                        //Save the end point for time out event
                        node = zwnode_find(&nw->ctl, nw->sm_job.node_id[0]);
                        if (node)
                        {
                            zwep_get_desc(&node->ep, &nw->ni_sm_ep);
                        }
                        else
                        {
                            nw->ni_sm_ep.epid = 0;
                            nw->ni_sm_ep.nodeid = nw->sm_job.node_id[0];
                            nw->ni_sm_ep.net = nw;
                        }

                        nw->ni_sm_sta = ZWNET_STA_GET_NODE_INFO;
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_NODE_INFO_TMOUT, zwnet_tmout_cb, nw);
                        break;
                    }
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zw_request_node_info with error:%d", result);

                    //Error: free node_id
                    free(nw->sm_job.node_id);
                    nw->sm_job.node_id = NULL;
                    nw->sm_job.num_node = 0;

                    //Clean up and execute callback function if this is a delayed get node info operation
                    if (sm_job == NULL)
                    {
                        zwnet_sm_next(nw);
                    }
                }
            }
            else if (evt == EVT_DELAYED_GET_NODE_INFO)
            {   //Delayed get node info operation
                sm_job_t  *sm_job = (sm_job_t *)data;

                //Copy the job
                nw->sm_job.cb = sm_job->cb;
                nw->sm_job.num_node = sm_job->num_node;
                nw->sm_job.op = sm_job->op;
                nw->sm_job.auto_cfg = sm_job->auto_cfg;
//              nw->sm_job.cfg_lifeline = sm_job->cfg_lifeline;
                nw->sm_job.node_id = malloc(sm_job->num_node);
                if (!nw->sm_job.node_id)
                {
                    break;
                }
                memcpy(nw->sm_job.node_id, sm_job->node_id, sm_job->num_node);
                nw->sm_job.rd_idx = 0;

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);

                //Start the delay timer
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, sm_job->delay, zwnet_node_info_cb, nw);

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_NODE_INFO:
        //----------------------------------------------------------------
            {
                zwep_p              ep = NULL;
                zwif_p              intf;

                if (evt == EVT_NODE_INFO)
                {
                    appl_node_info_t    *node_info;
                    zwnode_p            node;
                    zwifd_t             ifd;

                    node_info = (appl_node_info_t *)data;

                    node = zwnode_find(&nw->ctl, node_info->node_id);

                    ep = &node->ep;

                    intf = zwif_find_cls(ep->intf, COMMAND_CLASS_SECURITY);

                    printf("======================>sec add %d=======\n",node->sec_incl_failed);
                    if (nw->sec_enable && intf && (!node->sec_incl_failed)
                        && (!nw->ctl.sec_incl_failed))
                    {
                        printf("======================> goto panduan 1=======\n");
                        //This node supports security command class
                        //Setup report callback

                        zwif_get_desc(intf, &ifd);

                        result = zwif_set_report(&ifd, zwnet_sec_rpt_cb, SECURITY_COMMANDS_SUPPORTED_REPORT);
                        if (result == 0)
                        {
                            //Get the supported secure command classes

                            //Initialize report setting
                            if (nw->sec_cmd_cls.cmd_cls_buf)
                            {
                                free(nw->sec_cmd_cls.cmd_cls_buf);
                                nw->sec_cmd_cls.cmd_cls_buf = NULL;
                            }

                            result = zwsec_cmd_sup_get(&ifd);
                            if (result >= 0)
                            {
                                nw->ni_sm_sta = ZWNET_STA_GET_SEC_CMD_SUP;
                                //Restart timer
                                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, NONCE_REQ_TIMEOUT, zwnet_tmout_cb, nw);
                                break;
                            }
                            debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zwsec_cmd_sup_get with error:%d", result);
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    zwnode_p node;
                    node = zwnode_find(&nw->ctl,  nw->sm_job.node_id[nw->sm_job.rd_idx]);
                    if (node)
                    {   //Update alive status only if it is not local controller
                        if (node != &nw->ctl)
                        {
                            node->alive = 0;
                        }
                    }

                    zwnet_sm_next(nw);
                    break;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_START);

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_SEC_CMD_SUP:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_SEC_SUP_REPORT)
                {
                    zwif_p  intf;
                    uint16_t *sec_cls;

                    memcpy(&sec_cls, data + 2, sizeof(uint16_t *));
                    memcpy(&intf, data + 2 + sizeof(uint16_t *), sizeof(zwif_p));

                    //Add the secure command classes to interfaces
                    ep = intf->ep;

                    zwnet_ni_sec_updt(ep, sec_cls, data[0]);

                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_START);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_ASSIGN_RR:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_RR_TX_STS)
                {
                    memcpy(&ep, data + 2, sizeof(zwep_p));
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_VERSION);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_CMD_VER:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_VER_REPORT)
                {
                    zwif_p      ver_intf;
                    uint16_t    cmd_cls;
                    uint8_t     cmd_cls_ver;

                    cmd_cls = data[0];

                    cmd_cls = (cmd_cls << 8) | data[1];

                    cmd_cls_ver = data[2];

                    //Check whether the reported command class matches the request
                    if (cmd_cls != nw->ni_sm_cls)
                    {
                        break;
                    }

                    memcpy(&ver_intf, data + 4, sizeof(zwif_p));

                    result = zwnet_sm_ver_rpt_hdlr(nw, &ver_intf, zwnet_ver_rpt_cb, cmd_cls, cmd_cls_ver);

                    if (result == 0)
                    {   //Successfully sent get version command
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                        break;
                    }
                    else if (result == 1)
                    {
                        zwifd_t ver_ifd;
                        //No more command class version to query
                        //Get library, protocol and application versions
                        zwif_get_desc(ver_intf, &ver_ifd);
                        result = zwif_set_report(&ver_ifd, zwnet_zwver_rpt_cb, VERSION_REPORT);
                        if (result == 0)
                        {
                            result = zwif_get_report(&ver_ifd, NULL, 0,
                                                     VERSION_GET, zwif_tx_sts_cb);

                            if (result >= 0)
                            {
                                //Restart timer
                                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                                break;
                            }

                            debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zwif_get_report detailed version with error:%d", result);
                        }
                    }

                    ep = ver_intf->ep;

                }
                else if (evt == EVT_ZWVER_REPORT)
                {
                    zwif_p      ver_intf;
                    zwnode_p    node;
                    ext_ver_t   *ext_ver;

                    memcpy(&ver_intf, data + 6, sizeof(zwif_p));
                    ep = ver_intf->ep;
                    node = ep->node;

                    memcpy(&ext_ver, data + 6 + sizeof(zwif_p), sizeof(ext_ver_t *));

                    node->proto_ver = ((uint16_t)data[0]) << 8 | data[1];
                    node->app_ver = ((uint16_t)data[2]) << 8 | data[3];
                    node->lib_type = data[4];

                    if (ext_ver)
                    {
                        if (node->ext_ver)
                        {
                            free(node->ext_ver);
                        }

                        node->ext_ver = (ext_ver_t *)malloc(sizeof(ext_ver_t) + (ext_ver->fw_cnt*2));
                        if (node->ext_ver)
                        {
                            node->ext_ver->hw_ver = ext_ver->hw_ver;
                            node->ext_ver->fw_cnt = ext_ver->fw_cnt;
                            memcpy(node->ext_ver->fw_ver, ext_ver->fw_ver, (ext_ver->fw_cnt*2));
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_MANF_SPEC);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_MANF_PDT_ID:
        //----------------------------------------------------------------
            {
                zwep_p      ep;

                ep = NULL;

                if (evt == EVT_MANF_PDT_ID_REPORT)
                {
                    zwif_p      intf;
                    zwnode_p    node;
                    uint16_t    *manf_pdt_id;
                    dev_rec_t   dev_rec = {0};

                    memcpy(&intf, data + 6, sizeof(zwif_p));
                    ep = intf->ep;
                    node = ep->node;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != MS_SUBSTA_PDT_ID)
                    {
                        break;
                    }

                    //Copy manufacturer id, product type id and product id to the node
                    manf_pdt_id = (uint16_t *)data;
                    node->vid = manf_pdt_id[0];
                    node->vtype = manf_pdt_id[1];
                    node->pid = manf_pdt_id[2];

                    //Get the device category
                    if (zwnet_dev_rec_find(nw, node->vid, node->vtype, node->pid, &dev_rec))
                    {
                        if (dev_rec.fld_bitmsk & DEV_REC_MSK_CAT)
                        {
                            node->category = dev_rec.category;
                        }
                    }

                    //Check if version 2 is supported
                    if (intf->ver >= 2)
                    {
                        //Get device serial number
                        if (zwnet_sm_dev_id_get(nw, intf, DEV_ID_TYPE_OEM) == 0)
                            break;
                    }
                }
                else if (evt == EVT_DEVICE_ID_REPORT)
                {
                    zwif_p      intf;
                    zwnode_p    node;
                    dev_id_t    *dev_id;

                    memcpy(&dev_id, data, sizeof(dev_id_t *));
                    memcpy(&intf, data + sizeof(dev_id_t *), sizeof(zwif_p));
                    ep = intf->ep;
                    node = ep->node;

                    if (nw->ni_sm_sub_sta == MS_SUBSTA_DEV_ID_OEM)
                    {
                        if (dev_id->type != DEV_ID_TYPE_OEM)
                        {
                            //Save the device id
                            node->dev_id = *dev_id;
                        }
/*
                        if (dev_id->type == DEV_ID_TYPE_OEM)
                        {
                            //Get the OEM factory default Device ID Type
                            if ((dev_id->len > 0) && (dev_id->dev_id[0] != DEV_ID_TYPE_OEM))
                            {   //Get device id
                                if (zwnet_sm_dev_id_get(nw, intf, dev_id->dev_id[0]) == 0)
                                    break;
                            }
                            else
                            {   //Workaround for device that return no device id
                                //e.g. greenwave NS210-F binary switch
                                if (zwnet_sm_dev_id_get(nw, intf, DEV_ID_TYPE_SN) == 0)
                                    break;
                            }
                        }
*/
                    }
/*
                    else if (nw->ni_sm_sub_sta == MS_SUBSTA_DEV_ID)
                    {
                        if (dev_id->type != DEV_ID_TYPE_OEM)
                        {
                            //Save the device id
                            node->dev_id = *dev_id;
                        }
                    }
*/
                    else
                    {   //Unknown sub-state
                        break;
                    }

                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_DEVICE_CFG);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_WKUP_CFG:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_WKUP_CAP_REP)
                {
                    zwnode_p        node;
                    zwif_p          intf;
                    zwif_wakeup_p   wkup_cap;
                    dev_rec_t       dev_rec = {0};
                    zwifd_t         ifd;
                    uint8_t         cmd[6];

                    memcpy(&wkup_cap, data, sizeof(zwif_wakeup_p));
                    memcpy(&intf, data + sizeof(zwif_wakeup_p), sizeof(zwif_p));

                    ep = intf->ep;
                    node = ep->node;

                    //Save wake up interval
                    node->wkup_intv = wkup_cap->cur;

                    //Check whether to use default or device specific wake up interval
//                  if (!zwnet_dev_rec_find(nw, node->vid, node->vtype, node->pid, &dev_rec))
//                  {   //Use default value
//                      dev_rec.wkup_intv = nw->dflt_wkup_intv;
//                      dev_rec.fld_bitmsk = DEV_REC_MSK_WKUP;
//                  }

                    if (nw->sm_job.auto_cfg)
                    {
                        if (zwnet_dev_rec_find(nw, node->vid, node->vtype, node->pid, &dev_rec))
                        {   //Found
                            //Check for validity of wake up interval value
                            if (dev_rec.fld_bitmsk & DEV_REC_MSK_WKUP)
                            {
                                if (intf->ver >= 2)
                                {
                                    //Check input values
                                    if (dev_rec.wkup_intv < wkup_cap->min)
                                    {
                                        dev_rec.wkup_intv = wkup_cap->min;
                                    }
                                    else if (dev_rec.wkup_intv > wkup_cap->max)
                                    {
                                        dev_rec.wkup_intv = wkup_cap->max;
                                    }
                                    else
                                    {
                                        uint32_t intv_above_min;
                                        uint32_t num_of_step;

                                        intv_above_min = dev_rec.wkup_intv - wkup_cap->min;

                                        num_of_step = intv_above_min / wkup_cap->interval;

                                        dev_rec.wkup_intv = wkup_cap->min + (num_of_step * wkup_cap->interval);

                                    }
                                }

                                //Update wake up interval
                                node->wkup_intv = dev_rec.wkup_intv;
                            }
                        }

                        //Set wakeup interval and notification receiving node
                        cmd[0] = COMMAND_CLASS_WAKE_UP;
                        cmd[1] = WAKE_UP_INTERVAL_SET;
                        cmd[2] = (node->wkup_intv >> 16) & 0xFF;
                        cmd[3] = (node->wkup_intv >> 8) & 0xFF;
                        cmd[4] = node->wkup_intv & 0xFF;
                        cmd[5] = node->net->ctl.nodeid;

                        //Send the command
                        zwif_get_desc(intf, &ifd);
                        zwif_exec(&ifd, cmd, 6, zwif_tx_sts_cb);

                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_OTHERS);
            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_OTHERS:
        //----------------------------------------------------------------
            if (evt == EVT_GET_OTHERS_DONE)
            {
                zwep_p  ep;

                //Get saved end point
                ep = zwep_get_ep(&nw->ni_sm_ep);

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start processing other command classes
                zwnet_sm_cls_hdlr(nw, ep, ACT_END_POINT);
            }
            break;


        //----------------------------------------------------------------
        case ZWNET_STA_MULTI_INSTANCE:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                ep = NULL;

                if (evt == EVT_MULTI_INSTANCE_REP)
                {
                    zwif_p  intf;
                    zwif_p  mc_intf;

                    memcpy(&mc_intf, data + 2, sizeof(zwif_p));
                    ep = mc_intf->ep;

                    //Update multi-channel version
                    ep->node->mul_ch_ver = 1;

                    intf = zwif_find_cls(ep->intf, data[0]);
                    if (intf)
                    {
                        //Create or update end points for the command class
                        zwnet_ep_updt(ep, data[0], data[1], mc_intf->propty);

                        //Get number of end points for next command class
                        intf = (zwif_p)intf->obj.next;
                        if (intf)
                        {
                            //Check for extended command class (2-byte command class)
                            if (intf->cls & 0xFF00)
                            {
                                debug_zwapi_msg(&nw->plt_ctx, "multi instance get: extended command class UNSUPPORTED");
                            }
                            else
                            {
                                zwifd_t ifd;
                                uint8_t param;

                                zwif_get_desc(mc_intf, &ifd);
                                param = (uint8_t)intf->cls;
                                result = zwif_get_report(&ifd, &param, 1,
                                                         MULTI_INSTANCE_GET, zwif_tx_sts_cb);

                                if (result >= 0)
                                {
                                    //Restart timer
                                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                                    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                                    break;
                                }

                                debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_sm: zwif_get_report mul inst with error:%d", result);
                            }
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start get version of endpoint 1 command classes
                result = zwnet_sm_ep_info_get(nw, (zwep_p)ep->obj.next);
                if (result < 0)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_MULTI_CHANNEL:
        //----------------------------------------------------------------
            {
                zwif_p      intf;
                zwnode_p    node;
                zwep_p      ep;

                ep = NULL;

                if (evt == EVT_EP_NUM_REPORT)
                {
                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    ep = intf->ep;
                    node = ep->node;

                    node->num_of_ep = data[1];
                    node->dynamic_ep = data[0] & 0x80;
                    node->ident_ep = data[0] & 0x40;

                    //Update multi-channel version
                    node->mul_ch_ver = 2;

                    //Check for changes in the number of endpoints
                    if (node->num_of_ep != zwep_count(node))
                    {
                        zwep_p temp_ep;

                        //Remove the second endpoint onwards (i.e. all real endpoints)
                        while ((temp_ep = (zwep_p)node->ep.obj.next) != NULL)
                        {
                            zwnet_ep_rm(temp_ep);
                        }
                    }

                    //Get capability of first end point
                    if (node->num_of_ep > 1)
                    {
                        result = zwnet_sm_cap_get(nw, intf, 1);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_EP_CAP_REPORT)
                {
                    ep_cap_t    *epcap;
                    uint8_t     i;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&epcap, data + sizeof(zwif_p), sizeof(ep_cap_t *));

                    ep = intf->ep;
                    node = ep->node;
                    zwnet_ep_add(ep, epcap, intf->propty);

                    //Get next end point capability
                    if (epcap->ep < node->num_of_ep)
                    {
                        //Increment end point id
                        epcap->ep++;

                        if (node->ident_ep)
                        {   //All end points are identical
                            for (i = epcap->ep; i <= node->num_of_ep; i++)
                            {
                                epcap->ep = i;
                                zwnet_ep_add(ep, epcap, intf->propty);
                            }
                        }
                        else
                        {   //End points are different
                            result = zwnet_sm_cap_get(nw, intf, epcap->ep);
                            if (result == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_NW_TMOUT)
                {
                    //Get saved end point
                    ep = zwep_get_ep(&nw->ni_sm_ep);
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Check whether to process next command class
                if (!ep)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                    break;
                }

                //Start endpoint info state-machine
                result = zwnet_sm_ep_info_get(nw, (zwep_p)ep->obj.next);
                if (result < 0)
                {
                    //Process next node
                    zwnet_sm_next(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_EP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_EP_INFO_DONE)
                {
                    //Check whether to configure sensor settings
                    if (nw->sm_job.auto_cfg)
                    {
                        //search for any sensor at end points
                        zwnode_p node;
                        zwif_p   intf;

                        node = zwnode_find(&nw->ctl,  nw->sm_job.node_id[0]);

                        if (node)
                        {
                            zwep_p    temp_ep;

                            temp_ep = &node->ep;    //virtual endpoint (epid = 0)
                            temp_ep = (zwep_p)temp_ep->obj.next;    //real endpoint (epid = 1)

                            while (temp_ep)
                            {
                                //Find binary sensor
                                intf = zwif_find_cls(temp_ep->intf, COMMAND_CLASS_SENSOR_BINARY);
                                if (!intf)
                                {   //Try multi-level sensor
                                    intf = zwif_find_cls(temp_ep->intf, COMMAND_CLASS_SENSOR_MULTILEVEL);
                                }

                                if (intf)
                                {   //Use endpoint's association and configuration command class as first choice;
                                    //if failed, use the node (virtual endpoint)'s command classes.
                                    result = zwnet_sm_snsr_cfg(nw, temp_ep->intf, node->ep.intf);
                                    if (result != 0)
                                    {
                                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_snsr_cfg: error %d", result);
                                    }

                                    //Current implementation is to configure once, assuming all sensor reports
                                    //should be sent to one association group.
                                    break;
                                }

                                temp_ep = (zwep_p)temp_ep->obj.next;
                            }
                        }
                    }

                    //Process next node
                    zwnet_sm_next(nw);
                }
            }
            break;
    }
    curr_sta = nw->ni_sm_sta;

    return curr_sta;
}


/**
zwnet_node_updt_cb - Update node neighbors callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_node_updt_cb(appl_layer_ctx_t    *appl_ctx, uint8_t sts, void *user_prm)
{
    /*
    #define REQUEST_NEIGHBOR_UPDATE_STARTED     0x21
    #define REQUEST_NEIGHBOR_UPDATE_DONE        0x22
    #define REQUEST_NEIGHBOR_UPDATE_FAILED      0x23
    */
    static char    *neighbor_updt_sts[] =
    {
        "requesting neighbor list in progress",
        "new neighbor list received",
        "getting new neighbor list failed"
    };
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    unsigned    nodeid = (unsigned)user_prm;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_updt_cb: node id:%u, status:%s", nodeid,
                    (sts >= 0x21 && sts <= 0x23)?
                    neighbor_updt_sts[sts - 0x21]  : "unknown");

    //Call state-machine
    plt_mtx_lck(nw->mtx);
    if (sts == REQUEST_NEIGHBOR_UPDATE_DONE)
    {
        zwnet_updt_sm(nw, EVT_NB_UPDT_DONE, NULL);
    }
    else if (sts == REQUEST_NEIGHBOR_UPDATE_FAILED)
    {
        zwnet_updt_sm(nw, EVT_NB_UPDT_FAILED, NULL);
    }
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_node_info_cmplt_cb - Get node info complete callback function
@param[in]	nw    The network
@return
*/
static void zwnet_node_info_cmplt_cb(zwnet_p nw)
{
    zwnet_updt_sm(nw, EVT_NI_UPDT_DONE, NULL);
}


/**
zwnet_sm_nu_progress_cb - Network update state machine progress timer callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_sm_nu_progress_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->nu_sm_tmr_ctx);
    nw->nu_sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash
    plt_mtx_ulck(nw->mtx);

    //Callback to report progress of the network update operation
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, ZWNET_OP_UPDATE, nw->nu_progress_sts);
    }
}


/**
zwnet_sm_neighbor_updt - Get the node neighbor update and update the state-machine's state
@param[in]	nw		    Network
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_neighbor_updt(zwnet_p nw)
{
    int      result;
    zwnode_p node;

    //Get the node id list
    if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        nw->nu_sm_ctx.num_node = 0;
        nw->nu_sm_ctx.rd_idx = 0;

        while (node)
        {
            nw->nu_sm_ctx.node_id[nw->nu_sm_ctx.num_node++] = node->nodeid;
            node = (zwnode_p)node->obj.next;
        }

        result = zw_request_node_neighbor_update(&nw->appl_ctx,
                                                 nw->nu_sm_ctx.node_id[0], zwnet_node_updt_cb);
        if (result == 0)
        {
            //Change state
            nw->nu_sm_ctx.sta = NU_STA_NEIGHBOR_UPDT;

            //Update progress status
            nw->nu_progress_sts = OP_NU_NEIGHBOR;

            //Stop timer
            plt_tmr_stop(&nw->plt_ctx, nw->nu_sm_tmr_ctx);

            //Start timer for network update progress callback
            nw->nu_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_sm_nu_progress_cb, nw);

        }

        return result;
    }

    return ZW_ERR_NODE_NOT_FOUND;
}


/**
zwnet_sm_node_info_updt - Get the node info and update the state-machine's state
@param[in]	nw		    Network
@return  0 on success, negative error number on failure
*/
static int zwnet_sm_node_info_updt(zwnet_p nw)
{
    zwnode_p    node;
    sm_job_t    sm_job;
    uint8_t     node_id[256];

    sm_job.op = ZWNET_OP_UPDATE;
    sm_job.num_node = 0;
    sm_job.node_id = node_id;
    sm_job.cb = zwnet_node_info_cmplt_cb;
    sm_job.auto_cfg = 0;
//  sm_job.cfg_lifeline = 0;

    //Get the node id list
    if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        while (node)
        {
            node_id[sm_job.num_node++] = node->nodeid;
            node = (zwnode_p)node->obj.next;
        }

        if (zwnet_node_info_sm(nw, EVT_GET_NODE_INFO, (uint8_t *)&sm_job)
            == ZWNET_STA_GET_NODE_INFO)
        {
            //Change state
            nw->nu_sm_ctx.sta = NU_STA_NODE_INFO_UPDT;

            //Update progress status
            nw->nu_progress_sts = OP_NU_GET_NODE_INFO;

            //Stop timer
            plt_tmr_stop(&nw->plt_ctx, nw->nu_sm_tmr_ctx);

            //Start timer for network update progress callback
            nw->nu_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_sm_nu_progress_cb, nw);
            return ZW_ERR_NONE;

        }
        return ZW_ERR_OP_FAILED;
    }
    return ZW_ERR_NODE_NOT_FOUND;
}


/**
zwnet_sm_nu_failed - Stop the network update state-machine
                     due to failure to start node info update
@param[in]	nw		Network
@return
*/
static void    zwnet_sm_nu_failed(zwnet_p nw)
{
    //Reset to idle state
    nw->nu_sm_ctx.sta = NU_STA_IDLE;
    nw->nu_sm_ctx.num_node = 0;

    //Reset operation to "no operation"
    nw->curr_op = ZWNET_OP_NONE;

    //Invoke notify callback
    zwnet_notify(nw, ZWNET_OP_UPDATE, OP_FAILED);
}

static const char    *nw_updt_sts_str[] =
{   "ok",
    "aborted",
    "SUC busy",
    "SUC disabled",
    "Too many changes",
    "SUC set ok",
    "SUC set failed"
};


/**
zwnet_update_cb - Request network topology updates from the SUC/SIS node callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status of update
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_update_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_update_cb: status:%s", (sts < 7)?
                    nw_updt_sts_str[sts]  : "unknown");


    //Call state-machine
    plt_mtx_lck(nw->mtx);
    zwnet_updt_sm(nw, (sts == ZW_SUC_UPDATE_DONE)?
                  EVT_NU_TOPOLOGY_DONE : EVT_NU_TOPOLOGY_FAILED, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_updt_sm - Network update state-machine
@param[in]	nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             The current state of the state-machine.
*/
static int    zwnet_updt_sm(zwnet_p  nw, zwnet_nu_evt_t  evt, uint8_t *data)
{
    int     curr_sta;
    int32_t result;

    switch (nw->nu_sm_ctx.sta)
    {
        //----------------------------------------------------------------
        case NU_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_NU_START)
            {
                uint8_t role;
                //Reset the state machine context
                memset(&nw->nu_sm_ctx, 0, sizeof(nu_sm_ctx_t));

                //Determine the contoller network role
                result = zw_get_controller_capabilities(&nw->appl_ctx, &role);
                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_updt_sm: failed to get nw role");
                    break;
                }

                if (role & CONTROLLER_NODEID_SERVER_PRESENT)
                {
                    nw->nu_sm_ctx.nw_role = (role & CONTROLLER_IS_SUC)? NW_ROLE_SIS : NW_ROLE_INCLUSION;
                }
                else
                {
                    if (role & CONTROLLER_IS_REAL_PRIMARY)
                    {
                        nw->nu_sm_ctx.nw_role = NW_ROLE_PRIMARY;
                    }
                    else
                    {
                        nw->nu_sm_ctx.nw_role = (role & CONTROLLER_IS_SUC)? NW_ROLE_SUC : NW_ROLE_SECONDARY;
                    }
                }

                //Start network topology update only for secondary and inclusion controllers
                if ((nw->nu_sm_ctx.nw_role == NW_ROLE_SECONDARY) ||
                    (nw->nu_sm_ctx.nw_role == NW_ROLE_INCLUSION))
                {
                    uint8_t     suc_node_id;

                    //Get SUC node id
                    result = zw_get_suc_node_id(&nw->appl_ctx, &suc_node_id);
                    if ((result == 0) && (suc_node_id != 0))
                    {   //Have valid SUC node id
                        result = zw_request_network_update(&nw->appl_ctx, zwnet_update_cb);

                        if (result == 0)
                        {
                            //Change state
                            nw->nu_sm_ctx.sta = NU_STA_TOPOLOGY_UPDT;

                            //Update progress status
                            nw->nu_progress_sts = OP_NU_TOPOLOGY;

                            //Stop timer
                            plt_tmr_stop(&nw->plt_ctx, nw->nu_sm_tmr_ctx);

                            //Start timer for network update progress callback
                            nw->nu_sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_sm_nu_progress_cb, nw);

                            break;
                        }
                        debug_zwapi_msg(&nw->plt_ctx, "request_network_update with error:%d", result);
                    }
                    //Fail to start network topology update

                    //Proceed to node neighbour update for inclusion server
                    if (nw->nu_sm_ctx.nw_role == NW_ROLE_INCLUSION)
                    {
                        result = zwnet_sm_neighbor_updt(nw);
                        if (result < 0)
                        {
                            debug_zwapi_msg(&nw->plt_ctx, "request_node_neighbor_update with error:%d", result);
                        }
                    }
                    else //Proceed to node info update for secondary server
                    {
                        result = zwnet_sm_node_info_updt(nw);
                        if (result < 0)
                        {
                            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_node_info_updt with error:%d", result);
                        }
                    }


                }
                else if ((nw->nu_sm_ctx.nw_role == NW_ROLE_PRIMARY) ||
                         (nw->nu_sm_ctx.nw_role == NW_ROLE_SIS))
                {   //Start node neighbour update only for primary and SIS controllers
                    result = zwnet_sm_neighbor_updt(nw);
                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "request_node_neighbor_update with error:%d", result);
                    }
                }
                else
                {   //SUC.  Start node info update.
                    result = zwnet_sm_node_info_updt(nw);
                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_node_info_updt with error:%d", result);
                    }
                }
            }
            break;

        //----------------------------------------------------------------
        case NU_STA_TOPOLOGY_UPDT:
        //----------------------------------------------------------------
            switch (evt)
            {
                case EVT_NU_TOPOLOGY_FAILED:
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_updt_sm: network topology update failed");

                case EVT_NU_TOPOLOGY_DONE:
                    if (nw->nu_sm_ctx.nw_role == NW_ROLE_INCLUSION)
                    {   //For inclusion controller, proceed to node neighbor update
                        result = zwnet_sm_neighbor_updt(nw);
                        if (result < 0)
                        {
                            debug_zwapi_msg(&nw->plt_ctx, "request_node_neighbor_update with error:%d", result);
                            //Node neighbor update failed, proceed to node info update
                            result = zwnet_sm_node_info_updt(nw);
                            if (result < 0)
                            {
                                debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_node_info_updt with error:%d", result);
                                zwnet_sm_nu_failed(nw);
                            }

                        }
                    }
                    else if (nw->nu_sm_ctx.nw_role == NW_ROLE_SECONDARY)
                    {   //For secondary controller, proceed to node info update
                        result = zwnet_sm_node_info_updt(nw);
                        if (result < 0)
                        {
                            debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_node_info_updt with error:%d", result);
                            zwnet_sm_nu_failed(nw);
                        }
                    }
                    break;

                default:
                    break;
            }
            break;

        //----------------------------------------------------------------
        case NU_STA_NEIGHBOR_UPDT:
        //----------------------------------------------------------------
            switch (evt)
            {
                case EVT_NB_UPDT_FAILED:
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_updt_sm: neighbor update failed on node:%u",
                                    nw->nu_sm_ctx.node_id[nw->nu_sm_ctx.rd_idx]);
                    //Store the failed node
                    nw->nu_sm_ctx.failed_id[nw->nu_sm_ctx.num_failed_node++] = nw->nu_sm_ctx.node_id[nw->nu_sm_ctx.rd_idx];


                case EVT_NB_UPDT_DONE:
                    if (++nw->nu_sm_ctx.rd_idx < nw->nu_sm_ctx.num_node)
                    {
                        //Process the next node
                        result = zw_request_node_neighbor_update(&nw->appl_ctx,
                                                                 nw->nu_sm_ctx.node_id[nw->nu_sm_ctx.rd_idx],
                                                                 zwnet_node_updt_cb);

                        if (result == 0)
                        {
                            break;
                        }
                    }
                    else if ((nw->nu_sm_ctx.num_failed_node > 0)
                             && (nw->nu_sm_ctx.num_retry < MAX_NEIGHBOR_UPDT_RETRIES))
                    {
                        nw->nu_sm_ctx.num_retry++;
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_updt_sm: neighbor update retry: %u", nw->nu_sm_ctx.num_retry);
                        memcpy(nw->nu_sm_ctx.node_id, nw->nu_sm_ctx.failed_id, nw->nu_sm_ctx.num_failed_node);
                        nw->nu_sm_ctx.num_node = nw->nu_sm_ctx.num_failed_node;
                        nw->nu_sm_ctx.num_failed_node = 0;
                        nw->nu_sm_ctx.rd_idx = 0;

                        result = zw_request_node_neighbor_update(&nw->appl_ctx,
                                                                 nw->nu_sm_ctx.node_id[0],
                                                                 zwnet_node_updt_cb);


                        if (result == 0)
                        {
                            break;
                        }


                    }
                    //Proceed to node info update
                    result = zwnet_sm_node_info_updt(nw);
                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sm_node_info_updt with error:%d", result);
                        zwnet_sm_nu_failed(nw);
                    }

                default:
                    break;
            }
            break;

        //----------------------------------------------------------------
        case NU_STA_NODE_INFO_UPDT:
        //----------------------------------------------------------------
            if (evt == EVT_NI_UPDT_DONE)
            {
                //Reset to idle state
                nw->nu_sm_ctx.sta = NU_STA_IDLE;
                nw->nu_sm_ctx.num_node = 0;
            }
            break;


    }
    curr_sta = nw->nu_sm_ctx.sta;

    return curr_sta;
}


/**
application_update_cb - Application update callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status
@param[in]	node_info   Node information such as node id, basic, generic and specific device classes and supported command classes
@return
*/
static void    application_update_cb(struct _appl_layer_ctx   *appl_ctx,
                                     uint8_t sts, appl_node_info_t *node_info)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    zwnode_p    node;
    int         result;

    char *sts_str;
    switch (sts)
    {
        case    UPDATE_STATE_NODE_INFO_RECEIVED:
            sts_str = "node info received";
            break;

        case    UPDATE_STATE_NODE_INFO_REQ_DONE:
            sts_str = "node info req done";
            break;

        case    UPDATE_STATE_NODE_INFO_REQ_FAILED:
            sts_str = "node info req failed";
            break;

        case    UPDATE_STATE_ROUTING_PENDING:
            sts_str = "routing pending";
            break;

        case    UPDATE_STATE_NEW_ID_ASSIGNED:
            sts_str = "new id assigned";
            break;

        case    UPDATE_STATE_DELETE_DONE:
            sts_str = "delete done";
            break;

        case    UPDATE_STATE_SUC_ID:
            sts_str = "SUC node Id was updated";
            break;

        default:
            sts_str = "unknown";

    }

    if (node_info->cmd_cnt > 0)
    {
        debug_zwapi_ts_msg(&nw->plt_ctx, "application_update_cb: updated node:%u, status:%s",
                           (unsigned)node_info->node_id, sts_str);
        debug_zwapi_msg(&nw->plt_ctx, "basic:%02X, generic:%02X, specific:%02X, cmd class:",
                        (unsigned)node_info->basic, (unsigned)node_info->gen,
                        (unsigned)node_info->spec);
        zwnet_cmd_cls_show(nw, node_info->cmd_cls, node_info->cmd_cnt);

        if (sts == UPDATE_STATE_NODE_INFO_RECEIVED)
        {   //This could be node added or deleted or even from other network!
            uint8_t res;

            //Check whether the node is in the protocol routing table
            result = zwnet_node_is_valid(nw, node_info->node_id, &res);
            if (result == 0 && res)
            {
                //The node is in the protocol routing table, update the
                //network data structure
                plt_mtx_lck(nw->mtx);
                zwnet_node_info_update(nw, node_info);

                zwnet_rp_ni_sm(nw, EVT_NODE_INFO, (uint8_t *)node_info);
                zwnet_node_info_sm(nw, EVT_NODE_INFO, (uint8_t *)node_info);
                plt_mtx_ulck(nw->mtx);

            }
        }
    }
    else
    {   //No node information is available
        debug_zwapi_ts_msg(&nw->plt_ctx, "application_update_cb: updated node:%u, status:%s",
                           (unsigned)node_info->node_id, sts_str);

        if (sts == UPDATE_STATE_DELETE_DONE)
        {   //The node was deleted
            node = zwnode_find(&nw->ctl, node_info->node_id);
            if (node)
            {
                if (nw->init.node)
                {
                    zwnoded_t noded;
                    zwnode_get_desc(node, &noded);
                    nw->init.node(nw->init.user, &noded, ZWNET_NODE_REMOVED);
                }
                //Remove the node
                zwnode_rm(nw, node_info->node_id);
            }
        }
        else if (sts == UPDATE_STATE_SUC_ID)
        {
            //Check whether the SUC/SIS is assigned to the controller
            if (node_info->node_id == nw->ctl.nodeid)
            {
                uint8_t role;

                result = zw_get_controller_capabilities(&nw->appl_ctx, &role);
                if (result == 0)
                {
                    if ((role & CONTROLLER_NODEID_SERVER_PRESENT)
                        && (role & CONTROLLER_IS_SUC))
                    {   //SIS is assigned
                        zwnet_exec_req_t     cfg_lifeln_req;

                        cfg_lifeln_req.node_id = node_info->node_id;
                        cfg_lifeln_req.action = EXEC_ACT_CFG_LIFELINE;

                        util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                      (uint8_t *)&cfg_lifeln_req, sizeof(zwnet_exec_req_t));
                        plt_sem_post(nw->nw_exec_sem);

                    }
                }
            }
        }
        //Don't use the UPDATE_STATE_NODE_INFO_REQ_FAILED as it does not
        //provide the node id of the failed request
/*
        else if (sts == UPDATE_STATE_NODE_INFO_REQ_FAILED)
        {   //Node does not respond with node info
            plt_mtx_lck(nw->mtx);
            zwnet_node_info_sm(nw, EVT_NODE_INFO_FAILED, &node_info->node_id);
            plt_mtx_ulck(nw->mtx);
        }
*/
    }

    //New node added with or without command classes info
    if (sts == UPDATE_STATE_NEW_ID_ASSIGNED)
    {   //New node added
        zwnet_node_info_update(nw, node_info);

        //Call back to notify a node was added
        node = zwnode_find(&nw->ctl, node_info->node_id);

        if (node && nw->init.node)
        {
            zwnoded_t noded;
            zwnode_get_desc(node, &noded);
            nw->init.node(nw->init.user, &noded, ZWNET_NODE_ADDED);
        }
    }
}

/**
zwnet_sec_tx_cb - security layer transmission callback
@param[in]	nw		network
@param[in]  nodeid	destination node id of the transmission
@param[in]  status	0= transmission ok; else negative error number
@param[in]	user	user context
*/
void zwnet_sec_tx_cb(zwnet_p nw, uint8_t nodeid, int status, void **user)
{
    if (status != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_sec_tx_cb: node id:%u, status:%d",
                        nodeid, status);
    }

}


/**
zwnet_rpt_send - send report
@param[in]	nw		    network
@param[in]	cmd_buf     command buffer
@param[in]	len		    length of cmd_buf
@param[in]  dst_node	destination node id of the transmission
@param[in]  msg_type	message encapsulation type: ZWNET_MSG_TYPE_XXX
@return		ZW_ERR_xxx
*/
static int zwnet_rpt_send(zwnet_p nw, uint8_t *cmd_buf, uint8_t len, uint8_t dst_node, uint8_t msg_type)
{
    int             result;
    appl_snd_data_t *prm;

    //Allocate buffer that is large enough for CRC-16 encapsulation
    prm = (appl_snd_data_t *)malloc(sizeof(appl_snd_data_t) + len + CRC16_OVERHEAD);
    if (!prm)
        return ZW_ERR_MEMORY;

    //Prepare the report
    memcpy(prm->dat_buf, cmd_buf, len);

    //Send the report
    prm->node_id = dst_node;
    prm->tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE
                  | TRANSMIT_OPTION_EXPLORE;
    prm->dat_len = len;

    if (nw->sec_enable && (msg_type == ZWNET_MSG_TYPE_SECURE))
    {
        zwifd_t ifd;

        //Send the command using security message encapsulation
        zwsec_tx_sm_param_t *tx_param;

        tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + prm->dat_len);
        if (tx_param)
        {
            ifd.nodeid = dst_node;
            ifd.net = nw;

            tx_param->opp[0] = prm->dat_len;
            memcpy(tx_param->opp + 1, prm->dat_buf, prm->dat_len);

            tx_param->cb = zwnet_sec_tx_cb;
            tx_param->user[0] = NULL;
            tx_param->use_0_nw_key = 0;
            tx_param->nodeid = dst_node;

            result = zwsec_send(&ifd, tx_param);
            free(tx_param);

        }
        else
        {
            result = ZW_ERR_MEMORY;
        }
    }
#ifdef CRC16_ENCAP
    else if (msg_type == ZWNET_MSG_TYPE_CRC16)
    {
        //Send the command using CRC-16 encapsulation
        uint16_t crc;

        memmove(prm->dat_buf + 2, prm->dat_buf, prm->dat_len);
        prm->dat_buf[0] = COMMAND_CLASS_CRC_16_ENCAP;
        prm->dat_buf[1] = CRC_16_ENCAP;
        crc = zwutl_crc16_chk(CRC_INIT, prm->dat_buf, prm->dat_len + 2);
        prm->dat_buf[prm->dat_len + 2] = (crc >> 8);
        prm->dat_buf[prm->dat_len + 3] = (crc & 0x00ff);

        prm->dat_len += CRC16_OVERHEAD;
        result = zw_send_data(&nw->appl_ctx, prm, NULL, NULL);
    }
#endif
    else
    {
        //Send the command without queuing
        result = zw_send_data(&nw->appl_ctx, prm, NULL, NULL);
    }

    free(prm);
    return result;
}

#ifdef  TEST_SEC_WAKE_UP
/**
zwnet_wkup_tmout_cb - Wake up timeout callback
@param[in] data     Network
@return
*/
static void    zwnet_wkup_tmout_cb(void *data)
{
    zwnet_p             nw = (zwnet_p)data;
    int32_t             result;
    appl_snd_data_t     *prm;

    prm = (appl_snd_data_t *)malloc(sizeof(appl_snd_data_t) + 2);
    if (!prm)
        return;

    //Prepare the report
    prm->dat_buf[0] = COMMAND_CLASS_WAKE_UP;
    prm->dat_buf[1] = WAKE_UP_NOTIFICATION;

    //Send the report
    prm->node_id = nw->wkup_node_id;
    prm->tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE
                  | TRANSMIT_OPTION_EXPLORE;
    prm->dat_len = 2;

    if (nw->sec_enable && nw->sec_wkup)
    {
        zwifd_t ifd;

        //Send the command using security message encapsulation
        zwsec_tx_sm_param_t *tx_param;

        tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + prm->dat_len);
        if (tx_param)
        {
            ifd.nodeid = prm->node_id;
            ifd.net = nw;

            tx_param->opp[0] = prm->dat_len;
            memcpy(tx_param->opp + 1, prm->dat_buf, prm->dat_len);

            tx_param->cb = zwnet_sec_tx_cb;
            tx_param->user[0] = NULL;
            tx_param->use_0_nw_key = 0;
            tx_param->nodeid = ifd.nodeid;

            result = zwsec_send(&ifd, tx_param);
            free(tx_param);

        }
    }
    else
    {
        //Send the command without queuing
        result = zw_send_data(&nw->appl_ctx, prm, NULL, NULL);
    }

    free(prm);
}
#endif

/**
zwnet_power_lvl_tmr_cb - Power level timer callback
@param[in] data     Network
@remark It callbacks for every 1 sec interval
@return
*/
static void    zwnet_power_lvl_tmr_cb(void *data)
{
    zwnet_p             nw = (zwnet_p)data;

	if(nw->mtx)
	{
		plt_mtx_lck(nw->mtx);

		if(nw->byPowerLvlTimeout > 0)
		{
			nw->byPowerLvlTimeout--;

			if(nw->byPowerLvlTimeout == 0 && nw->pPowerLvl_tmr_ctx)
			{
				//Timer reaches 0. Stop timer. Reset power level
				if (plt_tmr_stop(&nw->plt_ctx, nw->pPowerLvl_tmr_ctx) == 0)
				{
					plt_msg_ts_show(&nw->plt_ctx, "Power level timer stop failed");
				}

				nw->pPowerLvl_tmr_ctx = NULL;

				//Reset power level to normal power since all the timer out has finished
				zw_set_rf_powerlevel(&nw->appl_ctx, POWERLEVEL_SET_NORMALPOWER, NULL);
			}
		}
		plt_mtx_ulck(nw->mtx);
	}
}

/**
zwnet_power_lvl_testframe_cb - send power level test frame callback
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
*/
static void zwnet_power_lvl_testframe_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p nw;
    int32_t result;
	zwpowerlvl_tst_tmp_t *pstPowerTstData = (zwpowerlvl_tst_tmp_t *)user_prm;
    extern const char *zwif_tx_cmplt_sts[];

    nw = (zwnet_p)appl_ctx->data;

    if (tx_sts != TRANSMIT_COMPLETE_OK)
    {
		if(pstPowerTstData->wPowerTstCurrFrmCnt > 0)
		{
			debug_zwapi_msg(&nw->plt_ctx, "zwnet_power_lvl_testframe_cb with error:%s",
							(tx_sts < 6)?
							zwif_tx_cmplt_sts[tx_sts]  : "unknown");
		}
    }
	else
	{
		//Transmit OK. Update status.
		nw->wPowerTstAckFrmCnt++;
	}

	if(pstPowerTstData->wPowerTstCurrFrmCnt >= pstPowerTstData->wPowerTstFrmCnt)
	{
		//Test done.
		uint8_t c_buf[8] = {0};

		if(nw->wPowerTstAckFrmCnt > 0)
		{
			nw->byPowerTstStatus = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES;
		}
		else
		{
			nw->byPowerTstStatus = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED;
		}

		//Reset to normal power
		result = zw_set_rf_powerlevel(appl_ctx, POWERLEVEL_SET_NORMALPOWER, NULL);

		//Send result back
		c_buf[0] = COMMAND_CLASS_POWERLEVEL;
		c_buf[1] = POWERLEVEL_TEST_NODE_REPORT;
		c_buf[2] = nw->byPowerTstNodeID; //If never tested before, it will be 0, which is ZW_TEST_NOT_A_NODEID;
		c_buf[3] = nw->byPowerTstStatus; //If never tested before, it will be 0, which is POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED
		c_buf[4] = (uint8_t)(nw->wPowerTstAckFrmCnt >> 8);
		c_buf[5] = (uint8_t)(nw->wPowerTstAckFrmCnt & 0x00FF);

		//Send the report
		result = zwnet_rpt_send(nw, c_buf, 6, pstPowerTstData->bysrc_node, pstPowerTstData->bymsg_type);
		if (result < 0)
		{
			debug_zwapi_msg(&nw->plt_ctx, "Send power level rpt with error:%d", result);
		}

		free(pstPowerTstData);
		pstPowerTstData = NULL;
	}
	else
	{
		//Test not done. Continue to send test frame.
		pstPowerTstData->wPowerTstCurrFrmCnt++;

		zw_send_test_frame(appl_ctx,
						   nw->byPowerTstNodeID,
						   pstPowerTstData->byPowerLvl,
						   zwnet_power_lvl_testframe_cb,
						   pstPowerTstData);
	}
}


/* Received frame status flags */
#define RECEIVE_STATUS_ROUTED_BUSY    0x01
#define RECEIVE_STATUS_LOW_POWER      0x02    /* received at low output power level, this must */
/* have the same value as TRANSMIT_OPTION_LOW_POWER */
#define RECEIVE_STATUS_TYPE_MASK      0x0C    /* Mask for masking out the received frametype bits */
#define RECEIVE_STATUS_TYPE_SINGLE    0x00    /* Received frame is singlecast frame (rxOptions == xxxx00xx) */
#define RECEIVE_STATUS_TYPE_BROAD     0x04    /* Received frame is broadcast frame  (rxOptions == xxxx01xx) */
#define RECEIVE_STATUS_TYPE_MULTI     0x08    /* Received frame is multiecast frame (rxOptions == xxxx10xx) */


static void appl_cmd_hdlr(uint8_t cmd_len, uint8_t *cmd_buf, appl_cmd_prm_t *prm)
{
    static const char    *sts_str[] =
    {   "unknown",
        "response route is locked",//RECEIVE_STATUS_ROUTED_BUSY
        "received at low output power level",//RECEIVE_STATUS_LOW_POWER
        "unknown"
    };

    static const char    *frame_type_str[] =
    {   "single",
        "broadcast",
        "multicast",
        "unknown"
    };

    zwnode_p    node;
    zwif_p      intf;
    zwnet_p     nw  = prm->nw;
    int         wait_2nd_frame = prm->wait_2nd_frame;
    int         node_sec_ena = prm->node_sec_ena;
    uint8_t     rx_sts = prm->rx_sts;
    uint8_t     src_node = prm->src_node;
    uint8_t     msg_type = prm->msg_type;

#ifdef  TEST_ALARM_V2
    if ((cmd_buf[0] == COMMAND_CLASS_ALARM) && (cmd_buf[1] == ALARM_REPORT))
    {
        if (cmd_buf[2] == 0x15)
        {
            cmd_buf[4] =  15; //src node id
            cmd_buf[5] =  0xFF;//status
            cmd_buf[6] =  ZW_ALRM_CO2;//type
            cmd_buf[7] =  ZW_ALRM_EVT_CO2;//event
            cmd_buf[8] =  0;//number of event parameters
            cmd_len = 9;
        }
        else if (cmd_buf[2] == 0x16)
        {
            cmd_buf[4] =  16; //src node id
            cmd_buf[5] =  0xFF;//status
            cmd_buf[6] =  ZW_ALRM_CO;//type
            cmd_buf[7] =  ZW_ALRM_EVT_CO_L;//event
            cmd_buf[8] =  8;//number of event parameters
            cmd_buf[9] = COMMAND_CLASS_NODE_NAMING;
            cmd_buf[10] = NODE_NAMING_NODE_LOCATION_REPORT;
            cmd_buf[11] =  CHAR_PRES_ASCII;
            cmd_buf[12] =  's';
            cmd_buf[13] =  't';
            cmd_buf[14] =  'a';
            cmd_buf[15] =  'i';
            cmd_buf[16] =  'r';

            cmd_len = 17;
        }
        else if (cmd_buf[2] == 0x17)
        {
#ifdef OS_LINUX
            uint8_t utf16_str[] = {'B', 0, 'a', 0, 'c', 0, 'k', 0, ' ', 0, 'd', 0, 'o', 0, 'o', 0, 'r', 0
            };
#endif
            cmd_buf[4] =  17; //src node id
            cmd_buf[5] =  0xFF;//status
            cmd_buf[6] =  ZW_ALRM_CO;//type
            cmd_buf[7] =  ZW_ALRM_EVT_CO_L;//event
            cmd_buf[8] =  18+3;//number of event parameters
            cmd_buf[9] = COMMAND_CLASS_NODE_NAMING;
            cmd_buf[10] = NODE_NAMING_NODE_LOCATION_REPORT;
            cmd_buf[11] =  CHAR_PRES_UTF16;
#ifdef WIN32
            memcpy(cmd_buf + 12, L"Back door", 18);
#else
            memcpy(cmd_buf + 12, utf16_str, 18);
#endif
            util_ntohs((uint16_t *)(cmd_buf + 12), 9);

            cmd_len = 30;
        }
        else if (cmd_buf[2] == 0x18)
        {
            cmd_buf[4] =  18; //src node id
            cmd_buf[5] =  0xFF;//status
            cmd_buf[6] =  ZW_ALRM_LOCK;//type
            cmd_buf[7] =  ZW_ALRM_EVT_KEYPAD_LCK;//event
            cmd_buf[8] =  3;//number of event parameters
            cmd_buf[9] = COMMAND_CLASS_USER_CODE;
            cmd_buf[10] = USER_CODE_REPORT;
            cmd_buf[11] =  8;

            cmd_len = 12;
        }
        else if (cmd_buf[2] == 0x19)
        {
            cmd_buf[1] =  ALARM_TYPE_SUPPORTED_REPORT_V2;
            cmd_buf[2] =  0x82;//support vendor specific type and 2 bit-mask bytes follow
            cmd_buf[3] =  0x12; //type:1 and 4
            cmd_buf[4] =  0x0E;//type:9 to 11

            cmd_len = 5;
        }
    }
#endif
#ifdef  ZW_STRESS_TEST
    if ((cmd_buf[0] == COMMAND_CLASS_MANUFACTURER_PROPRIETARY) && (cmd_len >= 7))
    {   //Check if the manufacturer id is correct
        if ((cmd_buf[1] == MANF_ID_1)  && (cmd_buf[2] == MANF_ID_2))
        {
            uint32_t    seq_num;
            seq_num = ((uint32_t)cmd_buf[3]) << 24 | ((uint32_t)cmd_buf[4]) << 16
                      | ((uint32_t)cmd_buf[5]) << 8 | ((uint32_t)cmd_buf[6]);
            //Callback
            if (nw->stress_tst_cb)
            {
                nw->stress_tst_cb(nw->init.user, seq_num);
            }
        }
        return;
    }
#endif

#ifdef  SUPPORT_SECURITY
    //Check whether this is a nonce request (NR bit of the security header is set)
    if (nw->sec_enable && node_sec_ena
        && (cmd_buf[0] == COMMAND_CLASS_SECURITY)
        && (cmd_buf[1] & 0x40))
    {
        int idx;

        plt_mtx_lck(nw->sec_ctx->sec_mtx);
        idx = zwsec_inonce_gen(nw->sec_ctx, src_node, INONCE_STA_INIT);
        plt_mtx_ulck(nw->sec_ctx->sec_mtx);

        if (idx >= 0)
        {
            appl_snd_data_t     *prm;

            prm = (appl_snd_data_t *)malloc(sizeof(appl_snd_data_t) + 10);
            if (!prm)
                return;

            //Prepare the report
            prm->dat_buf[0] = COMMAND_CLASS_SECURITY;
            prm->dat_buf[1] = SECURITY_NONCE_REPORT;
            memcpy(prm->dat_buf + 2, nw->sec_ctx->inonce_tbl[idx].nonce, 8);

            //Send the report
            prm->node_id = src_node;
            prm->tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE
                          | TRANSMIT_OPTION_EXPLORE;
            prm->dat_len = 10;

            if (zw_send_data(&nw->appl_ctx, prm, NULL, NULL) == 0)
            {   //Set the expiry time only after the SECURITY_NONCE_REPORT was sent
                zwsec_inonce_expire_set(nw->sec_ctx, idx);
            }
            else
            {   //Delete nonce from the internal nonce table
                nw->sec_ctx->inonce_tbl[idx].state = INONCE_STA_INVALID;

            }
            free(prm);
        }

        //Return if the message is SECURITY_NONCE_GET or second frame of split SPP pending
        if ((cmd_buf[1] == SECURITY_NONCE_GET)
            || wait_2nd_frame)
            return;
    }

    //Check for commands for secure network inclusion
    if (nw->sec_enable && node_sec_ena
        && (cmd_buf[0] == COMMAND_CLASS_SECURITY))
    {
        switch (cmd_buf[1])
        {
            case SECURITY_SCHEME_GET:
            case NETWORK_KEY_SET:
            case SECURITY_SCHEME_INHERIT:
                zwsec_incd_msg_hdlr(nw, src_node, cmd_buf, cmd_len, (msg_type == ZWNET_MSG_TYPE_SECURE)? 1:0);
                return;
                break;

            case NETWORK_KEY_VERIFY:
            case SECURITY_SCHEME_REPORT:
                zwsec_add_node_msg_hdlr(nw, src_node, cmd_buf, cmd_len, (msg_type == ZWNET_MSG_TYPE_SECURE)? 1:0);
                return;
                break;

            case SECURITY_NONCE_REPORT:
                if (cmd_len >= 10)
                {
                    //Call back system defined function
                    zwsec_nonce_rpt_hdlr(nw, src_node, cmd_buf + 2);
                }
                return;
                break;

            case SECURITY_COMMANDS_SUPPORTED_GET:
                if (msg_type == ZWNET_MSG_TYPE_SECURE)
                {
                    int                 result;
                    zwsec_tx_sm_param_t *tx_param;
                    zwifd_t             ifd;
                    uint8_t             sec_cmd_cnt;    //number of secure command classes

                    sec_cmd_cnt = (sizeof(supp_cmd_class)/sizeof(uint16_t)) - UNSECURE_CMD_CLS_CNT;

                    //Send security commands supported report using security message encapsulation
                    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 3 +
                                                             (sec_cmd_cnt * sizeof(uint16_t)));
                    if (tx_param)
                    {
                        int     i;
                        uint8_t *cmd_cls;
                        uint8_t prm_len;    //the length of the parameters

                        //tx_param->opp[0] = 3 + sec_cmd_cnt;   //length
                        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                        tx_param->opp[2] = SECURITY_COMMANDS_SUPPORTED_REPORT;
//#ifdef  TEST_SEC_WAKE_UP
//                        tx_param->opp[3] = 1;   //1 report to follow
//#else
                        tx_param->opp[3] = 0;   //no report to follow
//#endif
                        cmd_cls = tx_param->opp + 4;
                        prm_len = 0;
                        for (i=0; i<sec_cmd_cnt; i++)
                        {
                            if (supp_cmd_class[i] & 0xFF00)
                            {   //Extended command class
                                cmd_cls[prm_len++] = supp_cmd_class[i] >> 8;
                            }
                            cmd_cls[prm_len++] = supp_cmd_class[i] & 0x00FF;
                        }
                        tx_param->opp[0] = 3 + prm_len;

                        tx_param->cb = zwnet_sec_tx_cb;
                        tx_param->user[0] = NULL;
                        tx_param->use_0_nw_key = 0;
                        tx_param->nodeid = src_node;

                        ifd.nodeid = src_node;
                        ifd.net = nw;

                        result = zwsec_send(&ifd, tx_param);
                        free(tx_param);

                    }
                    else
                    {
                        result = ZW_ERR_MEMORY;
                    }

                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "send sec cmd sup report securely with error:%d", result);
                    }

//#ifdef  TEST_SEC_WAKE_UP
#if 0
                    //Send security commands supported report using security message encapsulation
                    tx_param = (zwsec_tx_sm_param_t *)malloc(sizeof(zwsec_tx_sm_param_t) + 5);
                    if (tx_param)
                    {
                        tx_param->opp[0] = 5;   //length
                        tx_param->opp[1] = COMMAND_CLASS_SECURITY;
                        tx_param->opp[2] = SECURITY_COMMANDS_SUPPORTED_REPORT;
                        tx_param->opp[3] = 0;   //report to follow
                        tx_param->opp[4] = COMMAND_CLASS_SWITCH_BINARY;
                        tx_param->opp[5] = COMMAND_CLASS_SWITCH_MULTILEVEL;

                        tx_param->cb = zwnet_sec_tx_cb;
                        tx_param->user[0] = NULL;
                        tx_param->use_0_nw_key = 0;
                        tx_param->nodeid = src_node;

                        ifd.nodeid = src_node;
                        ifd.net = nw;

                        result = zwsec_send(&ifd, tx_param);
                        free(tx_param);

                    }
                    else
                    {
                        result = ZW_ERR_MEMORY;
                    }

                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "send sec cmd sup report securely with error:%d", result);
                    }
#endif
                }
                return;
                break;
        }
    }
#endif

    //Display command received
    debug_zwapi_ts_msg(&nw->plt_ctx, "application_command_handler_cb from node:%u, status:%s, frame type:%s, cmd:",
                       (unsigned)src_node, sts_str[rx_sts & 0x03],
                       frame_type_str[(rx_sts & RECEIVE_STATUS_TYPE_MASK) >> 2]);
    debug_zwapi_bin_msg(&nw->plt_ctx, cmd_buf, cmd_len);


    //
    //Find the interface to handle the report
    //
    node = zwnode_find(&nw->ctl, src_node);
    if (node && (cmd_len >= 2))
    {
        uint8_t epid = 0;   //Default to virtual end point
        zwep_p  ep;
        //Determine if this is a multi-channel encapsulation command
        if (cmd_buf[0] == COMMAND_CLASS_MULTI_CHANNEL_V2)
        {
            //Strip off the header and adjust the cmd_len
            if (cmd_buf[1] == MULTI_CHANNEL_CMD_ENCAP_V2)
            {
                if (cmd_len <= 4)
                {   //No command class available after stripping off the header
                    return;
                }
                epid = cmd_buf[2];
                cmd_len -= 4;
                memmove(cmd_buf, cmd_buf + 4, cmd_len);
            }
            else if (cmd_buf[1] == MULTI_INSTANCE_CMD_ENCAP)
            {
                if (cmd_len <= 3)
                {   //No command class available after stripping off the header
                    return;
                }
                epid = cmd_buf[2];
                cmd_len -= 3;
                memmove(cmd_buf, cmd_buf + 3, cmd_len);
            }
        }
        //Find end point
        ep = zwep_find(&node->ep, epid);
        if (ep)
        {
            uint16_t    cls;

            cls = cmd_buf[0];

            if (cmd_buf[0] >= 0xF1)
            {   //Extended command class
                cls = (cls << 8) | cmd_buf[1];
            }

            //Find the interface that corresponds to the command class
            intf = zwif_find_cls(ep->intf, cls);

            if (intf)
            {
                //Check message type against interface security property
                if ((intf->propty & IF_PROPTY_SECURE) && ((intf->propty & IF_PROPTY_UNSECURE) == 0))
                {   //Should accept secure message only
                    if ((msg_type & ZWNET_MSG_TYPE_SECURE) == 0)
                    {
                        return;
                    }
                }


//              if (cmd_buf[0] == 0x20 && cmd_buf[1] == 3)
//              {
//                  return; //testing
//              }
                //Check whether the report is the one which the node is waiting for
                zwnode_wait_rpt_chk(intf, cmd_buf, cmd_len);

                //Check whether the report is the polling report
                zwpoll_rpt_chk(intf, cmd_buf, cmd_len);

                //Invoke the report handler
                zwif_rep_hdlr(intf, cmd_buf, cmd_len, rx_sts);
            }
        }
    }

    //Handle manufacturer specific command class request
    if (cmd_buf[0] == COMMAND_CLASS_MANUFACTURER_SPECIFIC)
    {
        int     result;
        if (cmd_buf[1] == MANUFACTURER_SPECIFIC_GET)
        {
            uint8_t c_buf[8];

            //Prepare the report
            c_buf[0] = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
            c_buf[1] = MANUFACTURER_SPECIFIC_REPORT;
            c_buf[2] = (uint8_t)(nw->ctl.vid >> 8);
            c_buf[3] = (uint8_t)(nw->ctl.vid & 0x00FF);
            c_buf[4] = (uint8_t)(nw->ctl.vtype >> 8);
            c_buf[5] = (uint8_t)(nw->ctl.vtype & 0x00FF);
            c_buf[6] = (uint8_t)(nw->ctl.pid >> 8);
            c_buf[7] = (uint8_t)(nw->ctl.pid & 0x00FF);

            //Send the report
            result = zwnet_rpt_send(nw, c_buf, 8, src_node, msg_type);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Send manf specific rpt with error:%d", result);
            }
        }
#ifdef MANF_SPEC_V2
        else if ((cmd_buf[1] == DEVICE_SPECIFIC_GET_V2) && (cmd_len >= 3))
        {
            uint8_t c_buf[32 + 4];//max. device id data length + terminating char (0xFF) + Device specific report overhead
            uint8_t ser_num_buf[32];
            //uint8_t dev_id_type;
            uint8_t dev_id_len;
            int     i;

#if 0
            //UTF-8 (for testing)
    #ifdef USE_SAFE_VERSION
            strcpy_s(c_buf + 4, 32,"sd123456789");
    #else
            strcpy(c_buf + 4, "sd123456789");
    #endif
            c_buf[4] = 0xC2;
            c_buf[5] = 0xA9;    //Copyright character
            c_buf[6] = 0xC2;
            c_buf[7] = 0xAE;    //Register trademark character

            dev_id_len = 11;

            //Prepare the report
            c_buf[0] = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
            c_buf[1] = DEVICE_SPECIFIC_REPORT_V2;
            c_buf[2] = cmd_buf[2] & 0x07;//Device ID type
            c_buf[3] = dev_id_len;//utf-8 format, data length

#else       //Binary

            //Always return Device ID Type = serial number
            //Get from the non-volatile memory
            result = zw_memory_get_buffer(&nw->appl_ctx, ser_num_buf, SERIAL_NUMBER_OFFSET, 32);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Get serial number from NVRAM with error:%d", result);
                return;
            }
            else
            {
                debug_zwapi_msg(&nw->plt_ctx, "Serial number from NVRAM:");
                debug_zwapi_bin_msg(&nw->plt_ctx, ser_num_buf, 32);
            }

            //Get device id length
            dev_id_len = 31;//maximum

            for (i=0; i<32; i++)
            {
                if (ser_num_buf[i] == 0xFF)
                {
                    dev_id_len = i;
                    break;
                }
            }

            //Prepare the report
            c_buf[0] = COMMAND_CLASS_MANUFACTURER_SPECIFIC;
            c_buf[1] = DEVICE_SPECIFIC_REPORT_V2;
            c_buf[2] = 1;//serial number
            c_buf[3] = 0x20 | dev_id_len;//binary format, data length
            memcpy(c_buf + 4, ser_num_buf, dev_id_len);

#endif
            //Send the report
            result = zwnet_rpt_send(nw, c_buf, dev_id_len + 4, src_node, msg_type);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Send device specific rpt with error:%d", result);
            }
        }
#endif
    }

    //Handle version command class request
    else if ((cmd_len >= 2) && (cmd_buf[0] == COMMAND_CLASS_VERSION))
    {
        int     result;
        uint8_t c_buf[8];

        switch (cmd_buf[1])
        {
            case VERSION_COMMAND_CLASS_GET:
                if (cmd_len >= 3)
                {
                    uint8_t cmd_cls_ver;

                    //Check whether the requested command class is valid
                    if (memchr(supp_cmd_class, cmd_buf[2], sizeof(supp_cmd_class)) ||
                        (cmd_buf[2] == COMMAND_CLASS_BASIC))
                    {
                        switch (cmd_buf[2])
                        {
#ifdef MANF_SPEC_V2
                            case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
#endif
                            case COMMAND_CLASS_ASSOCIATION:
                            case COMMAND_CLASS_ZWAVEPLUS_INFO:
                                cmd_cls_ver = 2;
                                break;

                            default:
                                cmd_cls_ver = 1;
                        }
                    }
                    else
                    {   //Invalid requested command class
                        cmd_cls_ver = 0;
                    }

                    //Prepare the report
                    c_buf[0] = COMMAND_CLASS_VERSION;
                    c_buf[1] = VERSION_COMMAND_CLASS_REPORT;
                    c_buf[2] = cmd_buf[2];
                    c_buf[3] = cmd_cls_ver;

                    //Send the report
                    result = zwnet_rpt_send(nw, c_buf, 4, src_node, msg_type);
                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "Send ver rpt with error:%d", result);
                    }
                }
                break;

            case VERSION_GET:
                {
                    //Prepare the report
                    c_buf[0] = COMMAND_CLASS_VERSION;
                    c_buf[1] = VERSION_REPORT;
                    c_buf[2] = nw->ctl_lib_type;// library type
                    c_buf[3] = (uint8_t)(nw->ctl.proto_ver >> 8); //protocol version
                    c_buf[4] = (uint8_t)(nw->ctl.proto_ver & 0xFF); //protocol sub-version
                    c_buf[5] = APPL_VERSION; //application version
                    c_buf[6] = APPL_SUBVERSION; //application sub-version

                    //Send the report
                    result = zwnet_rpt_send(nw, c_buf, 7, src_node, msg_type);
                    if (result < 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "Send ver rpt with error:%d", result);
                    }
                }
                break;
        }
    }

    //Handle version command class request
    else if ((cmd_len >= 2) && (cmd_buf[0] == COMMAND_CLASS_ZWAVEPLUS_INFO))
    {
        int     result;
        uint8_t c_buf[16];

        if (cmd_buf[1] == ZWAVEPLUS_INFO_GET)
        {
            //Prepare the report
            c_buf[0] = COMMAND_CLASS_ZWAVEPLUS_INFO;
            c_buf[1] = ZWAVEPLUS_INFO_REPORT;
            c_buf[2] = 1;//Zwave+ version
            c_buf[3] = ROLE_TYPE_CENTRAL_STATIC_CONTROLLER;//Role type
            c_buf[4] = NODE_TYPE_ZWAVEPLUS_NODE;//Node type
            c_buf[5] = 0;  //Installer icon type (MSB)
            c_buf[6] = 0;  //Installer icon type (LSB)
            c_buf[7] = 0;  //User icon type (MSB)
            c_buf[8] = 0;  //User icon type (LSB)

            //Send the report
            result = zwnet_rpt_send(nw, c_buf, 9, src_node, msg_type);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Send zwave+ info rpt with error:%d", result);
            }
        }
    }
	else if((cmd_len >= 2) && (cmd_buf[0] == COMMAND_CLASS_POWERLEVEL))
	{
		int result;
		if (cmd_buf[1] == POWERLEVEL_GET)
        {
			uint8_t c_buf[8] = {0};

            //Prepare the report
            c_buf[0] = COMMAND_CLASS_POWERLEVEL;
            c_buf[1] = POWERLEVEL_REPORT;

			result = zw_get_rf_powerlevel(prm->appl_ctx, &c_buf[2]);

			plt_mtx_lck(nw->mtx);
			c_buf[3] = nw->byPowerLvlTimeout; //Time out value
			plt_mtx_ulck(nw->mtx);

			if(result == 0)
			{
				//Send the report
				result = zwnet_rpt_send(nw, c_buf, 4, src_node, msg_type);
				if (result < 0)
				{
					debug_zwapi_msg(&nw->plt_ctx, "Send power level rpt with error:%d", result);
				}
			}
        }
		else if(cmd_buf[1] == POWERLEVEL_SET)
		{
			if(cmd_len >= 4)
			{
				uint8_t byPowerLvl = cmd_buf[2];
				uint8_t byTimeout = cmd_buf[3];

				if(byPowerLvl >= POWERLEVEL_SET_NORMALPOWER && byPowerLvl <= POWERLEVEL_SET_MINUS9DBM)
				{
					//Set the power level here. Reset power level at callback function.
					result = zw_set_rf_powerlevel(prm->appl_ctx, byPowerLvl, NULL);

					if(result == 0 && byTimeout != 0)
					{
						plt_mtx_lck(nw->mtx);
						if(nw->pPowerLvl_tmr_ctx != NULL)
						{
							//Already has a power level timer running. Stop it.
							if (plt_tmr_stop(&nw->plt_ctx, nw->pPowerLvl_tmr_ctx) == 0)
							{
								debug_zwapi_msg(&nw->plt_ctx, "Power level timer stop failed");
							}

							nw->byPowerLvlTimeout = 0;
							nw->pPowerLvl_tmr_ctx = NULL;
						}

						nw->byPowerLvlTimeout = byTimeout;

						nw->pPowerLvl_tmr_ctx = plt_periodic_start(&nw->plt_ctx,
																   1000,	//Every 1 sec callback to update the remaining time
																   zwnet_power_lvl_tmr_cb,
																   nw);
						plt_mtx_ulck(nw->mtx);
					}
				}
			}
		}
		else if(cmd_buf[1] == POWERLEVEL_TEST_NODE_GET)
		{
			uint8_t c_buf[8] = {0};

            //Prepare the report
			c_buf[0] = COMMAND_CLASS_POWERLEVEL;
			c_buf[1] = POWERLEVEL_TEST_NODE_REPORT;
			c_buf[2] = nw->byPowerTstNodeID; //If never tested before, it will be 0, which is ZW_TEST_NOT_A_NODEID;
			c_buf[3] = nw->byPowerTstStatus; //If never tested before, it will be 0, which is POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED
			c_buf[4] = (uint8_t)(nw->wPowerTstAckFrmCnt >> 8);
			c_buf[5] = (uint8_t)(nw->wPowerTstAckFrmCnt & 0x00FF);

			//Send the report
			result = zwnet_rpt_send(nw, c_buf, 6, src_node, msg_type);
			if (result < 0)
			{
				debug_zwapi_msg(&nw->plt_ctx, "Send power level rpt with error:%d", result);
			}
		}
		else if(cmd_buf[1] == POWERLEVEL_TEST_NODE_SET)
		{
			if(cmd_len >= 6)
			{
				uint8_t byTestNodeID = cmd_buf[2];
				uint8_t byPowerLvl = cmd_buf[3];
				uint16_t wTstFrmCnt = (cmd_buf[4] << 8) | cmd_buf[5];
				zwpowerlvl_tst_tmp_t *pstPowerTstData = (zwpowerlvl_tst_tmp_t *)calloc(1, sizeof(zwpowerlvl_tst_tmp_t));

				if(byPowerLvl >= POWERLEVEL_SET_NORMALPOWER && byPowerLvl <= POWERLEVEL_SET_MINUS9DBM)
				{

					if(wTstFrmCnt != 0)
					{
						//Set the power level here. Reset power level when the test has finish.
						result = zw_set_rf_powerlevel(prm->appl_ctx, byPowerLvl, NULL);

						if(result == 0)
						{
							nw->byPowerTstNodeID = byTestNodeID;
							nw->byPowerTstStatus = POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_INPROGRESS;
							nw->wPowerTstAckFrmCnt = 0;

							//Prepare the data and start to send test frames (through callback function).
							pstPowerTstData->byPowerLvl = byPowerLvl;
							pstPowerTstData->wPowerTstFrmCnt = wTstFrmCnt;
							pstPowerTstData->bysrc_node = src_node;
							pstPowerTstData->bymsg_type = msg_type;
							//The first tx status will be fake. Just any status but not completed ok to prevent
							//increasing the acknowledgement counter.
							zwnet_power_lvl_testframe_cb(prm->appl_ctx, TRANSMIT_ROUTING_NOT_IDLE, (void*)pstPowerTstData);
						}
					}
					//Reset to normal power will be done at the call back function when the whole test finishes.
				}
			}
		}
	}
	else if((cmd_len >= 2) && (cmd_buf[0] == COMMAND_CLASS_ASSOCIATION_GRP_INFO))
	{
		handle_association(cmd_len, cmd_buf, prm);
	}
	else if((cmd_len >= 2) && (cmd_buf[0] == COMMAND_CLASS_ASSOCIATION))
	{
		handle_association(cmd_len, cmd_buf, prm);
	}

#ifdef  TEST_SEC_WAKE_UP
    else if (cmd_buf[0] == COMMAND_CLASS_WAKE_UP &&
             cmd_buf[1] == WAKE_UP_INTERVAL_SET)
    {
        uint32_t secs;

        secs = (((uint32_t)cmd_buf[2]) << 16) | (((uint32_t)cmd_buf[3]) << 8) | ((uint32_t)cmd_buf[4]);
        nw->wkup_node_id = cmd_buf[5];
        nw->sec_wkup = (msg_type == ZWNET_MSG_TYPE_SECURE)? 1:0;

        if (secs > 0)
        {
            nw->wkup_tmr_ctx = plt_periodic_start(secs*1000, zwnet_wkup_tmout_cb, nw);
        }
        else
        {
            plt_tmr_stop(&nw->plt_ctx, nw->wkup_tmr_ctx);
            nw->wkup_tmr_ctx = NULL;

        }
    }
#endif
#if defined (TEST_UTF8_NAME_LOC) || defined (TEST_UTF16_NAME_LOC)
    else if (cmd_buf[0] == COMMAND_CLASS_NODE_NAMING &&
             (cmd_buf[1] == NODE_NAMING_NODE_NAME_SET || cmd_buf[1] == NODE_NAMING_NODE_LOCATION_SET))
    {
        if (cmd_buf[1] == NODE_NAMING_NODE_NAME_SET)
        {   //Store the node name
            nw->name_ch_fmt = cmd_buf[2] & 0x07;
            nw->nameloc_n_len = cmd_len - 3;
            memcpy(nw->nameloc_n, cmd_buf + 3, nw->nameloc_n_len);
        }
        else
        {   //Store the node location
            nw->loc_ch_fmt = cmd_buf[2] & 0x07;
            nw->nameloc_l_len = cmd_len - 3;
            memcpy(nw->nameloc_l, cmd_buf + 3, nw->nameloc_l_len);
        }

    }
#endif
#ifdef  TEST_UTF16_NAME_LOC
    else if (cmd_buf[0] == COMMAND_CLASS_NODE_NAMING &&
             (cmd_buf[1] == NODE_NAMING_NODE_NAME_GET || cmd_buf[1] == NODE_NAMING_NODE_LOCATION_GET))
    {
        int     result;
        uint8_t utf16_name_str[] = {0x79, 0x72, 0x8A, 0x6B, 0xA2, 0x7E, 0x72, 0x82, 0x03, 0x8C, 0x49, 0x51, 0xBE, 0x8B, 0x07, 0x59
        };
        uint8_t utf16_loc_str[] = {0xA2, 0x5B, 0x85, 0x53, 0xCC, 0x91, 0x84, 0x76, 0x27, 0x59, 0x99, 0x51, 0x57, 0x5B, 0xF0, 0x53
        };
        uint8_t c_buf[3 + ZW_DEV_LOC_STR_MAX];
        uint8_t rpt_str_len;

        //Prepare the report
        c_buf[0] = COMMAND_CLASS_NODE_NAMING;
        c_buf[2] = CHAR_PRES_UTF16;
        if (cmd_buf[1] == NODE_NAMING_NODE_NAME_GET)
        {
            c_buf[1] = NODE_NAMING_NODE_NAME_REPORT;
            if (nw->nameloc_n_len)
            {
                c_buf[2] = nw->name_ch_fmt;
                memcpy(c_buf + 3, nw->nameloc_n, nw->nameloc_n_len);
                rpt_str_len = nw->nameloc_n_len;
            }
            else
            {   //default
                memcpy(c_buf + 3, utf16_name_str, 16);
                rpt_str_len = 16;
                util_ntohs((uint16_t *)(c_buf + 3), 8);
            }
        }
        else
        {
            c_buf[1] = NODE_NAMING_NODE_LOCATION_REPORT;
            if (nw->nameloc_l_len)
            {
                c_buf[2] = nw->loc_ch_fmt;
                memcpy(c_buf + 3, nw->nameloc_l, nw->nameloc_l_len);
                rpt_str_len = nw->nameloc_l_len;
            }
            else
            {   //default
                memcpy(c_buf + 3, utf16_loc_str, 16);
                rpt_str_len = 16;
                util_ntohs((uint16_t *)(c_buf + 3), 8);
            }
        }

        //Send the report
        result = zwnet_rpt_send(nw, c_buf, 3 + rpt_str_len, src_node, msg_type);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Send nameloc rpt with error:%d", result);
        }
    }
#endif
#ifdef  TEST_UTF8_NAME_LOC
    else if (cmd_buf[0] == COMMAND_CLASS_NODE_NAMING &&
             (cmd_buf[1] == NODE_NAMING_NODE_NAME_GET || cmd_buf[1] == NODE_NAMING_NODE_LOCATION_GET))
    {
        int     result;
        uint8_t c_buf[3 + ZW_DEV_LOC_STR_MAX];
        uint8_t rpt_str_len;

        //Prepare the report
        c_buf[0] = COMMAND_CLASS_NODE_NAMING;
        c_buf[2] = CHAR_PRES_ASCII;
        if (cmd_buf[1] == NODE_NAMING_NODE_NAME_GET)
        {
            c_buf[1] = NODE_NAMING_NODE_NAME_REPORT;
            if (nw->nameloc_n_len)
            {
                c_buf[2] = nw->name_ch_fmt;
                memcpy(c_buf + 3, nw->nameloc_n, nw->nameloc_n_len);
                rpt_str_len = nw->nameloc_n_len;
            }
            else
            {   //default
                memcpy(c_buf + 3, "Binary Switch", 14);
                rpt_str_len = 14;
            }
        }
        else
        {
            c_buf[1] = NODE_NAMING_NODE_LOCATION_REPORT;
            if (nw->nameloc_l_len)
            {
                c_buf[2] = nw->loc_ch_fmt;
                memcpy(c_buf + 3, nw->nameloc_l, nw->nameloc_l_len);
                rpt_str_len = nw->nameloc_l_len;
            }
            else
            {   //default
                memcpy(c_buf + 3, "Anywhere ABCD", 14);
                rpt_str_len = 14;
            }

        }

        //Send the report
        result = zwnet_rpt_send(nw, c_buf, 3 + rpt_str_len, src_node, msg_type);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Send nameloc rpt with error:%d", result);
        }
    }
#endif
#ifdef  TEST_HANDLE_BASIC_COMMAND_CLASS
    //Handle basic command class request
    else if (cmd_buf[0] == COMMAND_CLASS_BASIC)
    {
        int         result;

        if (cmd_buf[1] == BASIC_GET)
        {
            uint8_t c_buf[3];

            //Prepare the report
            c_buf[0] = COMMAND_CLASS_BASIC;
            c_buf[1] = BASIC_REPORT;
            c_buf[2] = nw->basic_val;

            //Send the report
            result = zwnet_rpt_send(nw, c_buf, 3, src_node, msg_type);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Send basic rpt with error:%d", result);
            }
        }
        else if (cmd_buf[1] == BASIC_SET)
        {   //Store the value
            nw->basic_val = cmd_buf[2];
        }
    }
#endif
#ifdef  TEST_EXT_CMD_CLASS
    //Handle basic command class request
    else if (cmd_buf[0] == 0xF2 && cmd_buf[1] == 0x12)
    {
        int         result;

        if (cmd_buf[2] == BASIC_GET)
        {
            uint8_t c_buf[4];

            //Prepare the report
            c_buf[0] = 0xF2;
            c_buf[1] = 0x12;
            c_buf[2] = BASIC_REPORT;
            c_buf[3] = nw->basic_val;

            //Send the report
            result = zwnet_rpt_send(nw, c_buf, 4, src_node, msg_type);
            if (result < 0)
            {
                debug_zwapi_msg(&nw->plt_ctx, "Send extended basic rpt with error:%d", result);
            }
        }
        else if (cmd_buf[2] == BASIC_SET)
        {   //Store the value
            nw->basic_val = cmd_buf[3];
        }
    }
#endif

    //
    //Send acknowledgement on receiving replication command
    //
    else if ((cmd_len >= 1) &&
             cmd_buf[0] == COMMAND_CLASS_CONTROLLER_REPLICATION)
    {
        zw_replication_receive_complete(prm->appl_ctx);
    }

}


/**
application_command_handler_cb - Application command handler callback function
@param[in]	appl_ctx    The application layer context
@param[in]	rx_sts		Receive status
@param[in]	src_node    Node id of the sender
@param[in]	rx_len      The length of the received command
@param[in]	rx_buf      Receive buffer
@return
*/
static void    application_command_handler_cb(struct _appl_layer_ctx   *appl_ctx,
                                              uint8_t rx_sts, uint8_t src_node,
                                              uint8_t rx_len, uint8_t *rx_buf)
{
    appl_cmd_prm_t  prm;
    int         wait_2nd_frame = 0;
    int         node_sec_ena = 1;   //flag to indicate node is allowed to respond to security commands
    zwnet_p     nw;
    zwnode_p    node;
    uint8_t     msg_type;         //incoming message type
    uint8_t     cmd_len;
    uint8_t     cmd_buf[80];

    nw = (zwnet_p)appl_ctx->data;
    msg_type = 0;

#ifdef  SUPPORT_SECURITY
    //Work around for early Security Scheme Get Command
    if (nw->sec_chk && rx_len >= 3)
    {
        if (rx_buf[0] == COMMAND_CLASS_SECURITY
            && rx_buf[1] == SECURITY_SCHEME_GET)
        {
            //Save the source node id & scheme
            nw->sec_node_id = src_node;
            nw->sec_scheme = rx_buf[2];

            //Stop checking of SECURITY_SCHEME_GET command
            nw->sec_chk = 0;

        }
    }

    //Check whether the node has been marked as security inclusion failed
    //or the controller itself is not in secure network
    node = zwnode_find(&nw->ctl, src_node);
    if (node)
    {
        node_sec_ena = ((node->sec_incl_failed && !(node->sec_incl_failed == 2)) || nw->ctl.sec_incl_failed)? 0 : 1;
        printf("====================================================> check sec %d, %d, sec_enable%d \n",node->sec_incl_failed,nw->ctl.sec_incl_failed, node_sec_ena);
    }

    //Check whether this is a security message encapsulation command
    if (nw->sec_enable && node_sec_ena
        && (rx_buf[0] == COMMAND_CLASS_SECURITY)
        && ((rx_buf[1] & 0x81) == 0x81))
    {
        printf("==================> go here =========!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        int result;
        uint8_t *opp;

        //Decrypt the message
        result = zwsec_depacket(nw, src_node, rx_len, rx_buf, &opp);
        if (result == 0)
        {
            msg_type = ZWNET_MSG_TYPE_SECURE;
            cmd_len = opp[0];
            memcpy(cmd_buf, opp + 1, cmd_len);
            free(opp);
        }
        else if (result == 1)
        {   //Waiting for second frame
            wait_2nd_frame = 1;
            cmd_len = rx_len;
            memcpy(cmd_buf, rx_buf, cmd_len);
        }
        else
        {
            debug_zwapi_ts_msg(&nw->plt_ctx, "Security package depacketization error:%d", result);
            return;
        }

    }
    else
#endif
    {   //Non-secure message

#ifdef CRC16_ENCAP
        //Check for CRC-16 encapsulation
        if ((rx_len > CRC16_OVERHEAD) &&
            (rx_buf[0] == COMMAND_CLASS_CRC_16_ENCAP) &&
            (rx_buf[1] == CRC_16_ENCAP))
        {
            //Check for data integrity
            if (zwutl_crc16_chk(CRC_INIT, rx_buf, rx_len) == 0)
            {
                cmd_len = rx_len - CRC16_OVERHEAD;
                memcpy(cmd_buf, rx_buf + 2, cmd_len);
                msg_type = ZWNET_MSG_TYPE_CRC16;
            }
            else
            {
                debug_zwapi_ts_msg(&nw->plt_ctx, "CRC-16 checksum failed");
                return;
            }

        }
        else
        {
#endif
            cmd_len = rx_len;
            memcpy(cmd_buf, rx_buf, cmd_len);
#ifdef CRC16_ENCAP
        }
#endif
    }
    //
    //Check for Multi Command Encapsulated Command
    //

    prm.msg_type = msg_type;
    prm.node_sec_ena = node_sec_ena;
    prm.nw = nw;
    prm.rx_sts = rx_sts;
    prm.src_node = src_node;
    prm.wait_2nd_frame = wait_2nd_frame;
    prm.appl_ctx = appl_ctx;

    if ((cmd_buf[0] == COMMAND_CLASS_MULTI_CMD) && (cmd_buf[1] == MULTI_CMD_ENCAP))
    {
        uint8_t     *cmd_ptr;
        uint8_t     cmd_cnt;
        uint8_t     bytes_to_process;
        uint8_t     enc_cmd_len;
        uint8_t     i;

        if (cmd_len >= 3)
        {
            cmd_cnt = cmd_buf[2];
            if (cmd_cnt > 0)
            {
                bytes_to_process = cmd_len - 3;
                cmd_ptr = cmd_buf + 3;
                for (i=0; i < cmd_cnt; i++)
                {
                    if (bytes_to_process > 0)
                    {
                        enc_cmd_len = *cmd_ptr++;
                        bytes_to_process--;

                        //Check whether there are enough bytes for the encapsulated command
                        if (bytes_to_process >= enc_cmd_len)
                        {
                            appl_cmd_hdlr(enc_cmd_len, cmd_ptr, &prm);

                            //Adjustment
                            cmd_ptr += enc_cmd_len;
                            bytes_to_process -= enc_cmd_len;

                        }
                        else
                        {   //Error
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {   //Single command
        appl_cmd_hdlr(cmd_len, cmd_buf, &prm);
    }
}


/**
zwnet_ctlr_info_get - Get the controller info from the non-volatile memory.
@param[in, out]	nw	                The Z-wave network context
@param[in]	    is_create_nw	    Whether to create network structure in system memory.
                                    1 = create, 0 = don't create.
@return  0 on success, negative error number on failure
*/
int32_t zwnet_ctlr_info_get(zwnet_p nw, uint8_t is_create_nw)
{
    int             result;
    int             i;
    int             j;
    appl_hci_cap_t  hci_cap;
    appl_init_dat_t init_dat;
    static const char    *lib_types[] =
    {   "unknown",
        "static controller",
        "controller",
        "enhanced slave",
        "slave",
        "installer",
        "routing slave",
        "bridge controller",
        "dut"
    };

    static const char    *ctlr_nw_role[] =
    {   "secondary",
        "on other nw",
        "node id server present",
        "real primary",
        "suc",
        "no nodes included",
        "unknown",
        "unknown"
    };
    uint8_t     suc_node_id;

    //Get home id
    result = zw_memory_get_id(&nw->appl_ctx, &nw->homeid, &nw->ctl.nodeid);
    if (result == 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "Home id:%08X, node id:%u", nw->homeid,
                        (unsigned)nw->ctl.nodeid);
    }
    else
    {
        debug_zwapi_msg(&nw->plt_ctx, "zw_memory_get_id with error:%d", result);
        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
    }

    //Get lib type
    result = zw_version(&nw->appl_ctx, nw->ctl.basis_api_ver,
                        &nw->ctl_lib_type);

    if (result == 0)
    {
        if (nw->ctl_lib_type > ZW_LIB_DUT)
        {
            nw->ctl_lib_type = 0;
        }
        debug_zwapi_msg(&nw->plt_ctx, "Basis api ver:%s, lib type:%s",
                        nw->ctl.basis_api_ver, lib_types[nw->ctl_lib_type]);

    }
    else
    {
        debug_zwapi_msg(&nw->plt_ctx, "zw_version with error:%d", result);
        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
    }

    //Get the host controller interface (serial api) capabilities
    result = zw_serial_api_capabilities(&nw->appl_ctx, &hci_cap);

    if (result == 0)
    {
        char        hex_str[256];
        uint16_t    serial_app_ver;

        nw->ctl.vid = hci_cap.manf_id;
        nw->ctl.vtype = hci_cap.manf_pdt_typ;
        nw->ctl.pid = hci_cap.manf_pdt_id;
        serial_app_ver = ((uint16_t)hci_cap.version) << 8 | hci_cap.revision;

        debug_zwapi_msg(&nw->plt_ctx, "HCI capabilities:");
        debug_zwapi_msg(&nw->plt_ctx, "App version:%04X, manufacturer id:%04X",
                        (unsigned)serial_app_ver, (unsigned)hci_cap.manf_id);

        debug_zwapi_msg(&nw->plt_ctx, "manufacturer product type:%04X, manufacturer product id:%04X",
                        (unsigned)hci_cap.manf_pdt_typ,
                        (unsigned)hci_cap.manf_pdt_id);
        debug_zwapi_msg(&nw->plt_ctx, "Supported function id:");
        hex_str[0] = '\0';
        nw->ctlr_cap = 0;   //reset controller's capabilities
        for (i = 0; i < 256; i++)
        {
            if (((hci_cap.func_id[i>>3]) >> (i & 7)) & 0x01)
            {
                if ((i+1) == FUNC_ID_ZW_ENABLE_SUC)
                {
                    nw->ctlr_cap |= CTLR_CAP_ENA_SUC;
                }
                else if ((i+1) == FUNC_ID_ZW_SET_SUC_NODE_ID)
                {
                    nw->ctlr_cap |= CTLR_CAP_SET_SUC;
                }
                else if ((i+1) == FUNC_ID_AUTO_PROGRAMMING)
                {
                    nw->ctlr_cap |= CTLR_CAP_FLASH_PROG;
                }

                util_hex_string_add(hex_str, 256, (unsigned)(i+1));
                if (strlen(hex_str) > 100)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "%s", hex_str);
                    hex_str[0] = '\0';
                }
            }
        }
        debug_zwapi_msg(&nw->plt_ctx, "%s", hex_str);
    }
    else
    {
        debug_zwapi_msg(&nw->plt_ctx, "zw_serial_api_capabilities with error:%d", result);
        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
    }

    //Get the initialization data stored in non-volatile memory
    result = zw_serial_api_get_init_data(&nw->appl_ctx, &init_dat);

    if (result == 0)
    {
        int     total_nodes = 0;
        char    num_str[256];

        debug_zwapi_msg(&nw->plt_ctx, "Initialization data:");
        debug_zwapi_msg(&nw->plt_ctx, "Serial API version:%u , capabilities:%u, chip type:%02X, chip ver:%02X",
                        (unsigned)init_dat.version, (unsigned)init_dat.cap,
                        (unsigned)init_dat.chip_typ, (unsigned)init_dat.chip_ver);
        debug_zwapi_msg(&nw->plt_ctx, "Stored node id:");

        num_str[0] = '\0';
        for (i = j = 0; i < 232; i++)
        {
            if (((init_dat.nodes[i>>3]) >> (i & 7)) & 0x01)
            {
                total_nodes++;
                util_num_string_add(num_str, 256, (unsigned)(i+1));
                if (j++ > 30)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "%s", num_str);
                    j = 0;
                    num_str[0] = '\0';
                }
            }
        }
        debug_zwapi_msg(&nw->plt_ctx, "%s", num_str);

        if (is_create_nw)
        {   //Create the network structure in the system memory
            //Display detailed info about the stored nodes

            plt_mtx_lck(nw->mtx);

            for (i = 0; i < 232; i++)
            {
                if (((init_dat.nodes[i>>3]) >> (i & 7)) & 0x01)
                {
                    uint8_t node_info[8];
                    result = zw_get_node_protocol_info(&nw->appl_ctx, (i+1),
                                                       node_info);

                    if (result == 0)
                    {
                        if (nw->ctl.nodeid == (i+1))
                        {   //Controller own node id
                            nw->ctl.capability = node_info[0];
                            nw->ctl.security = node_info[1];
                            nw->ctl.basic = node_info[3];
                            nw->ctl.alive = 1;

                            //Init end point
                            nw->ctl.ep.epid = VIRTUAL_EP_ID;
                            nw->ctl.ep.node = &nw->ctl;
                            nw->ctl.ep.generic = node_info[4];
                            nw->ctl.ep.specific = node_info[5];
                            nw->ctl.ep.intf = NULL;
                        }
                        else
                        {   //Add other new node
                            zwnode_p     node;

                            node = (zwnode_p)calloc(1, sizeof(zwnode_t));
                            if (!node)
                            {
                                plt_mtx_ulck(nw->mtx);
                                return ZW_ERR_MEMORY;
                            }

                            node->alive = 1;

                            //Init all the back links
                            node->net = nw;
                            node->ep.node = node;

                            node->nodeid = (i+1);
                            node->capability = node_info[0];
                            node->security = node_info[1];
                            //node->security = 0x60;//Testing
                            node->basic = node_info[3];

                            //Init end point
                            node->ep.epid = VIRTUAL_EP_ID;
                            node->ep.generic = node_info[4];
                            node->ep.specific = node_info[5];
                            node->ep.intf = NULL;

                            zwobj_add(&nw->ctl.obj.next, &node->obj);

                        }
                        debug_zwapi_msg(&nw->plt_ctx, "Node id:%d, capability:%02X, security:%02X, basic:%02X, generic:%02X, specific:%02X,"
                                        , i+1, node_info[0], node_info[1], node_info[3], node_info[4], node_info[5]);
                    }
                    else
                    {
                        plt_mtx_ulck(nw->mtx);
                        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
                    }
                }
            }
            plt_mtx_ulck(nw->mtx);

            //Get SUC node id
            result = zw_get_suc_node_id(&nw->appl_ctx, &suc_node_id);
            if (result != 0)
            {
                return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
            }

            if (suc_node_id == 0)
            {
                //Get controller network role
                result = zw_get_controller_capabilities(&nw->appl_ctx, &nw->ctl.ctlr_nw_role);

                if (result == 0)
                {
//                  debug_zwapi_msg(&nw->plt_ctx, "previous controller network role:");
//                  for (i=0 ; i<8; i++)
//                  {
//                      if ((nw->ctl.ctlr_nw_role >> i) & 0x01)
//                      {
//                          debug_zwapi_msg(&nw->plt_ctx, "%s",ctlr_nw_role[i]);
//                      }
//                  }

                }
                else
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zw_get_controller_capabilities with error:%d", result);
                    return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
                }

                if ((total_nodes == 1) || (nw->ctl.ctlr_nw_role & CONTROLLER_IS_REAL_PRIMARY))
                {
                    result = zwnet_assign_ex(nw, nw->ctl.nodeid, ZWNET_ROLE_SIS, NULL);

                    if (result != 0)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_assign_ex with error:%d", result);
                        return result;
                    }
                }
            }
        }

        //Get controller network role
        result = zw_get_controller_capabilities(&nw->appl_ctx, &nw->ctl.ctlr_nw_role);

        if (result == 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "controller network role:");
            for (i=0 ; i<8; i++)
            {
                if ((nw->ctl.ctlr_nw_role >> i) & 0x01)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "%s",ctlr_nw_role[i]);
                }
            }

        }
        else
        {
            debug_zwapi_msg(&nw->plt_ctx, "zw_get_controller_capabilities with error:%d", result);
            return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
        }

        //Get SUC node id
        result = zw_get_suc_node_id(&nw->appl_ctx, &suc_node_id);
        if (result == 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "SUC node id:%u", suc_node_id);
        }
        else
        {
            return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
        }

    }
    else
    {
        debug_zwapi_msg(&nw->plt_ctx, "zw_serial_api_get_init_data with error:%d", result);
        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
    }
    return 0;
}

#define APPLICATION_NODEINFO_NOT_LISTENING            0x00
#define APPLICATION_NODEINFO_LISTENING                0x01
#define APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY   0x02
#define APPLICATION_FREQ_LISTENING_MODE_1000ms        0x10

/**
zwnet_node_info_set - Set the controller node information in the non-volatile memory
@param[in]	net	        Network
@param[in]	cmd_class	The command classes to be set
@param[in]	cmd_cnt	    command class count
@return  0 on success, negative error number on failure
*/
static int32_t zwnet_node_info_set(zwnet_p net, const uint16_t *cmd_class, uint8_t cmd_cnt)
{
    appl_node_info_t    node_info;

    node_info.basic = BASIC_TYPE_STATIC_CONTROLLER;
    node_info.gen = GENERIC_TYPE_STATIC_CONTROLLER;
    node_info.spec = SPECIFIC_TYPE_PC_CONTROLLER;
    node_info.cmd_cnt = cmd_cnt;
    node_info.cmd_cls = (uint16_t *)cmd_class;

    return application_node_info(&net->appl_ctx, &node_info,
                                 APPLICATION_NODEINFO_LISTENING
                                 | APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY);
}


#ifdef  TEST_WRITE_SERIAL_NUM
static uint8_t serial_num[] = { 0x5A, 0x31, 0x04, 0x13, 0x25, 0x37, 0x49, 0x2B, 0x92, 0xDA, 0xFF
};

static void write_cmplt_cb(struct _appl_layer_ctx *appl_ctx, void *user_prm)
{
    //uint8_t *data = (uint8_t *)user_prm;

    debug_zwapi_msg(appl_ctx->plt_ctx, "Write completed to NVMEM, with data:");
    debug_zwapi_bin_msg(appl_ctx->plt_ctx, user_prm, sizeof(serial_num));
}

#endif


/**
zwnet_dev_cfg_load - Load and store device specific configurations
@param[in]	dev_spec_cfg	Device specific configurations unsorted
@param[out]	dev_cfg_bufs	Device specific configurations sorted according to device matching priorities
@return		ZW_ERR_xxx
@pre    dev_cfg_bufs must be cleared by the caller
*/
static int zwnet_dev_cfg_load(dev_spec_cfg_t  *dev_spec_cfg, dev_cfg_buf_t *dev_cfg_bufs)
{
    size_t      rec_buf_sz;
    int         conf_entries[4] = {0, 0, 0, 0};
    int         i;
    int         priority_num;
    uint32_t    dev_rec_msk;

    if (dev_spec_cfg->dev_cfg_ver != 1)
    {
        return ZW_ERR_VERSION;
    }

    //Calculate the number of entries in each sorted buffer
    for (i=0; i<dev_spec_cfg->dev_cfg_cnt; i++)
    {
        dev_rec_msk = dev_spec_cfg->dev_cfg_buf[i].fld_bitmsk & DEV_REC_SEARCH_KEY_MSK;

        if (dev_rec_msk == DEV_REC_SEARCH_KEY_MSK)
        {   //Highest priority for device matching in the format  (Manf id, Product Type, Product id):
             //(V, V, V) where V=valid value; X=don't care
            conf_entries[0]++;
        }
        else if (dev_rec_msk == (DEV_REC_MSK_VID | DEV_REC_MSK_PTYPE))
        {   //Second highest priority : (V, V, X)
            conf_entries[1]++;
        }
        else if (dev_rec_msk == DEV_REC_MSK_VID)
        {   //Third highest priority : (V, X, X)
            conf_entries[2]++;
        }
        else if (dev_rec_msk == 0)
        {   //Lowest priority (matches every device) : (X, X, X)
            conf_entries[3]++;
        }
    }

    //Allocate memory for each sorted buffer
    for (i=0; i<4; i++)
    {
        if (conf_entries[i] > 0)
        {
            rec_buf_sz = conf_entries[i] * sizeof(dev_rec_t);

            dev_cfg_bufs[i].dev_cfg_buf = (dev_rec_t *)malloc(rec_buf_sz);

            if (!dev_cfg_bufs[i].dev_cfg_buf)
            {
                //Free all buffers
                int j;

                for (j=0; j<4; j++)
                {
                    free(dev_cfg_bufs[j].dev_cfg_buf);
                    dev_cfg_bufs[j].dev_cfg_buf = NULL;
                }
                return ZW_ERR_MEMORY;
            }
        }
    }

    //Storing device specific configurations sorted according to device matching priorities
    for (i=0; i<dev_spec_cfg->dev_cfg_cnt; i++)
    {
        dev_rec_msk = dev_spec_cfg->dev_cfg_buf[i].fld_bitmsk & DEV_REC_SEARCH_KEY_MSK;

        priority_num = -1;

        if (dev_rec_msk == DEV_REC_SEARCH_KEY_MSK)
        {   //Highest priority for device matching in the format  (Manf id, Product Type, Product id):
             //(V, V, V) where V=valid value; X=don't care
            priority_num = 0;
        }
        else if (dev_rec_msk == (DEV_REC_MSK_VID | DEV_REC_MSK_PTYPE))
        {   //Second highest priority : (V, V, X)
            priority_num = 1;
        }
        else if (dev_rec_msk == DEV_REC_MSK_VID)
        {   //Third highest priority : (V, X, X)
            priority_num = 2;
        }
        else if (dev_rec_msk == 0)
        {   //Lowest priority (matches every device) : (X, X, X)
            priority_num = 3;
        }

        if (priority_num >= 0)
        {
            dev_cfg_bufs[priority_num].dev_cfg_buf[dev_cfg_bufs[priority_num].dev_cfg_cnt++] = dev_spec_cfg->dev_cfg_buf[i];
        }
    }

    return ZW_ERR_NONE;
}


/**
zwnet_node_probe_cb - send NOP callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void  zwnet_node_probe_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnoded_p noded = (zwnoded_p)user_prm;
    zwnet_p   nw = noded->net;

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Node:%u is alive", noded->nodeid);
    }
    else
    {
        //Send request for failed node removal
        zwnet_exec_req_t     rm_failed_req = {0};

        rm_failed_req.node_id = noded->nodeid;
        rm_failed_req.action = EXEC_ACT_RMV_FAILED;
        debug_zwapi_msg(appl_ctx->plt_ctx, "Send NOP with tx status:%u", (unsigned)tx_sts);

        util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                      (uint8_t *)&rm_failed_req, sizeof(zwnet_exec_req_t));
        plt_sem_post(nw->nw_exec_sem);
    }
    free(noded);
}


/**
zwnet_restart_node_probe_cb - send NOP callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void  zwnet_restart_node_probe_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    unsigned    nodeid = (unsigned)user_prm;
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    zwnode_p    node;
    zwnoded_t   noded;

    if (nodeid == 0)
    {
        return;
    }

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, nodeid);

    if (!node)
    {
        plt_mtx_ulck(nw->mtx);
        return;
    }

    zwnode_get_desc(node, &noded);

    plt_mtx_ulck(nw->mtx);

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Node:%u is alive", nodeid);
        node->restart_cb(&noded, ZW_FW_UPDT_RESTART_OK);
    }
    else if (--node->poll_tgt_cnt > 0)
    {
        //Send request for another probe
        zwnet_exec_req_t     req = {0};

        req.node_id = nodeid;
        req.action = EXEC_ACT_PROBE_RESTART_NODE;

        debug_zwapi_msg(appl_ctx->plt_ctx, "zwnet_restart_node_probe_cb: Send NOP with tx status:%u", (unsigned)tx_sts);

        util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                      (uint8_t *)&req, sizeof(zwnet_exec_req_t));
        plt_sem_post(nw->nw_exec_sem);
    }
    else
    {   //Report failure
        debug_zwapi_msg(appl_ctx->plt_ctx, "zwnet_restart_node_probe_cb: Send NOP with tx status:%u", (unsigned)tx_sts);
        node->restart_cb(&noded, ZW_FW_UPDT_RESTART_FAILED);
    }
}


/**
zwnet_exec_thrd - Thread to execute certain operations
@param[in]	data		Network
@return
*/
static void zwnet_exec_thrd(void *data)
{
    int                 result;
    zwnet_p             nw = (zwnet_p)data;
    zwnode_p            node;
    util_lst_t          *req_lst;
    zwnet_exec_req_t    *exec_req;
    uint8_t             node_id;

    nw->nw_exec_thrd_sts = 1;
    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->nw_exec_sem);

        //Check whether to exit the thread
        if (nw->nw_exec_thrd_run == 0)
        {
            nw->nw_exec_thrd_sts = 0;
            return;
        }

        while ((req_lst = util_list_get(nw->nw_exec_mtx, &nw->nw_exec_req_hd)) != NULL)
        {
            exec_req = (zwnet_exec_req_t *)req_lst->wr_buf;

            //Check whether to exit the thread
            if (nw->nw_exec_thrd_run == 0)
            {
                free(req_lst);
                nw->nw_exec_thrd_sts = 0;
                return;
            }

            //Get the node id of the request
            node_id = exec_req->node_id;

            switch (exec_req->action)
            {
                //-------------------------------------------------
                case EXEC_ACT_PROBE_FAILED_NODE:
                //-------------------------------------------------
                    //Sleep to allow the sender of DEVICE_RESET_LOCALLY_NOTIFICATION
                    //to reset the device
                    plt_sleep(3000);

                    plt_mtx_lck(nw->mtx);

                    node = zwnode_find(&nw->ctl, node_id);

                    if (node)
                    {
                        zwnoded_p noded;

                        //Check for any network operation going on
                        if (nw->curr_op != ZWNET_OP_NONE)
                        {   //Busy, re-schedule the send NOP operation
                            plt_mtx_ulck(nw->mtx);
                            util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                          (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                            break;
                        }

                        noded = (zwnoded_p) calloc(1, sizeof(zwnoded_t));
                        if (noded)
                        {
                            result = zwnode_get_desc(node, noded);
                            if (result == 0)
                            {
                                result = zwnode_probe(node, zwnet_node_probe_cb, noded);
                                if (result < 0)
                                {
                                    free(noded);
                                    if (result == ZW_ERR_BUSY)
                                    {   //Send wait failed, re-schedule the send NOP operation
                                        util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                                      (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                                    }
                                }
                            }
                        }
                    }

                    plt_mtx_ulck(nw->mtx);
                    break;

                //-------------------------------------------------
                case EXEC_ACT_RMV_FAILED:
                //-------------------------------------------------
                    plt_mtx_lck(nw->mtx);

                    node = zwnode_find(&nw->ctl, node_id);

                    if (node)
                    {
                        zwnoded_t noded;

                        //Check for any network operation going on
                        if (nw->curr_op != ZWNET_OP_NONE)
                        {   //Busy, re-schedule the removed failed node operation
                            plt_mtx_ulck(nw->mtx);
                            util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                          (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                            plt_sleep(3000);

                            break;
                        }
                        result = zwnode_get_desc(node, &noded);
                        if (result == 0)
                        {
                            zwnet_fail(&noded, 0);
                        }
                    }

                    plt_mtx_ulck(nw->mtx);
                    break;

                //-------------------------------------------------
                case EXEC_ACT_PROBE_RESTART_NODE:
                //-------------------------------------------------
                {
                    zwnoded_t noded;

                    plt_sleep(FW_UPDT_RESTART_POLL_INTERVAL);

                    plt_mtx_lck(nw->mtx);

                    node = zwnode_find(&nw->ctl, node_id);

                    if (node)
                    {
                        //Check for any network operation going on
                        if (nw->curr_op != ZWNET_OP_NONE)
                        {   //Busy, re-schedule the send NOP operation
                            plt_mtx_ulck(nw->mtx);
                            util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                          (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                            break;
                        }

                        result = zwnode_probe(node, zwnet_restart_node_probe_cb, (void *)((unsigned)node_id));
                        if (result == 0)
                        {   //o.k.
                            plt_mtx_ulck(nw->mtx);
                            break;
                        }
                        else if (result == ZW_ERR_BUSY)
                        {   //Re-schedule the send NOP operation
                            plt_mtx_ulck(nw->mtx);
                            util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                          (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                            break;
                        }
                    }

                    result = zwnode_get_desc(node, &noded);
                    plt_mtx_ulck(nw->mtx);
                    if (result == 0)
                    {
                        //Report failure
                        node->restart_cb(&noded, ZW_FW_UPDT_RESTART_FAILED);
                    }

                }
                break;

                //-------------------------------------------------
                case EXEC_ACT_CFG_LIFELINE:
                //-------------------------------------------------
                    {
                        zwnode_p            node;
                        zwep_p              ep;
                        zwif_p              intf;
                        if_grp_info_dat_t   *grp_data;

                        plt_mtx_lck(nw->mtx);

                        //Check for any network operation going on
                        if (nw->curr_op != ZWNET_OP_NONE)
                        {   //Busy, re-schedule the operation
                            plt_mtx_ulck(nw->mtx);
                            util_list_add(nw->nw_exec_mtx, &nw->nw_exec_req_hd,
                                          (uint8_t *)exec_req, sizeof(zwnet_exec_req_t));
                            plt_sleep(3000);

                            break;
                        }

                        //Configure lifeline to always-on-slaves (AOS)
                        if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
                        {
                            while (node)
                            {
                                //Check whether it is an always-on-slave
                                if (node->ep.zwplus_info.role_type == ROLE_TYPE_ALWAYS_ON_SLAVE)
                                {
                                    ep = &node->ep;

                                    while (ep)
                                    {
                                        //Search for AGI interface
                                        intf = zwif_find_cls(ep->intf, COMMAND_CLASS_ASSOCIATION_GRP_INFO);
                                        if (intf && intf->tmp_data)
                                        {
                                            grp_data = (if_grp_info_dat_t *)intf->tmp_data;
                                            if (grp_data->group_cnt && grp_data->grp_info[0])
                                            {
                                                if (strcasecmp(grp_data->grp_info[0]->name, "LIFELINE") == 0)
                                                {
                                                    zwnet_sm_lifeline_cfg(nw, intf);
                                                }
                                            }
                                        }
                                        ep = (zwep_p)ep->obj.next;
                                    }
                                }
                                node = (zwnode_p)node->obj.next;
                            }
                        }
                        plt_mtx_ulck(nw->mtx);
                    }
                    break;
            }
            free(req_lst);
        }
    }
}


/**
zwnet_cb_thrd - Thread to execute certain operations
@param[in]	data		Network
@return
*/
static void zwnet_cb_thrd(void *data)
{
    zwnet_p         nw = (zwnet_p)data;
    util_lst_t      *req_lst;
    zwnet_cb_req_t  *cb_req;
    zwif_p          intf;

    nw->cb_thrd_sts = 1;
    while (1)
    {
        //Wait for a request
        plt_sem_wait(nw->cb_sem);

        //Check whether to exit the thread
        if (nw->cb_thrd_run == 0)
        {
            nw->cb_thrd_sts = 0;
            return;
        }

        while ((req_lst = util_list_get(nw->cb_mtx, &nw->cb_req_hd)) != NULL)
        {
            //Check whether to exit the thread
            if (nw->cb_thrd_run == 0)
            {
                nw->cb_thrd_sts = 0;
                return;
            }

            cb_req = (zwnet_cb_req_t *)req_lst->wr_buf;

            //Check for cached data
            plt_mtx_lck(nw->mtx);
            intf = zwif_get_if(&cb_req->ifd);
            if (intf && intf->data_cnt && cb_req->cb)
            {
                switch (cb_req->rpt_type)
                {
                    //-------------------------------------------------
                    case CB_RPT_TYP_THRMO_FAN_MD:
                    //-------------------------------------------------
                        {
                            if_thrmo_fan_mode_data_t    *thrmo_fan_mode_dat;
                            zwrep_thrmo_fan_md_sup_fn   rpt_cb = (zwrep_thrmo_fan_md_sup_fn)cb_req->cb;

                            thrmo_fan_mode_dat = (if_thrmo_fan_mode_data_t *)malloc(intf->data_item_sz);

                            if (thrmo_fan_mode_dat)
                            {
                                memcpy(thrmo_fan_mode_dat, intf->data, intf->data_item_sz);

                                plt_mtx_ulck(nw->mtx);
                                rpt_cb(&cb_req->ifd, thrmo_fan_mode_dat->off, thrmo_fan_mode_dat->mode_len, thrmo_fan_mode_dat->mode);
                                free(thrmo_fan_mode_dat);
                            }
                            else
                            {
                                plt_mtx_ulck(nw->mtx);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_THRMO_MD:
                    //-------------------------------------------------
                        {
                            if_thrmo_mode_data_t    *thrmo_mode_dat;
                            zwrep_thrmo_md_sup_fn   rpt_cb = (zwrep_thrmo_md_sup_fn)cb_req->cb;

                            thrmo_mode_dat = (if_thrmo_mode_data_t *)malloc(intf->data_item_sz);

                            if (thrmo_mode_dat)
                            {
                                memcpy(thrmo_mode_dat, intf->data, intf->data_item_sz);

                                plt_mtx_ulck(nw->mtx);
                                rpt_cb(&cb_req->ifd, thrmo_mode_dat->mode_len, thrmo_mode_dat->mode);
                                free(thrmo_mode_dat);
                            }
                            else
                            {
                                plt_mtx_ulck(nw->mtx);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_THRMO_SETPOINT:
                    //-------------------------------------------------
                        {
                            if_thrmo_setp_data_t    *thrmo_setp_dat;
                            zwrep_thrmo_setp_sup_fn rpt_cb = (zwrep_thrmo_setp_sup_fn)cb_req->cb;

                            thrmo_setp_dat = (if_thrmo_setp_data_t *)malloc(intf->data_item_sz);

                            if (thrmo_setp_dat)
                            {
                                memcpy(thrmo_setp_dat, intf->data, intf->data_item_sz);

                                plt_mtx_ulck(nw->mtx);
                                rpt_cb(&cb_req->ifd, thrmo_setp_dat->setp_len, thrmo_setp_dat->setp);
                                free(thrmo_setp_dat);
                            }
                            else
                            {
                                plt_mtx_ulck(nw->mtx);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_MAX_GROUP:
                    //-------------------------------------------------
                        {
                            uint8_t             max_grp;
                            zwrep_group_sup_fn  rpt_cb = (zwrep_group_sup_fn)cb_req->cb;

                            max_grp = *((uint8_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, max_grp);

                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_MUL_SWITCH:
                    //-------------------------------------------------
                        {
                            if_mul_switch_data_t    mul_switch_data;
                            zwrep_lvl_sup_fn        rpt_cb = (zwrep_lvl_sup_fn)cb_req->cb;

                            mul_switch_data = *((if_mul_switch_data_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, mul_switch_data.pri_type, mul_switch_data.sec_type);

                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_AV:
                    //-------------------------------------------------
                        {
                            uint16_t    length;
                            uint8_t     *mask;
                            zwrep_av_fn rpt_cb = (zwrep_av_fn)cb_req->cb;

                            mask = (uint8_t *)malloc(intf->data_item_sz);

                            if (mask)
                            {
                                memcpy(mask, intf->data, intf->data_item_sz);
                                length = intf->data_item_sz;

                                plt_mtx_ulck(nw->mtx);
                                rpt_cb(&cb_req->ifd, length, mask);
                                free(mask);
                            }
                            else
                            {
                                plt_mtx_ulck(nw->mtx);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_ALARM_TYPE:
                    //-------------------------------------------------
                        {
                            int                 i;
                            uint8_t             ztype[248];
                            if_alarm_data_t     *alarm_dat;
                            zwrep_alrm_sup_fn   rpt_cb = (zwrep_alrm_sup_fn)cb_req->cb;

                            alarm_dat = (if_alarm_data_t *)malloc(intf->data_item_sz);

                            if (alarm_dat)
                            {
                                memcpy(alarm_dat, intf->data, intf->data_item_sz);

                                for (i=0; i<alarm_dat->type_evt_cnt; i++)
                                {
                                    ztype[i] = alarm_dat->type_evt[i].ztype;
                                }

                                plt_mtx_ulck(nw->mtx);
                                rpt_cb(&cb_req->ifd, alarm_dat->have_vtype, alarm_dat->type_evt_cnt, ztype);
                                free(alarm_dat);
                            }
                            else
                            {
                                plt_mtx_ulck(nw->mtx);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_ALARM_EVENT:
                    //-------------------------------------------------
                        {
                            int                 i;
                            int                 j;
                            int                 found = 0;
                            int                 max_evt;
                            if_alarm_data_t     *alarm_dat;
                            zwrep_alrm_evt_fn   rpt_cb = (zwrep_alrm_evt_fn)cb_req->cb;
                            uint8_t             *evt_bitmask;
                            uint8_t             sup_evt[248];
                            uint8_t             evt_len;
                            uint8_t             ztype;

                            alarm_dat = (if_alarm_data_t *)intf->data;

                            ztype = (uint8_t)cb_req->extra;

                            //Find matching alarm type in cache
                            for (i=0; i<alarm_dat->type_evt_cnt; i++)
                            {
                                if(alarm_dat->type_evt[i].ztype == ztype)
                                {   //Found
                                    found = 1;
                                    evt_len = alarm_dat->type_evt[i].evt_len;
                                    if (evt_len > MAX_EVT_BITMASK_LEN)
                                    {
                                        evt_len = MAX_EVT_BITMASK_LEN;
                                    }
                                    max_evt = evt_len * 8;
                                    evt_len = 0;
                                    evt_bitmask = alarm_dat->type_evt[i].evt;

                                    for (j=0; j < max_evt; j++)
                                    {
                                        if ((evt_bitmask[(j>>3)] >> (j & 0x07)) & 0x01)
                                        {
                                            sup_evt[evt_len++] = j;
                                        }
                                    }
                                }
                            }

                            plt_mtx_ulck(nw->mtx);

                            if (found)
                            {
                                rpt_cb(&cb_req->ifd, ztype, evt_len, sup_evt);
                            }
                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_PROT:
                    //-------------------------------------------------
                        {
                            zwprot_sup_t        if_prot_data;
                            zwrep_prot_sup_fn   rpt_cb = (zwrep_prot_sup_fn)cb_req->cb;

                            if_prot_data = *((zwprot_sup_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, &if_prot_data);

                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_MAX_USR_CODES:
                    //-------------------------------------------------
                        {
                            uint8_t             max_usr_codes;
                            zwrep_usr_sup_fn    rpt_cb = (zwrep_usr_sup_fn)cb_req->cb;

                            max_usr_codes = *((uint8_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, max_usr_codes);

                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_METER:
                    //-------------------------------------------------
                        {
                            zwmeter_cap_t       if_meter_data;
                            zwrep_meter_sup_fn  rpt_cb = (zwrep_meter_sup_fn)cb_req->cb;

                            if_meter_data = *((zwmeter_cap_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, &if_meter_data);

                        }
                        break;

                    //-------------------------------------------------
                    case CB_RPT_TYP_METER_DESC:
                    //-------------------------------------------------
                        {
                            zwmeter_t        if_meter_data;
                            zwrep_meterd_fn  rpt_cb = (zwrep_meterd_fn)cb_req->cb;

                            if_meter_data = *((zwmeter_t *)intf->data);

                            plt_mtx_ulck(nw->mtx);
                            rpt_cb(&cb_req->ifd, &if_meter_data);

                        }
                        break;

                    default:
                        plt_mtx_ulck(nw->mtx);

                }
            }
            else
            {
                plt_mtx_ulck(nw->mtx);
            }

            free(req_lst);
        }
    }
}


/**
zwnet_init - Initialize network and return this network
@param[in]	init	        Client initialization parameters
@param[out]	ret	            Client initialization return values
@return		ZW_ERR_xxx
@note       If the return value is ZW_ERR_NO_RESP, there is a possibility the controller's NVM is
            corrupted.  User may consider to reset the controller using zwnet_quick_reset() API.
*/
int zwnet_init(const zwnet_init_p init, zwnet_init_ret_p ret)
{
    static const char    *init_layer[] =
                                        {   "ok",
                                            "transport",
                                            "frame",
                                            "session",
                                            "application"
                                        };

    int         result;
    int         i;
    zwnet_p     nw;

    //Allocate memory for network internal structure
    nw = (zwnet_p) calloc(1, sizeof(zwnet_t));

    if (!nw)
        return ZW_ERR_MEMORY;

    //Check which mode of device specific configurations required by the user application
    result = ZW_ERR_VALUE;

    if (init->dev_spec_cfg_usr)
    {
        nw->dev_cfg_ctx = init->dev_spec_cfg_usr->dev_cfg_ctx;
        nw->dev_rec_find_fn = init->dev_spec_cfg_usr->dev_rec_find_fn;

        if (nw->dev_cfg_ctx && nw->dev_rec_find_fn)
        {
            result = ZW_ERR_NONE;
        }
    }
    else if (init->dev_spec_cfg)
    {
        //Load and save device specific configurations
        if (init->dev_spec_cfg->dev_cfg_cnt && init->dev_spec_cfg->dev_cfg_buf)
        {
            result = zwnet_dev_cfg_load(init->dev_spec_cfg, nw->dev_cfg_bufs);
        }
        else
        {   //Accept empty device specific configuration record
            result = ZW_ERR_NONE;
        }

    }

    if (result < 0)
    {
        goto l_ZWNET_INIT_ERROR1;
    }

	//Initialize Association CC and AGI CC data
	nw->stAGIData.pAGIGroupList = (AGI_Group *)calloc(sizeof(AGI_Group), 1);

	if(nw->stAGIData.pAGIGroupList == NULL)
	{
		result = ZW_ERR_MEMORY;
		goto l_ZWNET_INIT_ERROR1;
	}

	//Controller only has 1 group, i.e. the lifeline group. And the group only sends Device reset locally command
	nw->stAGIData.pAGIGroupList->byGroupID = 1;
	nw->stAGIData.pAGIGroupList->wProfile = 0;
	nw->stAGIData.pAGIGroupList->wEventCode = 0;

#ifdef USE_SAFE_VERSION
    strcpy_s(nw->stAGIData.pAGIGroupList->cchGroupName, ZW_AGI_GROUPNAME_STR_MAX, "LifeLine\0");
#else
    strcpy(nw->stAGIData.pAGIGroupList->cchGroupName, "LifeLine\0");
#endif

	nw->stAGIData.pAGIGroupList->byMaxNodes = 1;
	nw->stAGIData.pAGIGroupList->stCCList.CCMSB = COMMAND_CLASS_DEVICE_RESET_LOCALLY;
	nw->stAGIData.pAGIGroupList->stCCList.CCID = DEVICE_RESET_LOCALLY_NOTIFICATION;

	nw->stAGIData.byNofGroups++;		//Supports only 1 group

    //Allocate memory for security layer
    if (init->sec_enable)
    {
        nw->sec_ctx = (zwsec_layer_t *)calloc(1, sizeof(zwsec_layer_t));

        if (!nw->sec_ctx)
        {
            result = ZW_ERR_MEMORY;
            goto l_ZWNET_INIT_ERROR2;
        }
    }

    //Allocate memory for polling facility
    nw->poll_ctx = (zwpoll_ctx_t *)calloc(1, sizeof(zwpoll_ctx_t));

    if (!nw->poll_ctx)
    {
        result = ZW_ERR_MEMORY;
        goto l_ZWNET_INIT_ERROR3;
    }

    //Initialize platform
    if (plt_init(&nw->plt_ctx, init->print_txt_fn) != 0)
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR4;
    }

    if (!plt_mtx_init(&nw->mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR5;
    }

    if (!plt_sem_init(&nw->cmd_q_sem))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR6;
    }

    if (!plt_mtx_init(&nw->cmd_q_mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR7;
    }

    if (!plt_sem_init(&nw->nw_exec_sem))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR8;
    }

    if (!plt_mtx_init(&nw->nw_exec_mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR9;
    }

    if (!plt_sem_init(&nw->cb_sem))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR10;
    }

    if (!plt_mtx_init(&nw->cb_mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR11;
    }

    if (!plt_sem_init(&nw->wait_q_sem))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR12;
    }

    //Initialize state-machines
    nw->ni_sm_sta = ZWNET_STA_IDLE;
    nw->nu_sm_ctx.sta = NU_STA_IDLE;

    //Reset operation to "no operation"
    nw->curr_op = ZWNET_OP_NONE;

    //Copy network initialization data
    nw->init = *init;

    //Init all the back links
    nw->ctl.net = nw;
    nw->ctl.ep.node = &nw->ctl;

    //Init zwhci
    nw->appl_ctx.application_command_handler_cb = application_command_handler_cb;
    printf("linhui set application_command_handler_cb 2\n");
    nw->appl_ctx.application_update_cb = application_update_cb;
    nw->appl_ctx.cb_tmout_ms = APPL_CB_TMOUT_MIN;
    nw->appl_ctx.data = nw;
    nw->appl_ctx.plt_ctx = &nw->plt_ctx;

    result = zwhci_init(&nw->appl_ctx, nw->init.comm_port_name);

    if (result != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "Init %s layer failed", init_layer[result]);
        result = ZW_ERR_TRANSPORT_INI - (result - 1);
        goto l_ZWNET_INIT_ERROR13;
    }

    //Set node information
    result = zwnet_node_info_set(nw, supp_cmd_class, sizeof(supp_cmd_class)/sizeof(uint16_t));

    if (result != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_info_set with error:%d", result);
        result = ZW_ERR_COMM;
        goto l_ZWNET_INIT_ERROR14;
    }

#ifdef  TEST_WRITE_SERIAL_NUM
    result = zw_memory_put_buffer(&nw->appl_ctx, serial_num, SERIAL_NUMBER_OFFSET,
                                  sizeof(serial_num), write_cmplt_cb, serial_num);
    if (result != 0)
    {
        plt_msg_show(&nw->plt_ctx, "write serial num with error:%d", result);
    }
#endif

    //Populate nodes and end points
    result = zwnet_ctlr_info_get(nw, 1);

    if (result != 0)
    {
        goto l_ZWNET_INIT_ERROR14;
    }

    nw->cmd_q_thrd_run = 1;
    if (plt_thrd_create(zwnode_cmd_q_thrd, nw) < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "Init cmd q thread failed");
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR14;
    }

    nw->wait_q_thrd_run = 1;
    if (plt_thrd_create(zwnode_wait_q_thrd, nw) < 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "Init wait q thread failed");
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR14;
    }

    nw->nw_exec_thrd_run = 1;
    if (plt_thrd_create(zwnet_exec_thrd, nw) < 0)
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR14;
    }

    nw->cb_thrd_run = 1;
    if (plt_thrd_create(zwnet_cb_thrd, nw) < 0)
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_INIT_ERROR14;
    }

    //Initialize the security layer
    if (init->sec_enable)
    {
        nw->sec_ctx->prng_ctx.appl_ctx = &nw->appl_ctx;
        nw->sec_ctx->net = nw;
        result = zwsec_init(nw->sec_ctx);
        if (result != 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Init security layer failed:%d", result);
            goto l_ZWNET_INIT_ERROR14;
        }
        //Initialization done, enable the security layer
        nw->sec_enable = 1;
    }

    //Initialize poll facility
    nw->poll_ctx->net = nw;
    result = zwpoll_init(nw->poll_ctx);
    if (result != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "Init poll facility failed:%d", result);
        goto l_ZWNET_INIT_ERROR14;
    }
    nw->poll_enable = 1;

    //No error
    ret->net_id = nw->homeid;
    ret->net = nw;
    ret->ctlr_id = nw->ctl.nodeid;
    ret->hc_api_type = HC_API_TYPE;
    return ZW_ERR_NONE;

l_ZWNET_INIT_ERROR14:
    zwnet_exit(nw, NULL);
    return result;

l_ZWNET_INIT_ERROR13:
    plt_sem_destroy(nw->wait_q_sem);
l_ZWNET_INIT_ERROR12:
    plt_mtx_destroy(nw->cb_mtx);
l_ZWNET_INIT_ERROR11:
    plt_sem_destroy(nw->cb_sem);
l_ZWNET_INIT_ERROR10:
    plt_mtx_destroy(nw->nw_exec_mtx);
l_ZWNET_INIT_ERROR9:
    plt_sem_destroy(nw->nw_exec_sem);
l_ZWNET_INIT_ERROR8:
    plt_mtx_destroy(nw->cmd_q_mtx);
l_ZWNET_INIT_ERROR7:
    plt_sem_destroy(nw->cmd_q_sem);
l_ZWNET_INIT_ERROR6:
    plt_mtx_destroy(nw->mtx);
l_ZWNET_INIT_ERROR5:
    plt_exit(&nw->plt_ctx);
l_ZWNET_INIT_ERROR4:
    free(nw->poll_ctx);
l_ZWNET_INIT_ERROR3:
    free(nw->sec_ctx);
l_ZWNET_INIT_ERROR2:
    free(nw->stAGIData.pAGIGroupList);
l_ZWNET_INIT_ERROR1:
    for (i=0; i<4; i++)
    {
        free(nw->dev_cfg_bufs[i].dev_cfg_buf);
    }
    free(nw);
    return result;

}


/**
zwnet_exit - Clean up
@param[in]	net		        Network
@param[in]	node_info_file	Node information file name.  If this parameter is NULL,
                            node information will not be saved
@return
*/
void zwnet_exit(zwnet_p net, const char *node_info_file)
{
    int             wait_count;
    int             result;
    int             i;
    util_lst_t      *req_lst;
    cmd_q_req_t     *cmd_q_req;

#ifdef  TEST_SEC_WAKE_UP
    plt_tmr_stop(&nw->plt_ctx, net->wkup_tmr_ctx);
#endif

	plt_mtx_lck(net->mtx);

	if(net->pPowerLvl_tmr_ctx != NULL)
	{
		plt_tmr_stop(&net->plt_ctx, net->pPowerLvl_tmr_ctx);

		net->byPowerLvlTimeout = 0;
		net->pPowerLvl_tmr_ctx = NULL;
	}
	plt_mtx_ulck(net->mtx);

    //Cancel any network management operation
    zwnet_abort(net);

    //Stop the threads
    net->cmd_q_thrd_run = 0;
    plt_sem_post(net->cmd_q_sem);
    net->wait_q_thrd_run = 0;
    plt_sem_post(net->wait_q_sem);
    net->nw_exec_thrd_run = 0;
    plt_sem_post(net->nw_exec_sem);
    net->cb_thrd_run = 0;
    plt_sem_post(net->cb_sem);

    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (net->cmd_q_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (net->wait_q_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    wait_count = 100;
    while (wait_count-- > 0)
    {
        if (net->nw_exec_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (net->cb_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    //Flush the lists
    while ((req_lst = util_list_get(net->cmd_q_mtx, &net->cmd_q_req_hd)) != NULL)
    {
        cmd_q_req = (cmd_q_req_t *)req_lst->wr_buf;

        zwnode_mul_cmd_rm_q(net, &cmd_q_req->req_hd);

        free(req_lst);
    }

    util_list_flush(net->mtx, &net->wait_q_req_hd);
    util_list_flush(net->mtx, &net->cb_req_hd);

    //Clean up security layer
    if (net->sec_enable)
    {
        zwsec_exit(net->sec_ctx);
    }

    //Shutdown poll facility
    if (net->poll_enable)
    {
        zwpoll_shutdown(net->poll_ctx);
    }

    //Check whether to save node info
    if (node_info_file)
    {
        result = zwutl_ni_save(net, node_info_file);
        if (result < 0)
        {
            debug_zwapi_msg(&net->plt_ctx, "Save node information file '%s' failed: %d", node_info_file, result);
        }
    }

    //Shutdown all lower layers
    zwhci_exit(&net->appl_ctx);

    //Free resources
    util_list_flush(net->nw_exec_mtx, &net->nw_exec_req_hd);

    zwnet_node_rm_all(net);
    if (net->poll_enable)
    {
        zwpoll_exit(net->poll_ctx);
    }

	while (net->stAGIData.pAGIGroupList != NULL)
	{
		AGI_Group *pGroup = net->stAGIData.pAGIGroupList;
		net->stAGIData.pAGIGroupList = net->stAGIData.pAGIGroupList->pNext;
		free(pGroup);
		pGroup = NULL;
	}
    plt_sem_destroy(net->wait_q_sem);
    plt_sem_destroy(net->cmd_q_sem);
    plt_sem_destroy(net->nw_exec_sem);
    plt_sem_destroy(net->cb_sem);
    plt_mtx_destroy(net->cmd_q_mtx);
    plt_mtx_destroy(net->mtx);
    plt_mtx_destroy(net->nw_exec_mtx);
    plt_mtx_destroy(net->cb_mtx);
    plt_exit(&net->plt_ctx);
    for (i=0; i<4; i++)
    {
        free(net->dev_cfg_bufs[i].dev_cfg_buf);
    }
    free(net->poll_ctx);
    free(net->sec_ctx);
    free(net);
}


/**
zwnet_load - Load network node information and security layer key and settings
@param[in]	nw		        Network
@param[in]	node_info_file	Node information file name.  If this parameter is NULL,
                            node information will be acquired through Z-Wave network
@return		ZW_ERR_xxx
*/
int zwnet_load(zwnet_p nw, const char *node_info_file)
{
    int32_t     result;
    zwnode_p    node;
    unsigned    prot_ver;
    unsigned    prot_subver;

    //Set current operation
    nw->curr_op = ZWNET_OP_INITIALIZE;

    //Check whether to load node information file
    if (node_info_file)
    {
        result = zwutl_ni_load(nw, node_info_file);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Load node info file '%s' failed: %d", node_info_file, result);
            return result;
        }

        //Re-generate authentication and encryption keys using the saved network key
        if (nw->sec_enable)
        {
            zwsec_keys_gen(nw->sec_ctx->nw_key, nw->sec_ctx->auth_key, nw->sec_ctx->enc_key);
            debug_zwapi_msg(&nw->plt_ctx, "Nw key:");
            debug_zwapi_bin_msg(&nw->plt_ctx, nw->sec_ctx->nw_key, 16);
/*
            debug_zwapi_msg(&nw->plt_ctx, "Auth key:");
            debug_zwapi_bin_msg(&nw->plt_ctx, nw->sec_ctx->auth_key, 16);
            debug_zwapi_msg(&nw->plt_ctx, "Enc key:");
            debug_zwapi_bin_msg(&nw->plt_ctx, nw->sec_ctx->enc_key, 16);
*/
        }
    }

    //Reset the controller protocol and application version to the current version
    nw->ctl.app_ver = ((uint16_t)APPL_VERSION) << 8 | APPL_SUBVERSION;

#ifdef USE_SAFE_VERSION
    if (sscanf_s(nw->ctl.basis_api_ver, "Z-Wave %u.%u", &prot_ver, &prot_subver) == 2)
#else
    if (sscanf((const char *)nw->ctl.basis_api_ver, "Z-Wave %u.%u", &prot_ver, &prot_subver) == 2)
#endif
    {
        nw->ctl.proto_ver = ((uint16_t)prot_ver) << 8 | prot_subver;
    }
    else
    {
        nw->ctl.proto_ver = 0;
    }

    //Reset the controller library type to the current type
    nw->ctl.lib_type = nw->ctl_lib_type;

    //Load node information through network for nodes that are not loaded
    //with detailed information
    plt_mtx_lck(nw->mtx);

    if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        sm_job_t  sm_job;
        uint8_t   node_id[256];

        sm_job.op = ZWNET_OP_INITIALIZE;
        sm_job.num_node = 0;
        sm_job.node_id = node_id;
        sm_job.cb = NULL;
        sm_job.auto_cfg = 0;
//      sm_job.cfg_lifeline = 0;

        //Create the node id list
        while (node)
        {
            if (!node->ep.intf)
            {   //Add node without interface to the query list
                node_id[sm_job.num_node++] = node->nodeid;
            }
            node = (zwnode_p)node->obj.next;
        }

        //Submit job to the state-machine
        if (sm_job.num_node > 0)
        {
            zwnet_node_info_sm(nw, EVT_GET_NODE_INFO, (uint8_t *)&sm_job);
            plt_mtx_ulck(nw->mtx);
            return ZW_ERR_NONE;
        }
    }

    //Notify init done
    nw->cb_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_notify_cb, nw);
    plt_mtx_ulck(nw->mtx);
    return ZW_ERR_NONE;

}


/**
zwnet_save - Save network node information and security layer key and setting into a file
@param[in]	net		        Network
@param[in]	node_info_file	Node information file name
@return         ZW_ERR_XXX.
*/
int zwnet_save(zwnet_p net, const char *node_info_file)
{
    if (node_info_file)
    {
        return zwutl_ni_save(net, node_info_file);
    }

    return ZW_ERR_FILE_OPEN;
}


/**
zwnet_get_desc - Get read-only network descriptor
@param[in]	net		Network
@return		network descriptor
*/
zwnetd_p zwnet_get_desc(zwnet_p net)
{
    int32_t            result;
    uint8_t            suc_node_id;

    plt_mtx_lck(net->mtx);

    net->net_desc.id = net->homeid;
    net->net_desc.local = &net->local_node;
    zwnet_get_node(net, &net->local_node);
    net->net_desc.user = net->init.user;
    net->net_desc.plt_ctx = &net->plt_ctx;

    //Get controller network role
    zw_get_controller_capabilities(&net->appl_ctx, &net->ctl.ctlr_nw_role);
    net->net_desc.ctl = net->ctl.ctlr_nw_role & 0x1F;

    //Find SUC node
    net->net_desc.suc = NULL;

    result = zw_get_suc_node_id(&net->appl_ctx, &suc_node_id);
    if ((result == 0) && (suc_node_id != 0))
    {
        zwnode_p    node;
        node = zwnode_find(&net->ctl, suc_node_id);
        if (node)
        {
            zwnode_get_desc(node, &net->suc_node);
            net->net_desc.suc = &net->suc_node;
        }

    }
    plt_mtx_ulck(net->mtx);

    return &net->net_desc;
}


/**
zwnet_sm_ni_stop - Stop the node info state-machine
@param[in]	nw		Network
@return
*/
static void    zwnet_sm_ni_stop(zwnet_p nw)
{
    plt_mtx_lck(nw->mtx);
    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;    //make sure timer context is null, else restart timer will crash

    //Reset end point information state-machine to idle state
    nw->ep_sm_sta = EP_STA_IDLE;

    //Reset single end point information state-machine to idle state
    nw->single_ep_sta = S_EP_STA_IDLE;

    //Reset node information state-machine to idle state
    nw->ni_sm_sta = ZWNET_STA_IDLE;

    if (nw->sm_job.node_id)
    {
        free(nw->sm_job.node_id);
        nw->sm_job.node_id = NULL;
    }
    nw->sm_job.num_node = 0;

    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_sm_nu_stop - Stop the network update state-machine
@param[in]	nw		Network
@return
*/
static void    zwnet_sm_nu_stop(zwnet_p nw)
{
    plt_mtx_lck(nw->mtx);

    //Stop node info update
    zwnet_sm_ni_stop(nw);

    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->nu_sm_tmr_ctx);
    nw->nu_sm_tmr_ctx = NULL;

    //Reset to idle state
    nw->nu_sm_ctx.sta = NU_STA_IDLE;
    nw->nu_sm_ctx.num_node = 0;

    plt_mtx_ulck(nw->mtx);
}


/**
zwnet_abort - Abort current action/transmission
@param[in]	net		Network
@return		ZW_ERR_xxx
*/
int zwnet_abort(zwnet_p net)
{
    int32_t result = 0;
    uint8_t cancel_op;

    plt_mtx_lck(net->mtx);
    cancel_op = net->curr_op;
    plt_mtx_ulck(net->mtx);


    switch (cancel_op)
    {
        case ZWNET_OP_RP_NODE:
        case ZWNET_OP_ADD_NODE:
            {
                result = zw_add_node_to_network(&net->appl_ctx,
                                                ADD_NODE_STOP, NULL);
                if (result != 0)
                {
                    debug_zwapi_msg(&net->plt_ctx, "zw_add_node_stop with error:%d", result);
                }
                zwnet_sm_ni_stop(net);

                //Stop security add node state-machine
                if (net->sec_enable)
                {
                    plt_mtx_lck(net->sec_ctx->sec_mtx);

                    plt_tmr_stop(&net->plt_ctx, net->sec_ctx->add_sm_tmr_ctx);
                    net->sec_ctx->add_sm_tmr_ctx = NULL;
                    //Reset to idle state
                    net->sec_ctx->add_sm_sta = ZWSEC_ADD_IDLE;
                    plt_mtx_ulck(net->sec_ctx->sec_mtx);
                }

            }
            break;

        case ZWNET_OP_RM_NODE:
            {
                result = zw_remove_node_from_network(&net->appl_ctx,
                                                     REMOVE_NODE_STOP, NULL);
                if (result != 0)
                {
                    debug_zwapi_msg(&net->plt_ctx, "zw_remove_node_from_network with error:%d", result);
                }
            }
            break;

        case ZWNET_OP_INITIATE:
            {
                result = zw_set_learn_mode(&net->appl_ctx, ZW_SET_LEARN_MODE_DISABLE, NULL);
                if (result != 0)
                {
                    debug_zwapi_msg(&net->plt_ctx, "zw_set_learn_mode 'stop' with error:%d", result);
                }
                zwnet_sm_ni_stop(net);

                //Stop security included node state-machine
                if (net->sec_enable)
                {
                    plt_mtx_lck(net->sec_ctx->sec_mtx);

                    if (net->sec_ctx->incd_sm_sta != ZWSEC_INCD_IDLE)
                    {
                        plt_tmr_stop(&net->plt_ctx, net->sec_ctx->incd_sm_tmr_ctx);
                        net->sec_ctx->incd_sm_tmr_ctx = NULL;
                        //Reset to idle state
                        net->sec_ctx->incd_sm_sta = ZWSEC_INCD_IDLE;
                    }
                    plt_mtx_ulck(net->sec_ctx->sec_mtx);
                }
            }
            break;

        case ZWNET_OP_MIGRATE_SUC:
            {
                result = zw_create_new_primary_ctrl(&net->appl_ctx,
                                                    CREATE_PRIMARY_STOP, NULL);
                if (result != 0)
                {
                    debug_zwapi_msg(&net->plt_ctx, "zw_create_new_primary_ctrl with error:%d", result);
                }
                zwnet_sm_ni_stop(net);

                //Stop security add node state-machine
                if (net->sec_enable)
                {
                    plt_mtx_lck(net->sec_ctx->sec_mtx);

                    plt_tmr_stop(&net->plt_ctx, net->sec_ctx->add_sm_tmr_ctx);
                    net->sec_ctx->add_sm_tmr_ctx = NULL;
                    //Reset to idle state
                    net->sec_ctx->add_sm_sta = ZWSEC_ADD_IDLE;
                    plt_mtx_ulck(net->sec_ctx->sec_mtx);
                }
            }
            break;

        case ZWNET_OP_MIGRATE:
            {
                result = zw_controller_change(&net->appl_ctx,
                                              CONTROLLER_CHANGE_STOP, NULL);
                if (result != 0)
                {
                    debug_zwapi_msg(&net->plt_ctx, "zw_controller_change with error:%d", result);
                }
                zwnet_sm_ni_stop(net);

                //Stop security add node state-machine
                if (net->sec_enable)
                {
                    plt_mtx_lck(net->sec_ctx->sec_mtx);

                    plt_tmr_stop(&net->plt_ctx, net->sec_ctx->add_sm_tmr_ctx);
                    net->sec_ctx->add_sm_tmr_ctx = NULL;
                    //Reset to idle state
                    net->sec_ctx->add_sm_sta = ZWSEC_ADD_IDLE;
                    plt_mtx_ulck(net->sec_ctx->sec_mtx);
                }
            }
            break;

        case ZWNET_OP_UPDATE:
            {
                zwnet_sm_nu_stop(net);
            }
            break;

        case ZWNET_OP_INITIALIZE:
        case ZWNET_OP_NODE_UPDATE:
            {
                zwnet_sm_ni_stop(net);
            }
            break;

        case ZWNET_OP_ASSIGN:
            {
                net->suc_updt.num_node = 0;
            }
            break;

#ifdef FLASH_PROG_API
        case ZWNET_OP_FW_UPDATE:
            {   //This operation cannot be cancelled
                return ZW_ERR_OP_FAILED;
            }
            break;
#endif
    }

    //Reset operation to "no operation"
    plt_mtx_lck(net->mtx);
    if (cancel_op == net->curr_op)
    {
        net->curr_op = ZWNET_OP_NONE;
    }
    plt_mtx_ulck(net->mtx);

    //Return result
    return(result == 0)? ZW_ERR_NONE : ZW_ERR_OP_FAILED;

}


/**
zwnet_node_rm_cb - Remove node from network callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Remove node status
@param[in]	node_id     Node id of the removed node
@param[in]	node_info   Node information such as basic, generic and specific device classes and supported command classes
@return
*/
static void zwnet_node_rm_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts,
                             uint8_t node_id, appl_node_info_t *node_info)
{
    static const char    *rm_node_sts[] =
    {   "unknown",
        "learn ready",
        "node found",
        "removing slave",
        "removing controller",
        "unknown2",
        "done!",
        "failed"
    };

    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_node_rm_cb: status:%s, node id:%u",
                    (sts < 8)?rm_node_sts[sts]  : "unknown", (unsigned)node_id);

    if (node_info)
    {
        debug_zwapi_msg(&nw->plt_ctx, "basic:%02X, generic:%02X, specific:%02X, cmd class:",
                        (unsigned)node_info->basic, (unsigned)node_info->gen,
                        (unsigned)node_info->spec);
        zwnet_cmd_cls_show(nw, node_info->cmd_cls, node_info->cmd_cnt);
    }

    //Callback
    if (sts == REMOVE_NODE_STATUS_REMOVING_SLAVE
        || sts == REMOVE_NODE_STATUS_REMOVING_CONTROLLER)
    {
        zwnode_p    node;

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RM_NODE, OP_RM_NODE_REMOVING);
        }

        if (node_id > 0)
        {   //Valid node id
            node = zwnode_find(&nw->ctl, node_id);
            if (node)
            {
                if (nw->init.node)
                {
                    zwnoded_t noded;
                    zwnode_get_desc(node, &noded);
                    nw->init.node(nw->init.user, &noded, ZWNET_NODE_REMOVED);
                }
                //Remove the node
                zwnode_rm(nw, node_id);
            }
        }
    }
    else if (sts == REMOVE_NODE_STATUS_DONE
             || sts >= REMOVE_NODE_STATUS_FAILED)//To include undocumented return
    //status 0x23 when secondary controller
    //invokes this operation
    {
        zwnet_abort(nw);
        //Callback to notify status
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RM_NODE, (sts == REMOVE_NODE_STATUS_DONE)?
                            OP_DONE : OP_FAILED);
        }
    }
    else if (sts == REMOVE_NODE_STATUS_LEARN_READY)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RM_NODE, OP_RM_NODE_LEARN_READY);
        }
    }
    else if (sts == REMOVE_NODE_STATUS_NODE_FOUND)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RM_NODE, OP_RM_NODE_FOUND);
        }
    }
}


/**
zwnet_add_ni_get - Get detailed node info to populate command classes and interfaces
@param[in]	nw              network
@param[in]  node_id	        the newly added node id
@param[in]	op	            current add node operation
@param[in]	sts_get_ni	    status used to notify upper layer about get detailed node info
@return     ZW_ERR_XXX
*/
static void zwnet_add_ni_get(zwnet_p nw, uint8_t node_id, uint8_t op, int sts_get_ni)
{
    sm_job_t  sm_job;

    sm_job.op = op;
    sm_job.num_node = 1;
    sm_job.node_id = &node_id;
    sm_job.delay = 1000;
    sm_job.cb = NULL;
    sm_job.auto_cfg = 1;
//  sm_job.cfg_lifeline = 1;

    plt_mtx_lck(nw->mtx);

    //Get the detailed node information
    if (zwnet_node_info_sm(nw, EVT_DELAYED_GET_NODE_INFO, (uint8_t *)&sm_job) != ZWNET_STA_IDLE)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_add_ni_get failed");
        zwnet_abort(nw);
        plt_mtx_ulck(nw->mtx);
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, op, OP_FAILED);
        }
        return;
    }
    plt_mtx_ulck(nw->mtx);

    //Notify the progress of the operation
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, op, sts_get_ni);
    }
}


/**
zwnet_sec_incl_cb - security layer add node callback
@param[in]	nw		network
@param[in]  status	0= secure inclusion ok; else negative error number
@param[in]	user	user context
@return
*/
static void zwnet_sec_incl_cb(zwnet_p nw, int status, void *user)
{
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);
    node = zwnode_find(&nw->ctl, nw->added_node_id);
    if (node)
    {
        //Check the status of secure inclusion
        if (status == 0)
        {
            node->sec_incl_failed = 0;
            printf("============================>sec_incl_failed %d<======3===================\n",node->sec_incl_failed);
        }
        else
        {
            //Failed, mark the node as failed security inclusion
            node->sec_incl_failed = 2;
            printf("============================>sec_incl_failed %d<======4===================\n",node->sec_incl_failed);

            //Remove all the endpoints as there may be some security related endpoints
            zwnode_ep_rm_all(node);
        }

    }
    plt_mtx_ulck(nw->mtx);


    //Try to populate command classes and interfaces
    zwnet_add_ni_get(nw, nw->added_node_id, nw->added_node_op, nw->get_ni_sts);
}


/**
zwnet_sec_incl - adding node to a secure network
@param[in]	nw		        network
@param[in]  node_id	        the newly added node id
@param[in]	op	            current add node operation
@param[in]	sts_sec_incl	status used to notify upper layer about security inclusion
@param[in]	sts_get_ni	    status used to notify upper layer about get detailed node info
@return     ZW_ERR_XXX
*/
static int zwnet_sec_incl(zwnet_p nw, uint8_t node_id, uint8_t op, int sts_sec_incl, int sts_get_ni)
{
    printf("===================================> linhui, zwnet_sec_incl, nodeid=%d\n", node_id);
    zwnode_p    node;
    zwif_p      intf = NULL;

    //Save the newly added node id, current operation, etc.
    nw->added_node_id = node_id;
    nw->added_node_op = op;
    nw->get_ni_sts = sts_get_ni;

    node = zwnode_find(&nw->ctl, node_id);

    if (node)
    {
        //Reset sec_incl_failed flag
        node->sec_incl_failed = 0;
        printf("============================>sec_incl_failed %d<======5===================\n",node->sec_incl_failed);

        intf = zwif_find_cls(node->ep.intf, COMMAND_CLASS_SECURITY);
    }

    //Check whether to include the node in secure network
    printf("======================> linhui ==> enable_sec=%d, intf=%p,nw->ctl.sec_incl_failed=%d\n",nw->sec_enable,intf,nw->ctl.sec_incl_failed);
    if (nw->sec_enable && intf && (!nw->ctl.sec_incl_failed))
    {
        printf("linhui, go here , sec add\n");
        zwsec_incd_sm_param_t   param;

        //Start the state-machine for secure-inclusion
        param.cb = zwnet_sec_incl_cb;
        param.user = nw;
        param.delay_ms = 500; //Add delay to solve firmware bug that reports
        //ADD_NODE_STATUS_PROTOCOL_DONE earlier than actual completion.

        plt_mtx_lck(nw->sec_ctx->sec_mtx);
        if (zwsec_add_node_sm(nw, node_id, EVT_UNSEC_INC_DONE, &param) == ZW_ERR_NONE)
        {
            plt_mtx_ulck(nw->sec_ctx->sec_mtx);
            //Notify the progress of the operation
            if (nw->init.notify)
                nw->init.notify(nw->init.user, op, sts_sec_incl);

            return ZW_ERR_NONE;
        }
        plt_mtx_ulck(nw->sec_ctx->sec_mtx);

        //Mark the node as failed security inclusion
        node->sec_incl_failed = 2;
        printf("============================>sec_incl_failed %d<======6===================\n",node->sec_incl_failed);
    }
    node->sec_incl_failed = 2;
    printf("linhui, go here , sec add,return failed\n");
    return ZW_ERR_FAILED;

}


static const char    *add_node_sts[] =
{   "unknown",
    "learn ready",
    "node found",
    "adding slave",
    "adding controller",
    "protocol done",
    "done!",
    "failed"
};


/**
zwnet_node_add_cb - Add node to network callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Add node status
@param[in]	node_id     Node id assigned to the new node
@param[in]	node_info   Node information such as basic, generic and specific device classes and supported command classes
@return
*/
static void zwnet_node_add_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts,
                              uint8_t node_id, appl_node_info_t *node_info)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_ts_msg(&nw->plt_ctx, "zwnet_node_add_cb: status:%s, node id:%u",
                       (sts < 8)?add_node_sts[sts]  : "unknown", (unsigned)node_id);

    if (node_info)
    {
        debug_zwapi_msg(&nw->plt_ctx, "basic:%02X, generic:%02X, specific:%02X, cmd class:",
                        (unsigned)node_info->basic, (unsigned)node_info->gen,
                        (unsigned)node_info->spec);
        zwnet_cmd_cls_show(nw, node_info->cmd_cls, node_info->cmd_cnt);

    }

    //Callback
    if ((sts == ADD_NODE_STATUS_ADDING_SLAVE)
        || (sts == ADD_NODE_STATUS_ADDING_CONTROLLER))
    {
        zwnode_p    node;

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ADD_NODE, OP_ADD_NODE_ADDING);
        }

        //Check whether the node is an existing node in the network
        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, node_id);
        nw->added_node_new = (node)? 0 : 1;
        plt_mtx_ulck(nw->mtx);

        //Check whether node info is available now
        if (node_info)
        {
            zwnet_node_info_update(nw, node_info);
        }
        else
        {   //Create a node info
            appl_node_info_t    ni;
            memset(&ni, 0, sizeof(appl_node_info_t));
            ni.node_id = node_id;

            zwnet_node_info_update(nw, &ni);
        }
        node = zwnode_find(&nw->ctl, node_id);

        if (node && nw->init.node)
        {
            zwnoded_t noded;
            zwnode_get_desc(node, &noded);
            nw->init.node(nw->init.user, &noded, ZWNET_NODE_ADDED);
        }
    }
    else if (sts == ADD_NODE_STATUS_PROTOCOL_DONE)
    {
        //Stop the add node operation
        zw_add_node_to_network(&nw->appl_ctx, ADD_NODE_STOP, NULL);

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ADD_NODE, OP_ADD_NODE_PROTOCOL_DONE);
        }

        //Do not try to add node into secure network if the new node is an existing network node
        if (nw->added_node_new)
        {
            printf("==========ADD_NODE_STATUS_PROTOCOL_DONE==================\n");
            //Check whether to add node into secure network
            if (zwnet_sec_incl(nw, node_id, ZWNET_OP_ADD_NODE, OP_ADD_NODE_SEC_INCD, OP_ADD_NODE_GET_NODE_INFO) == ZW_ERR_NONE)
                return;
        }

        //Get the detailed node information
        zwnet_add_ni_get(nw, node_id, ZWNET_OP_ADD_NODE, OP_ADD_NODE_GET_NODE_INFO);

    }
    else if (sts >= ADD_NODE_STATUS_FAILED) //To include undocumented return
    //status 0x23 when secondary controller
    //invokes this operation
    {
        zwnet_abort(nw);
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ADD_NODE, OP_FAILED);
        }
    }
    else if (sts == ADD_NODE_STATUS_LEARN_READY)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ADD_NODE, OP_ADD_NODE_LEARN_READY);
        }
    }
    else if (sts == ADD_NODE_STATUS_NODE_FOUND)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ADD_NODE, OP_ADD_NODE_FOUND);
        }
    }
}



/**
zwnet_add - Called by inclusion controller to add/remove initiating node to network
@param[in]	net		Network
@param[in]	add		0-remove, else-add
@return		ZW_ERR_xxx, otherwise node callback will be called.
@see		zwnet_initiate
*/
int zwnet_add(zwnet_p net, uint8_t add)
{
    int32_t result;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    if (add == 0)
    {   //Remove node
        result = zw_remove_node_from_network(&net->appl_ctx, REMOVE_NODE_ANY,
                                             zwnet_node_rm_cb);

        if (result == 0)
        {
            net->curr_op = ZWNET_OP_RM_NODE;
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_NONE;
        }
        debug_zwapi_msg(&net->plt_ctx, "zwnet_add: remove node with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }
    else
    {   //Add node
        result = zw_add_node_to_network(&net->appl_ctx, ADD_NODE_ANY | ADD_NODE_OPTION_HIGH_POWER
                                        | ADD_NODE_OPTION_NETWORK_WIDE, zwnet_node_add_cb);
        if (result == 0)
        {
            net->curr_op = ZWNET_OP_ADD_NODE;
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_NONE;
        }
        debug_zwapi_msg(&net->plt_ctx, "zwnet_add with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }
}

/**
zwnet_node_rm_all - Remove all nodes from the network.
@param[in]	nw	    The network
*/
void zwnet_node_rm_all(zwnet_p nw)
{
    zwnode_p    node;

    plt_mtx_lck(nw->mtx);
    //
    //Remove the second node onwards
    //
    while ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        zwnode_ep_rm_all(node);

        //Remove command queues
        zwnode_cmd_q_rm(node);
        zwnode_wait_cmd_q_rm(node);

        //Remove polling requests
        zwpoll_node_rm(nw, node->nodeid);

        //Stop timers
        plt_tmr_stop(&nw->plt_ctx, node->mul_cmd_tmr_ctx);
        node->mul_cmd_tmr_ctx = NULL;
        plt_tmr_stop(&nw->plt_ctx, node->wait_tmr_ctx);
        node->wait_tmr_ctx = NULL;

        //Free extended version information
        if (node->ext_ver)
        {
            free(node->ext_ver);
        }

        //Remove node
        zwobj_del(&nw->ctl.obj.next, &node->obj);
    }
    //
    //Remove the first node (controller node)
    //

    //Remove the first and only one ep
    zwnode_ep_rm_all(&nw->ctl);

    zwnode_ctl_clr(nw);

    //Free extended version information
    if (nw->ctl.ext_ver)
    {
        free(nw->ctl.ext_ver);
        nw->ctl.ext_ver = NULL;
    }

    plt_mtx_ulck(nw->mtx);
}

static const char    *failed_node_cb_sts[] =
{   "node is working properly",
    "failed node was removed",
    "failed node was not removed",
    "failed node is ready to be replaced",
    "failed node has been replaced",
    "failed node has not been replaced"
};

/**
zwnet_failed_id_rm_cb - Remove failed node id callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_failed_id_rm_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_failed_id_rm_cb: status:%s", (sts < 6)?
                    failed_node_cb_sts[sts]  : "unknown");

    if (sts == ZW_FAILED_NODE_REMOVED)
    {
        zwnode_p    node;

        if (nw->failed_id > 0)
        {   //Valid node id
            node = zwnode_find(&nw->ctl, nw->failed_id);
            if (node)
            {
                if (nw->init.node)
                {   //Callback
                    zwnoded_t noded;
                    zwnode_get_desc(node, &noded);
                    nw->init.node(nw->init.user, &noded, ZWNET_NODE_REMOVED);
                }
            }
            //Remove the failed node
            zwnode_rm(nw, nw->failed_id);
        }
    }

    zwnet_abort(nw);
    //Callback to notify status
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, ZWNET_OP_RM_FAILED_ID,
                        (sts == ZW_FAILED_NODE_REMOVED)?
                        OP_DONE : OP_FAILED);
    }
}

/**
zwnet_failed_id_rp_cb - Replace failed node id callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwnet_failed_id_rp_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_failed_id_rp_cb: status:%s", (sts < 6)?
                    failed_node_cb_sts[sts]  : "unknown");

    if (sts == ZW_FAILED_NODE_REPLACE_DONE)
    {
        //Stop the add node operation
        zw_add_node_to_network(&nw->appl_ctx, ADD_NODE_STOP, NULL);

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RP_NODE, OP_RP_NODE_PROTOCOL_DONE);
        }


        //Start node id replace node info state-machine
        if (zwnet_rp_ni_sm(nw, EVT_GET_NODE_INFO, &nw->failed_id) == ZWNET_STA_IDLE)
        {
            zwnet_abort(nw);
            if (nw->init.notify)
            {
                nw->init.notify(nw->init.user, ZWNET_OP_RP_NODE, OP_FAILED);
            }

        }

    }
    else if ((sts == ZW_FAILED_NODE_REPLACE_FAILED)
             || (sts == ZW_NODE_OK))
    {
        zwnet_abort(nw);
        //Callback to notify status
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RP_NODE, OP_FAILED);
        }
    }
    else if (sts == ZW_FAILED_NODE_REPLACE)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_RP_NODE, OP_RP_NODE_READY);
        }
    }
}




/**
zwnet_fail - Called by inclusion controller to remove failed node from
             the routing table or replace failed node with initiating node
@param[in]	noded	Failed node
@param[in]	replace	If 0-remove failed node, else replace it
@return		ZW_ERR_xxx, otherwise node callback will be called.
@see		zwnet_initiate
*/
int zwnet_fail(zwnoded_p noded, uint8_t replace)
{
    uint8_t             result;
    uint8_t             resp_flg;
    int32_t             res;
    appl_layer_ctx_t   *appl_ctx = (appl_layer_ctx_t   *)(&noded->net->appl_ctx);
    zwnet_p             net = noded->net;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    if (zw_is_failed_node(appl_ctx, noded->nodeid, &result) == 0)
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "Node id:%u is %s the list of failing node",
                        (unsigned)noded->nodeid, (result)? "in" : "NOT in");

    }
    else
    {
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_NO_RESP;
    }

    if (!result)
    {   //Node is not in protocol layer failed node ID list
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_NOT_IN_LIST;
    }

    if (replace == 0)
    {   //Remove failed node from the protocol layer failed node ID list
        res = zw_remove_failed_node_id(appl_ctx, noded->nodeid,
                                       zwnet_failed_id_rm_cb, &resp_flg);
        if (res == 0)
        {
            net->curr_op = ZWNET_OP_RM_FAILED_ID;
            net->failed_id = noded->nodeid;
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_NONE;
        }
    }
    else
    {   //Replace failed node
        res = zw_replace_failed_node(appl_ctx, noded->nodeid,
                                     zwnet_failed_id_rp_cb, &resp_flg);
        if (res == 0)
        {
            net->curr_op = ZWNET_OP_RP_NODE;
            net->failed_id = noded->nodeid;
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_NONE;
        }
    }
    plt_mtx_ulck(net->mtx);
    return ZW_ERR_OP_FAILED;

}


/**
zwnet_initiate_ni_get - Get detailed node info to populate command classes and interfaces
@param[in]	nw              network
@return     ZW_ERR_XXX
*/
static int zwnet_initiate_ni_get(zwnet_p nw)
{
    zwnode_p    node;

    if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        sm_job_t    sm_job;
        uint8_t     node_ids[256];

        sm_job.op = ZWNET_OP_INITIATE;
        sm_job.num_node = 0;
        sm_job.node_id = node_ids;
        sm_job.delay = 2000;
        sm_job.cb = NULL;
        sm_job.auto_cfg = 0;
//      sm_job.cfg_lifeline = 0;


        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_INI_GET_NODE_INFO);
        }

        //Get the node id list
        while (node)
        {
            node_ids[sm_job.num_node++] = node->nodeid;
            node = (zwnode_p)node->obj.next;
        }

        //Get the detailed node information
        plt_mtx_lck(nw->mtx);
        if (zwnet_node_info_sm(nw, EVT_DELAYED_GET_NODE_INFO, (uint8_t *)&sm_job) != ZWNET_STA_IDLE)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Fail to get detailed node info");
            zwnet_abort(nw);
            plt_mtx_ulck(nw->mtx);
            if (nw->init.notify)
            {
                nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_FAILED);
            }
            return ZW_ERR_FAILED;
        }
        plt_mtx_ulck(nw->mtx);
        return ZW_ERR_NONE;

    }
    return ZW_ERR_NODE_NOT_FOUND;
}


/**
zwnet_incd_node_cb - security layer included node callback
@param[in]	nw		network
@param[in]  status	0= secure inclusion ok; else negative error number
@param[in]	user	user context
*/
void zwnet_incd_node_cb(zwnet_p nw, int status, void *user)
{
    //Update the status of secure inclusion
    nw->ctl.sec_incl_failed = (status == 0)? 0 : 1;
    printf("============================>sec_incl_failed %d<======7===================\n",nw->ctl.sec_incl_failed);

    //Try to populate command classes and interfaces
    if (zwnet_initiate_ni_get(nw) == ZW_ERR_NODE_NOT_FOUND)
    {
        zwnet_abort(nw);
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_DONE);
        }
    }
}


/**
zwnet_initiate_cb - Set learn mode callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status
@param[in]	node_id     Node id of the controller that is adding/removing node
@param[in]	cmd_len     The length of the command classes
@param[in]	cmd_cls     Command classes
@return
*/
void zwnet_initiate_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts,
                       uint8_t node_id, uint8_t cmd_len, uint8_t *cmd_cls)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    char        *sts_str;

    switch (sts)
    {
        case    LEARN_MODE_STARTED:
            sts_str = "started";
            break;
        case    LEARN_MODE_DONE:
            sts_str = "done";
            break;
        case    LEARN_MODE_FAILED:
            sts_str = "failed";
            break;

        case    LEARN_MODE_DELETED:
            sts_str = "deleted";
            break;

        default:
            sts_str = "unknown";

    }
    if (sts == LEARN_MODE_STARTED)
    {   //Undocumented: only LEARN_MODE_STARTED gives correct primary controller node id
        debug_zwapi_ts_msg(&nw->plt_ctx, "zwnet_initiate_cb: status:%s, primary controller id:%u", sts_str,
                           (unsigned)node_id);
    }
    else
    {
        debug_zwapi_ts_msg(&nw->plt_ctx, "zwnet_initiate_cb: status:%s", sts_str);
    }


    if (sts == LEARN_MODE_DONE)
    {
        int joined_nw;  //flag to determine the node has already joined a network

        //
        //Callback upper layer and update the network structure
        //

        joined_nw = (nw->ctl.obj.next)?  1 : 0;

        //Stop checking of SECURITY_SCHEME_GET command
        nw->sec_chk = 0;

        //Stop initiate
        zw_set_learn_mode(&nw->appl_ctx, ZW_SET_LEARN_MODE_DISABLE, NULL);

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_INI_PROTOCOL_DONE);
        }

        //Remove all the old nodes
        zwnet_node_rm_all(nw);

        //Populate nodes and end points
        zwnet_ctlr_info_get(nw, 1);

        if (nw->ctl.obj.next != NULL)
        {   //Being added into a Z-wave network

            //Only try to be included into secure network if joining a new network
            if (nw->sec_enable && !joined_nw)
            {
                zwsec_incd_sm_param_t   param;

                //Start the state-machine for secure-inclusion
                param.cb = zwnet_incd_node_cb;
                param.user = nw;


                plt_mtx_lck(nw->sec_ctx->sec_mtx);
                if (zwsec_incd_node_sm(nw, node_id, EVT_UNSECURED_INC_DONE, &param) == ZW_ERR_NONE)
                {
                    //Work around for early Security Scheme Command
                    if (nw->sec_node_id)
                    {
                        zwsec_incd_node_sm(nw, nw->sec_node_id, EVT_SCHEME_GET, &nw->sec_scheme);
                    }

                    plt_mtx_ulck(nw->sec_ctx->sec_mtx);
                    //Notify the progress of the operation
                    if (nw->init.notify)
                        nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_INI_SEC_INCD);

                    return;
                }
                plt_mtx_ulck(nw->sec_ctx->sec_mtx);

                //Mark the controller as failed security inclusion
                nw->ctl.sec_incl_failed = 1;

            }

            //Try to populate command classes and interfaces
            zwnet_initiate_ni_get(nw);
            return;
        }
        else
        {
            //Being remove from a Z-wave network
        }
    }
    else if (sts == LEARN_MODE_STARTED)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_INITIATE, OP_INI_STARTED);
        }
    }

    if (sts != LEARN_MODE_STARTED)
    {
        //Callback only all the node info is available.
        //Success status will be reported after all the node info is available.
        zwnet_abort(nw);
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_INITIATE,
                            (sts == LEARN_MODE_DONE)?
                            OP_DONE : OP_FAILED);
        }
    }
}


/**
zwnet_initiate - called by controller to add/remove itself to/from the network
@param[in]	net		network
@return		ZW_ERR_xxx, otherwise node callback will be called
@see		zwnet_add, zwnet_fail, zwnet_migrate
*/
int zwnet_initiate(zwnet_p net)
{
    int32_t result;
    uint8_t mode;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //Work around for early Security Scheme Command
    if (net->sec_enable)
    {
        net->sec_chk = 1;
        net->sec_node_id = 0;
    }

    if (net->ctl.obj.next)
    {
        mode = ZW_SET_LEARN_MODE_CLASSIC;
        debug_zwapi_msg(&net->plt_ctx, "Set learn mode for exclusion");
    }
    else
    {
        mode = ZW_SET_LEARN_MODE_NWI;
        debug_zwapi_msg(&net->plt_ctx, "Set learn mode for inclusion");
    }

    result = zw_set_learn_mode(&net->appl_ctx, mode, zwnet_initiate_cb);

    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "zw_set_learn_mode with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }

    net->curr_op = ZWNET_OP_INITIATE;
    plt_mtx_ulck(net->mtx);
    return ZW_ERR_NONE;

}


/**
zwnet_suc_id_updt_cb - send suc id or assigned suc return route to a node callback
@param[in]	nw		    network
@param[in]	user_prm    The user specific parameter
@return		ZW_ERR_xxx
*/
static void zwnet_suc_id_updt_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p nw;
    int32_t result;
    int     rd_idx;
    extern const char *zwif_tx_cmplt_sts[];

    nw = (zwnet_p)appl_ctx->data;

    if (tx_sts != TRANSMIT_COMPLETE_OK)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_suc_id_updt_cb with error:%s",
                        (tx_sts < 6)?
                        zwif_tx_cmplt_sts[tx_sts]  : "unknown");
    }

    while (nw->suc_updt.rd_idx < nw->suc_updt.num_node)
    {
        //Adjust read pointer
        rd_idx = nw->suc_updt.rd_idx++;

        if (nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_ROUTING_SLAVE)
        {
            if (nw->suc_updt.assign_suc)
            {   //Inform  other routing slaves in the network of the new SUC/SIS node id
                result = zw_assign_suc_return_route(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx], zwnet_suc_id_updt_cb);
            }
            else
            {   //Inform  other routing slaves in the network of the removal of SUC/SIS node
                result = zw_delete_suc_return_route(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx], zwnet_suc_id_updt_cb);
            }

            if (result == 0)
            {
                return;
            }
        }
        else if ((nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_STATIC_CONTROLLER)
                 || (nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_CONTROLLER))
        {
            //Inform  other controllers in the network of the new SUC/SIS node id
            //or the removal of SUC/SIS node
            debug_zwapi_msg(&nw->plt_ctx, "send suc_node_id to node:%u", nw->suc_updt.node_id[rd_idx]);
            result = zw_send_suc_id(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx],
                                    TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE,
                                    zwnet_suc_id_updt_cb);
            if (result == 0)
            {
                return;
            }
        }
    }

    //Done.
    if (nw->suc_updt.notify_cb)
    {
        zwnet_abort(nw);
        //Callback
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, ZWNET_OP_ASSIGN, OP_DONE);
        }
    }
}



/**
zwnet_suc_id_updt - send suc id or assign suc return route to a node
@param[in]	nw		network
@return		ZW_ERR_xxx
*/
static int zwnet_suc_id_updt(zwnet_p nw)
{
    int32_t result;
    int     rd_idx;

    if (nw->suc_updt.rd_idx < nw->suc_updt.num_node)
    {
        //Adjust read pointer first to avoid race condition
        rd_idx = nw->suc_updt.rd_idx++;

        if (nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_ROUTING_SLAVE)
        {
            if (nw->suc_updt.assign_suc)
            {   //Inform  other routing slaves in the network of the new SUC/SIS node id
                result = zw_assign_suc_return_route(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx], zwnet_suc_id_updt_cb);
            }
            else
            {   //Inform  other routing slaves in the network of the removal of SUC/SIS node
                result = zw_delete_suc_return_route(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx], zwnet_suc_id_updt_cb);
            }

            if (result < 0)
            {
                return result;
            }
        }
        else if ((nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_STATIC_CONTROLLER)
                 || (nw->suc_updt.basic_dev_cls[rd_idx] == BASIC_TYPE_CONTROLLER))
        {
            //Inform  other controllers in the network of the new SUC/SIS node id
            //or the removal of SUC/SIS node
            result = zw_send_suc_id(&nw->appl_ctx, nw->suc_updt.node_id[rd_idx],
                                    TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE,
                                    zwnet_suc_id_updt_cb);
            if (result < 0)
            {
                return result;
            }
        }
        return ZW_ERR_NONE;
    }

    return ZW_ERR_OP_FAILED;

}


/**
zwnet_assign_cb - Set a controller node as SUC/SIS callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Status of update
@param[in]	user_prm    The user specific parameter
@return
*/
void zwnet_assign_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts, void *user_prm)
{
    int32_t     result;
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "zwnet_assign_cb: status:%s", (sts < 7)?
                    nw_updt_sts_str[sts]  : "unknown");

    if ((sts == ZW_SUC_SET_SUCCEEDED)
        && (nw->suc_updt.num_node > 0))
    {
        result = zwnet_suc_id_updt(nw);

        if (result == 0)
        {
            return;
        }
        else
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_suc_id_updt with error:%d", result);
        }
    }


    zwnet_abort(nw);
    //Callback
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, ZWNET_OP_ASSIGN,
                        (sts == ZW_SUC_SET_SUCCEEDED)?
                        OP_DONE : OP_FAILED);
    }

}

#ifdef  ZW_PLUS
    #ifndef ZW_SUC_FUNC_BASIC_SUC
        #define ZW_SUC_FUNC_BASIC_SUC       0x00
    #endif
#endif


/**
zwnet_assign_ex - Called by primary controller to assign or deassign SUC/SIS role to
               a static/bridge controller.
               Called by a controller to enable/disable its own SUC/SIS capability.
@param[in]	nw	    network
@param[in]	node_id	node (can be local)
@param[in]	role	role to assign ZWNET_ROLE_xxx or-mask
@param[in]	cb      callback function on transmit completion.
@return		ZW_ERR_xxx
*/
static int zwnet_assign_ex(zwnet_p nw, uint8_t node_id, uint8_t role, tx_cmplt_cb_t cb)
{
    appl_set_suc_t      param;
    int                 result;
    zwnode_p            node;
    uint8_t             res;
    uint8_t             suc_node_id;

    //Check whether this function is allowed
    if ((nw->ctlr_cap & CTLR_CAP_SET_SUC) == 0)
    {
        return ZW_ERR_UNSUPPORTED;
    }

    //Check the role. Currently, only ZWNET_ROLE_SIS is supported
    if ((role & ZWNET_ROLE_SIS) == 0)
    {
        return ZW_ERR_VALUE;
    }

    //Enable SUC/SIS role
    param.suc_sta = TRUE;
    //Set the capability
    param.cap = ZW_SUC_FUNC_NODEID_SERVER;

    //Check if the node is the local controller
    if (node_id == nw->ctl.nodeid)
    {
        //Check if it is primary controller
        if ((nw->ctl.ctlr_nw_role & CONTROLLER_IS_REAL_PRIMARY) != 0)
        {   //Local primary controller
            if (nw->ctlr_cap & CTLR_CAP_ENA_SUC)
            {   //Enable own SUC/SIS capability
                result = zw_enable_suc(&nw->appl_ctx, param.suc_sta,
                                       param.cap, &res);

                if (result != 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zw_enable_suc with error:%d", result);
                    return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
                }

                if (res == 0)
                {
                    debug_zwapi_ts_msg(&nw->plt_ctx, "zw_enable_suc error: attempting to disable a running SUC");
                    return ZW_ERR_OP_FAILED;
                }
            }
        }
        else    //Local secondary controller
        {
            //Enable own SIS capability
            if (nw->ctlr_cap & CTLR_CAP_ENA_SUC)
            {
                result = zw_enable_suc(&nw->appl_ctx, param.suc_sta,
                                       param.cap, &res);
                if (result != 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zw_enable_suc with error:%d", result);
                    return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
                }

                if (res == 0)
                {
                    debug_zwapi_ts_msg(&nw->plt_ctx, "zw_enable_suc error: attempting to disable a running SUC");
                    return ZW_ERR_OP_FAILED;
                }
            }

            //Notify enable of SIS done
            if (cb)
            {
                plt_mtx_lck(nw->mtx);
                nw->curr_op = ZWNET_OP_ASSIGN;
                nw->cb_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_notify_cb, nw);
                plt_mtx_ulck(nw->mtx);
                return ZW_ERR_NONE;
            }
        }
    }
    //Create a list of nodes to notify of the new SIS node id
    suc_node_id = node_id;
    nw->suc_updt.assign_suc = param.suc_sta;
    nw->suc_updt.num_node = 0;
    nw->suc_updt.rd_idx = 0;
    nw->suc_updt.notify_cb = (cb)? 1 : 0;

    plt_mtx_lck(nw->mtx);
    if ((node = (zwnode_p)nw->ctl.obj.next) != NULL)
    {
        //Get the node id list
        while (node)
        {
            if ((node->nodeid != suc_node_id)
                && (node->basic != BASIC_TYPE_SLAVE))
            {
                nw->suc_updt.node_id[nw->suc_updt.num_node] = node->nodeid;
                nw->suc_updt.basic_dev_cls[nw->suc_updt.num_node++] = node->basic;
            }
            node = (zwnode_p)node->obj.next;
        }
    }
    plt_mtx_ulck(nw->mtx);

    //Assign or deassign SUC/SIS role

    param.node_id = node_id;
    param.tx_opt = FALSE; //normal power tx

    result = zw_set_suc_node_id(&nw->appl_ctx, &param, cb);
    if (result != 0)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zw_set_suc_node_id with error:%d", result);
        return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
    }

    if (cb)
    {
        nw->curr_op = ZWNET_OP_ASSIGN;
    }


    //Check if the node is local controller
    if (node_id == nw->ctl.nodeid)
    {   //Local controller, no callback function will be called.
        //Update the other node about SUC node id now

        if (nw->suc_updt.num_node == 0)
        {   //No other node in the network.
            //Notify assign of SIS done
            if (cb)
            {
                plt_mtx_lck(nw->mtx);
                nw->curr_op = ZWNET_OP_ASSIGN;
                nw->cb_tmr_ctx = plt_tmr_start(&nw->plt_ctx, 10, zwnet_notify_cb, nw);
                plt_mtx_ulck(nw->mtx);
            }
            return ZW_ERR_NONE;
        }

        result = zwnet_suc_id_updt(nw);
        if (result < 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_suc_id_updt with error:%d", result);
            if (cb)
            {
                nw->curr_op = ZWNET_OP_NONE;
            }
            return (result == SESSION_ERROR_RESP_TMOUT)? ZW_ERR_NO_RESP : ZW_ERR_COMM;
        }
    }

    return ZW_ERR_NONE;
}


/**
zwnet_assign - Called by primary controller to assign or deassign SUC/SIS role to
               a static/bridge controller.
               Called by a controller to enable/disable its own SUC/SIS capability.
@param[in]	noded	node (can be local)
@param[in]	role	role to assign ZWNET_ROLE_xxx or-mask
@return		ZW_ERR_xxx
*/
//int zwnet_assign(zwnoded_p noded, uint8_t role)
//{
//    int     result;
//    zwnet_p net = noded->net;
//
//    if (net->curr_op != ZWNET_OP_NONE)
//    {
//        debug_zwapi_msg(&nw->plt_ctx, "Current operation not completed yet, try again later");
//        return ZW_ERR_LAST_OP_NOT_DONE;
//    }
//
//    //Get controller network role
//    result = zw_get_controller_capabilities(&net->appl_ctx, &net->ctl.ctlr_nw_role);
//
//    if (result != 0)
//    {
//        debug_zwapi_msg(&nw->plt_ctx, "zw_get_controller_capabilities with error:%d", result);
//        return ZW_ERR_OP_FAILED;
//    }
//
//    //Check whether SIS presents in the network
//    if (net->ctl.ctlr_nw_role & CONTROLLER_NODEID_SERVER_PRESENT)
//    {
//        debug_zwapi_msg(&nw->plt_ctx, "Assign SIS disallowed when SIS presents");
//        return ZW_ERR_DISALLOWED;
//    }
//
//    return zwnet_assign_ex(net, noded->nodeid, role, zwnet_assign_cb);
//}


/**
zwnode_update - Update node status and information
@param[in]	noded	Node
@return		ZW_ERR_xxx
*/
int zwnode_update(zwnoded_p noded)
{
    sm_job_t    sm_job;
    zwnet_p     net;

    net = noded->net;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //Block node update of controller itself
    if (net->ctl.nodeid == noded->nodeid)
    {
        plt_mtx_ulck(net->mtx);
        return  ZW_ERR_UNSUPPORTED;
    }

    net->curr_op = ZWNET_OP_NODE_UPDATE;
    sm_job.op = ZWNET_OP_NODE_UPDATE;
    sm_job.num_node = 1;
    sm_job.node_id = &noded->nodeid;
    sm_job.cb = NULL;
    sm_job.auto_cfg = 0;
//  sm_job.cfg_lifeline = 0;

    if (zwnet_node_info_sm(net, EVT_GET_NODE_INFO, (uint8_t *)&sm_job) != ZWNET_STA_GET_NODE_INFO)
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnode_update failed");
        net->curr_op = ZWNET_OP_NONE;
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }
    plt_mtx_ulck(net->mtx);

    return ZW_ERR_NONE;
}


/**
zwnet_update - Update network status and information
@param[in]	net		Network
@return		ZW_ERR_xxx
*/
int zwnet_update(zwnet_p net)
{
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //Call state-machine
    plt_mtx_lck(net->mtx);
    if (zwnet_updt_sm(net, EVT_NU_START, NULL) == NU_STA_IDLE)
    {
        plt_mtx_ulck(net->mtx);
        debug_zwapi_msg(&net->plt_ctx, "zwnet_update failed");
        return ZW_ERR_OP_FAILED;
    }
    net->curr_op = ZWNET_OP_UPDATE;
    plt_mtx_ulck(net->mtx);
    return ZW_ERR_NONE;

}


/**
zwnet_reset_cb - Restore factory default callback function
@param[in]	appl_ctx    The application layer context
@return
*/
static void zwnet_reset_cb(struct _appl_layer_ctx *appl_ctx)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_msg(&nw->plt_ctx, "Reset to factory default done!");
    zwnet_abort(nw);

    //Reset security layer
    if (nw->sec_enable)
    {
        int result;

        zwsec_exit(nw->sec_ctx);

        memset(nw->sec_ctx, 0, sizeof(zwsec_layer_t));

        nw->sec_ctx->prng_ctx.appl_ctx = &nw->appl_ctx;
        nw->sec_ctx->net = nw;
        result = zwsec_init(nw->sec_ctx);
        if (result != 0)
        {
            debug_zwapi_msg(&nw->plt_ctx, "Reinit security layer failed:%d", result);
            nw->sec_enable = 0;
        }
    }

    //Remove all the old nodes
    zwnet_node_rm_all(nw);

    //Populate nodes and end points
    zwnet_ctlr_info_get(nw, 1);

    //Callback to notify status
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, ZWNET_OP_RESET, OP_DONE);
    }
}


/**
zwnet_reset - Detach self from network ie. forget all nodes.
              A power-cycle is required after the reset.
@param[in]	net		Network
@return		ZW_ERR_xxx
*/
int zwnet_reset(zwnet_p net)
{
    int32_t result;
    AGI_Group *pMatchAGIGroup = net->stAGIData.pAGIGroupList;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //
    //Check whether self is SIS; if not, send device reset locally notification
    //

    //As long as the lifeline AGI group has been assigned with some Node ID, send the device notification command
    if (pMatchAGIGroup != NULL && pMatchAGIGroup->stNodeEpInfo.byNodeID != 0)
    {
        appl_snd_data_t *prm;

        prm = (appl_snd_data_t *)calloc(1, sizeof(appl_snd_data_t) + 2);
        if (!prm)
        {
            debug_zwapi_msg(&net->plt_ctx, "zwnet_reset memory error");
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_MEMORY;
        }

        prm->tx_opt = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE;
        prm->dat_buf[0] = COMMAND_CLASS_DEVICE_RESET_LOCALLY;
        prm->dat_buf[1] = DEVICE_RESET_LOCALLY_NOTIFICATION;
        prm->dat_len = 2;
        prm->node_id = pMatchAGIGroup->stNodeEpInfo.byNodeID;
        result = zw_send_data(&net->appl_ctx, prm, NULL, NULL);
        free(prm);
        if (result < 0)
        {
            debug_zwapi_msg(&net->plt_ctx, "send device reset notification with error:%d", result);
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_OP_FAILED;
        }
    }

    result = zw_set_default(&net->appl_ctx, zwnet_reset_cb);

    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnet_reset with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }
    net->curr_op = ZWNET_OP_RESET;
    plt_mtx_ulck(net->mtx);
    return ZW_ERR_NONE;

}


/**
zwnet_migrate_cb - Primary controller migration callback function
@param[in]	appl_ctx    The application layer context
@param[in]	sts		    Add node status
@param[in]	node_id     Node id assigned to the new node
@param[in]	node_info   Node information such as basic, generic and specific device classes and supported command classes
@return
*/
static void zwnet_migrate_cb(struct _appl_layer_ctx *appl_ctx, uint8_t sts,
                             uint8_t node_id, appl_node_info_t *node_info)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;

    debug_zwapi_ts_msg(&nw->plt_ctx, "zwnet_migrate_cb: status:%s, node id:%u",
                       (sts < 8)?add_node_sts[sts]  : "unknown", (unsigned)node_id);

    if (node_info)
    {
        debug_zwapi_msg(&nw->plt_ctx, "basic:%02X, generic:%02X, specific:%02X, cmd class:",
                        (unsigned)node_info->basic, (unsigned)node_info->gen,
                        (unsigned)node_info->spec);
        zwnet_cmd_cls_show(nw, node_info->cmd_cls, node_info->cmd_cnt);

    }

    //Callback
    if (sts == ADD_NODE_STATUS_ADDING_CONTROLLER)
    {
        zwnode_p    node;

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, nw->curr_op, OP_ADD_NODE_ADDING);
        }

        //Check whether the node is an existing node in the network
        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, node_id);
        nw->added_node_new = (node)? 0 : 1;
        plt_mtx_ulck(nw->mtx);


        //Check whether node info is available now
        if (node_info)
        {
            zwnet_node_info_update(nw, node_info);
        }
        else
        {   //Create a node info
            appl_node_info_t    ni;
            memset(&ni, 0, sizeof(appl_node_info_t));
            ni.node_id = node_id;

            zwnet_node_info_update(nw, &ni);
        }
        node = zwnode_find(&nw->ctl, node_id);

        //Save the newly added controller id
        nw->added_ctlr_id = node_id;

        if (node && nw->init.node)
        {
            zwnoded_t noded;
            zwnode_get_desc(node, &noded);
            nw->init.node(nw->init.user, &noded, ZWNET_NODE_ADDED);
        }
    }
    else if (sts == ADD_NODE_STATUS_PROTOCOL_DONE)
    {
        //Stop the add node operation
        zw_add_node_to_network(&nw->appl_ctx, ADD_NODE_STOP, NULL);
        if (nw->curr_op == ZWNET_OP_MIGRATE_SUC)
        {
            zw_create_new_primary_ctrl(&nw->appl_ctx, CREATE_PRIMARY_STOP, NULL);
        }
        else
        {
            zw_controller_change(&nw->appl_ctx, CONTROLLER_CHANGE_STOP, NULL);
        }

        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, nw->curr_op, OP_ADD_NODE_PROTOCOL_DONE);
        }

        //Do not try to add node into secure network if the new primary is an existing network node
        if (nw->added_node_new)
        {
            //Check whether to add node into secure network
            printf("====================================sts= protocol done\n");
            if (zwnet_sec_incl(nw, nw->added_ctlr_id, nw->curr_op, OP_ADD_NODE_SEC_INCD, OP_ADD_NODE_GET_NODE_INFO)
                == ZW_ERR_NONE)
                return;

        }

        //Get the detailed node information
        zwnet_add_ni_get(nw, nw->added_ctlr_id, nw->curr_op, OP_ADD_NODE_GET_NODE_INFO);

    }
    else if (sts == ADD_NODE_STATUS_LEARN_READY)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, nw->curr_op, OP_ADD_NODE_LEARN_READY);
        }
    }
    else if (sts == ADD_NODE_STATUS_NODE_FOUND)
    {
        //Notify the progress of the operation
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, nw->curr_op, OP_ADD_NODE_FOUND);
        }
    }
    else if (sts >= ADD_NODE_STATUS_FAILED) //To include undocumented return
                                            //status 0x23 when secondary controller
                                            //invokes this operation
    {
        zwnet_abort(nw);
        if (nw->init.notify)
        {
            nw->init.notify(nw->init.user, nw->curr_op, OP_FAILED);
        }
    }
}


/**
zwnet_migrate - Called by primary controller or SUC to make initiating controller primary.
                The controller invoking this function will become secondary.
@param[in]	net		Network
@return		ZW_ERR_xxx, otherwise node callback will be called
@see		zwnet_initiate
*/
int zwnet_migrate(zwnet_p net)
{
    int32_t result;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //Get controller network role
    result = zw_get_controller_capabilities(&net->appl_ctx, &net->ctl.ctlr_nw_role);

    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "zw_get_controller_capabilities with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }

    //Check whether SIS presents in the network
    if (net->ctl.ctlr_nw_role & CONTROLLER_NODEID_SERVER_PRESENT)
    {
        debug_zwapi_msg(&net->plt_ctx, "Migration disallowed when SIS presents");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_DISALLOWED;
    }

    if ((net->ctl.ctlr_nw_role & CONTROLLER_IS_SUC) != 0)
    {   //Make sure it is a pure SUC
        if ((net->ctl.ctlr_nw_role & (CONTROLLER_IS_REAL_PRIMARY | CONTROLLER_NODEID_SERVER_PRESENT))
            == 0)
        {   //Add a controller to the Z-Wave network as a replacement for the old primary controller
            result = zw_create_new_primary_ctrl(&net->appl_ctx, CREATE_PRIMARY_START,
                                                zwnet_migrate_cb);
            if (result != 0)
            {
                debug_zwapi_msg(&net->plt_ctx, "zwnet_migrate suc with error:%d", result);
                plt_mtx_ulck(net->mtx);
                return ZW_ERR_OP_FAILED;
            }
            net->curr_op = ZWNET_OP_MIGRATE_SUC;
            plt_mtx_ulck(net->mtx);
            return ZW_ERR_NONE;

        }
    }
    //No pure SUC present
    result = zw_controller_change(&net->appl_ctx, CONTROLLER_CHANGE_START,
                                  zwnet_migrate_cb);

    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnet_migrate with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }
    net->curr_op = ZWNET_OP_MIGRATE;
    plt_mtx_ulck(net->mtx);
    return ZW_ERR_NONE;
}


/**
zwnet_get_node - get first node (local controller) in the network
@param[in]	net		    Network
@param[out]	noded	The node descriptor
@return		ZW_ERR_NONE if successful
*/
int zwnet_get_node(zwnet_p net, zwnoded_p noded)
{
    int result;
    plt_mtx_lck(net->mtx);
    result = zwnode_get_desc(&net->ctl, noded);
    plt_mtx_ulck(net->mtx);
    return  result;
}


/**
zwnet_version - Get the home controller API version and subversion
@param[out]	ver		Version
@param[out]	subver	Sub version
@return
*/
void zwnet_version(uint8_t *ver, uint8_t *subver)
{
    *ver = APPL_VERSION;
    *subver = APPL_SUBVERSION;
}


/**
zwnet_rp_tmout_cb - Replace node id node info state-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_rp_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_rp_ni_sm(nw, EVT_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_rp_ni_sm - Replace node id node info state-machine
@param[in]	nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             The current state of the state-machine.
@pre caller must lock nw->mtx before calling this function
*/
static int zwnet_rp_ni_sm(zwnet_p nw, zwnet_ni_evt_t evt, uint8_t *data)
{
    int curr_sta;
    int result;

    switch (nw->rp_sm_sta)
    {
        //----------------------------------------------------------------
        case ZWNET_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_GET_NODE_INFO)
            {
                uint8_t  node_id = *data;

                result = zw_request_node_info(&nw->appl_ctx, node_id);

                if (result == 0)
                {
                    nw->rp_sm_sta = ZWNET_STA_GET_NODE_INFO;
                    //Restart timer
                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                    nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_NODE_INFO_TMOUT, zwnet_rp_tmout_cb, nw);
                    break;
                }
                debug_zwapi_msg(&nw->plt_ctx, "zwnet_rp_ni_sm: zw_request_node_info with error:%d", result);

            }
            break;

        //----------------------------------------------------------------
        case ZWNET_STA_GET_NODE_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_NODE_INFO)
                {
                    zwnode_p         node;
                    appl_node_info_t *node_info = (appl_node_info_t *)data;

                    if (node_info->node_id != nw->failed_id)
                    {
                        break;
                    }

                    //callback upper layer
                    node = zwnode_find(&nw->ctl, nw->failed_id);

                    if (node && nw->init.node)
                    {
                        zwnoded_t noded;
                        zwnode_get_desc(node, &noded);
                        //Unlock network mutex before callback
                        plt_mtx_ulck(nw->mtx);
                        nw->init.node(nw->init.user, &noded, ZWNET_NODE_UPDATED);
                        //Relock network mutex after callback
                        plt_mtx_lck(nw->mtx);
                    }

                    //Return back to idle state
                    nw->rp_sm_sta = ZWNET_STA_IDLE;

                    //Check whether to add node into secure network
                    printf("=================================>ZWNET_STA_GET_NODE_INFO  EVT_NODE_INFO\n");
                    if (zwnet_sec_incl(nw, nw->failed_id, ZWNET_OP_RP_NODE, OP_RP_NODE_SEC_INCD, OP_RP_NODE_GET_NODE_INFO) == ZW_ERR_NONE)
                        break;

                    //Get the detailed node information
                    zwnet_add_ni_get(nw, nw->failed_id, ZWNET_OP_RP_NODE, OP_RP_NODE_GET_NODE_INFO);

                }
                else if (evt == EVT_NW_TMOUT)
                {
                    zwnet_abort(nw);
                    if (nw->init.notify)
                    {
                        nw->init.notify(nw->init.user, ZWNET_OP_RP_NODE, OP_FAILED);
                    }

                    //Return back to idle state
                    nw->rp_sm_sta = ZWNET_STA_IDLE;
                }
            }
            break;

        default:
            break;
    }
    curr_sta = nw->rp_sm_sta;

    return curr_sta;
}


/**
zwnet_node_info_dump - dump the nodes information
@param[in]	net		The network
@return
*/
void    zwnet_node_info_dump(zwnet_p net)
{
    zwnode_p    zw_node;
    zwif_p      intf;
    zwep_p      ep;

    plt_mtx_lck(net->mtx);

    zw_node = &net->ctl;
    while (zw_node)
    {
        //Show node info
        plt_msg_show(&net->plt_ctx, "__________________________________________________________________________");
        plt_msg_show(&net->plt_ctx, "Node id:%u, Home id:%08X", (unsigned)zw_node->nodeid, (unsigned)net->homeid);
        plt_msg_show(&net->plt_ctx, "Device class: basic:%02X, generic:%02X, specific:%02X",
                     (unsigned)zw_node->basic, (unsigned)zw_node->ep.generic,
                     (unsigned)zw_node->ep.specific);
        plt_msg_show(&net->plt_ctx, "Node status:%s", (zw_node->alive)? "alive" : "down/sleeping");
        plt_msg_show(&net->plt_ctx, "Node security inclusion status:%s, %d", (zw_node->sec_incl_failed)?  "failed" : "unknown", zw_node->sec_incl_failed);
        plt_msg_show(&net->plt_ctx, "Z-wave library type:%u", zw_node->lib_type);
        plt_msg_show(&net->plt_ctx, "Z-wave protocol version:%u.%02u", (unsigned)(zw_node->proto_ver >> 8),
                     (unsigned)(zw_node->proto_ver & 0xFF));
        plt_msg_show(&net->plt_ctx, "Application version:%u.%02u", (unsigned)(zw_node->app_ver >> 8),
                     (unsigned)(zw_node->app_ver & 0xFF));

        ep = &zw_node->ep;
        while (ep)
        {
            plt_msg_show(&net->plt_ctx, "end point id:%u", ep->epid);
            plt_msg_show(&net->plt_ctx, "end point name:%s", ep->name);
            plt_msg_show(&net->plt_ctx, "end point location:%s", ep->loc);

            intf = ep->intf;
            //Show command class info
            while (intf)
            {
                plt_msg_show(&net->plt_ctx, "  Command class: %02X, ver:%u", (unsigned)intf->cls, (unsigned)intf->ver);
                //Show report commands
                if (intf->rpt_num > 0)
                {
                    int i;
                    for (i=0; i<intf->rpt_num; i++)
                    {
                        plt_msg_show(&net->plt_ctx, "              Report command: %02X", (unsigned)intf->rpt[i].rpt_cmd);
                    }
                }
                //Get the next interface
                intf = (zwif_p)intf->obj.next;
            }
            //Get the next end point
            ep = (zwep_p)ep->obj.next;
        }
        //Get the next node
        zw_node = (zwnode_p)zw_node->obj.next;
    }

    plt_mtx_ulck(net->mtx);
}

#ifdef  ZW_STRESS_TEST
/**
zwnet_stress_tst_cb_set - Set the stress test callback function
@param[in]	net	        Network
@param[in]	cb	        Callback function pointer.
@return
*/
void zwnet_stress_tst_cb_set(zwnet_p net, zw_stress_tst_fn cb)
{
    net->stress_tst_cb = cb;
}

#endif


/**
zwnet_get_user - get user context
@param[in]	net	        Network
@return	user context passed during initialization
*/
void *zwnet_get_user(zwnet_p net)
{
    return net->init.user;
}


/**
zwnet_send_nif_cb - send node information frame callback
@param[in]	nw		    network
@param[in]	user_prm    The user specific parameter
@return		ZW_ERR_xxx
*/
static void zwnet_send_nif_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p nw;
    extern const char *zwif_tx_cmplt_sts[];

    nw = (zwnet_p)appl_ctx->data;

    if (tx_sts != TRANSMIT_COMPLETE_OK)
    {
        debug_zwapi_msg(&nw->plt_ctx, "zwnet_send_nif_cb with error:%s",
                        (tx_sts < 6)?
                        zwif_tx_cmplt_sts[tx_sts]  : "unknown");
    }

    zwnet_abort(nw);
    //Callback
    if (nw->init.notify)
    {
        nw->init.notify(nw->init.user, ZWNET_OP_SEND_NIF,
                        (tx_sts == TRANSMIT_COMPLETE_OK)? OP_DONE : OP_FAILED);
    }
}


/**
zwnet_send_nif - send node information frame to a node or broadcast it
@param[in]	nw		    Network
@param[in]	noded	    Destination node to receive the node information frame
@param[in]	broadcast	Broadcast flag. 1= broadcast; 0= single cast
@return		ZW_ERR_XXX.
*/
int zwnet_send_nif(zwnet_p net, zwnoded_p noded, uint8_t broadcast)
{
    int         result;
    uint8_t     tx_opt;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    tx_opt = TRANSMIT_OPTION_EXPLORE;
    if (!broadcast)
    {
        tx_opt |= (TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE);
    }


    result = zw_send_node_info(&net->appl_ctx, (broadcast)? 0xFF : noded->nodeid,
                               tx_opt, zwnet_send_nif_cb);

    if (result == 0)
    {
        net->curr_op = ZWNET_OP_SEND_NIF;
    }

    plt_mtx_ulck(net->mtx);
    return result;

}


/**
zwnet_ep_tmout_cb - State-machine timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwnet_ep_tmout_cb(void *data)
{
    zwnet_p   nw = (zwnet_p)data;

    //Stop send timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;//make sure timer context is null, else restart timer will crash

    //Call state-machine
    zwnet_ep_info_sm(nw, EVT_EP_NW_TMOUT, NULL);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ep_ver_rpt_cb - Version report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The command class the version is referred to
@param[in]	ver	    The version of cls
@return
*/
static void zwnet_ep_ver_rpt_cb(zwif_p intf, uint16_t cls, uint8_t ver)
{
    zwnet_p     nw;
    uint8_t     data[16];

    //Testing only
//  if (cls == COMMAND_CLASS_MULTI_INSTANCE)
//  {
//      ver = 1;
//  }
    data[0] = cls >> 8;
    data[1] = cls & 0x00FF;
    data[2] = ver;

    memcpy(data + 4, &intf, sizeof(zwif_p));
    nw = intf->ep->node->net;

    plt_mtx_lck(nw->mtx);
    zwnet_ep_info_sm(nw, EVT_EP_VER_REPORT, data);
    plt_mtx_ulck(nw->mtx);

}


/**
zwnet_ep_sm_ver_get - Get version of each command class of the endpoint and update the state-machine's state
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_ep_sm_ver_get(zwnet_p nw, zwep_p ep)
{
    int     result;
    zwifd_t ifd;
    zwif_p  ver_intf;

    //Find version interface
    if (ep)
    {
        ver_intf = zwif_find_cls(ep->intf, COMMAND_CLASS_VERSION);
        if (ver_intf)
        {
            zwif_p    temp_intf;

            zwif_get_desc(ver_intf, &ifd);

            //Setup report callback
            result = zwif_set_report(&ifd, zwnet_ep_ver_rpt_cb, VERSION_COMMAND_CLASS_REPORT);
            if (result != 0)
            {
                return result;
            }

            //Get the version of first command class

            //Find the first non-basic/non-extended command class
            temp_intf = ep->intf;
            while (temp_intf)
            {
                if ((temp_intf->cls != COMMAND_CLASS_BASIC)
                    && ((temp_intf->cls & 0xFF00) == 0))
                {
                    break;
                }
                temp_intf =  (zwif_p)temp_intf->obj.next;
            }

            if (temp_intf)
            {
                uint8_t param;

                param = (uint8_t)temp_intf->cls;
                result = zwif_get_report(&ifd, &param, 1, VERSION_COMMAND_CLASS_GET, zwif_tx_sts_cb);

                if (result < 0)
                {
                    debug_zwapi_msg(&nw->plt_ctx, "zwnet_ep_sm_ver_get: zwif_get_report with error:%d", result);
                    return result;
                }
                //Update state-machine's state
                nw->ep_sm_sta = EP_STA_GET_CMD_VER;

                //Save the command class in version get
                nw->ni_sm_cls = temp_intf->cls;

                //Restart timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);

                return ZW_ERR_NONE;
            }
        }
    }

    return ZW_ERR_EP_NOT_FOUND;

}


/**
zwnet_ep_sm_sec_get - Get security supported command classes of the endpoint and update the state-machine's state
@param[in]	nw		    Network
@param[in]	ep		    Endpoint
@return  0 on success, negative error number on failure
*/
static int zwnet_ep_sm_sec_get(zwnet_p nw, zwep_p ep)
{
    int         result;
    zwifd_t     ifd;
    zwif_p      intf;
    uint8_t     cmd_buf[2];

    //Check whether the node has security command class
    intf = zwif_find_cls(ep->node->ep.intf, COMMAND_CLASS_SECURITY);

    printf("==========================>  check sec ,%d , %d \n", ep->node->sec_incl_failed, nw->ctl.sec_incl_failed);
    if (!intf || !nw->sec_enable || ep->node->sec_incl_failed || nw->ctl.sec_incl_failed){
        printf(" =========================return  unsupported\n");
        return ZW_ERR_UNSUPPORTED;
    }

    //Check whether the endpoint has security command class
    intf = zwif_find_cls(ep->intf, COMMAND_CLASS_SECURITY);

    if (intf)
    {
        //This node supports security command class
        //Setup report callback

        zwif_get_desc(intf, &ifd);

        result = zwif_set_report(&ifd, zwnet_sec_rpt_cb, SECURITY_COMMANDS_SUPPORTED_REPORT);
        if (result == 0)
        {
            //Get the supported secure command classes

            //Initialize report setting
            if (nw->sec_cmd_cls.cmd_cls_buf)
            {
                free(nw->sec_cmd_cls.cmd_cls_buf);
                nw->sec_cmd_cls.cmd_cls_buf = NULL;
            }

            cmd_buf[0] = COMMAND_CLASS_SECURITY;
            cmd_buf[1] = SECURITY_COMMANDS_SUPPORTED_GET;

            //Make sure SECURITY_COMMANDS_SUPPORTED_GET is always sent securely
            ifd.propty |= IF_PROPTY_SECURE;
            result = zwif_exec(&ifd, cmd_buf, 2, NULL);

            if (result >= 0)
            {
                //Update state-machine's state
                nw->ep_sm_sta = EP_STA_GET_SEC_CMD_SUP;

                //Restart timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);

                return ZW_ERR_NONE;
            }
            debug_zwapi_msg(&nw->plt_ctx, "zwnet_ep_sm_sec_get: zwsec_cmd_sup_get with error:%d", result);
            return result;
        }
        else
        {
            return result;
        }
    }
    return ZW_ERR_UNSUPPORTED;
}


/**
zwnet_ep_sm_next - Start processing next endpoint
@param[in]	nw		Network
@return 0 on success processing of next endpoint, negative error number on failure
*/
static int zwnet_ep_sm_next(zwnet_p nw)
{
    int     result;
    zwep_p  ep;


    //Stop timer
    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;    //make sure timer context is null, else restart timer will crash

    ep = (zwep_p)nw->ep_sm_job.first_ep->obj.next;
    nw->ep_sm_job.first_ep = ep;
    while (ep)
    {
        //Check whether the endpoint is a secure endpoint
        //and if so, get the security supported command classes
        result = zwnet_ep_sm_sec_get(nw, ep);
        if (result == 0)
        {
            return 0;
        }
        //Start processing version command class
        result = zwnet_ep_sm_ver_get(nw, ep);
        if (result == 0)
        {
            return 0;
        }
        //Point to next endpoint
        ep = (zwep_p)ep->obj.next;
        nw->ep_sm_job.first_ep = ep;
    }

    return ZW_ERR_FAILED;
}


/**
zwnet_1_ep_job_cmplt - Reset to idle state and callback to notify job completion
@param[in]	nw		    Network
@return
*/
static void zwnet_1_ep_job_cmplt(zwnet_p nw)
{
    //Completed job
    nw->single_ep_sta = S_EP_STA_IDLE;

    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
    nw->sm_tmr_ctx = NULL;

    //Invoke user callback
    if (nw->single_ep_job.cb)
    {
        nw->single_ep_job.cb(nw);
    }
}


/**
zwnet_1_ep_sm_cls_hdlr - Handle command classes in an endpoint
@param[in]	nw		    Network
@param[in]	ep		    end point
@param[in]	act		    starting action
@return 0 on success handling of next command class, negative error number on failure
*/
static int zwnet_1_ep_sm_cls_hdlr(zwnet_p nw, zwep_p ep, zwnet_1_ep_act_t act)
{
    static const zw_sm_hdlr_dat_t hdlr_dat[] = {
        {zwnet_sm_zwplus_info_get, COMMAND_CLASS_ZWAVEPLUS_INFO, 0},
        {zwnet_sm_sensor_get, COMMAND_CLASS_SENSOR_MULTILEVEL, 0},
        {zwnet_sm_max_group_get, COMMAND_CLASS_ASSOCIATION, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2},
        {zwnet_sm_grp_info_start, COMMAND_CLASS_ASSOCIATION_GRP_INFO, 0},
        {zwnet_sm_thrmo_fan_mode_sup_get, COMMAND_CLASS_THERMOSTAT_FAN_MODE, 0},
        {zwnet_sm_thrmo_mode_sup_get, COMMAND_CLASS_THERMOSTAT_MODE, 0},
        {zwnet_sm_thrmo_setp_sup_get, COMMAND_CLASS_THERMOSTAT_SETPOINT, 0},
        {zwnet_sm_lvl_sup_get, COMMAND_CLASS_SWITCH_MULTILEVEL_V3, 0},
        {zwnet_sm_av_get, COMMAND_CLASS_SIMPLE_AV_CONTROL, 0},
        {zwnet_sm_alrm_sup_get, COMMAND_CLASS_ALARM, 0},
        {zwnet_sm_prot_sup_get, COMMAND_CLASS_PROTECTION_V2, 0},
        {zwnet_sm_max_usr_code_get, COMMAND_CLASS_USER_CODE, 0},
        {zwnet_sm_meter_get, COMMAND_CLASS_METER, 0},
        {zwnet_sm_meter_desc_get, COMMAND_CLASS_METER_TBL_MONITOR, 0},
        {zwnet_sm_name_get, COMMAND_CLASS_NODE_NAMING, 0}
    };

    int    i;
    int    size;
	int    result;
    zwif_p intf;

    result = ZW_ERR_FAILED;

    size = sizeof(hdlr_dat)/sizeof(zw_sm_hdlr_dat_t);

    for (i=act; i<size; i++)
    {
        intf = zwif_find_cls(ep->intf, hdlr_dat[i].cls);
        if (!intf && hdlr_dat[i].alt_cls)
        {
            intf = zwif_find_cls(ep->intf, hdlr_dat[i].alt_cls);
        }

        if (intf)
        {
            result = hdlr_dat[i].get_fn(nw, intf);
            if (result == 0)
            {
                break;
            }
        }
    }

    return result;
}


/**
zwnet_1_ep_info_sm - Single endpoint info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return   Non-zero when the state-machine is started from idle state; otherwise return zero
*/
static int zwnet_1_ep_info_sm(zwnet_p nw, zwnet_1_ep_evt_t evt, uint8_t *data)
{
    int     result;
    zwif_p  intf;

    switch (nw->single_ep_sta)
    {
        //----------------------------------------------------------------
        case S_EP_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_S_EP_SM_START)
            {
                ep_sm_job_t  *ep_sm_job = (ep_sm_job_t *)data;

                nw->single_ep_job = *ep_sm_job;

                //Start processing command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, ep_sm_job->first_ep, S_EP_ACT_ZWPLUS_INFO);
                if (result == 0)
                {
                    return 1;//o.k.
                }
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_ZWPLUS_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_INFO_REPORT)
                {
                    zwplus_info_t   *info;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&info, data + sizeof(zwif_p), sizeof(zwplus_info_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Save the Z-wave+ information into endpoint
                    intf->ep->zwplus_info = *info;
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_SNSR);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_SENSOR:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_SENSOR_INFO_REPORT)
                {
                    if_sensor_data_t    *sensor_dat;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    sensor_dat = (if_sensor_data_t *)malloc(sizeof(if_sensor_data_t));

                    if (sensor_dat)
                    {
                        sensor_dat->sensor_type = data[0];
                        sensor_dat->sensor_unit = (0x01 << data[1]);

                        if (intf->data_cnt > 0)
                        {
                            free(intf->data);
                        }

                        //Assign to interface
                        intf->data_cnt = 1;
                        intf->data_item_sz = sizeof(if_sensor_data_t);
                        intf->data = sensor_dat;
                    }
                }
                else if (evt == EVT_S_EP_SENSOR_TYPE_REPORT)
                {
                    int                 i;
                    if_sensor_data_t    *sensor_dat;
                    uint8_t             *snsr_type;
                    uint8_t             snsr_type_cnt;

                    snsr_type_cnt = data[0];
                    memcpy(&snsr_type, data + 1, sizeof(uint8_t *));
                    memcpy(&intf, data + 1 + sizeof(uint8_t *), sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != SENSOR_SUBSTA_TYPE)
                    {
                        break;
                    }

                    if (snsr_type_cnt > 0)
                    {
                        sensor_dat = (if_sensor_data_t *)calloc(snsr_type_cnt, sizeof(if_sensor_data_t));

                        if (sensor_dat)
                        {
                            //Assign to interface
                            if (intf->data_cnt > 0)
                            {
                                free(intf->data);
                            }

                            //Save the sensor types
                            for (i=0; i<snsr_type_cnt; i++)
                            {
                                sensor_dat[i].sensor_type = snsr_type[i];
                            }

                            intf->data_cnt = snsr_type_cnt;
                            intf->data_item_sz = sizeof(if_sensor_data_t);
                            intf->data = sensor_dat;

                            //Start getting supported sensor units
                            nw->snsr_rd_idx = 0;
                            result = zwnet_sm_sensor_unit_get(nw, intf);
                            if (result == 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = SENSOR_SUBSTA_UNIT;
                                break;
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_SENSOR_UNIT_REPORT)
                {
                    if_sensor_data_t    *sensor_dat;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != SENSOR_SUBSTA_UNIT)
                    {
                        break;
                    }

                    sensor_dat = (if_sensor_data_t *)intf->data;

                    //Check whether the sensor type is correct
                    if ((nw->snsr_rd_idx < intf->data_cnt)
                        && (sensor_dat[nw->snsr_rd_idx].sensor_type == data[0]))
                    {
                        //Save the supported sensor units
                        sensor_dat[nw->snsr_rd_idx].sensor_unit = data[1];

                        //Get next supported sensor units
                        nw->snsr_rd_idx++;
                        result = zwnet_sm_sensor_unit_get(nw, intf);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MAX_GROUP);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MAX_GROUP:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_GRP_CNT_REPORT)
                {
                    uint8_t *max_grp;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        max_grp = (uint8_t *)malloc(1);
                    }
                    else
                    {   //Use previously allocated memory
                        max_grp = (uint8_t *)intf->data;
                    }

                    if (max_grp)
                    {
                        intf->data_cnt = intf->data_item_sz = 1;
                        intf->data = max_grp;
                        *max_grp = data[0];

                        //Check whether to mirror the max_grp to multi-channel association interface
                        if (intf->cls == COMMAND_CLASS_ASSOCIATION)
                        {
                            zwif_p  mc_assoc_intf;
                            mc_assoc_intf = zwif_find_cls(intf->ep->intf, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2);
                            if (mc_assoc_intf)
                            {
                                if (mc_assoc_intf->data_cnt == 0)
                                {
                                    //Allocate memory for data
                                    max_grp = (uint8_t *)malloc(1);
                                }
                                else
                                {   //Use previously allocated memory
                                    max_grp = (uint8_t *)mc_assoc_intf->data;
                                }

                                if (max_grp)
                                {
                                    mc_assoc_intf->data_cnt = mc_assoc_intf->data_item_sz = 1;
                                    mc_assoc_intf->data = max_grp;
                                    *max_grp = data[0];
                                }
                            }
                        }

                        //Set controller node id into group 1
                        if (nw->sm_job.auto_cfg)
                        {
                            zwnet_add_ctlr_grp(nw, intf->ep->intf, NULL, 1);
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_AGI);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_GROUP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_GRP_CMD_LST_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    grp_cmd_ent_t       *cmd_lst;
                    uint8_t             cmd_ent_cnt;
                    uint8_t             grp_id;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_CMD_LST)
                    {
                        break;
                    }

                    grp_id = data[0];
                    cmd_ent_cnt = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&cmd_lst, data + 2 + sizeof(zwif_p), sizeof(grp_cmd_ent_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the report is what we requested.
                    if (grp_id != nw->grp_rd_idx)
                    {
                        break;
                    }

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (grp_data->grp_info[grp_id-1] == NULL)
                    {
                        zw_grp_info_p   grp_info;
                        grp_info = (zw_grp_info_p)calloc(1, sizeof(zw_grp_info_t) + (cmd_ent_cnt * sizeof(grp_cmd_ent_t)));

                        if (grp_info)
                        {
                            grp_data->grp_info[grp_id-1] = grp_info;
                            grp_data->valid_grp_cnt++;

                            grp_info->grp_num = grp_id;
                            grp_info->cmd_ent_cnt = cmd_ent_cnt;
                            memcpy(grp_info->cmd_lst, cmd_lst, cmd_ent_cnt * sizeof(grp_cmd_ent_t));

                            //Get command list for next group
                            result = zwnet_sm_grp_cmd_lst_get(nw, intf);
                            if (result == 0)
                            {
                                break;
                            }
                            else if (result == 1)
                            {   //No more command list to get
                                //Get group name
                                nw->grp_rd_idx = 0;
                                result = zwnet_sm_grp_name_get(nw, intf);
                                if (result == 0)
                                {
                                    //Change sub-state
                                    nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_NAME;
                                    break;
                                }
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_GRP_NAME_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    zw_grp_info_p       grp_info;
                    uint8_t             *name;
                    uint8_t             grp_id;
                    uint8_t             str_len;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_NAME)
                    {
                        break;
                    }

                    grp_id = data[0];
                    str_len = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&name, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the report is what we requested.
                    if (grp_id != nw->grp_rd_idx)
                    {
                        break;
                    }

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    grp_info = grp_data->grp_info[grp_id-1];

                    if (grp_info)
                    {
                        //Check for valid UTF-8 string
                        str_len = plt_utf8_chk(name, str_len);

                        memcpy(grp_info->name, name, str_len);
                        grp_info->name[str_len] = '\0';
                    }

                    //Get group name for next group
                    result = zwnet_sm_grp_name_get(nw, intf);
                    if (result == 0)
                    {
                        break;
                    }
                    else if (result == 1)
                    {   //No more group name to get
                        //Get group info
                        nw->grp_rd_idx = 0;
                        result = zwnet_sm_grp_info_get(nw, intf);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = GRP_INFO_SUBSTA_INFO;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_GRP_INFO_REPORT)
                {
                    if_grp_info_dat_t   *grp_data;
                    int                 i;
                    zw_grp_info_p       grp_info;
                    zw_grp_info_ent_t   *grp_info_entries;
                    uint8_t             grp_cnt;
                    uint8_t             grp_id;
                    uint8_t             dynamic;

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != GRP_INFO_SUBSTA_INFO)
                    {
                        break;
                    }

                    grp_cnt = data[0];
                    dynamic = data[1];
                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&grp_info_entries, data + 2 + sizeof(zwif_p), sizeof(zw_grp_info_ent_t *));

                    grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    grp_data->dynamic = dynamic;

                    grp_id = 0;

                    for (i=0; i<grp_cnt; i++)
                    {
                        grp_id = grp_info_entries[i].grp_num;

                        if (grp_id <= grp_data->group_cnt)
                        {
                            grp_info = grp_data->grp_info[grp_id-1];

                            if (grp_info)
                            {
                                grp_info->profile = grp_info_entries[i].profile;
                                grp_info->evt_code = grp_info_entries[i].evt_code;
                            }
                        }
                    }

                    if ((grp_cnt == grp_data->group_cnt)
                        || (grp_id == grp_data->group_cnt))
                    {   //Done.
/*
                        if (nw->sm_job.cfg_lifeline)
                        {
                            //Configure lifeline
                            if (grp_data->grp_info[0] &&
                                (strcasecmp(grp_data->grp_info[0]->name, "lifeline") == 0))
                            {
                                zwnet_sm_lifeline_cfg(nw, intf);
                            }
                        }
*/

                        nw->ni_sm_sub_sta = 0;
                    }
                    else
                    {
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_FAN_MD);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_FAN_MD:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_FAN_MD)
                {
                    if_thrmo_fan_mode_data_t    *thrmo_fan_mode_dat;
                    uint8_t                     *mode;

                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&mode, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Free any existing data
                    if (intf->data_cnt > 0)
                    {
                        free(intf->data);
                    }

                    intf->data_cnt = 1;
                    intf->data_item_sz = sizeof(if_thrmo_fan_mode_data_t) + data[1] - 1;

                    thrmo_fan_mode_dat = (if_thrmo_fan_mode_data_t *)malloc(intf->data_item_sz);

                    if (thrmo_fan_mode_dat)
                    {
                        thrmo_fan_mode_dat->off = data[0];
                        thrmo_fan_mode_dat->mode_len = data[1];
                        memcpy(thrmo_fan_mode_dat->mode, mode, data[1]);

                        //Assign to interface
                        intf->data = thrmo_fan_mode_dat;
                    }
                    else
                    {
                        intf->data_cnt = 0;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_MD);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_MD:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_MD)
                {
                    if_thrmo_mode_data_t    *thrmo_mode_dat;
                    uint8_t                 *mode;

                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&mode, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Free any existing data
                    if (intf->data_cnt > 0)
                    {
                        free(intf->data);
                    }

                    intf->data_cnt = 1;
                    intf->data_item_sz = sizeof(if_thrmo_mode_data_t) + data[0] - 1;

                    thrmo_mode_dat = (if_thrmo_mode_data_t *)malloc(intf->data_item_sz);

                    if (thrmo_mode_dat)
                    {
                        thrmo_mode_dat->mode_len = data[0];
                        memcpy(thrmo_mode_dat->mode, mode, data[0]);

                        //Assign to interface
                        intf->data = thrmo_mode_dat;
                    }
                    else
                    {
                        intf->data_cnt = 0;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_THRMO_SETP);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_THRMO_SETP:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_THRMO_SETP)
                {
                    if_thrmo_setp_data_t    *thrmo_setp_dat;
                    uint8_t                 *setp;

                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&setp, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Free any existing data
                    if (intf->data_cnt > 0)
                    {
                        free(intf->data);
                    }

                    intf->data_cnt = 1;
                    intf->data_item_sz = sizeof(if_thrmo_setp_data_t) + data[0] - 1;

                    thrmo_setp_dat = (if_thrmo_setp_data_t *)malloc(intf->data_item_sz);

                    if (thrmo_setp_dat)
                    {
                        thrmo_setp_dat->setp_len = data[0];
                        memcpy(thrmo_setp_dat->setp, setp, data[0]);

                        //Assign to interface
                        intf->data = thrmo_setp_dat;
                    }
                    else
                    {
                        intf->data_cnt = 0;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MUL_SWITCH);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MUL_SWITCH:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_MUL_SWITCH)
                {
                    if_mul_switch_data_t    *mul_switch_data;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        mul_switch_data = (if_mul_switch_data_t *)malloc(sizeof(if_mul_switch_data_t));
                    }
                    else
                    {   //Use previously allocated memory
                        mul_switch_data = (if_mul_switch_data_t *)intf->data;
                    }

                    if (mul_switch_data)
                    {
                        intf->data_cnt = 1;
                        intf->data_item_sz = sizeof(if_mul_switch_data_t);
                        intf->data = mul_switch_data;
                        mul_switch_data->pri_type = data[0];
                        mul_switch_data->sec_type = data[1];
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_AV);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_AV:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_AV_REPORT_CNT)
                {
                    zwif_av_cap_t   *av_cap;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != AV_SUBSTA_RPT_CNT)
                    {
                        break;
                    }

                    if (intf->tmp_data)
                    {
                        av_cap = (zwif_av_cap_t *)intf->tmp_data;

                        //Clear data
                        if (av_cap->bit_mask_buf)
                        {
                            free(av_cap->bit_mask_buf);
                        }
                        memset(av_cap, 0, sizeof(zwif_av_cap_t));
                    }
                    else
                    {
                        intf->tmp_data = calloc(1, sizeof(zwif_av_cap_t));

                        av_cap = (zwif_av_cap_t *)intf->tmp_data;
                    }

                    if (av_cap && data[0])
                    {
                        av_cap->total_rpt = data[0];
                        //Start getting supported simple AV controls
                        result = zwnet_sm_av_ctl_get(nw, intf, av_cap);
                        if (result == 0)
                        {
                            //Change sub-state
                            nw->ni_sm_sub_sta = AV_SUBSTA_CTL_RPT;
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_AV_CTL_REPORT)
                {

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != AV_SUBSTA_CTL_RPT)
                    {
                        break;
                    }

                    if (intf->tmp_data)
                    {
                        uint8_t         rpt_num;        //Report number of this report
                        uint16_t        bit_mask_len;   //Bit mask length of this report
                        uint8_t         *tmp_buf;
                        uint8_t         *data_buf;
                        zwif_av_cap_t   *av_cap;

                        av_cap = (zwif_av_cap_t *)intf->tmp_data;

                        rpt_num = data[0];

                        bit_mask_len = data[1];

                        memcpy(&data_buf, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

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
                                   data_buf, bit_mask_len);

                            av_cap->rpt_num = rpt_num;
                            av_cap->bit_mask_len += bit_mask_len;

                            //Get next report
                            result = zwnet_sm_av_ctl_get(nw, intf, av_cap);
                            if (result == 0)
                            {
                                break;
                            }

                            if (result == 1)
                            {   //No more report to get
                                //Save the supported AV control bit-mask

                                //Free any existing data
                                if (intf->data_cnt > 0)
                                {
                                    free(intf->data);
                                }

                                if (av_cap->bit_mask_len)
                                {
                                    intf->data = malloc(av_cap->bit_mask_len);
                                    if (intf->data)
                                    {
                                        memcpy(intf->data, av_cap->bit_mask_buf, av_cap->bit_mask_len);
                                        intf->data_cnt = 1;
                                        intf->data_item_sz = (uint8_t)av_cap->bit_mask_len;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_ALARM);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);
            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_ALARM:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_ALARM_TYPE)
                {
                    if_alarm_data_t     *alarm_dat;
                    uint8_t             *ztype;

                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&ztype, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != ALARM_SUBSTA_TYPE)
                    {
                        break;
                    }

                    //Free any existing data
                    if (intf->data_cnt > 0)
                    {
                        free(intf->data);
                    }

                    intf->data_cnt = 1;
                    intf->data_item_sz = sizeof(if_alarm_data_t) + ((data[1] - 1)*sizeof(if_alarm_type_evt_t));

                    alarm_dat = (if_alarm_data_t *)calloc(1, intf->data_item_sz);

                    if (alarm_dat)
                    {
                        int i;
                        alarm_dat->have_vtype = data[0];
                        alarm_dat->type_evt_cnt = data[1];
                        for (i=0; i<alarm_dat->type_evt_cnt; i++)
                        {
                            alarm_dat->type_evt[i].ztype = ztype[i];
                        }

                        //Assign to interface
                        intf->data = alarm_dat;

                        //Start getting supported events of each alarm type
                        if (intf->ver > 2)
                        {
                            nw->alrm_rd_idx = 0;
                            result = zwnet_sm_alrm_evt_get(nw, intf);
                            if (result == 0)
                            {
                                //Change sub-state
                                nw->ni_sm_sub_sta = ALARM_SUBSTA_EVENT;
                                break;
                            }
                        }
                    }
                    else
                    {
                        intf->data_cnt = 0;
                    }
                }
                else if (evt == EVT_S_EP_ALARM_EVENT)
                {
                    if_alarm_data_t     *alarm_dat;
                    uint8_t             *evt_msk;
                    uint8_t             evt_len;
                    uint8_t             idx;

                    memcpy(&intf, data + 2, sizeof(zwif_p));
                    memcpy(&evt_msk, data + 2 + sizeof(zwif_p), sizeof(uint8_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    //Check whether sub-state is o.k.
                    if (nw->ni_sm_sub_sta != ALARM_SUBSTA_EVENT)
                    {
                        break;
                    }

                    alarm_dat = (if_alarm_data_t *)intf->data;
                    idx = nw->alrm_rd_idx;

                    //Check whether the alarm type is correct
                    if ((idx < alarm_dat->type_evt_cnt)
                        && (alarm_dat->type_evt[idx].ztype == data[0]))
                    {
                        //Save the supported events
                        evt_len = (data[1] > MAX_EVT_BITMASK_LEN)? MAX_EVT_BITMASK_LEN : data[1];
                        memcpy(alarm_dat->type_evt[idx].evt, evt_msk, evt_len);
                        alarm_dat->type_evt[idx].evt_len = evt_len;

                        //Get next supported events
                        nw->alrm_rd_idx++;
                        result = zwnet_sm_alrm_evt_get(nw, intf);
                        if (result == 0)
                        {
                            break;
                        }
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_PROT);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_PROT:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_PROT)
                {
                    zwprot_sup_t    *prot_sup_data;
                    zwprot_sup_t    *if_prot_data;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&prot_sup_data, data + sizeof(zwif_p), sizeof(zwprot_sup_t *));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        if_prot_data = (zwprot_sup_t *)malloc(sizeof(zwprot_sup_t));
                    }
                    else
                    {   //Use previously allocated memory
                        if_prot_data = (zwprot_sup_t *)intf->data;
                    }

                    if (if_prot_data)
                    {
                        intf->data_cnt = 1;
                        intf->data_item_sz = sizeof(zwprot_sup_t);
                        intf->data = if_prot_data;
                        *if_prot_data = *prot_sup_data;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_MAX_USR_CODES);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_MAX_USR_CODES:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_USR_CODE_CNT_REPORT)
                {
                    uint8_t *max_usr_codes;

                    memcpy(&intf, data + 2, sizeof(zwif_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        max_usr_codes = (uint8_t *)malloc(1);
                    }
                    else
                    {   //Use previously allocated memory
                        max_usr_codes = (uint8_t *)intf->data;
                    }

                    if (max_usr_codes)
                    {
                        intf->data_cnt = intf->data_item_sz = 1;
                        intf->data = max_usr_codes;
                        *max_usr_codes = data[0];

                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_METER);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_METER:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_METER)
                {
                    zwmeter_cap_p meter_cap;
                    zwmeter_cap_p if_meter_data;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&meter_cap, data + sizeof(zwif_p), sizeof(zwmeter_cap_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        if_meter_data = (zwmeter_cap_t *)malloc(sizeof(zwmeter_cap_t));
                    }
                    else
                    {   //Use previously allocated memory
                        if_meter_data = (zwmeter_cap_t *)intf->data;
                    }

                    if (if_meter_data)
                    {
                        intf->data_cnt = 1;
                        intf->data_item_sz = sizeof(zwmeter_cap_t);
                        intf->data = if_meter_data;
                        *if_meter_data = *meter_cap;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_METER_DESC);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_METER_DESC:
        //----------------------------------------------------------------
            {
                if (evt == EVT_S_EP_METER_TBL_DESC)
                {
                    zwmeter_p   meter_desc;
                    zwmeter_p   if_meter_data;

                    memcpy(&intf, data, sizeof(zwif_p));
                    memcpy(&meter_desc, data + sizeof(zwif_p), sizeof(zwmeter_p));

                    //Check whether the endpoint matches the request
                    if (intf->ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    if (intf->data_cnt == 0)
                    {
                        //Allocate memory for data
                        if_meter_data = (zwmeter_t *)malloc(sizeof(zwmeter_t));
                    }
                    else
                    {   //Use previously allocated memory
                        if_meter_data = (zwmeter_t *)intf->data;
                    }

                    if (if_meter_data)
                    {
                        intf->data_cnt = 1;
                        intf->data_item_sz = sizeof(zwmeter_t);
                        intf->data = if_meter_data;
                        *if_meter_data = *meter_desc;
                    }
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing other command classes
                result = zwnet_1_ep_sm_cls_hdlr(nw, nw->single_ep_job.first_ep, S_EP_ACT_NAME_LOC);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;

        //----------------------------------------------------------------
        case S_EP_STA_GET_NODE_NAMELOC:
        //----------------------------------------------------------------
            {
                zwep_p      ep;

                if (evt == EVT_S_EP_NAME_REPORT)
                {
                    zwifd_t ifd;
                    uint8_t str_len;    //string length

                    str_len = data[1];
                    memcpy(&intf, data + 2 + str_len, sizeof(zwif_p));
                    ep = intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    zwnet_sm_updt_nameloc(data, ep->name);

                    //Setup location report callback
                    zwif_get_desc(intf, &ifd);

                    result = zwif_set_report(&ifd, zwnet_loc_rpt_cb, NODE_NAMING_NODE_LOCATION_REPORT);
                    if (result == 0)
                    {
                        //Get the location
                        result = zwif_get_report(&ifd, NULL, 0, NODE_NAMING_NODE_LOCATION_GET, zwif_tx_sts_cb);
                        if (result >= 0)
                        {
                            //Restart timer
                            plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                            nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_1_ep_tmout_cb, nw);
                            break;
                        }
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_1_ep_info_sm: zwif_get_report for loc with error:%d", result);
                    }

                }
                else if (evt == EVT_S_EP_LOC_REPORT)
                {
                    uint8_t     str_len;    //string length

                    str_len = data[1];
                    memcpy(&intf, data + 2 + str_len, sizeof(zwif_p));
                    ep = intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->single_ep_job.first_ep)
                    {
                        break;
                    }

                    zwnet_sm_updt_nameloc(data, ep->loc);
                }
                else if (evt == EVT_S_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Completed job
                zwnet_1_ep_job_cmplt(nw);

            }
            break;
    }

    return 0;
}


/**
zwnet_sm_1_ep_info_cb - Get single endpoint info callback
@param[in]	nw		    Network
@return
*/
static void zwnet_sm_1_ep_info_cb(zwnet_p nw)
{
    zwnet_ep_info_sm(nw, EVT_EP_S_EP_INFO_DONE, NULL);
}


/**
zwnet_ep_info_sm - Endpoint info state-machine
@param[in] nw		Network
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return             The current state of the state-machine.
*/
static int zwnet_ep_info_sm(zwnet_p nw, zwnet_ep_evt_t evt, uint8_t *data)
{
    int curr_sta;
    int result;

    switch (nw->ep_sm_sta)
    {
        //----------------------------------------------------------------
        case EP_STA_IDLE:
        //----------------------------------------------------------------
            if (evt == EVT_EP_SM_START)
            {
                ep_sm_job_t  *ep_sm_job = (ep_sm_job_t *)data;

                if (ep_sm_job)
                {
                    //Copy the job
                    nw->ep_sm_job.cb = ep_sm_job->cb;
                    nw->ep_sm_job.first_ep = ep_sm_job->first_ep;
                }

                //Check whether the endpoint is a secure endpoint
                //and if so, get the security supported command classes
                result = zwnet_ep_sm_sec_get(nw, ep_sm_job->first_ep);
                if (result == 0)
                {
                    break;
                }

                //Start processing version command class
                result = zwnet_ep_sm_ver_get(nw, ep_sm_job->first_ep);
                if (result == 0)
                {
                    break;
                }
                //Start processing next endpoint
                zwnet_ep_sm_next(nw);
            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_SEC_CMD_SUP:
        //----------------------------------------------------------------
            {
                zwep_p  ep;

                if (evt == EVT_EP_SEC_SUP_REPORT)
                {
                    zwif_p  intf;
                    uint16_t *sec_cls;

                    memcpy(&sec_cls, data + 2, sizeof(uint16_t *));
                    memcpy(&intf, data + 2 + sizeof(uint16_t *), sizeof(zwif_p));

                    ep = intf->ep;

                    //Check whether the endpoint matches the request
                    if (ep != nw->ep_sm_job.first_ep)
                    {
                        break;
                    }

                    //Add the secure command classes to interfaces
                    zwnet_ni_sec_updt(ep, sec_cls, data[0]);

                }
                else if (evt == EVT_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //Start processing version command class
                result = zwnet_ep_sm_ver_get(nw, nw->ep_sm_job.first_ep);
                if (result == 0)
                {
                    break;
                }

                //Start processing next endpoint
                result = zwnet_ep_sm_next(nw);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                nw->ep_sm_sta = EP_STA_IDLE;

                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Invoke user callback
                if (nw->ep_sm_job.cb)
                {
                    nw->ep_sm_job.cb(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_CMD_VER:
        //----------------------------------------------------------------
            {
                ep_sm_job_t sm_job;
                zwep_p      ep;

                if (evt == EVT_EP_VER_REPORT)
                {
                    zwif_p      ver_intf;
                    uint16_t    cmd_cls;
                    uint8_t     cmd_cls_ver;

                    cmd_cls = data[0];

                    cmd_cls = (cmd_cls << 8) | data[1];

                    cmd_cls_ver = data[2];

                    //Check whether the reported command class matches the request
                    if (cmd_cls != nw->ni_sm_cls)
                    {
                        break;
                    }

                    memcpy(&ver_intf, data + 4, sizeof(zwif_p));
                    ep = ver_intf->ep;
                    //Check whether the endpoint matches the request
                    if (ep != nw->ep_sm_job.first_ep)
                    {
                        break;
                    }

                    result = zwnet_sm_ver_rpt_hdlr(nw, &ver_intf, zwnet_ep_ver_rpt_cb, cmd_cls, cmd_cls_ver);

                    if (result == 0)
                    {   //Successfully send command to query next command class version
                        //Restart timer
                        plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                        nw->sm_tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_TMOUT, zwnet_ep_tmout_cb, nw);
                        break;
                    }
                }
                else if (evt == EVT_EP_NW_TMOUT)
                {
                    ;
                }
                else
                {   //Unknown event for this state
                    break;
                }

                //
                //Start processing other command classes by using a single
                //endpoint info state-machine
                //

                //Stop timer
                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Initialize endpoint info state-machine
                //nw->ep_sm_sta = EP_STA_IDLE;

                sm_job.cb = zwnet_sm_1_ep_info_cb;
                sm_job.first_ep = nw->ep_sm_job.first_ep;

                if (zwnet_1_ep_info_sm(nw, EVT_S_EP_SM_START, (uint8_t *)&sm_job))
                {
                    //Change state
                    nw->ep_sm_sta = EP_STA_GET_EP_INFO;
                    break;
                }
                else
                {
                    if (nw->single_ep_sta != S_EP_STA_IDLE)
                    {
                        debug_zwapi_msg(&nw->plt_ctx, "zwnet_ep_info_sm: single endpoint state-machine in wrong state: %d", nw->single_ep_sta);
                    }
                }

                //Start processing next endpoint
                result = zwnet_ep_sm_next(nw);
                if (result == 0)
                {
                    break;
                }

                //Completed job
                nw->ep_sm_sta = EP_STA_IDLE;

                plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                nw->sm_tmr_ctx = NULL;

                //Invoke user callback
                if (nw->ep_sm_job.cb)
                {
                    nw->ep_sm_job.cb(nw);
                }

            }
            break;

        //----------------------------------------------------------------
        case EP_STA_GET_EP_INFO:
        //----------------------------------------------------------------
            {
                if (evt == EVT_EP_S_EP_INFO_DONE)
                {
                    //Start processing next endpoint
                    result = zwnet_ep_sm_next(nw);
                    if (result == 0)
                    {
                        break;
                    }

                    //Completed job
                    nw->ep_sm_sta = EP_STA_IDLE;

                    plt_tmr_stop(&nw->plt_ctx, nw->sm_tmr_ctx);
                    nw->sm_tmr_ctx = NULL;

                    //Invoke user callback
                    if (nw->ep_sm_job.cb)
                    {
                        nw->ep_sm_job.cb(nw);
                    }
                }
            }
            break;

    }
    curr_sta = nw->ep_sm_sta;

    return curr_sta;
}


/**
zwnet_poll_rm - remove a polling request
@param[in]	net	        network
@param[in]	handle	    handle of the polling request to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwnet_poll_rm(zwnet_p net, uint16_t handle)
{
    return zwpoll_rm(net, handle);
}


/**
zwnet_poll_rm_mul - remove multiple polling requests
@param[in]	net	        network
@param[in]	usr_token	usr_token of the polling requests to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwnet_poll_rm_mul(zwnet_p net, uint32_t usr_token)
{
    return zwpoll_rm_mul(net, usr_token);
}


/**
handle_association - Handler function for Association CC and AGI CC
@param[in]	cmd_len	    Length of the incoming command buffer
@param[in]	cmd_buf		Incoming command buffer
@param[in]	prm			Application command param context
*/
void handle_association(uint8_t cmd_len, uint8_t *cmd_buf, appl_cmd_prm_t *prm)
{
    zwnet_p     nw  = prm->nw;
    uint8_t     src_node = prm->src_node;
    uint8_t     msg_type = prm->msg_type;
	uint8_t		send_buf[46] = {0};
    uint8_t     i = 0;
    int         result = 0;

    switch (cmd_buf[0])
    {
        case COMMAND_CLASS_ASSOCIATION:
            {
                switch (cmd_buf[1])
                {
                    case ASSOCIATION_SET:
                        {
                            if (cmd_len >= 4)
                            {
                                //Since only support 1 group, ignore the group ID from the incoming buffer.
                                //Since only support 1 node, just fill in the one node. If there are more nodes, ignore.
                                AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;
                                if (pMatchAGIGroup->stNodeEpInfo.byNodeID == 0)
                                {   //If node list is already full, cannot assign
                                    pMatchAGIGroup->stNodeEpInfo.byNodeID = cmd_buf[3];
                                    pMatchAGIGroup->stNodeEpInfo.byEpID = 0;
                                }
                            }
                        }
                        break;

                    case ASSOCIATION_GET:
                        {
                            if (cmd_len >= 3)
                            {
                                //Since only support 1 group, ignore the group ID from the incoming buffer.
                                AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;

                                send_buf[i++] = cmd_buf[0];
                                send_buf[i++] = ASSOCIATION_REPORT;
                                send_buf[i++] = pMatchAGIGroup->byGroupID;
                                send_buf[i++] = pMatchAGIGroup->byMaxNodes;
                                send_buf[i++] = 0; //Report to follow

                                if (pMatchAGIGroup->stNodeEpInfo.byNodeID != 0)
                                    send_buf[i++] = pMatchAGIGroup->stNodeEpInfo.byNodeID;

                                result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                                if (result < 0)
                                    debug_zwapi_msg(&nw->plt_ctx, "Association ASSOCIATION_REPORT: Send rpt with error:%d", result);
                            }
                        }
                        break;

                    case ASSOCIATION_REMOVE:
                        {
                            if (cmd_len >= 3)
                            {
                                //Since only support 1 group, ignore the group ID from the incoming buffer.
                                AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;

                                if (cmd_buf[2] != pMatchAGIGroup->byGroupID)
                                {
                                    debug_zwapi_msg(&nw->plt_ctx, "Group ID not supported:%d", cmd_buf[2]);
                                    break;
                                }

                                //If no node list is supplied or the node ID is matched, remove the node ID.
                                if (cmd_len == 3)
                                {
                                    //group ID >= 1 && Number of node ID's in list 0
                                    //group ID = 0 && Number of node ID's in list 0
                                    if (cmd_buf[2] == pMatchAGIGroup->byGroupID || cmd_buf[2] == 0)
                                        pMatchAGIGroup->stNodeEpInfo.byNodeID = 0;  //Remove the node
                                }
                                else if (cmd_len >= 4)
                                {
                                    //group ID = 0 && Number of node ID's in list match record
                                    //group ID match record && Number of node ID's in list match record
                                    if (cmd_buf[2] == pMatchAGIGroup->byGroupID || cmd_buf[2] == 0)
                                    {
                                        if (cmd_buf[3] == pMatchAGIGroup->stNodeEpInfo.byNodeID)
                                            pMatchAGIGroup->stNodeEpInfo.byNodeID = 0;  //Remove the node
                                    }
                                }
                            }
                        }
                        break;

                    case ASSOCIATION_GROUPINGS_GET:
                        {
                            send_buf[i++] = cmd_buf[0];
                            send_buf[i++] = ASSOCIATION_GROUPINGS_REPORT;
                            send_buf[i++] = nw->stAGIData.byNofGroups;

                            result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                            if (result < 0)
                                debug_zwapi_msg(&nw->plt_ctx, "Association ASSOCIATION_GROUPINGS_REPORT: Send rpt with error:%d", result);

                        }
                        break;

                    case ASSOCIATION_SPECIFIC_GROUP_GET_V2:
                        {
                            AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;
                            send_buf[i++] = cmd_buf[0];
                            send_buf[i++] = ASSOCIATION_SPECIFIC_GROUP_REPORT_V2;
                            send_buf[i++] = pMatchAGIGroup->byGroupID;

                            result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                            if (result < 0)
                                debug_zwapi_msg(&nw->plt_ctx, "Association ASSOCIATION_SPECIFIC_GROUP_REPORT: Send rpt with error:%d", result);

                        }
                        break;
                }
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                switch (cmd_buf[1])
                {
                    case ASSOCIATION_GROUP_NAME_GET:
                        {
                            if (cmd_len >= 3)
                            {
                                size_t  name_len;
                                if (cmd_buf[2] != nw->stAGIData.pAGIGroupList[0].byGroupID)
                                {
                                    debug_zwapi_msg(&nw->plt_ctx, "Group ID not supported:%d", cmd_buf[2]);
                                    break;
                                }

                                send_buf[i++] = cmd_buf[0];
                                send_buf[i++] = ASSOCIATION_GROUP_NAME_REPORT;
                                send_buf[i++] = cmd_buf[2];
                                name_len = strlen(nw->stAGIData.pAGIGroupList[0].cchGroupName);
                                send_buf[i++] = name_len;
                                memcpy(send_buf + i, nw->stAGIData.pAGIGroupList[0].cchGroupName, name_len);

                                i += name_len;

                                result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                                if (result < 0)
                                    debug_zwapi_msg(&nw->plt_ctx, "AGI ASSOCIATION_GROUP_NAME_REPORT: Send rpt with error:%d", result);
                            }
                        }
                        break;

                    case ASSOCIATION_GROUP_INFO_GET:
                        {
                            if (cmd_len >= 4)
                            {
                                //Since we only support 1 group, ignore the 'list mode' field
                                uint8_t byListMode = cmd_buf[2] & ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_LIST_MODE_BIT_MASK,
                                                     //byRefreshCache = cmd_buf[2] & ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_REFRESH_CACHE_BIT_MASK,
                                                     byGroupID = cmd_buf[3];
                                AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;

                                if (byListMode == 0 && byGroupID != pMatchAGIGroup->byGroupID)
                                {
                                    debug_zwapi_msg(&nw->plt_ctx, "Group ID not supported:%d", byGroupID);
                                    break;
                                }

                                send_buf[i++] = cmd_buf[0];
                                send_buf[i++] = ASSOCIATION_GROUP_INFO_REPORT;
                                //on return report: Listmode = byListMode (bit 6 to bit 7), dynamic info = 0, group count = 1
                                send_buf[i++] = (byListMode << 1) | nw->stAGIData.byNofGroups;

                                send_buf[i++] = pMatchAGIGroup->byGroupID;

                                send_buf[i++] = 0; //Mode = 0
                                send_buf[i++] = (uint8_t)((pMatchAGIGroup->wProfile & 0xFF00) >> 8);
                                send_buf[i++] = (uint8_t)(pMatchAGIGroup->wProfile & 0xFF);
                                send_buf[i++] = 0; //Reserved
                                send_buf[i++] = (uint8_t)((pMatchAGIGroup->wEventCode & 0xFF00) >> 8);
                                send_buf[i++] = (uint8_t)(pMatchAGIGroup->wEventCode & 0xFF);

                                result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                                if (result < 0)
                                    debug_zwapi_msg(&nw->plt_ctx, "AGI ASSOCIATION_GROUP_INFO_REPORT: Send rpt with error:%d", result);
                            }
                        }
                        break;

                    case ASSOCIATION_GROUP_COMMAND_LIST_GET:
                        {
                            if (cmd_len >= 4)
                            {
                                uint8_t j = 0;
                                AGI_Group *pMatchAGIGroup = nw->stAGIData.pAGIGroupList;

                                if (cmd_buf[3] != pMatchAGIGroup->byGroupID)
                                {
                                    debug_zwapi_msg(&nw->plt_ctx, "Group ID not supported:%d", cmd_buf[3]);
                                    break;
                                }

                                send_buf[i++] = cmd_buf[0];
                                send_buf[i++] = ASSOCIATION_GROUP_COMMAND_LIST_REPORT;
                                send_buf[i++] = pMatchAGIGroup->byGroupID;
                                j = i++;
                                send_buf[i++] = pMatchAGIGroup->stCCList.CCMSB;

                                if (pMatchAGIGroup->stCCList.CCLSB != 0)
                                    send_buf[i++] = pMatchAGIGroup->stCCList.CCLSB;

                                send_buf[i++] = pMatchAGIGroup->stCCList.CCID;

                                send_buf[j] = i - j - 1;

                                result = zwnet_rpt_send(nw, send_buf, i, src_node, msg_type);
                                if (result < 0)
                                    debug_zwapi_msg(&nw->plt_ctx, "AGI ASSOCIATION_GROUP_COMMAND_LIST_REPORT: Send rpt with error:%d", result);
                            }
                        }
                        break;
                }
            }
            break;
    }
}


#ifdef FLASH_PROG_API

/**
zwnet_flash_prog - Start programming flash
@param[in]	net		        Network
@param[in]	comm_port_id	Pointer to platform specific serial comm port id/name. NULL for auto-select.
@param[in]	fw_file	        Firmware file
@return		ZW_ERR_xxx
*/
static int zwnet_flash_prog(zwnet_p net, void *comm_port_id, const char *fw_file)
{
    flash_prog_ctx_t    fl_ctx = {0};
    int         result;
    uint16_t    crc;
    uint8_t     rbap;
    uint8_t     nvr[FL_NVR_SZ];
    uint8_t     signature[8];
    uint8_t     signature_ok[5] = {0x7F, 0x7F, 0x7F, 0x7F, 0x1F};
#ifdef WIN32
    int         retry_cnt = 10;
    unsigned    prog_port = 0;
    unsigned    i;
    unsigned    port_count;
    unsigned    ports[80] = {0};
    TCHAR       *port_names[80] = {0};
    TCHAR       comm_port_name[16] = {0};
#endif

    //Check whether com port name for flash programming is given
    if (!comm_port_id)
    {
#ifdef WIN32
        //Auto select com port with name "Sigma Designs ZWave programming interface"

        while (retry_cnt-- > 0)
        {
            //Wait for Z-wave chip to reboot
            plt_sleep(500);
            port_count = 80;
            if (util_com_port_get(&port_count, ports, port_names))
            {
                for (i=0; i<port_count; i++)
                {
                    if (ports[i])
                    {
                        debug_zwapi_msg(&net->plt_ctx, "\r\nCom port number:%u", ports[i]);
                        if (port_names[i])
                        {   //Find the ZWave programming port
                            net->plt_ctx.print_txt(port_names[i]);

                            if (prog_port == 0
                                && _tcsnicmp(port_names[i], _T("Sigma Designs ZWave programming interface"), 41) == 0)
                            {   //Found
                                prog_port = ports[i];
                            }
                            //Free port names
                            free(port_names[i]);
                        }
                    }
                }

                if (prog_port)
                {
                    //Create serial comm port name for Windows platform
                    //Note: The comm port name must have a prefix "\\\\.\\" in order to access
                    //      COM10 or port number >= 10.
                    _stprintf_s(comm_port_name, sizeof(comm_port_name)/sizeof(TCHAR),
                                _T("\\\\.\\COM%u"), prog_port);

                    comm_port_id = comm_port_name;
                    break;
                }
            }
        }
#endif
    }
    else
    {
        //Wait for Z-wave chip to reboot
        plt_sleep(500);
    }

    fl_ctx.comm_port_id = comm_port_id;
    fl_ctx.plt_ctx = &net->plt_ctx;

    //Init flash programming com port
    result = fl_prog_init(&fl_ctx);
    if (result != 0)
    {
        return result;
    }

    //
    //Enable flash programming interface
    //
    result = fl_prog_enable(&fl_ctx);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't enable flash programming interface:%d", result);
        goto l_FLASH_PROG_ERROR1;
    }

    //
    //Read signature
    //
    result = fl_prog_sign_get(&fl_ctx, signature);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't get signature:%d", result);
        goto l_FLASH_PROG_ERROR2;
    }
    debug_zwapi_msg(&net->plt_ctx, "Chip signature:");
    debug_zwapi_bin_msg(&net->plt_ctx, signature, 7);

    if (memcmp(signature, signature_ok, 5) != 0)
    {
        result = ZWHCI_ERROR_VERIFY;
        goto l_FLASH_PROG_ERROR2;
    }

    //
    //Read NVR data from 0x09 to 0x7F
    //
    result = fl_prog_nvr_get(&fl_ctx, nvr + 0x09, 0x09, FL_SAVED_NVR_SZ);
    if (result == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "NVR from start addr 09h:");
        debug_zwapi_bin_msg(&net->plt_ctx, nvr + 0x09, FL_SAVED_NVR_SZ);
    }
    else
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't get nvr values:%d", result);
    }
    //Calculate checksum from 0x10 to 0x7F
    crc = zwutl_crc16_chk(CRC_INIT, nvr + 0x10, 112);
    if (crc == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "NVR CRC-16 checksum is o.k.");
    }

    //
    //Erase chip
    //
    result = fl_prog_erase_chip(&fl_ctx);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't erase chip:%d", result);
    }

    //
    //Clear AutoProg0 bit in NVR RBAP
    //
    rbap = 0xFD;
    result = fl_prog_nvr_set(&fl_ctx, &rbap, 0x08, 1);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't clear AutoProg0 bit:%d", result);
    }

    //
    //Write firmware to flash
    //
    result = zwutl_fl_fw_wr(&fl_ctx, fw_file);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't write firmware to flash:%d", result);
    }

    //
    //Write saved NVR data from 0x09 to 0x7F
    //

    //Calculate checksum from 0x10 to 0x7D
    crc = zwutl_crc16_chk(CRC_INIT, nvr + 0x10, (FL_SAVED_NVR_SZ - 2 - (0x10 - 0x09)));
    nvr[0x7E] = (crc >> 8);
    nvr[0x7F] = (crc & 0x00ff);

    //debug_zwapi_msg(&net->plt_ctx, "CRC:%04X", (unsigned)crc);

    result = fl_prog_nvr_set(&fl_ctx, nvr + 0x09, 0x09, FL_SAVED_NVR_SZ);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't write NVR:%d", result);
    }

    //
    //Clear AutoProg1 bit and Readback Protection bit in NVR RBAP
    //
    rbap = 0xFA;
    result = fl_prog_nvr_set(&fl_ctx, &rbap, 0x08, 1);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't clear AutoProg1 bit:%d", result);
    }

    //
    //Write lock bits
    //
//  result = fl_prog_lck_bits_set(&fl_ctx, 0xFC, 1);
//  if (result != 0)
//  {
//      debug_zwapi_msg(&net->plt_ctx, "Couldn't set lock bits:%d", result);
//  }

    //
    //Read lock bits
    //
//  result = fl_prog_lck_bits_get(&fl_ctx, lockbits);
//  if (result != 0)
//  {
//      debug_zwapi_msg(&net->plt_ctx, "Couldn't get lock bits:%d", result);
//      goto l_FLASH_PROG_ERROR2;
//  }
//  debug_zwapi_msg(&net->plt_ctx, "Lock bits:");
//  debug_zwapi_bin_msg(&net->plt_ctx, lockbits, 9);


    //o.k.
    result = ZW_ERR_NONE;

l_FLASH_PROG_ERROR2:
    //
    //Reset Chip
    //
    if (fl_prog_reset(&fl_ctx) == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Chip has been reset");
    }
    else
    {
        debug_zwapi_msg(&net->plt_ctx, "Chip has not been reset");
    }

l_FLASH_PROG_ERROR1:

    //Close com port
    fl_prog_exit(&fl_ctx);
    return result;
}


/**
zwnet_fw_updt - Local firmware update
@param[in]	net		Network
@param[in]	fw_file	Firmware file
@return		ZW_ERR_xxx
*/
int zwnet_fw_updt(zwnet_p net, const char *fw_file)
{
    int     result;
    FILE    *file;

    //Check whether flash programming is supported
    if ((net->ctlr_cap & CTLR_CAP_FLASH_PROG) == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Flash programming is unsupported");
        return ZW_ERR_UNSUPPORTED;
    }

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    //Check whether firmware file exists
    if (!fw_file)
    {
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_FILE_OPEN;
    }
#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, fw_file, "rb") != 0)
    {
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_FILE_OPEN;
    }
#else
    file = fopen(fw_file, "rb");
    if (!file)
    {
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_FILE_OPEN;
    }
#endif
    fclose(file);

    //Switch to Auto Programming Mode
    result = zw_flash_auto_prog_mode(&net->appl_ctx);

    if (result == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "\r\n***  Flashing firmware, do not power off device ***");
    }
    else
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnet_fw_updt with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }

    net->curr_op = ZWNET_OP_FW_UPDATE;
    plt_mtx_ulck(net->mtx);

#ifdef WIN32
    result = zwnet_flash_prog(net, NULL, fw_file);
#else
    result = zwnet_flash_prog(net, "/dev/ttyAPM", fw_file);
#endif


    if (result == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Please power cycle the device for new firmware to take effect.");
    }
    else
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnet_flash_prog with error:%d", result);
        return ZW_ERR_OP_FAILED;
    }
    return ZW_ERR_NONE;
}

#ifdef OS_LINUX
/**
zwnet_auto_prog - Enables the Auto Program Mode and resets the 500 Sereis Z-Wave chip
@param[in]	net		Network
@return		ZW_ERR_xxx
*/
int zwnet_auto_prog(zwnet_p net)
{
    flash_prog_ctx_t    fl_ctx = {0};
    uint8_t     signature[8];
    uint8_t     signature_ok[5] = {0x7F, 0x7F, 0x7F, 0x7F, 0x1F};
    int         result;

    plt_mtx_lck(net->mtx);
    if (net->curr_op != ZWNET_OP_NONE)
    {
        debug_zwapi_msg(&net->plt_ctx, "Current operation not completed yet, try again later");
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_LAST_OP_NOT_DONE;
    }

    result = zw_flash_auto_prog_mode(&net->appl_ctx);

    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "zwnet_auto_prog with error:%d", result);
        plt_mtx_ulck(net->mtx);
        return ZW_ERR_OP_FAILED;
    }

    fl_ctx.comm_port_id = "/dev/ttyAPM";
    fl_ctx.plt_ctx = &net->plt_ctx;

    //Init flash programming com port
    result = fl_prog_init(&fl_ctx);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't init flash programming comm port:%d", result);
        return result;
    }

    //
    //Enable flash programming interface
    //
    result = fl_prog_enable(&fl_ctx);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't enable flash programming interface:%d", result);
        fl_prog_exit(&fl_ctx);
        return result;

    }

    //
    //Read signature
    //
    result = fl_prog_sign_get(&fl_ctx, signature);
    if (result != 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Couldn't get signature:%d", result);
        if (fl_prog_reset(&fl_ctx) == 0)
        {
            debug_zwapi_msg(&net->plt_ctx, "Chip has been reset");
        }
        else
        {
            debug_zwapi_msg(&net->plt_ctx, "Chip has not been reset");
        }

        fl_prog_exit(&fl_ctx);
        return result;
    }
    debug_zwapi_msg(&net->plt_ctx, "Chip signature:");
    debug_zwapi_bin_msg(&net->plt_ctx, signature, 7);

    if (memcmp(signature, signature_ok, 5) != 0)
    {
        result = ZWHCI_ERROR_VERIFY;
        debug_zwapi_msg(&net->plt_ctx, "Wrong Chip signature");
    }

    //net->curr_op = ZWNET_OP_FW_UPDATE;
    plt_mtx_ulck(net->mtx);

    if (fl_prog_reset(&fl_ctx) == 0)
    {
        debug_zwapi_msg(&net->plt_ctx, "Chip has been reset");
    }
    else
    {
        debug_zwapi_msg(&net->plt_ctx, "Chip has not been reset");
    }

    fl_prog_exit(&fl_ctx);
    return result;
}
#endif
#endif

/**
zwnet_quick_reset_cb - Restore factory default callback function
@param[in]	appl_ctx    The application layer context
@return
*/
static void zwnet_quick_reset_cb(struct _appl_layer_ctx *appl_ctx)
{
    zwnet_p nw = (zwnet_p)appl_ctx->data;

    plt_mtx_lck(nw->rst_mtx);

    //Release the waiting for callback flag
    nw->rst_cb_sts = 1;

    //Wake up any wait thread
    plt_cond_signal(nw->rst_cv);

    plt_mtx_ulck(nw->rst_mtx);

}


/**
zwnet_quick_reset - Quick reset of controller without initialization of network
@param[in]	comm_port_name	Pointer to platform-dependent serial comm port name
@return		ZW_ERR_xxx
@note   This API should not be used if zwnet_init() returns 0, instead, use zwnet_reset(). Calling
        this API while the network has been initialized and not yet closed can cause undesirable
        effects on the network.
*/
int zwnet_quick_reset(void *comm_port_name)
{
    int         ret;
    int         result;
    zwnet_p     nw;

    //Allocate memory for network internal structure
    nw = (zwnet_p) calloc(1, sizeof(zwnet_t));

    if (!nw)
        return ZW_ERR_MEMORY;

    //Initialize platform
    if (plt_init(&nw->plt_ctx, NULL) != 0)
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_RESET_ERROR1;
    }

    if (!plt_mtx_init(&nw->mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_RESET_ERROR2;
    }

    if (!plt_mtx_init(&nw->rst_mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_RESET_ERROR3;
    }

    if (plt_cond_init(&nw->rst_cv) == 0)
    {
        result = ZW_ERR_NO_RES;
        goto l_ZWNET_RESET_ERROR4;
    }


    //Init all the back links
    nw->ctl.net = nw;
    nw->ctl.ep.node = &nw->ctl;

    //Init zwhci
    nw->appl_ctx.application_command_handler_cb = application_command_handler_cb;
    printf("linhui, set application_command_handler_cb 1\n");
    nw->appl_ctx.application_update_cb = application_update_cb;
    nw->appl_ctx.cb_tmout_ms = APPL_CB_TMOUT_MIN;
    nw->appl_ctx.data = nw;
    nw->appl_ctx.plt_ctx = &nw->plt_ctx;

    result = zwhci_init(&nw->appl_ctx, comm_port_name);

    if (result != 0)
    {
        result = ZW_ERR_TRANSPORT_INI - (result - 1);
        goto l_ZWNET_RESET_ERROR5;
    }

    //Reset the controller
    result = zw_set_default(&nw->appl_ctx, zwnet_quick_reset_cb);

    if (result != 0)
    {
        result = ZW_ERR_OP_FAILED;
        goto l_ZWNET_RESET_ERROR6;
    }

    //Wait for callback
    plt_mtx_lck(nw->rst_mtx);
    ret = 0;
    while ((nw->rst_cb_sts == 0) && ret == 0)
        ret = plt_cond_timedwait(nw->rst_cv, nw->rst_mtx, 3000);

    if (ret == 1)
    {   //Wait timeout
        result = ZW_ERR_TIMEOUT;
    }

    if (nw->rst_cb_sts)
    {
        result = 0; //Wait success
    }

    plt_mtx_ulck(nw->rst_mtx);


    //Shutdown all lower layers
l_ZWNET_RESET_ERROR6:
    zwhci_exit(&nw->appl_ctx);

l_ZWNET_RESET_ERROR5:
    plt_cond_destroy(nw->rst_cv);
l_ZWNET_RESET_ERROR4:
    plt_mtx_destroy(nw->rst_mtx);
l_ZWNET_RESET_ERROR3:
    plt_mtx_destroy(nw->mtx);
l_ZWNET_RESET_ERROR2:
    plt_exit(&nw->plt_ctx);
l_ZWNET_RESET_ERROR1:
    free(nw);
    return result;

}

#ifdef INTERNAL_TEST
uint32_t zwnet_get_tx_tmout_cnt(zwnet_p net)
{
    return net->sec_ctx->tx_tmout_cnt;
}
#endif


/**
@}
*/



