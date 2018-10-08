/**
@file   zw_api_test.c - Z-wave HC API test program.

        To test the Z-wave HC API.

@author David Chow

@version    1.0 8-6-10  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include <windowsx.h>
#include <Commdlg.h>
#include "resource.h"
#include "zw_api_test.h"

/**
@defgroup HCAPITest Z-wave HC API test
High-level application layer to test the Z-wave HC API.
@{
*/

#define  MSG_ZWAPI_NOTIFY   (WM_USER + 10)

#define MAX_LOADSTRING 100


#define  DESC_TYPE_NODE     1
#define  DESC_TYPE_EP       2
#define  DESC_TYPE_INTF     3


///
/// Sub Menu constants
#define SUBMENU_FILE        0
#define SUBMENU_TEST1       1
#define SUBMENU_TEST2       2
#define SUBMENU_TEST3       3
#define SUBMENU_TEST4       4
#define SUBMENU_MANAGE      5
#define SUBMENU_DEVICE      6
#define SUBMENU_HELP        7


typedef struct{
    hl_appl_ctx_t   *hl_appl;   //< high level application context
    uint16_t        sts;        //< status of current operation
    uint8_t         op;         //< network operation ZWNET_OP_XXX

} nw_notify_msg_t;

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND ghWnd;                                     // the main window handle
hl_appl_ctx_t   my_appl;                        // High-level application context

const char *zw_alrm_type[] =
{
    "Unknown alarm",
    "Smoke alarm",
    "Carbon monoxide alarm",
    "Carbon dioxide alarm",
    "Heat alarm",
    "Water alarm",
    "Lock access control alarm",
    "Burglar alarm",
    "Power management alarm",
    "System alarm",
    "Emergency alarm",
    "Alarm clock"
};

extern const char *sensor_type_str[];
extern const char *sensor_unit_str[];
extern const char *dev_category_str[];

static void hl_msg_show(void *msg);


/**
hl_hex_get - get hex value from an edit control
@param[in]	hDlg		The dialog box handle
@param[in]	ecid		The edit control id
@param[out]	hex_val	    The result of the conversion from string to hex
@return     1 if the edit control contains string that can be converted to hex; else return 0
*/
int  hl_hex_get(HWND hDlg, int ecid, unsigned *hex_val)
{
    int             len;

    //Get hex string for level
    len = GetWindowTextLength(GetDlgItem(hDlg, ecid));
    if (len > 0)
    {
        char *buf;

        buf = (char *)calloc(1, len + 1);
        GetDlgItemTextA(hDlg, ecid, buf, len + 1);
#ifdef USE_SAFE_VERSION
        if (sscanf_s(buf, "%x", hex_val) == 1)
#else
        if (sscanf(buf, "%x", hex_val) == 1)
#endif
        {
            free(buf);
            return 1;
        }
        free(buf);
    }
    return 0;
}


/**
hl_hex_get - get integer value from a byte stream
@param[in]	byte_buf    The buffer that holds the bytes
@param[in]	size		The integer size
@param[out]	int_val	    The result of the conversion from bytes to integer
@return     1 on success; else return 0
*/
int  hl_int_get(uint8_t *byte_buf,  uint8_t size, int32_t *int_val)
{

    if (size == 1)
    {
        int8_t  val8;

        val8 = byte_buf[0];
        *int_val = val8;
        return 1;
    }
    else if (size == 2)
    {
        int16_t val16;

        val16 = byte_buf[0];
        val16 = (val16 << 8) | byte_buf[1];
        *int_val = val16;
        return 1;
    }
    else
    {
        int32_t val32;

        val32 = ((int32_t)(byte_buf[0])) << 24
            | ((int32_t)(byte_buf[1])) << 16
            | ((int32_t)(byte_buf[2])) << 8
            | byte_buf[3];

        *int_val = val32;
        return 1;
    }
    return 0;

}


#if 0
/**
hl_float_get - get floating point integer value from an integer
@param[in]	int_value    Integer value
@param[in]	precision	 Number of decimal points
@param[out]	float_val	 The result of the conversion from integer to floating point integer
@return
*/
static void  hl_float_get(int32_t int_value,  uint8_t precision, float *float_val)
{

    int i;
    float   divisor;

    divisor = 1.0;
    for(i=0 ; i < precision; i++)
    {
        divisor *= 10;
    }
    *float_val = int_value / divisor;

}
#endif


/**
hl_float_get - get floating point integer string from an integer with specified precision
@param[in]	int_value    Integer value
@param[in]	precision	 Number of decimal points
@param[in]	buf_size	 Output buffer size
@param[out]	out_buf	     Output buffer that contains the converted string.
@return
*/
void  hl_float_get(int32_t int_value,  uint8_t precision, uint8_t buf_size, char *out_buf)
{

    int i;
    int32_t   divisor;
    int32_t   whole_num;
    int32_t   dec_num;
    char      format_str[20];

    divisor = 1;
    for(i=0 ; i < precision; i++)
    {
        divisor *= 10;
    }

    whole_num = int_value / divisor;

    if (int_value < 0)
    {
        dec_num = (int_value * (-1)) % divisor;
    }
    else
    {
        dec_num = int_value % divisor;
    }

    if (precision > 0)
    {
#ifdef USE_SAFE_VERSION
        sprintf_s(format_str, 20, "%%d.%%.%ud", precision);
        sprintf_s(out_buf, buf_size, format_str, whole_num, dec_num);
#else
        sprintf(format_str, "%%d.%%.%ud", precision);
        sprintf(out_buf, format_str, whole_num, dec_num);
#endif
    }
    else
    {
#ifdef USE_SAFE_VERSION
        sprintf_s(out_buf, buf_size, "%d", int_value);
#else
        sprintf(out_buf, "%d", int_value);
#endif
    }
}


/**
hl_bin2str - convert binary string to hex string
@param[in]  buf      The buffer that stores the binary string
@param[in]  len      The length of the binary string.
@param[out] hex_str  The buffer that stores the output string.
@param[in]  hex_str_len      The length of the output buffer.
@return
*/
void hl_bin2str(void * buf, uint32_t len, char *hex_str, uint32_t hex_str_len)
{
    uint8_t     *bin_byte = (uint8_t *)buf;
    char        tmp[8];

    hex_str[0] = '\0';

    //Convert a line of binary data into hex string
    while (len-- > 0)
    {
#ifdef USE_SAFE_VERSION
        sprintf_s(tmp, 8, "%02X",(unsigned) *bin_byte++);
        strcat_s(hex_str, hex_str_len, tmp);
#else
        sprintf(tmp,"%02X",(unsigned) *bin_byte++);
        strcat(hex_str, tmp);
#endif
    }
}


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
            break;
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

                intf = &ifd;
                intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
                if (!intf_cont)
                {
                    return ZW_ERR_MEMORY;
                }
                intf_cont->type = DESC_TYPE_INTF;
                intf_cont->id = hl_desc_id_gen(noded->net);
                *((zwifd_p)intf_cont->desc) = ifd;
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

            ep = &ep_desc;
            ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
            if (!ep_cont)
            {
                return ZW_ERR_MEMORY;
            }
            ep_cont->type = DESC_TYPE_EP;
            ep_cont->id = hl_desc_id_gen(noded->net);
            *((zwepd_p)ep_cont->desc) = ep_desc;
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
        if (last_node_cont->id == desc_id)
        {
            return last_node_cont;
        }

        //Search endpoint
        last_ep_cont = last_node_cont->down;

        while (last_ep_cont)
        {
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
    desc_cont_t *desc_cont;

    //Get the interface descriptor
    desc_cont = hl_desc_cont_get(head, desc_id);
    if (!desc_cont)
    {
        //plt_msg_ts_show("hl_intf_desc_get invalid desc id:%u", desc_id);
        return NULL;
    }

    if (desc_cont->type != DESC_TYPE_INTF)
    {
        //plt_msg_ts_show("hl_intf_desc_get desc id:%u is not type interface", desc_id);
        return NULL;
    }

    return (zwifd_p)desc_cont->desc;
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

    return (zwepd_p)desc_cont->desc;
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
                return (zwnoded_p)last_node_cont->desc;
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
            //plt_msg_show(hl_plt_ctx_get(hl_appl), "hl_desc_init get controller node with error:%d", result);
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

                    intf = &ifd;
                    intf_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwifd_t) - 1);
                    if (!intf_cont)
                    {
                        return ZW_ERR_MEMORY;
                    }
                    intf_cont->type = DESC_TYPE_INTF;
                    intf_cont->id = hl_desc_id_gen(nw);
                    *((zwifd_p)intf_cont->desc) = ifd;
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

                ep = &ep_desc;
                ep_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwepd_t) - 1);
                if (!ep_cont)
                {
                    return ZW_ERR_MEMORY;
                }
                ep_cont->type = DESC_TYPE_EP;
                ep_cont->id = hl_desc_id_gen(nw);
                *((zwepd_p)ep_cont->desc) = ep_desc;
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

            node = &noded;

            node_cont = (desc_cont_t *)calloc(1, sizeof(desc_cont_t) + sizeof(zwnoded_t) - 1);
            if (!node_cont)
            {
                return ZW_ERR_MEMORY;
            }
            node_cont->type = DESC_TYPE_NODE;
            node_cont->id = hl_desc_id_gen(nw);
            *((zwnoded_p)node_cont->desc) = noded;
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
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_desc_init with error:%d", result);
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
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Node security inclusion status:%s", (node->sec_incl_failed)?  "failed" : "unknown");
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Vendor id:%04X", node->vid);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Product type id:%04X", node->type);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Product id:%04X", node->pid);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Category:%s", (node->category <= DEV_WALL_CTLR)?
                     dev_category_str[node->category] : "unknown");
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Z-wave library type:%u", node->lib_type);
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Z-wave protocol version:%u.%02u\n", (unsigned)(node->proto_ver >> 8),
                     (unsigned)(node->proto_ver & 0xFF));
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Application version:%u.%02u\n", (unsigned)(node->app_ver >> 8),
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
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Device class: generic:%02X, specific:%02X",
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
    plt_mtx_ulck(hl_appl->desc_cont_mtx);
}


/**
hl_stress_test_thrd - stress test thread
@param[in]	data		The high-level api context
@return
*/
void hl_stress_test_thrd(void *data)
{
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t   *)data;
    int32_t         result;

    while (hl_appl->is_to_run_test)
    {
        result = hl_appl->stress_tst_func(hl_appl);
        if ( result < 0)
            Sleep(2000);//error, retry after 2 seconds
        else if ( result == ZW_ERR_QUEUED)
        {
            Sleep(300);//command queued, retry after 0.3 seconds
        }
    }
    return;
}


