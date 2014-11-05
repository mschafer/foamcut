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

#include <StepDir.hpp>

namespace foamcut {

struct StepperInfo {
public:
	double xStepSize_;
	double yStepSize_;
	double xLength_;
	double yLength_;
	double frameSeparation_;
	int maxStepsPerSecond_;
	stepper::device::StepDir invertMask_;
};

}

#endif
