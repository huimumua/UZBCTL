/**
@file   zw_api_test_intf.c - Z-wave host controller API test program.

        Interfaces (Command Classes) part of the test program.

@author David Chow

@version    1.0 13-4-11  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include "zw_api_test.h"


extern const char *sensor_type_str[];
extern const char *sensor_unit_str[];

/**
hl_int_get - get integer value from a byte stream
@param[in]	byte_buf    The buffer that holds the bytes
@param[in]	size		The integer size
@param[out]	int_val	    The result of the conversion from bytes to integer
@return     1 on success; else return 0
*/
int  hl_int_get(uint8_t *byte_buf,  uint8_t size, int32_t *int_val)
{

    if (size == 1)
    {
        int8_t  val8;

        val8 = byte_buf[0];
        *int_val = val8;
        return 1;
    }
    else if (size == 2)
    {
        int16_t val16;

        val16 = byte_buf[0];
        val16 = (val16 << 8) | byte_buf[1];
        *int_val = val16;
        return 1;
    }
    else
    {
        int32_t val32;

        val32 = ((int32_t)(byte_buf[0])) << 24
                | ((int32_t)(byte_buf[1])) << 16
                | ((int32_t)(byte_buf[2])) << 8
                | byte_buf[3];

        *int_val = val32;
        return 1;
    }
    return 0;

}

/**
hl_float_get - get floating point integer string from an integer with specified precision
@param[in]	int_value    Integer value
@param[in]	precision	 Number of decimal points
@param[in]	buf_size	 Output buffer size
@param[out]	out_buf	     Output buffer that contains the converted string.
@return
*/
void  hl_float_get(int32_t int_value,  uint8_t precision, uint8_t buf_size, char *out_buf)
{

    int i;
    int32_t   divisor;
    int32_t   whole_num;
    int32_t   dec_num;
    char      format_str[20];

    divisor = 1;
    for (i=0 ; i < precision; i++)
    {
        divisor *= 10;
    }

    whole_num = int_value / divisor;

    if (int_value < 0)
    {
        dec_num = (int_value * (-1)) % divisor;
    }
    else
    {
        dec_num = int_value % divisor;
    }

    if (precision > 0)
    {
        sprintf(format_str, "%%d.%%.%ud", precision);
        sprintf(out_buf, format_str, whole_num, dec_num);
    }
    else
    {
        sprintf(out_buf, "%d", int_value);
    }
}


/**
hl_bin2str - convert binary string to hex string
@param[in]  buf      The buffer that stores the binary string
@param[in]  len      The length of the binary string.
@param[out] hex_str  The buffer that stores the output string.
@param[in]  hex_str_len      The length of the output buffer.
@return
*/
void hl_bin2str(void * buf, uint32_t len, char *hex_str, uint32_t hex_str_len)
{
    uint8_t     *bin_byte = (uint8_t *)buf;
    char        tmp[8];

    hex_str[0] = '\0';

    //Convert a line of binary data into hex string
    while (len-- > 0)
    {
        sprintf(tmp,"%02X ",(unsigned) *bin_byte++);
        strcat(hex_str, tmp);
    }
}


/**
hl_stress_test_thrd - stress test thread
@param[in]	data		The high-level api context
@return
*/
void hl_stress_test_thrd(void *data)
{
    hl_appl_ctx_t   *hl_appl = (hl_appl_ctx_t   *)data;
    int32_t         result;

    while (hl_appl->is_to_run_test)
    {
        result = hl_appl->stress_tst_func(hl_appl);
        if (result < 0)
            plt_sleep(2000);//error, retry after 2 seconds
        else if (result == ZW_ERR_QUEUED)
        {
            plt_sleep(300);//command queued, retry after 0.3 seconds
        }
    }
    return;
}


/**
hl_ni_stress_test - toggle between start and stop node update stress test
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_ni_stress_test(hl_appl_ctx_t   *hl_appl)
{
    if (hl_appl->is_ni_stress_tst_run == 0)
    {
        if (!hl_appl->stress_tst_func)
            return;

        if (hl_appl->stress_tst_func == hl_multi_lvl_snd)
        {   //Multi-level switch stress test
            hl_appl->is_to_run_test = 1;
            if (plt_thrd_create(hl_stress_test_thrd, hl_appl) < 0)
            {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Create stress test thread failed");
                return;
            }
        }
        else
        {   //Node info state-machine stress test
            if (hl_node_updt(hl_appl) != 0)
            {
                plt_msg_show(hl_plt_ctx_get(hl_appl), "Stress Test failed to start");
                return;
            }
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test running ...");
        hl_appl->is_ni_stress_tst_run = 1;

    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test stopped");
        hl_appl->is_to_run_test = 0;
        hl_appl->is_ni_stress_tst_run = 0;
    }

}

#ifdef  ZW_STRESS_TEST

/**
hl_test_rpt_show - display the current test report
@param[in]	hl_appl		The high-level api context
@return
*/
void hl_test_rpt_show(hl_appl_ctx_t   *hl_appl)
{
    plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test report:");
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Transmitted multi level switch frames:%u", hl_appl->test_stat.tx_multi_lvl_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Transmitted seq number frames:%u", hl_appl->test_stat.tx_seq_num_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Received seq number frames:%u", hl_appl->test_stat.rx_seq_num_frm);
    plt_msg_show(hl_plt_ctx_get(hl_appl), "Received seq number error:%u", hl_appl->test_stat.rx_seq_num_err);

}


