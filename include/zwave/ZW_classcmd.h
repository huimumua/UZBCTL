// Generated on: 7/9/2013 12:09:34 PM
/*******************************  ZW_classcmd.h  *******************************
 *           #######
 *           ##  ##
 *           #  ##    ####   #####    #####  ##  ##   #####
 *             ##    ##  ##  ##  ##  ##      ##  ##  ##
 *            ##  #  ######  ##  ##   ####   ##  ##   ####
 *           ##  ##  ##      ##  ##      ##   #####      ##
 *          #######   ####   ##  ##  #####       ##  #####
 *                                           #####
 *          Z-Wave, the wireless language.
 *
 *              Copyright (c) 2001
 *              Zensys A/S
 *              Denmark
 *
 *              All Rights Reserved
 *
 *    This source file is subject to the terms and conditions of the
 *    Zensys Software License Agreement which restricts the manner
 *    in which it may be used.
 *
 *---------------------------------------------------------------------------
 *
 * Description: Device and command class types and definitions.
 *
 *              Be aware that temporary types and definitions can be present
 *              in this header file. Only the types and definitions found in
 *              the latest version of the Z-Wave Device Class Specification
 *              (doc. nr. 9031002xx) is valid.
 *
 * Author: Jшrgen Franck
 *
 * Last Changed By:  $Author$
 * Revision:         $Revision$
 * Last Changed:     $Date$
 *
 ****************************************************************************/
#ifndef _ZW_CLASSCMD_H_
#define _ZW_CLASSCMD_H_

/****************************************************************************
 *    TYPES and DEFINITIONS
 ***************************************************************************/
/************ Basic Device Class identifiers **************/
#define BASIC_TYPE_CONTROLLER                                                            0x01 /*Node is a portable controller */
#define BASIC_TYPE_ROUTING_SLAVE                                                         0x04 /*Node is a slave with routing capabilities*/
#define BASIC_TYPE_SLAVE                                                                 0x03 /*Node is a slave*/
#define BASIC_TYPE_STATIC_CONTROLLER                                                     0x02 /*Node is a static controller*/


/***** Generic and Specific Device Class identifiers ******/
/* Device class Av Control Point */
#define GENERIC_TYPE_AV_CONTROL_POINT                                                    0x03 /*AV Control Point*/
#define SPECIFIC_TYPE_DOORBELL                                                           0x12
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER                                                 0x04 /*Satellite Receiver*/
#define SPECIFIC_TYPE_SATELLITE_RECEIVER_V2                                              0x11 /*Satellite Receiver V2*/

/* Device class Display */
#define GENERIC_TYPE_DISPLAY                                                             0x04
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_SIMPLE_DISPLAY                                                     0x01 /*Display (simple) Device Type*/

/* Device class Entry Control */
#define GENERIC_TYPE_ENTRY_CONTROL                                                       0x40 /*Entry Control*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class Not Used*/
#define SPECIFIC_TYPE_DOOR_LOCK                                                          0x01 /*Door Lock*/
#define SPECIFIC_TYPE_ADVANCED_DOOR_LOCK                                                 0x02 /*Advanced Door Lock*/
#define SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK                                            0x03 /*Door Lock (keypad –lever) Device Type*/
#define SPECIFIC_TYPE_SECURE_KEYPAD_DOOR_LOCK_DEADBOLT                                   0x04 /*Door Lock (keypad – deadbolt) Device Type*/

/* Device class Generic Controller */
#define GENERIC_TYPE_GENERIC_CONTROLLER                                                  0x01 /*Remote Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER                                         0x01 /*Remote Control (Multi Purpose) Device Type*/
#define SPECIFIC_TYPE_PORTABLE_SCENE_CONTROLLER                                          0x02 /*Portable Scene Controller*/
#define SPECIFIC_TYPE_PORTABLE_INSTALLER_TOOL                                            0x03
#define SPECIFIC_TYPE_REMOTE_CONTROL_AV                                                  0x04 /*Remote Control (AV) Device Type*/
#define SPECIFIC_TYPE_REMOTE_CONTROL_SIMPLE                                              0x06 /*Remote Control (Simple) Device Type*/

/* Device class Meter */
#define GENERIC_TYPE_METER                                                               0x31 /*Meter*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_METER                                                       0x01 /*Sub Energy Meter Device Type*/
#define SPECIFIC_TYPE_ADV_ENERGY_CONTROL                                                 0x02 /*Whole Home Energy Meter (Advanced) Device Type*/
#define SPECIFIC_TYPE_WHOLE_HOME_METER_SIMPLE                                            0x03 /*Whole Home Meter (Simple) Device Type*/

/* Device class Meter Pulse */
#define GENERIC_TYPE_METER_PULSE                                                         0x30 /*Pulse Meter*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Non Interoperable */
#define GENERIC_TYPE_NON_INTEROPERABLE                                                   0xFF /*Non interoperable*/

/* Device class Repeater Slave */
#define GENERIC_TYPE_REPEATER_SLAVE                                                      0x0F /*Repeater Slave*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_REPEATER_SLAVE                                                     0x01 /*Basic Repeater Slave*/

/* Device class Security Panel */
#define GENERIC_TYPE_SECURITY_PANEL                                                      0x17
#define SPECIFIC_TYPE_ZONED_SECURITY_PANEL                                               0x01

/* Device class Semi Interoperable */
#define GENERIC_TYPE_SEMI_INTEROPERABLE                                                  0x50 /*Semi Interoperable*/
#define SPECIFIC_TYPE_ENERGY_PRODUCTION                                                  0x01 /*Energy Production*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/

/* Device class Sensor Alarm */
#define GENERIC_TYPE_SENSOR_ALARM                                                        0xA1
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_ALARM_SENSOR                                        0x05
#define SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR                                        0x0A
#define SPECIFIC_TYPE_BASIC_ROUTING_ALARM_SENSOR                                         0x01
#define SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR                                         0x06
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_ALARM_SENSOR                                      0x03
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR                                      0x08
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ROUTING_ALARM_SENSOR                                               0x02
#define SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR                                               0x07
#define SPECIFIC_TYPE_ZENSOR_NET_ALARM_SENSOR                                            0x04
#define SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR                                            0x09
#define SPECIFIC_TYPE_ALARM_SENSOR                                                       0x0B /*Sensor (Alarm) Device Type*/

/* Device class Sensor Binary */
#define GENERIC_TYPE_SENSOR_BINARY                                                       0x20 /*Binary Sensor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_BINARY                                              0x01 /*Routing Binary Sensor*/

/* Device class Sensor Multilevel */
#define GENERIC_TYPE_SENSOR_MULTILEVEL                                                   0x21 /*Multilevel Sensor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_ROUTING_SENSOR_MULTILEVEL                                          0x01 /*Sensor (Multilevel) Device Type*/
#define SPECIFIC_TYPE_CHIMNEY_FAN                                                        0x02

/* Device class Static Controller */
#define GENERIC_TYPE_STATIC_CONTROLLER                                                   0x02 /*Static Controller*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_PC_CONTROLLER                                                      0x01 /*Central Controller Device Type*/
#define SPECIFIC_TYPE_SCENE_CONTROLLER                                                   0x02 /*Scene Controller*/
#define SPECIFIC_TYPE_STATIC_INSTALLER_TOOL                                              0x03
#define SPECIFIC_TYPE_SET_TOP_BOX                                                        0x04 /*Set Top Box Device Type*/
#define SPECIFIC_TYPE_SUB_SYSTEM_CONTROLLER                                              0x05 /*Sub System Controller Device Type*/
#define SPECIFIC_TYPE_TV                                                                 0x06 /*TV Device Type*/
#define SPECIFIC_TYPE_GATEWAY                                                            0x07 /*Gateway Device Type*/

/* Device class Switch Binary */
#define GENERIC_TYPE_SWITCH_BINARY                                                       0x10 /*Binary Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_BINARY                                                0x01 /*On/Off Power Switch Device Type*/
#define SPECIFIC_TYPE_SCENE_SWITCH_BINARY                                                0x03 /*Binary Scene Switch*/
#define SPECIFIC_TYPE_POWER_STRIP                                                        0x04 /*Power Strip Device Type*/
#define SPECIFIC_TYPE_SIREN                                                              0x05 /*Siren Device Type*/
#define SPECIFIC_TYPE_VALVE_OPEN_CLOSE                                                   0x06 /*Valve (open/close) Device Type*/
#define SPECIFIC_DEVICE_BINARY_TUNABLE_COLOR_LIGHT                                       0x02

/* Device class Switch Multilevel */
#define GENERIC_TYPE_SWITCH_MULTILEVEL                                                   0x11 /*Multilevel Switch*/
#define SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL                                              0x05 /*Window Covering No Position/Endpoint Device Type*/
#define SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL                                              0x06 /*Window Covering Endpoint Aware Device Type*/
#define SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL                                              0x07 /*Window Covering Position/Endpoint Aware Device Type*/
#define SPECIFIC_TYPE_MOTOR_MULTIPOSITION                                                0x03 /*Multiposition Motor*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL                                            0x01 /*Light Dimmer Switch Device Type*/
#define SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL                                            0x04 /*Multilevel Scene Switch*/
#define SPECIFIC_TYPE_FAN_SWITCH                                                         0x05 /*Fan Switch Device Type*/

/* Device class Switch Remote */
#define GENERIC_TYPE_SWITCH_REMOTE                                                       0x12 /*Remote Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_BINARY                                               0x01 /*Binary Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_MULTILEVEL                                           0x02 /*Multilevel Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_BINARY                                        0x03 /*Binary Toggle Remote Switch*/
#define SPECIFIC_TYPE_SWITCH_REMOTE_TOGGLE_MULTILEVEL                                    0x04 /*Multilevel Toggle Remote Switch*/

/* Device class Switch Toggle */
#define GENERIC_TYPE_SWITCH_TOGGLE                                                       0x13 /*Toggle Switch*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_BINARY                                               0x01 /*Binary Toggle Switch*/
#define SPECIFIC_TYPE_SWITCH_TOGGLE_MULTILEVEL                                           0x02 /*Multilevel Toggle Switch*/

/* Device class Thermostat */
#define GENERIC_TYPE_THERMOSTAT                                                          0x08 /*Thermostat*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SETBACK_SCHEDULE_THERMOSTAT                                        0x03 /*Setback Schedule Thermostat*/
#define SPECIFIC_TYPE_SETBACK_THERMOSTAT                                                 0x05 /*Thermostat (Setback) Device Type*/
#define SPECIFIC_TYPE_SETPOINT_THERMOSTAT                                                0x04
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL                                                 0x02 /*Thermostat General*/
#define SPECIFIC_TYPE_THERMOSTAT_GENERAL_V2                                              0x06 /*Thermostat (HVAC) Device Type*/
#define SPECIFIC_TYPE_THERMOSTAT_HEATING                                                 0x01 /*Thermostat Heating*/

/* Device class Ventilation */
#define GENERIC_TYPE_VENTILATION                                                         0x16
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_RESIDENTIAL_HRV                                                    0x01

/* Device class Window Covering */
#define GENERIC_TYPE_WINDOW_COVERING                                                     0x09 /*Window Covering*/
#define SPECIFIC_TYPE_NOT_USED                                                           0x00 /*Specific Device Class not used*/
#define SPECIFIC_TYPE_SIMPLE_WINDOW_COVERING                                             0x01 /*Simple Window Covering Control*/

/* Device class Zip Node */
#define GENERIC_TYPE_ZIP_NODE                                                            0x15
#define SPECIFIC_TYPE_NOT_USED                                                           0x00
#define SPECIFIC_TYPE_ZIP_ADV_NODE                                                       0x02
#define SPECIFIC_TYPE_ZIP_TUN_NODE                                                       0x01

/* Device class Wall Controller */
#define GENERIC_TYPE_WALL_CONTROLLER                                                     0x18
#define SPECIFIC_TYPE_BASIC_WALL_CONTROLLER                                              0x01 /*Wall Controller Device Type*/

/* Device class Network Extender */
#define GENERIC_TYPE_NETWORK_EXTENDER                                                    0x05 /*Network Extender Generic Device Class*/
#define SPECIFIC_TYPE_SECURE_EXTENDER                                                    0x01 /*Specific Device Secure Extender*/

/* Device class Appliance */
#define GENERIC_TYPE_APPLIANCE                                                           0x06
#define SPECIFIC_TYPE_GENERAL_APPLIANCE                                                  0x01
#define SPECIFIC_TYPE_KITCHEN_APPLIANCE                                                  0x02
#define SPECIFIC_TYPE_LAUNDRY_APPLIANCE                                                  0x03


/************* Z-Wave+ Role Type identifiers **************/
/*
#define ROLE_TYPE_CENTRAL_STATIC_CONTROLLER                                              0x01
#define ROLE_TYPE_SUB_STATIC_CONTROLLER                                                  0x02
#define ROLE_TYPE_PORTABLE_CONTROLLER                                                    0x03
#define ROLE_TYPE_PORTABLE_REPORTING_CONTROLLER                                          0x04
#define ROLE_TYPE_PORTABLE_SLAVE                                                         0x05
#define ROLE_TYPE_ALWAYS_ON_SLAVE                                                        0x06
#define ROLE_TYPE_SLEEPING_REPORTING_SLAVE                                               0x07
#define ROLE_TYPE_REACHABLE_SLEEPING_SLAVE                                               0x08
*/

/************* Z-Wave+ Icon Type identifiers **************/
#define ICON_TYPE_NONE                                                                 0x0000 //No Icon

/************* Manufacturer ID identifiers **************/
#define MFG_ID_NOT_DEFINED_OR_UNDEFINED                            0xFFFF   //Not defined or un-defined
#define MFG_ID_2B_ELECTRONICS                                      0x0028   //2B Electronics
#define MFG_ID_2GIG_TECHNOLOGIES_INC                               0x009B   //2gig Technologies Inc.
#define MFG_ID_3E_TECHNOLOGIES                                     0x002A   //3e Technologies
#define MFG_ID_A1_COMPONENTS                                       0x0022   //A-1 Components
#define MFG_ID_ABILIA                                              0x0117   //Abilia
#define MFG_ID_ACT_ADVANCED_CONTROL_TECHNOLOGIES                   0x0001   //ACT - Advanced Control Technologies
#define MFG_ID_AEON_LABS                                           0x0086   //AEON Labs
#define MFG_ID_AIRLINE_MECHANICAL_CO_LTD                           0x0111   //Airline Mechanical Co., Ltd.
#define MFG_ID_ALARMCOM                                            0x0094   //Alarm.com
#define MFG_ID_ASIA_HEADING                                        0x0029   //Asia Heading
#define MFG_ID_ATECH                                               0x002B   //Atech
#define MFG_ID_BALBOA_INSTRUMENTS                                  0x0018   //Balboa Instruments
#define MFG_ID_BENEXT                                              0x008A   //BeNext
#define MFG_ID_BESAFER                                             0x002C   //BeSafer
#define MFG_ID_BFT_SPA                                             0x014B   //BFT S.p.A.
#define MFG_ID_BOCA_DEVICES                                        0x0023   //Boca Devices
#define MFG_ID_BROADBAND_ENERGY_NETWORKS_INC                       0x002D   //Broadband Energy Networks Inc.
#define MFG_ID_BULOGICS                                            0x0026   //BuLogics
#define MFG_ID_CAMEO_COMMUNICATIONS_INC                            0x009C   //Cameo Communications Inc.
#define MFG_ID_CARRIER                                             0x002E   //Carrier
#define MFG_ID_CASAWORKS                                           0x000B   //CasaWorks
#define MFG_ID_CHECKIT_SOLUTIONS_INC                               0x014E   //Check-It Solutions Inc.
#define MFG_ID_CHROMAGIC_TECHNOLOGIES_CORPORATION                  0x0116   //Chromagic Technologies Corporation
#define MFG_ID_COLOR_KINETICS_INCORPORATED                         0x002F   //Color Kinetics Incorporated
#define MFG_ID_COMPUTIME                                           0x0140   //Computime
#define MFG_ID_CONNECTED_OBJECT                                    0x011B   //Connected Object
#define MFG_ID_CONTROLTHINK_LC                                     0x0019   //ControlThink LC
#define MFG_ID_CONVERGEX_LTD                                       0x000F   //ConvergeX Ltd.
#define MFG_ID_COOPER_LIGHTING                                     0x0079   //Cooper Lighting
#define MFG_ID_COOPER_WIRING_DEVICES                               0x001A   //Cooper Wiring Devices
#define MFG_ID_CORNUCOPIA_CORP                                     0x012D   //Cornucopia Corp
#define MFG_ID_COVENTIVE_TECHNOLOGIES_INC                          0x009D   //Coventive Technologies Inc.
#define MFG_ID_CYBERHOUSE                                          0x0014   //Cyberhouse
#define MFG_ID_CYBERTAN_TECHNOLOGY_INC                             0x0067   //CyberTAN Technology, Inc.
#define MFG_ID_CYTECH_TECHNOLOGY_PRE_LTD                           0x0030   //Cytech Technology Pre Ltd.
#define MFG_ID_DANFOSS                                             0x0002   //Danfoss
#define MFG_ID_DEFACONTROLS_BV                                     0x013F   //Defacontrols BV
#define MFG_ID_DESTINY_NETWORKS                                    0x0031   //Destiny Networks
#define MFG_ID_DIEHL_AKO                                           0x0103   //Diehl AKO
#define MFG_ID_DIGITAL_5_INC                                       0x0032   //Digital 5, Inc.
#define MFG_ID_DYNAQUIP_CONTROLS                                   0x0132   //DynaQuip Controls
#define MFG_ID_ECOLINK                                             0x014A   //Ecolink
#define MFG_ID_EKA_SYSTEMS                                         0x0087   //Eka Systems
#define MFG_ID_ELECTRONIC_SOLUTIONS                                0x0033   //Electronic Solutions
#define MFG_ID_ELGEV_ELECTRONICS_LTD                               0x0034   //El-Gev Electronics LTD
#define MFG_ID_ELK_PRODUCTS_INC                                    0x001B   //ELK Products, Inc.
#define MFG_ID_EMBEDIT_AS                                          0x0035   //Embedit A/S
#define MFG_ID_ENBLINK_CO_LTD                                      0x014D   //Enblink Co. Ltd
#define MFG_ID_EUROTRONICS                                         0x0148   //Eurotronics
#define MFG_ID_EVERSPRING                                          0x0060   //Everspring
#define MFG_ID_EVOLVE                                              0x0113   //Evolve
#define MFG_ID_EXCEPTIONAL_INNOVATIONS                             0x0036   //Exceptional Innovations
#define MFG_ID_EXHAUSTO                                            0x0004   //Exhausto
#define MFG_ID_EXIGENT_SENSORS                                     0x009F   //Exigent Sensors
#define MFG_ID_EXPRESS_CONTROLS                                    0x001E   //Express Controls (former Ryherd Ventures)
#define MFG_ID_FAKRO                                               0x0085   //Fakro
#define MFG_ID_FIBARGROUP                                          0x010F   //Fibargroup
#define MFG_ID_FOARD_SYSTEMS                                       0x0037   //Foard Systems
#define MFG_ID_FOLLOWGOOD_TECHNOLOGY_COMPANY_LTD                   0x0137   //FollowGood Technology Company Ltd.
#define MFG_ID_FORTREZZ_LLC                                        0x0084   //FortrezZ LLC
#define MFG_ID_FOXCONN                                             0x011D   //Foxconn
#define MFG_ID_FROSTDALE                                           0x0110   //Frostdale
#define MFG_ID_GOOD_WAY_TECHNOLOGY_CO_LTD                          0x0068   //Good Way Technology Co., Ltd
#define MFG_ID_GREENWAVE_REALITY_INC                               0x0099   //GreenWave Reality Inc.
#define MFG_ID_HITECH_AUTOMATION                                   0x0017   //HiTech Automation
#define MFG_ID_HOLTEC_ELECTRONICS_BV                               0x013E   //Holtec Electronics BV
#define MFG_ID_HOME_AUTOMATED_INC                                  0x005B   //Home Automated Inc.
#define MFG_ID_HOME_AUTOMATED_LIVING                               0x000D   //Home Automated Living
#define MFG_ID_HOME_AUTOMATION_EUROPE                              0x009A   //Home Automation Europe
#define MFG_ID_HOME_DIRECTOR                                       0x0038   //Home Director
#define MFG_ID_HOMEMANAGEABLES_INC                                 0x0070   //Homemanageables, Inc.
#define MFG_ID_HOMEPRO                                             0x0050   //Homepro
#define MFG_ID_HOMESCENARIO                                        0x0162   //HomeScenario
#define MFG_ID_HOMESEER_TECHNOLOGIES                               0x000C   //HomeSeer Technologies
#define MFG_ID_HONEYWELL                                           0x0039   //Honeywell
#define MFG_ID_HORSTMANN_CONTROLS_LIMITED                          0x0059   //Horstmann Controls Limited
#define MFG_ID_ICOM_TECHNOLOGY_BV                                  0x0011   //iCOM Technology b.v.
#define MFG_ID_INGERSOLL_RAND_SCHLAGE                              0x006C   //Ingersoll Rand (Schlage)
#define MFG_ID_INGERSOLL_RAND_ECOLINK                              0x011F   //Ingersoll Rand (Former Ecolink)
#define MFG_ID_INLON_SRL                                           0x003A   //Inlon Srl
#define MFG_ID_INNOBAND_TECHNOLOGIES_INC                           0x0141   //Innoband Technologies, Inc
#define MFG_ID_INNOVUS                                             0x0077   //INNOVUS
#define MFG_ID_INTEL                                               0x0006   //Intel
#define MFG_ID_INTELLICON                                          0x001C   //IntelliCon
#define MFG_ID_INTERMATIC                                          0x0005   //Intermatic
#define MFG_ID_INTERNET_DOM                                        0x0013   //Internet Dom
#define MFG_ID_IR_SEC_SAFETY                                       0x003B   //IR Sec. & Safety
#define MFG_ID_IWATSU                                              0x0123   //IWATSU
#define MFG_ID_JASCO_PRODUCTS                                      0x0063   //Jasco Products
#define MFG_ID_KAMSTRUP_AS                                         0x0091   //Kamstrup A/S
#define MFG_ID_LAGOTEK_CORPORATION                                 0x0051   //Lagotek Corporation
#define MFG_ID_LEVITON                                             0x001D   //Leviton
#define MFG_ID_LIFESTYLE_NETWORKS                                  0x003C   //Lifestyle Networks
#define MFG_ID_LINEAR_CORP                                         0x014F   //Linear Corp
#define MFG_ID_LIVING_STYLE_ENTERPRISES_LTD                        0x013A   //Living Style Enterprises, Ltd.
#define MFG_ID_LOGITECH                                            0x007F   //Logitech
#define MFG_ID_LOUDWATER_TECHNOLOGIES_LLC                          0x0025   //Loudwater Technologies, LLC
#define MFG_ID_LS_CONTROL                                          0x0071   //LS Control
#define MFG_ID_MARMITEK_BV                                         0x003D   //Marmitek BV
#define MFG_ID_MARTEC_ACCESS_PRODUCTS                              0x003E   //Martec Access Products
#define MFG_ID_MB_TURN_KEY_DESIGN                                  0x008F   //MB Turn Key Design
#define MFG_ID_MERTEN                                              0x007A   //Merten
#define MFG_ID_MITSUMI                                             0x0112   //MITSUMI
#define MFG_ID_MONSTER_CABLE                                       0x007E   //Monster Cable
#define MFG_ID_MOTOROLA                                            0x003F   //Motorola
#define MFG_ID_MTC_MAINTRONIC_GERMANY                              0x0083   //MTC Maintronic Germany
#define MFG_ID_NAPCO_SECURITY_TECHNOLOGIES_INC                     0x0121   //Napco Security Technologies, Inc.
#define MFG_ID_NORTHQ                                              0x0096   //NorthQ
#define MFG_ID_NOVAR_ELECTRICAL_DEVICES_AND_SYSTEMS_EDS            0x0040   //Novar Electrical Devices and Systems (EDS)
#define MFG_ID_OMNIMA_LIMITED                                      0x0119   //Omnima Limited
#define MFG_ID_ONSITE_PRO                                          0x014C   //OnSite Pro
#define MFG_ID_OPENPEAK_INC                                        0x0041   //OpenPeak Inc.
#define MFG_ID_PHILIO_TECHNOLOGY_CORP                              0x013C   //Philio Technology Corp
#define MFG_ID_POLYCONTROL                                         0x010E   //Poly-control
#define MFG_ID_POWERLYNX                                           0x0016   //PowerLynx
#define MFG_ID_PRAGMATIC_CONSULTING_INC                            0x0042   //Pragmatic Consulting Inc.
#define MFG_ID_PULSE_TECHNOLOGIES_ASPALIS                          0x005D   //Pulse Technologies (Aspalis)
#define MFG_ID_QEES                                                0x0095   //Qees
#define MFG_ID_QUBY                                                0x0130   //Quby
#define MFG_ID_RADIO_THERMOSTAT_COMPANY_OF_AMERICA_RTC             0x0098   //Radio Thermostat Company of America (RTC)
#define MFG_ID_RARITAN                                             0x008E   //Raritan
#define MFG_ID_REITZGROUPDE                                        0x0064   //Reitz-Group.de
#define MFG_ID_REMOTEC_TECHNOLOGY_LTD                              0x5254   //Remotec Technology Ltd
#define MFG_ID_RESIDENTIAL_CONTROL_SYSTEMS_INC_RCS                 0x0010   //Residential Control Systems, Inc. (RCS)
#define MFG_ID_RIMPORT_LTD                                         0x0147   //R-import Ltd.
#define MFG_ID_RS_SCENE_AUTOMATION                                 0x0065   //RS Scene Automation
#define MFG_ID_SAECO                                               0x0139   //Saeco
#define MFG_ID_SAN_SHIH_ELECTRICAL_ENTERPRISE_CO_LTD               0x0093   //San Shih Electrical Enterprise Co., Ltd.
#define MFG_ID_SANAV                                               0x012C   //SANAV
#define MFG_ID_SCIENTIA_TECHNOLOGIES_INC                           0x001F   //Scientia Technologies, Inc.
#define MFG_ID_SECURE_WIRELESS                                     0x011E   //Secure Wireless
#define MFG_ID_SELUXIT                                             0x0069   //Seluxit
#define MFG_ID_SENMATIC_AS                                         0x0043   //Senmatic A/S
#define MFG_ID_SEQUOIA_TECHNOLOGY_LTD                              0x0044   //Sequoia Technology LTD
#define MFG_ID_SIGMA_DESIGNS                                       0x0000   //Sigma Designs
#define MFG_ID_SINE_WIRELESS                                       0x0045   //Sine Wireless
#define MFG_ID_SMART_PRODUCTS_INC                                  0x0046   //Smart Products, Inc.
#define MFG_ID_SMK_MANUFACTURING_INC                               0x0102   //SMK Manufacturing Inc.
#define MFG_ID_SOMFY                                               0x0047   //Somfy
#define MFG_ID_SYLVANIA                                            0x0009   //Sylvania
#define MFG_ID_SYSTECH_CORPORATION                                 0x0136   //Systech Corporation
#define MFG_ID_TEAM_PRECISION_PCL                                  0x0089   //Team Precision PCL
#define MFG_ID_TECHNIKU                                            0x000A   //Techniku
#define MFG_ID_TELL_IT_ONLINE                                      0x0012   //Tell It Online
#define MFG_ID_TELSEY                                              0x0048   //Telsey
#define MFG_ID_THERE_CORPORATION                                   0x010C   //There Corporation
#define MFG_ID_TKB_HOME                                            0x0118   //TKB Home
#define MFG_ID_TKH_GROUP_EMINENT                                   0x011C   //TKH Group / Eminent
#define MFG_ID_TRANE_CORPORATION                                   0x008B   //Trane Corporation
#define MFG_ID_TRICKLESTAR                                         0x0066   //TrickleStar
#define MFG_ID_TRICKLESTAR_LTD_EMPOWER_CONTROLS_LTD                0x006B   //Tricklestar Ltd. (former Empower Controls Ltd.)
#define MFG_ID_TRIDIUM                                             0x0055   //Tridium
#define MFG_ID_TWISTHINK                                           0x0049   //Twisthink
#define MFG_ID_UNIVERSAL_ELECTRONICS_INC                           0x0020   //Universal Electronics Inc.
#define MFG_ID_VDA                                                 0x010A   //VDA
#define MFG_ID_VERO_DUCO                                           0x0080   //Vero Duco
#define MFG_ID_VIEWSONIC_CORPORATION                               0x005E   //ViewSonic Corporation
#define MFG_ID_VIMAR_CRS                                           0x0007   //Vimar CRS
#define MFG_ID_VISION_SECURITY                                     0x0109   //Vision Security
#define MFG_ID_VISUALIZE                                           0x004A   //Visualize
#define MFG_ID_WATT_STOPPER                                        0x004B   //Watt Stopper
#define MFG_ID_WAYNE_DALTON                                        0x0008   //Wayne Dalton
#define MFG_ID_WENZHOU_MTLC_ELECTRIC_APPLIANCES_COLTD              0x011A   //Wenzhou MTLC Electric Appliances Co.,Ltd.
#define MFG_ID_WIDOM                                               0x0149   //wiDom
#define MFG_ID_WILSHINE_HOLDING_CO_LTD                             0x012D   //Wilshine Holding Co., Ltd
#define MFG_ID_WINTOP                                              0x0097   //Wintop
#define MFG_ID_WOODWARD_LABS                                       0x004C   //Woodward Labs
#define MFG_ID_WRAP                                                0x0003   //Wrap
#define MFG_ID_WUHAN_NWD_TECHNOLOGY_CO_LTD                         0x012E   //Wuhan NWD Technology Co., Ltd.
#define MFG_ID_XANBOO                                              0x004D   //Xanboo
#define MFG_ID_ZDATA_LLC                                           0x004E   //Zdata, LLC.
#define MFG_ID_ZIPATO                                              0x0131   //Zipato
#define MFG_ID_ZONOFF                                              0x0120   //Zonoff
#define MFG_ID_ZWAVE_TECHNOLOGIA                                   0x004F   //Z-Wave Technologia
#define MFG_ID_ZWAVEME                                             0x0115   //Z-Wave.Me
#define MFG_ID_ZYKRONIX                                            0x0021   //Zykronix
#define MFG_ID_ZYXEL                                               0x0135   //ZyXEL


