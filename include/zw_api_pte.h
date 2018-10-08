/**
Z-Wave private definitions
@author	sasi@sdesigns.sg
@date		16 Mar 2010
*/

#ifndef _ZWAVE_API_PTE_SASI_
#define _ZWAVE_API_PTE_SASI_

#include <stdint.h>
#include <time.h>

/* forward references */
struct _zwnet;
struct _zwnode;
struct _zwep;
struct _zwif;
typedef struct _zwnet	*zwnet_p;		/**< opaque network handle */
typedef struct _zwnode	*zwnode_p;		/**< opaque node handle */
typedef struct _zwif	*zwif_p;		/**< opaque interface handle */
typedef struct _zwep	*zwep_p;		/**< opaque endpoint handle */

#include "zw_api.h"

/**
@defgroup Base Base object APIs
@{
*/

#define    MAX_ZWAVE_PKT_SIZE  46   /**< Maximum Z-wave packet size */

/** Z-Wave base object for linking and user context */
typedef struct _zwobj
{
	struct _zwobj	*next;		/**< next object in list */
	void			*ctx;		/**< user context (opaque to server) */
}
zwobj_t, *zwobj_p;


void zwobj_add(zwobj_p *head, zwobj_p obj);
int32_t zwobj_del(zwobj_p *head, zwobj_p obj);
int32_t zwobj_rplc(zwobj_p *head, zwobj_p obj, zwobj_p new_obj);

/**
@}
@addtogroup EP Endpoint APIs
@{
*/

#define     VIRTUAL_EP_ID       0

/** Z-Wave channel/instance/endpoint abstraction */
typedef struct _zwep
{
	zwobj_t		    obj;			            /**< base link object */
	struct _zwnode	*node;			            /**< back link to associated device */
	zwif_p          intf;			            /**< list of interfaces */
	uint8_t		    epid;		                /**< Endpoint id: ranging from 1 to 0xff */
	uint8_t		    generic;		            /**< Generic Device Class */
	uint8_t		    specific;		            /**< Specific Device Class */
    char		    name[ZW_LOC_STR_MAX + 1];   /**< user configured name string of the endpoint */
    char		    loc[ZW_LOC_STR_MAX + 1];    /**< user configured location string of the endpoint */
    zwplus_info_t   zwplus_info;                /**< Z-wave+ information */
}
#ifdef WIN32
zwep_t, *zwep_p;
#else
zwep_t;
#endif

int zwep_get_desc(zwep_p ep, zwepd_p desc);
zwep_p zwep_get_ep(zwepd_p ep_desc);
void zwep_intf_rm_all(zwep_p ep);
zwep_p zwep_find(zwep_p first_ep, uint8_t epid);
uint8_t zwep_count(zwnode_p node);

/**
@}
@addtogroup Node Node APIs
@{
*/

#define     ZWNODE_MUL_CMD_TMOUT    1000        ///< Multi command time out in milliseconds
#define     ZWNODE_WAIT_RPT_TMOUT   10000       ///< Wait for report time out in milliseconds


/** Command queue request for thread to execute */
typedef struct
{
    uint8_t             node_id;        /**< Node id to which the request should be sent*/
#ifdef SEND_NO_MORE_INFO_AFTER_DATA
	uint8_t		        wkup_brdcst;	/**< Flag to indicate wake up notification received is a broadcast message*/
#endif
    util_lst_t          *req_hd;        /**< Head of the request list*/
}
cmd_q_req_t;

#define     EXEC_ACT_PROBE_FAILED_NODE  0        ///< Send NOP to failed node
#define     EXEC_ACT_RMV_FAILED         1        ///< Remove failed node
#define     EXEC_ACT_PROBE_RESTART_NODE 2        ///< Send NOP to restarted node
#define     EXEC_ACT_CFG_LIFELINE       3        ///< Configure lifeline to all always-on-slaves (AOS)

#define     FW_UPDT_RESTART_POLL_INTERVAL   3000 ///< Firmware update target node poll interval (in ms)
#define     FW_UPDT_RESTART_POLL_MAX        10   ///< Firmware update target node poll maximum counts

/** Request for thread to execute */
typedef struct
{
    uint8_t     node_id;        /**< Node id */
    uint8_t     action;         /**< Action to perform, EXEC_ACT_XXX */
    void        *user;          /**< User data */
}
zwnet_exec_req_t;

#define     CB_RPT_TYP_THRMO_FAN_MD     0   ///< Supported thermostat fan operating modes
#define     CB_RPT_TYP_THRMO_MD         1   ///< Supported thermostat modes
#define     CB_RPT_TYP_THRMO_SETPOINT   2   ///< Supported thermostat setpoint types
#define     CB_RPT_TYP_MAX_GROUP        3   ///< Maximum supported groupings
#define     CB_RPT_TYP_MUL_SWITCH       4   ///< Supported multilevel switch
#define     CB_RPT_TYP_AV               5   ///< Supported simple AV controls
#define     CB_RPT_TYP_ALARM_TYPE       6   ///< Supported alarm types
#define     CB_RPT_TYP_ALARM_EVENT      7   ///< Supported alarm events
#define     CB_RPT_TYP_PROT             8   ///< Supported protection states
#define     CB_RPT_TYP_MAX_USR_CODES    9   ///< Maximum supported user codes
#define     CB_RPT_TYP_METER            10  ///< Supported Meter
#define     CB_RPT_TYP_METER_DESC       11  ///< Meter descriptor
#define     CB_RPT_TYP_MAX_DRLOG_REC    12  ///< Maximum supported door lock logging records

/** Request for thread to callback */
typedef struct
{
	zwifd_t     ifd;	        /**< Interface associated with the callback */
    uint16_t    rpt_type;       /**< Report type CB_RPT_TYP_XXX */
    uint16_t    extra;          /**< Extra parameter for the rpt_type*/
	void        *cb;	        /**< Callback function */
}
zwnet_cb_req_t;


/** Multi Command queue entry for node that supports either Multi Command or Wake Up command classes */
typedef struct
{
	zwifd_t             ifd;	        /**< Interface associated with the command */
    util_lst_t          *extra;         /**< list of extra data for executing other functions*/
    uint16_t            cmd_id;         /**< interface API command id */
    uint8_t             cmd_num;        /**< the n th command class command of the interface API; start from 1, ... */
    uint8_t             wait_rpt;       /**< flag to indicate whether to wait for report (for node that requires wakeup beam) */
    uint8_t             dat_len;        /**< the length of the dat_buf field */
    uint8_t             dat_buf[1];     /**< place holder for the data buffer */
}
mul_cmd_q_ent_t;


/** Command queue extra handler parameter */
typedef struct
{
    void                *extra;         /**< extra data for executing other functions*/
    uint16_t            cmd_id;         /**< interface API command id */
    uint8_t             node_id;        /**< node id*/
}
cmd_q_xtra_t;


