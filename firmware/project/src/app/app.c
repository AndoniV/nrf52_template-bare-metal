/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      app.c
 @brief     Provides the main application
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "./version.h"
#include "hal/board.h"
#include "ble/ble_if.h"
#include "app/app.h"
#include "app/device.h"

#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#endif

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/**
 * Service the application
 */
static void app_service();

// IMPLEMENTATION *************************************************************/

/**
 * Run the main application
 */
void app_run()
{
    // Initialise log and print firmware header
    ret_code_t nrf_result = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(nrf_result);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("********************************************************************************");
    NRF_LOG_INFO("Firmware v%s", FW_VERSION_STR);
    NRF_LOG_INFO("Built %s %s", __DATE__, __TIME__);
    NRF_LOG_INFO("********************************************************************************");

    // Initialise NRF modules
    nrf_result = app_timer_init();
    APP_ERROR_CHECK(nrf_result);
    nrf_result = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(nrf_result);

    // Initialise common HAL modules
    ProtResult result = board_init();
    APP_ERROR_CHECK((ret_code_t)result);
    result = device_init();
    APP_ERROR_CHECK((ret_code_t)result);

    // Initialise BLE interface
    result = ble_if_init();
    APP_ERROR_CHECK((ret_code_t)result);
    // Start BLE advertising
    result = ble_if_advertising_enable(true);
    APP_ERROR_CHECK((ret_code_t)result);

    NRF_LOG_INFO("[APP] Initialised");

    // Application loop =======================================================/
    while (1)
    {
        app_service();
    }
}

/**
 * Service the application
 */
static void app_service()
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
    device_service();
}
