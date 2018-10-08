/**
@file   zw_api_test_intf2.c - Z-wave host controller API test program.

        Interfaces (Command Classes) part of the test program.

@author David Chow

@version    1.0 13-4-11  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include "zw_api_test.h"


const char *thrmo_fan_md_str[] =
{
    "auto low",
    "low",
    "auto high",
    "high",
    "auto medium",
    "medium",
    "circulation",
    "humidity circulation",
    "unknown"
};

const char *thrmo_fan_sta_str[] =
{
    "idle",
    "running low",
    "running high",
    "unknown"
};

const char *thrmo_md_str[] =
{
    "off",
    "heating only",
    "cooling only",
    "auto",
    "auxiliary heat",
    "resume from last mode",
    "fan only",
    "furnace only",
    "dry air",
    "moisturize air",
    "auto changeover",
    "heating with energy saving",
    "cooling with energy saving",
    "away from home",
    "unknown"
};

const char *thrmo_op_sta_str[] =
{
    "idle",
    "heating",
    "cooling",
    "fan only",
    "pending heat",
    "pending cool",
    "vent / economizer",
    "unknown"
};

const char *thrmo_setb_sta_str[] =
{
    "Setback in 1/10 degrees (Kelvin)",
    "Frost Protection",
    "Energy Saving Mode",
    "Unused",
    "unknown"
};

const char *thrmo_setb_typ_str[] =
{
    "No override",
    "Temporary override",
    "Permanent override",
    "unknown"
};

const char *thrmo_setp_str[] =
{
    "invalid",
    "heating",
    "cooling",
    "invalid",
    "invalid",
    "invalid",
    "invalid",
    "furnace",
    "dry air",
    "moisturize air",
    "auto changeover",
    "heating with energy saving",
    "cooling with energy saving",
    "away from home",
    "unknown"
};

const char *day_str[] =
{
    "undefined",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

const char *lprot_str[] =
{
    "Unprotected",
    "Protection by sequence",
    "No operation possible",
    "unknown"
};

const char *rfprot_str[] =
{
    "Unprotected",
    "No RF control",
    "No RF control and response",
    "unknown"
};

const char *appl_busy_str[] =
{
    "Try again later",
    "Try again in Wait Time seconds",
    "Request queued",
    "unknown"
};

const char *sensor_type_str[] =
{
    "undefined",
    "Temperature sensor",
    "General purpose sensor",
    "Luminance sensor",
    "Power sensor",
    "Relative humidity sensor",
    "Velocity sensor",
    "Direction sensor",
    "Atmospheric pressure sensor",
    "Barometric pressure sensor",
    "Solar radiation sensor",
    "Dew point sensor",
    "Rain rate sensor",
    "Tide level sensor",
    "Weight sensor",
    "Voltage sensor",
    "Current sensor",
    "CO2-level sensor",
    "Air flow sensor",
    "Tank capacity sensor",
    "Distance sensor",
    "Angle Position sensor",
    "Rotation sensor",
    "Water temperature sensor",
    "Soil temperature sensor",
    "Seismic intensity sensor",
    "Seismic magnitude sensor",
    "Ultraviolet sensor",
    "Electrical resistivity sensor",
    "Electrical conductivity sensor",
    "Loudness sensor",
    "Moisture sensor",
    "Frequency sensor",
    "Time sensor",
    "Target temperature sensor"
};


const char *sensor_unit_str[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    "Celsius (C)",    //Air temperature unit
    "Fahrenheit (F)",
    NULL,
    NULL,
    "Percentage",   //General purpose unit
    "Dimensionless value",
    NULL,
    NULL,
    "Percentage",   //Luminance unit
    "Lux",
    NULL,
    NULL,
    "W",      //Power unit
    "Btu/h",
    NULL,
    NULL,
    "Percentage",   //Relative humidity unit
    "g/m3",
    NULL,
    NULL,
    "m/s",      //Velocity unit
    "mph",
    NULL,
    NULL,
    "degrees",      //Direction unit
    NULL,
    NULL,
    NULL,
    "kPa",        //Atmospheric pressure unit
    "inches of Mercury",
    NULL,
    NULL,
    "kPa",        //Barometric pressure unit
    "inches of Mercury",
    NULL,
    NULL,
    "W/m2",         //Solar radiation unit
    NULL,
    NULL,
    NULL,
    "Celsius (C)",    //Dew point unit
    "Fahrenheit (F)",
    NULL,
    NULL,
    "mm/h",   //Rain rate unit
    "in/h",
    NULL,
    NULL,
    "m",      //Tide level unit
    "feet",
    NULL,
    NULL,
    "kg",       //Weight unit
    "pounds",
    NULL,
    NULL,
    "V",      //Voltage unit
    "mV",
    NULL,
    NULL,
    "A",   //Current unit
    "mA",
    NULL,
    NULL,
    "ppm",    //CO2-level unit
    NULL,
    NULL,
    NULL,
    "m3/h",   //Air flow unit
    "cfm",
    NULL,
    NULL,
    "l",    //Tank capacity unit
    "cbm",
    "US gallons",
    NULL,
    "m",      //Distance unit
    "cm",
    "feet",
    NULL,
    "Percentage",     //Angle Position unit
    "Degrees rel. to north pole",
    "Degrees rel. to south pole",
    NULL,
    "rpm",   //Rotation unit
    "Hz",
    NULL,
    NULL,
    "Celsius (C)",    //Water temperature unit
    "Fahrenheit (F)",
    NULL,
    NULL,
    "Celsius (C)",    //Soil temperature unit
    "Fahrenheit (F)",
    NULL,
    NULL,
    "Mercalli",       //Seismic intensity unit
    "European Macroseismic",
    "Liedu",
    "Shindo",
    "Local (ML)",     //Seismic magnitude unit
    "Moment (MW)",
    "Surface wave (MS)",
    "Body wave (MB)",
    "UV index",    //Ultraviolet unit
    NULL,
    NULL,
    NULL,
    "ohm metre",    //Electrical resistivity unit
    NULL,
    NULL,
    NULL,
    "siemens per metre",    //Electrical conductivity unit
    NULL,
    NULL,
    NULL,
    "Absolute loudness (dB)", //Loudness unit
    "A-weighted decibels (dBA)",
    NULL,
    NULL,
    "Percentage", //Moisture unit
    "Volume water content (m3/m3)",
    "Impedance (k ohm)",
    "Water activity (aw)",
    "Hertz (Hz)",           //Frequency unit
    "Kilo Hertz (KHz)",
    NULL,
    NULL,
    "Second (s)",           //Time unit
    NULL,
    NULL,
    NULL,
    "Celsius (C)",          //Target temperature unit
    "Fahrenheit (F)",
    NULL,
    NULL
};


const char *dev_category_str[] =
{
    "undefined",
    "Sensor alarm",
    "On/off switch",
    "Power strip",
    "Siren",
    "Valve",    //5
    "Simple display",
    "Door lock with keypad",
    "Sub energy meter",
    "Advanced whole home energy meter",
    "Simple whole home energy meter",   //10
    "Sensor",
    "Light dimmer switch",
    "Window covering no position/endpoint",
    "Window covering end point aware",
    "Window covering position/end point aware", //15
    "Fan switch",
    "Remote control - multipurpose",
    "Remote control - AV",
    "Remote control - simple",
    "Gateway (unrecognized by client)", //20
    "Central controller",
    "Set top box",
    "TV",
    "Sub system controller",
    "Gateway",  //25
    "Thermostat - HVAC",
    "Thermostat - setback",
    "Wall controller"
};

const char *power_level_ind_str[] =
{
	"Normal power",
	"minus 1dBm",
	"minus 2dBm",
	"minus 3dBm",
	"minus 4dBm",
	"minus 5dBm",
	"minus 6dBm",
	"minus 7dBm",
	"minus 8dBm",
	"minus 9dBm"
};

const char *power_level_test_str[] =
{
	"Test failed",
	"Test success",
	"Test in progress"
};

/**
hl_thrmo_fan_md_rep_cb - Thermostat fan operating mode report callback
@param[in]	ifd	    interface that received the report
@param[in]	off     fan off mode flag. Non-zero indicates that the fan is fully OFF,
                    “0” indicates that it is possible to change between Fan Modes.
@param[in]	mode    fan operating mode, ZW_THRMO_FAN_MD_XXX
@return
*/
void hl_thrmo_fan_md_rep_cb(zwifd_p ifd, uint8_t off, uint8_t mode)
{

    if (off)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat fan is off");
    }
    else
    {
        uint8_t fan_mode;

        fan_mode = (mode <= 7)? mode : 8;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat fan current operating mode:%u(%s)", mode, thrmo_fan_md_str[fan_mode]);
    }
}