/** Return route for adding node to a group */
typedef struct
{
    uint8_t dst_node[4];                //Destination node of the return route
    uint8_t num_ent;                    //Number of entries in the buffer
    uint8_t rd_pos;                     //Read position
}
ret_route_t;


/** Command class, get command and report command table entry*/
typedef struct
{
    uint16_t    cls;        //Command class
    uint8_t     get_cmd;    //Get command
    uint8_t     rpt;        //Report command
}
cmd_get_resp_t;


/** Z-Wave node abstraction */
typedef struct _zwnode
{
	zwobj_t		    obj;	    /**< base link object */
	struct _zwnet   *net;	    /**< back link to associated network */
	zwep_t		    ep;		    /**< head in list of endpoints */

	/* Node status */
	uint8_t		alive;		    /**< Flag to indicate node is alive. 1= alive; 0= down or sleeping*/

	/* Manufacturer specific report */
	uint16_t	vid;			/**< Vendor ID */
	uint16_t	vtype;			/**< Vendor Product Type */
	uint16_t	pid;			/**< Product ID */
    dev_id_t    dev_id;         /**< Device ID */
    uint8_t     category;	    /**< Device category, DEV_XXX */

	/* Version class report */
	uint16_t	proto_ver;		/**< Z-Wave Protocol Version*/
	uint16_t	app_ver;		/**< Application Version*/
    ext_ver_t   *ext_ver;       /**< Extended version information*/
	uint8_t		lib_type;		/**< Z-Wave Library Type*/
	uint8_t		basis_api_ver[20];	/**< Basis API library version*/

	/* Node information report */
	uint8_t		nodeid;			/**< Node ID */
	uint8_t		capability;		/**< Capability */
	uint8_t		ctlr_nw_role;   /**< Controller network role */
	uint8_t		security;		/**< Security */
	uint8_t		basic;			/**< Basic Device Class */

    /* Multi-channel report*/
    uint8_t		mul_ch_ver;	    /**< Multi-channel version */
	uint8_t		num_of_ep;	    /**< Number of endpoints */
	uint8_t		dynamic_ep;	    /**< Flag to indicate whether the number of endpoints is dynamic */
	uint8_t		ident_ep;	    /**< Flag to indicate whether the endpoints are identical */

    /* Sleeping node (wake up command class)*/
    uint16_t    cmd_id;         /**< Interface API command id */
    uint8_t     cmd_num;        /**< The n th command class command of the interface API; start from 1, ...
                                     A value of zero indicates that the command should not be queued */
    uint8_t     enable_cmd_q;   /**< Flag to enable command queuing */
    uint8_t     sleep_cap;      /**< Flag to indicate the node is capable to sleep */
    uint32_t    wkup_intv;      /**< Wake up interval in seconds */

	/* Multi Command Encapsulation*/
    uint8_t		mul_cmd_cap;	/**< Flag to indicate the node is capable to encapsulate multiple commands */
    uint8_t		mul_cmd_ctl;	/**< Multi Command Encapsulation control: 0 = off, 1 = on*/
    void        *mul_cmd_tmr_ctx;/**< Multi command timer context*/

    /* Command queue for both sleeping node and multi command encapsulation*/
    util_lst_t  *mul_cmd_q_hd;  /**< List head of command queue */
    uint8_t     mul_cmd_q_cnt;  /**< The number of entries in the command queue */

	/* CRC-16 Encapsulation*/
	uint8_t		crc_cap;	    /**< Flag to indicate the node is capable to check/generate CRC-16 checksum */

	/* Node that requires wakeup beam*/
    util_lst_t  *wait_cmd_q_hd; /**< List head of command queue */
    void        *wait_tmr_ctx;  /**< Wait REPORT command timer context*/
    uint8_t     wait_cmd_q_cnt; /**< The number of entries in the command queue */
	uint8_t		wait_rpt_flg;	/**< Flag to indicate the node is waiting for report */
	uint16_t	wait_cmd;	    /**< The command class of the report */
	uint8_t		wait_rpt;	    /**< The report command of the report */

    /* Grouping (association command class)*/
    ret_route_t add_grp_rr;     /**< Return route to configure after the add group command */

    /* Security */
    uint8_t     sec_incl_failed;/**< Flag to mark a node who failed the inclusion process. 1=failed, 0=success */
    uint8_t     sec_snd_seq_num;/**< 4-bit sequence number used when sending split original payload package (OPP)*/
    uint8_t     sec_rcv_seq_num;/**< 4-bit sequence number used when receiving split original payload package (OPP)
                                     Note: lower 4 bits are used for sequence number; bit-7 is used to indicate
                                     validity of the sequence number.  bit-7=1 means valid; bit-7=0 means invalid*/
    uint8_t     first_spp_frm[29];/**< The first frame of the received SPP. Note the 1st byte contains sequence number */
    uint8_t     first_spp_frm_len;/**< The first frame payload length (including the sequence number header) */

	/* Firmware update version 3 */
    zwrep_fw_tgt_restart_fn restart_cb;     /**< Firmware update target restart completion callback function */
    int                     poll_tgt_cnt;   /**< Poll count*/

}
#ifdef WIN32
zwnode_t, *zwnode_p;
#else
zwnode_t;
#endif

#define zwnode_ctl_clr(nw)  do{ \
                                (nw)->ctl.nodeid = 0;           \
                                (nw)->ctl.mul_ch_ver = 0;       \
                                (nw)->ctl.sleep_cap = 0;        \
                                (nw)->ctl.enable_cmd_q = 0;     \
                                (nw)->ctl.crc_cap = 0;          \
                                (nw)->ctl.sec_incl_failed = 0;  \
                                (nw)->ctl.mul_cmd_cap = 0;      \
                                (nw)->ctl.dev_id.len = 0;       \
                              } while(0)

zwnode_p zwnode_find(zwnode_p first_node, uint8_t nodeid);
int zwnode_get_desc(zwnode_p node, zwnoded_p desc);
void zwnode_rm(zwnet_p nw, uint8_t node_id);
void zwnode_ep_rm_all(zwnode_p node);
void zwnode_cmd_q_rm(zwnode_p node);
void zwnode_cmd_q_thrd(void   *data);
void zwnode_mul_cmd_rm_q(zwnet_p net, util_lst_t  **cmd_q_hd);
void zwnode_wait_rpt_chk(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len);
void zwnode_wait_q_thrd(void   *data);
void zwnode_wait_cmd_q_rm(zwnode_p node);
int zwnode_probe(zwnode_p node, tx_cmplt_cb_t cb, void *user);
int zwnode_get_rpt(uint8_t* cmd, uint8_t len, uint16_t *cmd_cls, uint8_t *rpt);
void zwnode_wait_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm);
int zwpoll_node_rm(zwnet_p net, uint8_t node_id);


/**
@}
@addtogroup Net Network APIs
@{
*/

#define ZWNET_NODE_INFO_TMOUT           10000       ///< Get node info time out in milliseconds
#define ZWNET_TMOUT                     7000        ///< Network time out in milliseconds
#define ZWNET_AV_KEEP_ALIVE_TMOUT       400         ///< "key hold" keep alive in milliseconds

