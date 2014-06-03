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

#include <boost/chrono.hpp>
#include <Simulator.hpp>
#include <Device.hpp>
#include "Host.hpp"
#include "TCPLink.hpp"
#include "Script.hpp"

namespace stepper {

Host::Host() : work_(ios_), timer_(ios_), state_(Host::NO_DEVICE), pongCount_(0)
{
	boost::function<void ()> f = boost::bind(&boost::asio::io_service::run, &ios_);
    thread_.reset(new boost::thread(f));

    timer_.expires_from_now(boost::posix_time::milliseconds(20));
    timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));
}

Host::~Host()
{
	ios_.stop();
	if (thread_->joinable()) {
		thread_->join();
	}
}

bool Host::connectToSimulator()
{
	link_.reset();
	state_ = NO_DEVICE;

	device::Simulator::reset();
	uint16_t port = device::Simulator::instance().port();

	link_.reset(new TCPLink("localhost", port, ios_));
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	return ping();
}

void Host::executeScript(const Script &s)
{
	if (state_ != IDLE_DEVICE) {
		throw std::logic_error("Host::executeScript failed, device is not idle");
	}

	std::unique_ptr<Script::MessageCollection> msgs(s.generateMessages());

	// send an init script message and wait for ack
	device::InitScriptMsg *ism = new device::InitScriptMsg();
	link_->send(ism);

}

void Host::runOnce(const boost::system::error_code& error)
{
	if (link_) {
		device::Message *m;
		while ((m = link_->receive()) != nullptr) {
			switch (m->function()) {

			case device::PingResponseMsg::FUNCTION:
				++pongCount_;
				delete m;
				break;

			default:
				///\todo unrecognized message error
				delete m;
				break;
			}
		}
	}

	if (!ios_.stopped()) {
		timer_.expires_from_now(boost::posix_time::milliseconds(20));
		timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));
	}
}

bool Host::ping()
{
	size_t currentCount = pongCount_.load();
	device::PingMsg *pm = new device::PingMsg();
	int tries = 10;
	while (link_->send(pm) != device::HAL::SUCCESS && tries > 0) {
		--tries;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
	if (tries == 0) {
		link_.reset();
		return false;
	}

	tries = 10;
	while (pongCount_.load() == currentCount && tries != 0) {
		--tries;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
	}
	return (tries != 0);
}

}