/**
hl_thrmo_fan_md_rep_setup - Setup thermostat fan operating mode report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_md_rep_setup(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_md_rpt_set(ifd, hl_thrmo_fan_md_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_md_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_fan_md_rep_get - Get the thermostat fan operating mode
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_md_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_md_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_md_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_fan_md_set - Set the thermostat fan operating mode
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_md_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_md_set(ifd, hl_appl->thrmo_fan_off, hl_appl->thrmo_fan_md);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_md_set with error:%d", result);
    }

    return result;

}


/**
hl_thrmo_fan_md_sup_rep_cb - Report callback for supported thermostat fan operating modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat fan operating modes (ZW_THRMO_FAN_MD_XXX)
*/
void hl_thrmo_fan_md_sup_rep_cb(zwifd_p ifd, uint8_t off, uint8_t mode_len, uint8_t *mode)
{
    uint8_t       i;
    uint8_t       fan_mode;
    hl_appl_ctx_t *hl_appl = (hl_appl_ctx_t *)zwnet_get_user(ifd->net);

    hl_appl->thrmo_fan_off_cap = off;

    if (off)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat fan off mode is supported");
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported thermostat fan mode:");

    for (i=0; i<mode_len; i++)
    {

        fan_mode = (mode[i] <= 7)? mode[i] : 8;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "%u(%s),", mode[i], thrmo_fan_md_str[fan_mode]);
    }

}


/**
hl_thrmo_fan_md_sup_get - Get supported thermostat fan operating modes
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_md_sup_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_md_sup_get(ifd, hl_thrmo_fan_md_sup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_md_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_fan_sta_rep_cb - Thermostat fan operating state report callback
@param[in]	ifd	    interface that received the report
@param[in]	state   fan operating mode, ZW_THRMO_FAN_STA_XXX
@return
*/
void hl_thrmo_fan_sta_rep_cb(zwifd_p ifd, uint8_t state)
{
    uint8_t fan_sta;

    fan_sta = (state <= 2)? state : 3;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat fan current operating state:%u(%s)", state, thrmo_fan_sta_str[fan_sta]);
}


