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
#ifndef foamcut_spline_hpp
#define foamcut_spline_hpp

#include <vector>

namespace foamcut {

class Spline {
public:
	Spline(const std::vector<double> &x, const std::vector<double> &y);
	~Spline();

	/** Result of evaluating a spline. */
	struct Point {
		double x;    ///\< x coordinate
		double y;    ///\< y coordinate
		double y_x;  ///\< dy/dx
		double y_xx; ///\< d2y / dx2;
	};

	const std::vector<double> &x() const {
		return x_;
	}
	const std::vector<double> &y() const {
		return y_;
	}
	const std::vector<double> &dydx() const {
		return y_x_;
	}

	/** Evaluate the spline.
	 * \param xe The spline coordinate to be evaluated.
	 * \return The value (y) of the spline at xe.  Note that the evaluation will
	 * extrapolate if x is outside the spline.
	 */
	double evaluate(double xe) const;

    /**
     * Evaluate the Spline and its first and second derivative at a point.
     * \param x The spline coordinate to evaluate.
     * \return A Spline.Point evaluated at x.
     */
	Point evaluateAll(double xe) const;

    /**
     * Invert the spline.  Find the x value that corresponds to the y argument.
     * This is not a black box routine since the spline may be multi-valued or
     * undefined at the specified y value.  Therefore a good initial guess is
     * required.
     * \param xGuess Initial guess for x.
     * \param y Y coordinate for which the inverse spline value is desired.
     * \return The x value of the spline inverse.
     */
	double invert(double xGuess, double ye) const;

	/**
	 * Inserts a new control on the spline at the specified x coordinate.
	 * Extrapolates if the point is outside the ends of the spline.
	 * \parameter x x coordinate of point to insert
	 * \return index of inserted point.
	 */
	int insertControlPoint(double xi);

private:

	/** Calculate slopes at each control point. */
	void calculate();

    /**
     * Return the segment containing the specified value of x.
     * \param xe The x coordinate being searched for.
     * \return The index of the first point greater than (not ==) x up to a
     * maximum of length-1.
     */
	size_t find(double xe) const;

	std::vector<double> x_;
	std::vector<double> y_;
	std::vector<double> y_x_;

};
}

#endif