/**
hl_stress_test - toggle between start and stop stress test
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_stress_test(hl_appl_ctx_t   *hl_appl)
{
    if (hl_appl->is_stress_tst_run == 0)
    {
        //if (!hl_appl->stress_tst_func)
        //    return;
        hl_appl->stress_tst_func = hl_seq_num_snd;


        //Reset test statistic
        memset(&hl_appl->test_stat, 0, sizeof(test_stat_t));

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test running ...");
        hl_appl->is_to_run_test = 1;
        hl_appl->is_stress_tst_run = 1;

        if (plt_thrd_create(hl_stress_test_thrd, hl_appl) < 0)
        {
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Create stress test thread failed");
        }
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Test stopped");
        hl_appl->is_to_run_test = 0;
        hl_appl->is_stress_tst_run = 0;
        hl_test_rpt_show(hl_appl);
    }
}
#endif


/**
hl_switch_lvl_gen - genarate multi-level switch level
@param[in]	hl_appl		The high-level api context
@return                 Generated switch-level
*/
static uint8_t  hl_switch_lvl_gen(hl_appl_ctx_t *hl_appl)
{
    static const uint8_t switch_lvl[]={ 0, 0x10, 0x20, 0x40
    };

    if (++hl_appl->lvl > 3)
    {
        hl_appl->lvl = 0;
    }

    return switch_lvl[hl_appl->lvl];
}


/**
hl_multi_lvl_snd - Send multi-level switch level
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_snd(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_set(ifd, hl_switch_lvl_gen(hl_appl), 0);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_snd with error:%d", result);
    }

    //update test statistic
    hl_appl->test_stat.tx_multi_lvl_frm++;

    return result;

}


#ifdef  ZW_STRESS_TEST

/**
hl_seq_num_snd - Send sequence number for testing using COMMAND_CLASS_MANUFACTURER_PROPRIETARY
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_seq_num_snd(hl_appl_ctx_t   *hl_appl)
{
    uint8_t     cmd[8];
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_MANUFACTURER_PROPRIETARY)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Prepare the command to send sequence number
    cmd[0] = COMMAND_CLASS_MANUFACTURER_PROPRIETARY;
    cmd[1] = MANF_ID_1;
    cmd[2] = MANF_ID_2;
    cmd[3] = hl_appl->seq_num >> 24;
    cmd[4] = (hl_appl->seq_num >> 16) & 0xff;
    cmd[5] = (hl_appl->seq_num >> 8) & 0xff;
    cmd[6] = hl_appl->seq_num  & 0xff;

    //Send the command
    result = zwif_exec(ifd, cmd, 7, NULL);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_seq_num_snd with error:%d", result);
    }

    //increment the seq number
    hl_appl->seq_num++;

    //update test statistic
    hl_appl->test_stat.tx_seq_num_frm++;

    return result;
}
#endif


/**
hl_multi_lvl_set - Set multi-level switch level
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_set(ifd, (uint8_t)hl_appl->mul_lvl_val, hl_appl->mul_lvl_dur);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_set with error:%d", result);
    }

    return result;

}


/**
hl_multi_lvl_chg - toggle between start and stop level change
@param[in]	hl_appl		The high-level api context
@return
*/
void    hl_multi_lvl_chg(hl_appl_ctx_t   *hl_appl)
{
    int             result;
    zwifd_p     ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_chg: interface descriptor:%u not found", hl_appl->temp_desc);
        return ;
    }

    if (hl_appl->mul_lvl_change_started == 0)
    {
        zwlevel_t       lvl_ctl;

        lvl_ctl.dur = hl_appl->mul_lvl_dur;
        lvl_ctl.pri_dir = hl_appl->mul_lvl_dir;
        lvl_ctl.pri_level = (uint8_t)hl_appl->mul_lvl_val;
        lvl_ctl.pri_ignore_lvl = (hl_appl->mul_lvl_val == 0xFF)? 1:0;
        lvl_ctl.sec_dir = hl_appl->mul_lvl_sec_dir;
        lvl_ctl.sec_step = hl_appl->mul_lvl_sec_step;

        //Change state to start level change
        result = zwif_level_start(ifd, &lvl_ctl);

        plt_mtx_ulck(hl_appl->desc_cont_mtx);

        if (result != 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_start with error:%d", result);
            return;
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Start level change ...");
        hl_appl->mul_lvl_change_started = 1;

    }
    else
    {
        //Change state to start level change
        result = zwif_level_stop(ifd);

        plt_mtx_ulck(hl_appl->desc_cont_mtx);

        if (result != 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_stop with error:%d", result);
            return;
        }

        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Stop level change ...");
        hl_appl->mul_lvl_change_started = 0;
    }
}


/**
hl_multi_lvl_sup_cb - multi level switch type callback
@param[in]	ifd	interface
@param[in]	pri_type    primary switch type, SW_TYPE_XX
@param[in]	sec_type    secondary switch type , SW_TYPE_XX.
@return
*/
void hl_multi_lvl_sup_cb(zwifd_p ifd,  uint8_t pri_type, uint8_t sec_type)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Primary switch type:%u", pri_type);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Secondary switch type:%u", sec_type);

}


