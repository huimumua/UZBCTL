/**
@file   zw_hci_frame.c - Z-wave host controller interface frame layer implementation.

        To parse byte stream from transport layer for creating a frame.
        Also, create a frame for sending to the transport layer.

@author David Chow

@version    1.0 7-5-10  Initial release

version: 1.0
comments: Initial release
*/
#include "../include/zw_hci_frame.h"

/**
@defgroup Frame Frame layer APIs
Assemble frame from the bytes received from transport layer
and handling of frame resend if required.
@{
*/

static int frm_resend(frm_layer_ctx_t   *frm_ctx, uint32_t delay_ms);

/**
frm_frm_typ_get - Get the frame type field of the frame
@param[in] frame    Pointer to the frame object.
@return Frame type
*/
static frm_typ_t frm_frm_typ_get(frm_t  *frame)
{
    return(frm_typ_t)(*(frame->frm_buf + FRAME_FRAME_TYPE_OFFSET));
}

/**
frm_frm_typ_set - Set the frame type field of the frame
@param[in,out] frame        Pointer to the frame object.
@param[in] frame_type       The frame type.
@return
*/
static void frm_frm_typ_set(frm_t  *frame, frm_typ_t  frame_type)
{
    *(frame->frm_buf + FRAME_FRAME_TYPE_OFFSET) = (uint8_t)frame_type;
}

/**
frm_len_get - Get the length of the data frame, excluding the length of SOF and Checksum fields
@param[in] frame        Pointer to the frame object.
@return Length of the frame, excluding the length of SOF and Checksum fields
*/
static uint8_t frm_len_get(frm_t  *frame)
{
    return  *(frame->frm_buf + FRAME_LENGTH_OFFSET);
}

/**
frm_len_set - Set the length field of the data frame
@param[in,out] frame    Pointer to the frame object.
@param[in] length       The length of the frame, excluding the length of SOF and Checksum fields.
@return
*/
static void frm_len_set(frm_t  *frame, uint8_t  length)
{
    *(frame->frm_buf + FRAME_LENGTH_OFFSET) = length;
}

/**
frm_typ_get - Get the type field of the data frame
@param[in] frame Pointer to the frame object.
@return     The type of data frame: Request (REQ) is equal to 0x00 and Response (RES) is equal to 0x01.
*/
static dat_frm_typ_t frm_typ_get(frm_t  *frame)
{
    return(dat_frm_typ_t)(*(frame->frm_buf + FRAME_TYPE_OFFSET));
}

/**
frm_typ_set - Set the type field of the data frame
@param[in,out] frame Pointer to the frame object.
@param[in] type The type of data frame.
@return
*/
static void frm_typ_set(frm_t  *frame, dat_frm_typ_t   type)
{
    *(frame->frm_buf + FRAME_TYPE_OFFSET) = (uint8_t)type;
}

/**
frm_cmd_id_get - Get the Serial API Command ID field of the data frame
@param[in] frame Pointer to the frame object.
@return The Serial API Command ID.
*/
static uint8_t frm_cmd_id_get(frm_t  *frame)
{
    return  *(frame->frm_buf + FRAME_COMMAND_ID_OFFSET);
}

/**
frm_cmd_id_set - Set the Serial API Command ID field of the data frame
@param[in,out] frame    Pointer to the frame object.
@param[in] cmd_id   Unique command ID for the function to be carried out.
@return
*/
static void frm_cmd_id_set(frm_t  *frame, uint8_t   cmd_id)
{
    *(frame->frm_buf + FRAME_COMMAND_ID_OFFSET) = cmd_id;
}


/**
frm_dat_get - Get the command specific data from the data frame
@param[in] frame        Pointer to the frame object.
@param[out] dat_buf     Buffer to store the data (if any)
@return     The number of bytes copied into dat_buf
*/
static uint8_t frm_dat_get(frm_t  *frame, uint8_t  *dat_buf)
{
    uint8_t dat_len;        //the length of the data field
    uint8_t len_incl_hdr;   //the length of the data frame

    len_incl_hdr = frm_len_get(frame);

    if (len_incl_hdr > FRAME_HEADER_LEN)
    {
        dat_len = len_incl_hdr - FRAME_HEADER_LEN;
        //Copy the data into the buffer
        memcpy(dat_buf, frame->frm_buf + FRAME_DATA_OFFSET, dat_len);
        return  dat_len;

    }
    return  0;
}


