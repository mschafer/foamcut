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
#ifndef stepper_device_Simulator_hpp
#define stepper_device_Simulator_hpp

#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/utility.hpp>
#include <Stepper.hpp>
#include "ASIOImpl.hpp"
#include "Machine.hpp"

namespace boost {
class thread;
}

namespace stepper { namespace device {

class Simulator : public Stepper
{
public:
	Simulator(uint16_t port=0);
	virtual ~Simulator();

	virtual void setStepDirBits(const StepDir &s);

	virtual LimitSwitches readLimitSwitches();

	virtual void startTimer(uint32_t period);

	uint16_t port() const { return port_; }

//protected:
	virtual void notifySender();
	virtual void pollForMessages();


//private:
	friend class ASIOImpl<Simulator>;
	boost::asio::io_service ios_;
	std::auto_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    boost::asio::ip::tcp::socket socket_;
	boost::asio::deadline_timer backgroundTimer_;
	boost::asio::deadline_timer stepTimer_;
	volatile bool running_;
	std::unique_ptr<boost::thread> thread_;
	uint16_t port_;

    std::unique_ptr<ASIOImpl<Simulator> > impl_;

    Machine machine_;

	void runComm();
	void acceptComplete(const boost::system::error_code &error);

	void runBackground();
	void runTimer();

	//ASIOImpl support
	boost::asio::ip::tcp::socket &socket() { return socket_; }
	boost::asio::io_service &ios() { return ios_; }
	void handler(DeviceMessage *msg, const boost::system::error_code &error);
	DeviceMessage *popTx();
};

}}

#endif

