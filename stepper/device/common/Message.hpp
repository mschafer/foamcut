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

#include <stdlib.h>
#include <stdint.h>
//#include <assert.h>
#include <new>

namespace stepper { namespace device {

struct MessageHeader
{
    /**
     * Size of payload in bytes does not include header.
     * Always little endian.
     */
	uint16_t payloadSize_;

	uint8_t id_;

	uint8_t function_;
};


class Message
{
public:
	enum Priority {
		NORMAL_PRIORITY,
		HIGH_PRIORITY
	};

    Message() {}
	~Message() {}

    void *operator new(size_t size);
    void operator delete(void *ptr) throw();

    void *operator new(size_t size, const std::nothrow_t &nothrow_value) throw();
    void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) throw();

    void *operator new(size_t size, void *place) throw() { return ::operator new(size, place); }
    void operator delete(void *ptr, void *place) throw() { return ::operator delete(ptr, place); }

    /** Allocate a Message with at least the specified payload space. */
    static Message *alloc(uint16_t payloadSize);

    /** \return The payload capacity in bytes of this. */
    uint16_t payloadCapacity() const;

    /** \return The maximum payload capacity message that can be allocated. */
    static uint16_t maxPayloadCapacity();

    const MessageHeader &header() const  {
        // use asserts to verify layout of class b/c we don't have c++11 aligment
        //assert(sizeof(MessageHeader) == 4);
        const MessageHeader *p = reinterpret_cast<const MessageHeader*>(payload() - sizeof(MessageHeader));
        //assert((reinterpret_cast<size_t>(p) & 3) == 0); // alignment
        return *p;
    }
    MessageHeader &header() { return const_cast<MessageHeader&>(static_cast<const Message*>(this)->header()); }

    void header(const MessageHeader &h) {
        MessageHeader &mine = header();
        mine = h;
    }

    uint16_t payloadSize() const { return header().payloadSize_; }
    void payloadSize(uint16_t v) { header().payloadSize_ = v; }

    uint8_t id() const { return header().id_; }
    void id(uint8_t v) { header().id_ = v; }

    uint8_t function() const { return header().function_; }
    void function(uint8_t v) { header().function_ = v; }

	const uint8_t *payload() const { 
        const uint8_t *ret = reinterpret_cast<const uint8_t*>(this+1);
        //assert((size_t)ret % sizeof(void*) == 0);
        return ret;
    }
    uint8_t *payload() { return const_cast<uint8_t*>(static_cast<const Message*>(this)->payload()); }

    /** \return Number of bytes to transmit = payload + header. */
    uint16_t transmitSize() const { return header().payloadSize_ + (uint16_t)sizeof(MessageHeader); }

    /** \return Starting address of bytes to transmit. */
    const uint8_t *transmitStart() const { return payload() - sizeof(MessageHeader); }
    uint8_t *transmitStart() { return const_cast<uint8_t*>(static_cast<const Message*>(this)->transmitStart()); }

	/** Amount of memory to get from MemoryAllocator for a given payload size. */
	static uint16_t memoryNeeded(uint16_t payloadSize) { return sizeof(Message) + payloadSize; }

    union {
    	void *align_;
	    uint8_t reserved_[sizeof(void*)];
        MessageHeader fauxHeader_;
    };

private:
    /** Allocating arrays of Message is not allowed. */
    void* operator new[] (std::size_t size);
    void operator delete[] (void* ptr) throw();
    void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_value) throw();
    void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant) throw();
    void* operator new[] (std::size_t size, void* ptr) throw();
    void operator delete[] (void* ptr, void* voidptr2) throw();
};

}}

#endif
