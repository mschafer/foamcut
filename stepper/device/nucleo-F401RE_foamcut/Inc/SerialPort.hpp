#ifndef SerialPort_hpp_
#define SerialPort_hpp_

#include "fifo.hpp"

class SerialPort
{
    enum {
        MAX_RX_PACKET_SIZE = 1024
    };

    enum Status {
        SUCCESS = 0,
        FAILURE
    };

    /** Send a packet.  Blocks until done or error. */
    Status send(const uint8_t* buf, uint16_t len);

    /**
     * Receive a packet.
     * \param buff Output parameter that holds a pointer to the buffer
     * containing the packet on success or nullptr on failure.
     * \param len Output parameter holding the length of the received packet
     * on success.  Value is undefined on failure.
     */
    Status receive(uint8_t *&buf, uint16_t &len);


private:
    enum {
        SYNC = 0x7e,
        FIFO_SIZE = 64
    };

    enum State {
        INIT_SYNC,
        HEADER,
        HEADER_SYNC,
        PAYLOAD,
        PAYLOAD_SYNC,
        COMPLETE,
        ERROR
    };

    struct Header {
        uint16_t len_;
        uint8_t *array() { return reinterpret_cast<uint8_t *> (this); }
    };

    struct Message {
        uint8_t *buf_;
        Header header_;
        State state_;
        uint16_t progress_;
    };

    fifo<uint8_t, FIFO_SIZE> rxFifo_;
    fifo<uint8_t, FIFO_SIZE> txFifo_;
    uint8_t rxBuf_[MAX_RX_PACKET_SIZE];
    Message rx_;
    Message tx_;

    void sendNextBytes();
};

#endif
