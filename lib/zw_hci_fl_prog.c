/**
@file   zw_hci_fl_prog.c - Flash programming implementation

        Platform specific implementation of flash programming.

@author David Chow

@version    1.0 2-9-13  Initial release

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
#include "../include/zw_hci_fl_prog.h"

/**
@defgroup FlashProg Flash programming APIs
Platform specific implementation of flash programming.
@{
*/

//-----------------------------------------------------------------------------------------------------
//  Windows platform
//-----------------------------------------------------------------------------------------------------

#ifdef WIN32
/**
fl_prog_rd - read 4 bytes of data from the serial comm port
@param[in]	    fl_prog_ctx		Context
@param[out]	    rd_buf		    Buffer that stores the read data
@return     Return zero indicates success, non-zero indicates failure
*/
static int fl_prog_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *rd_buf)
{
    OVERLAPPED reader_overlapped = {0}; // overlapped structure for read operations
    HANDLE     handle;                  // The handle for the purpose of polling
    DWORD      bytes_rd;                // bytes actually read
    DWORD      result;
    DWORD      err_no;
    int        ret = ZWHCI_ERROR_READ;

    reader_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);//manual reset, non-signaled
    if (reader_overlapped.hEvent == NULL)
    {
        return ZWHCI_ERROR_RESOURCE;
    }

    handle = reader_overlapped.hEvent;

    if (!ReadFile(fl_prog_ctx->comm_port_hdl, rd_buf, FLASH_PROG_CMD_LEN, NULL, &reader_overlapped))
    {
        err_no = GetLastError();
        if (err_no != ERROR_IO_PENDING)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "ReadFile error: %u", (unsigned)err_no);
            goto l_FL_PROG_RD_ERROR;
        }
    }
    else    //Read completed immediately
    {
        ret = 0;
//      debug_msg_show(fl_prog_ctx->plt_ctx, "ReadFile returns immediately");
//      debug_msg_show(fl_prog_ctx->plt_ctx, "%02X %02X %02X %02X",
//                     (unsigned)rd_buf[0],  (unsigned)rd_buf[1], (unsigned)rd_buf[2], (unsigned)rd_buf[3]);
        goto l_FL_PROG_RD_ERROR;
    }

    //
    // wait for pending operations to complete
    //
    result = WaitForSingleObject(handle, FLASH_PROG_READ_TIMEOUT);
    switch (result)
    {
        //
        // read completed
        //
        case WAIT_OBJECT_0:
            if (!GetOverlappedResult(fl_prog_ctx->comm_port_hdl, &reader_overlapped, &bytes_rd, FALSE))
            {
                err_no = GetLastError();
                debug_msg_show(fl_prog_ctx->plt_ctx, "GetOverlappedResult (in Reader): %u", (unsigned)err_no);
            }
            else // read completed successfully
            {
                ret = (bytes_rd == FLASH_PROG_CMD_LEN)? 0 : ZWHCI_ERROR_TIMEOUT;
//              debug_msg_show(fl_prog_ctx->plt_ctx, "ReadFile returns %d bytes:%02X %02X %02X %02X", bytes_rd,
//                             (unsigned)rd_buf[0],  (unsigned)rd_buf[1], (unsigned)rd_buf[2], (unsigned)rd_buf[3]);
            }
            break;


        case WAIT_TIMEOUT:
            //Timeout
            debug_msg_show(fl_prog_ctx->plt_ctx, "ReadFile timeout");
            break;

        default:
            debug_msg_show(fl_prog_ctx->plt_ctx, "WaitForSingleObject(Reader)");
            break;
    }

l_FL_PROG_RD_ERROR:
    //
    // close event handle
    //
    CloseHandle(reader_overlapped.hEvent);

    return ret;
}


