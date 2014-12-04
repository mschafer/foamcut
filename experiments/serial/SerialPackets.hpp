#ifndef SerialPackets_hpp
#define SerialPackets_hpp

#include <stdint.h>
#include <stdlib.h>

enum PacketId {
	CONNECT_ID,
	ACK_ID,
	DATA_ID,
	DISCONNECT_ID
};

enum {
	MAX_SERIAL_PACKET_SIZE = 32
};


struct Connect
{
	enum {
		ID = CONNECT_ID
	};

	Connect() : id_(ID), maxPacketSize_(0), windowSize_(0) {}

	uint8_t id_;
	uint8_t maxPacketSize_;
	uint8_t windowSize_;
	uint8_t crc_;
};

struct Ack
{
	enum {
		ID = ACK_ID
	};

	Ack() : id_(ID), sequence_(0) {}

	uint8_t id_;
	uint8_t sequence_;
};

struct Data
{
	enum {
		ID = DATA_ID,
		PAYLOAD_SIZE = MAX_SERIAL_PACKET_SIZE - 4
	};

	Data() : id_(ID), sequence_(0), dataSize_(0), crc_(0) {}

	uint8_t id_;
	uint8_t sequence_;
	uint8_t dataSize_;
	uint8_t crc_;
	uint8_t data_[PAYLOAD_SIZE];
};

struct Disconnect
{
	enum {
		ID = DISCONNECT_ID
	};

	Disconnect() : id_(ID) {}
	
	uint8_t id_;
};


#endif
