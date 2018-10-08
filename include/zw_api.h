/**
@mainpage	Z-Wave API for controllers
@author		sasi
@date		2010-11-2
@version	1.3
*/

#ifndef _ZWAVE_API_SASI_
#define _ZWAVE_API_SASI_

#define     ZW_CONTROLLER           //For controller
#define     REPLACE_FAILED          //For failed node replacement

#include "zw_hci_application.h"
#include "zwave/ZW_controller_api.h"
#include "zwave/ZW_classcmd.h"


//--------------- testing control ---------------------------------
//#define  ZW_STRESS_TEST             //Support stress test
//#define  TEST_EXT_CMD_CLASS
//#define  TEST_HANDLE_BASIC_COMMAND_CLASS
//#define  TEST_WRITE_SERIAL_NUM
//#define  TEST_SEC_WAKE_UP
//#define  TEST_UTF16_NAME_LOC
//#define  TEST_UTF8_NAME_LOC
//#define  TEST_ALARM_V2
//#define INTERNAL_TEST
#define SEND_NO_MORE_INFO_AFTER_DATA


//-----------------------------------------------------------------
#define  ZW_PLUS            //To support Z-wave+


#define  CRC16_ENCAP        //To support CRC-16 encapsulation
#define  MANF_SPEC_V2       //To support manufacturer specific version 2

#define  SUPPORT_SECURITY   //To enable support of security layer
#define  SUPPORT_ASSOCIATION_RESETLOC	 //To support Association CC, AGI CC and reset locally CC

#define DEBUG_ZWAPI         //Print debug messages

#ifdef  DEBUG_ZWAPI
#define     debug_zwapi_msg         plt_msg_show
#define     debug_zwapi_ts_msg      plt_msg_ts_show
#define     debug_zwapi_bin_msg     plt_bin_show
#else
#define     debug_zwapi_msg(...)
#define     debug_zwapi_ts_msg(...)
#define     debug_zwapi_bin_msg(...)
#endif


#ifdef  ZW_STRESS_TEST
#define  MANF_ID_1      0    //high-byte of manufacturer id (Zensys) for testing
#define  MANF_ID_2      0    //low-byte of manufacturer id (Zensys) for testing
#endif

#define APPL_VERSION        3   //application/uzbctl API version
#define APPL_SUBVERSION     23  //application/uzbctl API sub-version

#ifndef _ZWAVE_API_PTE_SASI_
//#error Please enable opaque network handle.
typedef void	*zwnet_p;		/**< opaque network handle */
#endif

#define SERIAL_NUMBER_OFFSET        0x33    //offset of non-volatile memory tag for serial number

#define ZW_DEV_LOC_STR_MAX	16  //Actual device name & location maximum string length
#define ZW_LOC_STR_MAX	    32  //Endpoint name & location maximum string length

/* forward references */

struct  _zwnoded;
struct  _zwepd;
struct  _zwifd;
typedef struct _zwnoded	    *zwnoded_p;		/**< node descriptor */
typedef struct _zwepd	    *zwepd_p;		/**< endpoint descriptor */
typedef struct _zwifd	    *zwifd_p;		/**< interface descriptor */


/** Z-Wave error code */
#define ZW_ERR_QUEUED			  1	   /**< The command is queued */
#define ZW_ERR_NONE				  0    /**< Operation succeeded */
#define ZW_ERR_FAILED			 -1	   /**< Operation failed */
#define ZW_ERR_WRONG_IF         -10    /**< Wrong interface */
#define ZW_ERR_NO_RESP          -11    /**< No response from controller */
#define ZW_ERR_MEMORY           -12    /**< Out of memory */
#define ZW_ERR_NODE_NOT_FOUND   -13    /**< Node not found*/
#define ZW_ERR_CLASS_NOT_FOUND  -14    /**< Command Class not found*/
#define ZW_ERR_INTF_NOT_FOUND   -15    /**< Interface to a class not found*/
#define ZW_ERR_INTF_NO_REP_HDLR -16    /**< Interface report handler not found*/
#define ZW_ERR_LAST_OP_NOT_DONE -17    /**< Last operation uncompleted yet, try again*/
#define ZW_ERR_NOT_IN_LIST      -18    /**< Node not in protocol layer failed node ID list*/
#define ZW_ERR_OP_FAILED        -19    /**< The requested operation failed*/
#define ZW_ERR_EP_NOT_FOUND     -20    /**< Endpoint not found*/
#define ZW_ERR_RPT_NOT_FOUND    -21    /**< The report command of an interface not found*/
#define ZW_ERR_NET_NOT_FOUND    -22    /**< Network not found*/
#define ZW_ERR_CMD_VERSION      -23    /**< Incorrect command class version*/
#define ZW_ERR_PENDING			-24	   /**< Operation pending, it cannot be canceled now */
#define ZW_ERR_VALUE			-25	   /**< The value is invalid */
#define ZW_ERR_QUEUE_FULL		-26	   /**< The queue is full */
#define ZW_ERR_UNSUPPORTED		-27	   /**< The requested function is unsupported for this node */
#define ZW_ERR_FILE_OPEN	    -28	   /**< Open file error */
#define ZW_ERR_FILE_WRITE	    -29	   /**< Write file error */
#define ZW_ERR_FILE_EOF	        -30	   /**< The end-of-file was reached*/
#define ZW_ERR_FILE	            -31	   /**< File is corrupted */
#define ZW_ERR_FILE_HOME_ID     -32	   /**< File home id doesn't match */
#define ZW_ERR_EXPIRED          -33    /**< Expired */
#define ZW_ERR_NO_RES           -34    /**< No resource for mutex, semaphore, timer, etc */
#define ZW_ERR_EVENT            -35    /**< Event is not processed */
#define ZW_ERR_TOO_LARGE        -36    /**< Data size is too large */
#define ZW_ERR_TOO_SMALL        -37    /**< Data size is too small */
#define ZW_ERR_TIMEOUT          -38    /**< Timeout */
#define ZW_ERR_TRANSMIT         -39    /**< Transmission failed */
#define ZW_ERR_NONCE_NOT_FOUND  -40    /**< Security nonce not found */
#define ZW_ERR_AUTH             -41    /**< Authentication error */
#define ZW_ERR_SEQ_NUMBER       -42    /**< Incorrect sequence number */
#define ZW_ERR_BUSY             -43    /**< Busy, try again later */
#define ZW_ERR_SEC_SCHEME       -44    /**< Security scheme unsupported */
#define ZW_ERR_TRANSPORT_INI    -45    /**< Initialization error on transport layer */
#define ZW_ERR_FRAME_INI        -46    /**< Initialization error on frame layer */
#define ZW_ERR_SESSION_INI      -47    /**< Initialization error on session layer */
#define ZW_ERR_APPL_INI         -48    /**< Initialization error on application layer */
#define ZW_ERR_UNEXPECTED       -49    /**< The error was unexpected under normal circumstances*/
#define ZW_ERR_DISALLOWED       -50    /**< The operation is disallowed under certain circumstances*/
#define ZW_ERR_VERSION          -51    /**< Wrong version number*/
#define ZW_ERR_INTF_NO_DATA     -52    /**< Interface data is missing*/
#define ZW_ERR_FILE_READ	    -53	   /**< Read file error */
#define ZW_ERR_COMM	            -54	   /**< Communication error with controller*/


/** Z-Wave interface command id for command queuing */
#define ZW_CID_BSENSOR_RPT_GET	  1	   /**< zwif_bsensor_get */
#define ZW_CID_CONFIG_RPT_GET	  2	   /**< zwif_config_get */
#define ZW_CID_CONFIG_SET	      3	   /**< zwif_config_set */
#define ZW_CID_GRP_ACTV_GET	      4	   /**< zwif_group_actv_get */
#define ZW_CID_GRP_ADD	          5	   /**< zwif_group_add */
#define ZW_CID_GRP_CMD_GET	      6	   /**< zwif_group_cmd_get */
#define ZW_CID_GRP_CMD_SET	      7	   /**< zwif_group_cmd_set */
#define ZW_CID_GRP_CMD_SUP_GET    8	   /**< zwif_group_cmd_sup_get */
#define ZW_CID_GRP_DEL	          9	   /**< zwif_group_del */
#define ZW_CID_GRP_GET	         10	   /**< zwif_group_get */
#define ZW_CID_GRP_SUP_GET       11	   /**< zwif_group_sup_get */
#define ZW_CID_LEVEL_RPT_GET	 12	   /**< zwif_level_get */
#define ZW_CID_LEVEL_SET	     13	   /**< zwif_level_set */
#define ZW_CID_LEVEL_START	     14	   /**< zwif_level_start */
#define ZW_CID_LEVEL_STOP	     15	   /**< zwif_level_stop */
#define ZW_CID_LEVEL_SUP_GET     16	   /**< zwif_level_sup_get */
#define ZW_CID_NAME_LOC_SET	     17	   /**< zwif_nameloc_set */
#define ZW_CID_SENSOR_RPT_GET	 18	   /**< zwif_sensor_get */
#define ZW_CID_SWITCH_RPT_GET	 19	   /**< zwif_switch_get */
#define ZW_CID_SWITCH_SET	     20	   /**< zwif_switch_set */
#define ZW_CID_WAKE_UP_GET       21	   /**< zwif_wakeup_get */
#define ZW_CID_WAKE_UP_SET	     22	   /**< zwif_wakeup_set */
#define ZW_CID_METER_DESC_GET    23	   /**< zwif_meter_get_desc */
#define ZW_CID_METER_RESET       24	   /**< zwif_meter_reset */
#define ZW_CID_METER_RPT_GET     25	   /**< zwif_meter_get */
#define ZW_CID_METER_SUP_GET     26	   /**< zwif_meter_sup_get */
#define ZW_CID_METER_ADMIN_SET   27	   /**< zwif_meter_set_admin */
#define ZW_CID_AV_CAP_GET        28	   /**< zwif_av_caps */
#define ZW_CID_AV_SET            29	   /**< zwif_av_set */
#define ZW_CID_PULSE_METER_RPT_GET     30	   /**< zwif_pulsemeter_get */
#define ZW_CID_BASIC_RPT_GET	 31	   /**< zwif_level_get */
#define ZW_CID_BASIC_SET	     32	   /**< zwif_level_set */
#define ZW_CID_DL_OP_SET	     33	   /**< zwif_dlck_op_set */
#define ZW_CID_DL_OP_RPT_GET	 34	   /**< zwif_dlck_op_get */
#define ZW_CID_DL_CFG_SET	     35	   /**< zwif_dlck_cfg_set */
#define ZW_CID_DL_CFG_RPT_GET	 36	   /**< zwif_dlck_cfg_get */
#define ZW_CID_USRCOD_SET	     37	   /**< zwif_usrcod_set */
#define ZW_CID_USRCOD_GET	     38	   /**< zwif_usrcod_get */
#define ZW_CID_USRCOD_SUP_GET    39	   /**< zwif_usrcod_sup_get */
#define ZW_CID_ALRM_RPT_GET	     40	   /**< zwif_alrm_get */
#define ZW_CID_ALRM_SET	         41	   /**< zwif_alrm_set */
#define ZW_CID_ALRM_SUP_GET      42	   /**< zwif_alrm_sup_get */
#define ZW_CID_BATTERY_RPT_GET	 43	   /**< zwif_battery_get */
#define ZW_CID_THRMO_FAN_MD_GET	 44	   /**< zwif_thrmo_fan_md_get */
#define ZW_CID_THRMO_FAN_MD_SET	 45	   /**< zwif_thrmo_fan_md_set */
#define ZW_CID_THRMO_FAN_MD_SUP_GET      46	   /**< zwif_thrmo_fan_md_sup_get */
#define ZW_CID_THRMO_FAN_STA_GET 47	   /**< zwif_thrmo_fan_sta_get */
#define ZW_CID_THRMO_MD_GET	     48	   /**< zwif_thrmo_md_get */
#define ZW_CID_THRMO_MD_SET	     49	   /**< zwif_thrmo_md_set */
#define ZW_CID_THRMO_MD_SUP_GET  50	   /**< zwif_thrmo_md_sup_get */
#define ZW_CID_THRMO_OP_STA_GET  51	   /**< zwif_thrmo_op_sta_get */
#define ZW_CID_THRMO_SETB_GET	 52	   /**< zwif_thrmo_setb_get */
#define ZW_CID_THRMO_SETB_SET	 53	   /**< zwif_thrmo_setb_set */
#define ZW_CID_THRMO_SETP_GET	 54	   /**< zwif_thrmo_setp_get */
#define ZW_CID_THRMO_SETP_SET	 55	   /**< zwif_thrmo_setp_set */
#define ZW_CID_THRMO_SETP_SUP_GET       56	   /**< zwif_thrmo_setp_sup_get */
#define ZW_CID_CLOCK_RPT_GET	 57	   /**< zwif_clock_get */
#define ZW_CID_CLOCK_SET	     58	   /**< zwif_clock_set */
#define ZW_CID_CLMT_CTL_SCHD_GET 59	   /**< zwif_clmt_ctl_schd_get */
#define ZW_CID_CLMT_CTL_SCHD_SET 60	   /**< zwif_clmt_ctl_schd_set */
#define ZW_CID_CLMT_CTL_SCHD_CHG_GET    61	   /**< zwif_clmt_ctl_schd_chg_get */
#define ZW_CID_CLMT_CTL_SCHD_OVR_GET    62	   /**< zwif_clmt_ctl_schd_ovr_get */
#define ZW_CID_CLMT_CTL_SCHD_OVR_SET    63	   /**< zwif_clmt_ctl_schd_ovr_set */
#define ZW_CID_WKUP_NO_INFO      64	   /**< wake up no more info (for internal use)*/
#define ZW_CID_PROT_GET	         65	   /**< zwif_prot_get */
#define ZW_CID_PROT_SET	         66	   /**< zwif_prot_set */
#define ZW_CID_PROT_SUP_GET      67	   /**< zwif_prot_sup_get */
#define ZW_CID_PROT_EC_GET	     68	   /**< zwif_prot_ec_get */
#define ZW_CID_PROT_EC_SET	     69	   /**< zwif_prot_ec_set */
#define ZW_CID_PROT_TIMEOUT_GET	 70	   /**< zwif_prot_tmout_get */
#define ZW_CID_PROT_TIMEOUT_SET	 71	   /**< zwif_prot_tmout_set */
#define ZW_CID_INDICATOR_GET	 72	   /**< zwif_ind_get */
#define ZW_CID_INDICATOR_SET	 73	   /**< zwif_ind_set */
#define ZW_CID_SENSOR_SUP_GET    74	   /**< zwif_sensor_sup_get */
#define ZW_CID_SENSOR_UNIT_GET   75	   /**< zwif_sensor_unit_get */
#define ZW_CID_FW_INFO_GET       76	   /**< zwif_fw_info_get */
#define ZW_CID_FW_UPDT_GET       77	   /**< zwif_fw_updt_req */
#define ZW_CID_POWER_LVL_GET	 78	   /**< zwif_power_level_get */
#define ZW_CID_POWER_LVL_TST_GET 79	   /**< zwif_power_level_test_get */
#define ZW_CID_POWER_LVL_SET     80	   /**< zwif_power_level_set */
#define ZW_CID_POWER_LVL_TST_SET 81	   /**< zwif_power_level_test_set */
#define ZW_CID_ALRM_SUP_EVT_GET  82	   /**< zwif_alrm_sup_evt_get */
#define ZW_CID_DRLOG_REC_SUP_GET 83	   /**< zwif_drlog_rec_sup_get */
#define ZW_CID_DRLOG_REC_GET     84	   /**< zwif_drlog_rec_get */

#define ZW_CID_POLL              0xFFFF /**< The command is meant fo polling purposes only */

/** Device Categories definition */
#define DEV_SENSOR_ALARM            1	   /**< Sensor alarm */
#define DEV_ON_OFF_SWITCH           2	   /**< On/off switch */
#define DEV_POWER_STRIP             3	   /**< Power strip */
#define DEV_SIREN                   4	   /**< Siren */
#define DEV_VALVE                   5	   /**< Valve */
#define DEV_SIMPLE_DISPLAY          6	   /**< Simple display */
#define DEV_DOORLOCK_KEYPAD         7	   /**< Door lock with keypad */
#define DEV_SUB_ENERGY_METER        8	   /**< Sub energy meter */
#define DEV_ADV_WHL_HOME_ENER_METER 9	   /**< Advanced whole home energy meter */
#define DEV_SIM_WHL_HOME_ENER_METER 10	   /**< Simple whole home energy meter */
#define DEV_SENSOR                  11	   /**< Sensor */
#define DEV_LIGHT_DIMMER            12	   /**< Light dimmer switch */
#define DEV_WIN_COVERING_NO_POS     13	   /**< Window covering no position/endpoint */
#define DEV_WIN_COVERING_EP         14	   /**< Window covering end point aware */
#define DEV_WIN_COVERING_POS_EP     15	   /**< Window covering position/end point aware */
#define DEV_FAN_SWITCH              16	   /**< Fan switch */
#define DEV_RMT_CTL_MULTIPURPOSE    17	   /**< Remote control - multipurpose */
#define DEV_RMT_CTL_AV              18	   /**< Remote control - AV */
#define DEV_RMT_CTL_SIMPLE          19	   /**< Remote control - simple */
#define DEV_UNRECOG_GATEWAY         20	   /**< Gateway (unrecognized by client) */
#define DEV_CENTRAL_CTLR            21	   /**< Central controller */
#define DEV_SET_TOP_BOX             22	   /**< Set top box */
#define DEV_TV                      23	   /**< TV */
#define DEV_SUB_SYS_CTLR            24	   /**< Sub system controller */
#define DEV_GATEWAY                 25	   /**< Gateway */
#define DEV_THERMOSTAT_HVAC         26	   /**< Thermostat - HVAC */
#define DEV_THERMOSTAT_SETBACK      27	   /**< Thermostat - setback */
#define DEV_WALL_CTLR               28	   /**< Wall controller */


/**
@defgroup Net Network APIs
A Z-Wave Network is seen through a local controller node which can manage
control and data in the mesh network.
@{
*/

/** network operations */
#define ZWNET_OP_NONE           0   /**< No operation is executing*/
#define ZWNET_OP_INITIALIZE     1   /**< Initialization operation*/
#define ZWNET_OP_ADD_NODE       2   /**< Add node operation*/
#define ZWNET_OP_RM_NODE        3   /**< Remove node operation*/
#define ZWNET_OP_RP_NODE        4   /**< Replace failed node operation*/
#define ZWNET_OP_RM_FAILED_ID   5   /**< Remove failed node id operation*/
#define ZWNET_OP_INITIATE       6   /**< Initiation operation by controller*/
#define ZWNET_OP_UPDATE         7   /**< Update network topology from the SUC/SIS*/
#define ZWNET_OP_RESET          8   /**< Restore to factory default setting*/
#define ZWNET_OP_MIGRATE_SUC    9   /**< Create primary controller by a SUC*/
#define ZWNET_OP_MIGRATE        10  /**< Migrate primary controller operation*/
#define ZWNET_OP_ASSIGN         11  /**< assign or deassign SUC/SIS operation*/
#define ZWNET_OP_SEND_DATA      12  /**< Send data operation*/
#define ZWNET_OP_NODE_UPDATE    13  /**< Update node info*/
#define ZWNET_OP_SEND_NIF       14  /**< Send node info frame*/
#define ZWNET_OP_FW_UPDATE      15  /**< Local firmware update. This operation cannot be cancelled*/

/** Final progress status for all operations */
#define     OP_DONE                     0       /**< Operation completed*/
#define     OP_FAILED                   0x00FF  /**< Operation failed*/

/** Progress status of all getting node information */
#define     OP_GET_NI_TOTAL_NODE_MASK   0xFF00  /**< Mask to get the total number of nodes*/
#define     OP_GET_NI_NODE_CMPLT_MASK   0x00FF  /**< Mask to get the number of completed nodes*/

/** Progress status of adding node and migrating primary controller */
#define     OP_ADD_NODE_LEARN_READY     1   /**< Ready to accept new node*/
#define     OP_ADD_NODE_FOUND           2   /**< Found new node*/
#define     OP_ADD_NODE_ADDING          3   /**< Adding new node*/
#define     OP_ADD_NODE_PROTOCOL_DONE   4   /**< Protocol part of adding node done*/
#define     OP_ADD_NODE_SEC_INCD        5   /**< Adding node securely*/
#define     OP_ADD_NODE_GET_NODE_INFO   6   /**< Getting node detailed information*/


/** Progress status of removing node */
#define     OP_RM_NODE_LEARN_READY      1   /**< Ready to remove a node*/
#define     OP_RM_NODE_FOUND            2   /**< Found a node*/
#define     OP_RM_NODE_REMOVING         3   /**< Removing the node*/

/** Progress status of replacing failed node */
#define     OP_RP_NODE_READY            1   /**< Ready to replace a node*/
#define     OP_RP_NODE_PROTOCOL_DONE    2   /**< Protocol part of replacing node done*/
#define     OP_RP_NODE_SEC_INCD         3   /**< Adding node securely*/
#define     OP_RP_NODE_GET_NODE_INFO    4   /**< Getting node detailed information*/

/** Progress status of initiating */
#define     OP_INI_STARTED              1   /**< Initiating started, ready to be added/removed to/from network*/
#define     OP_INI_PROTOCOL_DONE        2   /**< Protocol part of initiating done*/
#define     OP_INI_SEC_INCD             3   /**< Trying to be included securely */
#define     OP_INI_GET_NODE_INFO        4   /**< Getting node detailed information*/

/** Progress status of network update */
#define     OP_NU_TOPOLOGY              1   /**< Network topology update started*/
#define     OP_NU_NEIGHBOR              2   /**< Node neighbor update started*/
#define     OP_NU_GET_NODE_INFO         3   /**< Node information update started*/

/** network controller */
#define ZWNET_CTL_SEC		1		/**< secondary */
#define ZWNET_CTL_STRAY		2		/**< network different from default */
#define ZWNET_CTL_INCL		4		/**< SIS present in network */
#define ZWNET_CTL_PREPRI	8	    /**< was primary before SIS was added */
#define ZWNET_CTL_UPDATE	0x10    /**< SUC - update controller */

/** network descriptor */
typedef struct
{
	uint32_t	    id;			/**< home id */
	zwnoded_p	    local;		/**< local node */
	zwnoded_p	    suc;		/**< SUC node */
	uint8_t		    ctl;		/**< controller role ZWNET_CTL_xxx or-mask */
    void            *user;      /**< User specified information that was passed to zwnet_init() */
    void            *plt_ctx;   /**< Platform context for printing of output text messages*/
}
zwnetd_t, *zwnetd_p;


/** Z-Wave network abstraction */

