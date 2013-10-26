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

void rtest(const boost::system::error_code &error)
{
	std::cout << "rtest" << std::endl;
}


BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	using namespace stepper;
	using namespace stepper::device;

	Host host;

	BOOST_CHECK(host.connectToSimulator());
}
