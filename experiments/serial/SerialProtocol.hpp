#ifndef SerialProtocol_hpp
#define SerialProtocol_hpp

#include <stdint.h>
#include <stdlib.h>

#include "Port.hpp"
#include "Synchronizer.hpp"
#include <chrono>

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
        MAX_PACKET_SIZE = 32,
        RX_WINDOW_SIZE = 8
    };
    static const double SYNC_INTERVAL_SEC;


    bool timeToSendSync();
    void run();
    ErrorCode recv(uint8_t *buff, size_t &size);
    ErrorCode send(const uint8_t *data, size_t &size);
    State state() const { return state_; }

private:
    static const char *SYNC_STRING;
    static const size_t SYNC_SIZE;
    Port &port_;
    State state_;
    Synchronizer sync_;
    size_t sendingSync_;
    std::chrono::time_point<std::chrono::system_clock> syncTime_;
    uint8_t rxBuff_[MAX_PACKET_SIZE];
    uint8_t txBuff_[MAX_PACKET_SIZE];
    ptrdiff_t rxPos_;
    ptrdiff_t rxDataPos_;

    bool receivePacket();
    void handlePacket();
    void handleError();

    SerialProtocol();
    SerialProtocol(const SerialProtocol &cpy);
    SerialProtocol &operator=(const SerialProtocol &rhs);

};

#endif
