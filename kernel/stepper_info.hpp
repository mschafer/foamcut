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
#ifndef foamcut_stepper_info_hpp
#define foamcut_stepper_info_hpp

#include <stdlib.h>
#include <stdint.h>

namespace foamcut {

/**
 * Describes the physical characteristics of the stepper.
 */
struct StepperInfo {
	StepperInfo();

	double xStepSize_; ///\< Step size for the horizontal axis.
	double yStepSize_; ///\< Step size for the vertical axis.
	double xLength_;   ///\< Length of the foam cutter horizontal axis.
	double yLength_;   ///\< Length of the foam cutter vertical axis.
	uint32_t maxStepsPerSecond_;  ///\< Maximum rate for the stepper motors before they start missing steps.
	double frameSeparation_; ///\< Distance between the left and right frames of the foam cutter.

	static const double DEFAULT_STEP_SIZE;
	static const size_t DEFAULT_MAX_STEP_RATE;
	static const double DEFAULT_FRAME_SEPARATION;

};

}

#endif
