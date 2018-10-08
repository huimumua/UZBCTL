/**
@file   zw_api_intf.c - Z-wave High Level API interface specific implementation.


@author David Chow

@version    1.0 28-7-10  Initial release

version: 1.0
comments: Initial release
*/
#include <ctype.h>
#include "../include/zw_api_pte.h"
#include "../include/zw_api_util.h"
#include "../include/zw_poll.h"

static int zwif_get_report_poll(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd,
                                zwpoll_req_t *poll_req);


static const uint8_t data_storage_sz[] = {1, 2, 4};

static const uint8_t switch_dir[] = {0, 1, 3};

static const uint8_t usr_id_sts[] = {0, 1, 2, 0xFE};

static const uint8_t dlck_op_mode[] =
    {   ZW_DOOR_UNSEC, ZW_DOOR_UNSEC_TMOUT, ZW_DOOR_UNSEC_IN,
        ZW_DOOR_UNSEC_IN_TMOUT, ZW_DOOR_UNSEC_OUT,
        ZW_DOOR_UNSEC_OUT_TMOUT, ZW_DOOR_SEC
    };


/**
zwif_cmd_id_set - Set the command id for the command queue
@param[in]	ifd         Interface descriptor
@param[in]	cmd_id	    Command id ZW_CID_XXX
@param[in]	cmd_num	    The n th command class command of the interface API; start from 1, ...
return      ZW_ERR_XXX
*/
static int zwif_cmd_id_set(zwifd_p ifd, uint16_t cmd_id, uint8_t cmd_num)
{
    zwnode_p node;
    plt_mtx_lck(ifd->net->mtx);
    node = zwnode_find(&ifd->net->ctl, ifd->nodeid);
    if (node)
    {
        node->cmd_id = cmd_id;
        node->cmd_num = cmd_num;
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NONE;
    }
    plt_mtx_ulck(ifd->net->mtx);
    return ZW_ERR_NODE_NOT_FOUND;
}


/**
zwif_cached_cb - Create a callback request to the callback thread
@param[in]	ifd         Interface descriptor
@param[in]	cb	        Report callback function
@param[in]	rpt_type 	CB_RPT_TYP_XXX
@param[in]	extra 	    extra parameter
@return     Non-zero if success; else return zero
*/
static int zwif_cached_cb(zwifd_p ifd, void *cb, uint16_t rpt_type, uint16_t extra)
{
    zwnet_p         nw = ifd->net;
    zwif_p          intf;
    zwnet_cb_req_t  cb_req;

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);
    if (intf && intf->data_cnt)
    {
        plt_mtx_ulck(nw->mtx);
        //Send request to callback thread
        cb_req.ifd = *ifd;
        cb_req.cb = cb;
        cb_req.rpt_type = rpt_type;
        cb_req.extra = extra;

        util_list_add(nw->cb_mtx, &nw->cb_req_hd,
                      (uint8_t *)&cb_req, sizeof(zwnet_cb_req_t));
        plt_sem_post(nw->cb_sem);

        return 1;
    }
    plt_mtx_ulck(nw->mtx);
    return 0;
}


/**
@defgroup If_Loc Location Interface APIs
Clients can assign descriptive name and location strings
Their state can be read back by the generic zwif_get_report.
@{
*/


/**
zwif_nameloc_set - Set node name and location string
@param[in]	ifd	        Interface
@param[in]	nameloc	    Name & location string null terminated
@return	ZW_ERR_XXX
*/
int zwif_nameloc_set(zwifd_p ifd, const zw_nameloc_p nameloc)
{
    int         result;
    uint8_t     str_len;    //Character string length
    uint8_t     cmd[3 + ZW_DEV_LOC_STR_MAX];
    char        tmp_str[ZW_DEV_LOC_STR_MAX + 1];

    //Check whether the interface belongs to command class COMMAND_CLASS_NODE_NAMING
    if (ifd->cls != COMMAND_CLASS_NODE_NAMING)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Prepare the command to set name
    cmd[0] = COMMAND_CLASS_NODE_NAMING;
    cmd[1] = NODE_NAMING_NODE_NAME_SET;
    cmd[2] = 0;//default to using standard ASCII codes

    //Check for valid UTF-8 string
    memcpy(tmp_str, nameloc->name, ZW_DEV_LOC_STR_MAX);
    tmp_str[ZW_DEV_LOC_STR_MAX] = '\0';
    str_len = strlen(tmp_str);
    str_len = plt_utf8_chk((const uint8_t *)tmp_str, str_len);

    memcpy(cmd + 3, tmp_str, str_len);

    result = zwif_cmd_id_set(ifd, ZW_CID_NAME_LOC_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Send the command
    result = zwif_exec(ifd, cmd, 3 + str_len, zwif_exec_cb);
    if (result < 0)
    {
        return result;
    }

    //Prepare the command to set location
    //cmd[0] = COMMAND_CLASS_NODE_NAMING;
    cmd[1] = NODE_NAMING_NODE_LOCATION_SET;
    //cmd[2] = 0;//default to using standard ASCII codes

    //Check for valid UTF-8 string
    memcpy(tmp_str, nameloc->loc, ZW_DEV_LOC_STR_MAX);
    tmp_str[ZW_DEV_LOC_STR_MAX] = '\0';
    str_len = strlen(tmp_str);
    str_len = plt_utf8_chk((const uint8_t *)tmp_str, str_len);

    memcpy(cmd + 3, tmp_str, str_len);

    result = zwif_cmd_id_set(ifd, ZW_CID_NAME_LOC_SET, 2);
    if ( result < 0)
    {
        return result;
    }

    //Send the command
    result = zwif_exec(ifd, cmd, 3 + str_len, zwif_exec_cb);
    if (result < 0)
    {
        return result;
    }

    return result;

}


/**
@}
@defgroup BSns Binary Sensor Interface APIs
Binary sensors state can be idle (no event) or event detected
Their state can be read back by the generic zwif_get_report.
@{
*/

/**
zwif_bsensor_rpt_set - Setup a binary sensor report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_bsensor_rpt_set(zwifd_p ifd, zwrep_bsensor_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SENSOR_BINARY)
    {
        return zwif_set_report(ifd, rpt_cb, SENSOR_BINARY_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_bsensor_get_ex - get binary sensor report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_bsensor_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SENSOR_BINARY)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0, SENSOR_BINARY_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_BSENSOR_RPT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0, SENSOR_BINARY_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_bsensor_get - get binary sensor report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_bsensor_get(zwifd_p ifd)
{
    return zwif_bsensor_get_ex(ifd, NULL);
}


/**
zwif_bsensor_get_poll - get binary sensor report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_bsensor_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_bsensor_get_ex(ifd, poll_req);
}


/**
@}
@defgroup Sns Sensor Interface APIs
Multi-level sensors value can be 1, 2 or 4 bytes signed number
Their value can be read back by the generic zwif_get_report.
@{
*/


/**
zwif_sensor_rpt_set - Setup a multilevel sensor report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_sensor_rpt_set(zwifd_p ifd, zwrep_sensor_fn rpt_cb)
{
    printf("linhui zwif_sensor_rpt_set, setcallback");
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SENSOR_MULTILEVEL)
    {
        return zwif_set_report(ifd, rpt_cb, SENSOR_MULTILEVEL_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_sensor_get_ex - get multilevel sensor report through report callback
@param[in]	ifd	        interface
@param[in]	type	    preferred sensor type, ZW_SENSOR_TYPE_XXX. If type equals to zero, the
                        sensor report will return the factory default sensor type.
@param[in]	unit	    preferred sensor unit, ZW_SENSOR_UNIT_XXX.  This parameter is ignored
                        if type=0.
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
@note  Preferred sensor type and unit are not guaranteed to be returned in the report callback.  It
       depends on the multilevel sensor command class version number and the device supported.
*/
static int zwif_sensor_get_ex(zwifd_p ifd, uint8_t type, uint8_t unit, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SENSOR_MULTILEVEL)
    {
        int result;

        if (!poll_req)
        {
            result = zwif_cmd_id_set(ifd, ZW_CID_SENSOR_RPT_GET, 1);
            if (result < 0)
            {
                return result;
            }
        }

        //Check version to determine whether to send parameter 'type' and 'unit' as
        //they are only valid for version 5 and above
        if ((ifd->ver < 5) || (type == 0))
        {
            if (poll_req)
            {
                return zwif_get_report_poll(ifd, NULL, 0,
                                            SENSOR_MULTILEVEL_GET, poll_req);
            }
            else
            {
                return zwif_get_report(ifd, NULL, 0,
                                       SENSOR_MULTILEVEL_GET, zwif_exec_cb);

            }
        }
        else
        {
            uint8_t param[2];

            if (unit > 3)
            {
                return ZW_ERR_VALUE;
            }

            param[0] = type;
            param[1] = (unit & 0x07) << 3;
            if (poll_req)
            {
                return zwif_get_report_poll(ifd, param, 2,
                                            SENSOR_MULTILEVEL_GET, poll_req);
            }
            else
            {
                return zwif_get_report(ifd, param, 2,
                                       SENSOR_MULTILEVEL_GET, zwif_exec_cb);

            }

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_sensor_get - get multilevel sensor report through report callback
@param[in]	ifd	        interface
@param[in]	type	    preferred sensor type, ZW_SENSOR_TYPE_XXX. If type equals to zero, the
                        sensor report will return the factory default sensor type.
@param[in]	unit	    preferred sensor unit, ZW_SENSOR_UNIT_XXX.  This parameter is ignored
                        if type=0.
@return		ZW_ERR_XXX
@note  Preferred sensor type and unit are not guaranteed to be returned in the report callback.  It
       depends on the multilevel sensor command class version number and the device supported.
*/
int zwif_sensor_get(zwifd_p ifd, uint8_t type, uint8_t unit)
{
    return zwif_sensor_get_ex(ifd, type, unit, NULL);
}


/**
zwif_sensor_get_poll - get multilevel sensor report through report callback
@param[in]	ifd	        interface
@param[in]	type	    preferred sensor type, ZW_SENSOR_TYPE_XXX. If type equals to zero, the
                        sensor report will return the factory default sensor type.
@param[in]	unit	    preferred sensor unit, ZW_SENSOR_UNIT_XXX.  This parameter is ignored
                        if type=0.
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
@note  Preferred sensor type and unit are not guaranteed to be returned in the report callback.  It
       depends on the multilevel sensor command class version number and the device supported.
*/
int zwif_sensor_get_poll(zwifd_p ifd, uint8_t type, uint8_t unit, zwpoll_req_t *poll_req)
{
    return zwif_sensor_get_ex(ifd, type, unit, poll_req);
}


/**
zwif_sensor_sup_get - get the supported sensor types through report callback
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_sensor_sup_get(zwifd_p ifd, zwrep_sensor_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SENSOR_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 5 and above
    if (ifd->ver < 5)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_SENSOR_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5, zwif_exec_cb);
    }
    return result;

}


/**
zwif_sensor_unit_get - get the supported sensor units through report callback
@param[in]	ifd	            interface
@param[in]	sensor_type	    sensor type
@param[in]	cb	            report callback function
@return ZW_ERR_XXX
*/
int zwif_sensor_unit_get(zwifd_p ifd, uint8_t sensor_type, zwrep_sensor_unit_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SENSOR_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 5 and above
    if (ifd->ver < 5)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check value
    if (sensor_type == 0)
    {
        return ZW_ERR_VALUE;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_SENSOR_UNIT_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, &sensor_type, 1,
                                 SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5, zwif_exec_cb);
    }
    return result;

}


/**
zwif_sensor_sup_cache_get - get supported sensor types and units from cache
@param[in]	ifd	        interface
@param[out]	sup_snsr	pointer to array of supported sensors
@param[out]	snsr_cnt	supported sensor counts
@return ZW_ERR_XXX
*/
int zwif_sensor_sup_cache_get(zwifd_p ifd, if_sensor_data_t **sup_snsr, uint8_t *snsr_cnt)
{
    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SENSOR_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if ((ifd->data_cnt == 0) || (!ifd->data))
    {
        return ZW_ERR_INTF_NO_DATA;
    }

    *sup_snsr = (if_sensor_data_t *)ifd->data;
    *snsr_cnt = ifd->data_cnt;

    return 0;

}


/**
@}
@defgroup If_Mtr Meter Interface APIs
Meters can be reset
@{
*/


/**
zwif_meter_rpt_set - Setup a meter report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_meter_rpt_set(zwifd_p ifd, zwrep_meter_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_METER)
    {
        return zwif_set_report(ifd, rpt_cb, METER_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_meter_get_ex - Get meter report through report callback
@param[in]	ifd	        Interface
@param[in]	unit	    the preferred unit (ZW_METER_UNIT_XXX). The report may not return
                        the preferred unit if the device doesn't support it.
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_meter_get_ex(zwifd_p ifd, uint8_t unit, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_METER)
    {
        int         result;

        if (!poll_req)
        {
            result = zwif_cmd_id_set(ifd, ZW_CID_METER_RPT_GET, 1);
            if (result < 0)
            {
                return result;
            }
        }

        //Check version to determine whether to send parameter 'unit' as
        //it is only valid for version 2 and above
        if (ifd->ver < 2)
        {
            if (poll_req)
            {
                return zwif_get_report_poll(ifd, NULL, 0, METER_GET, poll_req);
            }
            else
            {
                return zwif_get_report(ifd, NULL, 0, METER_GET, zwif_exec_cb);
            }
        }
        else
        {
            uint8_t param;

            param = (unit & 0x07) << 3;
            if (poll_req)
            {
                return zwif_get_report_poll(ifd, &param, 1, METER_GET, poll_req);
            }
            else
            {
                return zwif_get_report(ifd, &param, 1, METER_GET, zwif_exec_cb);
            }

        }

    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_meter_get - Get meter report through report callback
@param[in]	ifd	        Interface
@param[in]	unit	    the preferred unit (ZW_METER_UNIT_XXX). The report may not return
                        the preferred unit if the device doesn't support it.
@return		ZW_ERR_XXX
*/
int zwif_meter_get(zwifd_p ifd, uint8_t unit)
{
    return zwif_meter_get_ex(ifd, unit, NULL);
}


/**
zwif_meter_get_poll - Get meter report through report callback
@param[in]	ifd	        Interface
@param[in]	unit	    the preferred unit (ZW_METER_UNIT_XXX). The report may not return
                        the preferred unit if the device doesn't support it.
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_meter_get_poll(zwifd_p ifd, uint8_t unit, zwpoll_req_t *poll_req)
{
    return zwif_meter_get_ex(ifd, unit, poll_req);
}


/**
zwif_meter_sup_get - get information on the meter capabilities through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_meter_sup_get(zwifd_p ifd,  zwrep_meter_sup_fn cb)
{
    int         result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_METER)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_METER, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, METER_SUPPORTED_REPORT_V2);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_METER_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 METER_SUPPORTED_GET_V2, zwif_exec_cb);
    }
    return result;

}

/**
zwif_meter_reset - reset all accumulated values stored in the meter device
@param[in]	ifd	interface
@return	ZW_ERR_XXX
*/
int zwif_meter_reset(zwifd_p ifd)
{
    int         result;
    uint8_t     cmd[2];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_METER)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_METER_RESET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_METER_V2;
    cmd[1] = METER_RESET_V2;

    //Send the command
    return zwif_exec(ifd, cmd, 2, zwif_exec_cb);

}


/**
zwif_meter_set_admin - set meter admin name
@param[in]	ifd	interface
@param[in]	name	admin number
@return	ZW_ERR_XXX
*/
int zwif_meter_set_admin(zwifd_p ifd, char *name)
{
    int         result;
    uint8_t     str_len;    //Character string length
    uint8_t     cmd[3 + ZW_ADMIN_STR_MAX];

    //Check whether the command class is correct
    if (ifd->cls != COMMAND_CLASS_METER_TBL_CONFIG)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_METER_ADMIN_SET, 1);
    if ( result < 0)
    {
        return result;
    }
    //Truncate the admin number string if it is too long
    str_len = strlen(name);
    if (str_len > ZW_ADMIN_STR_MAX)
    {
        str_len = ZW_ADMIN_STR_MAX;
    }


    //Prepare the command to set name
    cmd[0] = COMMAND_CLASS_METER_TBL_CONFIG;
    cmd[1] = METER_TBL_TABLE_POINT_ADM_NO_SET;
    cmd[2] = str_len;
    memcpy(cmd + 3, name, str_len);

    //Send the command
    return zwif_exec(ifd, cmd, 3 + str_len, zwif_exec_cb);
}