/*************** command class identifiers ****************/
#define COMMAND_CLASS_ALARM                                                              0x71
#define COMMAND_CLASS_ALARM_V2                                                           0x71
#define COMMAND_CLASS_NOTIFICATION_V3                                                    0x71
#define COMMAND_CLASS_NOTIFICATION_V4                                                    0x71
#define COMMAND_CLASS_APPLICATION_STATUS                                                 0x22
#define COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION                                  0x9B
#define COMMAND_CLASS_ASSOCIATION                                                        0x85
#define COMMAND_CLASS_ASSOCIATION_V2                                                     0x85
#define COMMAND_CLASS_AV_CONTENT_DIRECTORY_MD                                            0x95
#define COMMAND_CLASS_AV_CONTENT_SEARCH_MD                                               0x97
#define COMMAND_CLASS_AV_RENDERER_STATUS                                                 0x96
#define COMMAND_CLASS_AV_TAGGING_MD                                                      0x99
#define COMMAND_CLASS_BASIC_TARIFF_INFO                                                  0x36
#define COMMAND_CLASS_BASIC_WINDOW_COVERING                                              0x50
#define COMMAND_CLASS_BASIC                                                              0x20
#define COMMAND_CLASS_BATTERY                                                            0x80
#define COMMAND_CLASS_CHIMNEY_FAN                                                        0x2A
#define COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE                                           0x46
#define COMMAND_CLASS_CLOCK                                                              0x81
#define COMMAND_CLASS_EXT_TEST                                                           0xF212 //Testing of extended command class
#define COMMAND_CLASS_CONFIGURATION                                                      0x70
#define COMMAND_CLASS_CONFIGURATION_V2                                                   0x70
#define COMMAND_CLASS_CONTROLLER_REPLICATION                                             0x21
#define COMMAND_CLASS_CRC_16_ENCAP                                                       0x56
#define COMMAND_CLASS_DCP_CONFIG                                                         0x3A
#define COMMAND_CLASS_DCP_MONITOR                                                        0x3B
#define COMMAND_CLASS_DOOR_LOCK_LOGGING                                                  0x4C
#define COMMAND_CLASS_DOOR_LOCK                                                          0x62
#define COMMAND_CLASS_DOOR_LOCK_V2                                                       0x62
#define COMMAND_CLASS_ENERGY_PRODUCTION                                                  0x90
#define COMMAND_CLASS_FIRMWARE_UPDATE_MD                                                 0x7A
#define COMMAND_CLASS_FIRMWARE_UPDATE_MD_V2                                              0x7A
#define COMMAND_CLASS_GEOGRAPHIC_LOCATION                                                0x8C
#define COMMAND_CLASS_GROUPING_NAME                                                      0x7B
#define COMMAND_CLASS_HAIL                                                               0x82
#define COMMAND_CLASS_HRV_CONTROL                                                        0x39
#define COMMAND_CLASS_HRV_STATUS                                                         0x37
#define COMMAND_CLASS_INDICATOR                                                          0x87
#define COMMAND_CLASS_IP_CONFIGURATION                                                   0x9A
#define COMMAND_CLASS_LANGUAGE                                                           0x89
#define COMMAND_CLASS_LOCK                                                               0x76
#define COMMAND_CLASS_MANUFACTURER_PROPRIETARY                                           0x91
#define COMMAND_CLASS_MANUFACTURER_SPECIFIC                                              0x72
#define COMMAND_CLASS_MANUFACTURER_SPECIFIC_V2                                           0x72
#define COMMAND_CLASS_MARK                                                               0xEF
#define COMMAND_CLASS_METER_PULSE                                                        0x35
#define COMMAND_CLASS_METER_TBL_CONFIG                                                   0x3C
#define COMMAND_CLASS_METER_TBL_MONITOR                                                  0x3D
#define COMMAND_CLASS_METER_TBL_MONITOR_V2                                               0x3D
#define COMMAND_CLASS_METER_TBL_PUSH                                                     0x3E
#define COMMAND_CLASS_METER                                                              0x32
#define COMMAND_CLASS_METER_V2                                                           0x32
#define COMMAND_CLASS_METER_V3                                                           0x32
#define COMMAND_CLASS_MTP_WINDOW_COVERING                                                0x51
#define COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2                                       0x8E
#define COMMAND_CLASS_MULTI_CHANNEL_V2                                                   0x60
#define COMMAND_CLASS_MULTI_CHANNEL_V3                                                   0x60
#define COMMAND_CLASS_MULTI_CMD                                                          0x8F
#define COMMAND_CLASS_MULTI_INSTANCE_ASSOCIATION                                         0x8E /*Discontinued*/
#define COMMAND_CLASS_MULTI_INSTANCE                                                     0x60 /*Discontinued*/
#define COMMAND_CLASS_NETWORK_MANAGEMENT_PROXY                                           0x52
#define COMMAND_CLASS_NETWORK_MANAGEMENT_BASIC                                           0x4D
#define COMMAND_CLASS_NETWORK_MANAGEMENT_INCLUSION                                       0x34
#define COMMAND_CLASS_NO_OPERATION                                                       0x00
#define COMMAND_CLASS_NODE_NAMING                                                        0x77
#define COMMAND_CLASS_NON_INTEROPERABLE                                                  0xF0
#define COMMAND_CLASS_POWERLEVEL                                                         0x73
#define COMMAND_CLASS_PREPAYMENT_ENCAPSULATION                                           0x41
#define COMMAND_CLASS_PREPAYMENT                                                         0x3F
#define COMMAND_CLASS_PROPRIETARY                                                        0x88
#define COMMAND_CLASS_PROTECTION                                                         0x75
#define COMMAND_CLASS_PROTECTION_V2                                                      0x75
#define COMMAND_CLASS_RATE_TBL_CONFIG                                                    0x48
#define COMMAND_CLASS_RATE_TBL_MONITOR                                                   0x49
#define COMMAND_CLASS_REMOTE_ASSOCIATION_ACTIVATE                                        0x7C
#define COMMAND_CLASS_REMOTE_ASSOCIATION                                                 0x7D
#define COMMAND_CLASS_SCENE_ACTIVATION                                                   0x2B
#define COMMAND_CLASS_SCENE_ACTUATOR_CONF                                                0x2C
#define COMMAND_CLASS_SCENE_CONTROLLER_CONF                                              0x2D
#define COMMAND_CLASS_SCHEDULE_ENTRY_LOCK                                                0x4E
#define COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V2                                             0x4E
#define COMMAND_CLASS_SCHEDULE_ENTRY_LOCK_V3                                             0x4E
#define COMMAND_CLASS_SCREEN_ATTRIBUTES                                                  0x93
#define COMMAND_CLASS_SCREEN_ATTRIBUTES_V2                                               0x93
#define COMMAND_CLASS_SCREEN_MD                                                          0x92
#define COMMAND_CLASS_SCREEN_MD_V2                                                       0x92
#define COMMAND_CLASS_SECURITY_PANEL_MODE                                                0x24
#define COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR                                         0x2F
#define COMMAND_CLASS_SECURITY_PANEL_ZONE                                                0x2E
#define COMMAND_CLASS_SECURITY                                                           0x98
#define COMMAND_CLASS_SENSOR_ALARM                                                       0x9C /*SDS10963-4 The Sensor Alarm command class can be used to realize Sensor Alarms.*/
#define COMMAND_CLASS_SENSOR_BINARY                                                      0x30
#define COMMAND_CLASS_SENSOR_BINARY_V2                                                   0x30
#define COMMAND_CLASS_SENSOR_CONFIGURATION                                               0x9E /*This command class adds the possibility for sensors to act on either a measured value or on a*/
#define COMMAND_CLASS_SENSOR_MULTILEVEL                                                  0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V2                                               0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V3                                               0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V4                                               0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V5                                               0x31
#define COMMAND_CLASS_SENSOR_MULTILEVEL_V6                                               0x31
#define COMMAND_CLASS_SILENCE_ALARM                                                      0x9D /*SDS10963-4 The Alarm Silence command class can be used to nuisance silence to temporarily disable the sounding*/
#define COMMAND_CLASS_SIMPLE_AV_CONTROL                                                  0x94
#define COMMAND_CLASS_SWITCH_ALL                                                         0x27
#define COMMAND_CLASS_SWITCH_BINARY                                                      0x25
#define COMMAND_CLASS_SWITCH_MULTILEVEL                                                  0x26
#define COMMAND_CLASS_SWITCH_MULTILEVEL_V2                                               0x26
#define COMMAND_CLASS_SWITCH_MULTILEVEL_V3                                               0x26
#define COMMAND_CLASS_SWITCH_TOGGLE_BINARY                                               0x28
#define COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL                                           0x29
#define COMMAND_CLASS_TARIFF_CONFIG                                                      0x4A
#define COMMAND_CLASS_TARIFF_TBL_MONITOR                                                 0x4B
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE                                                0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE_V2                                             0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE_V3                                             0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_MODE_V4                                             0x44
#define COMMAND_CLASS_THERMOSTAT_FAN_STATE                                               0x45
#define COMMAND_CLASS_THERMOSTAT_HEATING                                                 0x38
#define COMMAND_CLASS_THERMOSTAT_MODE                                                    0x40
#define COMMAND_CLASS_THERMOSTAT_MODE_V2                                                 0x40
#define COMMAND_CLASS_THERMOSTAT_MODE_V3                                                 0x40
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE                                         0x42
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE_V2                                      0x42
#define COMMAND_CLASS_THERMOSTAT_SETBACK                                                 0x47
#define COMMAND_CLASS_THERMOSTAT_SETPOINT                                                0x43
#define COMMAND_CLASS_THERMOSTAT_SETPOINT_V2                                             0x43
#define COMMAND_CLASS_THERMOSTAT_SETPOINT_V3                                             0x43
#define COMMAND_CLASS_TIME_PARAMETERS                                                    0x8B
#define COMMAND_CLASS_TIME                                                               0x8A
#define COMMAND_CLASS_TIME_V2                                                            0x8A
#define COMMAND_CLASS_TRANSPORT_SERVICE                                                  0x55
#define COMMAND_CLASS_USER_CODE                                                          0x63
#define COMMAND_CLASS_VERSION                                                            0x86
#define COMMAND_CLASS_VERSION_V2                                                         0x86
#define COMMAND_CLASS_WAKE_UP                                                            0x84
#define COMMAND_CLASS_WAKE_UP_V2                                                         0x84
#define COMMAND_CLASS_ZIP_6LOWPAN                                                        0x4F
#define COMMAND_CLASS_ZIP                                                                0x23
#define COMMAND_CLASS_APPLICATION_CAPABILITY                                             0x57
#define COMMAND_CLASS_COLOR_CONTROL                                                      0x33
#define COMMAND_CLASS_SCHEDULE                                                           0x53
#define COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY                                         0x54
#define COMMAND_CLASS_ZIP_ND                                                             0x58
#define COMMAND_CLASS_ASSOCIATION_GRP_INFO                                               0x59
#define COMMAND_CLASS_DEVICE_RESET_LOCALLY                                               0x5A
#define COMMAND_CLASS_CENTRAL_SCENE                                                      0x5B
#define COMMAND_CLASS_IP_ASSOCIATION                                                     0x5C
#define COMMAND_CLASS_ANTITHEFT                                                          0x5D
#define COMMAND_CLASS_ANTITHEFT_V2                                                       0x5D
#define COMMAND_CLASS_ZWAVEPLUS_INFO                                                     0x5E /*SDS11907-3*/
#define COMMAND_CLASS_ZWAVEPLUS_INFO_V2                                                  0x5E /*SDS11907-3*/
#define COMMAND_CLASS_ZIP_GATEWAY                                                        0x5F
#define COMMAND_CLASS_ZIP_PORTAL                                                         0x61
#define COMMAND_CLASS_APPLIANCE                                                          0x64
#define COMMAND_CLASS_DMX                                                                0x65

/*************** command class extended identifiers ****************/
#define COMMAND_CLASS_SECURITY_SCHEME0_MARK                                              0xF100

/* Unknown command class commands */
#define UNKNOWN_VERSION                                 0x00

/* Alarm command class commands */
#define ALARM_VERSION                                                                    0x01
#define ALARM_GET                                                                        0x04
#define ALARM_REPORT                                                                     0x05

/* Alarm command class commands */
#define ALARM_VERSION_V2                                                                 0x02
#define ALARM_GET_V2                                                                     0x04
#define ALARM_REPORT_V2                                                                  0x05
#define ALARM_SET_V2                                                                     0x06
#define ALARM_TYPE_SUPPORTED_GET_V2                                                      0x07
#define ALARM_TYPE_SUPPORTED_REPORT_V2                                                   0x08
/* Values used for Alarm Get command */
#define ALARM_GET_RESERVED_V2                                                            0x00
#define ALARM_GET_SMOKE_V2                                                               0x01
#define ALARM_GET_CO_V2                                                                  0x02
#define ALARM_GET_CO2_V2                                                                 0x03
#define ALARM_GET_HEAT_V2                                                                0x04
#define ALARM_GET_WATER_V2                                                               0x05
#define ALARM_GET_ACCESS_CONTROL_V2                                                      0x06
#define ALARM_GET_BURGLAR_V2                                                             0x07
#define ALARM_GET_POWER_MANAGEMENT_V2                                                    0x08
#define ALARM_GET_SYSTEM_V2                                                              0x09
#define ALARM_GET_EMERGENCY_V2                                                           0x0A
#define ALARM_GET_CLOCK_V2                                                               0x0B
#define ALARM_GET_FIRST_V2                                                               0xFF
/* Values used for Alarm Report command */
#define ALARM_REPORT_RESERVED_V2                                                         0x00
#define ALARM_REPORT_SMOKE_V2                                                            0x01
#define ALARM_REPORT_CO_V2                                                               0x02
#define ALARM_REPORT_CO2_V2                                                              0x03
#define ALARM_REPORT_HEAT_V2                                                             0x04
#define ALARM_REPORT_WATER_V2                                                            0x05
#define ALARM_REPORT_ACCESS_CONTROL_V2                                                   0x06
#define ALARM_REPORT_BURGLAR_V2                                                          0x07
#define ALARM_REPORT_POWER_MANAGEMENT_V2                                                 0x08
#define ALARM_REPORT_SYSTEM_V2                                                           0x09
#define ALARM_REPORT_EMERGENCY_V2                                                        0x0A
#define ALARM_REPORT_CLOCK_V2                                                            0x0B
#define ALARM_REPORT_FIRST_V2                                                            0xFF
/* Values used for Alarm Set command */
#define ALARM_SET_RESERVED_V2                                                            0x00
#define ALARM_SET_SMOKE_V2                                                               0x01
#define ALARM_SET_CO_V2                                                                  0x02
#define ALARM_SET_CO2_V2                                                                 0x03
#define ALARM_SET_HEAT_V2                                                                0x04
#define ALARM_SET_WATER_V2                                                               0x05
#define ALARM_SET_ACCESS_CONTROL_V2                                                      0x06
#define ALARM_SET_BURGLAR_V2                                                             0x07
#define ALARM_SET_POWER_MANAGEMENT_V2                                                    0x08
#define ALARM_SET_SYSTEM_V2                                                              0x09
#define ALARM_SET_EMERGENCY_V2                                                           0x0A
#define ALARM_SET_CLOCK_V2                                                               0x0B
#define ALARM_SET_FIRST_V2                                                               0xFF
/* Values used for Alarm Type Supported Report command */
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V2              0x1F
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V2                         0x60
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                        0x05
#define ALARM_TYPE_SUPPORTED_REPORT_PROPERTIES1_V1_ALARM_BIT_MASK_V2                     0x80

/* Notification command class commands */
#define NOTIFICATION_VERSION_V3                                                          0x03
#define NOTIFICATION_GET_V3                                                              0x04
#define NOTIFICATION_REPORT_V3                                                           0x05
#define NOTIFICATION_SET_V3                                                              0x06
#define NOTIFICATION_SUPPORTED_GET_V3                                                    0x07
#define NOTIFICATION_SUPPORTED_REPORT_V3                                                 0x08
#define EVENT_SUPPORTED_GET_V3                                                           0x01
#define EVENT_SUPPORTED_REPORT_V3                                                        0x02
/* Values used for Notification Get command */
#define NOTIFICATION_GET_RESERVED_V3                                                     0x00
#define NOTIFICATION_GET_SMOKE_V3                                                        0x01
#define NOTIFICATION_GET_CO_V3                                                           0x02
#define NOTIFICATION_GET_CO2_V3                                                          0x03
#define NOTIFICATION_GET_HEAT_V3                                                         0x04
#define NOTIFICATION_GET_WATER_V3                                                        0x05
#define NOTIFICATION_GET_ACCESS_CONTROL_V3                                               0x06
#define NOTIFICATION_GET_BURGLAR_V3                                                      0x07
#define NOTIFICATION_GET_POWER_MANAGEMENT_V3                                             0x08
#define NOTIFICATION_GET_SYSTEM_V3                                                       0x09
#define NOTIFICATION_GET_EMERGENCY_V3                                                    0x0A
#define NOTIFICATION_GET_CLOCK_V3                                                        0x0B
#define NOTIFICATION_GET_FIRST_V3                                                        0xFF
/* Values used for Notification Report command */
#define NOTIFICATION_REPORT_RESERVED_V3                                                  0x00
#define NOTIFICATION_REPORT_SMOKE_V3                                                     0x01
#define NOTIFICATION_REPORT_CO_V3                                                        0x02
#define NOTIFICATION_REPORT_CO2_V3                                                       0x03
#define NOTIFICATION_REPORT_HEAT_V3                                                      0x04
#define NOTIFICATION_REPORT_WATER_V3                                                     0x05
#define NOTIFICATION_REPORT_ACCESS_CONTROL_V3                                            0x06
#define NOTIFICATION_REPORT_BURGLAR_V3                                                   0x07
#define NOTIFICATION_REPORT_POWER_MANAGEMENT_V3                                          0x08
#define NOTIFICATION_REPORT_SYSTEM_V3                                                    0x09
#define NOTIFICATION_REPORT_EMERGENCY_V3                                                 0x0A
#define NOTIFICATION_REPORT_CLOCK_V3                                                     0x0B
#define NOTIFICATION_REPORT_FIRST_V3                                                     0xFF
#define NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V3                  0x1F
#define NOTIFICATION_REPORT_PROPERTIES1_RESERVED_MASK_V3                                 0x60
#define NOTIFICATION_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                                0x05
#define NOTIFICATION_REPORT_PROPERTIES1_SEQUENCE_BIT_MASK_V3                             0x80
/* Values used for Notification Set command */
#define NOTIFICATION_SET_RESERVED_V3                                                     0x00
#define NOTIFICATION_SET_SMOKE_V3                                                        0x01
#define NOTIFICATION_SET_CO_V3                                                           0x02
#define NOTIFICATION_SET_CO2_V3                                                          0x03
#define NOTIFICATION_SET_HEAT_V3                                                         0x04
#define NOTIFICATION_SET_WATER_V3                                                        0x05
#define NOTIFICATION_SET_ACCESS_CONTROL_V3                                               0x06
#define NOTIFICATION_SET_BURGLAR_V3                                                      0x07
#define NOTIFICATION_SET_POWER_MANAGEMENT_V3                                             0x08
#define NOTIFICATION_SET_SYSTEM_V3                                                       0x09
#define NOTIFICATION_SET_EMERGENCY_V3                                                    0x0A
#define NOTIFICATION_SET_CLOCK_V3                                                        0x0B
#define NOTIFICATION_SET_FIRST_V3                                                        0xFF
/* Values used for Notification Supported Report command */
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V3            0x1F
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                       0x60
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                      0x05
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_V1_ALARM_BIT_MASK_V3                   0x80
/* Values used for Event Supported Get command */
#define EVENT_SUPPORTED_GET_RESERVED_V3                                                  0x00
#define EVENT_SUPPORTED_GET_SMOKE_V3                                                     0x01
#define EVENT_SUPPORTED_GET_CO_V3                                                        0x02
#define EVENT_SUPPORTED_GET_CO2_V3                                                       0x03
#define EVENT_SUPPORTED_GET_HEAT_V3                                                      0x04
#define EVENT_SUPPORTED_GET_WATER_V3                                                     0x05
#define EVENT_SUPPORTED_GET_ACCESS_CONTROL_V3                                            0x06
#define EVENT_SUPPORTED_GET_BURGLAR_V3                                                   0x07
#define EVENT_SUPPORTED_GET_POWER_MANAGEMENT_V3                                          0x08
#define EVENT_SUPPORTED_GET_SYSTEM_V3                                                    0x09
#define EVENT_SUPPORTED_GET_EMERGENCY_V3                                                 0x0A
#define EVENT_SUPPORTED_GET_CLOCK_V3                                                     0x0B
#define EVENT_SUPPORTED_GET_FIRST_V3                                                     0xFF
/* Values used for Event Supported Report command */
#define EVENT_SUPPORTED_REPORT_RESERVED_V3                                               0x00
#define EVENT_SUPPORTED_REPORT_SMOKE_V3                                                  0x01
#define EVENT_SUPPORTED_REPORT_CO_V3                                                     0x02
#define EVENT_SUPPORTED_REPORT_CO2_V3                                                    0x03
#define EVENT_SUPPORTED_REPORT_HEAT_V3                                                   0x04
#define EVENT_SUPPORTED_REPORT_WATER_V3                                                  0x05
#define EVENT_SUPPORTED_REPORT_ACCESS_CONTROL_V3                                         0x06
#define EVENT_SUPPORTED_REPORT_BURGLAR_V3                                                0x07
#define EVENT_SUPPORTED_REPORT_POWER_MANAGEMENT_V3                                       0x08
#define EVENT_SUPPORTED_REPORT_SYSTEM_V3                                                 0x09
#define EVENT_SUPPORTED_REPORT_EMERGENCY_V3                                              0x0A
#define EVENT_SUPPORTED_REPORT_CLOCK_V3                                                  0x0B
#define EVENT_SUPPORTED_REPORT_FIRST_V3                                                  0xFF
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V3                   0x1F
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                              0xE0
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                             0x05

/* Notification command class commands */
#define NOTIFICATION_VERSION_V4                                                          0x04
#define NOTIFICATION_GET_V4                                                              0x04
#define NOTIFICATION_REPORT_V4                                                           0x05
#define NOTIFICATION_SET_V4                                                              0x06
#define NOTIFICATION_SUPPORTED_GET_V4                                                    0x07
#define NOTIFICATION_SUPPORTED_REPORT_V4                                                 0x08
#define EVENT_SUPPORTED_GET_V4                                                           0x01
#define EVENT_SUPPORTED_REPORT_V4                                                        0x02
/* Values used for Notification Get command */
#define NOTIFICATION_GET_RESERVED_V4                                                     0x00
#define NOTIFICATION_GET_SMOKE_V4                                                        0x01
#define NOTIFICATION_GET_CO_V4                                                           0x02
#define NOTIFICATION_GET_CO2_V4                                                          0x03
#define NOTIFICATION_GET_HEAT_V4                                                         0x04
#define NOTIFICATION_GET_WATER_V4                                                        0x05
#define NOTIFICATION_GET_ACCESS_CONTROL_V4                                               0x06
#define NOTIFICATION_GET_BURGLAR_V4                                                      0x07
#define NOTIFICATION_GET_POWER_MANAGEMENT_V4                                             0x08
#define NOTIFICATION_GET_SYSTEM_V4                                                       0x09
#define NOTIFICATION_GET_EMERGENCY_V4                                                    0x0A
#define NOTIFICATION_GET_CLOCK_V4                                                        0x0B
#define NOTIFICATION_GET_APPLIANCE_V4                                                    0x0C
#define NOTIFICATION_GET_FIRST_V4                                                        0xFF
/* Values used for Notification Report command */
#define NOTIFICATION_REPORT_OFF_V4                                                       0x00
#define NOTIFICATION_REPORT_NO_PENDING_NOTIFICATIONS_V4                                  0xFE
#define NOTIFICATION_REPORT_ON_V4                                                        0xFF
#define NOTIFICATION_REPORT_RESERVED_V4                                                  0x00
#define NOTIFICATION_REPORT_SMOKE_V4                                                     0x01
#define NOTIFICATION_REPORT_CO_V4                                                        0x02
#define NOTIFICATION_REPORT_CO2_V4                                                       0x03
#define NOTIFICATION_REPORT_HEAT_V4                                                      0x04
#define NOTIFICATION_REPORT_WATER_V4                                                     0x05
#define NOTIFICATION_REPORT_ACCESS_CONTROL_V4                                            0x06
#define NOTIFICATION_REPORT_BURGLAR_V4                                                   0x07
#define NOTIFICATION_REPORT_POWER_MANAGEMENT_V4                                          0x08
#define NOTIFICATION_REPORT_SYSTEM_V4                                                    0x09
#define NOTIFICATION_REPORT_EMERGENCY_V4                                                 0x0A
#define NOTIFICATION_REPORT_CLOCK_V4                                                     0x0B
#define NOTIFICATION_REPORT_APPLIANCE_V4                                                 0x0C
#define NOTIFICATION_REPORT_FIRST_V4                                                     0xFF
#define NOTIFICATION_REPORT_PROPERTIES1_EVENT_PARAMETERS_LENGTH_MASK_V4                  0x1F
#define NOTIFICATION_REPORT_PROPERTIES1_RESERVED_MASK_V4                                 0x60
#define NOTIFICATION_REPORT_PROPERTIES1_RESERVED_SHIFT_V4                                0x05
#define NOTIFICATION_REPORT_PROPERTIES1_SEQUENCE_BIT_MASK_V4                             0x80
/* Values used for Notification Set command */
#define NOTIFICATION_SET_RESERVED_V4                                                     0x00
#define NOTIFICATION_SET_SMOKE_V4                                                        0x01
#define NOTIFICATION_SET_CO_V4                                                           0x02
#define NOTIFICATION_SET_CO2_V4                                                          0x03
#define NOTIFICATION_SET_HEAT_V4                                                         0x04
#define NOTIFICATION_SET_WATER_V4                                                        0x05
#define NOTIFICATION_SET_ACCESS_CONTROL_V4                                               0x06
#define NOTIFICATION_SET_BURGLAR_V4                                                      0x07
#define NOTIFICATION_SET_POWER_MANAGEMENT_V4                                             0x08
#define NOTIFICATION_SET_SYSTEM_V4                                                       0x09
#define NOTIFICATION_SET_EMERGENCY_V4                                                    0x0A
#define NOTIFICATION_SET_CLOCK_V4                                                        0x0B
#define NOTIFICATION_SET_APPLIANCE_V4                                                    0x0C
#define NOTIFICATION_SET_FIRST_V4                                                        0xFF
#define NOTIFICATION_SET_OFF_V4                                                          0x00
#define NOTIFICATION_SET_NO_PENDING_NOTIFICATIONS_V4                                     0xFE
#define NOTIFICATION_SET_ON_V4                                                           0xFF
/* Values used for Notification Supported Report command */
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V4            0x1F
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V4                       0x60
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V4                      0x05
#define NOTIFICATION_SUPPORTED_REPORT_PROPERTIES1_V1_ALARM_BIT_MASK_V4                   0x80
/* Values used for Event Supported Get command */
#define EVENT_SUPPORTED_GET_RESERVED_V4                                                  0x00
#define EVENT_SUPPORTED_GET_SMOKE_V4                                                     0x01
#define EVENT_SUPPORTED_GET_CO_V4                                                        0x02
#define EVENT_SUPPORTED_GET_CO2_V4                                                       0x03
#define EVENT_SUPPORTED_GET_HEAT_V4                                                      0x04
#define EVENT_SUPPORTED_GET_WATER_V4                                                     0x05
#define EVENT_SUPPORTED_GET_ACCESS_CONTROL_V4                                            0x06
#define EVENT_SUPPORTED_GET_BURGLAR_V4                                                   0x07
#define EVENT_SUPPORTED_GET_POWER_MANAGEMENT_V4                                          0x08
#define EVENT_SUPPORTED_GET_SYSTEM_V4                                                    0x09
#define EVENT_SUPPORTED_GET_EMERGENCY_V4                                                 0x0A
#define EVENT_SUPPORTED_GET_CLOCK_V4                                                     0x0B
#define EVENT_SUPPORTED_GET_APPLIANCE_V4                                                 0x0C
#define EVENT_SUPPORTED_GET_FIRST_V4                                                     0xFF
/* Values used for Event Supported Report command */
#define EVENT_SUPPORTED_REPORT_RESERVED_V4                                               0x00
#define EVENT_SUPPORTED_REPORT_SMOKE_V4                                                  0x01
#define EVENT_SUPPORTED_REPORT_CO_V4                                                     0x02
#define EVENT_SUPPORTED_REPORT_CO2_V4                                                    0x03
#define EVENT_SUPPORTED_REPORT_HEAT_V4                                                   0x04
#define EVENT_SUPPORTED_REPORT_WATER_V4                                                  0x05
#define EVENT_SUPPORTED_REPORT_ACCESS_CONTROL_V4                                         0x06
#define EVENT_SUPPORTED_REPORT_BURGLAR_V4                                                0x07
#define EVENT_SUPPORTED_REPORT_POWER_MANAGEMENT_V4                                       0x08
#define EVENT_SUPPORTED_REPORT_SYSTEM_V4                                                 0x09
#define EVENT_SUPPORTED_REPORT_EMERGENCY_V4                                              0x0A
#define EVENT_SUPPORTED_REPORT_CLOCK_V4                                                  0x0B
#define EVENT_SUPPORTED_REPORT_APPLIANCE_V4                                              0x0C
#define EVENT_SUPPORTED_REPORT_FIRST_V4                                                  0xFF
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_NUMBER_OF_BIT_MASKS_MASK_V4                   0x1F
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V4                              0xE0
#define EVENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V4                             0x05

/* Application Status command class commands */
#define APPLICATION_STATUS_VERSION                                                       0x01
#define APPLICATION_BUSY                                                                 0x01
#define APPLICATION_REJECTED_REQUEST                                                     0x02
/* Values used for Application Busy command */
#define APPLICATION_BUSY_TRY_AGAIN_LATER                                                 0x00
#define APPLICATION_BUSY_TRY_AGAIN_IN_WAIT_TIME_SECONDS                                  0x01
#define APPLICATION_BUSY_REQUEST_QUEUED_EXECUTED_LATER                                   0x02

/* Association Command Configuration command class commands */
#define ASSOCIATION_COMMAND_CONFIGURATION_VERSION                                        0x01
#define COMMAND_CONFIGURATION_GET                                                        0x04
#define COMMAND_CONFIGURATION_REPORT                                                     0x05
#define COMMAND_CONFIGURATION_SET                                                        0x03
#define COMMAND_RECORDS_SUPPORTED_GET                                                    0x01
#define COMMAND_RECORDS_SUPPORTED_REPORT                                                 0x02
/* Values used for Command Configuration Report command */
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_REPORTS_TO_FOLLOW_MASK                  0x0F
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                           0x70
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                          0x04
#define COMMAND_CONFIGURATION_REPORT_PROPERTIES1_FIRST_BIT_MASK                          0x80
/* Values used for Command Records Supported Report command */
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_CONF_CMD_BIT_MASK                   0x01
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_V_C_BIT_MASK                        0x02
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_MAX_COMMAND_LENGTH_MASK             0xFC
#define COMMAND_RECORDS_SUPPORTED_REPORT_PROPERTIES1_MAX_COMMAND_LENGTH_SHIFT            0x02

/* Association command class commands */
#define ASSOCIATION_VERSION                                                              0x01
#define ASSOCIATION_GET                                                                  0x02
#define ASSOCIATION_GROUPINGS_GET                                                        0x05
#define ASSOCIATION_GROUPINGS_REPORT                                                     0x06
#define ASSOCIATION_REMOVE                                                               0x04
#define ASSOCIATION_REPORT                                                               0x03
#define ASSOCIATION_SET                                                                  0x01

/* Association command class commands */
#define ASSOCIATION_VERSION_V2                                                           0x02
#define ASSOCIATION_GET_V2                                                               0x02
#define ASSOCIATION_GROUPINGS_GET_V2                                                     0x05
#define ASSOCIATION_GROUPINGS_REPORT_V2                                                  0x06
#define ASSOCIATION_REMOVE_V2                                                            0x04
#define ASSOCIATION_REPORT_V2                                                            0x03
#define ASSOCIATION_SET_V2                                                               0x01
#define ASSOCIATION_SPECIFIC_GROUP_GET_V2                                                0x0B
#define ASSOCIATION_SPECIFIC_GROUP_REPORT_V2                                             0x0C

/* Av Content Directory Md command class commands */
#define AV_CONTENT_DIRECTORY_MD_VERSION                                                  0x01
#define AV_CONTENT_BROWSE_MD_BY_LETTER_GET                                               0x03
#define AV_CONTENT_BROWSE_MD_BY_LETTER_REPORT                                            0x04
#define AV_CONTENT_BROWSE_MD_CHILD_COUNT_GET                                             0x05
#define AV_CONTENT_BROWSE_MD_CHILD_COUNT_REPORT                                          0x06
#define AV_CONTENT_BROWSE_MD_GET                                                         0x01
#define AV_CONTENT_BROWSE_MD_REPORT                                                      0x02
#define AV_MATCH_ITEM_TO_RENDERER_MD_GET                                                 0x07
#define AV_MATCH_ITEM_TO_RENDERER_MD_REPORT                                              0x08

/* Av Content Search Md command class commands */
#define AV_CONTENT_SEARCH_MD_VERSION                                                     0x01
#define AV_CONTENT_SEARCH_MD_GET                                                         0x01
#define AV_CONTENT_SEARCH_MD_REPORT                                                      0x02

/* Av Renderer Status command class commands */
#define AV_RENDERER_STATUS_VERSION                                                       0x01
#define AV_RENDERER_STATUS_GET                                                           0x01
#define AV_RENDERER_STATUS_REPORT                                                        0x02

/* Av Tagging Md command class commands */
#define AV_TAGGING_MD_VERSION                                                            0x01
#define AV_TAGGING_MD_GET                                                                0x01
#define AV_TAGGING_MD_REPORT                                                             0x02

/* Basic Tariff Info command class commands */
#define BASIC_TARIFF_INFO_VERSION                                                        0x01
#define BASIC_TARIFF_INFO_GET                                                            0x01
#define BASIC_TARIFF_INFO_REPORT                                                         0x02
/* Values used for Basic Tariff Info Report command */
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_TOTAL_NO_IMPORT_RATES_MASK                  0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_RESERVED_MASK                               0x70
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x04
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES1_DUAL_BIT_MASK                               0x80
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_E1_CURRENT_RATE_IN_USE_MASK                 0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_RESERVED_MASK                               0xF0
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES2_RESERVED_SHIFT                              0x04
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_E2_CURRENT_RATE_IN_USE_MASK                 0x0F
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_RESERVED_MASK                               0xF0
#define BASIC_TARIFF_INFO_REPORT_PROPERTIES3_RESERVED_SHIFT                              0x04

/* Basic Window Covering command class commands */
#define BASIC_WINDOW_COVERING_VERSION                                                    0x01
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE                                         0x01
#define BASIC_WINDOW_COVERING_STOP_LEVEL_CHANGE                                          0x02
/* Values used for Basic Window Covering Start Level Change command */
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_RESERVED1_MASK                    0x3F
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_OPEN_CLOSE_BIT_MASK               0x40
#define BASIC_WINDOW_COVERING_START_LEVEL_CHANGE_LEVEL_RESERVED2_BIT_MASK                0x80

/* Basic command class commands */
#define BASIC_VERSION                                                                    0x01
#define BASIC_GET                                                                        0x02
#define BASIC_REPORT                                                                     0x03
#define BASIC_SET                                                                        0x01

/* Battery command class commands */
#define BATTERY_VERSION                                                                  0x01
#define BATTERY_GET                                                                      0x02
#define BATTERY_REPORT                                                                   0x03