#ifdef  ZW_STRESS_TEST

/**
hl_test_rpt_show - display the current test report
@param[in]	hl_appl		The high-level api context
@return
*/
void hl_test_rpt_show(hl_appl_ctx_t   *hl_appl)
{
    plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test report:");
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Transmitted multi level switch frames:%u", hl_appl->test_stat.tx_multi_lvl_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Transmitted seq number frames:%u", hl_appl->test_stat.tx_seq_num_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Received seq number frames:%u", hl_appl->test_stat.rx_seq_num_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Received seq number error:%u", hl_appl->test_stat.rx_seq_num_err);

}


/**
hl_stress_test - toggle between start and stop stress test
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_stress_test(hl_appl_ctx_t   *hl_appl)
{
    HMENU submenu_h;
    BOOL  res;

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST1);

    if (hl_appl->is_stress_tst_run == 0)
    {
        //if (!hl_appl->stress_tst_func)
        //    return;
        hl_appl->stress_tst_func = hl_seq_num_snd;

        //Reset test statistic
        memset(&hl_appl->test_stat, 0, sizeof(test_stat_t));

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test running ...");
        hl_appl->is_to_run_test = 1;
        hl_appl->is_stress_tst_run = 1;

        //Change the menu to "stop stress test"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_TEST,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_TEST,
                            _T("Stop stress test")
                            );
            DrawMenuBar(ghWnd);

        }


        if (plt_thrd_create(hl_stress_test_thrd, hl_appl) < 0)
        {
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Create stress test thread failed");
        }
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test stopped");
        //Change the menu to "Start stress test"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_TEST,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_TEST,
                            _T("Start send sq stress test")
                            );
            DrawMenuBar(ghWnd);

        }
        hl_appl->is_to_run_test = 0;
        hl_appl->is_stress_tst_run = 0;
        hl_test_rpt_show(hl_appl);
    }

}
#endif


/**
hl_ni_stress_test - toggle between start and stop node update stress test
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_ni_stress_test(hl_appl_ctx_t   *hl_appl)
{
    HMENU submenu_h;
    BOOL  res;

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST1);

    if (hl_appl->is_ni_stress_tst_run == 0)
    {
        if (!hl_appl->stress_tst_func)
            return;

        if (hl_appl->stress_tst_func == hl_multi_lvl_snd)
        {   //Multi-level switch stress test
            hl_appl->is_to_run_test = 1;
            if (plt_thrd_create(hl_stress_test_thrd, hl_appl) < 0)
            {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Create stress test thread failed");
                return;
            }
        }
        else
        {   //Node info state-machine stress test
            if (hl_node_updt(hl_appl) != 0)
            {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Stress Test failed to start");
                return;
            }
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test running ...");
        hl_appl->is_ni_stress_tst_run = 1;

        //Change the menu to "stop stress test"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_NI_TEST,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_NI_TEST,
                            _T("Stop stress test")
                            );
            DrawMenuBar(ghWnd);

        }

    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test stopped");
        //Change the menu to "Start stress test"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_NI_TEST,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_NI_TEST,
                            _T("Start stress test")
                            );
            DrawMenuBar(ghWnd);

        }
        hl_appl->is_to_run_test = 0;
        hl_appl->is_ni_stress_tst_run = 0;
    }

}


uint8_t switch_lvl[]={
    0, 0x10, 0x20, 0x40
};


/**
hl_switch_lvl_gen - genarate multi-level switch level
@param[in]	hl_appl		The high-level api context
@return                 Generated switch-level
*/
static uint8_t  hl_switch_lvl_gen(hl_appl_ctx_t *hl_appl)
{
    static const uint8_t switch_lvl[]={ 0, 0x10, 0x20, 0x40
    };

    if (++hl_appl->lvl > 3)
    {
        hl_appl->lvl = 0;
    }

    return switch_lvl[hl_appl->lvl];
}


/**
hl_multi_lvl_snd - Send multi-level switch level
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_snd(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_set(ifd, hl_switch_lvl_gen(hl_appl), 0);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result >= 0)
    {
        //update test statistic
        hl_appl->test_stat.tx_multi_lvl_frm++;
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_snd with error:%d", result);
    }

    return result;

}

#ifdef  ZW_STRESS_TEST

/**
hl_seq_num_snd - Send sequence number for testing using COMMAND_CLASS_MANUFACTURER_PROPRIETARY
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_seq_num_snd(hl_appl_ctx_t   *hl_appl)
{
    uint8_t     cmd[8];
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_MANUFACTURER_PROPRIETARY)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Prepare the command to send sequence number
    cmd[0] = COMMAND_CLASS_MANUFACTURER_PROPRIETARY;
    cmd[1] = MANF_ID_1;
    cmd[2] = MANF_ID_2;
    cmd[3] = hl_appl->seq_num >> 24;
    cmd[4] = (hl_appl->seq_num >> 16) & 0xff;
    cmd[5] = (hl_appl->seq_num >> 8) & 0xff;
    cmd[6] = hl_appl->seq_num  & 0xff;

    //Send the command
    result = zwif_exec(ifd, cmd, 7, NULL);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result >= 0)
    {
        //increment the seq number
        hl_appl->seq_num++;

        //update test statistic
        hl_appl->test_stat.tx_seq_num_frm++;
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_seq_num_snd with error:%d", result);
    }

    return result;
}

#endif



/**
hl_multi_lvl_set - Set multi-level switch level
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_set(ifd, (uint8_t)hl_appl->mul_lvl_val, hl_appl->mul_lvl_dur);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_set with error:%d", result);
    }

    return result;

}


/**
hl_multi_lvl_chg - toggle between start and stop level change
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_multi_lvl_chg(hl_appl_ctx_t   *hl_appl)
{
    HMENU           submenu_h;
    BOOL            res;
    int             result;
    zwifd_p     ifd;

    //Get the test menu hanlde
    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST1);

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_chg: interface descriptor:%u not found", hl_appl->temp_desc);
        return ;
    }

    if (hl_appl->mul_lvl_change_started == 0)
    {
        zwlevel_t       lvl_ctl;

        lvl_ctl.dur = hl_appl->mul_lvl_dur;
        lvl_ctl.pri_dir = hl_appl->mul_lvl_dir;
        lvl_ctl.pri_level = (uint8_t)hl_appl->mul_lvl_val;
        lvl_ctl.pri_ignore_lvl = (hl_appl->mul_lvl_val == 0xFF)? 1:0;
        lvl_ctl.sec_dir = hl_appl->mul_lvl_sec_dir;
        lvl_ctl.sec_step = hl_appl->mul_lvl_sec_step;

        //Change state to start level change
        result = zwif_level_start(ifd, &lvl_ctl);

        plt_mtx_ulck(hl_appl->desc_cont_mtx);

        if (result != 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_start with error:%d", result);
            return;
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Start level change ...");
        hl_appl->mul_lvl_change_started = 1;

        //Change the menu to "Stop level change"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_MULTI_LVL_CHANGE,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_MULTI_LVL_CHANGE,
                            _T("Stop level change")
                            );
            DrawMenuBar(ghWnd);

        }


    }
    else
    {
        //Change state to start level change
        result = zwif_level_stop(ifd);

        plt_mtx_ulck(hl_appl->desc_cont_mtx);

        if (result != 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_stop with error:%d", result);
            return;
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Stop level change ...");
        hl_appl->mul_lvl_change_started = 0;

        //Change the menu to "Start level change"
        if (submenu_h)
        {
            res = ModifyMenu(
                            submenu_h,
                            IDM_MULTI_LVL_CHANGE,
                            MF_BYCOMMAND | MF_STRING,
                            IDM_MULTI_LVL_CHANGE,
                            _T("Start level change")
                            );
            DrawMenuBar(ghWnd);

        }
    }
}


/**
hl_multi_lvl_sup_cb - multi level switch type callback
@param[in]	ifd	interface
@param[in]	pri_type    primary switch type, SW_TYPE_XX
@param[in]	sec_type    secondary switch type , SW_TYPE_XX.
@return
*/
void hl_multi_lvl_sup_cb(zwifd_p ifd,  uint8_t pri_type, uint8_t sec_type)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Primary switch type:%u", pri_type);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Secondary switch type:%u", sec_type);

}


/**
hl_multi_lvl_sup - Get switch type
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_sup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_sup_get(ifd, hl_multi_lvl_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_sup_get with error:%d", result);
    }

    return result;
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
/*
int32_t hl_suc_set(hl_appl_ctx_t   *hl_appl)
{
    int32_t     result;
    zwnoded_p noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->suc_node_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnet_assign(noded, ZWNET_ROLE_SIS);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_suc_set with error:%d", result);
    }
    return result;
}
*/


/**
hl_ep_nameloc_set - Set name & location of an endpoint
@param[in]	hl_appl		        The high-level api context
@param[in]	ep_desc_id		    Endpoint descriptor id
@return  0 on success, negative error number on failure
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
int hl_ep_nameloc_set(hl_appl_ctx_t   *hl_appl, uint32_t ep_desc_id)
{
    int32_t     result;
    zwepd_p     epd;

    //Get the endpoint descriptor
    epd = hl_ep_desc_get(hl_appl->desc_cont_hd, ep_desc_id);
    if (!epd)
    {
        return ZW_ERR_EP_NOT_FOUND;
    }

    result = zwep_nameloc_set(epd, &hl_appl->nameloc);

    if (result == 0)
    {
        //Update the endpoint descriptor
#ifdef USE_SAFE_VERSION
        strcpy_s(epd->name, ZW_LOC_STR_MAX + 1, hl_appl->nameloc.name);
        strcpy_s(epd->loc, ZW_LOC_STR_MAX + 1, hl_appl->nameloc.loc);
#else
        strcpy(epd->name, hl_appl->nameloc.name);
        strcpy(epd->loc, hl_appl->nameloc.loc);
#endif
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ep_nameloc_set with error:%d", result);

    }
    return result;

}

/**
hl_nameloc_set - Set name & location of a  node id
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_nameloc_set(hl_appl_ctx_t   *hl_appl)
{
    int     result;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);

    result = hl_ep_nameloc_set(hl_appl, hl_appl->rep_desc_id);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nameloc_set with error:%d", result);
    }

    return result;
}


/**
hl_bin_set - Turn binary switch on/off
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_switch_set(ifd, hl_appl->bin_state);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_switch_set with error:%d", result);
    }

    return result;

}


/**
hl_cmd_q_ctl_get - Get the commmand queuing control state
@param[in]	hl_appl		    The high-level api context
@param[out]	q_ctl_state		The commmand queuing control state
@return  0 on success, negative error number on failure
*/
int32_t hl_cmd_q_ctl_get(hl_appl_ctx_t   *hl_appl, uint8_t *q_ctl_state)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_ena_get(noded, q_ctl_state);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_ctl_get with error:%d", result);
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queuing is:%s", (*q_ctl_state)? "on" : "off");
    }

    return result;

}


