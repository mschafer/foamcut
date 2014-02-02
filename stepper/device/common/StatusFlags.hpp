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

}}

#endif
