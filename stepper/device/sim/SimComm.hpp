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
#ifndef stepper_sim_SimComm_hpp
#define stepper_sim_SimComm_hpp

#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/utility.hpp>
#include "ASIOImpl.hpp"
#include <Communicator.hpp>

namespace stepper { namespace device {

class SimComm : public Communicator
{
public:
	SimComm(uint16_t port = 0);
	virtual ~SimComm();

	void initialize();
	void operator()();
	bool connected() const;

	uint16_t port() const { return port_; }
	boost::asio::io_service &ios() { return ios_; }

protected:
	void startSending();

private:
	friend class ASIOImpl<SimComm>;

	boost::asio::io_service ios_;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
	boost::asio::ip::tcp::socket socket_;
	std::unique_ptr<boost::thread> thread_;
	uint16_t port_;
	std::unique_ptr<ASIOImpl<SimComm> > impl_;
	std::atomic<bool> running_;

	void run();
	void acceptComplete(const boost::system::error_code &error);

	//ASIOImpl support
	boost::asio::ip::tcp::socket &socket() { return socket_; }
	void handler(DeviceMessage *msg, const boost::system::error_code &error);
	DeviceMessage *popTx() { return txQueue_.pop(); }
};

}}

#endif
