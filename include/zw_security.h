/**
@file   zw_security.h - Z-wave security functions header file.

        To implement some security functions.

@author David Chow

@version    1.0 18-1-2011  Initial release

version: 1.0
comments: Initial release
*/

#ifndef _ZW_SECURITY_DAVID_
#define _ZW_SECURITY_DAVID_
/**
@defgroup Security Security Functions
Some security functions.
@{
*/
//#define DEBUG_ZWAVE_SECURITY    // flag for debugging security layer

#define SEC_TIMER_TICK              200     /**< Periodic timer tick interval in ms */
#define INTERNAL_NONCE_TABLE_CHECK  5       /**< Internal nonce table check interval in terms of timer tick */
#define INTERNAL_NONCE_LIFE         (5*3)   /**< Internal nonce life span in terms of SEC_TIMER_TICK*/
#define INTERNAL_NONCE_TABLE_SIZE   38      /**< Number of entries in the internal nonce table */
#define EXTERNAL_NONCE_LIFE         (5*1)   /**< External nonce life span in terms of SEC_TIMER_TICK*/
#define NONCE_REQ_TIMEOUT           10000   /**< Nonce request timeout in ms*/
#define MAX_SPP_SIZE                48      /**< Maximum security payload package (SPP) size*/
#define INCL_STA_TIMEOUT            10000   /**< Inclusion of nodes state timeout in ms*/
#define SEQ_NUMBER_VALID_BIT        0x80    /**< Sequence number validity bit for split SPP*/
#define MAX_SEC_TX_Q                8       /**< Maximum original payload packages (OPP) that can be queued*/
#define OPP_SPLIT_TRESHOLD          26      /**< Original payload packages (OPP) split threshold*/

/** Sequence flag */
#define FLG_SEQUENCE        0x10        /**< Indicate this is a sequenced frame*/
#define FLG_SECOND_FRM      0x20        /**< Indicate this is second frame*/

/** Internal nonce state */
#define INONCE_STA_INVALID  0           /**< Nonce is invalid*/
#define INONCE_STA_INIT     1           /**< Nonce is initialized but timeout value is not set yet*/
#define INONCE_STA_VALID    2           /**< Nonce is valid*/

/** Internal nonce */
typedef struct
{
    uint8_t     state;                  /**< State of this nonce INONCE_STA_XXX*/
    uint8_t     rcv_nodeid;             /**< Receiver node id*/
    uint16_t    expiry_tick;            /**< Expiry timer tick */
    uint8_t     nonce[8];               /**< Internal nonce */
} zwsec_inonce_t;


/** External nonce */
typedef struct
{
    uint8_t     valid;                  /**< Validity of this nonce. 1= valid; 0= invalid*/
    uint8_t     snd_nodeid;             /**< Sender node id*/
    uint16_t    expiry_tick;            /**< Expiry timer tick */
    uint8_t     nonce[8];               /**< External nonce */
} zwsec_enonce_t;


/** Pseudo-random number generator context */
typedef struct  _random_ctx
{
    uint8_t             inner_state[16];    /**< inner state of the PRNG */
    appl_layer_ctx_t    *appl_ctx;          /**< application layer context */
} zwsec_random_t;


/** Security layer transmission state-machine events */
typedef enum
{
    EVT_OPP_RCVD,                       ///< Received original payload package (OPP)
    EVT_EXT_NONCE_RCVD,                 ///< Received external nonce
    EVT_TX_STATUS_RCVD,                 ///< Received transmission status
    EVT_NONCE_REQ_TIMEOUT               ///< Nonce request timeout

} zwsec_tx_evt_t;


/** Security layer transmission state-machine's states */
typedef enum
{
    ZWSEC_STA_IDLE,                 ///< Waiting for event
    ZWSEC_STA_WAIT_GET_NONCE_TX_STS,///< Waiting for Nonce Get transmission status
    ZWSEC_STA_WAIT_SPP_TX_STS,      ///< Waiting for SPP transmission status
    ZWSEC_STA_WAIT_NONCE            ///< Waiting for external nonce

} zwsec_tx_sta_t;


typedef void (*zwsec_tx_sm_fn)(zwnet_p nw, uint8_t nodeid, int status, void **user);
/**<
security layer transmission callback
@param[in]	nw		network
@param[in]  nodeid	destination node id of the transmission
@param[in]  status	0= transmission ok; else negative error number
@param[in]	user	user context
*/

/** Parameters for the security tx state-machine */
typedef struct
{
    zwsec_tx_sm_fn      cb;             /**< SPP transmission callback function*/
    void                *user[2];       /**< User defined context that will be passed as parameter by cb function*/
    uint8_t             use_0_nw_key;   /**< Flag to determine whether to use all-zeroes network key as in
                                             sending NETWORK_KEY_SET command*/
    uint8_t             nodeid;         /**< Destination node id*/
    uint8_t             opp[1];         /**< place holder for the OPP with first byte to indicate length of the OPP */

} zwsec_tx_sm_param_t;


