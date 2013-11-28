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
#ifndef stepper_device_Stepper_hpp
#define stepper_device_Stepper_hpp

#include "StepDir.hpp"
#include "LimitSwitches.hpp"
#include "Engine.hpp"

namespace stepper { namespace device {

class Stepper
{
public:
	typedef Message<DeviceMessageAllocator> DeviceMessage;

	enum {
		TIMER_PERIOD_USEC = 5,
	};

	Stepper();
	virtual ~Stepper() {}

	/**
	 * This method should be called from main as often as possible.
	 * It is responsible for processing incoming data from the host,
	 * sending telemetry back, and scheduling the timer to produce steps.
	 */
	void runBackgroundOnce();

	/**
	 * This method drives the steps onto the digital outputs.
	 * It should be called whenever the Platform's timer expires.
	 */
	void onTimerExpired();

	/**
	 * Specify bits in \sa StepDir that need to be inverted.
	 * Inverting a step bit will result in the generation of a falling edge
	 * whenever a step is desired instead of a rising edge.
	 * Inverting a direction bit will cause forward steps to output a logical
	 * 0 on the direction pin instead of a 1.
	 */
	void invertMask(StepDir invertMask) { invertMask_ = invertMask; }


	/**
	 * Set the output value of the digital I/O lines for each step
	 * and direction output the value in s.
	 */
	virtual void setStepDirBits(const StepDir &s) = 0;

	virtual LimitSwitches readLimitSwitches() = 0;

	virtual void startTimer(uint32_t period) = 0;

protected:

	/**
	 * This routine is called whenever a message is added to the send
	 * queue. Implementors should use this hook to wake up the transmission
	 * thread or to start an interrupt driven send.  It could also be used to
	 * do a blocking send.
	 */
	virtual void notifySender() = 0;

	/**
	 * Called from the background task.
	 * Implementors may add messages to the rxQueue in this method.
	 */
	virtual void pollForMessages() = 0;

private:
	StepDir invertMask_;
	Engine engine_;
	volatile bool pause_;

	void handleMessage(DeviceMessage &m);

	/** \return scaled value of the delay. */
	uint32_t scaleDelay(uint32_t delay);

	Stepper(const Stepper &cpy);
	Stepper &operator=(const Stepper &rhs);

#if __cplusplus >= 201103L
	static_assert(sizeof(DeviceMessage)%sizeof(void*)==0,"Alignment problem with Message");
	static_assert(offsetof(DeviceMessage, reserved_)+sizeof(void*) == sizeof(DeviceMessage), "Message layout is incorrect");
#endif

};

}}

#endif
