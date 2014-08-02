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
#ifndef stepper_Host_hpp
#define stepper_Host_hpp

#include <memory>
#include <atomic>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include "Link.hpp"
#include "Script.hpp"

namespace stepper {

namespace device {
class Simulator;
}

class Host
{
public:

	enum {
		BACKGROUND_PERIOD_MSEC = 20,
		HEARTBEAT_PERIOD_MSEC = 500,
		HEARTBEAT_DISCONNECT_COUNT = 5
	};

	Host();
	~Host();

	bool connectToSimulator();

	void executeScript(const Script &s);
	bool connected() { return connected_; }
	bool scriptRunning();

private:
	boost::asio::io_service ios_;
    std::unique_ptr<boost::thread> thread_;
	std::unique_ptr<Link> link_;
    boost::mutex mtx_;
    boost::asio::io_service::work work_;
    boost::asio::deadline_timer timer_;
    std::atomic<size_t> pongCount_;
    std::unique_ptr<Script::MessageCollection> scriptMsgs_;
    boost::chrono::time_point<boost::chrono::steady_clock> heartbeatTime_;
    std::atomic<size_t> heartbeatCount_;
    std::unique_ptr<device::ConnectResponseMsg> connectResponse_;
    std::unique_ptr<device::HeartbeatResponseMsg> heartbeatResponse_;
    std::atomic<bool> connected_;
	boost::chrono::time_point<boost::chrono::steady_clock> scriptDoneTime_;

    void runOnce(const boost::system::error_code& error);
    void handleMessage(device::Message *m);
    void heartbeat();
    bool ping();
};

}

#endif