/* Chimney Fan command class commands */
#define CHIMNEY_FAN_VERSION                                                              0x01
#define CHIMNEY_FAN_ALARM_LOG_GET                                                        0x20
#define CHIMNEY_FAN_ALARM_LOG_REPORT                                                     0x21
#define CHIMNEY_FAN_ALARM_LOG_SET                                                        0x1F
#define CHIMNEY_FAN_ALARM_STATUS_GET                                                     0x23
#define CHIMNEY_FAN_ALARM_STATUS_REPORT                                                  0x24
#define CHIMNEY_FAN_ALARM_STATUS_SET                                                     0x22
#define CHIMNEY_FAN_ALARM_TEMP_GET                                                       0x0E
#define CHIMNEY_FAN_ALARM_TEMP_REPORT                                                    0x0F
#define CHIMNEY_FAN_ALARM_TEMP_SET                                                       0x0D
#define CHIMNEY_FAN_BOOST_TIME_GET                                                       0x11
#define CHIMNEY_FAN_BOOST_TIME_REPORT                                                    0x12
#define CHIMNEY_FAN_BOOST_TIME_SET                                                       0x10
#define CHIMNEY_FAN_DEFAULT_SET                                                          0x28
#define CHIMNEY_FAN_MIN_SPEED_GET                                                        0x26
#define CHIMNEY_FAN_MIN_SPEED_REPORT                                                     0x27
#define CHIMNEY_FAN_MIN_SPEED_SET                                                        0x25
#define CHIMNEY_FAN_MODE_GET                                                             0x17
#define CHIMNEY_FAN_MODE_REPORT                                                          0x18
#define CHIMNEY_FAN_MODE_SET                                                             0x16
#define CHIMNEY_FAN_SETUP_GET                                                            0x1A
#define CHIMNEY_FAN_SETUP_REPORT                                                         0x1B
#define CHIMNEY_FAN_SETUP_SET                                                            0x19
#define CHIMNEY_FAN_SPEED_GET                                                            0x05
#define CHIMNEY_FAN_SPEED_REPORT                                                         0x06
#define CHIMNEY_FAN_SPEED_SET                                                            0x04
#define CHIMNEY_FAN_START_TEMP_GET                                                       0x08
#define CHIMNEY_FAN_START_TEMP_REPORT                                                    0x09
#define CHIMNEY_FAN_START_TEMP_SET                                                       0x07
#define CHIMNEY_FAN_STATE_GET                                                            0x02
#define CHIMNEY_FAN_STATE_REPORT                                                         0x03
#define CHIMNEY_FAN_STATE_SET                                                            0x01
#define CHIMNEY_FAN_STATUS_GET                                                           0x1D
#define CHIMNEY_FAN_STATUS_REPORT                                                        0x1E
#define CHIMNEY_FAN_STOP_TEMP_GET                                                        0x0B
#define CHIMNEY_FAN_STOP_TEMP_REPORT                                                     0x0C
#define CHIMNEY_FAN_STOP_TEMP_SET                                                        0x0A
#define CHIMNEY_FAN_STOP_TIME_GET                                                        0x14
#define CHIMNEY_FAN_STOP_TIME_REPORT                                                     0x15
#define CHIMNEY_FAN_STOP_TIME_SET                                                        0x13
/* Values used for Chimney Fan Alarm Log Report command */
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_RESERVED1_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_EXTERNAL_ALARM_1_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_SENSOR_ERROR_1_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_ALARM_TEMPERATURE_EXCEEDED_1_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_RESERVED2_MASK                        0x70
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_RESERVED2_SHIFT                       0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_1_ALARM_STILL_ACTIVE_1_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_RESERVED1_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_EXTERNAL_ALARM_2_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_SENSOR_ERROR_2_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_ALARM_TEMPERATURE_EXCEEDED_2_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_RESERVED2_MASK                        0x70
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_RESERVED2_SHIFT                       0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_2_ALARM_STILL_ACTIVE_2_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_RESERVED1_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_EXTERNAL_ALARM_3_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_SENSOR_ERROR_3_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_ALARM_TEMPERATURE_EXCEEDED_3_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_RESERVED2_MASK                        0x70
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_RESERVED2_SHIFT                       0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_3_ALARM_STILL_ACTIVE_3_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_RESERVED1_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_EXTERNAL_ALARM_4_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_SENSOR_ERROR_4_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_ALARM_TEMPERATURE_EXCEEDED_4_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_RESERVED2_MASK                        0x70
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_RESERVED2_SHIFT                       0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_4_ALARM_STILL_ACTIVE_4_BIT_MASK         0x80
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_RESERVED1_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_EXTERNAL_ALARM_5_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_SENSOR_ERROR_5_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_ALARM_TEMPERATURE_EXCEEDED_5_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_RESERVED2_MASK                        0x70
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_RESERVED2_SHIFT                       0x04
#define CHIMNEY_FAN_ALARM_LOG_REPORT_ALARM_EVENT_5_ALARM_STILL_ACTIVE_5_BIT_MASK         0x80
/* Values used for Chimney Fan Alarm Log Set command */
#define CHIMNEY_FAN_ALARM_LOG_SET_RESET_LOG                                              0x08
/* Values used for Chimney Fan Alarm Status Report command */
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SERVICE_BIT_MASK                    0x01
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_EXTERNAL_ALARM_BIT_MASK             0x02
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SENSOR_ERROR_BIT_MASK               0x04
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_NOT_USED_MASK                       0x30
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_NOT_USED_SHIFT                      0x04
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_SPEED_CHANGE_ENABLE_BIT_MASK        0x40
#define CHIMNEY_FAN_ALARM_STATUS_REPORT_ALARM_STATUS_START_TEMPERATURE_EXCEEDED_BIT_MASK 0x80
/* Values used for Chimney Fan Alarm Status Set command */
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED1_BIT_MASK                          0x01
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_EXTERNAL_ALARM_BIT_MASK         0x02
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_SENSOR_ERROR_BIT_MASK           0x04
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_ACKNOWLEDGE_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK 0x08
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED2_MASK                              0xF0
#define CHIMNEY_FAN_ALARM_STATUS_SET_MESSAGE_NOT_USED2_SHIFT                             0x04
/* Values used for Chimney Fan Alarm Temp Report command */
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SIZE_MASK                              0x07
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SCALE_MASK                             0x18
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                            0x03
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                         0xE0
#define CHIMNEY_FAN_ALARM_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                        0x05
/* Values used for Chimney Fan Alarm Temp Set command */
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SIZE_MASK                                 0x07
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SCALE_MASK                                0x18
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_SCALE_SHIFT                               0x03
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_PRECISION_MASK                            0xE0
#define CHIMNEY_FAN_ALARM_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                           0x05
/* Values used for Chimney Fan Mode Report command */
#define CHIMNEY_FAN_MODE_REPORT_OFF                                                      0x00
#define CHIMNEY_FAN_MODE_REPORT_ON                                                       0xFF
/* Values used for Chimney Fan Mode Set command */
#define CHIMNEY_FAN_MODE_SET_OFF                                                         0x00
#define CHIMNEY_FAN_MODE_SET_ON                                                          0xFF
/* Values used for Chimney Fan Setup Report command */
#define CHIMNEY_FAN_SETUP_REPORT_OFF                                                     0x00
#define CHIMNEY_FAN_SETUP_REPORT_ON                                                      0xFF
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SIZE_1_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SCALE_1_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_SCALE_1_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_PRECISION_1_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES1_PRECISION_1_SHIFT                           0x05
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SIZE_2_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SCALE_2_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_SCALE_2_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_PRECISION_2_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES2_PRECISION_2_SHIFT                           0x05
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SIZE_3_MASK                                 0x07
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SCALE_3_MASK                                0x18
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_SCALE_3_SHIFT                               0x03
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_PRECISION_3_MASK                            0xE0
#define CHIMNEY_FAN_SETUP_REPORT_PROPERTIES3_PRECISION_3_SHIFT                           0x05
/* Values used for Chimney Fan Setup Set command */
#define CHIMNEY_FAN_SETUP_SET_OFF                                                        0x00
#define CHIMNEY_FAN_SETUP_SET_ON                                                         0xFF
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SIZE_1_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SCALE_1_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_SCALE_1_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_PRECISION_1_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES1_PRECISION_1_SHIFT                              0x05
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SIZE_2_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SCALE_2_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_SCALE_2_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_PRECISION_2_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES2_PRECISION_2_SHIFT                              0x05
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SIZE_3_MASK                                    0x07
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SCALE_3_MASK                                   0x18
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_SCALE_3_SHIFT                                  0x03
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_PRECISION_3_MASK                               0xE0
#define CHIMNEY_FAN_SETUP_SET_PROPERTIES3_PRECISION_3_SHIFT                              0x05
/* Values used for Chimney Fan Start Temp Report command */
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SIZE_MASK                              0x07
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SCALE_MASK                             0x18
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                            0x03
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                         0xE0
#define CHIMNEY_FAN_START_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                        0x05
/* Values used for Chimney Fan Start Temp Set command */
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SIZE_MASK                                 0x07
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SCALE_MASK                                0x18
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_SCALE_SHIFT                               0x03
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_PRECISION_MASK                            0xE0
#define CHIMNEY_FAN_START_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                           0x05
/* Values used for Chimney Fan State Report command */
#define CHIMNEY_FAN_STATE_REPORT_OFF                                                     0x00
#define CHIMNEY_FAN_STATE_REPORT_BOOST                                                   0x01
#define CHIMNEY_FAN_STATE_REPORT_EXHAUST                                                 0x02
#define CHIMNEY_FAN_STATE_REPORT_RELOAD                                                  0x03
#define CHIMNEY_FAN_STATE_REPORT_VENTING                                                 0x04
#define CHIMNEY_FAN_STATE_REPORT_STOP                                                    0x05
#define CHIMNEY_FAN_STATE_REPORT_VENTING_EX                                              0x06
#define CHIMNEY_FAN_STATE_REPORT_SERVICE                                                 0x07
#define CHIMNEY_FAN_STATE_REPORT_SENSOR_FAILURE                                          0x08
#define CHIMNEY_FAN_STATE_REPORT_CHIMNEY_FIRE                                            0x09
#define CHIMNEY_FAN_STATE_REPORT_EXTERNAL_ALARM                                          0x0A
/* Values used for Chimney Fan State Set command */
#define CHIMNEY_FAN_STATE_SET_NEXT_STATE                                                 0x01
/* Values used for Chimney Fan Status Report command */
#define CHIMNEY_FAN_STATUS_REPORT_OFF                                                    0x00
#define CHIMNEY_FAN_STATUS_REPORT_BOOST                                                  0x01
#define CHIMNEY_FAN_STATUS_REPORT_EXHAUST                                                0x02
#define CHIMNEY_FAN_STATUS_REPORT_RELOAD                                                 0x03
#define CHIMNEY_FAN_STATUS_REPORT_VENTING                                                0x04
#define CHIMNEY_FAN_STATUS_REPORT_STOP                                                   0x05
#define CHIMNEY_FAN_STATUS_REPORT_VENTING_EX                                             0x06
#define CHIMNEY_FAN_STATUS_REPORT_SERVICE                                                0x07
#define CHIMNEY_FAN_STATUS_REPORT_SENSOR_FAILURE                                         0x08
#define CHIMNEY_FAN_STATUS_REPORT_CHIMNEY_FIRE                                           0x09
#define CHIMNEY_FAN_STATUS_REPORT_EXTERNAL_ALARM                                         0x0A
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SERVICE_BIT_MASK                          0x01
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_EXTERNAL_ALARM_BIT_MASK                   0x02
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SENSOR_ERROR_BIT_MASK                     0x04
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_ALARM_TEMPERATURE_EXCEEDED_BIT_MASK       0x08
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_NOT_USED_MASK                             0x30
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_NOT_USED_SHIFT                            0x04
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_SPEED_CHANGE_ENABLE_BIT_MASK              0x40
#define CHIMNEY_FAN_STATUS_REPORT_ALARM_STATUS_START_TEMPERATURE_EXCEEDED_BIT_MASK       0x80
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SIZE_MASK                                  0x07
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SCALE_MASK                                 0x18
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_SCALE_SHIFT                                0x03
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_PRECISION_MASK                             0xE0
#define CHIMNEY_FAN_STATUS_REPORT_PROPERTIES1_PRECISION_SHIFT                            0x05
/* Values used for Chimney Fan Stop Temp Report command */
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SIZE_MASK                               0x07
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SCALE_MASK                              0x18
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_SCALE_SHIFT                             0x03
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_PRECISION_MASK                          0xE0
#define CHIMNEY_FAN_STOP_TEMP_REPORT_PROPERTIES1_PRECISION_SHIFT                         0x05
/* Values used for Chimney Fan Stop Temp Set command */
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SIZE_MASK                                  0x07
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SCALE_MASK                                 0x18
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_SCALE_SHIFT                                0x03
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_PRECISION_MASK                             0xE0
#define CHIMNEY_FAN_STOP_TEMP_SET_PROPERTIES1_PRECISION_SHIFT                            0x05

/* Climate Control Schedule command class commands */
#define CLIMATE_CONTROL_SCHEDULE_VERSION                                                 0x01
#define SCHEDULE_CHANGED_GET                                                             0x04
#define SCHEDULE_CHANGED_REPORT                                                          0x05
#define SCHEDULE_GET                                                                     0x02
#define SCHEDULE_OVERRIDE_GET                                                            0x07
#define SCHEDULE_OVERRIDE_REPORT                                                         0x08
#define SCHEDULE_OVERRIDE_SET                                                            0x06
#define SCHEDULE_REPORT                                                                  0x03
#define SCHEDULE_SET                                                                     0x01
/* Values used for Schedule Get command */
#define SCHEDULE_GET_PROPERTIES1_WEEKDAY_MASK                                            0x07
#define SCHEDULE_GET_PROPERTIES1_RESERVED_MASK                                           0xF8
#define SCHEDULE_GET_PROPERTIES1_RESERVED_SHIFT                                          0x03
/* Values used for Schedule Override Report command */
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_OVERRIDE_TYPE_MASK                          0x03
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_RESERVED_MASK                               0xFC
#define SCHEDULE_OVERRIDE_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x02
#define SCHEDULE_OVERRIDE_REPORT_NO_OVERRIDE                                             0x00
#define SCHEDULE_OVERRIDE_REPORT_TEMPORARY_OVERRIDE                                      0x01
#define SCHEDULE_OVERRIDE_REPORT_PERMANENT_OVERRIDE                                      0x02
#define SCHEDULE_OVERRIDE_REPORT_RESERVED                                                0x03
/* Values used for Schedule Override Set command */
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_OVERRIDE_TYPE_MASK                             0x03
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_RESERVED_MASK                                  0xFC
#define SCHEDULE_OVERRIDE_SET_PROPERTIES1_RESERVED_SHIFT                                 0x02
#define SCHEDULE_OVERRIDE_SET_NO_OVERRIDE                                                0x00
#define SCHEDULE_OVERRIDE_SET_TEMPORARY_OVERRIDE                                         0x01
#define SCHEDULE_OVERRIDE_SET_PERMANENT_OVERRIDE                                         0x02
#define SCHEDULE_OVERRIDE_SET_RESERVED                                                   0x03
/* Values used for Schedule Report command */
#define SCHEDULE_REPORT_PROPERTIES1_WEEKDAY_MASK                                         0x07
#define SCHEDULE_REPORT_PROPERTIES1_RESERVED_MASK                                        0xF8
#define SCHEDULE_REPORT_PROPERTIES1_RESERVED_SHIFT                                       0x03
/* Values used for Schedule Set command */
#define SCHEDULE_SET_PROPERTIES1_WEEKDAY_MASK                                            0x07
#define SCHEDULE_SET_PROPERTIES1_RESERVED_MASK                                           0xF8
#define SCHEDULE_SET_PROPERTIES1_RESERVED_SHIFT                                          0x03

/* Clock command class commands */
#define CLOCK_VERSION                                                                    0x01
#define CLOCK_GET                                                                        0x05
#define CLOCK_REPORT                                                                     0x06
#define CLOCK_SET                                                                        0x04
/* Values used for Clock Report command */
#define CLOCK_REPORT_LEVEL_HOUR_MASK                                                     0x1F
#define CLOCK_REPORT_LEVEL_WEEKDAY_MASK                                                  0xE0
#define CLOCK_REPORT_LEVEL_WEEKDAY_SHIFT                                                 0x05
/* Values used for Clock Set command */
#define CLOCK_SET_LEVEL_HOUR_MASK                                                        0x1F
#define CLOCK_SET_LEVEL_WEEKDAY_MASK                                                     0xE0
#define CLOCK_SET_LEVEL_WEEKDAY_SHIFT                                                    0x05

/* Configuration command class commands */
#define CONFIGURATION_VERSION                                                            0x01
#define CONFIGURATION_GET                                                                0x05
#define CONFIGURATION_REPORT                                                             0x06
#define CONFIGURATION_SET                                                                0x04
/* Values used for Configuration Report command */
#define CONFIGURATION_REPORT_LEVEL_SIZE_MASK                                             0x07
#define CONFIGURATION_REPORT_LEVEL_RESERVED_MASK                                         0xF8
#define CONFIGURATION_REPORT_LEVEL_RESERVED_SHIFT                                        0x03
/* Values used for Configuration Set command */
#define CONFIGURATION_SET_LEVEL_SIZE_MASK                                                0x07
#define CONFIGURATION_SET_LEVEL_RESERVED_MASK                                            0x78
#define CONFIGURATION_SET_LEVEL_RESERVED_SHIFT                                           0x03
#define CONFIGURATION_SET_LEVEL_DEFAULT_BIT_MASK                                         0x80

/* Configuration command class commands */
#define CONFIGURATION_VERSION_V2                                                         0x02
#define CONFIGURATION_BULK_GET_V2                                                        0x08
#define CONFIGURATION_BULK_REPORT_V2                                                     0x09
#define CONFIGURATION_BULK_SET_V2                                                        0x07
#define CONFIGURATION_GET_V2                                                             0x05
#define CONFIGURATION_REPORT_V2                                                          0x06
#define CONFIGURATION_SET_V2                                                             0x04
/* Values used for Configuration Bulk Report command */
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_SIZE_MASK_V2                               0x07
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_RESERVED_MASK_V2                           0x38
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                          0x03
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_HANDSHAKE_BIT_MASK_V2                      0x40
#define CONFIGURATION_BULK_REPORT_PROPERTIES1_DEFAULT_BIT_MASK_V2                        0x80
/* Values used for Configuration Bulk Set command */
#define CONFIGURATION_BULK_SET_PROPERTIES1_SIZE_MASK_V2                                  0x07
#define CONFIGURATION_BULK_SET_PROPERTIES1_RESERVED_MASK_V2                              0x38
#define CONFIGURATION_BULK_SET_PROPERTIES1_RESERVED_SHIFT_V2                             0x03
#define CONFIGURATION_BULK_SET_PROPERTIES1_HANDSHAKE_BIT_MASK_V2                         0x40
#define CONFIGURATION_BULK_SET_PROPERTIES1_DEFAULT_BIT_MASK_V2                           0x80
/* Values used for Configuration Report command */
#define CONFIGURATION_REPORT_LEVEL_SIZE_MASK_V2                                          0x07
#define CONFIGURATION_REPORT_LEVEL_RESERVED_MASK_V2                                      0xF8
#define CONFIGURATION_REPORT_LEVEL_RESERVED_SHIFT_V2                                     0x03
/* Values used for Configuration Set command */
#define CONFIGURATION_SET_LEVEL_SIZE_MASK_V2                                             0x07
#define CONFIGURATION_SET_LEVEL_RESERVED_MASK_V2                                         0x78
#define CONFIGURATION_SET_LEVEL_RESERVED_SHIFT_V2                                        0x03
#define CONFIGURATION_SET_LEVEL_DEFAULT_BIT_MASK_V2                                      0x80

/* Controller Replication command class commands */
#define CONTROLLER_REPLICATION_VERSION                                                   0x01
#define CTRL_REPLICATION_TRANSFER_GROUP                                                  0x31
#define CTRL_REPLICATION_TRANSFER_GROUP_NAME                                             0x32
#define CTRL_REPLICATION_TRANSFER_SCENE                                                  0x33
#define CTRL_REPLICATION_TRANSFER_SCENE_NAME                                             0x34

/* Crc 16 Encap command class commands */
#define CRC_16_ENCAP_VERSION                                                             0x01
#define CRC_16_ENCAP                                                                     0x01

/* Dcp Config command class commands */
#define DCP_CONFIG_VERSION                                                               0x01
#define DCP_LIST_REMOVE                                                                  0x04
#define DCP_LIST_SET                                                                     0x03
#define DCP_LIST_SUPPORTED_GET                                                           0x01
#define DCP_LIST_SUPPORTED_REPORT                                                        0x02
/* Values used for Dcp List Set command */
#define DCP_LIST_SET_PROPERTIES1_NUMBER_OF_DC_MASK                                       0x03
#define DCP_LIST_SET_PROPERTIES1_RESERVED_MASK                                           0xFC
#define DCP_LIST_SET_PROPERTIES1_RESERVED_SHIFT                                          0x02

/* Dcp Monitor command class commands */
#define DCP_MONITOR_VERSION                                                              0x01
#define DCP_EVENT_STATUS_GET                                                             0x03
#define DCP_EVENT_STATUS_REPORT                                                          0x04
#define DCP_LIST_GET                                                                     0x01
#define DCP_LIST_REPORT                                                                  0x02
/* Values used for Dcp List Report command */
#define DCP_LIST_REPORT_PROPERTIES1_NUMBER_OF_DC_MASK                                    0x03
#define DCP_LIST_REPORT_PROPERTIES1_RESERVED_MASK                                        0xFC
#define DCP_LIST_REPORT_PROPERTIES1_RESERVED_SHIFT                                       0x02

/* Door Lock Logging command class commands */
#define DOOR_LOCK_LOGGING_VERSION                                                        0x01
#define DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_GET                                          0x01
#define DOOR_LOCK_LOGGING_RECORDS_SUPPORTED_REPORT                                       0x02
#define RECORD_GET                                                                       0x03
#define RECORD_REPORT                                                                    0x04
/* Values used for Record Report command */
#define RECORD_REPORT_PROPERTIES1_HOUR_LOCAL_TIME_MASK                                   0x1F
#define RECORD_REPORT_PROPERTIES1_RECORD_STATUS_MASK                                     0xE0
#define RECORD_REPORT_PROPERTIES1_RECORD_STATUS_SHIFT                                    0x05

/* Door Lock command class commands */
#define DOOR_LOCK_VERSION                                                                0x01
#define DOOR_LOCK_CONFIGURATION_GET                                                      0x05
#define DOOR_LOCK_CONFIGURATION_REPORT                                                   0x06
#define DOOR_LOCK_CONFIGURATION_SET                                                      0x04
#define DOOR_LOCK_OPERATION_GET                                                          0x02
#define DOOR_LOCK_OPERATION_REPORT                                                       0x03
#define DOOR_LOCK_OPERATION_SET                                                          0x01
/* Values used for Door Lock Configuration Report command */
#define DOOR_LOCK_CONFIGURATION_REPORT_CONSTANT_OPERATION                                0x01
#define DOOR_LOCK_CONFIGURATION_REPORT_TIMED_OPERATION                                   0x02
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK        0x0F
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK       0xF0
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT      0x04
/* Values used for Door Lock Configuration Set command */
#define DOOR_LOCK_CONFIGURATION_SET_CONSTANT_OPERATION                                   0x01
#define DOOR_LOCK_CONFIGURATION_SET_TIMED_OPERATION                                      0x02
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK           0x0F
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK          0xF0
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT         0x04
/* Values used for Door Lock Operation Report command */
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED                                        0x00
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_WITH_TIMEOUT                           0x01
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES                0x10
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT   0x11
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES               0x20
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT  0x21
#define DOOR_LOCK_OPERATION_REPORT_DOOR_SECURED                                          0xFF
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_MODE_MASK             0x0F
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_MASK            0xF0
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_SHIFT           0x04
/* Values used for Door Lock Operation Set command */
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED                                           0x00
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_WITH_TIMEOUT                              0x01
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES                   0x10
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT      0x11
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES                  0x20
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT     0x21
#define DOOR_LOCK_OPERATION_SET_DOOR_SECURED                                             0xFF

/* Door Lock command class commands */
#define DOOR_LOCK_VERSION_V2                                                             0x02
#define DOOR_LOCK_CONFIGURATION_GET_V2                                                   0x05
#define DOOR_LOCK_CONFIGURATION_REPORT_V2                                                0x06
#define DOOR_LOCK_CONFIGURATION_SET_V2                                                   0x04
#define DOOR_LOCK_OPERATION_GET_V2                                                       0x02
#define DOOR_LOCK_OPERATION_REPORT_V2                                                    0x03
#define DOOR_LOCK_OPERATION_SET_V2                                                       0x01
/* Values used for Door Lock Configuration Report command */
#define DOOR_LOCK_CONFIGURATION_REPORT_CONSTANT_OPERATION_V2                             0x01
#define DOOR_LOCK_CONFIGURATION_REPORT_TIMED_OPERATION_V2                                0x02
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK_V2     0x0F
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK_V2    0xF0
#define DOOR_LOCK_CONFIGURATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT_V2   0x04
/* Values used for Door Lock Configuration Set command */
#define DOOR_LOCK_CONFIGURATION_SET_CONSTANT_OPERATION_V2                                0x01
#define DOOR_LOCK_CONFIGURATION_SET_TIMED_OPERATION_V2                                   0x02
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_INSIDE_DOOR_HANDLES_STATE_MASK_V2        0x0F
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_MASK_V2       0xF0
#define DOOR_LOCK_CONFIGURATION_SET_PROPERTIES1_OUTSIDE_DOOR_HANDLES_STATE_SHIFT_V2      0x04
/* Values used for Door Lock Operation Report command */
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_V2                                     0x00
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_WITH_TIMEOUT_V2                        0x01
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_V2             0x10
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2 0x11
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_V2            0x20
#define DOOR_LOCK_OPERATION_REPORT_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2 0x21
#define DOOR_LOCK_OPERATION_REPORT_DOOR_LOCK_STATE_UNKNOWN_V2                            0xFE
#define DOOR_LOCK_OPERATION_REPORT_DOOR_SECURED_V2                                       0xFF
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_INSIDE_DOOR_HANDLES_MODE_MASK_V2          0x0F
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_MASK_V2         0xF0
#define DOOR_LOCK_OPERATION_REPORT_PROPERTIES1_OUTSIDE_DOOR_HANDLES_MODE_SHIFT_V2        0x04
/* Values used for Door Lock Operation Set command */
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_V2                                        0x00
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_WITH_TIMEOUT_V2                           0x01
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_V2                0x10
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2   0x11
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_V2               0x20
#define DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2  0x21
#define DOOR_LOCK_OPERATION_SET_DOOR_LOCK_STATE_UNKNOWN_V2                               0xFE
#define DOOR_LOCK_OPERATION_SET_DOOR_SECURED_V2                                          0xFF

/* Energy Production command class commands */
#define ENERGY_PRODUCTION_VERSION                                                        0x01
#define ENERGY_PRODUCTION_GET                                                            0x02
#define ENERGY_PRODUCTION_REPORT                                                         0x03
/* Values used for Energy Production Get command */
#define ENERGY_PRODUCTION_GET_INSTANT_ENERGY_PRODUCTION                                  0x00
#define ENERGY_PRODUCTION_GET_TOTAL_ENERGY_PRODUCTION                                    0x01
#define ENERGY_PRODUCTION_GET_ENERGY_PRODUCTION_TODAY                                    0x02
#define ENERGY_PRODUCTION_GET_TOTAL_PRODUCTION_TIME                                      0x03
/* Values used for Energy Production Report command */
#define ENERGY_PRODUCTION_REPORT_INSTANT_ENERGY_PRODUCTION                               0x00
#define ENERGY_PRODUCTION_REPORT_TOTAL_ENERGY_PRODUCTION                                 0x01
#define ENERGY_PRODUCTION_REPORT_ENERGY_PRODUCTION_TODAY                                 0x02
#define ENERGY_PRODUCTION_REPORT_TOTAL_PRODUCTION_TIME                                   0x03
#define ENERGY_PRODUCTION_REPORT_LEVEL_SIZE_MASK                                         0x07
#define ENERGY_PRODUCTION_REPORT_LEVEL_SCALE_MASK                                        0x18
#define ENERGY_PRODUCTION_REPORT_LEVEL_SCALE_SHIFT                                       0x03
#define ENERGY_PRODUCTION_REPORT_LEVEL_PRECISION_MASK                                    0xE0
#define ENERGY_PRODUCTION_REPORT_LEVEL_PRECISION_SHIFT                                   0x05

/* Firmware Update Md command class commands */
#define FIRMWARE_UPDATE_MD_VERSION                                                       0x01
#define FIRMWARE_MD_GET                                                                  0x01
#define FIRMWARE_MD_REPORT                                                               0x02
#define FIRMWARE_UPDATE_MD_GET                                                           0x05
#define FIRMWARE_UPDATE_MD_REPORT                                                        0x06
#define FIRMWARE_UPDATE_MD_REQUEST_GET                                                   0x03
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT                                                0x04
#define FIRMWARE_UPDATE_MD_STATUS_REPORT                                                 0x07
/* Values used for Firmware Update Md Get command */
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_REPORT_NUMBER_1_MASK                          0x7F
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_ZERO_BIT_MASK                                 0x80
/* Values used for Firmware Update Md Report command */
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_REPORT_NUMBER_1_MASK                       0x7F
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK                              0x80
/* Values used for Firmware Update Md Request Report command */
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_COMBINATION                            0x00
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_REQUIRES_AUTHENTICATION                        0x01
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION                              0xFF
/* Values used for Firmware Update Md Status Report command */
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR        0x00
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE                               0x01
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY                                    0xFF

/* Firmware Update Md command class commands */
#define FIRMWARE_UPDATE_MD_VERSION_V2                                                    0x02
#define FIRMWARE_MD_GET_V2                                                               0x01
#define FIRMWARE_MD_REPORT_V2                                                            0x02
#define FIRMWARE_UPDATE_MD_GET_V2                                                        0x05
#define FIRMWARE_UPDATE_MD_REPORT_V2                                                     0x06
#define FIRMWARE_UPDATE_MD_REQUEST_GET_V2                                                0x03
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_V2                                             0x04
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_V2                                              0x07
/* Values used for Firmware Update Md Get command */
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_REPORT_NUMBER_1_MASK_V2                       0x7F
#define FIRMWARE_UPDATE_MD_GET_PROPERTIES1_ZERO_BIT_MASK_V2                              0x80
/* Values used for Firmware Update Md Report command */
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_REPORT_NUMBER_1_MASK_V2                    0x7F
#define FIRMWARE_UPDATE_MD_REPORT_PROPERTIES1_LAST_BIT_MASK_V2                           0x80
/* Values used for Firmware Update Md Request Report command */
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_INVALID_COMBINATION_V2                         0x00
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_REQUIRES_AUTHENTICATION_V2                     0x01
#define FIRMWARE_UPDATE_MD_REQUEST_REPORT_VALID_COMBINATION_V2                           0xFF
/* Values used for Firmware Update Md Status Report command */
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_WITHOUT_CHECKSUM_ERROR_V2     0x00
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_UNABLE_TO_RECEIVE_V2                            0x01
#define FIRMWARE_UPDATE_MD_STATUS_REPORT_SUCCESSFULLY_V2                                 0xFF

/* Geographic Location command class commands */
#define GEOGRAPHIC_LOCATION_VERSION                                                      0x01
#define GEOGRAPHIC_LOCATION_GET                                                          0x02
#define GEOGRAPHIC_LOCATION_REPORT                                                       0x03
#define GEOGRAPHIC_LOCATION_SET                                                          0x01
/* Values used for Geographic Location Report command */
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL_LONGITUDE_MINUTES_MASK                          0x7F
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL_LONG_SIGN_BIT_MASK                              0x80
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL2_LATITUDE_MINUTES_MASK                          0x7F
#define GEOGRAPHIC_LOCATION_REPORT_LEVEL2_LAT_SIGN_BIT_MASK                              0x80
/* Values used for Geographic Location Set command */
#define GEOGRAPHIC_LOCATION_SET_LEVEL_LONGITUDE_MINUTES_MASK                             0x7F
#define GEOGRAPHIC_LOCATION_SET_LEVEL_LONG_SIGN_BIT_MASK                                 0x80
#define GEOGRAPHIC_LOCATION_SET_LEVEL2_LATITUDE_MINUTES_MASK                             0x7F
#define GEOGRAPHIC_LOCATION_SET_LEVEL2_LAT_SIGN_BIT_MASK                                 0x80

/* Grouping Name command class commands */
#define GROUPING_NAME_VERSION                                                            0x01
#define GROUPING_NAME_GET                                                                0x02
#define GROUPING_NAME_REPORT                                                             0x03
#define GROUPING_NAME_SET                                                                0x01
/* Values used for Grouping Name Report command */
#define GROUPING_NAME_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK                          0x07
#define GROUPING_NAME_REPORT_PROPERTIES1_RESERVED_MASK                                   0xF8
#define GROUPING_NAME_REPORT_PROPERTIES1_RESERVED_SHIFT                                  0x03
/* Values used for Grouping Name Set command */
#define GROUPING_NAME_SET_PROPERTIES1_CHAR_PRESENTATION_MASK                             0x07
#define GROUPING_NAME_SET_PROPERTIES1_RESERVED_MASK                                      0xF8
#define GROUPING_NAME_SET_PROPERTIES1_RESERVED_SHIFT                                     0x03

/* Hail command class commands */
#define HAIL_VERSION                                                                     0x01
#define HAIL                                                                             0x01

/* Hrv Control command class commands */
#define HRV_CONTROL_VERSION                                                              0x01
#define HRV_CONTROL_BYPASS_GET                                                           0x05
#define HRV_CONTROL_BYPASS_REPORT                                                        0x06
#define HRV_CONTROL_BYPASS_SET                                                           0x04
#define HRV_CONTROL_MODE_GET                                                             0x02
#define HRV_CONTROL_MODE_REPORT                                                          0x03
#define HRV_CONTROL_MODE_SET                                                             0x01
#define HRV_CONTROL_MODE_SUPPORTED_GET                                                   0x0A
#define HRV_CONTROL_MODE_SUPPORTED_REPORT                                                0x0B
#define HRV_CONTROL_VENTILATION_RATE_GET                                                 0x08
#define HRV_CONTROL_VENTILATION_RATE_REPORT                                              0x09
#define HRV_CONTROL_VENTILATION_RATE_SET                                                 0x07
/* Values used for Hrv Control Mode Report command */
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_MODE_MASK                                    0x1F
#define HRV_CONTROL_MODE_REPORT_MODE_OFF                                                 0x00
#define HRV_CONTROL_MODE_REPORT_MODE_DEMAND_AUTOMATIC                                    0x01
#define HRV_CONTROL_MODE_REPORT_MODE_SCHEDULE                                            0x02
#define HRV_CONTROL_MODE_REPORT_MODE_ENERGY_SAVINGS_MODE                                 0x03
#define HRV_CONTROL_MODE_REPORT_MODE_MANUAL                                              0x04
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_RESERVED_MASK                                0xE0
#define HRV_CONTROL_MODE_REPORT_PROPERTIES1_RESERVED_SHIFT                               0x05
/* Values used for Hrv Control Mode Set command */
#define HRV_CONTROL_MODE_SET_PROPERTIES1_MODE_MASK                                       0x1F
#define HRV_CONTROL_MODE_SET_MODE_OFF                                                    0x00
#define HRV_CONTROL_MODE_SET_MODE_DEMAND_AUTOMATIC                                       0x01
#define HRV_CONTROL_MODE_SET_MODE_SCHEDULE                                               0x02
#define HRV_CONTROL_MODE_SET_MODE_ENERGY_SAVINGS_MODE                                    0x03
#define HRV_CONTROL_MODE_SET_MODE_MANUAL                                                 0x04
#define HRV_CONTROL_MODE_SET_PROPERTIES1_RESERVED_MASK                                   0xE0
#define HRV_CONTROL_MODE_SET_PROPERTIES1_RESERVED_SHIFT                                  0x05
/* Values used for Hrv Control Mode Supported Report command */
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_MANUAL_CONTROL_SUPPORTED_MASK      0x0F
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_BYPASS_OPEN_CLOSE     0x00
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_BYPASS_AUTO           0x01
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_MODULATED_BYPASS      0x02
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_MANUAL_CONTROL_SUPPORTED_VENTILATION_RATE      0x03
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK                      0xF0
#define HRV_CONTROL_MODE_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT                     0x04

