/***************************************************************************//**
 @copyright © DeezNutz Ltd, 2021. All rights reserved
 @author    A Villarreal
 @date      01/04/21
 @file      main.c
 @brief     Main entry for the application firmware
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app/app.h"
#include "build_config.h"
#include "uicr_config.h"

#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
#include "nrf52.h"
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
#include "nrf52840.h"
#endif

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

// IMPLEMENTATION *************************************************************/

/**
 * Firmware entry point
 * @return      Exit code
 */
int main(void)
{
#if (DEBUG_MONITOR_MODE == 1)
    // Raise the priority of the debug monitor interrupt
    NVIC_SetPriority(DebugMonitor_IRQn, 6UL);
#endif

    // Raise the priority of GPIOTE interrupt
    NVIC_SetPriority(GPIOTE_IRQn, 2UL);

    app_run();
    return 0;
}