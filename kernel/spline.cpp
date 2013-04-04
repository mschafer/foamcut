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
#include <math.h>
#include <stdexcept>
#include <boost/math/special_functions/sign.hpp>
#include "spline.hpp"

namespace foamcut {

/**
 * Solve a tridiagonal system in the form:
 * a c           d
 * b a c         d
 *   b a c       d
 *     b a       d
 *
 * The answer is in d upon return.  a,b, and c are all destroyed.
 */
void trisol(std::vector<double> &a, std::vector<double> &b,
		std::vector<double> &c, std::vector<double> &d) {
	int n = static_cast<int>(a.size());
	int k, km;
	for (k = 1; k < n; k++) {
		km = k - 1;
		c[km] = c[km] / a[km];
		d[km] = d[km] / a[km];
		a[k] = a[k] - b[k] * c[km];
		d[k] = d[k] - b[k] * d[km];
	}

	d[n - 1] = d[n - 1] / a[n - 1];

	for (k = n - 2; k >= 0; k--) {
		d[k] = d[k] - c[k] * d[k + 1];
	}
}

Spline::Spline(const std::vector<double> &x, const std::vector<double> &y) :
		x_(x), y_(y) {
	if (x_.size() != y_.size() || x_.size() < 2) {
		throw(std::range_error(
				"Spline::Spline x and y must be the same length and >= 2"));
	}
	calculate();
}

Spline::~Spline() {
}

void Spline::calculate() {
	size_t n = x_.size();
	y_x_.resize(n);
	std::vector<double> a(n);
	std::vector<double> b(n);
	std::vector<double> c(n);

	size_t i;
	for (i = 1; i < n - 1; i++) {
		double dxm = x_[i] - x_[i - 1];
		double dxp = x_[i + 1] - x_[i];
		b[i] = dxp;
		a[i] = 2. * (dxm + dxp);
		c[i] = dxm;
		y_x_[i] = 3.
				* ((y_[i + 1] - y_[i]) * dxm / dxp
				+ (y_[i] - y_[i - 1]) * dxp / dxm);
	}

	// set 0 2nd derivative end conditions (not a knot)
	a[0] = 2.;
	c[0] = 1.;
	y_x_[0] = 3. * (y_[1] - y_[0]) / (x_[1] - x_[0]);
	b[n - 1] = 1.;
	a[n - 1] = 2.;
	y_x_[n - 1] = 3. * (y_[n - 1] - y_[n - 2]) / (x_[n - 1] - x_[n - 2]);

	trisol(a, b, c, y_x_);
}

double Spline::evaluate(double xe) const {
	Point sp = evaluateAll(xe);
	return sp.y;

}

Spline::Point Spline::evaluateAll(double xe) const {
	Point sp;
	sp.x = xe;
	size_t iseg = find(xe);
	double dx = x_[iseg] - x_[iseg - 1];
	double t = (xe - x_[iseg - 1]) / dx;

	double f0 = y_[iseg - 1];
	double f1 = dx * y_x_[iseg - 1];
	double f2 = -dx * (2. * y_x_[iseg - 1] + y_x_[iseg])
			+ 3. * (y_[iseg] - y_[iseg - 1]);
	double f3 = dx * (y_x_[iseg - 1] + y_x_[iseg])
			- 2. * (y_[iseg] - y_[iseg - 1]);

	sp.y = f0 + t * (f1 + t * (f2 + t * f3));
	sp.y_x = f1 + t * (2. * f2 + t * 3. * f3);
	sp.y_xx = 2. * f2 + t * 6. * f3;

	sp.y_x /= dx;
	sp.y_xx /= dx * dx;
	return sp;
}

double Spline::invert(double xGuess, double y) const {
	double x = xGuess;
	double yv = evaluate(x);
	double dy = y - yv;
	int iter = 0;

	// iteration tolerance, kludge abstol and reltol sort of
	double dyTol = 1.e-6;
	if (fabs(y) > 1.)
		dyTol = 1.e-6 * fabs(y);

	bool converged = false;
	while (!converged && iter < 20) {
		Point d = evaluateAll(x);
		double dx = dy / d.y_x;
		x += dx;
		yv = evaluate(x);
		dy = y - yv;
		if (fabs(dy) < dyTol) {
			converged = true;
		}
	}

	if (converged)
		return x;
	else {
		throw(std::domain_error("Spline:Invert failed to converge"));
	}
}

int Spline::insertControlPoint(double xi) {
	Point px = evaluateAll(xi);

	std::vector<double> nx(x_.size()+1);
	std::vector<double> ny(x_.size()+1);
	std::vector<double> nd(x_.size()+1);

	size_t i=0;
	double dir = x_[1] - x_[0];
	while (boost::math::copysign((xi-x_[i]), dir) > 0.) {
		nx[i] = x_[i];
		ny[i] = y_[i];
		nd[i] = y_x_[i];
		++i;
	}
	int iret = i;
	nx[i] = xi;
	ny[i] = px.y;
	nd[i] = px.y_x;
	++i;
	while (i < nx.size()) {
		nx[i] = x_[i-1];
		ny[i] = y_[i-1];
		nd[i] = y_x_[i-1];
		++i;
	}

	x_.swap(nx);
	y_.swap(ny);
	y_x_.swap(nd);

	return iret;
}

size_t Spline::find(double xe) const {
	size_t ilow, imid, i;
	size_t n = x_.size();
	ilow = 0;
	i = n - 1;
	while ((i - ilow) > 1) {
		imid = (i + ilow) / 2;
		if (xe < x_[imid]) {
			i = imid;
		} else {
			ilow = imid;
		}
	}
	return i;
}

}