/**
hl_multi_lvl_sup - Get switch type
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_sup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_sup_get(ifd, hl_multi_lvl_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_level_sup_get with error:%d", result);
    }

    return result;
}

/**
hl_ep_nameloc_set - Set name & location of an endpoint
@param[in]	hl_appl		        The high-level api context
@param[in]	ep_desc_id		    Endpoint descriptor id
@return  0 on success, negative error number on failure
@pre        Caller must lock the desc_cont_mtx before calling this function.
*/
int hl_ep_nameloc_set(hl_appl_ctx_t   *hl_appl, uint32_t ep_desc_id)
{
    int32_t     result;
    zwepd_p     epd;

    //Get the endpoint descriptor
    epd = hl_ep_desc_get(hl_appl->desc_cont_hd, ep_desc_id);
    if (!epd)
    {
        return ZW_ERR_EP_NOT_FOUND;
    }

    result = zwep_nameloc_set(epd, &hl_appl->nameloc);

    if (result == 0)
    {
        //Update the endpoint descriptor
#ifdef USE_SAFE_VERSION
        strcpy_s(epd->name, ZW_LOC_STR_MAX + 1, hl_appl->nameloc.name);
        strcpy_s(epd->loc, ZW_LOC_STR_MAX + 1, hl_appl->nameloc.loc);
#else
        strcpy(epd->name, hl_appl->nameloc.name);
        strcpy(epd->loc, hl_appl->nameloc.loc);
#endif
    }
    else
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ep_nameloc_set with error:%d", result);

    }
    return result;

}

/**
hl_nameloc_set - Set name & location of a  node id
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_nameloc_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);

    result = hl_ep_nameloc_set(hl_appl, hl_appl->rep_desc_id);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_nameloc_set with error:%d", result);
    }

    return result;
}


/**
hl_bin_set - Turn binary switch on/off
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_switch_set(ifd, hl_appl->bin_state);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_switch_set with error:%d", result);
    }

    return result;

}


/**
hl_cmd_q_ctl_get - Get the commmand queuing control state
@param[in]	hl_appl		    The high-level api context
@param[out]	q_ctl_state		The commmand queuing control state
@return  0 on success, negative error number on failure
*/
int32_t hl_cmd_q_ctl_get(hl_appl_ctx_t   *hl_appl, uint8_t *q_ctl_state)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_ena_get(noded, q_ctl_state);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_ctl_get with error:%d", result);
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queuing is:%s", (*q_ctl_state)? "on" : "off");
    }

    return result;

}


/**
hl_cmd_q_ctl_set - Set the commmand queuing control state
@param[in]	hl_appl		    The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cmd_q_ctl_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_ena_set(noded, hl_appl->cmd_q_ctl);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_ctl_set with error:%d", result);
    }

    return result;

}


/**
hl_cmd_q_view - View the commmand queue
@param[in]	hl_appl		    The high-level api context
@return
*/
void hl_cmd_q_view(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;
    uint16_t    *cmd_queue;
    uint16_t    i;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return;
    }

    result = zwnode_cmd_q_get(noded, &cmd_queue);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result > 0)
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queue:");
        for (i=0; i<result; i++)
        {
            plt_msg_show(hl_plt_ctx_get(hl_appl), "%u", cmd_queue[i]);
        }
        free(cmd_queue);
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Command queue is empty");
    }
}


/**
hl_cmd_q_cancel - Cancel the commmand queue
@param[in]	hl_appl		    The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_cmd_q_cancel(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwnoded_p   noded;

    //Get the node descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwnode_cmd_q_cancel(noded);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cmd_q_cancel with error:%d", result);
    }
    return result;
}


/**
hl_generic_report_cb - Generic report callback
@param[in]	ifd	The interface that received the report
@param[in]	level	    The reported level
@return
*/
void hl_generic_report_cb(zwifd_p ifd, uint8_t level)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Generic report level:%02Xh", level);
}

/**
hl_multi_lvl_rep_setup - Setup multi-level switch report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_level_rpt_set(ifd, hl_generic_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_multi_lvl_rep_get - Get multi-level switch level report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_multi_lvl_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_level_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_level_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_multi_lvl_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_bin_report_cb - binary switch report callback
@param[in]	ifd	The interface that received the report
@param[in]	on		    0=off, else on
@return
*/
void hl_bin_report_cb(zwifd_p ifd, uint8_t on)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Binary switch is %s", (on)? "on" : "off");
}


/**
hl_bin_rep_setup - Setup binary switch report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_switch_rpt_set(ifd, hl_bin_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_binary_rep_get - Get binary switch state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_binary_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_switch_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_switch_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_binary_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_basic_report_cb - basic command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	value	The value
@return
*/
void hl_basic_report_cb(zwifd_p ifd, uint8_t value)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Basic command value is %02Xh", value);
}


/**
hl_basic_rep_setup - Setup basic command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_basic_rpt_set(ifd, hl_basic_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_basic_rep_get - Get basic command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_basic_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_basic_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_basic_set - basic command set value
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_basic_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_basic_set(ifd, (uint8_t)hl_appl->basic_val);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_basic_set with error:%d", result);
    }

    return result;

}


/**
hl_dlck_op_report_cb - Door lock operation status report callback
@param[in]	ifd	    The interface that received the report
@param[in]	op_sts  Operation status
@return
*/
void hl_dlck_op_report_cb(zwifd_p ifd, zwdlck_op_p  op_sts)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door lock operation mode:%02X,", op_sts->mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Outside door handles mode:%02X,", op_sts->out_mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Inside door handles mode:%02X,", op_sts->in_mode);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door condition:%02X,", op_sts->cond);
    if (op_sts->tmout_min != 0xFE)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Remaining time in unsecured state:%u:%u,",
                     op_sts->tmout_min, op_sts->tmout_sec);
    }

}


/**
hl_dlck_op_rep_setup - Setup door lock operation status report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_op_rpt_set(ifd, hl_dlck_op_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_dlck_op_rep_get - Get the state of the door lock device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_dlck_op_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_dlck_op_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_dlck_op_set - Set door lock operation
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_op_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_op_set(ifd, hl_appl->dlck_mode);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_op_set with error:%d", result);
    }

    return result;

}


/**
hl_dlck_cfg_report_cb - Report callback for door lock configuration
@param[in]	ifd	    interface
@param[in]	config	configuration
*/
void hl_dlck_cfg_report_cb(zwifd_p ifd, zwdlck_cfg_p  config)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Door lock operation type:%s,",
                 (config->type == ZW_DOOR_OP_CONST)? "constant" : "timed");

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Outside door handles state:%02X,", config->out_sta);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Inside door handles state:%02X,", config->in_sta);
    if (config->type == ZW_DOOR_OP_TIMED)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Time the lock stays unsecured.:%u:%u,",
                     config->tmout_min, config->tmout_sec);
    }
}


