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
#include "SimCommunicator.hpp"
#include "ASIOSender.hpp"
#include "ASIOReceiver.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <iostream>
#include <Logger.hpp>

namespace stepper { namespace device {


SimCommunicator::SimCommunicator(boost::asio::io_service &ios, uint16_t port) :
		ios_(ios), socket_(ios_), port_(port)
{
}

SimCommunicator::~SimCommunicator()
{
	if (socket_.is_open()) {
		socket_.shutdown(boost::asio::socket_base::shutdown_both);
	}
}

void SimCommunicator::initialize()
{
    using boost::asio::ip::tcp;
    if (port_ != 0) {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint(tcp::v4(), port_)));
    } else {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint()));
        port_ = acceptor_->local_endpoint().port();
    }

    acceptor_->async_accept(socket_,
    boost::bind(&SimCommunicator::acceptComplete, this,
        boost::asio::placeholders::error));
}

void SimCommunicator::shutdown()
{
	if (socket_.is_open()) {
		socket_.shutdown(boost::asio::socket_base::shutdown_both);
	}
	sender_.reset();
	receiver_.reset();
}

uint16_t SimCommunicator::port() const
{
    return port_;
}

bool SimCommunicator::ready() const
{
	return (bool)(sender_);
}

Message *SimCommunicator::receiveMessage()
{
	if (receiver_) {
    	return receiver_->getMessage();
	} else {
		return nullptr;
	}
}

void SimCommunicator::acceptComplete(const boost::system::error_code &error)
{
    if (!error) {
		ASIOSender<boost::asio::ip::tcp::socket>::ErrorCallback ec = boost::bind(&SimCommunicator::handleError, this, _1);
		sender_.reset(new ASIOSender<boost::asio::ip::tcp::socket>(socket_, ec));
		receiver_.reset(new ASIOReceiver<boost::asio::ip::tcp::socket>(socket_, ec));
    } else {
    	handleError(error);
    }
}

ErrorCode SimCommunicator::sendMessage(Message *message, Message::Priority priority)
{
	ErrorCode ec;
	if (sender_.get() != nullptr) {
		ec = sender_->enqueue(message);
	} else {
		ec = RESOURCE_UNAVAILABLE;
	}
	if (ec != SUCCESS) {
		Logger::warning("sim", "failed to send message");
	}
	return ec;
}

void SimCommunicator::handleError(const boost::system::error_code &error)
{
	if (socket_.is_open()) {
		socket_.shutdown(boost::asio::socket_base::shutdown_both);
	}
	socket_.close();
    acceptor_->async_accept(socket_,
    boost::bind(&SimCommunicator::acceptComplete, this,
        boost::asio::placeholders::error));
}

}}
