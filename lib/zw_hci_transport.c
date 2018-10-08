/**
@file   zw_hci_transport.c - Z-wave host controller interface transport layer implementation.

        Platform specific implementation of serial port access.

@author David Chow

@version    1.0 7-5-10  Initial release

version: 1.0
comments: Initial release
*/

#ifdef OS_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#endif
#include "../include/zw_hci_transport.h"

/**
@defgroup Transport Transport layer APIs
Platform specific implementation of serial port access.
@{
*/

//-----------------------------------------------------------------------------------------------------
//  Windows platform
//-----------------------------------------------------------------------------------------------------

#ifdef WIN32

/**
tpt_wr_req_create - create and queue a write request to the write thread.
@param[in,out]	tpt_ctx		Context
@param[in]      buf         Buffer that store the data to be sent
@param[in]      dat_sz      Size of data to be sent
@return                     Return 0 on success, negative error number on failure.
*/
int32_t tpt_wr_req_create(tpt_layer_ctx_t *tpt_ctx, uint8_t   *buf, uint8_t dat_sz)
{
    int32_t     ret_val;    //Return value

    ret_val = util_list_add(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd, buf, dat_sz);
    if (ret_val)
    {
        return ret_val;
    }


    //
    // notify writer thread that a node has been added
    //
    if (!SetEvent(tpt_ctx->wr_evt_hdl))
        return TRANSPORT_ERROR_SIGNALING;

    return ZWHCI_NO_ERROR;
}

/**
tpt_wr - write all the write requests to the serial comm port.
@param[in,out]	tpt_ctx		Context
@return                     Zero if there is no more write
                            request to process. Nonzero if there
                            might be more write requests to process.

*/
static int32_t tpt_wr(tpt_layer_ctx_t *tpt_ctx)
{
    util_lst_t   *wr_req;
    OVERLAPPED wr_overlapped = {0};
    HANDLE handles[2];
    DWORD bytes_written;
    DWORD result;

    wr_req = util_list_get(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd);

    if (!wr_req)
        return 0;

    //Add delay
    //Sleep(80);

    //
    // create writes overlapped structure hEvent
    //
    wr_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (wr_overlapped.hEvent == NULL)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "CreateEvent(overlapped write request event) failed");
        free(wr_req);
        return 0;
    }

    handles[0] = wr_overlapped.hEvent;
    handles[1] = tpt_ctx->thrd_exit_evt_hdl;

    //
    // issue write
    //
    if (!WriteFile(tpt_ctx->comm_port_hdl, wr_req->wr_buf, wr_req->dat_sz, &bytes_written, &wr_overlapped))
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            //
            // write is delayed, wait for completion
            //
            result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);//Don't wait all objects in the handles array is signaled.
            switch (result)
            {
                //
                // write event set
                //
                case WAIT_OBJECT_0:
                    SetLastError(ERROR_SUCCESS);
                    if (!GetOverlappedResult(tpt_ctx->comm_port_hdl, &wr_overlapped, &bytes_written, FALSE))//Don't wait
                    {
                        if (GetLastError() == ERROR_OPERATION_ABORTED)
                        {
                            debug_msg_show(tpt_ctx->plt_ctx, "Write comm port aborted");
                        }
                        else
                        {
                            debug_msg_show(tpt_ctx->plt_ctx, "GetOverlappedResult(in Writer) failed");
                        }
                    }

                    if (bytes_written != wr_req->dat_sz)
                    {
                        if (GetLastError() == ERROR_SUCCESS)
                        {
                            debug_msg_show(tpt_ctx->plt_ctx, "Write timed out. (overlapped)");
                        }
                        else
                        {
                            debug_msg_show(tpt_ctx->plt_ctx, "Write to comm port failed");
                        }
                    }
                    break;

                    //
                    // thread exit event set
                    //
                case WAIT_OBJECT_0 + 1:
                    break;

                    //
                    // wait timed out
                    //
                    /*case WAIT_TIMEOUT:
                                UpdateStatus("Wait Timeout in WriterGeneric.\r\n");
                                break;*/

                case WAIT_FAILED:
                default:
                    debug_msg_show(tpt_ctx->plt_ctx, "WaitForMultipleObjects (WriterGeneric) failed");
                    break;
            }
        }
        else
        {
            //
            // write failed outright.
            //
            debug_msg_show(tpt_ctx->plt_ctx, "Write comm port failed outright");
        }
    }
    else
    {
        //
        // write returned immediately
        //
        if (bytes_written != wr_req->dat_sz)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "Write comm port timed out. (immediate)");
        }
    }
    CloseHandle(wr_overlapped.hEvent);
    free(wr_req);

    return 1;
}


