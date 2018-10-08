/**
@file   zw_hci_fl_prog.h - Flash programming implementation

        Platform specific implementation of flash programming.

@author David Chow

@version    1.0 2-9-13  Initial release

comments: Initial release
*/

#ifndef _ZW_HCI_FLASH_PROG_DAVID_
#define _ZW_HCI_FLASH_PROG_DAVID_

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
@defgroup FlashProg Flash programming APIs
Platform specific implementation of flash programming.
@{
*/

#define FLASH_PROG_CMD_LEN          4       ///< Flash programming command length
#define FLASH_PROG_READ_TIMEOUT     1000    ///< Flash read timeout
#define FL_PORT_READ_TIMEOUT        2000    ///< Flash port read timeout in milliseconds

///
/// Z-wave flash programming context
typedef struct _flash_prog_ctx
{
    plt_ctx_t   *plt_ctx;               ///< Platform context
    void        *comm_port_id;          ///< Pointer to platform specific serial comm port id/name

    #if defined(_WINDOWS) || defined(WIN32)
    TCHAR           *comm_port_name;    ///< pointer to comm port name
    COMMTIMEOUTS    comm_port_tmout;    ///< previous comm port timeout settings
    HANDLE          comm_port_hdl;      ///< comm port handle
    #endif

    #ifdef OS_LINUX
    char            *comm_port_name;    ///< pointer to comm port name
    int             comm_port_fd;       ///< comm port file descriptor
    #endif

} flash_prog_ctx_t;

int fl_prog_init(flash_prog_ctx_t *fl_prog_ctx);
void fl_prog_exit(flash_prog_ctx_t *fl_prog_ctx);
int fl_prog_enable(flash_prog_ctx_t *fl_prog_ctx);
int fl_prog_sign_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *signature);
int fl_prog_lck_bits_set(flash_prog_ctx_t *fl_prog_ctx, uint8_t lckbit, uint8_t pos);
int fl_prog_lck_bits_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *lckbit);
int fl_prog_reset(flash_prog_ctx_t *fl_prog_ctx);
int fl_prog_nvr_get(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t start_addr, uint8_t len);
int fl_prog_nvr_set(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t start_addr, uint8_t len);
int fl_prog_fl_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t sector, uint16_t len);
int fl_prog_fl_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint8_t sector);
int fl_prog_sram_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint16_t start_addr, uint16_t len);
int fl_prog_sram_wr(flash_prog_ctx_t *fl_prog_ctx, uint8_t *buf, uint16_t start_addr, uint16_t len);
int fl_prog_erase_sector(flash_prog_ctx_t *fl_prog_ctx, uint8_t sector);
int fl_prog_erase_chip(flash_prog_ctx_t *fl_prog_ctx);
int fl_prog_crc_chk(flash_prog_ctx_t *fl_prog_ctx, uint8_t *status);


/**
@}
*/


#endif /* _ZW_HCI_FLASH_PROG_DAVID_ */