/* Hrv Status command class commands */
#define HRV_STATUS_VERSION                                                               0x01
#define HRV_STATUS_GET                                                                   0x01
#define HRV_STATUS_REPORT                                                                0x02
#define HRV_STATUS_SUPPORTED_GET                                                         0x03
#define HRV_STATUS_SUPPORTED_REPORT                                                      0x04
/* Values used for Hrv Status Get command */
#define HRV_STATUS_GET_OUTDOOR_AIR_TEMPERATURE                                           0x00
#define HRV_STATUS_GET_SUPPLY_AIR_TEMPERATURE                                            0x01
#define HRV_STATUS_GET_EXHAUST_AIR_TEMPERATURE                                           0x02
#define HRV_STATUS_GET_DISCHARGE_AIR_TEMPERATURE                                         0x03
#define HRV_STATUS_GET_ROOM_TEMPERATURE                                                  0x04
#define HRV_STATUS_GET_RELATIVE_HUMIDITY_IN_ROOM                                         0x05
#define HRV_STATUS_GET_REMAINING_FILTER_LIFE                                             0x06
/* Values used for Hrv Status Report command */
#define HRV_STATUS_REPORT_OUTDOOR_AIR_TEMPERATURE                                        0x00
#define HRV_STATUS_REPORT_SUPPLY_AIR_TEMPERATURE                                         0x01
#define HRV_STATUS_REPORT_EXHAUST_AIR_TEMPERATURE                                        0x02
#define HRV_STATUS_REPORT_DISCHARGE_AIR_TEMPERATURE                                      0x03
#define HRV_STATUS_REPORT_ROOM_TEMPERATURE                                               0x04
#define HRV_STATUS_REPORT_RELATIVE_HUMIDITY_IN_ROOM                                      0x05
#define HRV_STATUS_REPORT_REMAINING_FILTER_LIFE                                          0x06
#define HRV_STATUS_REPORT_PROPERTIES1_SIZE_MASK                                          0x07
#define HRV_STATUS_REPORT_PROPERTIES1_SCALE_MASK                                         0x18
#define HRV_STATUS_REPORT_PROPERTIES1_SCALE_SHIFT                                        0x03
#define HRV_STATUS_REPORT_PROPERTIES1_PRECISION_MASK                                     0xE0
#define HRV_STATUS_REPORT_PROPERTIES1_PRECISION_SHIFT                                    0x05

/* Indicator command class commands */
#define INDICATOR_VERSION                                                                0x01
#define INDICATOR_GET                                                                    0x02
#define INDICATOR_REPORT                                                                 0x03
#define INDICATOR_SET                                                                    0x01

/* Ip Configuration command class commands */
#define IP_CONFIGURATION_VERSION                                                         0x01
#define IP_CONFIGURATION_GET                                                             0x02
#define IP_CONFIGURATION_RELEASE                                                         0x04
#define IP_CONFIGURATION_RENEW                                                           0x05
#define IP_CONFIGURATION_REPORT                                                          0x03
#define IP_CONFIGURATION_SET                                                             0x01
/* Values used for Ip Configuration Report command */
#define IP_CONFIGURATION_REPORT_PROPERTIES1_AUTO_DNS_BIT_MASK                            0x01
#define IP_CONFIGURATION_REPORT_PROPERTIES1_AUTO_IP_BIT_MASK                             0x02
#define IP_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                                0xFC
#define IP_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                               0x02
/* Values used for Ip Configuration Set command */
#define IP_CONFIGURATION_SET_PROPERTIES1_AUTO_DNS_BIT_MASK                               0x01
#define IP_CONFIGURATION_SET_PROPERTIES1_AUTO_IP_BIT_MASK                                0x02
#define IP_CONFIGURATION_SET_PROPERTIES1_RESERVED_MASK                                   0xFC
#define IP_CONFIGURATION_SET_PROPERTIES1_RESERVED_SHIFT                                  0x02

/* Language command class commands */
#define LANGUAGE_VERSION                                                                 0x01
#define LANGUAGE_GET                                                                     0x02
#define LANGUAGE_REPORT                                                                  0x03
#define LANGUAGE_SET                                                                     0x01

/* Lock command class commands */
#define LOCK_VERSION                                                                     0x01
#define LOCK_GET                                                                         0x02
#define LOCK_REPORT                                                                      0x03
#define LOCK_SET                                                                         0x01

/* Manufacturer Proprietary command class commands */
#define MANUFACTURER_PROPRIETARY_VERSION                                                 0x01

/* Manufacturer Specific command class commands */
#define MANUFACTURER_SPECIFIC_VERSION                                                    0x01
#define MANUFACTURER_SPECIFIC_GET                                                        0x04
#define MANUFACTURER_SPECIFIC_REPORT                                                     0x05

/* Manufacturer Specific command class commands */
#define MANUFACTURER_SPECIFIC_VERSION_V2                                                 0x02
#define MANUFACTURER_SPECIFIC_GET_V2                                                     0x04
#define MANUFACTURER_SPECIFIC_REPORT_V2                                                  0x05
#define DEVICE_SPECIFIC_GET_V2                                                           0x06
#define DEVICE_SPECIFIC_REPORT_V2                                                        0x07
/* Values used for Device Specific Get command */
#define DEVICE_SPECIFIC_GET_PROPERTIES1_DEVICE_ID_TYPE_MASK_V2                           0x07
#define DEVICE_SPECIFIC_GET_DEVICE_ID_TYPE_RESERVED_V2                                   0x00
#define DEVICE_SPECIFIC_GET_DEVICE_ID_TYPE_SERIAL_NUMBER_V2                              0x01
#define DEVICE_SPECIFIC_GET_PROPERTIES1_RESERVED_MASK_V2                                 0xF8
#define DEVICE_SPECIFIC_GET_PROPERTIES1_RESERVED_SHIFT_V2                                0x03
/* Values used for Device Specific Report command */
#define DEVICE_SPECIFIC_REPORT_PROPERTIES1_DEVICE_ID_TYPE_MASK_V2                        0x07
#define DEVICE_SPECIFIC_REPORT_DEVICE_ID_TYPE_RESERVED_V2                                0x00
#define DEVICE_SPECIFIC_REPORT_DEVICE_ID_TYPE_SERIAL_NUMBER_V2                           0x01
#define DEVICE_SPECIFIC_REPORT_PROPERTIES1_RESERVED_MASK_V2                              0xF8
#define DEVICE_SPECIFIC_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                             0x03
#define DEVICE_SPECIFIC_REPORT_PROPERTIES2_DEVICE_ID_DATA_LENGTH_INDICATOR_MASK_V2       0x1F
#define DEVICE_SPECIFIC_REPORT_PROPERTIES2_DEVICE_ID_DATA_FORMAT_MASK_V2                 0xE0
#define DEVICE_SPECIFIC_REPORT_PROPERTIES2_DEVICE_ID_DATA_FORMAT_SHIFT_V2                0x05
#define DEVICE_SPECIFIC_REPORT_DEVICE_ID_DATA_FORMAT_RESERVED_V2                         0x00
#define DEVICE_SPECIFIC_REPORT_DEVICE_ID_DATA_FORMAT_BINARY_V2                           0x01

/* Mark command class commands */
#define MARK_VERSION                                                                     0x01

/* Meter Pulse command class commands */
#define METER_PULSE_VERSION                                                              0x01
#define METER_PULSE_GET                                                                  0x04
#define METER_PULSE_REPORT                                                               0x05

/* Meter Tbl Config command class commands */
#define METER_TBL_CONFIG_VERSION                                                         0x01
#define METER_TBL_TABLE_POINT_ADM_NO_SET                                                 0x01
/* Values used for Meter Tbl Table Point Adm No Set command */
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK           0x1F
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_RESERVED_MASK                       0xE0
#define METER_TBL_TABLE_POINT_ADM_NO_SET_PROPERTIES1_RESERVED_SHIFT                      0x05

/* Meter Tbl Monitor command class commands */
#define METER_TBL_MONITOR_VERSION                                                        0x01
#define METER_TBL_STATUS_REPORT                                                          0x0B
#define METER_TBL_STATUS_DATE_GET                                                        0x0A
#define METER_TBL_STATUS_DEPTH_GET                                                       0x09
#define METER_TBL_STATUS_SUPPORTED_GET                                                   0x07
#define METER_TBL_STATUS_SUPPORTED_REPORT                                                0x08
#define METER_TBL_CURRENT_DATA_GET                                                       0x0C
#define METER_TBL_CURRENT_DATA_REPORT                                                    0x0D
#define METER_TBL_HISTORICAL_DATA_GET                                                    0x0E
#define METER_TBL_HISTORICAL_DATA_REPORT                                                 0x0F
#define METER_TBL_REPORT                                                                 0x06
#define METER_TBL_TABLE_CAPABILITY_GET                                                   0x05
#define METER_TBL_TABLE_ID_GET                                                           0x03
#define METER_TBL_TABLE_ID_REPORT                                                        0x04
#define METER_TBL_TABLE_POINT_ADM_NO_GET                                                 0x01
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT                                              0x02
/* Values used for Meter Tbl Current Data Report command */
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK                         0x03
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_MASK                          0xFC
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT                         0x02
/* Values used for Meter Tbl Historical Data Report command */
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK                      0x03
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_MASK                       0xFC
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT                      0x02
/* Values used for Meter Tbl Report command */
#define METER_TBL_REPORT_PROPERTIES1_METER_TYPE_MASK                                     0x3F
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_MASK                                      0xC0
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_SHIFT                                     0x06
#define METER_TBL_REPORT_PROPERTIES2_PAY_METER_MASK                                      0x0F
#define METER_TBL_REPORT_PAY_METER_RESERVED                                              0x00
#define METER_TBL_REPORT_PAY_METER_CREDITMETER                                           0x01
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER                                      0x02
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER_DEBT                                 0x03
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_MASK                                       0xF0
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_SHIFT                                      0x04
/* Values used for Meter Tbl Table Id Report command */
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK                  0x1F
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_MASK                              0xE0
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_SHIFT                             0x05
/* Values used for Meter Tbl Table Point Adm No Report command */
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK        0x1F
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_MASK                    0xE0
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_SHIFT                   0x05

/* Meter Tbl Monitor command class commands */
#define METER_TBL_MONITOR_VERSION_V2                                                     0x02
#define METER_TBL_STATUS_REPORT_V2                                                       0x0B
#define METER_TBL_STATUS_DATE_GET_V2                                                     0x0A
#define METER_TBL_STATUS_DEPTH_GET_V2                                                    0x09
#define METER_TBL_STATUS_SUPPORTED_GET_V2                                                0x07
#define METER_TBL_STATUS_SUPPORTED_REPORT_V2                                             0x08
#define METER_TBL_CURRENT_DATA_GET_V2                                                    0x0C
#define METER_TBL_CURRENT_DATA_REPORT_V2                                                 0x0D
#define METER_TBL_HISTORICAL_DATA_GET_V2                                                 0x0E
#define METER_TBL_HISTORICAL_DATA_REPORT_V2                                              0x0F
#define METER_TBL_REPORT_V2                                                              0x06
#define METER_TBL_TABLE_CAPABILITY_GET_V2                                                0x05
#define METER_TBL_TABLE_ID_GET_V2                                                        0x03
#define METER_TBL_TABLE_ID_REPORT_V2                                                     0x04
#define METER_TBL_TABLE_POINT_ADM_NO_GET_V2                                              0x01
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_V2                                           0x02
/* Values used for Meter Tbl Current Data Report command */
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK_V2                      0x03
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_MASK_V2                       0x7C
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                      0x02
#define METER_TBL_CURRENT_DATA_REPORT_PROPERTIES1_OPERATING_STATUS_INDICATION_BIT_MASK_V2 0x80
/* Values used for Meter Tbl Historical Data Report command */
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RATE_TYPE_MASK_V2                   0x03
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_MASK_V2                    0x7C
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                   0x02
#define METER_TBL_HISTORICAL_DATA_REPORT_PROPERTIES1_OPERATING_STATUS_INDICATION_BIT_MASK_V2 0x80
/* Values used for Meter Tbl Report command */
#define METER_TBL_REPORT_PROPERTIES1_METER_TYPE_MASK_V2                                  0x3F
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_MASK_V2                                   0xC0
#define METER_TBL_REPORT_PROPERTIES1_RATE_TYPE_SHIFT_V2                                  0x06
#define METER_TBL_REPORT_PROPERTIES2_PAY_METER_MASK_V2                                   0x0F
#define METER_TBL_REPORT_PAY_METER_RESERVED_V2                                           0x00
#define METER_TBL_REPORT_PAY_METER_CREDITMETER_V2                                        0x01
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER_V2                                   0x02
#define METER_TBL_REPORT_PAY_METER_PREPAYMENT_METER_DEBT_V2                              0x03
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_MASK_V2                                    0xF0
#define METER_TBL_REPORT_PROPERTIES2_RESERVED_SHIFT_V2                                   0x04
/* Values used for Meter Tbl Table Id Report command */
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK_V2               0x1F
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_MASK_V2                           0xE0
#define METER_TBL_TABLE_ID_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                          0x05
/* Values used for Meter Tbl Table Point Adm No Report command */
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_NUMBER_OF_CHARACTERS_MASK_V2     0x1F
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_MASK_V2                 0xE0
#define METER_TBL_TABLE_POINT_ADM_NO_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                0x05

/* Meter Tbl Push command class commands */
#define METER_TBL_PUSH_VERSION                                                           0x01
#define METER_TBL_PUSH_CONFIGURATION_GET                                                 0x02
#define METER_TBL_PUSH_CONFIGURATION_REPORT                                              0x03
#define METER_TBL_PUSH_CONFIGURATION_SET                                                 0x01
/* Values used for Meter Tbl Push Configuration Report command */
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_OPERATING_STATUS_PUSH_MODE_MASK  0x0F
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_PS_BIT_MASK                      0x10
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_MASK                    0xE0
#define METER_TBL_PUSH_CONFIGURATION_REPORT_PROPERTIES1_RESERVED_SHIFT                   0x05
/* Values used for Meter Tbl Push Configuration Set command */
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_OPERATING_STATUS_PUSH_MODE_MASK     0x0F
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_PS_BIT_MASK                         0x10
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_RESERVED_MASK                       0xE0
#define METER_TBL_PUSH_CONFIGURATION_SET_PROPERTIES1_RESERVED_SHIFT                      0x05

/* Meter command class commands */
#define METER_VERSION                                                                    0x01
#define METER_GET                                                                        0x01
#define METER_REPORT                                                                     0x02
/* Values used for Meter Report command */
#define METER_REPORT_ELECTRIC_METER                                                      0x01
#define METER_REPORT_GAS_METER                                                           0x02
#define METER_REPORT_WATER_METER                                                         0x03
#define METER_REPORT_PROPERTIES1_SIZE_MASK                                               0x07
#define METER_REPORT_PROPERTIES1_SCALE_MASK                                              0x18
#define METER_REPORT_PROPERTIES1_SCALE_SHIFT                                             0x03
#define METER_REPORT_PROPERTIES1_PRECISION_MASK                                          0xE0
#define METER_REPORT_PROPERTIES1_PRECISION_SHIFT                                         0x05

/* Meter command class commands */
#define METER_VERSION_V2                                                                 0x02
#define METER_GET_V2                                                                     0x01
#define METER_REPORT_V2                                                                  0x02
#define METER_RESET_V2                                                                   0x05
#define METER_SUPPORTED_GET_V2                                                           0x03
#define METER_SUPPORTED_REPORT_V2                                                        0x04
/* Values used for Meter Get command */
#define METER_GET_PROPERTIES1_RESERVED_MASK_V2                                           0x07
#define METER_GET_PROPERTIES1_SCALE_MASK_V2                                              0x18
#define METER_GET_PROPERTIES1_SCALE_SHIFT_V2                                             0x03
#define METER_GET_PROPERTIES1_RESERVED2_MASK_V2                                          0xE0
#define METER_GET_PROPERTIES1_RESERVED2_SHIFT_V2                                         0x05
/* Values used for Meter Report command */
#define METER_REPORT_PROPERTIES1_METER_TYPE_MASK_V2                                      0x1F
#define METER_REPORT_PROPERTIES1_RATE_TYPE_MASK_V2                                       0x60
#define METER_REPORT_PROPERTIES1_RATE_TYPE_SHIFT_V2                                      0x05
#define METER_REPORT_PROPERTIES1_RESERVED_BIT_MASK_V2                                    0x80
#define METER_REPORT_PROPERTIES2_SIZE_MASK_V2                                            0x07
#define METER_REPORT_PROPERTIES2_SCALE_MASK_V2                                           0x18
#define METER_REPORT_PROPERTIES2_SCALE_SHIFT_V2                                          0x03
#define METER_REPORT_PROPERTIES2_PRECISION_MASK_V2                                       0xE0
#define METER_REPORT_PROPERTIES2_PRECISION_SHIFT_V2                                      0x05
/* Values used for Meter Supported Report command */
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_TYPE_MASK_V2                            0x1F
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V2                              0x60
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                             0x05
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_RESET_BIT_MASK_V2                       0x80
#define METER_SUPPORTED_REPORT_PROPERTIES2_SCALE_SUPPORTED_MASK_V2                       0x0F
#define METER_SUPPORTED_REPORT_PROPERTIES2_RESERVED_MASK_V2                              0xF0
#define METER_SUPPORTED_REPORT_PROPERTIES2_RESERVED_SHIFT_V2                             0x04

/* Meter command class commands */
#define METER_VERSION_V3                                                                 0x03
#define METER_GET_V3                                                                     0x01
#define METER_REPORT_V3                                                                  0x02
#define METER_RESET_V3                                                                   0x05
#define METER_SUPPORTED_GET_V3                                                           0x03
#define METER_SUPPORTED_REPORT_V3                                                        0x04
/* Values used for Meter Get command */
#define METER_GET_PROPERTIES1_RESERVED_MASK_V3                                           0x07
#define METER_GET_PROPERTIES1_SCALE_MASK_V3                                              0x38
#define METER_GET_PROPERTIES1_SCALE_SHIFT_V3                                             0x03
#define METER_GET_PROPERTIES1_RESERVED2_MASK_V3                                          0xC0
#define METER_GET_PROPERTIES1_RESERVED2_SHIFT_V3                                         0x06
/* Values used for Meter Report command */
#define METER_REPORT_PROPERTIES1_METER_TYPE_MASK_V3                                      0x1F
#define METER_REPORT_PROPERTIES1_RATE_TYPE_MASK_V3                                       0x60
#define METER_REPORT_PROPERTIES1_RATE_TYPE_SHIFT_V3                                      0x05
#define METER_REPORT_PROPERTIES1_SCALE2_BIT_MASK_V3                                      0x80
#define METER_REPORT_PROPERTIES2_SIZE_MASK_V3                                            0x07
#define METER_REPORT_PROPERTIES2_SCALE_MASK_V3                                           0x18
#define METER_REPORT_PROPERTIES2_SCALE_SHIFT_V3                                          0x03
#define METER_REPORT_PROPERTIES2_PRECISION_MASK_V3                                       0xE0
#define METER_REPORT_PROPERTIES2_PRECISION_SHIFT_V3                                      0x05
/* Values used for Meter Supported Report command */
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_TYPE_MASK_V3                            0x1F
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                              0x60
#define METER_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                             0x05
#define METER_SUPPORTED_REPORT_PROPERTIES1_METER_RESET_BIT_MASK_V3                       0x80

/* Mtp Window Covering command class commands */
#define MTP_WINDOW_COVERING_VERSION                                                      0x01
#define MOVE_TO_POSITION_GET                                                             0x02
#define MOVE_TO_POSITION_REPORT                                                          0x03
#define MOVE_TO_POSITION_SET                                                             0x01

/* Multi Channel Association command class commands */
#define MULTI_CHANNEL_ASSOCIATION_VERSION_V2                                             0x02
#define MULTI_CHANNEL_ASSOCIATION_GET_V2                                                 0x02
#define MULTI_CHANNEL_ASSOCIATION_GROUPINGS_GET_V2                                       0x05
#define MULTI_CHANNEL_ASSOCIATION_GROUPINGS_REPORT_V2                                    0x06
#define MULTI_CHANNEL_ASSOCIATION_REMOVE_V2                                              0x04
#define MULTI_CHANNEL_ASSOCIATION_REPORT_V2                                              0x03
#define MULTI_CHANNEL_ASSOCIATION_SET_V2                                                 0x01
/* Values used for Multi Channel Association Remove command */
#define MULTI_CHANNEL_ASSOCIATION_REMOVE_MARKER_V2                                       0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
/* Values used for Multi Channel Association Report command */
#define MULTI_CHANNEL_ASSOCIATION_REPORT_MARKER_V2                                       0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
/* Values used for Multi Channel Association Set command */
#define MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2                                          0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/

/* Multi Channel command class commands */
#define MULTI_CHANNEL_VERSION_V2                                                         0x02
#define MULTI_CHANNEL_CAPABILITY_GET_V2                                                  0x09
#define MULTI_CHANNEL_CAPABILITY_REPORT_V2                                               0x0A
#define MULTI_CHANNEL_CMD_ENCAP_V2                                                       0x0D
#define MULTI_CHANNEL_END_POINT_FIND_V2                                                  0x0B
#define MULTI_CHANNEL_END_POINT_FIND_REPORT_V2                                           0x0C
#define MULTI_CHANNEL_END_POINT_GET_V2                                                   0x07
#define MULTI_CHANNEL_END_POINT_REPORT_V2                                                0x08
#define MULTI_INSTANCE_CMD_ENCAP_V2                                                      0x06
#define MULTI_INSTANCE_GET_V2                                                            0x04
#define MULTI_INSTANCE_REPORT_V2                                                         0x05
/* Values used for Multi Channel Capability Get command */
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_END_POINT_MASK_V2                       0x7F
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_RES_BIT_MASK_V2                         0x80
/* Values used for Multi Channel Capability Report command */
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_END_POINT_MASK_V2                    0x7F
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V2                  0x80
/* Values used for Multi Channel Cmd Encap command */
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_SOURCE_END_POINT_MASK_V2                     0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V2                              0x80
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_DESTINATION_END_POINT_MASK_V2                0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_BIT_ADDRESS_BIT_MASK_V2                      0x80
/* Values used for Multi Channel End Point Report command */
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_RES_MASK_V2                           0x3F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_IDENTICAL_BIT_MASK_V2                 0x40
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V2                   0x80
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_END_POINTS_MASK_V2                    0x7F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_RES_BIT_MASK_V2                       0x80
/* Values used for Multi Instance Cmd Encap command */
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_INSTANCE_MASK_V2                            0x7F
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V2                             0x80
/* Values used for Multi Instance Report command */
#define MULTI_INSTANCE_REPORT_PROPERTIES1_INSTANCES_MASK_V2                              0x7F
#define MULTI_INSTANCE_REPORT_PROPERTIES1_RES_BIT_MASK_V2                                0x80

/* Multi Channel command class commands */
#define MULTI_CHANNEL_VERSION_V3                                                         0x03
#define MULTI_CHANNEL_CAPABILITY_GET_V3                                                  0x09
#define MULTI_CHANNEL_CAPABILITY_REPORT_V3                                               0x0A
#define MULTI_CHANNEL_CMD_ENCAP_V3                                                       0x0D
#define MULTI_CHANNEL_END_POINT_FIND_V3                                                  0x0B
#define MULTI_CHANNEL_END_POINT_FIND_REPORT_V3                                           0x0C
#define MULTI_CHANNEL_END_POINT_GET_V3                                                   0x07
#define MULTI_CHANNEL_END_POINT_REPORT_V3                                                0x08
#define MULTI_INSTANCE_CMD_ENCAP_V3                                                      0x06
#define MULTI_INSTANCE_GET_V3                                                            0x04
#define MULTI_INSTANCE_REPORT_V3                                                         0x05
/* Values used for Multi Channel Capability Get command */
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_END_POINT_MASK_V3                       0x7F
#define MULTI_CHANNEL_CAPABILITY_GET_PROPERTIES1_RES_BIT_MASK_V3                         0x80
/* Values used for Multi Channel Capability Report command */
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_END_POINT_MASK_V3                    0x7F
#define MULTI_CHANNEL_CAPABILITY_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V3                  0x80
/* Values used for Multi Channel Cmd Encap command */
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_SOURCE_END_POINT_MASK_V3                     0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V3                              0x80
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_DESTINATION_END_POINT_MASK_V3                0x7F
#define MULTI_CHANNEL_CMD_ENCAP_PROPERTIES2_BIT_ADDRESS_BIT_MASK_V3                      0x80
/* Values used for Multi Channel End Point Report command */
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_RES_MASK_V3                           0x3F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_IDENTICAL_BIT_MASK_V3                 0x40
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES1_DYNAMIC_BIT_MASK_V3                   0x80
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_END_POINTS_MASK_V3                    0x7F
#define MULTI_CHANNEL_END_POINT_REPORT_PROPERTIES2_RES_BIT_MASK_V3                       0x80
/* Values used for Multi Instance Cmd Encap command */
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_INSTANCE_MASK_V3                            0x7F
#define MULTI_INSTANCE_CMD_ENCAP_PROPERTIES1_RES_BIT_MASK_V3                             0x80
/* Values used for Multi Instance Report command */
#define MULTI_INSTANCE_REPORT_PROPERTIES1_INSTANCES_MASK_V3                              0x7F
#define MULTI_INSTANCE_REPORT_PROPERTIES1_RES_BIT_MASK_V3                                0x80

/* Multi Cmd command class commands */
#define MULTI_CMD_VERSION                                                                0x01
#define MULTI_CMD_ENCAP                                                                  0x01

/* Multi Instance Association command class commands */
#define MULTI_INSTANCE_ASSOCIATION_VERSION                                               0x01
#define MULTI_INSTANCE_ASSOCIATION_GET                                                   0x02
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_GET                                         0x05
#define MULTI_INSTANCE_ASSOCIATION_GROUPINGS_REPORT                                      0x06
#define MULTI_INSTANCE_ASSOCIATION_REMOVE                                                0x04
#define MULTI_INSTANCE_ASSOCIATION_REPORT                                                0x03
#define MULTI_INSTANCE_ASSOCIATION_SET                                                   0x01
/* Values used for Multi Instance Association Remove command */
#define MULTI_INSTANCE_ASSOCIATION_REMOVE_MARKER                                         0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
/* Values used for Multi Instance Association Report command */
#define MULTI_INSTANCE_ASSOCIATION_REPORT_MARKER                                         0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/
/* Values used for Multi Instance Association Set command */
#define MULTI_INSTANCE_ASSOCIATION_SET_MARKER                                            0x00 /*This marker identifier is used to separate between nodes without and with end points attached. This field can be omitted in case no Multi Channel node follows.*/

/* Multi Instance command class commands */
#define MULTI_INSTANCE_VERSION                                                           0x01
#define MULTI_INSTANCE_CMD_ENCAP                                                         0x06
#define MULTI_INSTANCE_GET                                                               0x04
#define MULTI_INSTANCE_REPORT                                                            0x05

/* Network Management Proxy command class commands */
#define NETWORK_MANAGEMENT_PROXY_VERSION                                                 0x01
#define NODE_INFO_CACHED_GET                                                             0x03
#define NODE_INFO_CACHED_REPORT                                                          0x04
#define NODE_LIST_GET                                                                    0x01
#define NODE_LIST_REPORT                                                                 0x02
/* Values used for Node Info Cached Get command */
#define NODE_INFO_CACHED_GET_PROPERTIES1_MAX_AGE_MASK                                    0x0F
#define NODE_INFO_CACHED_GET_PROPERTIES1_RESERVED_MASK                                   0xF0
#define NODE_INFO_CACHED_GET_PROPERTIES1_RESERVED_SHIFT                                  0x04
/* Values used for Node Info Cached Report command */
#define NODE_INFO_CACHED_REPORT_PROPERTIES1_AGE_MASK                                     0x0F
#define NODE_INFO_CACHED_REPORT_PROPERTIES1_STATUS_MASK                                  0xF0
#define NODE_INFO_CACHED_REPORT_PROPERTIES1_STATUS_SHIFT                                 0x04
#define NODE_INFO_CACHED_REPORT_STATUS_STATUS_OK                                         0x00
#define NODE_INFO_CACHED_REPORT_STATUS_STATUS_NOT_RESPONDING                             0x01
#define NODE_INFO_CACHED_REPORT_STATUS_STATUS_UNKNOWN                                    0x02
#define NODE_INFO_CACHED_REPORT_PROPERTIES2_CAPABILITY_MASK                              0x7F
#define NODE_INFO_CACHED_REPORT_PROPERTIES2_LISTENING_BIT_MASK                           0x80
#define NODE_INFO_CACHED_REPORT_PROPERTIES3_SECURITY_MASK                                0x0F
#define NODE_INFO_CACHED_REPORT_PROPERTIES3_SENSOR_MASK                                  0x70
#define NODE_INFO_CACHED_REPORT_PROPERTIES3_SENSOR_SHIFT                                 0x04
#define NODE_INFO_CACHED_REPORT_PROPERTIES3_OPT_BIT_MASK                                 0x80
#define NODE_INFO_CACHED_REPORT_SECURITY_SCHEME_0_MARK                                   0xF100

/* Network Management Basic command class commands */
#define NETWORK_MANAGEMENT_BASIC_VERSION                                                 0x01
#define LEARN_MODE_SET                                                                   0x01
#define LEARN_MODE_SET_STATUS                                                            0x02
#define NODE_INFORMATION_SEND                                                            0x05
#define NETWORK_UPDATE_REQUEST                                                           0x03
#define NETWORK_UPDATE_REQUEST_STATUS                                                    0x04
#define DEFAULT_SET                                                                      0x06
#define DEFAULT_SET_COMPLETE                                                             0x07

/* Network Management Inclusion command class commands */
#define NETWORK_MANAGEMENT_INCLUSION_VERSION                                             0x01
#define FAILED_NODE_REMOVE                                                               0x07
#define FAILED_NODE_REMOVE_STATUS                                                        0x08
#define NODE_ADD                                                                         0x01
#define NODE_ADD_STATUS                                                                  0x02
#define NODE_REMOVE                                                                      0x03
#define NODE_REMOVE_STATUS                                                               0x04
#define FAILED_NODE_REPLACE                                                              0x09
#define FAILED_NODE_REPLACE_STATUS                                                       0x0A
#define NODE_NEIGHBOR_UPDATE_REQUEST                                                     0x0B
#define NODE_NEIGHBOR_UPDATE_STATUS                                                      0x0C
#define RETURN_ROUTE_ASSIGN                                                              0x0D
#define RETURN_ROUTE_ASSIGN_COMPLETE                                                     0x0E
#define RETURN_ROUTE_DELETE                                                              0x0F
#define RETURN_ROUTE_DELETE_COMPLETE                                                     0x10
/* Values used for Node Add Status command */
#define NODE_ADD_STATUS_PROPERTIES1_CAPABILITY_MASK                                      0x7F
#define NODE_ADD_STATUS_PROPERTIES1_LISTENING_BIT_MASK                                   0x80
#define NODE_ADD_STATUS_PROPERTIES2_SECURITY_MASK                                        0x7F
#define NODE_ADD_STATUS_PROPERTIES2_OPT_BIT_MASK                                         0x80

/* No Operation command class commands */
#define NO_OPERATION_VERSION                                                             0x01

/* Node Naming command class commands */
#define NODE_NAMING_VERSION                                                              0x01
#define NODE_NAMING_NODE_LOCATION_REPORT                                                 0x06
#define NODE_NAMING_NODE_LOCATION_SET                                                    0x04
#define NODE_NAMING_NODE_LOCATION_GET                                                    0x05
#define NODE_NAMING_NODE_NAME_GET                                                        0x02
#define NODE_NAMING_NODE_NAME_REPORT                                                     0x03
#define NODE_NAMING_NODE_NAME_SET                                                        0x01
/* Values used for Node Naming Node Location Report command */
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_CHAR_PRESENTATION_MASK                    0x07
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_RESERVED_MASK                             0xF8
#define NODE_NAMING_NODE_LOCATION_REPORT_LEVEL_RESERVED_SHIFT                            0x03
/* Values used for Node Naming Node Location Set command */
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_CHAR_PRESENTATION_MASK                       0x07
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_RESERVED_MASK                                0xF8
#define NODE_NAMING_NODE_LOCATION_SET_LEVEL_RESERVED_SHIFT                               0x03
/* Values used for Node Naming Node Name Report command */
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_CHAR_PRESENTATION_MASK                        0x07
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_RESERVED_MASK                                 0xF8
#define NODE_NAMING_NODE_NAME_REPORT_LEVEL_RESERVED_SHIFT                                0x03
/* Values used for Node Naming Node Name Set command */
#define NODE_NAMING_NODE_NAME_SET_LEVEL_CHAR_PRESENTATION_MASK                           0x07
#define NODE_NAMING_NODE_NAME_SET_LEVEL_RESERVED_MASK                                    0xF8
#define NODE_NAMING_NODE_NAME_SET_LEVEL_RESERVED_SHIFT                                   0x03

/* Non Interoperable command class commands */
#define NON_INTEROPERABLE_VERSION                                                        0x01

/* Powerlevel command class commands */
#define POWERLEVEL_VERSION                                                               0x01
#define POWERLEVEL_GET                                                                   0x02
#define POWERLEVEL_REPORT                                                                0x03
#define POWERLEVEL_SET                                                                   0x01
#define POWERLEVEL_TEST_NODE_GET                                                         0x05
#define POWERLEVEL_TEST_NODE_REPORT                                                      0x06
#define POWERLEVEL_TEST_NODE_SET                                                         0x04
/* Values used for Powerlevel Report command */
#define POWERLEVEL_REPORT_NORMALPOWER                                                    0x00
#define POWERLEVEL_REPORT_MINUS1DBM                                                      0x01
#define POWERLEVEL_REPORT_MINUS2DBM                                                      0x02
#define POWERLEVEL_REPORT_MINUS3DBM                                                      0x03
#define POWERLEVEL_REPORT_MINUS4DBM                                                      0x04
#define POWERLEVEL_REPORT_MINUS5DBM                                                      0x05
#define POWERLEVEL_REPORT_MINUS6DBM                                                      0x06
#define POWERLEVEL_REPORT_MINUS7DBM                                                      0x07
#define POWERLEVEL_REPORT_MINUS8DBM                                                      0x08
#define POWERLEVEL_REPORT_MINUS9DBM                                                      0x09
/* Values used for Powerlevel Set command */
#define POWERLEVEL_SET_NORMALPOWER                                                       0x00
#define POWERLEVEL_SET_MINUS1DBM                                                         0x01
#define POWERLEVEL_SET_MINUS2DBM                                                         0x02
#define POWERLEVEL_SET_MINUS3DBM                                                         0x03
#define POWERLEVEL_SET_MINUS4DBM                                                         0x04
#define POWERLEVEL_SET_MINUS5DBM                                                         0x05
#define POWERLEVEL_SET_MINUS6DBM                                                         0x06
#define POWERLEVEL_SET_MINUS7DBM                                                         0x07
#define POWERLEVEL_SET_MINUS8DBM                                                         0x08
#define POWERLEVEL_SET_MINUS9DBM                                                         0x09
/* Values used for Powerlevel Test Node Report command */
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_FAILED                                       0x00
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_SUCCES                                       0x01
#define POWERLEVEL_TEST_NODE_REPORT_ZW_TEST_INPROGRESS                                   0x02
/* Values used for Powerlevel Test Node Set command */
#define POWERLEVEL_TEST_NODE_SET_NORMALPOWER                                             0x00
#define POWERLEVEL_TEST_NODE_SET_MINUS1DBM                                               0x01
#define POWERLEVEL_TEST_NODE_SET_MINUS2DBM                                               0x02
#define POWERLEVEL_TEST_NODE_SET_MINUS3DBM                                               0x03
#define POWERLEVEL_TEST_NODE_SET_MINUS4DBM                                               0x04
#define POWERLEVEL_TEST_NODE_SET_MINUS5DBM                                               0x05
#define POWERLEVEL_TEST_NODE_SET_MINUS6DBM                                               0x06
#define POWERLEVEL_TEST_NODE_SET_MINUS7DBM                                               0x07
#define POWERLEVEL_TEST_NODE_SET_MINUS8DBM                                               0x08
#define POWERLEVEL_TEST_NODE_SET_MINUS9DBM                                               0x09

