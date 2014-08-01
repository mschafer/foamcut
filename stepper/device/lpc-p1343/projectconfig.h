/**************************************************************************/
/*! 
 @file     projectconfig.h
 @author   K. Townsend (microBuilder.eu)

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

#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#include "lpc134x.h"
#include "sysdefs.h"

/*=========================================================================
 BOARD SELECTION

 Because several boards use this code library with sometimes slightly
 different pin configuration, you will need to specify which board you
 are using by enabling one of the following definitions. The code base
 will then try to configure itself accordingly for that board.

 CFG_BRD_LPC1343_REFDESIGN   
 =========================

 microBuilder.eu LPC1343 Reference Design base board with
 on-board peripherals initialised (EEPROM, USB or UART CLI, etc.)
 
 This is the recommended starting point for new development
 since it makes it easy to send printf output to USB CDC, access
 the on-board EEPROM, etc.

 CFG_BRD_LPC1343_REFDESIGN_MINIMAL
 =================================

 microBuilder.eu LPC1343 Reference Design base board with 
 only the most common peripherals initialised by default.  
 
 Results in smallest code since EEPROM, USB, etc., are not
 initialised on startup.  By default, only the following
 peripherals are initialised by systemInit():
 
 - CPU (Configures the PLL, etc.)
 - GPIO
 - SysTick Timer
 - UART (with printf support) *
 
 * Can be removed to save 0.8kb in debug and 0.3 kb in
 release. Comment out 'CFG_PRINTF_UART' to disable it.
 
 The code size can be further reduced by several KB by removing
 any IRQ Handlers that are not used.  The I2C IRQHandler, for
 example, uses ~1KB of flash in debug and ~400KB in release mode,
 but because it is referenced in the startup code it is always
 included even if I2C is never used in the project.

 Other IRQ Handlers that you might be able to comment out
 to save some space are:

 IRQ Handler               Debug   Release
 ------------------------- ------  -------
 I2C_IRQHandler            1160 b    400 b
 SSP_IRQHandler             160 b     76 b
 UART_IRQHandler            246 b    116 b
 WAKEUP_IRQHandler          160 b    100 b
 WDT_IRQHandler              50 b     28 b

 CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
 ====================================

 microBuilder.eu/Adafruit Stand-Alone "Smart LCD" with USB enabled
 for the CLI interface.

 CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
 =====================================

 microBuilder.eu/Adafruit Stand-Alone "Smart LCD" with UART enabled
 for the CLI interface.

 CFG_BRD_LPC1343_802154USBSTICK
 ==============================

 microBuilder.eu USB stick 802.15.4 868/915MHz RF transceiver

 -----------------------------------------------------------------------*/
#define CFG_BRD_LPC1343_REFDESIGN
// #define CFG_BRD_LPC1343_REFDESIGN_MINIMAL
// #define CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
// #define CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
// #define CFG_BRD_LPC1343_802154USBSTICK
/*=========================================================================*/

/**************************************************************************
 PIN USAGE
 -----------------------------------------------------------------------
 This table tries to give an indication of which GPIO pins and 
 peripherals are used by the available drivers and SW examples.  Only
 dedicated GPIO pins available on the LPC1343 Reference Board are shown
 below.  Any unused peripheral blocks like I2C, SSP, ADC, etc., can
 also be used as GPIO if they are available.

 PORT 1        PORT 2                PORT 3 
 =========     =================     =======
 8 9 10 11     1 2 3 4 5 6 7 8 9     0 1 2 3

 SDCARD      . .  .  .     . . . . . . . . .     X . . .
 PWM         . X  .  .     . . . . . . . . .     . . . .
 STEPPER     . .  .  .     . . . . . . . . .     X X X X
 CHIBI       X X  X  .     . . . . . . . . .     . . . .
 ILI9325/8   X X  X  X     X X X X X X X X X     . . . X
 ST7565      X X  X  X     X X X X X X X X X     . . . X
 ST7735      . .  .  .     X X X X X X . . .     . . . .
 SHARPMEM    . .  .  .     X X X X . . . . .     . . . .
 SSD1306     . .  .  .     X X X . X X . . .     . . . .
 MCP121      . .  .  .     . . . . . . . . .     . X . .

 TIMERS                    SSP     ADC         UART
 ======================    ===     =======     ====
 16B0  16B1  32B0  32B1    0       0 1 2 3     0

 SDCARD      .     .     .     .       X       . . . .     .
 PWM         .     X     .     .       .       . . . .     .
 PMU [1]     .     .     X     .       .       . . . .     .
 USB         .     .     .     X       .       . . . .     .
 STEPPER     .     .     X     .       .       . . . .     .
 CHIBI       x     .     .     .       X       . . . .     .
 ILI9325/8   .     .     .     .       .       X X X X     .
 ST7565      .     .     .     .       .       X X X X     .
 ST7535      .     .     .     .       .       . . . .     .
 SHARPMEM    .     .     .     .       .       . . . .     .
 SSD1306     .     .     .     .       .       . . . .     .
 INTERFACE   .     .     .     .       .       . . . .     X[2]

 [1]  PMU uses 32-bit Timer 0 for SW wakeup from deep-sleep.  This timer
 can safely be used by other peripherals, but may need to be
 reconfigured when you wakeup from deep-sleep.
 [2]  INTERFACE can be configured to use either USBCDC or UART

 **************************************************************************/