///
/// Node info state-machine events
typedef enum
{
    EVT_NW_TMOUT,                       ///< Network timeout
    EVT_GET_NODE_INFO,                  ///< Start the get node info process
    EVT_DELAYED_GET_NODE_INFO,          ///< Start the get node info process at specified time later
    EVT_NODE_INFO,                      ///< Node info was received
    EVT_NODE_INFO_FAILED,               ///< Node info was not received
    EVT_SEC_SUP_REPORT,                 ///< Supported security command classes was received
    EVT_RR_TX_STS,                      ///< Assign return route transmit status was received
    EVT_VER_REPORT,                     ///< Version report of a command class was received
    EVT_ZWVER_REPORT,                   ///< Z-wave protocol and application version report was received
    EVT_EP_INFO_DONE,                   ///< Endpoint info state-machine has completed
    EVT_GET_OTHERS_DONE,                ///< Get other command classes info done
    EVT_MANF_PDT_ID_REPORT,             ///< Manufacturer and product id report was received
    EVT_DEVICE_ID_REPORT,               ///< Device id report was received
    EVT_WKUP_CAP_REP,                   ///< Wake up capabilities report was received
    EVT_MULTI_INSTANCE_REP,             ///< Multi-instance report was received
    EVT_EP_NUM_REPORT,                  ///< Number of endpoints in a node report was received
    EVT_EP_CAP_REPORT                   ///< Endpoint capabilities report was received

} zwnet_ni_evt_t;

///
/// Node info state-machine's states
typedef enum
{
    ZWNET_STA_IDLE,             ///< Waiting for event
    ZWNET_STA_GET_SEC_CMD_SUP,  ///< Getting supported secure command classes
    ZWNET_STA_GET_NODE_INFO,    ///< Getting node info
    ZWNET_STA_ASSIGN_RR,        ///< Assign controller's return route
    ZWNET_STA_GET_CMD_VER,      ///< Getting command class version
    ZWNET_STA_GET_OTHERS,       ///< Getting other command classes info
    ZWNET_STA_GET_MANF_PDT_ID,  ///< Getting manufacturer and product id
    ZWNET_STA_WKUP_CFG,         ///< Configuring wake up interval and notification receiving node
    ZWNET_STA_MULTI_INSTANCE,   ///< Multi-instance command class handling
    ZWNET_STA_MULTI_CHANNEL,    ///< Multi-channel command class handling
    ZWNET_STA_GET_EP_INFO       ///< Getting endpoint info

} zwnet_ni_state_t;

///
/// ZWNET_STA_GET_NODE_INFO sub-state definitions
//#define NODE_INFO_SUBSTA_GET        1   ///< Getting cached node information
//#define NODE_INFO_SUBSTA_PROBE      2   ///< Getting NOP response from the node

///
/// S_EP_STA_GET_SENSOR sub-state definitions
#define SENSOR_SUBSTA_TYPE          3   ///< Getting sensor types
#define SENSOR_SUBSTA_UNIT          4   ///< Getting sensor units

///
/// S_EP_STA_GET_GROUP_INFO sub-state definitions
#define GRP_INFO_SUBSTA_CMD_LST     5   ///< Getting group command lists
#define GRP_INFO_SUBSTA_NAME        6   ///< Getting group name
#define GRP_INFO_SUBSTA_INFO        7   ///< Getting group info

///
/// S_EP_STA_GET_AV sub-state definitions
#define AV_SUBSTA_RPT_CNT           8   ///< Getting total simple AV control reports
#define AV_SUBSTA_CTL_RPT           9   ///< Getting simple AV control report

///
/// ZWNET_STA_GET_MANF_PDT_ID sub-state definitions
#define MS_SUBSTA_PDT_ID            10  ///< Getting manufacturer, product type and id
#define MS_SUBSTA_DEV_ID_OEM        11  ///< Getting OEM factory default device ID

///
/// S_EP_STA_GET_ALARM sub-state definitions
#define ALARM_SUBSTA_TYPE           12   ///< Getting alarm types
#define ALARM_SUBSTA_EVENT          13   ///< Getting alarm events


///
/// Endpoint info state-machine events
typedef enum
{
    EVT_EP_NW_TMOUT,            ///< Network timeout
    EVT_EP_SM_START,            ///< Start endpoint get node info state-machine
    EVT_EP_SEC_SUP_REPORT,      ///< Supported security command classes was received
    EVT_EP_VER_REPORT,          ///< Version report of a endpoint command class was received
    EVT_EP_S_EP_INFO_DONE       ///< Getting single endpoint information done

} zwnet_ep_evt_t;

///
/// Endpoint info state-machine's states
typedef enum
{
    EP_STA_IDLE,                ///< Waiting for event
    EP_STA_GET_SEC_CMD_SUP,     ///< Getting supported secure command classes
    EP_STA_GET_CMD_VER,         ///< Getting command class version
    EP_STA_GET_EP_INFO          ///< Getting single endpoint information

} zwnet_ep_state_t;

///
/// Action for single endpoint information state-machine command handler
typedef enum
{
    S_EP_ACT_ZWPLUS_INFO,   ///< Get Z-wave+ information
    S_EP_ACT_SNSR,          ///< Get multi-level sensor information
    S_EP_ACT_MAX_GROUP,     ///< Get maximum number of supported groupings
    S_EP_ACT_AGI,           ///< Get group information
    S_EP_ACT_THRMO_FAN_MD,  ///< Get supported thermostat fan modes
    S_EP_ACT_THRMO_MD,      ///< Get supported thermostat modes
    S_EP_ACT_THRMO_SETP,    ///< Get supported thermostat setpoint types
    S_EP_ACT_MUL_SWITCH,    ///< Get supported multilevel switch
    S_EP_ACT_AV,            ///< Get supported simple AV controls
    S_EP_ACT_ALARM,         ///< Get supported alarm types
    S_EP_ACT_PROT,          ///< Get supported protection states
    S_EP_ACT_MAX_USR_CODES, ///< Get maximum number of supported user codes
    S_EP_ACT_METER,         ///< Get supported meter
    S_EP_ACT_METER_DESC,    ///< Get meter descriptor
    S_EP_ACT_NAME_LOC       ///< Get node naming and location information
} zwnet_1_ep_act_t;