typedef void (*zwnet_notify_fn)(void *user, uint8_t op, uint16_t sts);
/**<
network status notification callback
@param[in]	user	user context
@param[in]	op		network operation ZWNET_OP_XXX
@param[in]	sts		status of current operation
*/

#define ZWNET_NODE_ADDED	0		/**< Node is added */
#define ZWNET_NODE_REMOVED	1		/**< Node is removed */
#define ZWNET_NODE_UPDATED	2		/**< Node is updated */

typedef void (*zwnet_node_fn)(void *user, zwnoded_p noded, int mode);
/**<
network node status notification callback
@param[in]	user	user context
@param[in]	noded	newly added or removed node
@param[in]	mode	0 means node added, 1 means node removed, 2 means node updated
*/

#define ZWNET_TX_OK                 0x00
#define ZWNET_TX_NO_ACK             0x01  /**< retransmission error */
#define ZWNET_TX_FAIL               0x02  /**< transmit error */
#define ZWNET_TX_ROUTING_NOT_IDLE   0x03  /**< routing not idle */
#define ZWNET_TX_NOROUTE            0x04  /**< no route found in assignroute */
#define ZWNET_TX_NO_CB              0x05  /**< no callback frame from controller before timeout */

typedef void (*zwnet_appl_fn)(void *user, uint8_t tx_sts);
/**<
application transmit data status callback
@param[in]	user	user context
@param[in]	tx_sts	transmit status ZWNET_TX_xx
*/

struct _dev_record;
typedef int (*zwnet_dev_rec_find_fn)(void *dev_cfg_ctx, uint16_t vid, uint16_t ptype, uint16_t pid, struct _dev_record *dev_rec);
/**<
find device specific configuration record
@param[in]	dev_cfg_ctx	User specified device configuration context
@param[in]	vid		    Vendor ID
@param[in]	ptype		Product Type ID
@param[in]	pid		    Product ID
@param[out]	dev_rec     Device record
@return  Non-zero on found; zero on not found
*/

#ifdef  ZW_STRESS_TEST
typedef void (*zw_stress_tst_fn)(void *user, uint32_t seq_num);
/**<
stress test receiving sequence number callback
@param[in]	user	    User context during network initialization
@param[in]	seq_num	    Sequence number received.
*/
#endif


/** Device specific record field validity bitmask */
#define DEV_REC_MSK_VID         0x0001  /**< Vendor ID */
#define DEV_REC_MSK_PTYPE       0x0002  /**< Product Type ID */
#define DEV_REC_MSK_PID         0x0004  /**< Product ID */
#define DEV_REC_MSK_CAT         0x0008  /**< Device category, DEV_XXX */
#define DEV_REC_MSK_GID         0x0010  /**< Group id from which the member of the group will receive sensor report */
#define DEV_REC_MSK_CFG_NUM     0x0020  /**< Configuration parameter number */
#define DEV_REC_MSK_CFG_SZ      0x0040  /**< Configuration parameter data size */
#define DEV_REC_MSK_CFG_VAL     0x0080  /**< Configuration parameter value */
#define DEV_REC_MSK_WKUP        0x0100  /**< Wakeup interval */

#define DEV_REC_SEARCH_KEY_MSK  (DEV_REC_MSK_VID | DEV_REC_MSK_PTYPE | DEV_REC_MSK_PID) /**< Search key mask */
#define DEV_REC_CFG_PARAM_MSK   (DEV_REC_MSK_CFG_NUM | DEV_REC_MSK_CFG_SZ | DEV_REC_MSK_CFG_VAL) /**< Configuration parameter mask */

/** Device specific record */
typedef struct _dev_record
{
	uint16_t	vid;			/**< Vendor ID */
	uint16_t	ptype;			/**< Product Type ID */
	uint16_t	pid;			/**< Product ID */
    uint8_t     category;	    /**< Device category, DEV_XXX */
    uint8_t     grp_id;	        /**< Group id from which the member of the group will receive sensor report*/
    uint8_t     cfg_prm_num;	/**< Configuration parameter number*/
    uint8_t     cfg_prm_sz;	    /**< Configuration parameter data size: 1,2,or 4 bytes*/
    int32_t     cfg_prm_val;    /**< Configuration parameter value (signed integer)*/
    uint32_t    wkup_intv;      /**< Wakeup interval (in seconds) */
    uint32_t    fld_bitmsk;     /**< Field bitmask to indicate validity of the above-listed fields, DEV_REC_MSK_XXX */
} dev_rec_t;


/** Device specific configurations */
typedef struct
{
    dev_rec_t           *dev_cfg_buf;       /**< buffer that stores an array of device specific configurations*/
	uint16_t	        dev_cfg_cnt;	    /**< number of records in dev_cfg_buf */
	uint16_t	        dev_cfg_ver;	    /**< Device specific configuration version number. Currently supported version: 1 */
} dev_spec_cfg_t;


/** Device specific configurations (managed by user application) */
typedef struct
{
    zwnet_dev_rec_find_fn   dev_rec_find_fn;    /**< User supplied function to find device record*/
    void                    *dev_cfg_ctx;       /**< User specified device configuration context for use in dev_rec_find_fn*/
    uint16_t	            dev_cfg_ver;	    /**< Device specific configuration version number. Currently supported version: 1 */
} dev_spec_cfg_usr_t;


/** Device specific configurations buffer */
typedef struct
{
    dev_rec_t           *dev_cfg_buf;       /**< buffer that stores an array of device specific configurations*/
	uint16_t	        dev_cfg_cnt;	    /**< number of records in dev_cfg_buf */
} dev_cfg_buf_t;


/** network client initialization parameters */
typedef struct
{
	int				    instance;	        /**< 0 for now */
	void			    *user;		        /**< user specified information */
	void			    *comm_port_name;    /**< pointer to platform-dependent serial comm port name */
	zwnet_notify_fn	    notify;		        /**< command status callback */
	zwnet_node_fn	    node;		        /**< node add/del callback */
    zwnet_appl_fn       appl_tx;            /**< application transmit data status callback*/
	int				    sec_enable;	        /**< flag to enable security layer. 1 = enable, 0 = disable */
    print_fn            print_txt_fn;       /**< print text function */
    dev_spec_cfg_t      *dev_spec_cfg;      /**< Device specific configurations. If
                                                 it is NULL, device specific configurations will be managed by
                                                 user application. In this case dev_spec_cfg_usr must be valid  */
    dev_spec_cfg_usr_t  *dev_spec_cfg_usr;  /**< Device specific configurations (managed by user application). If
                                                 it is NULL, device specific configurations will be managed by
                                                 HCAPI library internally. In this case dev_spec_cfg must be valid */
}
zwnet_init_t, *zwnet_init_p;

#define HC_API_CLASSIC      0   /**< Classic HC API */
#define HC_API_ZIP          1   /**< Z/IP HC API */

#define HC_API_TYPE         HC_API_CLASSIC  /**< This project HC API type*/

/** network client initialization return values*/
typedef struct
{
	zwnet_p		    net;	    /**< network handle */
    uint32_t        net_id;     /**< network id */
    uint8_t         ctlr_id;    /**< controller id */
    uint8_t         hc_api_type;/**< HC API type HC_API_XXX */
}
zwnet_init_ret_t, *zwnet_init_ret_p;


int zwnet_init(const zwnet_init_p init, zwnet_init_ret_p ret);
/**<
initialize network and return this network
@param[in]	init	        client initialization parameters
@param[out]	ret	            client initialization return values
@return		ZW_ERR_XXX
@note       If the return value is ZW_ERR_NO_RESP, there is a possibility the controller's NVM is
            corrupted.  User may consider to reset the controller using zwnet_quick_reset() API.
*/

void zwnet_exit(zwnet_p net, const char *node_info_file);
/**<
clean up  network
@param[in]	net		        network
@param[in]	node_info_file	node information file name.  If this parameter is NULL,
                            node information will not be saved
*/

int zwnet_save(zwnet_p net, const char *node_info_file);
/**<
save network node information and security layer key and setting into a file
@param[in]	net		        network
@param[in]	node_info_file	node information file name
@return  ZW_ERR_XXX.
*/

int zwnet_load(zwnet_p nw, const char *node_info_file);
/**<
zwnet_load - Load network node information and security layer key and settings
@param[in]	nw		        Network
@param[in]	node_info_file	Node information file name.  If this parameter is NULL,
                            node information will be acquired through Z-Wave network
@return		ZW_ERR_XXX
*/

int zwnet_get_node(zwnet_p net, zwnoded_p noded);
/**<
get first node (local controller) in the network
@param[in]	net		network
@param[out]	noded	node
@return		ZW_ERR_NONE if successful
*/

void    zwnet_node_info_dump(zwnet_p net);
int32_t zwnet_ctlr_info_get(zwnet_p nw, uint8_t is_create_nw);

zwnetd_p zwnet_get_desc(zwnet_p net);
/**<
get read-only network descriptor
@param[in]	net		network
@return		network descriptor or NULL
*/

void *zwnet_get_user(zwnet_p net);
/**<
zwnet_get_user - get user context
@param[in]	net	        Network
@return	user context passed during initialization
*/

int zwnet_abort(zwnet_p net);
/**<
abort current action/transmission
@param[in]	net		network
@return		ZW_ERR_XXX
*/

int zwnet_add(zwnet_p net, uint8_t add);
/**<
zwnet_add - Called by inclusion controller to add/remove initiating node to network
@param[in]	net		Network
@param[in]	add		0=remove, else add node
@return		ZW_ERR_XXX, otherwise node callback will be called.
@see		zwnet_initiate
*/

int zwnet_fail(zwnoded_p noded, uint8_t replace);
/**<
called by inclusion controller to remove failed node from the routing table or replace failed node with initiating node
@param[in]	noded	failed node
@param[in]	replace	0-remove failed node, else replace it
@return		ZW_ERR_XXX, otherwise node callback will be called.
@see		zwnet_initiate
*/

int zwnet_migrate(zwnet_p net);
/**<
called by primary controller or SUC to make initiating controller primary, and itself secondary.
@param[in]	net		network
@return		ZW_ERR_XXX, otherwise node callback will be called
@see		zwnet_initiate
*/

int zwnet_initiate(zwnet_p net);
/**<
called by secondary controller to add/remove itself to/from the network
@param[in]	net		network
@return		ZW_ERR_XXX, otherwise node callback will be called
@see		zwnet_add, zwnet_fail, zwnet_migrate
*/


#ifdef  ZW_STRESS_TEST
void zwnet_stress_tst_cb_set(zwnet_p net, zw_stress_tst_fn cb);
#endif

/** roles assigned to a static controller */
#define ZWNET_ROLE_SIS		1	/**< SIS */

//int zwnet_assign(zwnoded_p noded, uint8_t role);
/**<
called by primary controller to assign or deassign SUC/SIS role to a static/bridge controller; or by a controller to enable/disable its own SUC/SIS capability.
@param[in]	noded	node (can be local)
@param[in]	role	role to assign ZWNET_ROLE_xxx or-mask
@return		ZW_ERR_XXX
*/

int zwnet_update(zwnet_p net);
/**<
update network status and information
@param[in]	net		network
@return		ZW_ERR_XXX
*/

int zwnet_reset(zwnet_p net);
/**<
detach self from network ie. forget all nodes.  A power-cycle is required after the reset.
@param[in]	net		network
@return		ZW_ERR_XXX
*/

void zwnet_version(uint8_t *ver, uint8_t *subver);
/**<
get the home controller API version and subversion
@param[out]	ver		Version
@param[out]	subver	Sub version
@return
*/

int zwnet_send_nif(zwnet_p net, zwnoded_p noded, uint8_t broadcast);
/**<
send node information frame to a node or broadcast it
@param[in]	net		    Network
@param[in]	noded	    Destination node to receive the node information frame
@param[in]	broadcast	Broadcast flag. 1= broadcast; 0= single cast
@return		ZW_ERR_XXX.
*/

int zwnet_fw_updt(zwnet_p net, const char *fw_file);
/**<
local firmware update
@param[in]	net		Network
@param[in]	fw_file	Firmware file
@return		ZW_ERR_xxx
*/

int zwnet_quick_reset(void *comm_port_name);
/**<
quick reset of controller without initialization of network
@param[in]	comm_port_name	Pointer to platform-dependent serial comm port name
@return		ZW_ERR_xxx
@note   This API should not be used if zwnet_init() returns 0, instead, use zwnet_reset(). Calling
        this API while the network has been initialized and not yet closed can cause undesirable
        effects on the network.
*/


/**
@}
@defgroup If_Vendor Manufacturer Specific Interface APIs
Used to get manufacturer and product info
@{
*/

#define MAX_DEV_ID_LEN      31      /**< Maximum device id length */

#define DEV_ID_TYPE_OEM     0       /**< Device id type is OEM factory default */
#define DEV_ID_TYPE_SN      1       /**< Device id type is serial number */

#define DEV_ID_FMT_UTF      0       /**< Device id format is UTF-8 and MUST be displayed as is. */
#define DEV_ID_FMT_BIN      1       /**< Device id format is binary and MUST be displayed as hexadecimal values
                                         e.g. 0x30, 0x31, 0x32, 0x33 MUST be displayed as h’30313233. */


/** device id */
typedef struct
{
    uint8_t   type;                         ///< device id type: DEV_ID_TYPE_XXX
    uint8_t   format;                       ///< device id data format: DEV_ID_FMT_XXX
    uint8_t   len;                          ///< device id length
    uint8_t   dev_id[MAX_DEV_ID_LEN + 1];   ///< device id

} dev_id_t;

/**
@}
@defgroup Node Node APIs
Nodes have certain attributes indicating its role and location in the network
and endpoints which indicate its utility to the user
@{
*/

/** Extended version information */
typedef struct
{
	uint8_t		hw_ver;		    /**< Hardware version*/
	uint8_t		fw_cnt;		    /**< Number of firmwares in the device, excluding the Z-wave firmware*/
	uint16_t	fw_ver[1];		/**< Firmware versions place holder as indicated in fw_cnt*/
}
ext_ver_t;

/** Node Descriptor - read only for client */
typedef struct  _zwnoded
{
	uint16_t	vid;			/**< Vendor ID */
	uint16_t	type;			/**< Vendor Product Type */
	uint16_t	pid;			/**< Product ID */
	uint8_t		nodeid;			/**< Node ID */
	uint8_t		alive;		    /**< Flag to indicate node is alive. 1= alive; 0= down or sleeping*/
    uint8_t     sec_incl_failed;/**< Flag to mark a node who failed the inclusion process. 1=failed, 0=success */
    zwnet_p     net;            /**< Network that the node belongs to */
    dev_id_t    dev_id;         /**< Device ID */
	uint16_t	proto_ver;		/**< Z-Wave Protocol Version*/
	uint16_t	app_ver;		/**< Application Version*/
	uint8_t		lib_type;		/**< Z-Wave Library Type*/
    uint8_t     category;	    /**< Device category, DEV_XXX */
	uint8_t		sensor;		    /**< Flag to indicate whether the node is a sensor (FLIRS) */
    uint8_t     sleep_cap;      /**< Flag to indicate the node is capable to sleep */
    uint32_t    wkup_intv;      /**< Wake up interval in seconds */

}
#ifdef WIN32
zwnoded_t, *zwnoded_p;
#else
zwnoded_t;
#endif


zwnet_p zwnode_get_net(zwnoded_p noded);
/**<
get node's associated network
@param[in]	noded	node
@return		network
*/

int zwnode_get_next(zwnoded_p noded, zwnoded_p nxt_noded);
/**<
get next node in network
@param[in]	noded	    The initial node for searching the next node
@param[out]	nxt_noded	The next node if found
@return		ZW_ERR_XXX
*/

int zwnode_get_ep(zwnoded_p noded, zwepd_p epd);
/**<
get first endpoint in device
@param[in]	noded	    node
@param[out]	epd	first endpoint
@return		ZW_ERR_XXX
*/

int zwnode_update(zwnoded_p noded);
/**<
update node status and information
@param[in]	noded	node
@return		ZW_ERR_XXX
*/

int zwnode_mul_cmd_ctl_set(zwnoded_p noded, uint8_t on);
/**<
start/stop multi command encapsulation
@param[in]	noded	node
@param[in]	on	    1=turn on multi command encapsulation;  0=turn off and flush
                    the commands in the buffer.
@return		ZW_ERR_xxx
*/

int zwnode_mul_cmd_ctl_get(zwnoded_p noded, uint8_t *on);
/**<
get the current state of multi command encapsulation
@param[in]	noded	    node
@param[out]	on	        state: 1=on; 0=off
return      ZW_ERR_XXX
*/

void zwnode_set_user(zwnoded_p noded, void *user);
/**<
set user specific information for node
@param[in]	noded	node
@param[in]	user	user specific information
*/

void *zwnode_get_user(zwnoded_p noded);
/**<
get user specific information for node
@param[in]	noded	node
@return	user specific information if found; else return NULL
*/

ext_ver_t *zwnode_get_ext_ver(zwnoded_p noded);
/**<
get extended version information
@param[in]	noded	Node
@return	extended version information if the node supports it; else return NULL
@post   Caller has to free the returned extended version information
*/

int zwnode_cmd_q_ena_set(zwnoded_p noded, uint8_t enable);
/**<
control the command queue
@param[in]	noded	    node
@param[in]	enable	    1=enable command queuing; 0=disable command queuing and
                        delete the commands in the queue.
return      ZW_ERR_XXX
*/


int zwnode_cmd_q_ena_get(zwnoded_p noded, uint8_t *enable);
/**<
get the setting of command queue control
@param[in]	noded	    node
@param[out]	enable	    current setting of command queue control;
                        1=enable command queuing; 0=disable command queuing
return      ZW_ERR_XXX
*/


uint8_t zwnode_cmd_q_get(zwnoded_p noded, uint16_t **cmd_q);
/**<
get command queue associated with the specified node
@param[in]	noded	node
@param[out]	cmd_q	command queue
@return     number of entries in cmd_q
@post       caller must free the cmd_q after this call if return value > 0
*/

int zwnode_cmd_q_cancel(zwnoded_p noded);
/**<
cancel all pending commands in command queue
@param[in]	noded	node
@return		ZW_ERR_XXX
*/

//Forward declaration of name location descriptor
struct _zw_nameloc;

int zwep_nameloc_set(zwepd_p epd, struct _zw_nameloc *nameloc);
/**<
zwep_nameloc_set - Set endpoint name and location string for device regardless whether the endpoint
                   has Node Naming And Location interface.  If it does, the strings will be sent to
                   the interface physically.
@param[in]	    epd	        The endpoint
@param[in,out]	nameloc	    Input:null terminated name & location string
                            Output:null terminated name & location string with proper UTF-8 delineation
@return		ZW_ERR_XXX
*/

/**
@}
@defgroup EP Endpoint APIs
Endpoints have interfaces that can received commands for a particular attribute of a utility
@{
*/

/** Z-wave+ info */
typedef struct
{
	uint8_t		zwplus_ver;		/**< Z-Wave+ version. Zero indicates this node is non-Z-Wave+ */
	uint8_t		node_type;		/**< Z-Wave+ node type*/
	uint8_t		role_type;		/**< Z-Wave+ role type*/
	uint16_t	instr_icon;		/**< Installer icon type */
	uint16_t	usr_icon;		/**< User icon type */

}
zwplus_info_t;

/** Endpoint Descriptor - read only for client */
typedef struct _zwepd
{
	uint8_t		    generic;		            /**< Generic Device Class */
	uint8_t		    specific;		            /**< Specific Device Class */
	uint8_t		    epid;			            /**< Endpoint ID */
	uint8_t		    nodeid;			            /**< Node ID */
    zwnet_p         net;                        /**< Network that the node belongs to */
    char		    name[ZW_LOC_STR_MAX + 1];	/**< user configured name string of the endpoint */
    char		    loc[ZW_LOC_STR_MAX + 1];    /**< user configured location string of the endpoint */
    zwplus_info_t   zwplus_info;                /**< Z-wave+ information */
}
#ifdef WIN32
zwepd_t, *zwepd_p;
#else
zwepd_t;
#endif


int zwep_get_node(zwepd_p epd, zwnoded_p noded);
/**<
get ep's associated node
@param[in]	epd		endpoint
@param[out]	noded	node
@return		ZW_ERR_XXX
*/

int zwep_get_if(zwepd_p epd, zwifd_p ifd);
/**<
get first interface in endpoint
@param[in]	epd	    endpoint
@param[out]	ifd	    interface
@return		ZW_ERR_XXX
@post   Caller must free ifd->data if ifd->data_cnt > 0
*/

int zwep_get_next(zwepd_p epd, zwepd_p nxt_epd);
/**<
get next endpoint in node
@param[in]	epd	    endpoint
@param[out]	nxt_epd	next endpoint
@return		ZW_ERR_XXX
*/


void zwep_set_user(zwepd_p epd, void *user);
/**<
set user specific information for endpoint
@param[in]	epd	    endpoint
@param[in]	user	user specific information
*/

void *zwep_get_user(zwepd_p epd);
/**<
get user specific information for endpoint
@param[in]	epd	    Endpoint
@return	    user specific information
*/



/**
@}
@defgroup If Interface APIs
@{
*/

#define IF_PROPTY_SECURE    0x01    /**< Interface can be access securely bit-mask*/
#define IF_PROPTY_UNSECURE  0x02    /**< Interface can be access unsecurely bit-mask*/


/** interface descriptor */
typedef struct _zwifd
{
    uint16_t    cls;			/**< Command class */
    uint8_t		ver;			/**< Version of the command class */
	uint8_t		propty;	        /**< Properties of the interface (bit-mask): IF_PROPTY_XXX */
    uint8_t		epid;			/**< Endpoint ID */
    uint8_t		nodeid;			/**< Node ID */
    uint8_t	    data_cnt;	    /**< number of data items */
    void        *data;          /**< data items which is specific to the command class*/
    zwnet_p     net;            /**< Network that the node belongs to */
}
#ifdef WIN32
zwifd_t, *zwifd_p;
#else
zwifd_t;
#endif

/** Multi-level sensor interface data */
typedef struct
{
	uint8_t		sensor_type;    /**< supported sensor type */
	uint8_t		sensor_unit;    /**< supported sensor units (bit-mask)*/
} if_sensor_data_t;

/** Thermostat Fan Mode interface data */
typedef struct
{
	uint8_t		off;            /**< flag to indicate whether off mode is supported */
	uint8_t		mode_len;       /**< number of entries in mode buffer*/
    uint8_t     mode[1];        /**< mode place holder*/
} if_thrmo_fan_mode_data_t;

/** Thermostat Mode interface data */
typedef struct
{
	uint8_t		mode_len;       /**< number of entries in mode buffer*/
    uint8_t     mode[1];        /**< mode place holder*/
} if_thrmo_mode_data_t;

