#include "SerialPort.hpp"

SerialPort::Status
SerialPort::send(const uint8_t *buf, uint16_t len)
{
    tx_.buf_ = const_cast<uint8_t*>(buf); // promise not to change it
    tx_.header_.len_ = len;
    tx_.progress_ = 0;
    tx_.state_ = INIT_SYNC;

    while (tx_.state_ != COMPLETE && tx_.state_ != ERROR) {
        sendNextBytes();
        ///\todo sleep here so fifo can empty out
    }

    if (tx_.state_ == COMPLETE) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

void
SerialPort::sendNextBytes()
{
    fifo<uint8_t, FIFO_SIZE>::carray d = txFifo_.space_carray();
    size_t n = 0;
    while (n < d.size_ && tx_.state_ != COMPLETE) {
        switch (tx_.state_) {
            case INIT_SYNC:
                d.addr_[n] = SYNC;
                tx_.state_ = HEADER;
                tx_.progress_ = 0;
                break;

            case HEADER:
                d.addr_[n] = tx_.header_.array()[tx_.progress_];
                if (d.addr_[n] == SYNC) {
                    tx_.state_ = HEADER_SYNC;
                } else {
                    ++tx_.progress_;
                    if (tx_.progress_ == sizeof(Header)) {
                        tx_.state_ = PAYLOAD;
                        tx_.progress_ = 0;
                    }
                }
                break;

            case HEADER_SYNC:
                d.addr_[n] = SYNC;
                ++tx_.progress_;
                if (tx_.progress_ == sizeof(Header)) {
                    tx_.state_ = PAYLOAD;
                    tx_.progress_ = 0;
                } else {
                    tx_.state_ = HEADER;
                }
                break;

            case PAYLOAD:
                d.addr_[n] = tx_.buf_[tx_.progress_];
                if (d.addr_[n] == SYNC) {
                    tx_.state_ = PAYLOAD_SYNC;
                } else {
                    ++tx_.progress_;
                    if (tx_.progress_ == tx_.header_.len_) {
                        tx_.state_ = COMPLETE;
                    }
                }
                break;

            case PAYLOAD_SYNC:
                d.addr_[n] = SYNC;
                ++tx_.progress_;
                if (tx_.progress_ == tx_.header_.len_) {
                    tx_.state_ = COMPLETE;
                } else {
                    tx_.state_ = PAYLOAD;
                }
                break;

            case COMPLETE:
            default:
                ///\todo error here
                break;
        }

        ++n;
    }
    txFifo_.contents_add(n);
}

SerialPort::Status
SerialPort::receive(uint8_t *&buf, uint16_t &len)
{
    return SUCCESS;
}
