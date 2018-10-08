/**
@file   zw_api_util.h - Z-wave High Level API utility/miscellaneous functions header file.

        To implement some utility/miscellaneous functions.

@author David Chow

@version    1.0 11-1-2011  Initial release

node info file version: 0.1
comments: Initial release

node info file version: 0.2
comments: -Added ZW_SUBTAG_CATEGORY, ZW_SUBTAG_EXT_VER_CAP, ZW_SUBTAG_HW_VER, ZW_SUBTAG_FW_VER, ZW_SUBTAG_ZWPLUS_VER,
           ZW_SUBTAG_ROLE_TYPE, ZW_SUBTAG_NODE_TYPE and ZW_SUBTAG_AGI_XXX.
          -When the bit-7 of subtag length field is set.  The length field is ignored, the next two
           bytes become the length field.
          -Changed MAX_SUBTAGS_SIZE  to  1200
          -Added storage of Association Group Information interface data.

node info file version: 0.3
comments: -Added ZW_SUBTAG_DEV_ID_TYPE and ZW_SUBTAG_GRP_NODE

node info file version: 0.4
comments: -Added ZW_SUBTAG_INSTR_ICON and ZW_SUBTAG_USER_ICON
          -Moved ZW_SUBTAG_ZWPLUS_VER, ZW_SUBTAG_ROLE_TYPE and ZW_SUBTAG_NODE_TYPE to endpoint tags.

node info file version: 0.5
comments: -Data structure for COMMAND_CLASS_ALARM has changed


*/

#ifndef _ZW_API_UTIL_DAVID_
#define _ZW_API_UTIL_DAVID_
/**
@defgroup Util Utility/Miscellaneous Functions
Some utility/miscellaneous functions.
@{
*/

#define FILE_MAGIC_NUM_H  0x7E      /**< High byte of magic identifier of the node information file*/
#define FILE_MAGIC_NUM_L  0xC3      /**< Low byte of magic identifier of the node information file*/

#define FILE_VER_NUM_H    0         /**< High byte of node information file version*/
#define FILE_VER_NUM_L    0x05      /**< Low byte of node information file version*/

#define NI_FILE_HDR_LEN     4       /**<  Node information file header length*/

/** Tag size definition */
#define ZW_TAG_ID_SIZE      2       /**< Tag identifier field size in bytes*/
#define ZW_TAG_LEN_SIZE     2       /**< Tag length field size in bytes*/
#define ZW_SUBTAG_ID_SIZE   1       /**< Sub-tag identifier field size in bytes*/
#define ZW_SUBTAG_LEN_SIZE  1       /**< Sub-tag length field size in bytes*/
#define MAX_SUBTAGS_SIZE    1200    /**< Maximum subtags size, used for pre-allocate memory */


/** Tag identifier definition */
#define ZW_TAG_NODE      0xA701     /**< Node tag id*/
#define ZW_TAG_EP        0xA702     /**< End point tag id*/
#define ZW_TAG_IF        0xA703     /**< Interface tag id*/
#define ZW_TAG_NW        0xA704     /**< Network tag id*/
#define ZW_TAG_SEC       0xA705     /**< Security tag id*/


/** Node sub-tag identifier definition */
#define ZW_SUBTAG_NODE_ID       0       /**< Node ID sub-tag id*/
#define ZW_SUBTAG_VID           1       /**< Vendor ID sub-tag id*/
#define ZW_SUBTAG_VTYPE         2       /**< Vendor Product Type sub-tag id*/
#define ZW_SUBTAG_PID           3       /**< Product ID sub-tag id*/
#define ZW_SUBTAG_NAME          4       /**< DEPRECATED:User configured name string of device sub-tag id*/
#define ZW_SUBTAG_LOC           5       /**< DEPRECATED:User configured location string of device sub-tag id*/
#define ZW_SUBTAG_MULCH_VER     6       /**< Multi-channel version sub-tag id*/
#define ZW_SUBTAG_EP_CNT        7       /**< Number of end points sub-tag id*/
#define ZW_SUBTAG_SLEEP_CAP     8       /**< Flag to indicate the node is capable to sleep sub-tag id*/
#define ZW_SUBTAG_SND_SEQ_N     9       /**< 4-bit sequence number in sending split secure packet*/
#define ZW_SUBTAG_SEC_INC_FAILED 10     /**< Flag to mark a node who failed the inclusion process.
                                             1=failed, 0=success*/
#define ZW_SUBTAG_CRC16_CAP         11       /**< Flag to indicate the node is capable to check/generate CRC-16 checksum*/
#define ZW_SUBTAG_DEV_ID_FMT        12       /**< Device id format*/
#define ZW_SUBTAG_DEV_ID_LEN        13       /**< Device id length*/
#define ZW_SUBTAG_DEV_ID_DAT        14       /**< Device id */
#define ZW_SUBTAG_LIB_TYPE          15       /**< Z-Wave Library Type sub-tag id*/
#define ZW_SUBTAG_PROT_VER          16       /**< Z-Wave Protocol Version sub-tag id*/
#define ZW_SUBTAG_APPL_VER          17       /**< Application Version sub-tag id*/
#define ZW_SUBTAG_MUL_CMD_CAP       18       /**< Flag to indicate the node is capable of multi command encapsulation*/
#define ZW_SUBTAG_CATEGORY          19       /**< Device category*/
#define ZW_SUBTAG_EXT_VER_CAP       20       /**< Flag to indicate the node supports extended version information
                                                  and firmware versions count*/