/** Thermostat Setpoint interface data */
typedef struct
{
	uint8_t		setp_len;       /**< number of entries in setp buffer*/
    uint8_t     setp[1];        /**< setpoint types place holder*/
} if_thrmo_setp_data_t;

/** Multilevel switch interface data */
typedef struct
{
	uint8_t		pri_type;       /**< primary switch type, SW_TYPE_XX */
    uint8_t     sec_type;       /**< secondary switch type , SW_TYPE_XX*/
} if_mul_switch_data_t;

#define MAX_EVT_BITMASK_LEN     6
/** Alarm type and its associated events */
typedef struct
{
    uint8_t     ztype;                      /**< alarm type*/
    uint8_t     evt_len;                    /**< supported event bitmask length*/
	uint8_t		evt[MAX_EVT_BITMASK_LEN];   /**< event bitmask. Bit-0 in evt[0] is unused.
                                            Bit-1 in evt[0] set means event 0x01 is supported, and so on...*/
} if_alarm_type_evt_t;

/** Alarm interface data */
typedef struct
{
    uint8_t                 have_vtype;     /**< flag to indicate whether vendor specific alarm type supported*/
	uint8_t		            type_evt_cnt;   /**< number of entries in type_evt buffer*/
    if_alarm_type_evt_t     type_evt[1];    /**< alarm type-event place holder*/
} if_alarm_data_t;

#define ZWCMD_GET		0
#define ZWCMD_SET_OFF	1
#define ZWCMD_SET_ON	2
#define ZWCMD_SET_V		3	/* parameter 1: 1%-99% */
#define ZWCMD_CHANGE	4	/* parameter 1: 1%-99% */

typedef void (*zwrep_fn)(zwifd_p ifd, uint8_t level);
/**<
generic report callback for switch, binary sensor, battery, lock
@param[in]	ifd	    interface
@param[in]	level	level
*/

typedef void (*zwrep_group_fn)(zwifd_p ifd, uint8_t group, uint8_t max_cnt, uint8_t cnt, zwepd_p ep);
/**<
group report callback
@param[in]	ifd	Interface
@param[in]	group	    Grouping identifier
@param[in]	max_cnt	    Maximum number of end points the grouping identifier above supports
@param[in]	cnt	        The number of end points in the grouping in this report
@param[in]	ep	        An array of cnt end points in the grouping

*/


int zwif_get_ep(zwifd_p ifd, zwepd_p epd);
/**<
get endpoint containing this interface
@param[in]	ifd		interface
@param[out]	epd		endpoint
@return		ZW_ERR_XXX
*/

int zwif_get_next(zwifd_p ifd, zwifd_p nxt_ifd);
/**<
get next interface in containing endpoint
@param[in]	ifd			interface
@param[out]	nxt_ifd		next interface
@return		ZW_ERR_XXX
@post   Caller must free nxt_ifd->data if nxt_ifd->data_cnt > 0
*/

int zwif_exec(zwifd_p ifdesc, uint8_t *cmd_buf, int buf_len, tx_cmplt_cb_t cb);
/**<
execute action on an interface
@param[in]	ifdesc      interface
@param[in]	cmd_buf		command and parameters
@param[in]	buf_len		length of cmd in bytes
@param[in]	cb		    callback function for transmit status
@return	ZW_ERR_XXX
*/

void zwif_set_user(zwifd_p ifd, void *user);
/**<
set user specific information for interface
@param[in]	ifd	    Interface
@param[in]	user	user specific information
*/

void *zwif_get_user(zwifd_p ifd);
/**<
get user specific information for interface
@param[in]	ifd	    interface
@return	user specific information else NULL
*/



/**
@}
@defgroup If_Loc Location Interface APIs
Clients can assign descriptive name and location strings
Their state can be read back by the generic zwif_get_report.
@{
*/

/** name location descriptor */
typedef struct _zw_nameloc
{
	char name[ZW_LOC_STR_MAX + 1];  /**< name string in UTF-8 */
	char loc[ZW_LOC_STR_MAX + 1];   /**< location string in UTF-8 */
}
zw_nameloc_t, *zw_nameloc_p;


/**
@}
@defgroup If_Grp Group Interface APIs
Groups are application-specific, and normally specify report recipients eg. for a sensor
@{
*/

int zwif_group_get(zwifd_p ifd, uint8_t group, zwrep_group_fn cb);
/**<
get information on specified group of interface through report callback
@param[in]		ifd	        interface
@param[in]		group	    group id
@param[in]		cb	        report callback function
@return  0 on success, else ZW_ERR_XXX
*/


int zwif_group_add(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt);
/**<
add endpoints to this group
@param[in]	ifd		interface
@param[in]	group	grouping identifier
@param[in]	ep		array of cnt end points in the grouping
@param[in]	cnt		number of end points
@return		ZW_ERR_XXX
*/


int zwif_group_del(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt);
/**<
remove endpoints from this group
@param[in]	ifd		interface
@param[in]	group	grouping identifier
@param[in]	ep		array of cnt endpoints in the grouping
@param[in]	cnt		number of end points
@return		ZW_ERR_XXX
*/


typedef void (*zwrep_group_sup_fn)(zwifd_p ifd,  uint8_t max_grp);
/**<
report callback for maximum number of groupings the given node supports
@param[in]	ifd	      interface
@param[in]	max_grp   maximum number of groupings
*/


int zwif_group_sup_get(zwifd_p ifd,  zwrep_group_sup_fn cb);
/**<
get information on the maximum number of groupings the given node supports through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/


typedef void (*zwrep_group_actv_fn)(zwifd_p ifd,  uint8_t group);
/**<
report callback for current active group
@param[in]	ifd	    interface
@param[in]	group   current active group
*/


int zwif_group_actv_get(zwifd_p ifd,  zwrep_group_actv_fn cb);
/**<
get information on the current active group from a node through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/


typedef struct
{
    uint8_t     configurable;    /**< 1=command record is configurable, 0=not configurable */
    uint8_t     config_type;    /**< Configuration type: 1=value type (only support Basic Set command),
                                     0=command type (support any command)*/
    uint8_t     max_len;        /**< maximum command length that can be set */
    uint16_t    free_rec;       /**< number of free command records that can be set */
    uint16_t    total_rec;	    /**< total number of command records supported */
}
zwgrp_cmd_cap_t, * zwgrp_cmd_cap_p;


typedef void (*zwrep_grp_cmd_sup_fn)(zwifd_p ifd, zwgrp_cmd_cap_p  cmd_cap);
/**<
report callback for command records supporting capabilities
@param[in]	ifd	        interface
@param[in]	cmd_cap     command records supporting capabilities
*/


int zwif_group_cmd_sup_get(zwifd_p ifd,  zwrep_grp_cmd_sup_fn cb);
/**<
get information on command records supporting capabilities through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/


typedef void (*zwrep_grp_cmd_fn)(zwifd_p ifd, uint8_t group, uint8_t nodeid, uint8_t *cmd_buf, uint8_t len);
/**<
report callback for command record for a node within a given grouping identifier.
@param[in]	ifd	        interface
@param[in]	group	grouping identifier
@param[in]	nodeid	node id of the node within the grouping specified
@param[in]	cmd_buf	command and parameters
@param[in]	len     length of cmd_buf
*/


int zwif_group_cmd_get(zwifd_p ifd, uint8_t group, uint8_t nodeid, zwrep_grp_cmd_fn cb);
/**<
get command record for a node within a given grouping identifier through report callback
@param[in]	ifd	    interface
@param[in]	group	grouping identifier
@param[in]	nodeid	node id of the node within the grouping specified
@param[in]  cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/


int zwif_group_cmd_set(zwifd_p ifd, uint8_t group, zwnoded_p node, uint8_t *cmd_buf, uint8_t len);
/**<
configure the command to send to the node in this group
@param[in]	ifd		interface
@param[in]	group	grouping identifier
@param[in]	node	node within the grouping specified, that should receive the command specified in cmd_buf
@param[in]	cmd_buf	command and parameters
@param[in]	len     length of cmd_buf
@return		ZW_ERR_XXX
*/


#define ZW_GRP_NAME_MAX	    42  //Group name maximum string length

/** group command entry*/
typedef struct
{
    uint16_t  cls;          ///< command class
    uint8_t   cmd;          ///< command
} grp_cmd_ent_t;

/** group information */
typedef struct _zw_grp_info
{
	uint8_t         grp_num;        /**< group number */
	uint8_t         cmd_ent_cnt;    /**< Number of entries in command list (cmd_lst) */
	uint16_t        profile;        /**< profile */
	uint16_t        evt_code;       /**< event code */
	char            name[ZW_GRP_NAME_MAX + 1];   /**< null terminated group name string in UTF-8 */
    grp_cmd_ent_t   cmd_lst[1];     /**< place holder for command list */
}
zw_grp_info_t, *zw_grp_info_p;

/** Association group info interface data */
typedef struct
{
	uint16_t	    group_cnt;      /**< Number of supported groups */
	uint16_t	    valid_grp_cnt;  /**< Number of valid group info in grp_info[]*/
    uint8_t         dynamic;        /**< Flag to indicate the group info is dynamic. 1=dynamic; 0=static */
    zw_grp_info_p   grp_info[1];    /**< Place holder for storing group information */

} if_grp_info_dat_t;


int zwif_group_info_get(zwifd_p ifd, if_grp_info_dat_t **grp_info);
/**<
get detailed group information
@param[in]	ifd	        interface
@param[out]	grp_info	grouping information if success; NULL on failure
@return  0 on success, else ZW_ERR_XXX
@post  Caller is required to call zwif_group_info_free to free the memory allocated to grp_info
*/


void zwif_group_info_free(if_grp_info_dat_t *grp_info);
/**<
free group information
@param[in]	ifd	        interface
@param[in]	grp_info	grouping information returned by zwif_group_info_get()
@return
@post   Caller should not use the grp_info after this call.
*/

/**
@}
@defgroup If_Wku Wake Up Interface APIs
Wake up APIs are for battery powered device that sleep most of the time
@{
*/

/** wake up interval settings in seconds */
typedef struct
{
	uint32_t	min;		/**< minimum */
	uint32_t	max;		/**< maximum */
	uint32_t	def;		/**< default */
	uint32_t	interval;	/**< steps between min and max */
	uint32_t	cur;		/**< current */
	zwnoded_t	node;       /**< node to notify wake up */
}
zwif_wakeup_t, *zwif_wakeup_p;

typedef int (*zwrep_wakeup_fn)(zwifd_p ifd, zwif_wakeup_p cap);
/**<
wake up report or notification callback
@param[in]	ifd	    interface
@param[in]	cap		capabilities report, null for notification
@return	    Only apply to notification: 0=no command pending to send; 1=commands pending to send.
*/


int  zwif_wakeup_set(zwifd_p ifd, uint32_t secs, zwnoded_p node);
/**<
set wake up interval and node to notify on wake up
@param[in]	ifd	    interface
@param[in]	secs	interval in seconds (24 bit)
@param[in]	node	node to notify
@return		ZW_ERR_XXX
*/

int  zwif_wakeup_get(zwifd_p ifd, zwrep_wakeup_fn cb);
/**<
get wake up report
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup If_Swt Switch Interface APIs
Switches can be switched on/off and on completion will call zwnet::notify
Their state can be read back by the generic zwif_get_report.
@{
*/

typedef void (*zwrep_switch_fn)(zwifd_p ifd, uint8_t on);
/**<
report callback for switch
@param[in]	ifd	    interface
@param[in]	on		0=off, else on
*/

int zwif_switch_set(zwifd_p ifd, uint8_t on);
/**<
turn on/off switch
@param[in]	ifd		interface
@param[in]	on		0=off, else on
@return		ZW_ERR_XXX
*/

int zwif_switch_rpt_set(zwifd_p ifd, zwrep_switch_fn rpt_cb);
/**<
setup a switch report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/


int zwif_switch_get(zwifd_p ifd);
/**<
get switch report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup If_Lvl Level Interface APIs
Levels can be set and auto-(in/de)cremented with start/stop and on completion will call zwnet::notify
Their state can be read back by zwif_get_report.
@{
*/
#define ZWIF_LEVEL_DIR_UP           0
#define ZWIF_LEVEL_DIR_DOWN         1
#define ZWIF_LEVEL_DIR_NO_CHANGE    3

typedef void (*zwrep_level_fn)(zwifd_p ifd, uint8_t level);
/**<
report callback for level
@param[in]	ifd	    interface
@param[in]	level	level
*/

int zwif_level_set(zwifd_p ifd, uint8_t v,  uint8_t dur);
/**<
set level
@param[in]	ifd		interface
@param[in]	v		0=off, 0xFF=on(previous level), 1-99=%
@param[in]	dur	    Dimming duration.  0=instantly;  0x01 to 0x7F = 1 second (0x01) to 127 seconds (0x7F);
                    0x80 to 0xFE = 1 minute (0x80) to 127 minutes (0xFE); 0xFF = factory default rate.
@return	ZW_ERR_XXX
*/


typedef struct
{
    uint8_t     pri_dir;        /**< primary switch direction.  0 = increase level; 1 = decrease level;3=no change */
    uint8_t     pri_level;      /**< primary switch start level */
    uint8_t     pri_ignore_lvl; /**< flag for ignoring primary switch start level.  0=use the start level as in pri_level;
                                     1=don’t use the start level and instead start from the actual level in the device. */
    uint8_t     sec_dir;        /**< secondary switch direction.  0 = increase level; 1 = decrease level;3=no change */
    uint8_t     sec_step;       /**< secondary switch step size. 0 to 99 = percentage of steps the increment or decrement
                                     function should execute. 255 (0xFF) = fixed step size defined by the OEM */
    uint8_t     dur;            /**< Dimming duration in seconds which is the interval it takes to dim from level 0 to 99 */
}
zwlevel_t, *zwlevel_p;

int zwif_level_start(zwifd_p ifd, zwlevel_p  level_ctrl);
/**<
start modifying levels
@param[in]	ifd	        interface
@param[in]	level_ctrl	level control of switches
@return	ZW_ERR_XXX
*/

int zwif_level_stop(zwifd_p ifd);
/**<
stop changing level
@param[in]	ifd		interface
@return		ZW_ERR_XXX
*/

int zwif_level_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb);
/**<
setup a level report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_level_get(zwifd_p ifd);
/**<
get level report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/


#define    SW_TYPE_NOT_SUP	        0x00	/**< Not present or supported */
#define    SW_TYPE_OFF_ON	        0x01	/**< When level =0 the switch is off,  level=0x63/0xFF the switch is on*/
#define    SW_TYPE_DOWN_UP	        0x02	/**< When level =0 the switch is down,  level=0x63/0xFF the switch is up*/
#define    SW_TYPE_CLOSE_OPEN	    0x03	/**< When level =0 the switch is close,  level=0x63/0xFF the switch is open*/
#define    SW_TYPE_COUNTER_CLOCKWISE 0x04	/**< When level =0 the switch is turning counter-clockwise,
                                                 level=0x63/0xFF the switch is turning clockwise*/
#define    SW_TYPE_LEFT_RIGHT	    0x05	/**< When level =0 the switch is left,  level=0x63/0xFF the switch is right*/
#define    SW_TYPE_REV_FWD	        0x06	/**< When level =0 the switch is reverse,  level=0x63/0xFF the switch is forward*/
#define    SW_TYPE_PULL_PUSH	    0x07	/**< When level =0 the switch is pulled,  level=0x63/0xFF the switch is pushed*/

typedef void (*zwrep_lvl_sup_fn)(zwifd_p ifd,  uint8_t pri_type, uint8_t sec_type);
/**<
report callback for supported multilevel switch types
@param[in]	ifd	        interface
@param[in]	pri_type    primary switch type, SW_TYPE_XX
@param[in]	sec_type    secondary switch type , SW_TYPE_XX.
*/

int zwif_level_sup_get(zwifd_p ifd, zwrep_lvl_sup_fn cb);
/**<
get a switch type report through report callback
@param[in]	ifd	        interface
@param[in]	cb	        callback function to receive the supported switch type report
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup BSns Binary Sensor Interface APIs
Binary sensors state can be idle (no event) or event detected
Their state can be read back by the generic zwif_get_report.
@{
*/

typedef void (*zwrep_bsensor_fn)(zwifd_p ifd, uint8_t state);
/**<
report callback for binary sensor
@param[in]	ifd	        interface
@param[in]	state		0=idle, else event detected
*/

int zwif_bsensor_rpt_set(zwifd_p ifd, zwrep_bsensor_fn rpt_cb);
/**<
setup a binary sensor report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_bsensor_get(zwifd_p ifd);
/**<
get binary sensor report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup Sns Sensor Interface APIs
Multi-level sensors value can be 1, 2 or 4 bytes signed number
Their value can be read back by the generic zwif_get_report.
@{
*/
/** Sensor Type definition */
#define ZW_SENSOR_TYPE_TEMP      0x01    /**< Temperature sensor*/
#define ZW_SENSOR_TYPE_GP        0x02    /**< General purpose sensor*/
#define ZW_SENSOR_TYPE_LUM       0x03    /**< Luminance sensor*/
#define ZW_SENSOR_TYPE_POW       0x04    /**< Power sensor*/
#define ZW_SENSOR_TYPE_HUMD      0x05    /**< Relative humidity sensor*/
#define ZW_SENSOR_TYPE_VELO      0x06    /**< Velocity sensor*/
#define ZW_SENSOR_TYPE_DIR       0x07    /**< Direction sensor*/
#define ZW_SENSOR_TYPE_ATM       0x08    /**< Atmospheric pressure sensor*/
#define ZW_SENSOR_TYPE_BARO      0x09    /**< Barometric pressure sensor*/
#define ZW_SENSOR_TYPE_SLR       0x0A    /**< Solar radiation sensor*/
#define ZW_SENSOR_TYPE_DEW       0x0B    /**< Dew point sensor*/
#define ZW_SENSOR_TYPE_RAIN      0x0C    /**< Rain rate sensor*/
#define ZW_SENSOR_TYPE_TIDE      0x0D    /**< Tide level sensor*/
#define ZW_SENSOR_TYPE_WGT       0x0E    /**< Weight sensor*/
#define ZW_SENSOR_TYPE_VOLT      0x0F    /**< Voltage sensor*/
#define ZW_SENSOR_TYPE_CUR       0x10    /**< Current sensor*/
#define ZW_SENSOR_TYPE_CO2       0x11    /**< CO2-level sensor*/
#define ZW_SENSOR_TYPE_AIR       0x12    /**< Air flow sensor*/
#define ZW_SENSOR_TYPE_TANK      0x13    /**< Tank capacity sensor*/
#define ZW_SENSOR_TYPE_DIST      0x14    /**< Distance sensor*/
#define ZW_SENSOR_TYPE_AGL       0x15    /**< Angle Position sensor*/
#define ZW_SENSOR_TYPE_ROT       0x16    /**< Rotation sensor*/
#define ZW_SENSOR_TYPE_WTR_TEMP  0x17    /**< Water temperature sensor*/
#define ZW_SENSOR_TYPE_SOIL_TEMP 0x18    /**< Soil temperature sensor*/
#define ZW_SENSOR_TYPE_SEIS_INT  0x19    /**< Seismic intensity sensor*/
#define ZW_SENSOR_TYPE_SEIS_MAG  0x1A    /**< Seismic magnitude sensor*/
#define ZW_SENSOR_TYPE_UV        0x1B    /**< Ultraviolet sensor*/
#define ZW_SENSOR_TYPE_ELEC_RES  0x1C    /**< Electrical resistivity sensor*/
#define ZW_SENSOR_TYPE_ELEC_COND 0x1D    /**< Electrical conductivity sensor*/
#define ZW_SENSOR_TYPE_LOUDNESS  0x1E    /**< Loudness sensor*/
#define ZW_SENSOR_TYPE_MOIST     0x1F    /**< Moisture sensor*/
#define ZW_SENSOR_TYPE_FREQ      0x20    /**< Frequency sensor*/
#define ZW_SENSOR_TYPE_TIME      0x21    /**< Time sensor*/
#define ZW_SENSOR_TYPE_TGT_TEMP  0x22    /**< Target temperature sensor*/

/** Sensor Unit definition */
#define ZW_SENSOR_UNIT_TEMP_CEL       0x00    /**< Air temperature unit: Celsius (C)*/
#define ZW_SENSOR_UNIT_TEMP_FAHR      0x01    /**< Air temperature unit: Fahrenheit (F)*/

#define ZW_SENSOR_UNIT_GP_PERC        0x00    /**< General purpose unit: Percentage value*/
#define ZW_SENSOR_UNIT_GP_NODIM       0x01    /**< General purpose unit: Dimensionless value*/

#define ZW_SENSOR_UNIT_LUM_PERC       0x00    /**< Luminance unit: Percentage value*/
#define ZW_SENSOR_UNIT_LUM_LUX        0x01    /**< Luminance unit: Lux*/

#define ZW_SENSOR_UNIT_POW_W          0x00    /**< Power unit: W*/
#define ZW_SENSOR_UNIT_POW_BTU        0x01    /**< Power unit: Btu/h*/

#define ZW_SENSOR_UNIT_HUMD_PERC      0x00    /**< Relative humidity unit: Percentage value*/
#define ZW_SENSOR_UNIT_HUMD_ABS       0x01    /**< Absolute humidity unit: (g/m3) */

#define ZW_SENSOR_UNIT_VELO_MS        0x00    /**< Velocity unit: m/s*/
#define ZW_SENSOR_UNIT_VELO_MPH       0x01    /**< Velocity unit: mph*/

#define ZW_SENSOR_UNIT_DIR_DEG        0x00    /**< Direction unit: 0 to 360 degrees. 0 = no wind, 90 = east,
                                                   180 = south, 270 = west, and 360 = north*/

#define ZW_SENSOR_UNIT_ATM_KPA        0x00    /**< Atmospheric pressure unit: kPa*/
#define ZW_SENSOR_UNIT_ATM_HG         0x01    /**< Atmospheric pressure unit: inches of Mercury*/

#define ZW_SENSOR_UNIT_BARO_KPA       0x00    /**< Barometric pressure unit: kPa*/
#define ZW_SENSOR_UNIT_BARO_HG        0x01    /**< Barometric pressure unit: inches of Mercury*/

#define ZW_SENSOR_UNIT_SLR_WM2        0x00    /**< Solar radiation unit: W/m2*/

#define ZW_SENSOR_UNIT_DEW_CEL        0x00    /**< Dew point unit: Celsius (C)*/
#define ZW_SENSOR_UNIT_DEW_FAHR       0x01    /**< Dew point unit: Fahrenheit (F)*/

