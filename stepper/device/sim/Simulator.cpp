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

	backgroundTimer_.expires_from_now(boost::posix_time::milliseconds(10));
	backgroundTimer_.async_wait(boost::bind(&Simulator::runBackground, this));

	impl_.reset(new ASIOImpl<Simulator>(*this));

	thread_.reset(new boost::thread(boost::bind(&Simulator::runComm, this)));
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
    ios_.post(boost::bind(&ASIOImpl<Simulator>::startSending, impl_.get()));
}

void Simulator::pollForMessages()
{
	// nothing to do here because asio does it in background
}

void Simulator::acceptComplete(const boost::system::error_code &error)
	{
    if (!error) {
    	std::cout << "accept complete" << std::endl;
    	impl_->receiveOne();
    	impl_->startSending();
    } else {
    	///\todo handle error
    }
}

void Simulator::handler(device::MessageBuffer *msg, const boost::system::error_code &error)
{
	if (msg != nullptr) {
		rxQueue_.push(msg);
		impl_->receiveOne();
	}

	if (error) {
		///\todo handle error
    	std::cout << "Simulator error " << error << std::endl;
	}
}

MessageBuffer *Simulator::popTx()
{
	return txQueue_.pop();
}

void Simulator::runBackground()
{
	runBackgroundOnce();
	if (running_) {
		backgroundTimer_.expires_from_now(boost::posix_time::milliseconds(10));
		backgroundTimer_.async_wait(boost::bind(&Simulator::runBackground, this));
	}
}

void Simulator::setStepDirBits(const StepDir &s)
{
	machine_.setStepDir(s);
}

LimitSwitches Simulator::readLimitSwitches()
{
	return machine_.readLimitSwitches();
}

void Simulator::startTimer(uint32_t period)
{
	uint32_t delay = period * Stepper::TIMER_PERIOD_USEC;
	stepTimer_.expires_from_now(boost::posix_time::microseconds(delay));
	stepTimer_.async_wait(boost::bind(&Stepper::onTimerExpired, this));
}

}}