/**
zwif_meter_get_desc - get meter admin name
@param[in]	ifd	interface
@param[in]	cb	meter descriptor report callback function.
@return	ZW_ERR_XXX
*/
int zwif_meter_get_desc(zwifd_p ifd, zwrep_meterd_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_METER_TBL_MONITOR)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_METER_DESC, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, METER_TBL_TABLE_ID_REPORT);
    if (result != 0)
    {
        return result;
    }

    zwif_set_report(ifd, cb, METER_TBL_TABLE_POINT_ADM_NO_REPORT);

    result = zwif_cmd_id_set(ifd, ZW_CID_METER_DESC_GET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Request for report
    return zwif_get_report(ifd, NULL, 0,
                           METER_TBL_TABLE_ID_GET, zwif_exec_cb);
}


/**
@}
@defgroup If_PlsMtr Pulse Meter Interface APIs
Intended for all kinds of meters that generate pulses.
@{
*/


/**
zwif_pulsemeter_rpt_set - setup a pulse meter report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_pulsemeter_rpt_set(zwifd_p ifd, zwrep_pulsemeter_fn rpt_cb)
{
    if (ifd->cls == COMMAND_CLASS_METER_PULSE)
    {
        return zwif_set_report(ifd, rpt_cb, METER_PULSE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}

/**
zwif_pulsemeter_get - get pulse meter report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_pulsemeter_get(zwifd_p ifd)
{
    if (ifd->cls == COMMAND_CLASS_METER_PULSE)
    {
        int         result;

        result = zwif_cmd_id_set(ifd, ZW_CID_PULSE_METER_RPT_GET, 1);
        if ( result < 0)
        {
            return result;
        }
        return zwif_get_report(ifd, NULL, 0,
                               METER_PULSE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
@}
@defgroup If_Av Audio-Video Interface APIs
Audio Video Interface is meant to replace TV/player remotes.
@{
*/


/**
zwif_av_tmout_cb - "key hold" keep alive timeout callback
@param[in] data     Pointer to network
@return
*/
static void    zwif_av_tmout_cb(void *data)
{
    int       result;
    zwnet_p   nw = (zwnet_p)data;

    //Stop the timer
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->av_key_hold.tmr_ctx);
    nw->av_key_hold.tmr_ctx = NULL;

    //Send keep alive
    nw->av_key_hold.cmd[2] = nw->av_seq_num++;
    plt_mtx_ulck(nw->mtx);

    //Send the command
    result = zwif_exec(&nw->av_key_hold.ifd, nw->av_key_hold.cmd, 8, zwif_exec_cb);

    if (result == 0)
    {
        //Prepare for sending of next "key hold" keep alive
        plt_mtx_lck(nw->mtx);
        if (nw->av_key_hold.run_tmr)
        {
            nw->av_key_hold.tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_AV_KEEP_ALIVE_TMOUT, zwif_av_tmout_cb, nw);
        }
        plt_mtx_ulck(nw->mtx);
    }
}


/**
zwif_av_set - send the status of AV button
@param[in]	ifd	    interface
@param[in]	ctl		button number ZW_BUTTON_XX
@param[in]	down	0=button up, else button down
@return	ZW_ERR_XXX
*/
int zwif_av_set(zwifd_p ifd, uint16_t ctl, uint8_t down)
{
    int                 result;
    uint8_t             cmd[8];
    zwnet_p             nw;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SIMPLE_AV_CONTROL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_AV_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_SIMPLE_AV_CONTROL;
    cmd[1] = SIMPLE_AV_CONTROL_SET;
    plt_mtx_lck(ifd->net->mtx);
    cmd[2] = ifd->net->av_seq_num++;
    plt_mtx_ulck(ifd->net->mtx);
    cmd[3] = (down)? 0 : 1;
    cmd[4] = 0; //Item ID MSB
    cmd[5] = 0; //Item ID LSB
    cmd[6] = ctl >> 8; //Command MSB,1
    cmd[7] = ctl & 0xFF; //Command LSB,1

    //Stop timer if it is active
    nw = ifd->net;
    plt_mtx_lck(nw->mtx);
    plt_tmr_stop(&nw->plt_ctx, nw->av_key_hold.tmr_ctx);
    nw->av_key_hold.tmr_ctx = NULL;
    nw->av_key_hold.run_tmr = 0;
    plt_mtx_ulck(nw->mtx);


    //Send the command
    result = zwif_exec(ifd, cmd, 8, zwif_exec_cb);

    if (result == 0)
    {
        //Prepare for sending of "key hold" keep alive
        if (down)
        {
            plt_mtx_lck(nw->mtx);
            nw->av_key_hold.ifd = *ifd;
            memcpy(nw->av_key_hold.cmd, cmd, 8);
            nw->av_key_hold.cmd[3] = 0x02; //keep alive command
            nw->av_key_hold.run_tmr = 1;
            nw->av_key_hold.tmr_ctx = plt_tmr_start(&nw->plt_ctx, ZWNET_AV_KEEP_ALIVE_TMOUT, zwif_av_tmout_cb, nw);
            plt_mtx_ulck(nw->mtx);

        }
    }

    return result;

}


/**
zwif_av_caps - Get supported AV commands
@param[in]	ifd	interface
@param[in]	cb	av capabilities report callback function.
@return	ZW_ERR_XXX
*/
int zwif_av_caps (zwifd_p ifd, zwrep_av_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_SIMPLE_AV_CONTROL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_AV, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, SIMPLE_AV_CONTROL_REPORT);
    if (result != 0)
    {
        return result;
    }

    result = zwif_set_report(ifd, cb, SIMPLE_AV_CONTROL_SUPPORTED_REPORT);
    if (result != 0)
    {
        return result;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_AV_CAP_GET, 1);
    if ( result < 0)
    {
        return result;
    }
    //Request for report
    return zwif_get_report(ifd, NULL, 0, SIMPLE_AV_CONTROL_GET, zwif_exec_cb);
}

/**
@}
@defgroup If_Cfg    Device Configuration APIs
Configure device specific parameters
@{
*/


/**
zwif_config_set - Set configuration parameter
@param[in]	ifd	    interface
@param[in]	param	parameter to set.
@return	ZW_ERR_XXX
*/
int zwif_config_set(zwifd_p ifd, zwconfig_p param)
{
    int         result;
    uint8_t     *cmd;
    uint8_t     cmd_len;    //the length of the command and parameters

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_CONFIGURATION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (memchr(data_storage_sz, param->size, sizeof(data_storage_sz)) == NULL)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_CONFIG_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    cmd_len = 4 + param->size;
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_CONFIGURATION;
    cmd[1] = CONFIGURATION_SET;
    cmd[2] = param->param_num;
    cmd[3] = (param->size & 0x07) | ((param->use_default & 0x01) << 7);
    memcpy(cmd + 4, param->data, param->size);


    //Send the command
    result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);

    free(cmd);
    return result;
}


/**
zwif_config_rpt_set - Setup a configuration parameter report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_config_rpt_set(zwifd_p ifd, zwrep_config_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CONFIGURATION)
    {
        return zwif_set_report(ifd, rpt_cb, CONFIGURATION_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_config_get - Get configuration parameter report through report callback
@param[in]	ifd	        interface
@param[in]	param_num	parameter number of the value to get
@return		ZW_ERR_XXX
*/
int zwif_config_get(zwifd_p ifd, uint8_t param_num)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_CONFIGURATION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_CONFIG_RPT_GET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Request for report
    result = zwif_get_report(ifd, &param_num, 1,
                             CONFIGURATION_GET, zwif_exec_cb);
    return result;
}


/**
@}
@defgroup If_Lvl Level Interface APIs
Levels can be set and auto-(in/de)cremented with start/stop and on completion will call zwnet::notify
Their state can be read back by zwif_get_report.
@{
*/

/**
zwif_level_set - set level
@param[in]	ifd		interface
@param[in]	v		0=off, 0xFF=on(previous level), 1-99=%
@param[in]	dur	    Dimming duration.  0=instantly;  0x01 to 0x7F = 1 second (0x01) to 127 seconds (0x7F);
                    0x80 to 0xFE = 1 minute (0x80) to 127 minutes (0xFE); 0xFF = factory default rate.
@return	ZW_ERR_XXX
*/
int zwif_level_set(zwifd_p ifd, uint8_t v,  uint8_t dur)
{
    uint8_t     cmd[4];
    int         result;


    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SWITCH_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (!((/*v >= 0  &&*/ v <= 99)
          || v == 0xFF))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_LEVEL_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command to set multilevel switch
    cmd[0] = COMMAND_CLASS_SWITCH_MULTILEVEL;
    cmd[1] = SWITCH_MULTILEVEL_SET;
    cmd[2] = v;

    //Check for version 2 and above
    if (ifd->ver >= 2)
    {
        cmd[3] = dur;
    }
    //Send the command
    return zwif_exec(ifd, cmd, (ifd->ver >= 2)? 4 : 3, zwif_exec_cb);

}


/**
zwif_level_start - start modifying levels
@param[in]	ifd	        interface
@param[in]	level_ctrl	level control of switches
@return	ZW_ERR_XXX
*/
int zwif_level_start(zwifd_p ifd, zwlevel_p  level_ctrl)
{
    uint8_t     cmd[6];
    uint8_t     cmd_len;
    int         result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SWITCH_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (level_ctrl->pri_ignore_lvl)
    {
        level_ctrl->pri_level = 0;
    }

    if (!(/*level_ctrl->pri_level >= 0  &&*/ level_ctrl->pri_level <= 99))
    {
        return ZW_ERR_VALUE;
    }

    //Prepare the command to start changing level
    cmd[0] = COMMAND_CLASS_SWITCH_MULTILEVEL;
    cmd[1] = SWITCH_MULTILEVEL_START_LEVEL_CHANGE;

    cmd[3] = level_ctrl->pri_level;//primary switch start level
    cmd[4] = level_ctrl->dur;   //dimming duration in seconds which is the interval it takes to dim from level 0 to 99
    cmd[5] = level_ctrl->sec_step;//secondary switch step size

    //Check for version 1 and 2
    if (ifd->ver <= 2)
    {
        //check whether the direction is valid
        if (level_ctrl->pri_dir > 1)
        {
            return ZW_ERR_VALUE;
        }
        cmd[2] = ((level_ctrl->pri_ignore_lvl & 0x01) << 5) | (level_ctrl->pri_dir << 6);
        cmd_len = (ifd->ver == 2)? 5 : 4;
    }
    else
    {
        //Check input values
        if (!((/*level_ctrl->sec_step >= 0  &&*/ level_ctrl->sec_step <= 99)
              || level_ctrl->sec_step == 0xFF))
        {
            return ZW_ERR_VALUE;
        }

        if (memchr(switch_dir, level_ctrl->pri_dir, sizeof(switch_dir)) == NULL)
        {
            return ZW_ERR_VALUE;
        }

        if (memchr(switch_dir, level_ctrl->sec_dir, sizeof(switch_dir)) == NULL)
        {
            return ZW_ERR_VALUE;
        }

        cmd[2] = ((level_ctrl->pri_ignore_lvl & 0x01) << 5)
                | ((level_ctrl->pri_dir & 0x03) << 6)
                | ((level_ctrl->sec_dir & 0x03) << 3);

        //If the Increment/Decrement field is set to 3, the Step Size field must be set to 0.
        if (level_ctrl->sec_dir == 3)
        {
            cmd[5] = 0;
        }

        cmd_len = 6;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_LEVEL_START, 1);
    if ( result < 0)
    {
        return result;
    }

    //Send the command
    return zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);
}


/**
zwif_level_stop - stop modifying level
                  used to arrive at optimum settings for dimmer, volume etc
@param[in]	ifd	interface
@return	ZW_ERR_XXX
*/
int zwif_level_stop(zwifd_p ifd)
{
    uint8_t     cmd[2];
    int         result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SWITCH_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_LEVEL_STOP, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command to stop changing level
    cmd[0] = COMMAND_CLASS_SWITCH_MULTILEVEL;
    cmd[1] = SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE;

    //Send the command
    return zwif_exec(ifd, cmd, 2, zwif_exec_cb);
}


/**
zwif_level_rpt_set - Setup a level report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_level_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SWITCH_MULTILEVEL)
    {
        return zwif_set_report(ifd, rpt_cb, SWITCH_MULTILEVEL_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_level_get_ex - get level report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_level_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SWITCH_MULTILEVEL)
    {

        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   SWITCH_MULTILEVEL_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_LEVEL_RPT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   SWITCH_MULTILEVEL_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_level_get - get level report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_level_get(zwifd_p ifd)
{
    return zwif_level_get_ex(ifd, NULL);
}


/**
zwif_level_get_poll - get level report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_level_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_level_get_ex(ifd, poll_req);
}


/**
zwif_level_sup_get - get a switch type report through report callback
@param[in]	ifd	        interface
@param[in]	cb	        callback function to receive the supported switch type report
@return		ZW_ERR_XXX
*/
int zwif_level_sup_get(zwifd_p ifd, zwrep_lvl_sup_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_SWITCH_MULTILEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_MUL_SWITCH, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3);
    if (result != 0)
    {
        return result;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_LEVEL_SUP_GET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Request for report
    return zwif_get_report(ifd, NULL, 0, SWITCH_MULTILEVEL_SUPPORTED_GET_V3, zwif_exec_cb);
}

