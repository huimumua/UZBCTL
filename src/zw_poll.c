/**
@file   zw_poll.c - Z-wave High Level API poll facility implementation.

@author David Chow

@version    1.0 26-3-13  Initial release

version: 1.0
comments: Initial release
*/

#include "../include/zw_api_pte.h"
#include "../include/zw_api_util.h"
#include "../include/zw_poll.h"


static int zwpoll_cmd_send(zwpoll_ctx_t *poll_ctx, util_lst_t *poll_lst_ent, util_lst_t *prev_lst_ent);

/**
@defgroup If_Poll Polling Interface APIs
Used to create and delete polling commands to a device
@{
*/

/**
zwpoll_tmr_exp_chk - Check whether the timer has expired
@param[in] now     Current time
@param[in] timer   The time of the timer to check
@return 1=timer has expired; otherwise 0
*/
static int    zwpoll_tmr_exp_chk(uint32_t now, uint32_t timer)
{
    uint32_t    time_diff;
    if (now == timer)
    {
        return 1;
    }

    //Handle wrap over case
    if (now > timer)
    {
        time_diff = now - timer;
        if (time_diff < 0xF0000000)
        {
            return 1;
        }
    }
    else //now < timer
    {
        time_diff = timer - now;
        if (time_diff >= 0xF0000000)
        {
            return 1;
        }
    }
    //Not expired
    return 0;
}


/**
zwpoll_tm_diff - Calculate time different in terms of timer tick
@param[in] tm1     Time 1
@param[in] tm2     Time 2
@return time different in terms of timer tick
*/
static uint32_t   zwpoll_tm_diff(uint32_t tm1, uint32_t tm2)
{
    uint32_t    time_diff;
    uint32_t    temp;

    if (tm1 == tm2)
    {
        return 0;
    }

    if (tm1 < tm2)
    {   //swap
        temp = tm1;
        tm1 = tm2;
        tm2 = temp;
    }

    time_diff = tm1 - tm2;
    if (time_diff >= 0xF0000000)
    {   //wrap over
        time_diff = (0xFFFFFFFF - tm1) + 1 + tm2;
    }
    return time_diff;

}


/**
zwpoll_rpt_chk - Check whether the report is the polling report
@param[in]	intf	Interface
@param[in]	cmd_buf	The report command received
@param[in]	cmd_len	The length of cmd_buf
@return
*/
void zwpoll_rpt_chk(zwif_p intf, uint8_t *cmd_buf, uint8_t cmd_len)
{
    zwnode_p        node = intf->ep->node;
    zwnet_p         nw = node->net;
    zwpoll_ctx_t    *poll_ctx = nw->poll_ctx;
    uint16_t        cls;
    uint8_t         rpt;

    //Get command class and the report command
    if (!zwif_cls_cmd_get(cmd_buf, cmd_len, &cls, &rpt))
    {
        return;
    }

    plt_mtx_lck(poll_ctx->poll_mtx);

    if (node->nodeid == poll_ctx->cur_node_id)
    {
        if ((poll_ctx->cur_cmd_cls == cls)
            && (poll_ctx->cur_rpt == rpt))
        {   //The polling report has arrived
            util_lst_t      *prev_ent;      //Pointer to previous list entry
            util_lst_t      *temp;
            poll_q_ent_t    *poll_q_ent;

            //Re-calculate next poll time
            poll_ctx->next_poll_tm = poll_ctx->tmr_tick + poll_ctx->cur_cmd_tm + MIN_POLL_TIME;

            //Clear the command class and report
            poll_ctx->cur_cmd_cls = poll_ctx->cur_rpt = 0;

            //
            //Check all the entries from the same node for polling time expiry
            //
            prev_ent = NULL;
            temp = poll_ctx->poll_lst_hd;

            while (temp)
            {
                poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

                //Search for the node id
                if (poll_q_ent->node_id == poll_ctx->cur_node_id)
                {
                    break;
                }
                prev_ent = temp;
                temp = temp->next;
            }

            //Found entry with current node id
            while (temp)
            {
                poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

                //Check for the node id
                if (poll_q_ent->node_id != poll_ctx->cur_node_id)
                {   //No more entry in this node has expired
                    //Set flag so that the next poll will start with other node
                    poll_ctx->cur_node_last = 1;
                    break;
                }

                if (zwpoll_tmr_exp_chk(poll_ctx->tmr_tick, poll_q_ent->next_poll_tm))
                {   //Expired
                    //Send the polling command
                    zwpoll_cmd_send(poll_ctx, temp, prev_ent);
                    break;
                }

                //Get the next poll entry
                prev_ent = temp;
                temp = temp->next;

                if (!temp)
                {
                    //No more polling requests that belong to this node have expired
                    //Set flag so that the next poll will start from the list head node
                    poll_ctx->cur_node_last = 1;
                }
            }
        }
    }

    plt_mtx_ulck(poll_ctx->poll_mtx);
}


