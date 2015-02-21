/*
 * (C) Copyright 2015 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#include "core/gpio/gpio.h"
#include "core/usbcdc/cdcuser.h"
#include "step_timer.h"
//#include "platform.h"
//#include "commands.h"
//#include "stepper.h"

/**
 * \file Implementation of ums platform specific functions for LPC-P1343 with Rowley
 */

uint8_t pf_send_bytes(const uint8_t *data, uint16_t size)
{
	CDC_BlockingWriteInEp(data, size);
	return 0;
}

uint8_t pf_receive_byte(uint8_t *rxByte)
{
    uint8_t ret = CDC_ReadByte(rxByte);
    ///\todo ums and lpc code have inverted notion of return value for success
    return (ret == 0) ? 1 : 0;
}

uint8_t pf_configure_port_pin(uint8_t port, uint8_t pin, enum ums_pin_func func)
{
	uint8_t ret = 1;
	switch (func) {
	case UMS_OUTPUT_PIN:
		ret = gpioSetPinFunction(port, pin, GPIO_OUTPUT_PIN);
		break;
	case UMS_INPUT_PULLUP_PIN:
		ret = gpioSetPinFunction(port, pin, GPIO_INPUT_PULLUP_PIN);
		break;
	case UMS_INPUT_PULLDOWN_PIN:
		ret = gpioSetPinFunction(port, pin, GPIO_INPUT_PULLDOWN_PIN);
		break;
	}
	return ret;
}

void pf_set_port_pin(uint8_t port, uint8_t pin, uint8_t val)
{
    gpioSetPin(port, pin, val);
}

uint8_t pf_read_port_pin(uint8_t port, uint8_t pin)
{
    return gpioGetPin(port, pin);
}

void pf_set_step_timer(uint32_t val)
{
	step_timer_start(val);
}

uint8_t pf_is_timer_running()
{
    return step_timer_enabled();
}

void pf_init_axes()
{
	// default pin configuration for lpc-p1343
	struct AxisCmd ac;
	ac.cmdId = AxisCmd_ID;
	ac.name = 'x';
	ac.stepPort = 3;
	ac.stepPin = 0;
	ac.dirPort = 3;
	ac.dirPin = 1;
	ac.fwdPort = 2;
	ac.fwdPin = 0 | UMS_INVERT_PIN;
	ac.revPort = 2;
	ac.revPin = 1 | UMS_INVERT_PIN;
	st_setup_axis(&ac);

    ac.name = 'y';
    ac.stepPort = 3;
    ac.stepPin = 2;
    ac.dirPort = 3;
    ac.dirPin = 3;
    ac.fwdPort = 2;
    ac.fwdPin = 2 | UMS_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 3 | UMS_INVERT_PIN;
    st_setup_axis(&ac);

    ac.name = 'z';
    ac.stepPort = 2;
    ac.stepPin = 4;
    ac.dirPort = 2;
    ac.dirPin = 5;
    ac.fwdPort = 2;
    ac.fwdPin = 8 | UMS_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 9 | UMS_INVERT_PIN;
    st_setup_axis(&ac);

    ac.name = 'u';
    ac.stepPort = 2;
    ac.stepPin = 6;
    ac.dirPort = 2;
    ac.dirPin = 7;
    ac.fwdPort = 2;
    ac.fwdPin = 10 | UMS_INVERT_PIN;
    ac.revPort = 2;
    ac.revPin = 11 | UMS_INVERT_PIN;
    st_setup_axis(&ac);
}
