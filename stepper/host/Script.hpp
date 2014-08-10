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
#ifndef stepper_Script_hpp
#define stepper_Script_hpp

#include <deque>
#include <StepDir.hpp>
#include <StepperDictionary.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/shared_ptr.hpp>

namespace stepper {

/**
 * A Script contains commands for the stepper engine to execute.
 * This class builds up a script from individual commands and then
 * creates messages that can be sent to the device for execution.
 */
class Script
{
public:
	typedef boost::ptr_deque<device::Message> MessageCollection;
	typedef std::shared_ptr<Script> handle;

	Script();
	~Script();

	/**
	 * Add a single step to the script.
	 * \param s The step and direction information.
	 * \param delay The delay in seconds after this step before the next one.
	 */
	void addStep(device::StepDir s, double delaySec);

	/**
	 * Add a linear move to the script relative to the current position.
	 * \param dx The number of x axis steps.
	 * \param dy The number of y axis steps.
	 * \param dz The number of z axis steps.
	 * \param du The number of u axis steps.
	 * \param duration The time in seconds to spend making this linear move.  It will be
	 * divided equally across all the individual steps.
	 */
	void addLine(int16_t dx, int16_t dy, int16_t dz, int16_t du, double duration);

	/**
	 * Add a delay to the script.
	 * \param duration The time in seconds to pause execution.
	 */
	void addDelay(double duration);

	/**
	 * Converts the script to a collection of Messages that can be sent to the device.
	 */
	std::unique_ptr<MessageCollection> generateMessages() const;

	/**
	 * \return The expected time it will take this script to run in seconds.
	 */
	double duration() const { return duration_;  }

private:
	std::deque<uint8_t> bytes_;
	double duration_;

};

}

#endif
