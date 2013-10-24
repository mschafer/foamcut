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
#ifndef stepper_device_Line_hpp
#define stepper_device_Line_hpp

#include "StepDir.hpp"

namespace stepper { namespace device {

class Line
{
public:

	struct NextStep {
		NextStep(uint32_t delay, StepDir step) : delay_(delay), step_(step) {}
		NextStep() : delay_(0) {}
		uint32_t delay_;
		StepDir step_;
	};

    Line();
	Line(int dx, int dy, int dz, int du, uint32_t delay);

	void reset(int dx, int dy, int dz, int du, uint32_t delay);
	NextStep nextStep();
	bool done() const { return count_ == maxd_; }

private:
	int absd_[5];
	int e_[5];
	int maxd_;
	int count_;
    uint32_t stepDelay_;
    StepDir dir_;

};

}}

#endif
