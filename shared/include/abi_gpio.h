
/*
    ABI definition - <gpio.h>
    
    This file includes a subset of the most commonly used functions from <gpio.h>
    For this reason, this file is subject to copyright as in <gpio.h> from raspberry pi pico SDK.
    
    See `LICENSE` for details.
    
    SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "abi_prereq.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file gpio.h
 *  \defgroup hardware_gpio hardware_gpio
 *
 * General Purpose Input/Output (GPIO) API
 *
 * RP2040 has 36 multi-functional General Purpose Input / Output (GPIO) pins, divided into two banks. In a typical use case,
 * the pins in the QSPI bank (QSPI_SS, QSPI_SCLK and QSPI_SD0 to QSPI_SD3) are used to execute code from an external
 * flash device, leaving the User bank (GPIO0 to GPIO29) for the programmer to use. All GPIOs support digital input and
 * output, but GPIO26 to GPIO29 can also be used as inputs to the chip’s Analogue to Digital Converter (ADC). Each GPIO
 * can be controlled directly by software running on the processors, or by a number of other functional blocks.
 *
 * The function allocated to each GPIO is selected by calling the \ref gpio_set_function function. \note Not all functions
 * are available on all pins.
 *
 * Each GPIO can have one function selected at a time. Likewise, each peripheral input (e.g. UART0 RX) should only be selected on
 * one _GPIO_ at a time. If the same peripheral input is connected to multiple GPIOs, the peripheral sees the logical OR of these
 * GPIO inputs. Please refer to the datasheet for more information on GPIO function select.
 *
 * ### Function Select Table
 *
 *  GPIO   | F1       | F2        | F3       | F4     | F5  | F6   | F7   | F8            | F9
 *  -------|----------|-----------|----------|--------|-----|------|------|---------------|----
 *  0      | SPI0 RX  | UART0 TX  | I2C0 SDA | PWM0 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  1      | SPI0 CSn | UART0 RX  | I2C0 SCL | PWM0 B | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  2      | SPI0 SCK | UART0 CTS | I2C1 SDA | PWM1 A | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  3      | SPI0 TX  | UART0 RTS | I2C1 SCL | PWM1 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  4      | SPI0 RX  | UART1 TX  | I2C0 SDA | PWM2 A | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  5      | SPI0 CSn | UART1 RX  | I2C0 SCL | PWM2 B | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  6      | SPI0 SCK | UART1 CTS | I2C1 SDA | PWM3 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  7      | SPI0 TX  | UART1 RTS | I2C1 SCL | PWM3 B | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  8      | SPI1 RX  | UART1 TX  | I2C0 SDA | PWM4 A | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  9      | SPI1 CSn | UART1 RX  | I2C0 SCL | PWM4 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  10     | SPI1 SCK | UART1 CTS | I2C1 SDA | PWM5 A | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  11     | SPI1 TX  | UART1 RTS | I2C1 SCL | PWM5 B | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  12     | SPI1 RX  | UART0 TX  | I2C0 SDA | PWM6 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  13     | SPI1 CSn | UART0 RX  | I2C0 SCL | PWM6 B | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  14     | SPI1 SCK | UART0 CTS | I2C1 SDA | PWM7 A | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  15     | SPI1 TX  | UART0 RTS | I2C1 SCL | PWM7 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  16     | SPI0 RX  | UART0 TX  | I2C0 SDA | PWM0 A | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  17     | SPI0 CSn | UART0 RX  | I2C0 SCL | PWM0 B | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  18     | SPI0 SCK | UART0 CTS | I2C1 SDA | PWM1 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  19     | SPI0 TX  | UART0 RTS | I2C1 SCL | PWM1 B | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  20     | SPI0 RX  | UART1 TX  | I2C0 SDA | PWM2 A | SIO | PIO0 | PIO1 | CLOCK GPIN0   | USB VBUS EN
 *  21     | SPI0 CSn | UART1 RX  | I2C0 SCL | PWM2 B | SIO | PIO0 | PIO1 | CLOCK GPOUT0  | USB OVCUR DET
 *  22     | SPI0 SCK | UART1 CTS | I2C1 SDA | PWM3 A | SIO | PIO0 | PIO1 | CLOCK GPIN1   | USB VBUS DET
 *  23     | SPI0 TX  | UART1 RTS | I2C1 SCL | PWM3 B | SIO | PIO0 | PIO1 | CLOCK GPOUT1  | USB VBUS EN
 *  24     | SPI1 RX  | UART1 TX  | I2C0 SDA | PWM4 A | SIO | PIO0 | PIO1 | CLOCK GPOUT2  | USB OVCUR DET
 *  25     | SPI1 CSn | UART1 RX  | I2C0 SCL | PWM4 B | SIO | PIO0 | PIO1 | CLOCK GPOUT3  | USB VBUS DET
 *  26     | SPI1 SCK | UART1 CTS | I2C1 SDA | PWM5 A | SIO | PIO0 | PIO1 |               | USB VBUS EN
 *  27     | SPI1 TX  | UART1 RTS | I2C1 SCL | PWM5 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET
 *  28     | SPI1 RX  | UART0 TX  | I2C0 SDA | PWM6 A | SIO | PIO0 | PIO1 |               | USB VBUS DET
 *  29     | SPI1 CSn | UART0 RX  | I2C0 SCL | PWM6 B | SIO | PIO0 | PIO1 |               | USB VBUS EN

 */

