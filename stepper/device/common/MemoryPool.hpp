#ifndef stepper_device_MemoryPool_hpp
#define stepper_device_MemoryPool_hpp

#include <stdint.h>
#include <stdlib.h>
#include <Platform.hpp>
#include "SegregatedStorage.hpp"
#include "LockGuard.hpp"

namespace stepper { namespace device {
    
/**
 * A fast memory allocator that divides a statically allocated block of
 * memory into small number of fixed size pools.
 */
template<uint8_t NUM_POOLS, typename size_type>
class MemoryPool
{
public:

    explicit MemoryPool(const size_type poolSizes[NUM_POOLS], uint8_t *block, size_t blockSize) :
    blockStart_(block), blockAvailable_(blockSize)
    {
        for (uint8_t i=0; i<NUM_POOLS; ++i) {
            assert(poolSizes[i] % sizeof(void*) == 0);
            poolSizes_[i] = poolSizes[i] + sizeof(void*);
        }
    }

    void *alloc(size_type request) {
        uint8_t poolIdx = whichPool(request);
        if (poolIdx == NUM_POOLS) return NULL;
        LockGuard guard(mtx_);

        void *ret = NULL;
        for (uint8_t i=poolIdx; i<NUM_POOLS; ++i) {
            if (!pool_[i].empty()) {
                return adjustAlloc(pool_[i].alloc(), i);
            }
        }

        ret = takeFromBlock(request);
        if (ret != NULL) {
            return adjustAlloc(ret, poolIdx);
        } else {
            return NULL;
        }
    }

    void free(void *chunk) {
        uint8_t poolIdx = poolIndex(chunk);
        {
            LockGuard guard(mtx_);
            pool_[poolIdx].free(chunk);
        }
    }
    
    size_t capacity(void *chunk) {
        return poolSizes_[poolIndex(chunk)] - sizeof(void*);
    }

    size_t maxSize() const { return poolSizes_[NUM_POOLS-1] - sizeof(void*); }

private:
    platform::Lock mtx_;
    SegregatedStorage<size_type> pool_[NUM_POOLS];
    uint8_t *blockStart_;
    size_type blockAvailable_;
    size_type poolSizes_[NUM_POOLS]; 

    uint8_t whichPool(size_type request) {
        uint8_t r = 0;
        while (poolSizes_[r] < request && r < NUM_POOLS) ++r;
        return r;
    }

    uint8_t *takeFromBlock(size_type request) {
        assert(request % sizeof(void*) == 0);
        if (blockAvailable_ < request) return NULL;
        blockAvailable_ -= request;
        return blockStart_ + blockAvailable_;
    }

    /** stash index for pool in allocated block and move pointer forward. */
    void *adjustAlloc(void *a, uint8_t poolIdx) {
        uint8_t *p = static_cast<uint8_t*>(a);
        *p = poolIdx;
        return p + sizeof(void*);
    }

    uint8_t poolIndex(void *chunk) {
    	uint8_t *p = static_cast<uint8_t*>(chunk);
    	p -= sizeof(void*);
    	uint8_t poolIdx = *p;
    	assert (poolIdx < NUM_POOLS);
    	return poolIdx;
    }
};

struct DeviceMessageAllocator
{
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	static void *malloc(const size_type bytes) {
		Platform::MemoryPool_type &ma = Platform::getMemoryPool();
		return ma.alloc(bytes);
	}

	static void free(void * const block) {
		Platform::MemoryPool_type &ma = Platform::getMemoryPool();
		ma.free(block);
	}
};

}}

#endif
