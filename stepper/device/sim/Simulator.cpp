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

void Simulator::intialize()
{
	Simulator &sim = instance();
}

void Simulator::setStepDirBits(const StepDir &s)
{
	Simulator &sim = instance();
}

LimitSwitches Simulator::readLimitSwitches()
{
	Simulator &sim = instance();
	return LimitSwitches();
}

Simulator::Status Simulator::sendMessage(Message *m, Priority priority)
{
	Simulator &sim = instance();
	return Simulator::instance().comm_->sendMessage(m, priority);
}

Message *Simulator::receiveMessage()
{
	Simulator &sim = instance();
	return NULL;
}

void Simulator::startTimer(uint32_t period)
{
	Simulator &sim = instance();

}

void Simulator::reset()
{
	theSim_.reset();
	Stepper &s = Stepper::instance();
	s.~Stepper();
	new (&s) Stepper();
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
