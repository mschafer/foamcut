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
#include "SList.hpp"
#include "LockGuard.hpp"
#include "IDevice.hpp"

namespace stepper { namespace device {
    
namespace detail {

struct Chunk : public SListBaseHook<> {
    enum {
        POOL_INDEX_OFFSET = 0,
        ALLOCATED_OFFSET = 1,
        IS_ALLOCATED = 1
    };

    uint8_t poolIndex() const { return header()[POOL_INDEX_OFFSET]; }
    void poolIndex(uint8_t v) { header()[POOL_INDEX_OFFSET] = v; }

    bool allocated() const { return header()[ALLOCATED_OFFSET] == 1; }
    void allocated(bool v) { header()[ALLOCATED_OFFSET] = (uint8_t)v; }

    const uint8_t *header() const {
        const uint8_t *p = reinterpret_cast<const uint8_t*>(this-1);
        return p;
    }

    uint8_t *header() {
        return const_cast<uint8_t*>(static_cast<const Chunk*>(this)->header());
    }
};

}

/**
 * A fast memory allocator based on a collection of pools.
 */
class MemoryAllocator
{
public:
    enum {
        MAX_POOLS = 4,
        MAX_CHUNK_SIZE = 2040
    };

    /**
     * Construct the MemoryAllocator.
     * \param poolSizes An array of sizes for the individual pools.  Each entry must be an 
     * integral multiple of sizeof(void*) and the list must be monotonically increasing.
     * The memory holding the list must persist for the lifetime of the MemoryAllocator--it isn't copied.
     * \param poolCount The number of entries in poolSizes must be less than MAX_POOLS.
     * \param block Address of memory block for segregation into pools.
     * \param blockSize Size in bytes of \em block.
     */
    explicit MemoryAllocator(const uint16_t *poolSizes, uint8_t poolCnt, void *block, size_t blockSize) :
    poolSizes_(poolSizes), blockAvailable_(blockSize), 
    blockStart_(reinterpret_cast<uint8_t*>(block)), poolCount_(poolCnt)
    {
        ///\todo preallocate and free some of each size but the smallest
        assert((size_t)blockStart_ % sizeof(void*) == 0);
        assert(poolCount_ <= MAX_POOLS && poolCount_ > 0);
        blockAvailable_ -= blockAvailable_ % sizeof(void*);
        for (uint8_t i=0; i<poolCount(); ++i) {
            assert(poolSize(i) % sizeof(void*) == 0);
            if (i>0) assert(poolSize(i) > poolSize(i-1) && poolSize(i) <= MAX_CHUNK_SIZE);
            else assert(poolSize(i) >= sizeof(void*));
        }
    }

    ~MemoryAllocator() {
        for (uint8_t i=0; i<poolCount(); ++i) {
            freeLists_[i].clear();
        }
    }

    uint16_t poolSize(uint8_t poolIdx) const {
        assert(poolIdx <= MAX_POOLS);
        return poolSizes_[poolIdx]; 
    }
    uint8_t poolCount() const { return poolCount_; }

    /**
     * Allocate a chunk of at least the requested size.
     * \return Pointer to allocated chunk or NULL.
     */
    void *alloc(size_t request) throw() {

        // try to find a large enough chunk on one of the free lists
        uint8_t poolIdx = whichPool(request);
        if (poolIdx == poolCount()) {
        	///\todo getStatusFlags().set(StatusFlags::MEMORY_ALLOCATION_FAILED);
        	return NULL;
        }
        LockGuard guard(mtx_);

        for (uint8_t i=poolIdx; i<poolCount(); ++i) {
            if (!freeLists_[i].empty()) {
                detail::Chunk &c = freeLists_[i].popFront();
                assert(c.poolIndex() == i && !c.allocated());
                c.allocated(true);
                return &c;
            }
        }

        void *ret = allocFromBlock(poolIdx);
        if (ret == NULL) {
        	///\todo getStatusFlags().set(StatusFlags::MEMORY_ALLOCATION_FAILED);
        }
        return ret;
    }

    /**
     * Return a chunk to the MemoryAllocator.
     * \param chunk Must point to a chunk allocated from the MemoryAllocator.
     */
    void free(void *p) {
        detail::Chunk *c = reinterpret_cast<detail::Chunk*>(p);
    	uint8_t poolIdx = c->poolIndex();
    	assert (poolIdx < poolCount() && c->allocated() == true);
        {
            LockGuard guard(mtx_);
            c->allocated(false);
            // 0 out memory used by next pointer or safe linking list insertion will fail
            c->nextOf(NULL);
            freeLists_[poolIdx].pushFront(*c);
        }
    }

    /** \return The actual capacity of the chunk. */
    uint16_t capacity(const void *p) const {
        const detail::Chunk *c = reinterpret_cast<const detail::Chunk*>(p);
    	uint8_t poolIdx = c->poolIndex();
    	assert (poolIdx < poolCount());
        return poolSize(poolIdx);
    }

    /** \return The maximum size chunk that this MemoryAllocator can allocate. */
    uint16_t maxCapacity() const { return poolSize(poolCount()-1); }
    
private:
    BaseSList<detail::Chunk> freeLists_[MAX_POOLS];
    Lock mtx_;
    const uint16_t *poolSizes_;
    size_t blockAvailable_;
    uint8_t *blockStart_;
    uint8_t poolCount_;

    uint8_t whichPool(size_t requestSize) {
        uint8_t r = 0;
        while (poolSize(r) < requestSize && r < poolCount()) ++r;
        return r;
    }

    void *allocFromBlock(uint8_t poolIdx) {
        // pad size for header
        size_t request = poolSize(poolIdx) + sizeof(void*);
        if (blockAvailable_ < request) return NULL;
        blockAvailable_ -= request;
        detail::Chunk *c = reinterpret_cast<detail::Chunk*>(blockStart_ + blockAvailable_ + sizeof(void*));
        c->poolIndex(poolIdx);
        c->allocated(true);
        return c;
    }
};

}}

#endif
