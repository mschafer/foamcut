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
#include <algorithm>
#include "Stepper.hpp"
#include "StepperDictionary.hpp"
#include "StatusFlags.hpp"
#include <HAL.hpp>
#include <Logger.hpp>

namespace stepper { namespace device {

Stepper::Stepper() : pause_(false), speedAdjust_(1<<SpeedAdjustMsg::UNITY_SPEED_ADJUST_SHIFT)
{
}

Stepper &Stepper::instance()
{
	static Stepper theStepper;
	return theStepper;
}

void Stepper::runOnce()
{
	// handle received messages
	Message *m = NULL;
	if ((m = HAL::receiveMessage()) != NULL) {
		handleMessage(m);
	}

	// check for status changes
	if (StatusFlags::instance().updated()) {
		StatusMsg *sm = new StatusMsg();
		if (sm) {
			sm->statusFlags_ = StatusFlags::instance();
			if (HAL::sendMessage(sm) == SUCCESS) {
				StatusFlags::instance().clear();
			} else {
				delete sm;
			}
		}
	}

	// run the engine which parses the script buffer
	engine_();

	// check for limit switch activations
	LimitSwitches limits = HAL::readLimitSwitches();
	if (limits != lastLimits_) {
		LimitSwitchesMsg *lsm = new LimitSwitchesMsg();
		if (lsm) {
			lsm->limits_ = limits;
			if (HAL::sendMessage(lsm) == SUCCESS) {
				lastLimits_ = limits;
			} else {
				delete lsm;
			}
		}
	}
}

void Stepper::onTimerExpired()
{
	if (pause_) {
		pause_ = false;
		StatusFlags::instance().clear(StatusFlags::ENGINE_RUNNING);
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
		StatusFlags::instance().clear(StatusFlags::ENGINE_RUNNING);
	}

}


void Stepper::handleMessage(Message *m)
{

	switch (m->id()) {

	case GO_MSG:
	{
		Logger::trace("stepper", "go received");
		if(StatusFlags::instance().get(StatusFlags::ENGINE_RUNNING)) {
			error(STEPPER_ALREADY_RUNNING);
		} else {
			StatusFlags::instance().set(StatusFlags::ENGINE_RUNNING);
			HAL::startTimer(200);
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

	case SPEED_ADJUST_MSG:
	{
		SpeedAdjustMsg *sam = static_cast<SpeedAdjustMsg*>(m);
		speedAdjust_ = sam->speedAdjust_;
		delete m;
	}
	break;

	case CONNECT_MSG:
	{
		ConnectMsg *cm  = static_cast<ConnectMsg*>(m);
		setupConnection(cm);
		StatusFlags::instance().set(StatusFlags::CONNECTED);
		ConnectResponseMsg *crm = new (m) ConnectResponseMsg();
		if (crm) {
			if (!HAL::sendMessage(crm) == SUCCESS) {
				delete crm;
			}
		}
	}
	break;

	case HEARTBEAT_MSG:
	{
		Logger::trace("stepper", "heartbeat received");
		HeartbeatResponseMsg *hrm = new (m) HeartbeatResponseMsg();
		if (hrm) {
			if (!HAL::sendMessage(hrm) == SUCCESS) {
				delete hrm;
			}
		}
	}
	break;

	case DATA_SCRIPT_MSG:
	{
		Logger::trace("stepper", "data received");
		engine_.addScriptMessage(m);
	}
	break;

	default:
		error(UNRECOGNIZED_MESSAGE);
		delete (m);
		break;
	}
}

void Stepper::setupConnection(ConnectMsg *cm)
{
	HAL::stopTimer();
	StatusFlags::instance().reset();
	pause_ = false;
	invertMask_ = cm->invertMask_;
	speedAdjust_ = 1 << SpeedAdjustMsg::UNITY_SPEED_ADJUST_SHIFT;
	StatusFlags::instance().clear(StatusFlags::CONNECTED);
	StatusFlags::instance().clear(StatusFlags::ENGINE_RUNNING);
	engine_.setupConnection();
}

uint32_t Stepper::scaleDelay(uint32_t delay)
{
	delay = std::min(delay, 0xFFFFu);
	static const uint32_t minDelay = 1000000 / (TIMER_PERIOD_USEC * MAX_STEPS_PER_SEC);
	uint32_t sdelay = (delay * speedAdjust_) >> SpeedAdjustMsg::UNITY_SPEED_ADJUST_SHIFT;
	sdelay = std::min(sdelay, 0xFFFFu);
	sdelay = std::max(sdelay, minDelay);
	return sdelay;
}

}}
