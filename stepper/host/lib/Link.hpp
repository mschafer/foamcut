/*
 * (C) Copyright 2014 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#ifndef stepper_Link_hpp
#define stepper_Link_hpp

#include <boost/utility.hpp>
#include <HAL.hpp>
#include <Message.hpp>

namespace stepper {

/** Base class for communications link to device. */
class Link : public boost::noncopyable
{
public:
	Link() {}
	virtual ~Link() {}

	virtual device::HAL::Status send(device::Message *mb) = 0;

	virtual device::Message *receive() = 0;


};

}

#endif
