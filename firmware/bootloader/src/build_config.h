/***************************************************************************//**
 @author    A Villarreal
 @date      12/02/23
 @file      build_config.h
 @brief     Provides build configuration
 ******************************************************************************/

#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/**
 * CONFIG: Target type, chosen automatically based on device type:
 * 0 = NRF52832
 * 1 = NRF52840
 */
#define CONFIG_TARGET_TYPE_NRF52832 (0)
#define CONFIG_TARGET_TYPE_NRF52840 (1)
#if (CONFIG_TARGET_TYPE_NRF52832)
#define CONFIG_TARGET_TYPE          (CONFIG_TARGET_TYPE_NRF52832)
#else
#define CONFIG_TARGET_TYPE          (CONFIG_TARGET_TYPE_NRF52840)
#endif

// DECLARATIONS ***************************************************************/

#ifdef __cplusplus
}
#endif

#endif // BUILD_CONFIG_H