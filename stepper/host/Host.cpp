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
#include "CDCLink.hpp"
#include "Script.hpp"
#include <Logger.hpp>
#include <boost/asio.hpp>

namespace stepper {

struct HostImpl
{
	HostImpl() : work_(ios_), timer_(ios_) {}

	boost::asio::io_service ios_;
	boost::asio::io_service::work work_;
	boost::asio::deadline_timer timer_;
};

Host::Host() : pongCount_(0),
		heartbeatCount_(0), connected_(false), scriptMsgCount_(0), scriptMsgAckd_(0)
{
	impl_.reset(new HostImpl());
    impl_->timer_.expires_from_now(boost::posix_time::milliseconds(20));
	impl_->timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));

	boost::function<void()> f = boost::bind(&boost::asio::io_service::run, &impl_->ios_);
    thread_.reset(new boost::thread(f));

}

Host::~Host()
{
	impl_->ios_.stop();
	if (thread_->joinable()) {
		thread_->join();
	}
}

void Host::connectToSimulator()
{
	connected_ = false;
	connectResponse_.reset();
	link_.reset();

	device::Simulator::instance().reset();
	uint16_t port = device::Simulator::instance().port();

	heartbeatTime_ = boost::chrono::steady_clock::now();
	link_.reset(new TCPLink("localhost", port, impl_->ios_));
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	device::ConnectMsg *cm = new device::ConnectMsg();
	link_->send(cm);
	device::HeartbeatMsg *hm = new device::HeartbeatMsg();
	link_->send(hm);

	int iter = 10;
	for (int iter=0; iter<10; ++iter) {
		if (connected() && deviceStatus_) return;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	}
	throw std::runtime_error("conect to sim failed");
}

void Host::connectToDevice(const std::string &portName)
{
	link_.reset(new CDCLink(portName, impl_->ios_));

	device::ConnectMsg *cm = new device::ConnectMsg();
	link_->send(cm);
	int iter = 10;
	for (int iter = 0; iter<10; ++iter) {
		if (connected() && deviceStatus_) return;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	}
	link_->shutdown();
	std::string msg = "connect to device failed on " + portName;
	throw std::runtime_error(msg);
}

void Host::executeScript(const Script &s)
{
	if (!connected()) {
		throw std::runtime_error("Host::executeScript failed, device is not connected");
	}

	if (scriptRunning()) {
		throw std::runtime_error("Host::executeScript failed, device is not idle");
	}

	scriptMsgs_ = s.generateMessages();
	scriptMsgCount_ = static_cast<int>(scriptMsgs_->size());
	scriptMsgAckd_ = 0;

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
		} else {
			++i;
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
	if (scriptMsgCount_ == 0) {
		return false;
	} else if (scriptMsgAckd_ == scriptMsgCount_) {
		return ((deviceStatus_->statusFlags_ & device::StatusFlags::ENGINE_RUNNING) != 0);
	} else {
		return true;
	}
}

double Host::scriptProgress()
{
	return (double)scriptMsgAckd_ / (double)scriptMsgCount_;
}

void Host::move(int16_t dx, int16_t dy, int16_t dz, int16_t du, double duration)
{
	Script script;
	script.addLine(dx, dy, dz, du, duration);
	executeScript(script);

	int iter = 5 + (int)(duration * 10.);
	while (iter > 0 && scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		--iter;
	}

	if (iter == 0) {
		throw std::runtime_error("Host::move failed to complete in reasonable time");
	}
}

void Host::home(double stepDelay)
{
	Script script;

	// for now, hardcode home to go to lower left on both frames.
	const uint8_t HOME_MASK = stepper::device::StepDir::X_STEP | stepper::device::StepDir::Y_STEP | stepper::device::StepDir::Z_STEP | stepper::device::StepDir::U_STEP;
	stepper::device::StepDir s(HOME_MASK);

	script.addHome(s, stepDelay);

	executeScript(script);

	int iter = 60;
	while (iter > 0 && scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
		--iter;
	}

	if (iter == 0) {
		throw std::runtime_error("Host::home failed to complete in reasonable time");
	}
}

void Host::speedScaleFactor(double scale)
{
	if (scale < .5 || scale > 2.) {
		throw std::out_of_range("Host::speedScaleFactor scale parameter is out of range");
	}
	device::SpeedAdjustMsg *sam = device::allocateMessage<device::SpeedAdjustMsg>();
	sam->speedAdjust_ = (uint32_t)(scale * (double)(1 << device::SpeedAdjustMsg::UNITY_SPEED_ADJUST_SHIFT));
	device::ErrorCode ec = link_->send(sam);
	if (ec != device::SUCCESS) {
		throw std::runtime_error("Host::speedScaleFactor Unexpected send failure");
	}
}

void Host::pause()
{
	device::PauseMsg *pm = device::allocateMessage<device::PauseMsg>();
	device::ErrorCode ec = link_->send(pm);
	if (ec != device::SUCCESS) {
		throw std::runtime_error("Host::pause Unexpected send failure");
	}
}

void Host::resume()
{
	device::GoMsg *gm = device::allocateMessage<device::GoMsg>();
	device::ErrorCode ec = link_->send(gm);
	if (ec != device::SUCCESS) {
		throw std::runtime_error("Host::resume Unexpected send failure");
	}
}

void Host::abort()
{
	device::ConnectMsg *cm = device::allocateMessage<device::ConnectMsg>();
	device::ErrorCode ec = link_->send(cm);
	if (ec != device::SUCCESS) {
		throw std::runtime_error("Host::abort Unexpected send failure");
	}
}

void Host::runOnce(const boost::system::error_code& error)
{
	if (link_) {
		device::Message *m;
		while ((m = link_->receive()) != nullptr) {
			handleMessage(m);
		}
	}

	if (!impl_->ios_.stopped()) {
		impl_->timer_.expires_from_now(boost::posix_time::milliseconds(BACKGROUND_PERIOD_MSEC));
		impl_->timer_.async_wait(boost::bind(&Host::runOnce, this, boost::asio::placeholders::error));
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
		scriptMsgAckd_++;
		if (!scriptMsgs_->empty()) {
			Script::MessageCollection::auto_type dsm = scriptMsgs_->pop_front();
			device::ErrorCode ec = link_->send(dsm.release());
			if (ec != device::SUCCESS) {
				throw std::runtime_error("Host::Unexpected send failure with script message");
			}
		}
		delete m;
	}
	break;

	case device::HEARTBEAT_RESPONSE_MSG:
	{
		///\todo notify if status changes
		Logger::trace("host", "heartbeat response received");
		heartbeatCount_ = 0;
	}
	break;

	case device::STATUS_MSG:
	{
		Logger::trace("host", "device status received");
		boost::lock_guard<boost::mutex> guard(mtx_);
		device::StatusMsg *sm = static_cast<device::StatusMsg*>(m);
		deviceStatus_.reset(sm);
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

	case device::LIMIT_SWITCHES_MSG:
	{
		///\todo handle limit switches notification
	}
	break;

	case device::FATAL_ERROR_MSG:
	{
		device::FatalErrorMsg *fem = static_cast<device::FatalErrorMsg*>(m);
		std::ostringstream oss;
		int ec = (int)fem->fatalErrorCode_;
		oss << "Host::runOnce Fatal error " << ec <<
				" from device: " << device::FatalErrorMessage[ec];
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
			stepper::Logger::trace("host", "heartbeat sent");
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
