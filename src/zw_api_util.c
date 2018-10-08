/**
@file   zw_api_util.c - Z-wave High Level API utility/miscellaneous functions implementation.


@author David Chow

@version    1.0 11-1-2011  Initial release

version: 1.0
comments: Initial release
*/

#include "../include/zw_api_pte.h"
#include "../include/zw_security.h"
#include "../include/zw_api_util.h"

/**
@defgroup Util Utility/Miscellaneous Functions
Some utility/miscellaneous functions.
@{
*/


/**
zwutl_ni_file_new - Create a new node info file
@param[in]      file_name     file name of the new file
@return		Non-zero on success; otherwise NULL.
@post       Caller must close the file handler if the function returns non-zero file handler.
*/
static FILE *zwutl_ni_file_new(const char *file_name)
{
    FILE *file;

    if (!file_name)
    {
        return NULL;
    }

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, file_name, "w+b") != 0)
    {
        return NULL;
    }
#else
    file = fopen(file_name, "w+b");
    if (!file)
    {
        return NULL;
    }
#endif

    if (file)
    {   //Write the file magic identifier and version
        uint8_t     file_hdr[4] = {FILE_MAGIC_NUM_H, FILE_MAGIC_NUM_L, FILE_VER_NUM_H, FILE_VER_NUM_L};
        size_t      wr_count;

        wr_count = fwrite(file_hdr, 1, 4, file);
        if (wr_count == 4)
        {
            return file;
        }
        fclose(file);
    }
    return NULL;
}


/**
zwutl_fwrite16 - Write 16-bit data to file in big endian format
@param[in]      file     file
@param[in]      data     16-bit data to be written
@return		Non-zero on success; otherwise zero.
*/
static int zwutl_fwrite16(FILE *file, uint16_t data)
{
    //Write MSB
    if (fputc((int)(data >> 8), file) != EOF )
    {
        //Write LSB
        if (fputc((int)(data & 0xFF), file) != EOF )
        {
            return 1;
        }
    }

    return 0;
}


/**
zwutl_fread16 - Read 16-bit big endian data from file
@param[in]      file     file
@param[out]     data     16-bit data read
@return		Non-zero on success; otherwise zero.
*/
static int zwutl_fread16(FILE *file, uint16_t *data)
{
    int ch;

    //Get MSB
    if ((ch = fgetc(file)) != EOF )
    {
        *data = ch & 0x00FF;
        //Get LSB
        if ((ch = fgetc(file)) != EOF )
        {
            *data = (*data << 8) | (ch & 0x00FF);
            return 1;
        }
    }

    return 0;
}


/**
zwutl_tag_rd - Read a tag and its content
@param[in]      file     node info file handler
@param[out]     tag      tag container on success
@return		Zero on success; otherwise negative error number.
@post       Caller must free the memory allocated to tag on success
*/
static int zwutl_tag_rd(FILE *file, zwtag_p *tag)
{
    uint16_t    tag_id;
    uint16_t    len;
    uint16_t    padding;
    zwtag_p     tag_cont;

    if (zwutl_fread16(file, &tag_id) == 0)
    {
        return ZW_ERR_FILE_EOF;
    }

    if (zwutl_fread16(file, &len) == 0)
    {
        return ZW_ERR_FILE_EOF;
    }

    padding = len & 0x01;   //padding to even number of bytes

    //Allocate memory for the data
    tag_cont = (zwtag_p) calloc(1, sizeof(zwtag_t) + len + padding);

    if (!tag_cont)
    {
        return ZW_ERR_MEMORY;
    }

    if (fread(tag_cont->data, len + padding, 1, file) == 0)
    {
        free(tag_cont);
        return ZW_ERR_FILE_EOF;
    }

    tag_cont->id = tag_id;
    tag_cont->len = len;

    *tag = tag_cont;

    return ZW_ERR_NONE;
}


/**
zwutl_tag_wr - Write a tag and its content into a file
@param[in]      file     node info file handler
@param[in]      tag      tag container
@return		Zero on success; otherwise negative error number.
*/
static int zwutl_tag_wr(FILE *file, zwtag_p tag)
{

    if (zwutl_fwrite16(file, tag->id) == 0)
    {
        return ZW_ERR_FILE_WRITE;
    }

    if (zwutl_fwrite16(file, tag->len) == 0)
    {
        return ZW_ERR_FILE_WRITE;
    }

    if (fwrite(tag->data, tag->len, 1, file) == 0)
    {
        return ZW_ERR_FILE_WRITE;
    }

    //padding to even number of bytes
    if (tag->len & 0x01)
    {
        if (fputc(0, file) == EOF)
        {
            return ZW_ERR_FILE_WRITE;
        }
    }

    return ZW_ERR_NONE;
}


