#include "Script.hpp"
#include <Engine.hpp>
#include <Stepper.hpp>
#include <limits>

namespace stepper {

const double MAX_STEP_DELAY = std::numeric_limits<uint16_t>::max() * device::Stepper::TIMER_PERIOD_USEC * 1.e-6;
const double MAX_LINE_TIME = std::numeric_limits<uint32_t>::max() * device::Stepper::TIMER_PERIOD_USEC * 1.e-6;

Script::Script()
{
	bytes_.push_back(device::Script::DONE_CMD);
}

Script::~Script()
{
}

void
Script::addStep(device::StepDir sd, double delaySec)
{
	if (delaySec > MAX_STEP_DELAY) {
		throw std::out_of_range("delaySec too large for uint16_t");
	}
	device::Script::SingleStepCmd ssc;
	ssc.delay_ = (uint16_t)(delaySec * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);
	ssc.delay_ = (uint16_t)(delaySec * 1.e6) / device::Stepper::TIMER_PERIOD_USEC;
	ssc.stepDir_ = sd;
	bytes_.back() = device::Script::SINGLE_STEP_CMD;
	uint8_t *p = (uint8_t*)&ssc;
	bytes_.insert(bytes_.end(), p, p+device::Script::SingleStepCmd::SIZE);
	bytes_.push_back(device::Script::DONE_CMD);
}

void
Script::addLine(int16_t dx, int16_t dy, int16_t dz, int16_t du, double time)
{
	if (time > MAX_LINE_TIME) {
		throw std::out_of_range("time too large for uint32_t");
	}
	uint32_t dtime = (uint32_t)(time * 1.e6 / device::Stepper::TIMER_PERIOD_USEC);

	int amaxd = abs(dx);
	amaxd = std::max(amaxd, abs(dy));
	amaxd = std::max(amaxd, abs(dz));
	amaxd = std::max(amaxd, abs(du));
	if (-amaxd < std::numeric_limits<int8_t>::min()) {
		device::Script::LongLineCmd llc;
		llc.time_ = dtime;
		llc.dx_ = dx;
		llc.dy_ = dy;
		llc.dz_ = dz;
		llc.du_ = du;
		bytes_.back() = device::Script::LONG_LINE_CMD;
		uint8_t *p = (uint8_t*)&llc;
		bytes_.insert(bytes_.end(), p, p+device::Script::LongLineCmd::SIZE);
	} else {
		device::Script::LineCmd lc;
		lc.time_ = dtime;
		lc.dx_ = dx;
		lc.dy_ = dy;
		lc.dz_ = dz;
		lc.du_ = du;
		bytes_.back() = device::Script::LINE_CMD;
		uint8_t *p = (uint8_t*)&lc;
		bytes_.insert(bytes_.end(), p, p+device::Script::LineCmd::SIZE);
	}
	bytes_.push_back(device::Script::DONE_CMD);
}

void
Script::fillNextMessage(device::ScriptMsg &sm)
{
	auto endIt = msgIt_;
	if (device::ScriptMsg::PAYLOAD_SIZE < bytes_.end() - msgIt_) {
		endIt += device::ScriptMsg::PAYLOAD_SIZE;
	} else {
		endIt = bytes_.end();
	}
	std::copy(msgIt_, endIt, sm.scriptData_);
}

}
