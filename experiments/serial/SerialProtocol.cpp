#include "SerialProtocol.hpp"
#include "SerialPackets.hpp"
#include <memory.h>
#include <algorithm>

const char *SerialProtocol::SYNC_STRING = "~~~~ synchronizing ~~~~ synchronizing ~~~~";
const size_t SerialProtocol::SYNC_SIZE = strlen(SerialProtocol::SYNC_STRING);
const double SerialProtocol::SYNC_INTERVAL_SEC = .5;

SerialProtocol::SerialProtocol(Port &port) :port_(port), state_(SerialProtocol::DISCONNECTED),
		sync_(SerialProtocol::SYNC_STRING, strlen(SerialProtocol::SYNC_STRING)), sendingSync_(0),
		rxDataFIFO_((RX_WINDOW_SIZE + 1)*MAX_SERIAL_PACKET_SIZE), rxRawFIFO_(64),
        rxPos_(0), rxDataPos_(0), txPos_(0)
{

}

bool SerialProtocol::send(AppPacket *a)
{
    sendQueue_.push_back(a);
    return true;
}

bool SerialProtocol::timeToSendSync()
{
	auto now = clock_hack::now();
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
            size_t n = (std::min)((size_t)(d.dataSize_ - rxPos_), maxSize);
            memcpy(&buff[size], dStart, n);
            size += n;
            rxDataPos_ += n;

            // did we use up all the data in the message?
            if (rxDataPos_ == d.dataSize_) {
                // send an ack
                rxDataPos_ = -1;
            }
        } else {
            // try to get another packet
            bool gotPacket = receiveSerialPacket();
            if (state() == ERROR) return SP_ERROR;
            if (!gotPacket) break;
        }
    }
    return SP_SUCCESS;
}

bool SerialProtocol::receiveSerialPacket()
{
    // do we have the packet id yet?
    if (rxPos_ == 0) {
        size_t size = 1;
        Port::ErrorCode ec = port_.recv(rxBuff_, size);
        if (ec) {
			state_ = ERROR;
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
		state_ = ERROR;
        return false;
        break;

    }

    size_t complete = n;
    Port::ErrorCode ec = port_.recv(&rxBuff_[rxPos_], n);
    if (ec) {
		state_ = ERROR;
        return false;
    } else if (n != complete) {
        rxPos_ += n;
        return false;
    } else {
        rxPos_ = 0;
        handleSerialPacket();
        return true;
    }
}

void SerialProtocol::doSend()
{
    Data *pData = reinterpret_cast<Data*>(txBuff_);
    pData->id_ = DATA_ID;
    while (!sendQueue_.empty()) {
		if (port_.sendAvailable() < MAX_SERIAL_PACKET_SIZE) return;

        // assemble one data packet and send it
        size_t dpPos = 0;
        while (dpPos < Data::PAYLOAD_SIZE) {

            AppPacket &a = sendQueue_.front();
            size_t n = std::min(sizeof(Data::PAYLOAD_SIZE)-dpPos, a.size()-txPos_);
            std::copy(a.data()+dpPos, a.data()+n, pData->data_);
            txPos_ += n;

            if (txPos_ == a.size()) {
                txPos_ = 0;
                sendQueue_.pop_front();
            }

            if (sendQueue_.empty()) break;
        }

        // send the packet
        pData->dataSize_ = static_cast<uint8_t>(dpPos);
        pData->sequence_ = 1;
        pData->crc_ = 0;
        pData->id_ = DATA_ID;
        size_t nsent = dpPos + 4;
        Port::ErrorCode ec = port_.send(reinterpret_cast<uint8_t *>(pData), nsent);
        if (ec) {
        }
        assert(nsent == dpPos + 4);
    }
}

void SerialProtocol::handleSerialPacket()
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
		state_ = ERROR;
        break;
    }
}

void SerialProtocol::run()
{
	// simulate interrupt driven reception
	size_t n = std::min(port_.recvAvailable(), rxRawFIFO_.reserve());
	uint8_t c;
	for (size_t i = 0; i < n; ++i) {
		size_t ir = 1;
		port_.recv(&c, ir);
		assert(ir == 1);
		rxRawFIFO_.push_back(c);
	}

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
			if (sendingSync_ == 0) {
				syncTime_ = std::chrono::steady_clock::now();
			}
		} else if (timeToSendSync()) {
			sendingSync_ = SYNC_SIZE;
		}

		while (!rxRawFIFO_.empty()) {
			uint8_t c = rxRawFIFO_.front();
			rxRawFIFO_.pop_front();
			if (sync_.run(c)) {
				state_ = SYNCHRONIZED;
				break;
			}
		}
		break;
	}

		/** 
		 * \todo synchronized means sync was received, but still need to complete sending it if in process
		 * also need to ensure we don't send the Connect until sending of sync is complete
		 * maybe append Connect to the end of sync?
		 */
		


	case SYNCHRONIZED:
		receiveConnect();
		break;

	case CONNECTED:
		break;

	case ERROR:
	{
		port_.close();
		rxRawFIFO_.clear();
		rxDataFIFO_.clear();
		sync_.reset();
		state_ = DISCONNECTED;
		break;
	}

	}

}

