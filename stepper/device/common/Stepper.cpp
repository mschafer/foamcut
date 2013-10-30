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
#include "Stepper.hpp"
#include "Dictionary.hpp"
#include <Platform.hpp>

namespace stepper { namespace device {

Stepper::Stepper() : pool_(messageBlock_, sizeof(messageBlock_)), engine_(this)
{
	// alloc and free big blocks for script
	for (int i=0; i<platform::SCRIPT_MSG_POOL; ++i) {
		MessageBuffer *mb = alloc(ScriptMsg::PAYLOAD_SIZE);
		assert(mb != NULL);
		free(mb);
	}
}

void Stepper::runBackgroundOnce()
{
	pollForMessages();
	MessageBuffer *m = rxQueue_.pop();
	if (m) { handleMessage(*m); }
	engine_();
}

void Stepper::onTimerExpired()
{
	Line::NextStep ns;
	bool r = engine_.nextStep(ns);
	if (!r) {
		///\todo underflow error here
	} else {
		// delay == 0 means we are stopping
		if (ns.delay_ == 0) {

		} else {
			startTimer(scaleDelay(ns.delay_));

			LimitSwitches limits = readLimitSwitches();
			StepDir s = limits.apply(ns.step_);

			// set the direction bits with all steps inactive
			setStepDirBits(s.getDirOnlyBitVals(invertMask_));

			// set the direction bits with desired steps active
			setStepDirBits(s.getStepDirBitVals(invertMask_));

			// steps are edge triggered so return them inactive
			setStepDirBits(s.getDirOnlyBitVals(invertMask_));
		}
	}
}

void Stepper::sendMessage(MessageBuffer *mb)
{
	txQueue_.push(mb);
	notifySender();
}

void Stepper::handleMessage(MessageBuffer &m)
{
	switch (m.header().id0_) {
	case PING_MSG:
	{
		PongMsg::init(m);
		sendMessage(&m);
	}
	break;

	case GO_MSG:
	{

	}
	break;

	case PAUSE_MSG:
	{

	}
	break;

	case RESET_MSG:
	{

	}
	break;

	case SPEED_ADJUST_MSG:
	{

	}
	break;

	case INIT_SCRIPT_MSG:
	{

	}
	break;

	case DATA_SCRIPT_MSG:
	{
		engine_.addScriptMessage(&m);
	}
	break;

	default:
		///\todo error unrecognized message here
		free(&m);
		break;
	}
}

uint32_t Stepper::scaleDelay(uint32_t delay)
{
	return delay;
}

}}
