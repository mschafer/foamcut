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
#include "Engine.hpp"
#include "StatusFlags.hpp"
#include <HAL.hpp>

namespace stepper { namespace device {

using namespace stepper::device::Script;

Engine::Engine() : msgOffset_(0), cmdId_(0), cmdOffset_(0), status_(IDLE)
{
}

void Engine::addScriptMessage(Message *m)
{
	if (messages_.full()) {
		error(SCRIPT_BUFFER_OVERFLOW_ERROR);
	} else {
		messages_.push(m);
		status_ = RUNNING;
	}
}

void Engine::operator()()
{
	while (!steps_full()) {
		if (!line_.done()) {
			steps_.push(line_.nextStep());
		} else {
			if (!parseNextCommand())
				break;
		}
	}
}

bool Engine::extractNextByte(uint8_t offset)
{
	if (messages_.empty()) {
		error(SCRIPT_QUEUE_UNDERFLOW_ERROR);
		return false;
	}

	Message *msg = messages_.front();
	cmd_[offset] = msg->payload()[msgOffset_++];

	// is that the end of data in this message?
	if (msgOffset_ == msg->payloadSize()) {
		messages_.pop();
		delete msg;

		// send an ack for the DataScriptMsg that we just finished processing
		AckScriptMsg *ack = new (msg) AckScriptMsg();
		if (ack == NULL) {
			error(MEMORY_ALLOCATION_ERROR);
			return false;
		}
		HAL::Status status;
		do {
			status = HAL::sendMessage(ack);
		} while (status != HAL::SUCCESS);

		msgOffset_ = 0;
	}
	return true;
}

bool Engine::extractCommandData(uint8_t cmdSize)
{
	// start at 1 because id is already extracted
	for (int i=1; i<size; ++i) {
		if (!extractNextByte(i)) return false;
	}
	return true;
}

void Engine::parseNextCommand()
{
	// get the id of the command
	if (msgOffset_ > 0)


	if (!extractNextByte(0)) return;

	switch (cmd_[0]) {
	case NO_OP_CMD:
		break;

	case SINGLE_STEP_CMD:
	{
		if (!extractCommandData(SingleStepCmd::SIZE)) return;
		SingleStepCmd *ss = reinterpret_cast<SingleStepCmd*>(cmd_);
		steps_.push(Line::NextStep(ss->delay_, ss->stepDir_));
	}
	break;

	case LINE_CMD:
	{
		if (!extractCommandData(LineCmd::SIZE)) return;
		LineCmd *l = reinterpret_cast<LineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case LONG_LINE_CMD:
	{
		if (!extractCommandData(LongLineCmd::SIZE)) return;
		LongLineCmd *l = reinterpret_cast<LongLineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case DELAY_CMD:
	{
		if (!extractCommandData(DelayCmd::SIZE)) return;
		DelayCmd *d = reinterpret_cast<DelayCmd*>(cmd_);
		steps_.push(Line::NextStep(d->delay_, StepDir()));
	}
	break;

	case DONE_CMD:
		init();
		if (steps_.empty())
			status_ = IDLE;
		else
			status_ = FINISHING;
		break;

	default:
		error(ILLEGAL_SCRIPT_DATA);
		break;
	}
}

void Engine::init()
{
	HAL::stopTimer();
	while (!messages_.empty()) {
		delete (messages_.front());
		messages_.pop();
	}
	line_.reset();
	steps_.clear();

	msgOffset_ = 0;
	cmdOffset_ = 0;
	status_ = IDLE;
}

}}
