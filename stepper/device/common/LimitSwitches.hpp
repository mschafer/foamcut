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
#ifndef stepper_device_LimitSwitches_hpp
#define stepper_device_LimitSwitches_hpp

#include <stdint.h>
#include "StepDir.hpp"

namespace stepper { namespace device {

/**
 * This class represents 4 axes of forward and reverse limit switches.
 */
class LimitSwitches
{
public:

	enum Bits {
		X_FWD = 0x01,
		X_REV = 0x02,
		Y_FWD = 0x04,
		Y_REV = 0x08,
		Z_FWD = 0x10,
		Z_REV = 0x20,
		U_FWD = 0x40,
		U_REV = 0x80,
	};

	LimitSwitches() : b_(0) {}

	void clear() { b_ = 0; }

	/** Mask out steps deactivated by limit switches. */
	StepDir apply(const StepDir &s) {
		StepDir o(s);
		if ((s.xDir() && xFwd()) || (!s.xDir() && xRev())) { o.xStep(false); }
		if ((s.yDir() && yFwd()) || (!s.yDir() && yRev())) { o.yStep(false); }
		if ((s.zDir() && zFwd()) || (!s.zDir() && zRev())) { o.zStep(false); }
		if ((s.uDir() && uFwd()) || (!s.uDir() && uRev())) { o.uStep(false); }
		return o;
	}

	bool forwardLimit(StepDir::AxisIdx axis) const { return getBit(forwardMask(axis)); }
	void forwardLimit(StepDir::AxisIdx axis, bool v) { setBit(forwardMask(axis), v); }
	bool reverseLimit(StepDir::AxisIdx axis) const { return getBit(reverseMask(axis)); }
	void reverseLimit(StepDir::AxisIdx axis, bool v) { setBit(reverseMask(axis), v); }

	bool xFwd() const { return getBit(X_FWD); }
	void xFwd(bool v) { setBit(X_FWD, v); }
	bool xRev() const { return getBit(X_REV); }
	void xRev(bool v) { setBit(X_REV, v); }

	bool yFwd() const { return getBit(Y_FWD); }
	void yFwd(bool v) { setBit(Y_FWD, v); }
	bool yRev() const { return getBit(Y_REV); }
	void yRev(bool v) { setBit(Y_REV, v); }

	bool zFwd() const { return getBit(Z_FWD); }
	void zFwd(bool v) { setBit(Z_FWD, v); }
	bool zRev() const { return getBit(Z_REV); }
	void zRev(bool v) { setBit(Z_REV, v); }

	bool uFwd() const { return getBit(U_FWD); }
	void uFwd(bool v) { setBit(U_FWD, v); }
	bool uRev() const { return getBit(U_REV); }
	void uRev(bool v) { setBit(U_REV, v); }


private:
	uint8_t b_;

	uint8_t forwardMask(StepDir::AxisIdx a) const {
		return (uint8_t)(1 << (a*2));
	}
	uint8_t reverseMask(StepDir::AxisIdx a) const {
		return (uint8_t)(1 << ((a*2)+1));
	}

	bool getBit(uint8_t mask) const { return (b_ & mask) != 0; }
	void setBit(uint8_t mask, bool val) {
		if (val)
			b_ |= mask;
		else
			b_ &= ~mask;
	}

};

}}

#endif

