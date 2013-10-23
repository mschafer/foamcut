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
#ifndef foamcut_device_Engine_hpp
#define foamcut_device_Engine_hpp

#include "RingBuffer.hpp"
#include "MessageQueue.hpp"
#include "StepDir.hpp"
#include "Line.hpp"

namespace stepper { namespace device {

namespace script {

enum {
	NO_OP_CMD        = 0,
	SINGLE_STEP_CMD  = 1,
	LINE_CMD         = 2,
	LONG_LINE_CMD    = 3,
	DELAY_CMD        = 4,
	PAUSE_CMD        = 5,
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

}


struct NoOpLock
{
	void lock() {}
	void unlock() {}
};

class Engine
{
public:
	Engine();
	~Engine() {}

	/** Add a script message to the queue waiting to be processed. */
	void addScriptMessage(MessageBuffer *mb) { queue_.push(mb); }

	/** \return Number of messages in the queue. */
	size_t queueSize() const { return queue_.size(); }

	/**
	 * Get the next StepDir to be executed.
	 * \return true on success, false on underflow.
	 */
	bool nextStep(Line::NextStep &out) { return steps_.pop(out); }

	/**
	 * Process enqueued ScriptMessages and generates StepDir commands from them.
	 */
	void operator()();

private:
	MessageQueue<NoOpLock> queue_;
	RingBuffer<Line::NextStep, 8> steps_;
	Line line_;

	bool parseNextCommand();

	Engine(const Engine &cpy);
	Engine &operator=(const Engine &assign);
};

}}

#endif
