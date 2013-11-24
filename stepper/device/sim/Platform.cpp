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
#include <memory>
#include "Platform.hpp"

namespace stepper { namespace device {


std::unique_ptr<MemoryAllocator_type> theAllocator;
std::unique_ptr<Communicator_type> theCommunicator;

MemoryAllocator_type *getMemoryAllocator()
{
	static size_t poolSizes[2] = {64, 512};
	static uint8_t block[10240];

	if (theAllocator.get() == nullptr) {
		theAllocator.reset(new MemoryAllocator_type(poolSizes, block, sizeof(block)));
	}
	return *(theAllocator.get());
}


Communicator_type *getCommunicator()
{
	if (theCommunicator.get() == nullptr) {
		theCommunicator.reset(new Communicator_type());
	}
	return *(theCommunicator.get());
}

}}
