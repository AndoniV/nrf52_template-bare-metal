/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      ble_if.c
 @brief     Provides a BLE interface
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_error.h"
#include "app_timer.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "fds.h"
#include "nordic_common.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_delay.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh.h"
#include "nrf_strerror.h"
#include "nrf.h"
#include "peer_manager_handler.h"
#include "peer_manager.h"
#include "app/device.h"
#include "ble/ble_if.h"
#include "ble/proto_ble_profile.h"

// DEFINITIONS ****************************************************************/

/** The advertising interval (in units of 0.625ms) */
#define BLE_IF_ADVERTISING_INTERVAL_UNITS               (668) // 417.5ms
/** The advertising duration (in units of 10ms), set forever */
#define BLE_IF_APP_ADVERTISING_DURATION_UNITS           (0)
/** Connection interval limits */
#define BLE_IF_MIN_CONN_INTERVAL_UNITS                  MSEC_TO_UNITS(7.5, UNIT_1_25_MS)
#define BLE_IF_MAX_CONN_INTERVAL_UNITS                  MSEC_TO_UNITS(10, UNIT_1_25_MS)
/** Slave latency */
#define BLE_IF_SLAVE_LATENCY                            (0)
/** Connection supervisory timeout */
#define BLE_IF_CONN_SUPERVISORY_TIMEOUT_UNITS           MSEC_TO_UNITS(8000, UNIT_10_MS)
/** Time from initiating an event (connect or start of notification) to the
    first time sd_ble_gap_conn_param_update is called */
#define BLE_IF_FIRST_CONN_PARAMS_UPDATE_DELAY_TICKS     APP_TIMER_TICKS(5000)
/** Time between each call to sd_ble_gap_conn_param_update after the first call */
#define BLE_IF_NEXT_CONN_PARAMS_UPDATE_DELAY_TICKS      APP_TIMER_TICKS(30000)
/** Number of attempts before giving up the connection parameter negotiation */
#define BLE_IF_MAX_CONN_PARAMS_UPDATE_ATTEMPTS          (3)

// DECLARATIONS ***************************************************************/

/** Whether the module is initialised */
static bool s_ble_if_initialised = false;
/** Buffer to hold service data to update */
static uint8_t s_ble_if_service_data[BLE_IF_SERVICE_DATA_1_LEN] = {0};
/** BLE connection handle */
static uint16_t s_ble_if_connection_handle = BLE_CONN_HANDLE_INVALID;
/** Advertised service UUIDs */
ble_uuid_t s_ble_if_adv_service_uuids[] = {{PROTO_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN}};
/** Buffer to hold data for characteristic writes and and its object */
static uint8_t s_ble_if_char_write_buf[512];
static ble_gatts_value_t s_gble_char_write_obj =
{
    .len = 512,
    .offset = 0,
    .p_value = s_ble_if_char_write_buf
};
/** Buffer to pass to the SoftDevice on memory requests and its object */
static uint8_t s_ble_if_sd_mem_buf[512];
static ble_user_mem_block_t s_ble_if_sd_mem_obj =
{
    .p_mem = s_ble_if_sd_mem_buf,
    .len = 512
};
static uint8_t s_ble_if_qwr_buff[512];

/** NRF BLE objects */
NRF_BLE_GATT_DEF(s_ble_if_gatt_obj);
NRF_BLE_QWR_DEF(s_ble_if_qwr_obj);
BLE_ADVERTISING_DEF(s_ble_if_advertising_obj);

/**
 * Initialise the BLE stack, i.e., the SoftDevice and BLE event callback
 */
static void ble_if_stack_init(void);
/**
 * Initialise the GAP parameters
 */
static void ble_if_gap_init(void);
/**
 * Initialise the GATT
 */
static void ble_if_gatt_init(void);
/**
 * Initialise all services
 */
static void ble_if_services_init(void);
/**
 * Initialise advertising
 */
static void ble_if_advertising_init(void);
/**
 * Initialise connection parameters
 */
static void ble_if_connection_params_init(void);
/**
 * Initialise the peer manager
 */
static void ble_if_peer_mgr_init(void);
/**
 * Helper to get the BLE stack event name from its ID
 * @param   event_id: To query
 * @return  The event name
 */
static const char *ble_if_get_stack_event_name(uint16_t event_id);
/**
 * Helper to get the BLE advertising event name from its ID
 * @param   event_id: To query
 * @return  The event name
 */
static const char *ble_if_get_advertising_event_name(uint16_t event_id);
/**
 * Helper to get the BLE connection parameters event name from its ID
 * @param   event_id: To query
 * @return  The event name
 */
