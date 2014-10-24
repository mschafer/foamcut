#ifndef APDU_hpp
#define APDU_hpp

#include <memory>
#include <stdint.h>

/**
 * Application Protocol Data Unit
 * Atomic unit of application communications.
 */
struct APDU
{
    APDU() : size_(0) {}
    APDU(size_t s) {
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

class APDUFactory
{
public:
    void feed(uint8_t *bytes, size_t &count);


};

#endif