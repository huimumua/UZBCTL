/**
@file   zw_hci_error.h - Z-wave host controller interface error code header file.

        Definition of error codes.

@author David Chow

@version    1.0 18-5-10  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_HCI_ERROR_DAVID_
#define _ZW_HCI_ERROR_DAVID_

#define     DEBUG_ZWHCI     //Enable debug message

/// Error codes definition
#define     ZWHCI_NO_ERROR                     0 ///< No error
#define     ZWHCI_ERROR_MEMORY              -100 ///< Out of memory
#define     TRANSPORT_ERROR_SIGNALING       -101 ///< Could not signal write thread to perform actual write
#define     FRAME_ERROR_MULTIPLE_WRITE      -102 ///< Write while the previous write in progress is not allowed
#define     FRAME_ERROR_SEND_TIMER          -103 ///< Send timer not functioning
#define     SESSION_ERROR_PREVIOUS_COMMAND_UNCOMPLETED  -104 ///< Previous command uncompleted, retry later
#define     SESSION_ERROR_SYNC              -105 ///< Synchronization error with the frame layer
#define     SESSION_ERROR_SEND_CHKSUM       -106 ///< Send error at lower layer due to checksum error
#define     SESSION_ERROR_SND_FRM_TMOUT     -107 ///< Send frame timeout due to no ACK received
#define     SESSION_ERROR_RESP_TMOUT        -108 ///< Wait for response frame timeout
#define     SESSION_ERROR_INVALID_RESP      -109 ///< The response command id doesn't match with the sent command id
#define     APPL_TX_Q_OVERFLOW              -110 ///< The controller transmit queue overflow
#define     APPL_OPER_ALREADY_ACTIVE        -111 ///< The requested operation is already active
#define     APPL_OPER_NOT_STARTED           -112 ///< The requested operation fails to start
#define     APPL_ERROR_WAIT_CB              -113 ///< Waiting for transmit complete callback function, retry later
#define     SESSION_ERROR_SEND_BUSY         -114 ///< Send error at lower layer due to controller busy
#define     ZWHCI_ERROR_RESOURCE            -115 ///< Out of resource
#define     ZWHCI_ERROR_INVALID_VALUE       -116 ///< Invalid value in one of the parameters
#define     ZWHCI_ERROR_WRITE               -117 ///< Write failed
#define     ZWHCI_ERROR_READ                -118 ///< Read failed
#define     ZWHCI_ERROR_TIMEOUT             -119 ///< Time out
#define     ZWHCI_ERROR_VERIFY              -120 ///< Verification failed

// Initialization error code
#define     INIT_ERROR_TRANSPORT        1   ///< Initialization error on transport layer
#define     INIT_ERROR_FRAME            2   ///< Initialization error on frame layer
#define     INIT_ERROR_SESSION          3   ///< Initialization error on session layer
#define     INIT_ERROR_APPL             4   ///< Initialization error on application layer

#ifdef  DEBUG_ZWHCI
#define     debug_msg_show    plt_msg_show
#else
#define     debug_msg_show(...)
#endif


#endif /* _ZW_HCI_ERROR_DAVID_ */
