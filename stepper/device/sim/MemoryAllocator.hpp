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
#ifndef stepper_device_MemoryAllocator_hpp
#define stepper_device_MemoryAllocator_hpp

#include <stdint.h>
#include <stdlib.h>
#include <boost/pool/singleton_pool.hpp>
#include <StatusFlags.hpp>

namespace stepper { namespace device {
    
/**
 * Implement MemoryAllocator concept with a wrapper around boost::singleton_pool
 */
class MemoryAllocator
{
public:
    /// implemented in HAL.cpp
    static MemoryAllocator &instance();

    void *alloc(size_t request) throw() {
        if (request <= MaxPool::requested_size) {
            return MaxPool::malloc();
        } else {
        	StatusFlags::instance().set(StatusFlags::MEMORY_ALLOCATION_FAILED);
            return NULL;
        }
    }

    void free(void *p) { MaxPool::free(p); }

    size_t capacity(void const *p) const { return maxCapacity(); }

    size_t maxCapacity() const { return MaxPool::requested_size; }

private:
    enum {
        POOL_SIZE = 64
    };

    typedef boost::singleton_pool<MemoryAllocator, POOL_SIZE> MaxPool;

    MemoryAllocator() {}
    ~MemoryAllocator() {}
};

}}

#endif
