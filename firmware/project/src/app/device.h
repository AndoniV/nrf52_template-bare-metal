/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      device.h
 @brief     Provides a common device interface
 ******************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

#include <stddef.h>
#include <stdint.h>
#include "protocol/proto.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/**
 * Initialise the device
 * @retval  NP_SUCCESS:     Success
 * @retval  NP_ERR_ERROR:   Failed to initialise
 */
ProtResult device_init(void);
/**
 * Service the device's state machine
 */
void device_service(void);
/**
 * Get the device serial number
 * @return  Pointer to the device serial number string
 */
uint8_t *device_get_serial_number(void);

#ifdef __cplusplus
}
#endif

#endif  // DEVICE_H