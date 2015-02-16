/*
 * (C) Copyright 2015 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#ifndef step_timer_h_
#define step_timer_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * This interface controls timer32_0 which is used to drive the stepper.
 */

/**
 * Configure timer32_0 to generate one interrupt based on match register 0 and then stop.
 * The prescalar is set to count in microseconds.
 * Timer is not left running by this call.
 */
void step_timer_init(void);

/**
 * Start the timer running.  A single interrupt will be generated when the TC reaches val
 * and then the timer will be stopped.
 * \param val The period in microseconds before the timer interrupt occurs.
 */
void step_timer_start(uint32_t val);

/**
 * Stop the timer from running.  No further interrupts will be generated.
 */
void step_timer_stop(void);

/**
 * \return 0 if timer is stopped, 1 if it is running.
 */
uint8_t step_timer_enabled(void);

#ifdef __cplusplus
}
#endif

#endif
