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
#ifndef stepper_CDCLink_hpp
#define stepper_CDCLink_hpp

#include <atomic>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Link.hpp"

namespace stepper {

namespace device {
	template <typename T> class ASIOSender;
	template <typename T> class ASIOReceiver;
}

/**
 * TCP/IP host link to the device.
 */
class CDCLink : public Link
{
public:
	explicit CDCLink(const std::string &portName, boost::asio::io_service &ios);
	virtual ~CDCLink();

	virtual device::ErrorCode send(device::Message *mb);

	virtual device::Message *receive();

	virtual void shutdown();

private:
	enum {
		TRY_CONNECT_TIMEOUT = 500
	};

	std::string portName_;
    boost::asio::io_service &ios_;
    boost::mutex mtx_;
    boost::asio::serial_port port_;
	std::unique_ptr<device::ASIOSender<boost::asio::serial_port> > sender_;
	std::unique_ptr<device::ASIOReceiver<boost::asio::serial_port> > receiver_;

	void handleError(const boost::system::error_code &error);
};

}

#endif