/* Prepayment Encapsulation command class commands */
#define PREPAYMENT_ENCAPSULATION_VERSION                                                 0x01
#define CMD_ENCAPSULATION                                                                0x01

/* Prepayment command class commands */
#define PREPAYMENT_VERSION                                                               0x01
#define PREPAYMENT_BALANCE_GET                                                           0x01
#define PREPAYMENT_BALANCE_REPORT                                                        0x02
#define PREPAYMENT_SUPPORTED_GET                                                         0x03
#define PREPAYMENT_SUPPORTED_REPORT                                                      0x04
/* Values used for Prepayment Balance Get command */
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_RESERVED_MASK                                 0x3F
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_BALANCE_TYPE_MASK                             0xC0
#define PREPAYMENT_BALANCE_GET_PROPERTIES1_BALANCE_TYPE_SHIFT                            0x06
#define PREPAYMENT_BALANCE_GET_BALANCE_TYPE_UTILITY                                      0x00
#define PREPAYMENT_BALANCE_GET_BALANCE_TYPE_MONETARY                                     0x01
/* Values used for Prepayment Balance Report command */
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_METER_TYPE_MASK                            0x3F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_BALANCE_TYPE_MASK                          0xC0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES1_BALANCE_TYPE_SHIFT                         0x06
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_SCALE_MASK                                 0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_BALANCE_PRECISION_MASK                     0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES2_BALANCE_PRECISION_SHIFT                    0x05
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_RESERVED_MASK                              0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_DEBT_PRECISION_MASK                        0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES3_DEBT_PRECISION_SHIFT                       0x05
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_RESERVED_MASK                              0x1F
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_EMER_CREDIT_PRECISION_MASK                 0xE0
#define PREPAYMENT_BALANCE_REPORT_PROPERTIES4_EMER_CREDIT_PRECISION_SHIFT                0x05
/* Values used for Prepayment Supported Report command */
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_TYPES_SUPPORTED_MASK                     0x0F
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK                            0xF0
#define PREPAYMENT_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT                           0x04

/* Proprietary command class commands */
#define PROPRIETARY_VERSION                                                              0x01
#define PROPRIETARY_GET                                                                  0x02
#define PROPRIETARY_REPORT                                                               0x03
#define PROPRIETARY_SET                                                                  0x01

/* Protection command class commands */
#define PROTECTION_VERSION                                                               0x01
#define PROTECTION_GET                                                                   0x02
#define PROTECTION_REPORT                                                                0x03
#define PROTECTION_SET                                                                   0x01
/* Values used for Protection Report command */
#define PROTECTION_REPORT_UNPROTECTED                                                    0x00
#define PROTECTION_REPORT_PROTECTION_BY_SEQUENCE                                         0x01
#define PROTECTION_REPORT_NO_OPERATION_POSSIBLE                                          0x02
/* Values used for Protection Set command */
#define PROTECTION_SET_UNPROTECTED                                                       0x00
#define PROTECTION_SET_PROTECTION_BY_SEQUENCE                                            0x01
#define PROTECTION_SET_NO_OPERATION_POSSIBLE                                             0x02

/* Protection command class commands */
#define PROTECTION_VERSION_V2                                                            0x02
#define PROTECTION_EC_GET_V2                                                             0x07
#define PROTECTION_EC_REPORT_V2                                                          0x08
#define PROTECTION_EC_SET_V2                                                             0x06
#define PROTECTION_GET_V2                                                                0x02
#define PROTECTION_REPORT_V2                                                             0x03
#define PROTECTION_SET_V2                                                                0x01
#define PROTECTION_SUPPORTED_GET_V2                                                      0x04
#define PROTECTION_SUPPORTED_REPORT_V2                                                   0x05
#define PROTECTION_TIMEOUT_GET_V2                                                        0x0A
#define PROTECTION_TIMEOUT_REPORT_V2                                                     0x0B
#define PROTECTION_TIMEOUT_SET_V2                                                        0x09
/* Values used for Protection Report command */
#define PROTECTION_REPORT_LEVEL_LOCAL_PROTECTION_STATE_MASK_V2                           0x0F
#define PROTECTION_REPORT_LEVEL_RESERVED_MASK_V2                                         0xF0
#define PROTECTION_REPORT_LEVEL_RESERVED_SHIFT_V2                                        0x04
#define PROTECTION_REPORT_LEVEL2_RF_PROTECTION_STATE_MASK_V2                             0x0F
#define PROTECTION_REPORT_LEVEL2_RESERVED_MASK_V2                                        0xF0
#define PROTECTION_REPORT_LEVEL2_RESERVED_SHIFT_V2                                       0x04
/* Values used for Protection Set command */
#define PROTECTION_SET_LEVEL_LOCAL_PROTECTION_STATE_MASK_V2                              0x0F
#define PROTECTION_SET_LEVEL_RESERVED_MASK_V2                                            0xF0
#define PROTECTION_SET_LEVEL_RESERVED_SHIFT_V2                                           0x04
#define PROTECTION_SET_LEVEL2_RF_PROTECTION_STATE_MASK_V2                                0x0F
#define PROTECTION_SET_LEVEL2_RESERVED_MASK_V2                                           0xF0
#define PROTECTION_SET_LEVEL2_RESERVED_SHIFT_V2                                          0x04
/* Values used for Protection Supported Report command */
#define PROTECTION_SUPPORTED_REPORT_LEVEL_TIMEOUT_BIT_MASK_V2                            0x01
#define PROTECTION_SUPPORTED_REPORT_LEVEL_EXCLUSIVE_CONTROL_BIT_MASK_V2                  0x02
#define PROTECTION_SUPPORTED_REPORT_LEVEL_RESERVED_MASK_V2                               0xFC
#define PROTECTION_SUPPORTED_REPORT_LEVEL_RESERVED_SHIFT_V2                              0x02

/* Rate Tbl Config command class commands */
#define RATE_TBL_CONFIG_VERSION                                                          0x01
#define RATE_TBL_REMOVE                                                                  0x02
#define RATE_TBL_SET                                                                     0x01
/* Values used for Rate Tbl Remove command */
#define RATE_TBL_REMOVE_PROPERTIES1_RATE_PARAMETER_SET_IDS_MASK                          0x3F
#define RATE_TBL_REMOVE_PROPERTIES1_RESERVED_MASK                                        0xC0
#define RATE_TBL_REMOVE_PROPERTIES1_RESERVED_SHIFT                                       0x06
/* Values used for Rate Tbl Set command */
#define RATE_TBL_SET_PROPERTIES1_NUMBER_OF_RATE_CHAR_MASK                                0x1F
#define RATE_TBL_SET_PROPERTIES1_RATE_TYPE_MASK                                          0x60
#define RATE_TBL_SET_PROPERTIES1_RATE_TYPE_SHIFT                                         0x05
#define RATE_TBL_SET_PROPERTIES1_RESERVED_BIT_MASK                                       0x80
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_SCALE_MASK                                  0x1F
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_PRECISION_MASK                              0xE0
#define RATE_TBL_SET_PROPERTIES2_CONSUMPTION_PRECISION_SHIFT                             0x05
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_SCALE_MASK                                   0x1F
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_PRECISION_MASK                               0xE0
#define RATE_TBL_SET_PROPERTIES3_MAX_DEMAND_PRECISION_SHIFT                              0x05

/* Rate Tbl Monitor command class commands */
#define RATE_TBL_MONITOR_VERSION                                                         0x01
#define RATE_TBL_ACTIVE_RATE_GET                                                         0x05
#define RATE_TBL_ACTIVE_RATE_REPORT                                                      0x06
#define RATE_TBL_CURRENT_DATA_GET                                                        0x07
#define RATE_TBL_CURRENT_DATA_REPORT                                                     0x08
#define RATE_TBL_GET                                                                     0x03
#define RATE_TBL_HISTORICAL_DATA_GET                                                     0x09
#define RATE_TBL_HISTORICAL_DATA_REPORT                                                  0x0A
#define RATE_TBL_REPORT                                                                  0x04
#define RATE_TBL_SUPPORTED_GET                                                           0x01
#define RATE_TBL_SUPPORTED_REPORT                                                        0x02
/* Values used for Rate Tbl Report command */
#define RATE_TBL_REPORT_PROPERTIES1_NUMBER_OF_RATE_CHAR_MASK                             0x1F
#define RATE_TBL_REPORT_PROPERTIES1_RATE_TYPE_MASK                                       0x60
#define RATE_TBL_REPORT_PROPERTIES1_RATE_TYPE_SHIFT                                      0x05
#define RATE_TBL_REPORT_PROPERTIES1_RESERVED_BIT_MASK                                    0x80
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_SCALE_MASK                               0x1F
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_PRECISION_MASK                           0xE0
#define RATE_TBL_REPORT_PROPERTIES2_CONSUMPTION_PRECISION_SHIFT                          0x05
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_SCALE_MASK                                0x1F
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_PRECISION_MASK                            0xE0
#define RATE_TBL_REPORT_PROPERTIES3_MAX_DEMAND_PRECISION_SHIFT                           0x05

/* Remote Association Activate command class commands */
#define REMOTE_ASSOCIATION_ACTIVATE_VERSION                                              0x01
#define REMOTE_ASSOCIATION_ACTIVATE                                                      0x01

/* Remote Association command class commands */
#define REMOTE_ASSOCIATION_VERSION                                                       0x01
#define REMOTE_ASSOCIATION_CONFIGURATION_GET                                             0x02
#define REMOTE_ASSOCIATION_CONFIGURATION_REPORT                                          0x03
#define REMOTE_ASSOCIATION_CONFIGURATION_SET                                             0x01

/* Scene Activation command class commands */
#define SCENE_ACTIVATION_VERSION                                                         0x01
#define SCENE_ACTIVATION_SET                                                             0x01

/* Scene Actuator Conf command class commands */
#define SCENE_ACTUATOR_CONF_VERSION                                                      0x01
#define SCENE_ACTUATOR_CONF_GET                                                          0x02
#define SCENE_ACTUATOR_CONF_REPORT                                                       0x03
#define SCENE_ACTUATOR_CONF_SET                                                          0x01
/* Values used for Scene Actuator Conf Set command */
#define SCENE_ACTUATOR_CONF_SET_LEVEL2_RESERVED_MASK                                     0x7F
#define SCENE_ACTUATOR_CONF_SET_LEVEL2_OVERRIDE_BIT_MASK                                 0x80

/* Scene Controller Conf command class commands */
#define SCENE_CONTROLLER_CONF_VERSION                                                    0x01
#define SCENE_CONTROLLER_CONF_GET                                                        0x02
#define SCENE_CONTROLLER_CONF_REPORT                                                     0x03
#define SCENE_CONTROLLER_CONF_SET                                                        0x01

/* Schedule Entry Lock command class commands */
#define SCHEDULE_ENTRY_LOCK_VERSION                                                      0x01
#define SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET                                               0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_SET                                                   0x01
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET                                                 0x04
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT                                              0x05
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET                                                 0x03
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET                                                 0x07
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT                                              0x08
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET                                                 0x06
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_GET                                                0x09
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT                                             0x0A

/* Schedule Entry Lock command class commands */
#define SCHEDULE_ENTRY_LOCK_VERSION_V2                                                   0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V2                                            0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_SET_V2                                                0x01
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V2                                           0x0B
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V2                                        0x0C
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V2                                           0x0D
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V2                                              0x04
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V2                                           0x05
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V2                                              0x03
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V2                                              0x07
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V2                                           0x08
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V2                                              0x06
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V2                                             0x09
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V2                                          0x0A
/* Values used for Schedule Entry Lock Time Offset Report command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_HOUR_TZO_MASK_V2                    0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_SIGN_TZO_BIT_MASK_V2                0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_MINUTE_OFFSET_DST_MASK_V2          0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2        0x80
/* Values used for Schedule Entry Lock Time Offset Set command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_HOUR_TZO_MASK_V2                       0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_SIGN_TZO_BIT_MASK_V2                   0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_MINUTE_OFFSET_DST_MASK_V2             0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2           0x80

/* Schedule Entry Lock command class commands */
#define SCHEDULE_ENTRY_LOCK_VERSION_V3                                                   0x03
#define SCHEDULE_ENTRY_LOCK_ENABLE_ALL_SET_V3                                            0x02
#define SCHEDULE_ENTRY_LOCK_ENABLE_SET_V3                                                0x01
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_GET_V3                                           0x0B
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_V3                                        0x0C
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_V3                                           0x0D
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_GET_V3                                              0x04
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_REPORT_V3                                           0x05
#define SCHEDULE_ENTRY_LOCK_WEEK_DAY_SET_V3                                              0x03
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_GET_V3                                              0x07
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_REPORT_V3                                           0x08
#define SCHEDULE_ENTRY_LOCK_YEAR_DAY_SET_V3                                              0x06
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_GET_V3                                             0x09
#define SCHEDULE_ENTRY_TYPE_SUPPORTED_REPORT_V3                                          0x0A
#define SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_GET_V3                                       0x0E
#define SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_REPORT_V3                                    0x0F
#define SCHEDULE_ENTRY_LOCK_DAILY_REPEATING_SET_V3                                       0x10
/* Values used for Schedule Entry Lock Time Offset Report command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_HOUR_TZO_MASK_V3                    0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL_SIGN_TZO_BIT_MASK_V3                0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_MINUTE_OFFSET_DST_MASK_V3          0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_REPORT_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V3        0x80
/* Values used for Schedule Entry Lock Time Offset Set command */
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_HOUR_TZO_MASK_V3                       0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL_SIGN_TZO_BIT_MASK_V3                   0x80
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_MINUTE_OFFSET_DST_MASK_V3             0x7F
#define SCHEDULE_ENTRY_LOCK_TIME_OFFSET_SET_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V3           0x80

/* Screen Attributes command class commands */
#define SCREEN_ATTRIBUTES_VERSION                                                        0x01
#define SCREEN_ATTRIBUTES_GET                                                            0x01
#define SCREEN_ATTRIBUTES_REPORT                                                         0x02
/* Values used for Screen Attributes Report command */
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_NUMBER_OF_LINES_MASK                        0x1F
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_MASK                               0xE0
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_SHIFT                              0x05

/* Screen Attributes command class commands */
#define SCREEN_ATTRIBUTES_VERSION_V2                                                     0x02
#define SCREEN_ATTRIBUTES_GET_V2                                                         0x01
#define SCREEN_ATTRIBUTES_REPORT_V2                                                      0x03
/* Values used for Screen Attributes Report command */
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_NUMBER_OF_LINES_MASK_V2                     0x1F
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_ESCAPE_SEQUENCE_BIT_MASK_V2                 0x20
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_MASK_V2                            0xC0
#define SCREEN_ATTRIBUTES_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                           0x06

/* Screen Md command class commands */
#define SCREEN_MD_VERSION                                                                0x01
#define SCREEN_MD_GET                                                                    0x01
#define SCREEN_MD_REPORT                                                                 0x02
/* Values used for Screen Md Report command */
#define SCREEN_MD_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK                              0x07
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_MASK                                0x38
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_SHIFT                               0x03
#define SCREEN_MD_REPORT_PROPERTIES1_RESERVED_BIT_MASK                                   0x40
#define SCREEN_MD_REPORT_PROPERTIES1_MORE_DATA_BIT_MASK                                  0x80

/* Screen Md command class commands */
#define SCREEN_MD_VERSION_V2                                                             0x02
#define SCREEN_MD_GET_V2                                                                 0x01
#define SCREEN_MD_REPORT_V2                                                              0x03
/* Values used for Screen Md Report command */
#define SCREEN_MD_REPORT_PROPERTIES1_CHAR_PRESENTATION_MASK_V2                           0x07
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_MASK_V2                             0x38
#define SCREEN_MD_REPORT_PROPERTIES1_SCREEN_SETTINGS_SHIFT_V2                            0x03
#define SCREEN_MD_REPORT_PROPERTIES1_RESERVED_BIT_MASK_V2                                0x40
#define SCREEN_MD_REPORT_PROPERTIES1_MORE_DATA_BIT_MASK_V2                               0x80
#define SCREEN_MD_REPORT_PROPERTIES2_SCREEN_TIMEOUT_BIT_MASK_V2                          0x01
#define SCREEN_MD_REPORT_PROPERTIES2_RESERVED_MASK_V2                                    0xFE
#define SCREEN_MD_REPORT_PROPERTIES2_RESERVED_SHIFT_V2                                   0x01

/* Security Panel Mode command class commands */
#define SECURITY_PANEL_MODE_VERSION                                                      0x01
#define SECURITY_PANEL_MODE_GET                                                          0x03
#define SECURITY_PANEL_MODE_REPORT                                                       0x04
#define SECURITY_PANEL_MODE_SET                                                          0x05
#define SECURITY_PANEL_MODE_SUPPORTED_GET                                                0x01
#define SECURITY_PANEL_MODE_SUPPORTED_REPORT                                             0x02

/* Security Panel Zone Sensor command class commands */
#define SECURITY_PANEL_ZONE_SENSOR_VERSION                                               0x01
#define COMMAND_CLASS_SECURITY_PANEL_ZONE_SENSOR_INSTALLED_REPORT                        0x02
#define SECURITY_PANEL_ZONE_SENSOR_TYPE_GET                                              0x03
#define SECURITY_PANEL_ZONE_SENSOR_TYPE_REPORT                                           0x04
#define SECURITY_PANEL_ZONE_SENSOR_INSTALLED_GET                                         0x01
#define SECURITY_PANEL_ZONE_SENSOR_STATE_GET                                             0x05
#define SECURITY_PANEL_ZONE_SENSOR_STATE_REPORT                                          0x06

/* Security Panel Zone command class commands */
#define SECURITY_PANEL_ZONE_VERSION                                                      0x01
#define SECURITY_PANEL_ZONE_NUMBER_SUPPORTED_GET                                         0x01
#define SECURITY_PANEL_ZONE_STATE_GET                                                    0x05
#define SECURITY_PANEL_ZONE_STATE_REPORT                                                 0x06
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT                                             0x02
#define SECURITY_PANEL_ZONE_TYPE_GET                                                     0x03
#define SECURITY_PANEL_ZONE_TYPE_REPORT                                                  0x04
/* Values used for Security Panel Zone Supported Report command */
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT_PARAMETERS1_ZONES_SUPPORTED_MASK            0x7F
#define SECURITY_PANEL_ZONE_SUPPORTED_REPORT_PARAMETERS1_ZM_BIT_MASK                     0x80

/* Security command class commands */
#define SECURITY_VERSION                                                                 0x01
#define NETWORK_KEY_SET                                                                  0x06
#define NETWORK_KEY_VERIFY                                                               0x07
#define SECURITY_COMMANDS_SUPPORTED_GET                                                  0x02
#define SECURITY_COMMANDS_SUPPORTED_REPORT                                               0x03
#define SECURITY_MESSAGE_ENCAPSULATION                                                   0x81
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET                                         0xC1
#define SECURITY_NONCE_GET                                                               0x40
#define SECURITY_NONCE_REPORT                                                            0x80
#define SECURITY_SCHEME_GET                                                              0x04
#define SECURITY_SCHEME_INHERIT                                                          0x08
#define SECURITY_SCHEME_REPORT                                                           0x05
/* Values used for Security Commands Supported Report command */
#define SECURITY_COMMANDS_SUPPORTED_REPORT_COMMAND_CLASS_MARK                            0xEF /*The COMMAND_CLASS_MARK is used to indicate that all preceding command classes are supported and all following command classes are controlled.*/
/* Values used for Security Message Encapsulation command */
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SEQUENCE_COUNTER_MASK                 0x0F
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SEQUENCED_BIT_MASK                    0x10
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_SECOND_FRAME_BIT_MASK                 0x20
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_RESERVED_MASK                         0xC0
#define SECURITY_MESSAGE_ENCAPSULATION_PROPERTIES1_RESERVED_SHIFT                        0x06
/* Values used for Security Message Encapsulation Nonce Get command */
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SEQUENCE_COUNTER_MASK       0x0F
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SEQUENCED_BIT_MASK          0x10
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_SECOND_FRAME_BIT_MASK       0x20
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_RESERVED_MASK               0xC0
#define SECURITY_MESSAGE_ENCAPSULATION_NONCE_GET_PROPERTIES1_RESERVED_SHIFT              0x06

/* Sensor Alarm command class commands */
#define SENSOR_ALARM_VERSION                                                             0x01
#define SENSOR_ALARM_GET                                                                 0x01
#define SENSOR_ALARM_REPORT                                                              0x02
#define SENSOR_ALARM_SUPPORTED_GET                                                       0x03
#define SENSOR_ALARM_SUPPORTED_REPORT                                                    0x04

/* Sensor Binary command class commands */
#define SENSOR_BINARY_VERSION                                                            0x01
#define SENSOR_BINARY_GET                                                                0x02
#define SENSOR_BINARY_REPORT                                                             0x03
/* Values used for Sensor Binary Report command */
#define SENSOR_BINARY_REPORT_IDLE                                                        0x00
#define SENSOR_BINARY_REPORT_DETECTED_AN_EVENT                                           0xFF

/* Sensor Binary command class commands */
#define SENSOR_BINARY_VERSION_V2                                                         0x02
#define SENSOR_BINARY_GET_V2                                                             0x02
#define SENSOR_BINARY_REPORT_V2                                                          0x03
#define SENSOR_BINARY_SUPPORTED_GET_SENSOR_V2                                            0x01
#define SENSOR_BINARY_SUPPORTED_SENSOR_REPORT_V2                                         0x04
/* Values used for Sensor Binary Report command */
#define SENSOR_BINARY_REPORT_IDLE_V2                                                     0x00
#define SENSOR_BINARY_REPORT_DETECTED_AN_EVENT_V2                                        0xFF

/* Sensor Configuration command class commands */
#define SENSOR_CONFIGURATION_VERSION                                                     0x01
#define SENSOR_TRIGGER_LEVEL_GET                                                         0x02
#define SENSOR_TRIGGER_LEVEL_REPORT                                                      0x03
#define SENSOR_TRIGGER_LEVEL_SET                                                         0x01
/* Values used for Sensor Trigger Level Report command */
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SIZE_MASK                                0x07
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SCALE_MASK                               0x18
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_SCALE_SHIFT                              0x03
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_PRECISION_MASK                           0xE0
#define SENSOR_TRIGGER_LEVEL_REPORT_PROPERTIES1_PRECISION_SHIFT                          0x05
/* Values used for Sensor Trigger Level Set command */
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_RESERVED_MASK                               0x3F
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_CURRENT_BIT_MASK                            0x40
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES1_DEFAULT_BIT_MASK                            0x80
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SIZE_MASK                                   0x07
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SCALE_MASK                                  0x18
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_SCALE_SHIFT                                 0x03
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_PRECISION_MASK                              0xE0
#define SENSOR_TRIGGER_LEVEL_SET_PROPERTIES2_PRECISION_SHIFT                             0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION                                                        0x01
#define SENSOR_MULTILEVEL_GET                                                            0x04
#define SENSOR_MULTILEVEL_REPORT                                                         0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1                                   0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1                         0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1                                     0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK                                         0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK                                        0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT                                       0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK                                    0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT                                   0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V2                                                     0x02
#define SENSOR_MULTILEVEL_GET_V2                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V2                                                      0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V2                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V2                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V2                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V2                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V2                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V2                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V2                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V2                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V2                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V2                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V2                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V2                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V2                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V2                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V2                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V2                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V2                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V2                                0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V3                                                     0x03
#define SENSOR_MULTILEVEL_GET_V3                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V3                                                      0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V3                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V3                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V3                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V3                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V3                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V3                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V3                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V3                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V3                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V3                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V3                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V3                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V3                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_WEIGHT_VERSION_3_V3                                     0x0E
#define SENSOR_MULTILEVEL_REPORT_VOLTAGE_VERSION_3_V3                                    0x0F
#define SENSOR_MULTILEVEL_REPORT_CURRENT_VERSION_3_V3                                    0x10
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL_VERSION_3_V3                                  0x11
#define SENSOR_MULTILEVEL_REPORT_AIR_FLOW_VERSION_3_V3                                   0x12
#define SENSOR_MULTILEVEL_REPORT_TANK_CAPACITY_VERSION_3_V3                              0x13
#define SENSOR_MULTILEVEL_REPORT_DISTANCE_VERSION_3_V3                                   0x14
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V3                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V3                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V3                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V3                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V3                                0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V4                                                     0x04
#define SENSOR_MULTILEVEL_GET_V4                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V4                                                      0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V4                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V4                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V4                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V4                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V4                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V4                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V4                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V4                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V4                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V4                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V4                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V4                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V4                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_WEIGHT_VERSION_3_V4                                     0x0E
#define SENSOR_MULTILEVEL_REPORT_VOLTAGE_VERSION_3_V4                                    0x0F
#define SENSOR_MULTILEVEL_REPORT_CURRENT_VERSION_3_V4                                    0x10
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL_VERSION_3_V4                                  0x11
#define SENSOR_MULTILEVEL_REPORT_AIR_FLOW_VERSION_3_V4                                   0x12
#define SENSOR_MULTILEVEL_REPORT_TANK_CAPACITY_VERSION_3_V4                              0x13
#define SENSOR_MULTILEVEL_REPORT_DISTANCE_VERSION_3_V4                                   0x14
#define SENSOR_MULTILEVEL_REPORT_ANGLE_POSITION_VERSION_4_V4                             0x15
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V4                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V4                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V4                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V4                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V4                                0x05

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V5                                                     0x05
#define SENSOR_MULTILEVEL_GET_V5                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V5                                                      0x05
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V5                                        0x01
#define SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V5                                     0x02
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V5                                         0x03
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V5                                      0x06
/* Values used for Sensor Multilevel Get command */
#define SENSOR_MULTILEVEL_GET_TEMPERATURE_VERSION_1_V5                                   0x01
#define SENSOR_MULTILEVEL_GET_GENERAL_PURPOSE_VALUE_VERSION_1_V5                         0x02
#define SENSOR_MULTILEVEL_GET_LUMINANCE_VERSION_1_V5                                     0x03
#define SENSOR_MULTILEVEL_GET_POWER_VERSION_2_V5                                         0x04
#define SENSOR_MULTILEVEL_GET_RELATIVE_HUMIDITY_VERSION_2_V5                             0x05
#define SENSOR_MULTILEVEL_GET_VELOCITY_VERSION_2_V5                                      0x06
#define SENSOR_MULTILEVEL_GET_DIRECTION_VERSION_2_V5                                     0x07
#define SENSOR_MULTILEVEL_GET_ATMOSPHERIC_PRESSURE_VERSION_2_V5                          0x08
#define SENSOR_MULTILEVEL_GET_BAROMETRIC_PRESSURE_VERSION_2_V5                           0x09
#define SENSOR_MULTILEVEL_GET_SOLAR_RADIATION_VERSION_2_V5                               0x0A
#define SENSOR_MULTILEVEL_GET_DEW_POINT_VERSION_2_V5                                     0x0B
#define SENSOR_MULTILEVEL_GET_RAIN_RATE_VERSION_2_V5                                     0x0C
#define SENSOR_MULTILEVEL_GET_TIDE_LEVEL_VERSION_2_V5                                    0x0D
#define SENSOR_MULTILEVEL_GET_WEIGHT_VERSION_3_V5                                        0x0E
#define SENSOR_MULTILEVEL_GET_VOLTAGE_VERSION_3_V5                                       0x0F
#define SENSOR_MULTILEVEL_GET_CURRENT_VERSION_3_V5                                       0x10
#define SENSOR_MULTILEVEL_GET_CO2_LEVEL_VERSION_3_V5                                     0x11
#define SENSOR_MULTILEVEL_GET_AIR_FLOW_VERSION_3_V5                                      0x12
#define SENSOR_MULTILEVEL_GET_TANK_CAPACITY_VERSION_3_V5                                 0x13
#define SENSOR_MULTILEVEL_GET_DISTANCE_VERSION_3_V5                                      0x14
#define SENSOR_MULTILEVEL_GET_ANGLE_POSITION_VERSION_4_V5                                0x15
#define SENSOR_MULTILEVEL_GET_ROTATION_V5_V5                                             0x16
#define SENSOR_MULTILEVEL_GET_WATER_TEMPERATURE_V5_V5                                    0x17
#define SENSOR_MULTILEVEL_GET_SOIL_TEMPERATURE_V5_V5                                     0x18
#define SENSOR_MULTILEVEL_GET_SEISMIC_INTENSITY_V5_V5                                    0x19
#define SENSOR_MULTILEVEL_GET_SEISMIC_MAGNITUDE_V5_V5                                    0x1A
#define SENSOR_MULTILEVEL_GET_ULTRAVIOLET_V5_V5                                          0x1B
#define SENSOR_MULTILEVEL_GET_ELECTRICAL_RESISTIVITY_V5_V5                               0x1C
#define SENSOR_MULTILEVEL_GET_ELECTRICAL_CONDUCTIVITY_V5_V5                              0x1D
#define SENSOR_MULTILEVEL_GET_LOUDNESS_V5_V5                                             0x1E
#define SENSOR_MULTILEVEL_GET_MOISTURE_V5_V5                                             0x1F
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED1_MASK_V5                              0x07
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_SCALE_MASK_V5                                  0x18
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_SCALE_SHIFT_V5                                 0x03
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED2_MASK_V5                              0xE0
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED2_SHIFT_V5                             0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V5                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V5                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V5                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V5                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V5                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V5                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V5                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V5                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V5                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V5                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V5                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V5                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V5                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_WEIGHT_VERSION_3_V5                                     0x0E
#define SENSOR_MULTILEVEL_REPORT_VOLTAGE_VERSION_3_V5                                    0x0F
#define SENSOR_MULTILEVEL_REPORT_CURRENT_VERSION_3_V5                                    0x10
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL_VERSION_3_V5                                  0x11
#define SENSOR_MULTILEVEL_REPORT_AIR_FLOW_VERSION_3_V5                                   0x12
#define SENSOR_MULTILEVEL_REPORT_TANK_CAPACITY_VERSION_3_V5                              0x13
#define SENSOR_MULTILEVEL_REPORT_DISTANCE_VERSION_3_V5                                   0x14
#define SENSOR_MULTILEVEL_REPORT_ANGLE_POSITION_VERSION_4_V5                             0x15
#define SENSOR_MULTILEVEL_REPORT_ROTATION_V5_V5                                          0x16
#define SENSOR_MULTILEVEL_REPORT_WATER_TEMPERATURE_V5_V5                                 0x17
#define SENSOR_MULTILEVEL_REPORT_SOIL_TEMPERATURE_V5_V5                                  0x18
#define SENSOR_MULTILEVEL_REPORT_SEISMIC_INTENSITY_V5_V5                                 0x19
#define SENSOR_MULTILEVEL_REPORT_SEISMIC_MAGNITUDE_V5_V5                                 0x1A
#define SENSOR_MULTILEVEL_REPORT_ULTRAVIOLET_V5_V5                                       0x1B
#define SENSOR_MULTILEVEL_REPORT_ELECTRICAL_RESISTIVITY_V5_V5                            0x1C
#define SENSOR_MULTILEVEL_REPORT_ELECTRICAL_CONDUCTIVITY_V5_V5                           0x1D
#define SENSOR_MULTILEVEL_REPORT_LOUDNESS_V5_V5                                          0x1E
#define SENSOR_MULTILEVEL_REPORT_MOISTURE_V5_V5                                          0x1F
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V5                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V5                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V5                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V5                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V5                                0x05
/* Values used for Sensor Multilevel Supported Get Scale command */
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TEMPERATURE_VERSION_1_V5                   0x01
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_GENERAL_PURPOSE_VALUE_VERSION_1_V5         0x02
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_LUMINANCE_VERSION_1_V5                     0x03
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_POWER_VERSION_2_V5                         0x04
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_RELATIVE_HUMIDITY_VERSION_2_V5             0x05
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_VELOCITY_VERSION_2_V5                      0x06
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DIRECTION_VERSION_2_V5                     0x07
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ATMOSPHERIC_PRESSURE_VERSION_2_V5          0x08
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_BAROMETRIC_PRESSURE_VERSION_2_V5           0x09
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SOLAR_RADIATION_VERSION_2_V5               0x0A
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DEW_POINT_VERSION_2_V5                     0x0B
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_RAIN_RATE_VERSION_2_V5                     0x0C
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TIDE_LEVEL_VERSION_2_V5                    0x0D
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_WEIGHT_VERSION_3_V5                        0x0E
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_VOLTAGE_VERSION_3_V5                       0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_CURRENT_VERSION_3_V5                       0x10
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_CO2_LEVEL_VERSION_3_V5                     0x11
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_AIR_FLOW_VERSION_3_V5                      0x12
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TANK_CAPACITY_VERSION_3_V5                 0x13
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DISTANCE_VERSION_3_V5                      0x14
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ANGLE_POSITION_VERSION_4_V5                0x15
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ROTATION_V5_V5                             0x16
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_WATER_TEMPERATURE_V5_V5                    0x17
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SOIL_TEMPERATURE_V5_V5                     0x18
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SEISMIC_INTENSITY_V5_V5                    0x19
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SEISMIC_MAGNITUDE_V5_V5                    0x1A
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ULTRAVIOLET_V5_V5                          0x1B
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ELECTRICAL_RESISTIVITY_V5_V5               0x1C
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ELECTRICAL_CONDUCTIVITY_V5_V5              0x1D
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_LOUDNESS_V5_V5                             0x1E
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_MOISTURE_V5_V5                             0x1F
/* Values used for Sensor Multilevel Supported Scale Report command */
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TEMPERATURE_VERSION_1_V5                0x01
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V5      0x02
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_LUMINANCE_VERSION_1_V5                  0x03
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_POWER_VERSION_2_V5                      0x04
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_RELATIVE_HUMIDITY_VERSION_2_V5          0x05
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_VELOCITY_VERSION_2_V5                   0x06
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DIRECTION_VERSION_2_V5                  0x07
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V5       0x08
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V5        0x09
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SOLAR_RADIATION_VERSION_2_V5            0x0A
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DEW_POINT_VERSION_2_V5                  0x0B
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_RAIN_RATE_VERSION_2_V5                  0x0C
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TIDE_LEVEL_VERSION_2_V5                 0x0D
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_WEIGHT_VERSION_3_V5                     0x0E
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_VOLTAGE_VERSION_3_V5                    0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_CURRENT_VERSION_3_V5                    0x10
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_CO2_LEVEL_VERSION_3_V5                  0x11
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_AIR_FLOW_VERSION_3_V5                   0x12
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TANK_CAPACITY_VERSION_3_V5              0x13
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DISTANCE_VERSION_3_V5                   0x14
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ANGLE_POSITION_VERSION_4_V5             0x15
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ROTATION_V5_V5                          0x16
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_WATER_TEMPERATURE_V5_V5                 0x17
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SOIL_TEMPERATURE_V5_V5                  0x18
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SEISMIC_INTENSITY_V5_V5                 0x19
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SEISMIC_MAGNITUDE_V5_V5                 0x1A
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ULTRAVIOLET_V5_V5                       0x1B
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ELECTRICAL_RESISTIVITY_V5_V5            0x1C
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ELECTRICAL_CONDUCTIVITY_V5_V5           0x1D
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_LOUDNESS_V5_V5                          0x1E
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_MOISTURE_V5_V5                          0x1F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_SCALE_BIT_MASK_MASK_V5      0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_RESERVED_MASK_V5            0xF0
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_RESERVED_SHIFT_V5           0x04

