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
#include "Device.hpp"
#include "StatusFlags.hpp"
#include <HAL.hpp>

namespace stepper { namespace device {

Stepper::Stepper() : pause_(false), timerRunning_(false)
{
}

Stepper &Stepper::instance()
{
	static Stepper theStepper;
	return theStepper;
}

void Stepper::runBackgroundOnce()
{
	// handle received messages
	Message *m = NULL;
	if ((m = HAL::receiveMessage()) != NULL) {
		switch (m->id()) {
		case Device::DEVICE_MESSAGE_ID:
			Device::instance()(m);
			break;

		case STEPPER_MESSAGE_ID:
			handleMessage(m);
			break;

		default:
			///\todo error unrecognized message
			delete m;
			break;
		}
	}

	// run the engine and device
	engine_();
	Device::instance()(NULL);
}

void Stepper::onTimerExpired()
{
	if (pause_) {
		pause_ = false;
		timerRunning_ = false;
		return;
	}

	Line::NextStep ns;
	bool r = engine_.nextStep(ns);
	if (r) {
		HAL::startTimer(scaleDelay(ns.delay_));

		LimitSwitches limits = HAL::readLimitSwitches();
		StepDir s = limits.apply(ns.step_);

		// set the direction bits with all steps inactive
		HAL::setStepDirBits(s.getDirOnlyBitVals(invertMask_));

		// set the direction bits with desired steps active
		HAL::setStepDirBits(s.getStepDirBitVals(invertMask_));

		// steps are edge triggered so return them inactive
		HAL::setStepDirBits(s.getDirOnlyBitVals(invertMask_));
	} else {
		timerRunning_ = false;
	}

}


void Stepper::handleMessage(Message *m)
{

	switch (m->function()) {

	case GO_MSG:
	{
		if (!timerRunning_) {
			timerRunning_ = true;
			HAL::startTimer(200);
		} else {
			warning(TIMER_ALREADY_RUNNING);
		}
		delete m;
	}
	break;

	case PAUSE_MSG:
	{
		pause_ = true;
		delete m;
	}
	break;

	case RESET_MSG:
	{
		HAL::reset();
	}
	break;

	case SPEED_ADJUST_MSG:
	{
		SpeedAdjustMsg *sam = static_cast<SpeedAdjustMsg*>(m);
		speedAdjust_ = sam->speedAdjust_;
		delete m;
	}
	break;

	case DATA_SCRIPT_MSG:
	{
		engine_.addScriptMessage(m);
	}
	break;

	default:
		error(UNRECOGNIZED_MESSAGE);
		delete (m);
		break;
	}
}

uint32_t Stepper::scaleDelay(uint32_t delay)
{
	return delay;
}

}}
