#include "Platform.hpp"
#include <memory>

namespace stepper { namespace device { namespace sim {

Platform *
Platform::instance()
{
	static std::auto_ptr<Platform> thePlatform;
	if (thePlatform.get() == NULL) {
		thePlatform.reset(new Platform());
	}
	return thePlatform.get();
}

}}}