///
/// Single endpoint info state-machine events
typedef enum
{
    EVT_S_EP_NW_TMOUT,                    ///< Network timeout
    EVT_S_EP_SM_START,                    ///< Start endpoint get node info state-machine
    EVT_S_EP_INFO_REPORT,                 ///< Z-wave+ information report was received
    EVT_S_EP_SENSOR_INFO_REPORT,          ///< Sensor type and unit report was received
    EVT_S_EP_SENSOR_TYPE_REPORT,          ///< Sensor supported types report was received
    EVT_S_EP_SENSOR_UNIT_REPORT,          ///< Sensor supported units report was received
    EVT_S_EP_GRP_CNT_REPORT,              ///< Maximum number of supported groupings report was received
    EVT_S_EP_GRP_CMD_LST_REPORT,          ///< Group command list report was received
    EVT_S_EP_GRP_NAME_REPORT,             ///< Group name report was received
    EVT_S_EP_GRP_INFO_REPORT,             ///< Group information report was received
    EVT_S_EP_THRMO_FAN_MD,                ///< Supported thermostat fan modes report was received
    EVT_S_EP_THRMO_MD,                    ///< Supported thermostat modes report was received
    EVT_S_EP_THRMO_SETP,                  ///< Supported thermostat setpoint types report was received
    EVT_S_EP_MUL_SWITCH,                  ///< Supported multilevel switch report was received
    EVT_S_EP_AV_REPORT_CNT,               ///< Total number of AV reports was received
    EVT_S_EP_AV_CTL_REPORT,               ///< Supported simple AV control report was received
    EVT_S_EP_ALARM_TYPE,                  ///< Supported alarm types report was received
    EVT_S_EP_ALARM_EVENT,                 ///< Supported alarm events report was received
    EVT_S_EP_PROT,                        ///< Supported protection states report was received
    EVT_S_EP_USR_CODE_CNT_REPORT,         ///< Maximum number of supported user codes report was received
    EVT_S_EP_METER,                       ///< Supported meter report was received
    EVT_S_EP_NAME_REPORT,                 ///< Endpoint name report was received
    EVT_S_EP_LOC_REPORT,                  ///< Endpoint location report was received
    EVT_S_EP_METER_TBL_DESC               ///< Meter table id and table point admin number were received

} zwnet_1_ep_evt_t;

///
/// Single endpoint info state-machine's states
typedef enum
{
    S_EP_STA_IDLE,             ///< Waiting for event
    S_EP_STA_GET_ZWPLUS_INFO,  ///< Getting Z-wave+ information
    S_EP_STA_GET_SENSOR,       ///< Getting multilevel sensor supported types and units
    S_EP_STA_GET_MAX_GROUP,    ///< Getting maximum supported group
    S_EP_STA_GET_GROUP_INFO,   ///< Getting group information
    S_EP_STA_GET_THRMO_FAN_MD, ///< Getting supported thermostat fan operating modes
    S_EP_STA_GET_THRMO_MD,     ///< Getting supported thermostat modes
    S_EP_STA_GET_THRMO_SETP,   ///< Getting supported thermostat setpoint types
    S_EP_STA_GET_MUL_SWITCH,   ///< Getting supported multilevel switch
    S_EP_STA_GET_AV,           ///< Getting supported simple AV control
    S_EP_STA_GET_ALARM,        ///< Getting supported alarm
    S_EP_STA_GET_PROT,         ///< Getting supported protection states
    S_EP_STA_GET_MAX_USR_CODES,///< Getting maximum supported user codes
    S_EP_STA_GET_METER,        ///< Getting supported meter
    S_EP_STA_GET_METER_DESC,   ///< Getting meter descriptor
    S_EP_STA_GET_NODE_NAMELOC  ///< Getting name and location

} zwnet_1_ep_sta_t;

///
/// Action for node information state-machine command handler
typedef enum
{
    ACT_START,          ///< Start checking for features like wake up, multi-command, crc16
    ACT_VERSION,        ///< Get version information
    ACT_MANF_SPEC,      ///< Get manufacturer specific information
    ACT_DEVICE_CFG,     ///< Configure device specific based on device information database
    ACT_OTHERS,         ///< Get other command classes information
    ACT_END_POINT       ///< Get endpoints information

} zwnet_info_sm_act_t;

///
/// Network update state-machine events
typedef enum
{
    EVT_NU_TMOUT,               ///< Network timeout
    EVT_NU_START,               ///< Start network update
    EVT_NU_TOPOLOGY_FAILED,     ///< Network topology update failed
    EVT_NU_TOPOLOGY_DONE,       ///< Network topology update done
    EVT_NB_UPDT_FAILED,         ///< Neighbor update failed
    EVT_NB_UPDT_DONE,           ///< Neighbor update done
    EVT_NI_UPDT_DONE            ///< Node info update done

} zwnet_nu_evt_t;

///
/// Network update state-machine's states
typedef enum
{
    NU_STA_IDLE,                ///< Waiting for event
    NU_STA_TOPOLOGY_UPDT,       ///< Getting network topology update
    NU_STA_NEIGHBOR_UPDT,       ///< Getting node neighbor info
    NU_STA_NODE_INFO_UPDT       ///< Getting node info

} zwnet_nu_state_t;


///
/// AV "key hold" keep alive storage
typedef struct
{
    zwifd_t     ifd;            ///< Interface descriptor
    uint8_t     cmd[8];         ///< The "key hold" keep alive command
    uint8_t     run_tmr;        ///< Flag to control whether to continue running timer
    void        *tmr_ctx;       ///< Timer context

} av_hold_t;

///
/// Association report storage
typedef struct
{
    uint8_t group_id;           ///< Group id the report refers to
    uint8_t max_node;           ///< Maximum node the group id supported
    uint8_t ep_cnt;             ///< Number of endpoints in the buffer
    zwepd_p ep_buf;             ///< Buffer to store endpoints
} asc_rpt_t;


///
/// AV capabilities report
typedef struct
{
    uint8_t total_rpt;      ///< The number of reports necessary to report the entire list of supported AV Commands
    uint8_t rpt_num;        ///< The last report number (starting from 1) stored in the buffer
    uint8_t *bit_mask_buf;  ///< The buffer to store the bit mask of supported AV Control Commands by the device
    uint16_t bit_mask_len;  ///< The length of the bit mask in the bit_mask_buf
} zwif_av_cap_t;

/** Association Group Information / Association data structure (to support) */
#define ZW_AGI_GROUPNAME_STR_MAX	42

typedef struct  _CommandClass_Command_ID
{
	uint8_t CCMSB;	///< MSB byte of the command class
	uint8_t CCLSB;	///< LSB byte of the command class
	uint8_t CCID;	///< value of the command
	//struct _CommandClass_Command_ID	*pNext;
}CC_CCID;

typedef struct _NodeEpInfo
{
	uint8_t byNodeID;
	uint8_t byEpID;
}NodeEpInfo;

typedef struct  _Association_Group_Info_Group
{
	uint8_t	byGroupID;  // Group Identifieer
	uint16_t wProfile;
	uint16_t wEventCode;
	//CC_CCID *pCCList;
	CC_CCID stCCList;
	char	cchGroupName[ZW_AGI_GROUPNAME_STR_MAX + 1];
	uint8_t	byMaxNodes;  //max nodes supported
	NodeEpInfo stNodeEpInfo;
	//SimpleArray_NodeEpInfo *pNodeArray;
	struct _Association_Group_Info_Group *pNext;
}AGI_Group;

typedef struct _zwAGI_t
{
	uint8_t			byNofGroups;  /**< number of groups supported */
	AGI_Group		*pAGIGroupList; /**< AGI group list */
}
zwAGI_t, *zwAGI_p;

