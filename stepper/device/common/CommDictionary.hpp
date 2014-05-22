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
#ifndef stepper_device_CommDictionary_hpp
#define stepper_device_CommDictionary_hpp

#include "Message.hpp"
#include "StatusFlags.hpp"

namespace stepper { namespace device {
 
enum {
    PING_MSG,
    PING_RESPONSE_MSG,
    CONNECT_MSG,
    CONNECT_RESPONSE_MSG,
    HEARTBEAT_MSG,
    HEARTBEAT_RESPONSE_MSG,
    IN_TEST_START_MSG,
    TEST_DATA_MSG,
    OUT_TEST_START_MSG,
    OUT_TEST_RESULT_MSG,
    TEST_CONCLUDED_MSG,
    COMMUNICATOR_ID = 0
};

class PingMsg : public Message
{
public:
    enum {
        FUNCTION = PING_MSG,
        PAYLOAD_SIZE = 0
    };

    PingMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
};

class PingResponseMsg : public Message
{
public:
    enum {
        FUNCTION = PING_RESPONSE_MSG,
        PAYLOAD_SIZE = 0
    };

    PingResponseMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
};

class ConnectMsg : public Message
{
public:
    enum {
        FUNCTION = CONNECT_MSG,
        PAYLOAD_SIZE = 0
    };

    ConnectMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
};

class ConnectResponseMsg : public Message
{
public:
    enum {
        FUNCTION = CONNECT_RESPONSE_MSG,
        PAYLOAD_SIZE = sizeof(uint16_t)
    };

    ConnectResponseMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
        maxPayload_ = maxPayloadCapacity();    
    }

    uint16_t maxPayload_;
};

/** Sent periodically by the Host. */
class HeartbeatMsg : public Message
{
public:
    enum {
        FUNCTION = HEARTBEAT_MSG,
        PAYLOAD_SIZE = 0
    };

    HeartbeatMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
};

/** Sent periodically by the Host. */
class HeartbeatResponseMsg : public Message
{
public:
    enum {
        FUNCTION = HEARTBEAT_RESPONSE_MSG,
        PAYLOAD_SIZE = sizeof(StatusFlags)
    };

    HeartbeatResponseMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }

    StatusFlags statusFlags_;
};

/**
 * Instruct the target to begin a test sending messages to the host.
 * The target will respond with the specified number of TestDataMsg's
 * that are each the specifed size.  A TestConcludedMsg will be sent last.
 */
class InTestStartMsg : public Message
{
public:
    enum {
        FUNCTION = IN_TEST_START_MSG,
        PAYLOAD_SIZE = 2 * sizeof(uint16_t)
    };

    InTestStartMsg() : testMsgCount_(0), testMsgSize_(0) {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
    uint16_t testMsgCount_;
    uint16_t testMsgSize_;
};

class TestDataMsg : public Message
{
public:
    enum {
        FUNCTION = TEST_DATA_MSG
    };

    explicit TestDataMsg(uint16_t size) {
    	payloadSize(size);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
private:
    TestDataMsg();
};

/**
 * Inform the target that the host is starting a test to send data to it.
 * The host will send the specified number of TestDataMsg's
 * that are each the specifed size followed by a TestConcludedMsg.
 * The target is expected to replay with a OutTestResultsMsg at the end.
 */
class OutTestStartMsg : public Message
{
public:
    enum {
        FUNCTION = OUT_TEST_START_MSG,
        PAYLOAD_SIZE = 2 * sizeof(uint16_t)
    };

    OutTestStartMsg() : testMsgCount_(0), testMsgSize_(0) {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }
    uint16_t testMsgCount_;
    uint16_t testMsgSize_;
};

class OutTestResultMsg : public Message
{
public:
    enum {
        FUNCTION = OUT_TEST_RESULT_MSG,
        PAYLOAD_SIZE = 1
    };

    enum {
        RESULT_OK = 0
    };

    OutTestResultMsg() {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
        result_ = RESULT_OK;
    }

    uint8_t result_;
};

/**
 * Sent by either host or target after the specified number of TestDatMsg's
 * have been sent.
 */
class TestConcludedMsg : public Message
{
public:
    enum {
        FUNCTION = TEST_CONCLUDED_MSG,
        PAYLOAD_SIZE = 1
    };

    TestConcludedMsg() : pad_(0) {
    	payloadSize(PAYLOAD_SIZE);
    	id(COMMUNICATOR_ID);
    	function(FUNCTION);
    }

    // make this message same size as response for buffer reuse.
    uint8_t pad_;
};

}}

#endif
