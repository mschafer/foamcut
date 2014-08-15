/* (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#ifndef stepper_device_StatusFlags_hpp
#define stepper_device_StatusFlags_hpp

#include <stdint.h>

namespace stepper { namespace device {

enum ErrorCode {
	SUCCESS = 0,
	FATAL_ERROR = 1,
	RESOURCE_UNAVAILABLE = 2
};

struct StatusFlags
{
    enum Flag {
        MEMORY_ALLOCATION_FAILED = 0x01,
        COMM_SEND_FAILED         = 0x02,
        CONNECTED                = 0x04,
        ENGINE_RUNNING           = 0x08,
        STICKY_FLAGS =  CONNECTED | ENGINE_RUNNING
    };

    StatusFlags();

    void set(Flag b) {
    	if (!get(b)) {
    		flags_ |= (uint32_t)b;
    		updated_ = true;
    	}
    }

    void clear(Flag b) {
    	if (get(b)) {
    		flags_ &= ~b;
    		updated_ = true;
    	}
    }

    bool get(Flag b) const { return (flags_ & (uint32_t)b) != 0; }

    /** Clears transient flags but leaves sticky ones. */
    void clear() {
    	flags_ &= ~STICKY_FLAGS;
    	updated_ = false;
    }

    /** Clears all flags. */
    void reset() { flags_ = 0; updated_ = false; }

    bool updated() {
    	bool r = updated_;
    	updated_ = false;
    	return r;
    }

    static StatusFlags &instance();

private:
    volatile uint32_t flags_;
    volatile bool updated_;
};

/** Fatal error codes. */
enum FatalError {
	SCRIPT_BUFFER_OVERFLOW_ERROR = 0,
	STEP_QUEUE_UNDERFLOW_ERROR   = 1,
	MEMORY_ALLOCATION_ERROR      = 2,
	ILLEGAL_SCRIPT_DATA          = 3,
	UNRECOGNIZED_MESSAGE         = 4,
	STEPPER_ALREADY_RUNNING      = 5
};

extern const char *FatalErrorMessage[6];

/**
 * This function is called whenever a fatal error occurs.
 * The stepper is stopped and all state is cleared.
 * A message is sent back to the host with the error code if possible.
 */
void error(FatalError ec);

}}

#endif