/**
hl_thrmo_fan_sta_rep_setup - Setup thermostat fan operating state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_sta_rep_setup(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_sta_rpt_set(ifd, hl_thrmo_fan_sta_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_sta_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_fan_sta_rep_get - Get the thermostat fan operating state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_fan_sta_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_fan_sta_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_fan_sta_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_md_rep_cb - Thermostat mode report callback
@param[in]	ifd	    interface that received the report
@param[in]	mode    mode, ZW_THRMO_MD_XXX
@return
*/
void hl_thrmo_md_rep_cb(zwifd_p ifd, uint8_t mode)
{

    uint8_t curr_mode;

    curr_mode = (mode <= 13)? mode : 14;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat current mode:%u(%s)", mode, thrmo_md_str[curr_mode]);
}


/**
hl_thrmo_md_rep_setup - Setup thermostat mode report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_md_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_thrmo_md_rpt_set(ifd, hl_thrmo_md_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_md_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_md_rep_get - Get the thermostat mode
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_md_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_md_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_md_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_md_set - Set the thermostat mode
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_md_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_md_set(ifd, hl_appl->thrmo_md);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_md_set with error:%d", result);
    }

    return result;

}


/**
hl_thrmo_md_sup_rep_cb - Report callback for supported thermostat modes
@param[in]	ifd	        interface
@param[in]	off         flag to indicate whether off mode is supported.
@param[in]	mode_len    size of mode buffer
@param[in]	mode        buffer to store supported thermostat modes (ZW_THRMO_MD_XXX)
*/
void hl_thrmo_md_sup_rep_cb(zwifd_p ifd, uint8_t mode_len, uint8_t *mode)
{
    uint8_t       i;
    uint8_t       sup_mode;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported thermostat mode:");

    for (i=0; i<mode_len; i++)
    {
        sup_mode = (mode[i] <= 13)? mode[i] : 14;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "%u(%s),", mode[i], thrmo_md_str[sup_mode]);
    }

}


/**
hl_thrmo_md_sup_get - Get supported thermostat modes
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_md_sup_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_md_sup_get(ifd, hl_thrmo_md_sup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_md_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_op_sta_rep_cb - Thermostat operating state report callback
@param[in]	ifd	    interface that received the report
@param[in]	state   operating state, ZW_THRMO_OP_STA_XXX
@return
*/
void hl_thrmo_op_sta_rep_cb(zwifd_p ifd, uint8_t state)
{
    uint8_t op_sta;

    op_sta = (state <= 6)? state : 7;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat current operating state:%u(%s)", state, thrmo_op_sta_str[op_sta]);
}


/**
hl_thrmo_op_sta_rep_setup - Setup thermostat operating state report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_op_sta_rep_setup(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_op_sta_rpt_set(ifd, hl_thrmo_op_sta_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_op_sta_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_op_sta_rep_get - Get the thermostat operating state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_op_sta_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_op_sta_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_op_sta_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_setb_rep_cb - Thermostat setback report callback
@param[in]	ifd	            interface that received the report
@param[in]	type            setback type, ZW_THRMO_SETB_TYP_XXX
@param[in]	state           setback state, ZW_THRMO_SETB_STA_XXX
@param[in]	tenth_degree	1/10 of a degree.  This parameter is valid only if state equals to ZW_THRMO_SETB_STA_SETB
@return
*/
void hl_thrmo_setb_rep_cb(zwifd_p ifd, uint8_t type, uint8_t state, int8_t tenth_degree)
{

    uint8_t curr_type;
    uint8_t curr_state;
    char    degree[12];

    curr_type = (type <= 2)? type : 3;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setback type:%u(%s)", type, thrmo_setb_typ_str[curr_type]);

    curr_state = (state <= 3)? state : 4;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setback state:%u(%s)", type, thrmo_setb_sta_str[curr_state]);

    if (state == ZW_THRMO_SETB_STA_SETB)
    {
        hl_float_get((int32_t)tenth_degree,  1, 12, degree);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setback value:%s degrees K", degree);
    }

}


/**
hl_thrmo_setb_rep_setup - Setup thermostat setback report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setb_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_thrmo_setb_rpt_set(ifd, hl_thrmo_setb_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setb_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_setb_rep_get - Get the thermostat setback state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setb_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_setb_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setb_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_setb_set - Set the thermostat setback state
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setb_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_setb_set(ifd, hl_appl->thrmo_setb_typ, hl_appl->thrmo_setb_sta, hl_appl->thrmo_setb_deg);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setb_set with error:%d", result);
    }

    return result;

}


/**
hl_thrmo_setp_rep_cb - Thermostat setpoint report callback
@param[in]	ifd	    interface that received the report
@param[in]	data    setpoint data
@return
*/
void hl_thrmo_setp_rep_cb(zwifd_p ifd, zwsetp_p data)
{

    uint8_t curr_type;
    int32_t setp_val;

    curr_type = (data->type <= 13)? data->type : 14;
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setpoint type:%u(%s)", data->type, thrmo_setp_str[curr_type]);

    if (!hl_int_get(data->data, data->size, &setp_val))
    {   //Error
        return;
    }

    if (data->precision == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setpoint value:%d degrees %c", setp_val,
                     (data->unit == ZW_THRMO_SETP_UNIT_C)? 'C' : 'F');
    }
    else
    {
        char    float_str[80];

        hl_float_get(setp_val, data->precision, 80, float_str);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setpoint value:%s degrees %c", float_str, (data->unit == ZW_THRMO_SETP_UNIT_C)? 'C' : 'F');

    }
}


