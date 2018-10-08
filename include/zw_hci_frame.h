/**
@file   zw_hci_frame.h - Z-wave host controller interface frame layer implementation header file.

        To parse byte stream from transport layer for creating a frame.
        Also, create a frame for sending to the transport layer.

@author David Chow

@version    1.0 7-5-10  Initial release

version: 1.0
comments: Initial release
*/
#ifndef _ZW_HCI_FRAME_DAVID_
#define _ZW_HCI_FRAME_DAVID_

//#define DEBUG_SERIAL_API

#include "zw_hci_transport.h"

/**
@defgroup Frame Frame layer APIs
Assemble frame from the bytes received from transport layer
and handling of frame resend if required.
@{
*/

#define FRAME_HEADER_LEN             3   ///< The length of the data frame header fields (currently length, type and command id)
#define FRAME_TYPE_FIELD_LEN         1   ///< The length of the frame type field
#define FRAME_CHECKSUM_FIELD_LEN     1   ///< The length of the checksum field
#define FRAME_LENGTH_FIELD_LEN       1   ///< The length of the length field


#define FRAME_MAX_RESEND             2   ///< The maximum number of frame resend

// Offset of the field in the Serial API frame
#define FRAME_FRAME_TYPE_OFFSET      0
#define FRAME_LENGTH_OFFSET          1
#define FRAME_TYPE_OFFSET            2
#define FRAME_COMMAND_ID_OFFSET      3
#define FRAME_DATA_OFFSET            4

//Minimum send frame time out value
#define     FRAME_SEND_TIMEOUT_MIN      2000    ///< Minimum send frame time out value in milliseconds

//resend frame delay value
#define     FRAME_RESEND_DELAY          1000     ///< Resend frame delay in milliseconds

///
/// Z-wave Serial API frame type
typedef enum
{
    ACK = 0x06,    ///< The ACK frame is used to acknowledge a successful transmission of a data frame
    NAK = 0x15,    ///< The NAK frame is used to de-acknowledge an unsuccessful transmission of a data frame
    CAN = 0x18,    ///< The CAN frame is used by the ZW to instruct the host that a host transmitted data frame has been dropped
    SOF = 0x01     ///< Start Of Frame of Data Frame

} frm_typ_t;


///
/// Z-wave Serial API data frame type
typedef enum
{
    REQ = 0x00,    ///< The frame is a request or callback
    RES = 0x01     ///< The frame is a response to the request

} dat_frm_typ_t;

///
/// Z-wave Serial API frame layer state-machine event
typedef enum
{
    EVENT_RECEIVED_DATA,    ///< Data was received from the serial comm transport layer.
    EVENT_RECEIVE_TIMEOUT  ///< Timeout from receiving the last byte of data
} frm_evt_t;

///
/// Z-wave Serial API frame layer state-machine's states
typedef enum
{
    FRAME_STATE_IDLE,                       ///< Waiting for data byte.
    FRAME_STATE_FOUND_DATA_FRAME,           ///< Frame type = SOF (data frame)
    FRAME_STATE_FOUND_LENGTH,               ///< Length field is valid
    FRAME_STATE_WAIT_COMPLETE_FRAME         ///< Start collecting the less of bytes to build a complete frame

} frame_state_t;

///
/// Z-wave Serial API send frame status
typedef enum
{
    FRAME_SEND_UNKNOWN,      ///< Unknown
    FRAME_SEND_OK,           ///< Send frame succeeded with ACK received.
    FRAME_SEND_TIMEOUT,      ///< Send frame timeout or no response from Z-wave controller
    FRAME_SEND_FAIL_CHKSUM,  ///< Send frame failed even after resend because of checksum error
    FRAME_SEND_FAIL_DROPPED  ///< Send frame failed even after resend because controller dropped frame
} frm_snd_sts_t;

///
/// To hold the Z-wave Serial API frame
typedef struct
{
    uint8_t     *frm_buf;   ///< frame buffer
    uint8_t     frm_sz;     ///< size of frame in the buffer
} frm_t;

///
/// Z-wave HCI frame layer state-machine context
typedef struct
{
    uint8_t         frm_buf[260];           ///< Buffer to store the bytes received
    int32_t         pending_rx_bytes;       ///< The number of bytes yet to receive in order to form a complete frame
    int32_t         frm_offset;             ///< The offset to the beginning of frm_buf where the next byte to be stored
    frame_state_t   frm_sta;                ///< The state-machine's current state
} frm_sta_mach_t;

//Forward declaration of session layer context
struct _ssn_layer_ctx;


///
/// Z-wave HCI frame layer context
typedef struct _frm_layer_ctx
{
    //Callback functions to session layer
    void    (*snd_frm_sts_cb)(struct _frm_layer_ctx *frm_ctx,
                              frm_snd_sts_t status);    ///<Pointer to callback function to indicate send frame status.
    void    (*rx_frm_cb)(struct _frm_layer_ctx *frm_ctx,
                         uint8_t   *buf, uint8_t data_length); ///<Pointer to callback function when a frame is received.
    //Data
    frm_sta_mach_t   sta_mach;  ///< the frame layer state machine context
    void      *wr_mtx;          ///< mutex for writing to transport layer
    void      *sta_mach_mtx;    ///< mutex for state machine
    void      *snd_tmr_ctx;     ///< Send frame timer context
    void      *resnd_tmr_ctx;   ///< Resend frame timer context
    uint8_t   *last_frm_buf;    ///< Buffer to store the last frame
    uint8_t   last_frm_sz;      ///< The size of last frame
    uint8_t   resend_cnt;       ///< Number of resend of the last frame
    uint32_t  snd_tmout_ms;     ///< Send frame timeout value in milliseconds
#ifdef DEBUG_SERIAL_API
    uint32_t  nak_cnt;          ///< Number of NAK received from the controller
    uint32_t  can_cnt;          ///< Number of CAN received from the controller
#endif
    tpt_layer_ctx_t   tpt_ctx;  ///< Transport layer context
    plt_ctx_t                 *plt_ctx;      ///< Platform context
    struct _ssn_layer_ctx     *ssn_layer_ctx;///< Pointer to session layer context

} frm_layer_ctx_t;


int32_t frm_dat_frm_snd(frm_layer_ctx_t   *frm_ctx, dat_frm_typ_t type,
                        uint8_t   cmd_id, uint8_t   *buf, uint8_t   dat_sz);
int32_t frm_init(frm_layer_ctx_t   *frm_ctx);
void frm_exit(frm_layer_ctx_t   *frm_ctx);

/**
@}
*/


#endif /* _ZW_HCI_FRAME_DAVID_ */
