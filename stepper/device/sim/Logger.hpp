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
#ifndef stepper_sim_Logger_hpp
#define stepper_sim_Logger_hpp

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>


namespace stepper {

/**
 * BOOST_LOG_CHANNEL_SEV(Logger::get(), "chan1", boost::log::trivial::trace) << "testing 1, 2, 3";
 */
struct Logger {
typedef boost::log::trivial::severity_level SeverityLevel;
typedef boost::log::sources::severity_channel_logger<SeverityLevel> LoggerType;

static void log(const char *channel, const char *msg, SeverityLevel sev);
static void trace(const char *channel, const char *msg)   { log(channel, msg, boost::log::trivial::trace); }
static void debug(const char *channel, const char *msg)   { log(channel, msg, boost::log::trivial::debug); }
static void info(const char *channel, const char *msg)    { log(channel, msg, boost::log::trivial::info); }
static void warning(const char *channel, const char *msg) { log(channel, msg, boost::log::trivial::warning); }
static void error(const char *channel, const char *msg)   { log(channel, msg, boost::log::trivial::error); }
static void fatal(const char *channel, const char *msg)   { log(channel, msg, boost::log::trivial::fatal); }

static void addConsoleSink();
static void removeAllSinks();
static LoggerType &getLogger();
};

}

#endif
