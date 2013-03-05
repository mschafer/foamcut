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
#ifndef foamcut_machine_hpp
#define foamcut_machine_hpp

#include <stdlib.h>
#include <stdint.h>

namespace foamcut {

/**
 * Container for parameters describing the foam cutting machine.
 */
struct Machine {
	Machine();

	double xStepSize_;
	double yStepSize_;
	double maxXSteps_;
	double maxYSteps_;
	uint32_t maxStepsPerSecond_;
	double frameSeparation_;

	static const float DEFAULT_STEP_SIZE;
	static const size_t DEFAULT_MAX_STEP_RATE;
	static const double DEFAULT_FRAME_SEPARATION;

};

}

#endif
