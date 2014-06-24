#include "Simulator.hpp"
#include "SimCommunicator.hpp"
#include "MemoryAllocator.hpp"
#include <Stepper.hpp>

namespace stepper { namespace device {

std::unique_ptr<Simulator> Simulator::theSim_;

MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}

Simulator::Simulator(uint16_t port)
{
	comm_.reset(new SimCommunicator(port));
	comm_->initialize();

	boost::function<void ()> f = boost::bind(&Simulator::run, this);
	thread_.reset(new boost::thread(f));
}

Simulator::~Simulator()
{
	if (thread_->joinable()) {
		thread_->interrupt();
		thread_->join();
	}
}

Simulator &Simulator::instance()
{
	if (theSim_.get() == nullptr) {
		theSim_.reset(new Simulator());
	}
	return *theSim_.get();
}

void Simulator::initialize()
{
	Simulator &sim = instance();
}

void Simulator::setStepDirBits(const StepDir &s)
{
	Simulator &sim = instance();
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx ai = static_cast<StepDir::AxisIdx>(i);

		// set direction bits
		sim.currentBits_.dir(ai, s.dir(ai));

		// was there an edge on a step bit?
		if (s.step(ai) != sim.currentBits_.step(ai)) {
			sim.currentBits_.step(ai, s.step(ai));
			// was the edge in the right direction to cause a step?
			if (sim.currentBits_.step(ai) != sim.invertMask_.step(ai)) {
				int np = sim.pos_[i] + ((sim.invertMask_.dir(ai) == sim.currentBits_.dir(ai)) ? -1 : 1);

				// apply limits
				if (np >= sim.limit_[i].first && np <= sim.limit_[i].second) {
					sim.pos_[i] = np;
				}
			}
		}
	}
}

LimitSwitches Simulator::readLimitSwitches()
{
	Simulator &sim = instance();
	LimitSwitches ret;
	for (int i=0; i<StepDir::AXIS_COUNT; ++i) {
		StepDir::AxisIdx idx = static_cast<StepDir::AxisIdx>(i);
		bool test = (sim.pos_[i] <= sim.limit_[i].first);
		ret.reverseLimit(idx, test);
		test = (sim.pos_[i] >= sim.limit_[i].second);
		ret.forwardLimit(idx, test);
	}
	return ret;
}

Simulator::Status Simulator::sendMessage(Message *m, Priority priority)
{
	Simulator &sim = instance();
	return Simulator::instance().comm_->sendMessage(m, priority);
}

Message *Simulator::receiveMessage()
{
	Simulator &sim = instance();
	return sim.comm_->receiveMessage();
}

void Simulator::startTimer(uint32_t period)
{
	Simulator &sim = instance();
	///\todo implement me
}

void Simulator::stopTimer()
{
	Simulator &sim = instance();
	///\todo implement me

}

void Simulator::reset()
{
	theSim_.reset();
	Stepper &s = Stepper::instance();
	s.~Stepper();
	new (&s) Stepper();
}

uint16_t Simulator::port() const
{
	return comm_->port();
}

void Simulator::run()
{
	Stepper &s = Stepper::instance();
	try {
		while(1) {
			boost::this_thread::interruption_point();
			s.runBackgroundOnce();
			boost::this_thread::yield();
		}
	}

	catch(boost::thread_interrupted &/*intex*/) {
	}
	catch(...) {
		std::cerr << "Simulator thread threw" << std::endl;
	}
}

}}
