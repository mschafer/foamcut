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

Simulator::Simulator(uint16_t port) : backgroundTimer_(ios_), stepTimer_(ios_)
{
	comm_.reset(new SimCommunicator(ios_, port));
	comm_->initialize();

	ios_.post(boost::bind(&Simulator::runOnce, this, boost::system::error_code()));

	boost::function<void ()> f = boost::bind(&boost::asio::io_service::run, &ios_);
	thread_.reset(new boost::thread(f));
}

Simulator::~Simulator()
{
	ios_.stop();
	if (thread_->joinable()) {
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
	Stepper &s = Stepper::instance();
	sim.stepTimer_.expires_from_now(boost::posix_time::microseconds(period*5));
	sim.stepTimer_.async_wait(boost::bind(&Simulator::stepTimerExpired, &sim, boost::asio::placeholders::error));
}

void Simulator::stopTimer()
{
	Simulator &sim = instance();
	sim.stepTimer_.cancel();

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

void Simulator::runOnce(const boost::system::error_code &ec)
{
	if (!ec) {
		Stepper &s = Stepper::instance();
		s.runBackgroundOnce();
		backgroundTimer_.expires_from_now(boost::posix_time::milliseconds(10));
		backgroundTimer_.async_wait(boost::bind(&Simulator::runOnce, this, boost::asio::placeholders::error));
	} else {
		std::string em("Simulator::runOnce error: ");
		em += ec.message();
		throw std::runtime_error(em);
	}
}

void Simulator::stepTimerExpired(const boost::system::error_code &ec)
{
	if (!ec) {
		Stepper &s = Stepper::instance();
		s.onTimerExpired();
	} else {
		std::string em("Simulator::stepTimerExpired error: ");
		em += ec.message();
		throw std::runtime_error(em);
	}
}

}}
