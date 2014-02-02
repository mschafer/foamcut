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

#ifndef stepper_device_DeviceMessages_hpp
#define stepper_device_DeviceMessages_hpp


/** \file
 * This file defines the Message class and dictionary for the target.
 * Include this file instead of Message.hpp or Dictionary.hpp
 */

#include <new>
#include <assert.h>
#include <stdint.h>
#include "SList.hpp"
#include "StatusFlags.hpp"

namespace stepper { namespace device {

#include "Message.hpp"
#include "CommDictionary.hpp"
#include "StepperDictionary.hpp"

typedef MemberSList<Message, &Message::slistMemberHook_> MessageList; 

}}

#endif