/**
frm_dat_set - Set the command specific data into the data frame
@param[in,out] frame    Pointer to the frame object.
@param[in] dat_buf      Buffer that stores the data
@param[in] dat_len      Number of bytes to be stored
@return
*/
static void frm_dat_set(frm_t  *frame, uint8_t  *dat_buf, uint8_t dat_len)
{
    if (dat_len > 0)
    {
        memcpy(frame->frm_buf + FRAME_DATA_OFFSET, dat_buf, dat_len);
    }
}


/**
frm_chksm_get - Get the checksum of the data frame.

Checksum calculation includes the length, Type, Serial API Command
Data and Command Specific Data fields. The Checksum is an
XOR checksum with an initial checksum value of 0xFF.
@param[in] frame Pointer to the frame object.
@return Checksum.
*/
static uint8_t frm_chksm_get(frm_t  *frame)
{
    return  *(frame->frm_buf + frm_len_get(frame) + FRAME_TYPE_FIELD_LEN);
}


/**
frm_chksm_set - Set the checksum of the data frame
@param[in,out] frame    Pointer to the frame object.
@param[in] checksum     The value of the checksum.
@return
*/
static void frm_chksm_set(frm_t  *frame, uint8_t   checksum)
{
    *(frame->frm_buf + frm_len_get(frame) + FRAME_TYPE_FIELD_LEN) = checksum;
}

/**
frm_chksm_cal - Calculate the checksum of the data frame.

Checksum calculation includes the length, Type, Serial API Command
Data and Command Specific Data fields. The Checksum is an
XOR checksum with an initial checksum value of 0xFF.
@param[in] frame Pointer to the frame object.
@return The calculated checksum
@pre This function should be called only after the other fields
     are filled.
*/
static uint8_t frm_chksm_cal(frm_t  *frame)
{
    uint8_t     checksum = 0xFF;
    uint32_t    number_of_entry = frm_len_get(frame);
    uint8_t     *entry = frame->frm_buf + FRAME_LENGTH_OFFSET;

    while (number_of_entry-- > 0)
    {
        checksum ^= *entry++;
    }

    return  checksum;
}


/**
frm_snd_tmout_cb - Send frame timeout callback
@param[in] data     Pointer to the frm_layer_ctx_t
@return
*/
static void    frm_snd_tmout_cb(void *data)
{
//  int               result;
    frm_layer_ctx_t   *frm_ctx = (frm_layer_ctx_t   *)data;

    plt_mtx_lck(frm_ctx->wr_mtx);

    //Stop send timer
    plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);
    frm_ctx->snd_tmr_ctx = 0;

    //Resend packet
//  result = frm_resend(frm_ctx, 0);
//
//  plt_mtx_ulck(frm_ctx->wr_mtx);
//
//  if (!result)
//  {
//      //Failed. Update send status to session layer
//      frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_TIMEOUT);
//
//  }

    //Stop resend
    if (frm_ctx->last_frm_buf)
    {
        free(frm_ctx->last_frm_buf);
        frm_ctx->last_frm_buf = NULL;
        frm_ctx->resend_cnt = 0;
    }

    plt_mtx_ulck(frm_ctx->wr_mtx);

    //Call back session layer
    frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_TIMEOUT);

}

/**
frm_single_byte_snd - Send a single byte (e.g. ACK, NAK, CAN)
@param[in] frm_ctx      Context.
@param[in] data         The data to send.
@return  0 on success, negative error number on failure
*/
static int32_t frm_single_byte_snd(frm_layer_ctx_t   *frm_ctx, uint8_t   data)
{
    int32_t     ret_val;

    plt_mtx_lck(frm_ctx->wr_mtx);
    ret_val = tpt_wr_req_create(&frm_ctx->tpt_ctx, &data, 1);
    plt_mtx_ulck(frm_ctx->wr_mtx);
    return ret_val;
}


