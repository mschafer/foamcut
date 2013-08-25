#ifndef stepper_Commands_hpp
#define stepper_Commands_hpp

#include <stdint.h>

/** \file This header defines all the commands that the script engine can execute. */

namespace stepper { namespace device {

enum AxisIdx {
	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,
	U_AXIS = 3
};

enum StepDirBits {
	X_STEP = 0x01,
	X_DIR = 0x02,
	Y_STEP = 0x04,
	Y_DIR = 0x08,
	Z_STEP = 0x10,
	Z_DIR = 0x20,
	U_STEP = 0x40,
	U_DIR = 0x80,
};

struct Command
{
	uint8_t id_;
};

/**
 * Command to define the port and pin for each of the four functions of an axis.
 * The step pin pulses when the axis is supposed to take a single step.
 * The dir pin is held high or low to control the direction of the step.
 * The fwd and rev pins are inputs that should be tied to limit switches.  If either
 * is active, then steps in that direction will be inhibited.  Assigning the limit
 * switches a port number of 255 (0xFF) will disable this function.
 *
 * Valid pin numbers must be in the range 0-15.
 * Valid port numbers must be in the range 0-254.
 */
struct AxisCmd : public Command
{
	/** Name of axis being configured. *
	enum AxisName {
		LEFT_X = 0,
		LEFT_Y = 1,
		RIGHT_X = 2,
		RIGHT_Y = 3,		
	};

	enum {
	    ID = 1,
		UNASSIGNED = 0xFF,
		INVERT_PIN = 0x10, ///\< Valid pin numbers are 0-15.  Adding this bit signifies inversion.
	}

	uint8_t axisName_;
	uint8_t stepPort_;
	uint8_t stepPin_;
	uint8_t dirPort_;
	uint8_t dirPin_;
	uint8_t fwdPort_;
	uint8_t fwdPin_;
	uint8_t revPort_;
	uint8_t revPin_;

};

// Axis
// Single Step
// Line
// Long Line
// Delay
// Pause

}}

#endif
