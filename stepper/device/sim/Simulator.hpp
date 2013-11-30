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
#include <boost/asio/deadline_timer.hpp>
#include <boost/utility.hpp>
#include <Stepper.hpp>
#include "Machine.hpp"

namespace boost {
class thread;
}

namespace stepper { namespace device {

class Simulator : public Stepper
{
public:
	Simulator();
	virtual ~Simulator();

	virtual void initialize();

	virtual void setStepDirBits(const StepDir &s);

	virtual LimitSwitches readLimitSwitches();

	virtual void startTimer(uint32_t period);


private:
	std::unique_ptr<boost::asio::deadline_timer> backgroundTimer_;
	std::unique_ptr<boost::asio::deadline_timer> stepTimer_;
	std::atomic<bool> running_;
    Machine machine_;

    void runBackground();
	void runTimer();
};

}}

#endif