/**
hl_thrmo_setp_rep_setup - Setup thermostat setpoint report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setp_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_thrmo_setp_rpt_set(ifd, hl_thrmo_setp_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setp_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_setp_rep_get - Get the thermostat setpoint
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setp_rep_get(hl_appl_ctx_t   *hl_appl)
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
        result = zwif_thrmo_setp_get_poll(ifd, hl_appl->thrmo_setp_typ, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_thrmo_setp_get(ifd, hl_appl->thrmo_setp_typ);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setp_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_thrmo_setp_set - Set the thermostat setpoint
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setp_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_setp_set(ifd, &hl_appl->thrmo_setp_data);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setp_set with error:%d", result);
    }

    return result;

}


/**
hl_thrmo_setp_sup_rep_cb - Report callback for supported thermostat setpoints
@param[in]	ifd	        interface
@param[in]	type_len    size of type buffer
@param[in]	type        buffer to store supported thermostat setpoint types (ZW_THRMO_SETP_TYP_XXX)
*/
void hl_thrmo_setp_sup_rep_cb(zwifd_p ifd, uint8_t type_len, uint8_t *type)
{
    uint8_t       i;
    uint8_t       sup_type;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported thermostat setpoint type:");

    for (i=0; i<type_len; i++)
    {
        sup_type = (type[i] <= 13)? type[i] : 14;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "%u(%s),", type[i], thrmo_setp_str[sup_type]);
    }

}


/**
hl_thrmo_setp_sup_get - Get supported thermostat setpoint types
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_thrmo_setp_sup_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_thrmo_setp_sup_get(ifd, hl_thrmo_setp_sup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_thrmo_setp_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_clock_report_cb - clock command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	weekday	Day of week
@param[in]	hour	Hour (in 24 hours format)
@param[in]	minute	Minute
@return
*/
void hl_clock_report_cb(zwifd_p ifd, uint8_t weekday, uint8_t hour, uint8_t minute)
{
    if (weekday > 7)
    {
        weekday = 0;
    }
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Reported time: %u:%02u (%s)", hour, minute, day_str[weekday]);
}


/**
hl_clock_rep_setup - Setup clock report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clock_rep_setup(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clock_rpt_set(ifd, hl_clock_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clock_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_clock_rep_get - Get clock report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clock_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clock_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clock_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_clock_set - clock command set value
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clock_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clock_set(ifd, hl_appl->weekday, hl_appl->hour, hl_appl->minute);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clock_set with error:%d", result);
    }

    return result;

}


/**
hl_clmt_ctl_schd_rep_cb - Thermostat climate control schedule report callback
@param[in]	ifd	    interface that received the report
@param[in]	sched   climate control schedule
@return
*/
void hl_clmt_ctl_schd_rep_cb(zwifd_p ifd, zwcc_shed_p sched)
{
    int     i;
    uint8_t curr_state;
    char    degree[12];

    if (sched->weekday > 7)
    {
        sched->weekday = 0;
    }

    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Climate control schedule for: %s", day_str[sched->weekday]);

    for (i=0; i < sched->total; i++)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "----------------------------------------");
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Schedule start time:%u:%u", sched->swpts[i].hour, sched->swpts[i].minute);

        curr_state = (sched->swpts[i].state <= 3)? sched->swpts[i].state : 4;

        plt_msg_show(hl_if_plt_ctx_get(ifd), "Schedule state: %s", thrmo_setb_sta_str[curr_state]);

        if (curr_state == ZW_THRMO_SETB_STA_SETB)
        {
            hl_float_get((int32_t)sched->swpts[i].tenth_deg,  1, 12, degree);
            plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setback value:%s degrees K", degree);
        }

    }
}


/**
hl_clmt_ctl_schd_chg_rep_cb - Climate control schedule change report callback
@param[in]	ifd	            interface
@param[in]	chg_counter     climate control schedule change counter.If the ChangeCounter
                            is different from last time, this indicates a change in a
                            climate control schedule.
                            Value from 1 to 255 indicates the climate control schedule change mechanism is enabled.
                            Value of 0 indicates the climate control schedule change mechanism is temporarily
                            disabled by the override function.
@return
*/
void hl_clmt_ctl_schd_chg_rep_cb(zwifd_p ifd, uint8_t chg_counter)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Climate control schedule change mechanism is: %s",
                    (chg_counter)? "enabled" : "temporarily disabled");

    if (chg_counter)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Change counter = %u", chg_counter);
    }

}


/**
hl_clmt_ctl_schd_ovr_rep_cb - report callback for climate control schedule override
@param[in]	ifd	            interface
@param[in]	schd_ovr        climate control schedule override
@return
*/
void hl_clmt_ctl_schd_ovr_rep_cb(zwifd_p ifd, zwcc_shed_ovr_p schd_ovr)
{
    uint8_t curr_state;
    uint8_t curr_type;
    char    degree[12];

    curr_state = (schd_ovr->state <= 3)? schd_ovr->state : 4;
    curr_type = (schd_ovr->type <= 2)? schd_ovr->type : 3;

    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Climate control schedule override type: %s", thrmo_setb_typ_str[curr_type]);
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Override state: %s", thrmo_setb_sta_str[curr_state]);
    if (curr_state == ZW_THRMO_SETB_STA_SETB)
    {
        hl_float_get((int32_t)schd_ovr->tenth_deg,  1, 12, degree);
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Thermostat setback value:%s degrees K", degree);
    }
}

