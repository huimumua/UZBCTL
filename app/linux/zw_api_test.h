/**
@file   zw_api_test.h - Z-wave host controller API test program header file.

        To test Z-wave host controller API.

@author David Chow

@version    1.0 13-4-11  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_API_TEST_DAVID_
#define _ZW_API_TEST_DAVID_
/**
@defgroup API_TEST Z-wave host controller API test functions
To test Z-wave host controller API.
@{
*/

#define APPL_CONFIG_FILE    "zw_api_test.cfg"  //Application configuration file
//#define USER_APPL_DEVICE_CFG        //User application manages the device specific configurations
//#define DEBUG_FLASH_APM       //Debug flash APM

#define  MAX_LOADSTRING 100

#define  ENABLE_SUC         1
#define  ENABLE_SIS         2
#define  DISABLE_SUC        3

#define  DESC_TYPE_NODE     1
#define  DESC_TYPE_EP       2
#define  DESC_TYPE_INTF     3


///
/// Container for different types of descriptor
typedef struct  _desc_cont
{
    struct  _desc_cont  *next;      ///< The next (same level) descriptor
    struct  _desc_cont  *down;      ///< The child (one lower level) descriptor
    uint32_t            id;         ///< Unique descriptor id
    uint32_t            type;       ///< The descriptor type in this container
    uint8_t             desc[1];    ///< Place holder for the descriptor

} desc_cont_t;


///
/// Test statistic
typedef struct
{
    uint32_t            rx_seq_num_err;     ///< Number of times received sequence number error
    uint32_t            rx_seq_num_frm;     ///< Number of sequence number frames received
    uint32_t            tx_seq_num_frm;     ///< Number of sequence number frames sent
    uint32_t            tx_multi_lvl_frm;   ///< Number of multi-level switch "set level" frames sent
} test_stat_t;

