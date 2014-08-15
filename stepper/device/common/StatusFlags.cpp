/* (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */

#include "StatusFlags.hpp"
#include "StepperDictionary.hpp"
#include <HAL.hpp>

namespace stepper { namespace device {

const char *FatalErrorMessage[6] = {
		"Script buffer overflow",
		"Step queue underflow",
		"Memory allocation",
		"Illegal script data",
		"Unrecognized message",
		"Stepper already running"
};

StatusFlags::StatusFlags() : flags_(0)
{

}

StatusFlags &StatusFlags::instance()
{
	static StatusFlags theFlags;
	return theFlags;
}

void error(FatalError ec)
{
	FatalErrorMsg *fem = new FatalErrorMsg((uint8_t)ec);
	HAL::sendMessage(fem);

	while(1) {}
}

}}
