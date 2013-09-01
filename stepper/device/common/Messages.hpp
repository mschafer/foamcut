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
#ifndef foamcut_device_Messages_hpp
#define foamcut_device_Messages_hpp

#include <stdint.h>

/** \file This header defines all the messages that are exchanged between the host and device. */


namespace foamcut { namespace device {

struct Message
{
	uint8_t id_;
};

/** A block of commands for the script engine. */
struct ScriptCommands : public Message
{
	ScriptCommands() : id_(ID);

	enum {
		ID = 1,
		SIZE = 256
	};
		
	uint8_t cmds_[SIZE-1];

};

// Go
// Pause
// Halt
// SpeedAdjust


}}

#endif
