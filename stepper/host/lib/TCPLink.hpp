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
#ifndef stepper_TCPLink_hpp
#define stepper_TCPLink_hpp

#include <atomic>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <HAL.hpp>
#include <Message.hpp>

namespace stepper {

namespace device {
class ASIOReceiver;
class ASIOSender;
}

/**
 * TCP/IP host link to the device.
 */
class TCPLink
{
public:

	enum {
		TRY_CONNECT_TIMEOUT = 500
	};

	TCPLink(const char *hostName, uint16_t port);
	virtual ~TCPLink();

	device::HAL::Status send(device::Message *mb);

	device::Message *receive() {
		device::Message *ret = nullptr;
		if (!rxList_.empty()) {
			boost::lock_guard<boost::mutex> guard(mtx_);
			ret = rxList_.front();
			rxList_.pop_front();
		}
		return ret;
	}

private:
	typedef std::list<device::Message*> MessageList;

    std::string hostName_;
    std::string portStr_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<boost::thread> thread_;
    boost::mutex mtx_;
    std::unique_ptr<device::ASIOSender> sender_;
    std::unique_ptr<device::ASIOReceiver> receiver_;

    MessageList rxList_;

    void run();
    void connectComplete(const boost::system::error_code &error);

	void handleMessage(device::Message *message);
	void handleError(const boost::system::error_code &error);
};

}

#endif
