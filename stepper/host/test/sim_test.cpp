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

BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	using namespace stepper;
	using namespace stepper::device;


	Host host;

	BOOST_CHECK(host.connectToSimulator());
}

BOOST_AUTO_TEST_CASE(sim_script_test)
{
	double radius = 1.;
	double dtheta = 1.; //degrees
	const double pi = 4. * atan(1.);
	const double d2r = pi / 180.;

	for (double t=0.; t<=360.; t+=dtheta) {
		double x = radius * cos(t * d2r);
		double y = radius * sin(t * d2r);
	}
}