/**
hl_dlck_cfg_get - Get configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_cfg_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_cfg_get(ifd, hl_dlck_cfg_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_cfg_get with error:%d", result);
    }

    return result;
}


/**
hl_dlck_cfg_set - Set the configuration of the door lock device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_dlck_cfg_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_dlck_cfg_set(ifd, &hl_appl->dlck_config);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_dlck_cfg_set with error:%d", result);
    }

    return result;

}


/**
hl_usrcod_report_cb - Report callback for user code
@param[in]	ifd	        interface
@param[in]	usr_cod     user code and its status
*/
static void hl_usrcod_report_cb(zwifd_p ifd, zwusrcod_p  usr_cod)
{
    static const char *usrid_sts[] =
    {
        "Available (not set)",
        "Occupied",
        "Reserved by administrator",
        "Status unavailable"
    };

    char usr_code[MAX_USRCOD_LENGTH + 1];

    if (usr_cod->id_sts > ZW_USRCOD_RSVD)
    {
        usr_cod->id_sts = 3;
    }

    memcpy(usr_code, usr_cod->code, usr_cod->code_len);
    usr_code[usr_cod->code_len] = '\0';

    plt_msg_show(hl_if_plt_ctx_get(ifd), "User id:%u, status:%s, code:%s", usr_cod->id,
                 usrid_sts[usr_cod->id_sts], usr_code);
}


/**
hl_usrcod_get - Get user code
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_get(ifd, hl_appl->usr_id, hl_usrcod_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_get with error:%d", result);
    }

    return result;
}


/**
hl_usrcod_set - Set user code
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_set(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_set(ifd, &hl_appl->usr_code);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_set with error:%d", result);
    }

    return result;
}


/**
hl_usrcod_sup_cb - Report callback for number of supported user codes
@param[in]	ifd	        interface
@param[in]	usr_num     number of supported user codes
*/
static void hl_usrcod_sup_cb(zwifd_p ifd, uint8_t  usr_num)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max. number of supported user codes:%u", usr_num);

}


/**
hl_usrcod_sup_get - Get number of supported user codes
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_usrcod_sup_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_usrcod_sup_get(ifd, hl_usrcod_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_usrcod_sup_get with error:%d", result);
    }

    return result;
}


static const char *zw_alrm_type[] =
{
    "Unknown alarm",
    "Smoke alarm",
    "Carbon monoxide alarm",
    "Carbon dioxide alarm",
    "Heat alarm",
    "Water alarm",
    "Lock access control alarm",
    "Burglar alarm",
    "Power management alarm",
    "System alarm",
    "Emergency alarm",
    "Alarm clock"
};


/**
hl_alrm_report_cb - Alarm status report callback
@param[in]	ifd	        The interface that received the report
@param[in]	alarm_info  alarm info
@return
*/
void hl_alrm_report_cb(zwifd_p ifd, zwalrm_p  alarm_info)
{

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Vendor specific alarm type:%02Xh, alarm level:%02Xh", alarm_info->type, alarm_info->level);

    if (alarm_info->ex_info)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Zensor net source node id:%u,", alarm_info->ex_zensr_nodeid);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave defined:");
        plt_msg_show(hl_if_plt_ctx_get(ifd), "alarm type:%s, status:%02Xh, event:%02Xh",
                     (alarm_info->ex_type <= ZW_ALRM_CLOCK)? zw_alrm_type[alarm_info->ex_type] : zw_alrm_type[0],
                     alarm_info->ex_status, alarm_info->ex_event);

        if (alarm_info->ex_evt_len)
        {
            switch (alarm_info->ex_evt_type)
            {
                case ZW_ALRM_PARAM_LOC:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "alarm location:%s", alarm_info->ex_evt_prm);
                    break;

                case ZW_ALRM_PARAM_USRID:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "user id:%u", alarm_info->ex_evt_prm[0]);
                    break;

                case ZW_ALRM_PARAM_OEM_ERR_CODE:
                    plt_msg_show(hl_if_plt_ctx_get(ifd), "OEM proprietary system failure code:");
                    plt_bin_show(hl_if_plt_ctx_get(ifd), alarm_info->ex_evt_prm, alarm_info->ex_evt_len);
                    break;
            }
        }
    }
}


/**
hl_alrm_rep_setup - Setup alarm status report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_rpt_set(ifd, hl_alrm_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_alrm_rep_get - Get the state of the alarm device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_alrm_get_poll(ifd, hl_appl->alrm_vtype, hl_appl->alrm_ztype, hl_appl->alrm_evt, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_alrm_get(ifd, hl_appl->alrm_vtype, hl_appl->alrm_ztype, hl_appl->alrm_evt);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_alrm_set - Set the activity of the Z-Wave Alarm Type
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_set(ifd, hl_appl->alrm_ztype, hl_appl->alrm_sts);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_set with error:%d", result);
    }

    return result;

}


/**
hl_alrm_sup_report_cb - Report callback for supported alarm types
@param[in]	ifd	        interface
@param[in]	have_vtype  flag to indicate whether vendor specific alarm type supported. 1=supported; else 0=unsupported
@param[in]	ztype_len   size of ztype buffer
@param[in]	ztype       buffer to store supported Z-wave alarm types (ZW_ALRM_XXX)
*/
void hl_alrm_sup_report_cb(zwifd_p ifd, uint8_t have_vtype, uint8_t ztype_len, uint8_t *ztype)
{

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Vendor specific alarm type is %s", (have_vtype)? "supported" : "unsupported");
    if (ztype_len > 0)
    {
        int i;
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave alarm types:");
        for (i=0; i<ztype_len; i++)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", (ztype[i] <= ZW_ALRM_CLOCK)? zw_alrm_type[ztype[i]] : zw_alrm_type[0]);
        }

    }
}


