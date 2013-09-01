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


	double theta = 1.;
	double r = 117.;

	int dx = (int)(r * cos(theta));
	int dy = (int)(r * sin(theta));
	Line l(dx, dy, 0, 0, 12345);
	while (!l.done()) {
		Line::NextStep ns = l.nextStep();

	}

}
