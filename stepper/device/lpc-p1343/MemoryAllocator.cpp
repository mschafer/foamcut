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
#include <MemoryAllocator.hpp>
#include <new>

namespace stepper { namespace device {

struct Node
{
	Node() : next_(NULL) {}
	Node *next_;
};
    
    
MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}
    
MemoryAllocator::MemoryAllocator() : lastAlloc_(block_ + CHUNK_SIZE*CHUNK_COUNT), free_(NULL)
{
	Node *nextFree = new (block_) Node();
	uint8_t *end = reinterpret_cast<uint8_t*>(nextFree) + MemoryAllocator::CHUNK_SIZE;
	while (end < block_ + CHUNK_SIZE*CHUNK_COUNT) {
		nextFree->next_ = free_;
		free_ = nextFree;
		nextFree = new (end) Node();
		end = reinterpret_cast<uint8_t*>(nextFree) + MemoryAllocator::CHUNK_SIZE;
	}
}

void *MemoryAllocator::alloc(size_t request) throw() 
{
	Node *ret = NULL;
	if (request > CHUNK_SIZE) {
		return NULL;
	} else {
        do {
            ret = reinterpret_cast<Node*>(__ldrex(reinterpret_cast<volatile uint32_t*>(&free_)));
            if (ret == NULL) {
            	__clrex();
            	return NULL;
            }
        } while (__strex(reinterpret_cast<unsigned int>(ret->next_), reinterpret_cast<volatile uint32_t*>(&free_)) == 1);
        return reinterpret_cast<void*>(ret);
	}
}

void MemoryAllocator::free(void *p)
{
	Node *n = new (p) Node();
    do {
        Node *head = reinterpret_cast<Node*>(__ldrex(reinterpret_cast<volatile uint32_t*>(&free_)));
        n->next_ = head;
    } while (__strex(reinterpret_cast<unsigned int>(n), reinterpret_cast<volatile uint32_t*>(&free_)) == 1);
}

}}
