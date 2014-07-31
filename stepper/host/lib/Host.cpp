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

#include <Simulator.hpp>
#include <StepperDictionary.hpp>
#include "Host.hpp"
#include "TCPLink.hpp"
#include "Script.hpp"

namespace stepper {

Host::Host() : work_(ios_), timer_(ios_),  pongCount_(0),
		heartbeatCount_(0), connected_(false)
{
    timer_.expires_from_now(boost::posix_time::milliseconds(20));
    timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));

    boost::function<void ()> f = boost::bind(&boost::asio::io_service::run, &ios_);
    thread_.reset(new boost::thread(f));

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
	connected_ = false;
	connectResponse_.reset();
	link_.reset();

	device::Simulator::instance().reset();
	uint16_t port = device::Simulator::instance().port();

	heartbeatTime_ = boost::chrono::steady_clock::now();
	link_.reset(new TCPLink("localhost", port, ios_));
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	device::ConnectMsg *cm = new device::ConnectMsg();
	link_->send(cm);
	device::HeartbeatMsg *hm = new device::HeartbeatMsg();
	link_->send(hm);

	int iter = 10;
	for (int iter=0; iter<10; ++iter) {
		if (connected() && heartbeatResponse_) return true;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	}
	return false;
}

void Host::executeScript(const Script &s)
{
	if (scriptRunning()) {
		throw std::logic_error("Host::executeScript failed, device is not idle");
	}

	scriptMsgs_ = s.generateMessages();

	// fill up the device window with script messages and then send a
	// GoMsg to start the device running.  The remainder of the
	// messages will be sent in response to Acks coming back.
	int i = 0;
	device::ErrorCode ec;
	while (i<device::DataScriptMsg::IN_FLIGHT_COUNT && !scriptMsgs_->empty()) {
		Script::MessageCollection::auto_type dsm = scriptMsgs_->pop_front();
		ec = link_->send(dsm.release());
		if (ec != device::SUCCESS) {
			throw std::runtime_error("Host::executeScript Unexpected send failure");
		}
	}
	device::GoMsg *gm = new device::GoMsg();
	ec = link_->send(gm);
	if (ec != device::SUCCESS) {
		throw std::runtime_error("Host::executeScript Unexpected send failure");
	}
}

bool Host::scriptRunning()
{
	boost::lock_guard<boost::mutex> guard(mtx_);
	return heartbeatResponse_->statusFlags_.get(device::StatusFlags::ENGINE_RUNNING);
}

void Host::runOnce(const boost::system::error_code& error)
{
	if (link_) {
		device::Message *m;
		while ((m = link_->receive()) != nullptr) {
			handleMessage(m);
		}
	}

	if (!ios_.stopped()) {
		timer_.expires_from_now(boost::posix_time::milliseconds(BACKGROUND_PERIOD_MSEC));
		timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));
	}
}

void Host::handleMessage(device::Message *m)
{
	switch (m->id()) {
	case device::PING_RESPONSE_MSG:
		++pongCount_;
		delete m;
		break;

	case device::ACK_SCRIPT_MSG:
	{
		if (!scriptMsgs_->empty()) {
			Script::MessageCollection::auto_type dsm = scriptMsgs_->pop_front();
			device::ErrorCode ec = link_->send(dsm.release());
			if (ec != device::SUCCESS) {
				throw std::runtime_error("Host::runOnce Unexpected send failure");
			}
		}
		delete m;
	}
	break;

	case device::HEARTBEAT_RESPONSE_MSG:
	{
		///\todo notify if status changes
		boost::lock_guard<boost::mutex> guard(mtx_);
		device::HeartbeatResponseMsg *hrm = static_cast<device::HeartbeatResponseMsg*>(m);
		heartbeatResponse_.reset(hrm);
		heartbeatCount_ = 0;
	}
	break;

	case device::CONNECT_RESPONSE_MSG:
	{
		boost::lock_guard<boost::mutex> guard(mtx_);
		device::ConnectResponseMsg *crm = static_cast<device::ConnectResponseMsg*>(m);
		connectResponse_.reset(crm);
		connected_ = true;
	}
	break;

	case device::FATAL_ERROR_MSG:
	{
		device::FatalErrorMsg *fem = static_cast<device::FatalErrorMsg*>(m);
		std::ostringstream oss;
		oss << "Host::runOnce Fatal error from device: " << (int)fem->fatalErrorCode_;
		throw std::runtime_error(oss.str());
		delete m;
	}
	break;

	default:
		delete m;
		throw std::runtime_error("Host::runOnce Unrecognized message");
		break;
	}
}

/**
 * Periodically send a heartbeat message to the device and
 * keep track of responses.  If more than a certain number
 * in a row go missing, then break the connection.
 */
void Host::heartbeat()
{
	// is it time to do a heartbeat?
	auto d = boost::chrono::steady_clock::now() - heartbeatTime_;
	if (d >= boost::chrono::milliseconds(HEARTBEAT_PERIOD_MSEC)) {

		// are we disconnected?
		if (heartbeatCount_ > HEARTBEAT_DISCONNECT_COUNT) {
			connected_ = false;
			throw std::runtime_error("Host::heartbeat Disconnect threshold exceeded");
		}

		heartbeatTime_ = boost::chrono::steady_clock::now();
		device::HeartbeatMsg *hm = new device::HeartbeatMsg();
		device::ErrorCode ec = link_->send(hm);
		if (ec == device::SUCCESS) {
			++heartbeatCount_;
		} else {
			delete hm;
		}
	}
}

bool Host::ping()
{
	size_t currentCount = pongCount_.load();
	device::PingMsg *pm = new device::PingMsg();
	int tries = 10;
	while (link_->send(pm) != device::SUCCESS && tries > 0) {
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