/**
fl_prog_cmd_wr - write 4 bytes of command to the serial comm port
@param[in]	    fl_prog_ctx		Context
@param[in]	    buf		        Buffer that stores command to write
@return     Return zero indicates success, non-zero indicates failure
*/
static int fl_prog_cmd_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf)
{
    OVERLAPPED  wr_overlapped = {0};
    HANDLE      handle;
    DWORD       bytes_written;
    DWORD       result;
    DWORD       err_no;
    int         ret = ZWHCI_ERROR_WRITE;

    //
    // create writes overlapped structure hEvent
    //
    wr_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (wr_overlapped.hEvent == NULL)
    {
        return ZWHCI_ERROR_RESOURCE;
    }

    handle = wr_overlapped.hEvent;

    if (!WriteFile(fl_prog_ctx->comm_port_hdl, buf, FLASH_PROG_CMD_LEN, NULL, &wr_overlapped))
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            //
            // write is delayed, wait for completion
            //
            result = WaitForSingleObject(handle, INFINITE);
            switch (result)
            {
                //
                // write event set
                //
                case WAIT_OBJECT_0:
                    SetLastError(ERROR_SUCCESS);
                    if (!GetOverlappedResult(fl_prog_ctx->comm_port_hdl, &wr_overlapped, &bytes_written, FALSE))//Don't wait
                    {
                        err_no = GetLastError();
                        debug_msg_show(fl_prog_ctx->plt_ctx, "GetOverlappedResult(in Writer) failed : %u", (unsigned)err_no);
                        goto l_FL_PROG_WR_ERROR;
                    }

                    if (bytes_written == FLASH_PROG_CMD_LEN)
                    {
                        ret = 0;
                    }
                    else
                    {
                        if (GetLastError() == ERROR_SUCCESS)
                        {
                            debug_msg_show(fl_prog_ctx->plt_ctx, "Write timed out. (overlapped)");
                        }
                        else
                        {
                            debug_msg_show(fl_prog_ctx->plt_ctx, "Write to comm port failed");
                        }
                    }
                    break;

                case WAIT_FAILED:
                default:
                    debug_msg_show(fl_prog_ctx->plt_ctx, "WaitForSingleObject (WriterGeneric) failed");
                    break;
            }
        }
        else
        {
            // write failed outright
            debug_msg_show(fl_prog_ctx->plt_ctx, "Write comm port failed outright");
        }
    }
    else
    {
        // write returns immediately
        //debug_msg_show(fl_prog_ctx->plt_ctx, "Write returns immediately");
    }

l_FL_PROG_WR_ERROR:
    CloseHandle(wr_overlapped.hEvent);

    return ret;
}


/**
fl_prog_port_setup - Setup comm port parameters
@param[in]	fl_prog_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static int fl_prog_port_setup(flash_prog_ctx_t *fl_prog_ctx)
{
    DCB             dcb = {0};              //Device class structure
    COMMTIMEOUTS    new_comm_port_tmout;    //Store the new comm port timeout parameters
    DWORD           result;

    dcb.DCBlength = sizeof(dcb);

    //
    // open communication port handle
    //
    fl_prog_ctx->comm_port_hdl = CreateFile(fl_prog_ctx->comm_port_name,
                                            GENERIC_READ | GENERIC_WRITE,
                                            0,
                                            0,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                            0);

    if (fl_prog_ctx->comm_port_hdl == INVALID_HANDLE_VALUE)
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Open flash programming port failed");
        return 0;
    }

    //
    // Save original comm timeouts and set new ones
    //
    if (!GetCommTimeouts(fl_prog_ctx->comm_port_hdl, &fl_prog_ctx->comm_port_tmout))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Get comm timeouts failed");
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
        debug_msg_show(fl_prog_ctx->plt_ctx, "Build comm dcb failed");
        return 0;
    }

    //
    // set new state
    //
    if (!SetCommState(fl_prog_ctx->comm_port_hdl, &dcb))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Set comm state failed");
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
    new_comm_port_tmout.ReadTotalTimeoutConstant = FL_PORT_READ_TIMEOUT;
    //A value of zero for both the WriteTotalTimeoutMultiplier  and WriteTotalTimeoutConstant members
    //indicates that total time-outs are not used for write operations.
    new_comm_port_tmout.WriteTotalTimeoutMultiplier = 0;
    new_comm_port_tmout.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(fl_prog_ctx->comm_port_hdl, &new_comm_port_tmout))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Set comm timeouts failed");
        return 0;
    }


    //
    // set comm buffer sizes
    //
    SetupComm(fl_prog_ctx->comm_port_hdl, 255, 255);

    //
    // Clear comm error, if any
    //
    if (!ClearCommError(fl_prog_ctx->comm_port_hdl, &result, NULL))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "ClearCommError");
    }

    //
    // raise DTR
    //
    if (!EscapeCommFunction(fl_prog_ctx->comm_port_hdl, SETDTR))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Raise DTR failed");
        return 0;
    }

    return 1;
}


/**
fl_prog_init - Initialize flash programming interface
@param[in]	fl_prog_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the fl_prog_ctx after this call
*/
int fl_prog_init(flash_prog_ctx_t *fl_prog_ctx)
{

    if (!fl_prog_ctx->comm_port_id)
        return ZWHCI_ERROR_INVALID_VALUE;

    fl_prog_ctx->comm_port_name = (TCHAR *)fl_prog_ctx->comm_port_id;

    // Open and setup comm port
    if (!fl_prog_port_setup(fl_prog_ctx))
    {
        return ZWHCI_ERROR_RESOURCE;
    }

    return 0;
}


