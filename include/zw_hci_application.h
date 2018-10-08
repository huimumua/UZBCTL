/**
@file   zw_hci_application.h - Z-wave host controller interface application layer implementation header file.

        To provide network management and controller functions.

@author David Chow

@version    1.0 21-5-10  Initial release

version: 1.0
comments: Initial release
*/
#ifndef _ZW_HCI_APPLICATION_DAVID_
#define _ZW_HCI_APPLICATION_DAVID_

#include "zw_hci_session.h"
#include "zwave/ZW_transport_api.h"


/**
@defgroup Application Application layer APIs
To provide network management and controller functions.
@{
*/
///
/// Boolean definition
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define APPL_CB_TMOUT_MIN        (SESSION_RESPONSE_TIMEOUT_MIN + 2000)  ///< Min. callback timeout from session layer
#define APPL_WAIT_SEND_TIMEOUT      7000    ///< The maximum wait time for sending a command in milliseconds
#define APPL_WAIT_SEND_MAX_RETRY    200     ///< The maximum retries for sending a command in unit of 10 milliseconds
#define TRANSMIT_COMPLETE_NO_CB     0x05    ///< Timeout waiting for callback frame
#ifndef ZW_FAILED_NODE_REMOVE_STARTED
#define ZW_FAILED_NODE_REMOVE_STARTED   0   ///< Status of  zw_remove_failed_node_id() call when the remove operation is started
#endif

///
/// Node information
typedef struct
{
    uint8_t   node_id;       ///< node identifier
    uint8_t   basic;         ///< basic device class
    uint8_t   gen;           ///< generic device class
    uint8_t   spec;          ///< specific device class
    uint8_t   cmd_cnt;       ///< the number of command classes in the unsecure cmd_cls field
    uint16_t  *cmd_cls;      ///< unsecure command classes

} appl_node_info_t;

///
/// Send data parameters
typedef struct
{
    uint8_t   node_id;      ///< node identifier
    uint8_t   tx_opt;       ///< transmit option
    uint8_t   dat_len;      ///< the length of the dat_buf field
    uint8_t   dat_buf[1];   ///< place holder for the data buffer

} appl_snd_data_t;

///
/// Send data multi parameters
typedef struct
{
    uint8_t   tx_opt;       ///< transmit option
    uint8_t   dat_len;      ///< the length of the dat_buf field
    uint8_t   *dat_buf;     ///< buffer for the data
    uint8_t   nodes_num;    ///< number of nodes to send data
    uint8_t   *nodes;       ///< destination nodes to send data

} appl_snd_data_multi_t;

///
/// Set SUC node id parameters
typedef struct
{
    uint8_t   node_id;      ///< node identifier
    uint8_t   suc_sta;      ///< enable/disable the static controller to be SUC node
    uint8_t   tx_opt;       ///< transmit option
    uint8_t   cap;          ///< SUC capability to be enabled

} appl_set_suc_t;

///
/// HCI capabilities
typedef struct
{
    uint8_t   version;          ///< serial api version
    uint8_t   revision;         ///< serial api revision
    uint16_t  manf_id;          ///< serial api manufacturer id
    uint16_t  manf_pdt_typ;     ///< serial api manufacturer product type
    uint16_t  manf_pdt_id;      ///< serial api manufacturer product id
    uint8_t   func_id[32];      ///< bitmask where every Serial API function ID which is supported
        ///< has a corresponding bit in the bitmask set to ‘1’. All Serial API function IDs which are not
        ///< supported have their corresponding bit set to ‘0’. First byte in bitmask corresponds to FuncIDs 1-8
        ///< where bit 0 corresponds to FuncID 1 and bit 7 corresponds to FuncID 8. Second byte in bitmask then
        ///< corresponds to FuncIDs 9-16 and so on.
} appl_hci_cap_t;

