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
#include "stepper_info.hpp"

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
	 *
	 * Preserves previously set traversal time (\sa setTime).
	 */
	handle interpolateZ(double lZ, double rZ) const;

	/**
	 * Set the traversal time for each segment of the ruled surface based
	 * on the desired speed.  The speed value is the maximum so the actual
	 * speed at the smaller end of a ruled surface will be lower.  This method
	 * should be called before doing the interpolation out to the frames.
	 * \param speed Maximum wire speed through the foam at the larger end of
	 * the ruled surface.  Units are length / second where length units match
	 * the input coordinates.
	 */
	void setTime(double speed);

	/**
	 * Generate a set of instructions for the stepper to cut this RuledSurface.
	 */
	std::shared_ptr<stepper::Script> generateScript(const StepperInfo &stepper);

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
	std::vector<double> time_;
	double lZ_;
	double span_;
};

}

#endif