/*! \brief  GPIO function definitions for use with function select
 *  \ingroup hardware_gpio
 * \brief GPIO function selectors
 *
 * Each GPIO can have one function selected at a time. Likewise, each peripheral input (e.g. UART0 RX) should only be
 * selected on one GPIO at a time. If the same peripheral input is connected to multiple GPIOs, the peripheral sees the logical
 * OR of these GPIO inputs.
 *
 * Please refer to the datsheet for more information on GPIO function selection.
 */
enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0x1f,
};

#define GPIO_OUT 1
#define GPIO_IN 0

/*! \brief  GPIO Interrupt level definitions (GPIO events)
 *  \ingroup hardware_gpio
 *  \brief GPIO Interrupt levels
 *
 * An interrupt can be generated for every GPIO pin in 4 scenarios:
 *
 * * Level High: the GPIO pin is a logical 1
 * * Level Low: the GPIO pin is a logical 0
 * * Edge High: the GPIO has transitioned from a logical 0 to a logical 1
 * * Edge Low: the GPIO has transitioned from a logical 1 to a logical 0
 *
 * The level interrupts are not latched. This means that if the pin is a logical 1 and the level high interrupt is active, it will
 * become inactive as soon as the pin changes to a logical 0. The edge interrupts are stored in the INTR register and can be
 * cleared by writing to the INTR register.
 */
enum gpio_irq_level {
    GPIO_IRQ_LEVEL_LOW = 0x1u,
    GPIO_IRQ_LEVEL_HIGH = 0x2u,
    GPIO_IRQ_EDGE_FALL = 0x4u,
    GPIO_IRQ_EDGE_RISE = 0x8u,
};

/*! Callback function type for GPIO events
 *  \ingroup hardware_gpio
 *
 * \param gpio Which GPIO caused this interrupt
 * \param event_mask Which events caused this interrupt. See \ref gpio_irq_level for details.
 * \sa gpio_set_irq_enabled_with_callback()
 * \sa gpio_set_irq_callback()
 */
typedef void (*gpio_irq_callback_t)(uint gpio, uint32_t event_mask);

enum gpio_override {
    GPIO_OVERRIDE_NORMAL = 0,      ///< peripheral signal selected via \ref gpio_set_function
    GPIO_OVERRIDE_INVERT = 1,      ///< invert peripheral signal selected via \ref gpio_set_function
    GPIO_OVERRIDE_LOW = 2,         ///< drive low/disable output
    GPIO_OVERRIDE_HIGH = 3,        ///< drive high/enable output
};