/**
frm_dat_frm_snd - Send a data frame
@param[in] frm_ctx      Context.
@param[in] type         The type of data frame.
@param[in] cmd_id       Unique command ID for the function to be carried out.
@param[in] buf          Buffer that stores the data
@param[in] dat_sz       Number of bytes stored in the buffer
@return  0 on success, negative error number on failure
*/
int32_t frm_dat_frm_snd(frm_layer_ctx_t   *frm_ctx, dat_frm_typ_t type,
                        uint8_t   cmd_id, uint8_t   *buf, uint8_t   dat_sz)
{
    int32_t     ret_val;
    frm_t       frame;

    plt_mtx_lck(frm_ctx->wr_mtx);

    //Check whether any write operation still pending
    if (frm_ctx->last_frm_buf)
    {
        plt_mtx_ulck(frm_ctx->wr_mtx);
        return FRAME_ERROR_MULTIPLE_WRITE;
    }

    frame.frm_sz = FRAME_HEADER_LEN + FRAME_TYPE_FIELD_LEN
                   + FRAME_CHECKSUM_FIELD_LEN + dat_sz;
    frame.frm_buf = (uint8_t *)malloc(frame.frm_sz);

    if (!frame.frm_buf)
    {
        plt_mtx_ulck(frm_ctx->wr_mtx);
        return ZWHCI_ERROR_MEMORY;
    }

    // Fill in the frame
    frm_frm_typ_set(&frame, SOF);
    frm_len_set(&frame, FRAME_HEADER_LEN + dat_sz);
    frm_typ_set(&frame, type);
    frm_cmd_id_set(&frame, cmd_id);
    frm_dat_set(&frame, buf, dat_sz);
    frm_chksm_set(&frame, frm_chksm_cal(&frame));

    /*printf("\n=========before tpt_wr_req_create,start, len:%d =================\n",frame.frm_sz);
    int y = 0;
    for(y= 0;y<frame.frm_sz;y++)
    {
        printf(" =>%02x<= ",frame.frm_buf[y]);
    }
    printf("\n===================tpt_wr_req_create message end======================\n");*/
    ret_val = tpt_wr_req_create(&frm_ctx->tpt_ctx, frame.frm_buf, frame.frm_sz);

    if (ret_val == ZWHCI_NO_ERROR)
    {
        //Start send timer
        frm_ctx->snd_tmr_ctx = plt_tmr_start(frm_ctx->plt_ctx, frm_ctx->snd_tmout_ms, frm_snd_tmout_cb, frm_ctx);

        if (frm_ctx->snd_tmr_ctx)
        {
            //Save frame for resend
            frm_ctx->last_frm_buf = frame.frm_buf;
            frm_ctx->last_frm_sz = frame.frm_sz;
            frm_ctx->resend_cnt = 0;
        }
        else
        {   //Timer error
            ret_val = FRAME_ERROR_SEND_TIMER;
            free(frame.frm_buf);
        }
    }
    else    //Error, free the buffer
        free(frame.frm_buf);

    plt_mtx_ulck(frm_ctx->wr_mtx);
    return ret_val;

}


/**
frm_resnd_tmr_cb - Frame resend timer callback
@param[in] data     Pointer to the frm_layer_ctx_t
@return
*/
static void    frm_resnd_tmr_cb(void *data)
{
    int32_t             ret_val;
    frm_layer_ctx_t     *frm_ctx = (frm_layer_ctx_t   *)data;

    plt_mtx_lck(frm_ctx->wr_mtx);

    //Stop resend timer
    plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->resnd_tmr_ctx);
    frm_ctx->resnd_tmr_ctx = 0;

    //Resend
    if (frm_ctx->last_frm_buf)
    {
        ret_val = tpt_wr_req_create(&frm_ctx->tpt_ctx, frm_ctx->last_frm_buf, frm_ctx->last_frm_sz);


        if (ret_val == ZWHCI_NO_ERROR)
        {
            //Restart send timer
            plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);

            frm_ctx->snd_tmr_ctx = plt_tmr_start(frm_ctx->plt_ctx, frm_ctx->snd_tmout_ms, frm_snd_tmout_cb, frm_ctx);

            if (frm_ctx->snd_tmr_ctx)
            {
                plt_mtx_ulck(frm_ctx->wr_mtx);
                return;
            }
        }
        //Free send data buffer
        free(frm_ctx->last_frm_buf);
        frm_ctx->last_frm_buf = NULL;
        frm_ctx->resend_cnt = 0;

        //Stop send timer
        plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);
        frm_ctx->snd_tmr_ctx = 0;

        plt_mtx_ulck(frm_ctx->wr_mtx);

        //Failed. Update send status to session layer
        frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_FAIL_DROPPED);

        return;
    }

    plt_mtx_ulck(frm_ctx->wr_mtx);

}