/**
hl_clmt_ctl_schd_rep_setup - Setup climate control schedule, change counter and override reports
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_clmt_ctl_schd_rpt_set(ifd, hl_clmt_ctl_schd_rep_cb);
    if (result == 0)
    {
        result = zwif_clmt_ctl_schd_chg_rpt_set(ifd, hl_clmt_ctl_schd_chg_rep_cb);
    }

    if (result == 0)
    {
        result = zwif_clmt_ctl_schd_ovr_rpt_set(ifd, hl_clmt_ctl_schd_ovr_rep_cb);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_clmt_ctl_schd_rep_get - Get the climate control schedule
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clmt_ctl_schd_get(ifd, hl_appl->weekday);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_clmt_ctl_schd_set - Set the thermostat climate control schedule
@param[in]	hl_appl	   The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clmt_ctl_schd_set(ifd, &hl_appl->clmt_ctl_schd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_set with error:%d", result);
    }

    return result;

}


/**
hl_clmt_ctl_schd_chg_rep_get - Get the climate control schedule change counter
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_chg_rep_get(hl_appl_ctx_t   *hl_appl)
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
        result = zwif_clmt_ctl_schd_chg_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_clmt_ctl_schd_chg_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_chg_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_clmt_ctl_schd_ovr_rep_get - Get the climate control schedule override
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_ovr_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clmt_ctl_schd_ovr_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_ovr_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_clmt_ctl_schd_ovr_set - Set the climate control schedule override
@param[in]	hl_appl	   The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_clmt_ctl_schd_ovr_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_clmt_ctl_schd_ovr_set(ifd, &hl_appl->clmt_ctl_schd_ovr);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_clmt_ctl_schd_ovr_set with error:%d", result);
    }

    return result;

}


/**
hl_mul_cmd_ctl_set - Start or stop multi command encapsulation
@param[in]	hl_appl		    The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_mul_cmd_ctl_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwnode_mul_cmd_ctl_set(noded, hl_appl->mul_cmd_ctl);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_mul_cmd_ctl_set with error:%d", result);
    }

    return result;

}


/**
hl_mul_cmd_ctl_get - Get the current state of multi command encapsulation
@param[in]	hl_appl		The high-level api context
@param[out]	on	        state: 1=on; 0=off
@return  0 on success, negative error number on failure
*/
int hl_mul_cmd_ctl_get(hl_appl_ctx_t   *hl_appl, uint8_t *on)
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

    result = zwnode_mul_cmd_ctl_get(noded, on);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_mul_cmd_ctl_get with error:%d", result);
    }
    else
    {
        plt_msg_show(hl_plt_ctx_get(hl_appl), "Multi command encapsulation is:%s", (*on)? "on" : "off");
    }

    return result;

}


/**
hl_prot_rep_cb - Protection states report callback
@param[in]	ifd	        interface that received the report
@param[in]	local_prot  local protection state, ZW_LPROT_XXX
@param[in]	rf_prot     RF protection state, ZW_RFPROT_XXX.
@return
*/
void hl_prot_rep_cb(zwifd_p ifd, uint8_t local_prot, uint8_t rf_prot)
{

    uint8_t state;

    state = (local_prot <= 2)? local_prot : 3;
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Local protection state:%u(%s)", local_prot, lprot_str[state]);

    state = (rf_prot <= 2)? rf_prot : 3;
    plt_msg_show(hl_if_plt_ctx_get(ifd), "RF protection state:%u(%s)", rf_prot, rfprot_str[state]);
}


/**
hl_prot_ec_rep_cb - Protection exclusive control node report callback
@param[in]	ifd	        interface that received the report
@param[in]	node_id     node ID that has exclusive control can override the RF protection state
                        of the device and can control it regardless of the protection state.
                        Node id of zero is used to reset the protection exclusive control state.
@return
*/
void hl_prot_ec_rep_cb(zwifd_p ifd, uint8_t node_id)
{
    if (node_id == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Protection exclusive control is inactive");
    }
    else
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Protection exclusive control node:%u", node_id);
    }
}


/**
hl_prot_tmout_rep_cb - RF protection timeout report callback
@param[in]	ifd	        interface that received the report
@param[in]	remain_tm   remaining time. 0x00 = No timer is set. All “normal operation” Commands must be accepted.
                        0x01 to 0x3C = 1 second (0x01) to 60 seconds (0x3C);
                        0x41 to 0xFE = 2 minutes (0x41) to 191 minutes (0xFE);
                        0xFF = No Timeout - The Device will remain in RF Protection mode infinitely.
@return
*/
void hl_prot_tmout_rep_cb(zwifd_p ifd, uint8_t remain_tm)
{
    if (remain_tm == 0)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "No timer is set for RF protection timeout");
    }
    else if ((remain_tm > 0) && (remain_tm <= 0x3C))
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "RF protection will timeout in %u seconds", remain_tm);
    }
    else if ((remain_tm >= 0x41) && (remain_tm <= 0xFE))
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "RF protection will timeout in %u minutes", remain_tm - 63);
    }
    else if (remain_tm == 0xFF)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "RF protection is always on");
    }
}


/**
hl_prot_rep_setup - Setup protection report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_prot_rpt_set(ifd, hl_prot_rep_cb);

    if (result == 0)
    {
        result = zwif_prot_ec_rpt_set(ifd, hl_prot_ec_rep_cb);
    }

    if (result == 0)
    {
        result = zwif_prot_tmout_rpt_set(ifd, hl_prot_tmout_rep_cb);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_prot_rep_get - Get the protection states
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_rep_get(hl_appl_ctx_t   *hl_appl)
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
        result = zwif_prot_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_prot_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_prot_set - Set the protection
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_set(ifd, hl_appl->local_prot, hl_appl->rf_prot);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_set with error:%d", result);
    }

    return result;

}


/**
hl_prot_sup_rep_cb - Report callback for supported protection states
@param[in]	ifd	        interface
@param[in]	sup_sta     supported Protection States
*/
void hl_prot_sup_rep_cb(zwifd_p ifd, zwprot_sup_p sup_sta)
{
    uint8_t       i;

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported Local Protection states:");

    for (i=0; i<sup_sta->lprot_len; i++)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", lprot_str[sup_sta->lprot[i]]);
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Supported RF Protection states:");

    for (i=0; i<sup_sta->rfprot_len; i++)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "%s", rfprot_str[sup_sta->rfprot[i]]);
    }

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Additional RF supported protection types:");

    if (sup_sta->excl_ctl)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Exclusive Control");
    }

    if (sup_sta->tmout)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Timeout");
    }

}