static const char *ble_if_get_connection_params_event_name(uint16_t event_id);
/**
 * Helper to get the QWR module event name from its ID
 * @param   event_id: To query
 * @return  The event name
 */
static const char *ble_if_get_qwr_event_name(uint16_t event_id);
/**
 * Helper to get the BLE peer manager event name from its ID
 * @param   event_id: To query
 * @return  The event name
 */
static const char *ble_if_get_peer_mgr_event_name(uint16_t event_id);
/**
 * Called when a BLE advertising event occurs for the application
 * @param   ble_adv_evt: Advertising event
 */
static void ble_if_advertising_evt_callback(ble_adv_evt_t ble_adv_evt);
/**
 * Called when a BLE stack GAP event occurs for the application
 * @param   p_ble_evt: Stack event
 * @param   p_context: Unused
 */
static void ble_if_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context);
/**
 * Called when an event occurs in the Connection Parameters Module for the
 * application. This is only used for error handling during initialisation
 * @param   p_evt: Connection parameters event
 */
static void ble_if_connection_params_evt_callback(ble_conn_params_evt_t *p_evt);
/**
 * Called when an event occurs in the QWR module for the application
 * @param p_qwr: Pointer to the QWR object 
 * @param p_evt Pointer to the QWR module event
 * @return uint16_t 
 */
static uint16_t ble_if_qwr_evt_callback(nrf_ble_qwr_t *p_qwr, nrf_ble_qwr_evt_t *p_evt);
/**
 * Called when an error occurs in the Connection Parameters Module
 * @param   nrf_error: The error
 */
static void ble_if_connection_params_error_callback(uint32_t nrf_error);
/**
 * Called when an error occurs in the QWR module
 * @param nrf_error: The error
 */
static void ble_if_qwr_error_handler(uint32_t nrf_error);
/**
 * Called when a peer manager event occurs for the application
 * @param   p_evt: Peer manager event
 */
static void ble_if_peer_mgr_evt_callback(pm_evt_t const *p_evt);

// IMPLEMENTATIONS ************************************************************/

/**
 * Initialise the module
 * @retval  NP_SUCCESS: Success
 * @retval  NP_ERR_ERROR:   Failed to initialise
 */
ProtResult ble_if_init(void)
{
    ProtResult result = NP_SUCCESS;
    if (!s_ble_if_initialised)
    {
        ble_if_stack_init();
        ble_if_gap_init();
        ble_if_gatt_init();
        ble_if_services_init();
        ble_if_advertising_init();
        ble_if_connection_params_init();
        ble_if_peer_mgr_init();
        s_ble_if_initialised = true;
        NRF_LOG_INFO("[BLE] Initialised");
    }
    return result;
}

/**
 * Enable/disable advertising
 */
ret_code_t ble_if_advertising_enable(bool enable)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    NRF_LOG_INFO("%s advertising...", enable ? "Starting" : "Stopping");
    if (enable)
    {
        nrf_result = ble_advertising_start(&s_ble_if_advertising_obj, BLE_ADV_MODE_FAST);
    }
    return nrf_result;
}

/**
 * Initialise the BLE stack, i.e., the SoftDevice and BLE event callback
 */
static void ble_if_stack_init(void)
{
    ret_code_t nrf_result = nrf_sdh_enable_request();
    APP_ERROR_CHECK(nrf_result);
    // Configure the BLE stack using the default settings. Fetch the start
    // address of the application RAM
    uint32_t ram_start = 0;
    nrf_result = nrf_sdh_ble_default_cfg_set(1, &ram_start);
    APP_ERROR_CHECK(nrf_result);
    // Enable BLE stack
    nrf_result = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(nrf_result);
    // Register a handler for BLE GAP events
    NRF_SDH_BLE_OBSERVER(s_ble_if_stack_gap_evt_observer, BLE_HRS_BLE_OBSERVER_PRIO, ble_if_stack_gap_evt_callback, NULL);
}

/**
 * Initialise the GAP parameters
 */
