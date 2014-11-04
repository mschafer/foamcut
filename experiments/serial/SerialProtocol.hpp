#ifndef SerialProtocol_hpp
#define SerialProtocol_hpp

#include <stdint.h>
#include <stdlib.h>

#include "Port.hpp"
#include "Synchronizer.hpp"
#include "APDU.hpp"
#include <chrono>
#include <boost/ptr_container/ptr_deque.hpp>
#include "SerialPackets.hpp"

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

    bool send(APDU *a);
    APDU *recv();
    
    bool timeToSendSync();
    void run();
    ErrorCode recv(uint8_t *buff, size_t &size);
    State state() const { return state_; }

	static uint8_t crc8(uint8_t seed, const uint8_t *data, size_t len);

private:

	/** For some reason only system_clock works on windows and only steady_clock works on Mac */
#ifdef WIN32
	typedef std::chrono::system_clock clock_hack;
#else
	typedef std::chrono::steady_clock clock_hack;
#endif

    static const char *SYNC_STRING;
    static const size_t SYNC_SIZE;
	static Connect MY_CONNECT;
    Port &port_;
    State state_;
    Synchronizer sync_;
    size_t sendingSync_;
    std::chrono::time_point<clock_hack> syncTime_;
	boost::circular_buffer<uint8_t> rxDataFIFO_;
	boost::circular_buffer<uint8_t> rxRawFIFO_;
	uint8_t rxBuff_[MAX_SERIAL_PACKET_SIZE];
	uint8_t txBuff_[MAX_SERIAL_PACKET_SIZE];
    ptrdiff_t rxPos_;
    ptrdiff_t rxDataPos_;
    boost::ptr_deque<APDU> sendQueue_;
    ptrdiff_t txPos_;
    APDUFactory factory_;
	Connect connect_;

    void doSend();
    bool receivePacket();
    void handlePacket();
	void sendConnect();
	void receiveConnect();

    SerialProtocol();
    SerialProtocol(const SerialProtocol &cpy);
    SerialProtocol &operator=(const SerialProtocol &rhs);

};

#endif
