Version:1.02                                                            Date: 5-5-10
------------                                                            ------------
-Completed the frame layer except the handling of CAN response.

Version:1.03                                                            Date: 6-5-10
------------                                                            ------------
-Implement platform specific timer and mutex functions.

Version:1.04                                                            Date: 7-5-10
------------                                                            ------------
-Redesign to use context instead of variable.

Version:1.07                                                            Date: 11-5-10
------------                                                            ------------
-Created the platform_show_message() to display message to user.
-Completed the coding of transport layer and basic test seems working.

Version:1.08                                                            Date: 12-5-10
------------                                                            ------------
-Created edit control to display debug messages.
-Close handle when the platform_mutex_destroy() is called.

Version:1.09                                                            Date: 13-5-10
------------                                                            ------------
-Platform timer is now a generic timer that takes in a timeout callback and a param.
-Frame layer state-machine is now capable of re-entrance.

Version:1.10                                                            Date: 14-5-10
------------                                                            ------------
-Design the session layer.

Version:1.11                                                            Date: 18-5-10
------------                                                            ------------
-Completed the session layer.

Version:1.12                                                            Date: 19-5-10
------------                                                            ------------
-Added sample code to add node to network, send data, etc.

Version:1.13                                                            Date: 21-5-10
------------                                                            ------------
-Added platform semaphore.
-Changed the session_command_send() to blocking call for command that expects response.
-Removed the response callback and send frame status callback to application layer.

Version:1.14                                                            Date: 25-5-10
------------                                                            ------------
-Move the session layer command callback out of the state machine to avoid deadlock.
-Implemented some application layer commands.
-Created test program to send commands to control the led continuously.

Version:1.15                                                            Date: 26-5-10
------------                                                            ------------
-Change the synchronization of send data by using event rather than semaphore.
-Added Add / Remove node from network command.

Version:1.16                                                            Date: 27-5-10
------------                                                            ------------
-Completed most of the application layer functions.

Version:1.17                                                            Date: 31-5-10
------------                                                            ------------
-Created new project zw_hci that uses *.c files and no stdafx.h.

Version:1.18                                                            Date: 31-5-10
------------                                                            ------------
-Created a thread in session layer to handle callback request so that application can
 send command within the callback function.
-Rename the functions in application layer to reflect those in the doc.

Version:1.19                                                            Date: 1-6-10
------------                                                            ------------
-Retry to send data if failed in the test program.

Version:1.20                                                            Date: 2-6-10
------------                                                            ------------
-Modified functions in application layer to wait for 2 seconds for the condition where
 sending of command is allowed.
-Modified the initialization method of layers, where upper layer initialize the lower layer.

Version:1.21                                                            Date: 3-6-10
------------                                                            ------------
-Allow user to enter comm port number and dest node to send data.
-Disable some menu items when comm port is uninitialized.
-Time function for printing message.

Version:1.22                                                            Date: 4-6-10
------------                                                            ------------
-Created stress test for receiving data and check against sequence number.
TODO: Create unlimited buffer for the edit control for displaying messages.

Version:1.23                                                            Date: 8-6-10
------------                                                            ------------
-Tidy up code for HCI.

Version:1.24                                                            Date: 9-6-10
------------                                                            ------------
-Turn on the BLOCKING_SEND_DATA flag to enable stress test on tx and rx concurrently.
-Set unlimited buffer (~4G) for the edit control for displaying messages.
-Tidy up code for HCI Test main code.

Version:1.25                                                            Date: 14-6-10
------------                                                            ------------
-Added code to use ZW400 series USB.  See #ifdef TRANSPORT_USB.  Status: sending is ok, but
 get no response from the controller.

Version:1.26                                                            Date: 16-6-10
------------                                                            ------------
-Added functions such as assign/del return route, reset, replication, replace failing node
 ,find neighbor and multi cast.

Version:1.27                                                            Date: 18-6-10
------------                                                            ------------
-Split the error code in session layer timeout to differentiate between no ACK from
 frame layer and no response frame.

Version:1.28                                                            Date: 21-6-10
------------                                                            ------------
-Fixed the bug in send multi-cast destination nodes list.

Version:1.29                                                            Date: 22-6-10
------------                                                            ------------
-Added more function related to SUC/SIS.

Version:1.30                                                            Date: 23-6-10
------------                                                            ------------
-Change the dialog box of comm setting.

Version:1.31                                                            Date: 24-6-10
------------                                                            ------------
-Added get suc node id function.
-Completed all the tests.

Version:1.32                                                            Date: 25-6-10
------------                                                            ------------
-Modify the message in application_update_cb() to "application_update_cb: updated node:%u, status:%s"

Version:1.33                                                            Date: 28-6-10
------------                                                            ------------
-Added zw_create_new_primary_ctrl() and zw_controller_change()

Version:1.34                                                            Date: 6-7-10
------------                                                            ------------
-Added zw_hci_util.c for linked list functions.
-Reformatted souce files with space after if/for

Version:1.35 *****                                                      Date: 8-7-10
------------                                                            ------------
-Added zw high-level API from Sasi.

Version:1.36                                                            Date: 9-7-10
------------                                                            ------------
-Development of Base object,  Node, Endpoint Device APIs were completed but not fully tested.

Version:1.37                                                            Date: 12-7-10
------------                                                            ------------
-Solved a bug in appl_updt() where the case of no node info is unhandled.

Version:1.38                                                            Date: 14-7-10
------------                                                            ------------
-Move CreateEvent for tpt_ctx->wr_evt_hdl to tpt_thrd_start() from tpt_wr_thrd to prevent
 the use of wr_evt_hdl before it is created.
-Added code to fill-up the network, node and endpoint data structures on initialization.

Version:1.39                                                            Date: 15-7-10
------------                                                            ------------
-Added interfaces based on command class.

Version:1.40                                                            Date: 16-7-10
------------                                                            ------------
-Interface and command are mapped 1 to 1.

Version:1.41                                                            Date: 16-7-10
------------                                                            ------------
-Changed zwdev_t zwnode_t.
-Changed hl_multi_lvl_snd() to use the high-level API.

Version:1.42                                                            Date: 19-7-10
------------                                                            ------------
-Added support for interface report.

Version:1.43                                                            Date: 21-7-10
------------                                                            ------------
-Added zwnet_add(), zwnet_abort() and zwnet_fail().
-zwnet_init() will request node info of all nodes to get supported command classes.

Version:1.44                                                            Date: 22-7-10
------------                                                            ------------
-Added zwnet_initiate() and zwnet_assign().

Version:1.45                                                            Date: 23-7-10
------------                                                            ------------
-Completed all the zwnet_xxx().
-Solved a bug in appl_updt() where the case of no class info, the node id is
 not passed.

Version:1.46                                                            Date: 26-7-10
------------                                                            ------------
-zwnet_migrate() will check whether is pure SUC before calling zw_create_new_primary_ctrl();
-Added notify callback support.

Version:1.47                                                            Date: 27-7-10
------------                                                            ------------
-Added notify callback support for all zwnet_xxx().
-Handle the case when undocumented zwnet_add() callback return status of 0x23 (whenever
 a secondary controller invokes this operation) is returned.
-Developing COMMAND_CLASS_NODE_NAMING.

Version:1.48                                                            Date: 28-7-10
------------                                                            ------------
-Developed COMMAND_CLASS_NODE_NAMING.

Version:1.49                                                            Date: 29-7-10
------------                                                            ------------
-Remove the need of report handler for each type of report.
-Added zwnode_update() to update command classes and name & location if supported.

Version:1.50                                                            Date: 30-7-10
------------                                                            ------------
-Added mutex to access network structure (work in progress).

Version:1.51                                                            Date: 3-8-10
------------                                                            ------------
-Added descriptor for node, ep, interface, command and change all public functions to use
 descriptors instead of pointers to the network structure.

Version:1.52                                                            Date: 4-8-10
------------                                                            ------------
-Solved bug zw_request_node_info() fails when zwnet_nameloc_rep_get() is called before
 it is called in the application_update_cb().