/**
hl_cmd_q_ctl_set - Set the commmand queuing control state
@param[in]	hl_appl		    The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cmd_q_ctl_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_ena_set(noded, hl_appl->cmd_q_ctl);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_ctl_set with error:%d", result);
    }

    return result;

}


/**
hl_cmd_q_view - View the commmand queue
@param[in]	hl_appl		    The high-level api context
@return
*/
void hl_cmd_q_view(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;
    uint16_t    *cmd_queue;
    uint16_t    i;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return;
    }

    result = zwnode_cmd_q_get(noded, &cmd_queue);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result > 0)
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queue:");
        for (i=0; i<result; i++)
        {
            plt_msg_show(hl_plt_ctx_get(hl_appl), "%u", cmd_queue[i]);
        }
        free(cmd_queue);
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queue is empty");
    }
}


/**
hl_cmd_q_cancel - Cancel the commmand queue
@param[in]	hl_appl		    The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_cmd_q_cancel(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_cancel(noded);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_cancel with error:%d", result);
    }
    return result;
}


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
hl_quick_reset - Restore factory default
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_quick_reset(hl_appl_ctx_t *hl_appl)
{
    int     result;
    TCHAR   comm_port_name[16];
    TCHAR   error_msg[80];

    memset(comm_port_name, 0, sizeof(comm_port_name));

    //Create serial comm port name for Windows platform
    //Note: The comm port name must have a prefix "\\\\.\\" in order to access
    //      COM10 or port number >= 10.
    _stprintf_s(comm_port_name, sizeof(comm_port_name)/sizeof(TCHAR),
                _T("\\\\.\\COM%d"), hl_appl->port_number);


    result = zwnet_quick_reset(comm_port_name);
    if (result == 0)
    {
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("hl_quick_reset done!\r\n"));
    }
    else
    {
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("hl_quick_reset with error:%d\r\n"), result);
    }
    hl_msg_show(error_msg);
    return result;
}


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
hl_generic_report_cb - Generic report callback
@param[in]	ifd	The interface that received the report
@param[in]	level	    The reported level
@return
*/
void hl_generic_report_cb(zwifd_p ifd, uint8_t level)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Generic report level:%02Xh", level);
}

/**
hl_multi_lvl_rep_setup - Setup multi-level switch report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_rpt_set(ifd, hl_generic_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_multi_lvl_rep_get - Get multi-level switch level report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_level_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_level_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_bin_report_cb - binary switch report callback
@param[in]	ifd	The interface that received the report
@param[in]	on		    0=off, else on
@return
*/
void hl_bin_report_cb(zwifd_p ifd, uint8_t on)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Binary switch is %s", (on)? "on" : "off");
}


/**
hl_bin_rep_setup - Setup binary switch report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_switch_rpt_set(ifd, hl_bin_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_binary_rep_get - Get binary switch state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_binary_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_switch_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_switch_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_binary_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_basic_report_cb - basic command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	value	The value
@return
*/
void hl_basic_report_cb(zwifd_p ifd, uint8_t value)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Basic command value is %02Xh", value);
}


/**
hl_basic_rep_setup - Setup basic command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_basic_rpt_set(ifd, hl_basic_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_basic_rep_get - Get basic command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_basic_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_basic_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_basic_set - basic command set value
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_basic_set(ifd, (uint8_t)hl_appl->basic_val);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_set with error:%d", result);
    }

    return result;

}


/**
hl_dlck_op_report_cb - Door lock operation status report callback
@param[in]	ifd	    The interface that received the report
@param[in]	op_sts  Operation status
@return
*/
void hl_dlck_op_report_cb(zwifd_p ifd, zwdlck_op_p  op_sts)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door lock operation mode:%02X,", op_sts->mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Outside door handles mode:%02X,", op_sts->out_mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Inside door handles mode:%02X,", op_sts->in_mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door condition:%02X,", op_sts->cond);
    if (op_sts->tmout_min != 0xFE)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Remaining time in unsecured state:%u:%u,",
                     op_sts->tmout_min, op_sts->tmout_sec);
    }

}


/**
hl_dlck_op_rep_setup - Setup door lock operation status report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_op_rpt_set(ifd, hl_dlck_op_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_dlck_op_rep_get - Get the state of the door lock device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_dlck_op_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_dlck_op_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_dlck_op_set - Set door lock operation
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_op_set(ifd, hl_appl->dlck_mode);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_set with error:%d", result);
    }

    return result;

}


/**
hl_dlck_cfg_report_cb - Report callback for door lock configuration
@param[in]	ifd	    interface
@param[in]	config	configuration
*/
void hl_dlck_cfg_report_cb(zwifd_p ifd, zwdlck_cfg_p  config)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door lock operation type:%s,",
                 (config->type == ZW_DOOR_OP_CONST)? "constant" : "timed");

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Outside door handles state:%02X,", config->out_sta);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Inside door handles state:%02X,", config->in_sta);
    if (config->type == ZW_DOOR_OP_TIMED)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Time the lock stays unsecured.:%u:%u,",
                     config->tmout_min, config->tmout_sec);
    }
}


/**
hl_dlck_cfg_get - Get configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_cfg_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_cfg_get(ifd, hl_dlck_cfg_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_cfg_get with error:%d", result);
    }

    return result;
}


/**
hl_dlck_cfg_set - Set the configuration of the door lock device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_cfg_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_cfg_set(ifd, &hl_appl->dlck_config);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_cfg_set with error:%d", result);
    }

    return result;

}


/**
hl_usrcod_report_cb - Report callback for user code
@param[in]	ifd	        interface
@param[in]	usr_cod     user code and its status
*/
static void hl_usrcod_report_cb(zwifd_p ifd, zwusrcod_p  usr_cod)
{
    static const char *usrid_sts[] =
    {
        "Available (not set)",
        "Occupied",
        "Reserved by administrator",
        "Status unavailable"
    };

    char usr_code[MAX_USRCOD_LENGTH + 1];

    if (usr_cod->id_sts > ZW_USRCOD_RSVD)
    {
        usr_cod->id_sts = 3;
    }

    memcpy(usr_code, usr_cod->code, usr_cod->code_len);
    usr_code[usr_cod->code_len] = '\0';

    plt_msg_show(hl_if_plt_ctx_get(ifd), "User id:%u, status:%s, code:%s", usr_cod->id,
                 usrid_sts[usr_cod->id_sts], usr_code);
}


/**
hl_usrcod_get - Get user code
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_get(ifd, hl_appl->usr_id, hl_usrcod_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_get with error:%d", result);
    }

    return result;
}


/**
hl_usrcod_set - Set user code
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_set(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_set(ifd, &my_appl.usr_code);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_set with error:%d", result);
    }

    return result;
}


/**
hl_usrcod_sup_cb - Report callback for number of supported user codes
@param[in]	ifd	        interface
@param[in]	usr_num     number of supported user codes
*/
static void hl_usrcod_sup_cb(zwifd_p ifd, uint8_t  usr_num)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max. number of supported user codes:%u", usr_num);

}


/**
hl_usrcod_sup_get - Get number of supported user codes
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_sup_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_sup_get(ifd, hl_usrcod_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_alrm_report_cb - Alarm status report callback
@param[in]	ifd	        The interface that received the report
@param[in]	alarm_info  alarm info
@return
*/
void hl_alrm_report_cb(zwifd_p ifd, zwalrm_p  alarm_info)
{

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Vendor specific alarm type:%02Xh, alarm level:%02Xh", alarm_info->type, alarm_info->level);

    if (alarm_info->ex_info)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Zensor net source node id:%u,", alarm_info->ex_zensr_nodeid);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave defined:");
        plt_msg_show(hl_if_plt_ctx_get(ifd), "alarm type:%s, status:%02Xh, event:%02Xh",
                     (alarm_info->ex_type <= ZW_ALRM_CLOCK)? zw_alrm_type[alarm_info->ex_type] : zw_alrm_type[0],
                     alarm_info->ex_status, alarm_info->ex_event);

        if (alarm_info->ex_evt_len)
        {
            switch (alarm_info->ex_evt_type)
            {
                case ZW_ALRM_PARAM_LOC:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "alarm location:%s", alarm_info->ex_evt_prm);
                    break;

                case ZW_ALRM_PARAM_USRID:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "user id:%u", alarm_info->ex_evt_prm[0]);
                    break;

                case ZW_ALRM_PARAM_OEM_ERR_CODE:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "OEM proprietary system failure code:");
                    plt_bin_show(hl_if_plt_ctx_get(ifd), alarm_info->ex_evt_prm, alarm_info->ex_evt_len);
                    break;
            }
        }
    }
}


/**
hl_alrm_rep_setup - Setup alarm status report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_rpt_set(ifd, hl_alrm_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_alrm_rep_get - Get the state of the alarm device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_alrm_get_poll(ifd, hl_appl->alrm_vtype, hl_appl->alrm_ztype, hl_appl->alrm_evt, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_alrm_get(ifd, hl_appl->alrm_vtype, hl_appl->alrm_ztype, hl_appl->alrm_evt);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_alrm_set - Set the activity of the Z-Wave Alarm Type
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_set(ifd, hl_appl->alrm_ztype, hl_appl->alrm_sts);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_set with error:%d", result);
    }

    return result;

}


/**
hl_alrm_sup_report_cb - Report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	have_vtype  flag to indicate whether vendor specific alarm type supported. 1=supported; else 0=unsupported
@param[in]	ztype_len   size of ztype buffer
@param[in]	ztype       buffer to store supported Z-wave alarm types (ZW_ALRM_XXX)
*/
void hl_alrm_sup_report_cb(zwifd_p ifd, uint8_t have_vtype, uint8_t ztype_len, uint8_t *ztype)
{

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Vendor specific alarm type is %s", (have_vtype)? "supported" : "unsupported");
    if (ztype_len > 0)
    {
        int i;
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave alarm types:");
        for (i=0; i<ztype_len; i++)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", (ztype[i] <= ZW_ALRM_CLOCK)? zw_alrm_type[ztype[i]] : zw_alrm_type[0]);
        }

    }
}


