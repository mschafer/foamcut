#include "gpio.h"
#include "lpc134x.h"

static volatile uint32_t *const gpioDIR[]  = {&GPIO_GPIO0DIR,  &GPIO_GPIO1DIR,  &GPIO_GPIO2DIR,  &GPIO_GPIO3DIR };
static volatile uint32_t *const gpioDATA[] = {&GPIO_GPIO0DATA, &GPIO_GPIO1DATA, &GPIO_GPIO2DATA, &GPIO_GPIO3DATA };

#define RESERVED_PIN (*(pREG32 (0x00000000)))

static volatile uint32_t *const gpioIOCON0[] = {
        &RESERVED_PIN, &IOCON_PIO0_1, &IOCON_PIO0_2, &IOCON_PIO0_3, &RESERVED_PIN, &RESERVED_PIN,
        &IOCON_PIO0_6, &IOCON_PIO0_7, &IOCON_PIO0_8, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN
};

static volatile uint32_t *const gpioIOCON1[] = {
        &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &IOCON_PIO1_4, &IOCON_PIO1_5,
        &IOCON_PIO1_6, &IOCON_PIO1_7, &IOCON_PIO1_8, &IOCON_PIO1_9, &IOCON_PIO1_10
};

static volatile uint32_t *const gpioIOCON2[] = {
        &IOCON_PIO2_0, &IOCON_PIO2_1, &IOCON_PIO2_2, &IOCON_PIO2_3, &IOCON_PIO2_4, &IOCON_PIO2_5,
        &IOCON_PIO2_6, &IOCON_PIO2_7, &IOCON_PIO2_8, &IOCON_PIO2_9, &IOCON_PIO2_10
};

static volatile uint32_t *const gpioIOCON3[] = {
        &IOCON_PIO3_0, &IOCON_PIO3_1, &IOCON_PIO3_2, &IOCON_PIO3_3, &IOCON_PIO3_4, &IOCON_PIO3_5,
        &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN, &RESERVED_PIN
};

static volatile uint32_t *const *const gpioIOCON[] = { gpioIOCON0, gpioIOCON1, gpioIOCON2, gpioIOCON3 };

void gpioInit(void) {
    /* Enable AHB clock to the GPIO domain. */
    SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_GPIO);
}

uint8_t gpioSetPinFunction(uint_fast8_t port, uint_fast8_t pin, gpioPinFunction_t func)
{
    volatile uint32_t *iocon = gpioIOCON[port][pin];
    uint32_t mask = 0x07;
    if (iocon == &RESERVED_PIN) {
        assert(0);
        return 1;
    }

    // set the function to 0 which selects i/o for all the allowed pins
    *iocon = (*iocon) & ~mask;

    // turn off pullup/pulldown, bits 3-4 = 0 means floating
    mask = 0x18;
   *iocon = (*iocon) & ~mask;

    if (func == GPIO_OUTPUT_PIN) {
        *gpioDIR[port] |= (1 << pin);
    } else {
        *gpioDIR[port] &= ~(1 << pin);

        // float, pullup, or pulldown
        switch (func) {
        case GPIO_INPUT_FLOAT_PIN:
            break;

        case GPIO_INPUT_PULLUP_PIN:
            *iocon |= 0x10;
            break;

        case GPIO_INPUT_PULLDOWN_PIN:
            *iocon |= 0x08;
            break;
        }
    }
    return 0;
}

void gpioSetPin(uint_fast8_t port, uint_fast8_t pin, uint_fast8_t val)
{
    uint32_t mask = 1;
    mask <<= pin;
    if (val == 0) {
        *gpioDATA[port] &= (~mask);
    } else {
        *gpioDATA[port] |= mask;
    }
}

uint_fast8_t gpioGetPin(uint_fast8_t port, uint_fast8_t pin)
{
    uint32_t val = *gpioDATA[port];
    uint32_t mask = 1;
    uint_fast8_t ret;
    mask <<= pin;
    ret = (val & mask) ? 1 : 0;
    
    return ret;
}