Version:1.53                                                            Date: 5-8-10
------------                                                            ------------
-Created descriptor container linked-list.

Version:1.54                                                            Date: 6-8-10
------------                                                            ------------
-converted all the high level functions to use descriptors.
-hl_node_desc_dump() can display command class & interface names.

Version:1.55                                                            Date: 10-8-10
------------                                                            ------------
-Added function to add and delete node descriptor container.
-typedef void (*zwnet_node_fn)(void *user, zwnode_desc_p node_desc, int mode) callback
 function's parameter mode expanded to include ZWNET_NODE_UPDATED.

Version:1.56                                                            Date: 11-8-10
------------                                                            ------------
-Delayed the notify callback in zwnet_initiate_cb() if any node info is unavailable.
 This is to ensure when the notify callback is executed, the high level application
 can update the whole descriptor container data structure.

Version:1.57 *****                                                      Date: 11-8-10
------------                                                            ------------
-Cleaned up zw_hci_test.c by removing functions not supported by zw_api.h
-Used header file zw_api.h in zw_hci_test.c
-Clean up the UI menu system.

Version:1.58                                                            Date: 12-8-10
------------                                                            ------------
-Fixed bug when program starts and immediately add a new node, the descriptor container
 linked-list will contain only the added node.

Version:1.59                                                            Date: 17-8-10
------------                                                            ------------
-Remove waiting for the initialization done in zwnet_init() using semaphore to avoid
 menu system not responding during this time.

Version:1.60                                                            Date: 18-8-10
------------                                                            ------------
-Implemented state-machine for getting node info.

Version:1.61                                                            Date: 19-8-10
------------                                                            ------------
-Implemented state-machine state ZWNET_STA_GET_CMD_VER and ZWNET_STA_GET_NODE_NAMELOC.
-Implemented timer for state-machine.

Version:1.62                                                            Date: 20-8-10
------------                                                            ------------
-Implemented state-machine state ZWNET_STA_MULTI_INSTANCE.
-Solved bug in hl_desc_init(): when a command class does not have interface the descriptor
 container data structure is incomplete.

Version:1.63                                                            Date: 23-8-10
------------                                                            ------------
-Implemented multi-level and binary switch interfaces.

Version:1.64                                                            Date: 24-8-10
------------                                                            ------------
-Implemented binary sensor interfaces.

Version:1.65                                                            Date: 25-8-10
------------                                                            ------------
-Implemented multi-level sensor interfaces.

Version:1.66                                                            Date: 26-8-10
------------                                                            ------------
-Implemented group interfaces.

Version:1.67                                                            Date: 27-8-10
------------                                                            ------------
-Tested group interfaces except zwif_group_cmd_set().

Version:1.68                                                            Date: 30-8-10
------------                                                            ------------
-Solve bug of can't open COM port number that is >= 10 (e.g. COM12)
-After adding a node into the network, run the state-machine to get version ...

Version:1.69                                                            Date: 1-9-10
------------                                                            ------------
-Completed the state-machine for multi-instance and multi-channel.

Version:1.70                                                            Date: 2-9-10
------------                                                            ------------
-When removing node, start by removing endpoints, then command classes and interfaces.
-Added multi-level sensor support in the menu.
-Tested multi-level sensor support.
-Implemented multi-channel command encapsulation.

Version:1.71                                                            Date: 3-9-10
------------                                                            ------------
-Completed multi-instance and multi-channel association command classes.

Version:1.72                                                            Date: 6-9-10
------------                                                            ------------
-Ignore command class mark and the subsequent command classes when adding classes to an
 endpoint to avoid recursive in the operation of getting command class version.

Version:1.73                                                            Date: 7-9-10
------------                                                            ------------
-When updating node and endpoint, if there is no change, just do nothing.  Previous
 implementation was first delete the node or endpoint, then add the node or endpoint
 without comparing whether there is a change.
-The real endpoint command classes version will be assigned the same as in the virtual
 endpoint.

Version:1.74                                                            Date: 8-9-10
------------                                                            ------------
-Solved bug when query multi-channel association report, the node or endpoint will not
 be displayed if the network structure doesn't contain them due to power off, etc.
-Use a state-machine job structure to submit job to state-machine to get node info.

Version:1.75                                                            Date: 9-9-10
------------                                                            ------------
-Implemented and tested wake-up interface.
-Created dialog box to set multi-level switch level.

Version:1.76                                                            Date: 13-9-10
------------                                                            ------------
-Solved bug name & location not updated in the descriptor container level.

Version:1.77                                                            Date: 15-9-10
------------                                                            ------------
-Solved bug initiate can not be completed due to the controller that adds the node
 sends node info request too soon.
-Created a thread in session layer to handle zw_send_data callback to avoid limitation
 of sending more than one command in any callback function.

Version:1.78                                                            Date: 17-9-10
------------                                                            ------------
-Implemented stress test for sending/receiving sequence number using
 COMMAND_CLASS_MANUFACTURER_PROPRIETARY.
-Created a function to set the node info of a controller with
 COMMAND_CLASS_MANUFACTURER_PROPRIETARY for use in stress test.

Version:1.79                                                            Date: 24-9-10
------------                                                            ------------
-Added the option TRANSMIT_OPTION_EXPLORE to the zw_send_data() so if a frame fails to be
 routed, an explorer frame will be sent to find alternate route.
-Solved bug when replacing failed node and the status is ZW_NODE_OK (Node is working),
 the program will crash.

Version:1.80                                                            Date: 28-9-10
------------                                                            ------------
-Completed the state-machine for network update.

Version:1.81                                                            Date: 30-9-10
------------                                                            ------------
-When the zwnet_assign() is called, the controller will send suc id or assigned suc return
 route to other nodes so that they are aware of the new SUC/SIS node id.
-Solved bug of zw_assign_suc_return_route() does not callback on completion.

Version:1.82                                                            Date: 1-10-10
------------                                                            ------------
-Bug fixed on when using secondary controller to call zwnet_migrate(), it will crash.
-The network update will proceed to other state if call to network topology update failed.
-Info dump will display SUC node id.

Version:1.83                                                            Date: 15-10-10
------------                                                            ------------
-Added callback function to report transmit status to higher-level application.

Version:1.84                                                            Date: 2-11-10
------------                                                            ------------
-Bug fixed on zwnet_node_add_cb() when node_info is NULL.

Version:2.00 *****                                                      Date: 5-10-10
------------                                                            ------------
-Remove the zwcls_t from the network structure (based on version 1.82)

Version:2.01                                                            Date: 11-10-10
------------                                                            ------------
-Created specific function to set/get report for each command class.
-Automatic send WAKE_UP_NO_MORE_INFORMATION if higher level application has no data to send.
-Enable zwif_wakeup_set() to set the notification to broadcast mode.

Version:2.02                                                            Date: 12-10-10
------------                                                            ------------
-Clean up and callback if the EVT_GET_NODE_INFO event failed when the operation is
 delayed get node info.

Version:2.03                                                            Date: 14-10-10
------------                                                            ------------
-Implemented meter command class.

Version:2.04                                                            Date: 15-10-10
------------                                                            ------------
-Added callback function to report transmit status to higher-level application (port from
 version 1.83).
-Implemented partially AV command class.

Version:2.05                                                            Date: 18-10-10
------------                                                            ------------
-Implemented AV command class.
-Added state ZWNET_STA_GET_MANF_PDT_ID to support Manufacturer Specific command class.

Version:2.06                                                            Date: 29-10-10
------------                                                            ------------
-Implemented zw_memory_get_buffer() to get data from NVM of the device.

Version:2.07                                                            Date: 8-11-10
------------                                                            ------------
-Bug fixed on zwnet_node_add_cb() when node_info is NULL.

Version:2.08 *****                                                      Date: 10-11-10
------------                                                            ------------
-Added support for multiple nodes network wide inclusion in zwnet_add().
-When adding node, a network wide inclusion is carried out.  If a controller is added as a node,
 the operation will terminate due to limitation in protocol stack. If a slave device is added,
 caller needs to call zwnet_abort() to terminate the adding operation.
