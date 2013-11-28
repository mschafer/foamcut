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
#ifndef stepper_device_SegregatedStorage_hpp
#define stepper_device_SegregatedStorage_hpp

#include <assert.h>

namespace stepper { namespace device {

/**
 * A pooling memory allocator.
 */
template <typename size_type=size_t>
class SegregatedStorage
{
public:
    SegregatedStorage() :  head_(0), count_(0) {}

    bool empty() const { return (head_ == 0); }
    
    void *alloc() {
        assert(!empty());
        void * const ret = head_;
        head_ = nextOf(head_);
        --count_;
        return ret;
    }
    
    void free(void *const chunk) {
        nextOf(chunk) = head_;
        head_ = chunk;
        ++count_;
    }
    
    size_t count() const { return count_; }

    void addBlock(void *start, size_type blockSize, size_type chunkSize) {
        assert(blockSize >= chunkSize && chunkSize % sizeof(void*) == 0);
        char *lastChunk = static_cast<char *>(start)
            + ((blockSize - chunkSize) / chunkSize) * chunkSize;
        nextOf(lastChunk) = head_;
        ++count_;
        if (lastChunk == start) {
            head_ = start;
            return;
        }
        
        for (char *iter = lastChunk-chunkSize; iter != start; 
            lastChunk = iter, iter -= chunkSize) {
            nextOf(iter) = lastChunk;
            ++count_;
        }
        nextOf(start) = lastChunk;    
        head_ = start;
    }
    
private:
    void *head_;
    size_t count_;
    
    static void *&nextOf(void *const ptr) { return *(static_cast<void **>(ptr));}
    
    SegregatedStorage(const SegregatedStorage &cpy);
    SegregatedStorage &operator=(const SegregatedStorage &assign);
};

}}

#endif
/* END_PUBLISHED */