/**
hl_prot_sup_get - Get supported protection states
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_sup_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_sup_get(ifd, hl_prot_sup_rep_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_sup_get with error:%d", result);
    }

    return result;
}


/**
hl_prot_ec_rep_get - Get the protection exclusive control node
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_ec_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_ec_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_ec_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_prot_ec_set - Set the protection exclusive control node
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_ec_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_ec_set(ifd, hl_appl->node_id);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_ec_set with error:%d", result);
    }

    return result;

}


/**
hl_prot_tmout_rep_get - Get the RF protection timeout
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_tmout_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_tmout_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_tmout_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_prot_tmout_set - Set the RF protection timeout
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_prot_tmout_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_prot_tmout_set(ifd, hl_appl->time);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_prot_tmout_set with error:%d", result);
    }

    return result;

}


/**
hl_appl_busy_rep_cb - Application busy status report callback
@param[in]	ifd	        interface that received the report
@param[in]	status      busy status, ZW_BSY_STS_XXX
@param[in]	wait_tm     wait time in seconds; only valid if status = ZW_BSY_STS_TRY_WAIT
@return
*/
void hl_appl_busy_rep_cb(zwifd_p ifd, uint8_t status, uint8_t wait_tm)
{
    uint8_t curr_sts;

    curr_sts = (status <= 2)? status : 3;
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Application busy status:%u(%s)", status, appl_busy_str[curr_sts]);
    if (status == ZW_BSY_STS_TRY_WAIT)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Wait time:%u s", wait_tm);
    }
}

/**
hl_appl_reject_rep_cb - Application rejected request report callback
@param[in]	ifd	        interface that received the report
@param[in]	status      application rejected request status, ZW_RJ_STS_XXX
@return
*/
void hl_appl_reject_rep_cb(zwifd_p ifd, uint8_t status)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Application rejected request status:%u(%s)", status,
                 (status == ZW_RJ_STS_REJECT)? "Supported command rejected" : "unknown");
}


/**
hl_appl_sts_rep_setup - Setup application status report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_appl_sts_rep_setup(hl_appl_ctx_t  *hl_appl)
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

    result = zwif_appl_busy_rep_set(ifd, hl_appl_busy_rep_cb);

    if (result == 0)
    {
        result = zwif_appl_reject_rep_set(ifd, hl_appl_reject_rep_cb);
    }


    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_appl_sts_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_ind_report_cb - Indicator report callback
@param[in]	ifd	    The interface that received the report
@param[in]	value	The value
@return
*/
void hl_ind_report_cb(zwifd_p ifd, uint8_t value)
{
    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Indicator value is %02Xh", value);
}


/**
hl_ind_rep_setup - Setup indicator report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ind_rep_setup(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_ind_rpt_set(ifd, hl_ind_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ind_rep_setup with error:%d", result);
    }

    return result;
}


/**
hl_ind_rep_get - Get indicator report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ind_rep_get(hl_appl_ctx_t   *hl_appl)
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
        result = zwif_ind_get_poll(ifd, &hl_appl->poll_req);
        if (result == 0)
        {
            plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Polling request handle:%u", hl_appl->poll_req.handle);
        }
    }
    else
    {
        result = zwif_ind_get(ifd);
    }

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ind_rep_get with error:%d", result);
    }

    return result;
}


/**
hl_ind_set - indicator set value
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_ind_set(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_ind_set(ifd, (uint8_t)hl_appl->ind_val);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_ind_set with error:%d", result);
    }

    return result;

}


/**
hl_fw_info_report_cb - Firmwares information report callback
@param[in]	ifd	    The interface that received the report
@param[in]	fw_info	Firmwares information
@return
*/
void hl_fw_info_report_cb(zwifd_p ifd, zwfw_info_t *fw_info)
{
    plt_msg_show(hl_if_plt_ctx_get(ifd), "Vendor id: %04Xh, z-wave firmware id: %04Xh, checksum: %04Xh",
                 fw_info->vid, fw_info->zw_fw_id, fw_info->chksum);

    plt_msg_show(hl_if_plt_ctx_get(ifd), "Max fragment size: %u(%s), upgradable: %s",
                 fw_info->max_frag_sz, (fw_info->fixed_frag_sz)? "fixed" : "variable",
                 (fw_info->upgrade_flg == ZW_FW_UPGD_YES)? "Y" : "N");

    if (fw_info->other_fw_cnt)
    {
        int i;
        for (i=0; i<fw_info->other_fw_cnt; i++)
        {
            plt_msg_show(hl_if_plt_ctx_get(ifd), "Firmware target %d id: %04Xh",
                         i+1, fw_info->other_fw_id[i]);

        }
    }
}


