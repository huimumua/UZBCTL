/**
@file   zw_api_test_ui.c - Z-wave host controller API test program.

        User Interface part of the test program.

@author David Chow

@version    1.0 13-4-11  Initial release

version: 1.0
comments: Initial release
*/

#include "../../include/zw_api.h"
#include "zw_api_test.h"
#include <ctype.h>
#ifdef OS_MAC_X
#include <dirent.h>
#include <sys/types.h>
#endif

extern const char *sensor_type_str[];
extern const char *sensor_unit_str[];
extern const char *dev_category_str[];

/**
@defgroup API_TEST Z-wave host controller API test functions
To test Z-wave host controller API.
@{
*/

#define  ALPHA_OFFSET    0x72AA

/**
prompt_str - prompt for a string from user
@param[in] disp_str   The prompt string to display
@param[in] out_buf_sz The size of out_str buffer
@param[out] out_str   The buffer where the user input string to be stored
@return          The out_str if successful; else NULL.
*/
char  *prompt_str(const char *disp_str, int out_buf_sz, char *out_str)
{
    int retry;
    puts(disp_str);
    retry = 3;
    while (retry-- > 0)
    {
        if (fgets(out_str, out_buf_sz, stdin) && (*out_str) && ((*out_str) != '\n'))
        {
            char *newline;
            //Remove newline character

            newline = strchr(out_str, '\n');
            if (newline)
            {
                *newline = '\0';
            }
            return out_str;
        }
    }
    return NULL;
}


/**
prompt_int - prompt for an integer input from user
@param[in] str   The prompt string to display
@return          The integer that user has input
*/
int prompt_int(char *str)
{
    char user_input_str[36];
    int  ret;

    if (prompt_str(str, 36, user_input_str))
    {
        if (sscanf(user_input_str, "%d", &ret) == 1)
        {
            return ret;
        }
    }
    return 0;
}


/**
prompt_uint - prompt for an unsigned integer input from user
@param[in] str   The prompt string to display
@return          The unsigned integer that user has input
*/
unsigned prompt_uint(char *str)
{
    char user_input_str[36];
    unsigned  ret;

    if (prompt_str(str, 36, user_input_str))
    {
        if (sscanf(user_input_str, "%u", &ret) == 1)
        {
            return ret;
        }
    }
    return 0;
}


/**
prompt_hex - prompt for an hexadecimal unsigned integer input from user
@param[in] str   The prompt string to display
@return          The unsigned integer that user has input
*/
unsigned prompt_hex(char *str)
{
    char user_input_str[36];
    unsigned  ret;

    if (prompt_str(str, 36, user_input_str))
    {
        if (sscanf(user_input_str, "%x", &ret) == 1)
        {
            return ret;
        }
    }
    return 0;
}


/**
prompt_char - prompt for a character input from user
@param[in] str   The prompt string to display
@return          The character that user has input. Null character on error.
*/
char prompt_char(char *str)
{
    char ret[80];

    if (prompt_str(str, 80, ret))
    {
        return ret[0];
    }
    return 0;
}


/**
prompt_yes - prompt for yes or no from user
@param[in] str   The prompt string to display
@return          1 = user has input yes, 0 =  user has input no
*/
int prompt_yes(char *str)
{
    char c;

    c = prompt_char(str);
    if (c == 'y' || c == 'Y')
    {
        return 1;
    }
    return 0;
}


/**
prompt_menu - prompt for a menu choice from user
@param[in] str   The prompt string to display
@return          The integer value of the choice (for number) or alphabet + ALPHA_OFFSET (for alphabet); -1 on error.
*/
int prompt_menu(char *str)
{
    char choice[80];
    unsigned choice_number;

    if (prompt_str(str, 80, choice))
    {
        if (sscanf(choice, "%u", &choice_number) == 1)
        {
            return(int)choice_number;
        }
        return ALPHA_OFFSET + choice[0];
    }
    return -1;

}


/**
subm_report - prompt for descriptor id of report
@param[in] hl_appl   High-level application context
@return
*/
void subm_report(hl_appl_ctx_t   *hl_appl)
{
    hl_appl->rep_desc_id = prompt_uint("Enter desc id of the report:");
    printf("linhui set report nodeid %d\n",hl_appl->rep_desc_id);
}

/**
subm_setting - prompt for destination descriptor id
@param[in] hl_appl   High-level application context
@return
*/
void subm_setting(hl_appl_ctx_t   *hl_appl)
{
    hl_appl->dst_desc_id = prompt_uint("Enter destination desc id:");
}

/**
subm_multi_lvl_chg - prompt for multilevel change parameters
@param[in] hl_appl   High-level application context
@return
*/
void subm_multi_lvl_chg(hl_appl_ctx_t   *hl_appl)
{
    hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
    hl_appl->mul_lvl_dir = prompt_uint("Primary switch change dir: (0) Up, (1) Down, (3) No change (for version 3 switch)");
    hl_appl->mul_lvl_val = prompt_uint("Primary switch start level, enter 0 to 99 or 255 to use device current level");
    hl_appl->mul_lvl_sec_dir = prompt_uint("Secondary switch change dir: (0) Up, (1) Down, (3) No change");
    hl_appl->mul_lvl_sec_step = prompt_uint("Secondary switch step size, enter 0 to 99 or 255 to use default value");
    hl_appl->mul_lvl_dur = prompt_uint("Dimming duration in seconds which is the interval it takes to dim from level 0 to 99:");
}

/**
subm_grp_add - prompt for adding endpoints to group
@param[in] hl_appl   High-level application context
@return
*/
void subm_grp_add(hl_appl_ctx_t   *hl_appl)
{
    int i;

    for (i=0; i<5; i++)
    {
        hl_appl->ep_desc_id[i] = 0;
    }

    hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
    hl_appl->group_id = prompt_uint("Group id:");

    for (i=0; i<5; i++)
    {
        hl_appl->ep_desc_id[i] = prompt_uint("Endpoint descriptor id (0 to end the entry):");
        if (hl_appl->ep_desc_id[i] == 0)
        {
            break;
        }
    }
}


/**
subm_grp_cmd_set - prompt for setting up command to a node in a group
@param[in] hl_appl   High-level application context
@return
*/
void subm_grp_cmd_set(hl_appl_ctx_t   *hl_appl)
{
    int i;


    hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
    hl_appl->group_id = prompt_uint("Group id:");
    hl_appl->node_desc_id = prompt_uint("Node desc id in the group:");

    hl_appl->command[0] = prompt_hex("Enter command class to set (in hex):");
    hl_appl->command[1] = prompt_hex("Enter command to set (in hex):");
    hl_appl->cmd_len = prompt_uint("Enter length of command parameters (bytes):");
    hl_appl->cmd_len += 2;

    for (i=2; i<hl_appl->cmd_len ; i++)
    {
        hl_appl->command[i] = prompt_hex("Command param:");
    }
}


/**
subm_cfg_get - prompt for getting configuration parameters
@param[in] hl_appl   High-level application context
@return
*/
void subm_cfg_get(hl_appl_ctx_t   *hl_appl)
{
    hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
    hl_appl->cfg_param_mode = prompt_uint("How many parameters to get: (0) Single, (1) Range");
    if (hl_appl->cfg_param_mode == 0)
    {
        hl_appl->cfg_param = prompt_uint("Parameter number:");
    }
    else
    {
        hl_appl->cfg_range_start = prompt_uint("Parameter range start:");
        hl_appl->cfg_range_end = prompt_uint("Parameter range end:");
    }
}