/**
frm_resend - Resend a frame
@param[in] frm_ctx Context.
@param[in] delay_ms Delay to resend frame in ms.
@return  1 on success, else 0 on failure
On success: restart the send timer. On failure: stop the send timer, free the send data buffer

*/
static int frm_resend(frm_layer_ctx_t   *frm_ctx, uint32_t delay_ms)
{
    int ret_val;

    plt_mtx_lck(frm_ctx->wr_mtx);
    //Check the buffer is valid
    if (!frm_ctx->last_frm_buf)
        goto l_RESEND_FRAME_ERROR;

    //Check number of resend has been exceeded
    if (++frm_ctx->resend_cnt > FRAME_MAX_RESEND)
        goto l_RESEND_FRAME_ERROR;

    if (delay_ms > 0)
    {
        //Restart resend timer to wait for the delay before resending frame
        plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->resnd_tmr_ctx);

        frm_ctx->resnd_tmr_ctx = plt_tmr_start(frm_ctx->plt_ctx, delay_ms, frm_resnd_tmr_cb, frm_ctx);

        if (frm_ctx->resnd_tmr_ctx)
        {
            plt_mtx_ulck(frm_ctx->wr_mtx);
            return 1;
        }
    }
    else
    {   //Send immediately
        ret_val = tpt_wr_req_create(&frm_ctx->tpt_ctx, frm_ctx->last_frm_buf, frm_ctx->last_frm_sz);


        if (ret_val == ZWHCI_NO_ERROR)
        {
            //Restart send timer
            plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);

            frm_ctx->snd_tmr_ctx = plt_tmr_start(frm_ctx->plt_ctx, frm_ctx->snd_tmout_ms, frm_snd_tmout_cb, frm_ctx);

            if (frm_ctx->snd_tmr_ctx)
            {
                plt_mtx_ulck(frm_ctx->wr_mtx);
                return 1;
            }
        }
    }

l_RESEND_FRAME_ERROR:
    //Free send data buffer
    if (frm_ctx->last_frm_buf)
        free(frm_ctx->last_frm_buf);
    frm_ctx->last_frm_buf = NULL;
    frm_ctx->resend_cnt = 0;

    //Stop send timer
    plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);
    frm_ctx->snd_tmr_ctx = 0;

    plt_mtx_ulck(frm_ctx->wr_mtx);
    return 0;

}


