/***************************************************************************//**
 @author    A Villarreal
 @date      06/03/23
 @file      proto_ble_profile.h
 @brief     Provides the protocol BLE profile specification
 ******************************************************************************/

#ifndef PROT_BLE_PROFILE_H
#define PROT_BLE_PROFILE_H

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "build_config.h"
#include "nrf.h"
#include "nrf_ble_qwr.h"
#include "nrf_sdh.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** Protocol service base UUID */
#define PROTO_SERVICE_BASE_UUID {{0xFC, 0x10, 0x2C, 0xE9, 0xC2, 0x4B, 0x42, 0x52, 0x9A, 0xDE, 0xEE, 0x03, 0xFC, 0xC0, 0x80, 0x6A}}
/** Protocol service UUID */
#define PROTO_SERVICE_UUID      (0xC0FC)
/** Protocol charcteristic base UUID */
#define PROTO_CHAR_BASE_UUID    {{0x24, 0x79, 0x1F, 0x49, 0x4F, 0x3A, 0x42, 0xF5, 0x99, 0x22, 0x42, 0x37, 0x84, 0x89, 0xE8, 0x87}}
/** Protocol characteristic UUID */
#define PROTO_CHAR_UUID         (0x8984)

// DECLARATIONS ***************************************************************/

/** Strongly typed protocol characteristic write callback */
typedef void (*proto_ble_char_write_callback_t)(ble_gatts_value_t *data);

/**
 * Encapsulation of protocol service init
 */
typedef struct
{
    /** Pointer to the initialized queued write context */
    nrf_ble_qwr_t *p_qwr_ctx;
} ProtoServiceInit;

/**
 * Encapsulation of protocol service
 */
typedef struct
{
    /** Whether indications are enabled */
    bool indications_enabled;
    /** UUID type */
    uint8_t uuid_type;
    /** Service handle assigned by the SoftDevice */
    uint16_t service_handle;
    /** Connection handle assigned by the Softdevice */
    uint16_t conn_handle;
    /** Characteristic handles */
    ble_gatts_char_handles_t char_handles;
} ProtoService;

/** Exposed variables */
extern ProtoService g_proto_ble_service;

/**
 * Initialise the protocol service
 * @return ret_code_t NRF_SUCCESS if successful
 */
ret_code_t proto_ble_service_init(ProtoServiceInit *p_init);
/**
 * Register protocol characteristic write callback
 * @param callback: Pointer to callback function
 * @return NP_SUCCESS if successful 
 */
ProtResult proto_ble_register_char_write_callback(proto_ble_char_write_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // PROT_BLE_PROFILE_H