#define ZW_SENSOR_UNIT_RAIN_MMH       0x00    /**< Rain rate unit: mm/h*/
#define ZW_SENSOR_UNIT_RAIN_INH       0x01    /**< Rain rate unit: in/h*/

#define ZW_SENSOR_UNIT_TIDE_M         0x00    /**< Tide level unit: m*/
#define ZW_SENSOR_UNIT_TIDE_FT        0x01    /**< Tide level unit: feet*/

#define ZW_SENSOR_UNIT_WGT_KG         0x00    /**< Weight unit: kg*/
#define ZW_SENSOR_UNIT_WGT_LBS        0x01    /**< Weight unit: pounds*/

#define ZW_SENSOR_UNIT_VOLT_V         0x00    /**< Voltage unit: V*/
#define ZW_SENSOR_UNIT_VOLT_MV        0x01    /**< Voltage unit: mV*/

#define ZW_SENSOR_UNIT_CUR_A          0x00    /**< Current unit: A*/
#define ZW_SENSOR_UNIT_CUR_MA         0x01    /**< Current unit: mA*/

#define ZW_SENSOR_UNIT_CO2_PPM        0x00    /**< CO2-level unit: ppm*/

#define ZW_SENSOR_UNIT_AIR_M3H        0x00    /**< Air flow unit: m3/h*/
#define ZW_SENSOR_UNIT_AIR_CFM        0x01    /**< Air flow unit: cfm (cubic feet per minute)*/

#define ZW_SENSOR_UNIT_TANK_L         0x00    /**< Tank capacity unit: l*/
#define ZW_SENSOR_UNIT_TANK_CBM       0x01    /**< Tank capacity unit: cbm*/
#define ZW_SENSOR_UNIT_TANK_GAL       0x02    /**< Tank capacity unit: US gallons*/

#define ZW_SENSOR_UNIT_DIST_M         0x00    /**< Distance unit: m*/
#define ZW_SENSOR_UNIT_DIST_CM        0x01    /**< Distance unit: cm*/
#define ZW_SENSOR_UNIT_DIST_FT        0x02    /**< Distance unit: feet*/

#define ZW_SENSOR_UNIT_AGL_PERC       0x00    /**< Angle Position unit: Percentage value*/
#define ZW_SENSOR_UNIT_AGL_DEGN       0x01    /**< Angle Position unit: Degrees relative to
                                                   north pole of standing eye view*/
#define ZW_SENSOR_UNIT_AGL_DEGS       0x02    /**< Angle Position unit: Degrees relative to
                                                   south pole of standing eye view*/
#define ZW_SENSOR_UNIT_ROT_RPM        0x00    /**< Rotation unit: rpm (revolutions per minute)*/
#define ZW_SENSOR_UNIT_ROT_HZ         0x01    /**< Rotation unit: Hz (Hertz)*/

#define ZW_SENSOR_UNIT_WTR_TEMP_CEL   0x00    /**< Water temperature unit: Celsius (C)*/
#define ZW_SENSOR_UNIT_WTR_TEMP_FAHR  0x01    /**< Water temperature unit: Fahrenheit (F)*/

#define ZW_SENSOR_UNIT_SOIL_TEMP_CEL  0x00    /**< Soil temperature unit: Celsius (C)*/
#define ZW_SENSOR_UNIT_SOIL_TEMP_FAHR 0x01    /**< Soil temperature unit: Fahrenheit (F)*/

#define ZW_SENSOR_UNIT_SEIS_INT_MERC  0x00    /**< Seismic intensity unit: Mercalli*/
#define ZW_SENSOR_UNIT_SEIS_INT_EMCRO 0x01    /**< Seismic intensity unit: European Macroseismic*/
#define ZW_SENSOR_UNIT_SEIS_INT_LIEDU 0x02    /**< Seismic intensity unit: Liedu*/
#define ZW_SENSOR_UNIT_SEIS_INT_SHNDO 0x03    /**< Seismic intensity unit: Shindo*/

#define ZW_SENSOR_UNIT_SEIS_MAG_LOCAL 0x00    /**< Seismic magnitude unit: Local (ML)*/
#define ZW_SENSOR_UNIT_SEIS_MAG_MOM   0x01    /**< Seismic magnitude unit: Moment (MW)*/
#define ZW_SENSOR_UNIT_SEIS_MAG_SWAVE 0x02    /**< Seismic magnitude unit: Surface wave (MS)*/
#define ZW_SENSOR_UNIT_SEIS_MAG_BWAVE 0x03    /**< Seismic magnitude unit: Body wave (MB)*/

#define ZW_SENSOR_UNIT_UV_INDEX       0x00    /**< Ultraviolet unit: UV index*/

#define ZW_SENSOR_UNIT_ELEC_RES_OHMM  0x00    /**< Electrical resistivity unit: ohm metre*/

#define ZW_SENSOR_UNIT_ELEC_COND_SIEM 0x00    /**< Electrical conductivity unit: siemens per metre (S/m)*/

#define ZW_SENSOR_UNIT_LOUDNESS_ABS   0x00    /**< Loudness unit: Absolute loudness (dB)*/
#define ZW_SENSOR_UNIT_LOUDNESS_A_WT  0x01    /**< Loudness unit: A-weighted decibels (dBA)*/

#define ZW_SENSOR_UNIT_MOIST_PERC     0x00    /**< Moisture unit: Percentage value*/
#define ZW_SENSOR_UNIT_MOIST_VOL_WTR  0x01    /**< Moisture unit: Volume water content (m3/m3)*/
#define ZW_SENSOR_UNIT_MOIST_IMPD     0x02    /**< Moisture unit: Impedance (k ohm)*/
#define ZW_SENSOR_UNIT_MOIST_WTR_ACT  0x03    /**< Moisture unit: Water activity (aw)*/

#define ZW_SENSOR_UNIT_FREQ_HZ        0x00    /**< Frequency unit: Hz - Must be used until 4.294967295 GHz*/
#define ZW_SENSOR_UNIT_FREQ_KHZ       0x01    /**< Frequency unit: kHz- Must be used until 4.294967295 GHz*/

#define ZW_SENSOR_UNIT_TIME_SEC       0x00    /**< Time unit: seconds*/

#define ZW_SENSOR_UNIT_TGT_TEMP_CEL   0x00    /**< Target temperature unit: Celsius (C)*/
#define ZW_SENSOR_UNIT_TGT_TEMP_FAHR  0x01    /**< Target temperature unit: Fahrenheit (F)*/

/** sensor data */
typedef struct
{
    uint8_t     type;		/**< ZW_SENSOR_TYPE_XXX */
    uint8_t     precision;  /**< Decimal places of the value.  The decimal
                                 value 1025 with precision 2 is therefore equal to 10.25.*/
    uint8_t     unit;	    /**< ZW_SENSOR_UNIT_XXX */
    uint8_t     size;	    /**< data size: 1,2,or 4 bytes*/
    uint8_t     data[4];	/**< sensor data (a signed number) with the first byte
                                 is the most significant byte*/
}
zwsensor_t, *zwsensor_p;

typedef void (*zwrep_sensor_fn)(zwifd_p ifd, zwsensor_p data);
/**<
report callback for multi-level sensor
@param[in]	ifd	        interface
@param[in]	data		current value and unit of the sensor
*/

int zwif_sensor_rpt_set(zwifd_p ifd, zwrep_sensor_fn rpt_cb);
/**<
setup a multilevel sensor report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_sensor_get(zwifd_p ifd, uint8_t type, uint8_t unit);
/**<
get multilevel sensor report through report callback
@param[in]	ifd	        interface
@param[in]	type	    preferred sensor type, ZW_SENSOR_TYPE_XXX. If type equals to zero, the
                        sensor report will return the factory default sensor type.
@param[in]	unit	    preferred sensor unit, ZW_SENSOR_UNIT_XXX.  This parameter is ignored
                        if type=0.
@return		ZW_ERR_XXX
@note  Preferred sensor type and unit are not guaranteed to be returned in the report callback.  It
       depends on the multilevel sensor command class version number and the device supported.
*/

typedef void (*zwrep_sensor_sup_fn)(zwifd_p ifd, uint8_t type_len, uint8_t *type);
/**<
report callback for supported sensor types
@param[in]	ifd	       interface
@param[in]	type_len   size of sensor type buffer
@param[in]	type       buffer to store supported sensor types (ZW_SENSOR_TYPE_XXX)
*/

int zwif_sensor_sup_get(zwifd_p ifd, zwrep_sensor_sup_fn cb);
/**<
get the supported sensor types through report callback
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

typedef void (*zwrep_sensor_unit_fn)(zwifd_p ifd, uint8_t sensor_type, uint8_t sensor_unit_msk);
/**<
report callback for supported sensor types
@param[in]	ifd	            interface
@param[in]	sensor_type     sensor type, ZW_SENSOR_TYPE_XXX
@param[in]	sensor_unit_msk bitmask of units supported for the sensor_type, ZW_SENSOR_UNIT_XXX
*/

int zwif_sensor_unit_get(zwifd_p ifd, uint8_t sensor_type, zwrep_sensor_unit_fn cb);
/**<
get the supported sensor units through report callback
@param[in]	ifd	            interface
@param[in]	sensor_type	    sensor type
@param[in]	cb	            report callback function
@return ZW_ERR_XXX
*/

int zwif_sensor_sup_cache_get(zwifd_p ifd, if_sensor_data_t **sup_snsr, uint8_t *snsr_cnt);
/**<
get supported sensor types and units from cache
@param[in]	ifd	        interface
@param[out]	sup_snsr	pointer to array of supported sensors
@param[out]	snsr_cnt	supported sensor counts
@return ZW_ERR_XXX
*/

/**
@}
@defgroup If_Cfg    Device Configuration APIs
Configure device specific parameters
@{
*/

typedef struct
{
    uint8_t     param_num;	    /**< parameter number */
    uint8_t     size;	        /**< data size: 1,2,or 4 bytes*/
    uint8_t     data[4];	    /**< data (a signed number) with the first byte is the most significant byte*/
    uint8_t     use_default;	/**< parameter flag:  1=use default factory setting 0=use the value in data[]*/

}
zwconfig_t, *zwconfig_p;

int zwif_config_set(zwifd_p ifd, zwconfig_p param);
/**<
set configuration parameter
@param[in]	ifd	    interface
@param[in]	param	parameter to set.
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_config_fn)(zwifd_p ifd, zwconfig_p param);
/**<
report callback for configuration parameter
@param[in]	ifd	    interface
@param[in]	param   parameter value
*/

int zwif_config_rpt_set(zwifd_p ifd, zwrep_config_fn rpt_cb);
/**<
setup a configuration parameter report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_config_get(zwifd_p ifd, uint8_t param_num);
/**<
get configuration parameter report through report callback
@param[in]	ifd	        interface
@param[in]	param_num	parameter number of the value to get
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup If_Mtr Meter Interface APIs
Meters can be reset
@{
*/

/** Meter Type definition */
#define ZW_METER_TYPE_ELEC           0x01    /**< Electric meter*/
#define ZW_METER_TYPE_GAS            0x02    /**< Gas meter*/
#define ZW_METER_TYPE_WATER          0x03    /**< Water meter*/


/** Meter Unit definition */
#define ZW_METER_UNIT_ELEC_KWH       0x00    /**< Electric meter unit: kWh*/
#define ZW_METER_UNIT_ELEC_KVAH      0x01    /**< Electric meter unit: kVAh*/
#define ZW_METER_UNIT_ELEC_W         0x02    /**< Electric meter unit: W*/
#define ZW_METER_UNIT_ELEC_PULSE     0x03    /**< Electric meter unit: pulse count*/
#define ZW_METER_UNIT_ELEC_V         0x04    /**< Electric meter unit: V*/
#define ZW_METER_UNIT_ELEC_A         0x05    /**< Electric meter unit: A*/
#define ZW_METER_UNIT_ELEC_PF        0x06    /**< Electric meter unit: power factor*/

#define ZW_METER_UNIT_GAS_CM         0x00    /**< Gas meter unit: cubic meters*/
#define ZW_METER_UNIT_GAS_CF         0x01    /**< Gas meter unit: cubic feet*/
#define ZW_METER_UNIT_GAS_PULSE      0x03    /**< Gas meter unit: pulse count*/

#define ZW_METER_UNIT_WATER_CM       0x00    /**< Water meter unit: cubic meters*/
#define ZW_METER_UNIT_WATER_CF       0x01    /**< Water meter unit: cubic feet*/
#define ZW_METER_UNIT_WATER_GAL      0x02    /**< Water meter unit: US gallons*/
#define ZW_METER_UNIT_WATER_PULSE    0x03    /**< Water meter unit: pulse count*/


/** Meter Supported Unit Bit-mask definition : 1=supported, 0=not supported */
#define ZW_METER_SUP_UNIT_ELEC_KWH       0x01    /**< Electric meter unit: kWh*/
#define ZW_METER_SUP_UNIT_ELEC_KVAH      0x02    /**< Electric meter unit: kVAh*/
#define ZW_METER_SUP_UNIT_ELEC_W         0x04    /**< Electric meter unit: W*/
#define ZW_METER_SUP_UNIT_ELEC_PULSE     0x08    /**< Electric meter unit: pulse count*/
#define ZW_METER_SUP_UNIT_ELEC_V         0x10    /**< Electric meter unit: V*/
#define ZW_METER_SUP_UNIT_ELEC_A         0x20    /**< Electric meter unit: A*/
#define ZW_METER_SUP_UNIT_ELEC_PF        0x40    /**< Electric meter unit: power factor*/

#define ZW_METER_SUP_UNIT_GAS_CM         0x01    /**< Gas meter unit: cubic meters*/
#define ZW_METER_SUP_UNIT_GAS_CF         0x02    /**< Gas meter unit: cubic feet*/
#define ZW_METER_SUP_UNIT_GAS_PULSE      0x08    /**< Gas meter unit: pulse count*/

#define ZW_METER_SUP_UNIT_WATER_CM       0x01    /**< Water meter unit: cubic meters*/
#define ZW_METER_SUP_UNIT_WATER_CF       0x02    /**< Water meter unit: cubic feet*/
#define ZW_METER_SUP_UNIT_WATER_GAL      0x04    /**< Water meter unit: US gallons*/
#define ZW_METER_SUP_UNIT_WATER_PULSE    0x08    /**< Water meter unit: pulse count*/


/** Meter Rate Type definition */
#define ZW_METER_RATE_IMPORT           0x01    /**< Import: Meter Value is a consumed measurement*/
#define ZW_METER_RATE_EXPORT           0x02    /**< Export: Meter Value is a produced measurement*/


#define ZW_ADMIN_STR_MAX    32  /**< maximum number of characters in the administration number*/
#define ZW_ID_STR_MAX       32  /**< maximum number of characters in the identification of customer*/
#define ZW_METER_CAP_MON	1	/**< id & admin number available */
#define ZW_METER_CAP_CFG	2	/**< admin number can be set */

/** meter descriptor */
typedef struct
{
	uint8_t	caps;			                /**< ZW_METER_CAP_xxx or-mask */
	char	id[ZW_ID_STR_MAX + 1];			/**< id, used for identification of customer and metering device */
	char	admin[ZW_ADMIN_STR_MAX + 1];    /**< admin number used to identify customer */
}
zwmeter_t, *zwmeter_p;


/** meter data */
typedef struct
{
    uint8_t     type;		/**< ZW_METER_TYPE_XXX */
    uint8_t     precision;  /**< Decimal places of the value.  The decimal
                                 value 1025 with precision 2 is therefore equal to 10.25.*/
    uint8_t     unit;	    /**< ZW_METER_UNIT_XXX */
    uint8_t     size;	    /**< data size: 1,2,or 4 bytes*/
    uint8_t     data[4];	/**< meter data (a signed number) with the first byte
                                 is the most significant byte*/
    uint16_t    delta_time; /**< elapsed time in seconds between the ‘Meter Value’ and the ‘Previous
                                 Meter Value’ measurements.  If delta_time = 0, it means no ‘Previous
                                 Meter Value’ measurement*/
    uint8_t     prv_data[4];/**< previous meter data (a signed number) with the first byte
                                 is the most significant byte. This field is valid only if delta_time > 0*/
    uint8_t     rate_type;	/**< rate type: ZW_METER_RATE_XXX */

}
zwmeter_dat_t, *zwmeter_dat_p;


/** meter capabilities */
typedef struct
{
    uint8_t     type;		/**< ZW_METER_TYPE_XXX */
    uint8_t     reset_cap;	/**< capability to reset all accumulated values stored in the meter device.
                                 1=reset supported, 0=not supported. See zwif_meter_reset()*/
    uint8_t     unit_sup;   /**< supported unit bit-mask : ZW_METER_SUP_UNIT_XXX */
}
zwmeter_cap_t, *zwmeter_cap_p;


typedef void (*zwrep_meter_fn)(zwifd_p ifd, zwmeter_dat_p data);
/**<
report callback for meter reading
@param[in]	ifd	    interface
@param[in]	data    current value and unit of the meter
*/

typedef void (*zwrep_meterd_fn)(zwifd_p ifd, zwmeter_p meter);
/**<
report callback for meter descriptor
@param[in]	ifd	    interface
@param[in]	meter	meter descriptor
*/

typedef void (*zwrep_meter_sup_fn)(zwifd_p ifd, zwmeter_cap_p meter_cap);
/**<
report callback for meter capabilities
@param[in]	ifd	        interface
@param[in]	meter_cap	meter capabilities
*/

int zwif_meter_rpt_set(zwifd_p ifd, zwrep_meter_fn rpt_cb);
/**<
setup a meter report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_meter_get(zwifd_p ifd, uint8_t unit);
/**<
get meter report through report callback
@param[in]	ifd	        interface
@param[in]	unit	    the preferred unit (ZW_METER_UNIT_XXX). The report may not return
                        the preferred unit if the device doesn't support it.
@return		ZW_ERR_XXX
*/

int zwif_meter_sup_get(zwifd_p ifd,  zwrep_meter_sup_fn cb);
/**<
get information on the meter capabilities through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/

int zwif_meter_reset(zwifd_p ifd);
/**<
reset all accumulated values stored in the meter device
@param[in]	ifd	interface
@return	ZW_ERR_XXX
*/

int zwif_meter_get_desc(zwifd_p ifd, zwrep_meterd_fn cb);
/**<
get meter admin name
@param[in]	ifd	interface
@param[in]	cb	meter descriptor report callback function
@return	ZW_ERR_XXX
*/

int zwif_meter_set_admin(zwifd_p ifd, char *name);
/**<
set meter admin name
@param[in]	ifd	interface
@param[in]	name	admin number
@return	ZW_ERR_XXX
*/


/**
@}
@defgroup If_PlsMtr Pulse Meter Interface APIs
Intended for all kinds of meters that generate pulses.
@{
*/

typedef void (*zwrep_pulsemeter_fn)(zwifd_p ifd, uint32_t cnt);
/**<
report callback for pulsemeter
@param[in]	ifd	interface
@param[in]	cnt	number of pulses detected in the device
*/

int zwif_pulsemeter_rpt_set(zwifd_p ifd, zwrep_pulsemeter_fn rpt_cb);
/**<
setup a pulse meter report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_pulsemeter_get(zwifd_p ifd);
/**<
get pulse meter report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/**
@}
@defgroup If_Av Audio-Video Interface APIs
Audio Video Interface is meant to replace TV/player remotes.
@{
*/