/**
hl_alrm_sup_get - Get supported alarm types
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_sup_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_sup_get(ifd, hl_alrm_sup_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_alrm_sup_evt_report_cb - Report callback for supported alarm events
@param[in]	ifd	        interface
@param[in]	ztype       Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	evt_len     size of evt buffer
@param[in]	evt         buffer to store supported event of the alarm type specified in ztype
*/
void hl_alrm_sup_evt_report_cb(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt)
{

    if (evt_len > 0)
    {
        int i;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave alarm type = %s, supported events:",
                     (ztype <= ZW_ALRM_CLOCK)? zw_alrm_type[ztype] : zw_alrm_type[0]);

        for (i=0; i<evt_len; i++)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "%02Xh, ", evt[i]);
        }
    }
}

/**
hl_alrm_sup_evt_get - Get supported alarm events
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_sup_evt_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_sup_evt_get(ifd, hl_appl->alrm_ztype, hl_alrm_sup_evt_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_alrm_sup_evt_get with error:%d", result);
    }

    return result;
}


/**
hl_bin_snsr_rep_cb - binary sensor report callback
@param[in]	ifd	The interface that received the report
@param[in]	state	    The state of the sensor: 0=idle, else event detected
@return
*/
void hl_bin_snsr_rep_cb(zwifd_p ifd, uint8_t state)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Binary sensor state :%s", (state == 0)? "idle" : "event detected");
}

/**
hl_bin_snsr_rep_setup - Setup binary sensor report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_snsr_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_bsensor_rpt_set(ifd, hl_bin_snsr_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_bin_snsr_rep_get - Get binary sensor state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_snsr_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_bsensor_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_bsensor_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_ml_snsr_rep_cb - multi-level sensor report callback
@param[in]	ifd	interface
@param[in]	value		The current value and unit of the sensor.
*/
static void hl_ml_snsr_rep_cb(zwifd_p ifd, zwsensor_t *value)
{
    int32_t         snsr_value;
    const char      **unit_str;

    if (value->type > ZW_SENSOR_TYPE_TGT_TEMP)
    {
        value->type = 0;
    }

    unit_str = sensor_unit_str + (value->type * 4) + value->unit;

    if (*unit_str == NULL)
    {
        *unit_str = "undefined";
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Multi-level sensor type:%s, precision:%u, unit:%s",
                    sensor_type_str[value->type], value->precision, *unit_str);

    if (!hl_int_get(value->data, value->size, &snsr_value))
    {   //Error, default to zero
        snsr_value = 0;
    }

    if (value->precision == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Sensor reading:%d", snsr_value);
    }
    else
    {
        char    float_str[80];

        hl_float_get(snsr_value, value->precision, 80, float_str);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Sensor reading:%s", float_str);

    }
}

/**
hl_ml_snsr_rep_setup - Setup multi-level sensor report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_ml_snsr_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_rpt_set(ifd, hl_ml_snsr_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_ml_snsr_rep_get - Get multi-level sensor state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_ml_snsr_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_sensor_get_poll(ifd, hl_appl->sensor_type, hl_appl->sensor_unit, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_sensor_get(ifd, hl_appl->sensor_type, hl_appl->sensor_unit);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_snsr_sup_report_cb - Report callback for supported sensor types
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_SENSOR_TYPE_XXX)
*/
static void hl_snsr_sup_report_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type)
{
    if (type_len > 0)
    {
        int i;
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave sensor types:");
        for (i=0; i<type_len; i++)
        {
            if (type[i] > ZW_SENSOR_TYPE_TGT_TEMP)
            {
                type[i] = 0;
            }

            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", sensor_type_str[type[i]]);
        }
    }
}


/**
hl_ml_snsr_sup_get - Get supported sensor types through report callback
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ml_snsr_sup_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_sup_get(ifd, hl_snsr_sup_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_snsr_unit_report_cb - Report callback for supported sensor units
@param[in]	ifd	            interface
@param[in]	sensor_type     sensor type, ZW_SENSOR_TYPE_XXX
@param[in]	sensor_unit_msk bitmask of units supported for the sensor_type, ZW_SENSOR_UNIT_XXX
*/
static void hl_snsr_unit_report_cb(zwifd_p ifd, uint8_t sensor_type, uint8_t sensor_unit_msk)
{
    int             i;
    const char      **unit_str;

    if (sensor_type > ZW_SENSOR_TYPE_TGT_TEMP)
    {
        sensor_type = 0;
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave sensor type:%s, supported units:", sensor_type_str[sensor_type]);

    for (i=0; i<4; i++)
    {
        if (sensor_unit_msk & (0x01 << i))
        {
            unit_str = sensor_unit_str + (sensor_type * 4) + i;

            if (*unit_str == NULL)
            {
                *unit_str = "undefined";
            }

            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", *unit_str);
        }
    }
}


/**
hl_ml_snsr_unit_get - Get supported sensor units through report callback
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ml_snsr_unit_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_unit_get(ifd, hl_appl->sensor_type, hl_snsr_unit_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_unit_get with error:%d", result);
    }

    return result;
}


/**
hl_wkup_rep_cb - wake up notification callback
@param[in]	ifd	interface
@param[in]	cap	capabilities report, null for notification
@return	Only apply to notification: 0=no command pending to send; 1=commands pending to send.
*/
int hl_wkup_rep_cb(zwifd_p ifd, zwif_wakeup_p cap)
{
    if (!cap)
    {   //Notification
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Wake up notification from node:%u", ifd->nodeid);
        //Nothing to send, tell the node to sleep again
        //return 0;

        //Assume user has something to send
        return 1;
    }
    else
    {   //Capabilities report
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Wake up settings:");
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Alert receiving node: %u", cap->node.nodeid);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Current interval: %u s", cap->cur);
        if (cap->min == 0)
        {
            return 0;
        }
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Min: %u s, Max: %u s", cap->min, cap->max);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Default: %u s, Step: %u s", cap->def, cap->interval);

    }
    return 0;
}


/**
hl_wkup_get - Get wake up setting
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_wkup_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_wakeup_get(ifd, hl_wkup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_wkup_get with error:%d", result);
    }

    return result;
}


/**
hl_wkup_set - Set wake up interval and alert receiving node
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_wkup_set(hl_appl_ctx_t   *hl_appl)
{
    int             result;
    zwifd_p     ifd;
    zwnoded_p   noded;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->node_desc_id);
    if (!noded && hl_appl->node_desc_id)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwif_wakeup_set(ifd, hl_appl->wkup_interval, noded);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_wkup_set with error:%d", result);
    }

    return result;
}


/**
hl_grp_rep_cb - Group info report callback
@param[in]	ifd	Interface
@param[in]	group	    Grouping identifier
@param[in]	max_cnt	    Maximum number of end points the grouping identifier above supports
@param[in]	cnt	        The number of end points in the grouping in this report
@param[in]	ep	        An array of cnt end points in the grouping
@return
*/
void hl_grp_rep_cb(zwifd_p ifd, uint8_t group, uint8_t max_cnt, uint8_t cnt, zwepd_p ep)
{
    int i;
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Group id:%u, max supported endpoints:%u, Group members:", group, max_cnt);
    for (i=0; i<cnt; i++)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Node id:%u, endpoint id:%u", ep[i].nodeid, ep[i].epid);
    }

}

/**
hl_grp_rep_get - Get group info report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_get(ifd, hl_appl->group_id, hl_grp_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_add - Add endpoints into group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_add(hl_appl_ctx_t   *hl_appl)
{
    int         i;
    int         result;
    uint8_t     ep_cnt;
    zwifd_p ifd;
    zwepd_t ep_desc[5];
    zwepd_p ep;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    ep_cnt = 0;
    for (i=0; i<5; i++)
    {
        ep = hl_ep_desc_get(hl_appl->desc_cont_hd, hl_appl->ep_desc_id[i]);
        if (ep)
        {
            ep_desc[ep_cnt++] = *ep;
        }
    }

    result = zwif_group_add(ifd, hl_appl->group_id, ep_desc, ep_cnt);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_add with error:%d", result);
    }

    return result;
}


/**
hl_grp_del - Delete endpoints from group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_del(hl_appl_ctx_t   *hl_appl)
{
    int         i;
    int         result;
    uint8_t     ep_cnt;
    zwifd_p ifd;
    zwepd_t ep_desc[5];
    zwepd_p ep;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    ep_cnt = 0;
    for (i=0; i<5; i++)
    {
        ep = hl_ep_desc_get(hl_appl->desc_cont_hd, hl_appl->ep_desc_id[i]);
        if (ep)
        {
            ep_desc[ep_cnt++] = *ep;
        }
    }

    result = zwif_group_del(ifd, hl_appl->group_id, ep_desc, ep_cnt);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_del with error:%d", result);
    }

    return result;
}


/**
hl_grp_sup_cb - max number of groupings callback
@param[in]	ifd	      interface
@param[in]	max_grp   maximum number of groupings
@return
*/
void hl_grp_sup_cb(zwifd_p ifd,  uint8_t max_grp)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max number of groupings:%u", max_grp);
}


/**
hl_grp_sup - Get max number of groupings
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_sup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_sup_get(ifd, hl_grp_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_active_cb - active group callback
@param[in]	ifd	    interface
@param[in]	group   current active group
@return
*/
void hl_grp_active_cb(zwifd_p ifd,  uint8_t group)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Current active group:%u", group);
}


/**
hl_grp_active - Get active group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_active(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_actv_get(ifd, hl_grp_active_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_actv_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_sup_cb - Get cmd records supported callback
@param[in]	ifd	      interface
@param[in]	cmd_cap   command records supporting capabilities
@return
*/
void hl_grp_cmd_sup_cb(zwifd_p ifd, zwgrp_cmd_cap_p  cmd_cap)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Command %s configurable", (cmd_cap->configurable)? "is" : "is not");
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Support %s command", (cmd_cap->config_type)? "basic set" : "any");
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max command length: %u", cmd_cap->max_len);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Number of free command records: %u", cmd_cap->free_rec);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Total number of command records supported: %u", cmd_cap->total_rec);
}


/**
hl_grp_cmd_sup - Get cmd records supported
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_sup(hl_appl_ctx_t   *hl_appl)

{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_cmd_sup_get(ifd, hl_grp_cmd_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_set - Set cmd to node in group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    zwnoded_p   noded;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->node_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwif_group_cmd_set(ifd, hl_appl->group_id, noded, hl_appl->command, hl_appl->cmd_len);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_set with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_get_cb - Get command record for a node within a given grouping identifier callback
@param[in]	ifd	    interface
@param[in]	group	grouping identifier
@param[in]	nodeid	node id of the node within the grouping specified
@param[in]	cmd_buf	command and parameters
@param[in]	len     length of cmd_buf
*/
void hl_grp_cmd_get_cb(zwifd_p ifd, uint8_t group, uint8_t nodeid, uint8_t *cmd_buf, uint8_t len)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Command record for group id:%u, node id:%u :", group, nodeid);
    plt_bin_show(hl_if_plt_ctx_get(ifd), cmd_buf, len);
}