/**
fl_prog_port_shutdown - Shut down comm port
@param[in]	fl_prog_ctx		Context
@return
*/
static void fl_prog_port_shutdown(flash_prog_ctx_t *fl_prog_ctx)
{
    //
    // lower DTR
    //
    if (!EscapeCommFunction(fl_prog_ctx->comm_port_hdl, CLRDTR))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Lower DTR failed");
        return;
    }

    //
    // restore original comm timeouts
    //
    if (!SetCommTimeouts(fl_prog_ctx->comm_port_hdl,  &fl_prog_ctx->comm_port_tmout))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "SetCommTimeouts (Restoration to original)failed");
        return;
    }

    //
    // Purge reads/writes, input buffer and output buffer
    //
    if (!PurgeComm(fl_prog_ctx->comm_port_hdl, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR ))
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Purge comm port failed");
        return;
    }

    CloseHandle(fl_prog_ctx->comm_port_hdl);
}


/**
fl_prog_exit - Clean up the flash programming interface
@param[in]	fl_prog_ctx		Context
@return
*/
void fl_prog_exit(flash_prog_ctx_t *fl_prog_ctx)
{
    fl_prog_port_shutdown(fl_prog_ctx);
}


#endif

//-----------------------------------------------------------------------------------------------------
//  Linux platform
//-----------------------------------------------------------------------------------------------------
#ifdef OS_LINUX

/**
fl_prog_rd - read 4 bytes of data from the serial comm port
@param[in]	    fl_prog_ctx		Context
@param[out]	    rd_buf		    Buffer that stores the read data
@return     Return zero indicates success, non-zero indicates failure
*/
static int fl_prog_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *rd_buf)
{
    int             result;
    int             filedes;
    ssize_t         bytes_rd;   //bytes actually read
    fd_set          read_set;
    struct timeval  timeout;

    /* Initialize the file descriptor set. */
    filedes = fl_prog_ctx->comm_port_fd;

    FD_ZERO (&read_set);
    FD_SET (filedes, &read_set);

    /* Initialize the timeout data structure. */
    timeout.tv_sec = FLASH_PROG_READ_TIMEOUT / 1000;
    timeout.tv_usec = (FLASH_PROG_READ_TIMEOUT % 1000) * 1000;

    /* select returns 0 if timeout,
       total number of ready file descriptors in all of the sets if input available,
       -1 if error. */
    result = (select(filedes + 1, &read_set, NULL, NULL, &timeout));

    if (result > 0)
    {
        if (FD_ISSET(filedes, &read_set))
        {
            bytes_rd = read(filedes, rd_buf, FLASH_PROG_CMD_LEN);
            return (bytes_rd == FLASH_PROG_CMD_LEN)? 0 : ZWHCI_ERROR_TIMEOUT;
        }

    }
    else if(result == 0)
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "Timeout reading");
        return ZWHCI_ERROR_TIMEOUT;
    }

    debug_msg_show(fl_prog_ctx->plt_ctx, "Read comm port select error");
    return ZWHCI_ERROR_READ;
}


/**
fl_prog_cmd_wr - write 4 bytes of command to the serial comm port
@param[in]	    fl_prog_ctx		Context
@param[in]	    buf		        Buffer that stores command to write
@return     Return zero indicates success, non-zero indicates failure
*/
static int fl_prog_cmd_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf)
{
    ssize_t bytes_written;
    ssize_t total_written;

    total_written = 0;

    while (total_written < FLASH_PROG_CMD_LEN)
    {
        bytes_written = write(fl_prog_ctx->comm_port_fd, buf + total_written,
                              FLASH_PROG_CMD_LEN - total_written);

        if (bytes_written < 0)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "Write comm port error");
            return ZWHCI_ERROR_WRITE;
        }

        total_written += bytes_written;

    }

    return 0;
}


