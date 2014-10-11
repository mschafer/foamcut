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

    enum State {
        DISCONNECTED,
        INITIALIZED,
        SYNCHRONIZED,
        CONNECTED,
        ERROR
    };
    static const size_t MAX_PACKET_SIZE = 32;
    static const double SYNC_INTERVAL_SEC;


    bool timeToSendSync();
    void run();
    State state() const { return state_; }

private:
    static const char *SYNC_STRING;
    static const size_t SYNC_SIZE;
    Port &port_;
    State state_;
    Synchronizer sync_;
    size_t sendingSync_;
    std::chrono::time_point<std::chrono::steady_clock> syncTime_;
    uint8_t rxBuff_[MAX_PACKET_SIZE];
    uint8_t txBuff_[MAX_PACKET_SIZE];

    SerialProtocol();
    SerialProtocol(const SerialProtocol &cpy);
    SerialProtocol &operator=(const SerialProtocol &rhs);

};

#endif