/**
hl_grp_cmd_get - Get command record for a node within a given grouping identifier
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_cmd_get(ifd, hl_appl->group_id, hl_appl->node_id, hl_grp_cmd_get_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_set with error:%d", result);
    }

    return result;
}


/**
hl_cfg_set - Set configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    zwconfig_t  param;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check for invalid size
    switch (hl_appl->cfg_size)
    {
        case 1:
            param.data[0] = hl_appl->cfg_value & 0xFF;
            break;

        case 2:
            param.data[0] = (hl_appl->cfg_value >> 8) & 0xFF;
            param.data[1] = hl_appl->cfg_value & 0xFF;
            break;

        case 4:
            param.data[0] = (hl_appl->cfg_value >> 24) & 0xFF;
            param.data[1] = (hl_appl->cfg_value >> 16) & 0xFF;
            param.data[2] = (hl_appl->cfg_value >> 8) & 0xFF;
            param.data[3] = hl_appl->cfg_value & 0xFF;
            break;

        default:
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Invalid config param size:%u", hl_appl->cfg_size);
            return ZW_ERR_VALUE;
    }

    param.param_num = hl_appl->cfg_param;
    param.size = hl_appl->cfg_size;
    param.use_default = hl_appl->cfg_value_default;

    result = zwif_config_set(ifd, &param);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_set with error:%d", result);
    }

    return result;
}


/**
hl_cfg_report_cb - Get configuration parameter callback
@param[in]	ifd	    interface
@param[in]	param   parameter value
*/
void hl_cfg_report_cb(zwifd_p ifd, zwconfig_p param)
{
    int32_t param_value;

    if (!hl_int_get(param->data, param->size, &param_value))
    {   //Error, default to zero
        param_value = 0;
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Configuration parameter:%u, value:%d", param->param_num, param_value);

    //Check whether to get another report
    if (my_appl.cfg_param_mode == 1)
    {
        if (param->param_num < my_appl.cfg_range_end)
        {
            zwif_config_get(ifd, param->param_num + 1);
        }
    }
}


/**
hl_cfg_rep_setup - Setup a configuration parameter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_config_rpt_set(ifd, hl_cfg_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_cfg_get - Get configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    uint8_t     param_num;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check whether to get single or a range of parameter value
    if (hl_appl->cfg_param_mode == 0)
    {   //Single
        param_num = hl_appl->cfg_param;
    }
    else
    {
        //Multiple
        if (hl_appl->cfg_range_start > hl_appl->cfg_range_end)
        {
            hl_appl->cfg_range_end = hl_appl->cfg_range_start;
        }
        param_num = (uint8_t)hl_appl->cfg_range_start;

    }

    result = zwif_config_get(ifd, param_num);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_get with error:%d", result);
    }

    return result;
}


/**
hl_pulsemeter_rep_cb - pulse meter report callback
@param[in]	ifd	    The interface that received the report
@param[in]	cnt	    The number of pulses detected in the device
@return
*/
void hl_pulsemeter_rep_cb(zwifd_p ifd, uint32_t cnt)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Number of pulses detected :%u", cnt);
}

/**
hl_pulsemeter_rep_setup - Setup pulse meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_pulsemeter_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_pulsemeter_rpt_set(ifd, hl_pulsemeter_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_pulsemeter_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_pulsemeter_rep_get - Get number of pulses detected report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_pulsemeter_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_pulsemeter_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_pulsemeter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_rep_cb - meter report callback
@param[in]	ifd	    The interface that received the report
@param[in]	data    current value and unit of the meter
@return
*/
static void hl_meter_rep_cb(zwifd_p ifd, zwmeter_dat_p value)
{
    static char *meter_type[] = { "unknown", "electric", "gas", "water"
    };
    static char *meter_rate[] = { "unknown", "import", "export"
    };
    int32_t meter_value;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter type:%s, precision:%u, unit:%u, rate type:%s",
                    meter_type[value->type], value->precision, value->unit,
                    meter_rate[value->rate_type]);

    if (!hl_int_get(value->data, value->size, &meter_value))
    {   //Error, default to zero
        meter_value = 0;
    }

    if (value->precision == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter reading:%d", meter_value);
    }
    else
    {
        char    float_str[80];

        hl_float_get(meter_value, value->precision, 80, float_str);

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter reading:%s", float_str);

    }

    //Check if to display previous reading
    if (value->delta_time > 0)
    {
        if (!hl_int_get(value->prv_data, value->size, &meter_value))
        {   //Error, default to zero
            meter_value = 0;
        }

        if (value->precision == 0)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "Previous Meter reading:%d, taken %us ago", meter_value, value->delta_time);
        }
        else
        {
            char    float_str[80];

            hl_float_get(meter_value, value->precision, 80, float_str);

            plt_msg_show(hl_if_plt_ctx_get(ifd), "Previous Meter reading:%s, taken %us ago", float_str, value->delta_time);

        }
    }


}

/**
hl_meter_rep_setup - Setup meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_rpt_set(ifd, hl_meter_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_meter_rep_get - Get meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_meter_get_poll(ifd, hl_appl->meter_unit, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_meter_get(ifd, hl_appl->meter_unit);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_sup_cb - report callback for meter capabilities
@param[in]	ifd	        interface
@param[in]	meter_cap	meter capabilities
*/
void hl_meter_sup_cb(zwifd_p ifd, zwmeter_cap_p meter_cap)
{
    static char *meter_type[] = { "unknown", "electric", "gas", "water"
    };
    plt_msg_show(hl_if_plt_ctx_get(ifd), "meter type:%s, meter supported unit bit-mask:%02xh", meter_type[meter_cap->type], meter_cap->unit_sup);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "meter %s be reset", (meter_cap->reset_cap)? "can" : "can not");

}

/**
hl_meter_sup - get information on the meter capabilities
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_sup(hl_appl_ctx_t   *hl_appl)

{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_sup_get(ifd, hl_meter_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_reset - Reset all accumulated values stored in the meter device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_reset(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_reset(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_reset with error:%d", result);
    }

    return result;
}


/**
hl_meter_admin_cb - meter admin name and capability callback
@param[in]	ifd	    The interface
@param[in]	meter	Meter descriptor
@return
*/
static void hl_meter_admin_cb(zwifd_p ifd, zwmeter_p meter)
{
    if (meter->caps & ZW_METER_CAP_MON)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter admin name:%s, id:%s",
                        meter->admin, meter->id);
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter admin name %s be set",
                    (meter->caps & ZW_METER_CAP_CFG)? "can" : "cannot");
}


/**
hl_meter_admin_set - Set meter admin name
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_admin_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_set_admin(ifd, hl_appl->meter_admin);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_admin_set with error:%d", result);
    }

    return result;

}


/**
hl_meter_admin_cap_get - Get meter admin name and capability
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_admin_cap_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_get_desc(ifd, hl_meter_admin_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_av_get_cb - Get supported AV commands callback
@param[in]	ifd	    interface
@param[in]	length	mask byte length
@param[in]	mask	mask buffer pointer
@return
*/
void hl_av_get_cb(zwifd_p ifd, uint16_t length, uint8_t *mask)
{
    uint16_t    i;
    char    num_str[56];

    num_str[0] = '\0';

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported AV commands:");
    for (i = 0; i < (length*8); i++)
    {
        if ((mask[i>>3] >> (i & 7)) & 0x01)
        {
            util_num_string_add(num_str, 56, (unsigned)(i+1));
            if (strlen(num_str) > 50)
            {
                plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", num_str);
                num_str[0] = '\0';
            }
        }
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", num_str);
}


/**
hl_av_get - Get supported AV commands
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_av_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_av_caps(ifd, hl_av_get_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_av_get with error:%d", result);
    }

    return result;
}


/**
hl_battery_report_cb - battery command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	value	The value
@return
*/
void hl_battery_report_cb(zwifd_p ifd, uint8_t value)
{
    if (/*(value >= 0) &&*/ (value <= 100))
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Battery level is %u%%", value);
    }
    else if (value == 0xFF)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Battery low warning!");
    }
}


/**
hl_battery_rep_setup - Setup battery command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_battery_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_battery_rpt_set(ifd, hl_battery_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_battery_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_battery_rep_get - Get battery command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_battery_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_battery_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_battery_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_battery_rep_get with error:%d", result);
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
        PostMessage(ghWnd, MSG_ZWAPI_NOTIFY, 0, (LPARAM )nw_notify);
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
    UINT  length;       // the length of message in the edit control window

    length = SendDlgItemMessage(ghWnd, IDC_MAIN_EDIT, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
    SendDlgItemMessage(ghWnd, IDC_MAIN_EDIT, EM_SETSEL, (WPARAM)length, (LPARAM)length);
    SendDlgItemMessage(ghWnd, IDC_MAIN_EDIT, EM_REPLACESEL, FALSE, (LPARAM)msg);
}


/**
hl_init_menu_set - Control the display of File->Init menu and the other menus
@param[in]	hl_appl		        The high-level api context
@param[in]	show_init_menu		Flag to display "Init" or "Close"
@return
*/
static void hl_init_menu_set(hl_appl_ctx_t   *hl_appl, int show_init_menu)
{
	int		i;
    HMENU   submenu_h;
    UINT    menu_flag;
    UINT    menu_flag_inv;  //inverse of menu_flag
    BOOL  res;



    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_FILE);

    if (submenu_h)
    {
        res = ModifyMenu(
                        submenu_h,
                        IDM_START,
                        MF_BYCOMMAND | MF_STRING,
                        IDM_START,
                        (show_init_menu)? _T("&Init") : _T("&Close")
                        );

        //Enable/ disable save menu
        EnableMenuItem(submenu_h, IDM_SAVE, (show_init_menu)?
                       (MF_BYCOMMAND | MF_GRAYED) : MF_BYCOMMAND);
    }

    menu_flag = (show_init_menu)?
        (MF_BYPOSITION | MF_GRAYED) : (MF_BYPOSITION | MF_ENABLED);

    menu_flag_inv = (show_init_menu)?
        (MF_BYPOSITION | MF_ENABLED) : (MF_BYPOSITION | MF_GRAYED);

    //Enable menu items when initialized
    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST1);//Test 1
#ifdef  ZW_STRESS_TEST
    for (i=0; i<46; i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }
#else
    for (i=0; i<(46 - 3); i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }
#endif

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST2);//Test 2
    for (i=0; i<35; i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST3);//Test 3
    for (i=0; i<35; i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_TEST4);//Test 4
    for (i=0; i<29; i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_MANAGE);//Manage nw
    for (i=0; i<16; i++)//Must also count separators
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_DEVICE);//Device
    for (i=0; i<3; i++)
    {
        if (i == 1)
        {   //Reset controller NVM without initialization
            EnableMenuItem(submenu_h, i, menu_flag_inv);
        }
        else
        {
            EnableMenuItem(submenu_h, i, menu_flag);
        }
    }

    submenu_h = GetSubMenu(GetMenu(ghWnd), SUBMENU_HELP);//Help
    for (i=0; i<3; i++)
    {
        EnableMenuItem(submenu_h, i, menu_flag);
    }
    DrawMenuBar(ghWnd);
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
    TCHAR           error_msg[80];
    int             ret;
    char            dev_file[384];

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, APPL_CONFIG_FILE, "rt") != 0)
#else
    file = fopen(APPL_CONFIG_FILE, "rt");
    if (!file)
#endif
    {
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("hl_config_get can't open file:%s\r\n"), _T(APPL_CONFIG_FILE));
        hl_msg_show(error_msg);
        return ZW_ERR_FILE_OPEN;
    }

    //Get device specific configuration file paths
    ret = hl_config_file_get(file, dev_file);

    fclose(file);

    if (ret != 1)
    {
        hl_msg_show(_T("Failed to get file path and configuration parameter!\r\n"));
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
        hl_msg_show(_T("Failed to open device settings file.\r\n"));
        return ZW_ERR_FILE_OPEN;
    }

    ret = hl_dev_setting_get(file, dev_cfg);
    fclose(file);

    if (ret != 0)
    {
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("hl_dev_setting_get with error:%d\r\n"), ret);
        hl_msg_show(error_msg);
        return ret;
    }

    _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                _T("Number of device settings records:%u\r\n"), dev_cfg->dev_cfg_cnt);
    hl_msg_show(error_msg);

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
@return
*/
void    hl_init(hl_appl_ctx_t *hl_appl)
{
    int                 result;
    TCHAR               error_msg[200];
    TCHAR               comm_port_name[16];
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
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("hl_config_get with error:%d\r\n"), result);
        hl_msg_show(error_msg);
        return;
    }

#ifdef  USER_APPL_DEVICE_CFG

    //Load and save device specific configurations
    if (dev_spec_cfg.dev_cfg_cnt && dev_spec_cfg.dev_cfg_buf)
    {
        result = hl_dev_cfg_load(&dev_spec_cfg, hl_appl->dev_cfg_bufs);
        if (result < 0)
        {
            _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                        _T("hl_dev_cfg_load with error:%d\r\n"), result);
            hl_msg_show(error_msg);
            free(dev_spec_cfg.dev_cfg_buf);
            return;
        }
    }

    dev_spec_cfg_usr.dev_cfg_ctx = hl_appl;
    dev_spec_cfg_usr.dev_cfg_ver = dev_spec_cfg.dev_cfg_ver;
    dev_spec_cfg_usr.dev_rec_find_fn = hl_dev_rec_find;
#endif

    //Init high-level appl layer
    if (!plt_mtx_init(&hl_appl->desc_cont_mtx))
    {
        goto l_HL_INIT_ERROR1;
    }

    memset(comm_port_name, 0, sizeof(comm_port_name));

    //Create serial comm port name for Windows platform
    //Note: The comm port name must have a prefix "\\\\.\\" in order to access
    //      COM10 or port number >= 10.
    _stprintf_s(comm_port_name, sizeof(comm_port_name)/sizeof(TCHAR),
                _T("\\\\.\\COM%d"), hl_appl->port_number);

    zw_init.instance = 0;
    zw_init.comm_port_name = comm_port_name;
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
        _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                    _T("zwnet_init with error:%d\r\n"), result);
        hl_msg_show(error_msg);

        if (result == ZW_ERR_NO_RESP)
        {
            _stprintf_s(error_msg, sizeof(error_msg)/sizeof(TCHAR),
                        _T("The controller's NVM might be corrupted, please select Device->Quick reset of controller.\r\n"));
            hl_msg_show(error_msg);
        }
        goto l_HL_INIT_ERROR2;
    }

    hl_appl->zwnet = zw_init_ret.net;

#ifdef USE_SAFE_VERSION
    sprintf_s(net_id, 10, "%08X", zw_init_ret.net_id);
#else
    sprintf(net_id, "%08X", zw_init_ret.net_id);
#endif

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

    //Display menu accordingly
    hl_init_menu_set(hl_appl, 0);

    return;

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

    //Display menu accordingly
    hl_init_menu_set(hl_appl, 1);

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
//      zwnet_exit(hl_appl->zwnet,
//                 (hl_appl->save_ni_file)? hl_appl->node_info_file : NULL);
//      plt_mtx_destroy(hl_appl->desc_cont_mtx);
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


