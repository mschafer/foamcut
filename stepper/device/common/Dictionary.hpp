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

template <typename Allocator>
struct PingMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = PING_MSG
	};

	PingMsg() {
		payloadSize(PAYLOAD_SIZE);
		id0(ID0);
	}
};

template <typename Allocator>
struct PongMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = PONG_MSG
	};

	PongMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}
};

/** Starts the engine executing the currently cached script. */
template <typename Allocator>
struct GoMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = GO_MSG
	};

	GoMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}
};

/** Stops the engine executing the currently cached script, resumes on GoMsg. */
template <typename Allocator>
struct PauseMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = PAUSE_MSG
	};

	PauseMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}
};

template <typename Allocator>
struct ResetMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = RESET_MSG
	};

	ResetMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}
};

template <typename Allocator>
struct SpeedAdjustMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = sizeof(uint32_t),
		ID0 = SPEED_ADJUST_MSG
	};

	SpeedAdjustMsg() : speedAdjust_(0) {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}

	uint32_t speedAdjust_;
};



/**
 * Initialize the engine in preparation for receiving a script to run.
 * Device responds with ACK_SCRIPT
 */
template <typename Allocator>
struct InitScriptMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 0,
		ID0 = INIT_SCRIPT_MSG
	};

	InitScriptMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}
};

/**
 * Response to InitScriptMsg or a sequence of ScriptMsg
 * id1_ is the ScriptMsg sequence number being Ack'd.
 * payload byte is number of ScriptMsg buffer available.
 */
template <typename Allocator>
struct AckScriptMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = sizeof(uint8_t),
		ID0 = ACK_SCRIPT_MSG
	};

	AckScriptMsg() : window_(0) {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}

	uint8_t window_;
};

/**
 * Message containing script bytes for the stepper to run.
 * id1_ is sequence number.
 */
template <typename Allocator>
struct DataScriptMsg : Message<Allocator>
{
	enum {
		PAYLOAD_SIZE = 256,
		ID0 = DATA_SCRIPT_MSG
	};

	DataScriptMsg() {
		this->payloadSize(PAYLOAD_SIZE);
		this->id0(ID0);
	}

	uint8_t scriptData_[PAYLOAD_SIZE];
};

}}

#endif
