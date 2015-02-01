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
#include <deque>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <StepDir.hpp>
#include <HAL.hpp>

namespace stepper { namespace device {

struct Position
{
	Position() : pos_({ 0, 0, 0, 0 }), time_(0.) {}
	std::array<int, StepDir::AXIS_COUNT> pos_;
	double time_{ 0. };
};

struct AxisLimit
{
	int low_;
	int high_;
};

class SimCommunicator;

class Simulator
{
public:
	typedef std::array<AxisLimit, StepDir::AXIS_COUNT> LimitType;

	enum {
		X_LOWER_LIMIT_DEFAULT = 0,
		X_UPPER_LIMIT_DEFAULT = 30000,
		Y_LOWER_LIMIT_DEFAULT = 0,
		Y_UPPER_LIMIT_DEFAULT = 30000
	};

	Simulator(uint16_t port=0);
	~Simulator();

	static Simulator &instance();

	/// \return tcp/ip port used by the simulator
	uint16_t port() const;

	boost::mutex &positionMutex() { return posLock_; }
	const Position &position() const { return position_; }
	const std::deque<Position> &positionLog() const { return posLog_; }

	/** Clears the position history and resets the current position to 0,0. */
	void clearLog() { posLog_.resize(1); }

	LimitType limits() const { return limit_; }
	void limits(LimitType &l) { limit_ = l; }

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
	Position position_;
	std::deque<Position> posLog_;
	boost::mutex posLock_;
	LimitType limit_;
	double time_{ 0. };

	void runOnce(const boost::system::error_code &ec);
	void stepTimerExpired(const boost::system::error_code &ec);

};

}}

#endif