/**
zwpoll_tx_sts_hdlr - handle transmit status
@param[in]	poll_ctx    Polling context
@param[in]	cur_tm      Current time
@param[in]	tx_sts		The transmit complete status
@return
*/
static void zwpoll_tx_sts_hdlr(zwpoll_ctx_t *poll_ctx, uint32_t cur_tm, uint8_t tx_sts)
{
    plt_mtx_lck(poll_ctx->poll_mtx);

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        uint32_t    cmd_tm;

        //Calculate command time
        cmd_tm = zwpoll_tm_diff(cur_tm, poll_ctx->cur_start_tm);
        if (cmd_tm == 0)
        {
            cmd_tm++;
        }

        //Re-calculate next poll time
        poll_ctx->next_poll_tm = poll_ctx->tmr_tick + cmd_tm + MIN_POLL_TIME;

        //Save the command time
        poll_ctx->cur_cmd_tm = cmd_tm;

    }
    else
    {
        //Re-calculate next poll time
        poll_ctx->next_poll_tm = poll_ctx->tmr_tick + MIN_POLL_TIME;
    }

    plt_mtx_ulck(poll_ctx->poll_mtx);
}


/**
zwpoll_beam_tx_cb - send command to node that requires wake up beam callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwpoll_beam_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p         nw = (zwnet_p)appl_ctx->data;
    zwpoll_ctx_t    *poll_ctx = nw->poll_ctx;
    uint32_t        cur_tm;

    cur_tm = poll_ctx->tmr_tick;

    //Call back zwnode_wait_tx_cb
    zwnode_wait_tx_cb(appl_ctx, tx_sts, user_prm);

    zwpoll_tx_sts_hdlr(poll_ctx, cur_tm, tx_sts);

}


/**
zwpoll_tx_cb - send poll command callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwpoll_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p         nw = (zwnet_p)appl_ctx->data;
    zwpoll_ctx_t    *poll_ctx = nw->poll_ctx;
    uint32_t        cur_tm;

    cur_tm = poll_ctx->tmr_tick;

    zwpoll_tx_sts_hdlr(poll_ctx, cur_tm, tx_sts);

}


/**
zwpoll_cmd_send - Send a poll command and update the next poll time and poll list
@param[in] poll_ctx     Polling context
@param[in] poll_lst_ent Poll list entry
@param[in] prev_lst_ent Previous poll list entry
@return	ZW_ERR_xxx of call to zwif_exec_ex
@pre    Caller must lock the poll_mtx
*/
static int zwpoll_cmd_send(zwpoll_ctx_t *poll_ctx, util_lst_t *poll_lst_ent, util_lst_t *prev_lst_ent)
{
    int             result;
    poll_q_ent_t    *poll_q_ent = (poll_q_ent_t *)poll_lst_ent->wr_buf;

    poll_ctx->cur_node_id = poll_q_ent->node_id;
    poll_ctx->cur_handle = poll_q_ent->handle;
    poll_ctx->cur_node_last = 0;

    result = zwif_exec_ex(&poll_q_ent->ifd, poll_q_ent->dat_buf, poll_q_ent->dat_len,
                          zwpoll_tx_cb, NULL,
                          ZWIF_OPT_POLL, NULL);

    poll_ctx->cur_start_tm = poll_ctx->tmr_tick;
    poll_ctx->cur_cmd_tm = 0;
    poll_ctx->cur_cmd_cls = poll_q_ent->cmd_cls;
    poll_ctx->cur_rpt = poll_q_ent->rpt;

    //Update the poll entry next polling time
    poll_q_ent->next_poll_tm = (poll_q_ent->interval < MIN_POLL_TIME)? MIN_POLL_TIME : poll_q_ent->interval;
    poll_q_ent->next_poll_tm += poll_ctx->tmr_tick;

    //Decrement poll count for non-repetitive polling
    if (poll_q_ent->poll_cnt > 1)
    {
        poll_q_ent->poll_cnt--;
    }
    else if (poll_q_ent->poll_cnt == 1)
    {
        //Remove the poll request
        if (prev_lst_ent)
        {
            prev_lst_ent->next = poll_lst_ent->next;
        }
        else
        {
            poll_ctx->poll_lst_hd = poll_lst_ent->next;
        }
        free(poll_lst_ent);
    }

    //Update poll context next polling time
    poll_ctx->next_poll_tm = poll_ctx->tmr_tick + MIN_POLL_TIME;

    return result;

}


