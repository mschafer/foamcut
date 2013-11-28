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
#ifndef stepper_device_Message_hpp
#define stepper_device_Message_hpp

#include <stdint.h>
#include <new>
#include "SList.hpp"

namespace stepper { namespace device {

struct MessageHeader
{
	uint16_t payloadSize_;
	uint8_t id0_;
	uint8_t id1_;
};

template <typename Allocator>
class Message : public SListHook<>
{
public:
	typedef Allocator allocator_type;

	enum {
		HEADER_SIZE = 4,
		PAYLOAD_SIZE_OFFSET = 0,
		ID0_OFFSET = 2,
		ID1_OFFSET = 3
	};

	Message();
	~Message();

	void *operator new(size_t size) {
		void *ret = Allocator::malloc(size);
		if (ret == NULL) {
			throw std::bad_alloc();
		}
		return ret;
	}

	void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) throw() {
		return Allocator::malloc(size);
	}

	void* operator new (std::size_t size, void* ptr) throw() { return ptr; }

	void operator delete (void* ptr) throw() {
		Allocator::free(ptr);
	}

	void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) throw() {
		Allocator::free(ptr);
	}

	static Message *alloc(uint16_t payloadSize) {
		void *b = Allocator::malloc(payloadSize + sizeof(Message));
		Message *ret = NULL;
		if (b != NULL) {
			ret = reinterpret_cast<Message*>(b);
		}
		return ret;
	}

	void* operator new[] (std::size_t size) { assert(false); return NULL; }
	void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value) throw() { assert(false); return NULL; }
	void* operator new[] (std::size_t size, void* ptr) throw() { assert(false); return NULL; }
	void operator delete[] (void* ptr) throw() { assert(false); }
	void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant) throw() { assert(false); }

	uint16_t payloadSize() const { return headerShort(PAYLOAD_SIZE_OFFSET); }
	void payloadSize(uint16_t val) { headerShort(val, PAYLOAD_SIZE_OFFSET); }

	uint8_t id0() const { return headerByte(ID0_OFFSET); }
	void id0(uint8_t val) { headerByte(val, ID0_OFFSET); }

	uint8_t id1() const { return headerByte(ID1_OFFSET); }
	void id1(uint8_t val) { headerByte(val, ID1_OFFSET); }

	void header(const MessageHeader &hdr) {
		payloadSize(hdr.payloadSize_);
		id0(hdr.id0_);
		id1(hdr.id1_);
	}

	uint8_t *payload() { return reinterpret_cast<uint8_t*>(this+1); }
	const uint8_t *payload() const { return reinterpret_cast<const uint8_t*>(this+1); }

	uint16_t transmitSize() const { return payloadSize() + HEADER_SIZE; }
	uint8_t *transmitStart() { return payload() - HEADER_SIZE; }

	/** Amount of memory to get from MemoryAllocator for a given payload size. */
	static uint16_t memoryNeeded(uint16_t payloadSize) { return sizeof(Message) + payloadSize; }

	uint8_t reserved_[sizeof(void*)];
private:

	uint8_t headerByte(size_t offset) const { return *(payload() - HEADER_SIZE + offset); }
	void headerByte(uint8_t val, size_t offset) { *(payload() - HEADER_SIZE + offset) = val; }
	uint16_t headerShort(size_t offset) const { return *(uint16_t*)(payload() - HEADER_SIZE + offset); }
	void headerShort(uint16_t val, size_t offset) { *(uint16_t*)(payload() - HEADER_SIZE + offset) = val; }

};

}}

#endif

