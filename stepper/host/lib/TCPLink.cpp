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

TCPLink::TCPLink(const char *hostName, uint16_t port) :
	hostName_(hostName), socket_(ios_)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
    thread_.reset(new boost::thread(boost::bind(&TCPLink::run, this)));
}

TCPLink::~TCPLink()
{
	try {
		thread_->interrupt();
		ios_.stop();
		thread_->join();
	} catch (...) {
		///\todo ?
	}
}

device::HAL::Status TCPLink::send(device::Message *m)
{
	if (sender_.get() != nullptr) {
		return sender_->enqueue(m);
	} else {
		return device::HAL::RESOURCE_UNAVAILABLE;
	}
}

device::Message *TCPLink::receive() {
	if (receiver_) {
		return receiver_->getMessage();
	} else {
		return nullptr;
	}
}

void TCPLink::run()
{
    using namespace boost::asio::ip;
    try {
        while (1) {
        	boost::this_thread::interruption_point();
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
                ios_.run();
                ios_.reset();
                sender_.reset();
                receiver_.reset();
            }
        	boost::this_thread::interruption_point();
            boost::posix_time::time_duration d = boost::posix_time::milliseconds(TRY_CONNECT_TIMEOUT);
            boost::this_thread::sleep(d);
        }
    }

    catch(boost::thread_interrupted &/*intex*/) {

    }
    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip client thread dying" << std::endl;
        std::cerr << ex.what();
    }
    catch(...) {
        std::cerr << "Exception caught, tcp/ip client thread dying" << std::endl;
    }
}

void TCPLink::handleError(const boost::system::error_code &error)
{
    socket_.close();
}

void TCPLink::connectComplete(const boost::system::error_code &error)
{
    if (!error) {
        device::ASIOSender::ErrorCallback ec = boost::bind(&TCPLink::handleError, this, _1);
        sender_.reset(new device::ASIOSender(socket_, ec));
        receiver_.reset(new device::ASIOReceiver(socket_, ec));
    } else {
    	handleError(error);
    }
}

}
