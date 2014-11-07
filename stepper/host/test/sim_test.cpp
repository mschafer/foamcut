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

std::ostream& operator<<(std::ostream& os, const Position& p)
{
	os << "time: " << p.time_;
	os << "\tx: " << p.pos_[StepDir::X_AXIS];
	os << "\ty: " << p.pos_[StepDir::Y_AXIS];
	os << "\tz: " << p.pos_[StepDir::Z_AXIS];
	os << "\tu: " << p.pos_[StepDir::U_AXIS] << std::endl;
	return os;
}


BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	Host host;
	host.connectToSimulator();
	BOOST_CHECK(host.connected());
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
	host.connectToSimulator();
	BOOST_CHECK(host.connected());

	host.executeScript(script);
	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	while (host.scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
	}
	Position answer;
	answer.pos_ = {1, 1, -1, -1};
	auto p = Simulator::instance().position();
	BOOST_CHECK_EQUAL_COLLECTIONS(p.pos_.begin(), p.pos_.end(), answer.pos_.begin(), answer.pos_.end());
	BOOST_CHECK_SMALL(script.duration() - Simulator::instance().position().time_, 1.e-6);
}

BOOST_AUTO_TEST_CASE( sim_single_line_script_test )
{
	Host host;
	Script script;
	script.addLine(113, -50, -100, 0, 1.);

	host.connectToSimulator();
	BOOST_CHECK(host.connected());

	host.executeScript(script);
	while (host.scriptRunning()) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
	}

	Position answer;
	answer.pos_ = { 113, -50, -100, 0 };
	auto p = Simulator::instance().position();
	BOOST_CHECK_EQUAL_COLLECTIONS(p.pos_.begin(), p.pos_.end(), answer.pos_.begin(), answer.pos_.end());
	BOOST_CHECK_SMALL(script.duration() - Simulator::instance().position().time_, 1.e-6);
	BOOST_CHECK_EQUAL(Simulator::instance().positionLog().size(), 114);
}

BOOST_AUTO_TEST_CASE(sim_circle_test)
{
	double radius = 1000.;
	double dtheta = 1.; //degrees
	const double pi = 4. * atan(1.);
	const double d2r = pi / 180.;
	double segmentTime = .125;
	stepper::Script script;
	double lox = radius;
	double loy = 0.;
	double rox = radius / 2.;
	double roy = 0.;
	for (double t=dtheta; t<=360.; t+=dtheta) {
		double x = radius * cos(t * d2r);
		double y = radius * sin(t * d2r);
		int16_t ldx = (int16_t)(x - lox);
		int16_t ldy = (int16_t)(y - loy);
		int16_t rdx = (int16_t)(x/2. - rox);
		int16_t rdy = (int16_t)(y/2. - roy);
		script.addLine(ldx, ldy, rdx, rdy, segmentTime);
		lox += ldx;
		loy += ldy;
		rox += rdx;
		roy += rdy;
	}

	Host host;
	host.connectToSimulator();
	BOOST_CHECK(host.connected());

	host.executeScript(script);

	int iter = 0;
	while (host.scriptRunning() && iter < 1000) {
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		++iter;
	}
	BOOST_CHECK(iter < 1000);

	auto pit = Simulator::instance().positionLog().begin();
	auto pend = Simulator::instance().positionLog().end();
	while (pit < pend) {
		double r1 = hypot(((double)pit->pos_[0]+radius), (double)pit->pos_[1]);
		BOOST_CHECK_SMALL(r1 - radius, 1.5);
		double r2 = hypot(((double)pit->pos_[2] + radius/2.), (double)pit->pos_[3]);
		BOOST_CHECK_SMALL(r2 - radius/2., 1.5);
		++pit;
	}
}