-zwnet_migrate() now using its own callback zwnet_migrate_cb().

Version:2.09                                                            Date: 10-11-10
------------                                                            ------------
-Removed the support of multiple-node add in zwnet_add().
-Added 3 retries for searching failed nodes in node neighbor update.

Version:2.10                                                            Date: 11-11-10
------------                                                            ------------
-Added support for multi-level switch version 2 and 3.
-Implemented zwif_get_ep().

Version:2.11                                                            Date: 12-11-10
------------                                                            ------------
-Added support for association command class version 2.
-Added support for association command configuration command class.

Version:2.12                                                            Date: 16-11-10
------------                                                            ------------
-Added support for configuration command class version 1.

Version:2.13                                                            Date: 16-11-10
------------                                                            ------------
-Preparation for realease.
-Remove meter, AV command classes and tidy up the source code.

Version:2.14  (zw_api rel ver 1.2)                                      Date: 18-11-10
------------                                                            ------------
-Bug fixed on a inclusion controller (SIS exists in the network) when adding a new node,
 some of the nodes command and device classes info were inadvertently deleted.  The fixed
 is to add a check of valid node_info before comparing node info in zwnet_node_info_update().
-Bug fixed on buffer overflow in plt_msg_ts_show().
-Remove #define  TRANSPORT_USB  //Use USB as physical transport in zw_hci_transport.c
-hl_init() will return if hl_desc_init() fails due to send error.

Version:2.15  (zw_api rel ver 1.21)                                     Date: 30-11-10
------------                                                            ------------
-Re-organized into zw_api.lib and zw_api_test application.

Version:2.16                                                            Date: 30-11-10
------------                                                            ------------
-Re-organized into uzbctl.lib, serial_api.lib and zw_api_test application.

Version:2.17                                                            Date: 6-12-10
------------                                                            ------------
-Return 1 in hl_wkup_rep_cb() on wake up notification so manual testing of sleeping
 devices can be done.

Version:2.18                                                            Date: 10-12-10
------------                                                            ------------
-Added stop node info state-machine instruction for aborting zwnet_migrate operation
 in zwnet_abort().

Version:2.19                                                            Date: 13-12-10
------------                                                            ------------
-Added incremental progress callbacks for network management operations.

Version:2.20                                                            Date: 14-12-10
------------                                                            ------------
-Added delay to resend frame if receive CAN (cancel) frame from controller. This solves
 the problem of frame send failed error on program startup.

Version:2.21  (zw_api rel ver 1.22)                                     Date: 15-12-10
------------                                                            ------------
-For zwif_group_get() the group reports (for multiple reports) will be consolidated before
 invoking report callback.

Version:2.22                                                            Date: 21-12-10
------------                                                            ------------
-Added feature to queue commands for sleeping node.

Version:2.23                                                            Date: 22-12-10
------------                                                            ------------
-Removed zwif_wakeup_notice_set() as the wake up notification callback is used by the system
 for sending commands in command queue.
-Added feature to view the queue commands and to cancel all pending commands in the queue.
-Renamed zwif_wakeup_q_ena_set() to zwnode_cmd_q_ena_set() for node level enabling of command queuing.

Version:2.24                                                            Date: 24-12-10
------------                                                            ------------
-Revamped the zw_hci application layer so that function that requires callback function parameter also
 requires callback parameter.  This enables the callback function when invoked, passes the user defined
 parameter.
-Changed execution of queued commands in a thread to avoid deadlock if it is executed in a send_data callback.

Version:2.25                                                            Date: 25-12-10
------------                                                            ------------
-When adding endpoint to a group, if the endpoint's node id is equal to the group's node id, no return route
 will be added to the group's node id.
