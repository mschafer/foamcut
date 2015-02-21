/*
 * (C) Copyright 2015 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#include "Script.hpp"
#include <Engine.hpp>
#include <Stepper.hpp>
#include <limits>
#include <stdlib.h>

namespace stepper {

const double MAX_STEP_DELAY = std::numeric_limits<uint16_t>::max() * device::Stepper::TIMER_PERIOD_USEC * 1.e-6;
const double MAX_LINE_TIME = std::numeric_limits<uint32_t>::max() * device::Stepper::TIMER_PERIOD_USEC * 1.e-6;
const double SCRIPT_START_DELAY = .001;

Script::Script() : duration_(SCRIPT_START_DELAY)
{
	bytes_.push_back(device::Engine::DONE_CMD);
}

Script::~Script()
{
}

void
Script::addStep(device::StepDir sd, double delaySec)
{
	if (delaySec > MAX_STEP_DELAY) {
		throw std::out_of_range("delaySec too large for uint16_t");
	}
	device::Engine::SingleStepCmd ssc;
	ssc.delay_ = (uint16_t)(delaySec * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);
	ssc.stepDir_ = sd;
	bytes_.back() = device::Engine::SINGLE_STEP_CMD;
	uint8_t *p = (uint8_t*)&ssc;
	bytes_.insert(bytes_.end(), p, p+device::Engine::SingleStepCmd::SIZE);
	bytes_.push_back(device::Engine::DONE_CMD);
	duration_ += delaySec;
}

void
Script::addLine(int16_t dx, int16_t dy, int16_t dz, int16_t du, double duration)
{
	if (duration > MAX_LINE_TIME) {
		throw std::out_of_range("time too large for uint32_t");
	}
	uint32_t dtime = (uint32_t)(duration * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);

	int amaxd = abs(dx);
	amaxd = std::max(amaxd, abs(dy));
	amaxd = std::max(amaxd, abs(dz));
	amaxd = std::max(amaxd, abs(du));
	if (-amaxd < std::numeric_limits<int8_t>::min()) {
		device::Engine::LongLineCmd llc;
		llc.time_ = dtime;
		llc.dx_ = dx;
		llc.dy_ = dy;
		llc.dz_ = dz;
		llc.du_ = du;
		bytes_.back() = device::Engine::LONG_LINE_CMD;
		uint8_t *p = reinterpret_cast<uint8_t*>(&llc);
		bytes_.insert(bytes_.end(), p, p+device::Engine::LongLineCmd::SIZE);
	} else {
		device::Engine::LineCmd lc;
		lc.time_ = dtime;
		lc.dx_ = static_cast<uint8_t>(dx);
		lc.dy_ = static_cast<uint8_t>(dy);
		lc.dz_ = static_cast<uint8_t>(dz);
		lc.du_ = static_cast<uint8_t>(du);
		bytes_.back() = device::Engine::LINE_CMD;
		uint8_t *p = reinterpret_cast<uint8_t*>(&lc);
		bytes_.insert(bytes_.end(), p, p+device::Engine::LineCmd::SIZE);
	}
	bytes_.push_back(device::Engine::DONE_CMD);
	duration_ += duration;
}

void Script::addDelay(double duration)
{
	if (duration > MAX_LINE_TIME) {
		throw std::out_of_range("time too large for uint32_t");
	}
	uint32_t dtime = (uint32_t)(duration * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);
	bytes_.back() = device::Engine::DELAY_CMD;
	device::Engine::DelayCmd dc;
	dc.delay_ = dtime;
	uint8_t *p = (uint8_t*)&dc;
	bytes_.insert(bytes_.end(), p, p+device::Engine::DelayCmd::SIZE);
	duration_ += duration;
}

void Script::addHome(device::StepDir sd, double delaySec)
{
	if (delaySec > MAX_STEP_DELAY) {
		throw std::out_of_range("delaySec too large for uint16_t");
	}
	device::Engine::HomeCmd hc;
	hc.delay_ = (uint16_t)(delaySec * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);
	hc.stepDir_ = sd;
	bytes_.back() = device::Engine::HOME_CMD;
	uint8_t *p = (uint8_t*)&hc;
	bytes_.insert(bytes_.end(), p, p + device::Engine::HomeCmd::SIZE);
	bytes_.push_back(device::Engine::DONE_CMD);
	duration_ += delaySec;
}

std::unique_ptr<Script::MessageCollection>
Script::generateMessages() const
{
	std::unique_ptr<MessageCollection> ret(new MessageCollection());
	auto bit = bytes_.begin();
	while (bit < bytes_.end()) {
		uint16_t avail = static_cast<uint16_t>(bytes_.end() - bit);
		uint16_t take = std::min(avail, device::Message::maxPayloadCapacity());
		device::Message *m = device::Message::alloc(take);
		device::DataScriptMsg *dsm = new (m) device::DataScriptMsg(take);
		std::copy(bit, bit+take, dsm->payload());
		ret->push_back(m);
		bit += take;
	}
	return ret;
}

}
