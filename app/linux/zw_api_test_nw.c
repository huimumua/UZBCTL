/**
@file   zw_api_test_nw.c - Z-wave host controller API test program.

        Network management part of the test program.

@author David Chow

@version    1.0 13-4-11  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include "zw_api_test.h"

extern const char *sensor_type_str[];
extern const char *sensor_unit_str[];
extern const char *dev_category_str[];

/**
hl_class_str_get - Get command class string
@param[in]	cls	        class
@param[in]	ver	        version of the command class
@return		Command class string if found, else return string "UNKNOWN"
*/
static char *hl_class_str_get(uint16_t cls, uint8_t ver)
{

    switch (cls)
    {
        case COMMAND_CLASS_BASIC:
            {
                return "COMMAND_CLASS_BASIC";
            }
            break;

        case COMMAND_CLASS_SWITCH_MULTILEVEL:
            {
                return "COMMAND_CLASS_SWITCH_MULTILEVEL";
            }
            break;

        case COMMAND_CLASS_SWITCH_BINARY:
            {
                return "COMMAND_CLASS_SWITCH_BINARY";
            }
            break;

        case COMMAND_CLASS_SWITCH_ALL:
            {
                return "COMMAND_CLASS_SWITCH_ALL";
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
            {
                return "COMMAND_CLASS_MANUFACTURER_SPECIFIC";
            }
            break;

        case COMMAND_CLASS_VERSION:
            {
                return "COMMAND_CLASS_VERSION";
            }
            break;

        case COMMAND_CLASS_POWERLEVEL:
            {
                return "COMMAND_CLASS_POWERLEVEL";
            }
            break;

        case COMMAND_CLASS_CONTROLLER_REPLICATION:
            {
                return "COMMAND_CLASS_CONTROLLER_REPLICATION";
            }
            break;

        case COMMAND_CLASS_NODE_NAMING:
            {
                return "COMMAND_CLASS_NODE_NAMING";
            }
            break;

        case COMMAND_CLASS_SENSOR_BINARY:
            {
                return "COMMAND_CLASS_SENSOR_BINARY";
            }
            break;

        case COMMAND_CLASS_SENSOR_MULTILEVEL:
            {
                return "COMMAND_CLASS_SENSOR_MULTILEVEL";
            }
            break;

        case COMMAND_CLASS_ASSOCIATION:
            {
                return "COMMAND_CLASS_ASSOCIATION";
            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2:
            {
                if (ver >= 2)
                {
                    return "COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION";
                }
                return "COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION";
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION:
            {
                return "COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION";
            }
            break;

        case COMMAND_CLASS_NO_OPERATION:
            {
                return "COMMAND_CLASS_NO_OPERATION";
            }
            break;

        case COMMAND_CLASS_MULTI_CHANNEL_V2:
            {
                if (ver >= 2)
                {
                    return "COMMAND_CLASS_MULTI_CHANNEL";
                }
                return "COMMAND_CLASS_MULTI_INSTANCE";
            }
            break;

        case COMMAND_CLASS_WAKE_UP:
            {
                return "COMMAND_CLASS_WAKE_UP";
            }
            break;

        case COMMAND_CLASS_MANUFACTURER_PROPRIETARY:
            {
                return "COMMAND_CLASS_MANUFACTURER_PROPRIETARY";
            }
            break;

        case COMMAND_CLASS_METER_TBL_MONITOR:
            {
                return "COMMAND_CLASS_METER_TBL_MONITOR";
            }
            break;

        case COMMAND_CLASS_METER_TBL_CONFIG:
            {
                return "COMMAND_CLASS_METER_TBL_CONFIG";
            }
            break;

        case COMMAND_CLASS_METER:
            {
                return "COMMAND_CLASS_METER";
            }
            break;

        case COMMAND_CLASS_METER_PULSE:
            {
                return "COMMAND_CLASS_METER_PULSE";
            }
            break;

        case COMMAND_CLASS_SIMPLE_AV_CONTROL:
            {
                return "COMMAND_CLASS_SIMPLE_AV_CONTROL";
            }
            break;

        case COMMAND_CLASS_CONFIGURATION:
            {
                return "COMMAND_CLASS_CONFIGURATION";
            }
            break;

        case COMMAND_CLASS_INDICATOR:
            {
                return "COMMAND_CLASS_INDICATOR";
            }
            break;

        case COMMAND_CLASS_SECURITY:
            {
                return "COMMAND_CLASS_SECURITY";
            }
            break;

        case COMMAND_CLASS_HAIL:
            {
                return "COMMAND_CLASS_HAIL";
            }
            break;

        case COMMAND_CLASS_PROTECTION:
            {
                return "COMMAND_CLASS_PROTECTION";
            }
            break;

        case COMMAND_CLASS_SWITCH_TOGGLE_BINARY:
            {
                return "COMMAND_CLASS_SWITCH_TOGGLE_BINARY";
            }
            break;

        case COMMAND_CLASS_BATTERY:
            {
                return "COMMAND_CLASS_BATTERY";
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK:
            {
                return "COMMAND_CLASS_DOOR_LOCK";
            }
            break;

        case COMMAND_CLASS_USER_CODE:
            {
                return "COMMAND_CLASS_USER_CODE";
            }
            break;

        case COMMAND_CLASS_ALARM:
            {
                if (ver >= 3)
                {
                    return "COMMAND_CLASS_NOTIFICATION";
                }
                return "COMMAND_CLASS_ALARM";
            }
            break;

        case COMMAND_CLASS_SCHEDULE_ENTRY_LOCK:
            {
                return "COMMAND_CLASS_SCHEDULE_ENTRY_LOCK";
            }
            break;

        case COMMAND_CLASS_DOOR_LOCK_LOGGING:
            {
                return "COMMAND_CLASS_DOOR_LOCK_LOGGING";
            }
            break;

        case COMMAND_CLASS_TIME_PARAMETERS:
            {
                return "COMMAND_CLASS_TIME_PARAMETERS";
            }
            break;

        case COMMAND_CLASS_CRC_16_ENCAP:
            {
                return "COMMAND_CLASS_CRC_16_ENCAP";
            }
            break;

        case COMMAND_CLASS_TRANSPORT_SERVICE:
            {
                return "COMMAND_CLASS_TRANSPORT_SERVICE";
            }
            break;

        case COMMAND_CLASS_ZIP:
            {
                return "COMMAND_CLASS_ZIP";
            }
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY:
            {
                return "COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY";
            }
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION:
            {
                return "COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION";
            }
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC:
            {
                return "COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC";
            }
            break;

        case COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY:
            {
                return "COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_MODE:
            {
                return "COMMAND_CLASS_THERMOSTAT_FAN_MODE";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_FAN_STATE:
            {
                return "COMMAND_CLASS_THERMOSTAT_FAN_STATE";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_MODE:
            {
                return "COMMAND_CLASS_THERMOSTAT_MODE";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_OPERATING_STATE:
            {
                return "COMMAND_CLASS_THERMOSTAT_OPERATING_STATE";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETPOINT:
            {
                return "COMMAND_CLASS_THERMOSTAT_SETPOINT";
            }
            break;

        case COMMAND_CLASS_THERMOSTAT_SETBACK:
            {
                return "COMMAND_CLASS_THERMOSTAT_SETBACK";
            }
            break;

        case COMMAND_CLASS_CLOCK:
            {
                return "COMMAND_CLASS_CLOCK";
            }
            break;

        case COMMAND_CLASS_LOCK:
            {
                return "COMMAND_CLASS_LOCK";
            }
            break;

        case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
            {
                return "COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE";
            }
            break;

        case COMMAND_CLASS_MULTI_CMD:
            {
                return "COMMAND_CLASS_MULTI_CMD";
            }
            break;

        case COMMAND_CLASS_APPLICATION_STATUS:
            {
                return "COMMAND_CLASS_APPLICATION_STATUS";
            }
            break;

        case COMMAND_CLASS_FIRMWARE_UPDATE_MD:
            {
                return "COMMAND_CLASS_FIRMWARE_UPDATE_MD";
            }
            break;

        case COMMAND_CLASS_ZWAVEPLUS_INFO:
            {
                return "COMMAND_CLASS_ZWAVEPLUS_INFO";
            }
            break;

        case COMMAND_CLASS_DEVICE_RESET_LOCALLY:
            {
                return "COMMAND_CLASS_DEVICE_RESET_LOCALLY";
            }
            break;

        case COMMAND_CLASS_ASSOCIATION_GRP_INFO:
            {
                return "COMMAND_CLASS_ASSOCIATION_GRP_INFO";
            }
            break;

        case COMMAND_CLASS_SCENE_ACTIVATION:
            {
                return "COMMAND_CLASS_SCENE_ACTIVATION";
            }
            break;

        case COMMAND_CLASS_SCENE_ACTUATOR_CONF:
            {
                return "COMMAND_CLASS_SCENE_ACTUATOR_CONF";
            }
            break;

        case COMMAND_CLASS_SCENE_CONTROLLER_CONF:
            {
                return "COMMAND_CLASS_SCENE_CONTROLLER_CONF";
            }
            break;

        case COMMAND_CLASS_ZIP_GATEWAY:
            {
                return "COMMAND_CLASS_ZIP_GATEWAY";
            }
            break;

        case COMMAND_CLASS_ZIP_PORTAL:
            {
                return "COMMAND_CLASS_ZIP_PORTAL";
            }
#ifdef  TEST_EXT_CMD_CLASS
        case COMMAND_CLASS_EXT_TEST:        //Testing of extended command class
            {
                return "COMMAND_CLASS_EXT_TEST";
            }
            break;
#endif
        default:
            return "UNKNOWN";
    }
}


/**
hl_plt_ctx_get - Get platform context
@return         platform context
*/
void  *hl_plt_ctx_get(hl_appl_ctx_t *hl_appl)
{
    if (!hl_appl->plt_ctx)
    {
        zwnetd_t    *net_desc;
        //Get and save the context
        net_desc = zwnet_get_desc(hl_appl->zwnet);

        hl_appl->plt_ctx = net_desc->plt_ctx;
    }

    return hl_appl->plt_ctx;
}


/**
hl_if_plt_ctx_get - Get platform context
@return         platform context
*/
void  *hl_if_plt_ctx_get(zwifd_p ifd)
{
    zwnetd_t    *net_desc;
    //Get and save the context
    net_desc = zwnet_get_desc(ifd->net);
    return net_desc->plt_ctx;
}


/**
hl_ep_plt_ctx_get - Get platform context
@return         platform context
*/
void  *hl_ep_plt_ctx_get(zwepd_p ep)
{
    zwnetd_t    *net_desc;
    //Get and save the context
    net_desc = zwnet_get_desc(ep->net);
    return net_desc->plt_ctx;
}


/**
hl_node_plt_ctx_get - Get platform context
@return         platform context
*/
void  *hl_node_plt_ctx_get(zwnoded_p node)
{
    zwnetd_t    *net_desc;
    //Get and save the context
    net_desc = zwnet_get_desc(node->net);
    return net_desc->plt_ctx;
}


/**
hl_desc_id_gen - genarate unique descriptor id
@param[in]	nw		Network
@return         Generated descriptor id
*/
static uint32_t  hl_desc_id_gen(zwnet_p nw)
{
    hl_appl_ctx_t       *hl_appl;
    zwnetd_t            *net_desc;

    net_desc = zwnet_get_desc(nw);

    hl_appl = (hl_appl_ctx_t *)net_desc->user;

    if (hl_appl->desc_id == 0)
    {   //ID of zero is invalid
        hl_appl->desc_id++;
    }
    return hl_appl->desc_id++;
}


/**
hl_desc_cont_del - delete node descriptor container for the specified id
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	desc_id		Descriptor id of the node descriptor container
@return
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
static void hl_desc_cont_del(desc_cont_t **head, uint32_t desc_id)
{
    desc_cont_t     *last_node_cont;
    desc_cont_t     *last_ep_cont;
    desc_cont_t     *last_intf_cont;
    desc_cont_t     *next_desc_cont;
    desc_cont_t     *prev_desc_cont;    //Previous descriptor container
    zwifd_p         ifd;

    //Start searching from the first node
    last_node_cont = *head;
    prev_desc_cont = NULL;

    while (last_node_cont)
    {
        if (last_node_cont->id == desc_id)
        {
            //Delete endpoint
            last_ep_cont = last_node_cont->down;

            while (last_ep_cont)
            {

                //Delete interface
                last_intf_cont = last_ep_cont->down;

                while (last_intf_cont)
                {
                    //Store the next interface container
                    next_desc_cont = last_intf_cont->next;

                    //Free command class specific data
                    ifd = (zwifd_p)last_intf_cont->desc;

                    if (ifd->data_cnt > 0)
                    {
                        free(ifd->data);
                    }

                    //Free interface container
                    free(last_intf_cont);

                    //Get the next interface
                    last_intf_cont = next_desc_cont;
                }

                //Store the next endpoint container
                next_desc_cont = last_ep_cont->next;

                free(last_ep_cont);

                //Get the next endpoint
                last_ep_cont = next_desc_cont;
            }
            //Check whether this is the first node
            if (!prev_desc_cont)
            {   //First node
                *head = last_node_cont->next;
                free(last_node_cont);
                return;
            }
            //Not the first node
            prev_desc_cont->next = last_node_cont->next;
            free(last_node_cont);
            return;

        }

        //Get the next node
        prev_desc_cont = last_node_cont;
        last_node_cont = last_node_cont->next;
    }
}


/**
hl_desc_cont_rm_all - remove the whole descriptor container linked-list
@param[in]	head	    The head of the descriptor container linked-list
@return
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
static void hl_desc_cont_rm_all(desc_cont_t **head)
{
    while (*head)
    {
        hl_desc_cont_del(head, (*head)->id);
    }
}


/**
hl_desc_id_get - get node descriptor id
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	noded	Node descriptor
@return     Node descriptor id if found; else return 0
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
uint32_t    hl_desc_id_get(desc_cont_t *head, zwnoded_p noded)
{
    desc_cont_t     *curr_node_cont;
    zwnoded_p       curr_node_desc;     //current node descriptor

    //Start searching from the first node
    curr_node_cont = head;

    while (curr_node_cont)
    {
        curr_node_desc = (zwnoded_p)curr_node_cont->desc;

        if (curr_node_desc->nodeid == noded->nodeid)
        {
            return curr_node_cont->id;
        }

        //Get the next node
        curr_node_cont = curr_node_cont->next;
    }
    return 0;
}


/**
hl_desc_cont_add - add a node into the descriptor container linked-list
@param[in]	head	The head of the descriptor container linked-list
@param[in]	noded	Node descriptor
@return Zero on success; else return ZW_ERR_XXX
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
int    hl_desc_cont_add(desc_cont_t **head, zwnoded_p noded)
{
    uint32_t    desc_id;
    int         result;
    zwepd_t     ep_desc;
    zwifd_t     ifd;
    zwnoded_t   updt_node;
    zwnoded_p   node;
    zwepd_p     ep;
    zwifd_p     intf;
    desc_cont_t     *last_node_cont;
    desc_cont_t     *last_ep_cont;
    desc_cont_t     *last_intf_cont;
    desc_cont_t     *new_node_cont;

    //Check whether the node already exists
    desc_id = hl_desc_id_get(*head, noded);

    if (desc_id)
    {   //Delete the existing node container
        hl_desc_cont_del(head, desc_id);
    }

    //Get the updated node descriptor
    if (zwnode_get_ep(noded, &ep_desc) < 0)
    {
        return ZW_ERR_EP_NOT_FOUND;
    }

    if (zwep_get_node(&ep_desc, &updt_node) < 0)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }


    //Check whether this is the first node to be added
    if (*head == NULL)
    {   //This is the first node to be added
        new_node_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t)
                                              + sizeof(zwnoded_t) - 1);
        if (!new_node_cont)
        {
            return ZW_ERR_MEMORY;
        }
        *head = new_node_cont;

    }
    else
    {   //Add new node container at the end of the list
        last_node_cont = *head;

        while (last_node_cont->next)
        {
            //Get the next node
            last_node_cont = last_node_cont->next;
        }
        new_node_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t)
                                              + sizeof(zwnoded_t) - 1);
        if (!new_node_cont)
        {
            return ZW_ERR_MEMORY;
        }

        last_node_cont->next = new_node_cont;
    }
    //Init the node container
    new_node_cont->type = DESC_TYPE_NODE;
    new_node_cont->id = hl_desc_id_gen(noded->net);
    node = (zwnoded_p)new_node_cont->desc;
    *node = updt_node;

    //Add endpoints

    last_ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
    if (!last_ep_cont)
    {
        return ZW_ERR_MEMORY;
    }
    last_ep_cont->type = DESC_TYPE_EP;
    last_ep_cont->id = hl_desc_id_gen(noded->net);
    ep = (zwepd_p)last_ep_cont->desc;
    zwnode_get_ep(node, ep);
    new_node_cont->down = last_ep_cont;


    while (ep)
    {
        if (zwep_get_if(ep, &ifd) < 0)
        {
            break;
        }

        //Add interfaces
        last_intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
        if (!last_intf_cont)
        {
            return ZW_ERR_MEMORY;
        }

        last_intf_cont->type = DESC_TYPE_INTF;
        last_intf_cont->id = hl_desc_id_gen(noded->net);
        intf = (zwifd_p)last_intf_cont->desc;
        *intf = ifd;
        last_ep_cont->down = last_intf_cont;

        while (intf)
        {
            //Get the next interface
            result = zwif_get_next(intf, &ifd);
            if (result == 0)
            {
                desc_cont_t     *intf_cont;
                zwifd_p         ifdp;

                intf = &ifd;
                intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
                if (!intf_cont)
                {
                    return ZW_ERR_MEMORY;
                }
                intf_cont->type = DESC_TYPE_INTF;
                intf_cont->id = hl_desc_id_gen(noded->net);
                ifdp = (zwifd_p)intf_cont->desc;
                *ifdp = ifd;
                last_intf_cont->next = intf_cont;
                last_intf_cont = intf_cont;

            }
            else
            {
                intf = NULL;
            }
        }

        //Get the next endpoint
        result = zwep_get_next(ep, &ep_desc);
        if (result == 0)
        {
            desc_cont_t     *ep_cont;
            zwepd_p         epp;

            ep = &ep_desc;
            ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
            if (!ep_cont)
            {
                return ZW_ERR_MEMORY;
            }
            ep_cont->type = DESC_TYPE_EP;
            ep_cont->id = hl_desc_id_gen(noded->net);
            epp = (zwepd_p)ep_cont->desc;
            *epp = ep_desc;
            last_ep_cont->next = ep_cont;
            last_ep_cont = ep_cont;

        }
        else
        {
            ep = NULL;
        }
    }

    return 0;
}


/**
hl_desc_cont_get - get the descriptor container for the specified id
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	desc_id		    Unique descriptor id
@return     Descriptor container if found; else return NULL
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
static desc_cont_t    *hl_desc_cont_get(desc_cont_t *head, uint32_t desc_id)
{
    desc_cont_t     *last_node_cont;
    desc_cont_t     *last_ep_cont;
    desc_cont_t     *last_cls_cont;
    desc_cont_t     *last_intf_cont;

    //Start searching from the first node
    last_node_cont = head;

    while (last_node_cont)
    {
        printf("linhui last_node_cont->id= %d\n",last_node_cont->id);
        if (last_node_cont->id == desc_id)
        {
            return last_node_cont;
        }

        //Search endpoint
        last_ep_cont = last_node_cont->down;

        while (last_ep_cont)
        {
            printf("linhui, last_ep_cont->id=%d\n",last_ep_cont->id );
            if (last_ep_cont->id == desc_id)
            {
                return last_ep_cont;
            }

            //Search class
            last_cls_cont = last_ep_cont->down;

            while (last_cls_cont)
            {

                if (last_cls_cont->id == desc_id)
                {
                    return last_cls_cont;
                }

                //Search interface
                last_intf_cont = last_cls_cont->down;

                while (last_intf_cont)
                {
                    if (last_intf_cont->id == desc_id)
                    {
                        return last_intf_cont;
                    }
                    //Get the next interface
                    last_intf_cont = last_intf_cont->next;
                }
                //Get the next class
                last_cls_cont = last_cls_cont->next;
            }
            //Get the next endpoint
            last_ep_cont = last_ep_cont->next;
        }
        //Get the next node
        last_node_cont = last_node_cont->next;
    }
    return NULL;
}


/**
hl_intf_desc_get - get interface descriptor from descriptor container
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	desc_id		Unique descriptor id
@return     Interface descriptor if found; else return NULL
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
zwifd_p    hl_intf_desc_get(desc_cont_t *head, uint32_t desc_id)
{
    printf("interface desc get, desc_id=%d\n",desc_id);
    desc_cont_t *desc_cont;

    //Get the interface descriptor
    desc_cont = hl_desc_cont_get(head, desc_id);
    if (!desc_cont)
    {
        printf("linhui, invalid desc id:%u\n",desc_id);
        //plt_msg_ts_show("hl_intf_desc_get invalid desc id:%u", desc_id);
        return NULL;
    }

    if (desc_cont->type != DESC_TYPE_INTF)
    {
        printf("linhui,hl_intf_desc_get desc id:%u is not type interface\n", desc_id);
        //plt_msg_ts_show("hl_intf_desc_get desc id:%u is not type interface", desc_id);
        return NULL;
    }

    return(zwifd_p)desc_cont->desc;
}


/**
hl_ep_desc_get - get endpoint descriptor from descriptor container
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	desc_id		Unique descriptor id
@return     Endpoint descriptor if found; else return NULL
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
zwepd_p    hl_ep_desc_get(desc_cont_t *head, uint32_t desc_id)
{
    desc_cont_t *desc_cont;

    //Get the interface descriptor
    desc_cont = hl_desc_cont_get(head, desc_id);
    if (!desc_cont)
    {
        //plt_msg_ts_show("hl_ep_desc_get invalid desc id:%u", desc_id);
        return NULL;
    }

    if (desc_cont->type != DESC_TYPE_EP)
    {
        //plt_msg_ts_show("hl_ep_desc_get desc id:%u is not type endpoint", desc_id);
        return NULL;
    }

    return(zwepd_p)desc_cont->desc;
}


/**
hl_node_desc_get - get node descriptor from descriptor container
@param[in]	head	    The head of the descriptor container linked-list
@param[in]	desc_id		Unique descriptor id
@return     Node descriptor if found; else return NULL
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
zwnoded_p    hl_node_desc_get(desc_cont_t *head, uint32_t desc_id)
{
    desc_cont_t     *last_node_cont;

    //Start searching from the first node
    last_node_cont = head;

    while (last_node_cont)
    {
        if (last_node_cont->id == desc_id)
        {
            if (last_node_cont->type == DESC_TYPE_NODE)
            {
                return(zwnoded_p)last_node_cont->desc;
            }
            //plt_msg_ts_show("hl_node_desc_get desc id:%u is not type node", desc_id);
            return NULL;
        }

        //Get the next node
        last_node_cont = last_node_cont->next;
    }
    //plt_msg_ts_show("hl_node_desc_get invalid desc id:%u", desc_id);
    return NULL;
}


/**
hl_desc_cont_updt - update a node in the descriptor container linked-list
@param[in]	head	The head of the descriptor container linked-list
@param[in]	noded	Node descriptor
@return Zero on success; else return ZW_ERR_XXX
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
int    hl_desc_cont_updt(desc_cont_t **head, zwnoded_p noded)
{
    uint32_t        desc_id;
    zwnoded_p   node;

    //Check whether the node already exists
    desc_id = hl_desc_id_get(*head, noded);

    if (!desc_id)
    {   //The node container doesn't exist, add it.
        return hl_desc_cont_add(head, noded);
    }

    //Update the node
    node = hl_node_desc_get(*head, desc_id);
    if (!node)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }

    *node = *noded;

    //TODO: update the endpoints, classes and interfaces etc.
    return 0;
}


/**
hl_info_get - display information such as home id, node id, basis API library version, etc about the controller
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_info_get(hl_appl_ctx_t   *hl_appl)
{
    uint8_t ver;
    uint8_t subver;

    zwnet_version(&ver, &subver);

    plt_msg_show(hl_plt_ctx_get(hl_appl), "Home controller API version:%u.%02u", (unsigned)ver, (unsigned)subver);

    zwnet_ctlr_info_get(hl_appl->zwnet, 0);
}


/**
hl_nodes_info_dump - dump the nodes information
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_nodes_info_dump(hl_appl_ctx_t   *hl_appl)
{
    zwnet_node_info_dump(hl_appl->zwnet);
}


/**
hl_desc_init - Initialize the descriptor linked-list
@param[in]	head	The head of the descriptor container linked-list
@param[in]	nw		Network
@return Zero on success; else return ZW_ERR_XXX
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
int    hl_desc_init(desc_cont_t **head, zwnet_p nw)
{
    int         result;
    zwnoded_t   noded;
    zwepd_t     ep_desc;
    zwifd_t     ifd;
    zwnoded_p   node;
    zwepd_p     ep;
    zwifd_p     intf;
    desc_cont_t *last_node_cont;
    desc_cont_t *last_ep_cont;
    desc_cont_t *last_intf_cont;

    *head = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwnoded_t) - 1);
    if (*head == NULL)
    {
        return ZW_ERR_MEMORY;
    }
    (*head)->type = DESC_TYPE_NODE;
    (*head)->id = hl_desc_id_gen(nw);
    node = (zwnoded_p)(*head)->desc;
    last_node_cont = *head;

    result = zwnet_get_node(nw, node);
    if (result != 0)
    {
        //plt_msg_show("hl_desc_init get controller node with error:%d", result);
        return result;
    }

    while (node)
    {
        last_ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
        if (!last_ep_cont)
        {
            return ZW_ERR_MEMORY;
        }
        last_ep_cont->type = DESC_TYPE_EP;
        last_ep_cont->id = hl_desc_id_gen(nw);
        ep = (zwepd_p)last_ep_cont->desc;
        zwnode_get_ep(node, ep);
        last_node_cont->down = last_ep_cont;

        while (ep)
        {
            if (zwep_get_if(ep, &ifd) < 0)
            {
                break;
            }

            //Add interfaces
            last_intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
            if (!last_intf_cont)
            {
                return ZW_ERR_MEMORY;
            }

            last_intf_cont->type = DESC_TYPE_INTF;
            last_intf_cont->id = hl_desc_id_gen(nw);
            intf = (zwifd_p)last_intf_cont->desc;
            *intf = ifd;
            last_ep_cont->down = last_intf_cont;

            while (intf)
            {
                //Get the next interface
                result = zwif_get_next(intf, &ifd);
                if (result == 0)
                {
                    desc_cont_t     *intf_cont;
                    zwifd_p         ifdp;

                    intf = &ifd;
                    intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
                    if (!intf_cont)
                    {
                        return ZW_ERR_MEMORY;
                    }
                    intf_cont->type = DESC_TYPE_INTF;
                    intf_cont->id = hl_desc_id_gen(nw);
                    ifdp = (zwifd_p)intf_cont->desc;
                    *ifdp = ifd;
                    last_intf_cont->next = intf_cont;
                    last_intf_cont = intf_cont;

                }
                else
                {
                    intf = NULL;
                }
            }

            //Get the next endpoint
            result = zwep_get_next(ep, &ep_desc);
            if (result == 0)
            {
                desc_cont_t     *ep_cont;
                zwepd_p         epp;

                ep = &ep_desc;
                ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
                if (!ep_cont)
                {
                    return ZW_ERR_MEMORY;
                }
                ep_cont->type = DESC_TYPE_EP;
                ep_cont->id = hl_desc_id_gen(nw);
                epp = (zwepd_p)ep_cont->desc;
                *epp = ep_desc;
                last_ep_cont->next = ep_cont;
                last_ep_cont = ep_cont;

            }
            else
            {
                ep = NULL;
            }
        }

        //Get the next node
        result = zwnode_get_next(node, &noded);
        if (result == 0)
        {
            desc_cont_t     *node_cont;
            zwnoded_p       nodedp;

            node = &noded;

            node_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwnoded_t) - 1);
            if (!node_cont)
            {
                return ZW_ERR_MEMORY;
            }
            node_cont->type = DESC_TYPE_NODE;
            node_cont->id = hl_desc_id_gen(nw);
            nodedp = (zwnoded_p)node_cont->desc;
            *nodedp = noded;
            last_node_cont->next = node_cont;
            last_node_cont = node_cont;

        }
        else
        {
            node = NULL;
        }
    }
    return 0;
}


/**
hl_grp_info_show - Show detailed group information
@param[in]	intf   interface
@return
*/
void    hl_grp_info_show(zwifd_p intf)
{
    int                 j;
    int                 i;
    int                 result;
    if_grp_info_dat_t   *grp_info;
    void                *plt_ctx;
    zw_grp_info_p       grp_info_ent;

    result = zwif_group_info_get(intf, &grp_info);

    plt_ctx = hl_if_plt_ctx_get(intf);

    if (result == 0)
    {
        plt_msg_show(plt_ctx, "                        Group info type:%s", (grp_info->dynamic)? "dynamic" : "static");
        plt_msg_show(plt_ctx, "                        Maximum supported groups:%u", grp_info->group_cnt);
        plt_msg_show(plt_ctx, "                        Valid groups:%u", grp_info->valid_grp_cnt);
        for (i=0; i<grp_info->valid_grp_cnt; i++)
        {
            grp_info_ent = grp_info->grp_info[i];
            if (grp_info_ent)
            {
                plt_msg_show(plt_ctx, "                        --------------------------------------------");
                plt_msg_show(plt_ctx, "                        Group id:%u, profile:%04xh, event code:%04xh,",
                             grp_info_ent->grp_num, grp_info_ent->profile, grp_info_ent->evt_code);
                plt_msg_show(plt_ctx, "                        name:%s, command list:",
                             grp_info_ent->name);
                for (j=0; j<grp_info_ent->cmd_ent_cnt; j++)
                {
                        plt_msg_show(plt_ctx, "                        command class:%04xh(%s), command:%02xh",
                                     grp_info_ent->cmd_lst[j].cls,
                                     hl_class_str_get(grp_info_ent->cmd_lst[j].cls, 1),
                                     grp_info_ent->cmd_lst[j].cmd);
                }
            }
        }
        //Free group info
        zwif_group_info_free(grp_info);
    }
}


/**
hl_sup_sensor_show - Show supported sensor types and units
@param[in]	intf	  Multilevel sensor interface
@return
*/
void    hl_sup_sensor_show(zwifd_p intf)
{
    int                 i;
    int                 j;
    uint8_t             type;
    uint8_t             sensor_cnt;
    if_sensor_data_t    *sup_sensor;
    const char          **unit_str;

    if (zwif_sensor_sup_cache_get(intf, &sup_sensor, &sensor_cnt) != 0)
    {
        return;
    }

    for (i=0; i<sensor_cnt; i++)
    {
        type = sup_sensor[i].sensor_type;

        if (type > ZW_SENSOR_TYPE_TGT_TEMP)
        {
            type = 0;
        }

        plt_msg_show(hl_if_plt_ctx_get(intf), "                        Supported sensor type:%s, sensor units:", sensor_type_str[type]);

        for (j=0; j<4; j++)
        {
            if (sup_sensor[i].sensor_unit & (0x01 << j))
            {
                unit_str = sensor_unit_str + (type * 4) + j;

                if (*unit_str == NULL)
                {
                    *unit_str = "undefined";
                }

                plt_msg_show(hl_if_plt_ctx_get(intf), "                                                         %s", *unit_str);
            }
        }
    }
}


/**
hl_ext_ver_show - Show extended version information
@param[in]	hl_appl   The high-level api context
@param[in]	node	  Node
@return
*/
static void hl_ext_ver_show(hl_appl_ctx_t *hl_appl, zwnoded_p node)
{
    ext_ver_t   *ext_ver;
    int         i;

    ext_ver = zwnode_get_ext_ver(node);
    if (ext_ver)
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Hardware version:%u", (unsigned)(ext_ver->hw_ver));
        for (i=0; i<ext_ver->fw_cnt; i++)
        {
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Firmware %d version:%u.%02u", i+1, (unsigned)(ext_ver->fw_ver[i] >> 8),
                         (unsigned)(ext_ver->fw_ver[i] & 0xFF));
        }
        free(ext_ver);
    }
}


/**
hl_zwaveplus_show - Show Z-Wave+ information
@param[in]	hl_appl   The high-level api context
@param[in]	info	  Z-wave+ information
@return
*/
static void hl_zwaveplus_show(hl_appl_ctx_t *hl_appl, zwplus_info_t *info)
{
    int         idx;
    const char *zwplus_node_type_str[] =
    {
        "Z-Wave+ node",
        "Z-Wave+ for IP router",
        "Z-Wave+ for IP gateway",
        "Z-Wave+ for IP - client IP node",
        "Z-Wave+ for IP - client Z-Wave node",
        "unknown"
    };

    const char *zwplus_role_type_str[] =
    {
        "Central Static Controller",
        "Sub Static Controller",
        "Portable Controller",
        "Portable Reporting Controller",
        "Portable Slave",
        "Always On Slave",
        "Sleeping Reporting Slave",
        "Reachable_Sleeping_Slave",
        "unknown"
    };

    plt_msg_show(hl_plt_ctx_get(hl_appl), "ZWave+ version:%u", (unsigned)(info->zwplus_ver));

    idx = (info->node_type <= 4)? info->node_type : 5;
    plt_msg_show(hl_plt_ctx_get(hl_appl), "ZWave+ node type:%s", zwplus_node_type_str[idx]);

    idx = (info->role_type <= 7)? info->role_type : 8;
    plt_msg_show(hl_plt_ctx_get(hl_appl), "ZWave+ role type:%s", zwplus_role_type_str[idx]);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "ZWave+ installer icon:%04Xh", (unsigned)(info->instr_icon));
    plt_msg_show(hl_plt_ctx_get(hl_appl), "ZWave+ user icon:%04Xh", (unsigned)(info->usr_icon));

}


/**
hl_dev_id_show - Show device id
@param[in]	hl_appl   The high-level api context
@param[in]	node	  Node
@return
*/
static void hl_dev_id_show(hl_appl_ctx_t *hl_appl, dev_id_t *dev_id)
{
    const char *dev_id_type_str[] =
    {
        "Device id oem",
        "Device serial number",
        "Device id unknown type"
    };
    uint8_t   id_type;

    id_type = (dev_id->type > DEV_ID_TYPE_SN)? 2 : dev_id->type;

    if (dev_id->format == DEV_ID_FMT_UTF)
    {   //UTF-8
        plt_msg_show(hl_plt_ctx_get(hl_appl), "%s:%s", dev_id_type_str[id_type], dev_id->dev_id);
    }
    else if (dev_id->format == DEV_ID_FMT_BIN)
    {   //Binary
        char hex_string[(32*3)+1];

        hl_bin2str(dev_id->dev_id, dev_id->len, hex_string, (32*3)+1);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "%s:h'%s", dev_id_type_str[id_type], hex_string);
    }
}


/**
hl_node_desc_dump - dump the node descriptor info
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_node_desc_dump(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnetd_p    net_desc;
    zwnoded_p   node;
    zwepd_p     ep;
    zwifd_p     intf;
    desc_cont_t *last_node_cont;
    desc_cont_t *last_ep_cont;
    desc_cont_t *last_intf_cont;

    plt_mtx_lck(hl_appl->desc_cont_mtx);

    //Check whether the descriptor container linked list is initialized
    if (!hl_appl->desc_cont_hd)
    {
        result = hl_desc_init(&hl_appl->desc_cont_hd, hl_appl->zwnet);
        if (result != 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_node_desc_dump with error:%d", result);
        }

    }

    //Get the first node (local controller) and home id
    last_node_cont = hl_appl->desc_cont_hd;

    net_desc = zwnet_get_desc(hl_appl->zwnet);

    while (last_node_cont)
    {
        if (last_node_cont->type != DESC_TYPE_NODE)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "node: wrong desc type:%u", last_node_cont->type);
        }

        node = (zwnoded_p)last_node_cont->desc;

        plt_msg_show(hl_plt_ctx_get(hl_appl), "__________________________________________________________________________");
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Node id:%u[%u], Home id:%08X", (unsigned)node->nodeid,
                     last_node_cont->id, (unsigned)net_desc->id);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Node status:%s", (node->alive)?  "alive" : "down/sleeping");
        if (node->sleep_cap)
        {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Node is capable to sleep with wakeup interval:%us", node->wkup_intv);
        }
        if (node->sensor)
        {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Node is FLIRS");
        }
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Node security inclusion status:%s, %d", (node->sec_incl_failed)?  "failed" : "unknown",node->sec_incl_failed);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Vendor id:%04X", node->vid);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Product type id:%04X", node->type);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Product id:%04X", node->pid);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Category:%s", (node->category <= DEV_WALL_CTLR)?
                     dev_category_str[node->category] : "unknown");
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Z-wave library type:%u", node->lib_type);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Z-wave protocol version:%u.%02u", (unsigned)(node->proto_ver >> 8),
                     (unsigned)(node->proto_ver & 0xFF));
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Application version:%u.%02u", (unsigned)(node->app_ver >> 8),
                     (unsigned)(node->app_ver & 0xFF));
        hl_ext_ver_show(hl_appl, node);

        if (node->dev_id.len > 0)
        {
            hl_dev_id_show(hl_appl, &node->dev_id);
        }

        //Get endpoint
        last_ep_cont = last_node_cont->down;

        while (last_ep_cont)
        {
            if (last_ep_cont->type != DESC_TYPE_EP)
            {
                plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "ep: wrong desc type:%u", last_ep_cont->type);
            }

            ep = (zwepd_p)last_ep_cont->desc;

            plt_msg_show(hl_plt_ctx_get(hl_appl), "Endpoint id:%u[%u]", ep->epid, last_ep_cont->id);
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Device class: generic:%02Xh, specific:%02Xh",
                         ep->generic, ep->specific);
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Endpoint name:%s", ep->name);
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Endpoint location:%s", ep->loc);
            if (ep->zwplus_info.zwplus_ver)
            {
                hl_zwaveplus_show(hl_appl, &ep->zwplus_info);
            }

            //Get interface
            last_intf_cont = last_ep_cont->down;

            while (last_intf_cont)
            {
                if (last_intf_cont->type != DESC_TYPE_INTF)
                {
                    plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "interface: wrong desc type:%u", last_intf_cont->type);
                }

                intf = (zwifd_p)last_intf_cont->desc;

                plt_msg_show(hl_plt_ctx_get(hl_appl), "              Interface: %02Xv%u:%s [%u]%c%c",
                             (unsigned)intf->cls, intf->ver, hl_class_str_get(intf->cls, intf->ver),
                             last_intf_cont->id, (intf->propty & IF_PROPTY_SECURE)? '*' : ' ',
                             (intf->propty & IF_PROPTY_UNSECURE)? '^' : ' ');

                if (intf->cls == COMMAND_CLASS_SENSOR_MULTILEVEL)
                {
                    hl_sup_sensor_show(intf);
                }
                else if ((intf->cls == COMMAND_CLASS_ASSOCIATION_GRP_INFO))
                {
                    hl_grp_info_show(intf);
                }

                //Get the next interface
                last_intf_cont = last_intf_cont->next;

            }

            //Get the next endpoint
            last_ep_cont = last_ep_cont->next;
        }

        //Get the next node
        last_node_cont = last_node_cont->next;
    }
    plt_msg_show(hl_plt_ctx_get(hl_appl), "__________________________________________________________________________");
    plt_mtx_ulck(hl_appl->desc_cont_mtx);
}


/**
hl_req_nw_updt - Request network topology updates from the SUC/SIS node
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_req_nw_updt(hl_appl_ctx_t   *hl_appl)
{
    return zwnet_update(hl_appl->zwnet);
}


/**
hl_suc_set - Set a controller node as SUC/SIS.
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
//int32_t hl_suc_set(hl_appl_ctx_t   *hl_appl)
//{
//    int32_t     result;
//    zwnoded_p noded;
//
//    //Get the node descriptor
//    plt_mtx_lck(hl_appl->desc_cont_mtx);
//    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->suc_node_id);
//    if (!noded)
//    {
//        plt_mtx_ulck(hl_appl->desc_cont_mtx);
//        return ZW_ERR_NODE_NOT_FOUND;
//    }
//
//    result = zwnet_assign(noded, ZWNET_ROLE_SIS);
//
//    plt_mtx_ulck(hl_appl->desc_cont_mtx);
//
//    if (result != 0)
//    {
//        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_suc_set with error:%d", result);
//    }
//    return result;
//}


#define APPLICATION_NODEINFO_NOT_LISTENING            0x00
#define APPLICATION_NODEINFO_LISTENING                0x01
#define APPLICATION_NODEINFO_OPTIONAL_FUNCTIONALITY   0x02
#define APPLICATION_FREQ_LISTENING_MODE_1000ms        0x10


/**
hl_failed_id_rm - Remove failed node id
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_failed_id_rm(hl_appl_ctx_t   *hl_appl)
{
    int32_t     result;
    zwnoded_p noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->failed_node_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnet_fail(noded, 0);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_failed_id_rm with error:%d", result);
    }
    return result;
}


/**
hl_failed_id_rp - Replace failed node id with a new one
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_failed_id_rp(hl_appl_ctx_t   *hl_appl)
{
    int32_t     result;
    zwnoded_p noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->failed_node_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnet_fail(noded, 1);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_failed_id_rm with error:%d", result);
    }
    return result;
}



/**
hl_nw_node_add - Add node to network
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_nw_node_add(hl_appl_ctx_t   *hl_appl)
{
    int result;
    result = zwnet_add(hl_appl->zwnet, 1);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_node_add with error:%d", result);
    }
    return result;
}


/**
hl_nw_node_rm - Remove node from network
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_nw_node_rm(hl_appl_ctx_t   *hl_appl)
{
    int result;
    result = zwnet_add(hl_appl->zwnet, 0);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_node_rm with error:%d", result);
    }
    return result;
}


/**
hl_ctlr_chg - Add a controller to the Z-Wave network and transfer the role as
              primary controller to it. The controller invoking this function will become secondary.
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_ctlr_chg(hl_appl_ctx_t   *hl_appl)
{
    int result;

    result = zwnet_migrate(hl_appl->zwnet);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ctlr_chg with error:%d", result);
    }
    return result;
}


/**
hl_lrn_mod_set - Start learn mode
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_lrn_mod_set(hl_appl_ctx_t   *hl_appl)
{
    int result;

    result = zwnet_initiate(hl_appl->zwnet);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_lrn_mod_set with error:%d", result);
    }
    return result;
}


/**
hl_deflt_set - Restore factory default
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_deflt_set(hl_appl_ctx_t   *hl_appl)
{
    int result;

    result = zwnet_reset(hl_appl->zwnet);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_deflt_set with error:%d", result);
    }
    return result;
}


/**
hl_nif_send - Add a controller to the Z-Wave network and transfer the role as
              primary controller to it. The controller invoking this function will become secondary.
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_nif_send(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!noded && !hl_appl->broadcast)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnet_send_nif(hl_appl->zwnet, noded, hl_appl->broadcast);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nif_send with error:%d", result);
    }
    return result;
}


/**
hl_node_updt - Update a node information
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_node_updt(hl_appl_ctx_t   *hl_appl)
{
    int32_t     result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    hl_appl->node_updt_desc = *noded;
    result = zwnode_update(noded);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_node_updt with error:%d", result);
    }
    return result;
}

static void dummy_post_msg(void *msg)
{

    hl_nw_notify_hdlr((nw_notify_msg_t *)msg);
    free(msg);
}

/**
hl_nw_notify_cb - Callback function to notify the status of current operation
@param[in]	user	The high-level api context
param[in]	op		Network operation ZWNET_OP_XXX
@param[in]	ret		The status of current operation
@return
*/
static void hl_nw_notify_cb(void *user, uint8_t op, uint16_t sts)
{
    nw_notify_msg_t  *nw_notify;

    nw_notify = (nw_notify_msg_t *)malloc(sizeof(nw_notify_msg_t));

    if (nw_notify)
    {
        nw_notify->hl_appl = (hl_appl_ctx_t *)user;
        nw_notify->op = op;
        nw_notify->sts = sts;

        //Post message to main windows
        //PostMessage(ghWnd, MSG_ZWAPI_NOTIFY, 0, (LPARAM )nw_notify);
        dummy_post_msg(nw_notify);
    }
}


/**
hl_nw_node_cb - Callback function to notify node is added, deleted, or updated
@param[in]	user	    The high-level api context
@param[in]	noded	Node
@param[in]	mode	    The node status
@return
*/
static void hl_nw_node_cb(void *user, zwnoded_p noded, int mode)
{
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t *)user;

    switch (mode)
    {
        case ZWNET_NODE_ADDED:
            {
                plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_node_cb node:%u added", (unsigned)noded->nodeid);
                //Store the last added node descriptor
                hl_appl->node_add_desc = *noded;

                //Add node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                hl_desc_cont_add(&hl_appl->desc_cont_hd, noded);
                plt_mtx_ulck(hl_appl->desc_cont_mtx);
            }
            break;

        case ZWNET_NODE_REMOVED:
            {
                plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_node_cb node:%u removed", (unsigned)noded->nodeid);
                //Remove the node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                hl_desc_cont_del(&hl_appl->desc_cont_hd, hl_desc_id_get(hl_appl->desc_cont_hd, noded));
                plt_mtx_ulck(hl_appl->desc_cont_mtx);

            }
            break;

        case ZWNET_NODE_UPDATED:
            {
                plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_node_cb node:%u updated", (unsigned)noded->nodeid);
                //Store the last replaced node descriptor
                hl_appl->node_rp_desc = *noded;

                //Update the node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                //hl_desc_cont_updt(&hl_appl->desc_cont_hd, noded);
                hl_desc_cont_add(&hl_appl->desc_cont_hd, noded);
                plt_mtx_ulck(hl_appl->desc_cont_mtx);

            }
            break;

    }
}


/**
hl_nw_tx_cb - Callback function to notify application transmit data status
@param[in]	user	    The high-level api context
param[in]	tx_sts	    Transmit status ZWNET_TX_xx
@return
*/
static void hl_nw_tx_cb(void *user, uint8_t tx_sts)
{
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t *)user;

    static char    *tx_cmplt_sts[] = {"ok",
        "no ACK before timeout",
        "failed",
        "routing not idle",
        "no route",
        "no callback frame before timeout"
    };

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        //plt_msg_show(hl_plt_ctx_get(hl_appl), "Higher level appl send data completed successfully");
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Higher level appl send data completed with error:%s",
                     (tx_sts < sizeof(tx_cmplt_sts)/sizeof(char *))?
                     tx_cmplt_sts[tx_sts]  : "unknown");
    }
}


#ifdef  ZW_STRESS_TEST
/**
hl_stress_tst_cb - stress test receiving sequence number callback
@param[in]	nw	        Network
@param[in]	seq_num	    Sequence number received.
*/
void hl_stress_tst_cb(void *user, uint32_t seq_num)
{
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t *)user;

    if (seq_num == hl_appl->exp_seq_num)
    {
        hl_appl->exp_seq_num++;
        hl_appl->test_stat.rx_seq_num_frm++;
    }
    else
    {
        hl_appl->exp_seq_num = seq_num + 1;
        hl_appl->test_stat.rx_seq_num_err++;
    }
}
#endif