/** Button Type definition */
#define ZW_AV_BTN_MUTE		0x0001
#define ZW_AV_BTN_VOL_DN	0x0002
#define ZW_AV_BTN_VOL_UP	0x0003
#define ZW_AV_BTN_CH_UP		0x0004
#define ZW_AV_BTN_CH_DN		0x0005
#define ZW_AV_BTN_0 		0x0006
#define ZW_AV_BTN_1 		0x0007
#define ZW_AV_BTN_2 		0x0008
#define ZW_AV_BTN_3 		0x0009
#define ZW_AV_BTN_4 		0x000A
#define ZW_AV_BTN_5 		0x000B
#define ZW_AV_BTN_6 		0x000C
#define ZW_AV_BTN_7 		0x000D
#define ZW_AV_BTN_8 		0x000E
#define ZW_AV_BTN_9 		0x000F
#define ZW_AV_BTN_LAST_CH   0x0010
#define ZW_AV_BTN_DISP_INFO	0x0011
#define ZW_AV_BTN_FAV		0x0012
#define ZW_AV_BTN_PLAY		0x0013
#define ZW_AV_BTN_STOP		0x0014
#define ZW_AV_BTN_PAUSE		0x0015
#define ZW_AV_BTN_FWD		0x0016
#define ZW_AV_BTN_REV		0x0017
#define ZW_AV_BTN_REPLAY	0x0018
#define ZW_AV_BTN_REC		0x0019
#define ZW_AV_BTN_AC3		0x001A
#define ZW_AV_BTN_PVR 		0x001B
#define ZW_AV_BTN_EPG		0x001C
#define ZW_AV_BTN_MENU_SET	0x001D
#define ZW_AV_BTN_MENU_UP	0x001E
#define ZW_AV_BTN_MENU_DN	0x001F
#define ZW_AV_BTN_MENU_L	0x0020
#define ZW_AV_BTN_MENU_R	0x0021
#define ZW_AV_BTN_PGUP		0x0022
#define ZW_AV_BTN_PGDN		0x0023
#define ZW_AV_BTN_SEL		0x0024
#define ZW_AV_BTN_EXIT		0x0025
#define ZW_AV_BTN_INPUT		0x0026
#define ZW_AV_BTN_POWER		0x0027
#define ZW_AV_BTN_ENTER		0x0028
#define ZW_AV_BTN_10		0x0029
#define ZW_AV_BTN_11		0x002A
#define ZW_AV_BTN_12		0x002B
#define ZW_AV_BTN_13		0x002C
#define ZW_AV_BTN_14		0x002D
#define ZW_AV_BTN_15		0x002E
#define ZW_AV_BTN_16		0x002F
#define ZW_AV_BTN_INC10		0x0030
#define ZW_AV_BTN_INC20		0x0031
#define ZW_AV_BTN_INC100	0x0032
#define ZW_AV_BTN_CH1_2		0x0033
#define ZW_AV_BTN_3CH		0x0034
#define ZW_AV_BTN_3D		0x0035
#define ZW_AV_BTN_6CH		0x0036
#define ZW_AV_BTN_A		    0x0037
#define ZW_AV_BTN_ADD		0x0038
#define ZW_AV_BTN_ALARM		0x0039
#define ZW_AV_BTN_AM		0x003A
#define ZW_AV_BTN_ANALOG	0x003B
#define ZW_AV_BTN_ANGLE		0x003C
#define ZW_AV_BTN_ANT_EXT	0x003D
#define ZW_AV_BTN_ANT_EAST	0x003E
#define ZW_AV_BTN_ANT_WEST	0x003F
#define ZW_AV_BTN_ASPECT	0x0040
#define ZW_AV_BTN_AUDIO1	0x0041
#define ZW_AV_BTN_AUDIO2	0x0042
#define ZW_AV_BTN_AUDIO3	0x0043
#define ZW_AV_BTN_AUDIO_DUB	0x0044
#define ZW_AV_BTN_AUDIO_DN	0x0045
#define ZW_AV_BTN_AUDIO_UP	0x0046
#define ZW_AV_BTN_AUTO		0x0047
#define ZW_AV_BTN_AUX1		0x0048
#define ZW_AV_BTN_AUX2		0x0049
#define ZW_AV_BTN_B		    0x004A
#define ZW_AV_BTN_BACK		0x004B
#define ZW_AV_BTN_BACKLIGHT	0x004C
#define ZW_AV_BTN_BALANCE	0x004D
#define ZW_AV_BTN_BALANCE_L	0x004E
#define ZW_AV_BTN_BALANCE_R	0x004F
#define ZW_AV_BTN_BW_FM_AM	0x0050
#define ZW_AV_BTN_BW_W_N	0x0051
#define ZW_AV_BTN_BASS		0x0052
#define ZW_AV_BTN_BASS_DN	0x0053
#define ZW_AV_BTN_BASS_UP	0x0054
#define ZW_AV_BTN_BLANK		0x0055
#define ZW_AV_BTN_BREEZE	0x0056
#define ZW_AV_BTN_BRIGHTEN	0x0057
#define ZW_AV_BTN_BRIGHTNESS	0x0058
#define ZW_AV_BTN_BRIGHT_DN	0x0059
#define ZW_AV_BTN_BRIGHT_UP	0x005A
#define ZW_AV_BTN_BUY		0x005B
#define ZW_AV_BTN_C		    0x005C
#define ZW_AV_BTN_CAMERA	0x005D
#define ZW_AV_BTN_CAT_DOWN	0x005E
#define ZW_AV_BTN_CAT_UP	0x005F
#define ZW_AV_BTN_CENTER		0x0060
#define ZW_AV_BTN_CENTER_VOL_DN	0x0061
#define ZW_AV_BTN_CENTER_MODE	0x0062
#define ZW_AV_BTN_CENTER_VOL_UP	0x0063
#define ZW_AV_BTN_CHANNEL	0x0064
#define ZW_AV_BTN_CANCEL	0x0065
#define ZW_AV_BTN_CLOSE		0x0066
#define ZW_AV_BTN_CC		0x0067
#define ZW_AV_BTN_COLD		0x0068
#define ZW_AV_BTN_COLOR		0x0069
#define ZW_AV_BTN_COLOR_DN	0x006A
#define ZW_AV_BTN_COLOR_UP	0x006B
#define ZW_AV_BTN_COMPO1	0x006C
#define ZW_AV_BTN_COMPO2	0x006D
#define ZW_AV_BTN_COMPO3	0x006E
#define ZW_AV_BTN_CONCERT	0x006F
#define ZW_AV_BTN_CONFIRM	0x0070
#define ZW_AV_BTN_CONTINUE	0x0071
#define ZW_AV_BTN_CONTRAST	0x0072
#define ZW_AV_BTN_CONTRAST_DN	0x0073
#define ZW_AV_BTN_CONTRAST_UP	0x0074
#define ZW_AV_BTN_COUNTER	    0x0075
#define ZW_AV_BTN_COUNTER_RESET	0x0076
#define ZW_AV_BTN_D		    0x0077
#define ZW_AV_BTN_DAY_DN	0x0078
#define ZW_AV_BTN_DAY_UP	0x0079
#define ZW_AV_BTN_DELAY		0x007A
#define ZW_AV_BTN_DELAY_DN	0x007B
#define ZW_AV_BTN_DELAY_UP	0x007C
#define ZW_AV_BTN_ERASE		0x007D
#define ZW_AV_BTN_DELIMIT	0x007E
#define ZW_AV_BTN_DIGEST	0x007F
#define ZW_AV_BTN_DIGITAL	0x0080
#define ZW_AV_BTN_DIM		0x0081
#define ZW_AV_BTN_DIRECT	0x0082
#define ZW_AV_BTN_DISARM	0x0083
#define ZW_AV_BTN_DISC		0x0084
#define ZW_AV_BTN_DISC1		0x0085
#define ZW_AV_BTN_DISC2		0x0086
#define ZW_AV_BTN_DISC3		0x0087
#define ZW_AV_BTN_DISC4		0x0088
#define ZW_AV_BTN_DISC5		0x0089
#define ZW_AV_BTN_DISC6		0x008A
#define ZW_AV_BTN_DISC_DN	0x008B
#define ZW_AV_BTN_DISC_UP	0x008C
#define ZW_AV_BTN_DISCO		0x008D
#define ZW_AV_BTN_EDIT		0x008E
#define ZW_AV_BTN_EFFECT_DN	0x008F
#define ZW_AV_BTN_EFFECT_UP	0x0090
#define ZW_AV_BTN_EJECT		0x0091
#define ZW_AV_BTN_END		0x0092
#define ZW_AV_BTN_EQ		0x0093
#define ZW_AV_BTN_FADER		0x0094
#define ZW_AV_BTN_FAN		0x0095
#define ZW_AV_BTN_FAN_HIGH	0x0096
#define ZW_AV_BTN_FAN_LOW	0x0097
#define ZW_AV_BTN_FAN_MEDIUM	0x0098
#define ZW_AV_BTN_FAN_SPEED	    0x0099
#define ZW_AV_BTN_FASTEXT_BLUE	0x009A
#define ZW_AV_BTN_FASTEXT_GREEN	0x009B
#define ZW_AV_BTN_FASTEXT_PURPLE	0x009C
#define ZW_AV_BTN_FASTEXT_RED		0x009D
#define ZW_AV_BTN_FASTEXT_WHITE	    0x009E
#define ZW_AV_BTN_FASTEXT_YELLOW	0x009F
#define ZW_AV_BTN_FAV_CH_DN	0x00A0
#define ZW_AV_BTN_FAV_CH_UP	0x00A1
#define ZW_AV_BTN_FINALIZE	0x00A2
#define ZW_AV_BTN_FINE TUNE	0x00A3
#define ZW_AV_BTN_FLAT		0x00A4
#define ZW_AV_BTN_FM		0x00A5
#define ZW_AV_BTN_FOCUS_DN	0x00A6
#define ZW_AV_BTN_FOCUS_UP	0x00A7
#define ZW_AV_BTN_FREEZE	0x00A8
#define ZW_AV_BTN_FRONT		0x00A9
#define ZW_AV_BTN_GAME		0x00AA
#define ZW_AV_BTN_GOTO		0x00AB
#define ZW_AV_BTN_HALL		0x00AC
#define ZW_AV_BTN_HEAT		0x00AD
#define ZW_AV_BTN_HELP		0x00AE
#define ZW_AV_BTN_HOME		0x00AF
#define ZW_AV_BTN_INDEX_VISS	0x00B0
#define ZW_AV_BTN_INDEX_FWD	0x00B1
#define ZW_AV_BTN_INDEX_REV	0x00B2
#define ZW_AV_BTN_PLANNER	0x00B3
#define ZW_AV_BTN_INTRO_SCAN   0x00B4
#define ZW_AV_BTN_JAZZ		0x00B5
#define ZW_AV_BTN_KARAOKE	0x00B6
#define ZW_AV_BTN_KEYSTONE	0x00B7
#define ZW_AV_BTN_KEYSTONE_DN	0x00B8
#define ZW_AV_BTN_KEYSTONE_UP	0x00B9
#define ZW_AV_BTN_LANGUAGE	0x00BA
#define ZW_AV_BTN_LCLICK	0x00BB
#define ZW_AV_BTN_VOLUME	0x00BC
#define ZW_AV_BTN_LIGHT		0x00BD
#define ZW_AV_BTN_LIST		0x00BE
#define ZW_AV_BTN_LIVE		0x00BF
#define ZW_AV_BTN_LOCAL		0x00C0
#define ZW_AV_BTN_LOUDNESS	0x00C1
#define ZW_AV_BTN_MAIL		0x00C2
#define ZW_AV_BTN_MARK		0x00C3
#define ZW_AV_BTN_RECALL	0x00C4
#define ZW_AV_BTN_MONITOR	0x00C5
#define ZW_AV_BTN_MOVIE		0x00C6
#define ZW_AV_BTN_MULTI		0x00C7
#define ZW_AV_BTN_MUSIC		0x00C8
#define ZW_AV_BTN_MUSIC_SCAN	0x00C9
#define ZW_AV_BTN_NATURAL	0x00CA
#define ZW_AV_BTN_NIGHT		0x00CB
#define ZW_AV_BTN_DOLBYNR	0x00CC
#define ZW_AV_BTN_NORMALIZE	0x00CD
#define ZW_AV_BTN_INPUT_CATV	0x00CE
#define ZW_AV_BTN_INPUT_CD1 	0x00CF
#define ZW_AV_BTN_INPUT_CD2	0x00D0
#define ZW_AV_BTN_INPUT_CDR	0x00D1
#define ZW_AV_BTN_INPUT_DAT	0x00D2
#define ZW_AV_BTN_INPUT_DVD	0x00D3
#define ZW_AV_BTN_INPUT_DVI	0x00D4
#define ZW_AV_BTN_INPUT_HDTV	0x00D5
#define ZW_AV_BTN_INPUT_LD	0x00D6
#define ZW_AV_BTN_INPUT_MD	0x00D7
#define ZW_AV_BTN_INPUT_PC	0x00D8
#define ZW_AV_BTN_INPUT_PVR	0x00D9
#define ZW_AV_BTN_INPUT_TV	0x00DA
#define ZW_AV_BTN_INPUT_TV_VCR	0x00DB
#define ZW_AV_BTN_INPUT_VCR	0x00DC
#define ZW_AV_BTN_OTPB		0x00DD
#define ZW_AV_BTN_OTR		0x00DE
#define ZW_AV_BTN_OPEN		0x00DF
#define ZW_AV_BTN_OPTICAL	0x00E0
#define ZW_AV_BTN_OPTIONS	0x00E1
#define ZW_AV_BTN_ORCHESTRA	0x00E2
#define ZW_AV_BTN_PAL_NTSC	0x00E3
#define ZW_AV_BTN_PARENTAL	0x00E4
#define ZW_AV_BTN_PBC		0x00E5
#define ZW_AV_BTN_PHONO		0x00E6
#define ZW_AV_BTN_PHOTO		0x00E7
#define ZW_AV_BTN_PIC_MENU	0x00E8
#define ZW_AV_BTN_PIC_MODE	0x00E9
#define ZW_AV_BTN_PIC_MUTE	0x00EA
#define ZW_AV_BTN_PIP_CHDN	0x00EB
#define ZW_AV_BTN_PIP_CHUP	0x00EC
#define ZW_AV_BTN_PIP_FREEZE	0x00ED
#define ZW_AV_BTN_PIP_INPUT	0x00EE
#define ZW_AV_BTN_PIP_MOVE	0x00EF
#define ZW_AV_BTN_PIP_OFF	0x00F0
#define ZW_AV_BTN_PIP_ON	0x00F1
#define ZW_AV_BTN_PIP_SIZE	0x00F2
#define ZW_AV_BTN_PIP_SPLIT	0x00F3
#define ZW_AV_BTN_PIP_SWAP	0x00F4
#define ZW_AV_BTN_PLAY_MODE	0x00F5
#define ZW_AV_BTN_PLAY_REV	0x00F6
#define ZW_AV_BTN_PWR_OFF	0x00F7
#define ZW_AV_BTN_PWR_ON	0x00F8
#define ZW_AV_BTN_PPV		0x00F9
#define ZW_AV_BTN_PRESET	0x00FA
#define ZW_AV_BTN_PROGRAM	0x00FB
#define ZW_AV_BTN_PROGRESSIVE	0x00FC
#define ZW_AV_BTN_PROLOGIC	0x00FD
#define ZW_AV_BTN_PTY		0x00FE
#define ZW_AV_BTN_SKIP		0x00FF
#define ZW_AV_BTN_SHUFFLE	0x0100
#define ZW_AV_BTN_RDS		0x0101
#define ZW_AV_BTN_REAR		0x0102
#define ZW_AV_BTN_REAR_VOLDN	0x0103
#define ZW_AV_BTN_REAR_VOLUP	0x0104
#define ZW_AV_BTN_REC_MUTE	0x0105
#define ZW_AV_BTN_REC_PAUSE	0x0106
#define ZW_AV_BTN_REPEAT	0x0107
#define ZW_AV_BTN_REPEAT_AB	0x0108
#define ZW_AV_BTN_RESUME	0x0109
#define ZW_AV_BTN_RGB		0x010A
#define ZW_AV_BTN_RCLICK	0x010B
#define ZW_AV_BTN_ROCK		0x010C
#define ZW_AV_BTN_ROT_L		0x010D
#define ZW_AV_BTN_ROT_R		0x010E
#define ZW_AV_BTN_SAT		0x010F
#define ZW_AV_BTN_CH_SCAN	0x0110
#define ZW_AV_BTN_SCART		0x0111
#define ZW_AV_BTN_SCENE		0x0112
#define ZW_AV_BTN_SCROLL	0x0113
#define ZW_AV_BTN_SERVICES	0x0114
#define ZW_AV_BTN_SETUP			0x0115
#define ZW_AV_BTN_SHARP			0x0116
#define ZW_AV_BTN_SHARPNESS		0x0117
#define ZW_AV_BTN_SHARP_DN		0x0118
#define ZW_AV_BTN_SHARP_UP		0x0119
#define ZW_AV_BTN_SIDE_AB		0x011A
#define ZW_AV_BTN_NEXT			0x011B
#define ZW_AV_BTN_PREV			0x011C
#define ZW_AV_BTN_SLEEP_OFF		0x011D
#define ZW_AV_BTN_SLOW			0x011E
#define ZW_AV_BTN_SLOW_FWD		0x011F
#define ZW_AV_BTN_SLOW_REV		0x0120
#define ZW_AV_BTN_SOUND_MENU	0x0121
#define ZW_AV_BTN_SOUND_MODE	0x0122
#define ZW_AV_BTN_SPEED_RECORD	0x0123
#define ZW_AV_BTN_SPEED_DN		0x0124
#define ZW_AV_BTN_SPEED_UP		0x0125
#define ZW_AV_BTN_SPORTS		0x0126
#define ZW_AV_BTN_STADIUM		0x0127
#define ZW_AV_BTN_START			0x0128
#define ZW_AV_BTN_START_ERASE		0x0129
#define ZW_AV_BTN_START_RENUMBER	0x012A
#define ZW_AV_BTN_START_WRITE		0x012B
#define ZW_AV_BTN_STEP			0x012C
#define ZW_AV_BTN_STEREO_MONO	0x012D
#define ZW_AV_BTN_STILL_FWD		0x012E
#define ZW_AV_BTN_STILL_REV		0x012F
#define ZW_AV_BTN_SUBTITLE		0x0130
#define ZW_AV_BTN_SUBWOOFER_DN	0x0131
#define ZW_AV_BTN_SUBWOOFER_UP	0x0132
#define ZW_AV_BTN_SUPER_BASS	0x0133
#define ZW_AV_BTN_SURROUND		0x0134
#define ZW_AV_BTN_SURROUND_MODE	0x0135
#define ZW_AV_BTN_SVIDEO		0x0136
#define ZW_AV_BTN_SWEEP			0x0137
#define ZW_AV_BTN_SYNCHRO		0x0138
#define ZW_AV_BTN_TAPE1DECK 1		0x0139
#define ZW_AV_BTN_TAPE1_2DECK1_2	0x013A
#define ZW_AV_BTN_TAPE2DECK2		0x013B
#define ZW_AV_BTN_TEMPERATURE_DOWN	0x013C
#define ZW_AV_BTN_TEMPERATURE_UP	0x013D
#define ZW_AV_BTN_TEST_TONE		0x013E
#define ZW_AV_BTN_TEXT_TELETEXT	0x013F
#define ZW_AV_BTN_TEXT_EXPAND	0x0140
#define ZW_AV_BTN_TEXT_HOLD		0x0141
#define ZW_AV_BTN_TEXT_INDEX	0x0142
#define ZW_AV_BTN_TEXT_MIX		0x0143
#define ZW_AV_BTN_TEXT_OFF		0x0144
#define ZW_AV_BTN_TEXT_REVEAL	0x0145
#define ZW_AV_BTN_TEXT_SUBPAGE	0x0146
#define ZW_AV_BTN_TEXT_TIMED_PAGE	0x0147
#define ZW_AV_BTN_TEXT CANCEL		0x0148
#define ZW_AV_BTN_THEATER 	0x0149
#define ZW_AV_BTN_THEME		0x014A
#define ZW_AV_BTN_THUMBS_DN	0x014B
#define ZW_AV_BTN_THUMBS_UP	0x014C
#define ZW_AV_BTN_TILT_DN	0x014D
#define ZW_AV_BTN_TILT_UP	0x014E
#define ZW_AV_BTN_TIME		0x014F
#define ZW_AV_BTN_TIMER		0x0150
#define ZW_AV_BTN_TIMER_DN	0x0151
#define ZW_AV_BTN_TIMER_UP	0x0152
#define ZW_AV_BTN_TINT		0x0153
#define ZW_AV_BTN_TINT_DN	0x0154
#define ZW_AV_BTN_TINT_UP	0x0155
#define ZW_AV_BTN_TITLE		0x0156
#define ZW_AV_BTN_TRACK		0x0157
#define ZW_AV_BTN_TRACKING	0x0158
#define ZW_AV_BTN_TRACKING_DN	0x0159
#define ZW_AV_BTN_TRACKING_UP	0x015A
#define ZW_AV_BTN_TREBLE	0x015B
#define ZW_AV_BTN_TREBLE_DN	0x015C
#define ZW_AV_BTN_TREBLE_UP	0x015D
#define ZW_AV_BTN_TUNE_DN	0x015E
#define ZW_AV_BTN_TUNE_UP	0x015F
#define ZW_AV_BTN_TUNER		0x0160
#define ZW_AV_BTN_SHOWVIEW	0x0161
#define ZW_AV_BTN_AV1		0x0162
#define ZW_AV_BTN_AV2		0x0163
#define ZW_AV_BTN_AV3		0x0164
#define ZW_AV_BTN_AV4		0x0165
#define ZW_AV_BTN_AV5		0x0166
#define ZW_AV_BTN_VIEW		0x0167
#define ZW_AV_BTN_VOICE		0x0168
#define ZW_AV_BTN_ZOOM		0x0169
#define ZW_AV_BTN_ZOOM_IN	0x016A
#define ZW_AV_BTN_ZOOM_OUT	0x016B
#define ZW_AV_BTN_365		0x016C
#define ZW_AV_BTN_DVD_MENU	0x016E
#define ZW_AV_BTN_MYTV		0x016F
#define ZW_AV_BTN_RECORDEDTV	0x0170
#define ZW_AV_BTN_MYVIDEOS2	    0x0171
#define ZW_AV_BTN_DVDANGLE2	    0x0172
#define ZW_AV_BTN_DVDAUDIO2	    0x0173
#define ZW_AV_BTN_DVDSUB	    0x0174
#define ZW_AV_BTN_RADIO			0x0175
#define ZW_AV_BTN_HASH			0x0176
#define ZW_AV_BTN_STAR			0x0177
#define ZW_AV_BTN_OEM1			0x0178
#define ZW_AV_BTN_OEM2			0x0179
#define ZW_AV_BTN_RQT_INFO		0x017A
#define ZW_AV_BTN_CAPS_NUM		0x017B
#define ZW_AV_BTN_TV_MODE		0x017C
#define ZW_AV_BTN_SOURCE		0x017D
#define ZW_AV_BTN_FILE			0x017E
#define ZW_AV_BTN_SEEK			0x017F
#define ZW_AV_BTN_MOUSEON		0x0180
#define ZW_AV_BTN_MOUSEOFF		0x0181
#define ZW_AV_BTN_VOD			0x0182
#define ZW_AV_BTN_THUMBS_UP_GUI	0x0183
#define ZW_AV_BTN_THUMBS_DN_GUI	0x0184
#define ZW_AV_BTN_APPS			0x0185
#define ZW_AV_BTN_MOUSE			0x0186
#define ZW_AV_BTN_TV			0x0187
#define ZW_AV_BTN_DVD			0x0188
#define ZW_AV_BTN_STB			0x0189
#define ZW_AV_BTN_AUX			0x018A
#define ZW_AV_BTN_BLURAY		0x018B
#define ZW_AV_BTN_STANDBY1		0x0194
#define ZW_AV_BTN_STANDBY2		0x0195
#define ZW_AV_BTN_STANDBY3		0x0196
#define ZW_AV_BTN_HDMI1			0x0197
#define ZW_AV_BTN_HDMI2			0x0198
#define ZW_AV_BTN_HDMI3			0x0199
#define ZW_AV_BTN_HDMI4			0x019A
#define ZW_AV_BTN_HDMI5			0x019B
#define ZW_AV_BTN_HDMI6			0x019C
#define ZW_AV_BTN_HDMI7			0x019D
#define ZW_AV_BTN_HDMI8			0x019E
#define ZW_AV_BTN_HDMI9			0x019F
#define ZW_AV_BTN_USB1			0x01A0
#define ZW_AV_BTN_USB2			0x01A1
#define ZW_AV_BTN_USB3			0x01A2
#define ZW_AV_BTN_USB4			0x01A3
#define ZW_AV_BTN_USB5			0x01A4
#define ZW_AV_BTN_ZOOM4_3_NORMAL	0x01A5
#define ZW_AV_BTN_ZOOM4_3_ZOOM	    0x01A6
#define ZW_AV_BTN_ZOOM16_9_NORMAL	0x01A7
#define ZW_AV_BTN_ZOOM16_9_ZOOM	    0x01A8
#define ZW_AV_BTN_ZOOM16_9_WIDE1	0x01A9
#define ZW_AV_BTN_ZOOM16_9_WIDE2	0x01AA
#define ZW_AV_BTN_ZOOM16_9_WIDE3	0x01AB
#define ZW_AV_BTN_ZOOM16_9_CINEMA	0x01AC
#define ZW_AV_BTN_ZOOM_DEFAULT	    0x01AD
#define ZW_AV_BTN_AUTO_ZOOM		    0x01B0
#define ZW_AV_BTN_ZOOM_SET_DEFAULT	0x01B1
#define ZW_AV_BTN_MUTE_ON		    0x01B2
#define ZW_AV_BTN_MUTE_OFF		    0x01B3
#define ZW_AV_BTN_AUMODE_AUDYSSEY_OFF	0x01B4
#define ZW_AV_BTN_AUMODE_AUDYSSEY_LO	0x01B5
#define ZW_AV_BTN_AUMODE_AUDYSSEY_MED	0x01B6
#define ZW_AV_BTN_AUMODE_AUDYSSEY_HI	0x01B7
#define ZW_AV_BTN_AUMODE_SRS_ON	    0x01BA
#define ZW_AV_BTN_AUMODE_SRS_OFF	0x01BB
#define ZW_AV_BTN_PICMODE_HOME	    0x01BF
#define ZW_AV_BTN_PICMODE_RETAIL	0x01C0
#define ZW_AV_BTN_PICMODE_VIVID	    0x01C1
#define ZW_AV_BTN_PICMODE_STANDARD	0x01C2
#define ZW_AV_BTN_PICMODE_THEATER	0x01C3
#define ZW_AV_BTN_PICMODE_SPORTS	0x01C4
#define ZW_AV_BTN_PICMODE_ENERGY	0x01C5
#define ZW_AV_BTN_PICMODE_CUSTOM	0x01C6
#define ZW_AV_BTN_COOL			0x01C7
#define ZW_AV_BTN_MEDIUM		0x01C8
#define ZW_AV_BTN_WARM_D65		0x01C9
#define ZW_AV_BTN_CCON			0x01CA
#define ZW_AV_BTN_CCOFF			0x01CB
#define ZW_AV_BTN_VMUTE			0x01CC
#define ZW_AV_BTN_VUNMUTE		0x01CD
#define ZW_AV_BTN_NEXTEV		0x01CE
#define ZW_AV_BTN_PREVEV		0x01CF
#define ZW_AV_BTN_CEC			0x01D0
#define ZW_AV_BTN_MTS			0x01D1

