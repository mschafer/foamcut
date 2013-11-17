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

#include "Machine.hpp"

namespace stepper { namespace device {

Machine::Machine()
{
	for(int i=0; i<StepDir::AXIS_COUNT; ++i) {
		pos_[i] = 0;
		limit_[i].first = std::numeric_limits<int>::min();
		limit_[i].second = std::numeric_limits<int>::max();
	}
}

void Machine::setStepDir(const StepDir &s)
{
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx ai = static_cast<StepDir::AxisIdx>(i);

		// set direction bits
		currentBits_.dir(ai, s.dir(ai));

		// was there an edge on a step bit?
		if (s.step(ai) != currentBits_.step(ai)) {
			currentBits_.step(ai, s.step(ai));
			// was the edge in the right direction to cause a step?
			if (currentBits_.step(ai) != invertMask_.step(ai)) {
				int np = pos_[i] + ((invertMask_.dir(ai) == currentBits_.dir(ai)) ? -1 : 1);

				// apply limits
				if (np >= limit_[i].first && np <= limit_[i].second) {
					pos_[i] = np;
				}
			}
		}
	}
}

LimitSwitches Machine::readLimitSwitches()
{
	LimitSwitches ret;
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx idx = static_cast<StepDir::AxisIdx>(i);
		bool test = (pos_[i] <= limit_[i].first);
		ret.reverseLimit(idx, test);
		test = (pos_[i] >= limit_[i].second);
		ret.forwardLimit(idx, test);
	}
	return ret;
}


}}
