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
#include "TCPLink.hpp"
#include <ASIOReceiver.hpp>
#include <ASIOSender.hpp>

namespace stepper {

TCPLink::TCPLink(const char *hostName, uint16_t port, boost::asio::io_service &ios) :
	hostName_(hostName), ios_(ios), socket_(ios)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
    connect();
}

TCPLink::~TCPLink()
{
}

device::ErrorCode TCPLink::send(device::Message *m)
{
	if (sender_.get() != nullptr) {
		return sender_->enqueue(m);
	} else {
		return device::RESOURCE_UNAVAILABLE;
	}
}

device::Message *TCPLink::receive() {
	if (receiver_) {
		return receiver_->getMessage();
	} else {
		return nullptr;
	}
}

void TCPLink::connect()
{
    using namespace boost::asio::ip;

    tcp::resolver resolver(ios_);
    tcp::resolver::query query(tcp::v4(), hostName_.c_str(), portStr_.c_str());
    boost::system::error_code error;
    tcp::resolver::iterator it = resolver.resolve(query, error);
    if (error || it == tcp::resolver::iterator()) {
    	throw std::runtime_error("TCPLink : resolve failed");
    } else {
        boost::asio::async_connect(socket_, it,
        		boost::bind(&TCPLink::connectComplete, this,
        				boost::asio::placeholders::error));
    }
}

void TCPLink::handleError(const boost::system::error_code &error)
{
    socket_.close();
    sender_.reset();
    receiver_.reset();
    if (!ios_.stopped()) {
    	connect();
    }
}

void TCPLink::connectComplete(const boost::system::error_code &error)
{
    if (!error) {
		device::ASIOSender<boost::asio::ip::tcp::socket>::ErrorCallback ec = boost::bind(&TCPLink::handleError, this, _1);
		sender_.reset(new device::ASIOSender<boost::asio::ip::tcp::socket>(socket_, ec));
		receiver_.reset(new device::ASIOReceiver<boost::asio::ip::tcp::socket>(socket_, ec));
    } else {
    	handleError(error);
    }
}

}
