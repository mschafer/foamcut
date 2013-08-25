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
