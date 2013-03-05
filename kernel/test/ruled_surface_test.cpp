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

#include "ruled_surface.hpp"

BOOST_AUTO_TEST_CASE( ruled_surface_zinterp )
{
	using namespace foamcut;

	double lxa[] = {0., 3., 2.};
	double lya[] = {0., 1., 4.};
	std::vector<double> lx(3);
	std::vector<double> ly(3);
	std::copy(lxa, lxa+3, lx.begin());
	std::copy(lya, lya+3, ly.begin());

	double rxa[] = {0., 6., 4.};
	double rya[] = {0., 2., 8.};
	std::vector<double> rx(3);
	std::vector<double> ry(3);
	std::copy(rxa, rxa+3, rx.begin());
	std::copy(rya, rya+3, ry.begin());

#if 0

	RuledSurface rs(lx, ly, rx, ry, 0., 1.);

	RuledSurface::handle hz = rs.interpolateZ(-1., 1.);

    BOOST_CHECK_SMALL(hz->leftX()[0], 1.e-9);
    BOOST_CHECK_SMALL(hz->leftY()[0], 1.e-9);
    BOOST_CHECK_SMALL(hz->leftX()[1], 1.e-9);
    BOOST_CHECK_SMALL(hz->leftY()[1], 1.e-9);
    BOOST_CHECK_SMALL(hz->leftX()[2], 1.e-9);
    BOOST_CHECK_SMALL(hz->leftY()[2], 1.e-9);

    BOOST_CHECK_CLOSE(hz->rightX()[0], rxa[0], 1.e-9);
    BOOST_CHECK_CLOSE(hz->rightY()[0], rya[0], 1.e-9);
    BOOST_CHECK_CLOSE(hz->rightX()[1], rxa[1], 1.e-9);
    BOOST_CHECK_CLOSE(hz->rightY()[1], rya[1], 1.e-9);
    BOOST_CHECK_CLOSE(hz->rightX()[2], rxa[2], 1.e-9);
    BOOST_CHECK_CLOSE(hz->rightY()[2], rya[2], 1.e-9);

#endif

}
