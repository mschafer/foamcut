#include "MemoryAllocator.hpp"
#include <Stepper.hpp>
#include <HAL.hpp>


namespace stepper { namespace device {

MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}


void HAL::initialize()
{
}

void HAL::setStepDirBits(const StepDir &s)
{
}

LimitSwitches HAL::readLimitSwitches()
{
    LimitSwitches ret;
	return ret;
}

ErrorCode HAL::sendMessage(Message *m, Message::Priority priority)
{
    ErrorCode ec;
    return ec;
}

Message *HAL::receiveMessage()
{
    return NULL;
}

void HAL::startTimer(uint32_t period)
{
}

void HAL::stopTimer()
{
}

}}
