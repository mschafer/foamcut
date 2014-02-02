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
#include "DeviceMessages.hpp"
#include "MemoryAllocator.hpp"
#include "IDevice.hpp"

namespace stepper { namespace device {

void *Message::operator new(size_t size)
{
    MemoryAllocator &ms = getMemoryAllocator();
    void *ret = ms.alloc(size);
    if (ret == NULL) throw std::bad_alloc();
    return ret;
}

void Message::operator delete(void *ptr) throw ()
{
    MemoryAllocator &ms = getMemoryAllocator();
    ms.free(ptr);
}

void *Message::operator new(size_t size, const std::nothrow_t &nothrow_value) throw()
{
    MemoryAllocator &ms = getMemoryAllocator();
    return ms.alloc(size);
}

void Message::operator delete (void* ptr, const std::nothrow_t& nothrow_constant) throw()
{
    MemoryAllocator &ms = getMemoryAllocator();
    ms.free(ptr);
}

Message *Message::alloc(uint16_t payloadSize)
{
    MemoryAllocator &ms = getMemoryAllocator();
    void *p = ms.alloc(payloadSize + sizeof(Message));
    return new (p) Message;
}

uint16_t Message::payloadCapacity() const
{
    MemoryAllocator &ms = getMemoryAllocator();
    return ms.capacity(this) - sizeof(Message);
}

uint16_t Message::maxPayloadCapacity()
{
    MemoryAllocator &ms = getMemoryAllocator();
    return ms.maxCapacity() - sizeof(Message);
}

}}