static void ble_if_gap_init(void)
{
    // Set connection security mode
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    // Set device name
    ret_code_t nrf_result = sd_ble_gap_device_name_set(&sec_mode, device_get_serial_number(), 8);
    APP_ERROR_CHECK(nrf_result);
    // Set device BLE appearance
    nrf_result = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_COMPUTER);
    APP_ERROR_CHECK(nrf_result);
    // Set connection parameters
    ble_gap_conn_params_t gap_conn_params = {0};
    gap_conn_params.min_conn_interval = BLE_IF_MIN_CONN_INTERVAL_UNITS;
    gap_conn_params.max_conn_interval = BLE_IF_MAX_CONN_INTERVAL_UNITS;
    gap_conn_params.slave_latency = BLE_IF_SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = BLE_IF_CONN_SUPERVISORY_TIMEOUT_UNITS;
    nrf_result = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise the GATT
 */
static void ble_if_gatt_init(void)
{
    ret_code_t nrf_result = nrf_ble_gatt_init(&s_ble_if_gatt_obj, NULL);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise all services
 */
static void ble_if_services_init(void)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    nrf_ble_qwr_init_t  qwr_init = {0};
    // Initialise QWR module
    qwr_init.callback = ble_if_qwr_evt_callback;
    qwr_init.error_handler = ble_if_qwr_error_handler;
    qwr_init.mem_buffer.p_mem = s_ble_if_qwr_buff;
    qwr_init.mem_buffer.len = 512;
    nrf_result = nrf_ble_qwr_init(&s_ble_if_qwr_obj, &qwr_init);
    APP_ERROR_CHECK(nrf_result);
    // Initialise protocol service
    ProtoServiceInit proto_service_init = {0};
    proto_service_init.p_qwr_ctx = &s_ble_if_qwr_obj;
    nrf_result = proto_ble_service_init(&proto_service_init);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise advertising
 */
static void ble_if_advertising_init(void)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    ble_advertising_init_t init = {0};
    // Configure advertise data
    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.uuids_complete.uuid_cnt = ARRAY_SIZE(s_ble_if_adv_service_uuids);
    init.advdata.uuids_complete.p_uuids  = s_ble_if_adv_service_uuids;
    init.advdata.include_appearance = false;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    // Configure service data
    ble_advdata_service_data_t service_data_obj = {0};
    service_data_obj.service_uuid = PROTO_SERVICE_UUID;
    service_data_obj.data.p_data = s_ble_if_service_data;
    service_data_obj.data.size = ARRAY_SIZE(s_ble_if_service_data);
    init.advdata.p_service_data_array = &service_data_obj;
    init.advdata.service_data_count = 1;
    // Configure advertising parameters
    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = BLE_IF_ADVERTISING_INTERVAL_UNITS;
    init.config.ble_adv_fast_timeout = BLE_IF_APP_ADVERTISING_DURATION_UNITS;
    init.evt_handler = ble_if_advertising_evt_callback;
    // Initialise advertising
    nrf_result = ble_advertising_init(&s_ble_if_advertising_obj, &init);
    APP_ERROR_CHECK(nrf_result);
    ble_advertising_conn_cfg_tag_set(&s_ble_if_advertising_obj, 1);

    NRF_LOG_DEBUG("[BLE] ADV:");
    NRF_LOG_HEXDUMP_DEBUG(s_ble_if_advertising_obj.adv_data.adv_data.p_data,
        s_ble_if_advertising_obj.adv_data.adv_data.len);
    NRF_LOG_DEBUG("[BLE] SRP:");
    NRF_LOG_HEXDUMP_DEBUG(s_ble_if_advertising_obj.adv_data.scan_rsp_data.p_data,
        s_ble_if_advertising_obj.adv_data.scan_rsp_data.len);
}

/**
 * Initialise connection parameters
 */
