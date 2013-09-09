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
#include <vector>
#include <cfloat>
#include <cmath>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "Line.hpp"

using stepper::device::Line;

BOOST_AUTO_TEST_CASE( line_test )
{
	int dx = 100;
	int dy = -87;
	int dz = 50;
	int du = -33;
	uint32_t dt = 517;
	Line l(dx, dy, dz, du, dt);
	int x=0, y=0, z=0, u=0, t=0;
	while (!l.done()) {
		Line::NextStep ns = l.nextStep();
		t += ns.delay_;
		if (ns.step_.xStep()) { x += ns.step_.xDir() ? 1 : -1; }
		if (ns.step_.yStep()) { y += ns.step_.yDir() ? 1 : -1; }
		if (ns.step_.zStep()) { z += ns.step_.zDir() ? 1 : -1; }
		if (ns.step_.uStep()) { u += ns.step_.uDir() ? 1 : -1; }
	}
	BOOST_CHECK(x == dx);
	BOOST_CHECK(y == dy);
	BOOST_CHECK(z == dz);
	BOOST_CHECK(u == du);
	BOOST_CHECK(t == dt);
}
