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
#include "stepper_info.hpp"
#include <limits>
#include <cstddef>

namespace foamcut {

const double StepperInfo::DEFAULT_STEP_SIZE = .001;
const size_t StepperInfo::DEFAULT_MAX_STEP_RATE = 1000;
const double StepperInfo::DEFAULT_FRAME_SEPARATION = 30.;


StepperInfo::StepperInfo() : xStepSize_(DEFAULT_STEP_SIZE), yStepSize_(DEFAULT_STEP_SIZE),
		xLength_(std::numeric_limits<double>::max()), yLength_(std::numeric_limits<double>::max()),
		maxStepsPerSecond_(DEFAULT_MAX_STEP_RATE), frameSeparation_(DEFAULT_FRAME_SEPARATION)
{
}

}