/**
zwpoll_tmr_chk_thrd - thread to process timer tick event
@param[in]	data		Context
@return
*/
static void zwpoll_tmr_chk_thrd(void *data)
{
    zwpoll_ctx_t    *poll_ctx = (zwpoll_ctx_t *)data;
    util_lst_t      *prev_ent;      //Pointer to previous list entry
    util_lst_t      *temp;
    poll_q_ent_t    *poll_q_ent;
    //int             result;
    uint16_t        start_handle;   //Starting point handle
    uint8_t         start_node_id;  //Starting point node id

    poll_ctx->tmr_chk_thrd_sts = 1;

    while (1)
    {
        //Wait for timer tick event
        plt_sem_wait(poll_ctx->tmr_sem);

        //Check whether to exit the thread
        if (poll_ctx->tmr_chk_thrd_run == 0)
        {
            poll_ctx->tmr_chk_thrd_sts = 0;
            return;
        }

        plt_mtx_lck(poll_ctx->poll_mtx);

        //Check whether the next poll time has expired
        if (zwpoll_tmr_exp_chk(poll_ctx->tmr_tick, poll_ctx->next_poll_tm) == 0)
        {   //Not expire yet, continue to wait
            plt_mtx_ulck(poll_ctx->poll_mtx);
            continue;
        }

        if (poll_ctx->poll_lst_hd == NULL)
        {   //Poll queue is empty
            poll_ctx->next_poll_tm = poll_ctx->tmr_tick + MIN_POLL_TIME;
            plt_mtx_ulck(poll_ctx->poll_mtx);
            continue;
        }

        prev_ent = NULL;
        temp = poll_ctx->poll_lst_hd;

        //Find a starting point (poll request) to check for poll expiry
        while (temp)
        {
            poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

            //Search for the node id & handle
            if (poll_q_ent->node_id == poll_ctx->cur_node_id)
            {
                if (poll_q_ent->handle == poll_ctx->cur_handle)
                {
                    //Point to the next poll entry as starting point
                    prev_ent = temp;
                    temp = temp->next;
                    break;
                }
            }
            else if (poll_q_ent->node_id > poll_ctx->cur_node_id)
            {   //All the poll entries belong to the current node id have been removed,
                //use the current node (higher node id) as starting point
                break;
            }

            prev_ent = temp;
            temp = temp->next;
        }

        if (temp)
        {
            poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

            if (poll_ctx->cur_node_last
                && (poll_q_ent->node_id == poll_ctx->cur_node_id))
            {   //Readjust starting point
                //Skip all entries that belong to current node
                while (temp)
                {
                    poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

                    if (poll_q_ent->node_id > poll_ctx->cur_node_id)
                    {
                        break;
                    }

                    prev_ent = temp;
                    temp = temp->next;
                }
            }
        }

        if (!temp)
        {
            //Use the first entry in the poll queue as starting point
            prev_ent = NULL;
            temp = poll_ctx->poll_lst_hd;
        }

        //Reset flag
        //poll_ctx->cur_node_last = 0;

        //Save the starting point
        poll_q_ent = (poll_q_ent_t *)temp->wr_buf;
        start_handle = poll_q_ent->handle;
        start_node_id = poll_q_ent->node_id;

        while (temp)
        {
            if (zwpoll_tmr_exp_chk(poll_ctx->tmr_tick, poll_q_ent->next_poll_tm))
            {   //Expired
                break;
            }

            //Get the next poll entry
            prev_ent = temp;
            temp = temp->next;

            if (!temp)
            {
                //Use the first entry in the poll queue
                prev_ent = NULL;
                temp = poll_ctx->poll_lst_hd;
            }

            poll_q_ent = (poll_q_ent_t *)temp->wr_buf;

            //Check whether all the poll entries have been checked
            if ((start_node_id == poll_q_ent->node_id) && (start_handle == poll_q_ent->handle))
            {
                temp = NULL;
            }
        }

        if (temp)
        {   //Send the polling command
             zwpoll_cmd_send(poll_ctx, temp, prev_ent);
        }
        else
        {
            //Update next poll time
            poll_ctx->next_poll_tm = poll_ctx->tmr_tick + CHECK_EXPIRY_INTERVAL;
        }

        plt_mtx_ulck(poll_ctx->poll_mtx);
    }
}