/**
subm_cfg_set - prompt for setting configuration parameters
@param[in] hl_appl   High-level application context
@return
*/
void subm_cfg_set(hl_appl_ctx_t   *hl_appl)
{
    hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
    hl_appl->cfg_param = prompt_uint("Parameter number:");
    hl_appl->cfg_size = prompt_uint("Parameter value size (1, 2 or 4 bytes):");
    hl_appl->cfg_value_default = prompt_yes("Use default value (y/n)?:");
    if (!hl_appl->cfg_value_default)
    {
        hl_appl->cfg_value = prompt_int("Parameter value:");
    }
    else
    {
        hl_appl->cfg_value = 0;
    }
}


#ifdef OS_MAC_X
typedef struct _commp_name
{
    struct _commp_name  *next;  ///< Pointer to next entry
    uint16_t  index;            ///< Index number starting from 1
    char      name[2];          ///< Place holder for comm port name
} commp_name_t;


/**
comm_port_name_add - add an entry into the end of the list
@param[in]	head    List head
@param[in]	name    Name of the comm port entry
@return ZW_ERR_XXX
*/
static int comm_port_name_add(commp_name_t **head, char *name)
{
    commp_name_t   *ent;     //Pointer to list entry
    commp_name_t   *temp;
    size_t          name_len;

    name_len = strlen(name);

    ent = (commp_name_t *)malloc(sizeof(commp_name_t) + name_len);

    if (!ent)
        return ZW_ERR_MEMORY;

    ent->next = NULL;
    strcpy(ent->name, name);

    if (*head == NULL)
    {
        ent->index = 1;
        *head = ent;
        return ZW_ERR_NONE;
    }

    temp = *head;
    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = ent;
    ent->index = temp->index + 1;

    return ZW_ERR_NONE;

}


/**
comm_port_name_print - print the index number and comm port name for each entry in the list
@param[in]	head    List head
@return
*/
static void comm_port_name_print(commp_name_t *head)
{
    commp_name_t   *temp;

    if (head == NULL)
    {
        printf("There is no valid comm port name!\n");
    }

    temp = head;
    while (temp)
    {
        printf("(%u)  %s\n", temp->index, temp->name);
        temp = temp->next;
    }
}