/**
@}
@defgroup If_Swt Switch Interface APIs
Switches can be switched on/off and on completion will call zwnet::notify
Their state can be read back by the generic zwif_get_report.
@{
*/

/**
zwif_switch_set - turn on/off switch
@param[in]	ifd	interface
@param[in]	on		0=off, 1=on
@return	ZW_ERR_XXX
*/
int zwif_switch_set(zwifd_p ifd, uint8_t on)
{
    uint8_t     cmd[3];
    int         result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_SWITCH_BINARY)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_SWITCH_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command to set binary switch
    cmd[0] = COMMAND_CLASS_SWITCH_BINARY;
    cmd[1] = SWITCH_BINARY_SET;
    cmd[2] = (on)? 0xFF : 0;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
zwif_switch_rpt_set - Setup a switch report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_switch_rpt_set(zwifd_p ifd, zwrep_switch_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SWITCH_BINARY)
    {
        return zwif_set_report(ifd, rpt_cb, SWITCH_BINARY_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_switch_get_ex - get switch report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_switch_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_SWITCH_BINARY)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   SWITCH_BINARY_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_SWITCH_RPT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   SWITCH_BINARY_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_switch_get - get switch report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_switch_get(zwifd_p ifd)
{
    return zwif_switch_get_ex(ifd, NULL);
}


/**
zwif_switch_get_poll - get switch report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_switch_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_switch_get_ex(ifd, poll_req);
}


/**
@}
@defgroup If_Grp Group Interface APIs
Groups are application-specific, and normally specify report recipients eg. for a sensor
@{
*/

/**
zwif_group_get - get information on specified group of interface through report callback
@param[in]		ifd	interface
@param[in]		group	    group id
@param[in]		cb	        report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_get(zwifd_p ifd, uint8_t group, zwrep_group_fn cb)
{
    int         result;
    zwifd_t     if_desc;

    if_desc = *ifd;

    if ((if_desc.cls != COMMAND_CLASS_ASSOCIATION) &&
        (if_desc.cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (group == 0)
    {
        return ZW_ERR_VALUE;
    }

    //Setup report callback
    result = zwif_set_report(&if_desc, cb, ASSOCIATION_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_GRP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(&if_desc, &group, 1,
                                 ASSOCIATION_GET, zwif_exec_cb);
    }
    return result;

}


/**
zwif_grp_cb - group return route callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
void zwif_grp_cb(appl_layer_ctx_t *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwnet_p     nw = (zwnet_p)appl_ctx->data;
    extern const char *zwif_tx_cmplt_sts[];

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        int         i;
        int         result;
        zwnode_p    node;
        unsigned    node_id = (unsigned)user_prm;
        debug_zwapi_msg(appl_ctx->plt_ctx, "Grp return route completed successfully");

        plt_mtx_lck(nw->mtx);
        node = zwnode_find(&nw->ctl, node_id);

        if (!node)
        {
            debug_zwapi_msg(appl_ctx->plt_ctx, "zwif_grp_cb:invalid node id:%u", node_id);
            plt_mtx_ulck(nw->mtx);
            return;
        }

        if (node->add_grp_rr.num_ent > 0)
        {
            if (node->add_grp_rr.rd_pos < node->add_grp_rr.num_ent)
            {
                i = node->add_grp_rr.rd_pos++;
                result = zw_assign_return_route(appl_ctx,
                                                node_id,
                                                node->add_grp_rr.dst_node[i],
                                                zwif_grp_cb, (void *)node_id);
                if (result < 0)
                {
                    debug_zwapi_msg(appl_ctx->plt_ctx, "zw_assign_return_route with error:%d", result);
                }
            }
        }
        plt_mtx_ulck(nw->mtx);
    }
    else
    {
        debug_zwapi_msg(appl_ctx->plt_ctx, "zw_assign_return_route completed with error:%s",
                        (tx_sts < 6)?
                        zwif_tx_cmplt_sts[tx_sts]  : "unknown");
    }
}


/**
zwif_group_mc_add - add endpoint to specified group of interface, and return route if possible
@note       It is recommended to call this function once with all the endpoints to be added
            to enable the return routes being setup correctly.
@param[in]	ifd	interface
@param[in]	group	    group id
@param[in]	ep	        An array of cnt recipient end points to be added into the grouping
@param[in]	cnt	        The number of end points in the array ep
@return  0 on success, else ZW_ERR_XXX
*/
static int zwif_group_mc_add(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt)
{
    int         i;
    int         j;
    int         result;
    unsigned    num_node;       //Number of nodes without real endpoints
    unsigned    num_node_ep;    //Number of nodes with real endpoints
    zwnode_p    node;
    ret_route_t *ret_route;
    uint8_t     *cmd;
    uint8_t     cmd_len;        //the length of the command and parameters
    uint8_t     rr_cnt;

    //Calculate number of nodes with/without real endpoints
    num_node = num_node_ep = 0;
    for (i = 0; i < cnt; i++)
    {
        if (ep[i].epid == 0)
        {
            num_node++;
        }
        else
        {
            num_node_ep++;
        }
    }

    //Allocate memory
    cmd_len = 4 + num_node + (num_node_ep * 2);
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2;
    cmd[1] = MULTI_CHANNEL_ASSOCIATION_SET_V2;
    cmd[2] = group;

    //Fill in the node without endpoints first
    for (i = 0, j = 0; i < cnt; i++)
    {
        if (ep[i].epid == 0)
        {
            cmd[3+j] = ep[i].nodeid;
            j++;
        }
    }

    //Fill in marker
    cmd[3 + num_node] = MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2;

    //Fill in the node with endpoints
    for (i = 0, j = 0; i < cnt; i++)
    {
        if (ep[i].epid > 0)
        {
            cmd[4 + num_node + j] = ep[i].nodeid;
            j++;
            cmd[4 + num_node + j] = ep[i].epid;
            j++;
        }
    }

    plt_mtx_lck(ifd->net->mtx);
    node = zwnode_find(&ifd->net->ctl, ifd->nodeid);

    if (!node)
    {
        free(cmd);
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    //Assign return routes (up to 4)
    rr_cnt = (cnt > 4)? 4 : cnt;

    //Store the return route assignment info
    node->add_grp_rr.rd_pos = 0;
    memset(node->add_grp_rr.dst_node, 0, 4);

    for (i = 0, j = 0; i < cnt; i++)
    {
        //Make sure the node id is not repeated
        if (memchr(node->add_grp_rr.dst_node, ep[i].nodeid, 4) == NULL)
        {
            //Make sure the destination node is different from the source node
            if (ep[i].nodeid != ifd->nodeid)
            {
                node->add_grp_rr.dst_node[j++] = ep[i].nodeid;
                if (j >= rr_cnt)
                {
                    break;
                }
            }
        }
    }

    if (j == 0)
    {   //No return route to set
        plt_mtx_ulck(ifd->net->mtx);

        //Send the command
        result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);
        free(cmd);
        return result;
    }

    node->add_grp_rr.num_ent = j;//actual number of return route

    //Return route should be invoked after the association set command has completed,
    //else it will interrupt the security messages nonce get, nonce report, security encapsulation sequence
    //if the association command class interface is secure.
    ret_route = (ret_route_t *)malloc(sizeof(ret_route_t));
    if (ret_route)
    {
        cmd_q_xtra_t    xtra;
        util_lst_t      *xtra_lst_hd = NULL;

        *ret_route = node->add_grp_rr;

        xtra.cmd_id = ZW_CID_GRP_ADD;
        xtra.node_id = node->nodeid;
        xtra.extra = ret_route;

        result = util_list_add(ifd->net->mtx, &xtra_lst_hd, (uint8_t *)&xtra, sizeof(cmd_q_xtra_t));

        if (result == 0)
        {
            result = zwif_exec_ex(ifd, cmd, cmd_len, zwif_cmd_q_cb, xtra_lst_hd, ZWIF_OPT_Q_EXTRA, xtra_lst_hd);

            if (result < 0)
            {   //Free xtra_lst_hd
                zwif_cmd_q_xtra_rm(ifd->net, &xtra_lst_hd);
            }
            else
            {   //No callback for the return route
                result = ZW_ERR_QUEUED;
            }
        }
        else
        {
            free(ret_route);
        }
    }
    else
    {
        result = ZW_ERR_MEMORY;
    }
    free(cmd);
    plt_mtx_ulck(ifd->net->mtx);
    return result;
}


/**
zwif_group_add - add endpoint to specified group of interface, and return route if possible
@note       It is recommended to call this function once with all the endpoints to be added
            to enable the return routes being setup correctly.
@param[in]	ifd	interface
@param[in]	group	    group id
@param[in]	ep	        An array of cnt recipient end points to be added into the grouping
@param[in]	cnt	        The number of end points in the array ep
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_add(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt)
{
    int         i;
    int         j;
    int         result;
    int         mul_ch_assoc_present;   //Flag to indicate whether Multi Channel Association CC presents.
                                        //0=absent; 1=present (in ifd); 2=present (in mul_ch_assoc_ifd).
    zwnode_p    node;
    ret_route_t *ret_route;
    uint8_t     *cmd;
    uint8_t     cmd_len;    //the length of the command and parameters
    zwifd_t     mul_ch_assoc_ifd;

    if (cnt == 0)
    {
        return 0;
    }


    //Check whether the interface belongs to the right command class
    if ((ifd->cls != COMMAND_CLASS_ASSOCIATION) &&
        (ifd->cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (group == 0)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_GRP_ADD, 1);
    if (result < 0)
    {
        return result;
    }

    //
    //All Controller Role types (CSC, SSC, PC, RPC) MUST use the Multi Channel Association CC with
    //Endpoint ID = 1 asserted when performing the association sets. This MUST be done regardless of
    //whether the Controller is a Multi Channel device or not.
    //

    //Check whether the endpoint supports Multi Channel Association CC
    if (ifd->cls == COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2)
    {
        mul_ch_assoc_present = 1;
    }
    else if (zwif_ifd_get(ifd, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, &mul_ch_assoc_ifd) == 0)
    {
        mul_ch_assoc_present = 2;
    }
    else
    {
        mul_ch_assoc_present = 0;
    }

    //Check for controller node id
    if (mul_ch_assoc_present)
    {
        for (i = 0; i < cnt; i++)
        {
            if (ep[i].nodeid == ifd->net->ctl.nodeid)
            {   //Found controller
                //Set to endpoint 1
                ep[i].epid = 1;
            }
        }
    }

    //Check whether there is any real endpoint
    for (i = 0; i < cnt; i++)
    {
        if (ep[i].epid > 0)
        {
            if (mul_ch_assoc_present == 0)
            {
                return ZW_ERR_WRONG_IF;
            }
            //Call the multi-channel version of this function
            if (mul_ch_assoc_present == 2)
            {
                ifd = &mul_ch_assoc_ifd;
            }
            return zwif_group_mc_add(ifd, group, ep, cnt);
        }
    }


    cmd_len = 3 + cnt;
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    //Prepare the command
    cmd[0] = (uint8_t)ifd->cls;
    cmd[1] = ASSOCIATION_SET;
    cmd[2] = group;
    printf("count= %d\n",cnt);
    for (i = 0; i < cnt; i++)
    {
        cmd[3+i] = ep[i].nodeid;
    }

    printf("add group cmd[3]:%d \n", cmd[3]);

    plt_mtx_lck(ifd->net->mtx);
    node = zwnode_find(&ifd->net->ctl, ifd->nodeid);

    if (!node)
    {
        free(cmd);
        plt_mtx_ulck(ifd->net->mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

    //Assign return routes (up to 4)
    if (cnt > 4)
    {
        cnt = 4;
    }

    //Store the return route assignment info
    node->add_grp_rr.num_ent = cnt;
    node->add_grp_rr.rd_pos = 0;


    for (i = 0, j = 0; i < cnt; i++)
    {
        //Make sure the destination node is different from the source node
        if (cmd[3+i] != ifd->nodeid)
        {
            node->add_grp_rr.dst_node[j++] = cmd[3+i];
        }
    }

    printf("go here j= %d\n",j);

    if (j == 0)
    {   //No return route to set
        plt_mtx_ulck(ifd->net->mtx);

        //Send the command
        result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);
        free(cmd);
        return result;
    }
    node->add_grp_rr.num_ent = j;//actual number of return route

    printf("go here ??????\n");

    //Return route should be invoked after the association set command has completed,
    //else it will interrupt the security messages nonce get, nonce report, security encapsulation sequence
    //if the association command class interface is secure.
    ret_route = (ret_route_t *)malloc(sizeof(ret_route_t));
    if (ret_route)
    {
        cmd_q_xtra_t    xtra;
        util_lst_t      *xtra_lst_hd = NULL;

        *ret_route = node->add_grp_rr;

        xtra.cmd_id = ZW_CID_GRP_ADD;
        xtra.node_id = node->nodeid;
        xtra.extra = ret_route;

        result = util_list_add(ifd->net->mtx, &xtra_lst_hd, (uint8_t *)&xtra, sizeof(cmd_q_xtra_t));

        if (result == 0)
        {
            result = zwif_exec_ex(ifd, cmd, cmd_len, zwif_cmd_q_cb, xtra_lst_hd, ZWIF_OPT_Q_EXTRA, xtra_lst_hd);

            if (result < 0)
            {   //Free xtra_lst_hd
                zwif_cmd_q_xtra_rm(ifd->net, &xtra_lst_hd);
            }
            else
            {   //No callback for the return route
                printf("here ...................\n");
                result = ZW_ERR_QUEUED;
            }
        }
        else
        {
            free(ret_route);
        }
    }
    else
    {
        result = ZW_ERR_MEMORY;
    }
    free(cmd);
    plt_mtx_ulck(ifd->net->mtx);
    return result;

}


/**
zwif_group_mc_del - remove endpoint from specified group of interface
@param[in]	ifd	interface
@param[in]	group	    group id
@param[in]	ep	        An array of cnt recipient end points to be removed from the grouping
@param[in]	cnt	        The number of end points in the array ep
@return  0 on success, else ZW_ERR_XXX
*/
static int zwif_group_mc_del(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt)
{
    int         i;
    int         j;
    int         result;
    unsigned    num_node;       //Number of nodes without real endpoints
    unsigned    num_node_ep;    //Number of nodes with real endpoints
    uint8_t     *cmd;
    uint8_t     cmd_len;        //the length of the command and parameters

    //Calculate number of nodes with/without real endpoints
    num_node = num_node_ep = 0;
    for (i = 0; i < cnt; i++)
    {
        if (ep[i].epid == 0)
        {
            num_node++;
        }
        else
        {
            num_node_ep++;
        }
    }

    //Allocate memory
    cmd_len = 4 + num_node + (num_node_ep * 2);
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2;
    cmd[1] = MULTI_CHANNEL_ASSOCIATION_REMOVE_V2;
    cmd[2] = group;

    //Fill in the node without endpoints first
    for (i = 0, j = 0; i < cnt; i++)
    {
        if (ep[i].epid == 0)
        {
            cmd[3+j] = ep[i].nodeid;
            j++;
        }
    }

    //Fill in marker
    cmd[3 + num_node] = MULTI_CHANNEL_ASSOCIATION_REMOVE_MARKER_V2;

    //Fill in the node with endpoints
    for (i = 0, j = 0; i < cnt; i++)
    {
        if (ep[i].epid > 0)
        {
            cmd[4 + num_node + j] = ep[i].nodeid;
            j++;
            cmd[4 + num_node + j] = ep[i].epid;
            j++;
        }
    }

    //Send the command
    result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);

    free(cmd);
    return result;

}


