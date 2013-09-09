#include "Platform.hpp"
#include <memory>

namespace stepper { namespace device { namespace sim {

Platform &
Platform::instance()
{
	static std::auto_ptr<Platform> thePlatform;
	if (thePlatform.get() == NULL) {
		thePlatform.reset(new Platform());
	}
	return *(thePlatform.get());
}

void Platform::initialize()
{

}

Platform::Platform() : time_(0)
{
	for (int i=0; i<StepDir::AXIS_END; ++i) pos_[i] = 0;
}

void Platform::setStepDirBits(const StepDir &s)
{
	for (int i=0; i<StepDir::AXIS_END; ++i) {
		StepDir::AxisIdx ai = static_cast<StepDir::AxisIdx>(i);

		// set direction bits
		currentBits_.dir(ai, s.dir(ai));

		// was there an edge on a step bit?
		if (s.step(ai) != currentBits_.step(ai)) {
			currentBits_.step(ai, s.step(ai));
			// was the edge in the right direction to cause a step?
			if (currentBits_.step(ai) != invertMask_.step(ai)) {
				pos_[i] += (invertMask_.dir(ai) == currentBits_.dir(ai)) ? -1 : 1;
			}
		}
	}
}


}}}

