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
#ifndef stepper_device_StaticPool_hpp
#define stepper_device_StaticPool_hpp

namespace stepper { namespace device {

template <uint16_t ChunkSize, size_t ChunkCount>
class StaticPool
{
	void *alloc();
	void free(void *p);

	bool contains(void *p) const {
		return (p >= block_ && p < block_ + sizeof(block_));
	}

private:
	uint8_t block_[ChunkSize*ChunkCount];
};

}}

#endif
