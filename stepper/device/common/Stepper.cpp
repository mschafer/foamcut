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

Stepper::Stepper() : pause_(false)
{
	// alloc and free big blocks for script
	platform::MemoryAllocator_type &ma = platform::getMemoryAllocator();
	for (int i=0; i<platform::SCRIPT_MSG_POOL; ++i) {
		void *buff = ma.alloc(Message::memoryNeeded(DataScriptMsg::PAYLOAD_SIZE));
		assert(buff != NULL);
		ma.free(buff);
	}
}

void Stepper::runBackgroundOnce()
{
	pollForMessages();
	Message *m = platform::getCommunicator().receive();
	if (m) { handleMessage(*m); }
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
	platform::MemoryAllocator_type &ma = platform::getMemoryAllocator();
	platform::Communicator_type &comm = platform::getCommunicator();

	switch (m.id0()) {
	case PING_MSG:
	{
		PongMsg::init(m);
		comm.send(&m);
	}
	break;

	case GO_MSG:
	{
		///\todo error if engine done
		startTimer(200);
		free(&m);
	}
	break;

	case PAUSE_MSG:
	{
		pause_ = true;
		free(&m);
	}
	break;

	case RESET_MSG:
	{
		///\todo implement me
		free(&m);
	}
	break;

	case SPEED_ADJUST_MSG:
	{
		///\todo implement me
		free(&m);
	}
	break;

	case INIT_SCRIPT_MSG:
	{
		engine_.init();
		AckScriptMsg &am = AckScriptMsg::init(m);
		am.window_ = 0;  ///\todo get the real window size!
		comm.send(&m);
	}
	break;

	case DATA_SCRIPT_MSG:
	{
		Message *amb = reinterpret_cast<Message*>(ma.alloc(Message::memoryNeeded(AckScriptMsg::PAYLOAD_SIZE)));
		if (amb == NULL) {
			///\todo error here
		} else {
			AckScriptMsg &am = AckScriptMsg::init(*amb);
			am.window_ = 0; ///\todo get real window size!
			amb->id1(m.id1());
			comm.send(amb);
		}
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
