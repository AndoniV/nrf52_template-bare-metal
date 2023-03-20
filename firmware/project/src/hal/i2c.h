/***************************************************************************//**
 @author    A Villarreal
 @date      16/03/23
 @file      i2c.h
 @brief     Provides an interface for i2c bus transactions
 ******************************************************************************/

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "nrf_twi_mngr.h"
#include "build_config.h"
#include "protocol/proto.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** Flag for stop transfer condition */
#define I2C_STOP                                    0x00
/** Flag for no stop transfer condition */
#define I2C_NO_STOP                                 NRF_TWI_MNGR_NO_STOP
/** I2C alias macro for constructing bus write transfer structures */
#define I2C_WRITE(address, p_data, length, flags)   NRF_TWI_MNGR_WRITE(address, p_data, length, flags)
/** I2C alias macro for constructing bus read transfer structures */
#define I2C_READ(address, p_data, length, flags)    NRF_TWI_MNGR_READ(address, p_data, length, flags)

// DECLARATIONS ***************************************************************/

// TODO: Change names for this aliases to camelcase to easily identify its a data type
/** Alias for bus transfers */
typedef nrf_twi_mngr_transfer_t i2c_transfer;
/** Alias for bus transactions */
typedef nrf_twi_mngr_transaction_t i2c_transaction;

/**
 * Initialise the i2c bus
 * @return ProtResult NP_SUCCESS if successful
 */
ProtResult i2c_init(void);
/**
 * Wrapper function for i2c bus transaction scheduling
 * @param p_transaction Pointer to transaction structure
 * @return ProtResult: NP_SUCCESS if successful
 */
ProtResult i2c_schedule(i2c_transaction const *p_transaction);
/**
 * Wrapper function for i2c bus transaction performing
 * @param p_transfers
 * @return ProtResult 
 */
ProtResult i2c_perform(i2c_transfer const *p_transfers, uint8_t len);
/**
 * Sets the parameters for a bus transfer
 * @param p_transfer To set
 * @param read       Whether its a read transfer
 * @param address    Device bus address
 * @param p_data     Pointer to data buffer
 * @param len        Number of bytes for transfer
 * @param flags      Transfer flags
 */
void i2c_set_transfer(i2c_transfer *p_transfer, bool read, uint8_t address, uint8_t *p_data, uint8_t len, uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif  // I2C_H