/**
tpt_wr_thrd - thread for writing to serial comm port.
@param[in]	thrd_param		Context
@return
*/
DWORD WINAPI tpt_wr_thrd(LPVOID thrd_param)
{
    tpt_layer_ctx_t *tpt_ctx = (tpt_layer_ctx_t *)thrd_param;
    HANDLE handles[2];      //The handles for the purpose of polling
    DWORD result;
    BOOL  is_done;          //The flag to determine when to exit this thread

    is_done = FALSE;



    handles[0] = tpt_ctx->wr_evt_hdl;
    handles[1] = tpt_ctx->thrd_exit_evt_hdl;

    while (!is_done)
    {
        result = WaitForMultipleObjects(2, handles, FALSE, COMM_WRITE_CHECK_TIMEOUT);
        switch (result)
        {
            case WAIT_TIMEOUT:
                break;

            case WAIT_FAILED:
                {
                    debug_msg_show(tpt_ctx->plt_ctx, "Write thread wait failed");
                }
                break;

                //
                // write request event
                //
            case WAIT_OBJECT_0:
                while (tpt_wr(tpt_ctx))
                {
                    ;
                }
                break;
                //
                // thread exit event
                //
            case WAIT_OBJECT_0 + 1:
                is_done = TRUE;
                break;
        }
    }

    CloseHandle(tpt_ctx->wr_evt_hdl);

    return 1;
}

/**
tpt_rd_thrd - thread to read data/status from serial
comm port.
@param[in]	thrd_param		Context
@return
*/
DWORD WINAPI tpt_rd_thrd(LPVOID thrd_param)
{
    tpt_layer_ctx_t *tpt_ctx = (tpt_layer_ctx_t *)thrd_param;
    OVERLAPPED reader_overlapped = {0}; // overlapped structure for read operations
    HANDLE     handles[2];              // The handles for the purpose of polling
    DWORD      bytes_rd;                // bytes actually read
    DWORD      last_err;                // last error
    DWORD      result;
    BOOL       rd_pending;              // Flag to determine whether read operation pending.  TRUE = pending, FALSE = read completed
    BOOL       thrd_done;               // The flag to determine when to exit this thread
    char       rd_buf[8];               // Buffer for read data

    rd_pending = FALSE;
    thrd_done = FALSE;

    //
    // create two overlapped structures, one for read events
    // and another for status events
    //
    reader_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (reader_overlapped.hEvent == NULL)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "CreateEvent (Reader Event)");
    }
    //
    // We want to detect the following events:
    //   Read events (from ReadFile)
    //   Thread exit events (from our shutdown functions)
    //
    handles[0] = reader_overlapped.hEvent;
    handles[1] = tpt_ctx->thrd_exit_evt_hdl;

    //
    // Clear comm error, if any
    //
    if (!ClearCommError(tpt_ctx->comm_port_hdl, &result, NULL))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "ClearCommError");
    }

    while (!thrd_done)
    {

        //
        // if no read is outstanding, then issue another one
        //
        if (!rd_pending)
        {
            if (!ReadFile(tpt_ctx->comm_port_hdl, rd_buf, 1, NULL, &reader_overlapped))//read 1 byte
            {
                last_err = GetLastError();
                if (last_err == ERROR_IO_PENDING)
                {
                    //Not a failure; it designates the read operation is pending completion asynchronously
                    rd_pending = TRUE;
                }
                else if (last_err == ERROR_OPERATION_ABORTED)
                {   //I/O operations were canceled completely
                    thrd_done = TRUE;
                }
                else
                {
                    debug_msg_show(tpt_ctx->plt_ctx, "ReadFile with error:%u", (unsigned)last_err);
                }

            }
            else    // read completed immediately, callback frame layer
            {
                tpt_ctx->tpt_rd_cb(tpt_ctx, (uint8_t *)rd_buf, 1);
            }
        }

        //
        // wait for pending operations to complete
        //
        if (rd_pending)
        {
            result = WaitForMultipleObjects(2, handles, FALSE, tpt_ctx->tpt_rd_tmout);
            switch (result)
            {
                //
                // read completed
                //
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(tpt_ctx->comm_port_hdl, &reader_overlapped, &bytes_rd, FALSE))
                    {
                        last_err = GetLastError();
                        if (last_err == ERROR_OPERATION_ABORTED)
                        {   //I/O operations were canceled completely
                            thrd_done = TRUE;
                        }
                        else
                        {
                            debug_msg_show(tpt_ctx->plt_ctx, "GetOverlappedResult (in Reader): %u", (unsigned)last_err);
                        }
                    }
                    else      // read completed successfully, callback frame layer
                    {
                        if (bytes_rd)
                            tpt_ctx->tpt_rd_cb(tpt_ctx, (uint8_t *)rd_buf, (uint8_t)bytes_rd);
                    }

                    rd_pending = FALSE;
                    break;

                    //
                    // thread exit event
                    //
                case WAIT_OBJECT_0 + 1:
                    thrd_done = TRUE;
                    break;

                case WAIT_TIMEOUT:
                    //Timeout, callback frame layer
                    tpt_ctx->tpt_rd_tmout_cb(tpt_ctx);
                    break;

                default:
                    debug_msg_show(tpt_ctx->plt_ctx, "WaitForMultipleObjects(Reader & Status handles)");
                    break;
            }
        }
    }

    //
    // close event handles
    //
    CloseHandle(reader_overlapped.hEvent);

    return 1;
}