/**
zwif_group_del - remove endpoint from specified group of interface
@param[in]	ifd	interface
@param[in]	group	    group id
@param[in]	ep	        An array of cnt recipient end points to be removed from the grouping
@param[in]	cnt	        The number of end points in the array ep. If cnt is zero,
                        all the endpoints in the group will be removed.
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_del(zwifd_p ifd, uint8_t group, zwepd_p ep, uint8_t cnt)
{
    int         i;
    int         result;
    int         mul_ch_assoc_present;   //Flag to indicate whether Multi Channel Association CC presents.
                                        //0=absent; 1=present (in ifd); 2=present (in mul_ch_assoc_ifd).
    zwifd_t     mul_ch_assoc_ifd;
    uint8_t     *cmd;
    uint8_t     cmd_len;    //the length of the command and parameters


    //Check whether the interface belongs to the right command class
    if ((ifd->cls != COMMAND_CLASS_ASSOCIATION) &&
        (ifd->cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (group == 0)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_GRP_DEL, 1);
    if ( result < 0)
    {
        return result;
    }

    //Check whether the endpoint supports Multi Channel Association CC
    if (ifd->cls == COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2)
    {
        mul_ch_assoc_present = 1;
    }
    else if (zwif_ifd_get(ifd, COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2, &mul_ch_assoc_ifd) == 0)
    {
        mul_ch_assoc_present = 2;
    }
    else
    {
        mul_ch_assoc_present = 0;
    }

    //Check for controller node id
    if (mul_ch_assoc_present)
    {
        for (i = 0; i < cnt; i++)
        {
            if (ep[i].nodeid == ifd->net->ctl.nodeid)
            {   //Found controller
                //Set to endpoint 1
                ep[i].epid = 1;
            }
        }
    }


    //Check whether there is any real endpoint
    for (i = 0; i < cnt; i++)
    {
        if (ep[i].epid > 0)
        {
            //Check whether the endpoint supports Multi Channel Association CC
            if (mul_ch_assoc_present == 0)
            {
                return ZW_ERR_WRONG_IF;
            }

            if (mul_ch_assoc_present == 2)
            {
                ifd = &mul_ch_assoc_ifd;
            }

            //Call the multi-channel version of this function
            return zwif_group_mc_del(ifd, group, ep, cnt);
        }
    }

    cmd_len = 3 + cnt;
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    //Prepare the command
    cmd[0] = (uint8_t)ifd->cls;
    cmd[1] = ASSOCIATION_REMOVE;
    cmd[2] = group;
    for (i = 0; i < cnt; i++)
    {
        cmd[3+i] = ep[i].nodeid;
    }

    //Send the command
    result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);

    free(cmd);
    return result;
}


/**
zwif_group_sup_get - get information on the maximum number of groupings the given node supports through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_sup_get(zwifd_p ifd,  zwrep_group_sup_fn cb)
{
    int         result;

    if ((ifd->cls != COMMAND_CLASS_ASSOCIATION) &&
        (ifd->cls != COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2))
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_MAX_GROUP, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, ASSOCIATION_GROUPINGS_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_GRP_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 ASSOCIATION_GROUPINGS_GET, zwif_exec_cb);
    }
    return result;
}


/**
zwif_group_actv_get - get information on the current active group from a node through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_actv_get(zwifd_p ifd,  zwrep_group_actv_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_ASSOCIATION_V2)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }
    //Setup report callback
    result = zwif_set_report(ifd, cb, ASSOCIATION_SPECIFIC_GROUP_REPORT_V2);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_GRP_ACTV_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 ASSOCIATION_SPECIFIC_GROUP_GET_V2, zwif_exec_cb);
    }
    return result;
}


/**
zwif_group_cmd_sup_get - get information on command records supporting capabilities through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_cmd_sup_get(zwifd_p ifd, zwrep_grp_cmd_sup_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }
    //Setup report callback
    result = zwif_set_report(ifd, cb, COMMAND_RECORDS_SUPPORTED_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_GRP_CMD_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 COMMAND_RECORDS_SUPPORTED_GET, zwif_exec_cb);
    }
    return result;
}



/**
zwif_group_cmd_set - specify which command should be sent to node within a given group
@param[in]	ifd	        interface
@param[in]	group	    group id
@param[in]	node	    node within the grouping specified, that should receive the command specified in cmd_buf
@param[in]	cmd_buf     command and parameters buffer
@param[in]	len		    length of cmd in bytes
@return	ZW_ERR_XXX
*/
int zwif_group_cmd_set(zwifd_p ifd, uint8_t group, zwnoded_p node, uint8_t *cmd_buf, uint8_t len)
{
    int         result;
    uint8_t     *cmd;
    uint8_t     cmd_len;    //the length of the command and parameters

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if ((group == 0) || (node->nodeid == 0))
    {
        return ZW_ERR_VALUE;
    }

    cmd_len = 5 + len;
    cmd = (uint8_t *)malloc(cmd_len);

    if (!cmd)
    {
        return ZW_ERR_MEMORY;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_GRP_CMD_SET, 1);
    if ( result < 0)
    {
        return result;
    }


    //Prepare the command
    cmd[0] = COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION;
    cmd[1] = COMMAND_CONFIGURATION_SET;
    cmd[2] = group;
    cmd[3] = node->nodeid;
    cmd[4] = len;
    memcpy(cmd + 5, cmd_buf, len);


    //Send the command
    result = zwif_exec(ifd, cmd, cmd_len, zwif_exec_cb);

    free(cmd);
    return result;
}


/**
zwif_group_cmd_get - get command record for a node within a given grouping identifier through report callback
@param[in]	ifd	    interface
@param[in]	group	grouping identifier
@param[in]	nodeid	node id of the node within the grouping specified
@param[in]  cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_group_cmd_get(zwifd_p ifd, uint8_t group, uint8_t nodeid, zwrep_grp_cmd_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if ((group == 0) || (nodeid == 0))
    {
        return ZW_ERR_VALUE;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, COMMAND_CONFIGURATION_REPORT);

    if (result == 0)
    {
        uint8_t param[2];
        //Prepare the parameters
        param[0] = group;
        param[1] = nodeid;

        result = zwif_cmd_id_set(ifd, ZW_CID_GRP_CMD_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, param, 2,
                                 COMMAND_CONFIGURATION_GET, zwif_exec_cb);
    }
    return result;
}


/**
zwif_group_info_get - get detailed group information
@param[in]	ifd	        interface
@param[out]	grp_info	grouping information if success; NULL on failure
@return  0 on success, else ZW_ERR_XXX
@post  Caller is required to call zwif_group_info_free to free the memory allocated to grp_info
*/
int zwif_group_info_get(zwifd_p ifd, if_grp_info_dat_t **grp_info)
{
    int                 result;
    int                 i;
    zwif_p              intf;
    zwnet_p             nw;
    if_grp_info_dat_t   *intf_grp_data;
    if_grp_info_dat_t   *grp_data;
    zw_grp_info_p       grp_info_ent;

    if (ifd->cls != COMMAND_CLASS_ASSOCIATION_GRP_INFO)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    nw = ifd->net;

    plt_mtx_lck(nw->mtx);
    intf = zwif_get_if(ifd);

    if (intf)
    {
        intf_grp_data = (if_grp_info_dat_t *)intf->tmp_data;

        if (!intf_grp_data)
        {
            result = ZW_ERR_INTF_NO_DATA;
            goto l_GPR_INFO_ERR1;
        }

        grp_data = (if_grp_info_dat_t *)calloc(1, sizeof(if_grp_info_dat_t) +
                                               (sizeof(zw_grp_info_p) * intf_grp_data->valid_grp_cnt));

        if (!grp_data)
        {
            result = ZW_ERR_MEMORY;
            goto l_GPR_INFO_ERR1;
        }

        grp_data->group_cnt = intf_grp_data->group_cnt;
        grp_data->valid_grp_cnt = intf_grp_data->valid_grp_cnt;
        grp_data->dynamic = intf_grp_data->dynamic;

        for (i=0; i<intf_grp_data->valid_grp_cnt; i++)
        {
            if (intf_grp_data->grp_info[i])
            {
                grp_info_ent = (zw_grp_info_p)calloc(1, sizeof(zw_grp_info_t) +
                                                 (intf_grp_data->grp_info[i]->cmd_ent_cnt * sizeof(grp_cmd_ent_t)));
                if (!grp_info_ent)
                {
                    result = ZW_ERR_MEMORY;
                    goto l_GPR_INFO_ERR2;
                }

                grp_data->grp_info[i] = grp_info_ent;
                *grp_info_ent = *intf_grp_data->grp_info[i];
                memcpy(grp_info_ent->cmd_lst, intf_grp_data->grp_info[i]->cmd_lst, grp_info_ent->cmd_ent_cnt * sizeof(grp_cmd_ent_t));
            }
            else
            {
                result = ZW_ERR_INTF_NO_DATA;
                goto l_GPR_INFO_ERR2;
            }
        }
    }
    else
    {
        result = ZW_ERR_INTF_NOT_FOUND;
        goto l_GPR_INFO_ERR1;
    }

    *grp_info = grp_data;
    plt_mtx_ulck(nw->mtx);
    return ZW_ERR_NONE;

l_GPR_INFO_ERR2:
    for (i=0; i<grp_data->valid_grp_cnt; i++)
    {
        free(grp_data->grp_info[i]);
    }
    free(grp_data);
l_GPR_INFO_ERR1:
    *grp_info = NULL;
    plt_mtx_ulck(nw->mtx);
    return result;
}


/**
zwif_group_info_free - free group information
@param[in]	grp_info	grouping information returned by zwif_group_info_get()
@return
@post   Caller should not use the grp_info after this call.
*/
void zwif_group_info_free(if_grp_info_dat_t *grp_info)
{
    int i;

    if (!grp_info)
    {
        return;
    }

    for (i=0; i<grp_info->valid_grp_cnt; i++)
    {
        free(grp_info->grp_info[i]);
    }

    free(grp_info);
}


/**
@}
@defgroup If_Wku Wake Up Interface APIs
Wake up APIs are for battery powered device that sleep most of the time
@{
*/

/**
zwif_wakeup_set - set wake up interval and node to notify on wake up
@param[in]	ifd	interface
@param[in]	secs	interval in seconds (24 bit)
@param[in]	node	node to notify or NULL for broadcast
@return		ZW_ERR_XXX
*/
int  zwif_wakeup_set(zwifd_p ifd, uint32_t secs, zwnoded_p node)
{
    uint8_t     cmd[6];
    int         result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_WAKE_UP)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (secs > 0xFFFFFF)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_WAKE_UP_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_WAKE_UP;
    cmd[1] = WAKE_UP_INTERVAL_SET;
    cmd[2] = (secs >> 16) & 0xFF;
    cmd[3] = (secs >> 8) & 0xFF;
    cmd[4] = secs & 0xFF;
    //If node is NULL, set the notification node to broadcast address
    cmd[5] = (node)? node->nodeid : 255;

    //Send the command
    return zwif_exec(ifd, cmd, 6, zwif_exec_cb);
}

/**
zwif_wakeup_get - get wake up report
@param[in]	ifd	interface
@param[in]	cb	        report callback function
@return		ZW_ERR_XXX
*/
int  zwif_wakeup_get(zwifd_p ifd, zwrep_wakeup_fn cb)
{
    int         result;

    if (ifd->cls != COMMAND_CLASS_WAKE_UP)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, WAKE_UP_INTERVAL_REPORT);
    if (result != 0)
    {
        return result;
    }

    zwif_set_report(ifd, cb, WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2);//May fail if command version is 1

    result = zwif_cmd_id_set(ifd, ZW_CID_WAKE_UP_GET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Request for report
    return zwif_get_report(ifd, NULL, 0, WAKE_UP_INTERVAL_GET, zwif_exec_cb);

}

/**
@}
@defgroup If_Basic Basic Interface APIs
Basic command that can be used to control the basic functionality of a device
@{
*/

/**
zwif_basic_set - set basic value
@param[in]	ifd		interface
@param[in]	v		value (the range of value is device specific)
@return	ZW_ERR_XXX
*/
int zwif_basic_set(zwifd_p ifd, uint8_t v)
{
#ifdef  TEST_EXT_CMD_CLASS
    uint8_t     cmd[4];
    int         result;


    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_EXT_TEST)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_BASIC_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = 0xF2;
    cmd[1] = 0x12;
    cmd[2] = BASIC_SET;
    cmd[3] = v;

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);
#else

    uint8_t     cmd[3];
    int         result;


    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_BASIC)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_BASIC_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_BASIC;
    cmd[1] = BASIC_SET;
    cmd[2] = v;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
#endif
}


/**
zwif_basic_rpt_set - Setup a basic report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_basic_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb)
{
    //Check whether the command class is correct
#ifdef  TEST_EXT_CMD_CLASS
    if (ifd->cls == COMMAND_CLASS_EXT_TEST)
    {
        return zwif_set_report(ifd, rpt_cb, BASIC_REPORT);
    }
#else
    if (ifd->cls == COMMAND_CLASS_BASIC)
    {
        return zwif_set_report(ifd, rpt_cb, BASIC_REPORT);
    }
#endif
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_basic_get_ex - get basic report through report callback
@param[in]	ifd	            interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_basic_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_BASIC)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   BASIC_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_BASIC_RPT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   BASIC_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_basic_get - get basic report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_basic_get(zwifd_p ifd)
{
    //Check whether the command class is correct
#ifdef  TEST_EXT_CMD_CLASS
    if (ifd->cls == COMMAND_CLASS_EXT_TEST)
    {
        return zwif_basic_get_ex(ifd, NULL);
    }
#else
    if (ifd->cls == COMMAND_CLASS_BASIC)
    {
        return zwif_basic_get_ex(ifd, NULL);
    }
#endif
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_basic_get - get basic report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_basic_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_basic_get_ex(ifd, poll_req);
}


/**
@}
@defgroup If_Doorlock Door Lock Interface APIs
Used to secure/unsecure a lock type as well as setting the configuration of an advanced Z-Wave door lock device
@{
*/

