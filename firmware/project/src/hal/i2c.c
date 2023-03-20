/***************************************************************************//**
 @author    A Villarreal
 @date      16/03/23
 @file      i2c.c
 @brief     Provides an interface for i2c bus transactions
 ******************************************************************************/

#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_twi_mngr.h"
#include "hal/board.h"
#include "hal/i2c.h"

// DEFINITIONS ****************************************************************/

/** Number of bus queued transactions */
#define I2C_PENDING_TRANSACTIONS    (5)

// DECLARATIONS ***************************************************************/

/** Whether the bus is initialised */
static bool s_i2c_initialised = false;
/** Bus manager definition */
NRF_TWI_MNGR_DEF(g_twi_mngr, I2C_PENDING_TRANSACTIONS, BOARD_I2C_INSTANCE);

// IMPLEMENTATION *************************************************************/

/**
 * Initialise the i2c bus
 * @return ProtResult NP_SUCCESS if successful
 */
ProtResult i2c_init(void)
{
    ProtResult result = NP_SUCCESS;

    if (!s_i2c_initialised)
    {
        nrf_drv_twi_config_t twi_config =
        {
            .scl                = BOARD_I2C_SCL,
            .sda                = BOARD_I2C_SDA,
            .frequency          = NRF_DRV_TWI_FREQ_400K,
            .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
            .clear_bus_init     = false,
        };
        ret_code_t nrf_result = nrf_twi_mngr_init(&g_twi_mngr, &twi_config);
        if (nrf_result != NRF_SUCCESS)
        {
            return NP_ERR_ERROR;
        }
        NRF_LOG_DEBUG("[I2C] Initialised");
    }
    return result;
}

/**
 * Wrapper function for i2c bus transaction scheduling
 */
ProtResult i2c_schedule(i2c_transaction const *p_transaction)
{
    ret_code_t nrf_result = nrf_twi_mngr_schedule(&g_twi_mngr, p_transaction);
    return nrf_result == NRF_SUCCESS ? NP_SUCCESS : NP_ERR_ERROR;
}

/**
 * Wrapper function for i2c bus transaction performing
 */
ProtResult i2c_perform(i2c_transfer const *p_transfers, uint8_t len)
{
    ret_code_t nrf_result = nrf_twi_mngr_perform(&g_twi_mngr,                                           //
                                                g_twi_mngr.p_nrf_twi_mngr_cb->p_current_configuration,  //
                                                p_transfers,                                            //
                                                len,                                                    //
                                                NULL);
    return nrf_result == NRF_SUCCESS ? NP_SUCCESS : NP_ERR_ERROR;
}

/**
 * Sets the parameters for a bus transfer
 */
void i2c_set_transfer(i2c_transfer *p_transfer, bool read, uint8_t address, uint8_t *p_data, uint8_t len, uint8_t flags)
{
    memset(p_transfer, 0x00, sizeof(i2c_transfer));
    p_transfer->p_data = p_data;
    p_transfer->length = len;
    p_transfer->operation = read ? NRF_TWI_MNGR_READ_OP(address) : NRF_TWI_MNGR_WRITE_OP(address);
    p_transfer->flags = flags;
}