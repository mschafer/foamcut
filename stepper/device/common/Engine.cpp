#include "Engine.hpp"
#include "Dictionary.hpp"

namespace stepper { namespace device {

using namespace stepper::device::Script;

Engine::Engine(MessagePool<platform::Lock> &pool) : pool_(pool), currentMsg_(NULL),
		msgOffset_(0), cmdId_(0), cmdOffset_(-1)
{
}

Engine::Status Engine::operator()()
{
	Status r = RUNNING;
	while (!steps_.full()  && cmdId_ != DONE_CMD) {
		if (!line_.done()) {
			steps_.push(line_.nextStep());
		} else {
			r = parseNextCommand();
			if (r != RUNNING) {
				break;
			}
		}
	}
	return r;
}

bool Engine::getNextByte(uint8_t &byte)
{
	if (currentMsg_ == NULL && (currentMsg_ = queue_.pop()) == NULL) {
		return false;
	}

	byte = currentMsg_->payload()[msgOffset_++];
	if (msgOffset_ == ScriptMsg::PAYLOAD_SIZE) {
		pool_.free(currentMsg_);
		currentMsg_ = NULL;
		msgOffset_ = 0;
	}
	return true;
}

Engine::Status Engine::parseNextCommand()
{
	if (cmdOffset_ == -1) {
		bool r = getNextByte(cmdId_);
		if (!r) return QUEUE_UNDERFLOW;
		cmdOffset_ = 0;
	}

	switch (cmdId_) {
	case NO_OP_CMD:
		break;

	case SINGLE_STEP_CMD:
	{
		while (cmdOffset_ < SingleStepCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return QUEUE_UNDERFLOW;
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
			if (!r) return QUEUE_UNDERFLOW;
			++cmdOffset_;
		}
		LineCmd *l = reinterpret_cast<LineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case LONG_LINE_CMD:
	{
		while (cmdOffset_ < LongLineCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return QUEUE_UNDERFLOW;
			++cmdOffset_;
		}
		LongLineCmd *l = reinterpret_cast<LongLineCmd*>(cmd_);
		line_.reset(l->dx_, l->dy_, l->dz_, l->du_, l->time_);
	}
	break;

	case DELAY_CMD:
	{
		while (cmdOffset_ < DelayCmd::SIZE) {
			bool r = getNextByte(cmd_[cmdOffset_]);
			if (!r) return QUEUE_UNDERFLOW;
			++cmdOffset_;
		}
		DelayCmd *d = reinterpret_cast<DelayCmd*>(cmd_);
		steps_.push(Line::NextStep(d->delay_, StepDir()));
	}
	break;

	case DONE_CMD:
		init();
		return DONE;

	default:
		init();
		return FATAL_ERROR;
		break;
	}

	cmdOffset_ = -1;
	return RUNNING;
}

void Engine::init()
{
	MessageBuffer *mb;
	while ((mb = queue_.pop()) != NULL) {
		pool_.free(mb);
	}

	if (currentMsg_ != NULL) {
		pool_.free(currentMsg_);
		currentMsg_ = NULL;
	}

	steps_.clear();

	msgOffset_ = 0;
	cmdOffset_ = -1;
	cmdId_ = 0;
}

}}