/**************************************************************************
 I2C Addresses
 -----------------------------------------------------------------------
 The following addresses are used by the different I2C sensors included
 in the code base [1]

 HEX       BINARY  
 ====      ========
 ISL12022M (RTC)             0xDE      1101111x
 ISL12022M (SRAM)            0xAE      1010111x
 LM75B                       0x90      1001000x
 MCP24AA                     0xA0      1010000x
 MCP4725                     0xC0      1100000x
 TSL2561                     0x72      0111001x
 TCS3414                     0x72      0111001x

 [1]  Alternative addresses may exists, but the addresses listed in this
 table are the values used in the code base

 **************************************************************************/

/*=========================================================================
 FIRMWARE VERSION SETTINGS
 -----------------------------------------------------------------------*/
#define CFG_FIRMWARE_VERSION_MAJOR            (0)
#define CFG_FIRMWARE_VERSION_MINOR            (9)
#define CFG_FIRMWARE_VERSION_REVISION         (8)
/*=========================================================================*/

/*=========================================================================
 CORE CPU SETTINGS
 -----------------------------------------------------------------------

 CFG_CPU_CCLK    Value is for reference only.  'core/cpu/cpu.c' must
 be modified to change the clock speed, but the value
 should be indicated here since CFG_CPU_CCLK is used by
 other peripherals to determine timing.

 -----------------------------------------------------------------------*/
#define CFG_CPU_CCLK                (72000000)  // 1 tick = 13.88nS
/*=========================================================================*/

/*=========================================================================
 SYSTICK TIMER
 -----------------------------------------------------------------------

 CFG_SYSTICK_DELAY_IN_MS   The number of milliseconds between each tick
 of the systick timer.

 -----------------------------------------------------------------------*/
#define CFG_SYSTICK_DELAY_IN_MS     (1)
/*=========================================================================*/

/*=========================================================================
 ALTERNATE RESET PIN
 -----------------------------------------------------------------------

 CFG_ALTRESET        If defined, indicates that a GPIO pin should be
 configured as an alternate reset pin in addition
 to the dedicated reset pin.
 CFG_ALTRESET_PORT   The GPIO port where the alt reset pin is located
 CFG_ALTRESET_PIN    The GPIO pin where the alt reset pin is located

 -----------------------------------------------------------------------*/
#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
#define CFG_ALTRESET
#define CFG_ALTRESET_PORT         (1)
#define CFG_ALTRESET_PIN          (5)   // P1.5 = RTS
#endif
/*=========================================================================*/

/*=========================================================================
 UART
 -----------------------------------------------------------------------

 CFG_UART_BAUDRATE         The default UART speed.  This value is used 
 when initialising UART, and should be a 
 standard value like 57600, 9600, etc.  
 NOTE: This value may be overridden if
 another value is stored in EEPROM!
 CFG_UART_BUFSIZE          The length in bytes of the UART RX FIFO. This
 will determine the maximum number of received
 characters to store in memory.

 -----------------------------------------------------------------------*/
#ifdef CFG_BRD_LPC1343_REFDESIGN
#define CFG_UART_BAUDRATE           (115200)
#define CFG_UART_BUFSIZE            (512)
#endif

