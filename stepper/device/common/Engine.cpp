#include "Engine.hpp"
#include "Dictionary.hpp"
#include "Stepper.hpp"

namespace stepper { namespace device {

using namespace stepper::device::Script;

Engine::Engine(Stepper *stepper) : stepper_(stepper), currentMsg_(NULL),
		msgOffset_(0), cmdOffset_(-1)
{
}

void Engine::operator()()
{
	while (!steps_.full()) {
		if (!line_.done()) {
			steps_.push(line_.nextStep());
		} else {
			if (!parseNextCommand()) {
				break;
			}
		}
	}
}

bool Engine::getNextByte(uint8_t &byte)
{
	if (currentMsg_ == NULL && (currentMsg_ = queue_.pop()) == NULL) {
		return false;
	}

	byte = currentMsg_->payload()[msgOffset_++];
	if (msgOffset_ == ScriptMsg::PAYLOAD_SIZE) {
		stepper_->free(currentMsg_);
		currentMsg_ = NULL;
		msgOffset_ = 0;
	}
	return true;
}

bool Engine::parseNextCommand()
{
	if (cmdOffset_ == -1) {
		bool r = getNextByte(cmdId_);
		if (!r) return false;
		cmdOffset_ = 0;
	}

	switch (cmdId_) {
	case NO_OP_CMD:
		break;

	case SINGLE_STEP_CMD:
	{
		while (cmdOffset_ < SingleStepCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return false;
			++cmdOffset_;
		}
		SingleStepCmd *ss = reinterpret_cast<SingleStepCmd*>(cmd_);
		steps_.push(Line::NextStep(ss->delay_, ss->stepDir_));
	}
	break;

	case LINE_CMD:
	{
		while (cmdOffset_ < LineCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return false;
			++cmdOffset_;
		}
		LineCmd *l = reinterpret_cast<LineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->delay_);
	}
	break;

	case LONG_LINE_CMD:
	{
		while (cmdOffset_ < LongLineCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return false;
			++cmdOffset_;
		}
		LongLineCmd *l = reinterpret_cast<LongLineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->delay_);
	}
	break;

	case DELAY_CMD:
	{
		while (cmdOffset_ < DelayCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return false;
			++cmdOffset_;
		}
		DelayCmd *d = reinterpret_cast<DelayCmd*>(cmd_);
		steps_.push(Line::NextStep(d->delay_, StepDir()));
	}
	break;

	case DONE_CMD:
		break;

	default:
		///\todo fatal error here, unrecognized stepper instruction
		break;
	}

	cmdOffset_ = -1;
	return true;
}

void Engine::init()
{
	MessageBuffer *mb;
	while ((mb = queue_.pop()) != NULL) {
		stepper_->free(mb);
	}

	if (currentMsg_ != NULL) {
		stepper_->free(currentMsg_);
		currentMsg_ = NULL;
	}

	steps_.clear();

	msgOffset_ = 0;
	cmdOffset_ = -1;
}

}}
