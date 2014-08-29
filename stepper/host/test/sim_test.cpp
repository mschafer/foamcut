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
#include <Script.hpp>
#include <Engine.hpp>
#include <Simulator.hpp>
#include <Logger.hpp>

using namespace stepper;
using namespace stepper::device;

std::ostream& operator<<(std::ostream& os, const Simulator::Position& pos)
{
	os << "x: "   << pos[StepDir::X_AXIS];
	os << "\ty: " << pos[StepDir::Y_AXIS];
	os << "\tz: " << pos[StepDir::Z_AXIS];
	os << "\tu: " << pos[StepDir::U_AXIS] << std::endl;
	return os;
}


BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	Host host;

	BOOST_CHECK(host.connectToSimulator());
}

BOOST_AUTO_TEST_CASE( sim_single_step_script_test )
{
	Host host;
	Script script;
	StepDir sd;
	sd.xStepDir(1);
	sd.yStepDir(1);
	sd.zStepDir(-1);
	sd.uStepDir(-1);
	script.addStep(sd, .1);

	BOOST_CHECK(host.connectToSimulator());

	host.executeScript(script);
	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	while (host.scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
	}
	Simulator::Position answer = {1, 1, -1, -1};
	auto p = Simulator::instance().position();
	BOOST_CHECK_EQUAL_COLLECTIONS(p.begin(), p.end(), answer.begin(), answer.end());
	BOOST_CHECK_SMALL(script.duration() - Simulator::instance().time(), 1.e-6);
}

BOOST_AUTO_TEST_CASE( sim_single_line_script_test )
{
	Host host;
	Script script;
	script.addLine(113, -50, -100, 0, 1.);

	BOOST_CHECK(host.connectToSimulator());

	host.executeScript(script);
	while (host.scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
	}

	Simulator::Position answer = {113, -50, -100, 0};
	auto p = Simulator::instance().position();
	BOOST_CHECK_EQUAL_COLLECTIONS(p.begin(), p.end(), answer.begin(), answer.end());
	BOOST_CHECK_SMALL(script.duration() - Simulator::instance().time(), 1.e-6);
}

#if 0

BOOST_AUTO_TEST_CASE(sim_circle_test)
{
	double radius = 1000.;
	double dtheta = 1.; //degrees
	const double pi = 4. * atan(1.);
	const double d2r = pi / 180.;
	double segmentTime = .02;
	stepper::Script script;
	double ox = radius;
	double oy = 0.;
	for (double t=dtheta; t<=360.; t+=dtheta) {
		double x = radius * cos(t * d2r);
		double y = radius * sin(t * d2r);
		uint16_t dx = (uint16_t)(x - ox);
		uint16_t dy = (uint16_t)(y - oy);
		script.addLine(dx, dy, dx/2, dy/2, segmentTime);
		ox = x;
		oy = y;
	}

	Host host;
	BOOST_CHECK(host.connectToSimulator());

	host.executeScript(script);

	boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));

	std::cout << Simulator::instance().position();
}

#endif