/** Security layer included node state-machine events */
typedef enum
{
    EVT_UNSECURED_INC_DONE,             ///< Unsecured inclusion completed
    EVT_SCHEME_GET,                     ///< Received SECURITY_SCHEME_GET command
    EVT_KEY_SET,                        ///< Received NETWORK_KEY_SET command
    EVT_SCHEME_INHERIT,                 ///< Received SECURITY_SCHEME_INHERIT command
    EVT_INCD_TIMEOUT                    ///< State timeout

} zwsec_incd_evt_t;


/** Security layer included node state-machine's states */
typedef enum
{
    ZWSEC_INCD_IDLE,            ///< Waiting for event
    ZWSEC_STA_WAIT_SCH_GET,     ///< Waiting for SECURITY_SCHEME_GET command
    ZWSEC_STA_WAIT_NW_KEY,      ///< Waiting for network key (NETWORK_KEY_SET command)
    ZWSEC_STA_WAIT_SCH_INHERIT  ///< Waiting for SECURITY_SCHEME_INHERIT command

} zwsec_incd_sta_t;


typedef void (*zwsec_incd_sm_fn)(zwnet_p nw, int status, void *user);
/**<
security layer included node callback
@param[in]	nw		network
@param[in]  status	0= secure inclusion ok; else negative error number
@param[in]	user	user context
*/


/** Parameters for the security included node state-machine */
typedef struct
{
    zwsec_incd_sm_fn    cb;             /**< completion callback function*/
    void                *user;          /**< user defined context that will be passed as parameter by cb function*/
    uint16_t            delay_ms;       /**< delay in milliseconds before start sending SECURITY_SCHEME_GET command*/

} zwsec_incd_sm_param_t;


/** Security layer add node state-machine events */
typedef enum
{
    EVT_UNSEC_INC_DONE,                 ///< Unsecured inclusion completed
    EVT_SCHEME_RPT,                     ///< Received SECURITY_SCHEME_REPORT command
    EVT_KEY_VERIFY,                     ///< Received NETWORK_KEY_VERIFY command
    EVT_ADD_TIMEOUT                     ///< State timeout

} zwsec_add_evt_t;


/** Security layer add node state-machine's states */
typedef enum
{
    ZWSEC_ADD_IDLE,             ///< Waiting for event
    ZWSEC_STA_WAIT_SCHEME,      ///< Waiting for SECURITY_SCHEME_REPORT command
    ZWSEC_STA_WAIT_VERIFY,      ///< Waiting for network key verification (NETWORK_KEY_VERIFY command)
    ZWSEC_STA_WAIT_INHERIT_RPT  ///< Waiting for SECURITY_SCHEME_REPORT in response to SECURITY_SCHEME_INHERIT command

} zwsec_add_sta_t;


