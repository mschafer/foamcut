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

/** \file
 * DO NOT DIRECTLY INCLUDE THIS FILE!
 * This file contains definitions for the messages exchanged between host and device.
 * It is meant to be shared between host and device and should be included inside
 * another namespace like \sa Message.hpp.
 */

enum {
	GO_MSG,
	PAUSE_MSG,
	RESET_MSG,
	SPEED_ADJUST_MSG,
	INIT_SCRIPT_MSG,
	ACK_SCRIPT_MSG,
	DATA_SCRIPT_MSG,
	STEPPER_ID = 1
};

/** Starts the engine executing the currently cached script. */
struct GoMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = GO_MSG
	};

	GoMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}
};

/** Stops the engine executing the currently cached script, resumes on GoMsg. */
struct PauseMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = PAUSE_MSG
	};

	PauseMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}
};

struct ResetMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = RESET_MSG
	};

	ResetMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}
};

struct SpeedAdjustMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(uint32_t),
		FUNCTION = SPEED_ADJUST_MSG
	};

	SpeedAdjustMsg() : speedAdjust_(0) {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}

	uint32_t speedAdjust_;
};



/**
 * Initialize the engine in preparation for receiving a script to run.
 * Device responds with ACK_SCRIPT
 */
struct InitScriptMsg : Message
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = INIT_SCRIPT_MSG
	};

	InitScriptMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}
};

/**
 * Response to InitScriptMsg or a sequence of ScriptMsg
 * id1_ is the ScriptMsg sequence number being Ack'd.
 * payload byte is number of ScriptMsg buffer available.
 */
struct AckScriptMsg : Message
{
	enum {
		PAYLOAD_SIZE = sizeof(uint8_t),
		FUNCTION = ACK_SCRIPT_MSG
	};

	AckScriptMsg() : window_(0) {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}

	uint8_t window_;
};

/**
 * Message containing script bytes for the stepper to run.
 * id1_ is sequence number.
 */
struct DataScriptMsg : Message
{
	enum {
		PAYLOAD_SIZE = 256,
		FUNCTION = DATA_SCRIPT_MSG
	};

	DataScriptMsg() {
		payloadSize(PAYLOAD_SIZE);
		id(STEPPER_ID);
		function(FUNCTION);
	}

	uint8_t scriptData_[PAYLOAD_SIZE];
};