#ifdef CFG_BRD_LPC1343_REFDESIGN_MINIMAL
#define CFG_UART_BAUDRATE           (115200)
#define CFG_UART_BUFSIZE            (512)
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
#define CFG_UART_BAUDRATE           (115200)
#define CFG_UART_BUFSIZE            (512)
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
#define CFG_UART_BAUDRATE           (57600)
#define CFG_UART_BUFSIZE            (512)
#endif

#ifdef CFG_BRD_LPC1343_802154USBSTICK
#define CFG_UART_BAUDRATE           (115200)
#define CFG_UART_BUFSIZE            (512)
#endif
/*=========================================================================*/

/*=========================================================================
 SSP
 -----------------------------------------------------------------------

 CFG_SSP0_SCKPIN_2_11      Indicates which pin should be used for SCK0
 CFG_SSP0_SCKPIN_0_6

 -----------------------------------------------------------------------*/
#ifdef CFG_BRD_LPC1343_REFDESIGN
#define CFG_SSP0_SCKPIN_2_11
// #define CFG_SSP0_SCKPIN_0_6
#endif

#ifdef CFG_BRD_LPC1343_REFDESIGN_MINIMAL
#define CFG_SSP0_SCKPIN_2_11
// #define CFG_SSP0_SCKPIN_0_6
#endif

#if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
#define CFG_SSP0_SCKPIN_2_11
// #define CFG_SSP0_SCKPIN_0_6
#endif

#ifdef CFG_BRD_LPC1343_802154USBSTICK
// #define CFG_SSP0_SCKPIN_2_11
#define CFG_SSP0_SCKPIN_0_6
#endif
/*=========================================================================*/

/*=========================================================================
 ON-BOARD LED
 -----------------------------------------------------------------------

 CFG_LED_PORT              The port for the on board LED
 CFG_LED_PIN               The pin for the on board LED
 CFG_LED_ON                The pin state to turn the LED on (0 = low, 1 = high)
 CFG_LED_OFF               The pin state to turn the LED off (0 = low, 1 = high)

 -----------------------------------------------------------------------*/
#ifdef CFG_BRD_LPC1343_REFDESIGN
#define CFG_LED_PORT                (2)
#define CFG_LED_PIN                 (10)
#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)
#endif

#ifdef CFG_BRD_LPC1343_REFDESIGN_MINIMAL
#define CFG_LED_PORT                (2)
#define CFG_LED_PIN                 (10)
#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)
#endif

#if defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB || defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
#define CFG_LED_PORT                (2)
#define CFG_LED_PIN                 (10)
#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)
#endif

#ifdef CFG_BRD_LPC1343_802154USBSTICK
#define CFG_LED_PORT                (3)
#define CFG_LED_PIN                 (2)
#define CFG_LED_ON                  (0)
#define CFG_LED_OFF                 (1)
#endif
/*=========================================================================*/

/*=========================================================================
 USB
 -----------------------------------------------------------------------

 CFG_USBHID                If this field is defined USB HID support will
 be included.  Currently uses ROM-based USB HID
 CFG_USBCDC                If this field is defined USB CDC support will
 be included, with the USB Serial Port speed
 set to 115200 BPS by default
 CFG_USBCDC_BAUDRATE       The default TX/RX speed.  This value is used 
 when initialising USBCDC, and should be a 
 standard value like 57600, 9600, etc.
 CFG_USBCDC_INITTIMEOUT    The maximum delay in milliseconds to wait for
 USB to connect.  Must be a multiple of 10!
 CFG_USBCDC_BUFFERSIZE     Size of the buffer (in bytes) that stores
 printf data until it can be sent out in
 64 byte frames.  The buffer is required since
 only one frame per ms can be sent using USB
 CDC (see 'puts' in systeminit.c).

 -----------------------------------------------------------------------*/
#define CFG_USB_VID                   (0x239A)
#define CFG_USB_PID                   (0x1002)

#ifdef CFG_BRD_LPC1343_REFDESIGN
// #define CFG_USBHID
#define CFG_USBCDC
#define CFG_USBCDC_BAUDRATE         (115200)
#define CFG_USBCDC_INITTIMEOUT      (5000)
#define CFG_USBCDC_BUFFERSIZE       (256)
#endif

