#ifndef foamcut_Platform_hpp
#define foamcut_Platform_hpp

#include "Pin.hpp"

namespace stepper {

/**
 * This class provides platform specific services.
 */
class Platform
{
public:


	/** Configure a digital i/o pin. */
	void configurePin(Pin &pin, Pin::Function fcn);

	/** Set the level of a digital output pin. */
	void setPin(const Pin &pin, bool val);

	/** Get the level of a digital input pin. */
	bool getPin(const Pin &pin);

	static Platform *instance();


};


}

#endif