static void ble_if_connection_params_init(void)
{
    ble_conn_params_init_t cp_init = {0};
    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = BLE_IF_FIRST_CONN_PARAMS_UPDATE_DELAY_TICKS;
    cp_init.next_conn_params_update_delay = BLE_IF_NEXT_CONN_PARAMS_UPDATE_DELAY_TICKS;
    cp_init.max_conn_params_update_count = BLE_IF_MAX_CONN_PARAMS_UPDATE_ATTEMPTS;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = ble_if_connection_params_evt_callback;
    cp_init.error_handler = ble_if_connection_params_error_callback;
    ret_code_t nrf_result = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise the peer manager
 */
static void ble_if_peer_mgr_init(void)
{
    ret_code_t nrf_result = pm_init();
    APP_ERROR_CHECK(nrf_result);
    // Security parameters to be used for all security procedures
    ble_gap_sec_params_t security_params = {0};
    security_params.bond = 1;
    security_params.mitm = 0;
    security_params.lesc = 0;
    security_params.keypress = 0;
    security_params.io_caps = BLE_GAP_IO_CAPS_NONE;
    security_params.oob = 0;
    security_params.min_key_size = 7;
    security_params.max_key_size = 16;
    security_params.kdist_own.enc = 1;
    security_params.kdist_own.id = 1;
    security_params.kdist_peer.enc = 1;
    security_params.kdist_peer.id = 1;
    nrf_result = pm_sec_params_set(&security_params);
    APP_ERROR_CHECK(nrf_result);
    // Register peer mngr callback
    nrf_result = pm_register(ble_if_peer_mgr_evt_callback);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Helper to get the BLE stack event name from its ID
 */
static const char *ble_if_get_stack_event_name(uint16_t event_id)
{
    switch (event_id)
    {
    case BLE_EVT_USER_MEM_REQUEST:                 return "USER_MEM_REQUEST";                //  1 Memory request for long writes
    case BLE_EVT_USER_MEM_RELEASE:                 return "USER_MEM_RELEASE";                //  2 Memory release for long writes
    case BLE_GAP_EVT_CONNECTED:                    return "GAP_CONNECTED";                   // 16 Connected to peer
    case BLE_GAP_EVT_DISCONNECTED:                 return "GAP_DISCONNECTED";                // 17 Disconnected from peer
    case BLE_GAP_EVT_CONN_PARAM_UPDATE:            return "GAP_CONN_PARAM_UPDATE";           // 18 Connection Parameters updated
    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:           return "GAP_SEC_PARAMS_REQUEST";          // 19 Request to provide security parameters
    case BLE_GAP_EVT_SEC_INFO_REQUEST:             return "GAP_SEC_INFO_REQUEST";            // 20 Request to provide security information
    case BLE_GAP_EVT_PASSKEY_DISPLAY:              return "GAP_PASSKEY_DISPLAY";             // 21 Request to display a passkey to the user
    case BLE_GAP_EVT_KEY_PRESSED:                  return "GAP_KEY_PRESSED";                 // 22 Notification of a keypress on the remote device
    case BLE_GAP_EVT_AUTH_KEY_REQUEST:             return "GAP_AUTH_KEY_REQUEST";            // 23 Request to provide an authentication key
    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:           return "GAP_LESC_DHKEY_REQUEST";          // 24 Request to calculate an LE Secure Connections DHKey
    case BLE_GAP_EVT_AUTH_STATUS:                  return "GAP_AUTH_STATUS";                 // 25 Authentication procedure completed with status
    case BLE_GAP_EVT_CONN_SEC_UPDATE:              return "GAP_CONN_SEC_UPDATE";             // 26 Connection security updated
    case BLE_GAP_EVT_TIMEOUT:                      return "GAP_TIMEOUT";                     // 27 Timeout expired
    case BLE_GAP_EVT_RSSI_CHANGED:                 return "GAP_RSSI_CHANGED";                // 28 RSSI report
    case BLE_GAP_EVT_ADV_REPORT:                   return "GAP_ADV_REPORT";                  // 29 Advertising report
    case BLE_GAP_EVT_SEC_REQUEST:                  return "GAP_SEC_REQUEST";                 // 30 Security Request
    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:    return "GAP_CONN_PARAM_UPDATE_REQUEST";   // 31 Connection Parameter Update Request
    case BLE_GAP_EVT_SCAN_REQ_REPORT:              return "GAP_SCAN_REQ_REPORT";             // 32 Scan request report
    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:           return "GAP_PHY_UPDATE_REQUEST";          // 33 PHY Update Request
    case BLE_GAP_EVT_PHY_UPDATE:                   return "GAP_PHY_UPDATE";                  // 34 PHY Update Procedure is complete
    case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:   return "GAP_DATA_LENGTH_UPDATE_REQUEST";  // 35 Data Length Update Request
    case BLE_GAP_EVT_DATA_LENGTH_UPDATE:           return "GAP_DATA_LENGTH_UPDATE";          // 36 LL Data Channel PDU payload length updated
    case BLE_GAP_EVT_QOS_CHANNEL_SURVEY_REPORT:    return "GAP_QOS_CHANNEL_SURVEY_REPORT";   // 37 Channel survey report
    case BLE_GAP_EVT_ADV_SET_TERMINATED:           return "GAP_ADV_SET_TERMINATED";          // 38 Advertising set terminated
    case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:         return "GATTC_PRIM_SRVC_DISC_RSP";        // 48 Primary Service Discovery Response event
    case BLE_GATTC_EVT_REL_DISC_RSP:               return "GATTC_REL_DISC_RSP";              // 49 Relationship Discovery Response event
    case BLE_GATTC_EVT_CHAR_DISC_RSP:              return "GATTC_CHAR_DISC_RSP";             // 50 Characteristic Discovery Response event
    case BLE_GATTC_EVT_DESC_DISC_RSP:              return "GATTC_DESC_DISC_RSP";             // 51 Descriptor Discovery Response event
    case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:         return "GATTC_ATTR_INFO_DISC_RSP";        // 52 Attribute Information Response event
    case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:  return "GATTC_CHAR_VAL_BY_UUID_READ_RSP"; // 53 Read By UUID Response event
    case BLE_GATTC_EVT_READ_RSP:                   return "GATTC_READ_RSP";                  // 54 Read Response event
    case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:         return "GATTC_CHAR_VALS_READ_RSP";        // 55 Read multiple Response event
    case BLE_GATTC_EVT_WRITE_RSP:                  return "GATTC_WRITE_RSP";                 // 56 Write Response event
    case BLE_GATTC_EVT_HVX:                        return "GATTC_HVX";                       // 57 Handle Value Notification or Indication event
    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:           return "GATTC_EXCHANGE_MTU_RSP";          // 58 Exchange MTU Response event
    case BLE_GATTC_EVT_TIMEOUT:                    return "GATTC_TIMEOUT";                   // 59 Timeout event
    case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:      return "GATTC_WRITE_CMD_TX_COMPLETE";     // 60 Write without Response transmission complete
    case BLE_GATTS_EVT_WRITE:                      return "GATTS_WRITE";                     // 80 Write operation performed
    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:       return "GATTS_RW_AUTHORIZE_REQUEST";      // 81 Read/Write Authorization request
    case BLE_GATTS_EVT_SYS_ATTR_MISSING:           return "GATTS_SYS_ATTR_MISSING";          // 82 A persistent system attribute access is pending
    case BLE_GATTS_EVT_HVC:                        return "GATTS_HVC";                       // 83 Handle Value Confirmation
    case BLE_GATTS_EVT_SC_CONFIRM:                 return "GATTS_SC_CONFIRM";                // 84 Service Changed Confirmation
    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:       return "GATTS_EXCHANGE_MTU_REQUEST";      // 85 Exchange MTU Request
    case BLE_GATTS_EVT_TIMEOUT:                    return "GATTS_TIMEOUT";                   // 86 Peer failed to respond to an ATT request in time
    case BLE_GATTS_EVT_HVN_TX_COMPLETE:            return "GATTS_HVN_TX_COMPLETE";           // 87 Handle Value Notification transmission complete
    default:                                       return "UNKNOWN";
    } // switch: event_id
}

/**
 * Helper to get the BLE advertising event name from its ID
 */
static const char *ble_if_get_advertising_event_name(uint16_t event_id)
{
    switch (event_id)
    {
    case BLE_ADV_EVT_IDLE:                  return "ADV_EVT_IDLE";
    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:    return "ADV_EVT_DIRECTED_HIGH_DUTY";
    case BLE_ADV_EVT_DIRECTED:              return "ADV_EVT_DIRECTED";
    case BLE_ADV_EVT_FAST:                  return "ADV_EVT_FAST";
    case BLE_ADV_EVT_SLOW:                  return "ADV_EVT_SLOW";
    case BLE_ADV_EVT_FAST_WHITELIST:        return "ADV_EVT_FAST_WHITELIST";
    case BLE_ADV_EVT_SLOW_WHITELIST:        return "ADV_EVT_SLOW_WHITELIST";
    case BLE_ADV_EVT_WHITELIST_REQUEST:     return "ADV_EVT_WHITELIST_REQUEST";
    case BLE_ADV_EVT_PEER_ADDR_REQUEST:     return "ADV_EVT_PEER_ADDR_REQUEST";
    default:                                return "UNKNOWN";
    } // switch: event_id
}

/**
 * Helper to get the BLE peer manager event name from its ID
 */
static const char *ble_if_get_peer_mgr_event_name(uint16_t event_id)
{
    switch (event_id)
    {
    case PM_EVT_BONDED_PEER_CONNECTED:              return "PM_EVT_BONDED_PEER_CONNECTED";
    case PM_EVT_CONN_CONFIG_REQ:                    return "PM_EVT_CONN_CONFIG_REQ";
    case PM_EVT_CONN_SEC_START:                     return "PM_EVT_CONN_SEC_START";
    case PM_EVT_CONN_SEC_SUCCEEDED:                 return "PM_EVT_CONN_SEC_SUCCEEDED";
    case PM_EVT_CONN_SEC_FAILED:                    return "PM_EVT_CONN_SEC_FAILED";
    case PM_EVT_CONN_SEC_CONFIG_REQ:                return "PM_EVT_CONN_SEC_CONFIG_REQ";
    case PM_EVT_CONN_SEC_PARAMS_REQ:                return "PM_EVT_CONN_SEC_PARAMS_REQ";
    case PM_EVT_STORAGE_FULL:                       return "PM_EVT_STORAGE_FULL";
    case PM_EVT_ERROR_UNEXPECTED:                   return "PM_EVT_ERROR_UNEXPECTED";
    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:         return "PM_EVT_PEER_DATA_UPDATE_SUCCEEDED";
    case PM_EVT_PEER_DATA_UPDATE_FAILED:            return "PM_EVT_PEER_DATA_UPDATE_FAILED";
    case PM_EVT_PEER_DELETE_SUCCEEDED:              return "PM_EVT_PEER_DELETE_SUCCEEDED";
    case PM_EVT_PEER_DELETE_FAILED:                 return "PM_EVT_PEER_DELETE_FAILED";
    case PM_EVT_PEERS_DELETE_SUCCEEDED:             return "PM_EVT_PEERS_DELETE_SUCCEEDED";
    case PM_EVT_PEERS_DELETE_FAILED:                return "PM_EVT_PEERS_DELETE_FAILED";
    case PM_EVT_LOCAL_DB_CACHE_APPLIED:             return "PM_EVT_LOCAL_DB_CACHE_APPLIED";
    case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:        return "PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED";
    case PM_EVT_SERVICE_CHANGED_IND_SENT:           return "PM_EVT_SERVICE_CHANGED_IND_SENT";
    case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:      return "PM_EVT_SERVICE_CHANGED_IND_CONFIRMED";
    case PM_EVT_SLAVE_SECURITY_REQ:                 return "PM_EVT_SLAVE_SECURITY_REQ";
    case PM_EVT_FLASH_GARBAGE_COLLECTED:            return "PM_EVT_FLASH_GARBAGE_COLLECTED";
    case PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED:    return "PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED";
    default:                                        return "UNKNOWN";
    } // switch: event_id
}

/**
 * Helper to get the BLE connection parameters event name from its ID
 */
static const char *ble_if_get_connection_params_event_name(uint16_t event_id)
{
    switch (event_id)
    {
    case BLE_CONN_PARAMS_EVT_FAILED:    return "CONN_PARAMS_EVT_FAILED";
    case BLE_CONN_PARAMS_EVT_SUCCEEDED: return "CONN_PARAMS_EVT_SUCCEDED";
    default:                            return "UNKNOWN";   
    } // switch: event_id
}

/**
 * Helper to get the QWR module event name from its ID
 */
static const char *ble_if_get_qwr_event_name(uint16_t event_id)
{
    switch (event_id)
    {
    case NRF_BLE_QWR_EVT_EXECUTE_WRITE: return "NRF_BLE_QWR_EVT_EXECUTE_WRITE";
    case NRF_BLE_QWR_EVT_AUTH_REQUEST:  return "NRF_BLE_QWR_EVT_AUTH_REQUEST";
    default:                            return "UNKNOWN";
    } // switch: event_id
}

/**
 * Called when a BLE stack GAP event occurs for the application
 */
static void ble_if_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    const char* name = ble_if_get_stack_event_name(p_ble_evt->header.evt_id);
    switch (p_ble_evt->header.evt_id)
    {
    // ========================================================================/
    case BLE_EVT_USER_MEM_REQUEST:
        nrf_result = sd_ble_user_mem_reply(p_ble_evt->evt.gap_evt.conn_handle, &s_ble_if_sd_mem_obj);
        if (nrf_result != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("[BLE] Failed to handle a memory request: %d", nrf_result);
        }
        break;
    // ========================================================================/
    case BLE_GAP_EVT_CONNECTED:
        s_ble_if_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
        nrf_result = nrf_ble_qwr_conn_handle_assign(&s_ble_if_qwr_obj, s_ble_if_connection_handle);
        if (NRF_SUCCESS != nrf_result)
        {
            NRF_LOG_ERROR("[BLE] Failed to assign connection handle to QWR module");
        }
        NRF_LOG_DEBUG("[BLE] Connected");
        break;
    // ========================================================================/
    case BLE_GAP_EVT_DISCONNECTED:
        s_ble_if_connection_handle = BLE_CONN_HANDLE_INVALID;
        NRF_LOG_DEBUG("[BLE] Disconnected");
        break;
    // ========================================================================/
    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        nrf_result = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        ble_gap_phys_t const phys =
        {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
        nrf_result = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(nrf_result);
        break;
    }
    // ========================================================================/
    case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
    {
        ble_gap_data_length_params_t dl_params = {BLE_GAP_DATA_LENGTH_AUTO};
        nrf_result = sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL);
        if (nrf_result != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("Failed to update data length: %d", nrf_result);
        }
        break;
    }
    // ========================================================================/
    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
        nrf_ble_gatt_eff_mtu_get(&s_ble_if_gatt_obj, s_ble_if_connection_handle);
        break;
    // ========================================================================/
    case BLE_GATTC_EVT_TIMEOUT:
        nrf_result = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(nrf_result);
    break;
    // ========================================================================/
    case BLE_GATTS_EVT_WRITE:
    {
        static uint16_t char_handle = 0x0000;
        char_handle = p_ble_evt->evt.gatts_evt.params.write.handle;
        if (char_handle == g_proto_ble_service.char_handles.cccd_handle)
        {
            NRF_LOG_WARNING("[BLE] Indications: %s", ble_srv_is_indication_enabled(p_ble_evt->evt.gatts_evt.params.write.data) ? "Enabled" : "Disabled");
        }
        else if (char_handle == g_proto_ble_service.char_handles.value_handle)
        {
            sd_ble_gatts_value_set(p_ble_evt->evt.gatts_evt.conn_handle, g_proto_ble_service.char_handles.value_handle, &s_gble_char_write_obj);
            // TODO: Relay information to protocol service, which will propagate received data to application to be processed
        }
        NRF_LOG_WARNING("[BLE] %s with char handle: %d, bytes received: %d", name, char_handle, p_ble_evt->evt.gatts_evt.params.write.len);
        NRF_LOG_HEXDUMP_WARNING(p_ble_evt->evt.gatts_evt.params.write.data, p_ble_evt->evt.gatts_evt.params.write.len);
        break;
    }
    // ========================================================================/
    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        NRF_LOG_WARNING("[BLE] %s with char handle: %d", name, p_ble_evt->evt.gatts_evt.params.write.handle);
        NRF_LOG_HEXDUMP_WARNING(p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.data,
                                p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.len);
        // TODO: Implement request authorisation
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        nrf_result = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gatts_evt.conn_handle, NULL, 0, 0);
        APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_TIMEOUT:
        nrf_result = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_HVC:
        // TODO: Clear protocol service indication ack pending flag
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
        sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gatts_evt.params.exchange_mtu_request.client_rx_mtu);
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        // TODO: Clear protocol service indication ack pending flag
        break;
    // ========================================================================/
    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    case BLE_GAP_EVT_PHY_UPDATE:
    case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
        // Do nothing
        break;
    // ========================================================================/
    case BLE_EVT_USER_MEM_RELEASE:
    case BLE_GAP_EVT_SEC_INFO_REQUEST:
    case BLE_GAP_EVT_PASSKEY_DISPLAY:
    case BLE_GAP_EVT_KEY_PRESSED:
    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
    case BLE_GAP_EVT_AUTH_STATUS:
    case BLE_GAP_EVT_CONN_SEC_UPDATE:
    case BLE_GAP_EVT_TIMEOUT:
    case BLE_GAP_EVT_RSSI_CHANGED:
    case BLE_GAP_EVT_ADV_REPORT:
    case BLE_GAP_EVT_SEC_REQUEST:
    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
    case BLE_GAP_EVT_SCAN_REQ_REPORT:
    case BLE_GAP_EVT_QOS_CHANNEL_SURVEY_REPORT:
    case BLE_GAP_EVT_ADV_SET_TERMINATED:
    case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
    case BLE_GATTC_EVT_REL_DISC_RSP:
    case BLE_GATTC_EVT_CHAR_DISC_RSP:
    case BLE_GATTC_EVT_DESC_DISC_RSP:
    case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
    case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
    case BLE_GATTC_EVT_READ_RSP:
    case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
    case BLE_GATTC_EVT_WRITE_RSP:
    case BLE_GATTC_EVT_HVX:
    case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
    case BLE_GATTS_EVT_SC_CONFIRM:
    default:
        NRF_LOG_WARNING("[BLE] Stack event received and not handled: %d, %s", p_ble_evt->header.evt_id, name);
        break;
    } // switch: event ID
}

