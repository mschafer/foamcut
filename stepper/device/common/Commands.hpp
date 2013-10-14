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
#ifndef stepper_Commands_hpp
#define stepper_Commands_hpp

#include "StepDir.hpp"

/** \file This header defines all the commands that the script engine can execute. */

namespace stepper { namespace device {

enum {
	NO_OP_CMD        = 0,
	SINGLE_STEP_CMD  = 1,
	LINE_CMD         = 2,
	LONG_LINE_CMD    = 3,
	DELAY_CMD        = 4,
	PAUSE_CMD        = 5,
};

struct NoOpCmd
{
};

struct SingleStepCmd
{
	StepDir stepDir;
};

struct LineCmd
{
	int8_t dx;
	int8_t dy;
	int8_t dz;
	int8_t du;
	uint16_t delay;
};

struct LongLineCmd
{
	int16_t dx;
	int16_t dy;
	int16_t dz;
	int16_t du;
	uint32_t delay;
};

struct DelayCmd
{
	uint32_t delay;
};

struct PauseCmd
{
};

}}

#endif
