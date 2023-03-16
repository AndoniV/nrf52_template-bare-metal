/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      ble_if.h
 @brief     Provides a BLE interface
 ******************************************************************************/

#ifndef BLE_IF_H
#define BLE_IF_H

#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "sdk_errors.h"
#include "protocol/proto.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** Length of service data, in bytes */
#define BLE_IF_SERVICE_DATA_1_LEN   (6U)

// DECLARATIONS ***************************************************************/

/**
 * Initialise the module
 * @retval  NP_SUCCESS: Success
 * @retval  NP_ERR_ERROR:   Failed to initialise
 */
ProtResult ble_if_init(void);
/**
 * Enable/disable advertising
 * @param   enable: True to enable, false to disable
 * @retval  NRF_SUCCESS or error code
 */
ret_code_t ble_if_advertising_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif // BLE_IF_H