/**
frm_sta_machine - frame layer state-machine.
Try to assemble a frame and pass it to the session layer
@param[in] frm_ctx      Context.
@param[in] frm_evt      The event for the state-machine
@param[in] data         The received byte
@return
*/
static void    frm_sta_machine(frm_layer_ctx_t   *frm_ctx, frm_evt_t frm_evt, uint8_t data)
{

    switch (frm_ctx->sta_mach.frm_sta)
    {
            //-------------------------------------
        case    FRAME_STATE_IDLE:
            //-------------------------------------
            if (frm_evt == EVENT_RECEIVED_DATA)
            {
                switch (data)
                {
                    case ACK:
                        //The sent request is acknowledged
                        plt_mtx_lck(frm_ctx->wr_mtx);

                        //Free send data buffer
                        if (frm_ctx->last_frm_buf)
                            free(frm_ctx->last_frm_buf);
                        frm_ctx->last_frm_buf = NULL;
                        frm_ctx->resend_cnt = 0;

                        //Stop send timer
                        plt_tmr_stop(frm_ctx->plt_ctx, frm_ctx->snd_tmr_ctx);
                        frm_ctx->snd_tmr_ctx = 0;

                        plt_mtx_ulck(frm_ctx->wr_mtx);

                        //Update send status to session layer
                        frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_OK);
                        break;

                    case NAK:
                        //Checksum error
#ifdef DEBUG_SERIAL_API
                        frm_ctx->nak_cnt++;
#endif
                        if (!frm_resend(frm_ctx, 0))
                        {
                            //Failed. Update send status to session layer
                            frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_FAIL_CHKSUM);

                        }
                        break;

                    case CAN:
#ifdef DEBUG_SERIAL_API
                        frm_ctx->can_cnt++;
#endif
                        //The sent request is dropped by the Z-wave controller
                        if (!frm_resend(frm_ctx, FRAME_RESEND_DELAY))
                        {
                            //Failed. Update send status to session layer
                            frm_ctx->snd_frm_sts_cb(frm_ctx, FRAME_SEND_FAIL_DROPPED);

                        }
                        break;

                    case SOF:
                        //Store the byte
                        frm_ctx->sta_mach.frm_buf[FRAME_FRAME_TYPE_OFFSET] = data;
                        frm_ctx->sta_mach.frm_sta = FRAME_STATE_FOUND_DATA_FRAME;
                        break;

                    default:
                        break;

                }
            }
            break;

            //-------------------------------------
        case FRAME_STATE_FOUND_DATA_FRAME:
            //-------------------------------------
            if (frm_evt == EVENT_RECEIVED_DATA)
            {
                if (data >= FRAME_HEADER_LEN)
                {
                    //Store the byte
                    frm_ctx->sta_mach.frm_buf[FRAME_LENGTH_OFFSET] = data;
                    frm_ctx->sta_mach.frm_sta = FRAME_STATE_FOUND_LENGTH;
                    break;

                }
            }
            //Read timeout or byte received is invalid
            frm_ctx->sta_mach.frm_sta = FRAME_STATE_IDLE;
            break;

            //-------------------------------------
        case FRAME_STATE_FOUND_LENGTH:
            //-------------------------------------
            if (frm_evt == EVENT_RECEIVED_DATA)
            {
                if (data == RES || data == REQ)
                {
                    //Store the byte
                    frm_ctx->sta_mach.frm_buf[FRAME_TYPE_OFFSET] = data;
                    frm_ctx->sta_mach.frm_sta = FRAME_STATE_WAIT_COMPLETE_FRAME;
                    //Calculate the number of bytes to receive for a complete frame
                    frm_ctx->sta_mach.pending_rx_bytes = frm_ctx->sta_mach.frm_buf[FRAME_LENGTH_OFFSET]
                                                         - FRAME_LENGTH_FIELD_LEN
                                                         - FRAME_TYPE_FIELD_LEN
                                                         + FRAME_CHECKSUM_FIELD_LEN;
                    //frm_offset to the buffer to store the next byte
                    frm_ctx->sta_mach.frm_offset = FRAME_COMMAND_ID_OFFSET;
                    break;

                }
            }
            //Read timeout or byte received is invalid
            frm_ctx->sta_mach.frm_sta = FRAME_STATE_IDLE;
            break;

            //-------------------------------------
        case FRAME_STATE_WAIT_COMPLETE_FRAME:
            //-------------------------------------
            if (frm_evt == EVENT_RECEIVED_DATA)
            {
                //Store the byte
                frm_ctx->sta_mach.frm_buf[frm_ctx->sta_mach.frm_offset++] = data;

                if (--frm_ctx->sta_mach.pending_rx_bytes <= 0)
                {   //Receive complete frame, calculate checksum
                    frm_t    frame;

                    frame.frm_buf = frm_ctx->sta_mach.frm_buf;
                    frame.frm_sz = frm_ctx->sta_mach.frm_offset;
                    if (frm_chksm_cal(&frame) == frm_chksm_get(&frame))
                    {
                        //Send ACK to Z-wave controller
                        frm_single_byte_snd(frm_ctx, ACK);

                        //Pass the frame (without frame type and checksum fields) to the session layer
                        frm_ctx->rx_frm_cb(frm_ctx, &frm_ctx->sta_mach.frm_buf[FRAME_LENGTH_OFFSET],
                                           frm_ctx->sta_mach.frm_buf[FRAME_LENGTH_OFFSET]);

                    }
                    else
                    {
                        //Send NAK to Z-wave controller
                        frm_single_byte_snd(frm_ctx, NAK);
                    }
                    //Done. Go back to idle state
                    frm_ctx->sta_mach.frm_sta = FRAME_STATE_IDLE;
                }
            }
            else    //EVENT_RECEIVE_TIMEOUT
            {
                //Read timeout
                frm_ctx->sta_mach.frm_sta = FRAME_STATE_IDLE;
            }
            break;

    }
}



