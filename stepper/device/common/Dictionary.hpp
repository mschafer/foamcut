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
#ifndef stepper_device_Messages_hpp
#define stepper_device_Messages_hpp

/** \file This header defines all the messages that are exchanged between the host and device. */

#include <Message.hpp>

namespace stepper { namespace device {

enum {
	PING_MSG         = 0,
	PONG_MSG         = 1,
	SCRIPT_MSG       = 2,
	GO_MSG           = 3,
	PAUSE_MSG        = 4,
	HALT_MSG         = 5,
	SPEED_ADJUST_MSG = 6,
	MESSAGE_COUNT    = 7
};

struct PingMsg
{
	static void init(MessageBuffer &mb) {
		mb.header().payloadSize_ = 0;
		mb.header().id0_ = PING_MSG;
		mb.header().id1_ = 0;
	}
};

struct PongMsg
{
	static void init(MessageBuffer &mb) {
		mb.header().payloadSize_ = 0;
		mb.header().id0_ = PONG_MSG;
		mb.header().id1_ = 0;
	}
};

/**
 * Message containing script bytes for the stepper to run.
 * id1_ is unused.
 */
struct ScriptMsg
{
	enum {
		PAYLOAD_SIZE=256
	};

	static void init(MessageBuffer &mb) {
		mb.header().payloadSize_ = PAYLOAD_SIZE;
		mb.header().id0_ = SCRIPT_MSG;
		mb.header().id1_ = 0;
	}
	uint8_t scriptData_[PAYLOAD_SIZE];
};

struct GoMsg
{
};

struct PauseMsg
{
};

struct HaltMsg
{
};

struct SpeedAdjustMsg
{
};

/** All messages are fixed size. */
const uint8_t MessageSize[MESSAGE_COUNT] = {
    static_cast<uint8_t>(sizeof(PingMsg)),
    static_cast<uint8_t>(sizeof(PongMsg)),
    static_cast<uint8_t>(sizeof(ScriptMsg)),
    static_cast<uint8_t>(sizeof(GoMsg)),
    static_cast<uint8_t>(sizeof(PauseMsg)),
    static_cast<uint8_t>(sizeof(HaltMsg)),
    static_cast<uint8_t>(sizeof(SpeedAdjustMsg))
};

enum {
	MAX_MESSAGE_SIZE = 16
};

}}

#endif
