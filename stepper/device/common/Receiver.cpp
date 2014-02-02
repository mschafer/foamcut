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
#include "Receiver.hpp"
#include "DeviceMessages.hpp"
#include "IDevice.hpp"

namespace stepper { namespace device {

Receiver *Receiver::registry_[MAX_ID + 1] = { NULL };

Receiver *Receiver::findById(uint8_t id)
{
	assert(id <= MAX_ID);
    return registry_[id];
}

void Receiver::dispatch(Message *message)
{
    uint8_t destId = message->id();
    assert(destId <= MAX_ID);
    Receiver *dest = registry_[destId];
    if (dest) {
        dest->receiveMessage(message);
    } else {
        delete message;
        getStatusFlags().set(StatusFlags::MSG_ID_OUT_OF_RANGE);
    }
}

void Receiver::connectionChanged(bool c)
{
    for (uint8_t id=1; id<=MAX_ID; ++id) {
        if (registry_[id] != NULL) {
            registry_[id]->connected(c);
        }
    }
}

}}