/* Sensor Multilevel command class commands */
#define SENSOR_MULTILEVEL_VERSION_V6                                                     0x06
#define SENSOR_MULTILEVEL_GET_V6                                                         0x04
#define SENSOR_MULTILEVEL_REPORT_V6                                                      0x05
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SENSOR_V6                                        0x01
#define SENSOR_MULTILEVEL_SUPPORTED_SENSOR_REPORT_V6                                     0x02
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_V6                                         0x03
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_V6                                      0x06
/* Values used for Sensor Multilevel Get command */
#define SENSOR_MULTILEVEL_GET_TEMPERATURE_VERSION_1_V6                                   0x01
#define SENSOR_MULTILEVEL_GET_GENERAL_PURPOSE_VALUE_VERSION_1_V6                         0x02
#define SENSOR_MULTILEVEL_GET_LUMINANCE_VERSION_1_V6                                     0x03
#define SENSOR_MULTILEVEL_GET_POWER_VERSION_2_V6                                         0x04
#define SENSOR_MULTILEVEL_GET_RELATIVE_HUMIDITY_VERSION_2_V6                             0x05
#define SENSOR_MULTILEVEL_GET_VELOCITY_VERSION_2_V6                                      0x06
#define SENSOR_MULTILEVEL_GET_DIRECTION_VERSION_2_V6                                     0x07
#define SENSOR_MULTILEVEL_GET_ATMOSPHERIC_PRESSURE_VERSION_2_V6                          0x08
#define SENSOR_MULTILEVEL_GET_BAROMETRIC_PRESSURE_VERSION_2_V6                           0x09
#define SENSOR_MULTILEVEL_GET_SOLAR_RADIATION_VERSION_2_V6                               0x0A
#define SENSOR_MULTILEVEL_GET_DEW_POINT_VERSION_2_V6                                     0x0B
#define SENSOR_MULTILEVEL_GET_RAIN_RATE_VERSION_2_V6                                     0x0C
#define SENSOR_MULTILEVEL_GET_TIDE_LEVEL_VERSION_2_V6                                    0x0D
#define SENSOR_MULTILEVEL_GET_WEIGHT_VERSION_3_V6                                        0x0E
#define SENSOR_MULTILEVEL_GET_VOLTAGE_VERSION_3_V6                                       0x0F
#define SENSOR_MULTILEVEL_GET_CURRENT_VERSION_3_V6                                       0x10
#define SENSOR_MULTILEVEL_GET_CO2_LEVEL_VERSION_3_V6                                     0x11
#define SENSOR_MULTILEVEL_GET_AIR_FLOW_VERSION_3_V6                                      0x12
#define SENSOR_MULTILEVEL_GET_TANK_CAPACITY_VERSION_3_V6                                 0x13
#define SENSOR_MULTILEVEL_GET_DISTANCE_VERSION_3_V6                                      0x14
#define SENSOR_MULTILEVEL_GET_ANGLE_POSITION_VERSION_4_V6                                0x15
#define SENSOR_MULTILEVEL_GET_ROTATION_V5_V6                                             0x16
#define SENSOR_MULTILEVEL_GET_WATER_TEMPERATURE_V5_V6                                    0x17
#define SENSOR_MULTILEVEL_GET_SOIL_TEMPERATURE_V5_V6                                     0x18
#define SENSOR_MULTILEVEL_GET_SEISMIC_INTENSITY_V5_V6                                    0x19
#define SENSOR_MULTILEVEL_GET_SEISMIC_MAGNITUDE_V5_V6                                    0x1A
#define SENSOR_MULTILEVEL_GET_ULTRAVIOLET_V5_V6                                          0x1B
#define SENSOR_MULTILEVEL_GET_ELECTRICAL_RESISTIVITY_V5_V6                               0x1C
#define SENSOR_MULTILEVEL_GET_ELECTRICAL_CONDUCTIVITY_V5_V6                              0x1D
#define SENSOR_MULTILEVEL_GET_LOUDNESS_V5_V6                                             0x1E
#define SENSOR_MULTILEVEL_GET_MOISTURE_V5_V6                                             0x1F
#define SENSOR_MULTILEVEL_GET_FREQUENCY_V6_V6                                            0x20
#define SENSOR_MULTILEVEL_GET_TIME_V6_V6                                                 0x21
#define SENSOR_MULTILEVEL_GET_TARGET_TEMPERATURE_V6_V6                                   0x22
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED1_MASK_V6                              0x07
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_SCALE_MASK_V6                                  0x18
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_SCALE_SHIFT_V6                                 0x03
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED2_MASK_V6                              0xE0
#define SENSOR_MULTILEVEL_GET_PROPERTIES1_RESERVED2_SHIFT_V6                             0x05
/* Values used for Sensor Multilevel Report command */
#define SENSOR_MULTILEVEL_REPORT_TEMPERATURE_VERSION_1_V6                                0x01
#define SENSOR_MULTILEVEL_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V6                      0x02
#define SENSOR_MULTILEVEL_REPORT_LUMINANCE_VERSION_1_V6                                  0x03
#define SENSOR_MULTILEVEL_REPORT_POWER_VERSION_2_V6                                      0x04
#define SENSOR_MULTILEVEL_REPORT_RELATIVE_HUMIDITY_VERSION_2_V6                          0x05
#define SENSOR_MULTILEVEL_REPORT_VELOCITY_VERSION_2_V6                                   0x06
#define SENSOR_MULTILEVEL_REPORT_DIRECTION_VERSION_2_V6                                  0x07
#define SENSOR_MULTILEVEL_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V6                       0x08
#define SENSOR_MULTILEVEL_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V6                        0x09
#define SENSOR_MULTILEVEL_REPORT_SOLAR_RADIATION_VERSION_2_V6                            0x0A
#define SENSOR_MULTILEVEL_REPORT_DEW_POINT_VERSION_2_V6                                  0x0B
#define SENSOR_MULTILEVEL_REPORT_RAIN_RATE_VERSION_2_V6                                  0x0C
#define SENSOR_MULTILEVEL_REPORT_TIDE_LEVEL_VERSION_2_V6                                 0x0D
#define SENSOR_MULTILEVEL_REPORT_WEIGHT_VERSION_3_V6                                     0x0E
#define SENSOR_MULTILEVEL_REPORT_VOLTAGE_VERSION_3_V6                                    0x0F
#define SENSOR_MULTILEVEL_REPORT_CURRENT_VERSION_3_V6                                    0x10
#define SENSOR_MULTILEVEL_REPORT_CO2_LEVEL_VERSION_3_V6                                  0x11
#define SENSOR_MULTILEVEL_REPORT_AIR_FLOW_VERSION_3_V6                                   0x12
#define SENSOR_MULTILEVEL_REPORT_TANK_CAPACITY_VERSION_3_V6                              0x13
#define SENSOR_MULTILEVEL_REPORT_DISTANCE_VERSION_3_V6                                   0x14
#define SENSOR_MULTILEVEL_REPORT_ANGLE_POSITION_VERSION_4_V6                             0x15
#define SENSOR_MULTILEVEL_REPORT_ROTATION_V5_V6                                          0x16
#define SENSOR_MULTILEVEL_REPORT_WATER_TEMPERATURE_V5_V6                                 0x17
#define SENSOR_MULTILEVEL_REPORT_SOIL_TEMPERATURE_V5_V6                                  0x18
#define SENSOR_MULTILEVEL_REPORT_SEISMIC_INTENSITY_V5_V6                                 0x19
#define SENSOR_MULTILEVEL_REPORT_SEISMIC_MAGNITUDE_V5_V6                                 0x1A
#define SENSOR_MULTILEVEL_REPORT_ULTRAVIOLET_V5_V6                                       0x1B
#define SENSOR_MULTILEVEL_REPORT_ELECTRICAL_RESISTIVITY_V5_V6                            0x1C
#define SENSOR_MULTILEVEL_REPORT_ELECTRICAL_CONDUCTIVITY_V5_V6                           0x1D
#define SENSOR_MULTILEVEL_REPORT_LOUDNESS_V5_V6                                          0x1E
#define SENSOR_MULTILEVEL_REPORT_MOISTURE_V5_V6                                          0x1F
#define SENSOR_MULTILEVEL_REPORT_FREQUENCY_V6_V6                                         0x20
#define SENSOR_MULTILEVEL_REPORT_TIME_V6_V6                                              0x21
#define SENSOR_MULTILEVEL_REPORT_TARGET_TEMPERATURE_V6_V6                                0x22
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SIZE_MASK_V6                                      0x07
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_MASK_V6                                     0x18
#define SENSOR_MULTILEVEL_REPORT_LEVEL_SCALE_SHIFT_V6                                    0x03
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_MASK_V6                                 0xE0
#define SENSOR_MULTILEVEL_REPORT_LEVEL_PRECISION_SHIFT_V6                                0x05
/* Values used for Sensor Multilevel Supported Get Scale command */
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TEMPERATURE_VERSION_1_V6                   0x01
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_GENERAL_PURPOSE_VALUE_VERSION_1_V6         0x02
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_LUMINANCE_VERSION_1_V6                     0x03
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_POWER_VERSION_2_V6                         0x04
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_RELATIVE_HUMIDITY_VERSION_2_V6             0x05
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_VELOCITY_VERSION_2_V6                      0x06
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DIRECTION_VERSION_2_V6                     0x07
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ATMOSPHERIC_PRESSURE_VERSION_2_V6          0x08
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_BAROMETRIC_PRESSURE_VERSION_2_V6           0x09
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SOLAR_RADIATION_VERSION_2_V6               0x0A
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DEW_POINT_VERSION_2_V6                     0x0B
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_RAIN_RATE_VERSION_2_V6                     0x0C
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TIDE_LEVEL_VERSION_2_V6                    0x0D
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_WEIGHT_VERSION_3_V6                        0x0E
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_VOLTAGE_VERSION_3_V6                       0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_CURRENT_VERSION_3_V6                       0x10
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_CO2_LEVEL_VERSION_3_V6                     0x11
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_AIR_FLOW_VERSION_3_V6                      0x12
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TANK_CAPACITY_VERSION_3_V6                 0x13
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_DISTANCE_VERSION_3_V6                      0x14
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ANGLE_POSITION_VERSION_4_V6                0x15
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ROTATION_V5_V6                             0x16
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_WATER_TEMPERATURE_V5_V6                    0x17
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SOIL_TEMPERATURE_V5_V6                     0x18
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SEISMIC_INTENSITY_V5_V6                    0x19
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_SEISMIC_MAGNITUDE_V5_V6                    0x1A
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ULTRAVIOLET_V5_V6                          0x1B
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ELECTRICAL_RESISTIVITY_V5_V6               0x1C
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_ELECTRICAL_CONDUCTIVITY_V5_V6              0x1D
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_LOUDNESS_V5_V6                             0x1E
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_MOISTURE_V5_V6                             0x1F
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_FREQUENCY_V6_V6                            0x20
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TIME_V6_V6                                 0x21
#define SENSOR_MULTILEVEL_SUPPORTED_GET_SCALE_TARGET_TEMPERATURE_V6_V6                   0x22
/* Values used for Sensor Multilevel Supported Scale Report command */
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TEMPERATURE_VERSION_1_V6                0x01
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_GENERAL_PURPOSE_VALUE_VERSION_1_V6      0x02
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_LUMINANCE_VERSION_1_V6                  0x03
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_POWER_VERSION_2_V6                      0x04
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_RELATIVE_HUMIDITY_VERSION_2_V6          0x05
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_VELOCITY_VERSION_2_V6                   0x06
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DIRECTION_VERSION_2_V6                  0x07
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ATMOSPHERIC_PRESSURE_VERSION_2_V6       0x08
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_BAROMETRIC_PRESSURE_VERSION_2_V6        0x09
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SOLAR_RADIATION_VERSION_2_V6            0x0A
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DEW_POINT_VERSION_2_V6                  0x0B
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_RAIN_RATE_VERSION_2_V6                  0x0C
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TIDE_LEVEL_VERSION_2_V6                 0x0D
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_WEIGHT_VERSION_3_V6                     0x0E
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_VOLTAGE_VERSION_3_V6                    0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_CURRENT_VERSION_3_V6                    0x10
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_CO2_LEVEL_VERSION_3_V6                  0x11
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_AIR_FLOW_VERSION_3_V6                   0x12
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TANK_CAPACITY_VERSION_3_V6              0x13
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_DISTANCE_VERSION_3_V6                   0x14
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ANGLE_POSITION_VERSION_4_V6             0x15
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ROTATION_V5_V6                          0x16
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_WATER_TEMPERATURE_V5_V6                 0x17
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SOIL_TEMPERATURE_V5_V6                  0x18
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SEISMIC_INTENSITY_V5_V6                 0x19
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_SEISMIC_MAGNITUDE_V5_V6                 0x1A
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ULTRAVIOLET_V5_V6                       0x1B
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ELECTRICAL_RESISTIVITY_V5_V6            0x1C
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_ELECTRICAL_CONDUCTIVITY_V5_V6           0x1D
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_LOUDNESS_V5_V6                          0x1E
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_MOISTURE_V5_V6                          0x1F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_FREQUENCY_V6_V6                         0x20
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TIME_V6_V6                              0x21
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_TARGET_TEMPERATURE_V6_V6                0x22
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_SCALE_BIT_MASK_MASK_V6      0x0F
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_RESERVED_MASK_V6            0xF0
#define SENSOR_MULTILEVEL_SUPPORTED_SCALE_REPORT_PROPERTIES1_RESERVED_SHIFT_V6           0x04

/* Silence Alarm command class commands */
#define SILENCE_ALARM_VERSION                                                            0x01
#define SENSOR_ALARM_SET                                                                 0x01

/* Simple Av Control command class commands */
#define SIMPLE_AV_CONTROL_VERSION                                                        0x01
#define SIMPLE_AV_CONTROL_GET                                                            0x02
#define SIMPLE_AV_CONTROL_REPORT                                                         0x03
#define SIMPLE_AV_CONTROL_SET                                                            0x01
#define SIMPLE_AV_CONTROL_SUPPORTED_GET                                                  0x04
#define SIMPLE_AV_CONTROL_SUPPORTED_REPORT                                               0x05
/* Values used for Simple Av Control Set command */
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_KEY_ATTRIBUTES_MASK                            0x07
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_MASK                                  0xF8
#define SIMPLE_AV_CONTROL_SET_PROPERTIES1_RESERVED_SHIFT                                 0x03

/* Switch All command class commands */
#define SWITCH_ALL_VERSION                                                               0x01
#define SWITCH_ALL_GET                                                                   0x02
#define SWITCH_ALL_OFF                                                                   0x05
#define SWITCH_ALL_ON                                                                    0x04
#define SWITCH_ALL_REPORT                                                                0x03
#define SWITCH_ALL_SET                                                                   0x01
/* Values used for Switch All Report command */
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                 0x00
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_ON_FUNCTIONALITY_BUT_NOT_ALL_OFF         0x01
#define SWITCH_ALL_REPORT_EXCLUDED_FROM_THE_ALL_OFF_FUNCTIONALITY_BUT_NOT_ALL_ON         0x02
#define SWITCH_ALL_REPORT_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                   0xFF
/* Values used for Switch All Set command */
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                    0x00
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_ON_FUNCTIONALITY_BUT_NOT_ALL_OFF            0x01
#define SWITCH_ALL_SET_EXCLUDED_FROM_THE_ALL_OFF_FUNCTIONALITY_BUT_NOT_ALL_ON            0x02
#define SWITCH_ALL_SET_INCLUDED_IN_THE_ALL_ON_ALL_OFF_FUNCTIONALITY                      0xFF

/* Switch Binary command class commands */
#define SWITCH_BINARY_VERSION                                                            0x01
#define SWITCH_BINARY_GET                                                                0x02
#define SWITCH_BINARY_REPORT                                                             0x03
#define SWITCH_BINARY_SET                                                                0x01

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION                                                        0x01
#define SWITCH_MULTILEVEL_GET                                                            0x02
#define SWITCH_MULTILEVEL_REPORT                                                         0x03
#define SWITCH_MULTILEVEL_SET                                                            0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE                                             0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE                                              0x05
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED1_MASK                        0x1F
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_IGNORE_START_LEVEL_BIT_MASK           0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_UP_DOWN_BIT_MASK                      0x40
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED2_BIT_MASK                    0x80

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION_V2                                                     0x02
#define SWITCH_MULTILEVEL_GET_V2                                                         0x02
#define SWITCH_MULTILEVEL_REPORT_V2                                                      0x03
#define SWITCH_MULTILEVEL_SET_V2                                                         0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V2                                          0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V2                                           0x05
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED1_MASK_V2               0x1F
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_IGNORE_START_LEVEL_BIT_MASK_V2  0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_BIT_MASK_V2             0x40
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED2_BIT_MASK_V2           0x80

/* Switch Multilevel command class commands */
#define SWITCH_MULTILEVEL_VERSION_V3                                                     0x03
#define SWITCH_MULTILEVEL_GET_V3                                                         0x02
#define SWITCH_MULTILEVEL_REPORT_V3                                                      0x03
#define SWITCH_MULTILEVEL_SET_V3                                                         0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V3                                          0x04
#define SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V3                                           0x05
#define SWITCH_MULTILEVEL_SUPPORTED_GET_V3                                               0x06
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_V3                                            0x07
/* Values used for Switch Multilevel Start Level Change command */
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_RESERVED_MASK_V3                0x07
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_INC_DEC_MASK_V3                 0x18
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_INC_DEC_SHIFT_V3                0x03
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_INCREMENT_V3                        0x00
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_DECREMENT_V3                        0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_RESERVED_V3                         0x02
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_INC_DEC_NONE_V3                             0x03
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_IGNORE_START_LEVEL_BIT_MASK_V3  0x20
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_MASK_V3                 0xC0
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_SHIFT_V3                0x06
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_UP_V3                               0x00
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_DOWN_V3                             0x01
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_RESERVED_V3                         0x02
#define SWITCH_MULTILEVEL_START_LEVEL_CHANGE_UP_DOWN_NONE_V3                             0x03
/* Values used for Switch Multilevel Supported Report command */
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_PRIMARY_SWITCH_TYPE_MASK_V3       0x1F
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_RESERVED_MASK_V3                  0xE0
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                 0x05
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_SECONDARY_SWITCH_TYPE_MASK_V3     0x1F
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_RESERVED_MASK_V3                  0xE0
#define SWITCH_MULTILEVEL_SUPPORTED_REPORT_PROPERTIES2_RESERVED_SHIFT_V3                 0x05

/* Switch Toggle Binary command class commands */
#define SWITCH_TOGGLE_BINARY_VERSION                                                     0x01
#define SWITCH_TOGGLE_BINARY_SET                                                         0x01 /*SWITCH_TOGGLE_BINARY_SET*/
#define SWITCH_TOGGLE_BINARY_GET                                                         0x02
#define SWITCH_TOGGLE_BINARY_REPORT                                                      0x03

/* Switch Toggle Multilevel command class commands */
#define SWITCH_TOGGLE_MULTILEVEL_VERSION                                                 0x01
#define SWITCH_TOGGLE_MULTILEVEL_SET                                                     0x01 /*SWITCH_TOGGLE_MULTILEVEL_SET*/
#define SWITCH_TOGGLE_MULTILEVEL_GET                                                     0x02
#define SWITCH_TOGGLE_MULTILEVEL_REPORT                                                  0x03
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE                                      0x04
#define SWITCH_TOGGLE_MULTILEVEL_STOP_LEVEL_CHANGE                                       0x05
/* Values used for Switch Toggle Multilevel Start Level Change command */
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED1_MASK                 0x1F
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_IGNORE_START_LEVEL_BIT_MASK    0x20
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_RESERVED2_BIT_MASK             0x40
#define SWITCH_TOGGLE_MULTILEVEL_START_LEVEL_CHANGE_LEVEL_ROLL_OVER_BIT_MASK             0x80

/* Tariff Config command class commands */
#define TARIFF_CONFIG_VERSION                                                            0x01
#define TARIFF_TBL_REMOVE                                                                0x03
#define TARIFF_TBL_SET                                                                   0x02
#define TARIFF_TBL_SUPPLIER_SET                                                          0x01
/* Values used for Tariff Tbl Remove command */
#define TARIFF_TBL_REMOVE_PROPERTIES1_RATE_PARAMETER_SET_IDS_MASK                        0x3F
#define TARIFF_TBL_REMOVE_PROPERTIES1_RESERVED_MASK                                      0xC0
#define TARIFF_TBL_REMOVE_PROPERTIES1_RESERVED_SHIFT                                     0x06
/* Values used for Tariff Tbl Set command */
#define TARIFF_TBL_SET_PROPERTIES1_RESERVED_MASK                                         0x1F
#define TARIFF_TBL_SET_PROPERTIES1_TARIFF_PRECISION_MASK                                 0xE0
#define TARIFF_TBL_SET_PROPERTIES1_TARIFF_PRECISION_SHIFT                                0x05
/* Values used for Tariff Tbl Supplier Set command */
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PERIOD_MASK                  0x1F
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PRECISION_MASK               0xE0
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES1_STANDING_CHARGE_PRECISION_SHIFT              0x05
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_NUMBER_OF_SUPPLIER_CHARACTERS_MASK           0x1F
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_RESERVED_MASK                                0xE0
#define TARIFF_TBL_SUPPLIER_SET_PROPERTIES2_RESERVED_SHIFT                               0x05

/* Tariff Tbl Monitor command class commands */
#define TARIFF_TBL_MONITOR_VERSION                                                       0x01
#define TARIFF_TBL_COST_GET                                                              0x05
#define TARIFF_TBL_COST_REPORT                                                           0x06
#define TARIFF_TBL_GET                                                                   0x03
#define TARIFF_TBL_REPORT                                                                0x04
#define TARIFF_TBL_SUPPLIER_GET                                                          0x01
#define TARIFF_TBL_SUPPLIER_REPORT                                                       0x02
/* Values used for Tariff Tbl Cost Report command */
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RATE_TYPE_MASK                                0x03
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RESERVED_MASK                                 0xFC
#define TARIFF_TBL_COST_REPORT_PROPERTIES1_RESERVED_SHIFT                                0x02
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_RESERVED_MASK                                 0x1F
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_COST_PRECISION_MASK                           0xE0
#define TARIFF_TBL_COST_REPORT_PROPERTIES2_COST_PRECISION_SHIFT                          0x05
/* Values used for Tariff Tbl Report command */
#define TARIFF_TBL_REPORT_PROPERTIES1_RESERVED_MASK                                      0x1F
#define TARIFF_TBL_REPORT_PROPERTIES1_TARIFF_PRECISION_MASK                              0xE0
#define TARIFF_TBL_REPORT_PROPERTIES1_TARIFF_PRECISION_SHIFT                             0x05
/* Values used for Tariff Tbl Supplier Report command */
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PERIOD_MASK               0x1F
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PRECISION_MASK            0xE0
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES1_STANDING_CHARGE_PRECISION_SHIFT           0x05
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_NUMBER_OF_SUPPLIER_CHARACTERS_MASK        0x1F
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_RESERVED_MASK                             0xE0
#define TARIFF_TBL_SUPPLIER_REPORT_PROPERTIES2_RESERVED_SHIFT                            0x05

/* Thermostat Fan Mode command class commands */
#define THERMOSTAT_FAN_MODE_VERSION                                                      0x01
#define THERMOSTAT_FAN_MODE_GET                                                          0x02
#define THERMOSTAT_FAN_MODE_REPORT                                                       0x03
#define THERMOSTAT_FAN_MODE_SET                                                          0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET                                                0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT                                             0x05
/* Values used for Thermostat Fan Mode Report command */
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_FAN_MODE_MASK                                   0x0F
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_LOW                                     0x00
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_LOW                                          0x01
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_HIGH                                    0x02
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HIGH                                         0x03
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_MASK                                   0xF0
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_SHIFT                                  0x04
/* Values used for Thermostat Fan Mode Set command */
#define THERMOSTAT_FAN_MODE_SET_LEVEL_FAN_MODE_MASK                                      0x0F
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_LOW                                        0x00
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_LOW                                             0x01
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_HIGH                                       0x02
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HIGH                                            0x03
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_SHIFT                                     0x04

/* Thermostat Fan Mode command class commands */
#define THERMOSTAT_FAN_MODE_VERSION_V2                                                   0x02
#define THERMOSTAT_FAN_MODE_GET_V2                                                       0x02
#define THERMOSTAT_FAN_MODE_REPORT_V2                                                    0x03
#define THERMOSTAT_FAN_MODE_SET_V2                                                       0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET_V2                                             0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_V2                                          0x05
/* Values used for Thermostat Fan Mode Report command */
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_FAN_MODE_MASK_V2                                0x0F
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_LOW_V2                                  0x00
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_LOW_V2                                       0x01
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_HIGH_V2                                 0x02
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HIGH_V2                                      0x03
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_MEDIUM_V2                               0x04
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_MEDIUM_V2                                    0x05
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_MASK_V2                                0xF0
#define THERMOSTAT_FAN_MODE_REPORT_LEVEL_RESERVED_SHIFT_V2                               0x04
/* Values used for Thermostat Fan Mode Set command */
#define THERMOSTAT_FAN_MODE_SET_LEVEL_FAN_MODE_MASK_V2                                   0x0F
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_LOW_V2                                     0x00
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_LOW_V2                                          0x01
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_HIGH_V2                                    0x02
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HIGH_V2                                         0x03
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_MEDIUM_V2                                  0x04
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_MEDIUM_V2                                       0x05
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_MASK_V2                                   0x70
#define THERMOSTAT_FAN_MODE_SET_LEVEL_RESERVED_SHIFT_V2                                  0x04
#define THERMOSTAT_FAN_MODE_SET_LEVEL_OFF_BIT_MASK_V2                                    0x80

/* Thermostat Fan Mode command class commands */
#define THERMOSTAT_FAN_MODE_VERSION_V3                                                   0x03
#define THERMOSTAT_FAN_MODE_GET_V3                                                       0x02
#define THERMOSTAT_FAN_MODE_REPORT_V3                                                    0x03
#define THERMOSTAT_FAN_MODE_SET_V3                                                       0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET_V3                                             0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_V3                                          0x05
/* Values used for Thermostat Fan Mode Report command */
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_FAN_MODE_MASK_V3                          0x0F
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_LOW_V3                                  0x00
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_LOW_V3                                       0x01
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_HIGH_V3                                 0x02
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HIGH_V3                                      0x03
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_MEDIUM_V3                               0x04
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_MEDIUM_V3                                    0x05
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_CIRCULATION_V3                               0x06
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HUMIDITY_V3                                  0x07
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_RESERVED_MASK_V3                          0x70
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_RESERVED_SHIFT_V3                         0x04
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_OFF_BIT_MASK_V3                           0x80
/* Values used for Thermostat Fan Mode Set command */
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_FAN_MODE_MASK_V3                             0x0F
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_LOW_V3                                     0x00
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_LOW_V3                                          0x01
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_HIGH_V3                                    0x02
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HIGH_V3                                         0x03
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_MEDIUM_V3                                  0x04
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_MEDIUM_V3                                       0x05
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_CIRCULATION_V3                                  0x06
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HUMIDITY_V3                                     0x07
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_RESERVED_MASK_V3                             0x70
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_RESERVED_SHIFT_V3                            0x04
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_OFF_BIT_MASK_V3                              0x80

/* Thermostat Fan Mode command class commands */
#define THERMOSTAT_FAN_MODE_VERSION_V4                                                   0x04
#define THERMOSTAT_FAN_MODE_GET_V4                                                       0x02
#define THERMOSTAT_FAN_MODE_REPORT_V4                                                    0x03
#define THERMOSTAT_FAN_MODE_SET_V4                                                       0x01
#define THERMOSTAT_FAN_MODE_SUPPORTED_GET_V4                                             0x04
#define THERMOSTAT_FAN_MODE_SUPPORTED_REPORT_V4                                          0x05
/* Values used for Thermostat Fan Mode Report command */
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_FAN_MODE_MASK_V4                          0x0F
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_LOW_V4                                  0x00
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_LOW_V4                                       0x01
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_HIGH_V4                                 0x02
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HIGH_V4                                      0x03
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_AUTO_MEDIUM_V4                               0x04
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_MEDIUM_V4                                    0x05
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_CIRCULATION_V4                               0x06
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_HUMIDITY_V4                                  0x07
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_LEFT_RIGHT_V4                                0x08
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_UP_DOWN_V4                                   0x09
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_QUIET_V4                                     0x0A
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_RESERVEDB_V4                                 0x0B
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_RESERVEDC_V4                                 0x0C
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_RESERVEDD_V4                                 0x0D
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_RESERVEDE_V4                                 0x0E
#define THERMOSTAT_FAN_MODE_REPORT_FAN_MODE_RESERVEDF_V4                                 0x0F
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_RESERVED_MASK_V4                          0x70
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_RESERVED_SHIFT_V4                         0x04
#define THERMOSTAT_FAN_MODE_REPORT_PROPERTIES1_OFF_BIT_MASK_V4                           0x80
/* Values used for Thermostat Fan Mode Set command */
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_FAN_MODE_MASK_V4                             0x0F
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_LOW_V4                                     0x00
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_LOW_V4                                          0x01
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_HIGH_V4                                    0x02
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HIGH_V4                                         0x03
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_AUTO_MEDIUM_V4                                  0x04
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_MEDIUM_V4                                       0x05
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_CIRCULATION_V4                                  0x06
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_HUMIDITY_V4                                     0x07
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_LEFT_RIGHT_V4                                   0x08
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_UP_DOWN_V4                                      0x09
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_QUIET_V4                                        0x0A
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_RESERVEDB_V4                                    0x0B
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_RESERVEDC_V4                                    0x0C
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_RESERVEDD_V4                                    0x0D
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_RESERVEDE_V4                                    0x0E
#define THERMOSTAT_FAN_MODE_SET_FAN_MODE_RESERVEDF_V4                                    0x0F
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_RESERVED_MASK_V4                             0x70
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_RESERVED_SHIFT_V4                            0x04
#define THERMOSTAT_FAN_MODE_SET_PROPERTIES1_OFF_BIT_MASK_V4                              0x80

/* Thermostat Fan State command class commands */
#define THERMOSTAT_FAN_STATE_VERSION                                                     0x01
#define THERMOSTAT_FAN_STATE_GET                                                         0x02
#define THERMOSTAT_FAN_STATE_REPORT                                                      0x03
/* Values used for Thermostat Fan State Report command */
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_FAN_OPERATING_STATE_MASK                       0x0F
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_RESERVED_MASK                                  0xF0
#define THERMOSTAT_FAN_STATE_REPORT_LEVEL_RESERVED_SHIFT                                 0x04

/* Thermostat Heating command class commands */
#define THERMOSTAT_HEATING_VERSION                                                       0x01
#define THERMOSTAT_HEATING_STATUS_REPORT                                                 0x0D
#define THERMOSTAT_HEATING_MODE_GET                                                      0x02
#define THERMOSTAT_HEATING_MODE_REPORT                                                   0x03
#define THERMOSTAT_HEATING_MODE_SET                                                      0x01
#define THERMOSTAT_HEATING_RELAY_STATUS_GET                                              0x09
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT                                           0x0A
#define THERMOSTAT_HEATING_SETPOINT_GET                                                  0x05
#define THERMOSTAT_HEATING_SETPOINT_REPORT                                               0x06
#define THERMOSTAT_HEATING_SETPOINT_SET                                                  0x04
#define THERMOSTAT_HEATING_STATUS_GET                                                    0x0C
#define THERMOSTAT_HEATING_STATUS_SET                                                    0x0B
#define THERMOSTAT_HEATING_TIMED_OFF_SET                                                 0x11
/* Values used for Thermostat Heating Status Report command */
#define THERMOSTAT_HEATING_STATUS_REPORT_HEATING                                         0x00
#define THERMOSTAT_HEATING_STATUS_REPORT_COOLING                                         0x01
/* Values used for Thermostat Heating Mode Report command */
#define THERMOSTAT_HEATING_MODE_REPORT_OFF                                               0x00
#define THERMOSTAT_HEATING_MODE_REPORT_OFF_TIMED                                         0x01
#define THERMOSTAT_HEATING_MODE_REPORT_OFF_3_HOURS                                       0x02
#define THERMOSTAT_HEATING_MODE_REPORT_ANTI_FREEZE                                       0x03
#define THERMOSTAT_HEATING_MODE_REPORT_MANUAL                                            0x04
#define THERMOSTAT_HEATING_MODE_REPORT_TEMPORARY_MANUAL                                  0x05
#define THERMOSTAT_HEATING_MODE_REPORT_AUTOMATIC                                         0x06
#define THERMOSTAT_HEATING_MODE_REPORT_MANUAL_TIMED                                      0x07
/* Values used for Thermostat Heating Mode Set command */
#define THERMOSTAT_HEATING_MODE_SET_OFF                                                  0x00
#define THERMOSTAT_HEATING_MODE_SET_OFF_TIMED                                            0x01
#define THERMOSTAT_HEATING_MODE_SET_OFF_3_HOURS                                          0x02
#define THERMOSTAT_HEATING_MODE_SET_ANTI_FREEZE                                          0x03
#define THERMOSTAT_HEATING_MODE_SET_MANUAL                                               0x04
#define THERMOSTAT_HEATING_MODE_SET_TEMPORARY_MANUAL                                     0x05
#define THERMOSTAT_HEATING_MODE_SET_AUTOMATIC                                            0x06
#define THERMOSTAT_HEATING_MODE_SET_MANUAL_TIMED                                         0x07
/* Values used for Thermostat Heating Relay Status Report command */
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT_OFF                                       0x00
#define THERMOSTAT_HEATING_RELAY_STATUS_REPORT_ON                                        0x01
/* Values used for Thermostat Heating Setpoint Report command */
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SIZE_MASK                         0x07
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SCALE_MASK                        0x18
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_SCALE_SHIFT                       0x03
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_PRECISION_MASK                    0xE0
#define THERMOSTAT_HEATING_SETPOINT_REPORT_PROPERTIES1_PRECISION_SHIFT                   0x05
/* Values used for Thermostat Heating Setpoint Set command */
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SIZE_MASK                            0x07
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SCALE_MASK                           0x18
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_SCALE_SHIFT                          0x03
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_PRECISION_MASK                       0xE0
#define THERMOSTAT_HEATING_SETPOINT_SET_PROPERTIES1_PRECISION_SHIFT                      0x05
/* Values used for Thermostat Heating Status Set command */
#define THERMOSTAT_HEATING_STATUS_SET_HEATING                                            0x00
#define THERMOSTAT_HEATING_STATUS_SET_COOLING                                            0x01

