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
#include "HostMessages.hpp"

namespace stepper {

template <typename link_type> class ASIOImpl;

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

	void send(Message *mb);
	Message *receive() { return rxQueue_.pop(); }

	bool connected() const { return connected_; }

private:
	friend class ASIOImpl<TCPLink>;
    std::string hostName_;
    std::string portStr_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<boost::thread> thread_;
    boost::mutex mtx_;
    std::atomic<bool> running_;
    std::atomic<bool> connected_;

    MessageList txList_;
    MessageList rxList_;

    std::unique_ptr<ASIOImpl<TCPLink> > impl_;

    void run();
    void connectComplete(const boost::system::error_code &error);

	boost::asio::ip::tcp::socket &socket() { return socket_; }
	boost::asio::io_service &ios() { return ios_; }
	void handler(Message *msg, const boost::system::error_code &error);
	Message *popTx() {
		Message *ret = nullptr;
		if (!txList_.empty()) {
			boost::lock_guard<mutex> guard(mtx_);
			ret = txList_.front();
			txList_.pop_front();
		}
		return ret;
};

}

#endif