/**
fl_prog_port_setup - Setup comm port parameters
@param[in]	fl_prog_ctx		Context
@return     Return non-zero indicates success, zero indicates failure.
*/
static int fl_prog_port_setup(flash_prog_ctx_t *fl_prog_ctx)
{
    int retry_cnt = 100; //Wait at most 20 seconds

    while (retry_cnt-- > 0)
    {
        //Wait for Z-wave chip to reboot and com port to be ready
        plt_sleep(200);
        fl_prog_ctx->comm_port_fd = open(fl_prog_ctx->comm_port_name, O_RDWR);

        if (fl_prog_ctx->comm_port_fd > 0)
        {
            break;
        }
    }

	if (fl_prog_ctx->comm_port_fd < 0)
	{
		debug_msg_show(fl_prog_ctx->plt_ctx, "Failed to open:%s. Err no:%d", fl_prog_ctx->comm_port_name, errno);
        if (errno == EACCES)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "Access to the port is not allowed \(need root access) or the port does not exist");
        }
		return 0;
	}

    if(isatty (fl_prog_ctx->comm_port_fd))
    {
        char            *term_name;
        struct termios  term_setting;

        term_name = ttyname (fl_prog_ctx->comm_port_fd);
        if (term_name)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "Terminal name:%s", term_name);
        }

        //Get setting
        if(tcgetattr (fl_prog_ctx->comm_port_fd, &term_setting) != 0)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "tcgetattr error");
            close(fl_prog_ctx->comm_port_fd);
            return 0;
        }

        //Set to raw mode
        cfmakeraw (&term_setting);

        //Set speed
        cfsetspeed (&term_setting, B115200);

        //Set input mode
        term_setting.c_cc[VMIN] = FLASH_PROG_CMD_LEN; //minimum 4 char
        term_setting.c_cc[VTIME] = FLASH_PROG_READ_TIMEOUT / 100;//specify how long to wait for input before returning, in units of 0.1 seconds

        //Set comm port setting
        if(tcsetattr (fl_prog_ctx->comm_port_fd, TCSAFLUSH, &term_setting) != 0)
        {
            debug_msg_show(fl_prog_ctx->plt_ctx, "tcsetattr error");
            close(fl_prog_ctx->comm_port_fd);
            return 0;
        }
    }
    else
    {
        debug_msg_show(fl_prog_ctx->plt_ctx, "The fd for %s is NOT a serial comm port", fl_prog_ctx->comm_port_name);
        close(fl_prog_ctx->comm_port_fd);
        return 0;
    }

    return 1;
}


/**
fl_prog_init - Initialize flash programming interface
@param[in]	fl_prog_ctx		Context
@return     Return zero indicates success, non-zero indicates failure.
@post       Caller should not modify the fl_prog_ctx after this call
*/
int fl_prog_init(flash_prog_ctx_t *fl_prog_ctx)
{

    if (!fl_prog_ctx->comm_port_id)
        return ZWHCI_ERROR_INVALID_VALUE;

    fl_prog_ctx->comm_port_name = (char *)fl_prog_ctx->comm_port_id;

    // Open and setup comm port
    if (!fl_prog_port_setup(fl_prog_ctx))
    {
        return ZWHCI_ERROR_RESOURCE;
    }

    return 0;
}


/**
fl_prog_exit - Clean up the flash programming interface
@param[in]	fl_prog_ctx		Context
@return
*/
void fl_prog_exit(flash_prog_ctx_t *fl_prog_ctx)
{
    close(fl_prog_ctx->comm_port_fd);
}

#endif


