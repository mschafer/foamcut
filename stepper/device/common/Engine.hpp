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

class Stepper;

/** namespace for Engine instructions. */
namespace Script {

enum {
	NO_OP_CMD        = 0,
	SINGLE_STEP_CMD  = 1,
	LINE_CMD         = 2,
	LONG_LINE_CMD    = 3,
	DELAY_CMD        = 4,
	DONE_CMD         = 5
};

struct SingleStepCmd
{
	enum { SIZE=3 };
	uint16_t delay_;
	StepDir stepDir_;
};

struct LineCmd
{
	enum { SIZE=8 };
	uint32_t delay_;
	int8_t dx_;
	int8_t dy_;
	int8_t dz_;
	int8_t du_;
};

struct LongLineCmd
{
	enum { SIZE=12 };
	uint32_t delay_;
	int16_t dx_;
	int16_t dy_;
	int16_t dz_;
	int16_t du_;
};

struct DelayCmd
{
	enum { SIZE=4 };
	uint32_t delay_;
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
	explicit Engine(Stepper *);
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
	 * Parse enqueued instructions and generates StepDir commands
	 * from them.  The StepDir commands are placed in a FIFO for use
	 * by \sa Stepper::onTimerExpired.
	 */
	void operator()();

	void init();

private:
	Stepper *stepper_;
	MessageQueue<NoOpLock> queue_;
	RingBuffer<Line::NextStep, 8> steps_;
	Line line_;
	MessageBuffer *currentMsg_;
	uint16_t msgOffset_;
	uint8_t cmdId_;
	int8_t cmdOffset_;
	uint8_t cmd_[sizeof(Script::LongLineCmd)];

	bool getNextByte(uint8_t &out);
	bool parseNextCommand();

	Engine();
	Engine(const Engine &cpy);
	Engine &operator=(const Engine &assign);
};

}}

#endif
