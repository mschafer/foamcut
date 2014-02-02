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
#ifndef stepper_device_IDevice_hpp
#define stepper_device_IDevice_hpp

/** \file This file defines the interfaces to global statics. */

#include <stdint.h>

namespace stepper { namespace device {

// forward declaration of target specific types
class MemoryAllocator;
class Communicator;
struct StatusFlags;

MemoryAllocator &getMemoryAllocator();
Communicator &getCommunicator();
StatusFlags &getStatusFlags();

}}

#endif
