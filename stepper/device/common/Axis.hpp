#ifndef stepper_Axis_hpp
#define stepper_Axis_hpp

#include "Pin.hpp"

struct Axis
{
private:
	Pin step_;
	Pin dir_;
	Pin fwdLimit_;
	Pin revLimit_;
};

#endif