#ifdef CFG_BRD_LPC1343_REFDESIGN_MINIMAL
// #define CFG_USBHID
// #define CFG_USBCDC
#define CFG_USBCDC_BAUDRATE         (115200)
#define CFG_USBCDC_INITTIMEOUT      (5000)
#define CFG_USBCDC_BUFFERSIZE       (256)
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
// #define CFG_USBHID
#define CFG_USBCDC
#define CFG_USBCDC_BAUDRATE         (115200)
#define CFG_USBCDC_INITTIMEOUT      (5000)
#define CFG_USBCDC_BUFFERSIZE       (256)
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
// #define CFG_USBHID
// #define CFG_USBCDC
#define CFG_USBCDC_BAUDRATE         (57600)
#define CFG_USBCDC_INITTIMEOUT      (5000)
#define CFG_USBCDC_BUFFERSIZE       (256)
#endif

#ifdef CFG_BRD_LPC1343_802154USBSTICK
// #define CFG_USBHID
#define CFG_USBCDC
#define CFG_USBCDC_BAUDRATE         (115200)
#define CFG_USBCDC_INITTIMEOUT      (5000)
#define CFG_USBCDC_BUFFERSIZE       (256)
#endif
/*=========================================================================*/

/*=========================================================================
 PRINTF REDIRECTION
 -----------------------------------------------------------------------

 CFG_PRINTF_UART           Will cause all printf statements to be 
 redirected to UART
 CFG_PRINTF_USBCDC         Will cause all printf statements to be
 redirect to USB Serial
 CFG_PRINTF_NEWLINE        This is typically "\r\n" for Windows or
 "\n" for *nix

 Note: If no printf redirection definitions are present, all printf
 output will be ignored.
 -----------------------------------------------------------------------*/
#ifdef CFG_BRD_LPC1343_REFDESIGN
// #define CFG_PRINTF_UART
#define CFG_PRINTF_USBCDC
#define CFG_PRINTF_NEWLINE          "\r\n"
#endif

#ifdef CFG_BRD_LPC1343_REFDESIGN_MINIMAL
#define CFG_PRINTF_UART
// #define CFG_PRINTF_USBCDC
#define CFG_PRINTF_NEWLINE          "\r\n"
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB
// #define CFG_PRINTF_UART
#define CFG_PRINTF_USBCDC
#define CFG_PRINTF_NEWLINE          "\r\n"
#endif

#ifdef CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART
#define CFG_PRINTF_UART
// #define CFG_PRINTF_USBCDC
#define CFG_PRINTF_NEWLINE          "\n"
#endif

#ifdef CFG_BRD_LPC1343_802154USBSTICK
// #define CFG_PRINTF_UART
#define CFG_PRINTF_USBCDC
#define CFG_PRINTF_NEWLINE          "\r\n"
#endif
/*=========================================================================*/


/*=========================================================================
 PWM SETTINGS
 -----------------------------------------------------------------------

 CFG_PWM                     If this is defined, a basic PWM driver
 will be included using 16-bit Timer 1 and
 Pin 1.9 (MAT0) for the PWM output.  In
 order to allow for a fixed number of
 pulses to be generated, some PWM-specific
 code is required in the 16-Bit Timer 1
 ISR.  See "core/timer16/timer16.c" for
 more information.
 CFG_PWM_DEFAULT_PULSEWIDTH  The default pulse width in ticks
 CFG_PWM_DEFAULT_DUTYCYCLE   The default duty cycle in percent

 DEPENDENCIES:               PWM output requires the use of 16-bit
 timer 1 and pin 1.9 (CT16B1_MAT0).
 -----------------------------------------------------------------------*/
// #define CFG_PWM
#define CFG_PWM_DEFAULT_PULSEWIDTH  (CFG_CPU_CCLK / 1000)
#define CFG_PWM_DEFAULT_DUTYCYCLE   (50)
/*=========================================================================*/


/*=========================================================================
 CONFIG FILE VALIDATION
 -------------------------------------------------------------------------
 Basic error checking to make sure that incompatible defines are not 
 enabled at the same time, etc.

 =========================================================================*/

