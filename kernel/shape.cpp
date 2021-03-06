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
#include "shape.hpp"
#include <stdexcept>
#include <boost/math/special_functions/sign.hpp>
#include <math.h>
#include <float.h>

namespace foamcut {
/**
 * Solve Ax = b for x, A is 2x2
 * answer returned in b
 */
void solve2x2(const double A[2][2], double b[2]) {
	double tol = std::max(fabs(b[0]), fabs(b[1])) * 1.e-12;
	double det = 1. / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
	if (fabs(det) < tol) {
		throw std::range_error("solve2x2: singular A matrix");
	}
	double x0 = (A[1][1] * b[0] - A[0][1] * b[1]) * det;
	double x1 = (A[0][0] * b[1] - A[1][0] * b[0]) * det;

	b[0] = x0; b[1] = x1;
}

Shape::Shape(const std::vector<double> &x, const std::vector<double> &y,
		const std::string &name, bool polyline) :
	x_(x), y_(y), name_(name)
{
	if (polyline) {
		invariant();
		size_t n = x.size();
		x_.clear(); y_.clear();
		x_.reserve(2*n); y_.reserve(2*n);
		x_.push_back(x[0]);
		y_.push_back(y[0]);
		for (size_t i=1; i<n-1; i++) {
			if (x[i] == x[i-1] && y[i] == y[i-1]) {
				throw std::logic_error("Shape::Shape no doubled points allowed in polyline Shape");
			}
			x_.push_back(x[i]);
			x_.push_back(x[i]);
			y_.push_back(y[i]);
			y_.push_back(y[i]);
		}
		x_.push_back(x[n-1]);
		y_.push_back(y[n-1]);
		buildSplines();
	} else {
		invariant();
		buildSplines();
	}
}

Shape::Shape(const DatFile &datfile) :
		x_(datfile.x()), y_(datfile.y()), name_(datfile.name())
{
	invariant();
	buildSplines();
}

Shape::Shape(const Shape &cpy) : x_(cpy.x_), y_(cpy.y_), name_(cpy.name_)
{
	buildSplines();
}

double
Shape::breakS(size_t idx) const
{
	size_t nseg = xSpline_.size();
	if (idx < nseg) {
		return xSpline_[idx].x().front();
	} else if (idx == nseg) {
		return s_.back();
	} else {
		throw std::out_of_range("Shape::breakS idx exceeds segment count");
	}
}

std::pair<double, double>
Shape::breakPoint(size_t idx) const
{
	std::pair<double, double> ret;
	size_t nseg = xSpline_.size();
	if (idx < nseg) {
		ret.first = xSpline_[idx].y().front();
		ret.second = ySpline_[idx].y().front();
	} else if (idx == nseg) {
		ret.first = x_.back();
		ret.second = y_.back();
	} else {
		throw std::out_of_range("Shape::breakPoint idx exceeds segment count");
	}
	return ret;
}

double
Shape::area() const
{
    double area = 0.;
    size_t npt = x_.size();
    for (size_t i=0; i<npt-1; i++) {
        area += x_[i] * y_[i+1] - x_[i+1] * y_[i];
    }
    area += x_[npt-1] * y_[0] - x_[0] * y_[npt-1];
    return area / 2.;
}

Shape::Point
Shape::evaluate(double s) const {
	size_t idx = findSplineIdx(s);
	return evaluate(s, idx);
}

Shape::Point
Shape::evaluate(double s, size_t idx) const {
	Spline::Point px = xSpline_[idx].evaluateAll(s);
	Spline::Point py = ySpline_[idx].evaluateAll(s);

	Point p;
	p.s = s;

	// s is the x coordinate of both splines
	// y values of xSpline are x coords of SplineShape
	p.x = px.y;
	p.x_s = px.y_x;
	p.x_ss = px.y_xx;

	p.y = py.y;
	p.y_s = py.y_x;
	p.y_ss = py.y_xx;

	return p;
}

Shape::handle
Shape::offset(double d) const {

	// change sign of d based on area because point offset method is always based on
	// right handed coordinate system regardless of shape winding.
	if (area() < 0.) {
		d *= -1.;
	}

	// displace all the splines and make each one into a new Shape
	std::vector<double> dx(x_.size()), dy(y_.size());
	boost::ptr_vector<Shape> dshape;
	for (size_t i=0; i<xSpline_.size(); ++i) {
		const std::vector<double> &sspl = xSpline_[i].x();
		dx.clear();
		dy.clear();
		for (size_t ispl=0; ispl<sspl.size(); ++ispl) {
			Point p = evaluate(sspl[ispl], i);
			std::pair<double, double> dp = p.offset(d);
			dx.push_back(dp.first);
			dy.push_back(dp.second);
		}
		dshape.push_back(new Shape(dx, dy));
	}

	// the displaced shapes will not be contiguous any longer so the
	// ends need to be adjusted.
	for (size_t ishp=0; ishp<dshape.size()-1; ++ishp) {
		Shape &s0 = dshape[ishp];
		Shape &s1 = dshape[ishp+1];

		double sInit[2] = {s0.s_.back(), 0};
		double sFin[2] =  {s0.s_.back(), 0};
		double ds[2];

		Point p0 = s0.evaluate(sInit[0]);
		Point p1 = s1.evaluate(sInit[1]);

		// new end point is average of offset end points
		double xFin = .5 * (p0.x + p1.x);
		double yFin = .5 * (p0.y + p1.y);

		// cross product of tangent vectors is measure of parallelism
		double cross = fabs(p1.x_s * p0.y_s - p0.x_s * p1.y_s);

		// separation of end points relative to offset
		double rdist = fabs(hypot(p0.x - p1.x, p0.y - p1.y) / d);

		// if the end points are relatively far apart and the ends of the two
		// segments are not parallel, then we can do better than the
		// average by finding where the segments intersect.
		if (rdist > .03 && cross > 1.e-2) {

			int iter = 8;
			double tol = std::max(1.e-3 * fabs(d), 1.e-7);
			double dsmax = fabs(3. * d);

			do {
				double A[2][2];
				A[0][0] = p0.x_s;
				A[0][1] = -p1.x_s;
				A[1][0] = p0.y_s;
				A[1][1] = -p1.y_s;

				ds[0] = p1.x - p0.x;
				ds[1] = p1.y - p0.y;

				solve2x2(A, ds);

				///limit the calculated ds to to dsmax
				for (size_t i = 0; i < 2; ++i) {
					double maybe = sFin[i] + ds[i] - sInit[i];
					if (fabs(maybe) > dsmax) {
						double dir = boost::math::copysign(maybe, 1.);
						ds[i] = dir * dsmax + sInit[i] - sFin[i];
					}
					sFin[i] += ds[i];
				}

				p0 = s0.evaluate(sFin[0]);
				p1 = s1.evaluate(sFin[1]);
				xFin = .5 * (p0.x + p1.x);
				yFin = .5 * (p0.y + p1.y);
				iter--;
			} while (iter > 0 && (fabs(ds[0]) > tol || fabs(ds[1]) > tol));

			if (iter == 0) {
				std::stringstream ss;
				ss << "Shape::offset breakpoint correction failed to converge at ishp = " << ishp;
				throw std::runtime_error(ss.str());
			}

		}

		// replace the endpoint of 0
		s0.replaceEndPoint(xFin, yFin);
		s1.replaceStartPoint(xFin, yFin);
	}

	// compose new x,y array from list of shapes
	dx.clear();
	dy.clear();
	boost::ptr_vector<Shape>::const_iterator dit;
	for (dit = dshape.begin(); dit != dshape.end(); ++dit) {
		dx.insert(dx.end(), dit->x_.begin(), dit->x_.end());
		dy.insert(dy.end(), dit->y_.begin(), dit->y_.end());
	}

	return handle(new Shape(dx, dy, name_));
}

Shape::handle
Shape::rotate(double theta) const
{
	theta *= atan(1.) / 45.;
	double sint = sin(theta);
	double cost = cos(theta);

	size_t n = x_.size();
	std::vector<double> x(n);
	std::vector<double> y(n);
	for (size_t i=0; i<n; i++) {
		x[i] = x_[i] * cost - y_[i] * sint;
		y[i] = x_[i] * sint + y_[i] * cost;
	}

	handle ret(new Shape(x, y, name_));
	return ret;
}

Shape::handle
Shape::scale(double sx, double sy) const
{
	size_t n = x_.size();
	std::vector<double> x(n);
	std::vector<double> y(n);
	for (size_t i=0; i<n; i++) {
		x[i] = x_[i] * sx;
		y[i] = y_[i] * sy;
	}

	handle ret(new Shape(x, y, name_));
	return ret;
}

Shape::handle
Shape::reverse() const
{
	size_t n = x_.size();
	std::vector<double> x(n);
	std::vector<double> y(n);
	for (size_t i=0; i<n; i++) {
		x[i] = x_[n-i];
		y[i] = y_[n-i];
	}

	handle ret(new Shape(x, y, name_));
	return ret;
}

void
Shape::replaceEndPoint(double xEnd, double yEnd)
{
	x_.back() = xEnd;
	y_.back() = yEnd;
	buildSplines();
}

void
Shape::replaceStartPoint(double xStart, double yStart)
{
	x_[0] = xStart;
	y_[0] = yStart;
	buildSplines();
}

double
Shape::fitLineSegment(double s0, double eps) const
{
	// maximum length is up to next break
	size_t spl_idx = findSplineIdx(s0);

	// if s0 == a break, then we need to increment spl_idx unless we are at the end.
	if (spl_idx < xSpline_.size()-1 && s0 == xSpline_[spl_idx].x().back())
		++spl_idx;
	
	double s_max = xSpline_[spl_idx].x().back();
	double ds_max = s_max - s0;
	if (ds_max <= 0.) {
		return s_max;
	}

	double err = 0.;
	double kappa_max = 0.;
	double ds = 0.;
	double s_end = 0.;
	Point p = evaluate(s0);
	while (err < eps) {
		s_end = s0 + ds;
		if (fabs(s_end-s_max) < eps) {
			s_end = s_max;
			break;
		}
		ds += eps;
		if (fabs(ds-ds_max) <= eps) {
			ds = ds_max;
		}
		p = evaluate(s0 + ds);
		kappa_max = std::max(kappa_max, fabs(p.curvature()));
        if (kappa_max != 0) {
            // small angle approximation
            err = (1. / kappa_max) * (1. - cos(.5 * kappa_max * ds));
        }

        // small angle is invalid
        if (ds * kappa_max > .75) {
            s_end = s0 + ds;
            break;
        }
	}
	return s_end;
}

Shape::handle
Shape::insertBreak(double sbrk) const
{
	// clone for return
	Shape::handle ret(new Shape(*this));
	std::vector<double> &rx = ret->x_;
	std::vector<double> &ry = ret->y_;

	// insertion must be inside range of shape
	if (sbrk <= 0. || sbrk >= s_.back()) {
		throw std::range_error("Shape::insertBreak cannot insert break outside ends of shape");
	}

	// find index for insertion
	size_t iidx = 1;
	while (s_[iidx] < sbrk) ++iidx;

	// insertion point
	Point ipt = evaluate(sbrk);
	double ds_slope = DBL_MAX;
	double kappa = ipt.curvature();
	if (kappa != 0.) {
		ds_slope =  fabs(1. / (60. * ipt.curvature())); // approximately 1 degree
	}

	// insertion vectors
	std::vector<double> ix;
	std::vector<double> iy;

	if (sbrk - s_[iidx-1] > ds_slope) {
		Point p = evaluate(sbrk - ds_slope);
		ix.push_back(p.x);
		iy.push_back(p.y);
	}

	ix.push_back(ipt.x); ix.push_back(ipt.x);
	iy.push_back(ipt.y); iy.push_back(ipt.y);

	if (s_[iidx] - sbrk > ds_slope) {
		Point p = evaluate(sbrk + ds_slope);
		ix.push_back(p.x);
		iy.push_back(p.y);
	}

	typedef std::vector<double>::iterator iterator;
	iterator itx = rx.begin() + iidx;
	rx.insert(itx, ix.begin(), ix.end());
	iterator ity = ry.begin() + iidx;
	ry.insert(ity, iy.begin(), iy.end());

	ret->buildSplines();
	return ret;
}

Shape::handle
Shape::prependShape(const Shape &shape) const
{
	// number of points in the new shape
	size_t npts = x_.size() + shape.x().size();
	std::vector<double> xn, yn;
	xn.reserve(npts); yn.reserve(npts);

	xn.insert(xn.begin(), shape.x().begin(), shape.x().end());
	yn.insert(yn.begin(), shape.y().begin(), shape.y().end());

	// translate this shape so its first point is the same as the last
	// point of the prepended shape.
	double dx = shape.x().back() - x_.front();
	double dy = shape.y().back() - y_.front();
	for (size_t ip=0; ip<x_.size(); ++ip) {
		xn.push_back(x_[ip] + dx);
		yn.push_back(y_[ip] + dy);
	}
	handle ret(new Shape(xn, yn, name_));
	return ret;
}


Shape::handle
Shape::insertShape(size_t insseg, const Shape &shape) const
{
	if (insseg == 0) return prependShape(shape);

	size_t nseg = xSpline_.size();

	std::vector<double> xn; xn.reserve(x_.size() + shape.x_.size());
	std::vector<double> yn; yn.reserve(y_.size() + shape.y_.size());
	double dx, dy;

	// copy over everything up to where new shape goes
	for (size_t iseg=0; iseg < insseg; ++iseg) {
		const std::vector<double> &xold = xSpline_[iseg].y();
		const std::vector<double> &yold = ySpline_[iseg].y();
		for (size_t ip=0; ip<xold.size(); ++ip) {
			xn.push_back(xold[ip]);
			yn.push_back(yold[ip]);
		}
	}

	// add in the translated new shape.
	dx = xn.back() - shape.x().front();
	dy = yn.back() - shape.y().front();
	for (size_t ip=0; ip<shape.x().size(); ++ip) {
		xn.push_back(shape.x()[ip] + dx);
		yn.push_back(shape.y()[ip] + dy);
	}

	// add the rest of the original shape translated to end of new shape
	dx = xn.back() - xSpline_[insseg].y().front();
	dy = yn.back() - ySpline_[insseg].y().front();

    for (size_t iseg=insseg; iseg<nseg; ++iseg) {
		const std::vector<double> &xold = xSpline_[iseg].y();
		const std::vector<double> &yold = ySpline_[iseg].y();
		for (size_t ip=0; ip<xold.size(); ++ip) {
			xn.push_back(xold[ip] + dx);
			yn.push_back(yold[ip] + dy);
		}
	}

	handle ret(new Shape(xn, yn, name_));
	return ret;
}

Shape::handle
Shape::addLeadInOut(double xl, double yl) const
{
	if (xl == 0. && yl == 0.) {
		return handle(new Shape(*this));
	}

	size_t n = x_.size();
	std::vector<double> xn(n+4);
	std::vector<double> yn(n+4);

	std::copy(x_.begin(), x_.end(), xn.begin()+2);
	xn[0] = xn[2] - xl; xn[1] = xn[2];
	xn[n+3] = xn[n+1] - xl; xn[n+2] = xn[n+1];

	std::copy(y_.begin(), y_.end(), yn.begin()+2);
	yn[0] = yn[2] - yl; yn[1] = yn[2];
	yn[n+3] = yn[n+1] - yl; yn[n+2] = yn[n+1];

	handle ret(new Shape(xn, yn, name_));
	return ret;
}

void
Shape::invariant() const
{
	size_t n = x_.size();
	if (n < 2 || y_.size() != n) {
		throw std::range_error("Shape::invariant x and y must be the same size > 2");
	}

	if ((x_[0] == x_[1] && y_[0] == y_[1]) ||
	    (x_[n-1] == x_[n-2] && y_[n-1] == y_[n-2])	) {
		throw std::logic_error("Shape::invariant no doubled points at beginning or end");
	}
}

void
Shape::buildSplines() {
	size_t n = x_.size();
	s_.clear();
	xSpline_.clear();
	ySpline_.clear();

	s_.resize(n);
	s_[0] = 0.;
	size_t segStart = 0;
	for (size_t i=1; i<n; ++i) {
		double ds = hypot(x_[i] - x_[i-1], y_[i] - y_[i-1]);
		s_[i] = s_[i-1] + ds;

		///\todo doubles might not be exactly ==, use tolerance
		if (ds == 0.) {
			// create subvectors over the continuous part
			std::vector<double> xs(x_.begin() + segStart, x_.begin() + i);
			std::vector<double> ys(y_.begin() + segStart, y_.begin() + i);
			std::vector<double> ss(s_.begin() + segStart, s_.begin() + i);

			// build splines
			xSpline_.push_back(new Spline(ss, xs));
			ySpline_.push_back(new Spline(ss, ys));
			segStart = i;
		}
	}

	// create subvectors over the continuous part
	std::vector<double> xs(x_.begin() + segStart, x_.end());
	std::vector<double> ys(y_.begin() + segStart, y_.end());
	std::vector<double> ss(s_.begin() + segStart, s_.end());

	// build splines
	xSpline_.push_back(new Spline(ss, xs));
	ySpline_.push_back(new Spline(ss, ys));
}

double Shape::nearestPoint(double x, double y, double guess) const {
	double s = guess;
	double tol = 1.e-8 * s_.back();
	size_t iterLeft = 10;
	double ds = 0.;
	Point p;

	do {
		p = evaluate(s);

		// form unit tangent vector
		double tx = p.x_s;
		double ty = p.y_s;
		double l = hypot(tx, ty);
		tx /= l;
		ty /= l;

		// error vector
		double dx = x - p.x;
		double dy = y - p.y;

		// ds = dot product of error and tangent
		ds = tx * dx + ty * dy;
		s += ds;

		--iterLeft;
	} while (fabs(ds) > tol && iterLeft > 0);

	if (iterLeft == 0) {
		throw(std::range_error("Shape::nearestPoint failed to converge"));
	}
	return s;
}

size_t Shape::nearestBreak(double x0, double y0) const
{
    double x = xSpline_[0].y()[0];
    double y = ySpline_[0].y()[0];
    double d = hypot(x-x0, y-y0);
    size_t ret = 0;
    size_t nseg = xSpline_.size();
    for (size_t i=0; i<nseg; ++i) {
        x = xSpline_[i].y().back();
        y = ySpline_[i].y().back();
        double di = hypot(x-x0, y-y0);
        if (di < d) {
            ret = i+1;
            d = di;
        }
    }
    return ret;
}

void
Shape::origin()
{
	double x0 = x_[0];
	double y0 = y_[0];
	for (size_t i=0; i<x_.size(); i++) {
		x_[i] = x_[i] - x0;
		y_[i] = y_[i] - y0;
	}
}

size_t Shape::findSplineIdx(double s) const {
	size_t ret = 0;
	boost::ptr_vector<Spline>::const_iterator split = xSpline_.begin();
	while (split != xSpline_.end()) {
		double s_end = split->x()[split->x().size()-1];
		if (s_end >= s) break;
		++split;
		++ret;
	}

	if (ret == xSpline_.size()) ret--;  // extrapolating
	return ret;
}

}
