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

#include <iomanip>
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <Logger.hpp>

BOOST_AUTO_TEST_CASE( log_test )
{
	using namespace stepper;

	BOOST_LOG_CHANNEL_SEV(Logger::getLogger(), "chan1", boost::log::trivial::trace) << "testing 1, 2, 3";
	BOOST_LOG_CHANNEL_SEV(Logger::getLogger(), "tv", boost::log::trivial::warning) << "watch out";
	BOOST_LOG_CHANNEL_SEV(Logger::getLogger(), "chan1", boost::log::trivial::error) << "kaboom";
}
