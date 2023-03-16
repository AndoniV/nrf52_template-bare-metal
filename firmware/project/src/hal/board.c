/***************************************************************************//**
 @author    A Villarreal
 @date      01/03/23
 @file      board.c
 @brief     Provides generic board functions and definitions
 ******************************************************************************/

#include "sdk_config.h"
#include "app_timer.h"
#include "build_config.h"
#include "nrf_error.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "hal/board.h"

// DEFINITIONS ****************************************************************/

/** Number of event callbacks able to register */
#define BOARD_EVENT_CALLBACK_LEN    (GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS)

// DECLARATIONS ***************************************************************/

/** Whether the board is initialised */
bool s_board_initialised = false;
/** Board event callbacks */
static board_event_callback_t s_board_event_callbacks[BOARD_EVENT_CALLBACK_LEN] = {0};

/**
 * Callback function for board pin events
 * @param pin: Pin that triggered this event
 * @param action: Action that lead to triggering this event.
 */
static void board_event_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

// IMPLEMENTATION *************************************************************/

/**
 * Initialise the board pins
 */
ProtResult board_init(void)
{
    ProtResult result = NP_ERR_ERROR;
    ret_code_t nrf_result = NRF_SUCCESS;

    if (!s_board_initialised)
    {
#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
        // Configure outputs
        nrf_gpio_cfg_output(BOARD_LED1_PIN);
        nrf_gpio_cfg_output(BOARD_LED2_PIN);
        nrf_gpio_cfg_output(BOARD_LED3_PIN);
        nrf_gpio_cfg_output(BOARD_LED4_PIN);
        // Set initial output states
        nrf_gpio_pin_set(BOARD_LED1_PIN);
        nrf_gpio_pin_set(BOARD_LED2_PIN);
        nrf_gpio_pin_set(BOARD_LED3_PIN);
        nrf_gpio_pin_set(BOARD_LED4_PIN);
        // Configure inputs
        nrf_gpio_cfg_default(BOARD_BTN1_PIN);
        nrf_gpio_cfg_default(BOARD_BTN2_PIN);
        nrf_gpio_cfg_default(BOARD_BTN3_PIN);
        nrf_gpio_cfg_default(BOARD_BTN4_PIN);

        // Configure GPIOTE inputs
        if (nrf_drv_gpiote_is_init() == false)
        {
            nrf_result = nrf_drv_gpiote_init();
            if (nrf_result != NRF_SUCCESS)
            {
                return NP_ERR_ERROR;
            }
        }

        nrf_drv_gpiote_in_config_t in_config_button1 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button2 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button3 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button4 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);

        in_config_button1.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button2.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button3.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button4.pull = NRF_GPIO_PIN_PULLUP;

        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN1_PIN, &in_config_button1, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN2_PIN, &in_config_button2, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN3_PIN, &in_config_button3, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN4_PIN, &in_config_button4, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }

        nrf_drv_gpiote_in_event_enable(BOARD_BTN1_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN2_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN3_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN4_PIN, true);
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
        // Configure outputs
        nrf_gpio_cfg_output(BOARD_LED1_PIN);
        nrf_gpio_cfg_output(BOARD_LED2_PIN);
        nrf_gpio_cfg_output(BOARD_LED3_PIN);
        nrf_gpio_cfg_output(BOARD_LED4_PIN);
        // Set initial output states
        nrf_gpio_pin_set(BOARD_LED1_PIN);
        nrf_gpio_pin_set(BOARD_LED2_PIN);
        nrf_gpio_pin_set(BOARD_LED3_PIN);
        nrf_gpio_pin_set(BOARD_LED4_PIN);
        // Configure inputs
        nrf_gpio_cfg_default(BOARD_BTN1_PIN);
        nrf_gpio_cfg_default(BOARD_BTN2_PIN);
        nrf_gpio_cfg_default(BOARD_BTN3_PIN);
        nrf_gpio_cfg_default(BOARD_BTN4_PIN);

        // Configure GPIOTE inputs
        if (nrf_drv_gpiote_is_init() == false)
        {
            nrf_result = nrf_drv_gpiote_init();
            if (nrf_result != NRF_SUCCESS)
            {
                return NP_ERR_ERROR;
            }
        }

        nrf_drv_gpiote_in_config_t in_config_button1 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button2 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button3 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        nrf_drv_gpiote_in_config_t in_config_button4 = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);

        in_config_button1.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button2.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button3.pull = NRF_GPIO_PIN_PULLUP;
        in_config_button4.pull = NRF_GPIO_PIN_PULLUP;

        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN1_PIN, &in_config_button1, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN2_PIN, &in_config_button2, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN3_PIN, &in_config_button3, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }
        nrf_result = nrf_drv_gpiote_in_init(BOARD_BTN4_PIN, &in_config_button4, board_event_callback);
        if (NRF_SUCCESS != nrf_result)
        {
            return NP_ERR_ERROR;
        }

        nrf_drv_gpiote_in_event_enable(BOARD_BTN1_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN2_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN3_PIN, true);
        nrf_drv_gpiote_in_event_enable(BOARD_BTN4_PIN, true);
#endif
        s_board_initialised = true;
        result = NP_SUCCESS;
        NRF_LOG_INFO("[BOARD] Initialised");
    }

    return result;
}

