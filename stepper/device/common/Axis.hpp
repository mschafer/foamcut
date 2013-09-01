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
#ifndef stepper_device_Axis_hpp
#define stepper_device_Axis_hpp

#include "Pin.hpp"

namespace stepper { namespace device {

struct Axis
{
	Pin step_;
	Pin dir_;
	Pin fwdLimit_;
	Pin revLimit_;
};

}}

#endif
