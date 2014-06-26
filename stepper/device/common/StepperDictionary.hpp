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

#include <Message.hpp>

namespace stepper { namespace device {

enum {
	GO_MSG,
	PAUSE_MSG,
	RESET_MSG,
	SPEED_ADJUST_MSG,
	INIT_SCRIPT_MSG,
	ACK_SCRIPT_MSG,
	DATA_SCRIPT_MSG,
	STEPPER_MESSAGE_ID = 2
};

struct StepperMessage : public Message
{
	StepperMessage() {
		id(STEPPER_MESSAGE_ID);
	}
};

/** Starts the engine executing the currently cached script. */
struct GoMsg : StepperMessage
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = GO_MSG
	};

	GoMsg() {
		payloadSize(PAYLOAD_SIZE);
		function(FUNCTION);
	}
};

/** Pauses the engine executing the currently cached script, resumes on GoMsg. */
struct PauseMsg : StepperMessage
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = PAUSE_MSG
	};

	PauseMsg() {
		payloadSize(PAYLOAD_SIZE);
		function(FUNCTION);
	}
};

/** Stops engine execution and empties the script buffer. */
struct ResetMsg : StepperMessage
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = RESET_MSG
	};

	ResetMsg() {
		payloadSize(PAYLOAD_SIZE);
		function(FUNCTION);
	}
};

/** Sets the speed scaling. */
struct SpeedAdjustMsg : StepperMessage
{
	enum {
		PAYLOAD_SIZE = sizeof(uint32_t),
		FUNCTION = SPEED_ADJUST_MSG
	};

	SpeedAdjustMsg() : speedAdjust_(0) {
		payloadSize(PAYLOAD_SIZE);
		function(FUNCTION);
	}

	uint32_t speedAdjust_;
};



/**
 * Sent when the engine is done processing a DataScriptMsg.
 */
struct AckScriptMsg : StepperMessage
{
	enum {
		PAYLOAD_SIZE = 0,
		FUNCTION = ACK_SCRIPT_MSG
	};

	AckScriptMsg() {
		payloadSize(PAYLOAD_SIZE);
		function(FUNCTION);
	}
};

/**
 * Message containing script bytes for the stepper to run.
 * The size of this message is up to the maximum as set by the device.
 */
struct DataScriptMsg : StepperMessage
{
	enum {
		FUNCTION = DATA_SCRIPT_MSG,
		IN_FLIGHT_COUNT = 10 ///\< Number of unacknowledged messages that host can send before waiting
	};

	explicit DataScriptMsg(uint16_t size) {
		function(FUNCTION);
		payloadSize(size);
	}

	DataScriptMsg() {
		function(FUNCTION);
	}
};

}}

#endif
