/**
@file   zw_hci_transport.h - Z-wave host controller interface transport layer implementation header file.

        Platform specific implementation of serial port access.

@author David Chow

@version    1.0 7-5-10  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_HCI_TRANSPORT_DAVID_
#define _ZW_HCI_TRANSPORT_DAVID_
//#define WIN32

#if defined(_WINDOWS) || defined(WIN32)
#include "zw_plt_windows.h"   //use in Windows platform
#elif defined(OS_LINUX)
#include "zw_plt_linux.h"     //use in Linux platform
#endif
#include <stdint.h>
#include <stdio.h>
#include "zw_hci_error.h"
#include "zw_hci_platform.h"

/**
@defgroup Transport Transport layer APIs
Platform specific implementation of serial port access.
@{
*/

//Minimum read time out value
#define TRANSPORT_READ_TIMEOUT_MIN      2000    ///< Minimum read time out value in milliseconds

///
/// Maximum comm port read and write buffer size
#define COMM_MAX_READ_BUFFER        1024
#define COMM_MAX_WRITE_BUFFER       512

///
/// Timeout to check for write request event
#define COMM_WRITE_CHECK_TIMEOUT     2000

///
/// Timeout to wait for threads to exit
#define TRANSPORT_THREADS_EXIT_TIMEOUT     5000


//Forward declaration of frame layer context
struct _frm_layer_ctx;

///
/// Z-wave hci transport layer context
typedef struct _tpt_layer_ctx
{
    //Callback functions to frame layer
    void    (*tpt_rd_cb)(struct _tpt_layer_ctx *tpt_ctx, uint8_t *buf, uint8_t data_length);///< Pointer to read callback function
    void    (*tpt_rd_tmout_cb)(struct _tpt_layer_ctx *tpt_ctx);///< Pointer to read timeout callback function
    //Data
    uint32_t    tpt_rd_tmout;///< Time out for read (time since the most recently received byte) in milliseconds
    struct _frm_layer_ctx *frm_layer_ctx; ///< Pointer to frame layer context
    plt_ctx_t   *plt_ctx;               ///< Platform context
    void        *comm_port_id;          ///< Pointer to platform specific serial comm port id/name
    #if defined(_WINDOWS) || defined(WIN32)
    void        *wr_req_mtx;            ///< mutex for accessing write request list
    TCHAR       *comm_port_name;        ///< pointer to comm port name
    COMMTIMEOUTS    comm_port_tmout;    ///< previous comm port timeout settings
    HANDLE  comm_port_hdl;              ///< comm port handle
    HANDLE  rd_thrd_hdl;                ///< read thread handle
    HANDLE  wr_thrd_hdl;                ///< write thread handle
    HANDLE  wr_evt_hdl;                 ///< handle of the synchronization event for write requests
    HANDLE  thrd_exit_evt_hdl;          ///< handle of the synchronization event for terminating threads
    util_lst_t   *wr_req_hd;            ///< head of linked list for write requests
    #endif

    #ifdef OS_LINUX
    char            *comm_port_name;    ///< pointer to comm port name
    int             comm_port_fd;       ///< comm port file descriptor
    void            *wr_req_mtx;        ///< mutex for accessing write request list
    void            *wr_q_sem;          ///< semaphore for waiting requests to write to the comm port
    util_lst_t      *wr_req_hd;         ///< head of linked list for write requests
    volatile int    rd_thrd_run;        ///< control the read thread whether to run. 1 = run, 0 = stop
    volatile int    rd_thrd_sts;        ///< read thread status. 1 = running, 0 = thread exited
    volatile int    wr_thrd_run;        ///< control the write thread whether to run. 1 = run, 0 = stop
    volatile int    wr_thrd_sts;        ///< write thread status. 1 = running, 0 = thread exited
    volatile int    wr_started;         ///< flag to indicate write has started.
    #endif

} tpt_layer_ctx_t;


int32_t tpt_wr_req_create(tpt_layer_ctx_t *tpt_ctx, uint8_t   *buf, uint8_t dat_sz);
int32_t tpt_init(tpt_layer_ctx_t *tpt_ctx);
void tpt_exit(tpt_layer_ctx_t *tpt_ctx);


/**
@}
*/


#endif /* _ZW_HCI_TRANSPORT_DAVID_ */