#define ZW_SUBTAG_HW_VER            21       /**< Hardware version*/
#define ZW_SUBTAG_FW_VER            22       /**< Firmware versions*/
#define ZW_SUBTAG_UNUSED_1          23       /**< Unused*/
#define ZW_SUBTAG_UNUSED_2          24       /**< Unused*/
#define ZW_SUBTAG_UNUSED_3          25       /**< Unused*/
#define ZW_SUBTAG_DEV_ID_TYPE       26       /**< Device ID type */


/** End point sub-tag identifier definition */
#define ZW_SUBTAG_EP_ID             0       /**< End point ID sub-tag id*/
#define ZW_SUBTAG_GEN               1       /**< Generic Device Class sub-tag id*/
#define ZW_SUBTAG_SPEC              2       /**< Specific Device Class sub-tag id*/
#define ZW_SUBTAG_EP_NAME           3       /**< User configured name string of endpoint sub-tag id*/
#define ZW_SUBTAG_EP_LOC            4       /**< User configured location string of endpoint sub-tag id*/
#define ZW_SUBTAG_ZWPLUS_VER        5       /**< Z-Wave+ version*/
#define ZW_SUBTAG_ROLE_TYPE         6       /**< Z-Wave+ role type*/
#define ZW_SUBTAG_NODE_TYPE         7       /**< Z-Wave+ node type*/
#define ZW_SUBTAG_INSTR_ICON        8       /**< Z-Wave+ installer icon type*/
#define ZW_SUBTAG_USER_ICON         9       /**< Z-Wave+ user icon type*/



/** Interface sub-tag identifier definition */
#define ZW_SUBTAG_CLS               0       /**< Command class sub-tag id*/
#define ZW_SUBTAG_VER               1       /**< Command class version sub-tag id*/
#define ZW_SUBTAG_PROPTY            2       /**< Command class properties sub-tag id*/
#define ZW_SUBTAG_DAT_CNT           3       /**< Number of data items*/
#define ZW_SUBTAG_DAT_ITEM_SZ       4       /**< Data item size in bytes*/
#define ZW_SUBTAG_DATA              5       /**< Data items which is specific to the command class*/
#define ZW_SUBTAG_AGI_GRP_CNT       6       /**< Association group information: number of groups*/
#define ZW_SUBTAG_AGI_GRP_DYNAMIC   7       /**< Association group information: flag to indicate the group info is dynamic*/
#define ZW_SUBTAG_AGI_GRP_ID        8       /**< Association group information: group id*/
#define ZW_SUBTAG_AGI_CMD_CNT       9       /**< Association group information: number of entries in command list*/
#define ZW_SUBTAG_AGI_PROFILE       10      /**< Association group information: profile*/
#define ZW_SUBTAG_AGI_EVENT         11      /**< Association group information: event code*/
#define ZW_SUBTAG_AGI_GRP_NAME      12      /**< Association group information: group name*/
#define ZW_SUBTAG_AGI_CMD_LIST      13      /**< Association group information: command list*/


/** Network sub-tag identifier definition */
#define ZW_SUBTAG_HOME_ID   0       /**< Home ID sub-tag id*/
#define ZW_SUBTAG_GRP_NODE  1       /**< Group 1 node sub-tag id*/

/** Security sub-tag identifier definition */
#define ZW_SUBTAG_NW_KEY    0       /**< Network key sub-tag id*/
#define ZW_SUBTAG_SCHEME    1       /**< Inherited security scheme sub-tag id*/

/** Tag container */
typedef struct
{
    uint16_t    id;		    /**< Tag id */
    uint16_t    len;	    /**< The length of the data[] */
    uint8_t     data[1];    /**< Data storage place holder */
}
zwtag_t, *zwtag_p;


int zwutl_ni_load(zwnet_p nw, const char *ni_file);
/**<
load node information from file
@param[in]	nw	        network
@param[in]	ni_file	    node information file
@return         ZW_ERR_XXX.
*/


int zwutl_ni_save(zwnet_p nw, const char *ni_file);
/**<
save node information into file
@param[in]	nw	        network
@param[in]	ni_file	    node information file
@return         ZW_ERR_XXX.
*/

#define POLY        0x1021      /**< crc-ccitt mask */
#define CRC_INIT    0x1D0F      /**< crc-ccitt initialization value */


#define     CRC16_OVERHEAD      4   /**< CRC-16 Encapsulation Command Class overhead */

uint16_t zwutl_crc16_chk(uint16_t crc, uint8_t *data, uint16_t len);
/**<
CRC-CCITT (0x1D0F) calculation / check
@param[in]	crc	        CRC initialization value
@param[in]	data	    Buffer that store the byte string
@param[in]	len	        Length of byte string
@return         If data contains the 16-bit checksum field: return zero if check o.k.
                If data excludes the 16-bit checksum field: return CRC-16 checksum.
*/

#ifdef FLASH_PROG_API
#include "zw_hci_fl_prog.h"

#define  FL_SECTOR_SIZE     2048                //Flash sector size
#define  FL_NVR_SZ          128                 //NVR size to allocate
#define  FL_SAVED_NVR_SZ    (0x7F - 0x09 + 1)   //Saved NVR size

int zwutl_fl_fw_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t sector_cnt, const char *fw_file);
int zwutl_fl_fw_wr(flash_prog_ctx_t *fl_prog_ctx, const char *fw_file);
#endif

/**
@}
*/



#endif /* _ZW_API_UTIL_DAVID_ */