/**
zwutl_node_load - Load the node sub-tags into zwnode_t structure
@param[in]	nw	        Network
@param[in]	subtag_buf	Buffer that stores the sub-tags
@param[in]	len	        length of subtag_buf
@param[out]	curr_node   the current node as specified in sub-tag ZW_SUBTAG_NODE_ID
@return         ZW_ERR_XXX.
*/
static int zwutl_node_load(zwnet_p nw, uint8_t *subtag_buf, uint16_t len, zwnode_p *curr_node)
{
    uint8_t     *subtag;
    uint16_t    subtag_len;
    uint16_t    ext_length_hdr;
    zwnode_p    node;

    subtag = subtag_buf;

    if (len < 3)
    {
        return ZW_ERR_FILE;
    }

    //Check that first sub-tag must be ZW_SUBTAG_NODE_ID
    if ((subtag[0] != ZW_SUBTAG_NODE_ID)
        || (subtag[1] != 1))
    {
        return ZW_ERR_FILE;
    }

    //Search for the node
    node = zwnode_find(&nw->ctl, subtag[2]);
    if (!node)
    {
        return ZW_ERR_NODE_NOT_FOUND;
    }

    node->nodeid = subtag[2];

    //Point to the next sub-tag
    subtag = subtag_buf + 3;
    len -= 3;

    while (len > (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE))
    {
        if (subtag[1] < 0x80)
        {
            subtag_len = subtag[1];
            ext_length_hdr = 0;
        }
        else
        {   //The next 2 bytes become length field
            subtag_len = subtag[2];
            subtag_len = (subtag_len << 8) | subtag[3];
            ext_length_hdr = 2;
        }


        if (len < (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len))
        {
            return ZW_ERR_FILE;
        }

        switch (subtag[0])
        {
            case ZW_SUBTAG_VID:
                node->vid = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_VTYPE:
                node->vtype = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_PID:
                node->pid = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_MULCH_VER:
                node->mul_ch_ver = subtag[2];
                break;

            case ZW_SUBTAG_EP_CNT:
                node->num_of_ep = subtag[2];
                break;

            case ZW_SUBTAG_SLEEP_CAP:
                node->enable_cmd_q = node->sleep_cap = subtag[2];//enable command queuing if the node is able to sleep
                break;

            case ZW_SUBTAG_SND_SEQ_N:
                node->sec_snd_seq_num = subtag[2];
                break;

            case ZW_SUBTAG_SEC_INC_FAILED:
                node->sec_incl_failed = subtag[2];
                printf("============================>sec_incl_failed %d<======8===================\n",node->sec_incl_failed);
                break;

#ifdef CRC16_ENCAP
            case ZW_SUBTAG_CRC16_CAP:
                node->crc_cap = subtag[2];
                break;
#endif

            case ZW_SUBTAG_DEV_ID_TYPE:
                node->dev_id.type = subtag[2];
                break;

            case ZW_SUBTAG_DEV_ID_FMT:
                node->dev_id.format = subtag[2];
                break;

            case ZW_SUBTAG_DEV_ID_LEN:
                node->dev_id.len = subtag[2];
                break;

            case ZW_SUBTAG_DEV_ID_DAT:
                memset(node->dev_id.dev_id, 0, MAX_DEV_ID_LEN + 1);
                if (subtag_len > MAX_DEV_ID_LEN)
                {
                    subtag_len = MAX_DEV_ID_LEN;
                }
                memcpy(node->dev_id.dev_id, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr, subtag_len);
                break;

            case ZW_SUBTAG_LIB_TYPE:
                node->lib_type = subtag[2];
                break;

            case ZW_SUBTAG_PROT_VER:
                node->proto_ver = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_APPL_VER:
                node->app_ver = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_MUL_CMD_CAP:
                node->mul_cmd_ctl = node->mul_cmd_cap = subtag[2];    //start multi command encapsulation if the node is multi command capable
                break;

            case ZW_SUBTAG_CATEGORY:
                node->category = subtag[2];
                break;

            case ZW_SUBTAG_EXT_VER_CAP:
                if (subtag[2])
                {   //The node supports extended version information
                    node->ext_ver = (ext_ver_t *)calloc(1, sizeof(ext_ver_t) + (subtag[3]*2));
                    if (node->ext_ver)
                    {
                        node->ext_ver->fw_cnt = subtag[3];
                    }
                }
                break;

            case ZW_SUBTAG_HW_VER:
                if (node->ext_ver)
                {
                    node->ext_ver->hw_ver = subtag[2];
                }
                break;

            case ZW_SUBTAG_FW_VER:
                if (node->ext_ver)
                {
                    memcpy(node->ext_ver->fw_ver, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr,
                           subtag_len);
                }
                break;

            default:
                debug_zwapi_msg(&nw->plt_ctx, "Unknown sub-tag:%02X", subtag[0]);

        }

        //Point to the next sub-tag
        subtag += (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
        len -= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
    }

    //Copy the current node pointer
    *curr_node = node;
    return ZW_ERR_NONE;
}


/**
zwutl_ep_load - Load the end point sub-tags into zwep_t structure
@param[in]	node	    node that contains the end point
@param[in]	subtag_buf	Buffer that stores the sub-tags
@param[in]	len	        length of subtag_buf
@param[out]	curr_ep     the current end point as specified in sub-tag ZW_SUBTAG_EP_ID
@return         ZW_ERR_XXX.
*/
static int zwutl_ep_load(zwnode_p node, uint8_t *subtag_buf, uint16_t len, zwep_p *curr_ep)
{
    uint8_t     *subtag;
    uint16_t    subtag_len;
    uint16_t    ext_length_hdr;
    zwep_p      ep;

    subtag = subtag_buf;

    if (len < 3)
    {
        return ZW_ERR_FILE;
    }

    //Check that first sub-tag must be ZW_SUBTAG_EP_ID
    if ((subtag[0] != ZW_SUBTAG_EP_ID)
        || (subtag[1] != 1))
    {
        return ZW_ERR_FILE;
    }

    //Check whether this is a virtual end point
    if (subtag[2] == VIRTUAL_EP_ID)
    {   //End point has already existed
        ep = &node->ep;
    }
    else
    {   //Create new end point
        zwep_p      new_ep;

        new_ep = (zwep_p)calloc(1, sizeof(zwep_t));
        if (!new_ep)
        {
            return ZW_ERR_MEMORY;
        }
        new_ep->node = node;
        new_ep->epid = subtag[2];

        //Add to the end point list
        zwobj_add(&node->ep.obj.next, &new_ep->obj);

        ep = new_ep;
    }

    //Point to the next sub-tag
    subtag = subtag_buf + 3;
    len -= 3;

    while (len > (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE))
    {
        if (subtag[1] < 0x80)
        {
            subtag_len = subtag[1];
            ext_length_hdr = 0;
        }
        else
        {   //The next 2 bytes become length field
            subtag_len = subtag[2];
            subtag_len = (subtag_len << 8) | subtag[3];
            ext_length_hdr = 2;
        }

        if (len < (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len))
        {
            return ZW_ERR_FILE;
        }

        switch (subtag[0])
        {
            case ZW_SUBTAG_GEN:
                ep->generic = subtag[2];
                break;

            case ZW_SUBTAG_SPEC:
                ep->specific = subtag[2];
                break;

            case ZW_SUBTAG_EP_NAME:
                memcpy(ep->name, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr, subtag_len);
                ep->name[subtag_len] = '\0';
                break;

            case ZW_SUBTAG_EP_LOC:
                memcpy(ep->loc, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr, subtag_len);
                ep->loc[subtag_len] = '\0';
                break;

            case ZW_SUBTAG_ZWPLUS_VER:
                ep->zwplus_info.zwplus_ver = subtag[2];
                break;

            case ZW_SUBTAG_ROLE_TYPE:
                ep->zwplus_info.role_type = subtag[2];
                break;

            case ZW_SUBTAG_NODE_TYPE:
                ep->zwplus_info.node_type = subtag[2];
                break;

            case ZW_SUBTAG_INSTR_ICON:
                ep->zwplus_info.instr_icon = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            case ZW_SUBTAG_USER_ICON:
                ep->zwplus_info.usr_icon = ((uint16_t)subtag[2]) << 8 | subtag[3];
                break;

            default:
                debug_zwapi_msg(&node->net->plt_ctx, "Unknown sub-tag:%02X", subtag[0]);

        }

        //Point to the next sub-tag
        subtag += (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
        len -= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
    }

    //Copy the current end point pointer
    *curr_ep = ep;
    return ZW_ERR_NONE;
}


/**
zwutl_if_load - Load the interface sub-tags into zwif_t structure
@param[in]	ep	        end point that contains the interface
@param[in]	subtag_buf	Buffer that stores the sub-tags
@param[in]	len	        length of subtag_buf
@param[in]	file_ver    opened file version
@return         ZW_ERR_XXX.
*/
static int zwutl_if_load(zwep_p ep, uint8_t *subtag_buf, uint16_t len, uint16_t file_ver)
{
    zwif_p      intf;
    uint8_t     *subtag;
    uint16_t    subtag_len;
    uint16_t    ext_length_hdr;
    uint16_t    cls;
    uint8_t     ver;
    uint8_t     propty;
    uint8_t     current_grp_id;

    subtag = subtag_buf;

    //Check for the total length of class, version, security level sub-tags
    if (len < (3*2 + 4))
    {
        return ZW_ERR_FILE;
    }

    //Check that first sub-tag must be ZW_SUBTAG_CLS
    if ((subtag[0] != ZW_SUBTAG_CLS)
        || (subtag[1] != 2))
    {
        return ZW_ERR_FILE;
    }

    cls = subtag[2];
    cls = (cls << 8) | subtag[3];

    //Check for duplicate interface
    if (zwif_find_cls(ep->intf, cls))
    {   //Duplicate interface, skip it
        return ZW_ERR_NONE;
    }

    //Point to the next sub-tag
    subtag += 4;
    len -= 4;

    //Check that second sub-tag must be ZW_SUBTAG_VER
    if ((subtag[0] != ZW_SUBTAG_VER)
        || (subtag[1] != 1))
    {
        return ZW_ERR_FILE;
    }

    ver = subtag[2];

    //Point to the next sub-tag
    subtag += 3;
    len -= 3;

    //Check that second sub-tag must be ZW_SUBTAG_PROPTY
    if ((subtag[0] != ZW_SUBTAG_PROPTY)
        || (subtag[1] != 1))
    {
        return ZW_ERR_FILE;
    }

    propty = subtag[2];

    //Create new interface
    intf = zwif_create(cls, ver, propty);

    if (!intf)
    {
        return ZW_ERR_MEMORY;
    }

    //Save back link to end point
    intf->ep = ep;

    //Add interface to the end point
    if (ep->intf)
    {   //Second interface onwards
        zwobj_add(&ep->intf->obj.next, &intf->obj);
    }
    else
    {   //First interface
        ep->intf = intf;
    }

    //Point to the next sub-tag
    subtag += 3;
    len -= 3;

    //Initialize group id of AGI
    current_grp_id = 0;

    while (len > (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE))
    {
        if (subtag[1] < 0x80)
        {
            subtag_len = subtag[1];
            ext_length_hdr = 0;
        }
        else
        {   //The next 2 bytes become length field
            subtag_len = subtag[2];
            subtag_len = (subtag_len << 8) | subtag[3];
            ext_length_hdr = 2;
        }

        if (len < (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len))
        {
            return ZW_ERR_FILE;
        }

        switch (subtag[0])
        {
            case ZW_SUBTAG_DAT_CNT:
                intf->data_cnt = subtag[2];
                break;

            case ZW_SUBTAG_DAT_ITEM_SZ:
                intf->data_item_sz = subtag[2];
                break;

            case ZW_SUBTAG_DATA:
                if (file_ver <= 0x0004)
                {
                    if (intf->cls == COMMAND_CLASS_ALARM)
                    {   //Ignore it as the data structure has changed
                        intf->data_cnt = 0;
                        intf->data_item_sz = 0;
                        break;
                    }
                }
                intf->data = malloc(subtag_len);
                if (intf->data)
                {
                    memcpy(intf->data, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr,
                           subtag_len);
                }
                break;

            case ZW_SUBTAG_AGI_GRP_CNT:
                {
                    if_grp_info_dat_t   *grp_data;
                    uint16_t	        group_cnt;

                    group_cnt = subtag[2];
                    group_cnt = (group_cnt << 8) | subtag[3];

                    grp_data = (if_grp_info_dat_t *)calloc(1, sizeof(if_grp_info_dat_t) +
                                                           (sizeof(zw_grp_info_p) * group_cnt));
                    if (grp_data)
                    {
                        grp_data->group_cnt = group_cnt;
                        intf->tmp_data = grp_data;
                    }
                }
                break;

            case ZW_SUBTAG_AGI_GRP_DYNAMIC:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    grp_data->dynamic = subtag[2];
                }
                break;

            case ZW_SUBTAG_AGI_GRP_ID:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (subtag[2] <= grp_data->group_cnt)
                    {
                        current_grp_id = subtag[2];
                    }
                }
                break;

            case ZW_SUBTAG_AGI_CMD_CNT:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (current_grp_id)
                    {
                        zw_grp_info_p       grp_info_ent;

                        grp_info_ent = (zw_grp_info_p)calloc(1, sizeof(zw_grp_info_t) +
                                                         (subtag[2] * sizeof(grp_cmd_ent_t)));

                        if (grp_info_ent)
                        {
                            grp_info_ent->grp_num = current_grp_id;
                            grp_info_ent->cmd_ent_cnt = subtag[2];
                            grp_data->grp_info[current_grp_id - 1] = grp_info_ent;
                            grp_data->valid_grp_cnt++;
                        }
                    }
                }
                break;

            case ZW_SUBTAG_AGI_PROFILE:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (current_grp_id)
                    {
                        zw_grp_info_p       grp_info_ent;

                        grp_info_ent = grp_data->grp_info[current_grp_id - 1];

                        if (grp_info_ent)
                        {
                            grp_info_ent->profile = subtag[2];
                            grp_info_ent->profile = (grp_info_ent->profile << 8) | subtag[3];
                        }
                    }
                }
                break;

            case ZW_SUBTAG_AGI_EVENT:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (current_grp_id)
                    {
                        zw_grp_info_p       grp_info_ent;

                        grp_info_ent = grp_data->grp_info[current_grp_id - 1];

                        if (grp_info_ent)
                        {
                            grp_info_ent->evt_code = subtag[2];
                            grp_info_ent->evt_code = (grp_info_ent->evt_code << 8) | subtag[3];
                        }
                    }
                }
                break;

            case ZW_SUBTAG_AGI_GRP_NAME:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (current_grp_id)
                    {
                        zw_grp_info_p       grp_info_ent;

                        grp_info_ent = grp_data->grp_info[current_grp_id - 1];

                        if (grp_info_ent)
                        {
                            memcpy(grp_info_ent->name, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE, subtag_len);
                            grp_info_ent->name[subtag_len] = '\0';
                        }
                    }
                }
                break;

            case ZW_SUBTAG_AGI_CMD_LIST:
                if (intf->tmp_data)
                {
                    if_grp_info_dat_t   *grp_data = (if_grp_info_dat_t *)intf->tmp_data;

                    if (current_grp_id)
                    {
                        zw_grp_info_p       grp_info_ent;

                        grp_info_ent = grp_data->grp_info[current_grp_id - 1];

                        if (grp_info_ent)
                        {
                            memcpy(grp_info_ent->cmd_lst, subtag + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr,
                                   subtag_len);
                        }
                    }
                }
                break;

            default:
                debug_zwapi_msg(&intf->ep->node->net->plt_ctx, "Unknown sub-tag:%02X", subtag[0]);

        }

        //Point to the next sub-tag
        subtag += (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
        len -= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + subtag_len);
    }

    return ZW_ERR_NONE;
}