/**
zwif_dlck_op_set - Set door lock operation
@param[in]	ifd	    interface
@param[in]	mode	operation mode (ZW_DOOR_XXX).
@return	ZW_ERR_XXX
*/
int zwif_dlck_op_set(zwifd_p ifd, uint8_t mode)
{
    int         result;
    uint8_t     cmd[3];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_DOOR_LOCK)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (memchr(dlck_op_mode, mode, sizeof(dlck_op_mode)) == NULL)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_DL_OP_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_DOOR_LOCK;
    cmd[1] = DOOR_LOCK_OPERATION_SET;
    cmd[2] = mode;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
zwif_dlck_op_rpt_set - Setup a door lock operation report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_dlck_op_rpt_set(zwifd_p ifd, zwrep_dlck_op_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_DOOR_LOCK)
    {
        return zwif_set_report(ifd, rpt_cb, DOOR_LOCK_OPERATION_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_dlck_op_get_ex - get the state of the door lock device through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_dlck_op_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_DOOR_LOCK)
    {

        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0, DOOR_LOCK_OPERATION_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_DL_OP_RPT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0, DOOR_LOCK_OPERATION_GET, zwif_exec_cb);
        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_dlck_op_get - get the state of the door lock device through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_dlck_op_get(zwifd_p ifd)
{
    return zwif_dlck_op_get_ex(ifd, NULL);
}


/**
zwif_dlck_op_get_poll - get the state of the door lock device through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_dlck_op_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_dlck_op_get_ex(ifd, poll_req);
}


/**
zwif_dlck_cfg_set - Set the configuration of the door lock device
@param[in]	ifd	    interface
@param[in]	config	configuration
@return	ZW_ERR_XXX
*/
int zwif_dlck_cfg_set(zwifd_p ifd, zwdlck_cfg_p config)
{
    int         result;
    uint8_t     cmd[6];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_DOOR_LOCK)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (!(config->type == ZW_DOOR_OP_CONST || config->type == ZW_DOOR_OP_TIMED))
    {
        return ZW_ERR_VALUE;
    }

    //Set timeout to 0xFE if timeout is not required
    if (config->type != ZW_DOOR_OP_TIMED)
    {
        config->tmout_sec = config->tmout_min = 0xFE;
    }

    //Check input values
    if (config->type == ZW_DOOR_OP_TIMED)
    {
    if (!(config->tmout_min >= 1  && config->tmout_min <= 254))
    {
        return ZW_ERR_VALUE;
    }

        if (!(config->tmout_sec >= 1  && config->tmout_sec <= 59))
    {
        return ZW_ERR_VALUE;
    }
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_DL_CFG_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_DOOR_LOCK;
    cmd[1] = DOOR_LOCK_CONFIGURATION_SET;
    cmd[2] = config->type;
    cmd[3] = (config->out_sta << 4) | (config->in_sta & 0x0F);
    cmd[4] = config->tmout_min;
    cmd[5] = config->tmout_sec;

    //Send the command
    return zwif_exec(ifd, cmd, 6, zwif_exec_cb);
}


/**
zwif_dlck_cfg_get - get configuration parameter through report callback
@param[in]		ifd	    interface
@param[in]		cb	    report callback function
@return  0 on success, else ZW_ERR_XXX
*/
int zwif_dlck_cfg_get(zwifd_p ifd, zwrep_dlck_cfg_fn cb)
{
    int result;

    if (ifd->cls != COMMAND_CLASS_DOOR_LOCK)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }
    //Setup report callback
    result = zwif_set_report(ifd, cb, DOOR_LOCK_CONFIGURATION_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_DL_CFG_RPT_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 DOOR_LOCK_CONFIGURATION_GET, zwif_exec_cb);
    }
    return result;
}


/**
@}
@defgroup If_Usrcode User Code Interface APIs
Used to manage user codes required to unlock a lock
@{
*/

/**
zwif_usrcod_set - set the user code
@param[in]	ifd	    interface
@param[in]	usr_cod user code and its status
@return	ZW_ERR_XXX
*/
int zwif_usrcod_set(zwifd_p ifd, zwusrcod_p usr_cod)
{
    int         result;
    int         i;
    uint8_t     cmd[4 + MAX_USRCOD_LENGTH];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_USER_CODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (memchr(usr_id_sts, usr_cod->id_sts, sizeof(usr_id_sts)) == NULL)
    {
        return ZW_ERR_VALUE;
    }

    if (!(usr_cod->code_len >= 4  && usr_cod->code_len <= 10))
    {
        return ZW_ERR_VALUE;
    }

    for (i=0; i<usr_cod->code_len; i++)
    {
        if (!isdigit(usr_cod->code[i]))
        {
            return ZW_ERR_VALUE;
        }
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_USRCOD_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_USER_CODE;
    cmd[1] = USER_CODE_SET;
    cmd[2] = usr_cod->id;
    cmd[3] = usr_cod->id_sts;
    memcpy(cmd + 4, usr_cod->code, usr_cod->code_len);

    //Send the command
    return zwif_exec(ifd, cmd, 4 + usr_cod->code_len, zwif_exec_cb);
}


/**
zwif_usrcod_get - get the specified user code and its status
@param[in]	ifd	    interface
@param[in]	usr_id  user identifier
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_usrcod_get(zwifd_p ifd, uint8_t usr_id, zwrep_usr_cod_fn cb)
{
    int result;

    if (ifd->cls != COMMAND_CLASS_USER_CODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (usr_id == 0)
    {
        return ZW_ERR_VALUE;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, USER_CODE_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_USRCOD_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, &usr_id, 1,
                                 USER_CODE_GET, zwif_exec_cb);
    }
    return result;
}


/**
zwif_usrcod_sup_get - get the number of supported user codes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_usrcod_sup_get(zwifd_p ifd, zwrep_usr_sup_fn cb)
{
    int result;

    if (ifd->cls != COMMAND_CLASS_USER_CODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_MAX_USR_CODES, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, USERS_NUMBER_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_USRCOD_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 USERS_NUMBER_GET, zwif_exec_cb);
    }
    return result;
}


/**
@}
@defgroup If_Alarm Alarm Interface APIs
Used to report alarm or service conditions
@{
*/

/**
zwif_alrm_rpt_set - setup an alarm report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_alrm_rpt_set(zwifd_p ifd, zwrep_alrm_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_ALARM)
    {
        return zwif_set_report(ifd, rpt_cb, ALARM_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_alrm_get_ex - get the state of the alarm device through report callback
@param[in]	ifd	        interface
@param[in]	vtype	    vendor specific alarm type. Zero if this field is not used
@param[in]	ztype	    Z-wave alarm type (ZW_ALRM_XXX). Zero if this field is not used; 0xFF=to retrieve the first alarm detection.
@param[in]	evt	        Event corresponding to Z-wave alarm type. Zero if this field is not used.
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_alrm_get_ex(zwifd_p ifd, uint8_t vtype, uint8_t ztype, uint8_t evt, zwpoll_req_t *poll_req)
{
    uint8_t     cmd[4];
    uint8_t     cmd_len;

    if (ifd->cls != COMMAND_CLASS_ALARM)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //For ztype = 0xFF, event must be set to 0
    if (ztype == 0xFF)
    {
        evt = 0;
    }

    //Prepare the command
    cmd[0] = vtype;
    cmd[1] = ztype;
    cmd[2] = evt;

    //Determine the command length
    cmd_len = (ifd->ver < 3)? ifd->ver : 3;

    //For backwark compatibility
    if (cmd_len == 3)
    {
        if ((evt == 0) && (ztype != 0xFF))
        {
            cmd_len--;
        }

        if (ztype == 0)
        {
            cmd_len--;
        }
    }
    else if (cmd_len == 2)
    {
        if (ztype == 0)
        {
            cmd_len--;
        }
    }

    //Request for report
    if (poll_req)
    {
        return zwif_get_report_poll(ifd, cmd, cmd_len,
                                    ALARM_GET, poll_req);
    }
    else
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_ALRM_RPT_GET, 1);
        if (result < 0)
        {
            return result;
        }
        return zwif_get_report(ifd, cmd, cmd_len,
                               ALARM_GET, zwif_exec_cb);
    }
}


/**
zwif_alrm_get - get the state of the alarm device through report callback
@param[in]	ifd	        interface
@param[in]	vtype	    vendor specific alarm type. Zero if this field is not used
@param[in]	ztype	    Z-wave alarm type (ZW_ALRM_XXX). Zero if this field is not used; 0xFF=to retrieve the first alarm detection.
@param[in]	evt	        Event corresponding to Z-wave alarm type. Zero if this field is not used.
@return		ZW_ERR_XXX
*/
int zwif_alrm_get(zwifd_p ifd, uint8_t vtype, uint8_t ztype, uint8_t evt)
{
    return zwif_alrm_get_ex( ifd, vtype, ztype, evt, NULL);
}

/**
zwif_alrm_get_poll_get - get the state of the alarm device through report callback
@param[in]	ifd	        interface
@param[in]	vtype	    vendor specific alarm type. Zero if this field is not used
@param[in]	ztype	    Z-wave alarm type (ZW_ALRM_XXX). Zero if this field is not used; 0xFF=to retrieve the first alarm detection.
@param[in]	evt	        Event corresponding to Z-wave alarm type. Zero if this field is not used.
@param[in, out] poll_req Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_alrm_get_poll(zwifd_p ifd, uint8_t vtype, uint8_t ztype, uint8_t evt, zwpoll_req_t *poll_req)
{
    return zwif_alrm_get_ex( ifd, vtype, ztype, evt, poll_req);
}


/**
zwif_alrm_set - set the activity of the specified Z-Wave Alarm Type
@param[in]	ifd	    interface
@param[in]	ztype	Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	sts     Z-wave alarm status. 0= deactivated; 0xFF= activated
@return	ZW_ERR_XXX
*/
int zwif_alrm_set(zwifd_p ifd, uint8_t ztype, uint8_t sts)
{
    int         result;
    uint8_t     cmd[4];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_ALARM)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_ALRM_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_ALARM;
    cmd[1] = ALARM_SET_V2;
    cmd[2] = ztype;
    cmd[3] = sts;

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);
}


/**
zwif_alrm_sup_get - get the supported alarm types
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_alrm_sup_get(zwifd_p ifd, zwrep_alrm_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_ALARM)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_ALARM_TYPE, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, ALARM_TYPE_SUPPORTED_REPORT_V2);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_ALRM_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 ALARM_TYPE_SUPPORTED_GET_V2, zwif_exec_cb);
    }
    return result;

}


/**
zwif_alrm_sup_evt_get - get the supported events of a specified alarm type
@param[in]	ifd	    interface
@param[in]	ztype   Z-wave alarm type (ZW_ALRM_XXX)
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_alrm_sup_evt_get(zwifd_p ifd, uint8_t ztype, zwrep_alrm_evt_fn cb)
{
    int     result;
    zwif_p  intf;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_ALARM)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 3 and above
    if (ifd->ver < 3)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check whether to use cached data
    plt_mtx_lck(ifd->net->mtx);
    intf = zwif_get_if(ifd);
    if (intf && intf->data_cnt)
    {
        int                 i;
        if_alarm_data_t     *alarm_dat;

        alarm_dat = (if_alarm_data_t *)intf->data;

        //Find matching alarm type in cache
        for (i=0; i<alarm_dat->type_evt_cnt; i++)
        {
            if(alarm_dat->type_evt[i].ztype == ztype)
            {
                if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_ALARM_EVENT, ztype))
                {
                    plt_mtx_ulck(ifd->net->mtx);
                    return ZW_ERR_QUEUED;
                }
            }
        }
    }
    plt_mtx_ulck(ifd->net->mtx);

    //Setup report callback
    result = zwif_set_report(ifd, cb, EVENT_SUPPORTED_REPORT_V3);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_ALRM_SUP_EVT_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, &ztype, 1,
                                 EVENT_SUPPORTED_GET_V3, zwif_exec_cb);
    }
    return result;
}


/**
@}
@defgroup If_Battery  Battery Interface APIs
Battery command that can be used to read the remaining level of a device
@{
*/

/**
zwif_battery_rpt_set - Setup a battery report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_battery_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb)
{
    printf("linhui, battery report set called\n");
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_BATTERY)
    {
        printf("linhui, class command battery correct\n");
        return zwif_set_report(ifd, rpt_cb, BATTERY_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_battery_get_ex - get battery report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_battery_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    printf("linhui zwif_battery_get_ex called\n");
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_BATTERY)
    {
        printf("linhui, command class correct, go on.\n");
        if (poll_req)
        {
            printf("linhui, poll_req?\n");
            return zwif_get_report_poll(ifd, NULL, 0,
                                   BATTERY_GET, poll_req);
        }
        else
        {
            printf("linhui, no poll req,\n");
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_BATTERY_RPT_GET, 1);
            printf("linhui, nopoll req, cmd id set, result=%d\n",result);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   BATTERY_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_battery_get - get battery report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_battery_get(zwifd_p ifd)
{
    return zwif_battery_get_ex(ifd, NULL);
}


/**
zwif_battery_get_poll - get battery report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_battery_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_battery_get_ex(ifd, poll_req);
}


/**
@}
@defgroup If_Thrm_fan Thermostat Fan Mode and State Interface APIs
Used to report thermostat fan operating conditions
@{
*/

