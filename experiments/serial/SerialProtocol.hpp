#ifndef SerialProtocol_hpp
#define SerialProtocol_hpp

#include <stdint.h>
#include <stdlib.h>

#include "LinkPair.hpp"

class SerialProtocol
{
public:
    
    enum {
        MAX_PACKET_SIZE = 1024
    };
    using size_type = uint16_t;

    explicit SerialProtocol(Link &link);

	/**
	 * Transmit a packet over the serial port.
     * Blocks until entire packet is sent.
     * \return true if packet was successfully sent and false if a 
     * fatal error occurred.
	 */
    bool sendPacket(const uint8_t *buff, size_type size);

	/**
     * Receive a serial packet.
     * \return Pointer to received packet or nullptr if one is not available.
     * The pointer is good until the next call to receivePacket.
	 */
    uint8_t *receivePacket();
    
	static uint8_t crc8(uint8_t seed, const uint8_t *data, size_t len);

private:
    
    Link &link_;
	uint8_t rxBuff_[MAX_PACKET_SIZE];
    size_type rxPos_;
    size_type rxLen_;
    uint8_t rxCtr_;
    uint8_t txCtr_;

    /**
     * Block until byte can be sent.
     */
    Link::ErrorCode blockingSend(uint8_t byte, bool sync=false);

    SerialProtocol() = delete;
    SerialProtocol(const SerialProtocol &cpy);
    SerialProtocol &operator=(const SerialProtocol &rhs);

};

#endif
