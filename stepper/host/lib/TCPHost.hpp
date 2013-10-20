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
#ifndef stepper_TCPHost_hpp
#define stepper_TCPHost_hpp

#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "ASIOImpl.hpp"
#include "MessagePool.hpp"

namespace stepper {

/**
 * TCP/IP host link to the device.
 */
class TCPHost
{
public:
	enum {
		TRY_CONNECT_TIMEOUT = 500
	};

	TCPHost(const char *hostName, uint16_t port);
	virtual ~TCPHost();

	device::MessageBuffer *alloc(uint16_t payloadSize) { return pool_->alloc(payloadSize); }
	void free(device::MessageBuffer *mb) { pool_->free(mb); }

	void send(device::MessageBuffer *mb);
	device::MessageBuffer *receive();

private:
	friend class device::ASIOImpl<TCPHost>;
    std::string hostName_;
    std::string portStr_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<boost::thread> thread_;
    volatile bool running_;

    typedef device::MessagePool<boost::mutex> pool_type;
    std::unique_ptr<pool_type> pool_;
	device::MessageQueue<boost::mutex> rxQueue_;
	device::MessageQueue<boost::mutex> txQueue_;

    std::unique_ptr<device::ASIOImpl<TCPHost> > impl_;

    void run();
    void connectComplete(const boost::system::error_code &error);

	boost::asio::ip::tcp::socket &socket() { return socket_; }
	boost::asio::io_service &ios() { return ios_; }
	void handler(device::MessageBuffer *msg, const boost::system::error_code &error);
	device::MessageBuffer *popTx() { return txQueue_.pop(); }
};

}

#endif
