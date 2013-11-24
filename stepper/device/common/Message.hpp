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
#include "SList.hpp"

namespace stepper { namespace device {

class Message : public SListHook<>
{
public:
	enum {
		HEADER_SIZE = 4,
		PAYLOAD_SIZE_OFFSET = 0,
		ID0_OFFSET = 2,
		ID1_OFFSET = 3
	};

	Message();
	~Message();

	uint16_t payloadSize() const { return headerShort(PAYLOAD_SIZE_OFFSET); }
	void payloadSize(uint16_t val) { headerShort(val, PAYLOAD_SIZE_OFFSET); }

	uint8_t id0() const { return headerByte(ID0_OFFSET); }
	void id0(uint8_t val) { headerByte(val, ID0_OFFSET); }

	uint8_t id1() const { return headerByte(ID1_OFFSET); }
	void id1(uint8_t val) { headerByte(val, ID1_OFFSET); }

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

#if __cplusplus >= 201103L
	static_assert(sizeof(Message)%sizeof(void*)==0,"Alignment problem with Message");
	static_assert(offsetof(Message, reserved_)+sizeof(void*) == sizeof(Message), "Message layout is incorrect");
#endif

}}

#endif

