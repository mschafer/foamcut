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

#include "Message.hpp"

namespace stepper { namespace device {

enum {
	PING_MSG,
	PONG_MSG,
	GO_MSG,
	PAUSE_MSG,
	RESET_MSG,
	SPEED_ADJUST_MSG,
	INIT_SCRIPT_MSG,
	ACK_SCRIPT_MSG,
	DATA_SCRIPT_MSG,
};

struct PingMsg
{
	enum { PAYLOAD_SIZE = 0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(PING_MSG);
		m.id1(0);
	}
};

struct PongMsg
{
	enum { PAYLOAD_SIZE = 0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(PONG_MSG);
		m.id1(0);
	}
};

/** Starts the engine executing the currently cached script. */
struct GoMsg
{
	enum { PAYLOAD_SIZE = 0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(GO_MSG);
		m.id1(0);
	}
};

/** Stops the engine executing the currently cached script, resumes on GoMsg. */
struct PauseMsg
{
	enum { PAYLOAD_SIZE = 0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(PAUSE_MSG);
		m.id1(0);
	}
};

struct ResetMsg
{
	enum { PAYLOAD_SIZE = 0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(RESET_MSG);
		m.id1(0);
	}
};

struct SpeedAdjustMsg
{
	enum { PAYLOAD_SIZE = 4 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(SPEED_ADJUST_MSG);
		m.id1(0);
	}
	uint32_t speedAdjust_;
};

/**
 * Initialize the engine in preparation for receiving a script to run.
 * Device responds with ACK_SCRIPT
 */
struct InitScriptMsg
{
	enum { PAYLOAD_SIZE=0 };
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(INIT_SCRIPT_MSG);
		m.id1(0);
	}
};

/**
 * Response to InitScriptMsg or a sequence of ScriptMsg
 * id1_ is the ScriptMsg sequence number being Ack'd.
 * payload byte is number of ScriptMsg buffer available.
 */
struct AckScriptMsg
{
	enum { PAYLOAD_SIZE=1 };
	static AckScriptMsg &init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(ACK_SCRIPT_MSG);
		m.id1(0);
		return *reinterpret_cast<AckScriptMsg*>(m.payload());
	}

	uint8_t window_;
};

/**
 * Message containing script bytes for the stepper to run.
 * id1_ is sequence number.
 */
struct DataScriptMsg
{
	enum { PAYLOAD_SIZE=256	};
	static void init(Message &m) {
		m.payloadSize(PAYLOAD_SIZE);
		m.id0(DATA_SCRIPT_MSG);
		m.id1(0);
	}
	uint8_t scriptData_[PAYLOAD_SIZE];
};

}}

#endif