/**
hl_msg_show - show message to the user
@param[in] msg   The output message to printf.
@return
*/
static void hl_msg_show(void *msg)
{
    fputs((const char *)msg, stdout);
}



/**
hl_file_ver_get - scan file for version number
@param[in]  file        file descriptor
@param[out] ver         version
@return  Non-zero on success; zero on failure.
*/
static int hl_file_ver_get(FILE *file, uint16_t *ver)
{
    static const char   ver_delimiters[] = " =,\r\n";
    char                line[180];
    char				*prm_name;
    char				*prm_val;

    while (fgets(line, 180, file))
    {
        if (*line == '#')
        {   //Skip comment line
            continue;
        }

        //Check if '=' exists
        if (strchr(line, '='))
        {
#ifdef USE_SAFE_VERSION
            char *next_token;
            //Get the parameter name and value
            prm_name = strtok_s(line, ver_delimiters, &next_token);
#else
            //Get the parameter name and value
            prm_name = strtok(line, ver_delimiters);
#endif

            if (prm_name)
            {
#ifdef USE_SAFE_VERSION
                prm_val = strtok_s(NULL, ver_delimiters, &next_token);
#else
                prm_val = strtok(NULL, ver_delimiters);
#endif

                if (!prm_val)
                {
                    continue;
                }

                //Compare the parameter name
                if (strcmp(prm_name, "ver") == 0)
                {
                    unsigned version;
#ifdef USE_SAFE_VERSION
                    if (sscanf_s(prm_val, "%u", &version) == 1)
                    {
                        *ver = (uint32_t)version;
                        return 1;
                    }
#else
                    if (sscanf(prm_val, "%u", &version) == 1)
                    {
                        *ver = (uint32_t)version;
                        return 1;
                    }
#endif
                }
            }
        }
    }

    return 0;
}


