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
#include <Logger.hpp>
#include <assert.h>

namespace stepper { namespace device {

Engine::Engine() : msgOffset_(0), cmdOffset_(0), currentCmdId_(NONE_CMD), status_(IDLE)
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
			StatusFlags::instance().clear(StatusFlags::ENGINE_RUNNING);
		} else {
			error(STEP_QUEUE_UNDERFLOW_ERROR);
		}
		return false;
	}
	out = steps_.front();

	// if we are homing, leave the current step active
	if ((out.flags_ & Line::NextStep::HOME_FLAG) == 0) {
		steps_.pop();
	}
	return true;
}

void Engine::homeComplete()
{
	assert(!steps_.empty());
	assert((steps_.front().flags_ & Line::NextStep::HOME_FLAG) != 0);
	steps_.pop();
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

uint8_t Engine::extractBytes(uint8_t *buff, uint8_t count)
{
	uint8_t ret = 0;
	while (count > 0 ) {
		if (messages_.empty()) {
			return ret;
		}

		Message *msg = messages_.front();
		buff[ret++] = msg->payload()[msgOffset_++];
		--count;

		// is that the end of data in this message?
		if (msgOffset_ == msg->payloadSize()) {
			msgOffset_ = 0;
			messages_.pop();

			// send an ack for the DataScriptMsg that we just finished processing
			AckScriptMsg *ack = new (msg) AckScriptMsg();
			ErrorCode ec;
			do {
				ec = HAL::sendMessage(ack);
			} while (ec != SUCCESS);
			Logger::trace("stepper", "data ack'd");
		}
	}
	return ret;
}

bool Engine::parseNextCommand()
{
	// get the id of the command
	if (currentCmdId_ == NONE_CMD) {
		if (extractBytes(&currentCmdId_, 1) == 0) return false;
		cmdOffset_ = 0;
	}

	switch (currentCmdId_) {
	case NO_OP_CMD:
		break;

	case SINGLE_STEP_CMD:
	{
		cmdOffset_ += extractBytes(cmd_+cmdOffset_, SingleStepCmd::SIZE-cmdOffset_);
		if (cmdOffset_ < SingleStepCmd::SIZE) return false;
		SingleStepCmd *ss = reinterpret_cast<SingleStepCmd*>(cmd_);
		steps_.push(Line::NextStep(ss->delay_, ss->stepDir_));
	}
	break;

	case LINE_CMD:
	{
		cmdOffset_ += extractBytes(cmd_+cmdOffset_, LineCmd::SIZE-cmdOffset_);
		if (cmdOffset_ < LineCmd::SIZE) return false;
		LineCmd *l = reinterpret_cast<LineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case LONG_LINE_CMD:
	{
		cmdOffset_ += extractBytes(cmd_+cmdOffset_, LongLineCmd::SIZE-cmdOffset_);
		if (cmdOffset_ < LongLineCmd::SIZE) return false;
		LongLineCmd *l = reinterpret_cast<LongLineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case DELAY_CMD:
	{
		cmdOffset_ += extractBytes(cmd_+cmdOffset_, DelayCmd::SIZE-cmdOffset_);
		if (cmdOffset_ < DelayCmd::SIZE) return false;
		DelayCmd *d = reinterpret_cast<DelayCmd*>(cmd_);
		steps_.push(Line::NextStep(d->delay_, StepDir()));
	}
	break;

	case HOME_CMD:
	{
		cmdOffset_ += extractBytes(cmd_ + cmdOffset_, HomeCmd::SIZE - cmdOffset_);
		if (cmdOffset_ < HomeCmd::SIZE) return false;
		HomeCmd *h = reinterpret_cast<HomeCmd*>(cmd_);
		steps_.push(Line::NextStep(h->delay_, h->stepDir_, Line::NextStep::HOME_FLAG));
	}
    break;

	case DONE_CMD:
		if (steps_.empty()) {
			status_ = IDLE;
			StatusFlags::instance().clear(StatusFlags::ENGINE_RUNNING);
		} else {
			status_ = FINISHING;
		}
		break;

	default:
		error(ILLEGAL_SCRIPT_DATA);
		break;
	}
	cmdOffset_ = 0;
	currentCmdId_ = NONE_CMD;
	return true;
}

void Engine::setupConnection()
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
