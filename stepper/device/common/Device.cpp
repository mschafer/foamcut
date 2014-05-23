#include "Device.hpp"
#include "HAL.hpp"

namespace stepper { namespace device {

Device::Device()
{
}

Device::~Device()
{
}

void Device::operator()(Message *m)
{
	// handle incoming message if there is one
	if (m != NULL) {
		handleMessage(m);
	}
}

Device &Device::instance()
{
	static Device theDevice;
	return theDevice;
}

void Device::handleMessage(Message *m)
{
    switch(m->header().function_) {

    case PingMsg::FUNCTION:
        new (m) PingResponseMsg;
        HAL::sendMessage(m);
        break;

    case PingResponseMsg::FUNCTION:
        delete m;
        break;
    }
}

}}
