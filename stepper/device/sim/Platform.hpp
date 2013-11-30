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
#ifndef stepper_sim_Platform_hpp
#define stepper_sim_Platform_hpp

#include <stdint.h>
#include <stddef.h>

namespace stepper { namespace device {

template<uint8_t NUM_POOLS, typename size_type> class MemoryPool;
class Communicator;

class Platform
{

enum {
	/**
	 * The number of cached script messages for the engine (minimum 4)
	 * Uses about 270 bytes each.
	 */
	SCRIPT_MSG_POOL = 4
};

typedef MemoryPool<2, size_t> MemoryPool_type;
MemoryPool_type &getMemoryPool();

Communicator &getCommunicator();

void resetPlatform();


};

}}

#endif
