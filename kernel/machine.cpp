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
#include "machine.hpp"
#include <limits>
#include <cstddef>

namespace foamcut {

const double Machine::DEFAULT_STEP_SIZE = .001;
const size_t Machine::DEFAULT_MAX_STEP_RATE = 1000;
const double Machine::DEFAULT_FRAME_SEPARATION = 30.;


Machine::Machine() : xStepSize_(DEFAULT_STEP_SIZE), yStepSize_(DEFAULT_STEP_SIZE),
		maxXSteps_(std::numeric_limits<double>::max()), maxYSteps_(std::numeric_limits<double>::max()),
		maxStepsPerSecond_(DEFAULT_MAX_STEP_RATE), frameSeparation_(DEFAULT_FRAME_SEPARATION)
{
}

}