/** Security layer context */
typedef struct  _sec_layer_ctx
{
    void                *tick_tmr_ctx;      /**< Tick timer context */
    void                *tx_sm_tmr_ctx;     /**< Tx state machine timer context*/
    void                *incd_sm_tmr_ctx;   /**< Included node state machine timer context*/
    void                *add_sm_tmr_ctx;    /**< Add node state machine timer context*/
    void                *sec_mtx;           /**< Mutex for security layer */
    volatile uint16_t   tmr_tick;           /**< Periodic timer tick, updated every SEC_TIMER_TICK ms */
    uint16_t            clean_up_tick;      /**< Internal nouce table clean up timer tick */
    zwsec_inonce_t      inonce_tbl[INTERNAL_NONCE_TABLE_SIZE];  /**< Internal nonce table */
    zwsec_enonce_t      ext_nonce;          /**< External nonce */
    zwsec_random_t      prng_ctx;           /**< Pseudo-random number generator context */
    uint8_t             sup_schm;           /**< Supported security schemes */
    uint8_t             inherit_schm;       /**< Inherited security schemes */
    uint8_t             nw_key[16];         /**< Network key */
    uint8_t             auth_key[16];       /**< Authentication key */
    uint8_t             enc_key[16];        /**< Encryption key */
    uint8_t             auth_0_key[16];     /**< Authentication key derived from all-zeroes network key*/
    uint8_t             enc_0_key[16];      /**< Encryption key derived from all-zeroes network key*/
    volatile zwsec_tx_sta_t  tx_sm_sta;     /**< Tx state-machine's state */
    uint8_t             tx_sm_opp[29*2];    /**< Tx state-machine's buffer to store OPP when external nonce is unavailable.
                                                 Note the first byte is the data length */
    uint8_t             tx_sm_2nd_frm[29];  /**< Tx state-machine's buffer to store second frame of split OPP.
                                                 Note the first byte is the data length */
    uint8_t             tx_sm_nodeid;       /**< Tx state-machine's destination node id */
    uint8_t             tx_sm_use_0_key;    /**< Flag to determine whether to use all-zeroes network key */
    uint8_t             tx_sm_q_cnt;        /**< Number of entries in tx OPP linked list*/
    zwsec_tx_sm_fn      tx_sm_cb;           /**< Callback function when transmission is complete or with error */
    void                *tx_sm_user[2];     /**< User defined context that will be passed as parameter by callback function*/
    util_lst_t          *tx_sm_opp_hd;      /**< Head of linked list of OPP*/
    zwsec_incd_sta_t    incd_sm_sta;        /**< Included node state-machine's state */
    zwsec_incd_sm_fn    incd_sm_cb;         /**< Callback function when included node operation is complete or with error */
    void                *incd_sm_user;      /**< User defined context that will be passed as parameter by callback function*/
    zwsec_add_sta_t     add_sm_sta;         /**< Add node state-machine's state */
    zwsec_incd_sm_fn    add_sm_cb;          /**< Callback function when add node operation is complete or with error */
    void                *add_sm_user;       /**< User defined context that will be passed as parameter by callback function*/
    uint8_t             add_sm_nodeid;      /**< Add node state-machine's destination node id */
    uint8_t             init_done;          /**< Flag to determine if initialization done. */
//  uint8_t             enonce_rcvd;        /**< Flag to indicate enonce was received ealier than tx status of nonce get. */
    zwnet_p             net;                /**< Network */
#ifdef INTERNAL_TEST
    uint32_t            tx_tmout_cnt;       /**< Transmission timeout count */
#endif
} zwsec_layer_t;



/** Message Authentication Code (MAC) input */
typedef struct
{
    uint8_t     i_vec[16];    /**< initialization vector */
    uint8_t     sec_hdr;      /**< security header */
    uint8_t     sender_nid;   /**< sender node id */
    uint8_t     rcvr_nid;     /**< receiver node id */
    uint8_t     payload_len;  /**< encrypted payload length (max. 29 bytes)*/
    uint8_t     payload[29];  /**< encrypted payload */
} zwsec_mac_t;


unsigned zwsec_rand_output(zwsec_random_t *random_ctx, uint8_t rand_num_cnt, uint8_t *rand_out);
/**<
get pseudo-random number
@param[in,out]	random_ctx	    random number generator context
@param[in]	    rand_num_cnt	number of bytes required (maximum 16 bytes)
@param[out]	    rand_out	    generated pseudo-random number
@return  number of bytes generated
*/


int zwsec_rand_init(zwsec_random_t *random_ctx);
/**<
initialize the pseudo-random number generator
@param[in,out]	random_ctx	random number generator context
@return  0 on success; negative error number on failure
*/


void zwsec_aes_encrypt(const uint8_t *msg, const uint8_t *key, uint8_t *c);
/**<
encrypt a single 128 bit block by a 128 bit key using AES
@param[in]	msg	    plain text message
@param[in]	key	    encryption key
@param[out]	c	    cipher text (encryption output)
@return
*/


void zwsec_aes_decrypt(const uint8_t *c, const uint8_t *key, uint8_t *m);
/**<
decrypt a single 128 bit block by a 128 bit key using AES
@param[in]	c	    cipher text
@param[in]	key	    encryption key
@param[out]	m	    plain text message (Decryption output)
@return
*/


void zwsec_keys_gen(const uint8_t *nw_key, uint8_t *auth_key, uint8_t *enc_key);
/**<
generate authentication and encryption keys
@param[in]	nw_key	    network key
@param[out]	auth_key	authentication key
@param[out]	enc_key	    encryption key
@return
*/


void zwsec_aes_ofb(const uint8_t *enc_key, const uint8_t *i_vec, uint8_t *buf, uint8_t buf_len);
/**<
encrypt/decrypt a message block by a 128 bit key using AES Output Feedback mode
@param[in]	enc_key	        encryption key
@param[in]	i_vec	        initialization vector
@param[in]	buf_len	        length of the plain/cipher text input data
@param[in,out]	buf	        buffer to store the plain/cipher text for input and cipher/plain text for output
@return
*/


int zwsec_mac_cal(zwsec_mac_t *mac_in, uint8_t *auth_key, uint8_t *mac_out);
/**<
calculate Message Authentication Code (MAC) using AES CBC mode
@param[in]	    mac_in	        Input data for the calculation
@param[in]	    auth_key	    Authentication key
@param[out]	    mac_out	        Calculated 8-byte MAC
@return  0 on success; negative error number on failure
*/