typedef void (*zwrep_av_fn)(zwifd_p ifd, uint16_t length, uint8_t *mask);
/**<
report callback for av capabilities
@param[in]	ifd	    interface
@param[in]	length	mask byte length
@param[in]	mask	mask buffer pointer
*/

int zwif_av_caps (zwifd_p ifd, zwrep_av_fn cb);
/**<
get supported AV commands
@param[in]	ifd	interface
@param[in]	cb	av capabilities report callback function.
@return	ZW_ERR_XXX
*/

int zwif_av_set(zwifd_p ifd, uint16_t ctl, uint8_t down);
/**<
press AV button
@param[in]	ifd	    interface
@param[in]	ctl		button number ZW_AV_BTN_XX
@param[in]	down	0=button up, else button down
@return	ZW_ERR_XXX
*/

/**
@}
@defgroup If_Basic Basic Interface APIs
Basic command that can be used to control the basic functionality of a device
@{
*/

int zwif_basic_set(zwifd_p ifd, uint8_t v);
/**<
set basic value
@param[in]	ifd		interface
@param[in]	v		value (the range of value is device specific)
@return	ZW_ERR_XXX
*/

int zwif_basic_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb);
/**<
setup a basic report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/

int zwif_basic_get(zwifd_p ifd);
/**<
get basic report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/


/**
@}
@defgroup If_Doorlock Door Lock Interface APIs
Used to secure/unsecure a lock type as well as setting the configuration of an advanced Z-Wave door lock device
@{
*/

/** Door lock operation mode */
#define ZW_DOOR_UNSEC           0x00    /**< Door unsecured*/
#define ZW_DOOR_UNSEC_TMOUT     0x01    /**< Door unsecured with timeout*/
#define ZW_DOOR_UNSEC_IN        0x10    /**< Door unsecured for inside door handles*/
#define ZW_DOOR_UNSEC_IN_TMOUT  0x11    /**< Door unsecured for inside door handles with timeout*/
#define ZW_DOOR_UNSEC_OUT       0x20    /**< Door unsecured for outside door handles*/
#define ZW_DOOR_UNSEC_OUT_TMOUT 0x21    /**< Door unsecured for outside door handles with timeout*/
#define ZW_DOOR_SEC             0xFF    /**< Door secured*/

int zwif_dlck_op_set(zwifd_p ifd, uint8_t mode);
/**<
set door lock operation
@param[in]	ifd	    interface
@param[in]	mode	operation mode (ZW_DOOR_XXX).
@return	ZW_ERR_XXX
*/

/** Door condition bit-mask */
#define ZW_COND_DOOR_MASK       0x01    /**< Bit-mask for door status.
                                             After masking bit=0 means door open; else closed*/
#define ZW_COND_BOLT_MASK       0x02    /**< Bit-mask for bolt status.
                                             After masking bit=0 means bolt locked; else unlocked*/
#define ZW_COND_LATCH_MASK      0x04    /**< Bit-mask for latch status.
                                             After masking bit=0 means latch open; else closed*/

typedef struct
{
    uint8_t     mode;           /**< Door lock operation mode (ZW_DOOR_XXX) */
    uint8_t     out_mode;       /**< Outside door handles mode. It's a 4-bit mask; bit=0 for inactive, bit=1 for active*/
    uint8_t     in_mode;        /**< Inside door handles mode. It's a 4-bit mask; bit=0 for inactive, bit=1 for active*/
    uint8_t     cond;           /**< Door condition. See ZW_COND_XXX_MASK */
    uint8_t     tmout_min;      /**< Lock timeout in minutes. Value of 0xFE means timeout is not supported*/
    uint8_t     tmout_sec;      /**< Lock timeout in seconds. Value of 0xFE means timeout is not supported*/
}
zwdlck_op_t, *zwdlck_op_p;


typedef void (*zwrep_dlck_op_fn)(zwifd_p ifd, zwdlck_op_p  op_sts);
/**<
report callback for door lock operation
@param[in]	ifd	        interface
@param[in]	op_sts      operation status
*/

int zwif_dlck_op_rpt_set(zwifd_p ifd, zwrep_dlck_op_fn rpt_cb);
/**<
setup a door lock operation report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_dlck_op_get(zwifd_p ifd);
/**<
get the state of the door lock device through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/** Door lock operation type */
#define ZW_DOOR_OP_CONST    0x01    /**< Constant operation*/
#define ZW_DOOR_OP_TIMED    0x02    /**< Timed operation*/

typedef struct
{
    uint8_t     type;           /**< Door lock operation type (ZW_DOOR_OP_XXX) */
    uint8_t     out_sta;        /**< Outside door handles state. It's a 4-bit mask; bit=0 for disable, bit=1 for enable*/
    uint8_t     in_sta;         /**< Inside door handles state. It's a 4-bit mask; bit=0 for disable, bit=1 for enable*/
    uint8_t     tmout_min;      /**< Lock timeout in minutes. Value of 0xFE means timeout is not supported*/
    uint8_t     tmout_sec;      /**< Lock timeout in seconds. Value of 0xFE means timeout is not supported*/
}
zwdlck_cfg_t, *zwdlck_cfg_p;

typedef void (*zwrep_dlck_cfg_fn)(zwifd_p ifd, zwdlck_cfg_p  config);
/**<
report callback for door lock configuration
@param[in]	ifd	    interface
@param[in]	config	configuration
*/

int zwif_dlck_cfg_set(zwifd_p ifd, zwdlck_cfg_p config);
/**<
set the configuration of the door lock device
@param[in]	ifd	    interface
@param[in]	config	configuration
@return	ZW_ERR_XXX
*/

int zwif_dlck_cfg_get(zwifd_p ifd, zwrep_dlck_cfg_fn cb);
/**<
get configuration parameter through report callback
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/


/**
@}
@defgroup If_Usrcode User Code Interface APIs
Used to manage user codes required to unlock a lock
@{
*/

/** User ID status */
#define ZW_USRCOD_AVAIL         0x00    /**< Available (not set)*/
#define ZW_USRCOD_UNAVAIL       0x01    /**< Occupied*/
#define ZW_USRCOD_RSVD          0x02    /**< Reserved by administrator*/
#define ZW_USRCOD_NO_STS        0xFE    /**< Status unavailable*/

#define MAX_USRCOD_LENGTH       10      /**< Max. user code length*/
typedef struct
{
    uint8_t     id;                     /**< User identifier */
    uint8_t     id_sts;                 /**< User id status*/
    uint8_t     code_len;               /**< User code length*/
    uint8_t     code[MAX_USRCOD_LENGTH];/**< User code; minimum length = 4, maximum length = 10*/
}
zwusrcod_t, *zwusrcod_p;

typedef void (*zwrep_usr_cod_fn)(zwifd_p ifd, zwusrcod_p  usr_cod);
/**<
report callback for user code
@param[in]	ifd	        interface
@param[in]	usr_cod     user code and its status
*/

typedef void (*zwrep_usr_sup_fn)(zwifd_p ifd, uint8_t  usr_num);
/**<
report callback for number of supported user codes
@param[in]	ifd	        interface
@param[in]	usr_num     number of supported user codes
*/

int zwif_usrcod_set(zwifd_p ifd, zwusrcod_p usr_cod);
/**<
set the user code
@param[in]	ifd	    interface
@param[in]	usr_cod user code and its status
@return	ZW_ERR_XXX
*/

int zwif_usrcod_get(zwifd_p ifd, uint8_t usr_id, zwrep_usr_cod_fn cb);
/**<
get the specified user code and its status
@param[in]	ifd	    interface
@param[in]	usr_id  user identifier
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

int zwif_usrcod_sup_get(zwifd_p ifd, zwrep_usr_sup_fn cb);
/**<
get the number of supported user codes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/


/**
@}
@defgroup If_Alarm Alarm Interface APIs
Used to report alarm or service conditions
@{
*/

/** Z-wave alarm type */
#define ZW_ALRM_SMOKE       0x01    /**< Smoke alarm*/
#define ZW_ALRM_CO          0x02    /**< Carbon monoxide alarm*/
#define ZW_ALRM_CO2         0x03    /**< Carbon dioxide alarm*/
#define ZW_ALRM_HEAT        0x04    /**< Heat alarm*/
#define ZW_ALRM_WATER       0x05    /**< Water alarm*/
#define ZW_ALRM_LOCK        0x06    /**< Lock access control alarm*/
#define ZW_ALRM_BURGLAR     0x07    /**< Burglar alarm or home security*/
#define ZW_ALRM_POWER       0x08    /**< Power management alarm*/
#define ZW_ALRM_SYSTEM      0x09    /**< System alarm*/
#define ZW_ALRM_EMERGENCY   0x0A    /**< Emergency alarm*/
#define ZW_ALRM_CLOCK       0x0B    /**< Alarm clock*/
#define ZW_ALRM_FIRST       0xFF    /**< Used by the zwif_alrm_get() to retrieve the
                                         first alarm detection from the supported list*/
/** Z-wave alarm event for all*/
#define ZW_ALRM_EVT_UNKNOWN             0xFE    /**< Unknown event*/

/** Z-wave alarm event for smoke alarm*/
#define ZW_ALRM_EVT_SMOKE_L             0x01    /**< Smoke detected with location info*/
#define ZW_ALRM_EVT_SMOKE               0x02    /**< Smoke detected with unknown location info*/
#define ZW_ALRM_EVT_SMOKE_TEST          0x03    /**< Smoke alarm test*/

/** Z-wave alarm event for carbon monoxide alarm*/
#define ZW_ALRM_EVT_CO_L                0x01    /**< Carbon monoxide detected with location info*/
#define ZW_ALRM_EVT_CO                  0x02    /**< Carbon monoxide detected with unknown location info*/

/** Z-wave alarm event for carbon dioxide alarm*/
#define ZW_ALRM_EVT_CO2_L               0x01    /**< Carbon dioxide detected with location info*/
#define ZW_ALRM_EVT_CO2                 0x02    /**< Carbon dioxide detected with unknown location info*/

/** Z-wave alarm event for heat alarm*/
#define ZW_ALRM_EVT_OVERHEAT_L          0x01    /**< Overheat detected with location info*/
#define ZW_ALRM_EVT_OVERHEAT            0x02    /**< Overheat detected with unknown location info*/
#define ZW_ALRM_EVT_TEMP_RISE_L         0x03    /**< Rapid temperature rise detected with location info*/
#define ZW_ALRM_EVT_TEMP_RISE           0x04    /**< Rapid temperature rise detected with unknown location info*/
#define ZW_ALRM_EVT_UNDRHEAT_L          0x05    /**< Underheat detected with location info*/
#define ZW_ALRM_EVT_UNDRHEAT            0x06    /**< Underheat detected with unknown location info*/

/** Z-wave alarm event for water alarm*/
#define ZW_ALRM_EVT_LEAK_L              0x01    /**< Water leak detected with location info*/
#define ZW_ALRM_EVT_LEAK                0x02    /**< Water leak detected with unknown location info*/
#define ZW_ALRM_EVT_LVL_L               0x03    /**< Water level dropped detected with location info*/
#define ZW_ALRM_EVT_LVL                 0x04    /**< Water level dropped detected with unknown location info*/

/** Z-wave alarm event for lock access control alarm*/
#define ZW_ALRM_EVT_MANUAL_LCK          0x01    /**< Manual lock operation*/
#define ZW_ALRM_EVT_MANUAL_ULCK         0x02    /**< Manual unlock operation*/
#define ZW_ALRM_EVT_RF_LCK              0x03    /**< RF lock operation*/
#define ZW_ALRM_EVT_RF_ULCK             0x04    /**< RF unlock operation*/
#define ZW_ALRM_EVT_KEYPAD_LCK          0x05    /**< Keypad lock operation with user identifier info*/
#define ZW_ALRM_EVT_KEYPAD_ULCK         0x06    /**< Keypad unlock operation with user identifier info*/
#define ZW_ALRM_EVT_MANUAL_NOT_FUL_LCK  0x07    /**< Manual not fully locked operation*/
#define ZW_ALRM_EVT_RF_NOT_FUL_LCK      0x08    /**< RF not fully loced operation*/
#define ZW_ALRM_EVT_AUTO_LCK            0x09    /**< Auto lock locked operation*/
#define ZW_ALRM_EVT_AUTO_NOT_FUL_OPER   0x0A    /**< Auto lock not fully operation*/
#define ZW_ALRM_EVT_LCK_JAMMED          0x0B    /**< Lock jammed*/
#define ZW_ALRM_EVT_ALL_CODE_DEL        0x0C    /**< All user codes deleted*/
#define ZW_ALRM_EVT_1_CODE_DEL          0x0D    /**< Single user code deleted*/
#define ZW_ALRM_EVT_CODE_ADDED          0x0E    /**< New user code added*/
#define ZW_ALRM_EVT_CODE_DUP            0x0F    /**< New user code not added due to duplicate code*/
#define ZW_ALRM_EVT_KEYPAD_DISABLED     0x10    /**< Keypad temporary disabled*/
#define ZW_ALRM_EVT_KEYPAD_BUSY         0x11    /**< Keypad busy*/
#define ZW_ALRM_EVT_NEW_PROG_CODE       0x12    /**< New program code entered - unique code for lock configuration*/
#define ZW_ALRM_EVT_USR_CODE_LIMIT      0x13    /**< Manually enter user access code exceeds code limit*/
#define ZW_ALRM_EVT_RF_ULCK_INVLD_CODE  0x14    /**< Unlock by RF with invalid user code*/
#define ZW_ALRM_EVT_RF_LCK_INVLD_CODE   0x15    /**< Locked by RF with invalid user code*/
#define ZW_ALRM_EVT_WINDOW_DOOR_OPEN    0x16    /**< Window/door is open*/
#define ZW_ALRM_EVT_WINDOW_DOOR_CLOSED  0x17    /**< Window/door is closed*/


/** Z-wave alarm event for burglar alarm*/
#define ZW_ALRM_EVT_INTRUSION_L         0x01    /**< Intrusion detected with location info*/
#define ZW_ALRM_EVT_INTRUSION           0x02    /**< Intrusion detected with unknown location info*/
#define ZW_ALRM_EVT_TMPR_COVER          0x03    /**< Tampering, product covering removed*/
#define ZW_ALRM_EVT_TMPR_CODE           0x04    /**< Tampering, Invalid Code*/
#define ZW_ALRM_EVT_GLASS_L             0x05    /**< Glass breakage detected with location info*/
#define ZW_ALRM_EVT_GLASS               0x06    /**< Glass breakage detected with unknown location info*/
#define ZW_ALRM_EVT_MOTION_DET_L        0x07    /**< Motion detected with location info*/

/** Z-wave alarm event for power management alarm*/
#define ZW_ALRM_EVT_POWER               0x01    /**< Power has been applied*/
#define ZW_ALRM_EVT_AC_OFF              0x02    /**< AC mains disconnected*/
#define ZW_ALRM_EVT_AC_ON               0x03    /**< AC mains re-connected*/
#define ZW_ALRM_EVT_SURGE               0x04    /**< Surge Detection*/
#define ZW_ALRM_EVT_VOLT_DROP           0x05    /**< Voltage Drop/Drift detected*/
#define ZW_ALRM_EVT_OVER_CURRENT        0x06    /**< Over-current detected*/
#define ZW_ALRM_EVT_OVER_VOLT           0x07    /**< Over-voltage detected*/
#define ZW_ALRM_EVT_OVER_LOAD           0x08    /**< Over-load detected*/
#define ZW_ALRM_EVT_LOAD_ERR            0x09    /**< Load error*/
#define ZW_ALRM_EVT_REPLACE_BATT_SOON   0x0A    /**< Replace battery soon*/
#define ZW_ALRM_EVT_REPLACE_BATT_NOW    0x0B    /**< Replace battery now*/

/** Z-wave alarm event for system alarm*/
#define ZW_ALRM_EVT_HW                  0x01    /**< System hardware failure*/
#define ZW_ALRM_EVT_SW                  0x02    /**< System software failure*/
#define ZW_ALRM_EVT_HW_OEM_CODE         0x03    /**< System hardware failure with OEM proprietary failure code*/
#define ZW_ALRM_EVT_SW_OEM_CODE         0x04    /**< System software failure with OEM proprietary failure code*/

/** Z-wave alarm event for emergency alarm*/
#define ZW_ALRM_EVT_POLICE              0x01    /**< Contact police*/
#define ZW_ALRM_EVT_FIRE                0x02    /**< Contact fire service*/
#define ZW_ALRM_EVT_MEDICAL             0x03    /**< Contact medical service*/

/** Z-wave alarm event for alarm clock*/
#define ZW_ALRM_EVT_WKUP                0x01    /**< Wake up aler*/
#define ZW_ALRM_EVT_TIMER_ENDED         0x02    /**< Timer ended*/

/** Z-wave alarm parameter type*/
#define ZW_ALRM_PARAM_LOC               1    /**< node location UTF-8 string (NULL terminated)*/
#define ZW_ALRM_PARAM_USRID             2    /**< user id*/
#define ZW_ALRM_PARAM_OEM_ERR_CODE      3    /**< OEM proprietary system failure code */

/** Z-wave alarm status*/
#define ZW_ALRM_STS_DEACTIVATED         0    /**< Unsolicited alarm report is deactivated*/
#define ZW_ALRM_STS_ACTIVATED           0xFF /**< Unsolicited alarm report is activated or a pending
                                                  notification is present*/
#define ZW_ALRM_STS_NO_PEND_NOTICE      0xFE /**< No pending notification */


typedef struct
{
    uint8_t     type;           /**< Vendor specific alarm type*/
    uint8_t     level;          /**< Vendor specific alarm level*/
    uint8_t     ex_info;        /**< Flag to indicate the following extended info fields are valid.
                                     1= valid; 0= invalid (no extended info fields)*/
    /*extended info fields*/
    uint8_t     ex_zensr_nodeid;/**< Zensor Net source node id. This field is 0 if device is not based on Zensor Net*/
    uint8_t     ex_status;      /**< Z-wave alarm status (ZW_ALRM_STS_XXX) */
    uint8_t     ex_type;        /**< Z-wave alarm type (ZW_ALRM_XXX) */
    uint8_t     ex_event;       /**< Z-wave alarm event (ZW_ALRM_EVT_XXX) */
    uint8_t     ex_evt_len;     /**< Z-wave alarm event parameter length. Zero if the event has no parameter */
    uint8_t     ex_evt_type;    /**< Z-wave alarm event parameter type (ZW_ALRM_PARAM_XXX) */
    uint8_t     ex_evt_prm[1];  /**< Z-wave alarm event parameter place holder*/
}
zwalrm_t, *zwalrm_p;

typedef void (*zwrep_alrm_fn)(zwifd_p ifd, zwalrm_p  alarm_info);
/**<
report callback for alarm
@param[in]	ifd	        interface
@param[in]	alarm_info  alarm info
*/

int zwif_alrm_rpt_set(zwifd_p ifd, zwrep_alrm_fn rpt_cb);
/**<
setup an alarm report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_alrm_get(zwifd_p ifd, uint8_t vtype, uint8_t ztype, uint8_t evt);
/**<
get the state of the alarm device through report callback
@param[in]	ifd	        interface
@param[in]	vtype	    vendor specific alarm type. Zero if this field is not used
@param[in]	ztype	    Z-wave alarm type (ZW_ALRM_XXX). Zero if this field is not used; 0xFF=to retrieve the first alarm detection.
@param[in]	evt	        Event corresponding to Z-wave alarm type. Zero if this field is not used.
@return		ZW_ERR_XXX
*/

int zwif_alrm_set(zwifd_p ifd, uint8_t ztype, uint8_t sts);
/**<
set the activity of the specified Z-Wave Alarm Type
@param[in]	ifd	    interface
@param[in]	ztype	Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	sts     Z-wave alarm status. 0= deactivated; 0xFF= activated
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_alrm_sup_fn)(zwifd_p ifd, uint8_t have_vtype, uint8_t ztype_len, uint8_t *ztype);
/**<
report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	have_vtype  flag to indicate whether vendor specific alarm type supported. 1=supported; else 0=unsupported
@param[in]	ztype_len   size of ztype buffer
@param[in]	ztype       buffer to store supported Z-wave alarm types (ZW_ALRM_XXX)
*/

int zwif_alrm_sup_get(zwifd_p ifd, zwrep_alrm_sup_fn cb);
/**<
get the supported alarm types
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

typedef void (*zwrep_alrm_evt_fn)(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt);
/**<
report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	ztype       Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	evt_len     size of evt buffer
@param[in]	evt         buffer to store supported event of the alarm type specified in ztype
*/

int zwif_alrm_sup_evt_get(zwifd_p ifd, uint8_t ztype, zwrep_alrm_evt_fn cb);
/**<
get the supported events of a specified alarm type
@param[in]	ifd	    interface
@param[in]	ztype   Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

/**
@}
@defgroup If_Battery  Battery Interface APIs
Battery command that can be used to read the remaining level of a device
@{
*/