/**
hl_alrm_sup_get - Get supported alarm types
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_sup_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_sup_get(ifd, hl_alrm_sup_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_alrm_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_alrm_sup_evt_report_cb - Report callback for supported alarm events
@param[in]	ifd	        interface
@param[in]	ztype       Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	evt_len     size of evt buffer
@param[in]	evt         buffer to store supported event of the alarm type specified in ztype
*/
void hl_alrm_sup_evt_report_cb(zwifd_p ifd, uint8_t ztype, uint8_t evt_len, uint8_t *evt)
{

    if (evt_len > 0)
    {
        int i;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave alarm type = %s, supported events:",
                     (ztype <= ZW_ALRM_CLOCK)? zw_alrm_type[ztype] : zw_alrm_type[0]);

        for (i=0; i<evt_len; i++)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "%02Xh, ", evt[i]);
        }
    }
}

/**
hl_alrm_sup_evt_get - Get supported alarm events
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_alrm_sup_evt_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_alrm_sup_evt_get(ifd, hl_appl->alrm_ztype, hl_alrm_sup_evt_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_alrm_sup_evt_get with error:%d", result);
    }

    return result;
}


/**
hl_bin_snsr_rep_cb - binary sensor report callback
@param[in]	ifd	The interface that received the report
@param[in]	state	    The state of the sensor: 0=idle, else event detected
@return
*/
void hl_bin_snsr_rep_cb(zwifd_p ifd, uint8_t state)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Binary sensor state :%s", (state == 0)? "idle" : "event detected");
}

/**
hl_bin_snsr_rep_setup - Setup binary sensor report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_snsr_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_bsensor_rpt_set(ifd, hl_bin_snsr_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_bin_snsr_rep_get - Get binary sensor state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_bin_snsr_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_bsensor_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_bsensor_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_ml_snsr_rep_cb - multi-level sensor report callback
@param[in]	ifd	interface
@param[in]	value		The current value and unit of the sensor.
*/
static void hl_ml_snsr_rep_cb(zwifd_p ifd, zwsensor_t *value)
{

    printf("linhui, sensor multi-level report, type=%02x\n",value->type);
    int32_t         snsr_value;
    const char      **unit_str;

    if (value->type > ZW_SENSOR_TYPE_TGT_TEMP)
    {
        value->type = 0;
    }

    unit_str = sensor_unit_str + (value->type * 4) + value->unit;

    if (*unit_str == NULL)
    {
        *unit_str = "undefined";
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Multi-level sensor type:%s, precision:%u, unit:%s",
                    sensor_type_str[value->type], value->precision, *unit_str);

    if (!hl_int_get(value->data, value->size, &snsr_value))
    {   //Error, default to zero
        snsr_value = 0;
    }

    if (value->precision == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Sensor reading:%d", snsr_value);
    }
    else
    {
        char    float_str[80];

        hl_float_get(snsr_value, value->precision, 80, float_str);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Sensor reading:%s", float_str);

    }
}


/**
hl_ml_snsr_rep_setup - Setup multi-level sensor report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_ml_snsr_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    printf("linhui Setup multi-level sensor report, id %d\n", hl_appl->rep_desc_id);
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        printf("linhui report setup, interface not found\n");
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_rpt_set(ifd, hl_ml_snsr_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_bin_snsr_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_ml_snsr_rep_get - Get multi-level sensor state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_ml_snsr_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_sensor_get_poll(ifd, hl_appl->sensor_type, hl_appl->sensor_unit, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_sensor_get(ifd, hl_appl->sensor_type, hl_appl->sensor_unit);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_snsr_sup_report_cb - Report callback for supported sensor types
@param[in]	ifd	        interface
@param[in]	type_len    size of sensor type buffer
@param[in]	type        buffer to store supported sensor types (ZW_SENSOR_TYPE_XXX)
*/
static void hl_snsr_sup_report_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type)
{
    if (type_len > 0)
    {
        int i;
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave sensor types:");
        for (i=0; i<type_len; i++)
        {
            if (type[i] > ZW_SENSOR_TYPE_TGT_TEMP)
            {
                type[i] = 0;
            }

            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", sensor_type_str[type[i]]);
        }
    }
}