int zwsec_init(zwsec_layer_t *sec_ctx);
/**<
initialize the security layer
@param[in]	sec_ctx	    Security layer context
@return  0 on success; negative error number on failure
*/


void zwsec_exit(zwsec_layer_t *sec_ctx);
/**<
clean up
@param[in]	sec_ctx	    Security layer context
@return
*/


int zwsec_inonce_gen(zwsec_layer_t *sec_ctx, uint8_t node_id, uint8_t state);
/**<
generate an internal nonce and store it into the table
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    node_id	    Receiver node id of the generated nonce
@param[in]	    state	    The state (INONCE_STA_VALID or INONCE_STA_INIT) assigned to the generated nonce
@return  an index to the table (0 to INTERNAL_NONCE_TABLE_SIZE-1) on success; otherwise ZW_ERR_XXX
@pre Caller should lock the mutext sec_mtx before calling this function.
*/


void zwsec_inonce_expire_set(zwsec_layer_t *sec_ctx, int index);
/**<
set the expiry time of the internal nonce
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    index	    Index to the internal nonce table which is given by the zwsec_inonce_gen()
@return
*/


int zwsec_inonce_get(zwsec_layer_t *sec_ctx, uint8_t rcv_nonce_id);
/**<
get an internal nonce from the table
@param[in]	sec_ctx	        Security layer context
@param[in]	rcv_nonce_id	Receiver nonce id (the first byte of the nonce)
@return  an index to the table (0 to INTERNAL_NONCE_TABLE_SIZE-1) on success; otherwise ZW_ERR_XXX
*/


void zwsec_inonce_rm(zwsec_layer_t *sec_ctx, uint8_t node_id);
/**<
remove an internal nonce from the table
@param[in]	sec_ctx	        Security layer context
@param[in]	node_id	        Receiver node id of the nonce
@return
*/


void zwsec_enonce_put(zwsec_layer_t *sec_ctx, uint8_t *ext_nonce, uint8_t node_id);
/**<
store an external nonce
@param[in,out]	sec_ctx	    Security layer context
@param[in]	    node_id	    Node id of the originator of the external nonce
@param[in]	    ext_nonce	External nonce
@return
*/


int zwsec_depacket(zwnet_p nw, uint8_t src_node, uint8_t spp_len, uint8_t *spp, uint8_t **opp);
/**<
convert a received SPP to OPP
@param[in]	nw		    network
@param[in]	src_node    node id of the sender
@param[in]	spp_len     length of the SPP
@param[in]	spp         security payload package (SPP)
@param[out] opp         OPP after the depacketization and decryption; first byte indicating the OPP length
@return 0=complete OPP frame is returned through param opp, 1=second frame pending; ZW_ERR_XXX on error.
@post Caller must free the buffer allocated to opp if the return value is 0.
*/


int zwsec_send(zwifd_p ifd, zwsec_tx_sm_param_t *tx_param);
/**<
convert an OPP into SPP and send it
@param[in]	ifd	        interface
@param[in]	tx_param    transmission parameter
@return	ZW_ERR_xxx
*/


void zwsec_nonce_rpt_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *nonce);
/**<
received nonce handler
@param[in]  nw		Network
@param[in]  nodeid	Source node id
@param[in]	nonce	nonce
*/

int zwsec_incd_node_sm(zwnet_p  nw, uint8_t nodeid, zwsec_incd_evt_t  evt, void *data);
/**<
security layer included node state-machine
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return Status of the processing of event
*/


void zwsec_incd_msg_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t cmd_secure);
/**<
security layer included node message handler
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] cmd_buf  Command buffer
@param[in] cmd_len  Command length
@param[in] cmd_secure  Flag to indicate the command is received securely. 1= secure, 0= non-secure
@return
*/


int zwsec_add_node_sm(zwnet_p  nw, uint8_t nodeid, zwsec_add_evt_t  evt, void *data);
/**<
security layer adding node (node inclusion) state-machine
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] evt      The event for the state-machine
@param[in] data     The data associated with the event
@return Status of the processing of event
*/


void zwsec_add_node_msg_hdlr(zwnet_p  nw, uint8_t nodeid, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t cmd_secure);
/**<
security layer add node message handler
@param[in] nw		Network
@param[in] nodeid	Source node id
@param[in] cmd_buf  Command buffer
@param[in] cmd_len  Command length
@param[in] cmd_secure  Flag to indicate the command is received securely. 1= secure, 0= non-secure
@return
*/


int zwsec_cmd_sup_get(zwifd_p ifd);
/**
request commands the device supports using Security Encapsulation through report callback
@param[in]	ifd     interface
@return		ZW_ERR_xxx
*/


/**
@}
*/



#endif /* _ZW_SECURITY_DAVID_ */

