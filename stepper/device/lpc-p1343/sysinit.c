/**************************************************************************/
/*! 
 @file     sysinit.c
 @author   K. Townsend (microBuilder.eu)
 @date     22 March 2010
 @version  0.10

 @section LICENSE

 Software License Agreement (BSD License)

 Copyright (c) 2010, microBuilder SARL
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. Neither the name of the copyright holders nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sysinit.h"
#include "core/cpu/cpu.h"
#include "step_timer.h"

#ifdef CFG_USBCDC
volatile unsigned int lastTick;
#include "core/usbcdc/usb.h"
#include "core/usbcdc/usbcore.h"
#include "core/usbcdc/usbhw.h"
#include "core/usbcdc/cdcuser.h"
#endif

void pmuInit(void) {
    /* Enable all clocks, even those turned off at power up. */
    SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_WDTOSC_MASK | SCB_PDRUNCFG_SYSOSC_MASK
            | SCB_PDRUNCFG_ADC_MASK);

    return;
}

/**************************************************************************/
/*! 
 Configures the core system clock and sets up any mandatory
 peripherals like the systick timer, UART for printf, etc.

 This function should set the HW to the default state you wish to be
 in coming out of reset/startup, such as disabling or enabling LEDs,
 setting specific pin states, etc.
 */
/**************************************************************************/
void systemInit() {
    uint32_t usbTimeout = 0;
    cpuInit(); // Configure the CPU
    systickInit(CFG_SYSTICK_DELAY_IN_MS); // Start systick timer
    gpioInit(); // Enable GPIO
    pmuInit(); // Configure power management
    step_timer_init();

    // Initialise USB CDC
#ifdef CFG_USBCDC
    lastTick = systickGetTicks(); // Used to control output/printf timing
    CDC_Init(); // Initialise VCOM
    USB_Init(); // USB Initialization
    USB_Connect(TRUE); // USB Connect
    // Wait until USB is configured or timeout occurs
    while (usbTimeout < CFG_USBCDC_INITTIMEOUT / 10) {
        if (USB_Configuration)
            break;
        systickDelay(10); // Wait 10ms
        usbTimeout++;
    }
#endif

    // Printf can now be used with UART or USBCDC

}