/**
tpt_thrd_start - Creates the Reader/Status and Writer threads.
@param[in,out]	tpt_ctx		Context
@return
*/
static void tpt_thrd_start(tpt_layer_ctx_t *tpt_ctx)
{
    DWORD thread_id;

    //
    // create synchronization events for write requests
    //
    tpt_ctx->wr_evt_hdl = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (tpt_ctx->wr_evt_hdl == NULL)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "CreateEvent(writ request event) failed");
        return;
    }

    //Start read thread
    tpt_ctx->rd_thrd_hdl =
    CreateThread( NULL,
                  0,
                  (LPTHREAD_START_ROUTINE) tpt_rd_thrd,
                  (LPVOID) tpt_ctx,
                  0,
                  &thread_id);

    if (tpt_ctx->rd_thrd_hdl == NULL)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Create read thread failed");
        return;
    }

    //Start write thread
    tpt_ctx->wr_thrd_hdl =
    CreateThread( NULL,
                  0,
                  (LPTHREAD_START_ROUTINE) tpt_wr_thrd,
                  (LPVOID) tpt_ctx,
                  0,
                  &thread_id );

    if (tpt_ctx->wr_thrd_hdl == NULL)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Create write thread failed");
        return;
    }

    return;
}

/**
tpt_thrd_stop - Stop the Reader/Status and Writer threads.
@param[in]	tpt_ctx		Context
@return
*/
static void tpt_thrd_stop(tpt_layer_ctx_t *tpt_ctx)
{
    HANDLE threads[2];
    DWORD  result;
    DWORD  last_err;                // last error

    threads[0] = tpt_ctx->rd_thrd_hdl;
    threads[1] = tpt_ctx->wr_thrd_hdl;

    // cancel read/write operation
    if (CancelIoEx(tpt_ctx->comm_port_hdl, NULL) == 0)
    {
        last_err = GetLastError();
        if (last_err != ERROR_NOT_FOUND)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "Cancel read operation failed, %u", (unsigned)last_err);
        }
    }

    //
    // set thread exit event here
    //
    SetEvent(tpt_ctx->thrd_exit_evt_hdl);

    result = WaitForMultipleObjects(2, threads, TRUE, TRANSPORT_THREADS_EXIT_TIMEOUT);
    switch (result)
    {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0 + 1:
            break;

        case WAIT_TIMEOUT:
            {
                debug_msg_show(tpt_ctx->plt_ctx, "Wait transport threads to exit timeout");
            }
            break;

        default:
            debug_msg_show(tpt_ctx->plt_ctx, "Wait transport threads to exit failed");
            break;
    }

    //
    // reset thread exit event here
    //
    ResetEvent(tpt_ctx->thrd_exit_evt_hdl);
}

