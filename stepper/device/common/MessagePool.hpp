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
#ifndef stepper_device_MessagePool_hpp
#define stepper_device_MessagePool_hpp

#include <stddef.h>
#include <assert.h>
#include "Message.hpp"
#include "LockGuard.hpp"

namespace stepper { namespace device {

/**
 * Sizes for individual free lists must be multiple of pointer size
 * and sorted in ascending order.
 */
const int POOL_COUNT = 3;
const uint16_t POOL_PAYLOAD_SIZES[3] = {4, 16, 256};

/**
 * Thread safe pool for allocation of MessageBuffers.
 * Implemented as a collection of pools to support
 * efficient allocation of different sizes.
 *
 * \tparam lock_type must be a default constructible synchronization object
 * with lock() and unlock() methods.  It does not need to be recursive
 * or timed.
 */
template <typename lock_type>
class MessagePool
{
public:

    /**
     * \param block A pointer to a block of memory for use as a heap
     * by the pool.
     *
     * \param blockSize The size in bytes of \em block.
     */
    MessagePool(uint8_t *block, size_t blockSize)  :
    	blockStart_(block), blockEnd_(block+blockSize)
    {
        for (int i=0; i<POOL_COUNT; ++i) {
            assert(sizes_[i] % sizeof(void*) == 0); ///\< chunk alignment requirement
            if (i>0) assert(sizes_[i] > sizes_[i-1]);
            sizes_[i] = POOL_PAYLOAD_SIZES[i] + static_cast<uint16_t>(sizeof(MessageBuffer));
            allocated_[i] = freed_[i] = 0;
            freeList_[i] = NULL;
        }

        totalPoolSize_ = blockSize;
    }

    /** Whoever created MessagePool should free the block. */
    ~MessagePool() { }

    /**
     * Allocate a \sa CommBuffer with a minimum number of payload 
     * bytes available.
     */
    MessageBuffer *alloc(uint16_t payloadSize) {
        uint16_t totalSize = sizeof(MessageBuffer) + payloadSize;
        uint8_t idx = whichList(totalSize);
        if (idx == POOL_COUNT) return NULL;

        LockGuard<lock_type> guard(mtx_);

        // get a block from the freeList
        // check all lists starting with the smallest one that satisfies the request
        MessageBuffer *ret;
        for (uint8_t fidx=idx; fidx<POOL_COUNT; ++fidx) {
            ret = freeList_[fidx];
            if (ret != NULL) {
                freeList_[idx] = ret->next();
                --freed_[idx];
                ++allocated_[idx];
                return ret;
            }
        }

        // nothing available in free list so try to take a piece of the block
        ret = reinterpret_cast<MessageBuffer*>(take(sizes_[idx]));
        if (ret != NULL) {
            ret->poolTag(idx);
            ++allocated_[idx];
        }
        return ret;
    }

    /** Place the chunk \em mb on the appropriate free list. */
    void free(MessageBuffer *mb) {
        assert(mb != NULL);
        LockGuard<lock_type> guard(mtx_);
        uint8_t idx = mb->poolTag();
        mb->next(freeList_[idx]);
        freeList_[idx] = mb;
        ++freed_[idx];
        --allocated_[idx];
    }

    /** \return The payload capacity in bytes of \em mb. */
    uint16_t payloadCapacity(MessageBuffer *mb) {
        uint16_t totalSize = sizes_[mb->poolTag()];
        return totalSize - sizeof(MessageBuffer);
    }

    /** \return the amount of memory left in the block that has not been segregated yet. */
    size_t unallocatedBlockMemory() const { return blockEnd_ - blockStart_; }

    /** \return An array containing the number of allocated chunks for each size. */
    const size_t *allocated() const { return allocated_; }

    /** \return An array containing the number of chunks in each free list. */
    const size_t *freed() const { return freed_; }

    /** Total size of all blocks added to this CBPool. */
    size_t totalPoolSize() const { return totalPoolSize_; }

private:
    lock_type mtx_;
    MessageBuffer *freeList_[POOL_COUNT];
    uint8_t *blockStart_;    ///\< start of block
    uint8_t *blockEnd_;      ///\< end of available memory, moved by \sa take

    uint16_t sizes_[POOL_COUNT];  ///\< size in bytes of chunks on each free list != payloadSize.
    size_t allocated_[POOL_COUNT];
    size_t freed_[POOL_COUNT];
    size_t totalPoolSize_;

    /**
     * Returns the index of the list that holds blocks at least as big
     * as size.  Returns MAX_NUM_SIZES if size is too big.
     */
    uint8_t whichList(uint16_t size) {
        uint8_t idx = 0;
        while (idx < POOL_COUNT && size > sizes_[idx]) {
            idx++;
        }
        return idx;
    }

    /** Take \em size bytes from the end of the block if available. */
    uint8_t *take(uint16_t size) {
        if (blockEnd_ - blockStart_ >= size) {
            blockEnd_ -= size;
            return blockEnd_;
        } else {
            return NULL;
        }
    }

    // non-copyable, non-assignable
    MessagePool(const MessagePool &cpy);
    MessagePool &operator=(const MessagePool &cpy);
};

}}

#endif
