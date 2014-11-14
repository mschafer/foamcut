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
#include <boost/thread.hpp>
#include "Link.hpp"
#include "Script.hpp"

///\todo use pimpl to move boost asio includes out of this header and avoid Winsock conflicts

namespace stepper {

namespace device {
class Simulator;
}

class Host
{
public:
	Host();
	~Host();

	/**
	 * Connect to the local device simulator.
	 * Sends disconnect and discards the current connection.
	 * Any running script will be aborted.
	 * If currently connected to the simulator, then the sim will be reset.
	 */
	void connectToSimulator();

	/**
	 * Initiates execution of a script on the device.
	 * Does not block.
	 * throws if scriptRunning already returns true.
	 */
	void executeScript(const Script &s);

	bool connected() { return connected_; }
	bool scriptRunning();

	/** 
	 * \return fraction complete of currently executing script.
	 */
	double scriptProgress();

	/**
	 * Instructs the device to take \em count steps in the direction
	 * specified by \em s.
	 * Blocks until the move is complete.
	 * throws if scriptRunning already returns true
	 */
	void move(int16_t dx, int16_t dy, int16_t dz, int16_t du, double duration);

	/**
	 * Instructs the device to run continuously in the direction specified by \em s
	 * until all limit switches are activated.
	 * Blocks until the home is complete.
	 * throws if scriptRunning already returns true
	 */
	void home();

	/**
	 * Scale the set speed of the device.
	 * \param scale The multiplier used for the device speed which must be
	 * in the range .5 < scale < 2.
	 */
	void speedScaleFactor(double scale);

	void pause();

	void resume();

	void abort();

private:
	enum {
		BACKGROUND_PERIOD_MSEC = 20,
		HEARTBEAT_PERIOD_MSEC = 500,
		HEARTBEAT_DISCONNECT_COUNT = 5
	};

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
    std::unique_ptr<device::StatusMsg> deviceStatus_;
    std::atomic<bool> connected_;
    std::atomic<int> scriptMsgCount_;
	std::atomic<int> scriptMsgAckd_;

    void runOnce(const boost::system::error_code& error);
    void handleMessage(device::Message *m);
    void heartbeat();
    bool ping();
};

}

#endif
