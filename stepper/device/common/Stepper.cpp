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

Stepper::Stepper() : pool_(messageBlock_, sizeof(messageBlock_)), engine_(this), pause_(false)
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
		am.window_ = pool_.freed()[1];

	}
	break;

	case DATA_SCRIPT_MSG:
	{
		MessageBuffer *mb = pool_.alloc(AckScriptMsg::PAYLOAD_SIZE);
		if (mb == NULL) {
			///\todo error here
		} else {
			AckScriptMsg &am = AckScriptMsg::init(*mb);
			am.window_ = pool_.freed()[1];
			mb->header().id1_ = m.header().id1_;
			sendMessage(mb);
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
