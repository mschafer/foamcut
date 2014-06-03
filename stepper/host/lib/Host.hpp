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
#include <boost/asio.hpp>
#include "Link.hpp"

namespace stepper {

class Script;

class TCPLink;

namespace device {
class Simulator;
}

class Host
{
public:

	enum State {
		NO_DEVICE,
		IDLE_DEVICE,
		ENGINE_RUNNING,
		ENGINE_PAUSED
	};

	Host();
	~Host();

	bool connectToSimulator();

	void executeScript(const Script &s);


private:
	std::unique_ptr<Link> link_;
	boost::asio::io_service ios_;
    std::unique_ptr<boost::thread> thread_;
    boost::mutex mtx_;
    boost::asio::io_service::work work_;
    boost::asio::deadline_timer timer_;
    State state_;
    std::atomic<size_t> pongCount_;

    void runOnce(const boost::system::error_code& error);
    bool ping();
};

}

#endif
