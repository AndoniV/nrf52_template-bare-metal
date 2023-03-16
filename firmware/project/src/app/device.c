/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      device.c
 @brief     Provides a common device interface
 ******************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include "nrf.h"
#include "nrf_log.h"
#include "version.h"
#include "build_config.h"
#include "app/device.h"

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/** Whether the device is initialised */
bool s_device_initialised = false;

// IMPLEMENTATION *************************************************************/

/**
 * Initialise the device
 */
ProtResult device_init(void)
{
    ProtResult result = NP_SUCCESS;

    if (!s_device_initialised)
    {
        NRF_LOG_INFO("[DEVICE] Initialised");        
        s_device_initialised = true;
    }
    return result;
}

/**
 * Service the device's state machine
 */
void device_service(void)
{
}

/**
 * Get the device serial number
 */
uint8_t *device_get_serial_number(void)
{
    // TODO: Get SN from UICRs
    // TODO: Define SN length
    return (uint8_t *)"DEEZNUTZ";
}