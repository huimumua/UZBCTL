/**
@file   zw_hci_session.h - Z-wave host controller interface session layer implementation header file.

        To handle commands sending and the corresponding responses and/or callback.

@author David Chow

@version    1.0 14-5-10  Initial release

version: 1.0
comments: Initial release
*/
#ifndef _ZW_HCI_SESSION_DAVID_
#define _ZW_HCI_SESSION_DAVID_

#include "zw_hci_frame.h"
/**
@defgroup Session Session layer APIs
To handle commands sending and the corresponding responses and/or callback.
@{
*/

// Offset of the field in the Serial API frame passed from the
// frame layer
#define SESSION_LENGTH_OFFSET          0
#define SESSION_TYPE_OFFSET            1
#define SESSION_COMMAND_ID_OFFSET      2
#define SESSION_DATA_OFFSET            3

// Command flag to indicate what type of response is expected
#define COMMAND_HAS_RESPONSE        0x01
#define COMMAND_HAS_CALLBACK        0x02


//Minimum send frame time out value
#define     SESSION_RESPONSE_TIMEOUT_MIN      (FRAME_SEND_TIMEOUT_MIN * (FRAME_MAX_RESEND + 1) + 500)    ///< Minimum response to command time out value in milliseconds

//Maximum funcID value
#define     SESSION_FUNCID_MAX      252   ///< Maximum funcID value used to identify which callback to call

///
/// Z-wave HCI session layer state-machine event
typedef enum
{
    EVENT_RECEIVED_COMMAND,             ///< Command is received from the frame layer.
    EVENT_RECEIVED_RESPONSE,            ///< Response is received from the frame layer.
    EVENT_RECEIVED_SEND_FRAME_STATUS,   ///< The status of the send frame operation given by the frame layer.
    EVENT_RESPONSE_TIMEOUT,             ///< Timeout from receiving the response of the command sent
    EVENT_COMMAND_SENT                  ///< Command is sent

} ssn_evt_t;

///
/// Z-wave HCI session layer state-machine's states
typedef enum
{
    SESSION_STATE_IDLE,          ///< Waiting for event.
    SESSION_STATE_COMMAND_SENT,  ///< Command is sent.
    SESSION_STATE_WAIT_RESPONSE  ///< Waiting for response

} session_state_t;

///
/// Z-wave Serial API command response status
typedef enum
{
    SESSION_RESPONSE_UNKNOWN,      ///< Unknown
    SESSION_RESPONSE_OK,           ///< Send command succeeded with response received.
    SESSION_RESPONSE_MEM,          ///< Failed to allocate memory for the response received.
    SESSION_RESPONSE_TIMEOUT       ///< Response timeout
} ssn_cmd_resp_sts_t;

///
/// Session command / response
typedef struct
{
    dat_frm_typ_t     type;     ///< The type to indicate command / response
    uint8_t     cmd_id;         ///< The command identifier
    uint8_t     len;            ///< The length of the data in the dat_buf
    uint8_t     dat_buf[1];     ///< The buffer to store the parameters of the command / response. Reserved 1 byte for function id.
} ssn_cmd_resp_t;

//Forward declaration of session layer context
struct _ssn_layer_ctx;

///
/// Command callback function
typedef void    (*cmd_cb_t)(struct _ssn_layer_ctx *ssn_ctx, ssn_cmd_resp_t *command, void **param);

///
/// network management callback function
typedef void    (*nm_cb_t)(struct _ssn_layer_ctx *ssn_ctx, uint8_t cmd_id);

///
/// Store the command id and the corresponding callback function pointer
typedef struct
{
    uint8_t   cmd_id;           ///< The command identifier
    cmd_cb_t  cmd_cb_func;      ///< The pointer to command callback
    void      *cmd_cb_prm[2];   ///< The parameters of cmd_cb_func
} ssn_cmd_cb_ref_t;