///
/// Initialization info
typedef struct
{
    uint8_t   version;      ///< serial api version
    uint8_t   cap;          ///< bitmask of capabilities
    uint8_t   chip_typ;     ///< chip type
    uint8_t   chip_ver;     ///< chip version
    uint8_t   nodes[29];    ///< bitmask of nodes currently stored in the EEPROM
} appl_init_dat_t;

//Forward declaration of application layer context
struct _appl_layer_ctx;

///
/// Transmit completion status callback function
typedef void (*tx_cmplt_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t tx_sts, void *user_prm);

///
/// Add node to network callback function
typedef void (*add_node_nw_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_id, appl_node_info_t *node_info);

///
/// Set learn mode callback function
typedef void (*set_lrn_mod_cb_t)(struct _appl_layer_ctx *appl_ctx, uint8_t sts, uint8_t node_id, uint8_t cmd_len, uint8_t *cmd_cls);

///
/// Set default callback function
typedef void (*set_deflt_cb_t)(struct _appl_layer_ctx *appl_ctx);

///
/// Write to NVMEM completion callback function
typedef void (*wr_cmplt_cb_t)(struct _appl_layer_ctx *appl_ctx, void *user_prm);

///
/// Z-wave HCI application layer context
typedef struct _appl_layer_ctx
{
    //Callback functions to high-level application layer
    void    (*application_command_handler_cb)(struct _appl_layer_ctx   *appl_ctx, uint8_t rx_sts, uint8_t src_node, uint8_t cmd_len, uint8_t *cmd_buf);///< Pointer to application command handler callback function
    void    (*application_update_cb)(struct _appl_layer_ctx   *appl_ctx, uint8_t sts, appl_node_info_t *node_info);///< Pointer to application update callback function

    //Data
    void      *data;                ///< For high-level application layer to store data/context
    void      *snd_mtx;             ///< Mutex for sending command
    void      *snd_cv;              ///< Condition variable for sending command
    tx_cmplt_cb_t send_data_cb;     ///< Send data transmit completion status callback function (used by timer callback only)
    void      *snd_dat_cb_prm;      ///< Send data transmit completion status callback user parameter (used by timer callback only)
    void      *cb_tmr_ctx;          ///< Callback waiting timer context
    uint32_t  cb_tmout_ms;          ///< Callback timeout from session layer in milliseconds
    volatile int wait_cmd_cb;       ///< Flag to determine whether to wait for command callback in order
                                    ///< for the next command to be sent.
    volatile int wait_nm_cb;        ///< Flag to indicate whether to wait for network management command callback
                                    ///< in order for the next command to be sent.
    ssn_layer_ctx_t   ssn_ctx;      ///< Session layer context
    plt_ctx_t         *plt_ctx;     ///< Platform context

} appl_layer_ctx_t;

int32_t    zw_send_data_abort(appl_layer_ctx_t   *appl_ctx);
int32_t    zw_get_node_protocol_info(appl_layer_ctx_t   *appl_ctx,
                                     uint8_t  node_id,
                                     uint8_t *node_info);
