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

#include <stdint.h>


namespace stepper { namespace device {

struct Message
{
	Message *next_;
	uint8_t header_[sizeof(void*)];

	enum {
		ID_OFFSET = sizeof(header_) - 1,
		POOL_TAG_OFFSET = sizeof(header_) - 2
	};

	uint8_t poolTag() const { return header_[POOL_TAG_OFFSET]; }
	void poolTag(uint8_t val) { header_[POOL_TAG_OFFSET] = val; }

	uint8_t id() const { return header_[ID_OFFSET]; }
	void id(uint8_t val) { header_[ID_OFFSET] = val; }

	uint8_t *payload() { return reinterpret_cast<uint8_t*>(this+1); }

};

static_assert(sizeof(Message)==2*sizeof(void*),"Alignment problem with Message");

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
};

struct PongMsg
{
};

struct ScriptMsg
{
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
