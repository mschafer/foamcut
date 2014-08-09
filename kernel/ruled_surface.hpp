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
#ifndef foamcut_ruled_surface_hpp
#define foamcut_ruled_surface_hpp

#include <memory>
#include <boost/shared_ptr.hpp>
#include "shape.hpp"
#include "Machine.hpp"

namespace stepper {
	class Script;
}

namespace foamcut {

class RuledSurface
{
public:
	typedef boost::shared_ptr<RuledSurface> handle;

	/**
	* Construct a ruled surface from splined shapes.
	*/
	RuledSurface(const Shape &left, const Shape &right,
		double leftZ, double span, double eps);

	virtual ~RuledSurface();

	double span() const { return span_; }
	double leftZ() const { return lZ_; }

	/**
	* Translate the ruled surface along the z axis by amount zOffset.
	*/
	handle translateZ(double zOffset) const;

	/**
	 * Calculate the intersection points for this ruled surface with the
	 * two planes defined by z = lZ and z = rZ.
	 *
	 * When cutting foam, the face of the foam block is offset from the plane in which the
	 * wire is moved.  This function calculates the points for the wire to move through in
	 * order to cut the correct shape at the block of foam.
	 */
	handle interpolateZ(double lZ, double rZ) const;

	/**
	 * Generate a set of instructions for the stepper to cut this RuledSurface.
	 */
	std::shared_ptr<stepper::Script> generateScript(const Machine &machine);

	/** \todo Add a method for sweep rotation */

	const std::vector<double> &leftX() const { return lx_; }
	const std::vector<double> &leftY() const { return ly_; }
	const std::vector<double> &rightX() const { return rx_; }
	const std::vector<double> &rightY() const { return ry_; }

private:
	RuledSurface() : lZ_(0.), span_(0.) {}

	std::vector<double> lx_;
	std::vector<double> ly_;
	std::vector<double> rx_;
	std::vector<double> ry_;
	double lZ_;
	double span_;
};

}

#endif
