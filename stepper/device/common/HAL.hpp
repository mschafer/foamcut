/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */

#ifndef stepper_device_HALBase_hpp
#define stepper_device_HALBase_hpp

#include "Message.hpp"
#include "StepDir.hpp"
#include "LimitSwitches.hpp"
#include "StatusFlags.hpp"

/**
 * This namespace contains the hardware abstraction layer that allows porting
 * the stepper device to other embedded processors.
 */
namespace stepper { namespace device { namespace HAL {


/**
 * This method will be called before any other HAL method.
 */
void initialize();

/**
 * The Stepper require 8 digital output pins organized in 4 pairs of step and
 * direction bits.  This functon should set the specified pins to the desired
 * output level where a 1 indicates a high voltage and 0 is low.
 */
void setStepDirBits(const StepDir &s);

/**
 * The stepper uses 8 digital input pins as limit switches.  If the hardware
 * does not support limit switches, then this should always return a default
 * constructed LimitSwitches object.
 */
LimitSwitches readLimitSwitches();

/**
 * Send a Message back to the host.
 * On SUCCESS, the HAL takes ownership of m and should delete it
 * when the send is complete.
 */
ErrorCode sendMessage(Message *m, Message::Priority p = Message::NORMAL_PRIORITY);

/**
 * Get the next message from the receive buffer or nullptr if there are none.
 */
Message *receiveMessage();

/**
 * The target must configure a one-shot timer with a 5 uSec input clock.
 * A call to this method should result in a call to
 * Stepper::instance().onTimerExpired() after  \em period * 5 uSec.
 */
void startTimer(uint32_t period);

/**
 * Stop the timer if it is running.  Otherwise do nothing.
 */
void stopTimer();

}}}

#endif
