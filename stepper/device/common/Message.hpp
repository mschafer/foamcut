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


namespace stepper { namespace device {

struct MessageHeader
{
	uint16_t payloadSize_;
	uint8_t id0_;
	uint8_t id1_;
};

struct MessageBuffer
{
	MessageBuffer *next() { return next_; }
	void next(MessageBuffer *v) { next_ = v; }

	uint8_t poolTag() { return pad_[0]; }
	void poolTag(uint8_t v) { pad_[0] = v; }

	uint16_t totalSize() const { return header_.payloadSize_ + sizeof(MessageHeader); }
	uint8_t *start() { return (uint8_t*)&header_; }

	MessageHeader &header() { return header_; }

	uint8_t *payload() { return reinterpret_cast<uint8_t*>(this+1); }

private:
	MessageBuffer *next_;
	uint8_t pad_[4];
	MessageHeader header_;

	MessageBuffer();
	MessageBuffer(const MessageBuffer &cpy);
	MessageBuffer &operator=(const MessageBuffer &assign);
};

#if __cplusplus >= 201103L
static_assert(sizeof(MessageBuffer)%sizeof(void*)==0,"Alignment problem with Message");
static_assert(sizeof(MessageHeader)==4,"Alignment problem with Message");
#endif

}}

#endif
