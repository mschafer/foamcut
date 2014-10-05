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

namespace stepper {

struct Logger {

enum SeverityLevel {};

static void log(const char *channel, const char *msg, SeverityLevel sev) {}
static void trace(const char *channel, const char *msg)   {}
static void debug(const char *channel, const char *msg)   {}
static void info(const char *channel, const char *msg)    {}
static void warning(const char *channel, const char *msg) {}
static void error(const char *channel, const char *msg)   {}
static void fatal(const char *channel, const char *msg)   {}
};

}

#endif