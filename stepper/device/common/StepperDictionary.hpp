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
#ifndef stepper_device_StepperDictionary_hpp
#define stepper_device_StepperDictionary_hpp

#include "Message.hpp"
#include "StatusFlags.hpp"
#include "StepDir.hpp"

namespace stepper { namespace device {

enum MessageId {
	GO_MSG 					= 1,
	PAUSE_MSG				= 2,
	RESET_MSG				= 3,
	SPEED_ADJUST_MSG		= 4,
	CONNECT_MSG				= 5,
	CONNECT_RESPONSE_MSG	= 6,
	ACK_SCRIPT_MSG			= 7,
	DATA_SCRIPT_MSG			= 8,
    PING_MSG                = 9,
    PING_RESPONSE_MSG       = 10,
    HEARTBEAT_MSG           = 11,
    HEARTBEAT_RESPONSE_MSG  = 12,
    FATAL_ERROR_MSG			= 13
};

/** Starts the engine executing the currently cached script. */
struct GoMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = GO_MSG
	};

	GoMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}
};

/** Pauses the engine executing the currently cached script, resumes on GoMsg. */
struct PauseMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = PAUSE_MSG
	};

	PauseMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}
};

/** Stops engine execution and empties the script buffer. */
struct ResetMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = RESET_MSG
	};

	ResetMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}
};

/** Sets the speed scaling. */
struct SpeedAdjustMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(uint32_t),
		ID = SPEED_ADJUST_MSG,
		UNITY_SPEED_ADJUST_SHIFT = 15
	};

	SpeedAdjustMsg() : speedAdjust_(0) {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}

	uint32_t speedAdjust_;
};

/** Initiates a connection. */
struct ConnectMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(StepDir),
		ID = CONNECT_MSG
	};

	ConnectMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}

	StepDir invertMask_;
};

/** Device Response to a ConnectMsg. */
struct ConnectResponseMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = CONNECT_RESPONSE_MSG
	};

	ConnectResponseMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}
};

/**
 * Sent when the engine is done processing a DataScriptMsg.
 */
struct AckScriptMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = ACK_SCRIPT_MSG
	};

	AckScriptMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
	}
};

/**
 * Message containing script bytes for the stepper to run.
 * The size of this message is up to the maximum as set by the device.
 */
struct DataScriptMsg : Message
{
	enum {
		ID = DATA_SCRIPT_MSG,
		IN_FLIGHT_COUNT = 10 ///\< Number of unacknowledged messages that host can send before waiting
	};

	explicit DataScriptMsg(uint16_t size) {
		id(ID);
		payloadSize(size);
	}

	DataScriptMsg() {
		id(ID);
	}
};

struct PingMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = PING_MSG
	};

	PingMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
	}
};

struct PingResponseMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = PING_RESPONSE_MSG
	};

	PingResponseMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
	}
};

struct HeartbeatMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = HEARTBEAT_MSG
	};

	HeartbeatMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
	}
};

struct HeartbeatResponseMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(StatusFlags),
		ID = HEARTBEAT_RESPONSE_MSG
	};

	HeartbeatResponseMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
	}

	StatusFlags statusFlags_;
};

struct FatalErrorMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(uint8_t),
		ID = FATAL_ERROR_MSG
	};

	explicit FatalErrorMsg(uint8_t fec) : fatalErrorCode_(fec) {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
	}

	uint8_t fatalErrorCode_;
};

}}

#endif