/**
tpt_port_setup - Setup comm port parameters.
@param[in,out]	tpt_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static int32_t tpt_port_setup(tpt_layer_ctx_t *tpt_ctx)
{
    DCB             dcb;                    //Device class structure
    COMMTIMEOUTS    new_comm_port_tmout;    //Store the new comm port timeout parameters

    memset(&dcb, 0, sizeof(dcb));

    dcb.DCBlength = sizeof(dcb);

    //
    // open communication port handle
    //
    tpt_ctx->comm_port_hdl = CreateFile(    tpt_ctx->comm_port_name,
                                            GENERIC_READ | GENERIC_WRITE,
                                            0,
                                            0,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                            0);

    if (tpt_ctx->comm_port_hdl == INVALID_HANDLE_VALUE)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Open comm port failed");
/*
        MessageBox(
                  NULL,
                  L"Open comm port failed",
                  L"Info",
                  MB_ICONEXCLAMATION | MB_OK
                  );
*/
        return 0;
    }

    //
    // Save original comm timeouts and set new ones
    //
    if (!GetCommTimeouts( tpt_ctx->comm_port_hdl, &tpt_ctx->comm_port_tmout))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Get comm timeouts failed");
        return 0;
    }

    //
    // Set port state
    // By default, BuildCommDCB disables XON/XOFF and hardware flow control. To enable flow control,
    // you must explicitly set the appropriate members of the DCB structure.
    if (!BuildCommDCB(_T("baud=115200 parity=N data=8 stop=1"), &dcb))
    {
        // Couldn't build the DCB. Usually a problem
        // with the communications specification string.
        debug_msg_show(tpt_ctx->plt_ctx, "Build comm dcb failed");
        return 0;
    }
    //
    // set new state
    //
    if (!SetCommState(tpt_ctx->comm_port_hdl, &dcb))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Set comm state failed");
        return 0;
    }

    //
    // set new timeouts
    //
    /*
    If an application sets ReadIntervalTimeout and ReadTotalTimeoutMultiplier to MAXDWORD and sets
    ReadTotalTimeoutConstant to a value greater than zero and less than MAXDWORD,
    one of the following occurs when the ReadFile function is called:

    * If there are any bytes in the input buffer, ReadFile returns immediately with the bytes in the buffer.
    * If there are no bytes in the input buffer, ReadFile waits until a byte arrives and then returns immediately.
    * If no bytes arrive within the time specified by ReadTotalTimeoutConstant, ReadFile times out.
    */
    new_comm_port_tmout.ReadIntervalTimeout = MAXDWORD;
    new_comm_port_tmout.ReadTotalTimeoutMultiplier = MAXDWORD;
    new_comm_port_tmout.ReadTotalTimeoutConstant = tpt_ctx->tpt_rd_tmout;
    //A value of zero for both the WriteTotalTimeoutMultiplier  and WriteTotalTimeoutConstant members
    //indicates that total time-outs are not used for write operations.
    new_comm_port_tmout.WriteTotalTimeoutMultiplier = 0;
    new_comm_port_tmout.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(tpt_ctx->comm_port_hdl, &new_comm_port_tmout))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Set comm timeouts failed");
        return 0;
    }


    //
    // set comm buffer sizes
    //
    SetupComm(tpt_ctx->comm_port_hdl, COMM_MAX_READ_BUFFER, COMM_MAX_WRITE_BUFFER);

    //
    // raise DTR
    //
    if (!EscapeCommFunction(tpt_ctx->comm_port_hdl, SETDTR))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Raise DTR failed");
        return 0;
    }

    //
    // start threads and set initial thread state to not done
    //
    tpt_thrd_start(tpt_ctx);

    return 1;
}

/**
tpt_comm_port_shutdown - Shut down comm port.
@param[in,out]	tpt_ctx		Context
@return
*/
static void tpt_comm_port_shutdown(tpt_layer_ctx_t *tpt_ctx)
{
    //Stop all the threads
    tpt_thrd_stop(tpt_ctx);

    //
    // lower DTR
    //
    if (!EscapeCommFunction(tpt_ctx->comm_port_hdl, CLRDTR))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Lower DTR failed");
        return;
    }

    //
    // restore original comm timeouts
    //
    if (!SetCommTimeouts(tpt_ctx->comm_port_hdl,  &tpt_ctx->comm_port_tmout))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "SetCommTimeouts (Restoration to original)failed");
        return;
    }

    //
    // Purge reads/writes, input buffer and output buffer
    //
    if (!PurgeComm(tpt_ctx->comm_port_hdl, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR ))
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Purge comm port failed");
        return;
    }

    CloseHandle(tpt_ctx->comm_port_hdl);
}