/**
zwif_thrmo_fan_md_rpt_set - setup a thermostat fan operating mode report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_fan_md_rpt_set(zwifd_p ifd, zwrep_thrmo_fan_md_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_FAN_MODE)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_FAN_MODE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_fan_md_get - get the thermostat fan operating mode through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_thrmo_fan_md_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_FAN_MODE)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_FAN_MD_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               THERMOSTAT_FAN_MODE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_fan_md_set - set the fan mode in the device
@param[in]	ifd	    interface
@param[in]	off     fan off flag. Non-zero will switch the fan fully OFF.
                    In order to activate a fan mode this flag must be set to “0”.
@param[in]	mode    fan operating mode, ZW_THRMO_FAN_MD_XXX
@return	ZW_ERR_XXX
*/
int zwif_thrmo_fan_md_set(zwifd_p ifd, uint8_t off, uint8_t mode)
{
    int         result;
    uint8_t     cmd[3];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_FAN_MODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as off flag is only valid for version 2 and above
    if (off && (ifd->ver < 2))
    {
        return ZW_ERR_CMD_VERSION;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_FAN_MD_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_THERMOSTAT_FAN_MODE;
    cmd[1] = THERMOSTAT_FAN_MODE_SET;
    cmd[2] = mode & 0x0F;

    if (off)
    {
        cmd[2] |= 0x80;
    }

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
zwif_thrmo_fan_md_sup_get - get the supported thermostat fan operating modes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_thrmo_fan_md_sup_get(zwifd_p ifd, zwrep_thrmo_fan_md_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_FAN_MODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_THRMO_FAN_MD, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, THERMOSTAT_FAN_MODE_SUPPORTED_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_FAN_MD_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 THERMOSTAT_FAN_MODE_SUPPORTED_GET, zwif_exec_cb);
    }
    return result;

}


/**
zwif_thrmo_fan_sta_rpt_set - setup a thermostat fan operating state report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_fan_sta_rpt_set(zwifd_p ifd, zwrep_thrmo_fan_sta_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_FAN_STATE)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_FAN_STATE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_fan_sta_get - get the thermostat fan operating state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_thrmo_fan_sta_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_FAN_STATE)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_FAN_STA_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               THERMOSTAT_FAN_STATE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_md_rpt_set - setup a thermostat mode report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_md_rpt_set(zwifd_p ifd, zwrep_thrmo_md_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_MODE)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_MODE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_md_get - get the thermostat mode through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_thrmo_md_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_MODE)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_MD_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               THERMOSTAT_MODE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_md_set - set the mode in the device
@param[in]	ifd	    interface
@param[in]	mode    mode, ZW_THRMO_MD_XXX
@return	ZW_ERR_XXX
*/
int zwif_thrmo_md_set(zwifd_p ifd, uint8_t mode)
{
    int         result;
    uint8_t     cmd[3];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_MODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_MD_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_THERMOSTAT_MODE;
    cmd[1] = THERMOSTAT_MODE_SET;
    cmd[2] = mode & 0x1F;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
zwif_thrmo_md_sup_get - get the supported thermostat modes
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_thrmo_md_sup_get(zwifd_p ifd, zwrep_thrmo_md_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_MODE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_THRMO_MD, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, THERMOSTAT_MODE_SUPPORTED_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_MD_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 THERMOSTAT_MODE_SUPPORTED_GET, zwif_exec_cb);
    }
    return result;

}


/**
zwif_thrmo_op_sta_rpt_set - setup a thermostat operating state report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_op_sta_rpt_set(zwifd_p ifd, zwrep_thrmo_op_sta_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_OPERATING_STATE)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_OPERATING_STATE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_op_sta_get - get the thermostat operating state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_thrmo_op_sta_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_OPERATING_STATE)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_OP_STA_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               THERMOSTAT_OPERATING_STATE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_setb_rpt_set - setup a thermostat setback report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_setb_rpt_set(zwifd_p ifd, zwrep_thrmo_setb_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_SETBACK)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_SETBACK_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_setb_get - get the thermostat setback state through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_thrmo_setb_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_SETBACK)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_SETB_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               THERMOSTAT_SETBACK_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_setb_set - set the state in the device
@param[in]	ifd	            interface
@param[in]	type            setback type, ZW_THRMO_SETB_TYP_XXX
@param[in]	state           setback state, ZW_THRMO_SETB_STA_XXX
@param[in]	tenth_degree	1/10 of a degree (Kelvin).  This parameter is valid if state equals to ZW_THRMO_SETB_STA_SETB.
                            Valid values: -128 to 120 (inclusive). i.e. setback temperature ranges from -12.8 degree K to 12 degree K.
@return	ZW_ERR_XXX
*/
int zwif_thrmo_setb_set(zwifd_p ifd, uint8_t type, uint8_t state, int8_t tenth_degree)
{
    int         result;
    uint8_t     cmd[4];
    uint8_t     setb_state = 0;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_SETBACK)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if ((type > ZW_THRMO_SETB_TYP_PERM_OVR) || (state > ZW_THRMO_SETB_STA_UNUSED))
    {
        return ZW_ERR_VALUE;
    }

    if (state == ZW_THRMO_SETB_STA_SETB)
    {
        if (!(/*(tenth_degree >= -128) &&*/ (tenth_degree <= 120)))
        {
            return ZW_ERR_VALUE;
        }

        setb_state = (uint8_t)tenth_degree;
    }
    else if (state == ZW_THRMO_SETB_STA_FROST_PROCT)
    {
        setb_state = 121;
    }
    else if (state == ZW_THRMO_SETB_STA_ENER_SAVE)
    {
        setb_state = 122;
    }
    else    //Unused
    {
        setb_state = 127;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_SETB_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_THERMOSTAT_SETBACK;
    cmd[1] = THERMOSTAT_SETBACK_SET;
    cmd[2] = type & 0x03;
    cmd[3] = setb_state;

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);
}


/**
zwif_thrmo_setp_rpt_set - setup a thermostat setpoint report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_thrmo_setp_rpt_set(zwifd_p ifd, zwrep_thrmo_setp_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_SETPOINT)
    {
        return zwif_set_report(ifd, rpt_cb, THERMOSTAT_SETPOINT_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_thrmo_setp_get_ex - get the thermostat setpoint through report callback
@param[in]	ifd	            interface
@param[in]	type	        setpoint type
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_thrmo_setp_get_ex(zwifd_p ifd, uint8_t type, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_THERMOSTAT_SETPOINT)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, &type, 1,
                                   THERMOSTAT_SETPOINT_GET, poll_req);

        }
        else
        {
            int result;

            result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_SETP_GET, 1);
            if ( result < 0)
            {
                return result;
            }

            return zwif_get_report(ifd, &type, 1,
                                   THERMOSTAT_SETPOINT_GET, zwif_exec_cb);
        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_thrmo_setp_get - get the thermostat setpoint through report callback
@param[in]	ifd	        interface
@param[in]	type	    setpoint type
@return		ZW_ERR_XXX
*/
int zwif_thrmo_setp_get(zwifd_p ifd, uint8_t type)
{
    return zwif_thrmo_setp_get_ex(ifd, type, NULL);
}


/**
zwif_thrmo_setp_get_poll - get the thermostat setpoint through report callback
@param[in]	ifd	            interface
@param[in]	type	        setpoint type
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_thrmo_setp_get_poll(zwifd_p ifd, uint8_t type, zwpoll_req_t *poll_req)
{
    return zwif_thrmo_setp_get_ex(ifd, type, poll_req);
}


/**
zwif_thrmo_setp_set - set the setpoint in the device
@param[in]	ifd	    interface
@param[in]	data    setpoint data
@return	ZW_ERR_XXX
*/
int zwif_thrmo_setp_set(zwifd_p ifd, zwsetp_p data)
{
    int         result;
    uint8_t     cmd[8];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_SETPOINT)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (memchr(data_storage_sz, data->size, sizeof(data_storage_sz)) == NULL)
    {
        return ZW_ERR_VALUE;
    }

    if (data->precision > 7)
    {
        return ZW_ERR_VALUE;
    }

    if (data->unit > 1)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_SETP_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_THERMOSTAT_SETPOINT;
    cmd[1] = THERMOSTAT_SETPOINT_SET;
    cmd[2] = data->type & 0x0F;
    cmd[3] = (data->precision << 5) | ((data->unit & 0x03) << 3) | data->size;
    memcpy(cmd + 4, data->data, data->size);

    //Send the command
    return zwif_exec(ifd, cmd, 4 + data->size, zwif_exec_cb);
}


/**
zwif_thrmo_setp_sup_get - get the supported thermostat setpoints
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_thrmo_setp_sup_get(zwifd_p ifd, zwrep_thrmo_setp_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_THERMOSTAT_SETPOINT)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_THRMO_SETPOINT, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, THERMOSTAT_SETPOINT_SUPPORTED_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_THRMO_SETP_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 THERMOSTAT_SETPOINT_SUPPORTED_GET, zwif_exec_cb);
    }
    return result;

}


/**
@}
@defgroup If_Clock Clock Interface APIs
Clock command that can be used to control the clock functionality of a device
@{
*/

/**
zwif_clock_set - set clock value
@param[in]	ifd		interface
@param[in]	weekday	Day of week, ZW_CLOCK_XXX
@param[in]	hour	Hour (in 24 hours format)
@param[in]	minute	Minute
@return	ZW_ERR_XXX
*/
int zwif_clock_set(zwifd_p ifd, uint8_t weekday, uint8_t hour, uint8_t minute)
{
    uint8_t     cmd[4];
    int         result;


    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_CLOCK)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    if ((weekday > 7) || (hour > 23) || (minute > 59))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_CLOCK_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_CLOCK;
    cmd[1] = CLOCK_SET;
    cmd[2] = (weekday << 5) | (hour & 0x1F);
    cmd[3] = minute;

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);

}


/**
zwif_clock_rpt_set - Setup a clock report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_clock_rpt_set(zwifd_p ifd, zwrep_clock_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLOCK)
    {
        return zwif_set_report(ifd, rpt_cb, CLOCK_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_clock_get - get clock report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_clock_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLOCK)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_CLOCK_RPT_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               CLOCK_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
@}
@defgroup If_Climate_Ctl Climate Control Schedule Interface APIs
Climate Control Schedule command that can be used to control a thermostat setback operation schedule
@{
*/

/**
zwif_clmt_ctl_schd_rpt_set - setup a climate control schedule report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        return zwif_set_report(ifd, rpt_cb, SCHEDULE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_clmt_ctl_schd_get - get the climate control schedule through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    Day of week, ZW_CLOCK_XXX
@return		ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_get(zwifd_p ifd, uint8_t weekday)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        int result;

        if ((weekday > 7) || (weekday == 0))
        {
            return ZW_ERR_VALUE;
        }

        result = zwif_cmd_id_set(ifd, ZW_CID_CLMT_CTL_SCHD_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, &weekday, 1,
                               SCHEDULE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_clmt_ctl_schd_set - set the climate control schedule in a device for a specific weekday
@param[in]	ifd	            interface
@param[in]	sched           climate control schedule
@return	ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_set(zwifd_p ifd, zwcc_shed_p sched)
{
    int         result;
    int         i;
    uint8_t     cmd[30];
    uint8_t     *cmdptr;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (!((sched->weekday >= ZW_CLOCK_MONDAY) && (sched->weekday <= ZW_CLOCK_SUNDAY)))
    {
        return ZW_ERR_VALUE;
    }

    if (sched->total > 9)
    {
        return ZW_ERR_VALUE;
    }

    //From "Z-Wave Command Class Specification" :
    //
    //The entire list of switchpoints in the Command must be ordered by time,
    //ascending from 00:00 towards 23:59. Switchpoints which have a Schedule State set to “Unused”
    //shall be placed last. No duplicates shall be allowed for Switchpoints which have a Schedule State
    //different from “Unused”.

    //Check each entry (switchpoint)
    for (i=0; i < sched->total; i++)
    {
        if ((sched->swpts[i].hour > 23) || (sched->swpts[i].minute > 59))
        {
            return ZW_ERR_VALUE;
        }

        if (sched->swpts[i].state > ZW_THRMO_SETB_STA_ENER_SAVE)
        {
            return ZW_ERR_VALUE;
        }

        if (sched->swpts[i].state == ZW_THRMO_SETB_STA_SETB)
        {
            if (!(/*(sched->swpts[i].tenth_deg >= -128) &&*/ (sched->swpts[i].tenth_deg <= 120)))
            {
                return ZW_ERR_VALUE;
            }
        }
        //Check for time in ascending order and also for duplicate schedule time
        if ((i+1) < sched->total)
        {
            if (sched->swpts[i].hour > sched->swpts[i+1].hour)
            {
                return ZW_ERR_VALUE;
            }
            else if (sched->swpts[i].hour == sched->swpts[i+1].hour)
            {
                if (sched->swpts[i].minute >= sched->swpts[i+1].minute)
                {
                    return ZW_ERR_VALUE;
                }
            }
        }

/*
        //Check for duplicate schedule time
        for (j=i; j < sched->total; j++)
        {
            if ((sched->swpts[i].hour == sched->swpts[j].hour)
                && (sched->swpts[i].minute == sched->swpts[j].minute))
            {
                return ZW_ERR_VALUE;
            }
        }
*/
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_CLMT_CTL_SCHD_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
    cmd[1] = SCHEDULE_SET;
    cmd[2] = sched->weekday;

    cmdptr = cmd + 3;
    for (i=0; i < sched->total; i++)
    {
        *cmdptr++ = sched->swpts[i].hour;
        *cmdptr++ = sched->swpts[i].minute;
        switch (sched->swpts[i].state)
        {
            case ZW_THRMO_SETB_STA_SETB:
                *cmdptr++ = (uint8_t)sched->swpts[i].tenth_deg;
                break;

            case ZW_THRMO_SETB_STA_FROST_PROCT:
                *cmdptr++ = 121;
                break;

            case ZW_THRMO_SETB_STA_ENER_SAVE:
                *cmdptr++ = 122;
                break;
        }
    }
    //Fill in the unused entries
    for (i=sched->total; i < 9; i++)
    {
        *cmdptr++ = 0;
        *cmdptr++ = 0;
        *cmdptr++ = 127;//unused state
    }

    printf("linhui, cmd 49, climate started");

    //Send the command
    return zwif_exec(ifd, cmd, 30, zwif_exec_cb);
}


