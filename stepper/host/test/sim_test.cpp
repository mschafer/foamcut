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
#include <boost/thread.hpp>
#include <TCPLink.hpp>
#include <Host.hpp>
#include <Dictionary.hpp>
#include <Script.hpp>
#include <Engine.hpp>

BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	using namespace stepper;
	using namespace stepper::device;

	Host host;

	BOOST_CHECK(host.connectToSimulator());
}

BOOST_AUTO_TEST_CASE(sim_script_test)
{
	double radius = 1000.;
	double dtheta = 1.; //degrees
	double speed = 1.;
	const double pi = 4. * atan(1.);
	const double d2r = pi / 180.;
	double segmentTime = 2.*pi*radius/speed*dtheta/360.;

	stepper::Script script;
	double ox = radius;
	double oy = 0.;
	for (double t=dtheta; t<=360.; t+=dtheta) {
		double x = radius * cos(t * d2r);
		double y = radius * sin(t * d2r);
		uint16_t dx = (uint16_t)(x - ox);
		uint16_t dy = (uint16_t)(y - oy);
		script.addLine(dx, dy, 0, 0, segmentTime);
	}

	stepper::device::Engine engine;

}
