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
#ifndef stepper_device_Machine_hpp
#define stepper_device_Machine_hpp

#include <StepDir.hpp>
#include <LimitSwitches.hpp>
#include <array>

namespace stepper { namespace device {

class Machine {
public:
	Machine();
	~Machine() {}

	void setStepDir(const StepDir &);
	LimitSwitches readLimitSwitches();


	StepDir invertMask_;
	StepDir currentBits_;
	int pos_[StepDir::AXIS_COUNT];
	std::array<std::pair<int, int>, StepDir::AXIS_COUNT> limit_;
};

}}

#endif
