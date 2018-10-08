/**
@file   zw_hci_util.c - Z-wave host controller interface utility/miscellaneous functions.

        To implement some utility/miscellaneous functions.

@author David Chow

@version    1.0 5-7-10  Initial release

version: 1.0
comments: Initial release
*/
#if defined(_WINDOWS) || defined(WIN32)
#include "../include/zw_plt_windows.h"   //use in Windows platform
#elif defined(OS_LINUX)
#include "../include/zw_plt_linux.h"     //use in Linux platform
#endif
#include <stdlib.h>
#ifndef OS_MAC_X
#include <malloc.h>
#endif
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "../include/zw_hci_error.h"
#include "../include/zw_hci_platform.h"

#ifdef WIN32
    #include <setupapi.h>
    #include <guiddef.h>

    #ifndef GUID_DEVINTERFACE_COMPORT
    DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
    #endif

typedef HKEY (__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
typedef BOOL (__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
typedef BOOL (__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
typedef BOOL (__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
typedef HDEVINFO (__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
typedef BOOL (__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);

    //Automatically pull in the library advapi32.lib since we be using registry access methods
    #pragma comment(lib, "advapi32.lib")
#endif

/**
@defgroup Util Utility/Miscellaneous Functions
Some utility/miscellaneous functions.
@{
*/

#ifdef WIN32
/**
util_is_digit_str - add an entry into the end of the list.
@param[in]      tst_string  String to test whether it is all digits
@return                     Return non-zero if string is all digits; else return zero.
*/
    #ifdef  _UNICODE
static int util_is_digit_str(LPCWSTR tst_string)
    #else
static int util_is_digit_str(LPCSTR tst_string)
    #endif
{
    size_t i;
    int    digit = 1;   //Flag to indicate whether the character is digit
    #ifdef  _UNICODE
    size_t nLen = wcslen(tst_string);
    #else
    size_t nLen = strlen(tst_string);
    #endif

    if (nLen == 0)
        return 0;

    for (i=0; i<nLen && digit; i++)
    {
    #ifdef  _UNICODE
        digit = iswdigit(tst_string[i]);
    #else
        digit = isdigit((int)tst_string[i]);
    #endif
    }

    return digit;
}


/**
util_lib_load - load win32 system library
@param[in]      lib_file    Library file name
@return                     Return handle to the library module on success; NULL on failure.
*/
static HMODULE util_lib_load(LPCTSTR lib_file)
{
    TCHAR   szFullPath[_MAX_PATH];

    szFullPath[0] = _T('\0');
    if (GetSystemDirectory(szFullPath, _countof(szFullPath)) == 0)
        return NULL;

    //Setup the full path and delegate to LoadLibrary
    _tcscat_s(szFullPath, _countof(szFullPath), _T("\\"));
    _tcscat_s(szFullPath, _countof(szFullPath), lib_file);
    return LoadLibrary(szFullPath);
}


/**
util_reg_str_get - get the string of a specified value from registry
@param[in]      reg_key     Registry key
@param[in]      val_name    Value name
@param[out]     val_string  Value string
@return         Return non-zero on success; else return zero.
@post           Caller must free the allocated string in val_string
*/
static int util_reg_str_get(HKEY reg_key, LPCTSTR val_name, LPTSTR *val_string)
{
    DWORD   allocated_sz;
    DWORD   returned_sz;
    DWORD   reg_type = 0;
    DWORD   val_string_sz = 0;
    LPTSTR  tmp_string;

    //First query for the size of the registry value string
    LONG result = RegQueryValueEx(reg_key, val_name, NULL, &reg_type, NULL, &val_string_sz);
    if (result != ERROR_SUCCESS)
    {
        return 0;
    }

    //Ensure the type is a val_string
    if (reg_type != REG_SZ)
    {
        return 0;
    }

    //Allocate enough bytes for the return value
    allocated_sz = (val_string_sz * 2) + sizeof(TCHAR);

    tmp_string = (LPTSTR)LocalAlloc(LMEM_FIXED, allocated_sz);
    if (tmp_string == NULL)
        return 0;

    //Get value string
    tmp_string[0] = _T('\0');
    returned_sz = allocated_sz;
    result = RegQueryValueEx(reg_key, val_name, NULL, &reg_type, (LPBYTE)(tmp_string), &returned_sz);
    if (result != ERROR_SUCCESS)
    {
        LocalFree(tmp_string);
        return 0;
    }

    //Handle the case where the data just returned is the same size as the allocated size. This could occur where the data
    //has been updated in the registry with a non null terminator between the two calls to ReqQueryValueEx above.
    if (returned_sz >= allocated_sz)
    {
        return 0;
    }

    //NULL terminate the data if it was not returned NULL terminated because it is not stored null terminated in the registry
    if (tmp_string[returned_sz/sizeof(TCHAR) - 1] != _T('\0'))
        tmp_string[returned_sz/sizeof(TCHAR)] = _T('\0');

    *val_string = tmp_string;
    return 1;
}


/**
util_reg_port_get - get a serial com port number from registry
@param[in]      reg_key     Registry key
@param[out]     port_num       Com port number
@return         Return non-zero on success; else return zero.
*/
static int util_reg_port_get(HKEY reg_key, int *port_num)
{
    int found = 0;

    //Read in the name of the port
    LPTSTR port_name = NULL;

    if (util_reg_str_get(reg_key, _T("PortName"), &port_name))
    {
        //If it looks like "COMx", add it to the array which will be returned
        size_t nLen = _tcslen(port_name);

        if (nLen > 3)
        {
            if ((_tcsnicmp(port_name, _T("COM"), 3) == 0) && util_is_digit_str(port_name + 3))
            {
                //Work out the port number
                *port_num = _ttoi(port_name + 3);

                found = 1;
            }
        }
        LocalFree(port_name);
    }

    return found;
}


/**
util_com_port_get - get serial com port numbers and their corresponding friendly names
@param[in, out] port_count     Input: The maximum number of serial com port numbers the ports and friendly_names
                                      buffers can store.
                               Output:The actual number of com port found.
@param[out]     ports          Buffer that stores the serial com port numbers
@param[out]     friendly_names Array of char * that store the friendly names
@return         Return non-zero on success; else return zero.
@post           Caller needs to free the character strings in friendly_names
*/
int util_com_port_get(unsigned *port_count, unsigned *ports, TCHAR **friendly_names)
{
    HDEVINFO        hDevInfoSet;
    GUID            guid;
    BOOL            more_item = TRUE;
    int             item_num = 0;
    unsigned        port_cnt = 0;
    unsigned        max_port = *port_count;
    SP_DEVINFO_DATA devInfo;
    SETUPDIOPENDEVREGKEY                *lpfnSETUPDIOPENDEVREGKEY;
    SETUPDIGETCLASSDEVS                 *lpfnSETUPDIGETCLASSDEVS;
    SETUPDIGETDEVICEREGISTRYPROPERTY    *lpfnSETUPDIGETDEVICEREGISTRYPROPERTY;
    SETUPDIDESTROYDEVICEINFOLIST        *lpfnSETUPDIDESTROYDEVICEINFOLIST;
    SETUPDIENUMDEVICEINFO               *lpfnSETUPDIENUMDEVICEINFO;

    //Get the various function pointers we require from setupapi.dll
    HMODULE setupAPI = util_lib_load(_T("SETUPAPI.DLL"));
    if (setupAPI == NULL)
        return 0;


    lpfnSETUPDIOPENDEVREGKEY = (SETUPDIOPENDEVREGKEY*)(GetProcAddress(setupAPI, "SetupDiOpenDevRegKey"));
#if defined _UNICODE
    lpfnSETUPDIGETCLASSDEVS = (SETUPDIGETCLASSDEVS*)(GetProcAddress(setupAPI, "SetupDiGetClassDevsW"));
    lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = (SETUPDIGETDEVICEREGISTRYPROPERTY*)(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyW"));
#else
    lpfnSETUPDIGETCLASSDEVS = (SETUPDIGETCLASSDEVS*)(GetProcAddress(setupAPI, "SetupDiGetClassDevsA"));
    lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = (SETUPDIGETDEVICEREGISTRYPROPERTY*)(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyA"));
#endif
    lpfnSETUPDIDESTROYDEVICEINFOLIST = (SETUPDIDESTROYDEVICEINFOLIST*)(GetProcAddress(setupAPI, "SetupDiDestroyDeviceInfoList"));
    lpfnSETUPDIENUMDEVICEINFO = (SETUPDIENUMDEVICEINFO*)(GetProcAddress(setupAPI, "SetupDiEnumDeviceInfo"));


    if ((lpfnSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
        (lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL)
        || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
    {
        FreeLibrary(setupAPI);
        return 0;
    }

    //Now create a "device information set" which is required to enumerate all the ports
    guid = GUID_DEVINTERFACE_COMPORT;
    hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
    {
        FreeLibrary(setupAPI);
        return 0;
    }

    //Enumeration
    while (more_item && (port_cnt < max_port))
    {
        //Enumerate the current device
        devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
        more_item = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, item_num, &devInfo);
        if (more_item)
        {
            //Did we find a serial port for this device
            int found_com_port = 0;

            //Get the registry key which stores the ports settings
            HKEY reg_key = lpfnSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
            if (reg_key != INVALID_HANDLE_VALUE)
            {
                int port_num = 0;
                if (util_reg_port_get(reg_key, &port_num))
                {
                    ports[port_cnt] = port_num;
                    found_com_port = 1;
                }

                //Close the key now that we are finished with it
                RegCloseKey(reg_key);
            }

            //If the port was a serial port, then also try to get its friendly name
            if (found_com_port)
            {
                TCHAR friendly_name[1024];
                DWORD dwSize = sizeof(friendly_name)/sizeof(TCHAR);
                DWORD reg_type = 0;

                friendly_name[0] = _T('\0');
                if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &reg_type,
                                                         (PBYTE)friendly_name, dwSize, &dwSize)
                    && (reg_type == REG_SZ))
                {
                    friendly_names[port_cnt] = _tcsdup(friendly_name);
                }
                port_cnt++;
            }
        }
        ++item_num;
    }

    //Free up the "device information set" now that we are finished with it
    lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);
    FreeLibrary(setupAPI);
    *port_count = port_cnt;

    return 1;
}

#endif


/**
util_list_add - add an entry into the end of the list.
@param[in]      mtx_ctx     Mutex context
@param[in,out]	head		Pointer to the list head
@param[in]      buf         Buffer that store the data
@param[in]      dat_sz      Size of data to be stored
@return                     Return 0 on success, negative error number on failure.
*/
int util_list_add(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint8_t dat_sz)
{
    util_lst_t   *ent;     //Pointer to list entry
    util_lst_t   *temp;

    ent = (util_lst_t   *)malloc(sizeof(util_lst_t) + dat_sz - 1);

    if (!ent)
        return ZWHCI_ERROR_MEMORY;

    ent->dat_sz = dat_sz;
    memcpy(ent->wr_buf, buf, dat_sz);

    plt_mtx_lck(mtx_ctx);

    if (*head == NULL)
    {
        ent->next = NULL;
        *head = ent;
        plt_mtx_ulck(mtx_ctx);
        return ZWHCI_NO_ERROR;
    }

    temp = *head;
    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = ent;
    ent->next = NULL;

    plt_mtx_ulck(mtx_ctx);
    return ZWHCI_NO_ERROR;
}


/**
util_list_add_no_dup - add an entry into the end of the list if the content of the buffer is
                       different from the last entry.
@param[in]      mtx_ctx     Mutex context
@param[in,out]	head		Pointer to the list head
@param[in]      buf         Buffer that store the data
@param[in]      dat_sz      Size of data to be stored
@param[in]      cmp_fn      The comparison function to identify the entry is identical with the last entry
@return                     Return 0 on success, 1 if the contents are same; else negative error number on failure.
*/
int util_list_add_no_dup(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint8_t dat_sz, util_list_cmp_fn cmp_fn)
{
    int          ret;
    util_lst_t   *ent;     //Pointer to list entry
    util_lst_t   *temp;

    ent = (util_lst_t   *)malloc(sizeof(util_lst_t) + dat_sz - 1);

    if (!ent)
        return ZWHCI_ERROR_MEMORY;

    ent->dat_sz = dat_sz;
    memcpy(ent->wr_buf, buf, dat_sz);

    plt_mtx_lck(mtx_ctx);

    if (*head == NULL)
    {
        ent->next = NULL;
        *head = ent;
        plt_mtx_ulck(mtx_ctx);
        return ZWHCI_NO_ERROR;
    }

    temp = *head;
    while (temp->next)
    {
        temp = temp->next;
    }

    //Check if last entry is identical with the new entry
    if (cmp_fn(temp->wr_buf, buf) == 0)
    {
        free(ent);
        ret = 1;
    }
    else
    {
        temp->next = ent;
        ent->next = NULL;
        ret = 0;
    }

    plt_mtx_ulck(mtx_ctx);
    return ret;
}


/**
util_list_get - get the entry from the beginning of the list.
@param[in]      mtx_ctx     Mutex context
@param[in, out]	head		List head
@return     The first entry in the list if the list is not empty; otherwise, NULL.
@post       The caller should free the returned entry.
*/
util_lst_t *util_list_get(void *mtx_ctx, util_lst_t **head)
{
    util_lst_t   *first_entry;  //The entry at the beginning of the list

    plt_mtx_lck(mtx_ctx);
    first_entry = *head;

    if (*head == NULL)
    {
        plt_mtx_ulck(mtx_ctx);
        return NULL;
    }

    *head = (*head)->next;
    plt_mtx_ulck(mtx_ctx);
    return first_entry;
}


/**
util_list_flush - flush the list.
@param[in]      mtx_ctx     Mutex context
@param[in, out]	head		List head
@return
*/
void util_list_flush(void *mtx_ctx, util_lst_t **head)
{
    util_lst_t   *first_entry;  //The entry at the beginning of the list
    util_lst_t   *del_entry;    //Entry to be deleted

    plt_mtx_lck(mtx_ctx);

    first_entry = *head;

    while (first_entry)
    {
        del_entry = first_entry;
        first_entry = first_entry->next;
        free(del_entry);
    }

    *head = NULL;
    plt_mtx_ulck(mtx_ctx);
}


/**
util_list_find - find an entry from the list without modifying the list.
@param[in]      mtx_ctx     Mutex context
@param[in]	    head		Pointer to the list head
@param[in]      cmp_fn      The comparison function to identify the entry using the key stored in buf
@param[in,out]  buf         Buffer that stores the key to be searched. On success, this buffer will be
                            returned with found entry.
@return                     Return non-zero on success, zero on failure.
*/
int util_list_find(void *mtx_ctx, util_lst_t *head, uint8_t  *buf, util_list_cmp_fn cmp_fn)
{
    util_lst_t   *temp;

    if (!head)
        return 0;

    plt_mtx_lck(mtx_ctx);

    temp = head;

    while (temp)
    {
        if (cmp_fn(temp->wr_buf, buf) == 0)
        {
            memcpy(buf, temp->wr_buf, temp->dat_sz);
            plt_mtx_ulck(mtx_ctx);
            return 1;
        }
        temp = temp->next;
    }

    plt_mtx_ulck(mtx_ctx);
    return 0;
}


/**
util_list_rm - remove the specified entry from the list.
@param[in]      mtx_ctx     Mutex context
@param[in]	    head		List head
@param[in]      cmp_fn      The comparison function to identify the entry using the key stored in buf
@param[in]      buf         Buffer that stores the key to be searched.
@return                     Return non-zero on success, zero on failure.
*/
int util_list_rm(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, util_list_cmp_fn cmp_fn)
{
    util_lst_t   *cur_ent;
    util_lst_t   *prev_ent;

    if (*head == NULL)
        return 0;

    plt_mtx_lck(mtx_ctx);

    cur_ent = *head;

    if (cmp_fn(cur_ent->wr_buf, buf) == 0)
    {   //List head to be removed
        *head = cur_ent->next;
        free(cur_ent);
        plt_mtx_ulck(mtx_ctx);
        return 1;
    }

    prev_ent = cur_ent;
    cur_ent = cur_ent->next;

    while (cur_ent)
    {
        if (cmp_fn(cur_ent->wr_buf, buf) == 0)
        {
            prev_ent->next = cur_ent->next;
            free(cur_ent);
            plt_mtx_ulck(mtx_ctx);
            return 1;
        }
        prev_ent = cur_ent;
        cur_ent = cur_ent->next;
    }

    plt_mtx_ulck(mtx_ctx);

    return 0;
}


/**
util_list_rplc - replace the specified entry from the list if found; else add the entry to the end of the list.
@param[in]      mtx_ctx     Mutex context
@param[in]	    head		List head
@param[in]      cmp_fn      The comparison function to identify the entry using the key stored in buf
@param[in]      buf         Buffer that stores the entry to be added.
@return     1=entry has been replaced, 0=entry has been added; negative value on error.
*/
int util_list_rplc(void *mtx_ctx, util_lst_t **head, uint8_t  *buf, uint16_t dat_sz, util_list_cmp_fn cmp_fn)
{
    util_lst_t   *cur_ent;
    util_lst_t   *prev_ent = NULL;

    plt_mtx_lck(mtx_ctx);

    if (*head == NULL)
    {
        cur_ent = (util_lst_t *)malloc(sizeof(util_lst_t) + dat_sz - 1);

        if (!cur_ent)
        {
            plt_mtx_ulck(mtx_ctx);
            return ZWHCI_ERROR_MEMORY;
        }

        cur_ent->dat_sz = dat_sz;
        memcpy(cur_ent->wr_buf, buf, dat_sz);

        cur_ent->next = NULL;
        *head = cur_ent;
        plt_mtx_ulck(mtx_ctx);
        return 0;
    }

    cur_ent = *head;

    while (cur_ent)
    {
        if (cmp_fn(cur_ent->wr_buf, buf) == 0)
        {   //Found and replace
            cur_ent->dat_sz = dat_sz;
            memcpy(cur_ent->wr_buf, buf, dat_sz);
            plt_mtx_ulck(mtx_ctx);
            return 1;
        }
        prev_ent = cur_ent;
        cur_ent = cur_ent->next;
    }

    //Add new entry at the end of list
    cur_ent = (util_lst_t *)malloc(sizeof(util_lst_t) + dat_sz - 1);

    if (!cur_ent)
    {
        plt_mtx_ulck(mtx_ctx);
        return ZWHCI_ERROR_MEMORY;
    }

    cur_ent->dat_sz = dat_sz;
    memcpy(cur_ent->wr_buf, buf, dat_sz);

    prev_ent->next = cur_ent;
    cur_ent->next = NULL;
    plt_mtx_ulck(mtx_ctx);
    return 0;
}


/**
util_num_string_add - append an unsigned number to a string
@param[in,out]	src		    The string where an unsigned number is to be appended
@param[in]	    src_size	The size of the src buffer
@param[in]	    num		    The unsigned number that is to be converted to number string
@return
*/
void    util_num_string_add(char *src, unsigned src_size, unsigned num)
{
    char    num_str[20];
#ifdef USE_SAFE_VERSION
    sprintf_s(num_str, 20, "%u, ", num);
    strcat_s(src, src_size, num_str);
#else
    sprintf(num_str, "%u, ", num);
    strcat(src, num_str);
#endif
}


/**
util_hex_string_add - append an hex number to a string
@param[in,out]	src		    The string where an hex number is to be appended
@param[in]	    src_size	The size of the src buffer
@param[in]	    num		    The unsigned number that is to be converted to hex number string
@return
*/
void    util_hex_string_add(char *src, unsigned src_size, unsigned num)
{
    char    num_str[20];
#ifdef USE_SAFE_VERSION
    sprintf_s(num_str, 20, "%02X, ", num);
    strcat_s(src, src_size, num_str);
#else
    sprintf(num_str, "%02X, ", num);
    strcat(src, num_str);
#endif
}


/**
util_ntohs - conversion from big-endian to host byte order
@param[in,out]	src		    The string for conversion from big-endian to host byte order
@param[in]	    elem_cnt	The number of elements (2-byte words) in the src
@return
*/
void    util_ntohs(uint16_t *src, unsigned elem_cnt)
{
    if (!util_is_host_big_endian())
    {
        unsigned i;
        for (i=0; i<elem_cnt; i++)
        {
            *src = (*src << 8) | (*src >> 8);
            src++;
        }
    }
}


/**
util_tmr_exp_chk - Check whether the timer has expired
@param[in] now     Current time
@param[in] timer   The time of the timer to check
@return 1=timer has expired; otherwise 0
*/
int    util_tmr_exp_chk(uint16_t now, uint16_t timer)
{
    uint16_t    time_diff;
    if (now == timer)
    {
        return 1;
    }

    //Handle wrap over case
    if (now > timer)
    {
        time_diff = now - timer;
        if (time_diff < 0xF000)
        {
            return 1;
        }
    }
    else //now < timer
    {
        time_diff = timer - now;
        if (time_diff >= 0xF000)
        {
            return 1;
        }
    }
    //Not expired
    return 0;
}


/**
util_is_host_big_endian - Get host endianess
@return 1 if host is big endian; otherwise 0
*/
int util_is_host_big_endian(void)
{
    union
    {
        long l; char c[sizeof (long)];
    } u;
    u.l = 1;
    return(u.c[sizeof (long) - 1] == 1);
}


#define CMD_CLS_PARSE_MODE_INSECURE     0
#define CMD_CLS_PARSE_MODE_CONTROL      1
#define CMD_CLS_PARSE_MODE_SECURE       2
/**
util_cmd_cls_parse - Parse buffer that stores mixture of 1-byte and 2-byte command classes
@param[in] cmd_buf      Command buffer
@param[in] cmd_buf_len  Size of command buffer
@param[out] cmd_cls     Output insecure command classes without any marker
@param[out] cmd_cnt     Output insecure command classes count
@param[out] cmd_cls_sec Output secure command classes without any marker
@param[out] cmd_cnt_sec Output secure command classes count
@return     Non-zero if no error; otherwise return zero
@post       If return value is non-zero, caller needs to free the cmd_cls and cmd_cls_sec buffers.
*/
int util_cmd_cls_parse(uint8_t *cmd_buf, uint8_t cmd_buf_len, uint16_t **cmd_cls, uint8_t *cmd_cnt,
                       uint16_t **cmd_cls_sec, uint8_t *cmd_cnt_sec)
{
    unsigned    i;
    int         mode; // 0=insecure supported command classes; 1=insecure controlled command classes;
                      // 2=secure supported command classes
    uint16_t    *cls_buf;
    uint16_t    *cls_sec_buf;
    uint8_t     cnt;
    uint8_t     cnt_sec;

    if ((cmd_buf_len == 0) || !cmd_buf)
    {
        return 0;
    }

    //Allocate memory for the buffers
    cls_buf = (uint16_t *)malloc(cmd_buf_len * sizeof(uint16_t));

    if (!cls_buf)
    {
        return 0;
    }

    cls_sec_buf = (uint16_t *)malloc(cmd_buf_len * sizeof(uint16_t));

    if (!cls_sec_buf)
    {
        free(cls_buf);
        return 0;
    }

    //Initialize the parser
    cnt_sec = cnt = i = 0;
    mode = CMD_CLS_PARSE_MODE_INSECURE;

    while (i < cmd_buf_len)
    {
        if (*cmd_buf >= 0xF1)
        {   //Check whether this is the security scheme 0 marker
            if (*(cmd_buf + 1) == 0)
            {   //Change to secure mode
                mode = CMD_CLS_PARSE_MODE_SECURE;

                cmd_buf += 2;
                i += 2;
                continue;
            }

            //Extended command class
            if (mode == CMD_CLS_PARSE_MODE_INSECURE)
            {
                cls_buf[cnt] = *cmd_buf++;
                cls_buf[cnt] = (cls_buf[cnt] << 8) | (*cmd_buf++);
                cnt++;
            }
            else if (mode == CMD_CLS_PARSE_MODE_SECURE)
            {
                cls_sec_buf[cnt_sec] = *cmd_buf++;
                cls_sec_buf[cnt_sec] = (cls_sec_buf[cnt_sec] << 8) | (*cmd_buf++);
                cnt_sec++;
            }
            else
            {
                cmd_buf += 2;
            }

            i += 2;
        }
        else
        {
            //Check whether this is the controlled command class marker
            if (*cmd_buf == 0xEF)
            {   //Change mode
                mode = CMD_CLS_PARSE_MODE_CONTROL;

                cmd_buf++;
                i++;
                continue;
            }


            //Normal command class
            if (mode == CMD_CLS_PARSE_MODE_INSECURE)
            {
                cls_buf[cnt++] = *cmd_buf++;
            }
            else if (mode == CMD_CLS_PARSE_MODE_SECURE)
            {
                cls_sec_buf[cnt_sec++] = *cmd_buf++;
            }
            else
            {
                cmd_buf++;
            }
            i++;
        }
    }
    //Parsing done
    *cmd_cnt = cnt;
    *cmd_cnt_sec = cnt_sec;
    *cmd_cls = cls_buf;
    *cmd_cls_sec = cls_sec_buf;
    return 1;
}


/**
@}
*/