/**
zwif_clmt_ctl_schd_chg_rpt_set - setup a climate control schedule change report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_chg_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_chg_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        return zwif_set_report(ifd, rpt_cb, SCHEDULE_CHANGED_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_clmt_ctl_schd_chg_get_ex - get the climate control schedule change counter through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    Day of week, ZW_CLOCK_XXX
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_clmt_ctl_schd_chg_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   SCHEDULE_CHANGED_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_CLMT_CTL_SCHD_CHG_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   SCHEDULE_CHANGED_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_clmt_ctl_schd_chg_get - get the climate control schedule change counter through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    Day of week, ZW_CLOCK_XXX
@return		ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_chg_get(zwifd_p ifd)
{
    return zwif_clmt_ctl_schd_chg_get_ex(ifd, NULL);
}


/**
zwif_clmt_ctl_schd_chg_get_poll - get the climate control schedule change counter through report callback
@param[in]	ifd	        interface
@param[in]	weekday	    Day of week, ZW_CLOCK_XXX
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_clmt_ctl_schd_chg_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_clmt_ctl_schd_chg_get_ex(ifd, poll_req);
}


/**
zwif_clmt_ctl_schd_ovr_rpt_set - setup a climate control schedule override report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_ovr_rpt_set(zwifd_p ifd, zwrep_clmt_ctl_schd_ovr_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        return zwif_set_report(ifd, rpt_cb, SCHEDULE_OVERRIDE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_clmt_ctl_schd_ovr_get - get the climate control schedule override through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_ovr_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        int result;

        result = zwif_cmd_id_set(ifd, ZW_CID_CLMT_CTL_SCHD_OVR_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               SCHEDULE_OVERRIDE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_clmt_ctl_schd_ovr_set - set the climate control schedule override in a device
@param[in]	ifd	            interface
@param[in]	schd_ovr        climate control schedule override
@return	ZW_ERR_XXX
*/
int zwif_clmt_ctl_schd_ovr_set(zwifd_p ifd, zwcc_shed_ovr_p schd_ovr)
{
    int         result;
    uint8_t     cmd[4];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (schd_ovr->state > ZW_THRMO_SETB_STA_UNUSED)
    {
        return ZW_ERR_VALUE;
    }

    if (schd_ovr->type > ZW_THRMO_SETB_TYP_PERM_OVR)
    {
        return ZW_ERR_VALUE;
    }

    if (schd_ovr->state == ZW_THRMO_SETB_STA_SETB)
    {
        if (!(/*(schd_ovr->tenth_deg >= -128) &&*/ (schd_ovr->tenth_deg <= 120)))
        {
            return ZW_ERR_VALUE;
        }
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_CLMT_CTL_SCHD_OVR_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
    cmd[1] = SCHEDULE_OVERRIDE_SET;
    cmd[2] = schd_ovr->type;

    switch (schd_ovr->state)
    {
        case ZW_THRMO_SETB_STA_SETB:
            cmd[3] = (uint8_t)schd_ovr->tenth_deg;
            break;

        case ZW_THRMO_SETB_STA_FROST_PROCT:
            cmd[3] = 121;
            break;

        case ZW_THRMO_SETB_STA_ENER_SAVE:
            cmd[3] = 122;
            break;

        case ZW_THRMO_SETB_STA_UNUSED:
            cmd[3] = 127;
            break;
    }

    printf("linhui, cmd 49, climate started");

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);
}


/**
@}
@defgroup If_Protection  Protection Interface APIs
Protection command that can be used to protect a device from unauthorized control
@{
*/

/**
zwif_prot_rpt_set - setup a protection report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_prot_rpt_set(zwifd_p ifd, zwrep_prot_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        return zwif_set_report(ifd, rpt_cb, PROTECTION_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_prot_get_ex - get the protection states through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_prot_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{

    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   PROTECTION_GET, poll_req);
        }
        else
        {
            int result;

            result = zwif_cmd_id_set(ifd, ZW_CID_PROT_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   PROTECTION_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_prot_get - get the protection states through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_prot_get(zwifd_p ifd)
{
    return zwif_prot_get_ex(ifd, NULL);
}


/**
zwif_prot_get_poll - get the protection states through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_prot_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_prot_get_ex(ifd, poll_req);
}


/**
zwif_prot_set - set the protection states in the device
@param[in]	ifd	        interface
@param[in]	local_prot  local protection state, ZW_LPROT_XXX
@param[in]	rf_prot     RF protection state, ZW_RFPROT_XXX. For device
                        that supports only version 1, this field will be ignored
@return	ZW_ERR_XXX
*/
int zwif_prot_set(zwifd_p ifd, uint8_t local_prot, uint8_t rf_prot)
{
    int         result;
    uint8_t     cmd[4];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_PROTECTION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (local_prot > 2)
    {
        return ZW_ERR_VALUE;
    }

    if ((ifd->ver > 1) && (rf_prot > 2))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_PROT_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_PROTECTION;
    cmd[1] = PROTECTION_SET;
    cmd[2] = local_prot;
    cmd[3] = rf_prot;

    //Send the command
    return zwif_exec(ifd, cmd, (ifd->ver > 1)? 4 : 3, zwif_exec_cb);
}


/**
zwif_prot_sup_get - get the supported protections
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_prot_sup_get(zwifd_p ifd, zwrep_prot_sup_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_PROTECTION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_PROT, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, PROTECTION_SUPPORTED_REPORT_V2);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_PROT_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 PROTECTION_SUPPORTED_GET_V2, zwif_exec_cb);
    }
    return result;

}


/**
zwif_prot_ec_rpt_set - setup a protection exclusive control report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_prot_ec_rpt_set(zwifd_p ifd, zwrep_prot_ec_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        return zwif_set_report(ifd, rpt_cb, PROTECTION_EC_REPORT_V2);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_prot_ec_get - get the protection exclusive control node through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_prot_ec_get(zwifd_p ifd)
{

    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        int         result;

        //Check version as this command is only valid for version 2 and above
        if (ifd->ver < 2)
        {
            return ZW_ERR_CMD_VERSION;
        }

        result = zwif_cmd_id_set(ifd, ZW_CID_PROT_EC_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               PROTECTION_EC_GET_V2, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_prot_ec_set - set the protection exclusive control node in the device
@param[in]	ifd	        interface
@param[in]	node_id     node ID that has exclusive control can override the RF protection state
                        of the device and can control it regardless of the protection state.
                        Node id of zero is used to reset the protection exclusive control state.
@return	ZW_ERR_XXX
*/
int zwif_prot_ec_set(zwifd_p ifd, uint8_t node_id)
{
    int         result;
    uint8_t     cmd[3];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_PROTECTION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check input values
    if (node_id > 232)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_PROT_EC_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_PROTECTION;
    cmd[1] = PROTECTION_EC_SET_V2;
    cmd[2] = node_id;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
zwif_prot_tmout_rpt_set - setup a RF protection timeout report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_prot_tmout_rpt_set(zwifd_p ifd, zwrep_prot_tmout_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        return zwif_set_report(ifd, rpt_cb, PROTECTION_TIMEOUT_REPORT_V2);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_prot_tmout_get - get the RF protection timeout through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_prot_tmout_get(zwifd_p ifd)
{

    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_PROTECTION)
    {
        int         result;

        //Check version as this command is only valid for version 2 and above
        if (ifd->ver < 2)
        {
            return ZW_ERR_CMD_VERSION;
        }

        result = zwif_cmd_id_set(ifd, ZW_CID_PROT_TIMEOUT_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               PROTECTION_TIMEOUT_GET_V2, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_prot_tmout_set - set the RF protection timeout in the device
@param[in]	ifd	        interface
@param[in]	tmout	    Timeout specifies the time (in different resolutions) a device will remain in RF Protection mode.
                        0x01 to 0x3C = 1 second (0x01) to 60 seconds (0x3C);
                        0x41 to 0xFE = 2 minutes (0x41) to 191 minutes (0xFE);
                        0xFF = No Timeout - The Device will remain in RF Protection mode infinitely.
@return	ZW_ERR_XXX
*/
int zwif_prot_tmout_set(zwifd_p ifd, uint8_t tmout)
{
    int         result;
    uint8_t     cmd[3];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_PROTECTION)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check version as this command is only valid for version 2 and above
    if (ifd->ver < 2)
    {
        return ZW_ERR_CMD_VERSION;
    }

    //Check input values
    if ((tmout == 0) || ((tmout > 0x3C) && (tmout < 0x41)))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_PROT_TIMEOUT_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_PROTECTION;
    cmd[1] = PROTECTION_TIMEOUT_SET_V2;
    cmd[2] = tmout;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);
}


/**
@}
@defgroup If_Status  Status Interface APIs
Application status command that can be used to inform the outcome of a request
@{
*/

/**
zwif_appl_busy_rep_set - setup an application busy report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_appl_busy_rep_set(zwifd_p ifd, zwrep_appl_busy_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_APPLICATION_STATUS)
    {
        return zwif_set_report(ifd, rpt_cb, APPLICATION_BUSY);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}

/**
zwif_appl_reject_rep_set - setup an application rejected request report callback function
@param[in]	ifd         interface descriptor
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_appl_reject_rep_set(zwifd_p ifd, zwrep_appl_reject_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_APPLICATION_STATUS)
    {
        return zwif_set_report(ifd, rpt_cb, APPLICATION_REJECTED_REQUEST);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
@}
@defgroup If_Indicator  Indicator Interface APIs
Indicator command that can be used to show the actual state, level etc. on a device
@{
*/


/**
zwif_ind_rpt_set - Setup an indicator report callback function
@param[in]	ifd         Interface descriptor
@param[in]	rpt_cb	    Report callback function
return      ZW_ERR_XXX
*/
int zwif_ind_rpt_set(zwifd_p ifd, zwrep_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_INDICATOR)
    {
        return zwif_set_report(ifd, rpt_cb, INDICATOR_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_ind_get_ex - get indicator report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
static int zwif_ind_get_ex(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_INDICATOR)
    {
        if (poll_req)
        {
            return zwif_get_report_poll(ifd, NULL, 0,
                                   INDICATOR_GET, poll_req);
        }
        else
        {
            int result;
            result = zwif_cmd_id_set(ifd, ZW_CID_INDICATOR_GET, 1);
            if ( result < 0)
            {
                return result;
            }
            return zwif_get_report(ifd, NULL, 0,
                                   INDICATOR_GET, zwif_exec_cb);

        }
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_ind_get - get indicator report through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_ind_get(zwifd_p ifd)
{
    return zwif_ind_get_ex(ifd, NULL);
}


/**
zwif_ind_get_poll - get indicator report through report callback
@param[in]	ifd	        interface
@param[in, out] poll_req    Poll request
@return		ZW_ERR_NONE if success; else ZW_ERR_XXX on error
*/
int zwif_ind_get_poll(zwifd_p ifd, zwpoll_req_t *poll_req)
{
    return zwif_ind_get_ex(ifd, poll_req);
}


/**
zwif_ind_set - set indicator value
@param[in]	ifd		interface
@param[in]	val		value. The value can be either 0x00 (off/disable) or 0xFF (on/enable).
                    Furthermore it can take values from 1 to 99 (0x01 - 0x63).

@return	ZW_ERR_XXX
*/
int zwif_ind_set(zwifd_p ifd, uint8_t val)
{
    uint8_t     cmd[3];
    int         result;


    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_INDICATOR)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check input values
    if (!((/*val >= 0  &&*/ val <= 99)
          || val == 0xFF))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_INDICATOR_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_INDICATOR;
    cmd[1] = INDICATOR_SET;
    cmd[2] = val;

    //Send the command
    return zwif_exec(ifd, cmd, 3, zwif_exec_cb);

}


/**
@}
@defgroup If_Firmware Firmware update Interface APIs
Used to update firmwares on a device
@{
*/

/**
zwif_fw_info_get - get firmwares information through report callback
@param[in]	ifd	        interface
@param[in]	cb	        report callback
@return		ZW_ERR_XXX
*/
int zwif_fw_info_get(zwifd_p ifd, zwrep_fw_info_fn cb)
{
    int result;

    if (ifd->cls != COMMAND_CLASS_FIRMWARE_UPDATE_MD)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }
    //Setup report callback
    result = zwif_set_report(ifd, cb, FIRMWARE_MD_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_FW_INFO_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 FIRMWARE_MD_GET, zwif_exec_cb);
    }
    return result;
}


/**
zwif_fw_frag_get - Get firmware fragment
@param[in]	    file	    Firmware file
@param[in]	    file_sz     firmware file size
@param[in]	    frag_num	Fragment number to get
@param[out]	    frag_dat    data buffer to store the requested firmware fragment
@param[in,out]	frag_sz     input:requested firmware fragment size; output:size of firmware fragment stored in frag_dat buffer
@param[out]	    last        flag to indicate whether this is the last fragment. 1=last fragment, 0=fragment to follow.
@return	Non-zero on success; zero on failure.
*/
static int zwif_fw_frag_get(FILE *file, long file_sz, uint16_t frag_num, uint8_t *frag_dat,
                            uint16_t *frag_sz, uint8_t *last)
{
    long          offset;
    size_t        byte_read;

    offset = (frag_num - 1) * (*frag_sz);

    if (fseek(file, offset, SEEK_SET) != 0)
    {
        return 0;
    }

    byte_read = fread(frag_dat, 1, *frag_sz, file);

    if (byte_read != *frag_sz)
    {
        if (ferror(file))
        {
            return 0;
        }
    }

    *last = ((offset + byte_read) == file_sz)? 1 : 0;
    *frag_sz = byte_read;

    return 1;
}


/**
zwif_fw_tx_cb - send firmware fragment callback function
@param[in]	appl_ctx    The application layer context
@param[in]	tx_sts		The transmit complete status
@param[in]	user_prm    The user specific parameter
@return
*/
static void zwif_fw_tx_cb(appl_layer_ctx_t  *appl_ctx, uint8_t tx_sts, void *user_prm)
{
    zwif_p          intf = (zwif_p)user_prm;
    //zwnet_p         nw = (zwnet_p)appl_ctx->data;
    if_fw_tmp_dat_t *fw_data;
    void            *report_cb;
    int             i;

    if (!intf->tmp_data)
    {
        return;
    }

    fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;

    if (tx_sts == TRANSMIT_COMPLETE_OK)
    {
        //Check whether any firmware fragment pending to send
        if (fw_data->rpt_cnt > 0)
        {
            report_cb = NULL;

            for (i=0; i<intf->rpt_num; i++)
            {
                if (intf->rpt[i].rpt_cmd == FIRMWARE_UPDATE_MD_GET)
                {
                    report_cb = intf->rpt[i].rpt_cb;
                    break;
                }
            }

            if (report_cb)
            {
                zw_fw_tx_fn     fw_tx_fn;

                fw_tx_fn = (zw_fw_tx_fn)report_cb;
                fw_tx_fn(intf, fw_data->rpt_num, fw_data->rpt_cnt);
            }
        }
    }
}


/**
zwif_fw_tx - Send firmware fragment
@param[in]	intf	    Interface
@param[in]	fw_get_fn   Function pointer for getting firmware fragment
@param[in]	frag_num	Fragment number to send
@param[in]	rpt_cnt	    The number of fragments to send
@return     ZW_ERR_XXX
*/
static int zwif_fw_tx(zwif_p intf, uint16_t frag_num, uint8_t rpt_cnt)
{
    if_fw_tmp_dat_t *fw_data;
    uint8_t         *frag_data;
    uint16_t        frag_sz;
    uint16_t        chksum_sz;
    uint8_t         last_frag;
    zwifd_t         ifd;

    if ((!intf->tmp_data) || (frag_num == 0) || (rpt_cnt == 0))
    {
        return ZW_ERR_VALUE;
    }

    fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;

    if (!fw_data->fw_file)
    {
        return ZW_ERR_FILE_OPEN;
    }

    frag_sz = fw_data->frag_sz;

    //Check whether to stop fragment transfer
    if (frag_sz == 0)
    {
        return ZW_ERR_FILE_EOF;
    }

    frag_data = (uint8_t *)malloc(frag_sz + 8);//allocate additional memory for the command header and checksum

    if (frag_data)
    {
        //Get a firmware fragment from the user application
        zwif_get_desc(intf, &ifd);
        if(!zwif_fw_frag_get(fw_data->fw_file, fw_data->fw_file_sz, frag_num, frag_data + 4, &frag_sz, &last_frag))
        {
            free(frag_data);
            return ZW_ERR_FAILED;
        }

        //Add header
        frag_data[0] = COMMAND_CLASS_FIRMWARE_UPDATE_MD;
        frag_data[1] = FIRMWARE_UPDATE_MD_REPORT;
        frag_data[2] = frag_num >> 8;
        frag_data[3] = frag_num & 0x00FF;
        if (last_frag)
        {
            frag_data[2] |= 0x80;
        }

        //Append checksum
        chksum_sz = 0;
        if (intf->ver > 1)
        {
            uint16_t crc;

            crc = zwutl_crc16_chk(CRC_INIT, frag_data, 4 + frag_sz);
            frag_data[4 + frag_sz] = (crc >> 8);
            frag_data[5 + frag_sz] = (crc & 0x00FF);
            chksum_sz = 2;
        }
        //Save the next firmware fragment info to send
        fw_data->rpt_cnt = (last_frag)? 0 : (rpt_cnt - 1);
        fw_data->rpt_num = frag_num + 1;

        //Send the firmware fragment
        zwif_exec_ex(&ifd, frag_data, 4 + frag_sz + chksum_sz,
                     zwif_fw_tx_cb, intf, ZWIF_OPT_SKIP_ALL_IMD, NULL);

        free(frag_data);
    }
    return ZW_ERR_NONE;
}


/**
zwif_fw_updt_req - request firmware update
@param[in]	ifd	        interface
@param[in]	req	        firmware update request
@return	ZW_ERR_XXX
@note Caller should call zwif_fw_info_get() first before calling this function.
*/
int zwif_fw_updt_req(zwifd_p ifd, zwfw_updt_req_t *req)
{
    int     result;

    if (ifd->cls != COMMAND_CLASS_FIRMWARE_UPDATE_MD)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Setup report callbacks
    result = zwif_set_report(ifd, req->sts_cb, FIRMWARE_UPDATE_MD_REQUEST_REPORT);
    if (result < 0)
    {
        return result;
    }

    result = zwif_set_report(ifd, req->cmplt_cb, FIRMWARE_UPDATE_MD_STATUS_REPORT);
    if (result < 0)
    {
        return result;
    }

    result = zwif_set_report(ifd, zwif_fw_tx, FIRMWARE_UPDATE_MD_GET);

    if (result == 0)
    {
        zwif_p          intf;
        if_fw_tmp_dat_t *fw_data;
        uint8_t         param[10];

        intf = zwif_get_if(ifd);
        if (!intf)
        {
            return ZW_ERR_INTF_NOT_FOUND;
        }

        param[0] = req->vid >> 8;
        param[1] = req->vid & 0xFF;
        param[2] = req->fw_id >> 8;
        param[3] = req->fw_id & 0xFF;
        param[4] = req->chksum >> 8;
        param[5] = req->chksum & 0xFF;

        if (ifd->ver < 3)
        {
            uint16_t    dflt_frag_sz;

            dflt_frag_sz = (intf->ver == 1)? ZW_FW_FRAG_SZ_V1 : ZW_FW_FRAG_SZ_V2;

            if ((req->fw_tgt != 0) || (req->frag_sz != dflt_frag_sz))
            {
                return ZW_ERR_VALUE;
            }
        }
        else
        {
            param[6] = req->fw_tgt;
            param[7] = req->frag_sz >> 8;
            param[8] = req->frag_sz & 0xFF;

        }

        //Check and save requested fragment size
        if (intf->tmp_data)
        {
            fw_data = (if_fw_tmp_dat_t *)intf->tmp_data;

            if (fw_data->fixed_frag_sz)
            {   //Fixed fragment size
                if (req->frag_sz != fw_data->max_frag_sz)
                {
                    return ZW_ERR_VALUE;
                }
            }
            else
            {   //Variable fragment size
                if ((req->frag_sz > fw_data->max_frag_sz)
                    //|| (req->frag_sz < 40))   //testing
                    || (req->frag_sz < ZW_FW_FRAG_SZ_MIN_V3))
                {
                    return ZW_ERR_VALUE;
                }
            }

            fw_data->frag_sz = req->frag_sz;

            //Open the firmware file
            if (!req->fw_file)
            {
                return ZW_ERR_FILE_OPEN;
            }

            if (fw_data->fw_file)
            {
                fclose(fw_data->fw_file);
                fw_data->fw_file = NULL;
            }

        #ifdef USE_SAFE_VERSION
            if (fopen_s(&fw_data->fw_file, req->fw_file, "rb") != 0)
            {
                return ZW_ERR_FILE_OPEN;
            }
        #else
            fw_data->fw_file = fopen(req->fw_file, "rb");
            if (!fw_data->fw_file)
            {
                return ZW_ERR_FILE_OPEN;
            }
        #endif

            //Get file size
            fseek(fw_data->fw_file, 0, SEEK_END); // seek to end of file
            fw_data->fw_file_sz = ftell(fw_data->fw_file);

            //Check for zero-size file
            if (fw_data->fw_file_sz == 0)
            {
                fclose(fw_data->fw_file);
                fw_data->fw_file = NULL;
                return ZW_ERR_FILE_EOF;
            }

            fseek(fw_data->fw_file, 0, SEEK_SET); // seek to beginning of file

        }
        else
        {
            return ZW_ERR_INTF_NO_DATA;
        }

        //Save firmware target restart callback
        intf->ep->node->restart_cb = req->restart_cb;
        intf->ep->node->poll_tgt_cnt = FW_UPDT_RESTART_POLL_MAX;

        result = zwif_cmd_id_set(ifd, ZW_CID_FW_UPDT_GET, 1);

        //Request for report
        if ( result == 0)
        {
            result = zwif_get_report(ifd, param, (ifd->ver < 3)? 6 : 9,
                                     FIRMWARE_UPDATE_MD_REQUEST_GET, zwif_exec_cb);
        }

        if ( result < 0)
        {
            fclose(fw_data->fw_file);
            fw_data->fw_file = NULL;
            return result;
        }
    }
    return result;
}


/**
zwif_get_report_poll - get interface report through report callback
@param[in]	ifd	        interface
@param[in]	param	    Parameter for the report get command
@param[in]	len     	Length of param
@param[in]	get_rpt_cmd Command to get the report
@param[in, out] poll_req Poll request
@return		ZW_ERR_xxx
*/
static int zwif_get_report_poll(zwifd_p ifd, uint8_t *param, uint8_t len, uint8_t get_rpt_cmd,
                                zwpoll_req_t *poll_req)
{
    int             result;
    int             hdr_len;
    poll_q_ent_t    *poll_ent;

    //Check if the parameter length is too long
    if (len > 29)
    {
        return ZW_ERR_MEMORY;
    }

    poll_ent = (poll_q_ent_t *)calloc(1, sizeof(poll_q_ent_t) + 32);

    if (!poll_ent)
    {
        return ZW_ERR_MEMORY;
    }

    //Check for extended command class (2-byte command class)
    if (ifd->cls & 0xFF00)
    {
        hdr_len = 3;
        poll_ent->dat_buf[0] = ifd->cls >> 8 ;
        poll_ent->dat_buf[1] = (ifd->cls & 0x00FF);
        poll_ent->dat_buf[2] = get_rpt_cmd;
    }
    else
    {
        hdr_len = 2;
        poll_ent->dat_buf[0] = (uint8_t)ifd->cls;
        poll_ent->dat_buf[1] = get_rpt_cmd;
    }

    if (len > 0)
    {
        memcpy(poll_ent->dat_buf + hdr_len, param, len);
    }

    poll_ent->usr_token = poll_req->usr_token;
    poll_ent->interval = (poll_req->interval == 0)? MIN_POLL_TIME : (poll_req->interval * POLL_TICK_PER_SEC);
    poll_ent->poll_cnt = poll_req->poll_cnt;
    poll_ent->ifd = *ifd;
    poll_ent->node_id = ifd->nodeid;
    poll_ent->dat_len = len + hdr_len;

    result = zwpoll_add(ifd->net, poll_ent);
    //Save the polling handle
    poll_req->handle = poll_ent->handle;

    free(poll_ent);

    if (result < 0)
    {
        debug_zwapi_msg(&ifd->net->plt_ctx, "zwif_get_report_poll with error:%d", result);
    }
    return result;

}

/**
@}
@defgroup If_Power_level Power level Interface APIs
Used to test power level of the target device
@{
*/

/**
zwif_power_level_rpt_set - setup power level report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_power_level_rpt_set(zwifd_p ifd, zwrep_power_level_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_POWERLEVEL)
    {
        return zwif_set_report(ifd, rpt_cb, POWERLEVEL_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}

/**
zwif_power_level_test_rpt_set - setup power level test report callback function
@param[in]	ifd         interface
@param[in]	rpt_cb	    report callback function
return      ZW_ERR_XXX
*/
int zwif_power_level_test_rpt_set(zwifd_p ifd, zwrep_power_level_test_fn rpt_cb)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_POWERLEVEL)
    {
        return zwif_set_report(ifd, rpt_cb, POWERLEVEL_TEST_NODE_REPORT);
    }
    return ZW_ERR_CLASS_NOT_FOUND;

}