-Removed the processing of UPDATE_STATE_NODE_INFO_REQ_FAILED when the node info request does not get any response
 from the node.  This is because the message does not provide the failed node id. This message if used in the
 node info state-machine will cause the current node info request of node (let's say node id B) to fail if the
 message UPDATE_STATE_NODE_INFO_REQ_FAILED belongs to previous node (let's say node id A) arrives.

Version:2.26                                                            Date: 30-12-10
------------                                                            ------------
-Added support for pulse meter.

Version:2.27                                                            Date: 31-12-10
------------                                                            ------------
-Added support for meter.

Version:2.28                                                            Date: 3-1-11
------------                                                            ------------
-Moved the clearing of wake_up setting (net->wake_up) to the zwif_rep_hdlr() to support
 queuing of commands.

-Moved the clearing of meter descriptor setting (net->meter_desc) to the zwif_rep_hdlr() to support
 queuing of commands.

Version:2.29                                                            Date: 5-1-11
------------                                                            ------------
-Added support for Simple AV class.

Version:2.30                                                            Date: 7-1-11
------------                                                            ------------
-Added version 2 & 3 support for meter command class.

Version:2.31                                                            Date: 14-1-11
------------                                                            ------------
-Added persistent storage of node information into file.
-Name and location can be set on node without support of node naming and location command class.
 The name and location can be stored in node information file and retrieved later.

Version:3.00  *****                                                     Date: 1-11-10
------------                                                            ------------
-Implemented pseudo-random number generator.

Version:3.01                                                            Date: 2-11-10
------------                                                            ------------
-Bug fixed on zwnet_node_add_cb() when node_info is NULL.
-Added comments in zw_api.h for documentation purposes.

Version:3.02                                                            Date: 4-11-10
------------                                                            ------------
-Added AES-128 block cipher encryption/decryption
-Generation of network key, authentication key and encryption key
-Message encryption/decryption using output feedback mode
NOTE: pseudo-random number generator has to be changed to used Zensys's version.

Version:3.03                                                            Date: 19-1-11
------------                                                            ------------
-Ported pseudo-random number generator from ZWaveSecurity library.
-Added function to calculate Message Authentication Code (MAC).

Version:3.04                                                            Date: 21-1-11
------------                                                            ------------
-Added support of internal nounce generation and handling.

Version:3.05                                                            Date: 25-1-11
------------                                                            ------------
-Added support of external nounce storing and handling.
-Changed to safe version of functions such as sprintf_s, sscanf_s, etc.
 See #ifdef USE_SAFE_VERSION.
-Changed hl_float_get() to eliminate the use of type float.

Version:3.06                                                            Date: 28-1-11
------------                                                            ------------
-Added secure message transmission state-machine.
-Send security nonce report if received nonce request.

Version:3.07                                                            Date: 1-2-11
------------                                                            ------------
-De-packetization of different SPP.
-Re-assembling of split-payload packets.

Version:3.08                                                            Date: 2-2-11
------------                                                            ------------
-Added secure message transmission message queue to allow multiple messages to be sent
 without waiting for each message completion.
-Tested send /receive secure message for both non-split and split SPP payload.

Version:3.09                                                            Date: 7-2-11
------------                                                            ------------
-Added sec_enable flag in the structure zwnet_init_t to control whether to enable security
 layer.
-Added flag in structure zwsec_tx_sm_param_t to determine whether to use all-zeroes network key
 to facilitate sending of NETWORK_KEY_SET command.

Version:3.10                                                            Date: 9-2-11
------------                                                            ------------
-Added state-machine for included node (the controller being added to the secure network).

Version:3.11                                                            Date: 10-2-11
------------                                                            ------------
-Added state-machine for adding node to the secure network.

Version:3.12                                                            Date: 14-2-11
------------                                                            ------------
-Updated ZW_classcmd.h file.
-Get command classes which are listed in the secure mode using SECURITY_COMMANDS_SUPPORTED_GET command.
-Combine all the SECURITY_COMMANDS_SUPPORTED_REPORT commands received into one report before
 sending to application layer.
-Modified get node info, version, name location state-machine and related functions to support
 mixture of secure and unsecure command classes.
-Modified stress test function to handle command sent being queued.

Version:3.13                                                            Date: 18-2-11
------------                                                            ------------
-Supported command queuing with security encapsulation.
-Increase the timeout in ZWNET_STA_GET_SEC_CMD_SUP to 10 seconds to wait for nonce.
-Don't display the debug message while in secure inclusion mode.

Version:3.14                                                            Date: 21-2-11
------------                                                            ------------
-Moved the creation of command queue thread after the zwhci_init();
-Bug fixed zw_init.notify() callback function not invoked in zwnet_init()
-Created zwnet_save() function to save network and node info settings.
-Change local variable whenever user application calls zwif_nameloc_set().

Version:3.15                                                            Date: 23-2-11
------------                                                            ------------
-Convert node name and location from UTF16 to UTF8 when the report indicates that
 char presentation is UTF16.
-Supported command recording.

Version:3.16    (Zensys SVN Revision:57)                                Date: 24-2-11
------------                                                            ------------
-Supported basic command class.
-Enable command queuing by default if the node is able to sleep (for
 persistent storage file loading).

Version:3.17                                                            Date: 25-2-11
------------                                                            ------------
-Added security inclusion for newly added node through failed node replacement
 and controller migration.

Version:3.18   (Zensys SVN Revision:60)                                 Date: 1-3-11
------------                                                            ------------
-Bug fixed when a valid comm port is used in initialization but it is not connected to a
 Z-wave controller would result in program crash.
-File->Init menu will change to File->Close menu once the initialization is done successfully.
 This allows user to close the connection and to re-init to other comm port.
-Added delay before starting of security inclusion because of firmware bug that reports
 ADD_NODE_STATUS_PROTOCOL_DONE earlier than actual completion.

Version:3.19   (Zensys SVN Revision:61)                                 Date: 3-3-11
------------                                                            ------------
-Bug fixed of notify callback report invalid operation after call to zwnet_init() using saved node information.

Version:3.20   (Zensys SVN Revision:62)                                 Date: 8-3-11
------------                                                            ------------
-Split zwnet_init() into two functions : zwnet_init() to initialize comm port and lower layers
 and zwnet_load() to get detailed node info from file or network.
-Report progress of get node info of every node.

Version:3.21                                                            Date: 11-3-11
------------                                                            ------------
-Added Door Lock command class.

Version:3.22                                                            Date: 16-3-11
------------                                                            ------------
-Changed status for network operation progress from ZW_ERR_NONE to OP_DONE, and
 from ZW_ERR_FAILED to OP_FAILED to support report progress of get node info of every node
 where the MSB of sts as in typedef void (*zwnet_notify_fn)(void *user, uint8_t op, uint16_t sts)
 is used to report total nodes.
-Supported manufacturer specific command class.

Version:3.23   (Zensys SVN Revision:63)                                 Date: 17-3-11
------------                                                            ------------
-Changed manufacturer id to Zensys (which is 0x0000) in the Manufacturer proprietary command class
 used in stress test.
-When load node information through file (persistent storage),  nodes that have no entries in the file
 will be loaded through Z-wave network.

Version:3.24                                                            Date: 18-3-11
------------                                                            ------------
-Supported User Code command class.

Version:3.25                                                            Date: 25-3-11
------------                                                            ------------
-Supported alarm command class (version 1 & 2).
-zwnet_assign() will notify application layer through callback even if the operation completed locally.
-Added a variable node->alive to indicate node is alive and responding to node info request,
 or down/sleeping.

Version:3.26                                                            Date: 28-3-11
------------                                                            ------------
-If Basic Command Class neither listed in Node Info Frame, nor in the secure list
 (SECURITY_COMMANDS_SUPPORTED_REPORT), the Basic Command Class will be added to the secure
 list as required by the Z-Wave security command class.
-Changed the interpretation of ALARM_REPORT's Event Parameters to full report.

Version:3.27  (zw_api rel ver 1.30)                                     Date: 31-3-11
------------                                                            ------------
-Fixed bug if security layer has not finish initialized and an unsolicited security encapsulation
 message is received, the program will crash.
-Fixed bug after calling zwnet_migrate() in secure mode, the interface descriptors that support
 security encapsulation are not updated.

Version:3.28                                                            Date: 28-4-11
------------                                                            ------------
-Fixed bug if COMMAND_CLASS_VERSION version greater than 1, the program will crash.
-Fixed bug on incorrect "alive" status when the node info request is sent to the local controller itself.
-Fixed memory leak in zwsec_mac_cal().

Version:3.29  (zw_api rel ver 1.31)                                     Date: 19-5-11
------------                                                            ------------
-Fixed memory leak in session layer that does not destroy snd_dat_cb_sem semaphore on exit.
-Fixed bug in application layer when timeout occurs due to no callback received after sending a command,
 the appl_cb_tmout_cb() does not callback higher layer with user parameter, a NULL is used instead.
-Enhance security/robustness by checking strictly the Z-wave reports that contain length field
 in zwif_rep_hdlr().
-Fixed bug on stress test when using a incorrect descriptor id, the lock desc_cont_mtx is not
 released causing other command to hang.
-When controller is not included in secure network, it will not respond to any security command.
-When a node failed to be included into a secure network, the controller will not send to or respond
 to any security commands from the node.
-Allow 0 length user code in USER_CODE_REPORT of COMMAND_CLASS_USER_CODE.  This may
 occur when user id status is available (not set) in some of the door locks (e.g. Kwikset SmartCode).

Version:3.30  (zw_api rel ver 1.32)                                     Date: 27-5-11
------------                                                            ------------
-Reset to factory default does not need to restart the zw_api library anymore.
-Fixed bug of controller which previous node id was not 1 and after reset to factory default, it
 does not calculate the  MAC part of Security Message Encapsulation correctly.
-Fixed bug of primary controller which is not included in secure network, send Security Scheme Get
 message after adding a new node done.
-Removed the menu item "Set controller node info" from the "device" menu.
-zwnet_init() will set local controller node information.
-Enhanced zwnet_node_info_update() to check whether the received node info matches the node interfaces.

Version:3.31  (zw_api rel ver 1.33)                                     Date: 10-6-11
------------                                                            ------------
-Fixed bug node_sec_ena in application_command_handler_cb() may not be initialized if the parameter
 src_node is not listed in the controller's routing table.
-Fixed issue of early Security Scheme Command received before LEARN_MODE_DONE callback.
-Disable stress test in order to remove support of COMMAND_CLASS_MANUFACTURER_PROPRIETARY in node
 information frame.
-Added support of COMMAND_CLASS_VERSION.

Version:3.32                                                            Date: 11-7-11
------------                                                            ------------
-Added support of COMMAND_CLASS_CRC_16_ENCAP.  The encapsulation and decapsulation of CRC-16 are transparent
 to higher level application.
-Added support of COMMAND_CLASS_MANUFACTURER_SPECIFIC version 2.
-Changed plt_msg_show() and plt_msg_ts_show() to support the display of UTF-8 string.
-Use plt_mtx_trylck() instead of plt_mtx_lck() in zwsec_tmout_cb() to avoid deadlock.
-Updated include\zwave\*.h files.
-Added stress test to run the node info state-machine repeatedly.

Version:3.33                                                            Date: 11-7-11
------------                                                            ------------
-When receive DEVICE_SPECIFIC_GET, retrieve serial number from the non-volatile memory to
 generate report.
-When initialize the zw_api using zwnet_init(), the high level user application needs to pass
 in a print text function pointer for displaying text string.  This is to facilitate the
 distribution of zw_api library without source code.

Version:3.34                                                            Date: 13-7-11
------------                                                            ------------
-Changed the member of structure zwifd_t from "secure" to "propty" for storing properties of an
 interface descriptor.  This allows the indication of an interface whether it can be access through
 secure or unsecure mode or both.
-Enhanced zwnet_node_info_update() to check whether the received node info matches the node interfaces.
-Modified node neighbor update callback function to display the node id along with the status.
-Check for duplicate external nonce before storing it.

Version:3.35                                                            Date: 16-8-11
------------                                                            ------------
-Fixed bug of when command recording is turned on, the CRC-16 encapsulation also recorded.

Version:3.36                                                            Date: 24-8-11
------------                                                            ------------
-Disable CRC-16 encapsulation and COMMAND_CLASS_MANUFACTURER_SPECIFIC version 2 support.
-Added COMMAND_CLASS_BASIC to all the endpoints if node information frame has none.

Version:3.37                                                            Date: 25-8-11
------------                                                            ------------
-Added zwnet_version() for application to get the home controller API version and subversion.
-Get version of each endpoint command classes if the endpoint supports version command class.

Version:3.38  (zw_api rel ver 1.34)                                     Date: 31-8-11
------------                                                            ------------
-Node info state-machine will check for changes in the number of endpoints when receives
 EVT_EP_NUM_REPORT event.

Version:3.39  (zw_api rel ver 1.35)                                     Date: 7-9-11
------------                                                            ------------
-Fixed TO# 3254 by changing definition of MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2 to 0 in ZW_classcmd.h

Version:3.40                                                            Date: 15-9-11
------------                                                            ------------
-Added feature to truncate UTF-16 string in order to fit into the UTF-8 output buffer in plt_utf16_to_8().
-Return controller node id when the network initialization API zwnet_init() is called.
-In security message transmission state-machine, handle unexpected case where nonce report event
 arrives earlier than transmission status event.
-Fixed TO# 03270 : select "file->close" before initialization done will crash the test application.
-Fixed TO# 03269 : after replace node id from a device with wake up command class to a device without one,
                   the new device command queue is still enabled.
-Fixed TO# 03277 : select "test->Set name loc" and enter Japanese or other UTF-16 characters to set the name
                   location.  Then select "help->Display node descriptors", the name and location show ????.

Version:3.41                                                            Date: 19-9-11
------------                                                            ------------
-Added feature to check validity of UTF-8 string when set/get node name location.
-Fixed bug:  after replace node id from a device with security command class to a device without one,
 the node security inclusion status shows failed.

Version:3.42  (zw_api rel ver 1.36)                                     Date: 4-10-11
------------                                                            ------------
-Added control of battery command class.
-Disable use of sender nonce from received SPP as external nonce.
-Removed COMMAND_CLASS_BASIC version query.
-Node info state-machine will query nodes for Z-wave library, protocol, and application versions.
-Defined HC_API_TYPE to indicate HC API type of the library.
-Return HC API type when the network initialization API zwnet_init() is called.
-Changed ZWNET_NODE_INFO_TMOUT from 5 seconds to 10 seconds to prevent transmit queue overflow.
-Block node update of controller itself.

Version:3.43  (zw_api rel ver 1.37)                                     Date: 10-10-11
------------                                                            ------------
-Fixed TO# 03313 : after added a node successfully but failed security,
                   the node sec_incl_failed status was not updated to 1 (failed).
-Changed all the function names from zwif_xx_rpt_get to zwif_xx_get

Version:3.44  (zw_api rel ver 1.38)                                     Date: 25-10-11
------------                                                            ------------
-Check input values to the HC API interface commands.
-Changed some dialog boxes text to guide user on permissible input values.
-Return VERSION_COMMAND_CLASS_REPORT with "Command Class Version" field equal to zero if the
 requested command class is invalid.
-Changed the local controller protocol and application versions to be in sync as those get from
 the VERSION_GET command.
-Changed ZWNET_TMOUT value to 7000 to cater for FLIRS wake up beam timing.

Version:3.45                                                            Date: 24-11-11
------------                                                            ------------
-Split zw_api_test.c into zw_api_test.c, zw_api_test2.c and zw_api_test_dlgbx.c
-Added zwnet_get_user() to get user context of a network.
-Supported control of COMMAND_CLASS_THERMOSTAT_FAN_MODE.
-Supported control of COMMAND_CLASS_THERMOSTAT_FAN_STATE.
-Supported control of COMMAND_CLASS_THERMOSTAT_MODE.
-Supported control of COMMAND_CLASS_THERMOSTAT_OPERATING_STATE.

Version:3.46                                                            Date: 28-11-11
------------                                                            ------------
-Supported control of COMMAND_CLASS_THERMOSTAT_SETBACK.
-Supported control of COMMAND_CLASS_THERMOSTAT_SETPOINT.

Version:3.47                                                            Date: 6-12-11
------------                                                            ------------
-Supported control of COMMAND_CLASS_CLOCK.
-Supported control of COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE.

Version:3.48                                                            Date: 8-12-11
------------                                                            ------------
-Supported COMMAND_CLASS_MULTI_CMD.

Version:3.49                                                            Date: 8-12-11
------------                                                            ------------
-Supported control of COMMAND_CLASS_PROTECTION.
-Supported control of COMMAND_CLASS_APPLICATION_STATUS.
-Supported control of COMMAND_CLASS_INDICATOR.

Version:3.50  (zw_api rel ver 1.39)                                     Date: 15-12-11
------------                                                            ------------
-Nonce with the first byte equals to zero will not be sent as Nonce Report.
-Primary node migration process will not try to include the new primary controller in secure
 network if the new primary controller is from the existing Z-wave network.  Likewise, when
 a secondary controller in an existing Z-wave network is assigned primary controller role,
 it will not try to be included in a secure network; i.e. the security status of the controller
 before and after the migration remains unchanged.
-Add node process will not try to include the new node in secure network if the
 new node is from the existing Z-wave network (this is the case for replication). Likewise, when
 a controller in an existing Z-wave network is added to the same network again (replication),
 it will not try to be included in a secure network; i.e. the security status of the controller
 before and after the inclusion remains unchanged.
-Added zwnet_send_nif() for sending node information frame.

Version:4.00                                                            Date: 6-4-11
------------                                                            ------------
-Linux port initial version.

Version:4.01                                                            Date: 12-4-11
------------                                                            ------------
-Ported zw_hci.
-Delay 1 second or until write started, before reading data at transport layer to avoid
 race condition where upper layer read callback functions are not ready.

Version:4.02                                                            Date: 12-4-11
------------                                                            ------------
-Ported zw_api.

Version:4.03                                                            Date: 13-4-11
------------                                                            ------------
-Fixed memory leak in session layer that does not destroy snd_dat_cb_sem semaphore on exit.
-Fixed memory leak in zwsec_mac_cal().
-Fixed deadlock in porting timer to linux in tmr_chk_thrd().
-Created text mode menu to test zw_api.


Version:4.04                                                            Date: 14-4-11
------------                                                            ------------
-Created device menu.
-Created test1 - stress test menu.
-Created network management menu.
-Periodic timer id changed to be in different range from one-shot timer id in plt_tmr_create()
 to avoid inadvertently removal of periodic timer when timer id generation roll-over from 0xFFFF.

Version:4.05                                                            Date: 19-4-11
------------                                                            ------------
-Added linux port for plt_utf16_to_8().
-Reduce number of functions in zw_api_test_ui.c

Version:4.06                                                            Date: 21-4-11
------------                                                            ------------
-Changed the prompt_str() and prompt_char() to use gets() because it can handle input with space
 in between words.

Version:4.07                                                            Date: 25-4-11
------------                                                            ------------
-Fixed bug in application layer when timeout occurs due to no callback received after sending a command,
 the appl_cb_tmout_cb() does not callback higher layer with user parameter, a NULL is used instead.

Version:4.08                                                            Date: 26-4-11
------------                                                            ------------
-Added all items in "Test2 menu"

Version:4.09                                                            Date: 27-4-11
------------                                                            ------------
-Added all items in "File menu"

Version:4.10                                                            Date: 28-4-11
------------                                                            ------------
-Fixed bug if COMMAND_CLASS_VERSION version greater than 1, the program will crash.
-Fixed bug on incorrect "alive" status when the node info request is sent to the local controller itself.

Version:4.11                                                            Date: 5-5-11
------------                                                            ------------
-Fixed bug on plt_tmr_create() will return NULL if the plt_ctx.id_gen is equal to 0x8000.
-In plt_mtx_init(), change the constant PTHREAD_MUTEX_RECURSIVE to PTHREAD_MUTEX_RECURSIVE_NP as some
 linux platform, e.g. Fedora 8 does not support PTHREAD_MUTEX_RECURSIVE.
-Enhance security/robustness by checking strictly the Z-wave reports that contain length field
 in zwif_rep_hdlr().

Version:4.12                                                            Date: 6-5-11
------------                                                            ------------
-Fixed bug on stress test when using a incorrect descriptor id, the lock desc_cont_mtx is not
 released causing other command to hang.
-The node info state-machine will not issue "Security Commands Supported Get" command if the
 node is not securely included.

Version:4.13  (zw_api rel ver 2.00L)                                    Date: 9-5-11
------------                                                            ------------
-When controller is not included in secure network, it will not respond to any security command.
-When a node failed to be included into a secure network, the controller will not send to or respond
 to any security commands from the node.

Version:4.14  (zw_api rel ver 2.01L)                                    Date: 27-5-11
------------                                                            ------------
-Allow 0 length user code in USER_CODE_REPORT of COMMAND_CLASS_USER_CODE.  This may
 occur when user id status is available (not set) in some of the door locks (e.g. Kwikset SmartCode).
-Reset to factory default does not need to restart the zw_api library anymore.
-Fixed bug of controller which previous node id was not 1 and after reset to factory default, it
 does not calculate the  MAC part of Security Message Encapsulation correctly.
-Fixed bug of primary controller which is not included in secure network, send Security Scheme Get
 message after adding a new node done.
-Removed the menu item "Set controller node info" from the "device" menu.
-zwnet_init() will set local controller node information.
-Enhanced zwnet_node_info_update() to check whether the received node info matches the node interfaces.

Version:4.15  (zw_api rel ver 2.02L)                                    Date: 14-6-11
------------                                                            ------------
-Fixed bug node_sec_ena in application_command_handler_cb() may not be initialized if the parameter
 src_node is not listed in the controller's routing table.
-Fixed issue of early Security Scheme Command received before LEARN_MODE_DONE callback.
-Disable stress test in order to remove support of COMMAND_CLASS_MANUFACTURER_PROPRIETARY in node
 information frame.
-Added support of COMMAND_CLASS_VERSION.

Version:4.16  (zw_api rel ver 2.XXL)                                    Date: 30-6-11
------------                                                            ------------
-Fixed bug when the sequence "File->close" followed by "File->init" with invalid comm port,
 the program crashes (TO# 03197).
-Hide some menu items (e.g. manage network, test1, test2, etc) when the test program is not initialized.
-Modified node neighbor update callback function to display the node id along with the status.
-Create new thread to serve timer expire callback in tmr_chk_thrd() to avoid deadlock.
-Use plt_mtx_trylck() instead of plt_mtx_lck() in zwsec_tmout_cb() to avoid deadlock.

Version:4.17  (zw_api rel ver 2.XXL)                                    Date: 13-7-11
------------                                                            ------------
-Added support of COMMAND_CLASS_CRC_16_ENCAP.  The encapsulation and decapsulation of CRC-16 are transparent
 to higher level application.
-Added support of COMMAND_CLASS_MANUFACTURER_SPECIFIC version 2.
-Updated include\zwave\*.h files.
-Added stress test to run the node info state-machine repeatedly.
-When receive DEVICE_SPECIFIC_GET, retrieve serial number from the non-volatile memory to
 generate report.
-When initialize the zw_api using zwnet_init(), the high level user application needs to pass
 in a print text function pointer for displaying text string.  This is to facilitate the
 distribution of zw_api library without source code.
-Changed the member of structure zwifd_t from "secure" to "propty" for storing properties of an
 interface descriptor.  This allows the indication of an interface whether it can be access through
 secure or unsecure mode or both.
-Enhanced zwnet_node_info_update() to check whether the received node info matches the node interfaces.
-Check for duplicate external nonce before storing it.

Version:4.18  (zw_api rel ver 2.XXL)                                    Date: 16-8-11
------------                                                            ------------
-Fixed bug of when command recording is turned on, the CRC-16 encapsulation also recorded.

Version:4.19  (zw_api rel ver 2.03L)                                    Date: 29-8-11
------------                                                            ------------
-Disable CRC-16 encapsulation and COMMAND_CLASS_MANUFACTURER_SPECIFIC version 2 support.
-Added COMMAND_CLASS_BASIC to all the endpoints if node information frame has none.
-Added zwnet_version() for application to get the home controller API version and subversion.
-Get version of each endpoint command classes if the endpoint supports version command class.

Version:4.20  (zw_api rel ver 2.04L)                                    Date: 9-9-11
------------                                                            ------------
Fixed TO# 3254 by changing definition of MULTI_CHANNEL_ASSOCIATION_SET_MARKER_V2 to 0 in ZW_classcmd.h

Version:4.21  (zw_api rel ver 2.0XL)                                    Date: 19-9-11
------------                                                            -------------
-Added feature to truncate UTF-16 string in order to fit into the UTF-8 output buffer in plt_utf16_to_8().
-Return controller node id when the network initialization API zwnet_init() is called.
-In security message transmission state-machine, handle unexpected case where nonce report event
 arrives earlier than transmission status event.
-Fixed TO# 03270 : select "file->close" before initialization done will crash the test application.
-Fixed TO# 00967 : after replace node id from a device with wake up command class to a device without one,
                   the new device command queue is still enabled.
-Added feature to check validity of UTF-8 string when set/get node name location.
-Fixed bug:  after replace node id from a device with security command class to a device without one,
 the node security inclusion status shows failed.

Version:4.22  (zw_api rel ver 2.05L)                                    Date: 5-10-11
------------                                                            -------------
-Added control of battery command class.
-Disable use of sender nonce from received SPP as external nonce.
-Removed COMMAND_CLASS_BASIC version query.
-Node info state-machine will query nodes for Z-wave library, protocol, and application versions.
-Defined HC_API_TYPE to indicate HC API type of the library.
-Return HC API type when the network initialization API zwnet_init() is called.
-Changed ZWNET_NODE_INFO_TMOUT from 5 seconds to 10 seconds to prevent transmit queue overflow.
-Block node update of controller itself.

Version:4.23  (zw_api rel ver 2.06L)                                    Date: 10-10-11
------------                                                            -------------
-Fixed TO# 03313 : after added a node successfully but failed security,
                   the node sec_incl_failed status was not updated to 1 (failed).
-changed all the function names from zwif_xx_rpt_get to zwif_xx_get

Version:4.24  (zw_api rel ver 2.07L)                                    Date: 25-10-11
------------                                                            -------------
-Check input values to the HC API interface commands.
-Changed some user interface texts to guide user on permissible input values.
-Return VERSION_COMMAND_CLASS_REPORT with "Command Class Version" field equal to zero if the
 requested command class is invalid.
-Changed the local controller protocol and application versions to be in sync as those get from
 the VERSION_GET command.
-Changed ZWNET_TMOUT value to 7000 to cater for FLIRS wake up beam timing.

Version:4.25  (zw_api rel ver 2.08L)                                    Date: 15-12-11
------------                                                            -------------
-Added zwnet_get_user() to get user context of a network.
-Supported control of COMMAND_CLASS_THERMOSTAT_FAN_MODE.
-Supported control of COMMAND_CLASS_THERMOSTAT_FAN_STATE.
-Supported control of COMMAND_CLASS_THERMOSTAT_MODE.
-Supported control of COMMAND_CLASS_THERMOSTAT_OPERATING_STATE.
-Supported control of COMMAND_CLASS_THERMOSTAT_SETBACK.
-Supported control of COMMAND_CLASS_THERMOSTAT_SETPOINT.
-Supported control of COMMAND_CLASS_CLOCK.
-Supported control of COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE.
-Supported COMMAND_CLASS_MULTI_CMD.
-Supported control of COMMAND_CLASS_PROTECTION.
-Supported control of COMMAND_CLASS_APPLICATION_STATUS.
-Supported control of COMMAND_CLASS_INDICATOR.
-Fixed TO# 03373: Nonce with the first byte equals to zero will not be sent as Nonce Report.
-Fixed TO# 03374: Primary node migration process will not try to include the new primary controller in secure
 network if the new primary controller is from the existing Z-wave network.  Likewise, when
 a secondary controller in an existing Z-wave network is assigned primary controller role,
 it will not try to be included in a secure network; i.e. the security status of the controller
 before and after the migration remains unchanged.
-Add node process will not try to include the new node in secure network if the
 new node is from the existing Z-wave network (this is the case for replication). Likewise, when
 a controller in an existing Z-wave network is added to the same network again (replication),
 it will not try to be included in a secure network; i.e. the security status of the controller
 before and after the inclusion remains unchanged.
-TO# 03394: Added zwnet_send_nif() for sending node information frame.

Version:5.00  (zw_api rel ver 3.00)                                     Date: 13-1-12
------------                                                            ------------
-Consolidated Windows and Linux versions into one code base.
-Node info state-machine will check whether the command class in version report matches the
 requested command class to prevent looping.
-Fixed bug of select "file->close" before initialization done will crash the test application
 when next node in the node list for getting node info is a secure node.
-Promoted name/loc to node level, i.e. zwif_nameloc_set() is deprecated and
 zwnode_nameloc_set() will set the name and location in the node if the node
 has COMMAND_CLASS_NODE_NAMING interface.

Version:5.01                                                            Date: 26-1-12
------------                                                            ------------
-TO# 03402: Problem when Nonce Get was retransmitted and a new nonce report(2) arrived after
 the controller had used the previous nonce report(1) for security encapsulation.
-Removed the dedicated thread in session layer to handle callbacks resulted from the
 zw_send_data() completion. This is required to ensure Z-wave ACK and SECURITY_NONCE_REPORT
 to arrive at the security transmission state-machine in correct sequence.
-Modified security transmission state-machine to wait for Z-wave ACK to the previously sent
 Nonce Get Command before accepting any SECURITY_NONCE_REPORT.

Version:5.02  (zw_api rel ver 3.01)                                     Date: 2-2-12
------------                                                            ------------
-Device name and location on node level has been deprecated. Instead, it is moved to endpoint level.
 The function zwnode_nameloc_set() has been replaced by zwep_nameloc_set().
-Created endpoint node info state-machine to query endpoint's version and name and location info.
-Name and version stored in database to have priority over the values stored in the device.

Version:5.03  (zw_api rel ver 3.XX)                                     Date: 2-3-12
------------                                                            ------------
-Supported MAC OS X Lion.
-Added "INSTALL" file for instructions to compile the project.
-Deprecated HOST_BYTE_ORDER preprocessor definition to indicated target host processor endianess.
 The target host processor endianess is determined during run time.
-Modified report callback for supported thermostat fan operating modes to support OFF mode
 for version >= 2 (instead of 3 previously).
-Enabled the use of "unused" state in zwif_thrmo_setb_set().
-Modified zwif_clmt_ctl_schd_set() to allow number of valid switchpoint equals to zero.
-Reduced the maximum size of a multi-command buffer to 46 bytes
-After multi-command encapsulation is started, whenever the multi-command buffer is full,
 it will be sent regardless whether the "multi-command encapsulation stop" command is
 received.
-Enabled the use of "unused" state in zwif_clmt_ctl_schd_ovr_set().
-Do not modify the override state when override type field is set to "No Override" in
 zwif_clmt_ctl_schd_ovr_set().

Version:5.04  (zw_api rel ver 3.02)                                     Date: 20-3-12
------------                                                            ------------
-Improved send data function by using conditional variable to wait for condition to send data.
-Multi-command encapsulation will be turned on automatically if a device supports multi-cmd.
-If the multi-command encapsulation mode is on and command queuing (for wake up command class)
 is on, all the commands for sending will be queued until the wake up notification is received.
 All the commands in the queue will then be encapsulated using multi-cmd for sending.
-If the multi-command encapsulation mode is on and command queuing (for wake up command class)
 is off, all the commands for sending will be sent after 1 second timeout.  If there is only one
 command for sending, the command will not be encapsulated using multi-cmd; otherwise, they will
 be sent using multi-cmd encapsulation.
-Allow application to check the current state of the multi-cmd mode.

Version:5.05  (zw_api rel ver 3.03)                                     Date: 29-3-12
------------                                                            ------------
-Support waiting for response (report) to a GET command before sending another command
 in nodes that requires wakeup beam.
-Reduced the security frame payload split threshold from 28 bytes to 26 bytes to take into
 account of Z-wave routed frame size which has a maximum payload size of 46 bytes.

Version:5.06  (zw_api rel ver 3.04)                                     Date: 18-4-12
------------                                                            ------------
-Changed the maximum wait time for sending a command to 7 seconds to take into account for nodes
 that require wake up beam.
-Changed wait-for-response (report) timeout to 10 seconds for nodes that requires wakeup beam.
-Start wait-for-response (report) timer after the GET command send status is received.
-Sending a command will wait until the completion of previous add/delete return route.
-For security encapsulation, internal nonce expiry time will start after nonce report
 was sent through Z-wave.

Version:5.07                                                            Date: 15-5-12
------------                                                            ------------
-Changed COMMAND_CLASS_NETWORK_MANAGEMENT_PRIMARY from 0x33 to 0x54
-Supported extended command class (2-byte command class).

Version:5.08  (zw_api rel ver 3.05)                                     Date: 18-5-12
------------                                                            ------------
-Added state-machine to query each endpoint for its supported security command classes
 after a node was added securely.
-Changed makefile to generate 32-bit binary code in OS X platform.

Version:5.09  (zw_api rel ver 3.06)                                     Date: 10-7-12
------------                                                            ------------
-Map thermostat setpoint supported type 0 to type 1 for Danfoss Living Connect workaround.
-When a controller is in a network without other nodes, it will assign itself as SIS.
-When a controller obtains a primary role status (either through a SUC creating a primary controller,
 or the other primary controller initiating a controller shift), it will assign itself as SIS if
 there is no SUC in the network.
-zwnet_assign() will only accept ZWNET_ROLE_SIS as role parameter.
-When a network consists a SIS, call to zwnet_migrate() and zwnet_assign() is disallowed.

Version:5.10                                                            Date: 9-10-12
------------                                                            ------------
-Removed zwnet_assign() API.
-Supported multiple instances of HC API.
-Supported auto configuration of slave devices with wake up and sensor command classes
 when added to the Z-wave network.
-Added configuration file "zw_api_test.cfg" for configuring default wake up interval,
 and device configuration file path.
-For sensor configuration, default association group id is 1.
-Remove duplicate command in command queues.

Version:5.11  (zw_api rel ver 3.07)                                     Date: 18-10-12
------------                                                            ------------
-Use single device specific settings file.
-Added device category definitions (DEV_XXX).

Version:5.12  (zw_api rel ver 3.08)                                     Date: 29-10-12
------------                                                            ------------
-Changed zwif_sensor_get() API to support multilevel sensor command class version 5.
-Supported multilevel sensor command class version 5
-Changed node info state-machine to query multilevel sensor supported types and units.
-Fixed TO# 03686:Persistent storage created by older versions are treated as corrupt.
-Changed persistent storage format.  All tags and length fields are stored in big-endian format
 Added file version field for future compatibility. Append CRC16 checksum at the end of file to
 improve integrity of the persistent data.

Version:5.13  (zw_api rel ver 3.09)                                     Date: 28-11-12
------------                                                            ------------
-Fixed TO# 03886: Wake up interval is set to 0 when device specific configuration has no records.
-Fixed TO# 03900: Deadlock scenario during node inclusion.
-Fixed TO# 03901: Initialization fails to complete when persistent storage has duplicate set
 of interfaces in an endpoint.
-Fixed TO# 03907: Repeated sending of KeepAlive AV cmd does not stop with KeyUp cmd
 in certain timing conditon.
-Added bitmap to indicate fields validity in device specific configuration record.
-Changed device matching algorithm to support "don't care" cases in device specific
 configuration records. Valid "don't care" combinations in the format (Manf id, Product Type, Product id)
 are : (X, X, X), (V, X, X) and (V, V, X)  where V="valid value"; X="don't care".
-Node will be updated with device category from the device specific configuration record.
-Provided flexibility to user application for choosing between built-in device specific configurations management,
 or user supplied device specific configurations management functions.
-Accepted empty SECURITY_COMMANDS_SUPPORTED_REPORT (i.e. the report contains no command class).

Version:5.14  (zw_api rel ver 3.10)                                     Date: 25-2-13
------------                                                            ------------
-Supported COMMAND_CLASS_FIRMWARE_UPDATE_MD version 3.
-Supported COMMAND_CLASS_ZWAVEPLUS_INFO
-Supported COMMAND_CLASS_DEVICE_RESET_LOCALLY
-Supported CRC16_ENCAP
-Supported COMMAND_CLASS_ASSOCIATION_GRP_INFO
-Supported COMMAND_CLASS_VERSION version 2.
-Auto-configure lifeline group.
-Auto-remove node upon receiving DEVICE_RESET_LOCALLY_NOTIFICATION
-Modified persistent storage to support field that is more than 255 bytes.
-If a secondary controller being assigned as SIS, it will configure lifeline in all
 always-on-slaves (AOS).
-If a controller isn't a SIS, it will send DEVICE_RESET_LOCALLY_NOTIFICATION to SIS before
 resetting itself.

Version:5.15  (zw_api rel ver 3.11)                                     Date: 5-4-13
------------                                                            ------------
-When sending multi-channel encapsulation frame, set source endpoint to 0.
-Added polling facility.
-Remove command recording facility.

Version:5.16  (zw_api rel ver 3.12)                                     Date: 25-4-13
------------                                                            ------------
-Fixed TO# 04115: SIGSEGV when used with a hung UZB controller
-Reject firmware update request if firmware file is 0 in size.

Version:5.17  (zw_api rel ver 3.13)                                     Date: 13-5-13
------------                                                            ------------
-Work-around for Astral 2-gang dimmer which requires source and destination endpoint id to be
 same when sending command to it.
-Streamlined all the state-machines.
-Query supported type/feature of some command classes during initialization process and cache
 them for later use.
-Set learn mode using option ZW_SET_LEARN_MODE_NWI for inclusion, and using option
 ZW_SET_LEARN_MODE_CLASSIC for exclusion.
-When performing the association set to a device that supports Multi Channel Association Command Class,
 use endpoint id = 1 for controller node. This will force the multi-channel device to send multi-channel
 encapsulated report to the controller.
-The interface passed as parameter to zwif_group_add() and zwif_group_del() can either be Association
 command class or Multi Channel Association Command Class.  The APIs will try to find the suitable
 interface based on the endpoints to add/delete.
-Check received message type against interface security property.

Version:5.18  (zw_api rel ver 3.14)                                     Date: 23-5-13
------------                                                            ------------
-Query maximum supported user codes during initialization process and cache
 it for later use.
-Close firmware file after receiving FIRMWARE_UPDATE_MD_STATUS_REPORT.
-Fixed TO:4151 - Network update fails to complete when a node is not reachable

Version:5.19  (zw_api rel ver 3.15)                                     Date: 27-5-13
------------                                                            ------------
-Renamed zwif_poll_rm_mul to zwnet_poll_rm_mul; zwif_poll_rm to zwnet_poll_rm.
-Supported Manufacturer Specific Command Class version 2.
-Supported COMMAND_CLASS_POWERLEVEL
-Added serial API support for power level related functionality.
-Modify error print out for COMMAND_CLASS_POWERLEVEL (to control).
-Removed auto-configuration of default wakeup interval on device with WAKE UP command class.
-Removed the DefaultWkUpInterval entry in configuration file "zw_api_tst.cfg".
-Supported Association CC, AGI CC and reset locally CC
-Modified firmware upgrade completion status to include wait time for target to reboot,
 which is specified in COMMAND_CLASS_FIRMWARE_UPDATE_MD version 3.
-Added zwif_thrmo_setp_get_poll() API.
-Added zwnoded_t structure with sleep_cap (to indicate node is capable to sleep),
 sensor (to indicate node is FLIRS) and wkup_intv (to indicate the cached version of wake up interval).

Version:5.20  (zw_api rel ver 3.16)                                     Date: 12-7-13
------------                                                            ------------
-Added file "cmdclass_info.txt" to document supported and control of command classes.
-Added valid_grp_cnt to if_grp_info_dat_t structure to indicate valid group info count.
-Changed Power level Interface APIs parameter names and documentation.
-Added support and control of Z-wave+ info command class version 2.
-Added control of multilevel sensor command class version 6.
-Changed zwif_sensor_get() and zwif_sensor_get_poll() to accept parameter type=0 as an
 indication to use SENSOR_MULTILEVEL_GET command version 1.
-Added zwif_sensor_sup_cache_get() to get supported sensor types and units from the cache.
-When receiving user code report with invalid user code (i.e. non ASCII digits), the report
 will be rejected.

Version:5.21  (zw_api rel ver 3.17)                                     Date: 26-7-13
------------                                                            ------------
-When receiving MULTI_CHANNEL_ASSOCIATION_REPORT, check for controller's node id and set
 its endpoint id to zero.
-Changed zwif_group_del() to check for controller in the endpoint list.  If controller is
 present in the list, change controller's endpoint id to 1.
-Added control of COMMAND_CLASS_ALARM (a.k.a. COMMAND_CLASS_NOTIFICATION) version 3.
-Fixed TO# 4512: Unable to complete inclusion of AEON DSC24 and DSC25 devices.

Version:5.22  (zw_api rel ver 3.18)                                     Date: 14-8-13
------------                                                            ------------
-Removed checking of ASCII digits in USER_CODE_REPORT.
-Set controller node id into group 1 of all endpoints that support association or mult-channel
 association command class after the node is included into the network.
-Changed default firmware fragment size for version 1 and 2 to 40 bytes.
-Changed minimum firmware fragment size for version 3 to 40 bytes.

Version:5.23  (zw_api rel ver 3.19)                                     Date: 27-8-13
------------                                                            ------------
-Supported local firmware update for Windows and Linux platform.
-Added directory "apm_driver" to store flash Auto Programming Mode driver.
-Query for wakeup capabilities, supported meter type and meter descriptor during inclusion
 and cache it for later use.
-Fixed TO# 04673: Does not Assign Return Routes for association Set and Wakeup Set doing inclusion.
-Fixed TO# 04674: Does not send Wakeup No More Information.
-Fixed TO# 04676: Did not do meter get_sup during node update

Version:5.24  (zw_api rel ver 3.20)                                     Date: 1-10-13
------------                                                            ------------
-Changed the interpretation of Manufacturer specific's device specific get/report as follows:
 The report should report the default device ID data and type field as they belong together.
 When being asked with device ID=0. I.e if ID=1 (serial number) is 'default' then a GET(id=0)
 should trigger a Report(Id=1, serial number data).
-Fixed TO# 04820:Unable to proceed with post inclusion process for battery operated device in Z-Ware.

Version:5.25  (zw_api rel ver 3.21)                                     Date: 14-10-13
------------                                                            ------------
-Fixed TO# 4797: Z-Ware cannot connect to a Serial API device with corrupted EEPROM.
-Added note to zwnet_init() : If the return value is ZW_ERR_NO_RESP, there is a possibility
 the controller's NVR is corrupted.  User may consider to reset the controller using zwnet_quick_reset() API.
-Added zwnet_quick_reset() API to reset controller without initialization of network.

Version:5.26  (zw_api rel ver 3.22)                                     Date: 18-10-13
------------                                                            ------------
-Restrict the error code ZW_ERR_NO_RESP in zwnet_init() to only condition where no response
 is received from serial API commands that return info from NVM.

TODO:
1. Remove duplicate command in wakeup command queue for polling
2. When a node is not respoding, skip all other commands in that node for polling.
3. Z-wave firmware update for 500 series.
5 add meter_get_sup to inclusion
6 assignreturnroute for any device we include
7 Handle condition where "nonce report" event arrives earlier than transmission status
  of "nonce get" in security message.




