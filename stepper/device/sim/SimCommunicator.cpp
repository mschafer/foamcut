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

namespace stepper { namespace device {


SimCommunicator::SimCommunicator(uint16_t port) : socket_(ios_), port_(port)
{
}

SimCommunicator::~SimCommunicator()
{
	if (thread_->joinable()) {
		thread_->interrupt();
	}
    ios_.stop();
    thread_->join();
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

    thread_.reset(new boost::thread(boost::bind(&SimCommunicator::run, this)));
}

uint16_t SimCommunicator::port() const
{
    return port_;
}

void SimCommunicator::run()
{
    using namespace boost::asio::ip;
    try {
        while (1) {
            boost::this_thread::interruption_point();
            acceptor_->async_accept(socket_,
            boost::bind(&SimCommunicator::acceptComplete, this,
                boost::asio::placeholders::error));
            ios_.run();
            ios_.reset();
        }
    }

    catch (boost::thread_interrupted &/*intex*/) {
    }
    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
        std::cerr << ex.what();
    }
    catch(...) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
    }
}

void SimCommunicator::acceptComplete(const boost::system::error_code &error)
{
    if (!error) {
        ASIOSender::ErrorCallback ec = boost::bind(&SimCommunicator::handleError, this, _1);
        ASIOReceiver::Handler hm = boost::bind(&SimCommunicator::handleMessage, this, _1);
        sender_.reset(new ASIOSender(socket_, ec));
        receiver_.reset(new ASIOReceiver(socket_, hm, ec));
    } else {
    	handleError(error);
    }
}

HAL::Status SimCommunicator::sendMessage(Message *message, HAL::Priority priority)
{
	if (sender_.get() != nullptr) {
		return sender_->enqueue(message);
	} else {
		return HAL::RESOURCE_UNAVAILABLE;
	}
}

void SimCommunicator::handleError(const boost::system::error_code &error)
{
    socket_.close();
}

void SimCommunicator::handleMessage(Message *message)
{
	///\todo implement me
}

}}
