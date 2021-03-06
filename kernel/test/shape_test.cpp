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
#include "shape.hpp"

BOOST_AUTO_TEST_CASE( shape_break_kerf )
{
    using namespace foamcut;

    std::vector<double> x,y;
    x.push_back(0.); y.push_back(0.);
    x.push_back(1.); y.push_back(0.);
    x.push_back(1.); y.push_back(0.);
    x.push_back(1.); y.push_back(1.);

    Shape shape(x, y);

    Shape::handle dshape = shape.offset(.1);
    BOOST_CHECK(dshape->x().size() == 4);
    BOOST_CHECK_SMALL(dshape->x()[0],      1e-9); BOOST_CHECK_CLOSE(dshape->y()[0], -.1, 1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[1], 1.1, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[1], -.1, 1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[2], 1.1, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[2], -.1,  1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[3], 1.1, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[3],  1.,  1e-9);

    dshape = shape.offset(-.1);
    BOOST_CHECK(dshape->x().size() == 4);
    BOOST_CHECK_SMALL(dshape->x()[0],     1e-9); BOOST_CHECK_CLOSE(dshape->y()[0], .1, 1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[1], .9, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[1], .1, 1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[2], .9, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[2], .1, 1e-9);
    BOOST_CHECK_CLOSE(dshape->x()[3], .9, 1e-9); BOOST_CHECK_CLOSE(dshape->y()[3], 1.,  1e-9);
}

BOOST_AUTO_TEST_CASE( shape_fit_line )
{
	using namespace foamcut;

	double pi = 4. * atan(1.);
	double dt = pi / 180.;
	std::vector<double> x,y;
	for (double t=-pi/4.; t<pi/4.; t+=dt) {
		x.push_back(cos(t));
		y.push_back(sin(t));
	}
	Shape s(x,y);

	double s0 = pi/4.;
	double eps = .001;
	double sfit = s.fitLineSegment(s0, eps);
	BOOST_CHECK_SMALL(sfit-s0 - 2.*acos(1.-eps), eps);

	eps = .01;
	sfit = s.fitLineSegment(s0, eps);
	BOOST_CHECK_SMALL(sfit-s0 - 2.*acos(1.-eps), eps);
}

BOOST_AUTO_TEST_CASE( shape_leloop_kerf )
{
	using namespace foamcut;
	std::vector<double> x,y;
	x.push_back(9.99952); y.push_back(-0.157591);
	x.push_back(9.99189); y.push_back(-0.575956);
	x.push_back(9.99189); y.push_back(-0.575956);
	x.push_back(10.4179); y.push_back(-0.165221);
	x.push_back(10.0071); y.push_back(0.260775);

	std::auto_ptr<Shape> shape(new Shape(x, y));
	Shape::Point p = shape->evaluate(0.);

	auto kshape = shape->offset(-.03);
}

