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
#include "Link.hpp"

namespace stepper {

namespace device {
class ASIOReceiver;
class ASIOSender;
}

/**
 * TCP/IP host link to the device.
 */
class TCPLink : public Link
{
public:
	explicit TCPLink(const char *hostName, uint16_t port, boost::asio::io_service &ios);
	virtual ~TCPLink();

	virtual device::ErrorCode send(device::Message *mb);

	virtual device::Message *receive();

private:
	enum {
		TRY_CONNECT_TIMEOUT = 500
	};

	std::string hostName_;
    std::string portStr_;
    boost::asio::io_service &ios_;
    boost::mutex mtx_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<device::ASIOSender> sender_;
    std::unique_ptr<device::ASIOReceiver> receiver_;

    void connect();
    void connectComplete(const boost::system::error_code &error);
	void handleError(const boost::system::error_code &error);
};

}

#endif