/*! \brief Slew rate limiting levels for GPIO outputs
 *  \ingroup hardware_gpio
 *
 * Slew rate limiting increases the minimum rise/fall time when a GPIO output
 * is lightly loaded, which can help to reduce electromagnetic emissions.
 * \sa gpio_set_slew_rate
 */
enum gpio_slew_rate {
    GPIO_SLEW_RATE_SLOW = 0,  ///< Slew rate limiting enabled
    GPIO_SLEW_RATE_FAST = 1   ///< Slew rate limiting disabled
};

/*! \brief Drive strength levels for GPIO outputs
 *  \ingroup hardware_gpio
 *
 * Drive strength levels for GPIO outputs.
 * \sa gpio_set_drive_strength
 */
enum gpio_drive_strength {
    GPIO_DRIVE_STRENGTH_2MA = 0, ///< 2 mA nominal drive strength
    GPIO_DRIVE_STRENGTH_4MA = 1, ///< 4 mA nominal drive strength
    GPIO_DRIVE_STRENGTH_8MA = 2, ///< 8 mA nominal drive strength
    GPIO_DRIVE_STRENGTH_12MA = 3 ///< 12 mA nominal drive strength
};


/*! \brief Select GPIO function
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \param fn Which GPIO function select to use from list \ref gpio_function
 */
void gpio_set_function(uint gpio, enum gpio_function fn);

/*! \brief Determine current GPIO function
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \return Which GPIO function is currently selected from list \ref gpio_function
 */
enum gpio_function gpio_get_function(uint gpio);

/*! \brief Select up and down pulls on specific GPIO
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \param up If true set a pull up on the GPIO
 * \param down If true set a pull down on the GPIO
 *
 * \note On the RP2040, setting both pulls enables a "bus keep" function,
 * i.e. a weak pull to whatever is current high/low state of GPIO.
 */
void gpio_set_pulls(uint gpio, bool up, bool down);

/*! \brief Set specified GPIO to be pulled up.
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 */
static inline void gpio_pull_up(uint gpio) {
    gpio_set_pulls(gpio, true, false);
}

/*! \brief Determine if the specified GPIO is pulled up.
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \return true if the GPIO is pulled up
 */
static inline bool gpio_is_pulled_up(uint gpio) {
    return (padsbank0_hw->io[gpio] & PADS_BANK0_GPIO0_PUE_BITS) != 0;
}

/*! \brief Set specified GPIO to be pulled down.
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 */
static inline void gpio_pull_down(uint gpio) {
    gpio_set_pulls(gpio, false, true);
}

/*! \brief Determine if the specified GPIO is pulled down.
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \return true if the GPIO is pulled down
 */
static inline bool gpio_is_pulled_down(uint gpio) {
    return (padsbank0_hw->io[gpio] & PADS_BANK0_GPIO0_PDE_BITS) != 0;
}

/*! \brief Disable pulls on specified GPIO
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 */
static inline void gpio_disable_pulls(uint gpio) {
    gpio_set_pulls(gpio, false, false);
}

/*! \brief Enable GPIO input
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \param enabled true to enable input on specified GPIO
 */
void gpio_set_input_enabled(uint gpio, bool enabled);

/*! \brief Enable/disable GPIO input hysteresis (Schmitt trigger)
 *  \ingroup hardware_gpio
 *
 * Enable or disable the Schmitt trigger hysteresis on a given GPIO. This is
 * enabled on all GPIOs by default. Disabling input hysteresis can lead to
 * inconsistent readings when the input signal has very long rise or fall
 * times, but slightly reduces the GPIO's input delay.
 *
 * \sa gpio_is_input_hysteresis_enabled
 * \param gpio GPIO number
 * \param enabled true to enable input hysteresis on specified GPIO
 */
void gpio_set_input_hysteresis_enabled(uint gpio, bool enabled);

/*! \brief Determine whether input hysteresis is enabled on a specified GPIO
 *  \ingroup hardware_gpio
 *
 * \sa gpio_set_input_hysteresis_enabled
 * \param gpio GPIO number
 */
