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
#include "Stepper.hpp"
#include "Platform.hpp"

namespace stepper { namespace device {

Stepper::Stepper() : pool_(messageBlock_, sizeof(messageBlock_))
{
	// alloc and free big blocks for script
}

void Stepper::runBackgroundOnce()
{
}

void Stepper::onTimerExpired()
{

}

void Stepper::sendMessage(MessageBuffer *mb)
{
	txQueue_.push(mb);
	notifySender();
}

}}