///
/// Supported secure command classes report
typedef struct
{
    uint8_t *cmd_cls_buf;   ///< The buffer to store the supported secure command classes
    uint8_t buf_len;        ///< The length of the secure command classes in the cmd_cls_buf
} zwif_sec_cmd_cls_t;


typedef void (*sm_fn)(zwnet_p nw);

///
/// The job for the node info state-machine to execute
typedef struct
{
    sm_fn           cb;             /**< The callback function when get node info has completed */
    uint8_t         *node_id;       /**< The array to store the node id of nodes */
    uint32_t        delay;          /**< The time delay to start the get node information process */
    uint8_t         num_node;       /**< The number of node in the node_id array */
    uint8_t         rd_idx;         /**< The index to the node_id array for the current node */
    uint8_t         op;             /**< The operation parameter for notify callback to pass
                                         once the get node information process is completed */
    //uint8_t         cfg_lifeline;   /**< Flag to determine whether to configure lifeline */

    /* Auto configuration of wakeup and sensor command classes*/
    uint8_t         auto_cfg;       /**< Flag to determine whether to auto configure wake up and sensor command classes */
} sm_job_t;

///
/// The job for the endpoint info state-machine to execute
typedef struct
{
    sm_fn           cb;             /**< The callback function when endpoint info state-machine has completed */
    zwep_p          first_ep;       /**< The first endpoint to query for info*/
} ep_sm_job_t;

#define NW_ROLE_PRIMARY     1       /**< SIS not present, primary controller*/
#define NW_ROLE_SECONDARY   2       /**< SIS not present, secondary controller*/
#define NW_ROLE_SUC         3       /**< SIS not present, SUC*/
#define NW_ROLE_INCLUSION   4       /**< SIS present, inclusion controller*/
#define NW_ROLE_SIS         5       /**< SIS*/

#define MAX_NEIGHBOR_UPDT_RETRIES   3   /**< maximum number of retries to find a failed node*/
///
/// Network update state-machine context
typedef struct
{
    uint8_t         failed_id[256]; /**< The array to store the node id of failed nodes */
    uint8_t         node_id[256];   /**< The array to store the node id of nodes */
    zwnet_nu_state_t    sta;        /**< The state of the network update state machine */
    uint8_t         num_node;       /**< The number of node in the node_id array */
    uint8_t         rd_idx;         /**< The index to the node_id array for the current node */
    uint8_t         nw_role;        /**< The network role of the controller */
    uint8_t         num_failed_node;/**< The number of node in the failed_id array */
    uint8_t         num_retry;      /**< The number of retries to find the failed nodes */

} nu_sm_ctx_t;

///
/// Store nodes to which the new SUC/SIS node id is sent
typedef struct
{
    uint8_t         node_id[256];            /**< The array to store the node id of nodes */
    uint8_t         basic_dev_cls[256];      /**< The basic device class of the node */
    uint8_t         num_node;                /**< The number of node in the node_id array */
    uint8_t         rd_idx;                  /**< The index to the node_id array for the current node */
    uint8_t         assign_suc;              /**< Flag to determine whether the operation is to assign SUC
                                                  or disable SUC */
    uint8_t         notify_cb;               /**< Flag to determine whether to notify application on completion */
} suc_updt_t;

//Forward declaration of security layer context
struct _sec_layer_ctx;

//Forward declaration of poll context
struct _poll_ctx;

/** controller capabilities bit-mask*/
#define CTLR_CAP_ENA_SUC    0x0001   /**< Controller is capable to enable SUC */
#define CTLR_CAP_SET_SUC    0x0002   /**< Controller is capable to set SUC node id */
#define CTLR_CAP_FLASH_PROG 0x0004   /**< Controller is capable to read/write flash */


