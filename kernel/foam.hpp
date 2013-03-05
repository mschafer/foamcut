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
#ifndef foamcut_foam_hpp
#define foamcut_foam_hpp

namespace foamcut {

/**
 * Describes the properties of the foam being cut.
 */
class Foam {
public:
	virtual ~Foam() {}

	/** The desired speed of the wire through the foam on the fasted end. */
	double maxSpeed_;

	/**
	 * Calculate the kerf based on cutting speed.
	 * Units for speed are length / second where length is the same as that used for step size.
	 */
	virtual double kerf(double speed) const = 0;

};

class TestFoam : public Foam {
public:
	TestFoam() : maxSpeed_(1.) {}

	double kerf(double speed) const {
		if (speed < maxSpeed_) {
			return .05;
		} else {
			return .03;
		}
	}
};

}

#endif