int32_t    zw_controller_change(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb);
int32_t    zw_create_new_primary_ctrl(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb);
int32_t    zw_get_suc_node_id(appl_layer_ctx_t   *appl_ctx, uint8_t *suc_node_id);
int32_t    zw_send_suc_id(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t  tx_opt, tx_cmplt_cb_t cb);
int32_t    zw_set_suc_node_id(appl_layer_ctx_t   *appl_ctx, appl_set_suc_t *set_suc_param, tx_cmplt_cb_t cb);
int32_t    zw_request_network_update(appl_layer_ctx_t   *appl_ctx, tx_cmplt_cb_t cb);
int32_t    zw_enable_suc(appl_layer_ctx_t   *appl_ctx, uint8_t state, uint8_t cap, uint8_t *res);
int32_t    zw_assign_suc_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t src_node_id, tx_cmplt_cb_t cb);
int32_t    zw_delete_suc_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb);
int32_t    zw_request_node_neighbor_update(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb);
int32_t    zw_is_failed_node(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t *res);
int32_t    zw_remove_failed_node_id(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, uint8_t *resp_flg);
int32_t    zw_replace_failed_node(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, uint8_t *resp_flg);
int32_t    zw_replication_send(appl_layer_ctx_t   *appl_ctx, appl_snd_data_t  *prm, tx_cmplt_cb_t cb);
int32_t    zw_replication_receive_complete(appl_layer_ctx_t   *appl_ctx);
int32_t    zw_assign_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t src_node_id, uint8_t dst_node_id, tx_cmplt_cb_t cb, void *cb_prm);
int32_t    zw_delete_return_route(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, tx_cmplt_cb_t cb, void *cb_prm);
int32_t    zw_soft_reset(appl_layer_ctx_t   *appl_ctx);
int32_t    zw_request_node_info(appl_layer_ctx_t   *appl_ctx, uint8_t  node_id);
int32_t    zw_set_default(appl_layer_ctx_t   *appl_ctx, set_deflt_cb_t cb);
int32_t    zw_set_learn_mode(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, set_lrn_mod_cb_t cb);
int32_t    zw_serial_api_get_init_data(appl_layer_ctx_t   *appl_ctx, appl_init_dat_t  *init_data);
int32_t    zw_serial_api_capabilities(appl_layer_ctx_t   *appl_ctx, appl_hci_cap_t  *cap);
int32_t    zw_get_controller_capabilities(appl_layer_ctx_t   *appl_ctx, uint8_t *cap);
int32_t    zw_memory_get_id(appl_layer_ctx_t   *appl_ctx, uint32_t  *home_id, uint8_t *node_id);
int32_t    zw_memory_get_buffer(appl_layer_ctx_t   *appl_ctx, uint8_t  *buf, uint16_t ofs, uint8_t len);
int32_t    zw_memory_put_buffer(appl_layer_ctx_t   *appl_ctx, uint8_t  *buf, uint16_t ofs,
                                uint16_t len, wr_cmplt_cb_t cb, void *user_prm);
int32_t    zw_remove_node_from_network(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb);
int32_t    zw_add_node_to_network(appl_layer_ctx_t   *appl_ctx, uint8_t  mode, add_node_nw_cb_t cb);
int32_t    zw_version(appl_layer_ctx_t   *appl_ctx, uint8_t  *lib_ver, uint8_t *lib_type);
int32_t    zw_send_data(appl_layer_ctx_t   *appl_ctx, appl_snd_data_t  *prm, tx_cmplt_cb_t cb, void *cb_prm);
int32_t    application_node_info(appl_layer_ctx_t   *appl_ctx, appl_node_info_t *node_info, uint8_t dev_opt);
int32_t    zwhci_init(appl_layer_ctx_t   *appl_ctx, void   *comm_port_id);
void       zwhci_exit(appl_layer_ctx_t   *appl_ctx);
int32_t    zw_set_rf_receive_mode(appl_layer_ctx_t   *appl_ctx, uint8_t mode);
int32_t    zw_get_random_word(appl_layer_ctx_t   *appl_ctx, uint8_t *count, uint8_t *buf);
int32_t    zw_send_node_info(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t  tx_opt, tx_cmplt_cb_t cb);
int32_t    zw_get_rf_powerlevel(appl_layer_ctx_t   *appl_ctx, uint8_t *pbyPowerLvl);
int32_t    zw_set_rf_powerlevel(appl_layer_ctx_t   *appl_ctx, uint8_t byPowerLvl, uint8_t *pbyPowerLvlRet);
int32_t    zw_send_test_frame(appl_layer_ctx_t   *appl_ctx, uint8_t node_id, uint8_t byPowerLvl, tx_cmplt_cb_t cb, void *cb_prm);
int        zw_flash_auto_prog_mode(appl_layer_ctx_t *appl_ctx);

/**
@}
*/

#endif /* _ZW_HCI_APPLICATION_DAVID_ */