int zwif_battery_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb);
/**<
setup a battery report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/

int zwif_battery_get(zwifd_p ifd);
/**<
get battery report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/**
@}
@defgroup If_Thrm_fan Thermostat Fan Mode and State Interface APIs
Used to report thermostat fan operating conditions
@{
*/

/** Thermostat Fan Mode */
#define ZW_THRMO_FAN_MD_AUTO_LO     0       /**< Turn the manual fan operation off unless turned on by the furnace or AC.
                                                 Lower speed is selected in case it is a two-speed fan.*/
#define ZW_THRMO_FAN_MD_LOW         1       /**< Turn the manual fan to low speed*/
#define ZW_THRMO_FAN_MD_AUTO_HI     2       /**< Turn the manual fan operation off unless turned on by the furnace or AC.
                                                 High speed is selected in case it is a two-speed fan.*/
#define ZW_THRMO_FAN_MD_HI          3       /**< Turn the manual fan to high speed*/
#define ZW_THRMO_FAN_MD_AUTO_MID    4       /**< Turn the manual fan operation off unless turned on by the furnace or AC.
                                                 Medium speed is selected in case it is a three-speed fan.*/
#define ZW_THRMO_FAN_MD_MID         5       /**< Turn the manual fan to medium speed*/
#define ZW_THRMO_FAN_MD_CIR         6       /**< Turn the manual fan operation off unless turned on by the circulation
                                                 algorithms.*/
#define ZW_THRMO_FAN_MD_HUM_CIR     7       /**< Turn the manual fan operation off unless turned on by the humidity
                                                 circulation algorithms.*/

typedef void (*zwrep_thrmo_fan_md_fn)(zwifd_p ifd, uint8_t off, uint8_t mode);
/**<
report callback for thermostat fan operating mode
@param[in]	ifd	    interface
@param[in]	off     fan off mode flag. Non-zero indicates that the fan is fully OFF,
                    “0” indicates that it is possible to change between Fan Modes.
@param[in]	mode    fan operating mode, ZW_THRMO_FAN_MD_XXX
*/

int zwif_thrmo_fan_md_rpt_set(zwifd_p ifd, zwrep_thrmo_fan_md_fn rpt_cb);
/**<
setup an alarm report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_fan_md_get(zwifd_p ifd);
/**<
get the thermostat fan operating mode through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_thrmo_fan_md_set(zwifd_p ifd, uint8_t off, uint8_t mode);
/**<
set the fan mode in the device
@param[in]	ifd	    interface
@param[in]	off     fan off mode flag. Non-zero will switch the fan fully OFF.
                    In order to activate a fan mode this flag must be set to “0”.
@param[in]	mode    fan operating mode, ZW_THRMO_FAN_MD_XXX
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_thrmo_fan_md_sup_fn)(zwifd_p ifd, uint8_t off, uint8_t mode_len, uint8_t *mode);
/**<
report callback for supported thermostat fan operating modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat fan operating modes (ZW_THRMO_FAN_MD_XXX)
*/

int zwif_thrmo_fan_md_sup_get(zwifd_p ifd, zwrep_thrmo_fan_md_sup_fn cb);
/**<
get the supported thermostat fan operating modes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

/** Thermostat Fan State */
#define ZW_THRMO_FAN_STA_IDLE       0       /**< Idle */
#define ZW_THRMO_FAN_STA_LOW        1       /**< Running / Running Low - Lower speed is selected in case it is a two-speed fan*/
#define ZW_THRMO_FAN_STA_HI         2       /**< Running High - High speed is selected in case it is a two-speed fan*/

typedef void (*zwrep_thrmo_fan_sta_fn)(zwifd_p ifd, uint8_t state);
/**<
report callback for thermostat fan operating state
@param[in]	ifd	    interface
@param[in]	state   fan operating state, ZW_THRMO_FAN_STA_XXX
*/

int zwif_thrmo_fan_sta_rpt_set(zwifd_p ifd, zwrep_thrmo_fan_sta_fn rpt_cb);
/**<
setup a thermostat fan operating state report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_fan_sta_get(zwifd_p ifd);
/**<
get the thermostat fan operating state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/** Thermostat Mode */
#define ZW_THRMO_MD_OFF             0       /**< System is off */
#define ZW_THRMO_MD_HEAT            1       /**< Only heating will occur*/
#define ZW_THRMO_MD_COOL            2       /**< Only cooling will occur */
#define ZW_THRMO_MD_AUTO            3       /**< Auto*/
#define ZW_THRMO_MD_AUX_HEAT        4       /**< Auxiliary/Emergency Heat- the thermostat may be put into Aux
                                                 heat mode simply to use a more efficient secondary heat source when
                                                 there are no failures of the compressor or heat pump unit itself.*/
#define ZW_THRMO_MD_RESUME          5       /**< The system will resume from last active mode.*/
#define ZW_THRMO_MD_FAN             6       /**< Only cycle fan to circulate air. */
#define ZW_THRMO_MD_FURNACE         7       /**< Only furnace */
#define ZW_THRMO_MD_DRY_AIR         8       /**< The system will cycle cooling in relation to the room and set point
                                                 temperatures in order to remove moisture from ambient*/
#define ZW_THRMO_MD_MOIST_AIR       9       /**< Humidification */
#define ZW_THRMO_MD_AUTO_CHANGEOVER 10      /**< Heating or cooling will come on according to the auto changeover setpoint*/
#define ZW_THRMO_MD_ENE_SAVE_HEAT   11      /**< Energy Save Mode Heating will occur */
#define ZW_THRMO_MD_ENE_SAVE_COOL   12      /**< Energy Save Mode Cooling will occur.*/
#define ZW_THRMO_MD_AWAY            13      /**< special Heating Mode, i.e. preventing water from freezing in
                                                 forced water systems.*/

typedef void (*zwrep_thrmo_md_fn)(zwifd_p ifd, uint8_t mode);
/**<
report callback for thermostat mode
@param[in]	ifd	    interface
@param[in]	mode    mode, ZW_THRMO_MD_XXX
*/

int zwif_thrmo_md_rpt_set(zwifd_p ifd, zwrep_thrmo_md_fn rpt_cb);
/**<
setup a thermostat mode report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_md_get(zwifd_p ifd);
/**<
get the thermostat mode through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_thrmo_md_set(zwifd_p ifd, uint8_t mode);
/**<
set the mode in the device
@param[in]	ifd	    interface
@param[in]	mode    mode, ZW_THRMO_MD_XXX
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_thrmo_md_sup_fn)(zwifd_p ifd, uint8_t mode_len, uint8_t *mode);
/**<
report callback for supported thermostat modes
@param[in]	ifd	        interface
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat modes (ZW_THRMO_MD_XXX)
*/

int zwif_thrmo_md_sup_get(zwifd_p ifd, zwrep_thrmo_md_sup_fn cb);
/**<
get the supported thermostat modes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

/** Thermostat Operating State */
#define ZW_THRMO_OP_STA_IDLE       0       /**< Idle */
#define ZW_THRMO_OP_STA_HEAT       1       /**< Heating*/
#define ZW_THRMO_OP_STA_COOL       2       /**< Cooling*/
#define ZW_THRMO_OP_STA_FAN        3       /**< Fan only */
#define ZW_THRMO_OP_STA_PD_HEAT    4       /**< Pending Heat*/
#define ZW_THRMO_OP_STA_PD_COOL    5       /**< Pending Cool*/
#define ZW_THRMO_OP_STA_VENT       6       /**< Vent/Economizer */

typedef void (*zwrep_thrmo_op_sta_fn)(zwifd_p ifd, uint8_t state);
/**<
report callback for thermostat operating state
@param[in]	ifd	    interface
@param[in]	state   operating state, ZW_THRMO_OP_STA_XXX
*/

int zwif_thrmo_op_sta_rpt_set(zwifd_p ifd, zwrep_thrmo_op_sta_fn rpt_cb);
/**<
setup a thermostat operating state report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_op_sta_get(zwifd_p ifd);
/**<
get the thermostat operating state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/** Thermostat Setback Type */
#define ZW_THRMO_SETB_TYP_NO_OVERRIDE     0       /**< No override */
#define ZW_THRMO_SETB_TYP_TEMP_OVR        1       /**< Temporary override */
#define ZW_THRMO_SETB_TYP_PERM_OVR        2       /**< Permanent override */

/** Thermostat Setback State */
#define ZW_THRMO_SETB_STA_SETB              0       /**< Setback in 1/10 degrees (Kelvin)*/
#define ZW_THRMO_SETB_STA_FROST_PROCT       1       /**< Frost Protection */
#define ZW_THRMO_SETB_STA_ENER_SAVE         2       /**< Energy Saving Mode */
#define ZW_THRMO_SETB_STA_UNUSED            3       /**< Unused*/

typedef void (*zwrep_thrmo_setb_fn)(zwifd_p ifd, uint8_t type, uint8_t state, int8_t tenth_degree);
/**<
report callback for thermostat setback
@param[in]	ifd	            interface
@param[in]	type            setback type, ZW_THRMO_SETB_TYP_XXX
@param[in]	state           setback state, ZW_THRMO_SETB_STA_XXX
@param[in]	tenth_degree	1/10 of a degree.  This parameter is valid only if state equals to ZW_THRMO_SETB_STA_SETB
*/

int zwif_thrmo_setb_rpt_set(zwifd_p ifd, zwrep_thrmo_setb_fn rpt_cb);
/**<
setup a thermostat setback report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_setb_get(zwifd_p ifd);
/**<
get the thermostat setback state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_thrmo_setb_set(zwifd_p ifd, uint8_t type, uint8_t state, int8_t tenth_degree);
/**<
set the state in the device
@param[in]	ifd	            interface
@param[in]	type            setback type, ZW_THRMO_SETB_TYP_XXX
@param[in]	state           setback state, ZW_THRMO_SETB_STA_XXX
@param[in]	tenth_degree	1/10 of a degree (Kelvin).  This parameter is valid if state equals to ZW_THRMO_SETB_STA_SETB.
                            Valid values: -128 to 120 (inclusive). i.e. setback temperature ranges from -12.8 degree K to 12 degree K.
@return	ZW_ERR_XXX
*/


/** Thermostat Setpoint Type */
#define ZW_THRMO_SETP_TYP_HEATING           1       /**< Heating */
#define ZW_THRMO_SETP_TYP_COOLING           2       /**< Cooling */
#define ZW_THRMO_SETP_TYP_FURNACE           7       /**< Furnace */
#define ZW_THRMO_SETP_TYP_DRY               8       /**< Dry air */
#define ZW_THRMO_SETP_TYP_MOIST             9       /**< Moist air */
#define ZW_THRMO_SETP_TYP_AUTO_CHANGEOVER   10      /**< Auto changeover */
#define ZW_THRMO_SETP_TYP_ENE_SAVE_HEAT     11      /**< Energy Save Heating */
#define ZW_THRMO_SETP_TYP_ENE_SAVE_COOL     12      /**< Energy Save Cooling */
#define ZW_THRMO_SETP_TYP_AWAY              13      /**< Special Heating Mode, i.e. preventing water from freezing in
                                                         forced water systems*/
/** Thermostat Setpoint Unit */
#define ZW_THRMO_SETP_UNIT_C           0       /**< Celsius */
#define ZW_THRMO_SETP_UNIT_F           1       /**< Fahrenheit */


/** Thermostat setpoint data */
typedef struct
{
    uint8_t     type;		/**< ZW_THRMO_SETP_TYP_XXX */
    uint8_t     precision;  /**< Decimal places of the value.  The decimal
                                 value 1025 with precision 2 is therefore equal to 10.25.*/
    uint8_t     unit;	    /**< ZW_THRMO_SETP_UNIT_XXX */
    uint8_t     size;	    /**< data size: 1,2,or 4 bytes*/
    uint8_t     data[4];	/**< setpoint data (a signed number) with the first byte
                                 is the most significant byte*/
}
zwsetp_t, *zwsetp_p;


typedef void (*zwrep_thrmo_setp_fn)(zwifd_p ifd, zwsetp_p data);
/**<
report callback for thermostat setpoint
@param[in]	ifd	            interface
@param[in]	data            setpoint data
*/

int zwif_thrmo_setp_rpt_set(zwifd_p ifd, zwrep_thrmo_setp_fn rpt_cb);
/**<
setup a thermostat setpoint report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_thrmo_setp_get(zwifd_p ifd, uint8_t type);
/**<
get the thermostat setpoint through report callback
@param[in]	ifd	        interface
@param[in]	type	    setpoint type
@return		ZW_ERR_XXX
*/

int zwif_thrmo_setp_set(zwifd_p ifd, zwsetp_p data);
/**<
set the setpoint in the device
@param[in]	ifd	    interface
@param[in]	data    setpoint data
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_thrmo_setp_sup_fn)(zwifd_p ifd, uint8_t type_len, uint8_t *type);
/**<
report callback for supported thermostat modes
@param[in]	ifd	        interface
@param[in]	type_len    size of type buffer
@param[in]	type        buffer to store supported thermostat setpoint types (ZW_THRMO_SETP_TYP_XXX)
*/

int zwif_thrmo_setp_sup_get(zwifd_p ifd, zwrep_thrmo_setp_sup_fn cb);
/**<
get the supported thermostat setpoint types
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

/**
@}
@defgroup If_Clock Clock Interface APIs
Clock command that can be used to control the clock functionality of a device
@{
*/
/** Day of week */
#define ZW_CLOCK_UNDEFINED          0       /**< Undefined */
#define ZW_CLOCK_MONDAY             1       /**< Monday */
#define ZW_CLOCK_TUESDAY            2       /**< Tuesday */
#define ZW_CLOCK_WEDNESDAY          3       /**< Wednesday */
#define ZW_CLOCK_THURSDAY           4       /**< Thursday */
#define ZW_CLOCK_FRIDAY             5       /**< Friday */
#define ZW_CLOCK_SATURDAY           6       /**< Saturday */
#define ZW_CLOCK_SUNDAY             7       /**< Sunday */

typedef void (*zwrep_clock_fn)(zwifd_p ifd, uint8_t weekday, uint8_t hour, uint8_t minute);
/**<
report callback for time of day
@param[in]	ifd	    interface
@param[in]	weekday	Day of week, ZW_CLOCK_XXX
@param[in]	hour	Hour (in 24 hours format)
@param[in]	minute	Minute
*/

int zwif_clock_set(zwifd_p ifd, uint8_t weekday, uint8_t hour, uint8_t minute);
/**<
set clock value
@param[in]	ifd		interface
@param[in]	weekday	Day of week, ZW_CLOCK_XXX
@param[in]	hour	Hour (in 24 hours format)
@param[in]	minute	Minute
@return	ZW_ERR_XXX
*/

int zwif_clock_rpt_set(zwifd_p ifd, zwrep_clock_fn rpt_cb);
/**<
setup a clock report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_clock_get(zwifd_p ifd);
/**<
get clock report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

/**
@}
@defgroup If_Climate_Ctl Climate Control Schedule Interface APIs
Climate Control Schedule command that can be used to control a thermostat setback operation schedule
@{
*/

/** Thermostat Climate Control Schedule Entry (Switchpoint)*/
typedef struct
{
    uint8_t     hour;       /**< Hour: 0 ~ 23*/
    uint8_t     minute;		/**< Minute: 0 ~ 59 */
    uint8_t     state;      /**< Schedule state, ZW_THRMO_SETB_STA_XXX */
    int8_t      tenth_deg;  /**< 1/10 of a degree (Kelvin).  This parameter is valid if state
                                 equals to ZW_THRMO_SETB_STA_SETB. Valid values: -128 to 120 (inclusive).
                                 i.e. setback temperature ranges from -12.8 degree K to 12 degree K.*/
}
zwcc_shed_swpt_t;

/** Thermostat Climate Control Schedule */
typedef struct
{
    uint8_t             weekday;    /**< Day of week. Valid from ZW_CLOCK_MONDAY to ZW_CLOCK_SUNDAY*/
    uint8_t             total;      /**< Total number of valid schedule entries*/
    zwcc_shed_swpt_t    swpts[9];   /**< Schedule entries (switchpoints). The entries must be ordered by time, ascending
                                         from 00:00 towards 23:59. No duplicates of time shall be allowed*/
}
zwcc_shed_t, *zwcc_shed_p;


typedef void (*zwrep_clmt_ctl_schd_fn)(zwifd_p ifd, zwcc_shed_p sched);
/**<
report callback for climate control schedule
@param[in]	ifd	    interface
@param[in]	sched   climate control schedule
*/

int zwif_clmt_ctl_schd_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_fn rpt_cb);
/**<
setup a climate control schedule report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_clmt_ctl_schd_get(zwifd_p ifd, uint8_t weekday);
/**<
get the climate control schedule through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    day of week, ZW_CLOCK_XXX
@return		ZW_ERR_XXX
*/

int zwif_clmt_ctl_schd_set(zwifd_p ifd, zwcc_shed_p sched);
/**<
set the climate control schedule in a device for a specific weekday
@param[in]	ifd	            interface
@param[in]	sched           climate control schedule
@return	ZW_ERR_XXX
*/

typedef void (*zwrep_clmt_ctl_schd_chg_fn)(zwifd_p ifd, uint8_t chg_counter);
/**<
report callback for climate control schedule change
@param[in]	ifd	            interface
@param[in]	chg_counter     climate control schedule change counter.If the ChangeCounter
                            is different from last time, this indicates a change in a
                            climate control schedule.
                            Value from 1 to 255 indicates the climate control schedule change mechanism is enabled.
                            Value of 0 indicates the climate control schedule change mechanism is temporarily
                            disabled by the override function.
*/

int zwif_clmt_ctl_schd_chg_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_chg_fn rpt_cb);
/**<
setup a climate control schedule change report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_clmt_ctl_schd_chg_get(zwifd_p ifd);
/**<
get the climate control schedule change counter through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    day of week, ZW_CLOCK_XXX
@return		ZW_ERR_XXX
*/

/** Thermostat Climate Control Schedule Override*/
typedef struct
{
    uint8_t     type;		/**< Schedule override type, ZW_THRMO_SETB_TYP_XXX */
    uint8_t     state;      /**< Schedule override state, ZW_THRMO_SETB_STA_XXX */
    int8_t      tenth_deg;  /**< 1/10 of a degree (Kelvin).  This parameter is valid if state
                                 equals to ZW_THRMO_SETB_STA_SETB. Valid values: -128 to 120 (inclusive).
                                 i.e. setback temperature ranges from -12.8 degree K to 12 degree K.*/
}
zwcc_shed_ovr_t, *zwcc_shed_ovr_p;

typedef void (*zwrep_clmt_ctl_schd_ovr_fn)(zwifd_p ifd, zwcc_shed_ovr_p schd_ovr);
/**<
report callback for climate control schedule override
@param[in]	ifd	            interface
@param[in]	schd_ovr        climate control schedule override
*/

int zwif_clmt_ctl_schd_ovr_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_ovr_fn rpt_cb);
/**<
setup a climate control schedule override report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_clmt_ctl_schd_ovr_get(zwifd_p ifd);
/**<
get the climate control schedule override through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_clmt_ctl_schd_ovr_set(zwifd_p ifd, zwcc_shed_ovr_p schd_ovr);
/**<
set the climate control schedule override in a device
@param[in]	ifd	            interface
@param[in]	schd_ovr        climate control schedule override
@return	ZW_ERR_XXX
*/

/**
@}
@defgroup If_Protection  Protection Interface APIs
Protection command that can be used to protect a device from unauthorized control
@{
*/

/** Local Protection State */
#define ZW_LPROT_UNPROT         0       /**< Unprotected - The device is not protected,
                                             and can be operated normally via the user interface. */
#define ZW_LPROT_SEQ            1       /**< Protection by sequence - The device is protected by
                                             altering the way the device normally is operated into a more
                                             complicated sequence of actions */
#define ZW_LPROT_NO_CTL         2       /**< No operation possible - It is not possible at all to control
                                             a device directly via the user interface.*/

/** RF Protection State (version 2)*/
#define ZW_RFPROT_UNPROT        0       /**< Unprotected - The device must accept and respond to all RF Commands.*/
#define ZW_RFPROT_NO_CTL        1       /**< No RF control - all runtime Commands are ignored by the device.
                                             The device must still respond with status on requests.*/
#define ZW_RFPROT_NO_RESP       2       /**< No RF response at all. The device will not even reply to status requests.*/

typedef void (*zwrep_prot_fn)(zwifd_p ifd, uint8_t local_prot, uint8_t rf_prot);
/**<
report callback for protection states
@param[in]	ifd	        interface
@param[in]	local_prot  local protection state, ZW_LPROT_XXX
@param[in]	rf_prot     RF protection state, ZW_RFPROT_XXX.
*/

int zwif_prot_rpt_set(zwifd_p ifd, zwrep_prot_fn rpt_cb);
/**<
setup a protection report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_prot_get(zwifd_p ifd);
/**<
get the protection states through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_prot_set(zwifd_p ifd, uint8_t local_prot, uint8_t rf_prot);
/**<
set the protection states in the device
@param[in]	ifd	        interface
@param[in]	local_prot  local protection state, ZW_LPROT_XXX
@param[in]	rf_prot     RF protection state, ZW_RFPROT_XXX. For device
                        that supports only version 1, this field will be ignored
@return	ZW_ERR_XXX
*/


/** Supported Protection States*/
typedef struct
{
    uint8_t     excl_ctl;       /**< Flag to indicates whether the device supports Exclusive Control */
    uint8_t     tmout;          /**< Flag to indicates whether the device supports timeout for RF Protection State */
    uint8_t     lprot[16];      /**< Supported Local Protection States */
    uint8_t     lprot_len;      /**< Number of supported Local Protection States*/
    uint8_t     rfprot[16];     /**< Supported RF Protection States */
    uint8_t     rfprot_len;     /**< Number of supported RF Protection States*/

}
zwprot_sup_t, *zwprot_sup_p;


typedef void (*zwrep_prot_sup_fn)(zwifd_p ifd, zwprot_sup_p sup_sta);
/**<
report callback for supported protection states
@param[in]	ifd	        interface
@param[in]	sup_sta     supported Protection States
*/

int zwif_prot_sup_get(zwifd_p ifd, zwrep_prot_sup_fn cb);
/**<
get the supported protections
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

typedef void (*zwrep_prot_ec_fn)(zwifd_p ifd, uint8_t node_id);
/**<
report callback for protection exclusive control node
@param[in]	ifd	        interface
@param[in]	node_id     node ID that has exclusive control can override the RF protection state
                        of the device and can control it regardless of the protection state.
                        Node id of zero is used to reset the protection exclusive control state.
*/