/**
zwutl_sec_load - Load the security sub-tags
@param[in]	sec_ctx	    Security layer context
@param[in]	subtag_buf	Buffer that stores the sub-tags
@param[in]	len	        length of subtag_buf
@return         ZW_ERR_XXX.
*/
static int zwutl_sec_load(zwsec_layer_t *sec_ctx, uint8_t *subtag_buf, uint16_t len)
{
    uint8_t     *subtag;
    uint8_t     subtag_len;

    subtag = subtag_buf;

    if (len < 18)//Must have at leat 16 bytes of network key
    {
        return ZW_ERR_FILE;
    }

    //Check that first sub-tag must be ZW_SUBTAG_NW_KEY
    if ((subtag[0] != ZW_SUBTAG_NW_KEY)
        || (subtag[1] != 16))
    {
        return ZW_ERR_FILE;
    }

    //Load the network key
    memcpy(sec_ctx->nw_key, subtag + 2, 16);

    //Point to the next sub-tag
    subtag = subtag_buf + 18;
    len -= 18;

    while (len > (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE))
    {
        subtag_len = subtag[1];

        if (len < (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + subtag_len))
        {
            return ZW_ERR_FILE;
        }

        switch (subtag[0])
        {
            case ZW_SUBTAG_SCHEME:
                sec_ctx->inherit_schm = subtag[2];
                break;

            default:
                debug_zwapi_msg(&sec_ctx->net->plt_ctx, "Unknown sub-tag:%02X", subtag[0]);

        }

        //Point to the next sub-tag
        subtag += (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + subtag_len);
        len -= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + subtag_len);
    }

    return ZW_ERR_NONE;
}


