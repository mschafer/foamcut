#ifndef stepper_device_MemoryPool_hpp
#define stepper_device_MemoryPool_hpp

#include <stdint.h>
#include <stdlib.h>
#include <Platform.hpp>
#include "SList.hpp"
#include "LockGuard.hpp"

namespace stepper { namespace device {

namespace detail {

struct Chunk : public SListHook<>
{
	enum {
		POOL_INDEX_OFFSET = 0,
		ALLOCATED_OFFSET = 1
	};

	uint8_t poolIndex() { return header()[POOL_INDEX_OFFSET]; }
	void poolIndex(uint8_t v) { header()[POOL_INDEX_OFFSET] = v; }

	bool allocated() { return header()[ALLOCATED_OFFSET] == 1; }
	void allocated(bool v) { header()[ALLOCATED_OFFSET] = (uint8_t)v; }

	uint8_t *header() {
		uint8_t *p = reinterpret_cast<uint8_t*>(this-1);
		return p;
	}
};

}
    
/**
 * A fast memory allocator that divides a statically allocated block of
 * memory into small number of fixed size pools.
 */
template<uint8_t NUM_POOLS>
class MemoryPool
{
public:

	/**
	 * Smallest poolSize must be >= sizeof(void*)
	 * All poolSizes must be integral multiples of sizeof(void*)
	 * poolSizes must be monotonically increasing
	 */
    explicit MemoryPool(const size_t poolSizes[NUM_POOLS], uint8_t *block, size_t blockSize) :
    blockStart_(block), blockAvailable_(blockSize)
    {
        assert((size_t)blockStart_ % sizeof(void*) == 0);
        blockAvailable_ -= blockAvailable_ % sizeof(void*);
        for (uint8_t i=0; i<NUM_POOLS; ++i) {
            assert(poolSizes[i] % sizeof(void*) == 0);
            if (i>0) assert(poolSizes[i] > poolSizes[i-1]);
            else assert(poolSizes[i] >= sizeof(void*));
        }
    }

    void *alloc(size_t request) {
        uint8_t poolIdx = whichPool(request);
        if (poolIdx == NUM_POOLS) return NULL;

        LockGuard guard(mtx_);
        for (uint8_t i=poolIdx; i<NUM_POOLS; ++i) {
            if (!freeLists_[i].empty()) {
            	detail::Chunk &c  = freeLists_[i].popFront();
                assert(c.poolIndex() == i  && !c.allocated());
                c.allocated(true);
                return &c;
            }
        }

        return allocFromBlock(poolIdx);
    }

    void free(void *p) {
        detail::Chunk *c = reinterpret_cast<detail::Chunk*>(p);
    	uint8_t poolIdx = c->poolIndex();
    	assert (poolIdx < NUM_POOLS && c->allocated() == true);
        {
            LockGuard guard(mtx_);
            c->allocated(false);
            c->nextOf(NULL);
            freeLists_[poolIdx].pushFront(*c);
        }
    }
    
    size_t capacity(void *p) {
        detail::Chunk *c = reinterpret_cast<detail::Chunk*>(p);
        return poolSizes_[c->poolIndex()];
    }

    size_t maxSize() const { return poolSizes_[NUM_POOLS-1]; }

private:
    platform::Lock mtx_;
    SList<detail::Chunk> freeLists_[NUM_POOLS];
    uint8_t *blockStart_;
    size_t blockAvailable_;
    size_t poolSizes_[NUM_POOLS];

    uint8_t whichPool(size_t request) {
        uint8_t r = 0;
        while (poolSizes_[r] < request && r < NUM_POOLS) ++r;
        return r;
    }

    void *allocFromBlock(uint8_t poolIdx) {
    	size_t request = poolSizes_[poolIdx] + sizeof(void*);
    	if (blockAvailable_ < request) return NULL;
        blockAvailable_ -= request;
        detail::Chunk *c = reinterpret_cast<detail::Chunk*>(blockStart_ + blockAvailable_ + sizeof(void*));
        c->poolIndex(poolIdx);
        c->allocated(true);
        return c;
    }
};

struct DeviceMessageAllocator
{
	static void *malloc(const size_t bytes) {
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
