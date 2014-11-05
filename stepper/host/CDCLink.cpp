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
#include "CDCLink.hpp"
#include <ASIOReceiver.hpp>
#include <ASIOSender.hpp>

namespace stepper {

CDCLink::CDCLink(const std::string &portName, boost::asio::io_service &ios) :
	portName_(portName), ios_(ios), port_(ios)
{
	port_.open(portName_);

	device::ASIOSender<boost::asio::serial_port>::ErrorCallback ec = boost::bind(&CDCLink::handleError, this, _1);
	sender_.reset(new device::ASIOSender<boost::asio::serial_port>(port_, ec));
	receiver_.reset(new device::ASIOReceiver<boost::asio::serial_port>(port_, ec));
}

CDCLink::~CDCLink()
{
	port_.close();
}

device::ErrorCode CDCLink::send(device::Message *m)
{
	if (sender_.get() != nullptr) {
		return sender_->enqueue(m);
	} else {
		return device::RESOURCE_UNAVAILABLE;
	}
}

device::Message *CDCLink::receive() {
	if (receiver_) {
		return receiver_->getMessage();
	} else {
		return nullptr;
	}
}


void CDCLink::handleError(const boost::system::error_code &error)
{
    port_.close();
    sender_.reset();
    receiver_.reset();
}

}