/**
hl_field_get - get a field separated by comma
@param[in]  rec_str     Record string, will be modified
@return  Field on success, NULL if there is no more field.
*/
char *hl_field_get(char **rec_str)
{
    char *s;
    char *field;
    char c;

    s = *rec_str;

    if (s == NULL)
        return(NULL);

    field = s;

    while (1)
    {
        c = *s++;
        if ((c == ',') || (c == '\0'))
        {
            if (c == '\0')
            {
                s = NULL;
            }
            else
                s[-1] = '\0';

            *rec_str = s;
            return(field);
        }
    }
}


/**
hl_dev_setting_get - get device specific settings
@param[in]  file        File descriptor
@param[out] dev_cfg     Device specific settings.
@return  Zero on success, non-zero on failure.
@post Caller is required to free the allocate buffer in dev_cfg->dev_cfg_buf
      if return value equals to zero and dev_cfg->dev_cfg_cnt > 0.
*/
static int hl_dev_setting_get(FILE *file, dev_spec_cfg_t *dev_cfg)
{
    char        line[256];
    long int    file_pos;
    dev_rec_t   *rec_buf;
    char        *field;
    char        *rec_str;
    unsigned    vid;
    unsigned    ptype;
    unsigned    pid;
    unsigned    category;
    unsigned    wkup_intv;
    unsigned    grp_id;
    unsigned    cfg_prm_num;
    unsigned    cfg_prm_sz;
    int         cfg_prm_val;
    uint16_t    rec_cnt = 0;
    uint32_t    fld_bitmsk;

    //Initialize parameters
    dev_cfg->dev_cfg_buf = NULL;
    dev_cfg->dev_cfg_cnt = 0;
    dev_cfg->dev_cfg_ver = 0;

    //Get file format version
    //Note: version must come before device setting records
    if (!hl_file_ver_get(file, &dev_cfg->dev_cfg_ver))
    {
        return ZW_ERR_VALUE;
    }

    //Check supported version
    if (dev_cfg->dev_cfg_ver != 1)
    {
        return ZW_ERR_VERSION;
    }

    //Calculate number of records
    file_pos = ftell(file);
    if (file_pos == -1)
    {
        return ZW_ERR_FILE;
    }

    while (fgets(line, 256, file))
    {
        if (*line == '#')
        {   //Skip comment line
            continue;
        }

        //Check if ',' exists
        if (strchr(line, ','))
        {
            rec_cnt++;
        }
    }

    //Allocate buffer for the records
    rec_buf = (dev_rec_t *)calloc(rec_cnt, sizeof(dev_rec_t));

    dev_cfg->dev_cfg_buf = rec_buf;

    if (!rec_buf)
    {
        return ZW_ERR_MEMORY;
    }

    //Get records
    if (fseek(file, file_pos, SEEK_SET) != 0)
    {
        return ZW_ERR_FILE;
    }

    rec_cnt = 0;

    while (fgets(line, 256, file))
    {
        if (*line == '#')
        {   //Skip comment line
            continue;
        }

        rec_str = line;
        fld_bitmsk = 0;

        //Get Manufacturer id
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%x", &vid) == 1))
#else
        if (field && (sscanf(field, "%x", &vid) == 1))
#endif
        {
            rec_buf->vid = vid;
            fld_bitmsk = DEV_REC_MSK_VID;
        }

        //Get Product type
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%x", &ptype) == 1))
#else
        if (field && (sscanf(field, "%x", &ptype) == 1))
#endif
        {
            rec_buf->ptype = ptype;
            fld_bitmsk |= DEV_REC_MSK_PTYPE;
        }

        //Get Product id
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%x", &pid) == 1))
#else
        if (field && (sscanf(field, "%x", &pid) == 1))
#endif
        {
            rec_buf->pid = pid;
            fld_bitmsk |= DEV_REC_MSK_PID;
        }

        //Get device category
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%u", &category) == 1))
#else
        if (field && (sscanf(field, "%u", &category) == 1))
#endif
        {
            rec_buf->category = category;
            fld_bitmsk |= DEV_REC_MSK_CAT;
        }

        //Get wakeup interval
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%u", &wkup_intv) == 1))
#else
        if (field && (sscanf(field, "%u", &wkup_intv) == 1))
#endif
        {
            rec_buf->wkup_intv = wkup_intv;
            fld_bitmsk |= DEV_REC_MSK_WKUP;
        }

        //Get group id
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%u", &grp_id) == 1))
#else
        if (field && (sscanf(field, "%u", &grp_id) == 1))
#endif
        {
            rec_buf->grp_id = grp_id;
            fld_bitmsk |= DEV_REC_MSK_GID;
        }

        //Get configuration parameter number
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%u", &cfg_prm_num) == 1))
#else
        if (field && (sscanf(field, "%u", &cfg_prm_num) == 1))
#endif
        {
            rec_buf->cfg_prm_num = cfg_prm_num;
            fld_bitmsk |= DEV_REC_MSK_CFG_NUM;
        }

        //Get configuration parameter size (in bytes)
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%u", &cfg_prm_sz) == 1))
#else
        if (field && (sscanf(field, "%u", &cfg_prm_sz) == 1))
#endif
        {
            rec_buf->cfg_prm_sz = cfg_prm_sz;
            fld_bitmsk |= DEV_REC_MSK_CFG_SZ;
        }

        //Get configuration parameter value
        field = hl_field_get(&rec_str);

#ifdef USE_SAFE_VERSION
        if (field && (sscanf_s(field, "%d", &cfg_prm_val) == 1))
#else
        if (field && (sscanf(field, "%d", &cfg_prm_val) == 1))
#endif
        {
            rec_buf->cfg_prm_val = cfg_prm_val;
            fld_bitmsk |= DEV_REC_MSK_CFG_VAL;
        }

        //Save field bitmask
        if (fld_bitmsk)
        {
            rec_buf->fld_bitmsk = fld_bitmsk;
            //Adjustment
            rec_buf++;
            rec_cnt++;
        }
    }

    dev_cfg->dev_cfg_cnt = rec_cnt;

    if (rec_cnt == 0)
    {
        free(dev_cfg->dev_cfg_buf);
        dev_cfg->dev_cfg_buf = NULL;
    }

    return 0;
}


