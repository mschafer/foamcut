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
#include "LimitSwitches.hpp"

namespace stepper { namespace device {

enum MessageId {
	GO_MSG 					= 1,
	PAUSE_MSG				= 2,
	SPEED_ADJUST_MSG		= 3,
	CONNECT_MSG				= 4,
	CONNECT_RESPONSE_MSG	= 5,
	ACK_SCRIPT_MSG			= 6,
	DATA_SCRIPT_MSG			= 7,
    PING_MSG                = 8,
    PING_RESPONSE_MSG       = 9,
    HEARTBEAT_MSG           = 10,
    HEARTBEAT_RESPONSE_MSG  = 11,
    LIMIT_SWITCHES_MSG      = 12,
    FATAL_ERROR_MSG			= 13,
    STATUS_MSG              = 14
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
		function(0);
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
		function(0);
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
		function(0);
	}
	
	uint32_t speedAdjust_;
};

/**
 * Initiates a connection.
 * This message causes the device to reset all of its internal state.
 * Any running script will be aborted and the buffer cleared.
 */
struct ConnectMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(StepDir),
		ID = CONNECT_MSG
	};

	ConnectMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(ID);
		function(0);
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
		function(0);
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
		function(0);
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
		function(0);
	}

	DataScriptMsg() {
		id(ID);
		function(0);
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
		function(0);
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
		function(0);
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
		function(0);
	}
};

struct HeartbeatResponseMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		ID = HEARTBEAT_RESPONSE_MSG
	};

	HeartbeatResponseMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
		function(0);
	}
};

struct LimitSwitchesMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(LimitSwitches),
		ID = LIMIT_SWITCHES_MSG
	};

	LimitSwitchesMsg() {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
		function(0);
	}

	LimitSwitches limits_;
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
		function(0);
	}

	uint8_t fatalErrorCode_;
};

struct StatusMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(StatusFlags::FlagContainer),
		ID = STATUS_MSG
	};

	StatusMsg() : statusFlags_(0) {
		id(ID);
		payloadSize(PAYLOAD_SIZE);
		function(0);
	}

	StatusFlags::FlagContainer statusFlags_;
};

template <typename T>
T* allocateMessage() {
	Message *m = Message::alloc(T::PAYLOAD_SIZE);
	T *ret = new (m)T();
	return ret;
}

}}

#endif
