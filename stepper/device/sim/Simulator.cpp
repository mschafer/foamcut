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
#include "Simulator.hpp"

namespace stepper { namespace device {

Simulator::Simulator(uint16_t port) : socket_(ios_),
	backgroundTimer_(ios_), stepTimer_(ios_), running_(true), port_(port)
{
    using boost::asio::ip::tcp;
    if (port_ != 0) {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint(tcp::v4(), port_)));
    } else {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint()));
        port_ = acceptor_->local_endpoint().port();
    }


	backgroundTimer_.expires_from_now(std::chrono::milliseconds(10));
	backgroundTimer_.async_wait(boost::bind(&Simulator::runBackground, this));
	thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &ios_)));

	for (int i=0; i<StepDir::AXIS_END; ++i) {
		pos_[i] = 0;
	}

}

Simulator::~Simulator()
{
	running_ = false;
	ios_.stop();

	try {
		thread_->join();
	} catch(...) {
	}
}

void Simulator::runComm()
{
    using namespace boost::asio::ip;
    try {
        while (running_) {
            acceptor_->async_accept(socket_,
		    boost::bind(&Simulator::acceptComplete, this,
		        boost::asio::placeholders::error));
            ios_.run();
            ios_.reset();
        }
    }

    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
        std::cerr << ex.what();
    }

    catch(...) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
    }

}

void Simulator::notifySender()
{
	impl_->startSending();
}

void Simulator::acceptComplete(const boost::system::error_code &error)
	{
    if (!error) {
    	///\todo
    }
}

void Simulator::handler(device::MessageBuffer *msg, const boost::system::error_code &error)
{
	///\todo
}

MessageBuffer *Simulator::popTx()
{
	return txQueue_.pop();
}






















void Simulator::runBackground()
{
	do {
		runBackgroundOnce();
		backgroundTimer_.expires_from_now(std::chrono::milliseconds(10));
		backgroundTimer_.async_wait(boost::bind(&Simulator::runBackground, this));
	} while (running_);
}

void Simulator::setStepDirBits(const StepDir &s)
{
	for (int i=0; i<StepDir::AXIS_END; ++i) {
		StepDir::AxisIdx ai = static_cast<StepDir::AxisIdx>(i);

		// set direction bits
		currentBits_.dir(ai, s.dir(ai));

		// was there an edge on a step bit?
		if (s.step(ai) != currentBits_.step(ai)) {
			currentBits_.step(ai, s.step(ai));
			// was the edge in the right direction to cause a step?
			if (currentBits_.step(ai) != invertMask_.step(ai)) {
				int np = pos_[i] + ((invertMask_.dir(ai) == currentBits_.dir(ai)) ? -1 : 1);

				// apply limits
				if (np >= limit_[i].first && np <= limit_[i].second) {
					pos_[i] = np;
				}
			}
		}
	}
}

LimitSwitches Simulator::readLimitSwitches()
{
	LimitSwitches ret;
	for (int i=0; i<StepDir::AXIS_END; ++i) {
		StepDir::AxisIdx idx = static_cast<StepDir::AxisIdx>(i);
		bool test = (pos_[i] <= limit_[i].first);
		ret.reverseLimit(idx, test);
		test = (pos_[i] >= limit_[i].second);
		ret.forwardLimit(idx, test);
	}
	return ret;
}

void Simulator::startTimer(uint32_t period)
{
	uint32_t delay = period * 5;
	stepTimer_.expires_from_now(std::chrono::microseconds(delay));
	stepTimer_.async_wait(boost::bind(&Simulator::runBackground, this));
}

}}