/**
zwutl_file_add_crc - Append CRC16 checksum at the end of file
@param[in]	file	    File
@param[in]	file_len	File length
@return		Non-zero on success; otherwise zero.
*/
static int zwutl_file_add_crc(FILE *file, long file_len)
{
    int         ch;
    uint16_t    crc;
    uint8_t     tmp_data;
    uint8_t     bitmask;
    uint8_t     new_bit;

    crc = CRC_INIT;

    while (file_len-- > 0)
    {
        if ((ch = fgetc(file)) == EOF)
        {
            return 0;
        }

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
    /* According to fopen man page: It is good practice (and indeed sometimes necessary  under  Linux)  to
       put an fseek(3) or fgetpos(3) operation between write and read operations on such a stream.  This opera-
       tion may be an apparent no-op (as in fseek(..., 0L, SEEK_CUR) called for its synchronizing side effect.
       */
    fseek(file, 0L, SEEK_CUR);

    return zwutl_fwrite16(file, crc);
}


/**
zwutl_file_chk - check file integrity
@param[in]	file	    File to be checked
@return         zero if check o.k.; non-zero on error
*/
static uint16_t zwutl_file_chk(FILE *file)
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
    return crc;
}


/**
zwutl_ni_load - Load node information from file
@param[in]	nw	        Network
@param[in]	ni_file	    Node information file
@return         ZW_ERR_XXX.
*/
int zwutl_ni_load(zwnet_p nw, const char *ni_file)
{
    int         result;
    int         res;
    FILE        *file;
    zwtag_p     tag;
    zwnode_p    curr_node;      //current node
    zwep_p      curr_ep;        //current end point
    size_t      rd_count;
    uint8_t     file_hdr[4];
    uint16_t    file_ver;
    uint16_t    curr_ver;


    //Open the node info file
    if (!ni_file)
    {
        return ZW_ERR_FILE_OPEN;
    }

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, ni_file, "r+b") != 0)
    {
        return ZW_ERR_FILE_OPEN;
    }
#else
    file = fopen(ni_file, "r+b");
    if (!file)
    {
        return ZW_ERR_FILE_OPEN;
    }
