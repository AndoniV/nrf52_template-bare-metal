/***************************************************************************//**
 @author    A Villarreal
 @date      06/03/23
 @file      proto_ble_profile.c
 @brief     Provides the protocol BLE profile specification
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "nrf_ble_gatt.h"
// #include "nrf_ble_qwr.h"
#include "nrf_delay.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh.h"
#include "nrf_strerror.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "peer_manager_handler.h"
#include "peer_manager.h"
#include "ble/ble_if.h"
#include "ble/proto_ble_profile.h"

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/** Protocol service */
ProtoService g_proto_ble_service = {0};
/** Protocol characteristic write callback */
static proto_ble_char_write_callback_t s_proto_ble_char_write_callback = NULL;

/**
 * Initialise protocol characteristics
 * @return NRF_SUCCESS if successfiul
 */
static ret_code_t proto_ble_service_init_chars(void);
/**
 * Called when a BLE stack GAP event occurs for the application
 * @param   p_ble_evt: Stack event
 * @param   p_context: Unused
 */
static void proto_ble_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context);

// IMPLEMENTATION *************************************************************/

/**
 * Initialise the protocol service
 */
ret_code_t proto_ble_service_init(ProtoServiceInit *p_init)
{
    ret_code_t nrf_result = NRF_ERROR_NULL;
    if ((p_init == NULL) || (p_init->p_qwr_ctx == NULL))
    {
        return nrf_result;
    }
    // Add stack event observer
    NRF_SDH_BLE_OBSERVER(s_proto_ble_stack_observer, BLE_HRS_BLE_OBSERVER_PRIO, proto_ble_stack_gap_evt_callback, NULL);
    // Disable indications
    g_proto_ble_service.indications_enabled = false;
    g_proto_ble_service.conn_handle = BLE_CONN_HANDLE_INVALID;
    // Add the service's base UUID
    ble_uuid128_t base_uuid = PROTO_SERVICE_BASE_UUID;
    nrf_result = sd_ble_uuid_vs_add(&base_uuid, &g_proto_ble_service.uuid_type);
    APP_ERROR_CHECK(nrf_result);
    // Add the service to the stack
    ble_uuid_t ble_uuid = {0};
    ble_uuid.type = g_proto_ble_service.uuid_type;
    ble_uuid.uuid = PROTO_SERVICE_UUID;
    nrf_result = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &g_proto_ble_service.service_handle);
    APP_ERROR_CHECK(nrf_result);
    // Add charactaristics
    nrf_result = proto_ble_service_init_chars();
    APP_ERROR_CHECK(nrf_result);
    // Register QWR to the service attribute
    nrf_result = nrf_ble_qwr_attr_register(p_init->p_qwr_ctx, g_proto_ble_service.char_handles.value_handle);
    APP_ERROR_CHECK(nrf_result);

    return nrf_result;
}

/**
 * Register protocol characteristic write callback
 */
ProtResult proto_ble_register_char_write_callback(proto_ble_char_write_callback_t callback)
{
    ProtResult result = NP_ERR_PARAM;
    if (callback != NULL)
    {
        s_proto_ble_char_write_callback = callback;
        result = NP_SUCCESS;
    }
    return result;
}

/**
 * Initialise protocol characteristics
 */
static ret_code_t proto_ble_service_init_chars(void)
{
    ret_code_t nrf_result = NRF_SUCCESS;

    // Set characteristic discovery properties
    ble_gatts_char_md_t char_md = {0};
    ble_gatts_attr_md_t cccd_md = {0};
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    char_md.char_props.indicate = 1;
    char_md.char_props.notify = 0;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = NULL;
    char_md.p_cccd_md = &cccd_md;
    char_md.p_sccd_md = NULL;

    // Define characteristic descriptor
    const char char_desc[8 + 1] = "Protocol";
    char_md.char_user_desc_max_size = 16;
    char_md.char_user_desc_size = 8;
    char_md.p_char_user_desc = (uint8_t*)char_desc;

    // Set characteristic properties
    ble_gatts_attr_md_t attr_md = {0};
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 1;

    // Add the characteristic base UUID
    ble_uuid128_t base_uuid = PROTO_CHAR_BASE_UUID;
    nrf_result = sd_ble_uuid_vs_add(&base_uuid, (uint8_t *)&g_proto_ble_service.uuid_type);
    APP_ERROR_CHECK(nrf_result);

    // Set characterstic UUID
    ble_uuid_t char_uuid = {0};
    char_uuid.type = g_proto_ble_service.uuid_type;
    char_uuid.uuid = PROTO_CHAR_UUID;

    // Set characteristic value properties
    ble_gatts_attr_t attr_char_value = {0};
    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len = 512;
    attr_char_value.init_len = 0;
    attr_char_value.init_offs = 0;
    // attr_char_value.p_value = &val;

    // Set characteristic value permissions
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    cccd_md.vlen = 1;

    // Add the characteristic to the service
    nrf_result = sd_ble_gatts_characteristic_add(g_proto_ble_service.service_handle,
            &char_md, &attr_char_value, (ble_gatts_char_handles_t*)&g_proto_ble_service.char_handles);
    APP_ERROR_CHECK(nrf_result);

    NRF_LOG_INFO("[BLE] Protocol char added with handle value: 0x%02X", g_proto_ble_service.char_handles.value_handle);
    
    return nrf_result;
}

/**
 * Called when a BLE stack GAP event occurs for the application
 */
static void proto_ble_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
    // ========================================================================/
    case BLE_GAP_EVT_CONNECTED:
        g_proto_ble_service.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        break;
    // ========================================================================/
    case BLE_GAP_EVT_DISCONNECTED:
        g_proto_ble_service.conn_handle = BLE_CONN_HANDLE_INVALID;
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_WRITE:
        break;
    // ========================================================================/
    default:
        break;
    } // switch: event ID
}
