/***************************************************************************//**
 @author    A Villarreal
 @date      01/04/23
 @file      version.h
 @brief     Provides build configuration

 NOTE: Do not move this file, it is modified by CI during the build
 process. Furthermore, do not change the formatting of the definitions as this
 will affect the regexes used in the deployment scripts
*******************************************************************************/

#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** Hardware version */
#define HW_VERSION_MAJ  (1)
#define HW_VERSION_MIN  (0)
#define HW_VERSION_STR  "1.0"

/** Firmware version */
#define FW_VERSION_MAJ  (1)
#define FW_VERSION_MIN  (0)
#define FW_VERSION_STR  "1.0"

/** Build number */
#define FW_BUILD_NUM    (0)

// DECLARATIONS ***************************************************************/

#ifdef __cplusplus
}
#endif

#endif // VERSION_H
