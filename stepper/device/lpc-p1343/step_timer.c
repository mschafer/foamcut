#include "lpc134x.h"
#include "core/gpio/gpio.h"

void step_timer_init()
{
    /* enable the clock for CT32B0 */
    SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT32B0);

    /* reset the timer */
    TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERRESET_ENABLED;

    /* leave the timer disabled */
    TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERENABLE_DISABLED;

    /* interrupt on match 0, reset timer on match, stop timer on match */
    TMR_TMR32B0MCR = TMR_TMR32B0MCR_MR0_INT_ENABLED |TMR_TMR32B0MCR_MR0_RESET_ENABLED | TMR_TMR32B0MCR_MR0_STOP_ENABLED;

    /* set prescale to divide by 72, 1usec */
    TMR_TMR32B0PR = 72 - 1;

    /* clear any pending interrupts and then enable the interrupt. */
    TMR_TMR32B0IR = TMR_TMR32B0IR_MR0;
    NVIC_EnableIRQ(TIMER_32_0_IRQn);

    /** \todo debug */
    gpioSetPinFunction(2, 7, GPIO_OUTPUT_PIN);
}

void step_timer_start(uint32_t val)
{
    TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERRESET_ENABLED;
    TMR_TMR32B0MR0 = val;
    TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERENABLE_ENABLED;
}

void step_timer_stop()
{
    TMR_TMR32B0TCR = TMR_TMR32B0TCR_COUNTERENABLE_DISABLED;
}

uint8_t step_timer_enabled()
{
	return (TMR_TMR32B0TCR & TMR_TMR32B0TCR_COUNTERENABLE_MASK) == 0 ? 0 : 1;
}