#endif

    //Verify the file magic identifier
    rd_count = fread(file_hdr, 1, 4, file);

    if ((rd_count == 4)
        && (file_hdr[0] == FILE_MAGIC_NUM_H)
        && (file_hdr[1] == FILE_MAGIC_NUM_L))
    {
        file_ver = file_hdr[2];
        file_ver = (file_ver << 8) | file_hdr[3];
    }
    else
    {
        fclose(file);
        return ZW_ERR_FILE;
    }

    //Check for file version
    curr_ver = FILE_VER_NUM_H;
    curr_ver = (curr_ver << 8) | FILE_VER_NUM_L;
    if ((file_ver == 0) || (file_ver > curr_ver))
    {
        fclose(file);
        return ZW_ERR_VERSION;
    }

    //Check CRC
    fseek(file, 0L, SEEK_SET);

    if (zwutl_file_chk(file) != 0)
    {
        fclose(file);
        return ZW_ERR_FILE;
    }

    if (fseek(file, NI_FILE_HDR_LEN, SEEK_SET) != 0)
    {
        fclose(file);
        return ZW_ERR_FILE;
    }

    //Check whether the home id is matched
    result = zwutl_tag_rd(file, &tag);
    if (result < 0)
    {
        goto l_ZWUTIL_LOAD_ERROR1;
    }

    //Check that first tag must be ZW_TAG_NW and
    //first sub-tag must be ZW_SUBTAG_HOME_ID
    if (tag->id != ZW_TAG_NW)
    {
        result = ZW_ERR_FILE;
        goto l_ZWUTIL_LOAD_ERROR2;
    }

    if ((tag->len >= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 4))
        && (tag->data[0] == ZW_SUBTAG_HOME_ID)
        && (tag->data[1] == 4))
    {
        uint32_t    home_id;
        uint8_t    *ptr;

        //Point to the payload of sub-tag
        ptr = tag->data + ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE;
        home_id = ((uint32_t)ptr[0])<<24 | ((uint32_t)ptr[1])<<16
                   | ((uint32_t)ptr[2])<<8 | ((uint32_t)ptr[3]);

        //Check whether home id is correct
        if (home_id != nw->homeid)
        {
            result = ZW_ERR_FILE_HOME_ID;
            goto l_ZWUTIL_LOAD_ERROR2;
        }

        //Process other network sub-tag
        tag->len -= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 4);
        ptr += 4;

        if ((tag->len >= (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 1))
            && (ptr[0] == ZW_SUBTAG_GRP_NODE)
            && (ptr[1] == 1))
        {
            nw->stAGIData.pAGIGroupList->stNodeEpInfo.byNodeID = ptr[2];
        }
    }
    else
    {
        result = ZW_ERR_FILE;
        goto l_ZWUTIL_LOAD_ERROR2;
    }

    free(tag);

    //
    //Process other tags
    //

    curr_node = NULL;
    curr_ep = NULL;

    plt_mtx_lck(nw->mtx);
    while((result = zwutl_tag_rd(file, &tag)) == ZW_ERR_NONE)
    {
        switch (tag->id)
        {
            case ZW_TAG_NODE:
                res = zwutl_node_load(nw, tag->data, tag->len, &curr_node);
                if (res == ZW_ERR_NODE_NOT_FOUND)
                {   //Node not found is considered o.k., this just means the controller's
                    //routing table is not up-to-date.
                    curr_node = NULL;
                    curr_ep = NULL;
                    res = ZW_ERR_NONE;
                }
                break;

            case ZW_TAG_EP:
                if (curr_node)
                {
                    res = zwutl_ep_load(curr_node, tag->data, tag->len, &curr_ep);
                }
                else
                {   //Node not found, skip the end point details
                    res = ZW_ERR_NONE;
                }
                break;

            case ZW_TAG_IF:
                if (curr_ep)
                {
                    res = zwutl_if_load(curr_ep, tag->data, tag->len, file_ver);
                }
                else
                {   //End point not found, skip the interface details
                    res = ZW_ERR_NONE;
                }
                break;

            case ZW_TAG_SEC:
                if (nw->sec_enable)
                {
                    res = zwutl_sec_load(nw->sec_ctx, tag->data, tag->len);
                }
                else
                {   //Don't process if security layer is disabled
                    res = ZW_ERR_NONE;
                }
                break;

            default:
                res = ZW_ERR_NONE;
                debug_zwapi_msg(&nw->plt_ctx, "Unknown tag:%04X", tag->id);

        }
        if (res < 0)
        {
            result = res;
            goto l_ZWUTIL_LOAD_ERROR3;
        }
        free(tag);
    }
    plt_mtx_ulck(nw->mtx);

    if (result == ZW_ERR_FILE_EOF)
    {   //Reading at end of file is considered o.k.
        result = ZW_ERR_NONE;
    }
    fclose(file);
    return result;

l_ZWUTIL_LOAD_ERROR3:
    plt_mtx_ulck(nw->mtx);
l_ZWUTIL_LOAD_ERROR2:
    free(tag);
l_ZWUTIL_LOAD_ERROR1:
    fclose(file);
    return result;

}

/**
zwutl_subtag_wr32 - Write a 32-bit sub-tag data into buffer
@param[in]      id          id of the sub-tag
@param[in]      data        data to be written
@param[in, out] buf         buffer to be written, the buf will be adjusted after the write
                            to point to the next empty location for subsequent write
@param[in, out] total_len   the number of bytes used for the sub-tag will be added to this total_len
@return
*/
static void zwutl_subtag_wr32(uint8_t id, uint32_t data, uint8_t **buf, uint16_t *total_len)
{
    uint8_t     *subtag;
    uint8_t     subtag_len;

    subtag = *buf;
    subtag[0] = id;
    subtag[1] = 4;
    subtag[2] = data >> 24;
    subtag[3] = (data >> 16) & 0xFF;
    subtag[4] = (data >> 8) & 0xFF;
    subtag[5] = data & 0xFF;

    subtag_len = (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 4);

    *buf = subtag + subtag_len;

    (*total_len) += subtag_len;

}


/**
zwutl_subtag_wr16 - Write a 16-bit sub-tag data into buffer
@param[in]      id          id of the sub-tag
@param[in]      data        data to be written
@param[in, out] buf         buffer to be written, the buf will be adjusted after the write
                            to point to the next empty location for subsequent write
@param[in, out] total_len   the number of bytes used for the sub-tag will be added to this total_len
@return
*/
static void zwutl_subtag_wr16(uint8_t id, uint16_t data, uint8_t **buf, uint16_t *total_len)
{
    uint8_t     *subtag;
    uint8_t     subtag_len;

    subtag = *buf;
    subtag[0] = id;
    subtag[1] = 2;
    subtag[2] = data >> 8;
    subtag[3] = data & 0xFF;

    subtag_len = (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 2);

    *buf = subtag + subtag_len;

    (*total_len) += subtag_len;

}


/**
zwutl_subtag_wr8 - Write a 8-bit sub-tag data into buffer
@param[in]      id          id of the sub-tag
@param[in]      data        data to be written
@param[in, out] buf         buffer to be written, the buf will be adjusted after the write
                            to point to the next empty location for subsequent write
@param[in, out] total_len   the number of bytes used for the sub-tag will be added to this total_len
@return
*/
static void zwutl_subtag_wr8(uint8_t id, uint8_t data, uint8_t **buf, uint16_t *total_len)
{
    uint8_t     *subtag;
    uint8_t     subtag_len;

    subtag = *buf;
    subtag[0] = id;
    subtag[1] = 1;
    subtag[2] = data;

    subtag_len = (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + 1);

    *buf = subtag + subtag_len;

    (*total_len) += subtag_len;

}