/**
hl_config_file_get - get device specific configuration file name and default parameter
@param[in]  file            Configuration file descriptor
@param[out] dev_file        Device specific configuration file path
@return     Number of parameters processed for the entry
*/
static int hl_config_file_get(FILE *file, char *dev_file)
{
    static const char delimiters[] = " =\r\n";
    char    line[384];
    char    *prm_name;
    char    *prm_val;
    int     param_cnt = 0;

    //Initialize parameters
    *dev_file = '\0';

    while ((param_cnt < 1) && (fgets(line, 384, file)))
    {
        if (*line == '#')
        {   //Skip comment line
            continue;
        }

        //Check if '=' exists
        if (strchr(line, '='))
        {
#ifdef USE_SAFE_VERSION
            char *next_token;
            //Get the parameter name and value
            prm_name = strtok_s(line, delimiters, &next_token);
#else
            //Get the parameter name and value
            prm_name = strtok(line, delimiters);
#endif

            if (prm_name)
            {
#ifdef USE_SAFE_VERSION
                prm_val = strtok_s(NULL, delimiters, &next_token);
#else
                prm_val = strtok(NULL, delimiters);
#endif

                if (!prm_val)
                {
                    continue;
                }

                //Compare the parameter name
                if (strcmp(prm_name, "DeviceCfgFile") == 0)
                {
#ifdef USE_SAFE_VERSION
                    strcpy_s(dev_file, 384, prm_val);
#else
                    strcpy(dev_file, prm_val);
#endif
                    param_cnt++;
                }
            }
        }
    }

    return param_cnt;
}


