#include "Engine.hpp"

namespace stepper { namespace device {

Engine::Engine()
{

}

void Engine::operator()()
{
	while (!steps_.full()) {
		if (!line_.done()) {
			steps_.push(line_.nextStep());
		} else {
			if (!parseNextCommand()) {
				///\todo underflow error here
				break;
			}
		}
	}
}

bool Engine::parseNextCommand()
{

}

}}