/**
tpt_init - Init the transport layer.
Should be called once before calling the other transport layer functions
@param[in,out]	tpt_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the tpt_ctx after this call
*/
int32_t tpt_init(tpt_layer_ctx_t *tpt_ctx)
{
    tpt_ctx->wr_req_hd = NULL;
    if (tpt_ctx->tpt_rd_tmout < TRANSPORT_READ_TIMEOUT_MIN)
        tpt_ctx->tpt_rd_tmout = TRANSPORT_READ_TIMEOUT_MIN;

    if (!tpt_ctx->comm_port_id)
        return INIT_ERROR_TRANSPORT;

    tpt_ctx->comm_port_name = (TCHAR *)tpt_ctx->comm_port_id;

    tpt_ctx->thrd_exit_evt_hdl = CreateEvent(NULL, TRUE, FALSE, NULL);//creates a manual-reset event object
    if (tpt_ctx->thrd_exit_evt_hdl == NULL)
        return INIT_ERROR_TRANSPORT;

    if (!plt_mtx_init(&tpt_ctx->wr_req_mtx))
        goto l_TRANSPORT_INIT_ERROR;


    if (!tpt_port_setup(tpt_ctx))
        goto l_TRANSPORT_INIT_ERROR1;

    return 0;

l_TRANSPORT_INIT_ERROR1:
    plt_mtx_destroy(tpt_ctx->wr_req_mtx);
l_TRANSPORT_INIT_ERROR:
    CloseHandle(tpt_ctx->thrd_exit_evt_hdl);

    return INIT_ERROR_TRANSPORT;

}

/**
tpt_exit - Clean up the transport layer
@param[in,out]	tpt_ctx		Context
@return
*/
void tpt_exit(tpt_layer_ctx_t *tpt_ctx)
{
    tpt_comm_port_shutdown(tpt_ctx);
    util_list_flush(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd);
    plt_mtx_destroy(tpt_ctx->wr_req_mtx);
    CloseHandle(tpt_ctx->thrd_exit_evt_hdl);
}
#endif

//-----------------------------------------------------------------------------------------------------
//  Linux platform
//-----------------------------------------------------------------------------------------------------
#ifdef OS_LINUX

/**
tpt_wr_req_create - create and queue a write request to the write thread.
@param[in,out]	tpt_ctx		Context
@param[in]      buf         Buffer that store the data to be sent
@param[in]      dat_sz      Size of data to be sent
@return                     Return 0 on success, negative error number on failure.
*/
int32_t tpt_wr_req_create(tpt_layer_ctx_t *tpt_ctx, uint8_t   *buf, uint8_t dat_sz)
{
    int32_t     ret_val;    //Return value

    ret_val = util_list_add(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd, buf, dat_sz);
    if (ret_val)
    {
        return ret_val;
    }

    // Update write started flag
    tpt_ctx->wr_started = 1;

    // Notify writer thread that a node has been added
    plt_sem_post(tpt_ctx->wr_q_sem);

    return ZWHCI_NO_ERROR;
}



/**
tpt_wr_thrd - thread for writing to serial comm port.
@param[in]	data		Context
@return
*/
static void tpt_wr_thrd(void   *data)
{
    tpt_layer_ctx_t *tpt_ctx = (tpt_layer_ctx_t *)data;
    util_lst_t      *wr_req;
    ssize_t         bytes_written;
    uint32_t        total_written;

    tpt_ctx->wr_thrd_sts = 1;

    while (1)
    {
        //Wait for a request
        plt_sem_wait(tpt_ctx->wr_q_sem);

        //Check whether to exit the thread
        if (tpt_ctx->wr_thrd_run == 0)
        {
            tpt_ctx->wr_thrd_sts = 0;
            return;
        }

        wr_req = util_list_get(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd);

        if (wr_req)
        {
            total_written = 0;

            while (total_written < wr_req->dat_sz)
            {
                bytes_written = write(tpt_ctx->comm_port_fd, wr_req->wr_buf + total_written,
                                      wr_req->dat_sz - total_written);
                int j=0;
                /*printf("write data==> ");
                for(j=0;j<wr_req->dat_sz;j++){
                    printf("%02x ",wr_req->wr_buf[j]);
                }
                printf("\n");*/

                if (bytes_written < 0)
                {
                    debug_msg_show(tpt_ctx->plt_ctx, "Write comm port error");
                    break;
                }

                total_written += bytes_written;

            }

            free(wr_req);
        }
    }
}