void SerialProtocol::sendConnect()
{
	Connect conn;
	conn.maxPacketSize_ = MAX_SERIAL_PACKET_SIZE;
	conn.windowSize_ = RX_WINDOW_SIZE;

	// assume the Connect packet can be sent immediately in its entirety
	size_t s = sizeof(Connect);
	Port::ErrorCode ec = port_.send(reinterpret_cast<const uint8_t*>(&conn), s);

	if (s != sizeof(Connect) || ec) {
		state_ = ERROR;
	}
}

void SerialProtocol::receiveConnect()
{
	if (rxRawFIFO_.size() >= sizeof(Connect)) {

		// are we getting a second sync string?
		if (rxRawFIFO_.front() == SYNC_STRING[0]) {
			state_ = INITIALIZED;
			sync_.reset();
			return;
		}

		// verify Connect packet
		if (rxRawFIFO_.front() != CONNECT_ID) {
			state_ = ERROR;
			return;
		}
		
		uint8_t *p = (uint8_t*)&connect_;
		for (size_t i = 0; i < sizeof(Connect); ++i) {
			p[i] = rxRawFIFO_.front();
			rxRawFIFO_.pop_front();
		}

		///\todo verfiy connect crc
		state_ = CONNECTED;
	}
}

static uint8_t crc8_table[] = {
	0x00, 0x3e, 0x7c, 0x42, 0xf8, 0xc6, 0x84, 0xba, 0x95, 0xab, 0xe9, 0xd7,
	0x6d, 0x53, 0x11, 0x2f, 0x4f, 0x71, 0x33, 0x0d, 0xb7, 0x89, 0xcb, 0xf5,
	0xda, 0xe4, 0xa6, 0x98, 0x22, 0x1c, 0x5e, 0x60, 0x9e, 0xa0, 0xe2, 0xdc,
	0x66, 0x58, 0x1a, 0x24, 0x0b, 0x35, 0x77, 0x49, 0xf3, 0xcd, 0x8f, 0xb1,
	0xd1, 0xef, 0xad, 0x93, 0x29, 0x17, 0x55, 0x6b, 0x44, 0x7a, 0x38, 0x06,
	0xbc, 0x82, 0xc0, 0xfe, 0x59, 0x67, 0x25, 0x1b, 0xa1, 0x9f, 0xdd, 0xe3,
	0xcc, 0xf2, 0xb0, 0x8e, 0x34, 0x0a, 0x48, 0x76, 0x16, 0x28, 0x6a, 0x54,
	0xee, 0xd0, 0x92, 0xac, 0x83, 0xbd, 0xff, 0xc1, 0x7b, 0x45, 0x07, 0x39,
	0xc7, 0xf9, 0xbb, 0x85, 0x3f, 0x01, 0x43, 0x7d, 0x52, 0x6c, 0x2e, 0x10,
	0xaa, 0x94, 0xd6, 0xe8, 0x88, 0xb6, 0xf4, 0xca, 0x70, 0x4e, 0x0c, 0x32,
	0x1d, 0x23, 0x61, 0x5f, 0xe5, 0xdb, 0x99, 0xa7, 0xb2, 0x8c, 0xce, 0xf0,
	0x4a, 0x74, 0x36, 0x08, 0x27, 0x19, 0x5b, 0x65, 0xdf, 0xe1, 0xa3, 0x9d,
	0xfd, 0xc3, 0x81, 0xbf, 0x05, 0x3b, 0x79, 0x47, 0x68, 0x56, 0x14, 0x2a,
	0x90, 0xae, 0xec, 0xd2, 0x2c, 0x12, 0x50, 0x6e, 0xd4, 0xea, 0xa8, 0x96,
	0xb9, 0x87, 0xc5, 0xfb, 0x41, 0x7f, 0x3d, 0x03, 0x63, 0x5d, 0x1f, 0x21,
	0x9b, 0xa5, 0xe7, 0xd9, 0xf6, 0xc8, 0x8a, 0xb4, 0x0e, 0x30, 0x72, 0x4c,
	0xeb, 0xd5, 0x97, 0xa9, 0x13, 0x2d, 0x6f, 0x51, 0x7e, 0x40, 0x02, 0x3c,
	0x86, 0xb8, 0xfa, 0xc4, 0xa4, 0x9a, 0xd8, 0xe6, 0x5c, 0x62, 0x20, 0x1e,
	0x31, 0x0f, 0x4d, 0x73, 0xc9, 0xf7, 0xb5, 0x8b, 0x75, 0x4b, 0x09, 0x37,
	0x8d, 0xb3, 0xf1, 0xcf, 0xe0, 0xde, 0x9c, 0xa2, 0x18, 0x26, 0x64, 0x5a,
	0x3a, 0x04, 0x46, 0x78, 0xc2, 0xfc, 0xbe, 0x80, 0xaf, 0x91, 0xd3, 0xed,
	0x57, 0x69, 0x2b, 0x15 
};


uint8_t SerialProtocol::crc8(uint8_t seed, const uint8_t *data, size_t len)
{
	const uint8_t *end;

	if (len == 0)
		return seed;
	unsigned crc =  seed ^ 0xff;
	end = data + len;
	do {
		crc = crc8_table[crc ^ *data++];
	} while (data < end);
	return (uint8_t)(crc ^ 0xff);
}
