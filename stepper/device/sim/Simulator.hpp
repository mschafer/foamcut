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

#include <memory>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <HALBase.hpp>

namespace stepper { namespace device {

class SimCommunicator;

class Simulator : public HALBase<Simulator>
{
public:
	Simulator(uint16_t port=0);
	~Simulator();

	// HAL emulation
	void initialize();
	static void setStepDirBits(const StepDir &s);
	static LimitSwitches readLimitSwitches();
	static Status sendMessage(Message *m, Priority priority = NORMAL_PRIORITY);
	static Message *receiveMessage();
	static void startTimer(uint32_t period);
	static void stopTimer();
	static void reset();

	static Simulator &instance();

	uint16_t port() const;

private:
	static std::unique_ptr<Simulator> theSim_;

	std::unique_ptr<SimCommunicator> comm_;
	std::unique_ptr<boost::thread> thread_;
	boost::asio::io_service ios_;
	boost::asio::deadline_timer backgroundTimer_;
	boost::asio::deadline_timer stepTimer_;
	StepDir invertMask_;
	StepDir currentBits_;
	int pos_[StepDir::AXIS_COUNT];
	std::array<std::pair<int, int>, StepDir::AXIS_COUNT> limit_;

	void runOnce(const boost::system::error_code &ec);
	void stepTimerExpired(const boost::system::error_code &ec);


};

}}

#endif