/**
 * Called when a BLE advertising event occurs for the application
 */
static void ble_if_advertising_evt_callback(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    const char *name = ble_if_get_advertising_event_name(ble_adv_evt);
    switch (ble_adv_evt)
    {
    // ========================================================================/
    case BLE_ADV_EVT_IDLE:
    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
    case BLE_ADV_EVT_DIRECTED:
    case BLE_ADV_EVT_FAST:
    case BLE_ADV_EVT_SLOW:
    case BLE_ADV_EVT_FAST_WHITELIST:
    case BLE_ADV_EVT_SLOW_WHITELIST:
    case BLE_ADV_EVT_WHITELIST_REQUEST:
    case BLE_ADV_EVT_PEER_ADDR_REQUEST:
    default:
        break;
    } // switch: event type
    NRF_LOG_WARNING("[BLE] Advertsing event received: %d, %s", ble_adv_evt, name);
}

/**
 * Called when an event occurs in the Connection Parameters Module for the
 * application. This is only used for error handling during initialisation
 */
static void ble_if_connection_params_evt_callback(ble_conn_params_evt_t *p_evt)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    const char *name = ble_if_get_connection_params_event_name(p_evt->evt_type);
    switch (p_evt->evt_type)
    {
    // ========================================================================/
    case BLE_CONN_PARAMS_EVT_FAILED:
        nrf_result = sd_ble_gap_disconnect(s_ble_if_connection_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        if (NRF_SUCCESS != nrf_result)
        {
            NRF_LOG_ERROR("[BLE] Failed to disconnect at BLE_CONN_PARAMS_EVT_FAILED");
        }
        break;
    // ========================================================================/
    case BLE_CONN_PARAMS_EVT_SUCCEEDED:
    default:
        NRF_LOG_WARNING("[BLE] Connection params event received: %d, %s", p_evt->evt_type, name);
        break;
    } // switch: event type
}