/**
comm_port_name_find - find the comm port name entry based on index
@param[in]	head    List head
@param[in]	index   Search index
@return comm port name entry if found; otherwise return NULL
*/
static commp_name_t *comm_port_name_find(commp_name_t *head, uint16_t  index)
{
    commp_name_t   *temp;

    temp = head;
    while (temp)
    {
        if (temp->index == index)
        {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}


/**
comm_port_name_flush - flush the list
@param[in]	head    List head
@return
*/
static void comm_port_name_flush(commp_name_t **head)
{
    commp_name_t   *first_entry;  //The entry at the beginning of the list
    commp_name_t   *del_entry;    //Entry to be deleted

    first_entry = *head;

    while (first_entry)
    {
        del_entry = first_entry;
        first_entry = first_entry->next;
        free(del_entry);
    }

    *head = NULL;
}


/**
comm_port_name_get - get serial communication port name
@param[out] name    buffer to store the communication port name
@param[in]  len     length of buffer
@return             1 if success; otherwise return 0
*/
static int comm_port_name_get(char *name, unsigned len)
{
    int             ret = 0;
    commp_name_t    *head = NULL;
    char            *dir_path;
    char            *valid_comm_name;
    DIR             *dir;
    struct dirent   *entry;
    char entry_path[100];
    size_t path_len;

    dir_path = "/dev";

    valid_comm_name = "cu.";

    // Copy the directory path into entry_path
    strncpy (entry_path, dir_path, sizeof (entry_path));
    path_len = strlen (dir_path);

    // If the directory path doesn't end with a slash, append a slash.
    if (entry_path[path_len - 1] != '/')
    {
        entry_path[path_len] = '/';
        entry_path[path_len + 1] = '\0';
        ++path_len;
    }

    // Start the listing operation of the directory
    dir = opendir (dir_path);
    while ((entry = readdir (dir)) != NULL)
    {
        if (strncmp(valid_comm_name, entry->d_name, strlen(valid_comm_name)) == 0)
        {
            comm_port_name_add(&head, entry->d_name);
        }
    }

    comm_port_name_print(head);

    if (head)
    {
        unsigned        comm_port_index;
        commp_name_t    *comm_port_ent;
        comm_port_index = prompt_uint("Please enter the number associated with the comm port name:");
        comm_port_ent = comm_port_name_find(head, comm_port_index);

        if (comm_port_ent)
        {
            strncpy(entry_path + path_len, comm_port_ent->name,
                     sizeof(entry_path) - path_len);
            strncpy(name, entry_path, len);
            ret = 1;
        }
        else
        {
            printf("Invalid entry:%u\n", comm_port_index);
        }
    }

    //Clean up
    comm_port_name_flush(&head);
    closedir (dir);
    return ret;
}
#endif

/**
subm_system_init - prompt for program initialization parameters
@param[in] hl_appl   High-level application context
@return  0 on success, negative error number on failure
*/
int subm_system_init(hl_appl_ctx_t   *hl_appl)
{
    int         ret;
#ifndef OS_MAC_X
    unsigned    port_num;
#endif

    if (hl_appl->is_init_done)
        return 0;
    //Clear application context
    memset(hl_appl, 0, sizeof(hl_appl_ctx_t));

    //Get comm port name
#ifdef OS_MAC_X
    if (!comm_port_name_get(hl_appl->comm_port_name, 80))
    {
        return ZW_ERR_FAILED;
    }
#else
    port_num = prompt_uint("Please enter the comm port number:");
    sprintf(hl_appl->comm_port_name, "/dev/ttyACM%d", port_num);
    //sprintf(hl_appl->comm_port_name, "/dev/ttyUSB%d", port_num);    // testing
#endif
    printf("Comm port name:'%s'\n", hl_appl->comm_port_name);

    //Get node info file option
    hl_appl->load_ni_file = prompt_yes("Load detailed network info from file (y/n)?:");

    hl_appl->save_ni_file = prompt_yes("Save detailed network info to file on exit (y/n)?:");

    if (hl_appl->load_ni_file || hl_appl->save_ni_file)
    {
        if (prompt_str("Please enter file name:", 200, hl_appl->node_info_file) == NULL)
        {
            return ZW_ERR_VALUE;
        }
    }

    //Initialize
    ret = hl_init(hl_appl);
    if (ret < 0)
    {
        printf("hl_init with error: %d\n", ret);
    }
    return ret;
}


/**
subm_quick_reset - prompt for port number to reset controller
@param[in] hl_appl   High-level application context
@return  0 on success, negative error number on failure
*/
int subm_quick_reset(hl_appl_ctx_t *hl_appl)
{
#ifndef OS_MAC_X
    unsigned    port_num;
#endif

    //Clear comm port name
    hl_appl->comm_port_name[0] = '\0';

    //Get comm port name
#ifdef OS_MAC_X
    if (!comm_port_name_get(hl_appl->comm_port_name, 80))
    {
        return ZW_ERR_FAILED;
    }
#else
    port_num = prompt_uint("Please enter the comm port number:");
    sprintf(hl_appl->comm_port_name, "/dev/ttyACM%d", port_num);
    //sprintf(hl_appl->comm_port_name, "/dev/ttyUSB%d", port_num);    // testing
#endif
    printf("Comm port name:'%s'\n", hl_appl->comm_port_name);

    //Reset the controller
    return hl_quick_reset(hl_appl);
}


/**
help_menu - display and run help menu
@param[in] hl_appl   High-level application context
@return
*/
static void help_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        printf("\n(1) Info\n");
        printf("(2) Display Node descriptors\n");
        printf("(3) Internal Nodes Info\n");
        printf("(4) About\n");
        printf("(x) Exit menu\n");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                hl_info_get(hl_appl);
#ifdef  INTERNAL_TEST
                printf("__________Internal test result__________\n");
                printf("Sec tx timeout count:%u\n", zwnet_get_tx_tmout_cnt(hl_appl->zwnet));
#endif
                break;
            case 2:
                hl_node_desc_dump(hl_appl);
                break;
            case 3:
                hl_nodes_info_dump(hl_appl);
                break;
            case 4:
                {
                    uint8_t ver;
                    uint8_t subver;

                    zwnet_version(&ver, &subver);

                    printf("Zw API Test, Version %u.%02u\n", (unsigned)ver, (unsigned)subver);
                    puts("Copyright (C) 2010 - 2013, Sigma Designs, Inc.");
                }
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
network_menu - display and run network menu
@param[in] hl_appl   High-level application context
@return
*/
static void network_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        printf("\n(1) Cancel operation\n");
        printf("(2) Start add node\n");
        printf("(3) Start remove node\n");
        printf("(4) Start initiate\n");
        //printf("(5) Assign SIS\n");
        printf("(5) Remove failed node\n");
        printf("(6) Replace failed node\n");
        printf("(7) Migrate primary controller\n");
        printf("(8) Network update\n");
        printf("(9) Node update\n");
        printf("(10) Send Node Information Frame\n");
        printf("(d) Display Node descriptors\n");
        printf("(s) Set destination desc id\n");
        printf("(x) Exit menu\n");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                zwnet_abort(hl_appl->zwnet);
                break;
            case 2:
                hl_nw_node_add(hl_appl);
                break;
            case 3:
                hl_nw_node_rm(hl_appl);
                break;
            case 4:
                hl_lrn_mod_set(hl_appl);
                break;
//          case 5:
//              hl_appl->suc_node_id = prompt_uint("Enter destination node desc id:");
//              hl_suc_set(hl_appl);
//              break;
            case 5:
                hl_appl->failed_node_id = prompt_uint("Enter failed node desc id:");
                hl_failed_id_rm(hl_appl);
                break;
            case 6:
                hl_appl->failed_node_id = prompt_uint("Enter failed node desc id:");
                hl_failed_id_rp(hl_appl);
                break;
            case 7:
                hl_ctlr_chg(hl_appl);
                break;
            case 8:
                hl_req_nw_updt(hl_appl);
                break;
            case 9:
                hl_node_updt(hl_appl);
                break;
            case 10:
                hl_appl->broadcast = prompt_uint("Send NIF as (0) unicast frame, (1) broadcast frame:");
                if (!hl_appl->broadcast)
                {   //unicast
                    hl_appl->rep_desc_id = prompt_uint("Enter destination node desc id to send NIF:");
                }
                hl_nif_send(hl_appl);
                break;
            case (ALPHA_OFFSET + 'd'):
                hl_node_desc_dump(hl_appl);
                break;
            case (ALPHA_OFFSET + 's'):
                subm_setting(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
device_menu - display and run device menu
@param[in] hl_appl   High-level application context
@return
*/
static void device_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        if (hl_appl->is_init_done)
        {
            printf("\n(1) Restore factory default\n");
#ifndef OS_MAC_X
            printf("(3) Start local firmware update\n");
#endif
#ifdef DEBUG_FLASH_APM
            printf("(4) Switch to Auto Program Mode\n");
#endif
        }
        else
        {
            printf("\n(2) Quick reset of controller\n");
        }
        printf("(x) Exit menu\n");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                if (hl_appl->is_init_done)
                {
                    hl_deflt_set(hl_appl);
                }
                break;

            case 2:
                if (!hl_appl->is_init_done)
                {
                    subm_quick_reset(hl_appl);
                }
                break;

#ifndef OS_MAC_X
            case 3:
                if (hl_appl->is_init_done)
                {
                    if (prompt_str("Firmware file (binary format):", 200, hl_appl->save_file))
                    {
                        hl_fw_updt(hl_appl);
                    }
                }
                break;
#endif
#ifdef DEBUG_FLASH_APM
            case 4:
                if (hl_appl->is_init_done)
                {
                    hl_auto_prog(hl_appl);
                }
                break;
#endif
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
test1_menu - display and run test1 menu
@param[in] hl_appl   High-level application context
@return
*/
static void test1_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;
    int     i;

    while (to_run)
    {
        printf("\n(1) %s stress test\n", (hl_appl->is_ni_stress_tst_run)? "Stop" : "Start");
        puts("(2) Send multi-level sw data");
        puts("---------------------------------");
        puts("(3) Setup binary sw report");
        puts("(4) Get binary sw report");
        puts("(5) Turn binary sw on/off");
        puts("---------------------------------");
        puts("(6) Setup multi-level sw report");
        puts("(7) Get multi-level sw report");
        printf("(8) %s level change\n", (hl_appl->mul_lvl_change_started)? "Stop" : "Start");
        puts("(9) Set level");
        puts("(10) Get supported sw report");
        puts("---------------------------------");
        puts("(11) Setup binary sensor report");
        puts("(12) Get binary sensor report");
        puts("---------------------------------");
        puts("(13) Setup multi-level sensor report");
        puts("(14) Get multi-level sensor report");
        puts("(15) Get supported multi-level sensor types");
        puts("(16) Get supported multi-level sensor units");
        puts("---------------------------------");
        puts("(17) Get group info");
        puts("(18) Add endpoints to group");
        puts("(19) Delete endpoints from group");
        puts("(20) Get max groupings supported");
        puts("(21) Get current active group");
        puts("---------------------------------");
        puts("(22) Get cmd records supported");
        puts("(23) Set cmd to node in group");
        puts("(24) Get cmd from node in group");
        puts("---------------------------------");
        puts("(25) Get command queuing state");
        puts("(26) Control command queuing");
        puts("(27) View command queue");
        puts("(28) Cancel commands in command queue");
        puts("(29) Get wake up setting");
        puts("(30) Set wake up interval");
        puts("---------------------------------");
        puts("(31) Setup config parameter report");
        puts("(32) Get config parameter");
        puts("(33) Set config parameter");
        puts("---------------------------------");
        puts("(34) Set name loc");
        puts("---------------------------------");
#ifdef  ZW_STRESS_TEST
        printf("(t) %s stress test\n", (hl_appl->is_stress_tst_run)? "Stop" : "Start send sq");
        puts("(r) Show Test Report");
        puts("---------------------------------");
#endif
        puts("(d) Display Node descriptors");
        puts("(s) Set destination desc id");
        puts("(x) Exit menu");
        switch (prompt_menu("Your choice:"))
        {
#ifdef  ZW_STRESS_TEST
            case (ALPHA_OFFSET + 't'):
                hl_stress_test(hl_appl);
                break;

            case (ALPHA_OFFSET + 'r'):
                hl_test_rpt_show(hl_appl);
                break;
#endif
            case 1:
                if (hl_appl->is_ni_stress_tst_run == 0)
                {
                    unsigned mode;

                    mode = prompt_uint("(1) Node information query, (2) Send multilevel switch command:");

                    if (mode == 1)
                        hl_appl->stress_tst_func = hl_node_updt;
                    else if (mode == 2)
                        hl_appl->stress_tst_func = hl_multi_lvl_snd;
                    else
                        hl_appl->stress_tst_func = NULL;

                }
                hl_ni_stress_test(hl_appl);
                break;

            case 2:
                hl_multi_lvl_snd(hl_appl);
                break;
            case 3:
                subm_report(hl_appl);
                hl_bin_rep_setup(hl_appl);
                break;
            case 4:
                hl_binary_rep_get(hl_appl);
                break;
            case 5:
                hl_appl->temp_desc = prompt_uint("Enter desc id:");
                hl_appl->bin_state = prompt_uint("(0) Turn off, (1) Turn on:");
                hl_bin_set(hl_appl);
                break;
            case 6:
                subm_report(hl_appl);
                hl_multi_lvl_rep_setup(hl_appl);
                break;
            case 7:
                hl_multi_lvl_rep_get(hl_appl);
                break;
            case 8:
                if (hl_appl->mul_lvl_change_started == 0)
                {
                    subm_multi_lvl_chg(hl_appl);
                }
                hl_multi_lvl_chg(hl_appl);
                break;
            case 9:
                hl_appl->mul_lvl_val = prompt_uint("Switch level, enter 1 to 99 or 0=off, 255=on");
                puts("Duration in seconds, enter 1 to 127 or 0=instantly, 255=default or");
                hl_appl->mul_lvl_dur = prompt_uint("duration in minutes, enter 128(for 1 min) to 254(for 127 minutes)");
                hl_multi_lvl_set(hl_appl);
                break;
            case 10:
                subm_report(hl_appl);
                hl_multi_lvl_sup(hl_appl);
                break;
            case 11:
                subm_report(hl_appl);
                hl_bin_snsr_rep_setup(hl_appl);
                break;
            case 12:
                hl_bin_snsr_rep_get(hl_appl);
                break;
            case 13:
                subm_report(hl_appl);
                hl_ml_snsr_rep_setup(hl_appl);
                break;
            case 14:
                {
                    const char **unit_str;

                    hl_appl->dst_desc_id = prompt_uint("Enter desc id:");
                    puts("Sensor types:");
                    for (i=0; i<=ZW_SENSOR_TYPE_TGT_TEMP; i++)
                    {
                        printf("<%d> %s\n", i, sensor_type_str[i]);
                    }
                    hl_appl->sensor_type = prompt_uint("Enter sensor type:");

                    if ((hl_appl->sensor_type > 0) && (hl_appl->sensor_type <= ZW_SENSOR_TYPE_TGT_TEMP))
                    {
                        unit_str = sensor_unit_str + (hl_appl->sensor_type * 4);

                        puts("Sensor units:");
                        for (i=0; i<4; i++)
                        {
                            if (unit_str[i])
                            {
                                printf("<%d> %s\n", i, unit_str[i]);
                            }
                        }
                        hl_appl->sensor_unit = prompt_uint("Enter sensor unit:");
                    }
                    else
                    {
                        hl_appl->sensor_unit = 0;
                    }

                    hl_ml_snsr_rep_get(hl_appl);
                }
                break;
            case 15://Get the supported sensor types
                hl_ml_snsr_sup_get(hl_appl);
                break;

            case 16://Get the supported sensor units
                hl_appl->dst_desc_id = prompt_uint("Enter desc id:");
                puts("Sensor types:");
                for (i=1; i<=ZW_SENSOR_TYPE_TGT_TEMP; i++)
                {
                    printf("<%d> %s\n", i, sensor_type_str[i]);
                }
                hl_appl->sensor_type = prompt_uint("Enter sensor type:");
                hl_ml_snsr_unit_get(hl_appl);
                break;

            case 17:
                hl_appl->temp_desc = prompt_uint("Enter desc id:");
                hl_appl->group_id = prompt_uint("Enter group id:");
                hl_grp_rep_get(hl_appl);
                break;
            case 18:
                subm_grp_add(hl_appl);
                hl_grp_add(hl_appl);
                break;
            case 19:
                subm_grp_add(hl_appl);
                hl_grp_del(hl_appl);
                break;
            case 20:
                subm_report(hl_appl);
                hl_grp_sup(hl_appl);
                break;
            case 21:
                subm_report(hl_appl);
                hl_grp_active(hl_appl);
                break;
            case 22:
                subm_report(hl_appl);
                hl_grp_cmd_sup(hl_appl);
                break;
            case 23:
                subm_grp_cmd_set(hl_appl);
                hl_grp_cmd_set(hl_appl);
                break;
            case 24:
                hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
                hl_appl->group_id = prompt_uint("Group id:");
                hl_appl->node_id = prompt_uint("Node id in the group:");
                hl_grp_cmd_get(hl_appl);
                break;
            case 25:
                hl_cmd_q_ctl_get(hl_appl, &hl_appl->cmd_q_ctl);
                break;
            case 26:
                hl_appl->temp_desc = prompt_uint("Enter desc id:");
                hl_appl->cmd_q_ctl = prompt_uint("(0) Turn off, (1) Turn on:");
                hl_cmd_q_ctl_set(hl_appl);
                break;
            case 27:
                hl_cmd_q_view(hl_appl);
                break;
            case 28:
                hl_cmd_q_cancel(hl_appl);
                break;
            case 29:
                subm_report(hl_appl);
                hl_wkup_get(hl_appl);
                break;
            case 30:
                hl_appl->temp_desc = prompt_uint("Enter destination desc id:");
                hl_appl->node_desc_id = prompt_uint("Receiving node desc id:");
                hl_appl->wkup_interval = prompt_uint("Wake up interval (seconds):");
                hl_wkup_set(hl_appl);
                break;
            case 31:
                subm_report(hl_appl);
                hl_cfg_rep_setup(hl_appl);
                break;
            case 32:
                subm_cfg_get(hl_appl);
                hl_cfg_get(hl_appl);
                break;
            case 33:
                subm_cfg_set(hl_appl);
                hl_cfg_set(hl_appl);
                break;
            case 34:
                {
                    char name_loc_str[120];

                    hl_appl->rep_desc_id = prompt_uint("Enter endpoint desc id:");
                    hl_appl->nameloc.name[0] = '\0';
                    hl_appl->nameloc.loc[0] = '\0';
                    if (prompt_str("Name:", 120, name_loc_str))
                    {
                        strncpy(hl_appl->nameloc.name, name_loc_str, ZW_LOC_STR_MAX + 1);
                        hl_appl->nameloc.name[ZW_LOC_STR_MAX] = '\0';
                    }
                    if (prompt_str("Location:", 120, name_loc_str))
                    {
                        strncpy(hl_appl->nameloc.loc, name_loc_str, ZW_LOC_STR_MAX + 1);
                        hl_appl->nameloc.loc[ZW_LOC_STR_MAX] = '\0';
                    }
                    hl_nameloc_set(hl_appl);
                }
                break;


            case (ALPHA_OFFSET + 'd'):
                hl_node_desc_dump(hl_appl);
                break;
            case (ALPHA_OFFSET + 's'):
                subm_setting(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
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
test2_menu - display and run test1 menu
@param[in] hl_appl   High-level application context
@return
*/
static void test2_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        puts("(1) Setup pulse meter report");
        puts("(2) Get pulse meter report");
        puts("---------------------------------");
        puts("(3) Setup meter report");
        puts("(4) Get meter report");
        puts("(5) Get meter capabilities");
        puts("(6) Reset meter accumulated value");
        puts("(7) Get meter admin name and capability");
        puts("(8) Set meter admin name");
        puts("---------------------------------");
        printf("(9) Send AV button %s message\n", (hl_appl->av_btn_down)? "up" : "down");
        puts("(10) Get supported AV commands");
        puts("---------------------------------");
        puts("(11) Setup basic command report");
        puts("(12) Get basic command report");
        puts("(13) Set basic command");
        puts("---------------------------------");
        puts("(14) Setup door lock operation report");
        puts("(15) Get door lock operation state");
        puts("(16) Set door lock operation");
        puts("(17) Get door lock config parameter");
        puts("(18) Set door lock config parameter");
        puts("---------------------------------");
        puts("(19) Get user code");
        puts("(20) Set user code");
        puts("(21) Get number of supported user codes");
        puts("---------------------------------");
        puts("(22) Setup alarm report");
        puts("(23) Get alarm state");
        puts("(24) Set alarm activity");
        puts("(25) Get supported alarm types");
        puts("(26) Get supported alarm events");
        puts("---------------------------------");
        puts("(27) Setup battery report");
        puts("(28) Get battery report");
        puts("---------------------------------");
        puts("(d) Display Node descriptors");
        puts("(s) Set destination desc id");
        puts("(x) Exit menu");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                subm_report(hl_appl);
                hl_pulsemeter_rep_setup(hl_appl);
                break;
            case 2:
                hl_pulsemeter_rep_get(hl_appl);
                break;
            case 3:
                subm_report(hl_appl);
                hl_meter_rep_setup(hl_appl);
                break;
            case 4:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Preferred unit:");
                puts("For electric meter: (0) kWh (1) kVAh (2) W (3) Pulse count");
                puts("For gas meter: (0) cubic meter (1) cubic feet  (3) Pulse count");
                puts("For water meter: (0) cubic meter (1) cubic feet (2) US gallons (3) Pulse count");
                hl_appl->meter_unit = prompt_uint("Preferred unit:");
                hl_meter_rep_get(hl_appl);
                break;
            case 5:
                hl_meter_sup(hl_appl);
                break;
            case 6:
                hl_meter_reset(hl_appl);
                break;
            case 7:
                hl_meter_admin_cap_get(hl_appl);
                break;
            case 8:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                prompt_str("Admin name:", ZW_ADMIN_STR_MAX + 1, hl_appl->meter_admin);
                hl_meter_admin_set(hl_appl);
                break;
            case 9:
                if (hl_appl->av_btn_down)
                {       //Button is down
                    hl_av_set(hl_appl, hl_appl->av_btn_ctl, 0);
                    hl_appl->av_btn_down = 0;
                }
                else
                {       //Button is up
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Button code (hex):");
                    puts("Example: (13)Play, (14)Stop, (15) Pause, (16) Forward, (17) Reverse");
                    hl_appl->av_btn_ctl = prompt_hex("Button code (hex):");
                    hl_av_set(hl_appl, hl_appl->av_btn_ctl, 1);
                    hl_appl->av_btn_down = 1;

                }
                break;
            case 10:
                hl_av_get(hl_appl);
                break;
            case 11:
                subm_report(hl_appl);
                hl_basic_rep_setup(hl_appl);
                break;
            case 12:
                hl_basic_rep_get(hl_appl);
                break;
            case 13:
                hl_appl->basic_val = prompt_hex("Value (hex):");
                hl_basic_set(hl_appl);
                break;
            case 14:
                subm_report(hl_appl);
                hl_dlck_op_rep_setup(hl_appl);
                break;
            case 15:
                hl_dlck_op_rep_get(hl_appl);
                break;
            case 16:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Set mode (hex):");
                puts("(0) Door Unsecured, (1) Door Unsecured with timeout");
                puts("(10) Door Unsecured for inside Door Handles");
                puts("(11) Door Unsecured for inside Door Handles with timeout");
                puts("(20) Door Unsecured for outside Door Handles");
                puts("(21) Door Unsecured for outside Door Handles with timeout");
                puts("(ff) Door Secured");
                hl_appl->dlck_mode = prompt_hex("Set mode (hex):");

                hl_dlck_op_set(hl_appl);
                break;
            case 17:
                hl_dlck_cfg_get(hl_appl);
                break;
            case 18:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                hl_appl->dlck_config.type = prompt_uint("Operation type: (1)Constant (2)Timed");
                puts("For door handles states, each bit represents a handle with bit set to 0 for disable; 1 for enable");
                hl_appl->dlck_config.out_sta = prompt_hex("Outside Door Handles State 0 to f (hex):");
                hl_appl->dlck_config.in_sta = prompt_hex("Inside Door Handles State 0 to f (hex):");
                if (hl_appl->dlck_config.type == ZW_DOOR_OP_TIMED)
                {
                    puts("Duration lock stays unsecured in");
                    hl_appl->dlck_config.tmout_min = prompt_uint("minutes (1-254):");
                    hl_appl->dlck_config.tmout_sec = prompt_uint("seconds (1-59):");
                }
                hl_dlck_cfg_set(hl_appl);

                break;
            case 19:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                hl_appl->usr_id = prompt_uint("User ID (starting from 1):");
                hl_usrcod_get(hl_appl);
                break;
            case 20:
                {
                    char user_code[MAX_USRCOD_LENGTH + 1];
                    int  user_code_len;

                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    hl_appl->usr_code.id = prompt_uint("User ID (0 for all users; starting from 1 for other user):");
                    prompt_str("User code:", MAX_USRCOD_LENGTH + 1, user_code);
                    user_code_len = strlen(user_code);
                    memcpy(hl_appl->usr_code.code, user_code, user_code_len);
                    hl_appl->usr_code.code_len = user_code_len;
                    puts("User id status (hex):");
                    puts("(0) Availabe (not set), (1) Occupied");
                    puts("(2) Reserved by administrator, (fe) Status not available");
                    hl_appl->usr_code.id_sts = prompt_hex("User id status (hex):");
                    hl_usrcod_set(hl_appl);
                }
                break;
            case 21:
                hl_usrcod_sup_get(hl_appl);
                break;
            case 22:
                subm_report(hl_appl);
                hl_alrm_rep_setup(hl_appl);
                break;
            case 23:
                {
                    int i;
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    hl_appl->alrm_vtype = prompt_hex("Vendor specific alarm type (hex):");
                    puts("Z-wave alarm type (hex):");
                    puts("(ff) First alarm detection");
                    for (i=1; i<=ZW_ALRM_CLOCK; i++)
                    {
                        printf("(%02x) %s\n", i, zw_alrm_type[i]);
                    }

                    hl_appl->alrm_ztype = prompt_hex("Z-wave alarm type (hex):");

                    if ((hl_appl->alrm_ztype == 0) || (hl_appl->alrm_ztype == 0xFF))
                    {
                        hl_appl->alrm_evt = 0;
                    }
                    else
                    {
                        hl_appl->alrm_evt = prompt_hex("Z-wave alarm event (hex):");
                    }

                    hl_alrm_rep_get(hl_appl);

                }
                break;
            case 24:
                {
                    int i;
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Z-wave alarm type (hex):");
                    for (i=1; i<=ZW_ALRM_CLOCK; i++)
                    {
                        printf("(%02x) %s\n", i, zw_alrm_type[i]);
                    }
                    hl_appl->alrm_ztype = prompt_hex("Z-wave alarm type (hex):");
                    hl_appl->alrm_sts = prompt_hex("Alarm reporting activity (hex): (0) off (ff) on");

                    hl_alrm_set(hl_appl);

                }
                break;
            case 25:
                hl_alrm_sup_get(hl_appl);
                break;
            case 26:
                hl_appl->alrm_ztype = prompt_hex("Z-wave alarm type (hex):");
                hl_alrm_sup_evt_get(hl_appl);
                break;
            case 27:
                subm_report(hl_appl);
                hl_battery_rep_setup(hl_appl);
                break;
            case 28:
                hl_battery_rep_get(hl_appl);
                break;


            case (ALPHA_OFFSET + 'd'):
                hl_node_desc_dump(hl_appl);
                break;
            case (ALPHA_OFFSET + 's'):
                subm_setting(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
test3_menu - display and run test1 menu
@param[in] hl_appl   High-level application context
@return
*/
static void test3_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        puts("(1) Setup thermostat fan mode report");
        puts("(2) Get thermostat fan mode");
        puts("(3) Set thermostat fan mode");
        puts("(4) Get supported thermostat fan modes");
        puts("---------------------------------");
        puts("(5) Setup thermostat fan state report");
        puts("(6) Get thermostat fan state");
        puts("---------------------------------");
        puts("(7) Setup thermostat mode report");
        puts("(8) Get thermostat mode");
        puts("(9) Set thermostat mode");
        puts("(10) Get supported thermostat modes");
        puts("---------------------------------");
        puts("(11) Setup thermostat operating state report");
        puts("(12) Get thermostat operating state");
        puts("---------------------------------");
        puts("(13) Setup thermostat setback state report");
        puts("(14) Get thermostat setback state");
        puts("(15) Set thermostat setback state");
        puts("---------------------------------");
        puts("(16) Setup thermostat setpoint type report");
        puts("(17) Get thermostat setpoint type");
        puts("(18) Set thermostat setpoint");
        puts("(19) Get supported thermostat setpoint types");
        puts("---------------------------------");
        puts("(20) Setup clock report");
        puts("(21) Get clock reading");
        puts("(22) Set clock");
        puts("---------------------------------");
        puts("(23) Setup climate control schedule report");
        puts("(24) Get climate control schedule");
        puts("(25) Set climate control schedule");
        puts("(26) Get climate control schedule change");
        puts("(27) Get climate control schedule override");
        puts("(28) Set climate control schedule override");
        puts("---------------------------------");
        puts("(d) Display Node descriptors");
        puts("(s) Set destination desc id");
        puts("(x) Exit menu");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                subm_report(hl_appl);
                hl_thrmo_fan_md_rep_setup(hl_appl);
                break;
            case 2:
                hl_thrmo_fan_md_rep_get(hl_appl);
                break;
            case 3:
                {
                    unsigned i;
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Thermostat fan mode:");
                    puts("(0) Off (only valid for version > 1) (1) Auto low (2) Low (3) Auto High");
                    puts("(4) High (5) Auto medium  (6) Medium (7) Circulation (8) Humidity circulation");
                    i = prompt_uint("Thermostat fan mode:");
                    if (i == 0)
                    {   //Turn off fan
                        hl_appl->thrmo_fan_off = 1;
                        hl_appl->thrmo_fan_md = 0;
                    }
                    else
                    {   //Other mode
                        hl_appl->thrmo_fan_off = 0;
                        hl_appl->thrmo_fan_md = i - 1;

                    }
                    hl_thrmo_fan_md_set(hl_appl);
                }
                break;
            case 4:
                hl_thrmo_fan_md_sup_get(hl_appl);
                break;
            case 5:
                subm_report(hl_appl);
                hl_thrmo_fan_sta_rep_setup(hl_appl);
                break;
            case 6:
                hl_thrmo_fan_sta_rep_get(hl_appl);
                break;
            case 7:
                subm_report(hl_appl);
                hl_thrmo_md_rep_setup(hl_appl);
                break;
            case 8:
                hl_thrmo_md_rep_get(hl_appl);
                break;
            case 9:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Thermostat mode:");
                puts("(0) Off (1) Heating only (2) Cooling only (3) Auto");
                puts("(4) Auxiliary heat (5) Resume from last mode  (6) Fan only (7) Furnace only (8) Dry air");
                puts("(9) Moisturize air (10) Auto changeover (11) Heating with energy saving");
                puts("(12) Cooling with energy saving (13) Away from home");
                hl_appl->thrmo_md = prompt_uint("Thermostat mode:");
                hl_thrmo_md_set(hl_appl);
                break;
            case 10:
                hl_thrmo_md_sup_get(hl_appl);
                break;
            case 11:
                subm_report(hl_appl);
                hl_thrmo_op_sta_rep_setup(hl_appl);
                break;
            case 12:
                hl_thrmo_op_sta_rep_get(hl_appl);
                break;
            case 13:
                subm_report(hl_appl);
                hl_thrmo_setb_rep_setup(hl_appl);
                break;
            case 14:
                hl_thrmo_setb_rep_get(hl_appl);
                break;
            case 15:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Thermostat setback type:");
                puts("(0) No override (1) Temporary override (2) Permanent override");
                hl_appl->thrmo_setb_typ = prompt_uint("Thermostat setback type:");

                puts("Thermostat setback state:");
                puts("(0) Setback in degrees (Kelvin) (1) Frost Protection (2) Energy Saving Mode (3) Unused");
                hl_appl->thrmo_setb_sta = prompt_uint("Thermostat setback state:");

                if (hl_appl->thrmo_setb_sta == 0)
                {
                    char    buf[80];
                    char    *decimal_pt;
                    int     degree;

                    if (prompt_str("Setback temperature in K (range from -12.8 to 12.0):", 80, buf))
                    {

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
                            hl_appl->thrmo_setb_deg = degree;

                        }
                        else //integer
                        {
                            degree = atoi(buf);
                            hl_appl->thrmo_setb_deg = degree * 10;
                        }
                    }
                    else
                    {
                        hl_appl->thrmo_setb_deg = 0;
                    }
                }
                hl_thrmo_setb_set(hl_appl);
                break;
            case 16:
                subm_report(hl_appl);
                hl_thrmo_setp_rep_setup(hl_appl);
                break;
            case 17:
                {
                    uint8_t i;
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Thermostat setpoint type:");
                    puts("(0) Heating (1) Cooling (2) Furnace (3) Dry air");
                    puts("(4) Moisturize air (5) Auto changeover (6) Heating with energy saving");
                    puts("(7) Cooling with energy saving (8) Away from home");
                    i = prompt_uint("Thermostat setpoint type:");
                    if (i < 2)
                    {
                        i++;
                    }
                    else
                    {
                        i += 5;
                    }
                    hl_appl->thrmo_setp_typ = i;
                    hl_thrmo_setp_rep_get(hl_appl);
                }
                break;
            case 18:
                {
                    char    buf[80];
                    char    *decimal_pt;
                    int     degree;
                    uint8_t i;

                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Thermostat setpoint type:");
                    puts("(0) Heating (1) Cooling (2) Furnace (3) Dry air");
                    puts("(4) Moisturize air (5) Auto changeover (6) Heating with energy saving");
                    puts("(7) Cooling with energy saving (8) Away from home");
                    i = prompt_uint("Thermostat setpoint type:");
                    if (i < 2)
                    {
                        i++;
                    }
                    else
                    {
                        i += 5;
                    }
                    hl_appl->thrmo_setp_data.type = i;

                    if (prompt_str("Setpoint value:", 80, buf))
                    {
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
                            hl_appl->thrmo_setp_data.data[0] = (uint8_t) ((degree >> 8) & 0xFF) ;
                            hl_appl->thrmo_setp_data.data[1] = (uint8_t) (degree & 0xFF);

                            hl_appl->thrmo_setp_data.precision = 1;
                            hl_appl->thrmo_setp_data.size = 2;

        /*
                            //Use 4-byte format
                            hl_appl->thrmo_setp_data.data[0] = (uint8_t) (degree >> 24);
                            hl_appl->thrmo_setp_data.data[1] = (uint8_t) ((degree >> 16) & 0xFF) ;
                            hl_appl->thrmo_setp_data.data[2] = (uint8_t) ((degree >> 8) & 0xFF) ;
                            hl_appl->thrmo_setp_data.data[3] = (uint8_t) (degree & 0xFF);

                            hl_appl->thrmo_setp_data.precision = 1;
                            hl_appl->thrmo_setp_data.size = 4;
        */

                        }
                        else //integer
                        {
                            degree = atoi(buf);
                            //Use 1-byte format
                            hl_appl->thrmo_setp_data.data[0] = (uint8_t) (degree & 0xFF);

                            hl_appl->thrmo_setp_data.precision = 0;
                            hl_appl->thrmo_setp_data.size = 1;
                        }

                    }
                    else
                    {   //Default to zero
                        hl_appl->thrmo_setp_data.data[0] = 0;
                        hl_appl->thrmo_setp_data.precision = 0;
                        hl_appl->thrmo_setp_data.size = 1;
                    }

                    hl_appl->thrmo_setp_data.unit = prompt_uint("Setpoint unit:(0) Celsius (1) Fahrenheit");

                    hl_thrmo_setp_set(hl_appl);
                }
                break;
            case 19:
                hl_thrmo_setp_sup_get(hl_appl);
                break;
            case 20:
                subm_report(hl_appl);
                hl_clock_rep_setup(hl_appl);
                break;
            case 21:
                hl_clock_rep_get(hl_appl);
                break;
            case 22:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                hl_appl->hour = prompt_uint("Hour (24 hours format):");
                hl_appl->minute = prompt_uint("Minute:");
                puts("Day:");
                puts("(1) Monday (2) Tuesday (3) Wednesday");
                puts("(4) Thursday (5) Friday (6) Saturday (7) Sunday");
                hl_appl->weekday = prompt_uint("Day:");
                hl_clock_set(hl_appl);
                break;
            case 23:
                subm_report(hl_appl);
                hl_clmt_ctl_schd_rep_setup(hl_appl);
                break;
            case 24:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Day:");
                puts("(1) Monday (2) Tuesday (3) Wednesday");
                puts("(4) Thursday (5) Friday (6) Saturday (7) Sunday");
                hl_appl->weekday = prompt_uint("Day:");
                hl_clmt_ctl_schd_rep_get(hl_appl);
                break;
            case 25:
                {
                    char    buf[80];
                    int     i;
                    unsigned sel;

                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Day:");
                    puts("(1) Monday (2) Tuesday (3) Wednesday");
                    puts("(4) Thursday (5) Friday (6) Saturday (7) Sunday");
                    hl_appl->clmt_ctl_schd.weekday = prompt_uint("Day:");

                    //Get Switchpoints
                    hl_appl->clmt_ctl_schd.total = 0;

                    for (i=0; i<9; i++)
                    {
                        printf("Switch Point %d:\n", i);
                        puts("Schedule state:");
                        puts("(0) Setback in 1/10 degrees (Kelvin) (1) Frost Protection");
                        puts("(2) Energy Saving Mode (3) Unused (select this to end the entry)");

                        sel = prompt_uint("Schedule state:");

                        if (sel == 3)
                        {   //"Unused" was selected
                            break;
                        }

                        hl_appl->clmt_ctl_schd.swpts[i].state = sel;
                        hl_appl->clmt_ctl_schd.total++;

                        //Get start time
                        puts("Start time:");
                        hl_appl->clmt_ctl_schd.swpts[i].hour = prompt_uint("Hour (24 hours format):");
                        hl_appl->clmt_ctl_schd.swpts[i].minute = prompt_uint("Minute:");

                        if (sel == ZW_THRMO_SETB_STA_SETB)
                        {
                            //Get setback temperature
                            if (prompt_str("Setback temperature in K:", 80, buf))
                            {
                                char    *decimal_pt;
                                int     degree;

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
                                    hl_appl->clmt_ctl_schd.swpts[i].tenth_deg = degree;

                                }
                                else //integer
                                {
                                    degree = atoi(buf);
                                    hl_appl->clmt_ctl_schd.swpts[i].tenth_deg = degree * 10;

                                }

                            }
                            else
                            {
                                hl_appl->clmt_ctl_schd.swpts[i].tenth_deg = 0;
                            }
                        }
                    }

                    hl_clmt_ctl_schd_set(hl_appl);

                }
                break;
            case 26:
                hl_clmt_ctl_schd_chg_rep_get(hl_appl);
                break;
            case 27:
                hl_clmt_ctl_schd_ovr_rep_get(hl_appl);
                break;
            case 28:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Thermostat setback type:");
                puts("(0) No override (1) Temporary override (2) Permanent override");
                hl_appl->clmt_ctl_schd_ovr.type = prompt_uint("Thermostat setback type:");

                puts("Thermostat setback state:");
                puts("(0) Setback in degrees (Kelvin) (1) Frost Protection (2) Energy Saving Mode (3) Unused");
                hl_appl->clmt_ctl_schd_ovr.state = prompt_uint("Thermostat setback state:");

                if (hl_appl->clmt_ctl_schd_ovr.state == 0)
                {
                    char    buf[80];
                    char    *decimal_pt;
                    int     degree;

                    if (prompt_str("Setback temperature in K (range from -12.8 to 12.0):", 80, buf))
                    {

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
                            hl_appl->clmt_ctl_schd_ovr.tenth_deg = degree;

                        }
                        else //integer
                        {
                            degree = atoi(buf);
                            hl_appl->clmt_ctl_schd_ovr.tenth_deg = degree * 10;
                        }
                    }
                    else
                    {
                        hl_appl->clmt_ctl_schd_ovr.tenth_deg = 0;
                    }
                }

                hl_clmt_ctl_schd_ovr_set(hl_appl);
                break;


            case (ALPHA_OFFSET + 'd'):
                hl_node_desc_dump(hl_appl);
                break;
            case (ALPHA_OFFSET + 's'):
                subm_setting(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
test4_menu - display and run test1 menu
@param[in] hl_appl   High-level application context
@return
*/
static void test4_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        puts("(1) Get multi command encapsulation state");
        puts("(2) Control multi command encapsulation");
        puts("---------------------------------");
        puts("(3) Setup protection states report");
        puts("(4) Get protection states");
        puts("(5) Set protection states");
        puts("(6) Get supported protection states");
        puts("(7) Get protection exclusive control node");
        puts("(8) Set protection exclusive control node");
        puts("(9) Get RF protection timeout");
        puts("(10) Set RF protection timeout");
        puts("---------------------------------");
        puts("(11) Setup application status report");
        puts("---------------------------------");
        puts("(12) Setup indicator report");
        puts("(13) Get indicator report");
        puts("(14) Set indicator");
        puts("---------------------------------");
        puts("(15) Get firmwares information");
        puts("(16) Request firmware update");
        puts("---------------------------------");
        puts("(17) Polling control");
        puts("(18) Remove polling request");
        puts("---------------------------------");
		puts("(19) Setup power level report");
        puts("(20) Get power level");
		puts("(21) Set power level");
		puts("(22) Get power level test node");
		puts("(23) Set power level test node");
        puts("---------------------------------");
        puts("(d) Display Node descriptors");
        puts("(s) Set destination desc id");
        puts("(x) Exit menu");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                hl_mul_cmd_ctl_get(hl_appl, &hl_appl->mul_cmd_ctl);
                break;
            case 2:
                hl_appl->temp_desc = prompt_uint("Enter node desc id:");
                puts("Multi command encapsulation:");
                hl_appl->mul_cmd_ctl = prompt_uint("(0) Turn off, (1) Turn on:");
                hl_mul_cmd_ctl_set(hl_appl);
                break;
            case 3:
                subm_report(hl_appl);
                hl_prot_rep_setup(hl_appl);
                break;
            case 4:
                hl_prot_rep_get(hl_appl);
                break;
            case 5:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                puts("Local Protection State:");
                puts("(0) Unprotected (1) Protection by sequence (2) No operation possible");
                hl_appl->local_prot = prompt_uint("Local Protection State:");

                puts("RF Protection State (version 2):");
                puts("(0) Unprotected (1) No RF control (2) No RF control and response");
                hl_appl->rf_prot = prompt_uint("RF Protection State:");
                hl_prot_set(hl_appl);
                break;
            case 6:
                hl_prot_sup_get(hl_appl);
                break;
            case 7:
                hl_prot_ec_rep_get(hl_appl);
                break;
            case 8:
                hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                hl_appl->node_id = prompt_uint("Enter node id:");
                hl_prot_ec_set(hl_appl);
                break;
            case 9:
                hl_prot_tmout_rep_get(hl_appl);
                break;
            case 10:
                {
                    unsigned    i;
                    unsigned    time = 0x40;

                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Timeout unit:");
                    puts("(0) seconds (1 to 60) (1) minutes (2 to 191) (2) No timeout (always protected)");
                    i = prompt_uint("Timeout unit:");
                    if (i == 2)
                    {   //No timeout
                        hl_appl->time = 0xFF;
                    }
                    else
                    {
                        time = prompt_uint("Timeout:");
                        if (i == 0)
                        {   //Seconds
                            if ((time > 0) && (time <= 60))
                            {
                                hl_appl->time = time;
                            }
                        }
                        else
                        {   //Minutes
                            if ((time >= 2) && (time <= 191))
                            {
                                hl_appl->time = time + 63;
                            }
                        }
                    }
                }

                hl_prot_tmout_set(hl_appl);
                break;

            case 11:
                subm_report(hl_appl);
                hl_appl_sts_rep_setup(hl_appl);
                break;
            case 12:
                subm_report(hl_appl);
                hl_ind_rep_setup(hl_appl);
                break;
            case 13:
                hl_ind_rep_get(hl_appl);
                break;
            case 14:
                puts("Value (hex):");
                puts("Note: 00 = off, disable; FF = on, enable; Other value: 1 to 63h");
                hl_appl->ind_val = prompt_hex("Value (hex):");
                hl_ind_set(hl_appl);
                break;
            case 15:
                hl_fw_info_get(hl_appl);
                break;
            case 16:
                hl_appl->temp_desc = prompt_uint("Enter desc id:");
                hl_appl->fw_vid = prompt_hex("Vendor id (hex):");
                hl_appl->fw_id = prompt_hex("Firmware id (hex):");
                hl_appl->fw_tgt = prompt_uint("Firmware target:");
                hl_appl->fw_frag_sz = prompt_uint("Firmware fragment size:");
                if (prompt_str("Firmware file name:", 200, hl_appl->save_file))
                {
                    hl_fw_updt_req(hl_appl);
                }
                break;
            case 17:
                hl_appl->poll_ctl = prompt_yes("Start enter polling command (y/n)?:");
                if (hl_appl->poll_ctl)
                {
                    hl_appl->poll_req.usr_token = prompt_uint("Enter user token:");
                    hl_appl->poll_req.interval = prompt_uint("Interval (seconds):");
                    hl_appl->poll_req.poll_cnt = prompt_uint("Poll count (zero for repetitive polling):");
                }
                break;
            case 18:
                hl_appl->poll_id_type = prompt_uint("Enter identifier type (0)Handle, (1)Token:");
                hl_appl->poll_id = prompt_uint("Identifier:");
                hl_poll_rm(hl_appl);
                break;

			case 19:
                subm_report(hl_appl);
                hl_power_level_rep_setup(hl_appl);
                break;
			case 20:
                hl_power_level_rep_get(hl_appl);
                break;
			case 21:
                {
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
                    puts("Power level:");
                    puts("(0) Normal Power (1) -1 dBm (2) -2 dBm (3) -3 dBm (4) -4 dBm");
                    puts("(5) -5 dBm (6) -6 dBm (7) -7 dBm (8) -8 dBm (9) -9 dBm");
                    hl_appl->pow_lvl = prompt_uint("Power level:");
					hl_appl->pow_lvl_timeout = prompt_uint("Time out in secs (1-255):");
                    hl_power_level_set(hl_appl);
                }
                break;
			case 22:
                hl_power_level_test_rep_get(hl_appl);
                break;
			case 23:
                {
                    hl_appl->rep_desc_id = prompt_uint("Enter desc id:");
					hl_appl->pow_lvl_test_node_desc_id = prompt_uint("Enter Test frame target node desc id:");
                    puts("Power level:");
                    puts("(0) Normal Power (1) -1 dBm (2) -2 dBm (3) -3 dBm (4) -4 dBm");
                    puts("(5) -5 dBm (6) -6 dBm (7) -7 dBm (8) -8 dBm (9) -9 dBm");
                    hl_appl->pow_lvl = prompt_uint("Power level:");
					hl_appl->pow_lvl_test_wframcnt = prompt_uint("Test frame count (1-65535):");
                    hl_power_level_test_set(hl_appl);
                }
                break;
            case (ALPHA_OFFSET + 'd'):
                hl_node_desc_dump(hl_appl);
                break;
            case (ALPHA_OFFSET + 's'):
                subm_setting(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
file_menu - display and run file menu
@param[in] hl_appl   High-level application context
@return
*/
static void file_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;

    while (to_run)
    {
        if (hl_appl->is_init_done)
        {
            printf("\n(1) Close\n");
            puts("(2) Setting");
            puts("(3) Save settings");
        }
        else
        {
            printf("\n(1) Init\n");
        }
        puts("(x) Exit menu");
        switch (prompt_menu("Your choice:"))
        {
            case 1:
                if (hl_appl->is_init_done)
                {
                    hl_close(hl_appl);
                }
                else
                {
                    subm_system_init(hl_appl);
                }
                break;

            case 2:
                if (hl_appl->is_init_done)
                {
                    subm_setting(hl_appl);
                }
                break;

            case 3:
                if (hl_appl->is_init_done)
                {
                    if (prompt_str("Please enter file name:", 200, hl_appl->save_file))
                    {
                        hl_save(hl_appl);
                    }
                }
                break;

            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


/**
main_menu - display and run main menu
@param[in] hl_appl   High-level application context
@return
*/
static void main_menu(hl_appl_ctx_t *hl_appl)
{
    int     to_run = 1;
    int     choice;

    while (to_run)
    {
        printf("\n(1) File\n");
        if (hl_appl->is_init_done)
        {
            printf("(2) Test1\n");
            printf("(3) Test2\n");
            printf("(4) Test3\n");
            printf("(5) Test4\n");
            printf("(6) Manage network\n");
        }

        printf("(7) Device\n");

        if (hl_appl->is_init_done)
        {
            printf("(8) Help\n");
        }
        printf("(x) Exit program\n");

        choice = prompt_menu("Your choice:");

        //Filter out invalid choices when initialization is not done.
        if ((choice >=2 && choice <=6)
            || (choice == 8))
        {
            if (!hl_appl->is_init_done)
                continue;
        }

        switch (choice)
        {
            case 1:
                file_menu(hl_appl);
                break;
            case 2:
                test1_menu(hl_appl);
                break;
            case 3:
                test2_menu(hl_appl);
                break;
            case 4:
                test3_menu(hl_appl);
                break;
            case 5:
                test4_menu(hl_appl);
                break;
            case 6:
                network_menu(hl_appl);
                break;
            case 7:
                device_menu(hl_appl);
                break;
            case 8:
                help_menu(hl_appl);
                break;
            case (ALPHA_OFFSET + 'x'):
                to_run = 0;
                break;
        }
    }
}


int main(int argc, char **argv)
{
    printf("main started\n");
    hl_appl_ctx_t   appl_ctx = {0};   // High-level application context

    main_menu(&appl_ctx);
    hl_exit(&appl_ctx);
    return 0;
}

/**
@}
*/