/**
hl_config_get - Get configuration parameters
@param[out]	    dev_cfg  Device specific settings
@return  Zero on success, non-zero on failure.
*/
int hl_config_get(dev_spec_cfg_t *dev_cfg)
{
    FILE            *file;
    int             ret;
    char            dev_file[384];

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, APPL_CONFIG_FILE, "rt") != 0)
#else
    file = fopen(APPL_CONFIG_FILE, "rt");
    if (!file)
#endif
    {
        printf("hl_config_get can't open file:%s\n", APPL_CONFIG_FILE);
        return ZW_ERR_FILE_OPEN;
    }

    //Get device specific configuration file paths
    ret = hl_config_file_get(file, dev_file);

    fclose(file);

    if (ret != 1)
    {
        printf("Failed to get file path and configuration parameter!\n");
        return ZW_ERR_VALUE;
    }

    //Get device specific settings
#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, dev_file, "rt") != 0)
#else
    file = fopen(dev_file, "rt");
    if (!file)
#endif
    {
        printf("Failed to open device settings file.\n");
        return ZW_ERR_FILE_OPEN;
    }

    ret = hl_dev_setting_get(file, dev_cfg);
    fclose(file);

    if (ret != 0)
    {
        printf("hl_dev_setting_get with error:%d\n", ret);
        return ret;
    }

    printf("Number of device settings records:%u\n", dev_cfg->dev_cfg_cnt);

    return 0;
}