///
/// High-level application context
typedef struct _hl_appl_ctx
{
    volatile    int32_t is_to_run_test;             ///< Flag to control the test thread to run test
    int32_t             port_number;                ///< The comm port number to use for communication with controller
    uint32_t            exp_seq_num;                ///< Expected received sequence number
    uint32_t            seq_num;                    ///< Sequence number to send
    uint32_t            home_id;                    ///< The Z-wave network home-id
    uint32_t            is_init_done;               ///< Flag to determine whether initialization is done
    int32_t             (*stress_tst_func)(struct _hl_appl_ctx   *hl_appl);   ///< Pointer to the stress test function.
    int32_t             is_stress_tst_run;          ///< Flag to indicate whether stress test is running
    int32_t             is_ni_stress_tst_run;       ///< Flag to indicate whether node update stress test is running
    uint8_t             basis_api_ver[20];          ///< Basis API version
    uint32_t            dst_desc_id;                ///< The destination descriptor id where commands are sent
    uint32_t            suc_node_id;                ///< The SUC node id
    uint32_t            failed_node_id;             ///< The failed node id to be removed/replaced
    uint32_t            rep_desc_id;                ///< The report receiving interface descriptor id
    uint32_t            intf_desc_id;               ///< Interface descriptor id
    uint32_t            node_desc_id;               ///< Node descriptor id
    uint32_t            temp_desc;                  ///< Temporary descriptor id
    uint32_t            wkup_interval;              ///< Wake up interval in seconds
    uint32_t            ep_desc_id[5];              ///< Endpoint descriptor id
    uint32_t            desc_id;                    ///< Descriptor id
    uint8_t             lvl;                        ///< Multi-level value
    uint8_t             cap;                        ///< Capabilities of controller
    uint8_t             lib_type;                   ///< Library type of basis API
    uint8_t             mul_lvl_dur;                ///< Multi level dimming duration
    uint8_t             mul_lvl_dir;                ///< Multi level primary switch change direction
    uint8_t             mul_lvl_sec_dir;            ///< Multi level secondary switch change direction
    uint8_t             mul_lvl_sec_step;           ///< Multi level secondary switch step size
    uint8_t             mul_lvl_change_started;     ///< Flag to indicate whether multi level change started
    uint16_t            mul_lvl_val;                ///< Multi level set value
    uint16_t            cfg_range_start;            ///< Configuration parameter number range start
    uint16_t            cfg_range_end;              ///< Configuration parameter number range end
    uint8_t             cfg_param_mode;             ///< Configuration parameter number mode: 0 = single, 1=range
    uint8_t             cfg_param;                  ///< Configuration parameter number
    uint8_t             cfg_size;                   ///< Configuration parameter size
    uint8_t             cfg_value_default;          ///< Configuration parameter value flag: 1=use device default, 0=use cfg_value
    int32_t             cfg_value;                  ///< Configuration parameter value
    uint8_t             bin_state;                  ///< Binary switch state: 0 = off, 1 = on
    uint8_t             cmd_q_ctl;                  ///< Commmand queuing control: 0 = disable, 1 = enable
    uint16_t            basic_val;                  ///< Basic command value
    uint16_t            ind_val;                    ///< Indicator value
    uint8_t             local_prot;                 ///< Local protection state
    uint8_t             rf_prot;                    ///< RF protection state
    uint8_t             time;                       ///< Time
    uint8_t             mul_cmd_ctl;                ///< Multi Command Encapsulation control: 0 = off, 1 = on
    uint8_t             group_id;                   ///< Group id
    uint8_t             node_id;                    ///< Node id
    uint8_t             command[22];                ///< Command and parameters
    uint8_t             cmd_len;                    ///< Command length
    uint8_t             meter_unit;                 ///< Preferred meter reading unit
    uint8_t             usr_id;                     ///< User id for user code
    uint8_t             dlck_mode;                  ///< Door lock operation mode
    uint8_t             broadcast;                  ///< Flag to determine whether to transmit frame in broadcast mode
    uint8_t             alrm_vtype;                 ///< Vendor specific alarm type
    uint8_t             alrm_ztype;                 ///< Z-wave alarm type
    uint8_t             alrm_evt;                   ///< Z-wave alarm event
    uint8_t             alrm_sts;                   ///< Alarm status
    uint8_t             sensor_type;                ///< Sensor type
    uint8_t             sensor_unit;                ///< Sensor unit
    uint8_t             weekday;                    ///< Day of a week
    uint8_t             hour;                       ///< Hour (24 hours format)
    uint8_t             minute;                     ///< Minute
    uint8_t             thrmo_md;                   ///< Thermostat operating mode
    uint8_t             thrmo_fan_md;               ///< Thermostat fan operating mode
    uint8_t             thrmo_fan_off;              ///< Turn thermostat fan off (depend on thrmo_fan_off_cap)
    uint8_t             thrmo_fan_off_cap;          ///< Thermostat fan off mode capability: 0 = not supported, 1 = supported
    uint8_t             thrmo_setb_sta;             ///< Thermostat setback state
    uint8_t             thrmo_setb_typ;             ///< Thermostat setback type
    int8_t              thrmo_setb_deg;             ///< Thermostat setback in units of 1/10 degree
    uint8_t             thrmo_setp_typ;             ///< Thermostat setpoint type
    zwsetp_t            thrmo_setp_data;            ///< Thermostat setpoint data
    zwcc_shed_t         clmt_ctl_schd;              ///< Climate control schedule
    zwcc_shed_ovr_t     clmt_ctl_schd_ovr;          ///< Climate control schedule override
    zwdlck_cfg_t        dlck_config;                ///< Door lock configuration
    zwusrcod_t          usr_code;                   ///< User code
    test_stat_t         test_stat;                  ///< Test statistic
    zwnoded_t           node_updt_desc;             ///< Store the descriptor of the node pending for update info
    zwnoded_t           node_add_desc;              ///< Store the descriptor of the last added node
    zwnoded_t           node_rp_desc;               ///< Store the descriptor of the replaced node
    appl_hci_cap_t      hci_cap;                    ///< HCI capabilities
    appl_init_dat_t     init_dat;                   ///< Initialization data from the controller
    appl_layer_ctx_t    appl_ctx;                   ///< Z-wave HCI application layer context
    zwnet_p             zwnet;                      ///< Z-wave network
    zw_nameloc_t        nameloc;                    ///< The name location of a node
	char	            meter_admin[ZW_ADMIN_STR_MAX + 1];    ///< Meter admin name
    desc_cont_t         *desc_cont_hd;              ///< Head of descriptor container
    void                *desc_cont_mtx;             ///< Mutex to access descriptor container
    char			    node_info_file[200];        ///< file name of the node information file
    char			    save_file[200];             ///< file name where the node information to be saved
	int				    load_ni_file;	            ///< flag to determine whether to load node info file on init. 1=yes, 0=no
	int				    save_ni_file;	            ///< flag to determine whether to save node info file on exit. 1=yes, 0=no
    int                 enable_rec_cmd;             ///< Flag to indicate whether to enable command recording
    void                *plt_ctx;                   ///< Platform context for printing of output text messages
    uint16_t            av_btn_down;                ///< Flag to indicate whether the AV button is down
    uint16_t            av_btn_ctl;                 ///< AV Button control code
    char                comm_port_name[80];         ///< Comm port name
    uint16_t            fw_vid;                     ///< Vendor/Manufacturer id
    uint16_t            fw_id;                      ///< Firmware id
    uint16_t            fw_frag_sz;                 ///< Meta data fragment size for firmware update
    uint8_t             fw_tgt;                     ///< Firmware target to update
    uint8_t             poll_ctl;                   ///< Polling control: 0 = off, 1 = on
    zwpoll_req_t        poll_req;                   ///< Polling request
    uint32_t            poll_id;                    ///< Polling request identifier
    int                 poll_id_type;               ///< Polling request identifier type: 0=handle, 1=token
	uint8_t				pow_lvl;					///< Power level indicator value
	uint8_t				pow_lvl_timeout;			///< Power level timeout value: 1-255
	uint8_t				pow_lvl_test_node_desc_id;		///< Power level test node descriptor ID
	uint16_t			pow_lvl_test_wframcnt;		///< Power level test frame count
#ifdef  USER_APPL_DEVICE_CFG
    dev_cfg_buf_t       dev_cfg_bufs[4];            ///< buffers that store an arrays of device specific configurations
                                                    ///< with the first buffer has the highest priority for device matching
#endif
} hl_appl_ctx_t;