/**
zwutl_subtag_wr - Write sub-tag data into buffer
@param[in]      id          id of the sub-tag
@param[in]      data        data to be written
@param[in]      data_len    size of data
@param[in, out] buf         buffer to be written, the buf will be adjusted after the write
                            to point to the next empty location for subsequent write
@param[in, out] total_len   the number of bytes used for the sub-tag will be added to this total_len
@return
*/
static void zwutl_subtag_wr(uint8_t id, uint16_t data_len, uint8_t *data, uint8_t **buf, uint16_t *total_len)
{
    int         ext_length_hdr;
    uint8_t     *subtag;
    uint16_t    subtag_len;

    //Only write sub-tag if there is data
    if (data_len == 0)
    {
        return;
    }

    subtag = *buf;
    subtag[0] = id;
    if (data_len < 0x80)
    {
        subtag[1] = (uint8_t)data_len;
        ext_length_hdr = 0;
    }
    else
    {   //The next 2 bytes become the length field
        subtag[1] = 0x80;//Set flag
        subtag[2] = data_len >> 8;
        subtag[3] = data_len & 0x00FF;
        ext_length_hdr = 2;
    }

    if (data_len == 1)
    {
        subtag[2] = *data;
    }
    else
    {
        memcpy(subtag + 2 + ext_length_hdr, data, data_len);
    }
    subtag_len = (ZW_SUBTAG_ID_SIZE + ZW_SUBTAG_LEN_SIZE + ext_length_hdr + data_len);

    *buf = subtag + subtag_len;

    (*total_len) += subtag_len;

}