/**
frm_tpt_rd_cb - Read callback from transport layer.
@param[in] tpt_ctx  Transport layer context.
@param[in] buf      Buffer that stores the received bytes
@param[in] dat_len  Number of bytes received
@return
*/
static void    frm_tpt_rd_cb(struct _tpt_layer_ctx *tpt_ctx, uint8_t *buf, uint8_t dat_len)
{
    frm_layer_ctx_t   *frm_ctx = tpt_ctx->frm_layer_ctx;
    plt_mtx_lck(frm_ctx->sta_mach_mtx);
    //Call the frame state-machine to assemble a frame
    while (dat_len-- > 0)
    {
        frm_sta_machine(frm_ctx, EVENT_RECEIVED_DATA, *buf++);
    }
    plt_mtx_ulck(frm_ctx->sta_mach_mtx);
}


/**
frm_tpt_rd_tmout_cb - Read time out callback from transport layer.
@param[in] tpt_ctx      Transport layer context.
@return
*/
static void    frm_tpt_rd_tmout_cb(struct _tpt_layer_ctx *tpt_ctx)
{
    frm_layer_ctx_t   *frm_ctx = tpt_ctx->frm_layer_ctx;
    plt_mtx_lck(frm_ctx->sta_mach_mtx);
    //Call the frame state-machine
    frm_sta_machine(frm_ctx, EVENT_RECEIVE_TIMEOUT, 0);
    plt_mtx_ulck(frm_ctx->sta_mach_mtx);
}


/**
frm_init - Init the frame layer.
Should be called once before calling the other frame layer functions
@param[in,out]	frm_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the context after this call
*/
int32_t frm_init(frm_layer_ctx_t   *frm_ctx)
{
    int32_t ret_val;

    //Init transport layer
    frm_ctx->tpt_ctx.tpt_rd_cb = frm_tpt_rd_cb;
    frm_ctx->tpt_ctx.tpt_rd_tmout_cb = frm_tpt_rd_tmout_cb;
    frm_ctx->tpt_ctx.tpt_rd_tmout = TRANSPORT_READ_TIMEOUT_MIN;
    frm_ctx->tpt_ctx.frm_layer_ctx = frm_ctx;
    frm_ctx->tpt_ctx.plt_ctx = frm_ctx->plt_ctx;

    ret_val = tpt_init(&frm_ctx->tpt_ctx);

    if (ret_val != 0)
        return ret_val;


    //Init frame layer
    frm_ctx->last_frm_buf = NULL;
    frm_ctx->sta_mach.frm_sta = FRAME_STATE_IDLE;
    if (frm_ctx->snd_tmout_ms < FRAME_SEND_TIMEOUT_MIN)
        frm_ctx->snd_tmout_ms = FRAME_SEND_TIMEOUT_MIN;

    if (!plt_mtx_init(&frm_ctx->wr_mtx))
        goto l_FRAME_INIT_ERROR;

    if (!plt_mtx_init(&frm_ctx->sta_mach_mtx))
        goto l_FRAME_INIT_ERROR1;

    return 0;
l_FRAME_INIT_ERROR1:
    plt_mtx_destroy(frm_ctx->wr_mtx);

l_FRAME_INIT_ERROR:
    tpt_exit(&frm_ctx->tpt_ctx);

    return INIT_ERROR_FRAME;

}


/**
frm_exit - Clean up the frame layer
@param[in,out]	frm_ctx		Context
@return
*/
void frm_exit(frm_layer_ctx_t   *frm_ctx)
{
    tpt_exit(&frm_ctx->tpt_ctx);

    plt_mtx_destroy(frm_ctx->sta_mach_mtx);
    plt_mtx_destroy(frm_ctx->wr_mtx);
}

/**
@}
*/