typedef struct{
    hl_appl_ctx_t   *hl_appl;   //< high level application context
    uint16_t        sts;        //< status of current operation
    uint8_t         op;         //< network operation ZWNET_OP_XXX

} nw_notify_msg_t;

void    *hl_plt_ctx_get(hl_appl_ctx_t *hl_appl);
void    *hl_if_plt_ctx_get(zwifd_p ifd);
void    *hl_ep_plt_ctx_get(zwepd_p ep);
void    *hl_node_plt_ctx_get(zwnoded_p node);
zwifd_p    hl_intf_desc_get(desc_cont_t *head, uint32_t desc_id);
zwnoded_p    hl_node_desc_get(desc_cont_t *head, uint32_t desc_id);
zwepd_p    hl_ep_desc_get(desc_cont_t *head, uint32_t desc_id);
uint32_t    hl_desc_id_get(desc_cont_t *head, zwnoded_p noded);
int  hl_int_get(uint8_t *byte_buf,  uint8_t size, int32_t *int_val);
void  hl_float_get(int32_t int_value,  uint8_t precision, uint8_t buf_size, char *out_buf);


int     hl_nif_send(hl_appl_ctx_t   *hl_appl);
int     hl_init(hl_appl_ctx_t   *hl_appl);
void    hl_exit(hl_appl_ctx_t   *hl_appl);
void    hl_nodes_info_dump(hl_appl_ctx_t   *hl_appl);
void    hl_info_get(hl_appl_ctx_t   *hl_appl);
void    hl_node_desc_dump(hl_appl_ctx_t   *hl_appl);
void    hl_nw_notify_hdlr(nw_notify_msg_t *notify_msg);
int32_t hl_nw_node_add(hl_appl_ctx_t   *hl_appl);
int32_t hl_nw_node_rm(hl_appl_ctx_t   *hl_appl);
int32_t hl_ctlr_chg(hl_appl_ctx_t   *hl_appl);
int32_t hl_lrn_mod_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_failed_id_rp(hl_appl_ctx_t   *hl_appl);
int32_t hl_failed_id_rm(hl_appl_ctx_t   *hl_appl);
int32_t hl_req_nw_updt(hl_appl_ctx_t   *hl_appl);
int32_t hl_node_updt(hl_appl_ctx_t   *hl_appl);
//int32_t hl_suc_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_deflt_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_seq_num_snd(hl_appl_ctx_t   *hl_appl);
int32_t hl_multi_lvl_snd(hl_appl_ctx_t   *hl_appl);
void hl_test_rpt_show(hl_appl_ctx_t   *hl_appl);
void    hl_stress_test(hl_appl_ctx_t   *hl_appl);
int32_t hl_bin_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_binary_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_bin_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_multi_lvl_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_multi_lvl_rep_get(hl_appl_ctx_t   *hl_appl);
void    hl_multi_lvl_chg(hl_appl_ctx_t   *hl_appl);
int32_t hl_multi_lvl_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_multi_lvl_sup(hl_appl_ctx_t   *hl_appl);
int32_t hl_bin_snsr_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_bin_snsr_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_ml_snsr_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_ml_snsr_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_ml_snsr_sup_get(hl_appl_ctx_t *hl_appl);
int     hl_ml_snsr_unit_get(hl_appl_ctx_t *hl_appl);
int32_t hl_grp_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_add(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_del(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_sup(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_active(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_cmd_sup(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_cmd_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_grp_cmd_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_cmd_q_ctl_get(hl_appl_ctx_t   *hl_appl, uint8_t *q_ctl_state);
int32_t hl_cmd_q_ctl_set(hl_appl_ctx_t   *hl_appl);
void hl_cmd_q_view(hl_appl_ctx_t   *hl_appl);
int hl_cmd_q_cancel(hl_appl_ctx_t   *hl_appl);
int32_t hl_wkup_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_wkup_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_cfg_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_cfg_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_cfg_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_nameloc_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_pulsemeter_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_pulsemeter_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_sup(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_reset(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_admin_cap_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_meter_admin_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_av_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_av_set(hl_appl_ctx_t   *hl_appl, uint16_t ctl, uint8_t down);
int32_t hl_basic_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_basic_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_basic_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_dlck_op_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_dlck_op_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_dlck_op_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_dlck_cfg_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_dlck_cfg_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_usrcod_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_usrcod_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_usrcod_sup_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_alrm_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_alrm_rep_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_alrm_set(hl_appl_ctx_t   *hl_appl);
int32_t hl_alrm_sup_get(hl_appl_ctx_t   *hl_appl);
int32_t hl_alrm_sup_evt_get(hl_appl_ctx_t *hl_appl);
int32_t hl_battery_rep_setup(hl_appl_ctx_t   *hl_appl);
int32_t hl_battery_rep_get(hl_appl_ctx_t   *hl_appl);
void    hl_close(hl_appl_ctx_t   *hl_appl);
void    hl_save(hl_appl_ctx_t   *hl_appl);
void    hl_ni_stress_test(hl_appl_ctx_t   *hl_appl);
void    hl_bin2str(void * buf, uint32_t len, char *hex_str, uint32_t hex_str_len);
int     hl_thrmo_fan_md_rep_setup(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_fan_md_set(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_fan_md_sup_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_fan_md_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_fan_sta_rep_setup(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_fan_sta_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_md_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_thrmo_md_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_md_set(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_md_sup_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_op_sta_rep_setup(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_op_sta_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_setb_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_thrmo_setb_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_setb_set(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_setp_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_thrmo_setp_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_setp_set(hl_appl_ctx_t   *hl_appl);
int     hl_thrmo_setp_sup_get(hl_appl_ctx_t   *hl_appl);
int     hl_clock_rep_setup(hl_appl_ctx_t   *hl_appl);
int     hl_clock_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_clock_set(hl_appl_ctx_t   *hl_appl);
int     hl_clmt_ctl_schd_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_clmt_ctl_schd_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_clmt_ctl_schd_set(hl_appl_ctx_t   *hl_appl);
int     hl_clmt_ctl_schd_chg_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_clmt_ctl_schd_chg_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_clmt_ctl_schd_ovr_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_clmt_ctl_schd_ovr_set(hl_appl_ctx_t   *hl_appl);
int     hl_mul_cmd_ctl_set(hl_appl_ctx_t   *hl_appl);
int     hl_mul_cmd_ctl_get(hl_appl_ctx_t   *hl_appl, uint8_t *on);
int     hl_prot_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_prot_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_prot_set(hl_appl_ctx_t   *hl_appl);
int     hl_prot_sup_get(hl_appl_ctx_t   *hl_appl);
int     hl_prot_ec_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_prot_ec_set(hl_appl_ctx_t   *hl_appl);
int     hl_prot_tmout_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_prot_tmout_set(hl_appl_ctx_t   *hl_appl);
int     hl_appl_sts_rep_setup(hl_appl_ctx_t  *hl_appl);
int     hl_ind_rep_setup(hl_appl_ctx_t   *hl_appl);
int     hl_ind_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_ind_set(hl_appl_ctx_t   *hl_appl);
int     hl_fw_info_get(hl_appl_ctx_t *hl_appl);
int     hl_fw_updt_req(hl_appl_ctx_t *hl_appl);
int     hl_poll_rm(hl_appl_ctx_t *hl_appl);
int     hl_power_level_rep_setup(hl_appl_ctx_t *hl_appl);
int     hl_power_level_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_power_level_set(hl_appl_ctx_t   *hl_appl);
int     hl_power_level_test_rep_get(hl_appl_ctx_t   *hl_appl);
int     hl_power_level_test_set(hl_appl_ctx_t   *hl_appl);
#ifndef OS_MAC_X
int     hl_fw_updt(hl_appl_ctx_t   *hl_appl);
#endif
#ifdef DEBUG_FLASH_APM
int     hl_auto_prog(hl_appl_ctx_t   *hl_appl);
#endif
int     hl_quick_reset(hl_appl_ctx_t *hl_appl);



/**
@}
*/



#endif /* _ZW_API_TEST_DAVID_ */