int zwif_prot_ec_rpt_set(zwifd_p ifd, zwrep_prot_ec_fn rpt_cb);
/**<
setup a protection exclusive control report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_prot_ec_set(zwifd_p ifd, uint8_t node_id);
/**<
set the protection exclusive control node in the device
@param[in]	ifd	        interface
@param[in]	node_id     node ID that has exclusive control can override the RF protection state
                        of the device and can control it regardless of the protection state.
                        Node id of zero is used to reset the protection exclusive control state.
@return	ZW_ERR_XXX
*/

int zwif_prot_ec_get(zwifd_p ifd);
/**<
get the protection exclusive control node through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

typedef void (*zwrep_prot_tmout_fn)(zwifd_p ifd, uint8_t remain_tm);
/**<
report callback for the remaining time that a device will remain in protection mode.
@param[in]	ifd	        interface
@param[in]	remain_tm   remaining time. 0x00 = No timer is set. All “normal operation” Commands must be accepted.
                        0x01 to 0x3C = 1 second (0x01) to 60 seconds (0x3C);
                        0x41 to 0xFE = 2 minutes (0x41) to 191 minutes (0xFE);
                        0xFF = No Timeout - The Device will remain in RF Protection mode infinitely.
*/

int zwif_prot_tmout_rpt_set(zwifd_p ifd, zwrep_prot_tmout_fn rpt_cb);
/**<
setup a RF protection timeout report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_prot_tmout_get(zwifd_p ifd);
/**<
get the RF protection timeout through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_prot_tmout_set(zwifd_p ifd, uint8_t tmout);
/**<
set the RF protection timeout in the device
@param[in]	ifd	        interface
@param[in]	tmout	    Timeout specifies the time (in different resolutions) a device will remain in RF Protection mode.
                        0x01 to 0x3C = 1 second (0x01) to 60 seconds (0x3C);
                        0x41 to 0xFE = 2 minutes (0x41) to 191 minutes (0xFE);
                        0xFF = No Timeout - The Device will remain in RF Protection mode infinitely.
@return	ZW_ERR_XXX
*/

/**
@}
@defgroup If_Status  Status Interface APIs
Application status command that can be used to inform the outcome of a request
@{
*/

/** Application Busy Status */
#define ZW_BSY_STS_TRY          0       /**< Try again later */
#define ZW_BSY_STS_TRY_WAIT     1       /**< Try again in Wait Time seconds */
#define ZW_BSY_STS_Q            2       /**< Request queued, will be executed later */

typedef void (*zwrep_appl_busy_fn)(zwifd_p ifd, uint8_t status, uint8_t wait_tm);
/**<
report callback for application busy status
@param[in]	ifd	        interface
@param[in]	status      busy status, ZW_BSY_STS_XXX
@param[in]	wait_tm     wait time in seconds; only valid if status = ZW_BSY_STS_TRY_WAIT
*/

int zwif_appl_busy_rep_set(zwifd_p ifd, zwrep_appl_busy_fn rpt_cb);
/**<
setup an application busy report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

/** Application Rejected Status */
#define ZW_RJ_STS_REJECT        0       /**< Supported command rejected by the application in the receiving node */

typedef void (*zwrep_appl_reject_fn)(zwifd_p ifd, uint8_t status);
/**<
report callback for application rejected request status
@param[in]	ifd	        interface
@param[in]	status      application rejected request status, ZW_RJ_STS_XXX
*/

int zwif_appl_reject_rep_set(zwifd_p ifd, zwrep_appl_reject_fn rpt_cb);
/**<
setup an application rejected request report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/


/**
@}
@defgroup If_Indicator  Indicator Interface APIs
Indicator command that can be used to show the actual state, level etc. on a device
@{
*/

int zwif_ind_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb);
/**<
setup an indicator report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/

int zwif_ind_get(zwifd_p ifd);
/**<
get indicator report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_ind_set(zwifd_p ifd, uint8_t val);
/**<
set indicator value
@param[in]	ifd		interface
@param[in]	val		value. The value can be either 0x00 (off/disable) or 0xFF (on/enable).
                    Furthermore it can take values from 1 to 99 (0x01 - 0x63).

@return	ZW_ERR_XXX
*/

/**
@}
@defgroup If_Firmware Firmware update Interface APIs
Used to update firmwares on a device
@{
*/

/** Firmware upgradable flag */
#define ZW_FW_UPGD_NO           0       /**< Firmware is not upgradable */
#define ZW_FW_UPGD_YES          0xFF    /**< Firmware is upgradable */

#define ZW_FW_FRAG_SZ_V1        40      /**< Default firmware fragment size for version 1*/
#define ZW_FW_FRAG_SZ_V2        40      /**< Default firmware fragment size for version 2*/
#define ZW_FW_FRAG_SZ_MIN_V3    40      /**< Minimum firmware fragment size for version 3*/

/** Firmwares information */
typedef struct
{
    uint16_t    vid;            /**< Vendor/Manufacturer id */
    uint16_t    zw_fw_id;       /**< Z-wave firmware id*/
    uint16_t    chksum;         /**< CRC-CCITT checksum of Z-wave firmware*/
    uint16_t    max_frag_sz;    /**< Maximum meta data fragment size for firmware update */
    uint8_t     fixed_frag_sz;  /**< Flag to indicate whether the max_frag_sz is fixed, i.e. firmware update
                                     request MUST use the given size. 1=fixed size; 0=variable size */
    uint8_t     upgrade_flg;    /**< Firmware upgradable flag. See ZW_FW_UPGD_XXX */
    uint8_t     other_fw_cnt;   /**< Number of other firmware IDs*/
    uint16_t    *other_fw_id;   /**< Pointer to array of other firmware IDs with the count given by other_fw_cnt */
}
zwfw_info_t;

typedef void (*zwrep_fw_info_fn)(zwifd_p ifd, zwfw_info_t  *fw_info);
/**<
report callback for firmwares information
@param[in]	ifd	        interface
@param[in]	fw_info     firmwares information
*/

int zwif_fw_info_get(zwifd_p ifd, zwrep_fw_info_fn cb);
/**<
get firmwares information through report callback
@param[in]	ifd	        interface
@param[in]	cb	        report callback
@return		ZW_ERR_XXX
*/

/** Firmware update request status */
#define ZW_FW_UPDT_ERR_INVALID      0       /**< Invalid combination of vendor id and firmware id */
#define ZW_FW_UPDT_ERR_AUTHEN       0x01    /**< Need out-of-band authentication event to enable firmware update */
#define ZW_FW_UPDT_ERR_FRAG_SZ      0x02    /**< The requested Fragment Size exceeds the Max Fragment Size */
#define ZW_FW_UPDT_ERR_UPGRD        0x03    /**< This firmware target is not upgradable */
#define ZW_FW_UPDT_ERR_VALID        0xFF    /**< Valid combination of vendor id and firmware id; the device will
                                                 start to request firmware data from the requester*/

/** Firmware update completion status */
#define ZW_FW_UPDT_CMPLT_CHKSUM         0       /**< Cheksum error in requested firmware */
#define ZW_FW_UPDT_CMPLT_DOWNLOAD       0x01    /**< Download of the requested firmware failed */
#define ZW_FW_UPDT_CMPLT_OK_NO_RESTART  0xFE    /**< New image was successfully stored in temporary non-volatile memory.
                                                     The device does not restart itself. */
#define ZW_FW_UPDT_CMPLT_OK_RESTART     0xFF    /**< New image was successfully stored in temporary non-volatile memory.
                                                     The device will now start storing the new image in primary non-volatile
                                                     memory dedicated to executable code. Then the device will restart itself.*/

/** Firmware update target restart status */
#define ZW_FW_UPDT_RESTART_OK       0       /**< Restart o.k. */
#define ZW_FW_UPDT_RESTART_FAILED   0x01    /**< Restart failed due to no response from the target. */

typedef void (*zwrep_fw_updt_sts_fn)(zwifd_p ifd, uint8_t status);
/**<
report callback for firmware update status
@param[in]	ifd	        interface
@param[in]	status      firmware update status, ZW_FW_UPDT_ERR_XXX
*/

typedef void (*zwrep_fw_updt_cmplt_fn)(zwifd_p ifd, uint8_t status, uint16_t wait_tm, int wait_tm_valid);
/**<
report callback for firmware update completion status
@param[in]	ifd	            interface
@param[in]	status          firmware update completion status, ZW_FW_UPDT_CMPLT_XXX
@param[in]	wait_tm         time (in seconds) that is needed before the receiving node becomes
                            available again for communication after the transfer of an image. This parameter is
                            valid only if wait_tm_valid=1
@param[in]	wait_tm_valid   flag to indicate the wait_tm parameter is valid.
*/

typedef void (*zwrep_fw_tgt_restart_fn)(zwnoded_p node, uint8_t status);
/**<
report callback for firmware update target restart status
@param[in]	node    node
@param[in]	status  firmware update target restart status, ZW_FW_UPDT_RESTART_XXX
*/

/** Firmware update request */
typedef struct
{
    uint16_t                vid;            /**< Vendor/Manufacturer id */
    uint16_t                fw_id;          /**< Firmware id*/
    uint16_t                chksum;         /**< CRC-CCITT checksum of the firmware*/
    uint16_t                frag_sz;        /**< Meta data fragment size for firmware update */
    uint8_t                 fw_tgt;         /**< Firmware target to update. 0= Z-wave firmware, 0x01 to 0xFF for
                                                 firmware target returned by zwif_fw_info_get(). For example,
                                                 to update firmware target in other_fw_id[0], use fw_tgt=1. */
    const char              *fw_file;       /**< Firmware file path*/
    zwrep_fw_updt_sts_fn    sts_cb;         /**< Firmware update request status callback function. See ZW_FW_UPDT_ERR_XXX */
    zwrep_fw_updt_cmplt_fn  cmplt_cb;       /**< Firmware update request completion status callback function.
                                                 See ZW_FW_UPDT_CMPLT_XXX. This function will only be invoked if update
                                                 request status callback sts_cb() reported ZW_FW_UPDT_ERR_VALID*/
    zwrep_fw_tgt_restart_fn restart_cb;     /**< Optional: Firmware update target restart completion callback function.
                                                 Can be NULL if callback is not required.  Note that the callback function
                                                 works only if the cmplt_cb() invoked with parameter wait_tm_valid=1*/
}
zwfw_updt_req_t;

int zwif_fw_updt_req(zwifd_p ifd, zwfw_updt_req_t *req);
/**<
request firmware update
@param[in]	ifd	        interface
@param[in]	req	        firmware update request
@return	ZW_ERR_XXX
@note Caller should call zwif_fw_info_get() first before calling this function.
*/


/**
@}
@defgroup If_Poll Polling Interface APIs
Used to create and delete polling commands to a device
@{
*/

/** Polling request */
typedef struct
{   /* Input*/
    uint32_t                usr_token;      /**< User defined token to facilitate deletion of multiple polling requests */
    uint16_t                interval;       /**< Polling interval in seconds; zero = the smallest possible interval*/
    uint16_t                poll_cnt;       /**< Number of times to poll; zero = unlimited times*/

    /* Output*/
    uint16_t                handle;         /**< Handle if the request is accepted into the polling queue. The handle
                                                 can be used to facilitate deletion of the polling request */
}
zwpoll_req_t;

int zwnet_poll_rm(zwnet_p net, uint16_t handle);
/**<
remove a polling request
@param[in]	net	        network
@param[in]	handle	    handle of the polling request to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwnet_poll_rm_mul(zwnet_p net, uint32_t usr_token);
/**<
remove multiple polling requests
@param[in]	net	        network
@param[in]	usr_token	user token of the polling requests to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_alrm_get_poll(zwifd_p ifd, uint8_t vtype, uint8_t ztype, uint8_t evt, zwpoll_req_t *poll_req);
/**<
get the state of the alarm device through report callback
@param[in]	ifd	        interface
@param[in]	vtype	    vendor specific alarm type. Zero if this field is not used
@param[in]	ztype	    Z-wave alarm type (ZW_ALRM_XXX). Zero if this field is not used; 0xFF=to retrieve the first alarm detection.
@param[in]	evt	        Event corresponding to Z-wave alarm type. Zero if this field is not used.
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_basic_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get basic report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_battery_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get battery report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_bsensor_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get binary sensor report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_clmt_ctl_schd_chg_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get the climate control schedule change counter through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    day of week, ZW_CLOCK_XXX
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_dlck_op_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get the state of the door lock device through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_ind_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get indicator report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_level_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get level report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_meter_get_poll(zwifd_p ifd, uint8_t unit, zwpoll_req_t *poll_req);
/**<
get meter report through report callback
@param[in]	ifd	        interface
@param[in]	unit	    the preferred unit (ZW_METER_UNIT_XXX). The report may not return
                        the preferred unit if the device doesn't support it.
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_prot_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get the protection states through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_sensor_get_poll(zwifd_p ifd, uint8_t type, uint8_t unit, zwpoll_req_t *poll_req);
/**<
get multilevel sensor report through report callback
@param[in]	ifd	        interface
@param[in]	type	    preferred sensor type, ZW_SENSOR_TYPE_XXX. If type equals to zero, the
                        sensor report will return the factory default sensor type.
@param[in]	unit	    preferred sensor unit, ZW_SENSOR_UNIT_XXX.  This parameter is ignored
                        if type=0.
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
@note  Preferred sensor type and unit are not guaranteed to be returned in the report callback.  It
       depends on the multilevel sensor command class version number and the device supported.
*/

int zwif_switch_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req);
/**<
get switch report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/

int zwif_thrmo_setp_get_poll(zwifd_p ifd, uint8_t type, zwpoll_req_t *poll_req);
/**<
get the thermostat setpoint through report callback
@param[in]	ifd	            interface
@param[in]	type	        setpoint type
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/


/**
@}
@defgroup If_Power_level Power level Interface APIs
Used to control power level and test power level on target device
@{
*/

typedef void (*zwrep_power_level_fn)(zwifd_p ifd, uint8_t lvl, uint8_t timeout);
/**<
report callback for power level get
@param[in]	ifd	    interface
@param[in]	lvl     Current power level indicator value in effect on the node. Ranges
                    from 0 to 9. 0=normal power; 1= -1dbm; 2= -2dbm, etc.
@param[in]	timeout Time out value, in seconds.
*/

int zwif_power_level_rpt_set(zwifd_p ifd, zwrep_power_level_fn rpt_cb);
/**<
setup power level report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_power_level_get(zwifd_p ifd);
/**<
get the power level value in use by the node through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_power_level_set(zwifd_p ifd, uint8_t lvl, uint8_t timeout);
/**<
set the power level which should be used by the node when transmitting RF
@param[in]	ifd	      interface
@param[in]	lvl		  Power level. Ranges from 0 to 9. 0=normal power; 1= -1dbm; 2= -2dbm, etc.
@param[in]	timeout   Time out value (in seconds) ranges from 1-255 before resetting to normal power level.
@return	ZW_ERR_XXX
*/

/** Power level test status */
#define POWERLEVEL_TEST_FAILED      0x00    /** No test frame transmissions has been acknowledged*/
#define POWERLEVEL_TEST_SUCCES      0x01    /** At least 1 test frame transmission has been acknowledged*/
#define POWERLEVEL_TEST_INPROGRESS  0x02    /** Test is still in progress*/

typedef void (*zwrep_power_level_test_fn)(zwifd_p ifd, uint8_t node_id, uint8_t status, uint16_t frame_cnt);
/**<
report callback for power level test
@param[in]	ifd	        interface
@param[in]	node_id     Test node ID. If node id is 0, it means no test has been made and the rest of the
                        parameters should be ignored.
@param[in]	status      Status of the test operation. POWERLEVEL_TEST_XXX
@param[in]	frame_cnt   Number of frame count which has been acknowledged by the node as specified by node_id.
*/

int zwif_power_level_test_rpt_set(zwifd_p ifd, zwrep_power_level_test_fn rpt_cb);
/**<
setup power level test report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_power_level_test_get(zwifd_p ifd);
/**<
get the result of power level test through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/

int zwif_power_level_test_set(zwifd_p ifd, zwnoded_p noded, uint8_t lvl, uint16_t frame_cnt);
/**<
set the power level test information and start the test
@param[in]	ifd			interface
@param[in]	node		Node descriptor of the test node which should recieves the transmitted test frames.
@param[in]	lvl		    Power level. Ranges from 0 to 9. 0=normal power; 1= -1dbm; 2= -2dbm, etc.
@param[in]	frame_cnt   Test frame count to be carried out. (1-65535)
@return	ZW_ERR_XXX
*/

/**
@}
@defgroup If_Doorlock_log Doorlock logging Interface APIs
Used to report doorlock access events
@{
*/

/** Event types */
#define ZW_DRLOG_LOCK_KEYPAD_CODE       1       /**< Lock Command: Keypad access code verified lock command*/
#define ZW_DRLOG_ULOCK_KEYPAD_CODE      2       /**< Unlock Command: Keypad access code verified unlock command*/
#define ZW_DRLOG_LOCK_KEYPAD_BUTTON     3       /**< Lock Command: Keypad lock button pressed*/
#define ZW_DRLOG_ULOCK_KEYPAD_BUTTON    4       /**< Unlock command: Keypad unlock button pressed*/
#define ZW_DRLOG_LOCK_KEYPAD_CODE_OOS   5       /**< Lock Command: Keypad access code out of schedule*/
#define ZW_DRLOG_ULOCK_KEYPAD_CODE_OOS  6       /**< Unlock Command: Keypad access code out of schedule*/
#define ZW_DRLOG_KEYPAD_CODE_ILLEGAL    7       /**< Keypad illegal access code entered*/
#define ZW_DRLOG_LOCK_MANUAL            8       /**< Key or latch operation locked (manual)*/
#define ZW_DRLOG_ULOCK_MANUAL           9       /**< Key or latch operation unlocked (manual)*/
#define ZW_DRLOG_LOCK_AUTO              10      /**< Auto lock operation*/
#define ZW_DRLOG_ULOCK_AUTO             11      /**< Auto unlock operation*/
#define ZW_DRLOG_LOCK_ZWAVE_CODE        12      /**< Lock Command: Z-Wave access code verified*/
#define ZW_DRLOG_ULOCK_ZWAVE_CODE       13      /**< Unlock Command: Z-Wave access code verified*/
#define ZW_DRLOG_LOCK_ZWAVE             14      /**< Lock Command: Z-Wave (no code)*/
#define ZW_DRLOG_ULOCK_ZWAVE            15      /**< Unlock Command: Z-Wave (no code)*/
#define ZW_DRLOG_LOCK_ZWAVE_CODE_OOS    16      /**< Lock Command: Z-Wave access code out of schedule*/
#define ZW_DRLOG_ULOCK_ZWAVE_CODE_OOS   17      /**< Unlock Command Z-Wave access code out of schedule*/
#define ZW_DRLOG_ZWAVE_CODE_ILLEGAL     18      /**< Z-Wave illegal access code entered*/
#define ZW_DRLOG_LOCK_MANUAL_2          19      /**< Key or latch operation locked (manual)*/
#define ZW_DRLOG_ULOCK_MANUAL_2         20      /**< Key or latch operation unlocked (manual)*/
#define ZW_DRLOG_LOCK_SECURE            21      /**< Lock secured*/
#define ZW_DRLOG_ULOCK_UNSECURE         22      /**< Lock unsecured*/
#define ZW_DRLOG_USR_CODE_ADDED         23      /**< User code added*/
#define ZW_DRLOG_USR_CODE_DELETED       24      /**< User code deleted*/
#define ZW_DRLOG_ALL_USR_CODE_DELETED   25      /**< All user codes deleted*/
#define ZW_DRLOG_MSTR_CODE_CHANGED      26      /**< Master code changed*/
#define ZW_DRLOG_USR_CODE_CHANGED       27      /**< User code changed*/
#define ZW_DRLOG_LOCK_RESET             28      /**< Lock reset*/
#define ZW_DRLOG_CONFIG_CHANGED         29      /**< Configuration changed*/
#define ZW_DRLOG_LOW_BATT               30      /**< Low battery*/
#define ZW_DRLOG_NEW_BATT_INSTALLED     31      /**< New Battery installed*/


/** Door Lock Logging Record */
typedef struct
{
    uint8_t     rec_num;        /**< Record number requested (1- 255) */
    uint8_t     rec_valid;      /**< Flag to indicate requested record holds valid data (1=valid; 0=empty) */
    uint16_t    year;           /**< Timestamp: year */
    uint8_t     month;          /**< Timestamp: month */
    uint8_t     day;            /**< Timestamp: day */
    uint8_t     hour;           /**< Timestamp: hour (local time) valid value : 0 to 23 */
    uint8_t     min;            /**< Timestamp: minute (local time) valid value : 0 to 59 */
    uint8_t     second;         /**< Timestamp: second (local time) valid value : 0 to 59 */
    uint8_t     evt;            /**< Event (ZW_DRLOG_XXX) */
    uint8_t     usr_id;         /**< User identifier. A valid User Identifier MUST be a value starting from 1 to
                                     the maximum number of users supported by the device. A User Identifier of 0
                                     is acceptable when the record does not need to identify a user or if the
                                     User Code is provided in this report */
    uint8_t     usr_code_len;   /**< The User Code Length field indicates the number of bytes used to hold the
                                     User Code. A length of 0 is acceptable when the record does not need to identify
                                     a User Code or when the User Identifier field is non-zero*/
    uint8_t     usr_code[1];    /**< Place holder for user code */

}
zwdrlog_rec_t;


typedef void (*zwrep_drlog_sup_fn)(zwifd_p ifd, uint8_t max_rec);
/**<
report callback for maximum number of records supported
@param[in]	ifd	        interface
@param[in]	max_rec     maximum number of records supported
*/

typedef void (*zwrep_drlog_rep_fn)(zwifd_p ifd, zwdrlog_rec_t *rec);
/**<
report callback for door lock logging record
@param[in]	ifd	    interface
@param[in]	rec     door lock logging record
*/

int zwif_drlog_rec_sup_get(zwifd_p ifd, zwrep_drlog_rep_fn cb);
/**<
get the maximum number of records supported
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/

int zwif_drlog_rec_rpt_set(zwifd_p ifd, zwrep_drlog_rep_fn rpt_cb);
/**<
setup a door lock logging record report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/

int zwif_drlog_rec_get(zwifd_p ifd, uint8_t rec_num);
/**<
get the door lock logging record through report callback
@param[in]	ifd	        interface
@param[in]	rec_num     record number to request. Valid value: 0 to the maximum number of records supported.
                        Zero is used to retrieve the most recent record.
@return		ZW_ERR_XXX
*/


/**
@}
*/

#ifdef INTERNAL_TEST
uint32_t zwnet_get_tx_tmout_cnt(zwnet_p net);
#endif

#endif
