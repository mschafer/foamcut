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
#include <IDevice.hpp>
#include <MemoryAllocator.hpp>
#include "Communicator.hpp"

namespace stepper { namespace device {

static MemoryAllocator *theMem = NULL;
static Communicator *theComm = NULL;
static StatusFlags theStatFlags;

MemoryAllocator &getMemoryAllocator()
{
    static const uint16_t poolSizes[2] = {248, MemoryAllocator::MAX_CHUNK_SIZE};
    static uint8_t memoryBlock[4194304];
    if (theMem == NULL) {
        theMem = new MemoryAllocator(poolSizes, 2, memoryBlock, sizeof(memoryBlock));
    }
    return *theMem;
}

Communicator &getCommunicator()
{
    if (theComm == NULL) {
        theComm = new Communicator();
        theComm->initialize();
    }
    return *theComm;
}

StatusFlags &getStatusFlags()
{
    return theStatFlags;
}

void resetServices()
{
    if (theComm) delete theComm;
    if (theMem) delete theMem;

    theComm = NULL;
    theMem = NULL;
    theStatFlags.clear();
}

}}
