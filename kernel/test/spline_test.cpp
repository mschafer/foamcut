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
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "spline.hpp"
#include "ruled_surface.hpp"

BOOST_AUTO_TEST_CASE( spline_1 )
{
    using namespace foamcut;
    
    std::vector<double> x(2);
    std::vector<double> y(2);
    
    x[0] = 1.; x[1] = 2.;
    y[0] = -1.; y[1] = -2.;
    
    Spline s(x, y);
    
    // simple interpolation into linear spline
    BOOST_CHECK_CLOSE(s.evaluate(1.5), -1.5, .001);
    
    // extrapolation before first point
    BOOST_CHECK_CLOSE(s.evaluate(.5), -.5, .001);
    
    // extrapolation after last point
    BOOST_CHECK_CLOSE(s.evaluate(2.5), -2.5, .001);
}

BOOST_AUTO_TEST_CASE( spline_shape_linearize )
{
    using namespace foamcut;
    
    std::vector<double> x(301);
    std::vector<double> y(301);
    
    // make a circle
    for (size_t i=0; i<301; i++) {
        double theta = 8. * atan(1.) * double (i) / 300;
        x[i] = cos(theta);
        y[i] = sin(theta);
    }
    
    Shape ssl(x, y);
    Shape ssr(x, y);
    RuledSurface rs(ssl, ssr, 0., 1., .01);
    
    const std::vector<double> &xl = rs.leftX();
    const std::vector<double> &yl = rs.leftY();
    
    for (size_t i=0; i<xl.size(); i++) {
        BOOST_CHECK_CLOSE(1., hypot(xl[i], yl[i]), .1);
    }
}

BOOST_AUTO_TEST_CASE( spline_slopes )
{
	using namespace foamcut;
	std::vector<double> x,y,s;
	x.push_back(-.5); y.push_back(-.5);
	x.push_back(.2); y.push_back(-.2);
	x.push_back(.5); y.push_back(.5);

	s.resize(x.size());
	s[0] = 0.;
	for (size_t i=1; i<x.size(); ++i) {
		s[i] = s[i-1] + hypot(x[i]-x[i-1], y[i]-y[i-1]);
	}

	std::auto_ptr<Spline> xspl(new Spline(s, x));
	std::auto_ptr<Spline> yspl(new Spline(s, y));

	for (size_t i=0; i<x.size(); ++i) {
		auto px0 = xspl->evaluateAll(s[i]);
		auto px1 = xspl->evaluateAll(s[i] + 1.e-6);
		BOOST_CHECK_CLOSE((px1.y-px0.y)/(px1.x-px0.x), xspl->dydx()[i], 1.e-2);

		auto py0 = yspl->evaluateAll(s[i]);
		auto py1 = yspl->evaluateAll(s[i] + 1.e-6);
		BOOST_CHECK_CLOSE((py1.y-py0.y)/(py1.x-py0.x), yspl->dydx()[i], 1.e-2);
	}

}