/**
hl_ml_snsr_sup_get - Get supported sensor types through report callback
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ml_snsr_sup_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_sup_get(ifd, hl_snsr_sup_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_snsr_unit_report_cb - Report callback for supported sensor units
@param[in]	ifd	            interface
@param[in]	sensor_type     sensor type, ZW_SENSOR_TYPE_XXX
@param[in]	sensor_unit_msk bitmask of units supported for the sensor_type, ZW_SENSOR_UNIT_XXX
*/
static void hl_snsr_unit_report_cb(zwifd_p ifd, uint8_t sensor_type, uint8_t sensor_unit_msk)
{
    int             i;
    const char      **unit_str;

    if (sensor_type > ZW_SENSOR_TYPE_TGT_TEMP)
    {
        sensor_type = 0;
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Z-wave sensor type:%s, supported units:", sensor_type_str[sensor_type]);

    for (i=0; i<4; i++)
    {
        if (sensor_unit_msk & (0x01 << i))
        {
            unit_str = sensor_unit_str + (sensor_type * 4) + i;

            if (*unit_str == NULL)
            {
                *unit_str = "undefined";
            }

            plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", *unit_str);
        }
    }
}


/**
hl_ml_snsr_unit_get - Get supported sensor units through report callback
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ml_snsr_unit_get(hl_appl_ctx_t *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_sensor_unit_get(ifd, hl_appl->sensor_type, hl_snsr_unit_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ml_snsr_unit_get with error:%d", result);
    }

    return result;
}


/**
hl_wkup_rep_cb - wake up notification callback
@param[in]	ifd	interface
@param[in]	cap	capabilities report, null for notification
@return	Only apply to notification: 0=no command pending to send; 1=commands pending to send.
*/
int hl_wkup_rep_cb(zwifd_p ifd, zwif_wakeup_p cap)
{
    if (!cap)
    {   //Notification
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Wake up notification from node:%u", ifd->nodeid);
        //Nothing to send, tell the node to sleep again
        //return 0;

        //Assume user has something to send
        return 1;
    }
    else
    {   //Capabilities report
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Wake up settings:");
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Alert receiving node: %u", cap->node.nodeid);
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Current interval: %u s", cap->cur);
        if (cap->min == 0)
        {
            return 0;
        }
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Min: %u s, Max: %u s", cap->min, cap->max);
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Default: %u s, Step: %u s", cap->def, cap->interval);

    }
    return 0;
}


/**
hl_wkup_get - Get wake up setting
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_wkup_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_wakeup_get(ifd, hl_wkup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_wkup_get with error:%d", result);
    }

    return result;
}


/**
hl_wkup_set - Set wake up interval and alert receiving node
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_wkup_set(hl_appl_ctx_t   *hl_appl)
{
    int             result;
    zwifd_p     ifd;
    zwnoded_p   noded;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->node_desc_id);
    if (!noded && hl_appl->node_desc_id)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwif_wakeup_set(ifd, hl_appl->wkup_interval, noded);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_wkup_set with error:%d", result);
    }

    return result;
}


/**
hl_grp_rep_cb - Group info report callback
@param[in]	ifd	Interface
@param[in]	group	    Grouping identifier
@param[in]	max_cnt	    Maximum number of end points the grouping identifier above supports
@param[in]	cnt	        The number of end points in the grouping in this report
@param[in]	ep	        An array of cnt end points in the grouping
@return
*/
void hl_grp_rep_cb(zwifd_p ifd, uint8_t group, uint8_t max_cnt, uint8_t cnt, zwepd_p ep)
{
    int i;
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Group id:%u, max supported endpoints:%u, Group members:", group, max_cnt);
    for (i=0; i<cnt; i++)
    {
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Node id:%u, endpoint id:%u", ep[i].nodeid, ep[i].epid);
    }

}

/**
hl_grp_rep_get - Get group info report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_get(ifd, hl_appl->group_id, hl_grp_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_add - Add endpoints into group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_add(hl_appl_ctx_t   *hl_appl)
{
    int         i;
    int         result;
    uint8_t     ep_cnt;
    zwifd_p ifd;
    zwepd_t ep_desc[5];
    zwepd_p ep;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    ep_cnt = 0;
    for (i=0; i<5; i++)
    {
        ep = hl_ep_desc_get(hl_appl->desc_cont_hd, hl_appl->ep_desc_id[i]);
        if (ep)
        {
            ep_desc[ep_cnt++] = *ep;
        }
    }

    result = zwif_group_add(ifd, hl_appl->group_id, ep_desc, ep_cnt);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_add with error:%d", result);
    }

    return result;
}


/**
hl_grp_del - Delete endpoints from group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_del(hl_appl_ctx_t   *hl_appl)
{
    int         i;
    int         result;
    uint8_t     ep_cnt;
    zwifd_p ifd;
    zwepd_t ep_desc[5];
    zwepd_p ep;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    ep_cnt = 0;
    for (i=0; i<5; i++)
    {
        ep = hl_ep_desc_get(hl_appl->desc_cont_hd, hl_appl->ep_desc_id[i]);
        if (ep)
        {
            ep_desc[ep_cnt++] = *ep;
        }
    }

    result = zwif_group_del(ifd, hl_appl->group_id, ep_desc, ep_cnt);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_grp_del with error:%d", result);
    }

    return result;
}


/**
hl_grp_sup_cb - max number of groupings callback
@param[in]	ifd	      interface
@param[in]	max_grp   maximum number of groupings
@return
*/
void hl_grp_sup_cb(zwifd_p ifd,  uint8_t max_grp)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max number of groupings:%u", max_grp);
}


/**
hl_grp_sup - Get max number of groupings
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_sup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_sup_get(ifd, hl_grp_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_active_cb - active group callback
@param[in]	ifd	    interface
@param[in]	group   current active group
@return
*/
void hl_grp_active_cb(zwifd_p ifd,  uint8_t group)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Current active group:%u", group);
}


/**
hl_grp_active - Get active group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_active(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_actv_get(ifd, hl_grp_active_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_actv_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_sup_cb - Get cmd records supported callback
@param[in]	ifd	      interface
@param[in]	cmd_cap   command records supporting capabilities
@return
*/
void hl_grp_cmd_sup_cb(zwifd_p ifd, zwgrp_cmd_cap_p  cmd_cap)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Command %s configurable", (cmd_cap->configurable)? "is" : "is not");
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Support %s command", (cmd_cap->config_type)? "basic set" : "any");
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max command length: %u", cmd_cap->max_len);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Number of free command records: %u", cmd_cap->free_rec);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Total number of command records supported: %u", cmd_cap->total_rec);
}


