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


namespace stepper { namespace sim {

class Logger {
public:
	typedef boost::log::trivial::severity_level SeverityLevel;
	typedef boost::log::sources::severity_channel_logger<SeverityLevel> Logger_t;

	static Logger_t &get();
};

}}

#endif
