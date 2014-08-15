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
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include "Logger.hpp"

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

namespace stepper {

void Logger::log(const char *channel, const char *msg, SeverityLevel sev)
{
	BOOST_LOG_CHANNEL_SEV(getLogger(), channel, sev) << msg;
}

Logger::LoggerType &Logger::getLogger() {
	static bool init;
	static LoggerType theLogger;
	if (!init) {
		addConsoleSink();
		init = true;
	}
	return theLogger;
}

void Logger::addConsoleSink()
{
	// add console sink to logging core
	boost::shared_ptr< logging::core > core = logging::core::get();

	// Create a backend and attach a couple of streams to it
	boost::shared_ptr< sinks::text_ostream_backend > backend =
			boost::make_shared< sinks::text_ostream_backend >();
	backend->add_stream(
			boost::shared_ptr< std::ostream >(&std::clog, boost::empty_deleter()));

	// Enable auto-flushing after each log record written
	backend->auto_flush(true);

	// Wrap it into the frontend and register in the core.
	// The backend requires synchronization in the frontend.
	typedef logging::sinks::synchronous_sink< logging::sinks::text_ostream_backend > sink_t;
	boost::shared_ptr< sink_t > sink(new sink_t(backend));

	sink->set_formatter
	(
			logging::expressions::stream
			// line id will be written in hex, 8-digits, zero-filled
			//<< std::hex << std::setw(8) << std::setfill('0') << boost::log::expressions::attr< unsigned int >("LineID") << " "
			//<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
			<< "[" << std::setw(7) << std::left << logging::expressions::attr<std::string>("Channel") << "] "
			<< "<" << std::setw(7) << std::left << logging::trivial::severity << "> "
			<< "[" << logging::expressions::attr< logging::attributes::current_thread_id::value_type>("ThreadID") << "] "
			<< logging::expressions::smessage
	);

	core->add_sink(sink);
	logging::add_common_attributes();
}

void Logger::removeAllSinks()
{
	logging::core::get()->remove_all_sinks();
}

}
