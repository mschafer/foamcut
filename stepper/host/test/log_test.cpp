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
	using stepper::sim::Logger;

	// add console sink to logging core
	boost::shared_ptr< boost::log::core > core = boost::log::core::get();

	// Create a backend and attach a couple of streams to it
	boost::shared_ptr< boost::log::sinks::text_ostream_backend > backend =
			boost::make_shared< boost::log::sinks::text_ostream_backend >();
	backend->add_stream(
			boost::shared_ptr< std::ostream >(&std::clog, boost::empty_deleter()));

	// Enable auto-flushing after each log record written
	backend->auto_flush(true);

	// Wrap it into the frontend and register in the core.
	// The backend requires synchronization in the frontend.
	typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));

	sink->set_formatter
	(
			boost::log::expressions::stream
			// line id will be written in hex, 8-digits, zero-filled
			//<< std::hex << std::setw(8) << std::setfill('0') << boost::log::expressions::attr< unsigned int >("LineID") << " "
			//<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
			<< "[" << std::setw(7) << std::left << boost::log::expressions::attr<std::string>("Channel") << "] "
			<< "<" << std::setw(7) << std::left << boost::log::trivial::severity << "> "
			<< "[" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "] "
			<< boost::log::expressions::smessage
	);

	core->add_sink(sink);

	boost::log::add_common_attributes();

	BOOST_LOG_CHANNEL_SEV(Logger::get(), "chan1", boost::log::trivial::trace) << "testing 1, 2, 3";
	BOOST_LOG_CHANNEL_SEV(Logger::get(), "tv", boost::log::trivial::warning) << "watch out";
	BOOST_LOG_CHANNEL_SEV(Logger::get(), "chan1", boost::log::trivial::error) << "kaboom";

	// Remove the sink
	core->remove_sink(sink);
}
