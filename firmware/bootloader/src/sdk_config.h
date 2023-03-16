/***************************************************************************//**
 @author    A Villarreal
 @date      12/02/23
 @file      sdk_config.h
 @brief     Provides a build wrapper around underlying sdk_config.h files
 ******************************************************************************/

#ifndef SDK_CONFIG_WRAPPER_H
#define SDK_CONFIG_WRAPPER_H

#include "build_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
#include "sdk_config_nrf52832.h"
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
#include "sdk_config_nrf52840.h"
#else
#error "Invalid configuration target"
#endif // CONFIG_TARGET_TYPE

// DECLARATIONS ***************************************************************/

#ifdef __cplusplus
}
#endif

#endif // SDK_CONFIG_WRAPPER_H