#if !defined CFG_BRD_LPC1343_REFDESIGN && !defined CFG_BRD_LPC1343_REFDESIGN_MINIMAL && !defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_USB && !defined CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART && !defined CFG_BRD_LPC1343_802154USBSTICK
#error "You must defined a target board (CFG_BRD_LPC1343_REFDESIGN or CFG_BRD_LPC1343_REFDESIGN_MINIMAL or CFG_BRD_LPC1343_TFTLCDSTANDALONE or CFG_BRD_LPC1343_TFTLCDSTANDALONE_UART or CFG_BRD_LPC1343_802154USBSTICK)"
#endif

#if defined CFG_PRINTF_USBCDC && defined CFG_PRINTF_UART
#error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC cannot both be defined at once"
#endif

#if defined CFG_PRINTF_USBCDC && !defined CFG_USBCDC
#error "CFG_PRINTF_CDC requires CFG_USBCDC to be defined as well"
#endif

#if defined CFG_USBCDC && defined CFG_USBHID
#error "Only one USB class can be defined at a time (CFG_USBCDC or CFG_USBHID)"
#endif

#if defined CFG_SSP0_SCKPIN_2_11 && defined CFG_SSP0_SCKPIN_0_6
#error "Only one SCK pin can be defined at a time for SSP0"
#endif

#if !defined CFG_SSP0_SCKPIN_2_11 && !defined CFG_SSP0_SCKPIN_0_6
#error "An SCK pin must be selected for SSP0 (CFG_SSP0_SCKPIN_2_11 or CFG_SSP0_SCKPIN_0_6)"
#endif

#ifdef CFG_INTERFACE
#if !defined CFG_PRINTF_UART && !defined CFG_PRINTF_USBCDC
#error "CFG_PRINTF_UART or CFG_PRINTF_USBCDC must be defined for for CFG_INTERFACE Input/Output"
#endif
#if defined CFG_PRINTF_USBCDC && CFG_INTERFACE_SILENTMODE == 1
#error "CFG_INTERFACE_SILENTMODE typically isn't enabled with CFG_PRINTF_USBCDC"
#endif
#endif

#ifdef CFG_CHIBI
#if !defined CFG_I2CEEPROM
#error "CFG_CHIBI requires CFG_I2CEEPROM to store and retrieve addresses"
#endif
#ifdef CFG_SDCARD
#error "CFG_CHIBI and CFG_SDCARD can not be defined at the same time. Only one SPI block is available on the LPC1343."
#endif
#ifdef CFG_TFTLCD
#error "CFG_CHIBI and CFG_TFTLCD can not be defined at the same time since they both use pins 1.8, 1.9 and 1.10."
#endif
#ifdef CFG_PWM
#error "CFG_CHIBI and CFG_PWM can not be defined at the same time since they both use pin 1.9."
#endif
#if CFG_CHIBI_PROMISCUOUS != 0 && CFG_CHIBI_PROMISCUOUS != 1
#error "CFG_CHIBI_PROMISCUOUS must be equal to either 1 or 0"
#endif
#endif

#ifdef CFG_TFTLCD
#ifdef CFG_ST7565
#error "CFG_TFTLCD and CFG_ST7565 can not be defined at the same time."
#endif
#ifdef CFG_SSD1306
#error "CFG_TFTLCD and CFG_SSD1306 can not be defined at the same time."
#endif
#ifdef CFG_PWM
#error "CFG_TFTLCD and CFG_PWM can not be defined at the same time since they both use pin 1.9."
#endif
#if !defined CFG_I2CEEPROM
#error "CFG_TFTLCD requires CFG_I2CEEPROM to store and retrieve configuration settings"
#endif
#endif

#ifdef CFG_SDCARD
#ifdef CFG_STEPPER
#error  "CFG_SDCARD and CFG_STEPPER can not be defined at the same time since they both use pin 3.0."
#endif
#endif

#ifdef CFG_ST7565
#ifdef CFG_SSD1306
#error "CFG_ST7565 and CFG_SSD1306 can not be defined at the same time"
#endif
#endif

#ifdef CFG_RSA
#if CFG_RSA_BITS != 64 && CFG_RSA_BITS != 32
#error "CFG_RSA_BITS must be equal to either 32 or 64."
#endif
#endif

// UMS specific additions

/// undef this to turn off ums and enable some test code
//#define CFG_UMS

#endif