///
/// Command attribute
typedef struct
{
    cmd_cb_t  cmd_cb_func;      ///< The pointer to command callback
    void      *cmd_cb_prm[2];   ///< The parameters of cmd_cb_func
    uint8_t   cmd_id;           ///< command identifier
    uint8_t   cmd_flag;         ///< command flag
    uint8_t   func_id;          ///< function id used for command that expects callback
} ssn_cmd_attr_t;


///
/// Send command parameters
typedef struct
{
    cmd_cb_t    cmd_cb_func;    ///< The pointer to command callback
    void        *cmd_cb_prm[2]; ///< The parameters of cmd_cb_func
    uint8_t     cmd_id;         ///< The command identifier
    uint8_t     cmd_flag;       ///< The command flag
    uint8_t     dat_sz;         ///< The length of the data in the dat_buf
    uint8_t     dat_buf[1];     ///< The buffer to store the parameters of the command / response. Reserved 1 byte for function id.
} ssn_cmd_snd_param_t;

///
/// Callback request
typedef struct  _ssn_cb_req
{
    ssn_cmd_resp_t      *cmd;               ///< The command info passed to command callback function
    cmd_cb_t            cmd_cb_func;        ///< The pointer to command callback
    void                *cmd_cb_prm[2];     ///< The parameters of cmd_cb_func

} ssn_cb_req_t;


//Forward declaration of application layer context
struct _appl_layer_ctx;

///
/// Z-wave Serial API session layer context
typedef struct _ssn_layer_ctx
{
    //Callback functions to application layer
    cmd_cb_t  unsolicited_cmd_cb;   ///<Pointer to unsolicited command receiving callback function.

    //Data
    ssn_cmd_cb_ref_t  cb_map[SESSION_FUNCID_MAX + 1];  ///< To facilitate the retrieval of callback function based on funcID
    session_state_t     ssn_sta;    ///< The session layer state machine state
    void      *sta_mach_mtx;        ///< Mutex for state machine
    void      *snd_frm_sts_sem;     ///< Semaphore for waiting of send frame status
    void      *cmd_resp_sts_sem;    ///< Semaphore for waiting of command response status
    void      *resp_tmr_ctx;        ///< Response waiting timer context
    void      *cb_thrd_mtx;         ///< Mutex for callback thread
    void      *cb_thrd_sem;         ///< Semaphore for waiting of callback requests
    nm_cb_t   nm_cb;                ///< Callback for network management and send data completion function
    ssn_cmd_resp_t *cmd_resp;       ///< store the response to a command
    util_lst_t   *cb_req_hd;        ///< head of linked list for callback requests
    uint8_t   last_cmd_id;          ///< Last sent command identifier
    uint8_t   last_cmd_flag;        ///< Last sent command flag
    uint8_t   last_gen_func_id;     ///< Last generated function id
    uint32_t  resp_tmout_ms;        ///< Response waiting timeout value in milliseconds
    volatile frm_snd_sts_t          snd_frm_sts;    ///< Send frame status from the frame layer
    volatile ssn_cmd_resp_sts_t     cmd_resp_sts;   ///< command response status
    volatile uint32_t               cb_thrd_run;    ///< Control the callback thread whether to run. 1 = run, 0 = stop
    volatile uint32_t               cb_thrd_sts;    ///< Callback thread status. 1 = run, 0 = thread exited
    frm_layer_ctx_t                 frm_ctx;        ///< Frame layer context
    plt_ctx_t                       *plt_ctx;       ///< Platform context
    struct _appl_layer_ctx          *appl_layer_ctx;///< Pointer to application layer context
} ssn_layer_ctx_t;

int32_t ssn_cmd_snd(ssn_layer_ctx_t   *ssn_ctx, ssn_cmd_snd_param_t   *param,
                    ssn_cmd_resp_t **cmd_resp);
int32_t ssn_init(ssn_layer_ctx_t   *ssn_ctx);
void ssn_exit(ssn_layer_ctx_t   *ssn_ctx);


/**
@}
*/

#endif /* _ZW_HCI_SESSION_DAVID_ */
