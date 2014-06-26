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

Engine::Engine() : msgOffset_(0), cmdOffset_(0), status_(IDLE)
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

bool Engine::nextStep(Line::NextStep &out)
{
	if (steps_.empty()) {
		if (status_ == FINISHING) {
			status_ = IDLE;
		} else {
			error(STEP_QUEUE_UNDERFLOW_ERROR);
		}
		return false;
	}
	out = steps_.front();
	steps_.pop();
	return true;
}

void Engine::operator()()
{
	while (!steps_.full()) {
		if (!line_.done()) {
			steps_.push(line_.nextStep());
		} else {
			if (!parseNextCommand())
				break;
		}
	}
}

bool Engine::extractBytes(uint8_t count)
{
	while (count > 0 ) {
		if (messages_.empty()) {
			return false;
		}

		Message *msg = messages_.front();
		cmd_[cmdOffset_++] = msg->payload()[msgOffset_++];
		--count;

		// is that the end of data in this message?
		if (msgOffset_ == msg->payloadSize()) {
			msgOffset_ = 0;
			messages_.pop();

			// send an ack for the DataScriptMsg that we just finished processing
			AckScriptMsg *ack = new (msg) AckScriptMsg();
			HAL::Status status;
			do {
				status = HAL::sendMessage(ack);
			} while (status != HAL::SUCCESS);
		}
	}
	return true;
}

bool Engine::parseNextCommand()
{
	// get the id of the command
	if (cmdOffset_ == 0) {
		if (!extractBytes(1)) return false;
	}

	switch (cmd_[0]) {
	case NO_OP_CMD:
		break;

	case SINGLE_STEP_CMD:
	{
		if (!extractBytes(SingleStepCmd::SIZE-cmdOffset_)) return false;
		SingleStepCmd *ss = reinterpret_cast<SingleStepCmd*>(cmd_);
		steps_.push(Line::NextStep(ss->delay_, ss->stepDir_));
	}
	break;

	case LINE_CMD:
	{
		if (!extractBytes(LineCmd::SIZE-cmdOffset_)) return false;
		LineCmd *l = reinterpret_cast<LineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case LONG_LINE_CMD:
	{
		if (!extractBytes(LongLineCmd::SIZE-cmdOffset_)) return false;
		LongLineCmd *l = reinterpret_cast<LongLineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case DELAY_CMD:
	{
		if (!extractBytes(DelayCmd::SIZE-cmdOffset_)) return false;
		DelayCmd *d = reinterpret_cast<DelayCmd*>(cmd_);
		steps_.push(Line::NextStep(d->delay_, StepDir()));
	}
	break;

	case DONE_CMD:
		if (steps_.empty())
			status_ = IDLE;
		else
			status_ = FINISHING;
		break;

	default:
		error(ILLEGAL_SCRIPT_DATA);
		break;
	}
	cmdOffset_ = 0;
	return true;
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
