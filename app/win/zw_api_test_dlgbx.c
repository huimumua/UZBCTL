/**
@file   zw_api_test_dlgbx.c - Z-wave HC API test dialog boxes.

        To test the Z-wave HC API.

@author David Chow

@version    1.0 10-11-11  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include <windowsx.h>
#include <Commdlg.h>
#include "resource.h"
#include "zw_api_test.h"

extern hl_appl_ctx_t    my_appl; // High-level application context
extern HINSTANCE        hInst;   // current instance
extern const char       *zw_alrm_type[];
WNDPROC                 lpfnButtonWndProc;  // default button WndProc

/**
@defgroup HCAPITest Z-wave HC API test
High-level application layer to test the Z-wave HC API.
@{
*/

/**
About - Message handler for about box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_setting_dlgbx - Message handler for settings dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_setting_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {   //Show the current destination node id
                TCHAR       dest_node_num[8];
                _stprintf_s(dest_node_num, sizeof(dest_node_num)/sizeof(TCHAR),
                            _T("%d"), my_appl.dst_desc_id);

                SetDlgItemText(hDlg, IDC_DEST_NODE, dest_node_num);
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the dest node id
                BOOL bSuccess;
                int node_id = GetDlgItemInt(hDlg, IDC_DEST_NODE, &bSuccess, FALSE);

                if (bSuccess)
                {
                    my_appl.dst_desc_id = node_id;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;


    }
    return(INT_PTR)FALSE;
}


/**
hl_stress_test_dlgbx - Message handler for stress test setting.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_stress_test_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Default to multi-level switch
                CheckRadioButton(hDlg, IDC_ST_SW, IDC_ST_SEQ, IDC_ST_SW);
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the stress test selection
                if (IsDlgButtonChecked(hDlg, IDC_ST_SW) == BST_CHECKED)
                    my_appl.stress_tst_func = hl_multi_lvl_snd;
                else
                    my_appl.stress_tst_func = hl_node_updt;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }

            if (LOWORD(wParam) == IDCANCEL)
            {
                my_appl.stress_tst_func = NULL;
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;


    }
    return(INT_PTR)FALSE;
}



/**
hl_failed_id_dlgbx - Message handler for getting failed node id dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_failed_id_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.failed_node_id = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the routing slave node id
                BOOL bSuccess;
                int node_id = GetDlgItemInt(hDlg, IDC_FAILED_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.failed_node_id = node_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_suc_dlgbx - Message handler for set SUC dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
//INT_PTR CALLBACK hl_set_suc_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//        case WM_INITDIALOG:
//            //Init
//            my_appl.suc_node_id = 0;
//            return(INT_PTR)TRUE;
//
//        case WM_COMMAND:
//            if (LOWORD(wParam) == IDOK)
//            {
//                //Get the destination node id
//                BOOL bSuccess;
//                int node_id;
//
//                node_id = GetDlgItemInt(hDlg, IDC_DST_NODE, &bSuccess, FALSE);
//                if (bSuccess)
//                    my_appl.suc_node_id = node_id;
//
//                EndDialog(hDlg, LOWORD(wParam));
//                return(INT_PTR)TRUE;
//            }
//            if (LOWORD(wParam) == IDCANCEL)
//            {
//                EndDialog(hDlg, LOWORD(wParam));
//                return(INT_PTR)TRUE;
//            }
//            break;
//    }
//    return(INT_PTR)FALSE;
//}

/**
hl_rep_setup_dlgbx - Message handler for setup of multi-level switch report dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_rep_setup_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the multi-level switch report receiving node descriptor id
                BOOL bSuccess;
                int node_id = GetDlgItemInt(hDlg, IDC_REPORT_NODE_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = node_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_nameloc_set_dlgbx - Message handler for setting name & location dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_nameloc_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;

            memset(&my_appl.nameloc, 0, sizeof(zw_nameloc_t));
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get node id
                LPWSTR  buf;
                BOOL bSuccess;
				int len;

                int node_id = GetDlgItemInt(hDlg, IDC_LOC_NODE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = node_id;

                //Get name
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NAME));
                if(len > 0)
                {
                    buf = (LPWSTR)calloc(len + 1, sizeof(WCHAR));
                    GetDlgItemText(hDlg, IDC_NAME, buf, len + 1);

                    //Convert utf16 to utf8
                    plt_utf16_to_8((const char *)buf, my_appl.nameloc.name, ZW_LOC_STR_MAX + 1, 0);

                    free(buf);
                }

                //Get location
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_LOCATION));
                if(len > 0)
                {
                    buf = (LPWSTR)calloc(len + 1, sizeof(WCHAR));
                    GetDlgItemText(hDlg, IDC_LOCATION, buf, len + 1);

                    //Convert utf16 to utf8
                    plt_utf16_to_8((const char *)buf, my_appl.nameloc.loc, ZW_LOC_STR_MAX + 1, 0);

                    free(buf);

                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_multi_lvl_chg_dlgbx - Message handler for start changing level dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_multi_lvl_chg_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.mul_lvl_dir = ZWIF_LEVEL_DIR_UP;
            my_appl.mul_lvl_sec_dir = ZWIF_LEVEL_DIR_NO_CHANGE;
            my_appl.mul_lvl_val = 0xFF;
            my_appl.mul_lvl_sec_step = 0xFF;
            my_appl.mul_lvl_dur = 49;

            //Default to up for primary
            CheckRadioButton(hDlg, IDC_ML_UP/*first*/, IDC_ML_NO_CHG/*last*/, IDC_ML_UP/*checked*/);
            //Default to no change for secondary
            CheckRadioButton(hDlg, IDC_SEC_UP/*first*/, IDC_SEC_NO_CHG/*last*/, IDC_SEC_NO_CHG/*checked*/);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination descriptor id
                BOOL bSuccess;
                int desc_id;
                int value;

                desc_id = GetDlgItemInt(hDlg, IDC_ML_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                //Get the primary switch direction
                if (IsDlgButtonChecked(hDlg, IDC_ML_UP) == BST_CHECKED)
                    my_appl.mul_lvl_dir = ZWIF_LEVEL_DIR_UP;
                else if (IsDlgButtonChecked(hDlg, IDC_ML_DOWN) == BST_CHECKED)
                    my_appl.mul_lvl_dir = ZWIF_LEVEL_DIR_DOWN;
                else
                    my_appl.mul_lvl_dir = ZWIF_LEVEL_DIR_NO_CHANGE;

                //Get the primary switch start level
                value = GetDlgItemInt(hDlg, IDC_ML_LVL, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.mul_lvl_val = value;

                //Get the secondary switch direction
                if (IsDlgButtonChecked(hDlg, IDC_SEC_UP) == BST_CHECKED)
                    my_appl.mul_lvl_sec_dir = ZWIF_LEVEL_DIR_UP;
                else if (IsDlgButtonChecked(hDlg, IDC_SEC_DOWN) == BST_CHECKED)
                    my_appl.mul_lvl_sec_dir = ZWIF_LEVEL_DIR_DOWN;
                else
                    my_appl.mul_lvl_sec_dir = ZWIF_LEVEL_DIR_NO_CHANGE;

                //Get the secondary switch step size
                value = GetDlgItemInt(hDlg, IDC_SEC_LVL, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.mul_lvl_sec_step = value;

                //Get the dimming duration
                value = GetDlgItemInt(hDlg, IDC_ML_DUR, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.mul_lvl_dur = value;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_bin_set_dlgbx - Message handler for set binary switch state dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_bin_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.bin_state = 1;
            //Default to on
            CheckRadioButton(hDlg, IDC_BIN_ON, IDC_BIN_OFF, IDC_BIN_ON);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination descriptor id
                BOOL bSuccess;
                int desc_id;

                desc_id = GetDlgItemInt(hDlg, IDC_BIN_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                //Get the direction
                if (IsDlgButtonChecked(hDlg, IDC_BIN_ON) == BST_CHECKED)
                    my_appl.bin_state = 1;
                else if (IsDlgButtonChecked(hDlg, IDC_BIN_OFF) == BST_CHECKED)
                    my_appl.bin_state = 0;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}

/**
hl_grp_get_dlgbx - Message handler for getting group info dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_grp_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the association report receiving node descriptor id
                BOOL bSuccess;
                int group_id;
                int node_id = GetDlgItemInt(hDlg, IDC_GROUP_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;

                group_id = GetDlgItemInt(hDlg, IDC_GROUP_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.group_id = group_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_grp_add_dlgbx - Message handler for adding endpoints into group dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_grp_add_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
		int i;
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            for (i=0; i<5; i++)
            {
                my_appl.ep_desc_id[i] = 0;
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the association report receiving node descriptor id
                BOOL bSuccess;
                int ep_id;
                int group_id;

                int node_id = GetDlgItemInt(hDlg, IDC_GROUP_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;

                group_id = GetDlgItemInt(hDlg, IDC_GROUP_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.group_id = group_id;

                for (i=0; i<5; i++)
                {
                    ep_id = GetDlgItemInt(hDlg, IDC_GRP_EP1 + i, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.ep_desc_id[i] = ep_id;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_wkup_set_dlgbx - Message handler for setting wake up interval and alert receiving node dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_wkup_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.node_desc_id = 0;
            my_appl.wkup_interval = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int desc_id;
                int interval;

                desc_id = GetDlgItemInt(hDlg, IDC_WKUP_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                desc_id = GetDlgItemInt(hDlg, IDC_WKUP_NODE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.node_desc_id = desc_id;

                interval = GetDlgItemInt(hDlg, IDC_WKUP_INTERVAL, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.wkup_interval = interval;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_multi_lvl_set_dlgbx - Message handler for setting multi-level switch level dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_multi_lvl_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.mul_lvl_val = 0xFFFF;
            my_appl.mul_lvl_dur = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                unsigned        hex_val;

                //Get hex string for level
                if(hl_hex_get(hDlg, IDC_ML_LVL, &hex_val))
                {
                    my_appl.mul_lvl_val = hex_val;
                }

                //Get hex string for duration
                if(hl_hex_get(hDlg, IDC_ML_DUR, &hex_val))
                {
                    my_appl.mul_lvl_dur = hex_val;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_grp_cmd_set_dlgbx - Message handler for set cmd to node in group dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_grp_cmd_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
		int i;
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.group_id = 0;
            my_appl.node_desc_id = 0;
            my_appl.cmd_len = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL        bSuccess;
                int         node_desc_id;
                int         group_id;
                unsigned    hex_val;

                int node_id = GetDlgItemInt(hDlg, IDC_GROUP_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;

                group_id = GetDlgItemInt(hDlg, IDC_GROUP_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.group_id = group_id;

                node_desc_id = GetDlgItemInt(hDlg, IDC_NODE1, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.node_desc_id = node_desc_id;

                for (i=0; i<5; i++)
                {
                    //Get hex string for duration
                    if(hl_hex_get(hDlg, IDC_CMD1 + i, &hex_val))
                    {
                        my_appl.command[my_appl.cmd_len++] = hex_val;
                    }
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_grp_cmd_get_dlgbx - Message handler for get command record for a node within a given grouping identifier dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_grp_cmd_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.group_id = 0;
            my_appl.node_id = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL        bSuccess;
                int         node_id;
                int         group_id;

                node_id = GetDlgItemInt(hDlg, IDC_GROUP_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;

                group_id = GetDlgItemInt(hDlg, IDC_GROUP_ID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.group_id = group_id;

                node_id = GetDlgItemInt(hDlg, IDC_NODE1, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.node_id = node_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_cfg_set_dlgbx - Message handler for setting configuration parameter dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_cfg_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.cfg_param = 0;
            my_appl.cfg_size = 0;
            my_appl.cfg_value = 0;
            my_appl.cfg_value_default = 1;//use device default value
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int value;

                int node_id = GetDlgItemInt(hDlg, IDC_DEST_NODE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;

                value = GetDlgItemInt(hDlg, IDC_PARAM_NUM, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.cfg_param = value;

                value = GetDlgItemInt(hDlg, IDC_PARAM_SIZE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.cfg_size = value;

                value = GetDlgItemInt(hDlg, IDC_PARAM_VALUE, &bSuccess, TRUE);//get signed integer
                if (bSuccess)
                {
                    my_appl.cfg_value = value;
                    my_appl.cfg_value_default = 0;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_cfg_get_dlgbx - Message handler for getting configuration parameter dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_cfg_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            my_appl.cfg_param = 0;
            my_appl.cfg_range_start = 0;
            my_appl.cfg_range_end = 0;
            my_appl.cfg_param_mode = 0;

            //Default to single parameter
            CheckRadioButton(hDlg, IDC_PARAM_SINGLE/*first*/, IDC_PARAM_RANGE/*last*/, IDC_PARAM_SINGLE/*checked*/);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int value;

                int node_id = GetDlgItemInt(hDlg, IDC_DEST_NODE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = node_id;


                //Get the parameter mode selection
                if (IsDlgButtonChecked(hDlg, IDC_PARAM_SINGLE) == BST_CHECKED)
                {
                    my_appl.cfg_param_mode = 0;

                    value = GetDlgItemInt(hDlg, IDC_PARAM_NUM, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.cfg_param = value;
                }
                else if (IsDlgButtonChecked(hDlg, IDC_PARAM_RANGE) == BST_CHECKED)
                {
                    my_appl.cfg_param_mode = 1;

                    value = GetDlgItemInt(hDlg, IDC_PARAM_NUM_START, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.cfg_range_start = value;

                    value = GetDlgItemInt(hDlg, IDC_PARAM_NUM_END, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.cfg_range_end = value;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_cmd_q_ctl_dlgbx - Message handler for commmand queuing control dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_cmd_q_ctl_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            CheckRadioButton(hDlg, IDC_Q_ON, IDC_Q_OFF, (my_appl.cmd_q_ctl)? IDC_Q_ON : IDC_Q_OFF);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination descriptor id
                BOOL bSuccess;
                int desc_id;

                desc_id = GetDlgItemInt(hDlg, IDC_BIN_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                //Get the direction
                if (IsDlgButtonChecked(hDlg, IDC_Q_ON) == BST_CHECKED)
                    my_appl.cmd_q_ctl = 1;
                else if (IsDlgButtonChecked(hDlg, IDC_Q_OFF) == BST_CHECKED)
                    my_appl.cmd_q_ctl = 0;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_admin_set_dlgbx - Message handler for setting meter admin name dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_admin_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;

            memset(my_appl.meter_admin, 0, ZW_ADMIN_STR_MAX + 1);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get node id
                BOOL bSuccess;
				int len;

                int desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get name
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_NAME));
                if(len > 0)
                {
                    char *buf;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_NAME, buf, len + 1);
#ifdef USE_SAFE_VERSION
                    strncpy_s(my_appl.meter_admin, ZW_ADMIN_STR_MAX+1, buf, _TRUNCATE);
#else
                    strncpy(my_appl.meter_admin, buf, ZW_ADMIN_STR_MAX);
#endif
                    free(buf);
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
av_button_proc - Windows procedure to handler button messages
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
*/
LRESULT CALLBACK av_button_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
            {
                WPARAM wParam;
                wParam = (message == WM_LBUTTONDOWN)? BN_PUSHED : BN_UNPUSHED;
                wParam = (wParam << 16) | (GetWindowID(hwnd) & 0xFFFF);
                SendMessage(GetParent(hwnd), WM_NOTIFY, wParam, 0L);
            }
            break;

    }
    return CallWindowProc(lpfnButtonWndProc, hwnd, message, wParam, lParam);
}


/**
hl_av_set - Send the status of AV button
@param[in]	hl_appl		The high-level api context
@param[in]	ctl		    button number ZW_BUTTON_XX
@param[in]	down	    0=button up, else button down
@return  0 on success, negative error number on failure
*/
static int32_t hl_av_set(hl_appl_ctx_t   *hl_appl, uint16_t ctl, uint8_t down)
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
hl_av_set_dlgbx - Message handler for sending AV button message dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_av_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //UNREFERENCED_PARAMETER(lParam);
    static HWND button_handler[3];
    WNDPROC  lpfnMenuButtonProc;
    WNDCLASS wndclass;

    switch (message)
    {
        case WM_INITDIALOG:
            //Init

            lpfnMenuButtonProc = (WNDPROC)MakeProcInstance((FARPROC)av_button_proc, hInst);
            GetClassInfo((HINSTANCE)NULL, L"BUTTON", &wndclass);    /* get default button class info */
            lpfnButtonWndProc = wndclass.lpfnWndProc;
            //Create Play button
            button_handler[0] = CreateWindow( L"BUTTON", L"Play",
                                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, //BS_OWNERDRAW,
                                           22*2, 66*2, 28*2, 14*2,
                                           hDlg,
                                           (HMENU) IDC_PLAY,
                                           hInst, NULL );
            SetWindowLong(button_handler[0], GWL_WNDPROC, (LONG)lpfnMenuButtonProc);

            //Create Pause button
            button_handler[1] = CreateWindow( L"BUTTON", L"Pause",
                                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                           75*2, 66*2, 28*2, 14*2,
                                           hDlg,
                                           (HMENU) IDC_PAUSE,
                                           hInst, NULL );
            SetWindowLong(button_handler[1], GWL_WNDPROC, (LONG)lpfnMenuButtonProc);

            //Create Stop button
            button_handler[2] = CreateWindow( L"BUTTON", L"Stop",
                                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                           127*2, 66*2, 28*2, 14*2,
                                           hDlg,
                                           (HMENU) IDC_STOP,
                                           hInst, NULL );
            SetWindowLong(button_handler[2], GWL_WNDPROC, (LONG)lpfnMenuButtonProc);

            return(INT_PTR)TRUE;


        case WM_NOTIFY:
            {
                switch (HIWORD(wParam))
                {
                    case BN_PUSHED:
                    case BN_UNPUSHED:
                        {
                            //Get node id
                            BOOL bSuccess;

                            int desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                            if (bSuccess)
                            {
                                my_appl.rep_desc_id = desc_id;
                                //Send button state message
                                hl_av_set(&my_appl, LOWORD(wParam), (HIWORD(wParam) == BN_PUSHED)? 1 : 0 );
                            }

                        }
                        break;

                }

            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                //Destroy buttons
                int i;
                for (i=0; i<3; i++)
                {
                    DestroyWindow(button_handler[i]);
                }
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            //Send button up message
            if (LOWORD(wParam) >= IDC_PLAY && LOWORD(wParam) <= IDC_PAUSE)
            {
                //Get node id
                BOOL bSuccess;

                int desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                {
                    my_appl.rep_desc_id = desc_id;
                    hl_av_set(&my_appl, LOWORD(wParam), 0);
                }
                return(INT_PTR)TRUE;
            }
            break;

    }
    return(INT_PTR)FALSE;
}


/**
hl_meter_get_dlgbx - Message handler for getting meter reading dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_meter_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get node id
                BOOL bSuccess;
                int preferred_unit;

                int desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                preferred_unit = GetDlgItemInt(hDlg, IDC_UNIT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.meter_unit = preferred_unit;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}

int hl_open_file_name_get(HWND hwnd, TCHAR *szFile, int buf_size, DWORD filter_idx)
{
    OPENFILENAME    ofn;       // common dialog box structure
    TCHAR           curDir[MAX_PATH];
    DWORD           ret;
    //TCHAR open_file_name[280];

    //Save the current directory
    ret = GetCurrentDirectory(MAX_PATH, curDir);

    if (ret == 0)
    {
        return 0;
    }
    else if (ret > MAX_PATH)
    {   //Buffer not big enough
        return 0;
    }


    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    szFile[0] = _T('\0');
    ofn.nMaxFile = buf_size;
    ofn.lpstrFilter = _T("All(*.*)\0*.*\0Node Info(*.ni)\0*.ni\0Binary File(*.bin)\0*.bin\0");
    ofn.nFilterIndex = filter_idx;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST; // | OFN_FILEMUSTEXIST;

// Display the Open dialog box.

    if (GetOpenFileName(&ofn)==TRUE)
    {
/*
#ifdef  _UNICODE
        //Convert UNICODE to ANSI char
        WideCharToMultiByte(CP_ACP, 0, open_file_name, -1, szFile,
                            buf_size, NULL, NULL);
#else
        strcpy(szFile, open_file_name);
#endif
*/
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    //Restore current directory
    SetCurrentDirectory(curDir);

    return ret;

/*
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);
*/
}


/**
hl_init_dlgbx - Message handler for initialization setting.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
HBITMAP hbmpOpenFile;    // handle to bitmap
INT_PTR CALLBACK hl_init_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            {
                hbmpOpenFile = LoadBitmap(hInst, MAKEINTRESOURCE(OPEN_FILE_BITMAP));
                // Set the button image
                SendMessage(GetDlgItem(hDlg, IDC_OPEN_FILE),  BM_SETIMAGE,
                            IMAGE_BITMAP, (LPARAM)hbmpOpenFile);

                //Set default comm port to comm 1
                SetDlgItemText(hDlg, IDE_COMM_NUMBER, _T("1"));

                //Set default to using node info file
                CheckRadioButton(hDlg, IDC_FROM_FILE, IDC_FROM_NW, IDC_FROM_FILE);

                //Set default to save node info file on exit
                CheckDlgButton(hDlg, IDC_SAVE_EXIT, BST_CHECKED);
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the comm port number
                BOOL bSuccess;

                int port_num = GetDlgItemInt(hDlg, IDE_COMM_NUMBER, &bSuccess, FALSE);

                if (bSuccess)
                {
                    my_appl.port_number = port_num;
                }

                //Get node info source
                if (IsDlgButtonChecked(hDlg, IDC_FROM_FILE) == BST_CHECKED)
                    my_appl.load_ni_file = 1;
                else
                    my_appl.load_ni_file = 0;

                //Get node info exit option
                if (IsDlgButtonChecked(hDlg, IDC_SAVE_EXIT) == BST_CHECKED)
                    my_appl.save_ni_file = 1;
                else
                    my_appl.save_ni_file = 0;

                //Get file name
                if (my_appl.load_ni_file || my_appl.save_ni_file)
                {
                    int len;

                    if (my_appl.node_info_file)
                    {
                        free(my_appl.node_info_file);
                        my_appl.node_info_file = NULL;
                    }
                    //Get name
                    len = GetWindowTextLength(GetDlgItem(hDlg, IDE_FILE_NAME));
                    if (len > 0)
                    {
                        char *buf;

                        buf = (char *)calloc(1, len + 1);
                        GetDlgItemTextA(hDlg, IDE_FILE_NAME, buf, len + 1);
                        my_appl.node_info_file = buf;;
                    }
                }

                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                my_appl.port_number = 0;

                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_OPEN_FILE)//open file button
            {
                TCHAR szFile[260];

                if(hl_open_file_name_get(hDlg, szFile, 260, 2))
                {
                    SetDlgItemText(hDlg, IDE_FILE_NAME, szFile);
                }

                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_save_dlgbx - Message handler for save settings into a file.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_save_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            {
                if (my_appl.save_file)
                {
                    free(my_appl.save_file);
                    my_appl.save_file = NULL;
                }

                hbmpOpenFile = LoadBitmap(hInst, MAKEINTRESOURCE(OPEN_FILE_BITMAP));
                // Set the button image
                SendMessage(GetDlgItem(hDlg, IDC_OPEN_FILE),  BM_SETIMAGE,
                            IMAGE_BITMAP, (LPARAM)hbmpOpenFile);
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get file name
                int len;

                //Get name
                len = GetWindowTextLength(GetDlgItem(hDlg, IDE_FILE_NAME));
                if (len > 0)
                {
                    char *buf;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDE_FILE_NAME, buf, len + 1);
                    my_appl.save_file = buf;;
                }
                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {

                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_OPEN_FILE)//open file button
            {
                TCHAR szFile[260];

                if(hl_open_file_name_get(hDlg, szFile, 260, 2))
                {
                    SetDlgItemText(hDlg, IDE_FILE_NAME, szFile);
                }

                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_basic_set_dlgbx - Message handler for basic command set dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_basic_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.basic_val = 0xFFFF;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                unsigned        hex_val;

                //Get hex string for level
                if(hl_hex_get(hDlg, IDC_BASIC_VAL, &hex_val))
                {
                    my_appl.basic_val = hex_val;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_dlck_mode_dlgbx - Message handler for set door lock operation mode dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_dlck_mode_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            //Default to door unsecured
            CheckRadioButton(hDlg, IDC_DLCK_MODE1, IDC_DLCK_MODE7, IDC_DLCK_MODE1);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination interface descriptor id
                BOOL bSuccess;
                int desc_id;
                int  i;

                uint8_t op_mode[] = { ZW_DOOR_UNSEC, ZW_DOOR_UNSEC_TMOUT,
                    ZW_DOOR_UNSEC_IN, ZW_DOOR_UNSEC_IN_TMOUT,
                    ZW_DOOR_UNSEC_OUT, ZW_DOOR_UNSEC_OUT_TMOUT,
                    ZW_DOOR_SEC
                };

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the mode
                for (i=0; i<7; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_DLCK_MODE1 + i) == BST_CHECKED)
                    {
                        my_appl.dlck_mode = op_mode[i];
                        break;
                    }

                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_dlck_cfg_dlgbx - Message handler for configuration of door lock dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_dlck_cfg_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            //Default to constant operation
            CheckRadioButton(hDlg, IDC_CONST, IDC_TIMED, IDC_CONST);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination interface descriptor id
                BOOL bSuccess;
                int desc_id;
                int  i;

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the operation type
                my_appl.dlck_config.type = (IsDlgButtonChecked(hDlg, IDC_CONST) == BST_CHECKED)?
                                            ZW_DOOR_OP_CONST : ZW_DOOR_OP_TIMED;

                //Outside Door Handles State

                my_appl.dlck_config.out_sta = 0;

                for (i=0; i<4; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_OUT_HANDLE1 + i) == BST_CHECKED)
                    {
                        my_appl.dlck_config.out_sta |= (0x01 << i);
                    }

                }

                //Inside Door Handles State

                my_appl.dlck_config.in_sta = 0;

                for (i=0; i<4; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_IN_HANDLE1 + i) == BST_CHECKED)
                    {
                        my_appl.dlck_config.in_sta |= (0x01 << i);
                    }

                }

                //Get timeout
                if (my_appl.dlck_config.type == ZW_DOOR_OP_CONST)
                {
                    my_appl.dlck_config.tmout_min = 0xFE;
                    my_appl.dlck_config.tmout_sec = 0xFE;
                }
                else
                {
                    //Set to default value
                    my_appl.dlck_config.tmout_min = 1;
                    my_appl.dlck_config.tmout_sec = 1;

                    i = GetDlgItemInt(hDlg, IDC_MINUTE, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.dlck_config.tmout_min = i;

                    i = GetDlgItemInt(hDlg, IDC_SECOND, &bSuccess, FALSE);
                    if (bSuccess)
                        my_appl.dlck_config.tmout_sec = i;

                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_get_usrcod_dlgbx - Message handler for get user code dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_get_usrcod_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination interface descriptor id
                BOOL bSuccess;
                int  desc_id;
                int  usr_id;

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                usr_id = GetDlgItemInt(hDlg, IDC_USRID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.usr_id = usr_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }

            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_usrcod_dlgbx - Message handler for set user code dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_usrcod_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.rep_desc_id = 0;
            //Default to available (not set)
            CheckRadioButton(hDlg, IDC_USRID_STA1, IDC_USRID_STA4, IDC_USRID_STA1);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination interface descriptor id
                BOOL bSuccess;
                int  desc_id, usr_id;
                int  i;
				int  len;

                uint8_t id_sts[] = { ZW_USRCOD_AVAIL, ZW_USRCOD_UNAVAIL,
                    ZW_USRCOD_RSVD, ZW_USRCOD_NO_STS
                };

                //Get descriptor
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get user id
                usr_id = GetDlgItemInt(hDlg, IDC_USRID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.usr_code.id = usr_id;

                //Get the user id status
                for (i=0; i<4; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_USRID_STA1 + i) == BST_CHECKED)
                    {
                        my_appl.usr_code.id_sts = id_sts[i];
                        break;
                    }
                }

                //Get user code
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_USRCOD));
                if(len > 0)
                {
                    char *buf;

                    if (len > MAX_USRCOD_LENGTH)
                    {
                        len = MAX_USRCOD_LENGTH;
                    }

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_USRCOD, buf, len + 1);
                    memcpy(my_appl.usr_code.code, buf, len);
                    my_appl.usr_code.code_len = len;
/*
                    my_appl.usr_code.code[0] = 1;
                    my_appl.usr_code.code[1] = 1;
                    my_appl.usr_code.code[2] = 2;
                    my_appl.usr_code.code[3] = 2;
                    my_appl.usr_code.code_len = 4;
*/
                    free(buf);
                }
                else
                {
                    memset(my_appl.usr_code.code, '0', MAX_USRCOD_LENGTH);
                    my_appl.usr_code.code_len = MAX_USRCOD_LENGTH;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_get_alrm_dlgbx - Message handler for get alarm status dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_get_alrm_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                int index, i;
                //Init
                my_appl.rep_desc_id = 0;
                my_appl.alrm_vtype = 0;
                my_appl.alrm_evt = 0;

                //Populate alarm type list box
                index = SendDlgItemMessageA(hDlg, IDC_ALRM_LIST, LB_ADDSTRING, 0, (LPARAM)"First alarm detection");

                for (i=ZW_ALRM_SMOKE; i<=ZW_ALRM_CLOCK; i++)
                {
                    index = SendDlgItemMessageA(hDlg, IDC_ALRM_LIST, LB_ADDSTRING, 0, (LPARAM)zw_alrm_type[i]);
                }
                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hList;
                BOOL bSuccess;
                int  desc_id;
                int  i, sel;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get vendor specific alarm type
                i = GetDlgItemInt(hDlg, IDC_ALRM_TYPE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.alrm_vtype = i;

                //Get alarm event
                i = GetDlgItemInt(hDlg, IDC_EVENT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.alrm_evt = i;

                //Get the z-wave alarm type
                hList = GetDlgItem(hDlg, IDC_ALRM_LIST);
                sel = SendMessage(hList, LB_GETCURSEL, 0, 0);

                if (sel == LB_ERR)
                {   //If error or no selection, don't use z-wave alarm type
                    //(i.e. use version 1 format)
                    sel = 0;
                }
                else if (sel == 0)
                {   //Use the first alarm detection
                    sel = ZW_ALRM_FIRST;
                }

                my_appl.alrm_ztype = sel;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_alrm_dlgbx - Message handler for setting alarm activity dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_alrm_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                int index, i;
                //Init
                my_appl.rep_desc_id = 0;
                my_appl.alrm_ztype = 0;

                //Populate alarm type list box
                for (i=ZW_ALRM_SMOKE; i<=ZW_ALRM_CLOCK; i++)
                {
                    index = SendDlgItemMessageA(hDlg, IDC_ALRM_LIST, LB_ADDSTRING, 0, (LPARAM)zw_alrm_type[i]);
                }

                //Default to on
                CheckRadioButton(hDlg, IDC_ALRM_ON, IDC_ALRM_OFF, IDC_ALRM_ON);
                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hList;
                BOOL bSuccess;
                int  desc_id;
                int  sel;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the z-wave alarm type
                hList = GetDlgItem(hDlg, IDC_ALRM_LIST);
                sel = SendMessage(hList, LB_GETCURSEL, 0, 0);

                if (sel == LB_ERR)
                {   //No selection was made
                    my_appl.alrm_ztype = 0;
                }
                else
                {
                    my_appl.alrm_ztype = sel + 1;
                }

                //Get the activity setting
                my_appl.alrm_sts = (IsDlgButtonChecked(hDlg, IDC_ALRM_ON) == BST_CHECKED)? 0xFF : 0;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_get_alrm_evt_dlgbx - Message handler for get supported alarm events dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_get_alrm_evt_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                int index, i;
                //Init
                my_appl.rep_desc_id = 0;
                my_appl.alrm_ztype = 0;

                //Populate alarm type list box
                for (i=ZW_ALRM_SMOKE; i<=ZW_ALRM_CLOCK; i++)
                {
                    index = SendDlgItemMessageA(hDlg, IDC_ALRM_LIST, LB_ADDSTRING, 0, (LPARAM)zw_alrm_type[i]);
                }


                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hList;
                BOOL bSuccess;
                int  desc_id;
                int  sel;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the z-wave alarm type
                hList = GetDlgItem(hDlg, IDC_ALRM_LIST);
                sel = SendMessage(hList, LB_GETCURSEL, 0, 0);

                if (sel == LB_ERR)
                {   //If error, use the first alarm type
                    sel = ZW_ALRM_SMOKE;
                }

                my_appl.alrm_ztype = sel + 1;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_thrmo_fan_md_dlgbx - Message handler for setting thermostat fan mode dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_thrmo_fan_md_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.thrmo_fan_md = 0xFF;

                //Default to auto low
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO8, IDC_RADIO1);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the mode
                for (i=0; i<9; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        if (i == 0)
                        {   //Turn off fan
                            my_appl.thrmo_fan_off = 1;
                            my_appl.thrmo_fan_md = 0;
                        }
                        else
                        {   //Other mode
                            my_appl.thrmo_fan_off = 0;
                            my_appl.thrmo_fan_md = i - 1;

                        }
                        break;
                    }

                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_thrmo_md_dlgbx - Message handler for setting thermostat mode dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_thrmo_md_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.thrmo_md = 0xFF;

                //Default to off
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO13, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the mode
                for (i=0; i<14; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        my_appl.thrmo_md = i;
                        break;
                    }

                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_thrmo_setb_sta_dlgbx - Message handler for setting thermostat setback state dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_thrmo_setb_sta_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.thrmo_setb_sta = 0xFF;

                //Default to no override
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO2, IDC_RADIO0);

                //Default to setback in degrees
                CheckRadioButton(hDlg, IDC_RADIO5, IDC_RADIO8, IDC_RADIO5);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;
				int  len;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the setback type
                for (i=0; i<3; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        my_appl.thrmo_setb_typ = i;
                        break;
                    }
                }

                //Get the setback state
                for (i=0; i<4; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO5 + i) == BST_CHECKED)
                    {
                        my_appl.thrmo_setb_sta = i;
                        break;
                    }
                }

                //Get setback temperature
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_SETB_DEG));
                if(len > 0)
                {
                    char    *buf;
                    char    *decimal_pt;
                    int     degree;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_SETB_DEG, buf, len + 1);

                    if ((decimal_pt = strchr(buf, '.')) != NULL)
                    {
                        char first_decimal_digit;

                        first_decimal_digit = *(decimal_pt + 1);
                        if(isdigit(first_decimal_digit))
                        {
                            *decimal_pt++ = first_decimal_digit;
                        }
                        else
                        {
                            *decimal_pt++ = '0';
                        }
                        *decimal_pt = '\0';
                        degree = atoi(buf);
                        my_appl.thrmo_setb_deg = degree;

                    }
                    else //integer
                    {
                        degree = atoi(buf);
                        my_appl.thrmo_setb_deg = degree * 10;

                    }
                    free(buf);
                }
                else
                {
                    my_appl.thrmo_setb_deg = 0;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_get_thrmo_setp_dlgbx - Message handler for getting thermostat setpoint dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_get_thrmo_setp_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.thrmo_setp_typ = 0xFF;

                //Default to heating
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO8, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the setpoint type
                for (i=0; i<9; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        if (i < 2)
                        {
                            i++;
                        }
                        else
                        {
                            i += 5;
                        }
                        my_appl.thrmo_setp_typ = i;
                        break;
                    }
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_thrmo_setp_dlgbx - Message handler for setting thermostat setpoint dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_thrmo_setp_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                //Init
                my_appl.rep_desc_id = 0;
                my_appl.thrmo_setp_data.type = 0xFF;

                //Default to heating
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO8, IDC_RADIO0);

                hCombo = GetDlgItem(hDlg, IDC_COMBO_UNIT);

                //Populate setpoint type list box
                ComboBox_ResetContent(hCombo);
                ComboBox_AddString(hCombo, L"Celsius");
                ComboBox_AddString(hCombo, L"Fahrenheit");

                //Default to Celsius
                ComboBox_SetCurSel(hCombo, 0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  i, sel;
				int  len;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the setpoint type
                for (i=0; i<9; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        if (i < 2)
                        {
                            i++;
                        }
                        else
                        {
                            i += 5;
                        }
                        my_appl.thrmo_setp_data.type = i;
                        break;
                    }
                }

                //Get the setpoint unit
                hCombo = GetDlgItem(hDlg, IDC_COMBO_UNIT);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.thrmo_setp_data.unit = sel;

                //Get setpoint temperature
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_SETP_VAL));
                if(len > 0)
                {
                    char    *buf;
                    char    *decimal_pt;
                    int     degree;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_SETP_VAL, buf, len + 1);

                    if ((decimal_pt = strchr(buf, '.')) != NULL)
                    {
                        char first_decimal_digit;

                        first_decimal_digit = *(decimal_pt + 1);
                        if(isdigit(first_decimal_digit))
                        {
                            *decimal_pt++ = first_decimal_digit;
                        }
                        else
                        {
                            *decimal_pt++ = '0';
                        }
                        *decimal_pt = '\0';
                        degree = atoi(buf);
                        //Use 2-byte format with 1 decimal point
                        my_appl.thrmo_setp_data.data[0] = (uint8_t) ((degree >> 8) & 0xFF) ;
                        my_appl.thrmo_setp_data.data[1] = (uint8_t) (degree & 0xFF);

                        my_appl.thrmo_setp_data.precision = 1;
                        my_appl.thrmo_setp_data.size = 2;

    /*
                        //Use 4-byte format
                        my_appl.thrmo_setp_data.data[0] = (uint8_t) (degree >> 24);
                        my_appl.thrmo_setp_data.data[1] = (uint8_t) ((degree >> 16) & 0xFF) ;
                        my_appl.thrmo_setp_data.data[2] = (uint8_t) ((degree >> 8) & 0xFF) ;
                        my_appl.thrmo_setp_data.data[3] = (uint8_t) (degree & 0xFF);

                        my_appl.thrmo_setp_data.precision = 1;
                        my_appl.thrmo_setp_data.size = 4;
    */

                    }
                    else //integer
                    {
                        degree = atoi(buf);
                        //Use 1-byte format
                        my_appl.thrmo_setp_data.data[0] = (uint8_t) (degree & 0xFF);

                        my_appl.thrmo_setp_data.precision = 0;
                        my_appl.thrmo_setp_data.size = 1;
                    }

                    free(buf);
                }
                else
                {   //Default to zero
                    my_appl.thrmo_setp_data.data[0] = 0;
                    my_appl.thrmo_setp_data.precision = 0;
                    my_appl.thrmo_setp_data.size = 1;
                }



                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}