/* Thermostat Mode command class commands */
#define THERMOSTAT_MODE_VERSION                                                          0x01
#define THERMOSTAT_MODE_GET                                                              0x02
#define THERMOSTAT_MODE_REPORT                                                           0x03
#define THERMOSTAT_MODE_SET                                                              0x01
#define THERMOSTAT_MODE_SUPPORTED_GET                                                    0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT                                                 0x05
/* Values used for Thermostat Mode Report command */
#define THERMOSTAT_MODE_REPORT_LEVEL_MODE_MASK                                           0x1F
#define THERMOSTAT_MODE_REPORT_MODE_OFF                                                  0x00
#define THERMOSTAT_MODE_REPORT_MODE_HEAT                                                 0x01
#define THERMOSTAT_MODE_REPORT_MODE_COOL                                                 0x02
#define THERMOSTAT_MODE_REPORT_MODE_AUTO                                                 0x03
#define THERMOSTAT_MODE_REPORT_MODE_AUXILIARY_HEAT                                       0x04
#define THERMOSTAT_MODE_REPORT_MODE_RESUME                                               0x05
#define THERMOSTAT_MODE_REPORT_MODE_FAN_ONLY                                             0x06
#define THERMOSTAT_MODE_REPORT_MODE_FURNACE                                              0x07
#define THERMOSTAT_MODE_REPORT_MODE_DRY_AIR                                              0x08
#define THERMOSTAT_MODE_REPORT_MODE_MOIST_AIR                                            0x09
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_CHANGEOVER                                      0x0A
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_MASK                                       0xE0
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_SHIFT                                      0x05
/* Values used for Thermostat Mode Set command */
#define THERMOSTAT_MODE_SET_LEVEL_MODE_MASK                                              0x1F
#define THERMOSTAT_MODE_SET_MODE_OFF                                                     0x00
#define THERMOSTAT_MODE_SET_MODE_HEAT                                                    0x01
#define THERMOSTAT_MODE_SET_MODE_COOL                                                    0x02
#define THERMOSTAT_MODE_SET_MODE_AUTO                                                    0x03
#define THERMOSTAT_MODE_SET_MODE_AUXILIARY_HEAT                                          0x04
#define THERMOSTAT_MODE_SET_MODE_RESUME                                                  0x05
#define THERMOSTAT_MODE_SET_MODE_FAN_ONLY                                                0x06
#define THERMOSTAT_MODE_SET_MODE_FURNACE                                                 0x07
#define THERMOSTAT_MODE_SET_MODE_DRY_AIR                                                 0x08
#define THERMOSTAT_MODE_SET_MODE_MOIST_AIR                                               0x09
#define THERMOSTAT_MODE_SET_MODE_AUTO_CHANGEOVER                                         0x0A
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_MASK                                          0xE0
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_SHIFT                                         0x05

/* Thermostat Mode command class commands */
#define THERMOSTAT_MODE_VERSION_V2                                                       0x02
#define THERMOSTAT_MODE_GET_V2                                                           0x02
#define THERMOSTAT_MODE_REPORT_V2                                                        0x03
#define THERMOSTAT_MODE_SET_V2                                                           0x01
#define THERMOSTAT_MODE_SUPPORTED_GET_V2                                                 0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT_V2                                              0x05
/* Values used for Thermostat Mode Report command */
#define THERMOSTAT_MODE_REPORT_LEVEL_MODE_MASK_V2                                        0x1F
#define THERMOSTAT_MODE_REPORT_MODE_OFF_V2                                               0x00
#define THERMOSTAT_MODE_REPORT_MODE_HEAT_V2                                              0x01
#define THERMOSTAT_MODE_REPORT_MODE_COOL_V2                                              0x02
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_V2                                              0x03
#define THERMOSTAT_MODE_REPORT_MODE_AUXILIARY_HEAT_V2                                    0x04
#define THERMOSTAT_MODE_REPORT_MODE_RESUME_V2                                            0x05
#define THERMOSTAT_MODE_REPORT_MODE_FAN_ONLY_V2                                          0x06
#define THERMOSTAT_MODE_REPORT_MODE_FURNACE_V2                                           0x07
#define THERMOSTAT_MODE_REPORT_MODE_DRY_AIR_V2                                           0x08
#define THERMOSTAT_MODE_REPORT_MODE_MOIST_AIR_V2                                         0x09
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_CHANGEOVER_V2                                   0x0A
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_HEAT_V2                                  0x0B
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_COOL_V2                                  0x0C
#define THERMOSTAT_MODE_REPORT_MODE_AWAY_V2                                              0x0D
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_MASK_V2                                    0xE0
#define THERMOSTAT_MODE_REPORT_LEVEL_RESERVED_SHIFT_V2                                   0x05
/* Values used for Thermostat Mode Set command */
#define THERMOSTAT_MODE_SET_LEVEL_MODE_MASK_V2                                           0x1F
#define THERMOSTAT_MODE_SET_MODE_OFF_V2                                                  0x00
#define THERMOSTAT_MODE_SET_MODE_HEAT_V2                                                 0x01
#define THERMOSTAT_MODE_SET_MODE_COOL_V2                                                 0x02
#define THERMOSTAT_MODE_SET_MODE_AUTO_V2                                                 0x03
#define THERMOSTAT_MODE_SET_MODE_AUXILIARY_HEAT_V2                                       0x04
#define THERMOSTAT_MODE_SET_MODE_RESUME_V2                                               0x05
#define THERMOSTAT_MODE_SET_MODE_FAN_ONLY_V2                                             0x06
#define THERMOSTAT_MODE_SET_MODE_FURNACE_V2                                              0x07
#define THERMOSTAT_MODE_SET_MODE_DRY_AIR_V2                                              0x08
#define THERMOSTAT_MODE_SET_MODE_MOIST_AIR_V2                                            0x09
#define THERMOSTAT_MODE_SET_MODE_AUTO_CHANGEOVER_V2                                      0x0A
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_HEAT_V2                                     0x0B
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_COOL_V2                                     0x0C
#define THERMOSTAT_MODE_SET_MODE_AWAY_V2                                                 0x0D
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_MASK_V2                                       0xE0
#define THERMOSTAT_MODE_SET_LEVEL_RESERVED_SHIFT_V2                                      0x05

/* Thermostat Mode command class commands */
#define THERMOSTAT_MODE_VERSION_V3                                                       0x03
#define THERMOSTAT_MODE_GET_V3                                                           0x02
#define THERMOSTAT_MODE_REPORT_V3                                                        0x03
#define THERMOSTAT_MODE_SET_V3                                                           0x01
#define THERMOSTAT_MODE_SUPPORTED_GET_V3                                                 0x04
#define THERMOSTAT_MODE_SUPPORTED_REPORT_V3                                              0x05
/* Values used for Thermostat Mode Report command */
#define THERMOSTAT_MODE_REPORT_LEVEL_MODE_MASK_V3                                        0x1F
#define THERMOSTAT_MODE_REPORT_MODE_OFF_V3                                               0x00
#define THERMOSTAT_MODE_REPORT_MODE_HEAT_V3                                              0x01
#define THERMOSTAT_MODE_REPORT_MODE_COOL_V3                                              0x02
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_V3                                              0x03
#define THERMOSTAT_MODE_REPORT_MODE_AUXILIARY_HEAT_V3                                    0x04
#define THERMOSTAT_MODE_REPORT_MODE_RESUME_V3                                            0x05
#define THERMOSTAT_MODE_REPORT_MODE_FAN_ONLY_V3                                          0x06
#define THERMOSTAT_MODE_REPORT_MODE_FURNACE_V3                                           0x07
#define THERMOSTAT_MODE_REPORT_MODE_DRY_AIR_V3                                           0x08
#define THERMOSTAT_MODE_REPORT_MODE_MOIST_AIR_V3                                         0x09
#define THERMOSTAT_MODE_REPORT_MODE_AUTO_CHANGEOVER_V3                                   0x0A
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_HEAT_V3                                  0x0B
#define THERMOSTAT_MODE_REPORT_MODE_ENERGY_SAVE_COOL_V3                                  0x0C
#define THERMOSTAT_MODE_REPORT_MODE_AWAY_V3                                              0x0D
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED_V3                                          0x0E
#define THERMOSTAT_MODE_REPORT_MODE_FULL_POWER_V3                                        0x0F
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED0_V3                                         0x10
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED1_V3                                         0x11
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED2_V3                                         0x12
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED3_V3                                         0x13
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED4_V3                                         0x14
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED5_V3                                         0x15
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED6_V3                                         0x16
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED7_V3                                         0x17
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED8_V3                                         0x18
#define THERMOSTAT_MODE_REPORT_MODE_RESERVED9_V3                                         0x19
#define THERMOSTAT_MODE_REPORT_MODE_RESERVEDA_V3                                         0x1A
#define THERMOSTAT_MODE_REPORT_MODE_RESERVEDB_V3                                         0x1B
#define THERMOSTAT_MODE_REPORT_MODE_RESERVEDC_V3                                         0x1C
#define THERMOSTAT_MODE_REPORT_MODE_RESERVEDD_V3                                         0x1D
#define THERMOSTAT_MODE_REPORT_MODE_RESERVEDE_V3                                         0x1E
#define THERMOSTAT_MODE_REPORT_MODE_MANUFACTURER_SPECIFC_V3                              0x1F
#define THERMOSTAT_MODE_REPORT_LEVEL_NO_OF_MANUFACTURER_DATA_FIELDS_MASK_V3              0xE0
#define THERMOSTAT_MODE_REPORT_LEVEL_NO_OF_MANUFACTURER_DATA_FIELDS_SHIFT_V3             0x05
/* Values used for Thermostat Mode Set command */
#define THERMOSTAT_MODE_SET_LEVEL_MODE_MASK_V3                                           0x1F
#define THERMOSTAT_MODE_SET_MODE_OFF_V3                                                  0x00
#define THERMOSTAT_MODE_SET_MODE_HEAT_V3                                                 0x01
#define THERMOSTAT_MODE_SET_MODE_COOL_V3                                                 0x02
#define THERMOSTAT_MODE_SET_MODE_AUTO_V3                                                 0x03
#define THERMOSTAT_MODE_SET_MODE_AUXILIARY_HEAT_V3                                       0x04
#define THERMOSTAT_MODE_SET_MODE_RESUME_V3                                               0x05
#define THERMOSTAT_MODE_SET_MODE_FAN_ONLY_V3                                             0x06
#define THERMOSTAT_MODE_SET_MODE_FURNACE_V3                                              0x07
#define THERMOSTAT_MODE_SET_MODE_DRY_AIR_V3                                              0x08
#define THERMOSTAT_MODE_SET_MODE_MOIST_AIR_V3                                            0x09
#define THERMOSTAT_MODE_SET_MODE_AUTO_CHANGEOVER_V3                                      0x0A
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_HEAT_V3                                     0x0B
#define THERMOSTAT_MODE_SET_MODE_ENERGY_SAVE_COOL_V3                                     0x0C
#define THERMOSTAT_MODE_SET_MODE_AWAY_V3                                                 0x0D
#define THERMOSTAT_MODE_SET_MODE_RESERVED_V3                                             0x0E
#define THERMOSTAT_MODE_SET_MODE_FULL_POWER_V3                                           0x0F
#define THERMOSTAT_MODE_SET_MODE_RESERVED0_V3                                            0x10
#define THERMOSTAT_MODE_SET_MODE_RESERVED1_V3                                            0x11
#define THERMOSTAT_MODE_SET_MODE_RESERVED2_V3                                            0x12
#define THERMOSTAT_MODE_SET_MODE_RESERVED3_V3                                            0x13
#define THERMOSTAT_MODE_SET_MODE_RESERVED4_V3                                            0x14
#define THERMOSTAT_MODE_SET_MODE_RESERVED5_V3                                            0x15
#define THERMOSTAT_MODE_SET_MODE_RESERVED6_V3                                            0x16
#define THERMOSTAT_MODE_SET_MODE_RESERVED7_V3                                            0x17
#define THERMOSTAT_MODE_SET_MODE_RESERVED8_V3                                            0x18
#define THERMOSTAT_MODE_SET_MODE_RESERVED9_V3                                            0x19
#define THERMOSTAT_MODE_SET_MODE_RESERVEDA_V3                                            0x1A
#define THERMOSTAT_MODE_SET_MODE_RESERVEDB_V3                                            0x1B
#define THERMOSTAT_MODE_SET_MODE_RESERVEDC_V3                                            0x1C
#define THERMOSTAT_MODE_SET_MODE_RESERVEDD_V3                                            0x1D
#define THERMOSTAT_MODE_SET_MODE_RESERVEDE_V3                                            0x1E
#define THERMOSTAT_MODE_SET_MODE_MANUFACTURER_SPECIFC_V3                                 0x1F
#define THERMOSTAT_MODE_SET_LEVEL_NO_OF_MANUFACTURER_DATA_FIELDS_MASK_V3                 0xE0
#define THERMOSTAT_MODE_SET_LEVEL_NO_OF_MANUFACTURER_DATA_FIELDS_SHIFT_V3                0x05

/* Thermostat Operating State command class commands */
#define THERMOSTAT_OPERATING_STATE_VERSION                                               0x01
#define THERMOSTAT_OPERATING_STATE_GET                                                   0x02
#define THERMOSTAT_OPERATING_STATE_REPORT                                                0x03
/* Values used for Thermostat Operating State Report command */
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_OPERATING_STATE_MASK                     0x0F
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_IDLE                           0x00
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_HEATING                        0x01
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_COOLING                        0x02
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_FAN_ONLY                       0x03
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_PENDING_HEAT                   0x04
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_PENDING_COOL                   0x05
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_VENT_ECONOMIZER                0x06
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_RESERVED_MASK                            0xF0
#define THERMOSTAT_OPERATING_STATE_REPORT_LEVEL_RESERVED_SHIFT                           0x04

/* Thermostat Operating State command class commands */
#define THERMOSTAT_OPERATING_STATE_VERSION_V2                                            0x02
#define THERMOSTAT_OPERATING_STATE_GET_V2                                                0x02
#define THERMOSTAT_OPERATING_STATE_REPORT_V2                                             0x03
#define THERMOSTAT_OPERATING_STATE_LOGGING_SUPPORTED_GET_V2                              0x01
#define THERMOSTAT_OPERATING_LOGGING_SUPPORTED_REPORT_V2                                 0x04
#define THERMOSTAT_OPERATING_STATE_LOGGING_GET_V2                                        0x05
#define THERMOSTAT_OPERATING_STATE_LOGGING_REPORT_V2                                     0x06
/* Values used for Thermostat Operating State Report command */
#define THERMOSTAT_OPERATING_STATE_REPORT_PROPERTIES1_OPERATING_STATE_MASK_V2            0x0F
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_IDLE_V2                        0x00
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_HEATING_V2                     0x01
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_COOLING_V2                     0x02
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_FAN_ONLY_V2                    0x03
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_PENDING_HEAT_V2                0x04
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_PENDING_COOL_V2                0x05
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_VENT_ECONOMIZER_V2             0x06
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_AUX_HEATING_V2                 0x07
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_2ND_STAGE_HEATING_V2           0x08
#define THERMOSTAT_OPERATING_STATE_REPORT_OPERATING_STATE_2ND_STAGE_COOLING_V2           0x09
#define THERMOSTAT_OPERATING_STATE_REPORT_PROPERTIES1_RESERVED_MASK_V2                   0xF0
#define THERMOSTAT_OPERATING_STATE_REPORT_PROPERTIES1_RESERVED_SHIFT_V2                  0x04

/* Thermostat Setback command class commands */
#define THERMOSTAT_SETBACK_VERSION                                                       0x01
#define THERMOSTAT_SETBACK_GET                                                           0x02
#define THERMOSTAT_SETBACK_REPORT                                                        0x03
#define THERMOSTAT_SETBACK_SET                                                           0x01
/* Values used for Thermostat Setback Report command */
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_SETBACK_TYPE_MASK                          0x03
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_NO_OVERRIDE                               0x00
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_TEMPORARY_OVERRIDE                        0x01
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_PERMANENT_OVERRIDE                        0x02
#define THERMOSTAT_SETBACK_REPORT_SETBACK_TYPE_RESERVED                                  0x03
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_RESERVED_MASK                              0xFC
#define THERMOSTAT_SETBACK_REPORT_PROPERTIES1_RESERVED_SHIFT                             0x02
/* Values used for Thermostat Setback Set command */
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_SETBACK_TYPE_MASK                             0x03
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_NO_OVERRIDE                                  0x00
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_TEMPORARY_OVERRIDE                           0x01
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_PERMANENT_OVERRIDE                           0x02
#define THERMOSTAT_SETBACK_SET_SETBACK_TYPE_RESERVED                                     0x03
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_RESERVED_MASK                                 0xFC
#define THERMOSTAT_SETBACK_SET_PROPERTIES1_RESERVED_SHIFT                                0x02

/* Thermostat Setpoint command class commands */
#define THERMOSTAT_SETPOINT_VERSION                                                      0x01
#define THERMOSTAT_SETPOINT_GET                                                          0x02
#define THERMOSTAT_SETPOINT_REPORT                                                       0x03
#define THERMOSTAT_SETPOINT_SET                                                          0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET                                                0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT                                             0x05
/* Values used for Thermostat Setpoint Get command */
#define THERMOSTAT_SETPOINT_GET_LEVEL_SETPOINT_TYPE_MASK                                 0x0F
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED                              0x00
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_HEATING_1                                  0x01
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_COOLING_1                                  0x02
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED1                             0x03
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED2                             0x04
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED3                             0x05
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED4                             0x06
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FURNACE                                    0x07
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_DRY_AIR                                    0x08
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MOIST_AIR                                  0x09
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AUTO_CHANGEOVER                            0x0A
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_SHIFT                                     0x04
/* Values used for Thermostat Setpoint Report command */
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_SETPOINT_TYPE_MASK                              0x0F
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED                           0x00
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_HEATING_1                               0x01
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_COOLING_1                               0x02
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED1                          0x03
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED2                          0x04
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED3                          0x05
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED4                          0x06
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FURNACE                                 0x07
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_DRY_AIR                                 0x08
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MOIST_AIR                               0x09
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER                         0x0A
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK                                   0xF0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_SHIFT                                  0x04
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK                                      0x07
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_MASK                                     0x18
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_SHIFT                                    0x03
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_MASK                                 0xE0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_SHIFT                                0x05
/* Values used for Thermostat Setpoint Set command */
#define THERMOSTAT_SETPOINT_SET_LEVEL_SETPOINT_TYPE_MASK                                 0x0F
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED                              0x00
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_HEATING_1                                  0x01
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_COOLING_1                                  0x02
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED1                             0x03
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED2                             0x04
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED3                             0x05
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED4                             0x06
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FURNACE                                    0x07
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_DRY_AIR                                    0x08
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MOIST_AIR                                  0x09
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AUTO_CHANGEOVER                            0x0A
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_MASK                                      0xF0
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_SHIFT                                     0x04
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SIZE_MASK                                         0x07
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_MASK                                        0x18
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_SHIFT                                       0x03
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_MASK                                    0xE0
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_SHIFT                                   0x05

/* Thermostat Setpoint command class commands */
#define THERMOSTAT_SETPOINT_VERSION_V2                                                   0x02
#define THERMOSTAT_SETPOINT_GET_V2                                                       0x02
#define THERMOSTAT_SETPOINT_REPORT_V2                                                    0x03
#define THERMOSTAT_SETPOINT_SET_V2                                                       0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET_V2                                             0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT_V2                                          0x05
/* Values used for Thermostat Setpoint Get command */
#define THERMOSTAT_SETPOINT_GET_LEVEL_SETPOINT_TYPE_MASK_V2                              0x0F
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED_V2                           0x00
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_HEATING_1_V2                               0x01
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_COOLING_1_V2                               0x02
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED1_V2                          0x03
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED2_V2                          0x04
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED3_V2                          0x05
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED4_V2                          0x06
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FURNACE_V2                                 0x07
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_DRY_AIR_V2                                 0x08
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MOIST_AIR_V2                               0x09
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                         0x0A
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                     0x0B
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                     0x0C
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AWAY_HEATING_V2                            0x0D
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_MASK_V2                                   0xF0
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_SHIFT_V2                                  0x04
/* Values used for Thermostat Setpoint Report command */
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_SETPOINT_TYPE_MASK_V2                           0x0F
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED_V2                        0x00
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_HEATING_1_V2                            0x01
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_COOLING_1_V2                            0x02
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED1_V2                       0x03
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED2_V2                       0x04
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED3_V2                       0x05
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED4_V2                       0x06
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FURNACE_V2                              0x07
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_DRY_AIR_V2                              0x08
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MOIST_AIR_V2                            0x09
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                      0x0A
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                  0x0B
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                  0x0C
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AWAY_HEATING_V2                         0x0D
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK_V2                                0xF0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_SHIFT_V2                               0x04
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK_V2                                   0x07
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_MASK_V2                                  0x18
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_SHIFT_V2                                 0x03
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_MASK_V2                              0xE0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_SHIFT_V2                             0x05
/* Values used for Thermostat Setpoint Set command */
#define THERMOSTAT_SETPOINT_SET_LEVEL_SETPOINT_TYPE_MASK_V2                              0x0F
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED_V2                           0x00
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_HEATING_1_V2                               0x01
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_COOLING_1_V2                               0x02
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED1_V2                          0x03
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED2_V2                          0x04
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED3_V2                          0x05
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED4_V2                          0x06
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FURNACE_V2                                 0x07
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_DRY_AIR_V2                                 0x08
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MOIST_AIR_V2                               0x09
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AUTO_CHANGEOVER_V2                         0x0A
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V2                     0x0B
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V2                     0x0C
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AWAY_HEATING_V2                            0x0D
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_MASK_V2                                   0xF0
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_SHIFT_V2                                  0x04
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SIZE_MASK_V2                                      0x07
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_MASK_V2                                     0x18
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_SHIFT_V2                                    0x03
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_MASK_V2                                 0xE0
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_SHIFT_V2                                0x05

/* Thermostat Setpoint command class commands */
#define THERMOSTAT_SETPOINT_VERSION_V3                                                   0x03
#define THERMOSTAT_SETPOINT_GET_V3                                                       0x02
#define THERMOSTAT_SETPOINT_REPORT_V3                                                    0x03
#define THERMOSTAT_SETPOINT_SET_V3                                                       0x01
#define THERMOSTAT_SETPOINT_SUPPORTED_GET_V3                                             0x04
#define THERMOSTAT_SETPOINT_SUPPORTED_REPORT_V3                                          0x05
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_V3                                          0x09
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_V3                                       0x0A
/* Values used for Thermostat Setpoint Get command */
#define THERMOSTAT_SETPOINT_GET_LEVEL_SETPOINT_TYPE_MASK_V3                              0x0F
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED_V3                           0x00
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_HEATING_1_V3                               0x01
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_COOLING_1_V3                               0x02
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED1_V3                          0x03
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED2_V3                          0x04
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED3_V3                          0x05
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_NOT_SUPPORTED4_V3                          0x06
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FURNACE_V3                                 0x07
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_DRY_AIR_V3                                 0x08
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_MOIST_AIR_V3                               0x09
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AUTO_CHANGEOVER_V3                         0x0A
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V3                     0x0B
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V3                     0x0C
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AWAY_HEATING_V3                            0x0D
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_AWAY_COOLING_V3                            0x0E
#define THERMOSTAT_SETPOINT_GET_SETPOINT_TYPE_FULL_POWER_V3                              0x0F
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_MASK_V3                                   0xF0
#define THERMOSTAT_SETPOINT_GET_LEVEL_RESERVED_SHIFT_V3                                  0x04
/* Values used for Thermostat Setpoint Report command */
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_SETPOINT_TYPE_MASK_V3                           0x0F
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED_V3                        0x00
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_HEATING_1_V3                            0x01
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_COOLING_1_V3                            0x02
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED1_V3                       0x03
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED2_V3                       0x04
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED3_V3                       0x05
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_NOT_SUPPORTED4_V3                       0x06
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FURNACE_V3                              0x07
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_DRY_AIR_V3                              0x08
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_MOIST_AIR_V3                            0x09
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER_V3                      0x0A
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V3                  0x0B
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V3                  0x0C
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AWAY_HEATING_V3                         0x0D
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_AWAY_COOLING_V3                         0x0E
#define THERMOSTAT_SETPOINT_REPORT_SETPOINT_TYPE_FULL_POWER_V3                           0x0F
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_MASK_V3                                0xF0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL_RESERVED_SHIFT_V3                               0x04
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SIZE_MASK_V3                                   0x07
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_MASK_V3                                  0x18
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_SCALE_SHIFT_V3                                 0x03
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_MASK_V3                              0xE0
#define THERMOSTAT_SETPOINT_REPORT_LEVEL2_PRECISION_SHIFT_V3                             0x05
/* Values used for Thermostat Setpoint Set command */
#define THERMOSTAT_SETPOINT_SET_LEVEL_SETPOINT_TYPE_MASK_V3                              0x0F
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED_V3                           0x00
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_HEATING_1_V3                               0x01
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_COOLING_1_V3                               0x02
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED1_V3                          0x03
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED2_V3                          0x04
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED3_V3                          0x05
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_NOT_SUPPORTED4_V3                          0x06
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FURNACE_V3                                 0x07
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_DRY_AIR_V3                                 0x08
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_MOIST_AIR_V3                               0x09
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AUTO_CHANGEOVER_V3                         0x0A
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V3                     0x0B
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V3                     0x0C
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AWAY_HEATING_V3                            0x0D
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_AWAY_COOLING_V3                            0x0E
#define THERMOSTAT_SETPOINT_SET_SETPOINT_TYPE_FULL_POWER_V3                              0x0F
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_MASK_V3                                   0xF0
#define THERMOSTAT_SETPOINT_SET_LEVEL_RESERVED_SHIFT_V3                                  0x04
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SIZE_MASK_V3                                      0x07
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_MASK_V3                                     0x18
#define THERMOSTAT_SETPOINT_SET_LEVEL2_SCALE_SHIFT_V3                                    0x03
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_MASK_V3                                 0xE0
#define THERMOSTAT_SETPOINT_SET_LEVEL2_PRECISION_SHIFT_V3                                0x05
/* Values used for Thermostat Setpoint Capabilities Get command */
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_PROPERTIES1_SETPOINT_TYPE_MASK_V3           0x0F
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_NOT_SUPPORTED_V3              0x00
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_HEATING_1_V3                  0x01
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_COOLING_1_V3                  0x02
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_NOT_SUPPORTED1_V3             0x03
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_NOT_SUPPORTED2_V3             0x04
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_NOT_SUPPORTED3_V3             0x05
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_NOT_SUPPORTED4_V3             0x06
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_FURNACE_V3                    0x07
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_DRY_AIR_V3                    0x08
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_MOIST_AIR_V3                  0x09
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_AUTO_CHANGEOVER_V3            0x0A
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V3        0x0B
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V3        0x0C
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_AWAY_HEATING_V3               0x0D
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_AWAY_COOLING_V3               0x0E
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_SETPOINT_TYPE_FULL_POWER_V3                 0x0F
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_PROPERTIES1_RESERVED_MASK_V3                0xF0
#define THERMOSTAT_SETPOINT_CAPABILITIES_GET_PROPERTIES1_RESERVED_SHIFT_V3               0x04
/* Values used for Thermostat Setpoint Capabilities Report command */
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES1_SETPOINT_TYPE_MASK_V3        0x0F
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_NOT_SUPPORTED_V3           0x00
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_HEATING_1_V3               0x01
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_COOLING_1_V3               0x02
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_NOT_SUPPORTED1_V3          0x03
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_NOT_SUPPORTED2_V3          0x04
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_NOT_SUPPORTED3_V3          0x05
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_NOT_SUPPORTED4_V3          0x06
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_FURNACE_V3                 0x07
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_DRY_AIR_V3                 0x08
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_MOIST_AIR_V3               0x09
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_AUTO_CHANGEOVER_V3         0x0A
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_ENERGY_SAVE_HEATING_V3     0x0B
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_ENERGY_SAVE_COOLING_V3     0x0C
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_AWAY_HEATING_V3            0x0D
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_AWAY_COOLING_V3            0x0E
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_SETPOINT_TYPE_FULL_POWER_V3              0x0F
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES1_RESERVED_MASK_V3             0xF0
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES1_RESERVED_SHIFT_V3            0x04
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES2_SIZE_MASK_V3                 0x07
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES2_SCALE_MASK_V3                0x18
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES2_SCALE_SHIFT_V3               0x03
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES2_PRECISION_MASK_V3            0xE0
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES2_PRECISION_SHIFT_V3           0x05
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES3_SIZE_MASK_V3                 0x07
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES3_SCALE_MASK_V3                0x18
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES3_SCALE_SHIFT_V3               0x03
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES3_PRECISION_MASK_V3            0xE0
#define THERMOSTAT_SETPOINT_CAPABILITIES_REPORT_PROPERTIES3_PRECISION_SHIFT_V3           0x05

/* Time Parameters command class commands */
#define TIME_PARAMETERS_VERSION                                                          0x01
#define TIME_PARAMETERS_GET                                                              0x02
#define TIME_PARAMETERS_REPORT                                                           0x03
#define TIME_PARAMETERS_SET                                                              0x01

/* Time command class commands */
#define TIME_VERSION                                                                     0x01
#define DATE_GET                                                                         0x03
#define DATE_REPORT                                                                      0x04
#define TIME_GET                                                                         0x01
#define TIME_REPORT                                                                      0x02
/* Values used for Time Report command */
#define TIME_REPORT_HOUR_LOCAL_TIME_HOUR_LOCAL_TIME_MASK                                 0x1F
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_MASK                                        0x60
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_SHIFT                                       0x05
#define TIME_REPORT_HOUR_LOCAL_TIME_RTC_FAILURE_BIT_MASK                                 0x80

/* Time command class commands */
#define TIME_VERSION_V2                                                                  0x02
#define DATE_GET_V2                                                                      0x03
#define DATE_REPORT_V2                                                                   0x04
#define TIME_GET_V2                                                                      0x01
#define TIME_OFFSET_GET_V2                                                               0x06
#define TIME_OFFSET_REPORT_V2                                                            0x07
#define TIME_OFFSET_SET_V2                                                               0x05
#define TIME_REPORT_V2                                                                   0x02
/* Values used for Time Offset Report command */
#define TIME_OFFSET_REPORT_LEVEL_HOUR_TZO_MASK_V2                                        0x7F
#define TIME_OFFSET_REPORT_LEVEL_SIGN_TZO_BIT_MASK_V2                                    0x80
#define TIME_OFFSET_REPORT_LEVEL2_MINUTE_OFFSET_DST_MASK_V2                              0x7F
#define TIME_OFFSET_REPORT_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2                            0x80
/* Values used for Time Offset Set command */
#define TIME_OFFSET_SET_LEVEL_HOUR_TZO_MASK_V2                                           0x7F
#define TIME_OFFSET_SET_LEVEL_SIGN_TZO_BIT_MASK_V2                                       0x80
#define TIME_OFFSET_SET_LEVEL2_MINUTE_OFFSET_DST_MASK_V2                                 0x7F
#define TIME_OFFSET_SET_LEVEL2_SIGN_OFFSET_DST_BIT_MASK_V2                               0x80
/* Values used for Time Report command */
#define TIME_REPORT_HOUR_LOCAL_TIME_HOUR_LOCAL_TIME_MASK_V2                              0x1F
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_MASK_V2                                     0x60
#define TIME_REPORT_HOUR_LOCAL_TIME_RESERVED_SHIFT_V2                                    0x05
#define TIME_REPORT_HOUR_LOCAL_TIME_RTC_FAILURE_BIT_MASK_V2                              0x80