/**
zwpoll_tmr_tick_cb - Timer tick timeout callback
@param[in] data     Pointer to security layer context
@return
*/
static void    zwpoll_tmr_tick_cb(void *data)
{
    zwpoll_ctx_t   *poll_ctx = (zwpoll_ctx_t *)data;

    //Increment timer tick
    poll_ctx->tmr_tick++;

    //Send timer tick event
    plt_sem_post(poll_ctx->tmr_sem);
}


/**
zwpoll_hdl_cmp - Compare two handles
@param[in]	h1	Handle 1
@param[in]	h2	Handle 2
@return     zero if h1 equals to h2
*/
static int zwpoll_hdl_cmp(uint8_t *h1, uint8_t *h2)
{
    poll_q_ent_t   *poll_ent1 = (poll_q_ent_t *)h1;
    poll_q_ent_t   *poll_ent2 = (poll_q_ent_t *)h2;

    return (poll_ent1->handle == poll_ent2->handle)? 0 : 1;

}


/**
zwpoll_rm - remove a polling request
@param[in]	net	        network
@param[in]	handle	    handle of the polling request to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwpoll_rm(zwnet_p net, uint16_t handle)
{
    poll_q_ent_t poll_ent;

    memset(&poll_ent, 0, sizeof(poll_q_ent_t));

    poll_ent.handle = handle;

    if (util_list_rm(net->poll_ctx->poll_mtx, &net->poll_ctx->poll_lst_hd, (uint8_t *)&poll_ent, zwpoll_hdl_cmp))
    {
        return ZW_ERR_NONE;
    }
    return ZW_ERR_FAILED;
}


/**
zwpoll_rm_mul - remove multiple polling requests
@param[in]	net	        network
@param[in]	usr_token	usr_token of the polling requests to remove
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwpoll_rm_mul(zwnet_p net, uint32_t usr_token)
{
    zwpoll_ctx_t *poll_ctx = net->poll_ctx;
    poll_q_ent_t *poll_ent;
    util_lst_t   *cur_ent;
    util_lst_t   *prev_ent;

    plt_mtx_lck(poll_ctx->poll_mtx);

    if (poll_ctx->poll_lst_hd == NULL)
    {
        plt_mtx_ulck(poll_ctx->poll_mtx);
        return ZW_ERR_FAILED;
    }

    prev_ent = NULL;
    cur_ent = poll_ctx->poll_lst_hd;

    while (cur_ent)
    {
        poll_ent = (poll_q_ent_t *)cur_ent->wr_buf;

        if (poll_ent->usr_token == usr_token)
        {
            if (prev_ent)
            {
                prev_ent->next = cur_ent->next;
                free(cur_ent);
                cur_ent = prev_ent->next;
            }
            else
            {   //Head of list
                poll_ctx->poll_lst_hd = cur_ent->next;
                free(cur_ent);
                cur_ent = poll_ctx->poll_lst_hd;
            }
        }
        else
        {
            prev_ent = cur_ent;
            cur_ent = cur_ent->next;
        }
    }

    plt_mtx_ulck(poll_ctx->poll_mtx);

    return ZW_ERR_NONE;

}


/**
zwpoll_node_rm - remove polling requests that belong to the specified node
@param[in]	net	        network
@param[in]	node_id	    node id
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwpoll_node_rm(zwnet_p net, uint8_t node_id)
{
    zwpoll_ctx_t *poll_ctx = net->poll_ctx;
    poll_q_ent_t *poll_ent;
    util_lst_t   *cur_ent;
    util_lst_t   *prev_ent;

    if (!net->poll_enable)
    {
        return ZW_ERR_UNSUPPORTED;
    }

    plt_mtx_lck(poll_ctx->poll_mtx);

    if (poll_ctx->poll_lst_hd == NULL)
    {
        plt_mtx_ulck(poll_ctx->poll_mtx);
        return ZW_ERR_FAILED;
    }

    prev_ent = NULL;
    cur_ent = poll_ctx->poll_lst_hd;

    while (cur_ent)
    {
        poll_ent = (poll_q_ent_t *)cur_ent->wr_buf;

        if (poll_ent->node_id == node_id)
        {
            if (prev_ent)
            {
                prev_ent->next = cur_ent->next;
                free(cur_ent);
                cur_ent = prev_ent->next;
            }
            else
            {   //Head of list
                poll_ctx->poll_lst_hd = cur_ent->next;
                free(cur_ent);
                cur_ent = poll_ctx->poll_lst_hd;
            }
        }
        else
        {
            prev_ent = cur_ent;
            cur_ent = cur_ent->next;
        }
    }

    plt_mtx_ulck(poll_ctx->poll_mtx);

    return ZW_ERR_NONE;

}


/**
zwpoll_add - add a polling request into polling queue
@param[in]	    net	        network
@param[in,out]	poll_ent	polling request entry. Handle is returned if o.k.
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwpoll_add(zwnet_p net, poll_q_ent_t *poll_ent)
{
    zwpoll_ctx_t *poll_ctx = net->poll_ctx;
    poll_q_ent_t *new_poll_ent;
    poll_q_ent_t *cur_poll_ent;
    util_lst_t   *cur_lst_ent;
    util_lst_t   *prev_lst_ent;
    util_lst_t   *new_lst_ent;
    uint32_t     poll_ent_sz;
    int          result;

    poll_ent_sz = sizeof(poll_q_ent_t) + poll_ent->dat_len;
    new_lst_ent = (util_lst_t *)malloc(sizeof(util_lst_t) + poll_ent_sz);

    if (!new_lst_ent)
        return ZW_ERR_MEMORY;

    new_lst_ent->dat_sz = poll_ent_sz;
    new_lst_ent->next = NULL;
    memcpy(new_lst_ent->wr_buf, poll_ent, poll_ent_sz);

    new_poll_ent = (poll_q_ent_t *)new_lst_ent->wr_buf;

    result = zwnode_get_rpt(new_poll_ent->dat_buf, new_poll_ent->dat_len, &new_poll_ent->cmd_cls, &new_poll_ent->rpt);

    if (!result)
    {
        free(new_lst_ent);
        return ZW_ERR_RPT_NOT_FOUND;
    }

    plt_mtx_lck(poll_ctx->poll_mtx);

    new_poll_ent->next_poll_tm = poll_ctx->tmr_tick + new_poll_ent->interval;

    poll_ent->handle = new_poll_ent->handle = ++poll_ctx->handle_gen;//TODO: make sure it's unique



    if (poll_ctx->poll_lst_hd == NULL)
    {   //List is empty
        poll_ctx->poll_lst_hd = new_lst_ent;
    }
    else
    {
        //Insert to the list sorted by ascending node id
        prev_lst_ent = NULL;
        cur_lst_ent = poll_ctx->poll_lst_hd;

        while (cur_lst_ent)
        {
            cur_poll_ent = (poll_q_ent_t *)cur_lst_ent->wr_buf;

            if (cur_poll_ent->node_id > new_poll_ent->node_id)
            {
                //Insert new entry as previous entry's "next"
                if (prev_lst_ent == NULL)
                {   //This node id is smallest. Insert as list head
                    new_lst_ent->next = poll_ctx->poll_lst_hd;
                    poll_ctx->poll_lst_hd = new_lst_ent;
                }
                else
                {
                    prev_lst_ent->next = new_lst_ent;
                    new_lst_ent->next = cur_lst_ent;
                }
                break;
            }

            if (!cur_lst_ent->next)
            {   //Current entry is at the end of the list
                cur_lst_ent->next = new_lst_ent;
                break;
            }

            prev_lst_ent = cur_lst_ent;
            cur_lst_ent = cur_lst_ent->next;
        }
    }

    if (poll_ctx->poll_lst_hd->next == NULL)
    {   //There is only one entry in the list
        cur_poll_ent = (poll_q_ent_t *)poll_ctx->poll_lst_hd->wr_buf;

        poll_ctx->next_poll_tm = cur_poll_ent->next_poll_tm;
        poll_ctx->cur_node_id = cur_poll_ent->node_id;
        poll_ctx->cur_handle = cur_poll_ent->handle;
        poll_ctx->cur_cmd_cls = 0;
        poll_ctx->cur_rpt = 0;
        poll_ctx->cur_node_last = 0;
    }

    plt_mtx_ulck(poll_ctx->poll_mtx);

    return ZW_ERR_NONE;

}


/**
zwpoll_init - Initialize the polling facility
@param[in]	poll_ctx	    Polling context
@return  0 on success; negative error number on failure
@note  Must call zwpoll_shutdown followed by zwpoll_exit to shutdown and clean up the polling facility
*/
int zwpoll_init(zwpoll_ctx_t *poll_ctx)
{

    poll_ctx->tmr_tick = 0;
    poll_ctx->next_poll_tm = MIN_POLL_TIME;
    poll_ctx->poll_lst_hd = NULL;
    poll_ctx->handle_gen = 0;
    poll_ctx->cur_node_id = 0;

    if (!plt_mtx_init(&poll_ctx->poll_mtx))
        return ZW_ERR_NO_RES;

    if (!plt_sem_init(&poll_ctx->tmr_sem))
    {
        goto l_POLL_INIT_ERROR1;
    }
    poll_ctx->tick_tmr_ctx = plt_periodic_start(&poll_ctx->net->plt_ctx, POLL_TIMER_TICK, zwpoll_tmr_tick_cb, poll_ctx);
    if (!poll_ctx->tick_tmr_ctx)
        goto l_POLL_INIT_ERROR2;

    //Start timer check thread
    poll_ctx->tmr_chk_thrd_run = 1;
    if (plt_thrd_create(zwpoll_tmr_chk_thrd, poll_ctx) < 0)
    {
        goto l_POLL_INIT_ERROR3;
    }

    return ZW_ERR_NONE;

l_POLL_INIT_ERROR3:
    plt_tmr_stop(&poll_ctx->net->plt_ctx, poll_ctx->tick_tmr_ctx);
l_POLL_INIT_ERROR2:
    plt_sem_destroy(poll_ctx->tmr_sem);
l_POLL_INIT_ERROR1:
    plt_mtx_destroy(poll_ctx->poll_mtx);
    return ZW_ERR_NO_RES;

}


/**
zwpoll_shutdown - Shutdown the polling facility
@param[in]	poll_ctx	    Polling context
@return
*/
void zwpoll_shutdown(zwpoll_ctx_t *poll_ctx)
{
    int wait_count;

    //Stop timer
    plt_tmr_stop(&poll_ctx->net->plt_ctx, poll_ctx->tick_tmr_ctx);

    //Stop thread
    poll_ctx->tmr_chk_thrd_run = 0;
    plt_sem_post(poll_ctx->tmr_sem);

    wait_count = 50;
    while (wait_count-- > 0)
    {
        if (poll_ctx->tmr_chk_thrd_sts == 0)
            break;
        plt_sleep(100);
    }

    plt_sleep(20);  //delay 20ms to give timer thread enough time to terminate and clean up

}


/**
zwpoll_exit - Clean up
@param[in]	poll_ctx	    Polling context
@return
*/
void zwpoll_exit(zwpoll_ctx_t *poll_ctx)
{
    //Flush the poll queue
    util_list_flush(poll_ctx->poll_mtx, &poll_ctx->poll_lst_hd);

    plt_sem_destroy(poll_ctx->tmr_sem);
    plt_mtx_destroy(poll_ctx->poll_mtx);
}


/**
@}
*/