typedef struct _zwnet
{
	uint32_t	            homeid;         /**< Network Home ID */
	zwnode_t		        ctl;	        /**< controller device, head in device list */
	zwnet_init_t	        init;		    /**< client initialization parameters */
    appl_layer_ctx_t        appl_ctx;       /**< Z-wave HCI application layer context */
    void                    *mtx;           /**< Mutex to access zwnet_t structure*/
    void                    *rst_mtx;       /**< Mutex for access reset done condition variable */
    void                    *rst_cv;        /**< Condition variable for reset done */
    volatile int            rst_cb_sts;     /**< Reset callback status. 0=callback not arrived yet; 1=reset done*/
	uint8_t		            ctl_lib_type;   /**< Controller Z-Wave Library Type*/
    uint8_t                 curr_op;        /**< Current executing operation*/
    uint8_t                 failed_id;      /**< Failed node id either to be removed or replaced*/
    uint8_t                 added_ctlr_id;  /**< Node id of the just added controller*/
    uint8_t                 added_node_new; /**< Flag to determine whether the just added node is new*/
    uint8_t                 added_node_id;  /**< Node id of the just added node*/
    uint8_t                 added_node_op;  /**< Current add node operation*/
    uint8_t                 nu_progress_sts;/**< Network update progress status*/
    uint8_t                 av_seq_num;     /**< AV sequence number to detect duplicates*/
    uint8_t                 sec_enable;     /**< Flag to enable security layer. 1 = enable, 0 = disable*/
    uint8_t                 poll_enable;    /**< Flag to indicate poll facility is enabled*/
    uint8_t                 snsr_rd_idx;    /**< Get sensor units read index*/
    uint8_t                 alrm_rd_idx;    /**< Get alarm events read index*/
    uint8_t                 grp_rd_idx;     /**< Get group info read index*/
    uint8_t                 ni_sm_sub_sta;  /**< The sub-state of the node info state machine */
    uint16_t                ni_sm_cls;      /**< Last command class of version get in the node info state-machine*/
    zwnet_ni_state_t        ni_sm_sta;      /**< The state of the node info state machine */
    zwnet_ni_state_t        rp_sm_sta;      /**< The state of the replace node id node info state machine */
    zwnet_ep_state_t        ep_sm_sta;      /**< The state of the endpoint info state machine */
    zwnet_1_ep_sta_t        single_ep_sta;  /**< The state of the single endpoint info state machine */
    sm_job_t                sm_job;         /**< The job for the node info state-machine to execute */
    ep_sm_job_t             ep_sm_job;      /**< The job for the endpoint info state-machine to execute */
    ep_sm_job_t             single_ep_job;  /**< The job for the single endpoint info state-machine to execute */
    nu_sm_ctx_t             nu_sm_ctx;      /**< The context of the network update state machine */
    void                    *sm_tmr_ctx;    /**< State machine timer context*/
    void                    *nu_sm_tmr_ctx; /**< Network update state machine timer context*/
    void                    *cb_tmr_ctx;    /**< Notify callback timer context*/
    int                     get_ni_sts;     /**< Add node operation get node info status*/
    zwepd_t                 ni_sm_ep;       /**< End point descriptor to use when time out occured in node info state machine*/
    zwif_wakeup_t           wake_up;        /**< Store the wake up setting*/
    zwif_av_cap_t           av_cap;         /**< Store the av capabilities setting*/
    zwif_sec_cmd_cls_t      sec_cmd_cls;    /**< Store the supported secure command classes*/
    suc_updt_t              suc_updt;       /**< Store nodes to be informed of the new SUC/SIS node id*/
    asc_rpt_t               asc_rpt;        /**< Store association (group) report which is split into multiple reports*/
    void                    *cmd_q_sem;     /**< Semaphore for waiting requests to execute queued commands*/
    void                    *cmd_q_mtx;     /**< Mutex for command queue thread */
    util_lst_t              *cmd_q_req_hd;  /**< Head of linked list for requests to execute queued commands*/
    void                    *wait_q_sem;    /**< Semaphore for waiting requests to execute queued commands for nodes
                                                 that require wakeup beam*/
    util_lst_t              *wait_q_req_hd; /**< Head of linked list for requests to execute queued commands for nodes
                                                 that require wakeup beam*/
    void                    *nw_exec_sem;   /**< Semaphore for waiting requests*/
    void                    *nw_exec_mtx;   /**< Mutex for requests to execute commands */
    util_lst_t              *nw_exec_req_hd;/**< Head of linked list for requests to execute commands*/
    void                    *cb_sem;        /**< Semaphore for callback requests*/
    void                    *cb_mtx;        /**< Mutex for requests to execute callback */
    util_lst_t              *cb_req_hd;     /**< Head of linked list for requests to execute callback*/
    volatile int            wait_q_thrd_run;/**< Control the command queue thread whether to run. 1 = run, 0 = stop*/
    volatile int            wait_q_thrd_sts;/**< Command queue thread status. 1 = run, 0 = thread exited*/
    volatile int            cmd_q_thrd_run; /**< Control the command queue thread whether to run. 1 = run, 0 = stop*/
    volatile int            cmd_q_thrd_sts; /**< Command queue thread status. 1 = run, 0 = thread exited*/
    volatile int            nw_exec_thrd_run;/**< Control the thread whether to run. 1 = run, 0 = stop*/
    volatile int            nw_exec_thrd_sts;/**< Remove failed node thread status. 1 = run, 0 = thread exited*/
    volatile int            cb_thrd_run;    /**< Control the thread whether to run. 1 = run, 0 = stop*/
    volatile int            cb_thrd_sts;    /**< Callback thread status. 1 = run, 0 = thread exited*/
    zwmeter_t               meter_desc;     /**< Store the meter descriptor*/
    av_hold_t               av_key_hold;    /**< Store the "key hold" keep alive*/
    plt_ctx_t               plt_ctx;        /**< Platform context */
    zwnetd_t                net_desc;       /**< Network descriptor */
    zwnoded_t               local_node;     /**< local node */
    zwnoded_t               suc_node;       /**< SUC node */
    struct _sec_layer_ctx   *sec_ctx;       /**< Security layer context*/
    struct _poll_ctx        *poll_ctx;      /**< Polling context*/
    zwnet_dev_rec_find_fn   dev_rec_find_fn;/**< User supplied function to find device record*/
    void                    *dev_cfg_ctx;   /**< User specified device configuration context for use in dev_rec_find_fn*/
    dev_cfg_buf_t           dev_cfg_bufs[4];/**< buffers that store an arrays of device specific configurations
                                                 with the first buffer has the highest priority for device matching */
#ifdef  TEST_SEC_WAKE_UP
    void                    *wkup_tmr_ctx;  /**< Timer context to send wake up notification*/
    uint8_t                 wkup_node_id;   /**< Node id to which wake up notification is sent*/
    uint8_t                 sec_wkup;       /**< Flag to determine whether send wake up notification securely*/
#endif
    uint8_t                 sec_chk;        /**< Flag to enable the checking of SECURITY_SCHEME_GET command*/
    uint8_t                 sec_node_id;    /**< The source node from which the SECURITY_SCHEME_GET command was sent*/
    uint8_t                 sec_scheme;     /**< The security scheme*/
    uint16_t                ctlr_cap;       /**< controller capabilities bit-mask CTLR_CAP_XXX */

#ifdef  ZW_STRESS_TEST
    zw_stress_tst_fn        stress_tst_cb;  /**< stress test receiving sequence number callback*/
#endif
#if defined (TEST_UTF8_NAME_LOC) || defined (TEST_UTF16_NAME_LOC)
    uint8_t                 name_ch_fmt;    /**< Name character representation*/
    uint8_t                 nameloc_n_len;  /**< Name location: name length*/
    uint8_t                 nameloc_n[ZW_LOC_STR_MAX];    /**< Name location: name*/
    uint8_t                 loc_ch_fmt;     /**< Location character representation*/
    uint8_t                 nameloc_l_len;  /**< Name location: location length*/
    uint8_t                 nameloc_l[ZW_LOC_STR_MAX];    /**< Name location: location*/
#endif
#if defined (TEST_HANDLE_BASIC_COMMAND_CLASS) || defined (TEST_EXT_CMD_CLASS)
    uint8_t                 basic_val;          /**< Basic command class value*/
#endif

	//Powerlevel request from another node
	uint8_t					byPowerLvlTimeout;	/**< Power level time out value in sec. Access this value must obtain mutex first */
	void					*pPowerLvl_tmr_ctx;	/**< Power level timer context */
	//Power level test report
	uint8_t					byPowerTstNodeID;   /**< Power level test node ID */
	uint8_t					byPowerTstStatus;	/**< Power level test status of operation */
	uint16_t				wPowerTstAckFrmCnt;	/**< Power level test frame acknowledged count */

	//Assocation / AGI data
	zwAGI_t					stAGIData; /**< Association CC and AGI CC data */

}
#ifdef WIN32
zwnet_t, *zwnet_p;
#else
zwnet_t;
#endif


void zwnet_node_rm_all(zwnet_p nw);
void zwnet_alrm_evt_rpt_cb(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt_msk);

/**
@}
@addtogroup If Interface APIs
@{
*/

/** Report in the interface */
typedef struct
{
	uint8_t		rpt_cmd;		/**< report command */
    void        *rpt_cb;        /**< pointer to report callback function */

} if_rpt_t;


/** interfaces within endpoint */
typedef struct _zwif
{
	zwobj_t		    obj;	        /**< base link object */
	struct _zwep    *ep;	        /**< back link to associated endpoint */
	uint16_t	    cls;	        /**< command class */
	uint8_t		    ver;	        /**< command class version */
	uint8_t		    propty;	        /**< Properties of the interface (bit-mask): IF_PROPTY_XXX */
    //Temporary data
    void            *tmp_data;      /**< temporary data which is specific to the command class*/
    //Persistent data to be stored in persistent storage
	uint8_t	        data_cnt;	    /**< number of data items */
	uint8_t	        data_item_sz;	/**< data item size in bytes*/
    void            *data;          /**< data items which is specific to the command class*/
	uint8_t		    rpt_num;/**< number of valid reports in rpt[]  */
    if_rpt_t        rpt[1]; /**< reports*/
}
#ifdef WIN32
zwif_t, *zwif_p;
#else
zwif_t;
#endif


