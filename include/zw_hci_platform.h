/**
@file   zw_hci_platform.h - Z-wave host controller interface platform specific implementation header file.

        To implement some common functions in multi-platform environment.

@author David Chow

@version    1.0 7-5-10  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_HCI_PLATFORM_DAVID_
#define _ZW_HCI_PLATFORM_DAVID_

#include "zw_hci_util.h"

/**
@defgroup Platform Multi-platform APIs
Some common functions in multi-platform environment.
@{
*/

#ifdef OS_LINUX
///
/// Platform timer resolution in milliseconds
#define PLT_TIMER_RESOLUTION    100
#endif

#ifdef WIN32
///
/// Use safe version of functions like sprintf_s
#define USE_SAFE_VERSION
#endif

///
/// Maximum message length for displaying message
#define MAX_PLT_MSG_LEN     256

///
/// Maximum number of binary bytes for displaying in a line
#define MAX_BIN_LINE_LEN    10

///
/// Timer callback function
typedef void    (*tmr_cb_t)(void *data);

///
/// Print text function
typedef void    (*print_fn)(void *msg);

#if defined(_WINDOWS) || defined(WIN32)
///Platform context
typedef struct
{
    print_fn          print_txt;      ///< Print text function
    int                 init_done;      ///< Counter to indicated how many times platform initialization has been invoked
} plt_ctx_t;

#elif defined(OS_LINUX)
///Platform context
typedef struct
{
    volatile uint16_t   tmr_tick;       ///< Timer tick every PLT_TIMER_RESOLUTION
    uint16_t            id_gen;         ///< One-shot Timer identifier generator (range 1 to 0x7FFF)
    uint16_t            per_id_gen;     ///< Periodic Timer identifier generator (range 0x8000 to 0xFFFF)
    int                 init_done;      ///< Counter to indicated how many times platform initialization has been invoked
    void                *tmr_mtx;       ///< mutex for accessing timer list
    void                *tmr_sem;       ///< semaphore for waiting timer tick event
    util_lst_t          *tmr_lst_hd;    ///< head of linked list for timers
    volatile int        tmr_tck_thrd_run;   ///< control the timer tick thread whether to run. 1 = run, 0 = stop
    volatile int        tmr_tck_thrd_sts;   ///< timer tick thread status. 1 = running, 0 = thread exited
    volatile int        tmr_chk_thrd_run;   ///< control the timer check thread whether to run. 1 = run, 0 = stop
    volatile int        tmr_chk_thrd_sts;   ///< timer check thread status. 1 = running, 0 = thread exited
    print_fn          print_txt;      ///< Print text function

} plt_ctx_t;
#endif

void        plt_msg_show(void *pltfm_ctx, char * format, ...);
void        plt_msg_ts_show(void *pltfm_ctx, char * format, ...);
void        plt_bin_show(void *pltfm_ctx, void * buf, uint32_t len);
void        *plt_tmr_start(plt_ctx_t *pltfm_ctx, uint32_t  tmout_ms, tmr_cb_t  tmout_cb, void *data);
uint32_t    plt_tmr_stop(plt_ctx_t *pltfm_ctx, void	*context);
uint32_t    plt_mtx_init(void **context);
void        plt_mtx_destroy(void *context);
int         plt_mtx_trylck(void *context);
void        plt_mtx_lck(void *context);
void        plt_mtx_ulck(void *context);
uint32_t    plt_sem_init(void **context);
void        plt_sem_destroy(void *context);
void        plt_sem_wait(void *context);
void        plt_sem_post(void *context);
int         plt_cond_init(void **context);
int         plt_cond_timedwait(void *cond_ctx, void *mtx, uint16_t timeout_ms);
void        plt_cond_broadcast(void *cond_ctx);
void        plt_cond_signal(void *cond_ctx);
void        plt_cond_destroy(void *cond_ctx);
int         plt_thrd_create(void (*start_adr)( void * ), void *args);
void        plt_sleep(uint32_t    tmout_ms);
void        *plt_periodic_start(plt_ctx_t *pltfm_ctx, uint32_t  tmout_ms, tmr_cb_t  tmout_cb, void *data);
#if defined(_WINDOWS) || defined(WIN32)
int         plt_utf16_to_8(const char *utf16_src, char *utf8_output, uint8_t out_buf_len, uint8_t big_endian);
#else
int         plt_utf16_to_8(const uint16_t *utf16_src, uint8_t *utf8_output, uint8_t out_buf_len, uint8_t big_endian);
#endif
int         plt_utf8_chk(const uint8_t *utf8, uint8_t utf8_len);
int         plt_init(plt_ctx_t *pltfm_ctx, print_fn display_txt_fn);
void        plt_exit(plt_ctx_t *pltfm_ctx);
int16_t     plt_rand_get(void);
void        *plt_memmem(const void *buf, size_t buf_len, const void *byte_sequence, size_t byte_sequence_len);

#if defined(_WINDOWS) || defined(WIN32)
#define strcasecmp _stricmp
#endif



/**
@}
*/



#endif /* _ZW_HCI_PLATFORM_DAVID_ */

