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

#include <HAL.hpp>
#include "MemoryAllocator.hpp"
#include "SimCommunicator.hpp"

namespace stepper { namespace device {

MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}

namespace HAL {

static std::unique_ptr<SimCommunicator> communicator_;

void intialize()
{
	communicator_.reset(new SimCommunicator());
	communicator_->initialize();
}

void setStepDirBits(const StepDir &s)
{
}

LimitSwitches readLimitSwitches()
{
	return LimitSwitches();
}

Status sendMessage(Message *m, HAL::Priority priority)
{
	return SUCCESS;
}

Message *receiveMessage()
{
	return NULL;
}

void startTimer(uint32_t period)
{

}

void reset()
{
	communicator_.reset();
}

}}}
