/**
@file   zw_hci_util.h - Z-wave host controller interface utility/miscellaneous functions header file.

        To implement some utility/miscellaneous functions.

@author David Chow

@version    1.0 5-7-10  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_HCI_UTIL_DAVID_
#define _ZW_HCI_UTIL_DAVID_
/**
@defgroup Util Utility/Miscellaneous Functions
Some utility/miscellaneous functions.
@{
*/

///
/// List entry
typedef struct  _util_lst
{
    struct _util_lst  *next;        ///< Point to the next list entry
    uint32_t          dat_sz;       ///< Data size
    uint8_t           wr_buf[1];    ///< Place holder for the data buffer

} util_lst_t;

///
/// List compare function that return 0 if s1 is same as s2; else return non-zero
typedef int (*util_list_cmp_fn)(uint8_t *s1, uint8_t *s2);


int     util_list_add(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint8_t dat_sz);
int     util_list_add_no_dup(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint8_t dat_sz, util_list_cmp_fn cmp_fn);
util_lst_t *util_list_get(void *mtx_ctx, util_lst_t **head);
void    util_list_flush(void *mtx_ctx, util_lst_t **head);
int     util_list_find(void *mtx_ctx, util_lst_t *head, uint8_t  *buf, util_list_cmp_fn cmp_fn);
int     util_list_rm(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, util_list_cmp_fn cmp_fn);
int     util_list_rplc(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint16_t dat_sz, util_list_cmp_fn cmp_fn);
void    util_hex_string_add(char *src, unsigned src_size, unsigned num);
void    util_num_string_add(char *src, unsigned src_size, unsigned num);
void    util_ntohs(uint16_t *src, unsigned elem_cnt);
int     util_tmr_exp_chk(uint16_t now, uint16_t timer);
int     util_is_host_big_endian(void);
int     util_cmd_cls_parse(uint8_t *cmd_buf, uint8_t cmd_buf_len, uint16_t **cmd_cls, uint8_t *cmd_cnt,
                           uint16_t **cmd_cls_sec, uint8_t *cmd_cnt_sec);
#ifdef WIN32
int     util_com_port_get(unsigned *port_count, unsigned *ports, TCHAR **friendly_names);
#endif


/**
@}
*/



#endif /* _ZW_HCI_UTIL_DAVID_ */