/**
hl_grp_cmd_sup - Get cmd records supported
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_sup(hl_appl_ctx_t   *hl_appl)

{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_cmd_sup_get(ifd, hl_grp_cmd_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_set - Set cmd to node in group
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    zwnoded_p   noded;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->node_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    result = zwif_group_cmd_set(ifd, hl_appl->group_id, noded, hl_appl->command, hl_appl->cmd_len);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_set with error:%d", result);
    }

    return result;
}


/**
hl_grp_cmd_get_cb - Get command record for a node within a given grouping identifier callback
@param[in]	ifd	    interface
@param[in]	group	grouping identifier
@param[in]	nodeid	node id of the node within the grouping specified
@param[in]	cmd_buf	command and parameters
@param[in]	len     length of cmd_buf
*/
void hl_grp_cmd_get_cb(zwifd_p ifd, uint8_t group, uint8_t nodeid, uint8_t *cmd_buf, uint8_t len)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Command record for group id:%u, node id:%u :", group, nodeid);
    plt_bin_show(hl_if_plt_ctx_get(ifd), cmd_buf, len);
}


/**
hl_grp_cmd_get - Get command record for a node within a given grouping identifier
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_grp_cmd_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_group_cmd_get(ifd, hl_appl->group_id, hl_appl->node_id, hl_grp_cmd_get_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_set with error:%d", result);
    }

    return result;
}


/**
hl_cfg_set - Set configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    zwconfig_t  param;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check for invalid size
    switch (hl_appl->cfg_size)
    {
        case 1:
            param.data[0] = hl_appl->cfg_value & 0xFF;
            break;

        case 2:
            param.data[0] = (hl_appl->cfg_value >> 8) & 0xFF;
            param.data[1] = hl_appl->cfg_value & 0xFF;
            break;

        case 4:
            param.data[0] = (hl_appl->cfg_value >> 24) & 0xFF;
            param.data[1] = (hl_appl->cfg_value >> 16) & 0xFF;
            param.data[2] = (hl_appl->cfg_value >> 8) & 0xFF;
            param.data[3] = hl_appl->cfg_value & 0xFF;
            break;

        default:
            plt_msg_show(hl_plt_ctx_get(hl_appl), "Invalid config param size:%u", hl_appl->cfg_size);
            return ZW_ERR_VALUE;
    }

    param.param_num = hl_appl->cfg_param;
    param.size = hl_appl->cfg_size;
    param.use_default = hl_appl->cfg_value_default;

    result = zwif_config_set(ifd, &param);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_set with error:%d", result);
    }

    return result;
}


/**
hl_cfg_report_cb - Get configuration parameter callback
@param[in]	ifd	    interface
@param[in]	param   parameter value
*/
void hl_cfg_report_cb(zwifd_p ifd, zwconfig_p param)
{
    int32_t         param_value;
    zwnetd_p        net_desc;
    hl_appl_ctx_t   *hl_appl;

    if (!hl_int_get(param->data, param->size, &param_value))
    {   //Error, default to zero
        param_value = 0;
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Configuration parameter:%u, value:%d", param->param_num, param_value);

    //Check whether to get another report
    net_desc = zwnet_get_desc(ifd->net);

    hl_appl = (hl_appl_ctx_t *)net_desc->user;

    if (hl_appl->cfg_param_mode == 1)
    {
        if (param->param_num < hl_appl->cfg_range_end)
        {
            zwif_config_get(ifd, param->param_num + 1);
        }
    }
}


/**
hl_cfg_rep_setup - Setup a configuration parameter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);

    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_config_rpt_set(ifd, hl_cfg_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_cfg_get - Get configuration parameter
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_cfg_get(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p     ifd;
    uint8_t     param_num;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Check whether to get single or a range of parameter value
    if (hl_appl->cfg_param_mode == 0)
    {   //Single
        param_num = hl_appl->cfg_param;
    }
    else
    {
        //Multiple
        if (hl_appl->cfg_range_start > hl_appl->cfg_range_end)
        {
            hl_appl->cfg_range_end = hl_appl->cfg_range_start;
        }
        param_num = (uint8_t)hl_appl->cfg_range_start;

    }

    result = zwif_config_get(ifd, param_num);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_cfg_get with error:%d", result);
    }

    return result;
}


/**
hl_pulsemeter_rep_cb - pulse meter report callback
@param[in]	ifd	    The interface that received the report
@param[in]	cnt	    The number of pulses detected in the device
@return
*/
void hl_pulsemeter_rep_cb(zwifd_p ifd, uint32_t cnt)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Number of pulses detected :%u", cnt);
}

/**
hl_pulsemeter_rep_setup - Setup pulse meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_pulsemeter_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_pulsemeter_rpt_set(ifd, hl_pulsemeter_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_pulsemeter_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_pulsemeter_rep_get - Get number of pulses detected report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_pulsemeter_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_pulsemeter_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_pulsemeter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_rep_cb - meter report callback
@param[in]	ifd	    The interface that received the report
@param[in]	data    current value and unit of the meter
@return
*/
static void hl_meter_rep_cb(zwifd_p ifd, zwmeter_dat_p value)
{
    static const char *meter_type[] = { "unknown", "electric", "gas", "water"
    };
    static const char *meter_rate[] = { "unknown", "import", "export"
    };
    int32_t meter_value;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter type:%s, precision:%u, unit:%u, rate type:%s",
                 meter_type[value->type], value->precision, value->unit,
                 meter_rate[value->rate_type]);

    if (!hl_int_get(value->data, value->size, &meter_value))
    {   //Error, default to zero
        meter_value = 0;
    }

    if (value->precision == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter reading:%d", meter_value);
    }
    else
    {
        char    float_str[80];

        hl_float_get(meter_value, value->precision, 80, float_str);

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Meter reading:%s", float_str);

    }

    //Check if to display previous reading
    if (value->delta_time > 0)
    {
        if (!hl_int_get(value->prv_data, value->size, &meter_value))
        {   //Error, default to zero
            meter_value = 0;
        }

        if (value->precision == 0)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "Previous Meter reading:%d, taken %us ago", meter_value, value->delta_time);
        }
        else
        {
            char    float_str[80];

            hl_float_get(meter_value, value->precision, 80, float_str);

            plt_msg_show(hl_if_plt_ctx_get(ifd), "Previous Meter reading:%s, taken %us ago", float_str, value->delta_time);

        }
    }


}

