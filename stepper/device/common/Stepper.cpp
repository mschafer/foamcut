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
#include <new>
#include "Stepper.hpp"
#include "StepperDictionary.hpp"
#include <Communicator.hpp>

namespace stepper { namespace device {

Stepper::Stepper() : pause_(false)
{
///\todo
#if 0
	// alloc and free big blocks for script
	for (int i=0; i<Platform::SCRIPT_MSG_POOL; ++i) {
		DataScriptMsg *p = new DataScriptMsg();
		assert(p != NULL);
		delete p;
		///\todo verify size of free list
	}
#endif
}

uint8_t Stepper::id() const
{
	return STEPPER_ID;
}

void Stepper::receiveMessage(Message *msg)
{
	handleMessage(*msg);
}

void Stepper::connected(bool c)
{
}

void Stepper::runBackgroundOnce()
{
	Communicator &comm = getCommunicator();
	comm();
	engine_();
}

void Stepper::onTimerExpired()
{
	if (engine_.done()) return;

	if (pause_) {
		pause_ = false;
		return;
	}

	Line::NextStep ns;
	bool r = engine_.nextStep(ns);
	if (!r) {
		///\todo underflow error here
	} else {
		if (ns.delay_ == 0) {
			///\todo delay == 0 means we are stopping
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


void Stepper::handleMessage(Message &m)
{
	Communicator &comm = getCommunicator();

	switch (m.function()) {

	case GO_MSG:
	{
		///\todo error if engine done
		startTimer(200);
		delete &m;
	}
	break;

	case PAUSE_MSG:
	{
		pause_ = true;
		delete &m;
	}
	break;

	case RESET_MSG:
	{
		///\todo implement me
		delete &m;
	}
	break;

	case SPEED_ADJUST_MSG:
	{
		///\todo implement me
		delete &m;
	}
	break;

	case INIT_SCRIPT_MSG:
	{
		engine_.init();
		AckScriptMsg *am = new (&m) AckScriptMsg();
		am->window_ = 0;  ///\todo get the real window size!
		comm.sendMessage(am);
	}
	break;

	case DATA_SCRIPT_MSG:
	{
		AckScriptMsg *amb = new AckScriptMsg();
		if (amb == NULL) {
			///\todo error here
		} else {
			amb->window_ = 0; ///\todo get real window size!
			comm.sendMessage(amb);
		}
		engine_.addScriptMessage(&m);
	}
	break;

	default:
		///\todo error unrecognized message here
		delete (&m);
		break;
	}
}

uint32_t Stepper::scaleDelay(uint32_t delay)
{
	return delay;
}

}}
