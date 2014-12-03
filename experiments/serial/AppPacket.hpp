#ifndef APDU_hpp
#define APDU_hpp

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
};

#endif