/**
fl_prog_enable - enable flashp programming interface
@param[in]	    fl_prog_ctx		Context
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_enable(flash_prog_ctx_t *fl_prog_ctx)
{
    int         result;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xAC, 0x53, 0xAA, 0x55};

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    //Check whether interface has been enabled
    if ((rd_buf[2] == 0xAA) && (rd_buf[3] == 0x55))
    {
        return 0;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }

}


/**
fl_prog_sign_get - read signature bytes from the chip
@param[in]	    fl_prog_ctx		Context
@param[out]	    signature		Buffer of at least 7 bytes to store the signature
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_sign_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *signature)
{
    int         result;
    int         i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x30, 0, 0, 0};

    for (i=0; i<7; i++)
    {
        wr_buf[1] = i;

        result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
        if (result != 0)
        {
            return result;
        }

        result = fl_prog_rd(fl_prog_ctx, rd_buf);
        if (result != 0)
        {
            return result;
        }

        if ((rd_buf[0] == 0x30) && (rd_buf[1] == i))
        {
            signature[i] = rd_buf[3];
        }
        else
        {
            return ZWHCI_ERROR_VERIFY;
        }
    }
    return 0;
}


/**
fl_prog_lck_bits_get - read lock bits
@param[in]	    fl_prog_ctx		Context
@param[out]	    lckbit		    Buffer of at least 9 bytes to store the lock bits
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_lck_bits_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *lckbit)
{
    int         result;
    int         i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xF1, 0, 0, 0};

    for (i=0; i<9; i++)
    {
        wr_buf[1] = i;

        result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
        if (result != 0)
        {
            return result;
        }

        result = fl_prog_rd(fl_prog_ctx, rd_buf);
        if (result != 0)
        {
            return result;
        }

        if ((rd_buf[0] == 0xF1) && (rd_buf[1] == i))
        {
            lckbit[i] = rd_buf[3];
        }
        else
        {
            return ZWHCI_ERROR_VERIFY;
        }
    }
    return 0;
}


/**
fl_prog_lck_bits_set - write lock bits
@param[in]	    fl_prog_ctx		Context
@param[in]	    lckbit		    lock bits byte
@param[in]	    pos		        lock bits byte position (valid value 0 to 8)
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_lck_bits_set(flash_prog_ctx_t *fl_prog_ctx, uint8_t lckbit, uint8_t pos)
{
    int         result;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xF0, 0, 0, 0};

    if (pos > 8)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    wr_buf[1] = pos;
    wr_buf[3] = lckbit;

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if ((rd_buf[0] == 0xF0) && (rd_buf[1] == pos) && (rd_buf[3] == lckbit))
    {
        plt_sleep(1);
        return 0;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }
}


/**
fl_prog_reset - clear Auto Programming mod register bit and reset chip
@param[in]	    fl_prog_ctx		Context
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_reset(flash_prog_ctx_t *fl_prog_ctx)
{
    uint8_t     wr_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    return fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
}


/**
fl_prog_nvr_get - read NVR
@param[in]	    fl_prog_ctx		Context
@param[in]	    start_addr		Start address to read the NVR bytes, valid range is 0x08 to 0xFF
@param[in]	    len		        Number of bytes to read. The combination of start_addr and len should
                                not exceed the valid range.
@param[out]	    buf		        Buffer to store the NVR bytes
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_nvr_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t start_addr, uint8_t len)
{
    int         result;
    uint16_t    max_range;
    uint8_t     i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xF2, 0, 0, 0};

    if (start_addr < 0x08)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    max_range = start_addr;
    max_range += len;

    if (max_range > 0x100)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    for (i=0; i<len; i++)
    {
        wr_buf[2] = i + start_addr;

        result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
        if (result != 0)
        {
            return result;
        }

        result = fl_prog_rd(fl_prog_ctx, rd_buf);
        if (result != 0)
        {
            return result;
        }

        if ((rd_buf[0] == 0xF2) && (rd_buf[2] == (i + start_addr)))
        {
            buf[i] = rd_buf[3];
        }
        else
        {
            return ZWHCI_ERROR_VERIFY;
        }
    }
    return 0;
}


/**
fl_prog_sts_get - get the status of programming logic
@param[in]	    fl_prog_ctx		Context
@param[in]	    sts		        status
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_sts_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *sts)
{
    int         result;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x7F, 0xFE, 0, 0};

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if ((rd_buf[0] == 0x7F) && (rd_buf[1] == 0xFE))
    {
        *sts = rd_buf[3];
        return 0;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }
}


/**
fl_prog_erase_sector - Erase code sector
@param[in]	    fl_prog_ctx		Context
@param[in]	    sector		    The sector to be erased, valid range is 0 to 63
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_erase_sector(flash_prog_ctx_t *fl_prog_ctx, uint8_t sector)
{
    int         result;
    uint8_t     sts;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x0B, 0, 0, 0};

    if (sector > 63)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    wr_buf[1] = sector;

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if ((rd_buf[0] == 0x0B) && (rd_buf[1] == sector))
    {
        int retry_cnt;

        //Wait for erase done
        retry_cnt = 3;

        while (retry_cnt-- > 0)
        {
            plt_sleep(50);

            if (fl_prog_sts_get(fl_prog_ctx, &sts) == 0)
            {
                if ((sts & 0x08) == 0)
                {
                    return 0;
                }
            }
        }

        return ZWHCI_ERROR_TIMEOUT;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }
}


/**
fl_prog_erase_chip - Erase chip
@param[in]	    fl_prog_ctx		Context
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_erase_chip(flash_prog_ctx_t *fl_prog_ctx)
{
    int         result;
    int         retry_cnt;
    uint8_t     sts;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x0A, 0, 0, 0};

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if (rd_buf[0] == 0x0A)
    {
        //Wait for erase done
        retry_cnt = 3;

        while (retry_cnt-- > 0)
        {
            plt_sleep(55);

            if (fl_prog_sts_get(fl_prog_ctx, &sts) == 0)
            {
                if ((sts & 0x08) == 0)
                {
                    return 0;
                }
            }
        }

        return ZWHCI_ERROR_TIMEOUT;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }
}


/**
fl_prog_nvr_set - write NVR in a flash fashion (i.e. only turn bit that is '1' to '0')
@param[in]	    fl_prog_ctx		Context
@param[in]	    start_addr		Start address to write the NVR bytes, valid range is 0x08 to 0xFF
@param[in]	    len		        Number of bytes to write. The combination of start_addr and len should
                                not exceed the valid range.
@param[in]	    buf		        Buffer where NVR bytes to be written are stored
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_nvr_set(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t start_addr, uint8_t len)
{
    int         result;
    uint16_t    max_range;
    uint8_t     i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xFE, 0, 0, 0};

    if (start_addr < 0x08)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    max_range = start_addr;
    max_range += len;

    if (max_range > 0x100)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    for (i=0; i<len; i++)
    {
        wr_buf[2] = i + start_addr;
        wr_buf[3] = buf[i];

        result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
        if (result != 0)
        {
            return result;
        }

        result = fl_prog_rd(fl_prog_ctx, rd_buf);
        if (result != 0)
        {
            return result;
        }

        if ((rd_buf[0] == 0xFE) && (rd_buf[2] == (i + start_addr)) && (rd_buf[3] == buf[i]))
        {
            plt_sleep(1);
        }
        else
        {
            return ZWHCI_ERROR_VERIFY;
        }
    }
    return 0;
}


/**
fl_prog_cont_rd - continue read 3 bytes of data
@param[in]	    fl_prog_ctx		Context
@param[out]	    buf		        Buffer to store the data bytes
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_cont_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf)
{
    int         result;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xA0, 0, 0, 0};

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if (rd_buf[0] == 0xA0)
    {
        memcpy(buf, rd_buf + 1, 3);
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }

    return 0;
}


/**
fl_prog_fl_rd - read flash data from a specified sector
@param[in]	    fl_prog_ctx		Context
@param[in]	    sector		    Flash sector of 2k bytes, valid range is 0 to 63
@param[in]	    len		        Number of bytes to read, maximum is 2k bytes (2048 bytes).
@param[out]	    buf		        Buffer to store the data bytes
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_fl_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t sector, uint16_t len)
{
    int         result;
    uint16_t    byte_rd = 0;    //number of bytes read
    int         i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x10, 0, 0, 0};

    if ((sector > 63) || (len > 2048))
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    while (byte_rd < len)
    {
        if (byte_rd == 0)
        {   //First byte
            wr_buf[1] = sector;
            result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
            if (result != 0)
            {
                return result;
            }

            result = fl_prog_rd(fl_prog_ctx, rd_buf);
            if (result != 0)
            {
                return result;
            }

            if ((rd_buf[0] == 0x10) && (rd_buf[1] == sector))
            {
                *buf++ = rd_buf[3];
                byte_rd++;
            }
            else
            {
                return ZWHCI_ERROR_VERIFY;
            }
        }
        else    //second byte onwards
        {
            result = fl_prog_cont_rd(fl_prog_ctx, rd_buf);
            if (result == 0)
            {
                for (i=0; i<3; i++)
                {
                    if (byte_rd < len)
                    {
                        *buf++ = rd_buf[i];
                        byte_rd++;
                    }
                }
            }
            else
            {
                return result;
            }
        }
    }

    return 0;
}


/**
fl_prog_sram_rd - read SRAM data
@param[in]	    fl_prog_ctx		Context
@param[in]	    start_addr		Start address to read the SRAM bytes, valid range is 0 to 0x7FF
@param[in]	    len		        Number of bytes to read. The combination of start_addr and len should
                                not exceed the valid range.
@param[out]	    buf		        Buffer to store the data bytes
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_sram_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint16_t start_addr, uint16_t len)
{
    int         result;
    uint16_t    max_range;
    uint16_t    byte_rd = 0;    //number of bytes read
    int         i;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x06, 0, 0, 0};

    if (start_addr > 0x07FF)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    max_range = start_addr;
    max_range += len;

    if (max_range > 0x0800)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    while (byte_rd < len)
    {
        if (byte_rd == 0)
        {   //First byte
            wr_buf[1] = (uint8_t)(start_addr >> 8);
            wr_buf[2] = (uint8_t)(start_addr & 0x00FF);

            result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
            if (result != 0)
            {
                return result;
            }

            result = fl_prog_rd(fl_prog_ctx, rd_buf);
            if (result != 0)
            {
                return result;
            }

            if ((rd_buf[0] == 0x06) && (rd_buf[1] == wr_buf[1]) && (rd_buf[2] == wr_buf[2]))
            {
                *buf++ = rd_buf[3];
                byte_rd++;
            }
            else
            {
                return ZWHCI_ERROR_VERIFY;
            }
        }
        else    //second byte onwards
        {
            result = fl_prog_cont_rd(fl_prog_ctx, rd_buf);
            if (result == 0)
            {
                for (i=0; i<3; i++)
                {
                    if (byte_rd < len)
                    {
                        *buf++ = rd_buf[i];
                        byte_rd++;
                    }
                }
            }
            else
            {
                return result;
            }
        }
    }

    return 0;
}


/**
fl_prog_cont_wr - continue write 3 bytes of data
@param[in]	    fl_prog_ctx		Context
@param[in]	    buf		        Buffer where SRAM bytes to be written are stored
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_cont_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf)
{
    int         result;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4];

    wr_buf[0] = 0x80;
    memcpy(wr_buf + 1, buf, 3);

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    return (memcmp(wr_buf, rd_buf, 4) == 0)? 0 : ZWHCI_ERROR_VERIFY;
}


/**
fl_prog_sram_wr - write SRAM data
@param[in]	    fl_prog_ctx		Context
@param[in]	    start_addr		Start address to write the SRAM bytes, valid range is 0 to 0x7FF
@param[in]	    len		        Number of bytes to write. The combination of start_addr and len should
                                not exceed the valid range. Also, the len must be (1 + 'multiple of 3'),
                                i.e. valid len are 1, 4, 7, 10, ...
@param[in]	    buf		        Buffer where SRAM bytes to be written are stored
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_sram_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint16_t start_addr, uint16_t len)
{
    int         result;
    uint16_t    max_range;
    uint16_t    byte_wr = 0;    //number of bytes written
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x04, 0, 0, 0};

    if (start_addr > 0x07FF)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    max_range = start_addr;
    max_range += len;

    if (max_range > 0x0800)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    if (len > 1)
    {
        uint16_t    remainder;

        remainder = (len - 1) % 3;

        if (remainder)
        {
            return ZWHCI_ERROR_INVALID_VALUE;
        }
    }

    while (byte_wr < len)
    {
        if (byte_wr == 0)
        {   //First byte
            wr_buf[1] = (uint8_t)(start_addr >> 8);
            wr_buf[2] = (uint8_t)(start_addr & 0x00FF);
            wr_buf[3] = *buf++;

            result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
            if (result != 0)
            {
                return result;
            }

            result = fl_prog_rd(fl_prog_ctx, rd_buf);
            if (result != 0)
            {
                return result;
            }

            if ((rd_buf[0] == 0x04) && (rd_buf[1] == wr_buf[1]) && (rd_buf[2] == wr_buf[2])
                && (rd_buf[3] == wr_buf[3]))
            {
                byte_wr++;
            }
            else
            {
                return ZWHCI_ERROR_VERIFY;
            }
        }
        else    //write 3 bytes
        {
            result = fl_prog_cont_wr(fl_prog_ctx, buf);
            if (result == 0)
            {
                byte_wr += 3;
                buf += 3;
            }
            else
            {
                return result;
            }
        }
    }

    return 0;
}


/**
fl_prog_fl_wr - write flash data to a specified sector
@param[in]	    fl_prog_ctx		Context
@param[in]	    sector		    Flash sector of 2k bytes, valid range is 0 to 63
@param[out]	    buf		        Buffer to store the sector data of 2K bytes
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_fl_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t sector)
{
    unsigned    i;
    int         result;
    int         wr_fl_done;
    int         retry_cnt;
    uint16_t    start_addr = 2048;      //offset to the sector starting address
    uint16_t    end_addr;
    uint16_t    total_len;              //total non 0xFF bytes in the sector
    uint16_t    byte_wr;                //number of bytes written
    uint16_t    byte_pending;           //number of bytes pending for writing
    uint16_t    byte_to_wr;             //number of bytes to be written
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0x20, 0, 0, 0};
    uint8_t     sts;

    if (sector > 63)
    {
        return ZWHCI_ERROR_INVALID_VALUE;
    }

    //Skip leading 0xFF
    for (i=0; i<2048; i++)
    {
        if (buf[i] != 0xFF)
        {
            start_addr = i;
            break;
        }
    }

    if (start_addr == 2048)
    {   //The whole sector is filled with 0xFF, don't need to write anything
        return 0;
    }

    //Skip trailing 0xFF
    end_addr = start_addr;
    for (i=2047; i>start_addr; i--)
    {
        if (buf[i] != 0xFF)
        {
            end_addr = i;
            break;
        }
    }

    total_len = end_addr - start_addr + 1;
    byte_wr = 0;

    while (byte_wr < total_len)
    {
        byte_pending = total_len - byte_wr;
        if (byte_pending < 4)
        {   //Write one byte at a time
            byte_to_wr = 1;
        }
        else
        {
            uint16_t    triplets;

            triplets = (byte_pending - 1)/3;
            byte_to_wr = 1 + (triplets * 3);
        }

        //Transfer the sector data to SRAM
        result = fl_prog_sram_wr(fl_prog_ctx, buf + start_addr, start_addr, byte_to_wr);

        if (result != 0)
        {
            return result;
        }

        //Write to the sector
        wr_buf[1] = sector;
        result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
        if (result != 0)
        {
            return result;
        }

        result = fl_prog_rd(fl_prog_ctx, rd_buf);
        if (result != 0)
        {
            return result;
        }


        if ((rd_buf[0] == 0x20) && (rd_buf[1] == sector))
        {
            //Wait for the process to finish
            wr_fl_done = 0;
            retry_cnt = 3;

            while (retry_cnt-- > 0)
            {
                plt_sleep(13);

                if (fl_prog_sts_get(fl_prog_ctx, &sts) == 0)
                {
                    if ((sts & 0x08) == 0)
                    {
                        wr_fl_done = 1;

                        //Adjustment
                        byte_wr += byte_to_wr;
                        start_addr += byte_to_wr;
                        break;
                    }
                }
            }

            if (!wr_fl_done)
            {
                return ZWHCI_ERROR_TIMEOUT;
            }

        }
        else
        {
            return ZWHCI_ERROR_VERIFY;
        }
    }

    return 0;
}


/**
fl_prog_crc_chk - Run CRC-32 check from address 0x0000 to 0x1FFFB
@param[in]	    fl_prog_ctx		Context
@param[in]	    status		    CRC status: 1=passed; 0=failed
@return     Return zero indicates success, non-zero indicates failure
*/
int fl_prog_crc_chk(flash_prog_ctx_t *fl_prog_ctx, uint8_t *status)
{
    int         result;
    int         retry_cnt;
    uint8_t     sts;
    uint8_t     rd_buf[4];
    uint8_t     wr_buf[4] = {0xC3, 0, 0, 0};

    result = fl_prog_cmd_wr(fl_prog_ctx, wr_buf);
    if (result != 0)
    {
        return result;
    }

    result = fl_prog_rd(fl_prog_ctx, rd_buf);
    if (result != 0)
    {
        return result;
    }

    if (rd_buf[0] == 0xC3)
    {
        //Wait for calculation done
        retry_cnt = 3;

        while (retry_cnt-- > 0)
        {
            plt_sleep(5);

            if (fl_prog_sts_get(fl_prog_ctx, &sts) == 0)
            {
                if ((sts & 0x01) == 0)
                {
                    debug_msg_show(fl_prog_ctx->plt_ctx, "fl status:%02X", (unsigned)sts);
                    *status = (sts & 0x02)? 1 : 0;
                    return 0;
                }
            }
        }

        return ZWHCI_ERROR_TIMEOUT;
    }
    else
    {
        return ZWHCI_ERROR_VERIFY;
    }
}

/**
@}
*/