/**
tpt_rd_thrd - thread to read data/status from serial
comm port.
@param[in]	data		Context
@return
*/
static void tpt_rd_thrd(void   *data)
{
    tpt_layer_ctx_t *tpt_ctx = (tpt_layer_ctx_t *)data;
    int             loop_cnt;
    int             fd_max;
    int             result;
    int             filedes;
    fd_set          read_set;
    fd_set          active_rd_set;
    struct timeval  timeout;
    unsigned        timeout_sec;
    unsigned        timeout_usec;
    uint8_t         rd_char;

    tpt_ctx->rd_thrd_sts = 1;

    //Delay 1 second or until write started, before reading data to avoid race condition where
    //upper layer read callback functions are not ready
    loop_cnt = 10;
    while (loop_cnt-- > 0)
    {
        if (tpt_ctx->wr_started)
        {
            break;
        }
        plt_sleep(100);
    }

    /* Initialize the file descriptor set. */
    filedes = tpt_ctx->comm_port_fd;

    FD_ZERO (&read_set);
    FD_SET (filedes, &read_set);
    fd_max = filedes;

    // Calculate timeout
    timeout_sec = tpt_ctx->tpt_rd_tmout / 1000;

    timeout_usec = (tpt_ctx->tpt_rd_tmout % 1000) * 1000;

    while (tpt_ctx->rd_thrd_run)
    {
        /* Initialize the timeout data structure. */
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = timeout_usec;

        /* select returns 0 if timeout,
        total number of ready file descriptors in all of the sets if input available,
         -1 if error. */
        active_rd_set = read_set;
        result = (select(fd_max + 1, &active_rd_set, NULL, NULL, &timeout));

        if (result > 0)
        {
            if (FD_ISSET(filedes, &active_rd_set))
            {
                if(read(filedes, &rd_char, 1) == 1)
                {// read completed immediately, callback frame layer
                    //printf("read data ==> %02x ",rd_char);
                    tpt_ctx->tpt_rd_cb(tpt_ctx, &rd_char, 1);
                }
            }

        }
        else if(result == 0)
        {
            //debug_msg_show(tpt_ctx->plt_ctx, "Timeout reading");
        }
        else
        {
            debug_msg_show(tpt_ctx->plt_ctx, "Read comm port select error");
        }
    }

    tpt_ctx->rd_thrd_sts = 0;

}



