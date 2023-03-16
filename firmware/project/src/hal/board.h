/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      board.h
 @brief     Provides generic board functions and definitions
 ******************************************************************************/

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "build_config.h"
#include "protocol/proto.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINITIONS ****************************************************************/

/** GPIO definitions */
#define BOARD_PIN_ON                    (1)
#define BOARD_PIN_OFF                   (0)
#define BOARD_PIN_HIGH                  (1)
#define BOARD_PIN_LOW                   (0)
#define BOARD_PIN_SET                   (1)
#define BOARD_PIN_CLEAR                 (0)
#define BOARD_ENABLE                    (1)
#define BOARD_DISABLE                   (0)

#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
/** LEDs */
#define BOARD_LED1_PIN                  (17)
#define BOARD_LED2_PIN                  (18)
#define BOARD_LED3_PIN                  (19)
#define BOARD_LED4_PIN                  (20)
/** Buttons */
#define BOARD_BTN1_PIN                  (13)
#define BOARD_BTN2_PIN                  (14)
#define BOARD_BTN3_PIN                  (15)
#define BOARD_BTN4_PIN                  (16)
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
/** LEDs */
#define BOARD_LED1_PIN                  (13)
#define BOARD_LED2_PIN                  (14)
#define BOARD_LED3_PIN                  (15)
#define BOARD_LED4_PIN                  (16)
/** Buttons */
#define BOARD_BTN1_PIN                  (11)
#define BOARD_BTN2_PIN                  (12)
#define BOARD_BTN3_PIN                  (24)
#define BOARD_BTN4_PIN                  (25)
#endif

// DECLARATIONS ***************************************************************/

typedef enum
{
    GPIO_LED1,
    GPIO_LED2,
    GPIO_LED3,
    GPIO_LED4,
    GPIO_BTN1,
    GPIO_BTN2,
    GPIO_BTN3,
    GPIO_BTN4
} Gpio;

/** Strongly-typed board event callback */
typedef void (*board_event_callback_t)(uint8_t board_pin, uint8_t action);

/**
 * Initialise the board pins
 * @retval  NP_SUCCESS: Success
 * @retval  NP_ERR_ERROR:   Failure
 */
ProtResult board_init(void);
/**
 * Get a the pin assigned for a GPIO
 * @param   gpio: To query
 * @return  The pin number, or 0xFF on error
 */
uint8_t board_get_pin(Gpio gpio);
/**
 * Get a GPIO value
 * @param   gpio: To get
 * @retval  0x00: Logic 0
 * @retval  0x01: Logic 1
 * @retval  0xFF: Error
 */
uint8_t board_get_gpio(Gpio gpio);
/**
 * Set a GPIO
 * @param   gpio: To set
 * @param   value: 0 to set logic 0, otherwise set logic 1
 */
void board_set_gpio(Gpio gpio, uint8_t value);
/**
 * Register callback handler for board event
 * @param   cb: Callback function pointer
 * @retval  NP_SUCCESS:     Success
 * @retval  NP_ERR_ERROR:   Failure 
 */
ProtResult board_register_event_callback(board_event_callback_t cb);

#ifdef __cplusplus
}
#endif

#endif  // BOARD_H