/**
_tWinMain - WIN32 API entry point
@param[in]	hInstance		Handle to current instance
@param[in]	hPrevInstance   Handle to previous instance
@param[in]	lpCmdLine       Command line parameters
@param[in]	nCmdShow        Flag to determine whether to show main window
@return
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    //UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_ZWSERIALPORTAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZWSERIALPORTAPI));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return(int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZWSERIALPORTAPI));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_ZWSERIALPORTAPI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }
    //Save a copy of main window handle
    ghWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_CREATE:
            {
                HFONT hfDefault;
                HWND hEdit;
                //Create edit window in the main windows to display messages
                hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
                                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                       0, 0, 100, 100, hWnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
                if (hEdit == NULL)
                    MessageBox(hWnd, _T("Could not create edit box."), _T("Error"), MB_OK | MB_ICONERROR);

                hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
                SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
                //Set text limit to max available
                //SendDlgItemMessage(ghWnd, IDC_MAIN_EDIT, EM_SETLIMITTEXT , (WPARAM)0, (LPARAM)0);
                SendMessage(hEdit, EM_SETLIMITTEXT, (WPARAM)0, (LPARAM)0);
            }
            break;

        case WM_SIZE:
            {
                HWND hEdit;
                RECT rcClient;

                GetClientRect(hWnd, &rcClient);

                hEdit = GetDlgItem(hWnd, IDC_MAIN_EDIT);
                SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
            }
            break;

        case WM_COMMAND:
            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;

                case IDM_GET_INFO:
                    hl_info_get(&my_appl);
#ifdef  INTERNAL_TEST
                    plt_msg_show(hl_plt_ctx_get(hl_appl), "__________Internal test result__________");
                    plt_msg_show(hl_plt_ctx_get(hl_appl), "Sec tx timeout count:%u", zwnet_get_tx_tmout_cnt(my_appl.zwnet));
#endif
                    break;

                case IDM_SET_DEFAULT:
                    hl_deflt_set(&my_appl);
                    break;

                case IDM_QUICK_RESET:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_RESET), hWnd, hl_reset_dlgbx);
                    if (my_appl.port_number == 0)
                        break;
                    hl_quick_reset(&my_appl);
                    break;

                case IDM_PROG_FLASH:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_FW_FILE), hWnd, hl_fw_file_dlgbx);
                    if (my_appl.save_file == NULL)
                        break;
                    hl_fw_updt(&my_appl);
                    free(my_appl.save_file);
                    my_appl.save_file = NULL;
                    break;

                case IDM_CANCEL_OP:
                    zwnet_abort(my_appl.zwnet);
                    break;

                case IDM_TEST:
#ifdef  ZW_STRESS_TEST
                    hl_stress_test(&my_appl);
#endif
                    break;

                case IDM_NI_TEST:
                    if (my_appl.is_ni_stress_tst_run == 0)
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_STRESS_TEST), hWnd, hl_stress_test_dlgbx);
                    }
                    hl_ni_stress_test(&my_appl);
                    break;


                case IDM_SET_LEARN_MODE:
                    hl_lrn_mod_set(&my_appl);
                    break;

                case IDM_ADD_NODE:
                    hl_nw_node_add(&my_appl);
                    break;

                case IDM_RM_NODE:
                    hl_nw_node_rm(&my_appl);
                    break;

                case IDM_CTLR_CHG:
                    hl_ctlr_chg(&my_appl);
                    break;

                case IDM_NW_UPDT:
                    hl_req_nw_updt(&my_appl);
                    break;

                case IDM_REQ_NODE_INFO:
                    hl_node_updt(&my_appl);
                    break;

                case IDM_NIF_SEND:
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_NIF_SEND), hWnd, hl_nif_send_dlgbx);
                        if (my_appl.broadcast == 0xFF)
                            break;
                        hl_nif_send(&my_appl);
                    }
                    break;

                case IDM_SEND_DATA:
                    hl_multi_lvl_snd(&my_appl);
                    break;

                case IDM_SHOW_TEST_RES:
#ifdef  ZW_STRESS_TEST
                    hl_test_rpt_show(&my_appl);
#endif
                    break;

                case IDM_SETTING:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_DEST_NODE), hWnd, hl_setting_dlgbx);
                    break;

                case IDM_RM_FAILED_ID://Remove failed node id
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_FAILED_NODE), hWnd, hl_failed_id_dlgbx);
                        if (my_appl.failed_node_id == 0)
                            break;
                        hl_failed_id_rm(&my_appl);
                    }
                    break;

                case IDM_RP_FAILED_NODE://Replace failed node id
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_FAILED_NODE), hWnd, hl_failed_id_dlgbx);
                        if (my_appl.failed_node_id == 0)
                            break;
                        hl_failed_id_rp(&my_appl);
                    }
                    break;


//              case IDM_SET_SUC_NODE://Set SUC
//                  {
//                      DialogBox(hInst, MAKEINTRESOURCE(IDD_SET_SUC), hWnd, hl_set_suc_dlgbx);
//                      if (my_appl.suc_node_id == 0)
//                          break;
//                      hl_suc_set(&my_appl);
//                  }
//                  break;

                case IDM_GET_NODES_INFO://Dump the internal nodes info in the memory
                    hl_nodes_info_dump(&my_appl);
                    break;

                case IDM_DISP_NODE_DESC://Display the node descriptor
                    hl_node_desc_dump(&my_appl);
                    break;

                case IDM_MULTI_LVL_REPORT://Setup multi-level switch report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_multi_lvl_rep_setup(&my_appl);
                    }
                    break;

                case IDM_MULTI_LVL_REP_GET://Get multi-level switch report from a node
                    hl_multi_lvl_rep_get(&my_appl);
                    break;

                case IDM_MULTI_LVL_SET://Set multi-level switch level
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_ML_SET), hWnd, hl_multi_lvl_set_dlgbx);
                        if (my_appl.mul_lvl_val == 0xFFFF)
                            break;
                        hl_multi_lvl_set(&my_appl);
                    }
                    break;

                case IDM_BIN_REPORT://Setup binary switch report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_bin_rep_setup(&my_appl);
                    }
                    break;

                case IDM_BIN_REP_GET://Get binary switch report from a node
                    hl_binary_rep_get(&my_appl);
                    break;

                case IDM_BIN_SET://Set binary switch
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_BIN_SET), hWnd, hl_bin_set_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_bin_set(&my_appl);
                    }
                    break;

                case IDM_CMD_Q_CTL_GET://Get commmand queuing control
                    {
                        hl_cmd_q_ctl_get(&my_appl, &my_appl.cmd_q_ctl);
                    }
                    break;

                case IDM_CMD_Q_CTL_SET://Set commmand queuing control
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_QUEUE_CTL), hWnd, hl_cmd_q_ctl_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;

                        hl_cmd_q_ctl_set(&my_appl);
                    }
                    break;

                case IDM_CMD_Q_VIEW://View commmand queue
                    {
                        hl_cmd_q_view(&my_appl);
                    }
                    break;

                case IDM_CMD_Q_RM://Cancel commands in command queue
                    {
                        hl_cmd_q_cancel(&my_appl);
                    }
                    break;

                case IDM_BIN_SNSR_REP://Setup binary sensor report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_bin_snsr_rep_setup(&my_appl);
                    }
                    break;

                case IDM_BIN_SNSR_GET://Get binary sensor report from a node
                    hl_bin_snsr_rep_get(&my_appl);
                    break;

                case IDM_ML_SNSR_REP://Setup multi-level sensor report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_ml_snsr_rep_setup(&my_appl);
                    }
                    break;

                case IDM_ML_SNSR_GET://Get multi-level sensor report from a node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_MLSNSR_GET), hWnd, hl_sensor_get_dlgbx);
                        if (my_appl.dst_desc_id == 0)
                            break;
                        hl_ml_snsr_rep_get(&my_appl);
                    }
                    break;

                case IDM_ML_SNSR_SUP_GET://Get the supported sensor types
                    hl_ml_snsr_sup_get(&my_appl);
                    break;

                case IDM_ML_SNSR_UNIT_GET://Get the supported sensor units
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_MLSNSR_UNIT_GET), hWnd, hl_sensor_unit_get_dlgbx);
                        if (my_appl.dst_desc_id == 0)
                            break;
                        hl_ml_snsr_unit_get(&my_appl);
                    }
                    break;

                case IDM_GRP_GET://Get group info from a node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_GROUP_GET), hWnd, hl_grp_get_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_grp_rep_get(&my_appl);
                    }
                    break;

                case IDM_GRP_ADD://Add endpoints into group
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_GROUP_ADD), hWnd, hl_grp_add_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_grp_add(&my_appl);
                    }
                    break;

                case IDM_GRP_DEL://Del endpoints from group
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_GROUP_DEL), hWnd, hl_grp_add_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_grp_del(&my_appl);
                    }
                    break;

                case IDM_GRP_SUP://Get max number of groupings
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_grp_sup(&my_appl);
                    }
                    break;

                case IDM_GRP_ATV://Get active group
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_grp_active(&my_appl);
                    }
                    break;

                case IDM_GRP_CMD_SUP://Get cmd records supported
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_grp_cmd_sup(&my_appl);
                    }
                    break;

                case IDM_GRP_CMD_SET://Set cmd to node in group
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_GROUP_CMD_SET), hWnd, hl_grp_cmd_set_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_grp_cmd_set(&my_appl);
                    }
                    break;

                case IDM_GRP_CMD_GET://Get command record for a node within a given grouping identifier
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_GROUP_CMD_GET), hWnd, hl_grp_cmd_get_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_grp_cmd_get(&my_appl);
                    }
                    break;

                case IDM_NAMELOC_REP_SET://Set name location to a node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_SET_NAMELOC), hWnd, hl_nameloc_set_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_nameloc_set(&my_appl);
                    }
                    break;

                case IDM_MULTI_LVL_CHANGE://Start/Stop changing multi level switch level
                    {
                        if (my_appl.mul_lvl_change_started == 0)
                        {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_ML_START_CHANGE), hWnd, hl_multi_lvl_chg_dlgbx);
                            if (my_appl.temp_desc == 0)
                                break;
                        }
                        hl_multi_lvl_chg(&my_appl);
                    }
                    break;

                case IDM_MULTI_LVL_SUP://Get switch type
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_multi_lvl_sup(&my_appl);
                    }
                    break;

                case IDM_WKUP_GET://Get wake up setting
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_wkup_get(&my_appl);
                    }
                    break;

                case IDM_WKUP_SET://Set wake up interval and alert receiving node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_WKUP_SET), hWnd, hl_wkup_set_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_wkup_set(&my_appl);
                    }
                    break;

                case IDM_CFG_SET://Set configuration parameter
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CFG_SET), hWnd, hl_cfg_set_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_cfg_set(&my_appl);
                    }
                    break;

                case IDM_CFG_REP_SETUP://Setup a configuration parameter report callback function
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_cfg_rep_setup(&my_appl);
                    }
                    break;

                case IDM_CFG_GET://Get configuration parameter
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CFG_GET), hWnd, hl_cfg_get_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;
                        hl_cfg_get(&my_appl);
                    }
                    break;

                case IDM_PULSE_REPORT://Setup pulse meter report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_pulsemeter_rep_setup(&my_appl);
                    }
                    break;

                case IDM_PULSE_REP_GET://Get pulse meter report from a node
                    hl_pulsemeter_rep_get(&my_appl);
                    break;

                case IDM_METER_REPORT://Setup meter report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_meter_rep_setup(&my_appl);
                    }
                    break;

                case IDM_METER_REP_GET://Get meter report from a node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_METER_GET), hWnd, hl_meter_get_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_meter_rep_get(&my_appl);
                    }
                    break;

                case IDM_METER_SUP_GET://Get meter capabilities from a node
                    hl_meter_sup(&my_appl);
                    break;

                case IDM_METER_RESET://Reset meter accumulated value
                    hl_meter_reset(&my_appl);
                    break;

                case IDM_METER_ADMIN_SET://Set meter admin name
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_SET_ADMIN), hWnd, hl_admin_set_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_meter_admin_set(&my_appl);
                    }
                    break;

                case IDM_METER_CAP_GET://Get meter admin name and capability
                    hl_meter_admin_cap_get(&my_appl);
                    break;

                case IDM_AV_SET://Send AV button message
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_SET_AV), hWnd, hl_av_set_dlgbx);
                    }
                    break;

                case IDM_AV_GET://Get supported AV commands
                    hl_av_get(&my_appl);
                    break;

                case IDM_START://Init
                    if (my_appl.is_init_done == 0)
                    {
                        memset(&my_appl, 0, sizeof(hl_appl_ctx_t));
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_INIT), hWnd, hl_init_dlgbx);
                        if (my_appl.port_number == 0)
                            break;
                        SendDlgItemMessage(hWnd, IDC_MAIN_EDIT, WM_SETTEXT, 0, (LPARAM)_T("Initializing ...\r\n"));
                        hl_init(&my_appl);
                    }
                    else
                    {
                        hl_close(&my_appl);
                    }
                    break;

                case IDM_SAVE://Save settings
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVE), hWnd, hl_save_dlgbx);
                        if (my_appl.save_file == NULL)
                            break;
                        hl_save(&my_appl);
                        free(my_appl.save_file);
                        my_appl.save_file = NULL;
                    }
                    break;

                case IDM_BASIC_REPORT://Setup basic command report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_basic_rep_setup(&my_appl);
                    }
                    break;

                case IDM_BASIC_REP_GET://Get basic command report
                    hl_basic_rep_get(&my_appl);
                    break;

                case IDM_BASIC_SET://Set basic command
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_BASIC_SET), hWnd, hl_basic_set_dlgbx);
                        if (my_appl.basic_val == 0xFFFF)
                            break;
                        hl_basic_set(&my_appl);
                    }
                    break;

                case IDM_DLCK_OP_REPORT://Setup door lock operation status report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_dlck_op_rep_setup(&my_appl);
                    }
                    break;

                case IDM_DLCK_OP_REP_GET://Get the state of the door lock device
                    hl_dlck_op_rep_get(&my_appl);
                    break;

                case IDM_DLCK_OP_SET://Set door lock operation
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_SET_DLCK_OP), hWnd, hl_set_dlck_mode_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_dlck_op_set(&my_appl);
                    }
                    break;

                case IDM_DLCK_CFG_GET://Get configuration parameter
                    hl_dlck_cfg_get(&my_appl);
                    break;

                case IDM_DLCK_CFG_SET://Set the configuration of the door lock device
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_DLCK_CFG), hWnd, hl_dlck_cfg_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_dlck_cfg_set(&my_appl);
                    }
                    break;

                case IDM_USRCOD_GET://Get user code
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_USRCOD_GET), hWnd, hl_get_usrcod_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_usrcod_get(&my_appl);
                    }
                    break;

                case IDM_USRCOD_SET://Set user code
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_USRCOD_SET), hWnd, hl_set_usrcod_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_usrcod_set(&my_appl);
                    }
                    break;

                case IDM_USRCOD_SUP_GET://Get number of supported user codes
                    hl_usrcod_sup_get(&my_appl);
                    break;

                case IDM_ALRM_REPORT://Setup alarm status report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_alrm_rep_setup(&my_appl);
                    }
                    break;

                case IDM_ALRM_REP_GET://Get the state of the alarm device
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ALRM_GET), hWnd, hl_get_alrm_dlgbx);
                    if (my_appl.rep_desc_id == 0)
                        break;
                    hl_alrm_rep_get(&my_appl);
                    break;

                case IDM_ALRM_SET://Set the activity of the Z-Wave Alarm Type
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_ALRM_SET), hWnd, hl_set_alrm_dlgbx);
                        if (my_appl.rep_desc_id == 0 || my_appl.alrm_ztype == 0)
                            break;
                        hl_alrm_set(&my_appl);
                    }
                    break;

                case IDM_ALRM_SUP_GET://Get the supported alarm types
                    hl_alrm_sup_get(&my_appl);
                    break;

                case IDM_ALRM_SUP_EVT_GET://Get the supported event
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ALRM_EVT_GET), hWnd, hl_get_alrm_evt_dlgbx);
                    if (my_appl.rep_desc_id == 0)
                        break;
                    hl_alrm_sup_evt_get(&my_appl);
                    break;

                case IDM_BATTERY_REPORT://Setup battery command report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_battery_rep_setup(&my_appl);
                    }
                    break;

                case IDM_BATTERY_REP_GET://Get battery command report
                    hl_battery_rep_get(&my_appl);
                    break;

                case IDM_THR_FAN_MD_REPORT://Setup thermostat fan mode report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_fan_md_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_FAN_MD_REP_GET://Get thermostat fan mode
                    hl_thrmo_fan_md_rep_get(&my_appl);
                    break;

                case IDM_THR_FAN_MD_SUP_GET://Get supported thermostat fan modes
                    hl_thrmo_fan_md_sup_get(&my_appl);
                    break;

                case IDM_THR_FAN_MD_SET://Set thermostat fan mode
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_THER_FAN_MOD_SET), hWnd, hl_set_thrmo_fan_md_dlgbx);
                        if (my_appl.thrmo_fan_md == 0xFF)
                            break;
                        hl_thrmo_fan_md_set(&my_appl);
                    }
                    break;

                case IDM_THR_FAN_STA_REPORT://Setup thermostat fan state report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_fan_sta_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_FAN_STA_REP_GET://Get thermostat fan state
                    hl_thrmo_fan_sta_rep_get(&my_appl);
                    break;

                case IDM_THR_MD_REPORT://Setup thermostat mode report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_md_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_MD_REP_GET://Get thermostat mode
                    hl_thrmo_md_rep_get(&my_appl);
                    break;

                case IDM_THR_MD_SUP_GET://Get supported thermostat modes
                    hl_thrmo_md_sup_get(&my_appl);
                    break;

                case IDM_THR_MD_SET://Set thermostat mode
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_THER_MOD_SET), hWnd, hl_set_thrmo_md_dlgbx);
                        if (my_appl.thrmo_md == 0xFF)
                            break;
                        hl_thrmo_md_set(&my_appl);
                    }
                    break;

                case IDM_THR_OP_STA_REPORT://Setup thermostat operating state report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_op_sta_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_OP_STA_REP_GET://Get thermostat operating state
                    hl_thrmo_op_sta_rep_get(&my_appl);
                    break;

                case IDM_THR_SETB_REPORT://Setup thermostat setback state report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_setb_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_SETB_REP_GET://Get thermostat setback state
                    hl_thrmo_setb_rep_get(&my_appl);
                    break;

                case IDM_THR_SETB_SET://Set thermostat setback state
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_THER_SETB_SET), hWnd, hl_set_thrmo_setb_sta_dlgbx);
                        if (my_appl.thrmo_setb_sta == 0xFF)
                            break;
                        hl_thrmo_setb_set(&my_appl);
                    }
                    break;

                case IDM_THR_SETP_REPORT://Setup thermostat setpoint report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_thrmo_setp_rep_setup(&my_appl);
                    }
                    break;

                case IDM_THR_SETP_REP_GET://Get thermostat setpoint
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_THER_SETP_GET), hWnd, hl_get_thrmo_setp_dlgbx);
                        if (my_appl.thrmo_setp_typ == 0xFF)
                            break;
                        hl_thrmo_setp_rep_get(&my_appl);
                    }
                    break;

                case IDM_THR_SETP_SUP_GET://Get supported thermostat setpoint types
                    hl_thrmo_setp_sup_get(&my_appl);
                    break;

                case IDM_THR_SETP_SET://Set thermostat setpoint
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_THER_SETP_SET), hWnd, hl_set_thrmo_setp_dlgbx);
                        if (my_appl.thrmo_setp_data.type == 0xFF)
                            break;
                        hl_thrmo_setp_set(&my_appl);
                    }
                    break;

                case IDM_CLOCK_REPORT://Setup clock command report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_clock_rep_setup(&my_appl);
                    }
                    break;

                case IDM_CLOCK_REP_GET://Get clock command report
                    hl_clock_rep_get(&my_appl);
                    break;

                case IDM_CLOCK_SET://Set clock command
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CLOCK_SET), hWnd, hl_clock_set_dlgbx);
                        if (my_appl.weekday == 0xFF)
                            break;
                        hl_clock_set(&my_appl);
                    }
                    break;

                case IDM_CC_SCHED_REPORT://Setup climate control schedule report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_clmt_ctl_schd_rep_setup(&my_appl);
                    }
                    break;

                case IDM_CC_SCHED_REP_GET://Get climate control schedule
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CLMT_CTL_SCHED_GET), hWnd, hl_clmt_ctl_schd_get_dlgbx);
                        if (my_appl.weekday == 0xFF)
                            break;
                        hl_clmt_ctl_schd_rep_get(&my_appl);
                    }
                    break;

                case IDM_CC_SCHED_SET://Set climate control schedule
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CLMT_CTL_SCHED_SET), hWnd, hl_clmt_ctl_schd_set_dlgbx);
                        if (my_appl.clmt_ctl_schd.weekday == 0xFF)
                            break;
                        hl_clmt_ctl_schd_set(&my_appl);
                    }
                    break;

                case IDM_CC_SCHED_CHG_REP_GET://Get climate control schedule change counter
                    hl_clmt_ctl_schd_chg_rep_get(&my_appl);
                    break;

                case IDM_CC_SCHED_OVR_REP_GET://Get climate control schedule override
                    hl_clmt_ctl_schd_ovr_rep_get(&my_appl);
                    break;

                case IDM_CC_SCHED_OVR_SET://Set climate control schedule override
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CC_SCHED_OVR_SET), hWnd, hl_set_clmt_ctl_schd_ovr_dlgbx);
                        if (my_appl.clmt_ctl_schd_ovr.state == 0xFF)
                            break;
                        hl_clmt_ctl_schd_ovr_set(&my_appl);
                    }
                    break;

                case IDM_MUL_CMD_ENC_CTL_SET://Start or stop multi command encapsulation
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_MUL_CMD_CTL), hWnd, hl_mul_cmd_ctl_dlgbx);
                        if (my_appl.temp_desc == 0)
                            break;

                        hl_mul_cmd_ctl_set(&my_appl);
                    }
                    break;

                case IDM_MUL_CMD_ENC_CTL_GET://Get multi command encapsulation state
                    {
                        hl_mul_cmd_ctl_get(&my_appl, &my_appl.mul_cmd_ctl);
                    }
                    break;

                case IDM_PROT_REPORT://Setup protection states report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_prot_rep_setup(&my_appl);
                    }
                    break;

                case IDM_PROT_REP_GET://Get protection states
                    hl_prot_rep_get(&my_appl);
                    break;

                case IDM_PROT_SUP_GET://Get supported protection states types
                    hl_prot_sup_get(&my_appl);
                    break;

                case IDM_PROT_SET://Set protection states
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_PROT_SET), hWnd, hl_prot_set_dlgbx);
                        if (my_appl.local_prot == 0xFF)
                            break;
                        hl_prot_set(&my_appl);
                    }
                    break;

                case IDM_PROT_EC_REP_GET://Get protection exclusive control node
                    hl_prot_ec_rep_get(&my_appl);
                    break;

                case IDM_PROT_EC_SET://Set protection exclusive control node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_PROT_EC_SET), hWnd, hl_prot_ec_set_dlgbx);
                        if (my_appl.node_id == 0xFF)
                            break;
                        hl_prot_ec_set(&my_appl);
                    }
                    break;

                case IDM_PROT_TMOUT_REP_GET://Get protection timeout
                    hl_prot_tmout_rep_get(&my_appl);
                    break;

                case IDM_PROT_TMOUT_SET://Set protection timeout
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_PROT_TMOUT_SET), hWnd, hl_prot_tmout_set_dlgbx);
                        if (my_appl.rep_desc_id == 0xFF)
                            break;
                        hl_prot_tmout_set(&my_appl);
                    }
                    break;

                case IDM_APPL_REPORT://Setup application status report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_appl_sts_rep_setup(&my_appl);
                    }
                    break;

                case IDM_IND_REPORT://Setup indicator report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_ind_rep_setup(&my_appl);
                    }
                    break;

                case IDM_IND_REP_GET://Get indicator report
                    hl_ind_rep_get(&my_appl);
                    break;

                case IDM_IND_SET://Set indicator
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_IND_SET), hWnd, hl_ind_set_dlgbx);
                        if (my_appl.ind_val == 0xFFFF)
                            break;
                        hl_ind_set(&my_appl);
                    }
                    break;

                case IDM_FW_INFO_GET://Get firmwares information
                    hl_fw_info_get(&my_appl);
                    break;

                case IDM_FW_UPDT_REQ://Request firmware update
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_FW_UPDT_REQ), hWnd, hl_fw_updt_dlgbx);
                        if (my_appl.save_file == NULL)
                            break;
                        hl_fw_updt_req(&my_appl);
                    }
                    break;

                case IDM_POLL_CTL://Start or stop entry of polling commands
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_POLL_CTL), hWnd, hl_poll_ctl_dlgbx);
                    }
                    break;

                case IDM_RM_POLL://Remove polling requests
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_POLL_RM), hWnd, hl_poll_rm_dlgbx);
                        if (my_appl.dlg_ok)
                        {
                            hl_poll_rm(&my_appl);
                        }
                    }
                    break;

				case IDM_POWERLVL_REPORT://Setup power level report
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_REPORT), hWnd, hl_rep_setup_dlgbx);
                        if (my_appl.rep_desc_id == 0)
                            break;
                        hl_power_level_rep_setup(&my_appl);
                    }
                    break;

                case IDM_POWERLVL_REP_GET://Get the power level
                    hl_power_level_rep_get(&my_appl);
                    break;

				case IDM_POWERLVL_SET://Set the power level
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_POWER_LEVEL_SET), hWnd, hl_set_power_level_dlgbx);
                        if (my_appl.pow_lvl_timeout == 0)
                            break;
                        hl_power_level_set(&my_appl);
                    }
                    break;

                case IDM_POWERLVL_TESTNODE_REP_GET://Get the power level test node
                    hl_power_level_test_rep_get(&my_appl);
                    break;

				case IDM_POWERLVL_TESTNODE_SET://Set the power level test node
                    {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_POWER_LEVEL_TESTNODE_SET), hWnd, hl_set_power_level_test_dlgbx);
                        if (my_appl.pow_lvl_test_node_desc_id == 0 || my_appl.pow_lvl_test_wframcnt == 0)
                            break;
                        hl_power_level_test_set(&my_appl);
                    }
                    break;

                case IDM_EXIT:
                    //hl_exit(&my_appl);
                    DestroyWindow(hWnd);
                    break;

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code here...
            EndPaint(hWnd, &ps);
            break;

        case MSG_ZWAPI_NOTIFY:
            {
                nw_notify_msg_t  *notify_msg = (nw_notify_msg_t *)lParam;
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
                    break;
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
                                plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_desc_init with error:%d", result);
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
            break;

        case WM_DESTROY:
            hl_exit(&my_appl);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



/**
@}
*/