/** Firmware update interface temporary data */
typedef struct
{
	uint16_t	max_frag_sz;    /**< Maximum meta data fragment size allowed for firmware update */
	uint16_t	frag_sz;        /**< Fragment size used in actual firmware update. Zero means stop any fragment transfer*/
	uint16_t	rpt_num;        /**< Pending report number to be sent to the device*/
	uint8_t	    rpt_cnt;        /**< Pending report count to be sent to the device*/
    uint8_t     fixed_frag_sz;  /**< Flag to indicate whether the max_frag_sz is fixed, i.e. firmware update
                                     request MUST use the given size. 1=fixed size; 0=variable size */
    FILE        *fw_file;       /**< Firmware file*/
    long        fw_file_sz;     /**< Firmware file size */

    //Firmware update target restart
    //
    zwnet_p     nw;             /**< Network*/
    zwnoded_t   node;           /**< Firmware update target node*/
    void        *restrt_tmr_ctx;/**< Firmware update target restart timer context*/

} if_fw_tmp_dat_t;


/** Sleeping device interface data */
typedef struct
{
	uint32_t	min;		/**< minimum */
	uint32_t	max;		/**< maximum */
	uint32_t	def;		/**< default */
	uint32_t	interval;	/**< steps between min and max (in seconds)*/
} if_wkup_data_t;


/** end point capability */
typedef struct
{
    uint8_t   ep;            ///< end point id
    uint8_t   ep_dynamic;    ///< flag to indicate whether end point is a dynamic end point
    uint8_t   gen;           ///< generic device class
    uint8_t   spec;          ///< specific device class
    uint8_t   cmd_cnt;       ///< the number of commands in the cmd_cls field
    uint16_t  *cmd_cls;      ///< command classes

} ep_cap_t;

uint8_t	_zwif_report_size(zwif_p intf);
/**<
get interface report size in bytes
@param[in]	intf		interface
@return	# bytes
*/

uint8_t	_zwif_report_precision(zwif_p intf);
/**<
get interface report precison in # decimal places
@param[in]	intf		interface
@return	# decimal places
*/

uint8_t	_zwif_report_unit(zwif_p intf);
/**<
get interface report unit
@param[in]	intf		interface
@return	unit
*/

#define _zwif_report_size(zwif)		    ((zwif)->format & 7)
#define _zwif_report_precision(zwif)	(((zwif)->format >> 3) & 3)
#define _zwif_report_unit(zwif)			((zwif)->format >> 5)

#define CHAR_PRES_ASCII     0       /**< ASCII*/
#define CHAR_PRES_OEM       1       /**< OEM Extended ASCII*/
#define CHAR_PRES_UTF16     2       /**< Unicode UTF-16*/

typedef void (*zwrep_info_fn)(zwif_p intf, zwplus_info_t *info);
/**<
ZWave+ info report callback
@param[in]	intf	    interface
@param[in]	info	    Z-Wave+ information
*/

typedef void (*zwrep_name_fn)(zwif_p intf, char *name, uint8_t len, uint8_t char_pres);
/**<
node name report callback
@param[in]	intf	    interface
@param[in]	name	    Node name
@param[in]	len	        Length of node name
@param[in]	char_pres   Char presentation
*/

typedef void (*zwrep_ver_fn)(zwif_p intf, uint16_t cls, uint8_t ver);
/**<
version report callback
@param[in]	intf	interface
@param[in]	cls	    The command class the version is referred
@param[in]	ver	    The version of cls
*/

typedef void (*zwrep_zwver_fn)(zwif_p intf, uint16_t proto_ver, uint16_t app_ver, uint8_t lib_type, ext_ver_t *ext_ver);
/**<
Z-wave versions report callback
@param[in]	intf	    interface
@param[in]	proto_ver	Z-Wave Protocol Version
@param[in]	app_ver	    Application Version
@param[in]	lib_type	Z-Wave Library Type
@param[in]	ext_ver	    Extended version information if this parameter is not NULL
*/

typedef void (*zwrep_mul_inst_fn)(zwif_p intf, uint8_t cls, uint8_t inst);
/**<
multi instance report callback
@param[in]	intf	interface
@param[in]	cls	    The command class the report is referred to
@param[in]	inst	The number of instances of a given command class
*/

typedef void (*zwrep_mc_cap_fn)(zwif_p intf, ep_cap_t *ep_cap);
/**<
multi channel capability report callback
@param[in]	intf	        interface
@param[in]	ep_cap	        Endpoint capability.
*/


typedef void (*zwrep_mc_ep_fn)(zwif_p intf, uint8_t num_of_ep, uint8_t flag);
/**<
multi channel end point report callback
@param[in]	intf	        interface
@param[in]	num_of_ep	    Number of end points embedded in the node. The maximum number of end points is 127.
@param[in]	flag	        Bit-7: 1 if the device has a dynamic number of end points.
                            Bit-6: 1 if all the end points in the node has the same generic and specific command class.
*/


typedef void (*zwrep_nameloc_fn)(zwif_p, zw_nameloc_p nameloc);
/**<
callback for node name and location string get_report
@param[in]	intf	interface
@param[in]	nameloc	name & location string null terminated
@return	ZW_ERR_xxx
@see zwif_get_report, zwif_set_reporter
*/

typedef void (*zwrep_manf_fn)(zwif_p intf, uint16_t *manf_pdt_id);
/**<
manufacturer and product id report callback
@param[in]	intf	        The interface that received the report
@param[in]	manf_pdt_id	    Array that stores manufacturer, product type and product id
@return
*/

typedef void (*zwrep_devid_fn)(zwif_p intf, dev_id_t *dev_id);
/**<
device id report callback
@param[in]	intf	        The interface that received the report
@param[in]	dev_id	        Device ID
@return
*/

typedef void (*zwrep_sup_sec_fn)(zwif_p intf, uint16_t *cls, uint8_t cnt);
/**<
supported security commands report callback
@param[in]	intf	The interface that received the report
@param[in]	cls	    The buffer that store the command classes
@param[in]	cnt	    Command class count
@return
*/

typedef void (*zwrep_grp_cmd_lst_fn)(zwif_p intf, uint8_t grp_id, uint8_t cmd_ent_cnt, grp_cmd_ent_t *cmd_lst);
/**<
group command list report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	cmd_ent_cnt	    Number of entries in the command list
@param[in]	cmd_lst	        Command list
@return
*/

typedef void (*zwrep_grp_name_fn)(zwif_p intf, uint8_t grp_id, uint8_t name_len, uint8_t *name);
/**<
group name report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_id	        Grouping identifier
@param[in]	name	        Group name in UTF-8 format
@param[in]	name_len        String length of group name
@return
*/