/**
 * Called when an event occurs in the QWR module for the application
 */
static uint16_t ble_if_qwr_evt_callback(nrf_ble_qwr_t *p_qwr, nrf_ble_qwr_evt_t *p_evt)
{
    uint16_t gatt_status = BLE_GATT_STATUS_SUCCESS;
    const char *name = ble_if_get_qwr_event_name(p_evt->evt_type);
    switch (p_evt->evt_type)
    {
    // ========================================================================/
    case NRF_BLE_QWR_EVT_EXECUTE_WRITE:
    case NRF_BLE_QWR_EVT_AUTH_REQUEST:
    default:
        NRF_LOG_WARNING("[BLE] QWR event received: %d, %s", p_evt->evt_type, name);
        break;
    } // switch: event type
    return gatt_status;
}

/**
 * Called when an error occurs in the Connection Parameters Module
 */
static void ble_if_connection_params_error_callback(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * Called when an error occurs in the QWR module
 */
static void ble_if_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * Called when a peer manager event occurs for the application
 */
static void ble_if_peer_mgr_evt_callback(pm_evt_t const *p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);
    const char *name = ble_if_get_peer_mgr_event_name(p_evt->evt_id);
    switch (p_evt->evt_id)
    {
    // ========================================================================/
    case PM_EVT_BONDED_PEER_CONNECTED:
    case PM_EVT_CONN_CONFIG_REQ:
    case PM_EVT_CONN_SEC_START:
    case PM_EVT_CONN_SEC_SUCCEEDED:
    case PM_EVT_CONN_SEC_FAILED:
    case PM_EVT_CONN_SEC_CONFIG_REQ:
    case PM_EVT_CONN_SEC_PARAMS_REQ:
    case PM_EVT_STORAGE_FULL:
    case PM_EVT_ERROR_UNEXPECTED:
    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
    case PM_EVT_PEER_DATA_UPDATE_FAILED:
    case PM_EVT_PEER_DELETE_SUCCEEDED:
    case PM_EVT_PEER_DELETE_FAILED:
    case PM_EVT_PEERS_DELETE_SUCCEEDED:
    case PM_EVT_PEERS_DELETE_FAILED:
    case PM_EVT_LOCAL_DB_CACHE_APPLIED:
    case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
    case PM_EVT_SERVICE_CHANGED_IND_SENT:
    case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
    case PM_EVT_SLAVE_SECURITY_REQ:
    case PM_EVT_FLASH_GARBAGE_COLLECTED:
    case PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED:
    default:
        break;
    } // switch: event ID
    NRF_LOG_WARNING("[BLE] Peer manager event received: %d, %s", p_evt->evt_id, name);
}
