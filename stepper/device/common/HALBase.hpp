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

/** \todo
 * Convert HALBase from class to HAL namespace and use free functions.
 * Make Status return type a class w/ overloaded bool conversion.
 */

namespace stepper { namespace device {

/**
 * This class contains the hardware abstraction layer that allows porting
 * the stepper device to other embedded processors.  Uses CRTP for static
 * polymorphism.
 */
template <typename Derived>
class HALBase {
public:

	enum Status {
		SUCCESS = 0,
		ERROR = 1,
		RESOURCE_UNAVAILABLE = 2
	};

	enum Priority {
		NORMAL_PRIORITY,
		HIGH_PRIORITY
	};

	/**
	 * This method will be called before any other HAL method.
	 */
	static void initialize() { Derived::initialize(); }

	/**
	 * The Stepper require 8 digital output pins organized in 4 pairs of step and
	 * direction bits.  This funciton should set the specifed pins to the desired
	 * output level where a 1 indicates a high voltage and 0 is low.
	 */
	static void setStepDirBits(const StepDir &s) { Derived::setStepDirBits(s); }

	/**
	 * The stepper uses 8 digital input pins as limit switches.  If the hardware
	 * does not support limit switches, then this should always return a default
	 * constructed LimitSwitches object.
	 */
	static LimitSwitches readLimitSwitches() { return Derived::readLimitSwitches(); }

	/**
	 * Send a Message back to the host.
	 * On SUCCESS, the HAL takes ownership of m and should delete it
	 * when the send is complete.
	 * \todo should be calling this instead of HAL::Send?
	 */
	static Status sendMessage(Message *m, Priority p = NORMAL_PRIORITY) { return Derived::sendMessage(m, p); }

	/**
	 * Get the next message from the receive buffer or nullptr if there are none.
	 */
	static Message *receiveMessage() { return Derived::receiveMessage(); }

	/**
	 * The target must configure a one-shot timer with a 5 uSec input clock.
	 * A call to this method should result in a call to
	 * Stepper::instance().onTimerExpired() after exactly period * 5 uSec.
	 */
	static void startTimer(uint32_t period) { Derived::startTimer(period); }

	/**
	 * Stop the timer if it is running.
	 */
	static void stopTimer();

	/**
	 * Causes a hard reset of the device.
	 */
	static void reset() { Derived::reset(); }

};

}}

#endif