extern const char *day_str[];

/**
hl_clock_set_dlgbx - Message handler for setting clock dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_clock_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                int i;

                //Init
                my_appl.weekday = 0xFF;

                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);

                //Populate weekday list box
                ComboBox_ResetContent(hCombo);

                ComboBox_AddString(hCombo, L"Unused");
                for (i=1; i<8; i++)
                {
                    SendDlgItemMessageA(hDlg, IDC_COMBO_DAY, CB_ADDSTRING, 0, (LPARAM)day_str[i]);
                }

                //Default to Monday
                ComboBox_SetCurSel(hCombo, 1);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  i, sel;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get hour and minute
                i = GetDlgItemInt(hDlg, IDC_HOUR, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.hour = i;

                i = GetDlgItemInt(hDlg, IDC_MINUTE, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.minute = i;

                //Get weekday
                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.weekday = sel;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_clmt_ctl_schd_get_dlgbx - Message handler for getting climate control schedule dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_clmt_ctl_schd_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                int i;

                //Init
                my_appl.weekday = 0xFF;

                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);

                //Populate weekday list box
                ComboBox_ResetContent(hCombo);

                ComboBox_AddString(hCombo, L"Select day");
                for (i=1; i<8; i++)
                {
                    SendDlgItemMessageA(hDlg, IDC_COMBO_DAY, CB_ADDSTRING, 0, (LPARAM)day_str[i]);
                }

                //Default to "Select day"
                ComboBox_SetCurSel(hCombo, 0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  sel;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get weekday
                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.weekday = sel;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


extern const char *thrmo_setb_sta_str[];
/**
hl_clmt_ctl_schd_set_dlgbx - Message handler for setting climate control schedule dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_clmt_ctl_schd_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                int i;
                int j;

                //Init
                my_appl.clmt_ctl_schd.weekday = 0xFF;

                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);

                //Populate weekday list box
                ComboBox_ResetContent(hCombo);

                ComboBox_AddString(hCombo, L"Select day");
                for (i=1; i<8; i++)
                {
                    SendDlgItemMessageA(hDlg, IDC_COMBO_DAY, CB_ADDSTRING, 0, (LPARAM)day_str[i]);
                }

                //Default to "Select day"
                ComboBox_SetCurSel(hCombo, 0);

                //Populate schedule state list box
                for (i=0; i<9; i++)
                {
                    for (j=0; j<4; j++)
                    {
                        SendDlgItemMessageA(hDlg, IDC_COMBO_STA0 + i, CB_ADDSTRING, 0, (LPARAM)thrmo_setb_sta_str[j]);
                    }
                    //Default to "Unused"
                    ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COMBO_STA0 + i), 3);

                }


                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  i, sel, num;
                int  len;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get weekday
                hCombo = GetDlgItem(hDlg, IDC_COMBO_DAY);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.clmt_ctl_schd.weekday = sel;

                //Get Switchpoints
                my_appl.clmt_ctl_schd.total = 0;

                for (i=0; i<9; i++)
                {
                    hCombo = GetDlgItem(hDlg, IDC_COMBO_STA0 + i);
                    sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                    if (sel == 3)
                    {   //"Unused" was selected
                        break;
                    }

                    my_appl.clmt_ctl_schd.swpts[i].state = sel;
                    my_appl.clmt_ctl_schd.total++;

                    //Get start time
                    num = GetDlgItemInt(hDlg, IDC_HOUR0 + i, &bSuccess, FALSE);
                    my_appl.clmt_ctl_schd.swpts[i].hour = (bSuccess)? num : 0;

                    num = GetDlgItemInt(hDlg, IDC_MIN0 + i, &bSuccess, FALSE);
                    my_appl.clmt_ctl_schd.swpts[i].minute = (bSuccess)? num : 0;

                    if (sel == ZW_THRMO_SETB_STA_SETB)
                    {
                        //Get setback temperature
                        len = GetWindowTextLength(hCombo);
                        if(len > 0)
                        {
                            char    *buf;
                            char    *decimal_pt;
                            int     degree;

                            buf = (char *)calloc(1, len + 1);
                            GetDlgItemTextA(hDlg, IDC_DEG0 + i, buf, len + 1);

                            if ((decimal_pt = strchr(buf, '.')) != NULL)
                            {
                                char first_decimal_digit;

                                first_decimal_digit = *(decimal_pt + 1);
                                if(isdigit(first_decimal_digit))
                                {
                                    *decimal_pt++ = first_decimal_digit;
                                }
                                else
                                {
                                    *decimal_pt++ = '0';
                                }
                                *decimal_pt = '\0';
                                degree = atoi(buf);
                                my_appl.clmt_ctl_schd.swpts[i].tenth_deg = degree;

                            }
                            else //integer
                            {
                                degree = atoi(buf);
                                my_appl.clmt_ctl_schd.swpts[i].tenth_deg = degree * 10;

                            }
                            free(buf);
                        }
                        else
                        {
                            my_appl.clmt_ctl_schd.swpts[i].tenth_deg = 0;
                        }
                    }
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_clmt_ctl_schd_ovr_dlgbx - Message handler for setting climate control schedule override dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_clmt_ctl_schd_ovr_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.clmt_ctl_schd_ovr.state = 0xFF;

                //Default to no override
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO2, IDC_RADIO0);

                //Default to setback in degrees
                CheckRadioButton(hDlg, IDC_RADIO5, IDC_RADIO8, IDC_RADIO5);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;
				int  len;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the setback type
                for (i=0; i<3; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        my_appl.clmt_ctl_schd_ovr.type = i;
                        break;
                    }
                }

                //Get the setback state
                for (i=0; i<4; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO5 + i) == BST_CHECKED)
                    {
                        my_appl.clmt_ctl_schd_ovr.state = i;
                        break;
                    }
                }

                //Get setback temperature
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_SETB_DEG));
                if(len > 0)
                {
                    char    *buf;
                    char    *decimal_pt;
                    int     degree;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_SETB_DEG, buf, len + 1);

                    if ((decimal_pt = strchr(buf, '.')) != NULL)
                    {
                        char first_decimal_digit;

                        first_decimal_digit = *(decimal_pt + 1);
                        if(isdigit(first_decimal_digit))
                        {
                            *decimal_pt++ = first_decimal_digit;
                        }
                        else
                        {
                            *decimal_pt++ = '0';
                        }
                        *decimal_pt = '\0';
                        degree = atoi(buf);
                        my_appl.clmt_ctl_schd_ovr.tenth_deg = degree;

                    }
                    else //integer
                    {
                        degree = atoi(buf);
                        my_appl.clmt_ctl_schd_ovr.tenth_deg = degree * 10;

                    }
                    free(buf);
                }
                else
                {
                    my_appl.clmt_ctl_schd_ovr.tenth_deg = 0;
                }


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_mul_cmd_ctl_dlgbx - Message handler for multi commmand encapsulation control dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_mul_cmd_ctl_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.temp_desc = 0;
            CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the destination descriptor id
                BOOL bSuccess;
                int desc_id;

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                //Get the control
                if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
                    my_appl.mul_cmd_ctl = 1;
                else
                    my_appl.mul_cmd_ctl = 0;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_prot_set_dlgbx - Message handler for setting protection states dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_prot_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.local_prot = 0xFF;

                //Default to unprotected
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO2, IDC_RADIO0);

                CheckRadioButton(hDlg, IDC_RADIO5, IDC_RADIO7, IDC_RADIO5);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the Local Protection States
                for (i=0; i<3; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        my_appl.local_prot = i;
                        break;
                    }
                }

                //Get the RF Protection States
                for (i=0; i<3; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO5 + i) == BST_CHECKED)
                    {
                        my_appl.rf_prot = i;
                        break;
                    }
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_prot_ec_set_dlgbx - Message handler for setting protection exclusive control node dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_prot_ec_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.node_id = 0xFF;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  node_id;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the node id
                node_id = GetDlgItemInt(hDlg, IDC_NODE1, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.node_id = node_id;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_prot_tmout_set_dlgbx - Message handler for setting protection timeout dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_prot_tmout_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.time = 0x40;
                my_appl.rep_desc_id = 0xFF;

                //Default to seconds
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO2, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;
                int  i;
				int  time;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the timeout value
                time = GetDlgItemInt(hDlg, IDC_TIMED, &bSuccess, FALSE);

                //Get the timeout unit
                for (i=0; i<3; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
                        if (i == 2)
                        {   //No timeout
                            my_appl.time = 0xFF;
                        }
                        else if (bSuccess)
                        {
                            if (i == 0)
                            {   //Seconds
                                if ((time > 0) && (time <= 60))
                                {
                                    my_appl.time = time;
                                }
                            }
                            else
                            {   //Minutes
                                if ((time >= 2) && (time <= 191))
                                {
                                    my_appl.time = time + 63;
                                }
                            }
                        }
                        break;
                    }
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_ind_set_dlgbx - Message handler for indicator set dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_ind_set_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            //Init
            my_appl.ind_val = 0xFFFF;
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                unsigned        hex_val;

                //Get hex string for indicator value
                if(hl_hex_get(hDlg, IDC_IND_VAL, &hex_val))
                {
                    my_appl.ind_val = hex_val;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_nif_send_dlgbx - Message handler for sending node information frame dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_nif_send_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.broadcast = 0xFF;

                //Default to single cast
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO1, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the tx mode
                if (IsDlgButtonChecked(hDlg, IDC_RADIO0) == BST_CHECKED)
                    my_appl.broadcast = 0;
                else
                    my_appl.broadcast = 1;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


extern const char *sensor_type_str[];
extern const char *sensor_unit_str[];

/**
hl_sensor_get_dlgbx - Message handler for getting sensor reading
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_sensor_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                int i;

                //Init
                my_appl.sensor_type = 0;
                my_appl.sensor_unit = 0xFF;
                my_appl.dst_desc_id = 0;

                hCombo = GetDlgItem(hDlg, IDC_COMBO_TYPE);

                //Populate sensor type list box
                ComboBox_ResetContent(hCombo);

                //ComboBox_AddString(hCombo, L"Unused");
                for (i=0; i<(ZW_SENSOR_TYPE_TGT_TEMP + 1) /*number of sensors*/; i++)
                {
                    SendDlgItemMessageA(hDlg, IDC_COMBO_TYPE, CB_ADDSTRING, 0, (LPARAM)sensor_type_str[i]);
                }

                //Default to "undefined"
                ComboBox_SetCurSel(hCombo, 0);

                //Clear sensor unit list box
                hCombo = GetDlgItem(hDlg, IDC_COMBO_UNIT);
                ComboBox_ResetContent(hCombo);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  sel;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.dst_desc_id = desc_id;

                //Get sensor type
                hCombo = GetDlgItem(hDlg, IDC_COMBO_TYPE);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.sensor_type = sel;

                //Get sensor unit
                hCombo = GetDlgItem(hDlg, IDC_COMBO_UNIT);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.sensor_unit = sel;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_COMBO_TYPE)
            {
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int  sel;
                    int  i;
                    HWND hCombo;
                    const char **unit_str;
                    sel = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);

                    //Populate sensor unit
                    hCombo = GetDlgItem(hDlg, IDC_COMBO_UNIT);
                    ComboBox_ResetContent(hCombo);

                    unit_str = sensor_unit_str + (sel * 4);

                    for (i=0; i<4; i++)
                    {
                        if (unit_str[i])
                        {
                            SendDlgItemMessageA(hDlg, IDC_COMBO_UNIT, CB_ADDSTRING, 0, (LPARAM)unit_str[i]);
                        }
                    }

                    //Default to the first unit
                    ComboBox_SetCurSel(hCombo, 0);

                    return(INT_PTR)TRUE;
                }
            }

            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_sensor_unit_get_dlgbx - Message handler for getting sensor supported units
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_sensor_unit_get_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                HWND hCombo;
                int i;

                //Init
                my_appl.sensor_type = 0;
                my_appl.dst_desc_id = 0;

                hCombo = GetDlgItem(hDlg, IDC_COMBO_TYPE);

                //Populate sensor type list box
                ComboBox_ResetContent(hCombo);

                for (i=0; i<(ZW_SENSOR_TYPE_TGT_TEMP + 1) /*number of sensors*/; i++)
                {
                    SendDlgItemMessageA(hDlg, IDC_COMBO_TYPE, CB_ADDSTRING, 0, (LPARAM)sensor_type_str[i]);
                }

                //Default to "undefined"
                ComboBox_SetCurSel(hCombo, 0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                HWND hCombo;
                BOOL bSuccess;
                int  desc_id;
                int  sel;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.dst_desc_id = desc_id;

                //Get sensor type
                hCombo = GetDlgItem(hDlg, IDC_COMBO_TYPE);
                sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                my_appl.sensor_type = sel;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }

            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_init_dlgbx - Message handler for requesting firmware update
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_fw_updt_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            {
                hbmpOpenFile = LoadBitmap(hInst, MAKEINTRESOURCE(OPEN_FILE_BITMAP));
                // Set the button image
                SendMessage(GetDlgItem(hDlg, IDC_OPEN_FILE),  BM_SETIMAGE,
                            IMAGE_BITMAP, (LPARAM)hbmpOpenFile);

                //Init
                if (my_appl.save_file)
                {
                    free(my_appl.save_file);
                    my_appl.save_file = NULL;
                }
                my_appl.temp_desc = 0;
                my_appl.fw_frag_sz = 0;
                my_appl.fw_id = 0;
                my_appl.fw_vid = 0;

            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL        bSuccess;
                int         desc_id;
                int         fw_tgt;
                int         frag_sz;
                int         len;
                unsigned    hex_val;

                //Get the destination interface descriptor id
                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.temp_desc = desc_id;

                //Get firmware file name
                len = GetWindowTextLength(GetDlgItem(hDlg, IDC_FW_FILE));
                if (len > 0)
                {
                    char *buf;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDC_FW_FILE, buf, len + 1);
                    my_appl.save_file = buf;;
                }

                //Get vendor id
                if(hl_hex_get(hDlg, IDC_FW_VID, &hex_val))
                {
                    my_appl.fw_vid = hex_val;
                }

                //Get vendor id
                if(hl_hex_get(hDlg, IDC_FW_ID, &hex_val))
                {
                    my_appl.fw_id = hex_val;
                }

                //Get firmware target
                fw_tgt = GetDlgItemInt(hDlg, IDC_FW_TGT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.fw_tgt = fw_tgt;

                //Get fragment size
                frag_sz = GetDlgItemInt(hDlg, IDC_FW_FRAG_SZ, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.fw_frag_sz = frag_sz;


                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                if (my_appl.save_file)
                {
                    free(my_appl.save_file);
                    my_appl.save_file = NULL;
                }

                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_OPEN_FILE)//open file button
            {
                TCHAR szFile[260];

                if(hl_open_file_name_get(hDlg, szFile, 260, 1))
                {
                    SetDlgItemText(hDlg, IDC_FW_FILE, szFile);
                }

                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_poll_ctl_dlgbx - Message handler for polling control dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_poll_ctl_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                int dflt_radio_but;
                char val_str[80];

                //Init
                dflt_radio_but = (my_appl.poll_ctl)? IDC_RADIO1 : IDC_RADIO2;

                CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, dflt_radio_but);

                //Fill in the existing values
                sprintf_s(val_str, 80, "%u", my_appl.poll_req.usr_token);
                SetDlgItemTextA(hDlg, IDC_USR_TOKEN, val_str);

                sprintf_s(val_str, 80, "%u", my_appl.poll_req.interval);
                SetDlgItemTextA(hDlg, IDC_ITV, val_str);

                sprintf_s(val_str, 80, "%u", my_appl.poll_req.poll_cnt);
                SetDlgItemTextA(hDlg, IDC_POLL_CNT, val_str);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int value;

                memset(&my_appl.poll_req, 0, sizeof(zwpoll_req_t));

                //Get user token
                value = GetDlgItemInt(hDlg, IDC_USR_TOKEN, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.poll_req.usr_token = (unsigned)value;

                //Get interval
                value = GetDlgItemInt(hDlg, IDC_ITV, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.poll_req.interval = (uint16_t)value;

                //Get poll count
                value = GetDlgItemInt(hDlg, IDC_POLL_CNT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.poll_req.poll_cnt = (uint16_t)value;

                //Get the polling control
                if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
                    my_appl.poll_ctl = 1;
                else
                    my_appl.poll_ctl = 0;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_poll_rm_dlgbx - Message handler for removing polling request dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_poll_rm_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.dlg_ok = 0;

                CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                int  value;
                char number_str[80];

                my_appl.dlg_ok = 1;

                //Get user token/handle
                value = GetDlgItemTextA(hDlg, IDC_ID, number_str, 80);
                if (value > 0)
                {
                    if (sscanf_s(number_str, "%u", &my_appl.poll_id) != 1)
                    {
                        my_appl.dlg_ok = 0;
                    }
                }

                //Get the polling control
                if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
                    my_appl.poll_id_type = 0;
                else
                    my_appl.poll_id_type = 1;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_power_level_dlgbx - Message handler for setting power level dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_power_level_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.pow_lvl_timeout = 0;

                //Default to normal power
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO9, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id, byTimeout;
                int  i;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the power level
                for (i = POWERLEVEL_SET_NORMALPOWER; i <= POWERLEVEL_SET_MINUS9DBM; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
						my_appl.pow_lvl = i;
                        break;
                    }
                }

				byTimeout = GetDlgItemInt(hDlg, IDC_TIMEOUT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.pow_lvl_timeout = byTimeout;

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_set_power_level_test_dlgbx - Message handler for setting power level test dialog box.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_set_power_level_test_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG:
            {
                //Init
                my_appl.pow_lvl_test_node_desc_id = 0;
				my_appl.pow_lvl_test_wframcnt = 0;

                //Default to normal power
                CheckRadioButton(hDlg, IDC_RADIO0, IDC_RADIO9, IDC_RADIO0);

                return(INT_PTR)TRUE;
            }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                BOOL bSuccess;
                int  desc_id, byTstNode_desc_id, wFramCnt;
                int  i;

                //Get the destination interface descriptor id

                desc_id = GetDlgItemInt(hDlg, IDC_DESC, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.rep_desc_id = desc_id;

                //Get the power level
                for (i = POWERLEVEL_SET_NORMALPOWER; i <= POWERLEVEL_SET_MINUS9DBM; i++)
                {
                    if (IsDlgButtonChecked(hDlg, IDC_RADIO0 + i) == BST_CHECKED)
                    {
						my_appl.pow_lvl = i;
                        break;
                    }
                }

				byTstNode_desc_id = GetDlgItemInt(hDlg, IDC_TESTNODEID, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.pow_lvl_test_node_desc_id = byTstNode_desc_id;

				wFramCnt = GetDlgItemInt(hDlg, IDC_FRAMECOUNT, &bSuccess, FALSE);
                if (bSuccess)
                    my_appl.pow_lvl_test_wframcnt = wFramCnt;


                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_fw_file_dlgbx - Message handler for openning firmware file.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_fw_file_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            {
                if (my_appl.save_file)
                {
                    free(my_appl.save_file);
                    my_appl.save_file = NULL;
                }

                hbmpOpenFile = LoadBitmap(hInst, MAKEINTRESOURCE(OPEN_FILE_BITMAP));
                // Set the button image
                SendMessage(GetDlgItem(hDlg, IDC_OPEN_FILE),  BM_SETIMAGE,
                            IMAGE_BITMAP, (LPARAM)hbmpOpenFile);
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get file name
                int len;

                //Get name
                len = GetWindowTextLength(GetDlgItem(hDlg, IDE_FILE_NAME));
                if (len > 0)
                {
                    char *buf;

                    buf = (char *)calloc(1, len + 1);
                    GetDlgItemTextA(hDlg, IDE_FILE_NAME, buf, len + 1);
                    my_appl.save_file = buf;;
                }
                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                DeleteObject(hbmpOpenFile);
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDC_OPEN_FILE)//open file button
            {
                TCHAR szFile[260];

                if(hl_open_file_name_get(hDlg, szFile, 260, 3))
                {
                    SetDlgItemText(hDlg, IDE_FILE_NAME, szFile);
                }

                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}


/**
hl_reset_dlgbx - Message handler for reset controller's NVM setting.
@param[in]	hDlg		The dialog box handle
@param[in]	message		The dialog box message
@param[in]	wParam		The dialog box parameter
@param[in]	lParam		The dialog box parameter
@return
*/
INT_PTR CALLBACK hl_reset_dlgbx(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
            {
                my_appl.port_number = 0;

                //Set default comm port to comm 1
                SetDlgItemText(hDlg, IDE_COMM_NUMBER, _T("1"));
            }
            return(INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                //Get the comm port number
                BOOL bSuccess;

                int port_num = GetDlgItemInt(hDlg, IDE_COMM_NUMBER, &bSuccess, FALSE);

                if (bSuccess)
                {
                    my_appl.port_number = port_num;
                }

                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return(INT_PTR)TRUE;
            }
            break;
    }
    return(INT_PTR)FALSE;
}



/**
@}
*/