typedef void (*zwrep_av_raw_fn)(zwif_p intf, uint8_t cmd_len, uint8_t *cmd_buf);
/**<
simple AV raw report callback
@param[in]	intf	        The interface that received the report
@param[in]	cmd_buf	        Command buffer
@param[in]	cmd_len         Length of command buffer
@return
*/

/** group information entry*/
typedef struct
{
	uint8_t         grp_num;        /**< group number */
	uint16_t        profile;        /**< profile */
	uint16_t        evt_code;       /**< event code */
}
zw_grp_info_ent_t;

typedef void (*zwrep_grp_info_fn)(zwif_p intf, uint8_t grp_cnt, uint8_t dynamic, zw_grp_info_ent_t *grp_info);
/**<
group info report callback
@param[in]	intf	        The interface that received the report
@param[in]	grp_cnt	        Group count
@param[in]	dynamic	        Flag to indicate dynamic info. 1=dynamic; 0=static
@param[in]	grp_info        Group information
@return
*/

#define ZWIF_OPT_Q_EXTRA        0x01    /**< bit-mask to indicate there is extra parameter for queuing */
#define ZWIF_OPT_SKIP_CHK       0x02    /**< bit-mask to skip chekcing of input size */
#define ZWIF_OPT_SKIP_MUL_CH    0x04    /**< bit-mask to skip multi-channel encapsulation */
#define ZWIF_OPT_POLL           0x08    /**< bit-mask to indicate whether this is a poll frame */
#define ZWIF_OPT_SKIP_MUL_Q     0x10    /**< bit-mask to skip command queuing and multi-command encapsulation */
#define ZWIF_OPT_SKIP_WK_BEAM   0x20    /**< bit-mask to skip checking for node that requires wakeup beam */
#define ZWIF_OPT_CMD_ENTRY      0x40    /**< bit-mask to indicate the command entry mul_cmd_q_ent_t is in the xtra param */
#define ZWIF_OPT_WAIT_REPORT    0x80    /**< bit-mask to indicate whether to wait for report (for node that requires wakeup beam)
                                             Note: MUST be used together with ZWIF_OPT_SKIP_WK_BEAM  */
#define ZWIF_OPT_WAIT_RPT_SET   0x0100  /**< bit-mask to set wait for report (for node that requires wakeup beam)
                                             Note: MUST be used together with ZWIF_OPT_SKIP_ALL_IMD  */

#define ZWIF_OPT_SKIP_ALL_IMD   (ZWIF_OPT_SKIP_WK_BEAM | ZWIF_OPT_SKIP_CHK | ZWIF_OPT_SKIP_MUL_CH | ZWIF_OPT_SKIP_MUL_Q)
                                /**< Skip all intermediate levels except CRC-16, security and raw levels.*/

typedef int (*zwnet_sm_get_fn)(zwnet_p nw, zwif_p intf);
/**<
state-machine get report function
@param[in]	nw	            Network
@param[in]	intf	        The interface that received the report
@return
*/

/** state-machine handler data structure*/
typedef struct
{
	zwnet_sm_get_fn get_fn;     /**< state-machine get report function */
	uint16_t        cls;        /**< command class */
	uint16_t        alt_cls;    /**< alternate command class, 0= not in used */
}
zw_sm_hdlr_dat_t;

zwif_p zwif_find_cls(zwif_p first_intf, uint16_t cls);
uint8_t zwif_count(zwif_p first_intf);
void zwif_rep_hdlr(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t rx_sts);
void zwif_get_desc(zwif_p intf, zwifd_p desc);
void zwif_exec_cb(appl_layer_ctx_t    *appl_ctx, uint8_t tx_sts, void *user_prm);
zwif_p zwif_get_if(zwifd_p ifd);
zwif_p zwif_create(uint16_t cls, uint8_t ver, uint8_t propty);
int zwif_get_report(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd, tx_cmplt_cb_t cb);
int zwif_set_report(zwifd_p ifd, void *rpt_cb, uint8_t rpt);
void zwif_tx_sts_cb(appl_layer_ctx_t    *appl_ctx, uint8_t tx_sts, void *user_prm);
void zwif_grp_cb(appl_layer_ctx_t    *appl_ctx, uint8_t tx_sts, void *user_prm);
int zwif_nameloc_set(zwifd_p ifd, const zw_nameloc_p nameloc);
int zwif_exec_ex(zwifd_p ifd, uint8_t *cmd_buf, int buf_len, tx_cmplt_cb_t cb, void *user, int opt, void *xtra);
void zwif_cmd_q_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm);
void zwif_cmd_q_xtra_rm(zwnet_p nw, util_lst_t **xtra_lst_hd);
void zwif_rm(zwif_p intf);
void zwif_dat_rm(zwif_p intf);
int zwif_cls_cmd_get(uint8_t* cmd_buf, uint8_t len, uint16_t *cmd_cls, uint8_t *cmd);
void zwnet_av_raw_rpt_cb(zwif_p intf, uint8_t cmd_len, uint8_t *cmd_buf);
int zwif_ifd_get(zwifd_p ifd, uint16_t cmd_cls, zwifd_p cmd_cls_ifd);



/**
@}
@defgroup If_Sec Security Interface APIs
Security interface is meant for internal use only.
@{
*/

typedef void (*zwsec_nonce_fn)(zwif_p intf, uint8_t *nonce);
/**<
report callback when nonce is received
@param[in]	intf	interface
@param[in]	nonce	nonce
*/

/**
@}
@defgroup If_Firmware Firmware update Interface APIs
Used to update firmwares on a device
@{
*/

typedef void (*zw_fw_tx_fn)(zwif_p intf, uint16_t frag_num, uint8_t rpt_cnt);
/**<
send firmware fragment function
@param[in]	    intf        interface
@param[in]	    frag_num    fragment number requested (fragment number starts from 1)
@param[in]	    rpt_cnt     number of fragments requested
@return     ZW_ERR_XXX
*/


/** Power level data temporary data structure*/
typedef struct
{
	//Power level test request data
	uint8_t					byPowerLvl;			/**< Power level to test */
	uint16_t				wPowerTstFrmCnt;	/**< Power level test frame count to test*/
	uint16_t				wPowerTstCurrFrmCnt;	/**< Power level test current frame count */
	uint8_t					bysrc_node;			/**< Source node ID of the sender */
	uint8_t					bymsg_type;			/**< Msg type when the request is received */
}
zwpowerlvl_tst_tmp_t, *zwpowerlvl_tst_tmp_p;

/** Application command handler parameters */
typedef struct
{
    appl_layer_ctx_t    *appl_ctx;
    zwnet_p     nw;
    int         wait_2nd_frame;
    int         node_sec_ena;   //flag to indicate node is allowed to respond to security commands
    uint8_t     rx_sts;
    uint8_t     src_node;
    uint8_t     msg_type;       //incoming message type
}
appl_cmd_prm_t;

/** Handling function for association CC and AGI CC (to support) */
void handle_association(uint8_t cmd_len, uint8_t *cmd_buf, appl_cmd_prm_t *prm);


/**
@}
*/

#endif