#ifdef  USER_APPL_DEVICE_CFG
/**
hl_dev_rec_find - Find device specific configuration record
@param[in]	ctx		    Context
@param[in]	vid		    Vendor ID
@param[in]	ptype		Product Type ID
@param[in]	pid		    Product ID
@param[out]	dev_rec     Device record
@return  Non-zero on found; zero on not found
*/
static int hl_dev_rec_find(void *ctx, uint16_t vid, uint16_t ptype, uint16_t pid, dev_rec_t *dev_rec)
{
    hl_appl_ctx_t *hl_appl = (hl_appl_ctx_t *)ctx;
    int i;
    int j;

    for (j=0; j<4; j++)
    {
        if (hl_appl->dev_cfg_bufs[j].dev_cfg_cnt)
        {
            for (i=0; i<hl_appl->dev_cfg_bufs[j].dev_cfg_cnt; i++)
            {
                if ((hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_VID)
                    && (hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].vid != vid))
                {
                    continue;
                }

                if ((hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_PTYPE)
                    && (hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].ptype != ptype))
                {
                    continue;
                }

                if ((hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].fld_bitmsk & DEV_REC_MSK_PID)
                    && (hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i].pid != pid))
                {
                    continue;
                }

                //Found a match
                *dev_rec = hl_appl->dev_cfg_bufs[j].dev_cfg_buf[i];
                return 1;
            }
        }
    }
    //No match found
    return 0;
}


