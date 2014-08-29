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
#include <StepDir.hpp>
#include <HAL.hpp>

namespace stepper { namespace device {

class SimCommunicator;

class Simulator
{
public:
	typedef std::array<int, StepDir::AXIS_COUNT> Position;

	Simulator(uint16_t port=0);
	~Simulator();

	static Simulator &instance();

	uint16_t port() const;
	const std::array<int, StepDir::AXIS_COUNT> &position() {
		return pos_;
	}
	double time() const { return time_; }

	static void reset();

private:
	friend void HAL::setStepDirBits(const StepDir &);
	friend LimitSwitches HAL::readLimitSwitches();
	friend ErrorCode HAL::sendMessage(Message *, Message::Priority);
	friend Message *HAL::receiveMessage();
	friend void HAL::startTimer(uint32_t);
	friend void HAL::stopTimer();

	static std::unique_ptr<Simulator> theSim_;

	boost::asio::io_service ios_;
	std::unique_ptr<boost::thread> thread_;
	boost::asio::deadline_timer backgroundTimer_;
	boost::asio::deadline_timer stepTimer_;
	std::unique_ptr<SimCommunicator> comm_;
	StepDir invertMask_;
	StepDir currentBits_;
	Position pos_;
	double time_;
	std::array<std::pair<int, int>, StepDir::AXIS_COUNT> limit_;

	void runOnce(const boost::system::error_code &ec);
	void stepTimerExpired(const boost::system::error_code &ec);

};

}}

#endif

