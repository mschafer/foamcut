#include "SerialProtocol.hpp"
#include <memory.h>
#include <algorithm>

enum PacketId {
    CONNECT_ID,
    ACK_ID,
    DATA_ID,
    DISCONNECT_ID
};


struct Connect
{
    uint8_t id_;
    uint8_t maxPacketSize_;
    uint8_t windowSize_;
};

struct Ack
{
    uint8_t id_;
    uint8_t sequence_;
};

struct Data
{
    uint8_t id_;
    uint8_t sequence_;
    uint8_t size_;
    uint8_t crc_;
    uint8_t data_[SerialProtocol::MAX_PACKET_SIZE-4];
};

struct Disconnect
{
};


const char *SerialProtocol::SYNC_STRING = "~~~~ synchronizing ~~~~ synchronizing ~~~~";
const size_t SerialProtocol::SYNC_SIZE = strlen(SerialProtocol::SYNC_STRING);
const double SerialProtocol::SYNC_INTERVAL_SEC = .5;

SerialProtocol::SerialProtocol(Port &port) :port_(port), state_(SerialProtocol::DISCONNECTED),
		sync_(SerialProtocol::SYNC_STRING, strlen(SerialProtocol::SYNC_STRING)), sendingSync_(0),
        rxPos_(0), rxDataPos_(0)
{

}

bool SerialProtocol::timeToSendSync()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> sec = now - syncTime_;
	if (sec.count() > SYNC_INTERVAL_SEC) {
		return true;
	} else {
		return false;
	}
}

SerialProtocol::ErrorCode SerialProtocol::recv(uint8_t *buff, size_t &size)
{
    // no data to receive if we aren't connected
    if (state() != CONNECTED) {
        size = 0;
        return SP_SUCCESS;
    }

    size_t maxSize = size;
    size = 0;
    while (size < maxSize) {
        // is there data left in the last data packet?
        if (rxDataPos_ >= 0) {
            Data &d = *(Data*)rxBuff_;
            uint8_t *dStart = &d.data_[rxPos_];
            size_t n = (std::min)((size_t)(d.size_ - rxPos_), maxSize);
            memcpy(&buff[size], dStart, n);
            size += n;
            rxDataPos_ += n;

            // did we use up all the data in the message?
            if (rxDataPos_ == d.size_) {
                // send an ack
                rxDataPos_ = -1;
            }
        } else {
            // try to get another packet
            bool gotPacket = receivePacket();
            if (state() == ERROR) return SP_ERROR;
            if (!gotPacket) break;
        }
    }
    return SP_SUCCESS;
}

bool SerialProtocol::receivePacket()
{
    // do we have the packet id yet?
    if (rxPos_ == 0) {
        size_t size = 1;
        Port::ErrorCode ec = port_.recv(rxBuff_, size);
        if (ec) {
            handleError();
            return false;
        } else if (size == 0) {
            return false;
        }
    }

    size_t n = 0;
    switch (rxBuff_[0]) {
    case CONNECT_ID:
        n = sizeof(Connect) - rxPos_;
        break;

    case ACK_ID:
        n = sizeof(Ack) - rxPos_;
        break;

    case DATA_ID:
        n = sizeof(Data) - rxPos_;
        break;

    case DISCONNECT_ID:
        n = sizeof(Disconnect) - rxPos_;
        break;

    default:
        handleError();
        return false;
        break;

    }

    size_t complete = n;
    Port::ErrorCode ec = port_.recv(&rxBuff_[rxPos_], n);
    if (ec) {
        handleError();
        return false;
    } else if (n != complete) {
        rxPos_ += n;
        return false;
    } else {
        rxPos_ = 0;
        handlePacket();
        return true;
    }
}

void SerialProtocol::handlePacket()
{
    switch (rxBuff_[0]) {
    case CONNECT_ID:
        // if state is synchronized then transition to connected, else error
        break;

    case ACK_ID:
        // verify sequence and adjust window
        break;

    case DATA_ID:
        rxDataPos_ = 0;
        break;

    case DISCONNECT_ID:
        // ??
        break;

    default:
        handleError();
        break;
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

void SerialProtocol::handleError()
{
}
