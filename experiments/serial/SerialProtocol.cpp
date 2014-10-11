#include "SerialProtocol.hpp"
#include <memory.h>


const char *SerialProtocol::SYNC_STRING = "~~~~ synchronizing ~~~~ synchronizing ~~~~";
const size_t SerialProtocol::SYNC_SIZE = strlen(SerialProtocol::SYNC_STRING);
const double SerialProtocol::SYNC_INTERVAL_SEC = .5;

SerialProtocol::SerialProtocol(Port &port) :port_(port), state_(SerialProtocol::DISCONNECTED),
		sync_(SerialProtocol::SYNC_STRING, strlen(SerialProtocol::SYNC_STRING)), sendingSync_(0)
{

}

bool SerialProtocol::timeToSendSync()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> sec = now - syncTime_;
	if (sec.count() > SYNC_INTERVAL_SEC) {
		return true;
	} else {
		return false;
	}
}

void SerialProtocol::run()
{
	switch (state_) {
	case DISCONNECTED:
	{
		auto err = port_.open();
		if (!err) {
			state_ = INITIALIZED;
			sendingSync_ = SYNC_SIZE;
		} else {
			state_ = ERROR;
		}
		break;
	}

	// port is initialized, look for sync string and periodically send it
	case INITIALIZED:
	{
		if (sendingSync_ != 0) {
			const uint8_t *p = reinterpret_cast<const uint8_t*>(SYNC_STRING) + SYNC_SIZE - sendingSync_;
			size_t n = sendingSync_;
			Port::ErrorCode err = port_.send(p, n);
			if (!err) {
				sendingSync_ -= n;
			} else {
				state_ = ERROR;
				break;
			}
			if (sendingSync_ == 0) syncTime_ = std::chrono::steady_clock::now();
		} else if (timeToSendSync()) {
			sendingSync_ = SYNC_SIZE;
		}

		size_t n = sync_.remaining();
		Port::ErrorCode err = port_.recv(rxBuff_, n);
		if (err) {
			state_ = ERROR;
			break;
		}
		if (sync_.run(rxBuff_, n)) {
			state_ = SYNCHRONIZED;
		}

		break;
	}
	case SYNCHRONIZED:
		break;

	case CONNECTED:
		break;

	case ERROR:
	{
		port_.close();
		state_ = DISCONNECTED;
		break;
	}

	}

}
