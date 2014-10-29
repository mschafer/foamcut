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
#include <StatusFlags.hpp>

namespace stepper { namespace device {
    
class MemoryAllocator
{
public:
    static MemoryAllocator &instance();

    void *alloc(size_t request) throw();
    void free(void *p);

    size_t capacity(void const *p) const { return maxCapacity(); }

    size_t maxCapacity() const { return CHUNK_SIZE; }

private:
    enum {
        CHUNK_SIZE = 64,
        CHUNK_COUNT = 16
    };

    MemoryAllocator();
    ~MemoryAllocator() {}

    uint8_t *volatile lastAlloc_;
    struct Node *volatile free_;        
    uint8_t block_[CHUNK_SIZE*CHUNK_COUNT];
};

}}

#endif
