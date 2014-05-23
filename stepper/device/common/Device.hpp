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
#ifndef stepper_device_Device_hpp
#define stepper_device_Device_hpp

#include "Message.hpp"

namespace stepper { namespace device {

/**
 * Base class for a connected device.
 * Implements some basic self-test capabilities like ping response./
 */
class Device
{
public:
	enum {
		DEVICE_MESSAGE_ID       = 1
	};

	enum MessageFunctions {
	    PING_MSG                = 0,
	    PING_RESPONSE_MSG       = 1,
	    CONNECT_MSG             = 2,
	    CONNECT_RESPONSE_MSG    = 3,
	    HEARTBEAT_MSG           = 4,
	    HEARTBEAT_RESPONSE_MSG  = 5,
	    IN_TEST_START_MSG       = 6,
	    TEST_DATA_MSG           = 7,
	    OUT_TEST_START_MSG      = 8,
	    OUT_TEST_RESULT_MSG     = 9,
	    TEST_CONCLUDED_MSG      = 10
	};

	enum Status {
		NO_ERRORS = 0,
		UNRECOGNIZED_MESSAGE = 1
	};

	Device();
	~Device();

	/**
	 * Handle incoming messages for the device and run background processing.
	 * This needs to be called periodically even if there are no new messages
	 * in order for tests to work--just pass NULL.
	 *
	 * This method is not reentrant and should only be called from a single thread.
	 */
	void operator()(Message *m);

	static Device &instance();

private:
    enum TestState {
        NO_TEST,
        IN_TEST,
        OUT_TEST
    };

    void handleMessage(Message *m);
};

struct DeviceMessage : public Message
{
	DeviceMessage() {
		id(Device::DEVICE_MESSAGE_ID);
	}
};

struct PingMsg : public DeviceMessage
{
public:
    enum {
        FUNCTION = Device::PING_MSG,
        PAYLOAD_SIZE = 0
    };

    PingMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	function(FUNCTION);
    }
};

struct PingResponseMsg : public Message
{
public:
    enum {
        FUNCTION = Device::PING_RESPONSE_MSG,
        PAYLOAD_SIZE = 0
    };

    PingResponseMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	function(FUNCTION);
    }
};

}}

#endif
