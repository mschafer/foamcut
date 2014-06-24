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
#include "Line.hpp"
#include <stdlib.h>

namespace stepper { namespace device {

Line::Line()
{
	reset();
}

Line::Line(int dx, int dy, int dz, int du, uint32_t delay)
{
	reset(dx, dy, dz, du, delay);
}

void
Line::reset()
{
	maxd_ = 0;
	stepDelay_ = 0;
	count_ = 0;
	for (int i=0; i<5; ++i) {
    	absd_[i] = 0;
    	e_[i] = 0;
    }
}

void
Line::reset(int dx, int dy, int dz, int du, uint32_t delay)
{
	count_ = 0;
	absd_[StepDir::X_AXIS] = abs(dx);
	absd_[StepDir::Y_AXIS] = abs(dy);
	absd_[StepDir::Z_AXIS] = abs(dz);
	absd_[StepDir::U_AXIS] = abs(du);

    dir_.clear();
    if (dx > 0) dir_.xDir(true);
    if (dy > 0) dir_.yDir(true);
    if (dz > 0) dir_.zDir(true);
    if (du > 0) dir_.uDir(true);

    maxd_ = 0;
    for (int i=0; i<4; ++i) {
    	maxd_ = absd_[i] > maxd_ ? absd_[i] : maxd_;
    	e_[i] = 0;
    }
    // dither delay between steps to get exactly the right delay
    if (maxd_ != 0) {
    	stepDelay_ = delay / maxd_;
    } else {
    	stepDelay_ = delay;
    }
    absd_[4] = delay - (stepDelay_ * maxd_);
    e_[4] = 0;
}

Line::NextStep
Line::nextStep()
{
	NextStep ret;
	ret.step_ = dir_;
	ret.delay_ = stepDelay_;
	if (done()) return ret;

	// loop over axes (no foreach for enums yet)
	for (int i=0; i<5; ++i) {
		e_[i] += 2 * absd_[i];
		if (e_[i] > maxd_) {
			e_[i] -= 2 * maxd_;

			// time handled differently than axes
			if (i<4) {
				ret.step_.step((StepDir::AxisIdx)i, true);
			} else {
				ret.delay_ += 1;
			}
		}
	}
    ++count_;
	return ret;
}


}}
