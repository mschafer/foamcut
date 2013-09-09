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
#ifndef stepper_device_Stepper_hpp
#define stepper_device_Stepper_hpp

#include "foamcut_stepper_dll.h"
#include "StepDir.hpp"

namespace stepper { namespace device {

class foamcut_stepper_API Stepper
{
public:
	Stepper &instance();

	/**
	 * This method should be called by the Platform when the timer expires.
	 */
	void run();

	/**
	 * Specify bits in \sa StepDir that need to be inverted.
	 * Inverting a step bit will result in the generation of a falling edge
	 * whenever a step is desired instead of a rising edge.
	 * Inverting a direction bit will cause forward steps to output a logical
	 * 0 on the direction pin instead of a 1.
	 */
	void invertMask(StepDir invertMask) { invertMask_ = invertMask; }


private:
	Stepper();
	Stepper(const Stepper &cpy);
	Stepper &operator=(const Stepper &rhs);

	StepDir invertMask_;
};

}}

#endif


