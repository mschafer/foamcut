#ifndef stepper_device_Line_hpp
#define stepper_device_Line_hpp

#include <stdint.h>

namespace stepper { namespace device {

class Line
{
public:
	Line(int dx, int dy, int dz, int du, uint32_t delay);

	void reset(int dx, int dy, int dz, int du, uint32_t delay);

	uint8_t nextStep();

private:
	int absd_[5];
	int e_[5];
	int maxd_;
	int count_;
    uint32_t stepDelay_;
    uint8_t dir_, step_, delayInc_;

    Line();
};

}}

#endif
