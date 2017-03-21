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
#include "Simulator.hpp"
#include "SimCommunicator.hpp"
#include "MemoryAllocator.hpp"
#include <Stepper.hpp>

std::ostream& operator<<(std::ostream& os, const stepper::device::StepDir& sd)
{
	for (int i=0; i<stepper::device::StepDir::AXIS_COUNT; ++i) {
		stepper::device::StepDir::AxisIdx ai = static_cast<stepper::device::StepDir::AxisIdx>(i);
		if (sd.dir(ai)) os << "+";
		else os << "-";
		if (sd.step(ai)) os << "1";
		else os << "0";
		os << "\t";
	}
	os << std::endl;
	return os;
}

namespace stepper { namespace device {

std::unique_ptr<Simulator> Simulator::theSim_;

MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}

Simulator::Simulator(uint16_t port) : backgroundTimer_(ios_), stepTimer_(ios_)
{
	posLog_.push_back(Position());
	for (int i=0; i<StepDir::AXIS_COUNT; i+=2) {
		limit_[i].low_ = X_LOWER_LIMIT_DEFAULT;
		limit_[i].high_ = X_UPPER_LIMIT_DEFAULT;
		limit_[i+1].low_ = Y_LOWER_LIMIT_DEFAULT;
		limit_[i+1].high_ = Y_UPPER_LIMIT_DEFAULT;
	}

	comm_.reset(new SimCommunicator(ios_, port));
	comm_->initialize();

	ios_.post(boost::bind(&Simulator::runOnce, this, boost::system::error_code()));

	boost::function<void ()> f = boost::bind(&boost::asio::io_service::run, &ios_);
	thread_.reset(new boost::thread(f));
}

Simulator::~Simulator()
{
	ios_.stop();
	comm_->shutdown();
	if (thread_->joinable()) {
		thread_->join();
	}
}

Simulator &Simulator::instance()
{
	if (theSim_.get() == nullptr) {
		theSim_.reset(new Simulator());
	}
	return *theSim_.get();
}

void Simulator::reset()
{
	theSim_.reset();
	Stepper &s = Stepper::instance();
	s.~Stepper();
	new (&s) Stepper();
}

uint16_t Simulator::port() const
{
	return comm_->port();
}

void Simulator::runOnce(const boost::system::error_code &ec)
{
	// don't do anything on error as it means Simulator is being forced to shut down
	if (!ec) {
		if (comm_->ready()) {
			Stepper &s = Stepper::instance();
			s.runOnce();
		}
		backgroundTimer_.expires_from_now(boost::posix_time::milliseconds(10));
		backgroundTimer_.async_wait(boost::bind(&Simulator::runOnce, this, boost::asio::placeholders::error));
	}
}

void Simulator::stepTimerExpired(const boost::system::error_code &ec)
{
    auto elapsed = std::chrono::steady_clock::now() - timerStart_;
    timerError_ -= elapsed;
    
	// don't do anything on error as it means Simulator is being forced to shut down
	if (!ec) {
		Stepper &s = Stepper::instance();
		s.runOnce();  // background sleeps in sim so prevent underflow
		s.onTimerExpired();
	}
}

void HAL::initialize()
{
}

void HAL::setStepDirBits(const StepDir &s)
{
	Simulator &sim = Simulator::instance();
	bool stepTaken = false;
	Position nextP;
	nextP.time_ = sim.time_;
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx ai = static_cast<StepDir::AxisIdx>(i);

		// set direction bits
		sim.currentBits_.dir(ai, s.dir(ai));

		// was there an edge on a step bit?
		int np = sim.position().pos_[i];
		if (s.step(ai) != sim.currentBits_.step(ai)) {
			sim.currentBits_.step(ai, s.step(ai));
			// was the edge in the right direction to cause a step?
			if (sim.currentBits_.step(ai) != sim.invertMask_.step(ai)) {
				stepTaken = true;
				np += ((sim.invertMask_.dir(ai) == sim.currentBits_.dir(ai)) ? -1 : 1);

				// apply limits
				np = std::max(np, sim.limit_[i].low_);
				np = std::min(np, sim.limit_[i].high_);
			}
		}
		nextP.pos_[i] = np;
	}

	if (stepTaken) {
		boost::lock_guard<boost::mutex> guard(sim.posLock_);
		sim.position_ = nextP;
		sim.posLog_.push_back(nextP);
	}
}

LimitSwitches HAL::readLimitSwitches()
{
	Simulator &sim = Simulator::instance();
	LimitSwitches ret;
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx idx = static_cast<StepDir::AxisIdx>(i);
		bool test = (sim.position().pos_[i] <= sim.limit_[i].low_);
		ret.reverseLimit(idx, test);
		test = (sim.position().pos_[i] >= sim.limit_[i].high_);
		ret.forwardLimit(idx, test);
	}
	return ret;
}

ErrorCode HAL::sendMessage(Message *m, Message::Priority priority)
{
	Simulator &sim = Simulator::instance();
	return sim.comm_->sendMessage(m, priority);
}

Message *HAL::receiveMessage()
{
	Simulator &sim = Simulator::instance();
	return sim.comm_->receiveMessage();
}

void HAL::startTimer(uint32_t period)
{
	Simulator &sim = Simulator::instance();
    double tick = (double)Stepper::TIMER_PERIOD_USEC * 1.e-6;
    sim.timerError_ += std::chrono::duration<double>(tick * period);
    sim.timerStart_ = std::chrono::steady_clock::now();
	sim.time_ += tick * period;

	// below a certain threshold, just go as fast as possible
    if (sim.timerError_ > std::chrono::milliseconds(100)) {
		sim.stepTimer_.expires_from_now(boost::posix_time::microseconds(1.e6*sim.timerError_.count()));
		sim.stepTimer_.async_wait(boost::bind(&Simulator::stepTimerExpired, &sim, boost::asio::placeholders::error));
	}
	else {
		sim.ios_.post(boost::bind(&Simulator::stepTimerExpired, &sim, boost::system::error_code()));
	}
}

void HAL::stopTimer()
{
	Simulator &sim = Simulator::instance();
	sim.stepTimer_.cancel();

}

}}
