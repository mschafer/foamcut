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
#ifndef stepper_device_Engine_hpp
#define stepper_device_Engine_hpp

#include "RingBuffer.hpp"
#include "StepDir.hpp"
#include "Line.hpp"
#include <StepperDictionary.hpp>

namespace stepper { namespace device {

class Stepper;

class Engine
{
public:
	enum {
		NO_OP_CMD        = 0,
		SINGLE_STEP_CMD  = 1,
		LINE_CMD         = 2,
		LONG_LINE_CMD    = 3,
		DELAY_CMD        = 4,
		HOME_CMD         = 5,
		DONE_CMD         = 6,
		NONE_CMD         = 0xFF
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
		uint32_t time_;
		int8_t dx_;
		int8_t dy_;
		int8_t dz_;
		int8_t du_;
	};

	struct LongLineCmd
	{
		enum { SIZE=12 };
		uint32_t time_;
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

	struct HomeCmd
	{
		enum { SIZE = 3 };
		uint16_t delay_;
		StepDir stepDir_;
	};

	enum Status {
		RUNNING,
		FINISHING,
		IDLE,
	};

	Engine();
	~Engine() {}

	/** Add a script message to the queue waiting to be processed. */
	void addScriptMessage(Message *m);

	/**
	 * Get the next StepDir to be executed.
	 * \return true on success, false on underflow.
	 */
	bool nextStep(Line::NextStep &out);

	Status status() const { return status_; }

	/**
	 * Parse enqueued instructions and generates StepDir commands
	 * from them.  The StepDir commands are placed in a FIFO for use
	 * by \sa Stepper::onTimerExpired.
	 */
	void operator()();

	void setupConnection();

private:
	RingBuffer<Message*, DataScriptMsg::IN_FLIGHT_COUNT+1> messages_;
	RingBuffer<Line::NextStep, 16> steps_;
	Line line_;
	uint16_t msgOffset_;
	uint8_t cmdOffset_;
	uint8_t currentCmdId_;
	uint8_t cmd_[sizeof(Engine::LongLineCmd)];
	Status status_;

	uint8_t extractBytes(uint8_t *buff, uint8_t count);
	bool parseNextCommand();

	Engine(const Engine &cpy);
	Engine &operator=(const Engine &assign);
};

}}

#endif