bool gpio_is_input_hysteresis_enabled(uint gpio);


/*! \brief Set slew rate for a specified GPIO
 *  \ingroup hardware_gpio
 *
 * \sa gpio_get_slew_rate
 * \param gpio GPIO number
 * \param slew GPIO output slew rate
 */
void gpio_set_slew_rate(uint gpio, enum gpio_slew_rate slew);

/*! \brief Determine current slew rate for a specified GPIO
 *  \ingroup hardware_gpio
 *
 * \sa gpio_set_slew_rate
 * \param gpio GPIO number
 * \return Current slew rate of that GPIO
 */
enum gpio_slew_rate gpio_get_slew_rate(uint gpio);

/*! \brief Set drive strength for a specified GPIO
 *  \ingroup hardware_gpio
 *
 * \sa gpio_get_drive_strength
 * \param gpio GPIO number
 * \param drive GPIO output drive strength
 */
void gpio_set_drive_strength(uint gpio, enum gpio_drive_strength drive);

/*! \brief Determine current slew rate for a specified GPIO
 *  \ingroup hardware_gpio
 *
 * \sa gpio_set_drive_strength
 * \param gpio GPIO number
 * \return Current drive strength of that GPIO
 */
enum gpio_drive_strength gpio_get_drive_strength(uint gpio);

/*! \brief Enable or disable specific interrupt events for specified GPIO
 *  \ingroup hardware_gpio
 *
 * This function sets which GPIO events cause a GPIO interrupt on the calling core. See
 * \ref gpio_set_irq_callback, \ref gpio_set_irq_enabled_with_callback and
 * \ref gpio_add_raw_irq_handler to set up a GPIO interrupt handler to handle the events.
 *
 * \note The IO IRQs are independent per-processor. This configures the interrupt events for
 * the processor that calls the function.
 *
 * \param gpio GPIO number
 * \param event_mask Which events will cause an interrupt
 * \param enabled Enable or disable flag
 *
 * Events is a bitmask of the following \ref gpio_irq_level values:
 *
 * bit | constant            | interrupt
 * ----|----------------------------------------------------------
 *   0 | GPIO_IRQ_LEVEL_LOW  | Continuously while level is low
 *   1 | GPIO_IRQ_LEVEL_HIGH | Continuously while level is high
 *   2 | GPIO_IRQ_EDGE_FALL  | On each transition from high to low
 *   3 | GPIO_IRQ_EDGE_RISE  | On each transition from low to high
 *
 * which are specified in \ref gpio_irq_level
 */
void gpio_set_irq_enabled(uint gpio, uint32_t event_mask, bool enabled);


/*! \brief Set the generic callback used for GPIO IRQ events for the current core
 *  \ingroup hardware_gpio
 *
 * This function sets the callback used for all GPIO IRQs on the current core that are not explicitly
 * hooked via \ref gpio_add_raw_irq_handler or other gpio_add_raw_irq_handler_ functions.
 *
 * This function is called with the GPIO number and event mask for each of the (not explicitly hooked)
 * GPIOs that have events enabled and that are pending (see \ref gpio_get_irq_event_mask).
 *
 * \note The IO IRQs are independent per-processor. This function affects
 * the processor that calls the function.
 *
 * \param callback default user function to call on GPIO irq. Note only one of these can be set per processor.
 */
void gpio_set_irq_callback(gpio_irq_callback_t callback);

