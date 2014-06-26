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

struct StatusFlags
{
    enum Bit {
        MEMORY_ALLOCATION_FAILED = 0x01,
        COMM_SEND_FAILED         = 0x02,
        UNRECOGNIZED_MSG         = 0x04,
        MSG_ID_OUT_OF_RANGE      = 0x08
    };

    void set(Bit b) { bits_ |= (uint32_t)b; }
    bool get(Bit b) const { return (bits_ & (uint32_t)b) != 0; }

    void clear() { bits_ = 0; }

    uint32_t bits_;
};

enum ErrorCode {
	SCRIPT_BUFFER_OVERFLOW_ERROR = 1,
	STEP_QUEUE_UNDERFLOW_ERROR = 2,
	MEMORY_ALLOCATION_ERROR = 3,
	ILLEGAL_SCRIPT_DATA = 4,
	UNRECOGNIZED_MESSAGE = 5
};

enum WarningCode {
	TIMER_ALREADY_RUNNING = 1
};

/**
 * This function is called whenever a fatal error occurs.
 * The stepper is stopped and all state is cleared.
 * A message is sent back to the host with the error code if possible.
 */
void error(ErrorCode ec);

void warning(WarningCode wc);

void info();

}}

#endif
