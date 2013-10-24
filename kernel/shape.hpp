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
#ifndef foamcut_shape_hpp
#define foamcut_shape_hpp

#include "foamcut_kernel_dll.h"
#include "spline.hpp"
#include "dat_file.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <cmath>

namespace foamcut {

/**
 * A 2 dimensional curve consisting of connected splines.  The splines define segments with
 * continuous value and slope.  The breaks between splines have continuous value only.
 * The end point conditions of the splines are "not a knot."
 */
class foamcut_kernel_API Shape {
public:
	typedef boost::shared_ptr<Shape> handle;

	struct Point {
		/** \return local curvature at this point. */
		double curvature() const {
			double kappa = (x_s * y_ss - y_s * x_ss)
					/ sqrt(pow(x_s * x_s + y_s * y_s, 3.));
			return kappa;
		}

		/**
		 * Offset the point by distance d in the direction of the normal.
		 * \return A std::pair containing the displaced x and y values.
		 */
		std::pair<double, double> offset(double d) const {
			double q = 1. / hypot(x_s, y_s);
			double nxh = y_s * q;
			double nyh = -x_s * q;

			std::pair<double, double> ret;
			ret.first = x + d * nxh;
			ret.second = y + d * nyh;
			return ret;
		}

		double x, y, s, x_s, y_s, x_ss, y_ss;
	};

	/**
	 * Construct a shape from a list of x,y coordinates in "dat" file format.
	 * Points are connected by a cubic spline with break at doubled points.
	 * \param polyline If true, this creates a shape consisting of straight
	 * line segments connecting all the x,y points.  There should be no
	 * doubled points in x,y.
	 */
	Shape(const std::vector<double> &x, const std::vector<double> &y,
			const std::string &name=std::string(""), bool polyline=false);

	Shape(const DatFile &datfile);
	Shape(const Shape &cpy);

	const std::vector<double> &x() const { return x_; }
	const std::vector<double> &y() const { return y_; }
	const std::vector<double> &s() const { return s_; }

	const std::string &name() const { return name_; }

	/**
	 * \return The number of spline segments in this shape.  Each segment boundary
	 * represents a break point or slope discontinuity.
	 */
	size_t segmentCount() const { return xSpline_.size(); }

	const foamcut::Spline &xSpline(size_t idx) const { return xSpline_[idx]; }
	const foamcut::Spline &ySpline(size_t idx) const { return ySpline_[idx]; }

	/**
	 * \return The s coordinate for the break point specified by idx.
	 */
	double breakS(size_t idx) const;

	/** \return x,y coordinates of break point at idx. */
	std::pair<double, double> breakPoint(size_t idx) const;

    /**
     * \return area of the shape.  A positive value indicates CCW winding.
     * If the first and last points are not the same, then the shape is
     * closed by connecting them.
     */
    double area() const;

	/**
	 * Evaluate the shape at the given arc length
	 * \param s Arc length coordinate
	 * \return Point on the shape at s.
	 */
	Point evaluate(double s) const;

	/**
	 * Evaluate the shape at the give arc length using the spline specified
	 * by idx.
	 */
	Point evaluate(double s, size_t idx) const;


	/**
	 * Offset all the points in the shape by distance d in the direction normal to the shape.
	 * A positive offset will increase the magnitude of the shapes area, ie. offset to the left for
	 * shapes w/ CW winding and to the right for shapes with CCW winding.
	 * a.k.a. kerf correction
	 * \return handle to the new, displaced shape
	 */
	handle offset(double d) const;

	/**
	 * Rotate the shape CCW by angle theta in degrees.
	 * \return handle to the new, rotated shape
	 */
	handle rotate(double theta) const;

	/**
	 * Scale x coordinates by sx, y coordinates by sy.
	 * \return handle to the new, scaled shape.
	 */
	handle scale(double sx, double sy) const ;

	/** scale(sc, sc) */
	handle scale(double sc) const { return scale(sc, sc); }

	/** \return A shape with the points in reverse order. */
	handle reverse() const;

	/**
	 * Fit a single line segment to the spline starting at s0.
	 * \param s0 Starting s value for the line segment.
	 * \param eps Maximum allowable error between the spline and the returned line segment.
	 * \return s value of line segment endpoint.
	 */
	double fitLineSegment(double s0, double eps) const;

	/**
	 * Find the point nearest x,y on the shape.
	 * \param guess Initial guess for s value of nearest point.
	 * \return The s value corresponding to the point on the shape nearest x,y.
	 */
	double nearestPoint(double x, double y, double guess) const;

    /**
    * Find the nearest break point to x,y on the shape.
    * \return Index of the segment that starts with the nearest break.
    */
    size_t nearestBreak(double x, double y) const;

	/**
	 * Creates a break at the specified s value.  Adds a doubled point at s and
	 * new points nearby on either side to preserve slope.
	 */
	handle insertBreak(double s) const;

	/**
	 * Splices in a shape with coordinates (x,y) at the beginning of segment with
     * index = insseg.  If insseg == nseg, the shape is appended.
	 */
	handle insertShape(size_t insseg, const Shape &shape) const;

	/**
	 * Approximate shape with line segments for plotting.
	 * \tparam vector for holding x,y values should conform to std::vector interface
	 */
	template<typename T> void plottable(T &x, T &y) const 
	{
		x.clear();
		y.clear();
		double s = 0.;
		Point p0 = evaluate(s);
		x.push_back(p0.x);
		y.push_back(p0.y);
		while (s < .999 * s_.back()) {
			double s1 = fitLineSegment(s, .001*s_.back());
			Point p1 = evaluate(s1);
			x.push_back(p1.x);
			y.push_back(p1.y);
			s = s1;
		}
		p0 = evaluate(s_.back());
		x.push_back(p0.x);
		y.push_back(p0.y);
	}

	template<typename T> void breaks(T &x, T &y) const
	{
		size_t n = xSpline_.size();

		x.push_back(xSpline_[0].y().front());
		y.push_back(ySpline_[0].y().front());
		for (size_t i=0; i<n; ++i) {
			x.push_back(xSpline_[i].y().back());
			y.push_back(ySpline_[i].y().back());
		}
	}

private:
	Shape();
	void invariant() const;
	void buildSplines();
	void origin();

	/**
	 * Replace the last point in the shape with one at the given x,y coordinates.
	 * All points past where xEnd, yEnd is nearest to the shape are discarded.
	 */
	void replaceEndPoint(double xEnd, double yEnd);

	/**
	 * Replace the first point in the shape with one at the given x,y coordinates.
	 * All points past where xEnd, yEnd is nearest to the shape are discarded.
	 */
	void replaceStartPoint(double xStart, double yStart);

	/**
	 *  Returns the index of the first spline with s_end >= s.
	 *  If s exactly equals a break point, then the index of the spline ending at s is returned.
	 */
	size_t findSplineIdx(double s) const;

	std::vector<double> x_;
	std::vector<double> y_;
	std::vector<double> s_;
	std::string name_;

	boost::ptr_vector<Spline> xSpline_;
	boost::ptr_vector<Spline> ySpline_;

};

}

#endif