/**
zwutl_ni_save - Save node information into file
@param[in]	nw	        Network
@param[in]	ni_file	    Node information file
@return         ZW_ERR_XXX.
*/
int zwutl_ni_save(zwnet_p nw, const char *ni_file)
{
    int         result;
    FILE        *file;
    zwtag_p     tag;
    zwnode_p    curr_node;      //current node
    zwep_p      curr_ep;        //current end point
    zwif_p      curr_intf;      //current interface
    uint8_t     *subtag;
    long        file_len;


    //Create a node info file
    file = zwutl_ni_file_new(ni_file);
    if (!file)
    {
        return ZW_ERR_FILE_OPEN;
    }

    //Allocate memory for tag
    tag = (zwtag_p)malloc(sizeof(zwtag_t) + MAX_SUBTAGS_SIZE);
    if (!tag)
    {
        result = ZW_ERR_MEMORY;
        goto l_ZWUTIL_SAVE_ERROR1;
    }

    //Write home id
    tag->id = ZW_TAG_NW;
    subtag = tag->data;
    tag->len = 0;

    zwutl_subtag_wr32(ZW_SUBTAG_HOME_ID, nw->homeid, &subtag, &tag->len);

    //Write group 1 node id
    zwutl_subtag_wr8(ZW_SUBTAG_GRP_NODE, nw->stAGIData.pAGIGroupList->stNodeEpInfo.byNodeID, &subtag, &tag->len);

    result = zwutl_tag_wr(file, tag);
    if (result < 0)
    {
        goto l_ZWUTIL_SAVE_ERROR2;
    }

    //Write security information
    if (nw->sec_enable)
    {
        tag->id = ZW_TAG_SEC;
        subtag = tag->data;
        tag->len = 0;

        zwutl_subtag_wr(ZW_SUBTAG_NW_KEY, 16, nw->sec_ctx->nw_key, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_SCHEME, nw->sec_ctx->inherit_schm, &subtag, &tag->len);
        result = zwutl_tag_wr(file, tag);
        if (result < 0)
        {
            goto l_ZWUTIL_SAVE_ERROR2;
        }
    }

    //Write node information
    curr_node = &nw->ctl;

    plt_mtx_lck(nw->mtx);

    while (curr_node)
    {
        //Write node tag
        tag->id = ZW_TAG_NODE;
        subtag = tag->data;
        tag->len = 0;

        zwutl_subtag_wr8(ZW_SUBTAG_NODE_ID, curr_node->nodeid, &subtag, &tag->len);
        zwutl_subtag_wr16(ZW_SUBTAG_VID, curr_node->vid, &subtag, &tag->len);
        zwutl_subtag_wr16(ZW_SUBTAG_VTYPE, curr_node->vtype, &subtag, &tag->len);
        zwutl_subtag_wr16(ZW_SUBTAG_PID, curr_node->pid, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_MULCH_VER, curr_node->mul_ch_ver, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_EP_CNT, curr_node->num_of_ep, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_SLEEP_CAP, curr_node->sleep_cap, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_SND_SEQ_N, curr_node->sec_snd_seq_num, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_SEC_INC_FAILED, curr_node->sec_incl_failed, &subtag, &tag->len);
#ifdef CRC16_ENCAP
        zwutl_subtag_wr8(ZW_SUBTAG_CRC16_CAP, curr_node->crc_cap, &subtag, &tag->len);
#endif
        zwutl_subtag_wr8(ZW_SUBTAG_DEV_ID_LEN, curr_node->dev_id.len, &subtag, &tag->len);
        if (curr_node->dev_id.len > 0)
        {
            zwutl_subtag_wr8(ZW_SUBTAG_DEV_ID_TYPE, curr_node->dev_id.type, &subtag, &tag->len);
            zwutl_subtag_wr8(ZW_SUBTAG_DEV_ID_FMT, curr_node->dev_id.format, &subtag, &tag->len);
            zwutl_subtag_wr(ZW_SUBTAG_DEV_ID_DAT, curr_node->dev_id.len, (uint8_t *)curr_node->dev_id.dev_id, &subtag, &tag->len);
        }
        zwutl_subtag_wr8(ZW_SUBTAG_LIB_TYPE, curr_node->lib_type, &subtag, &tag->len);
        zwutl_subtag_wr16(ZW_SUBTAG_PROT_VER, curr_node->proto_ver, &subtag, &tag->len);
        zwutl_subtag_wr16(ZW_SUBTAG_APPL_VER, curr_node->app_ver, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_MUL_CMD_CAP, curr_node->mul_cmd_cap, &subtag, &tag->len);
        zwutl_subtag_wr8(ZW_SUBTAG_CATEGORY, curr_node->category, &subtag, &tag->len);

        //Set extended node information of nodes other than the Z/IP gateway attached controller
        if (curr_node->ext_ver && (curr_node->nodeid != nw->ctl.nodeid))
        {
            uint16_t    ver_cap_cnt;

            ver_cap_cnt = 0x0100 | curr_node->ext_ver->fw_cnt;
            zwutl_subtag_wr16(ZW_SUBTAG_EXT_VER_CAP, ver_cap_cnt, &subtag, &tag->len);
            zwutl_subtag_wr8(ZW_SUBTAG_HW_VER, curr_node->ext_ver->hw_ver, &subtag, &tag->len);
            if (curr_node->ext_ver->fw_cnt)
            {
                zwutl_subtag_wr(ZW_SUBTAG_FW_VER, curr_node->ext_ver->fw_cnt * 2, (uint8_t *)curr_node->ext_ver->fw_ver, &subtag, &tag->len);
            }
        }

        result = zwutl_tag_wr(file, tag);
        if (result < 0)
        {
            goto l_ZWUTIL_SAVE_ERROR3;
        }

        //Write end point tag
        curr_ep = &curr_node->ep;

        while (curr_ep)
        {
            tag->id = ZW_TAG_EP;
            subtag = tag->data;
            tag->len = 0;

            zwutl_subtag_wr8(ZW_SUBTAG_EP_ID, curr_ep->epid, &subtag, &tag->len);
            zwutl_subtag_wr8(ZW_SUBTAG_GEN, curr_ep->generic, &subtag, &tag->len);
            zwutl_subtag_wr8(ZW_SUBTAG_SPEC, curr_ep->specific, &subtag, &tag->len);
            zwutl_subtag_wr(ZW_SUBTAG_EP_NAME, strlen(curr_ep->name), (uint8_t *)curr_ep->name, &subtag, &tag->len);
            zwutl_subtag_wr(ZW_SUBTAG_EP_LOC, strlen(curr_ep->loc), (uint8_t *)curr_ep->loc, &subtag, &tag->len);
            //Set ZWave+ info of end point other than the Z/IP gateway attached controller
            if (curr_ep->zwplus_info.zwplus_ver && (curr_ep->node->nodeid != nw->ctl.nodeid))
            {
                zwutl_subtag_wr8(ZW_SUBTAG_ZWPLUS_VER, curr_ep->zwplus_info.zwplus_ver, &subtag, &tag->len);
                zwutl_subtag_wr8(ZW_SUBTAG_ROLE_TYPE, curr_ep->zwplus_info.role_type, &subtag, &tag->len);
                zwutl_subtag_wr8(ZW_SUBTAG_NODE_TYPE, curr_ep->zwplus_info.node_type, &subtag, &tag->len);
                zwutl_subtag_wr16(ZW_SUBTAG_INSTR_ICON, curr_ep->zwplus_info.instr_icon, &subtag, &tag->len);
                zwutl_subtag_wr16(ZW_SUBTAG_USER_ICON, curr_ep->zwplus_info.usr_icon, &subtag, &tag->len);
            }

            result = zwutl_tag_wr(file, tag);
            if (result < 0)
            {
                goto l_ZWUTIL_SAVE_ERROR3;
            }

            //Write interface tag
            curr_intf = curr_ep->intf;

            while (curr_intf)
            {
                tag->id = ZW_TAG_IF;
                subtag = tag->data;
                tag->len = 0;

                zwutl_subtag_wr16(ZW_SUBTAG_CLS, curr_intf->cls, &subtag, &tag->len);
                zwutl_subtag_wr8(ZW_SUBTAG_VER, curr_intf->ver, &subtag, &tag->len);
                zwutl_subtag_wr8(ZW_SUBTAG_PROPTY, curr_intf->propty, &subtag, &tag->len);
                if (curr_intf->data_cnt > 0)
                {
                    zwutl_subtag_wr8(ZW_SUBTAG_DAT_CNT, curr_intf->data_cnt, &subtag, &tag->len);
                    zwutl_subtag_wr8(ZW_SUBTAG_DAT_ITEM_SZ, curr_intf->data_item_sz, &subtag, &tag->len);
                    zwutl_subtag_wr(ZW_SUBTAG_DATA, curr_intf->data_cnt * curr_intf->data_item_sz,
                                    (uint8_t *)curr_intf->data, &subtag, &tag->len);

                }

                if ((curr_intf->cls == COMMAND_CLASS_ASSOCIATION_GRP_INFO)
                    && (curr_intf->tmp_data))
                {
                    int                 i;
                    if_grp_info_dat_t   *grp_info = (if_grp_info_dat_t *)curr_intf->tmp_data;

                    zwutl_subtag_wr16(ZW_SUBTAG_AGI_GRP_CNT, grp_info->group_cnt, &subtag, &tag->len);
                    zwutl_subtag_wr8(ZW_SUBTAG_AGI_GRP_DYNAMIC, grp_info->dynamic, &subtag, &tag->len);

                    for (i=0; i<grp_info->valid_grp_cnt; i++)
                    {
                        if (grp_info->grp_info[i])
                        {
                            zw_grp_info_p grp_ent = grp_info->grp_info[i];

                            zwutl_subtag_wr8(ZW_SUBTAG_AGI_GRP_ID, grp_ent->grp_num, &subtag, &tag->len);
                            zwutl_subtag_wr8(ZW_SUBTAG_AGI_CMD_CNT, grp_ent->cmd_ent_cnt, &subtag, &tag->len);
                            zwutl_subtag_wr16(ZW_SUBTAG_AGI_PROFILE, grp_ent->profile, &subtag, &tag->len);
                            zwutl_subtag_wr16(ZW_SUBTAG_AGI_EVENT, grp_ent->evt_code, &subtag, &tag->len);
                            zwutl_subtag_wr(ZW_SUBTAG_AGI_GRP_NAME, strlen(grp_ent->name),
                                            (uint8_t *)grp_ent->name, &subtag, &tag->len);
                            zwutl_subtag_wr(ZW_SUBTAG_AGI_CMD_LIST, grp_ent->cmd_ent_cnt * sizeof(grp_cmd_ent_t),
                                            (uint8_t *)grp_ent->cmd_lst, &subtag, &tag->len);

                        }
                    }
                }

                result = zwutl_tag_wr(file, tag);
                if (result < 0)
                {
                    goto l_ZWUTIL_SAVE_ERROR3;
                }

                //Next interface
                curr_intf = (zwif_p)curr_intf->obj.next;

            }


            //Next end point
            curr_ep = (zwep_p)curr_ep->obj.next;
        }


        //Next node
        curr_node = (zwnode_p)curr_node->obj.next;

    }

    plt_mtx_ulck(nw->mtx);
    free(tag);

    //Append CRC16 checksum
    file_len = ftell(file);
    fseek(file, 0L, SEEK_SET);
    result = zwutl_file_add_crc(file, file_len);
    fclose(file);
    return (result != 0)? ZW_ERR_NONE : ZW_ERR_FILE_WRITE;

l_ZWUTIL_SAVE_ERROR3:
    plt_mtx_ulck(nw->mtx);
l_ZWUTIL_SAVE_ERROR2:
    free(tag);
l_ZWUTIL_SAVE_ERROR1:
    fclose(file);
    return result;

}


