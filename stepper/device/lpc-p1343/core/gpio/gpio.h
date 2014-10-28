#ifndef _gpio_h_
#define _gpio_h_

#include <stdint.h>

typedef enum {
    GPIO_OUTPUT_PIN,
    GPIO_INPUT_FLOAT_PIN,
    GPIO_INPUT_PULLUP_PIN,
    GPIO_INPUT_PULLDOWN_PIN,
} gpioPinFunction_t;

#ifdef __cplusplus
extern "C" {
#endif

void gpioInit(void);

/**
 * Configure an i/o pin as input or output,
 * \param val The initial value for an ouput pin. 0 is low.
 * \return 0 on success, 1 if the pin reserved
 */
uint8_t gpioSetPinFunction(uint_fast8_t port, uint_fast8_t pin, gpioPinFunction_t func);

void gpioSetPin(uint_fast8_t port, uint_fast8_t pin, uint_fast8_t val);

uint_fast8_t gpioGetPin(uint_fast8_t port, uint_fast8_t pin);

#ifdef __cplusplus
}
#endif

#endif
