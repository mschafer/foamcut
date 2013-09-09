#ifndef stepper_sim_Platform_hpp
#define stepper_sim_Platform_hpp

#include "Stepper.hpp"
#include "LimitSwitches.hpp"
#include <stdint.h>

namespace stepper { namespace device { namespace sim {

/**
 * This class provides platform specific services.
 */
class Platform
{
public:

	static Platform &instance();

	/**
	 * Must be called once at startup.
	 * \todo Possible to call more than once?
	 */
	void initialize();

	/**
	 * Sets the direction bits and then the step bits to value specified in s.
	 */
	void setStepDirBits(const StepDir &s);

	/**
	 * Returns the current state of the limit switches.
	 * The \sa Stepper will use this to mask steps before calling
	 * \sa setStep.
	 */
	LimitSwitches readLimitSwitches();

	/**
	 * Starts a one shot timer that will call \sa Stepper::run when it expires.
	 */
	void startTimer(uint32_t period);

	/**
	 * Returns true if the timer is currently running and false otherwise.
	 */
	bool timerRunning();

	/** Sim platform only.  \sa Stepper::invertMask */
	void invertMask(StepDir s) { invertMask_ = s; }


private:
	Platform();
	Platform(const Platform &cpy);
	Platform &operator=(const Platform &rhs);

	int pos_[StepDir::AXIS_END];
	int time_;
	StepDir invertMask_;
	StepDir currentBits_;

};

}}}

#endif
