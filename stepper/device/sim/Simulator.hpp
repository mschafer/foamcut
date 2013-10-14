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
#include "ASIOLink.hpp"
#include "Stepper.hpp"

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

protected:
	virtual void notifySender();


private:
	boost::asio::io_service ios_;
	std::auto_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    boost::asio::ip::tcp::socket socket_;
	boost::asio::high_resolution_timer backgroundTimer_;
	boost::asio::high_resolution_timer stepTimer_;
	volatile std::atomic_bool running_;
	std::unique_ptr<boost::thread> thread_;
	uint16_t port_;

    typedef device::MessagePool<boost::mutex> pool_type;
    typedef device::ASIOLink<boost::asio::ip::tcp::socket , pool_type> link_type;
    std::unique_ptr<pool_type> pool_;
    std::unique_ptr<link_type> link_;

	int pos_[StepDir::AXIS_END];
	std::array<std::pair<int, int>, StepDir::AXIS_END> limit_;
	int time_;
	StepDir invertMask_;
	StepDir currentBits_;

	void runComm();
	void acceptComplete(const boost::system::error_code &error);

	void runBackground();
	void runTimer();
};

}}

#endif

