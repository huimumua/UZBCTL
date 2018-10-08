/**
@file   zw_poll.h - Z-wave High Level API poll facility header file.

@author David Chow

@version    1.0 26-3-13  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_POLL_DAVID_
#define _ZW_POLL_DAVID_
/**
@defgroup If_Poll Polling Interface APIs
Used to create and delete polling commands to a device
@{
*/

#define POLL_TIMER_TICK             500     /**< Periodic timer tick interval in ms */
#define POLL_TICK_PER_SEC           (1000/POLL_TIMER_TICK)     /**< Number of timer ticks per second */
#define MIN_POLL_TIME               (10 * POLL_TICK_PER_SEC) /**< Minimum polling time in terms of timer tick */
#define CHECK_EXPIRY_INTERVAL       (1 * POLL_TICK_PER_SEC)  /**< Check for polling entries expiry interval
                                                                  in terms of timer tick */


/** Polling queue entry*/
typedef struct
{
	zwifd_t     ifd;	        /**< Interface associated with the command */
    uint32_t    next_poll_tm;   /**< Next polling time */
    uint32_t    usr_token;      /**< User defined token to facilitate deletion of multiple polling requests */
    uint32_t    interval;       /**< Polling interval in terms of timer tick*/
    uint16_t    poll_cnt;       /**< Number of times to poll; zero = unlimited times*/
    uint16_t    handle;         /**< Polling request handle*/
    uint16_t    cmd_cls;        /**< Expected command class of the report*/
    uint8_t     rpt;            /**< Expected report command of the report*/
    uint8_t     node_id;        /**< Node id*/
    uint8_t     dat_len;        /**< Length of the dat_buf field */
    uint8_t     dat_buf[1];     /**< Place holder for the data buffer */
}
poll_q_ent_t;


/** Polling context */
typedef struct  _poll_ctx
{
    volatile uint32_t   tmr_tick;           /**< Periodic timer tick, incremented every POLL_TIMER_TICK ms */
    volatile int        tmr_chk_thrd_run;   /**< Control the timer check thread whether to run. 1 = run, 0 = stop */
    volatile int        tmr_chk_thrd_sts;   /**< Timer check thread status. 1 = running, 0 = thread exited */
    uint32_t            next_poll_tm;       /**< Next polling time */
    void                *tmr_sem;           /**< Semaphore for waiting timer tick event */
    void                *poll_mtx;          /**< Mutex for the polling facility */
    void                *tick_tmr_ctx;      /**< Tick timer context */
    util_lst_t          *poll_lst_hd;       /**< Head of linked list for polling requests */
    zwnet_p             net;                /**< Network */
    uint16_t            handle_gen;         /**< Handle number generator */
    uint32_t            cur_start_tm;       /**< Start time of the current poll*/
    uint32_t            cur_cmd_tm;         /**< Command time of the current poll*/
    uint16_t            cur_handle;         /**< Handle of the current poll*/
    uint16_t            cur_cmd_cls;        /**< Expected command class of the report*/
    uint8_t             cur_rpt;            /**< Expected report command of the report*/
    uint8_t             cur_node_id;        /**< Node id of the current poll*/
    uint8_t             cur_node_last;      /**< Flag to indicate cur_node_id is used for the last polling and should not
                                                 be used again in the next polling*/

}
zwpoll_ctx_t;

int zwpoll_init(zwpoll_ctx_t *poll_ctx);
void zwpoll_shutdown(zwpoll_ctx_t *poll_ctx);
void zwpoll_exit(zwpoll_ctx_t *poll_ctx);
void zwpoll_beam_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm);
void zwpoll_rpt_chk(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len);
int zwpoll_rm(zwnet_p net, uint16_t handle);
int zwpoll_rm_mul(zwnet_p net, uint32_t usr_token);
int zwpoll_add(zwnet_p net, poll_q_ent_t *poll_ent);



/**
@}
*/



#endif /* _ZW_POLL_DAVID_ */

