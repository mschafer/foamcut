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
#include <chrono>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "SimStepper.hpp"

namespace stepper { namespace device {

SimStepper::SimStepper() : running_(true)
{
}

void SimStepper::initialize()
{
//	backgroundTimer_.expires_from_now(boost::posix_time::milliseconds(10));
//	backgroundTimer_.async_wait(boost::bind(&SimStepper::runBackground, this));
}


SimStepper::~SimStepper()
{
	running_ = false;
//	backgroundTimer_->cancel();
//	stepTimer_->cancel();
}


void SimStepper::runBackground()
{
	runBackgroundOnce();
	if (running_) {
		//backgroundTimer_->expires_from_now(boost::posix_time::milliseconds(10));
		//backgroundTimer_->async_wait(boost::bind(&SimStepper::runBackground, this));
	}
}

void SimStepper::setStepDirBits(const StepDir &s)
{
	machine_.setStepDir(s);
}

LimitSwitches SimStepper::readLimitSwitches()
{
	return machine_.readLimitSwitches();
}

void SimStepper::startTimer(uint32_t period)
{
	uint32_t delay = period * Stepper::TIMER_PERIOD_USEC;
	//stepTimer_->expires_from_now(boost::posix_time::microseconds(delay));
	//stepTimer_->async_wait(boost::bind(&Stepper::onTimerExpired, this));
}

}}