/* Transport Service command class commands */
#define TRANSPORT_SERVICE_VERSION                                                        0x01
#define COMMAND_FIRST_FRAGMENT                                                           0xC0
#define COMMAND_FRAGMENT_COMPLETE                                                        0xE8
#define COMMAND_FRAGMENT_REQUEST                                                         0xC8
#define COMMAND_FRAGMENT_WAIT                                                            0xF0
#define COMMAND_SUBSEQUENT_FRAGMENT                                                      0xE0
/* Values used for Command First Fragment command */
#define COMMAND_FIRST_FRAGMENT_MASK                                                      0xF8
#define COMMAND_FIRST_FRAGMENT_DATAGRAM_SIZE_1_MASK                                      0x07
#define COMMAND_FIRST_FRAGMENT_PROPERTIES2_SEQUENCE_NO_MASK                              0x0F
#define COMMAND_FIRST_FRAGMENT_PROPERTIES2_RESERVED_MASK                                 0xF0
#define COMMAND_FIRST_FRAGMENT_PROPERTIES2_RESERVED_SHIFT                                0x04
/* Values used for Command Fragment Complete command */
#define COMMAND_FRAGMENT_COMPLETE_MASK                                                   0xF8
#define COMMAND_FRAGMENT_COMPLETE_RESERVED_MASK                                          0x07
#define COMMAND_FRAGMENT_COMPLETE_PROPERTIES2_SEQUENCE_NO_MASK                           0x0F
#define COMMAND_FRAGMENT_COMPLETE_PROPERTIES2_RESERVED_MASK                              0xF0
#define COMMAND_FRAGMENT_COMPLETE_PROPERTIES2_RESERVED_SHIFT                             0x04
/* Values used for Command Fragment Request command */
#define COMMAND_FRAGMENT_REQUEST_MASK                                                    0xF8
#define COMMAND_FRAGMENT_REQUEST_RESERVED_MASK                                           0x07
#define COMMAND_FRAGMENT_REQUEST_PROPERTIES2_SEQUENCE_NO_MASK                            0x0F
#define COMMAND_FRAGMENT_REQUEST_PROPERTIES2_RESERVED_MASK                               0xF0
#define COMMAND_FRAGMENT_REQUEST_PROPERTIES2_RESERVED_SHIFT                              0x04
/* Values used for Command Fragment Wait command */
#define COMMAND_FRAGMENT_WAIT_MASK                                                       0xF8
#define COMMAND_FRAGMENT_WAIT_RESERVED_MASK                                              0x07
/* Values used for Command Subsequent Fragment command */
#define COMMAND_SUBSEQUENT_FRAGMENT_MASK                                                 0xF8
#define COMMAND_SUBSEQUENT_FRAGMENT_DATAGRAM_SIZE_1_MASK                                 0x07
#define COMMAND_SUBSEQUENT_FRAGMENT_PROPERTIES2_DATAGRAM_OFFSET_1_MASK                   0x07
#define COMMAND_SUBSEQUENT_FRAGMENT_PROPERTIES2_SEQUENCE_NO_MASK                         0x78
#define COMMAND_SUBSEQUENT_FRAGMENT_PROPERTIES2_SEQUENCE_NO_SHIFT                        0x03
#define COMMAND_SUBSEQUENT_FRAGMENT_PROPERTIES2_RESERVED_BIT_MASK                        0x80

/* User Code command class commands */
#define USER_CODE_VERSION                                                                0x01
#define USER_CODE_GET                                                                    0x02
#define USER_CODE_REPORT                                                                 0x03
#define USER_CODE_SET                                                                    0x01
#define USERS_NUMBER_GET                                                                 0x04
#define USERS_NUMBER_REPORT                                                              0x05
/* Values used for User Code Report command */
#define USER_CODE_REPORT_AVAILABLE_NOT_SET                                               0x00
#define USER_CODE_REPORT_OCCUPIED                                                        0x01
#define USER_CODE_REPORT_RESERVED_BY_ADMINISTRATOR                                       0x02
#define USER_CODE_REPORT_STATUS_NOT_AVAILABLE                                            0xFE
/* Values used for User Code Set command */
#define USER_CODE_SET_AVAILABLE_NOT_SET                                                  0x00
#define USER_CODE_SET_OCCUPIED                                                           0x01
#define USER_CODE_SET_RESERVED_BY_ADMINISTRATOR                                          0x02
#define USER_CODE_SET_STATUS_NOT_AVAILABLE                                               0xFE

/* Version command class commands */
#define VERSION_VERSION                                                                  0x01
#define VERSION_COMMAND_CLASS_GET                                                        0x13
#define VERSION_COMMAND_CLASS_REPORT                                                     0x14
#define VERSION_GET                                                                      0x11
#define VERSION_REPORT                                                                   0x12

/* Version command class commands */
#define VERSION_VERSION_V2                                                               0x02
#define VERSION_COMMAND_CLASS_GET_V2                                                     0x13
#define VERSION_COMMAND_CLASS_REPORT_V2                                                  0x14
#define VERSION_GET_V2                                                                   0x11
#define VERSION_REPORT_V2                                                                0x12

/* Wake Up command class commands */
#define WAKE_UP_VERSION                                                                  0x01
#define WAKE_UP_INTERVAL_GET                                                             0x05
#define WAKE_UP_INTERVAL_REPORT                                                          0x06
#define WAKE_UP_INTERVAL_SET                                                             0x04
#define WAKE_UP_NO_MORE_INFORMATION                                                      0x08
#define WAKE_UP_NOTIFICATION                                                             0x07

/* Wake Up command class commands */
#define WAKE_UP_VERSION_V2                                                               0x02
#define WAKE_UP_INTERVAL_CAPABILITIES_GET_V2                                             0x09
#define WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2                                          0x0A
#define WAKE_UP_INTERVAL_GET_V2                                                          0x05
#define WAKE_UP_INTERVAL_REPORT_V2                                                       0x06
#define WAKE_UP_INTERVAL_SET_V2                                                          0x04
#define WAKE_UP_NO_MORE_INFORMATION_V2                                                   0x08
#define WAKE_UP_NOTIFICATION_V2                                                          0x07

/* Zip 6lowpan command class commands */
#define ZIP_6LOWPAN_VERSION                                                              0x01
#define LOWPAN_FIRST_FRAGMENT                                                            0xC0
#define LOWPAN_SUBSEQUENT_FRAGMENT                                                       0xE0
/* Values used for Lowpan First Fragment command */
#define LOWPAN_FIRST_FRAGMENT_MASK                                                       0xF8
#define LOWPAN_FIRST_FRAGMENT_DATAGRAM_SIZE_1_MASK                                       0x07
/* Values used for Lowpan Subsequent Fragment command */
#define LOWPAN_SUBSEQUENT_FRAGMENT_MASK                                                  0xF8
#define LOWPAN_SUBSEQUENT_FRAGMENT_DATAGRAM_SIZE_1_MASK                                  0x07

/* Zip command class commands */
#define ZIP_VERSION                                                                      0x01
#define COMMAND_ZIP_PACKET                                                               0x02
/* Values used for Command Zip Packet command */
#define COMMAND_ZIP_PACKET_PROPERTIES1_RESERVED_MASK                                     0x03
#define COMMAND_ZIP_PACKET_PROPERTIES1_NACK_OPTION_ERROR_BIT_MASK                        0x04
#define COMMAND_ZIP_PACKET_PROPERTIES1_NACK_QUEUE_FULL_BIT_MASK                          0x08
#define COMMAND_ZIP_PACKET_PROPERTIES1_NACK_WAITING_BIT_MASK                             0x10
#define COMMAND_ZIP_PACKET_PROPERTIES1_NACK_RESPONSE_BIT_MASK                            0x20
#define COMMAND_ZIP_PACKET_PROPERTIES1_ACK_RESPONSE_BIT_MASK                             0x40
#define COMMAND_ZIP_PACKET_PROPERTIES1_ACK_REQUEST_BIT_MASK                              0x80
#define COMMAND_ZIP_PACKET_PROPERTIES2_RESERVED_MASK                                     0x1F
#define COMMAND_ZIP_PACKET_PROPERTIES2_MORE_INFORMATION_BIT_MASK                         0x20
#define COMMAND_ZIP_PACKET_PROPERTIES2_Z_WAVE_CMD_INCLUDED_BIT_MASK                      0x40
#define COMMAND_ZIP_PACKET_PROPERTIES2_HEADER_EXT_INCLUDED_BIT_MASK                      0x80
#define COMMAND_ZIP_PACKET_PROPERTIES3_SOURCE_END_POINT_MASK                             0x7F
#define COMMAND_ZIP_PACKET_PROPERTIES3_RES_BIT_MASK                                      0x80
#define COMMAND_ZIP_PACKET_PROPERTIES4_DESTINATION_END_POINT_MASK                        0x7F
#define COMMAND_ZIP_PACKET_PROPERTIES4_BIT_ADDRESS_BIT_MASK                              0x80

/* Application Capability command class commands */
#define APPLICATION_CAPABILITY_VERSION                                                   0x01
#define COMMAND_COMMAND_CLASS_NOT_SUPPORTED                                              0x01
/* Values used for Command Command Class Not Supported command */
#define COMMAND_COMMAND_CLASS_NOT_SUPPORTED_PROPERTIES1_RESERVED_MASK                    0x7F
#define COMMAND_COMMAND_CLASS_NOT_SUPPORTED_PROPERTIES1_DYNAMIC_BIT_MASK                 0x80

/* Color Control command class commands */
#define COLOR_CONTROL_VERSION                                                            0x01
#define CAPABILITY_GET                                                                   0x01
#define CAPABILITY_REPORT                                                                0x02
#define STATE_GET                                                                        0x03
#define STATE_REPORT                                                                     0x04
#define STATE_SET                                                                        0x05
#define START_CAPABILITY_LEVEL_CHANGE                                                    0x06
#define STOP_STATE_CHANGE                                                                0x07
/* Values used for State Set command */
#define STATE_SET_PROPERTIES1_STATE_DATA_LENGTH_MASK                                     0x1F
#define STATE_SET_PROPERTIES1_RESERVED_MASK                                              0xE0
#define STATE_SET_PROPERTIES1_RESERVED_SHIFT                                             0x05
/* Values used for Start Capability Level Change command */
#define START_CAPABILITY_LEVEL_CHANGE_PROPERTIES1_RES1_MASK                              0x1F
#define START_CAPABILITY_LEVEL_CHANGE_PROPERTIES1_IGNORE_START_STATE_BIT_MASK            0x20
#define START_CAPABILITY_LEVEL_CHANGE_PROPERTIES1_UP_DOWN_BIT_MASK                       0x40
#define START_CAPABILITY_LEVEL_CHANGE_PROPERTIES1_RES2_BIT_MASK                          0x80

/* Schedule command class commands */
#define SCHEDULE_VERSION                                                                 0x01
#define SCHEDULE_SUPPORTED_GET                                                           0x01
#define SCHEDULE_SUPPORTED_REPORT                                                        0x02
#define COMMAND_SCHEDULE_SET                                                             0x03
#define COMMAND_SCHEDULE_GET                                                             0x04
#define COMMAND_SCHEDULE_REPORT                                                          0x05
#define SCHEDULE_REMOVE                                                                  0x06
#define SCHEDULE_STATE_SET                                                               0x07
#define SCHEDULE_STATE_GET                                                               0x08
#define SCHEDULE_STATE_REPORT                                                            0x09
/* Values used for Schedule Supported Report command */
#define SCHEDULE_SUPPORTED_REPORT_PROPERTIES1_START_TIME_SUPPORT_MASK                    0x3F
#define SCHEDULE_SUPPORTED_REPORT_PROPERTIES1_FALLBACK_SUPPORT_BIT_MASK                  0x40
#define SCHEDULE_SUPPORTED_REPORT_PROPERTIES1_SUPPORT_ENABLE_DISABLE_BIT_MASK            0x80
#define SCHEDULE_SUPPORTED_REPORT_PROPERTIES3_SUPPORTED_OVERRIDE_TYPES_MASK              0x7F
#define SCHEDULE_SUPPORTED_REPORT_PROPERTIES3_OVERRIDE_SUPPORT_BIT_MASK                  0x80
/* Values used for Command Schedule Set command */
#define COMMAND_SCHEDULE_SET_PROPERTIES1_START_MONTH_MASK                                0x0F
#define COMMAND_SCHEDULE_SET_PROPERTIES1_RESERVED1_MASK                                  0xF0
#define COMMAND_SCHEDULE_SET_PROPERTIES1_RESERVED1_SHIFT                                 0x04
#define COMMAND_SCHEDULE_SET_PROPERTIES2_START_DAY_OF_MONTH_MASK                         0x1F
#define COMMAND_SCHEDULE_SET_PROPERTIES2_RESERVED2_MASK                                  0xE0
#define COMMAND_SCHEDULE_SET_PROPERTIES2_RESERVED2_SHIFT                                 0x05
#define COMMAND_SCHEDULE_SET_PROPERTIES3_START_WEEKDAY_MASK                              0x7F
#define COMMAND_SCHEDULE_SET_PROPERTIES3_RES_BIT_MASK                                    0x80
#define COMMAND_SCHEDULE_SET_PROPERTIES4_START_HOUR_MASK                                 0x1F
#define COMMAND_SCHEDULE_SET_PROPERTIES4_DURATION_TYPE_MASK                              0xE0
#define COMMAND_SCHEDULE_SET_PROPERTIES4_DURATION_TYPE_SHIFT                             0x05
#define COMMAND_SCHEDULE_SET_PROPERTIES5_START_MINUTE_MASK                               0x3F
#define COMMAND_SCHEDULE_SET_PROPERTIES5_RESERVED3_MASK                                  0xC0
#define COMMAND_SCHEDULE_SET_PROPERTIES5_RESERVED3_SHIFT                                 0x06
/* Values used for Command Schedule Report command */
#define COMMAND_SCHEDULE_REPORT_PROPERTIES1_START_MONTH_MASK                             0x0F
#define COMMAND_SCHEDULE_REPORT_PROPERTIES1_ACTIVE_ID_MASK                               0xF0
#define COMMAND_SCHEDULE_REPORT_PROPERTIES1_ACTIVE_ID_SHIFT                              0x04
#define COMMAND_SCHEDULE_REPORT_PROPERTIES2_START_DAY_OF_MONTH_MASK                      0x1F
#define COMMAND_SCHEDULE_REPORT_PROPERTIES2_RESERVED2_MASK                               0xE0
#define COMMAND_SCHEDULE_REPORT_PROPERTIES2_RESERVED2_SHIFT                              0x05
#define COMMAND_SCHEDULE_REPORT_PROPERTIES3_START_WEEKDAY_MASK                           0x7F
#define COMMAND_SCHEDULE_REPORT_PROPERTIES3_RES_BIT_MASK                                 0x80
#define COMMAND_SCHEDULE_REPORT_PROPERTIES4_START_HOUR_MASK                              0x1F
#define COMMAND_SCHEDULE_REPORT_PROPERTIES4_DURATION_TYPE_MASK                           0xE0
#define COMMAND_SCHEDULE_REPORT_PROPERTIES4_DURATION_TYPE_SHIFT                          0x05
#define COMMAND_SCHEDULE_REPORT_PROPERTIES5_START_MINUTE_MASK                            0x3F
#define COMMAND_SCHEDULE_REPORT_PROPERTIES5_RESERVED3_MASK                               0xC0
#define COMMAND_SCHEDULE_REPORT_PROPERTIES5_RESERVED3_SHIFT                              0x06
/* Values used for Schedule State Report command */
#define SCHEDULE_STATE_REPORT_PROPERTIES1_OVERRIDE_BIT_MASK                              0x01
#define SCHEDULE_STATE_REPORT_PROPERTIES1_REPORTS_TO_FOLLOW_MASK                         0xFE
#define SCHEDULE_STATE_REPORT_PROPERTIES1_REPORTS_TO_FOLLOW_SHIFT                        0x01
#define SCHEDULE_STATE_REPORT_PROPERTIES2_ACTIVE_ID_1_MASK                               0x0F
#define SCHEDULE_STATE_REPORT_PROPERTIES2_ACTIVE_ID_2_MASK                               0xF0
#define SCHEDULE_STATE_REPORT_PROPERTIES2_ACTIVE_ID_2_SHIFT                              0x04
#define SCHEDULE_STATE_REPORT_PROPERTIES3_ACTIVE_ID_3_MASK                               0x0F
#define SCHEDULE_STATE_REPORT_PROPERTIES3_ACTIVE_ID_N_MASK                               0xF0
#define SCHEDULE_STATE_REPORT_PROPERTIES3_ACTIVE_ID_N_SHIFT                              0x04

/* Network Management Primary command class commands */
#define NETWORK_MANAGEMENT_PRIMARY_VERSION                                               0x01
#define CONTROLLER_CHANGE                                                                0x01
#define CONTROLLER_CHANGE_STATUS                                                         0x02
/* Values used for Controller Change Status command */
#define CONTROLLER_CHANGE_STATUS_PROPERTIES1_CAPABILITY_MASK                             0x7F
#define CONTROLLER_CHANGE_STATUS_PROPERTIES1_LISTENING_BIT_MASK                          0x80
#define CONTROLLER_CHANGE_STATUS_PROPERTIES2_SECURITY_MASK                               0x7F
#define CONTROLLER_CHANGE_STATUS_PROPERTIES2_OPT_BIT_MASK                                0x80

/* Zip Nd command class commands */
#define ZIP_ND_VERSION                                                                   0x01
#define ZIP_NODE_SOLICITATION                                                            0x03
#define ZIP_INV_NODE_SOLICITATION                                                        0x04
#define ZIP_NODE_ADVERTISEMENT                                                           0x01
/* Values used for Zip Inv Node Solicitation command */
#define ZIP_INV_NODE_SOLICITATION_PROPERTIES1_RESERVED1_MASK                             0x03
#define ZIP_INV_NODE_SOLICITATION_PROPERTIES1_LOCAL_BIT_MASK                             0x04
#define ZIP_INV_NODE_SOLICITATION_PROPERTIES1_RESERVED2_MASK                             0xF8
#define ZIP_INV_NODE_SOLICITATION_PROPERTIES1_RESERVED2_SHIFT                            0x03
/* Values used for Zip Node Advertisement command */
#define ZIP_NODE_ADVERTISEMENT_PROPERTIES1_VALIDITY_MASK                                 0x03
#define ZIP_NODE_ADVERTISEMENT_VALIDITY_INFORMATION_OK                                   0x00
#define ZIP_NODE_ADVERTISEMENT_VALIDITY_INFORMATION_OBSOLETE                             0x01
#define ZIP_NODE_ADVERTISEMENT_VALIDITY_INFORMATION_NOT_FOUND                            0x02
#define ZIP_NODE_ADVERTISEMENT_PROPERTIES1_LOCAL_BIT_MASK                                0x04
#define ZIP_NODE_ADVERTISEMENT_PROPERTIES1_RESERVED_MASK                                 0xF8
#define ZIP_NODE_ADVERTISEMENT_PROPERTIES1_RESERVED_SHIFT                                0x03

/* Association Grp Info command class commands */
#define ASSOCIATION_GRP_INFO_VERSION                                                     0x01
#define ASSOCIATION_GROUP_NAME_GET                                                       0x01
#define ASSOCIATION_GROUP_NAME_REPORT                                                    0x02
#define ASSOCIATION_GROUP_INFO_GET                                                       0x03
#define ASSOCIATION_GROUP_INFO_REPORT                                                    0x04
#define ASSOCIATION_GROUP_COMMAND_LIST_GET                                               0x05
#define ASSOCIATION_GROUP_COMMAND_LIST_REPORT                                            0x06
/* Values used for Association Group Info Get command */
#define ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_RESERVED_MASK                             0x3F
#define ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_LIST_MODE_BIT_MASK                        0x40
#define ASSOCIATION_GROUP_INFO_GET_PROPERTIES1_REFRESH_CACHE_BIT_MASK                    0x80
/* Values used for Association Group Info Report command */
#define ASSOCIATION_GROUP_INFO_REPORT_PROPERTIES1_GROUP_COUNT_MASK                       0x3F
#define ASSOCIATION_GROUP_INFO_REPORT_PROPERTIES1_DYNAMIC_INFO_BIT_MASK                  0x40
#define ASSOCIATION_GROUP_INFO_REPORT_PROPERTIES1_LIST_MODE_BIT_MASK                     0x80
/* Values used for Association Group Command List Get command */
#define ASSOCIATION_GROUP_COMMAND_LIST_GET_PROPERTIES1_RESERVED_MASK                     0x7F
#define ASSOCIATION_GROUP_COMMAND_LIST_GET_PROPERTIES1_ALLOW_CACHE_BIT_MASK              0x80

/* Device Reset Locally command class commands */
#define DEVICE_RESET_LOCALLY_VERSION                                                     0x01
#define DEVICE_RESET_LOCALLY_NOTIFICATION                                                0x01

/* Central Scene command class commands */
#define CENTRAL_SCENE_VERSION                                                            0x01
#define CENTRAL_SCENE_SUPPORTED_GET                                                      0x01
#define CENTRAL_SCENE_SUPPORTED_REPORT                                                   0x02
#define CENTRAL_SCENE_NOTIFICATION                                                       0x03
/* Values used for Central Scene Notification command */
#define CENTRAL_SCENE_NOTIFICATION_PROPERTIES1_KEY_ATTRIBUTES_MASK                       0x07
#define CENTRAL_SCENE_NOTIFICATION_PROPERTIES1_RESERVED_MASK                             0xF8
#define CENTRAL_SCENE_NOTIFICATION_PROPERTIES1_RESERVED_SHIFT                            0x03

/* Ip Association command class commands */
#define IP_ASSOCIATION_VERSION                                                           0x01
#define IP_ASSOCIATION_SET                                                               0x01
#define IP_ASSOCIATION_GET                                                               0x02
#define IP_ASSOCIATION_REPORT                                                            0x03
#define IP_ASSOCIATION_REMOVE                                                            0x04
/* Values used for Ip Association Set command */
#define IP_ASSOCIATION_SET_PROPERTIES1_RESOURCE_NAME_LENGTH_MASK                         0x3F
#define IP_ASSOCIATION_SET_PROPERTIES1_RESERVED_MASK                                     0xC0
#define IP_ASSOCIATION_SET_PROPERTIES1_RESERVED_SHIFT                                    0x06
/* Values used for Ip Association Report command */
#define IP_ASSOCIATION_REPORT_PROPERTIES1_RESOURCE_NAME_LENGTH_MASK                      0x3F
#define IP_ASSOCIATION_REPORT_PROPERTIES1_RESERVED_MASK                                  0xC0
#define IP_ASSOCIATION_REPORT_PROPERTIES1_RESERVED_SHIFT                                 0x06
/* Values used for Ip Association Remove command */
#define IP_ASSOCIATION_REMOVE_PROPERTIES1_RESOURCE_NAME_LENGTH_MASK                      0x3F
#define IP_ASSOCIATION_REMOVE_PROPERTIES1_RESERVED_MASK                                  0xC0
#define IP_ASSOCIATION_REMOVE_PROPERTIES1_RESERVED_SHIFT                                 0x06

/* Antitheft command class commands */
#define ANTITHEFT_VERSION                                                                0x01
#define ANTITHEFT_SET                                                                    0x01
#define ANTITHEFT_GET                                                                    0x02
#define ANTITHEFT_REPORT                                                                 0x03
/* Values used for Antitheft Set command */
#define ANTITHEFT_SET_PROPERTIES1_NUMBER_OF_MAGIC_CODE_BYTES_MASK                        0x7F
#define ANTITHEFT_SET_PROPERTIES1_ENABLE_BIT_MASK                                        0x80

/* Antitheft command class commands */
#define ANTITHEFT_VERSION_V2                                                             0x02
#define ANTITHEFT_SET_V2                                                                 0x01
#define ANTITHEFT_GET_V2                                                                 0x02
#define ANTITHEFT_REPORT_V2                                                              0x03
/* Values used for Antitheft Set command */
#define ANTITHEFT_SET_PROPERTIES1_NUMBER_OF_MAGIC_CODE_BYTES_MASK_V2                     0x7F
#define ANTITHEFT_SET_PROPERTIES1_ENABLE_BIT_MASK_V2                                     0x80

/* Zwaveplus Info command class commands */
#define ZWAVEPLUS_INFO_VERSION                                                           0x01
#define ZWAVEPLUS_INFO_GET                                                               0x01
#define ZWAVEPLUS_INFO_REPORT                                                            0x02
/* Values used for Zwaveplus Info Report command */
#define ROLE_TYPE_CENTRAL_STATIC_CONTROLLER                        0x00
#define ROLE_TYPE_SUB_STATIC_CONTROLLER                            0x01
#define ROLE_TYPE_PORTABLE_CONTROLLER                              0x02
#define ROLE_TYPE_PORTABLE_REPORTING_CONTROLLER                    0x03
#define ROLE_TYPE_PORTABLE_SLAVE                                   0x04
#define ROLE_TYPE_ALWAYS_ON_SLAVE                                  0x05
#define ROLE_TYPE_SLEEPING_REPORTING_SLAVE                         0x06
#define ROLE_TYPE_REACHABLE_SLEEPING_SLAVE                         0x07
#define NODE_TYPE_ZWAVEPLUS_NODE                                   0x00
#define NODE_TYPE_ZWAVEPLUS_FOR_IP_ROUTER                          0x01
#define NODE_TYPE_ZWAVEPLUS_FOR_IP_GATEWAY                         0x02
#define NODE_TYPE_ZWAVEPLUS_FOR_IP_CLIENT_IP_NODE                  0x03
#define NODE_TYPE_ZWAVEPLUS_FOR_IP_CLIENT_ZWAVE_NODE               0x04

/* Zwaveplus Info command class commands */
#define ZWAVEPLUS_INFO_VERSION_V2                                                        0x02
#define ZWAVEPLUS_INFO_GET_V2                                                            0x01
#define ZWAVEPLUS_INFO_REPORT_V2                                                         0x02
/* Values used for Zwaveplus Info Report command */
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_CENTRAL_STATIC_CONTROLLER_V2                     0x00
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_SUB_STATIC_CONTROLLER_V2                         0x01
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_PORTABLE_CONTROLLER_V2                           0x02
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_PORTABLE_REPORTING_CONTROLLER_V2                 0x03
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_PORTABLE_SLAVE_V2                                0x04
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_ALWAYS_ON_SLAVE_V2                               0x05
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_SLEEPING_REPORTING_SLAVE_V2                      0x06
#define ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_REACHABLE_SLEEPING_SLAVE_V2                      0x07
#define ZWAVEPLUS_INFO_REPORT_NODE_TYPE_ZWAVEPLUS_NODE_V2                                0x00
#define ZWAVEPLUS_INFO_REPORT_NODE_TYPE_ZWAVEPLUS_FOR_IP_GATEWAY_V2                      0x02

/* Zip Gateway command class commands */
#define ZIP_GATEWAY_VERSION                                                              0x01
#define GATEWAY_MODE_SET                                                                 0x01
#define GATEWAY_MODE_GET                                                                 0x02
#define GATEWAY_MODE_REPORT                                                              0x03
#define GATEWAY_PEER_SET                                                                 0x04
#define GATEWAY_PEER_GET                                                                 0x05
#define GATEWAY_PEER_REPORT                                                              0x06
#define GATEWAY_LOCK_SET                                                                 0x07
#define UNSOLICITED_DESTINATION_SET                                                      0x08
#define UNSOLICITED_DESTINATION_GET                                                      0x09
#define UNSOLICITED_DESTINATION_REPORT                                                   0x0A
#define COMMAND_APPLICATION_NODE_INFO_SET                                                0x0B
#define COMMAND_APPLICATION_NODE_INFO_GET                                                0x0C
#define COMMAND_APPLICATION_NODE_INFO_REPORT                                             0x0D
/* Values used for Gateway Mode Set command */
#define GATEWAY_MODE_SET_STAND_ALONE                                                     0x01
#define GATEWAY_MODE_SET_PORTAL                                                          0x02
/* Values used for Gateway Mode Report command */
#define GATEWAY_MODE_REPORT_STAND_ALONE                                                  0x01
#define GATEWAY_MODE_REPORT_PORTAL                                                       0x02
/* Values used for Gateway Peer Set command */
#define GATEWAY_PEER_SET_PROPERTIES1_PEER_NAME_LENGTH_MASK                               0x3F
#define GATEWAY_PEER_SET_PROPERTIES1_RESERVED_MASK                                       0xC0
#define GATEWAY_PEER_SET_PROPERTIES1_RESERVED_SHIFT                                      0x06
/* Values used for Gateway Peer Report command */
#define GATEWAY_PEER_REPORT_PROPERTIES1_PEER_NAME_LENGTH_MASK                            0x3F
#define GATEWAY_PEER_REPORT_PROPERTIES1_RESERVED_MASK                                    0xC0
#define GATEWAY_PEER_REPORT_PROPERTIES1_RESERVED_SHIFT                                   0x06
/* Values used for Gateway Lock Set command */
#define GATEWAY_LOCK_SET_PROPERTIES1_LOCK_BIT_MASK                                       0x01
#define GATEWAY_LOCK_SET_PROPERTIES1_SHOW_BIT_MASK                                       0x02
#define GATEWAY_LOCK_SET_PROPERTIES1_RESERVED_MASK                                       0xFC
#define GATEWAY_LOCK_SET_PROPERTIES1_RESERVED_SHIFT                                      0x02

/* Zip Portal command class commands */
#define ZIP_PORTAL_VERSION                                                               0x01
#define GATEWAY_CONFIGURATION_SET                                                        0x01
#define GATEWAY_CONFIGURATION_STATUS                                                     0x02
#define GATEWAY_CONFIGURATION_GET                                                        0x03
#define GATEWAY_CONFIGURATION_REPORT                                                     0x04
#define GATEWAY_CONFIGURATION_UNREGISTER                                                 0x05   //Testing

/* Appliance command class commands */
#define APPLIANCE_VERSION                                                                0x01
#define APPLIANCE_TYPE_GET                                                               0x01
#define APPLIANCE_TYPE_REPORT                                                            0x02
#define APPLIANCE_PROGRAM_SUPPORTED_GET                                                  0x03
#define APPLIANCE_PROGRAM_SUPPORTED_REPORT                                               0x04
#define APPLIANCE_SET                                                                    0x05
#define APPLIANCE_GET                                                                    0x06
#define APPLIANCE_REPORT                                                                 0x07
/* Values used for Appliance Type Report command */
#define APPLIANCE_TYPE_REPORT_PROPERTIES1_APPLIANCE_TYPE_MASK                            0x3F
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_RESERVED                                    0x00
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_REFRIGERATOR                                0x01
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_FREEZER                                     0x02
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_MICROWAVE_OVEN                              0x03
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_LIGHT_WAVE_OVEN                             0x04
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_OVEN                                        0x05
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_DISHWASHER                                  0x06
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_WASHING_MACHINE                             0x07
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_DRYING_MACHINE                              0x08
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_DRYING_CABINET                              0x09
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_COMBO_WASHER_DRYER                          0x0A
#define APPLIANCE_TYPE_REPORT_APPLIANCE_TYPE_ROBOVAC                                     0x0B
#define APPLIANCE_TYPE_REPORT_PROPERTIES1_RESERVED_MASK                                  0xC0
#define APPLIANCE_TYPE_REPORT_PROPERTIES1_RESERVED_SHIFT                                 0x06
/* Values used for Appliance Set command */
#define APPLIANCE_SET_PROPERTIES1_APPLIANCE_MODE_MASK                                    0x0F
#define APPLIANCE_SET_APPLIANCE_MODE_OFF                                                 0x00
#define APPLIANCE_SET_APPLIANCE_MODE_ON                                                  0x01
#define APPLIANCE_SET_APPLIANCE_MODE_ECO                                                 0x02
#define APPLIANCE_SET_APPLIANCE_MODE_AWAY                                                0x03
#define APPLIANCE_SET_APPLIANCE_MODE_START                                               0x04
#define APPLIANCE_SET_APPLIANCE_MODE_STOP                                                0x05
#define APPLIANCE_SET_APPLIANCE_MODE_CANCEL                                              0x06
#define APPLIANCE_SET_APPLIANCE_MODE_CHARGE                                              0x07
#define APPLIANCE_SET_PROPERTIES1_NO_OF_MANUFACTURER_DATA_FIELDS_MASK                    0xF0
#define APPLIANCE_SET_PROPERTIES1_NO_OF_MANUFACTURER_DATA_FIELDS_SHIFT                   0x04
/* Values used for Appliance Report command */
#define APPLIANCE_REPORT_PROPERTIES1_APPLIANCE_MODE_MASK                                 0x0F
#define APPLIANCE_REPORT_APPLIANCE_MODE_OFF                                              0x00
#define APPLIANCE_REPORT_APPLIANCE_MODE_ON                                               0x01
#define APPLIANCE_REPORT_APPLIANCE_MODE_ECO                                              0x02
#define APPLIANCE_REPORT_APPLIANCE_MODE_AWAY                                             0x03
#define APPLIANCE_REPORT_APPLIANCE_MODE_START                                            0x04
#define APPLIANCE_REPORT_APPLIANCE_MODE_STOP                                             0x05
#define APPLIANCE_REPORT_APPLIANCE_MODE_CANCEL                                           0x06
#define APPLIANCE_REPORT_APPLIANCE_MODE_CHARGE                                           0x07
#define APPLIANCE_REPORT_PROPERTIES1_NO_OF_MANUFACTURER_DATA_FIELDS_MASK                 0xF0
#define APPLIANCE_REPORT_PROPERTIES1_NO_OF_MANUFACTURER_DATA_FIELDS_SHIFT                0x04

/* Dmx command class commands */
#define DMX_VERSION                                                                      0x01
#define DMX_ADDRESS_SET                                                                  0x01
#define DMX_ADDRESS_GET                                                                  0x02
#define DMX_ADDRESS_REPORT                                                               0x03
#define DMX_CAPABILITY_GET                                                               0x04
#define DMX_CAPABILITY_REPORT                                                            0x05
#define DMX_DATA_40                                                                      0x06
/* Values used for Dmx Address Set command */
#define DMX_ADDRESS_SET_PROPERTIES1_PAGE_ID_MASK                                         0x0F
#define DMX_ADDRESS_SET_PROPERTIES1_RESERVED_MASK                                        0xF0
#define DMX_ADDRESS_SET_PROPERTIES1_RESERVED_SHIFT                                       0x04
/* Values used for Dmx Address Report command */
#define DMX_ADDRESS_REPORT_PROPERTIES1_PAGE_ID_MASK                                      0x0F
#define DMX_ADDRESS_REPORT_PROPERTIES1_RESERVED_MASK                                     0xF0
#define DMX_ADDRESS_REPORT_PROPERTIES1_RESERVED_SHIFT                                    0x04
/* Values used for Dmx Data 40 command */
#define DMX_DATA_40_PROPERTIES1_PAGE_MASK                                                0x0F
#define DMX_DATA_40_PROPERTIES1_SEQUENCE_NO_MASK                                         0x30
#define DMX_DATA_40_PROPERTIES1_SEQUENCE_NO_SHIFT                                        0x04
#define DMX_DATA_40_PROPERTIES1_RESERVED_MASK                                            0xC0
#define DMX_DATA_40_PROPERTIES1_RESERVED_SHIFT                                           0x06



/* Max. frame size to allow routing over 4 hops */
#define META_DATA_MAX_DATA_SIZE                      48



#endif
