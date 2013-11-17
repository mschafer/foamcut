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
#ifndef stepper_device_StepDir_hpp
#define stepper_device_StepDir_hpp

#include <stdint.h>

namespace stepper { namespace device {

/**
 * This class represents 4 axes of step and direction information.
 */
class StepDir
{
public:
	enum AxisIdx {
		X_AXIS = 0,
		Y_AXIS = 1,
		Z_AXIS = 2,
		U_AXIS = 3,
		AXIS_COUNT = 4
	};

	enum Bits {
		X_STEP = 0x01,
		X_DIR = 0x02,
		Y_STEP = 0x04,
		Y_DIR = 0x08,
		Z_STEP = 0x10,
		Z_DIR = 0x20,
		U_STEP = 0x40,
		U_DIR = 0x80,
	};


	StepDir() : b_(0) {}

	void clear() { b_ = 0; }
	uint8_t byte() const { return b_; }

	/*
	 * \return logical value of bits to set direction with
	 * all the steps inactive.
	 */
	StepDir getDirOnlyBitVals(StepDir invertMask) {
		StepDir r;
		r.b_ = b_ & (X_DIR | Y_DIR | Z_DIR | U_DIR);
		r.b_ ^= invertMask.b_;
		return r;
	}

	StepDir getStepDirBitVals(StepDir invertMask) {
		StepDir r(invertMask);
		r.b_ ^= b_;
		return r;
	}

	bool step(AxisIdx axis) const { return getBit(stepMask(axis)); }
	void step(AxisIdx axis, bool v) { setBit(stepMask(axis), v); }
	bool dir(AxisIdx axis) const { return getBit(dirMask(axis)); }
	void dir(AxisIdx axis, bool v) { setBit(dirMask(axis), v); }

	bool xStep() const { return getBit(X_STEP); }
	void xStep(bool v) { setBit(X_STEP, v); }
	bool xDir() const { return getBit(X_DIR); }
	void xDir(bool v) { setBit(X_DIR, v); }

	bool yStep() const { return getBit(Y_STEP); }
	void yStep(bool v) { setBit(Y_STEP, v); }
	bool yDir() const { return getBit(Y_DIR); }
	void yDir(bool v) { setBit(Y_DIR, v); }

	bool zStep() const { return getBit(Z_STEP); }
	void zStep(bool v) { setBit(Z_STEP, v); }
	bool zDir() const { return getBit(Z_DIR); }
	void zDir(bool v) { setBit(Z_DIR, v); }

	bool uStep() const { return getBit(U_STEP); }
	void uStep(bool v) { setBit(U_STEP, v); }
	bool uDir() const { return getBit(U_DIR); }
	void uDir(bool v) { setBit(U_DIR, v); }

private:
	uint8_t b_;

	uint8_t stepMask(AxisIdx a) const {
		return (uint8_t)(1 << (a*2));
	}
	uint8_t dirMask(AxisIdx a) const {
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