/**
 * Get a the pin assigned for a GPIO
 */
uint8_t board_get_pin(Gpio gpio)
{
    if (!s_board_initialised)
    {
        return 0xFF;
    }

    switch (gpio)
    {
#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
    case GPIO_LED1:   return BOARD_LED1_PIN; break;
    case GPIO_LED2:   return BOARD_LED2_PIN; break;
    case GPIO_LED3:   return BOARD_LED3_PIN; break;
    case GPIO_LED4:   return BOARD_LED4_PIN; break;
    case GPIO_BTN1:   return BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   return BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   return BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   return BOARD_BTN4_PIN; break;
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
    case GPIO_LED1:   return BOARD_LED1_PIN; break;
    case GPIO_LED2:   return BOARD_LED2_PIN; break;
    case GPIO_LED3:   return BOARD_LED3_PIN; break;
    case GPIO_LED4:   return BOARD_LED4_PIN; break;
    case GPIO_BTN1:   return BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   return BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   return BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   return BOARD_BTN4_PIN; break;
#endif
    default: return 0xFF;
    } // switch: gpio
    return 0;
}

/**
 * Get a GPIO value
 */
uint8_t board_get_gpio(Gpio gpio)
{
    if (!s_board_initialised)
    {
        return 0xFF;
    }

    uint8_t pin = 0xFF;
    switch (gpio)
    {
#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
    case GPIO_LED1:   pin = BOARD_LED1_PIN; break;
    case GPIO_LED2:   pin = BOARD_LED2_PIN; break;
    case GPIO_LED3:   pin = BOARD_LED3_PIN; break;
    case GPIO_LED4:   pin = BOARD_LED4_PIN; break;
    case GPIO_BTN1:   pin = BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   pin = BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   pin = BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   pin = BOARD_BTN4_PIN; break;
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
    case GPIO_LED1:   pin = BOARD_LED1_PIN; break;
    case GPIO_LED2:   pin = BOARD_LED2_PIN; break;
    case GPIO_LED3:   pin = BOARD_LED3_PIN; break;
    case GPIO_LED4:   pin = BOARD_LED4_PIN; break;
    case GPIO_BTN1:   pin = BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   pin = BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   pin = BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   pin = BOARD_BTN4_PIN; break;
#endif
    default: break;
    } // switch: gpio

    if (pin != 0xFF)
    {
        return (nrf_gpio_pin_read(pin) == 0) ? 0 : 1;
    }
    return 0;    
}

/**
 * Set a GPIO
 */
void board_set_gpio(Gpio gpio, uint8_t value)
{
    if (!s_board_initialised)
    {
        return;
    }

    uint8_t pin = 0xFF;
    switch (gpio)
    {
#if (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52832)
    case GPIO_LED1:   pin = BOARD_LED1_PIN; break;
    case GPIO_LED2:   pin = BOARD_LED2_PIN; break;
    case GPIO_LED3:   pin = BOARD_LED3_PIN; break;
    case GPIO_LED4:   pin = BOARD_LED4_PIN; break;
    case GPIO_BTN1:   pin = BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   pin = BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   pin = BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   pin = BOARD_BTN4_PIN; break;
#elif (CONFIG_TARGET_TYPE == CONFIG_TARGET_TYPE_NRF52840)
    case GPIO_LED1:   pin = BOARD_LED1_PIN; break;
    case GPIO_LED2:   pin = BOARD_LED2_PIN; break;
    case GPIO_LED3:   pin = BOARD_LED3_PIN; break;
    case GPIO_LED4:   pin = BOARD_LED4_PIN; break;
    case GPIO_BTN1:   pin = BOARD_BTN1_PIN; break;
    case GPIO_BTN2:   pin = BOARD_BTN2_PIN; break;
    case GPIO_BTN3:   pin = BOARD_BTN3_PIN; break;
    case GPIO_BTN4:   pin = BOARD_BTN4_PIN; break;
#endif
    default: break;
    } // switch: gpio

    if (pin != 0xFF)
    {
        (value == 0) ? nrf_gpio_pin_clear(pin) : nrf_gpio_pin_set(pin);
    }
}

/**
 * Register callback handler for board event
 */
ProtResult board_register_event_callback(board_event_callback_t cb)
{
    if (cb == NULL)
    {
        return NP_ERR_ERROR;
    } // if: args ok

    uint8_t i = 0;
    for (i = 0; i < BOARD_EVENT_CALLBACK_LEN; i++)
    {
        if (s_board_event_callbacks[i] == NULL)
        {
            s_board_event_callbacks[i] = cb;
            break; 
        }
    }
    if (i > (BOARD_EVENT_CALLBACK_LEN) - 1)
    {
        return NP_ERR_ERROR;
    }

    return NP_SUCCESS;
}

/**
 * Callback function for board pin events
 */
static void board_event_callback(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_DEBUG("[BOARD] Pin: %d", pin);
    for (uint8_t i = 0; i < BOARD_EVENT_CALLBACK_LEN; i++)
    {
        if (s_board_event_callbacks[i] != NULL)
        {
            s_board_event_callbacks[i](pin, nrf_gpio_pin_read(pin));
        }
    }
}
