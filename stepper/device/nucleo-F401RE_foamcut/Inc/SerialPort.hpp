#ifndef SerialPort_hpp_
#define SerialPort_hpp_

#include "fifo.hpp"

class SerialPort
{
public:
	enum Status {
		SERIAL_OK,
		SERIAL_BUSY,
		SERIAL_ERROR,
		SERIAL_OVERRUN
	};

	/**
	 * Start sending a packet out the serial port.
	 * The contents of buf should remain valid until sendState returns SERIAL_OK.
	 */
	Status startSending(const uint8_t *buf, uint16_t len);

	/**
	 * \return SERIAL_OK if startSending can be called successfully.
	 */
	Status sendState() const;


	/**
	 * Start receiving a packet.
	 * \param buf Memory for the incoming packet.
	 * \param len Maximum length packet that will fit in buf.
	 */
	Status startReceiving(uint8_t *buf, uint16_t len);

	/**
	 * \return SERIAL_OK if a packet has been successfully received.  In this case len is an
	 * out parameter holding the length of the received packet.
	 * SERIAL_OVERRUN is returned if the length of the incoming packet exceeded the length of
	 * the buffer passed to \sa startReceiving.
	 */
	Status receiveState(uint16_t &len);


	/**
	 * Runs the serial protocol engine.  Must be called periodically to keep
	 * sending and receiving going.
	 */
	void runOnce();

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
		RX_OVERRUN,
        ERROR
    };

    struct Header {
        uint16_t len_;
        uint8_t *array() { return reinterpret_cast<uint8_t *> (this); }
    };

    struct Message {
    	uint16_t bufLen_;
        uint8_t *buf_;
        Header header_;
        State state_;
        uint16_t progress_;
    };

    fifo<uint8_t, FIFO_SIZE> rxFifo_;
    fifo<uint8_t, FIFO_SIZE> txFifo_;
    Message rx_;
    Message tx_;

    // driver specific
    uint16_t rxXferCount_;
    uint16_t txXferCount_;

    void sendBytes();
    void receiveBytes();
    void receiveDriver();
    void sendDriver();
};

#endif