/**
hl_dev_cfg_load - Load and store device specific configurations
@param[in]	dev_spec_cfg	Device specific configurations unsorted
@param[out]	dev_cfg_bufs	Device specific configurations sorted according to device matching priorities
@return		ZW_ERR_xxx
@pre    dev_cfg_bufs must be cleared by the caller
*/
static int hl_dev_cfg_load(dev_spec_cfg_t  *dev_spec_cfg, dev_cfg_buf_t *dev_cfg_bufs)
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
#endif


/**
hl_init - Initialize program and application layer
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_init(hl_appl_ctx_t   *hl_appl)
{
    int                 result;
    zwnet_init_t        zw_init = {0};
    zwnet_init_ret_t    zw_init_ret;
    dev_spec_cfg_t      dev_spec_cfg;
#ifdef  USER_APPL_DEVICE_CFG
    dev_spec_cfg_usr_t  dev_spec_cfg_usr;
    int                 i;
#endif
    char                net_id[10];

    //Get configuration parameters (wake up and sensor settings) from file
    result = hl_config_get(&dev_spec_cfg);
    if (result != 0)
    {
        printf("hl_config_get with error:%d\n", result);
        return result;
    }

#ifdef  USER_APPL_DEVICE_CFG

    //Load and save device specific configurations
    if (dev_spec_cfg.dev_cfg_cnt && dev_spec_cfg.dev_cfg_buf)
    {
        result = hl_dev_cfg_load(&dev_spec_cfg, hl_appl->dev_cfg_bufs);
        if (result < 0)
        {
            printf("hl_dev_cfg_load with error:%d\n", result);
            free(dev_spec_cfg.dev_cfg_buf);
            return result;
        }
    }

    dev_spec_cfg_usr.dev_cfg_ctx = hl_appl;
    dev_spec_cfg_usr.dev_cfg_ver = dev_spec_cfg.dev_cfg_ver;
    dev_spec_cfg_usr.dev_rec_find_fn = hl_dev_rec_find;
#endif

    //Init high-level appl layer
    if (!plt_mtx_init(&hl_appl->desc_cont_mtx))
    {
        result = ZW_ERR_NO_RES;
        goto l_HL_INIT_ERROR1;
    }

    zw_init.instance = 0;
    zw_init.comm_port_name = hl_appl->comm_port_name;
    zw_init.user = hl_appl; //high-level application context
    zw_init.node = hl_nw_node_cb;
    zw_init.notify = hl_nw_notify_cb;
    zw_init.appl_tx = hl_nw_tx_cb;
#ifdef  SUPPORT_SECURITY
    zw_init.sec_enable = 1;
#else
    zw_init.sec_enable = 0;
#endif
    zw_init.print_txt_fn = hl_msg_show;
#ifdef  USER_APPL_DEVICE_CFG
    zw_init.dev_spec_cfg_usr = &dev_spec_cfg_usr;
#else
    zw_init.dev_spec_cfg = &dev_spec_cfg;
#endif

    //Init ZW network
    result = zwnet_init(&zw_init, &zw_init_ret);

    if (result != 0)
    {
        printf("hl_init with error:%d\n", result);

        if (result == ZW_ERR_NO_RESP)
        {
            printf("The controller's NVM might be corrupted, please select Device->Quick reset of controller.\n");
        }

        goto l_HL_INIT_ERROR2;
    }

    hl_appl->zwnet = zw_init_ret.net;

    sprintf(net_id, "%08X", zw_init_ret.net_id);

    plt_msg_show(hl_plt_ctx_get(hl_appl), "network id:%s, controller id:%u, HC API type:%u",
                 net_id, zw_init_ret.ctlr_id, zw_init_ret.hc_api_type);

    result = zwnet_load(hl_appl->zwnet,
                        (hl_appl->load_ni_file)? hl_appl->node_info_file : NULL);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_init load node info with error:%d", result);
        goto l_HL_INIT_ERROR3;
    }

    //Rebuild the descriptor container linked-list
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    hl_desc_cont_rm_all(&hl_appl->desc_cont_hd);
    result = hl_desc_init(&hl_appl->desc_cont_hd, hl_appl->zwnet);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_desc_init with error:%d", result);
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        goto l_HL_INIT_ERROR3;
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    //Update init status
    hl_appl->is_init_done = 1;

#ifdef  ZW_STRESS_TEST
    zwnet_stress_tst_cb_set(hl_appl->zwnet, hl_stress_tst_cb);
#endif

    //Free device specific setting buffers
    free(dev_spec_cfg.dev_cfg_buf);
    return 0;

l_HL_INIT_ERROR3:
    zwnet_exit(hl_appl->zwnet, NULL);
l_HL_INIT_ERROR2:
    plt_mtx_destroy(hl_appl->desc_cont_mtx);
l_HL_INIT_ERROR1:
    free(dev_spec_cfg.dev_cfg_buf);
#ifdef  USER_APPL_DEVICE_CFG
    for (i=0; i<4; i++)
        free(hl_appl->dev_cfg_bufs[i].dev_cfg_buf);
#endif
    return result;
}


/**
hl_close - Close connection to controller
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_close(hl_appl_ctx_t   *hl_appl)
{
#ifdef  USER_APPL_DEVICE_CFG
    int i;
#endif

    zwnet_exit(hl_appl->zwnet,
               (hl_appl->save_ni_file)? hl_appl->node_info_file : NULL);

    //Remove all descriptor container entries
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    hl_desc_cont_rm_all(&hl_appl->desc_cont_hd);
    plt_mtx_ulck(hl_appl->desc_cont_mtx);
    plt_mtx_destroy(hl_appl->desc_cont_mtx);

    //Update init status
    hl_appl->is_init_done = 0;

#ifdef  USER_APPL_DEVICE_CFG
    for (i=0; i<4; i++)
        free(hl_appl->dev_cfg_bufs[i].dev_cfg_buf);
#endif

}


/**
hl_exit - Clean up program and application layer
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_exit(hl_appl_ctx_t   *hl_appl)
{
    if (hl_appl->is_init_done)
    {
        hl_close(hl_appl);
    }
}


/**
hl_save - Save settings into a file
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_save(hl_appl_ctx_t   *hl_appl)
{
    int         result;

    result = zwnet_save(hl_appl->zwnet, hl_appl->save_file);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "save settings with error:%d", result);
        return;

    }
}


void hl_nw_notify_hdlr(nw_notify_msg_t *notify_msg)
{
    hl_appl_ctx_t    *hl_appl = notify_msg->hl_appl;
    int              result;

    //Check to display progress of get detailed node info
    if (notify_msg->sts & OP_GET_NI_TOTAL_NODE_MASK)
    {
        uint16_t    total_nodes;
        uint16_t    cmplt_nodes;

        total_nodes = (notify_msg->sts & OP_GET_NI_TOTAL_NODE_MASK) >> 8;
        cmplt_nodes = notify_msg->sts & OP_GET_NI_NODE_CMPLT_MASK;
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_notify_cb op:%u, get node info %u/%u completed",
                        (unsigned)notify_msg->op, cmplt_nodes, total_nodes);
        return;
    }

    plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_notify_cb op:%u, status:%u", (unsigned)notify_msg->op, notify_msg->sts);

    switch (notify_msg->op)
    {
        case ZWNET_OP_UPDATE://TODO: update all the nodes, instead of rebuilding.
        case ZWNET_OP_INITIALIZE:
        case ZWNET_OP_INITIATE:
        case ZWNET_OP_RESET:

            if (notify_msg->sts == ZW_ERR_NONE)
            {   //Rebuild the descriptor container linked-list
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                hl_desc_cont_rm_all(&hl_appl->desc_cont_hd);
                result = hl_desc_init(&hl_appl->desc_cont_hd, hl_appl->zwnet);
                if (result != 0)
                {
                    plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nw_notify_hdlr with error:%d", result);
                }

                plt_mtx_ulck(hl_appl->desc_cont_mtx);
            }
            break;

        case ZWNET_OP_NODE_UPDATE:
            {
                //Check whether stress test in progress
                if (hl_appl->is_ni_stress_tst_run)
                {
                    hl_node_updt(hl_appl);
                    break;
                }

                //Update the node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                //hl_desc_cont_updt(&hl_appl->desc_cont_hd, &hl_appl->node_updt_desc);
                hl_desc_cont_add(&hl_appl->desc_cont_hd, &hl_appl->node_updt_desc);
                plt_mtx_ulck(hl_appl->desc_cont_mtx);

            }
            break;

        case ZWNET_OP_ADD_NODE:
        case ZWNET_OP_MIGRATE:
        case ZWNET_OP_MIGRATE_SUC:
            if (notify_msg->sts == ZW_ERR_NONE)
            {
                //Update the node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                hl_desc_cont_add(&hl_appl->desc_cont_hd, &hl_appl->node_add_desc);
                plt_mtx_ulck(hl_appl->desc_cont_mtx);

            }
            break;

        case ZWNET_OP_RP_NODE:
            if (notify_msg->sts == ZW_ERR_NONE)
            {
                //Update the node descriptor container
                plt_mtx_lck(hl_appl->desc_cont_mtx);
                hl_desc_cont_add(&hl_appl->desc_cont_hd, &hl_appl->node_rp_desc);
                plt_mtx_ulck(hl_appl->desc_cont_mtx);

            }
            break;
    }

}

#ifdef DEBUG_FLASH_APM

int zwnet_auto_prog(zwnet_p net);
/**
hl_auto_prog
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_auto_prog(hl_appl_ctx_t   *hl_appl)
{
    int result;

    result = zwnet_auto_prog(hl_appl->zwnet);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_auto_prog with error:%d", result);
    }
    return result;
}
#endif


#ifndef OS_MAC_X
/**
hl_fw_updt - Local firmware update
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_fw_updt(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    result = zwnet_fw_updt(hl_appl->zwnet, hl_appl->save_file);
    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_fw_updt with error:%d", result);
    }
    return result;
}
#endif


/**
hl_quick_reset - Restore factory default
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_quick_reset(hl_appl_ctx_t *hl_appl)
{
    int     result;

    result = zwnet_quick_reset(hl_appl->comm_port_name);
    if (result == 0)
    {
        printf("hl_quick_reset done!\n");
    }
    else
    {
        printf("hl_quick_reset with error:%d\n", result);
    }
    return result;
}