/**
hl_fw_info_get - Get firmwares information
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_fw_info_get(hl_appl_ctx_t *hl_appl)
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

    result = zwif_fw_info_get(ifd, hl_fw_info_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_fw_info_get with error:%d", result);
    }

    return result;
}

#define POLY        0x1021      /**< crc-ccitt mask */
#define CRC_INIT    0x1D0F      /**< crc-ccitt initialization value */
/**
hl_chksum_get - calculate checksum
@param[in]	file	    File to be checked
@return         checksum
*/
static uint16_t hl_chksum_get(FILE *file)
{
    int         ch;
    uint16_t    crc;
    uint8_t     tmp_data;
    uint8_t     bitmask;
    uint8_t     new_bit;

    crc = CRC_INIT;

    while ((ch = fgetc(file)) != EOF )
    {
        tmp_data = ch;
        for (bitmask = 0x80; bitmask != 0; bitmask >>= 1)
        {
            //Align test bit with next bit of the message byte, starting with msb.
            new_bit = ((tmp_data & bitmask) != 0) ^ ((crc & 0x8000) != 0);
            crc <<= 1;
            if (new_bit)
            {
                crc ^= POLY;
            }
        }
    }
    //Rewind the file pointer
    fseek(file, 0L, SEEK_SET);

    return crc;
}


/**
hl_fw_updt_sts_cb - report callback for firmware update status
@param[in]	ifd	        interface
@param[in]	status      firmware update status
*/
static void hl_fw_updt_sts_cb(zwifd_p ifd, uint8_t status)
{
    const char *fw_updt_sts_str[] =
    {   "Invalid combination of vendor id and firmware id",
        "Need out-of-band authentication event to enable firmware update",
        "The requested Fragment Size exceeds the Max Fragment Size",
        "This firmware target is not upgradable",
        "OK. Valid combination of vendor id and firmware id",
        "Unknown status"
    };
    int sts_idx;

    if (status <= 3)
    {
        sts_idx = status;
    }
    else if (status == ZW_FW_UPDT_ERR_VALID)
    {
        sts_idx = 4;
    }
    else
    {
        sts_idx = 5;
    }

    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Firmware update request status:%s(%u)", fw_updt_sts_str[sts_idx], status);
}


/**
hl_fw_updt_cmplt_cb - report callback for firmware update completion status
@param[in]	ifd	            interface
@param[in]	status          firmware update completion status, ZW_FW_UPDT_CMPLT_XXX
@param[in]	wait_tm         time (in seconds) that is needed before the receiving node becomes
                            available again for communication after the transfer of an image. This parameter is
                            valid only if wait_tm_valid=1
@param[in]	wait_tm_valid   flag to indicate the wait_tm parameter is valid.
*/
static void hl_fw_updt_cmplt_cb(zwifd_p ifd, uint8_t status, uint16_t wait_tm, int wait_tm_valid)
{
    const char *fw_updt_cmplt_str[] =
    {   "Cheksum error in requested firmware",
        "Download of the requested firmware failed",
        "Firmware updated successfully. Device is ready and operational",
        "Firmware updated successfully. Device will power cycle itself",
        "Unknown status"
    };
    int sts_idx;

    if (status <= 1)
    {
        sts_idx = status;
    }
    else if (status == ZW_FW_UPDT_CMPLT_OK_NO_RESTART)
    {
        sts_idx = 2;
    }
    else if (status == ZW_FW_UPDT_CMPLT_OK_RESTART)
    {
        sts_idx = 3;
    }
    else
    {
        sts_idx = 4;
    }

    plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Firmware update completion status:%s(%u)", fw_updt_cmplt_str[sts_idx], (unsigned)status);

    if (wait_tm_valid)
    {
        plt_msg_ts_show(hl_if_plt_ctx_get(ifd), "Expected device reboot time:%u s", (unsigned)wait_tm);
    }
}


/**
hl_fw_updt_restart_cb - report callback for firmware update target restart status
@param[in]	node    node
@param[in]	status  firmware update target restart status, ZW_FW_UPDT_RESTART_XXX
*/
static void hl_fw_updt_restart_cb(zwnoded_p node, uint8_t status)
{
    const char *fw_updt_restart_str[] =
    {   "Node restarted and is ready",
        "Failed"
    };

    plt_msg_ts_show(hl_node_plt_ctx_get(node), "Firmware update target restart status of node %u:%s",
                    node->nodeid,
                    fw_updt_restart_str[status]);
}


/**
hl_fw_updt_req - Request firmware update
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_fw_updt_req(hl_appl_ctx_t *hl_appl)
{
    int                 result;
    zwifd_p             ifd;
    FILE                *fw_file;
    zwfw_updt_req_t     fw_updt_req;


    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->temp_desc);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    //Open firmware file
#ifdef USE_SAFE_VERSION
    if (fopen_s(&fw_file, hl_appl->save_file, "rb") != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Can't open file:%s", hl_appl->save_file);
        return ZW_ERR_FILE_OPEN;
    }
#else
    fw_file = fopen(hl_appl->save_file, "rb");
    if (!fw_file)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "Can't open file:%s", hl_appl->save_file);
        return ZW_ERR_FILE_OPEN;
    }
#endif

    //Calculate checksum
    fw_updt_req.chksum = hl_chksum_get(fw_file);
    fclose(fw_file);


    fw_updt_req.vid = hl_appl->fw_vid;
    fw_updt_req.fw_id = hl_appl->fw_id;
    fw_updt_req.fw_tgt = hl_appl->fw_tgt;
    fw_updt_req.frag_sz = hl_appl->fw_frag_sz;
    fw_updt_req.fw_file = hl_appl->save_file;
    fw_updt_req.sts_cb = hl_fw_updt_sts_cb;
    fw_updt_req.cmplt_cb = hl_fw_updt_cmplt_cb;
    fw_updt_req.restart_cb = hl_fw_updt_restart_cb;

    result = zwif_fw_updt_req(ifd, &fw_updt_req);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_fw_updt_req with error:%d", result);
    }

    return result;
}


/**
hl_poll_rm - Remove polling requests
@param[in]	hl_appl		high-level api context
@return  0 on success, negative error number on failure
*/
int hl_poll_rm(hl_appl_ctx_t *hl_appl)
{
    int                 result;

    if (hl_appl->poll_id_type == 0)
    {   //Handle
        result = zwnet_poll_rm(hl_appl->zwnet, hl_appl->poll_id);
    }
    else
    {   //Token
        result = zwnet_poll_rm_mul(hl_appl->zwnet, hl_appl->poll_id);
    }

    if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_poll_rm with error:%d", result);
    }

    return result;
}