/**
tpt_thrd_start - Creates the Reader/Status and Writer threads.
@param[in,out]	tpt_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static int tpt_thrd_start(tpt_layer_ctx_t *tpt_ctx)
{
    //Start read thread
    tpt_ctx->rd_thrd_run = 1;
    if (plt_thrd_create(tpt_rd_thrd, tpt_ctx) < 0)
    {
        debug_msg_show(tpt_ctx->plt_ctx, "Init transport read thread failed");
        return 0;
    }

    //Start write thread
    tpt_ctx->wr_thrd_run = 1;
    if (plt_thrd_create(tpt_wr_thrd, tpt_ctx) < 0)
    {
        int  wait_count;

        debug_msg_show(tpt_ctx->plt_ctx, "Init transport write thread failed");

        //Stop the read thread
        tpt_ctx->rd_thrd_run = 0;
        wait_count = 50;
        while (wait_count-- > 0)
        {
            if (tpt_ctx->rd_thrd_sts == 0)
                break;
            plt_sleep(100);
        }

        return 0;
    }

    return 1;
}

/**
tpt_thrd_stop - Stop the Reader and Writer threads.
@param[in]	tpt_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static void tpt_thrd_stop(tpt_layer_ctx_t *tpt_ctx)
{
    int  wait_count;

    //Stop the read thread
    tpt_ctx->rd_thrd_run = 0;
    //Stop the write thread
    tpt_ctx->wr_thrd_run = 0;

    plt_sem_post(tpt_ctx->wr_q_sem);

    //Wait for threads to exit
    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (tpt_ctx->rd_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (tpt_ctx->wr_thrd_sts == 0)
            break;
        plt_sleep(100);
    }
    //Extra time for the thread to fully exit
    //Needed to avoid program crash if calling init and exit in a very short interval
    plt_sleep(100);
}

/**
tpt_port_setup - Setup comm port parameters.
@param[in,out]	tpt_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static int tpt_port_setup(tpt_layer_ctx_t *tpt_ctx)
{
	tpt_ctx->comm_port_fd = open(tpt_ctx->comm_port_name, O_RDWR);

	if (tpt_ctx->comm_port_fd < 0)
	{
		debug_msg_show(tpt_ctx->plt_ctx, "Failed to open:%s. Err no:%d", tpt_ctx->comm_port_name, errno);
        if (errno == EACCES)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "Access to the port is not allowed \(need root access) or the port does not exist");
        }
		return 0;
	}

    if(isatty (tpt_ctx->comm_port_fd))
    {
        //char            *term_name;
        struct termios  term_setting;

/*
        term_name = ttyname (tpt_ctx->comm_port_fd);
        if (term_name)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "Terminal name:%s", term_name);
        }
*/

        //Get setting
        if(tcgetattr (tpt_ctx->comm_port_fd, &term_setting) != 0)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "tcgetattr error");
            close(tpt_ctx->comm_port_fd);
            return 0;
        }

        //Set to raw mode
        cfmakeraw (&term_setting);

        //Set speed
        cfsetspeed (&term_setting, B115200);

        //Set input mode
        term_setting.c_cc[VMIN] = 1; //minimum one char
        term_setting.c_cc[VTIME] = 0;//no timeout

        //Set comm port setting
        if(tcsetattr (tpt_ctx->comm_port_fd, TCSAFLUSH, &term_setting) != 0)
        {
            debug_msg_show(tpt_ctx->plt_ctx, "tcsetattr error");
            close(tpt_ctx->comm_port_fd);
            return 0;
        }
    }
    else
    {
        debug_msg_show(tpt_ctx->plt_ctx, "The fd for %s is NOT a serial comm port", tpt_ctx->comm_port_name);
        close(tpt_ctx->comm_port_fd);
        return 0;
    }

    return 1;
}


/**
tpt_init - Init the transport layer.
Should be called once before calling the other transport layer functions
@param[in,out]	tpt_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the tpt_ctx after this call
*/
int32_t tpt_init(tpt_layer_ctx_t *tpt_ctx)
{
    tpt_ctx->wr_req_hd = NULL;
    if (tpt_ctx->tpt_rd_tmout < TRANSPORT_READ_TIMEOUT_MIN)
        tpt_ctx->tpt_rd_tmout = TRANSPORT_READ_TIMEOUT_MIN;

    if (!tpt_ctx->comm_port_id)
        return INIT_ERROR_TRANSPORT;

    tpt_ctx->comm_port_name = (char *)tpt_ctx->comm_port_id;

    if (!plt_mtx_init(&tpt_ctx->wr_req_mtx))
        return INIT_ERROR_TRANSPORT;

    if (!plt_sem_init(&tpt_ctx->wr_q_sem))
    {
        goto l_TRANSPORT_INIT_ERROR;
    }

    // Open and setup comm port
    if (!tpt_port_setup(tpt_ctx))
        goto l_TRANSPORT_INIT_ERROR1;

    // Start threads
    if (!tpt_thrd_start(tpt_ctx))
        goto l_TRANSPORT_INIT_ERROR2;

    return 0;

l_TRANSPORT_INIT_ERROR2:
    close(tpt_ctx->comm_port_fd);

l_TRANSPORT_INIT_ERROR1:
    plt_sem_destroy(tpt_ctx->wr_q_sem);

l_TRANSPORT_INIT_ERROR:
    plt_mtx_destroy(tpt_ctx->wr_req_mtx);

    return INIT_ERROR_TRANSPORT;

}

/**
tpt_exit - Clean up the transport layer
@param[in,out]	tpt_ctx		Context
@return
*/
void tpt_exit(tpt_layer_ctx_t *tpt_ctx)
{
    //Stop all the threads
    tpt_thrd_stop(tpt_ctx);

    close(tpt_ctx->comm_port_fd);
    plt_sem_destroy(tpt_ctx->wr_q_sem);

    util_list_flush(tpt_ctx->wr_req_mtx, &tpt_ctx->wr_req_hd);
    plt_mtx_destroy(tpt_ctx->wr_req_mtx);
}
#endif

/**
@}
*/

