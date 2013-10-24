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

#include "MessagePool.hpp"
#include "MessageQueue.hpp"
#include "StepDir.hpp"
#include "LimitSwitches.hpp"
#include "Engine.hpp"
#include <Platform.hpp>

namespace stepper { namespace device {

class Stepper
{
public:

	enum {
		TIMER_PERIOD_USEC = 5
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
	 * Allocate a MessageBuffer with at least the requested payload space.
	 */
	MessageBuffer *alloc(uint16_t payloadSize) {
		return pool_.alloc(payloadSize);
	}

	/**
	 * Free a MessageBuffer allocated from this Stepper.
	 */
	void free(MessageBuffer *mb) {
		pool_.free(mb);
	}

	/**
	 * This method transmits a message backs to the host.  Ownership
	 * of \em mb is taken over by the stepper and it will eventually
	 * be freed when transmission completes.
	 */
	void sendMessage(MessageBuffer *mb);

	/**
	 * Set the output value of the digital I/O lines for each step
	 * and direction output the value in s.
	 */
	virtual void setStepDirBits(const StepDir &s) = 0;

	virtual LimitSwitches readLimitSwitches() = 0;

	virtual void startTimer(uint32_t period) = 0;

protected:
	MessageQueue<platform::Lock> rxQueue_;
	MessageQueue<platform::Lock> txQueue_;

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
	uint8_t messageBlock_[2048];
	StepDir invertMask_;
	MessagePool<platform::Lock> pool_;
	Engine engine_;

	void handleMessage(MessageBuffer &m);

	/** \return scaled value of the delay. */
	uint32_t scaleDelay(uint32_t delay);

	Stepper(const Stepper &cpy);
	Stepper &operator=(const Stepper &rhs);

};

}}

#endif