/*! \brief Convenience function which performs multiple GPIO IRQ related initializations
 *  \ingroup hardware_gpio
 *
 * This method is a slightly eclectic mix of initialization, that:
 *
 * \li Updates whether the specified events for the specified GPIO causes an interrupt on the calling core based
 * on the enable flag.
 *
 * \li Sets the callback handler for the calling core to callback (or clears the handler if the callback is NULL).
 *
 * \li Enables GPIO IRQs on the current core if enabled is true.
 *
 * This method is commonly used to perform a one time setup, and following that any additional IRQs/events are enabled
 * via \ref gpio_set_irq_enabled. All GPIOs/events added in this way on the same core share the same callback; for multiple
 * independent handlers for different GPIOs you should use \ref gpio_add_raw_irq_handler and related functions.
 *
 * This method is equivalent to:
 *
 * \code{.c}
 * gpio_set_irq_enabled(gpio, event_mask, enabled);
 * gpio_set_irq_callback(callback);
 * if (enabled) irq_set_enabled(IO_IRQ_BANK0, true);
 * \endcode
 *
 * \note The IO IRQs are independent per-processor. This method affects only the processor that calls the function.
 *
 * \param gpio GPIO number
 * \param event_mask Which events will cause an interrupt. See \ref gpio_irq_level for details.
 * \param enabled Enable or disable flag
 * \param callback user function to call on GPIO irq. if NULL, the callback is removed
 */
void gpio_set_irq_enabled_with_callback(uint gpio, uint32_t event_mask, bool enabled, gpio_irq_callback_t callback);

/*! \brief Enable dormant wake up interrupt for specified GPIO and events
 *  \ingroup hardware_gpio
 *
 * This configures IRQs to restart the XOSC or ROSC when they are
 * disabled in dormant mode
 *
 * \param gpio GPIO number
 * \param event_mask Which events will cause an interrupt. See \ref gpio_irq_level for details.
 * \param enabled Enable/disable flag
 */
void gpio_set_dormant_irq_enabled(uint gpio, uint32_t event_mask, bool enabled);

/*! \brief Acknowledge a GPIO interrupt for the specified events on the calling core
 *  \ingroup hardware_gpio
 *
 * \note This may be called with a mask of any of valid bits specified in \ref gpio_irq_level, however
 * it has no effect on \a level sensitive interrupts which remain pending while the GPIO is at the specified
 * level. When handling \a level sensitive interrupts, you should generally disable the interrupt (see
 * \ref gpio_set_irq_enabled) and then set it up again later once the GPIO level has changed (or to catch
 * the opposite level).
 *
 * \param gpio GPIO number
 * \param events Bitmask of events to clear. See \ref gpio_set_irq_enabled for details.
 *
 * \note For callbacks set with \ref gpio_set_irq_enabled_with_callback, or \ref gpio_set_irq_callback,this function is called automatically.
 * \param event_mask Bitmask of events to clear. See \ref gpio_irq_level for details.
 */
void gpio_acknowledge_irq(uint gpio, uint32_t event_mask);


/*! \brief Initialise a GPIO for (enabled I/O and set func to GPIO_FUNC_SIO)
 *  \ingroup hardware_gpio
 *
 * Clear the output enable (i.e. set to input).
 * Clear any output value.
 *
 * \param gpio GPIO number
 */
void gpio_init(uint gpio);

/*! \brief Resets a GPIO back to the NULL function, i.e. disables it.
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 */
void gpio_deinit(uint gpio);

/*! \brief Initialise multiple GPIOs (enabled I/O and set func to GPIO_FUNC_SIO)
 *  \ingroup hardware_gpio
 *
 * Clear the output enable (i.e. set to input).
 * Clear any output value.
 *
 * \param gpio_mask Mask with 1 bit per GPIO number to initialize
 */
void gpio_init_mask(uint gpio_mask);


/*! \brief Get state of a single specified GPIO
 *  \ingroup hardware_gpio
 *
 * \param gpio GPIO number
 * \return Current state of the GPIO. 0 for low, non-zero for high
 */
static inline bool gpio_get(uint gpio) {
    return !!((1ul << gpio) & sio_hw->gpio_in);
}

/*! \brief Get raw value of all GPIOs
 *  \ingroup hardware_gpio
 *
 * \return Bitmask of raw GPIO values, as bits 0-29
 */
static inline uint32_t gpio_get_all(void) {
    return sio_hw->gpio_in;
}


#ifdef __cplusplus
} // extern "C"
#endif
