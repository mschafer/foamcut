#ifndef APDU_hpp
#define APDU_hpp

#include <boost/ptr_container/ptr_deque.hpp>
#include <memory>
#include <stdint.h>

/**
 * Simulated application data packet.
 * The application fills these packets and passes them to the protocol layer
 * for transmission or the protocol layer receives data and assembles it into
 * these to pass back to the application.
 */
struct AppPacket
{
    AppPacket() : size_(0) {}
    AppPacket(size_t s) {
        resize(s);
    }

    void resize(size_t s) {
        data_.reset(new uint8_t[s]);
        size_ = s;
    }

    size_t size() const { return size_; }
    uint8_t *data() { return data_.get(); }
    const uint8_t *data() const { return data_.get(); }

private:
    size_t size_;
    std::unique_ptr<uint8_t[]> data_;
	static boost::ptr_deque<AppPacket> recvQueue_;

};

/**
 * This class assembles bytes from the serial port into AppPackets.
 */
class AppPacketFactory
{
public:
	AppPacketFactory();
	~AppPacketFactory();

	/**
	* Take bytes from the SerialProtocol and assemble them into AppPackets.
	* \param data Buffer of bytes received from serial
	* \param size Numver of bytes in \sa data
	* \return Bytes actually consumed by the factory <= size.  If the factory cannot
	* consume all the bytes, then flow control must insure that the sender stops.
	*/
	size_t operator()(const uint8_t *data, size_t size);

	AppPacket *receive();

private:
	AppPacket *recv_;
	size_t pos_;
	boost::ptr_deque<AppPacket> recvQueue_;


};

#endif