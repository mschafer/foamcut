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

#include "Platform.hpp"
#include <memory>
#include "SimComm.hpp"
#include <MemoryPool.hpp>


namespace stepper { namespace device { namespace platform {


std::unique_ptr<MemoryPool_type> thePool;
std::unique_ptr<Communicator> theCommunicator;

MemoryPool_type &getMemoryPool()
{
	static size_t poolSizes[2] = {64, 512};
	static uint8_t block[10240];

	if (thePool.get() == nullptr) {
		thePool.reset(new MemoryPool_type(poolSizes, block, sizeof(block)));
	}
	return *(thePool.get());
}


Communicator &getCommunicator()
{
	if (theCommunicator.get() == nullptr) {
		theCommunicator.reset(new SimComm());
	}
	return *(theCommunicator.get());
}

}}}