/**
hl_meter_rep_setup - Setup meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_rpt_set(ifd, hl_meter_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_meter_rep_get - Get meter report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_rep_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_meter_get_poll(ifd, hl_appl->meter_unit, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_meter_get(ifd, hl_appl->meter_unit);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_sup_cb - report callback for meter capabilities
@param[in]	ifd	        interface
@param[in]	meter_cap	meter capabilities
*/
void hl_meter_sup_cb(zwifd_p ifd, zwmeter_cap_p meter_cap)
{
    static const char *meter_type[] = { "unknown", "electric", "gas", "water"
    };
    plt_msg_show(hl_if_plt_ctx_get(ifd), "meter type:%s, meter supported unit bit-mask:%02xh", meter_type[meter_cap->type], meter_cap->unit_sup);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "meter %s be reset", (meter_cap->reset_cap)? "can" : "can not");

}

/**
hl_meter_sup - get information on the meter capabilities
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_sup(hl_appl_ctx_t   *hl_appl)

{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_sup_get(ifd, hl_meter_sup_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "zwif_group_cmd_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_meter_reset - Reset all accumulated values stored in the meter device
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_reset(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_reset(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_reset with error:%d", result);
    }

    return result;
}


/**
hl_meter_admin_cb - meter admin name and capability callback
@param[in]	ifd	    The interface
@param[in]	meter	Meter descriptor
@return
*/
static void hl_meter_admin_cb(zwifd_p ifd, zwmeter_p meter)
{
    if (meter->caps & ZW_METER_CAP_MON)
    {
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Meter admin name:%s, id:%s",
                        meter->admin, meter->id);
    }

    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Meter admin name %s be set",
                    (meter->caps & ZW_METER_CAP_CFG)? "can" : "cannot");
}


/**
hl_meter_admin_set - Set meter admin name
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_admin_set(hl_appl_ctx_t   *hl_appl)
{
    int         result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_set_admin(ifd, hl_appl->meter_admin);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_admin_set with error:%d", result);
    }

    return result;

}


/**
hl_meter_admin_cap_get - Get meter admin name and capability
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_meter_admin_cap_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_meter_get_desc(ifd, hl_meter_admin_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_meter_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_av_get_cb - Get supported AV commands callback
@param[in]	ifd	    interface
@param[in]	length	mask byte length
@param[in]	mask	mask buffer pointer
@return
*/
void hl_av_get_cb(zwifd_p ifd, uint16_t length, uint8_t *mask)
{
    uint16_t    i;
    char    num_str[56];

    num_str[0] = '\0';

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported AV commands:");
    for (i = 0; i < (length*8); i++)
    {
        if ((mask[i>>3] >> (i & 7)) & 0x01)
        {
            util_num_string_add(num_str, 56, (unsigned)(i+1));
            if (strlen(num_str) > 50)
            {
                plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", num_str);
                num_str[0] = '\0';
            }
        }
    }
    plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", num_str);
}


/**
hl_av_get - Get supported AV commands
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_av_get(hl_appl_ctx_t   *hl_appl)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_av_caps(ifd, hl_av_get_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result < 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_av_get with error:%d", result);
    }

    return result;
}


/**
hl_av_set - Send the status of AV button
@param[in]	hl_appl		The high-level api context
@param[in]	ctl		    button number ZW_BUTTON_XX
@param[in]	down	    0=button up, else button down
@return  0 on success, negative error number on failure
*/
int32_t hl_av_set(hl_appl_ctx_t   *hl_appl, uint16_t ctl, uint8_t down)
{
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_av_set(ifd, ctl, down);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_av_set with error:%d", result);
    }

    return result;
}


/**
hl_battery_report_cb - battery command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	value	The value
@return
*/
void hl_battery_report_cb(zwifd_p ifd, uint8_t value)
{
    if (/*(value >= 0) &&*/ (value <= 100))
    {
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Battery level is %u%%", value);
    }
    else if (value == 0xFF)
    {
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Battery low warning!");
    }
}


/**
hl_battery_rep_setup - Setup battery command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_battery_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    printf("linhui, battery report setup...\n");
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        printf("linhui, battery interface not found\n");
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result = zwif_battery_rpt_set(ifd, hl_battery_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_battery_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_battery_rep_get - Get battery command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int32_t hl_battery_rep_get(hl_appl_ctx_t   *hl_appl)
{
    //hl_appl->rep_desc_id = prompt_uint("Enter desc id of the report:");
    printf("battery report get,hl_appl->dst_desc_id= %d",hl_appl->dst_desc_id);
    int     result;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->dst_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    if (hl_appl->poll_ctl)
    {
        result = zwif_battery_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_battery_get(ifd);
        printf("linhui, zwif_battery_get called,result=%d\n",result);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_battery_rep_get with error:%d", result);
    }

    return result;
}