/**
hl_power_level_report_cb - power level command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	bylvl	Power level indicator value
@param[in]	bytimeout	Time out value
@return
*/
void hl_power_level_report_cb(zwifd_p ifd, uint8_t bylvl, uint8_t bytimeout)
{
	plt_msg_show(hl_if_plt_ctx_get(ifd), "Power level is %s", power_level_ind_str[bylvl]);
    if (bylvl != POWERLEVEL_REPORT_NORMALPOWER)
    {
        plt_msg_show(hl_if_plt_ctx_get(ifd), "Time out is %u seconds.", bytimeout);
    }
}

/**
hl_power_level_test_node_report_cb - power level test node command report callback
@param[in]	ifd	    The interface that received the report
@param[in]	bylvl	Power level indicator value
@param[in]	bytimeout	Time out value
@return
*/
void hl_power_level_test_node_report_cb(zwifd_p ifd, uint8_t byNodeID, uint8_t byStatus, uint16_t wFramCnt)
{
	if(byNodeID == ZW_TEST_NOT_A_NODEID)
	{
		plt_msg_show(hl_if_plt_ctx_get(ifd), "No power level test has been made.");
	}
	else
	{
		plt_msg_show(hl_if_plt_ctx_get(ifd), "Test node ID: %u. ", byNodeID);
		plt_msg_show(hl_if_plt_ctx_get(ifd), "Status of operation: %s.", power_level_test_str[byStatus]);
		plt_msg_show(hl_if_plt_ctx_get(ifd), "Acknowledged frame count: %u.",  wFramCnt);
	}
}

/**
hl_power_level_rep_setup - Setup power level command report
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_power_level_rep_setup(hl_appl_ctx_t   *hl_appl)
{
    int     result1, result2;
    zwifd_p ifd;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

    result1 = zwif_power_level_rpt_set(ifd, hl_power_level_report_cb);
	result2 = zwif_power_level_test_rpt_set(ifd, hl_power_level_test_node_report_cb);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result1 != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl),
						"hl_power_level_rep_setup with error:%d for set up power level report",
						result1);
    }

	if (result2 != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl),
						"hl_power_level_rep_setup with error:%d for set up power level test report",
						result2);
    }

    return result1 + result2;
}

/**
hl_power_level_rep_get - Get the power level
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_power_level_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_power_level_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result == 1)
	{
		plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_rep_get command queued.");
	}
    else if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_rep_get with error:%d", result);
    }

    return result;
}

/**
hl_power_level_set - Set the power level value with time out
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_power_level_set(hl_appl_ctx_t   *hl_appl)
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

	result = zwif_power_level_set(ifd, hl_appl->pow_lvl, hl_appl->pow_lvl_timeout);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

	if (result == 1)
	{
		plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_set command queued.");
	}
    else if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_set with error:%d", result);
    }

    return result;

}

/**
hl_power_level_test_rep_get - Get the result of the latest Powerlevel Test
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_power_level_test_rep_get(hl_appl_ctx_t   *hl_appl)
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

    result = zwif_power_level_test_get(ifd);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result == 1)
	{
		plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_test_rep_get command queued.");
	}
    else if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_test_rep_get with error:%d", result);
    }

    return result;
}

/**
hl_power_level_test_set - Set the power level test information
@param[in]	hl_appl		The high-level api context
@return  0 on success, negative error number on failure
*/
int hl_power_level_test_set(hl_appl_ctx_t   *hl_appl)
{
	int         result;
    zwifd_p		ifd;
	zwnoded_p   noded;

    //Get the interface descriptor
    plt_mtx_lck(hl_appl->desc_cont_mtx);
    ifd = hl_intf_desc_get(hl_appl->desc_cont_hd, hl_appl->rep_desc_id);
    if (!ifd)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_INTF_NOT_FOUND;
    }

	noded = hl_node_desc_get(hl_appl->desc_cont_hd, hl_appl->pow_lvl_test_node_desc_id);
    if (!noded)
    {
        plt_mtx_ulck(hl_appl->desc_cont_mtx);
        return ZW_ERR_NODE_NOT_FOUND;
    }

	result = zwif_power_level_test_set(ifd,
									   noded,
									   hl_appl->pow_lvl,
									   hl_appl->pow_lvl_test_wframcnt);

    plt_mtx_ulck(hl_appl->desc_cont_mtx);

    if (result == 1)
	{
		plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_test_set command queued.");
	}
    else if (result != 0)
    {
        plt_msg_ts_show(hl_plt_ctx_get(hl_appl), "hl_power_level_test_set with error:%d", result);
    }

    return result;

}