/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      np_proto.h
 @brief     Provides includes for protocol
 ******************************************************************************/

#ifndef PROTO_H
#define PROTO_H

#include "build_config.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/**
 * Enumeration of protocol result codes
 */
typedef enum
{
    NP_SUCCESS   = 0x00,
    NP_ERR_PARAM = 0x01,
    NP_ERR_ERROR = 0xFF,
} ProtResult;

#ifdef __cplusplus
}
#endif

#endif // PROTO_H