/**
zwif_power_level_get - get the power level value in use by the node through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_power_level_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_POWERLEVEL)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_POWER_LVL_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               POWERLEVEL_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_power_level_set - set the power level which should be used by the node when transmitting RF
@param[in]	ifd	      interface
@param[in]	lvl		  Power level. Ranges from 0 to 9. 0=normal power; 1= -1dbm; 2= -2dbm, etc.
@param[in]	timeout   Time out value (in seconds) ranges from 1-255 before resetting to normal power level.
@return	ZW_ERR_XXX
*/
int zwif_power_level_set(zwifd_p ifd, uint8_t lvl, uint8_t timeout)
{
    int         result;
	uint8_t     cmd[4];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_POWERLEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

	//Check input values
    if (timeout == 0 || lvl > POWERLEVEL_REPORT_MINUS9DBM)
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_POWER_LVL_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_POWERLEVEL;
    cmd[1] = POWERLEVEL_SET;
    cmd[2] = lvl;
	cmd[3] = timeout;

    //Send the command
    return zwif_exec(ifd, cmd, 4, zwif_exec_cb);
}


/**
zwif_power_level_test_get - get the result of power level test through report callback
@param[in]	ifd	        interface
@return		ZW_ERR_XXX
*/
int zwif_power_level_test_get(zwifd_p ifd)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_POWERLEVEL)
    {
        int result;
        result = zwif_cmd_id_set(ifd, ZW_CID_POWER_LVL_TST_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, NULL, 0,
                               POWERLEVEL_TEST_NODE_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
zwif_power_level_test_set - set the power level test information and start the test
@param[in]	ifd			interface
@param[in]	node		Node descriptor of the test node which should recieves the transmitted test frames.
@param[in]	lvl		    Power level. Ranges from 0 to 9. 0=normal power; 1= -1dbm; 2= -2dbm, etc.
@param[in]	frame_cnt   Test frame count to be carried out. (1-65535)
@return	ZW_ERR_XXX
*/
int zwif_power_level_test_set(zwifd_p ifd, zwnoded_p noded, uint8_t lvl, uint16_t frame_cnt)
{
    int         result;
	uint8_t     cmd[6];

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_POWERLEVEL)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

	//Check input values
    if ((noded->nodeid == 0) || (lvl > POWERLEVEL_REPORT_MINUS9DBM) || (frame_cnt == 0))
    {
        return ZW_ERR_VALUE;
    }

    result = zwif_cmd_id_set(ifd, ZW_CID_POWER_LVL_TST_SET, 1);
    if ( result < 0)
    {
        return result;
    }

    //Prepare the command
    cmd[0] = COMMAND_CLASS_POWERLEVEL;
    cmd[1] = POWERLEVEL_TEST_NODE_SET;
	cmd[2] = noded->nodeid;
    cmd[3] = lvl;
	cmd[4] = frame_cnt >> 8;
	cmd[5] = (uint8_t)(frame_cnt & 0x00FF);

    //Send the command
    return zwif_exec(ifd, cmd, 6, zwif_exec_cb);
}


/**
@}
@defgroup If_Doorlock_log Doorlock logging Interface APIs
Used to report doorlock access events
@{
*/


/**
zwif_drlog_rec_sup_get - get the maximum number of records supported
@param[in]	ifd	    interface
@param[in]	cb	    report callback function
@return ZW_ERR_XXX
*/
int zwif_drlog_rec_sup_get(zwifd_p ifd, zwrep_drlog_rep_fn cb)
{
    int result;

    //Check whether the interface belongs to the right command class
    if (ifd->cls != COMMAND_CLASS_DOOR_LOCK_LOGGING)
    {
        return ZW_ERR_CLASS_NOT_FOUND;
    }

    //Check whether to use cached data
    if (zwif_cached_cb(ifd, cb, CB_RPT_TYP_MAX_DRLOG_REC, 0))
    {
        return ZW_ERR_QUEUED;
    }

    //Setup report callback
    result = zwif_set_report(ifd, cb, DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT);

    if (result == 0)
    {
        result = zwif_cmd_id_set(ifd, ZW_CID_DRLOG_REC_SUP_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        //Request for report
        result = zwif_get_report(ifd, NULL, 0,
                                 DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET, zwif_exec_cb);
    }
    return result;

}


/**
zwif_drlog_rec_get - get the door lock logging record through report callback
@param[in]	ifd	        interface
@param[in]	rec_num     record number to request. Valid value: 0 to the maximum number of records supported.
                        Zero is used to retrieve the most recent record.
@return		ZW_ERR_XXX
*/
int zwif_drlog_rec_get(zwifd_p ifd, uint8_t rec_num)
{
    //Check whether the command class is correct
    if (ifd->cls == COMMAND_CLASS_DOOR_LOCK_LOGGING)
    {
        int result;

        result = zwif_cmd_id_set(ifd, ZW_CID_DRLOG_REC_GET, 1);
        if ( result < 0)
        {
            return result;
        }

        return zwif_get_report(ifd, &rec_num, 1,
                               RECORD_GET, zwif_exec_cb);
    }
    return ZW_ERR_CLASS_NOT_FOUND;
}


/**
@}
*/

