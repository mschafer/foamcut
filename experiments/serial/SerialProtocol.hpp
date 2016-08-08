#ifndef SerialProtocol_hpp
#define SerialProtocol_hpp

#include <stdint.h>
#include <stdlib.h>

#include "SerialPackets.hpp"
#include "Port.hpp"
#include "Synchronizer.hpp"
#include "AppPacket.hpp"
#include <chrono>
#include <boost/ptr_container/ptr_deque.hpp>

class SerialProtocol
{
public:

    explicit SerialProtocol(Port &port);

    enum ErrorCode {
        SP_SUCCESS = 0,
        SP_ERROR
    };

    enum State {
        DISCONNECTED,
        INITIALIZED,
        SYNCHRONIZED,
        CONNECTED,
        ERROR
    };

    enum {
        RX_WINDOW_SIZE = 8
	};
    static const double SYNC_INTERVAL_SEC;

	/**
	 * Transmit an AppPacket over the serial transport.
	 * If true is returned, then the protocol layer has assumed ownership of the 
	 * AppPacket and will delete it after transmission is complete.
	 * If false is returned, then the caller retains ownership.
	 * \todo Should this API use smart pointers to make ownsership transfer explicit?
	 */
    bool send(AppPacket *a);

	/**
	 * Receive bytes of the serial transport and assemble them into AppPackets.
	 * \return NULL if no AppPackets are available.
	 */
    AppPacket *recv();
    
	/**
	 * Execute the internal state machine and possibly the low level driver if
	 * polling mode instead of interrupt or DMA are used.
	 */
    void run();

    ErrorCode recv(uint8_t *buff, size_t &size);
    State state() const { return state_; }

	static uint8_t crc8(uint8_t seed, const uint8_t *data, size_t len);

private:

    static const char *SYNC_STRING;
    static const size_t SYNC_SIZE;
    Port &port_;
    State state_;
    Synchronizer sync_;
    size_t sendingSync_;
    std::chrono::time_point<std::chrono::steady_clock> syncTime_;
	boost::circular_buffer<uint8_t> rxRawFIFO_;
	uint8_t rxBuff_[MAX_SERIAL_PACKET_SIZE];
	uint8_t txBuff_[MAX_SERIAL_PACKET_SIZE];
    ptrdiff_t rxPos_;
    ptrdiff_t rxDataPos_;
    boost::ptr_deque<AppPacket> sendQueue_;
    ptrdiff_t txPos_;
	Connect connect_;

	bool timeToSendSync();
	void doSend();

	/**
	 * Separate the byte stream arriving over the serial transport into
	 * individual serial protocol packets.
	 */
    bool receiveSerialPacket();

	/**
	 * A complete serial packet has been received into rxBuff_
	 * so take whatever action is required to handle it.
	 */
    void handleSerialPacket();

	void sendConnect();
	void receiveConnect();

    SerialProtocol();
    SerialProtocol(const SerialProtocol &cpy);
    SerialProtocol &operator=(const SerialProtocol &rhs);

};

#endif
