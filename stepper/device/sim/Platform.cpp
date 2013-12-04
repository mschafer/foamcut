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
#include "SimComm.hpp"
#include "SimStepper.hpp"
#include <MemoryPool.hpp>


namespace stepper { namespace device {

std::unique_ptr<Platform::MemoryPool_type> Platform::thePool;
std::unique_ptr<Communicator> Platform::theCommunicator;
std::unique_ptr<Stepper> Platform::theStepper;


Platform::MemoryPool_type &Platform::getMemoryPool()
{
	static size_t poolSizes[2] = {64, 512};
	static uint8_t block[10240];

	if (thePool.get() == nullptr) {
		thePool.reset(new MemoryPool_type(poolSizes, block, sizeof(block)));
	}
	return *(thePool.get());
}


Communicator &Platform::getCommunicator()
{
	if (theCommunicator.get() == nullptr) {
		theCommunicator.reset(new SimComm());
	}
	return *(theCommunicator.get());
}

Stepper &Platform::getStepper()
{
	if (theStepper.get() == nullptr) {
		theStepper.reset(new SimStepper());
	}
	return *(theStepper.get());
}

void Platform::reset()
{
	theCommunicator.reset(nullptr);
	thePool.reset(nullptr);
	theStepper.reset(nullptr);
}

}}