/**
zwutl_crc16_chk - CRC-CCITT (0x1D0F) calculation / check
@param[in]	crc	        CRC initialization value
@param[in]	data	    Buffer that store the byte string
@param[in]	len	        Length of byte string
@return         If data contains the 16-bit checksum field: return zero if check o.k.
                If data excludes the 16-bit checksum field: return CRC-16 checksum.
*/
uint16_t zwutl_crc16_chk(uint16_t crc, uint8_t *data, uint16_t len)
{
    uint8_t tmp_data;
    uint8_t bitmask;
    uint8_t new_bit;

    while (len--)
    {
        tmp_data = *data++;
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
    return crc;
}


#ifdef FLASH_PROG_API

/**
zwutl_fl_fw_wr - Write firmware file to flash code space
@param[in]	    fl_prog_ctx		Flash programming context
@param[in]	    fw_file	        Firmware file
@return         ZW_ERR_XXX
*/
int zwutl_fl_fw_wr(flash_prog_ctx_t *fl_prog_ctx, const char *fw_file)
{
    int         result;
    int         retry_cnt;
    FILE        *file;
    long        file_sz;
    unsigned    sector_cnt;
    unsigned    last_sector_dat_size;   //last sector data count
    unsigned    fw_rd_cnt;              //number of bytes to read from firmware file
    size_t      rd_count;
    uint8_t     rd_buf[FL_SECTOR_SIZE];
    uint8_t     wr_buf[FL_SECTOR_SIZE];
    uint8_t     i;


    //Open the file
    if (!fw_file)
    {
        return ZW_ERR_FILE_OPEN;
    }

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, fw_file, "r+b") != 0)
    {
        return ZW_ERR_FILE_OPEN;
    }
#else
    file = fopen(fw_file, "r+b");
    if (!file)
    {
        return ZW_ERR_FILE_OPEN;
    }
#endif

    //
    //Calculate number of sectors to write
    //

    //Get file size
    fseek(file, 0, SEEK_END); // seek to end of file
    file_sz = ftell(file);

    //Check for zero-size file
    if (file_sz == 0)
    {
        fclose(file);
        return ZW_ERR_FILE_EOF;
    }

    fseek(file, 0, SEEK_SET); // seek to beginning of file

    sector_cnt = file_sz / FL_SECTOR_SIZE;

    last_sector_dat_size = file_sz % FL_SECTOR_SIZE;


    //Read a sector and write it to flash
    i = 0;
    sector_cnt++; //for last sector
    while (sector_cnt-- > 0)
    {
        if (sector_cnt > 0)
        {
            fw_rd_cnt = FL_SECTOR_SIZE;
        }
        else
        {   //last sector
            if (last_sector_dat_size > 0)
            {
                fw_rd_cnt = last_sector_dat_size;

                //Pad the rest with 0xFF
                memset(wr_buf + last_sector_dat_size, 0xFF, FL_SECTOR_SIZE - last_sector_dat_size);
            }
            else
            {   //Done.
                break;
            }
        }

        rd_count = fread(wr_buf, 1, fw_rd_cnt, file);

        if (rd_count == fw_rd_cnt)
        {
            retry_cnt = 3;

            while (retry_cnt-- > 0)
            {
                //Write to sector
                result = fl_prog_fl_wr(fl_prog_ctx, wr_buf, i);
                if (result == 0)
                {
                    debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Write sector:%u completed", (unsigned)i);
                }
                else
                {
                    debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Couldn't write sector:%u (%d)", (unsigned)i, result);
                    fclose(file);
                    return result;
                }

                //Verify the sector
                result = fl_prog_fl_rd(fl_prog_ctx, rd_buf, i, FL_SECTOR_SIZE);
                if (result != 0)
                {
                    debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Couldn't read sector:%u (%d)", (unsigned)i, result);
                    fclose(file);
                    return result;
                }

                if (memcmp(wr_buf, rd_buf, FL_SECTOR_SIZE) == 0)
                {   //o.k.
                    i++;
                    break;
                }

                if (retry_cnt == 0)
                {
                    debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Failed to write sector:%u without error", (unsigned)i);
                    fclose(file);
                    return ZWHCI_ERROR_VERIFY;
                }

                //Data corrupted, erase sector for rewrite
                debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Data corrupted, rewrite sector:%u", (unsigned)i);
                result = fl_prog_erase_sector(fl_prog_ctx, i);
                if (result != 0)
                {
                    debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Couldn't erase sector:%u (%d)", (unsigned)i, result);
                    fclose(file);
                    return result;
                }
            }
        }
        else
        {
            fclose(file);
            return ZW_ERR_FILE_READ;
        }
    }

    fclose(file);
    return 0;
}


/**
zwutl_fl_fw_rd - Read firmware from flash code space to a file
@param[in]	    fl_prog_ctx		Flash programming context
@param[in]	    sector_cnt	    Number of sectors to read (up to maximum of 64 sectors)
@param[in]	    fw_file	        Firmware file to write to
@return         ZW_ERR_XXX
*/
int zwutl_fl_fw_rd(flash_prog_ctx_t *fl_prog_ctx, uint8_t sector_cnt, const char *fw_file)
{
    int         result;
    FILE        *file;
    size_t      wr_count;
    uint8_t     buf[FL_SECTOR_SIZE];
    uint8_t     i;


    //Open the file
    if (!fw_file)
    {
        return ZW_ERR_FILE_OPEN;
    }

#ifdef USE_SAFE_VERSION
    if (fopen_s(&file, fw_file, "w+b") != 0)
    {
        return ZW_ERR_FILE_OPEN;
    }
#else
    file = fopen(fw_file, "w+b");
    if (!file)
    {
        return ZW_ERR_FILE_OPEN;
    }
#endif

    //Read a sector and write it to file
    for (i=0; i<sector_cnt; i++)
    {
        result = fl_prog_fl_rd(fl_prog_ctx, buf, i, FL_SECTOR_SIZE);
        if (result != 0)
        {
            debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Couldn't read sector:%u (%d)", (unsigned)i, result);
            fclose(file);
            return result;
        }

        wr_count = fwrite(buf, 1, FL_SECTOR_SIZE, file);
        if (wr_count != FL_SECTOR_SIZE)
        {
            debug_zwapi_msg(fl_prog_ctx->plt_ctx, "Couldn't write to file");
            fclose(file);
            return ZW_ERR_FILE_WRITE;
        }
    }

    fclose(file);
    return 0;
}
#endif

/**
@}
*/















