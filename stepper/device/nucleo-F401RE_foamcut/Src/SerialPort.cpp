#include "SerialPort.hpp"
#include "usart.h"
#include "stm32f4xx_hal_uart.h"


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
	fifo<uint8_t, FIFO_SIZE>::carray d = rxFifo_.contents_carray();
    size_t n = 0;
    while (n < d.size_ && rx_.state_ != COMPLETE) {
        switch (rx_.state_) {

            // looking for a sync
            case INIT_SYNC:
                if (d.addr_[n] == SYNC) {
                    rx_.state_ = HEADER;
                    rx_.progress_ = 0;
                }
                break;

            case HEADER:
                rx_.header_.array()[rx_.progress_] = d.addr_[n];
                if (d.addr_[n] == SYNC) {
                    rx_.state_ = HEADER_SYNC;
                } else {
                    ++rx_.progress_;
                    if (rx_.progress_ == sizeof(Header)) {
                        rx_.state_ = PAYLOAD;
                        rx_.progress_ = 0;
                        ///\todo should check payload size against buffer size
                    }
                }
                break;

            // still receiving length, but next byte needs to be a sync
            case HEADER_SYNC:
                if (d.addr_[n] == SYNC) {
                    ++rx_.progress_;
                    if (rx_.progress_ == sizeof(Header)) {
                        rx_.state_ = PAYLOAD;
                        rx_.progress_ = 0;
                        ///\todo should check payload size against buffer size
                    } else {
                        rx_.state_ = HEADER;
                    }
                } else {
                    // error, no second sync.  previous message was incomplete
                    // this is the first header byte of a new message
                    rx_.header_.array()[0] = d.addr_[n];
                    rx_.state_ = HEADER;
                }
                break;

            case PAYLOAD:
                rx_.buf_[rx_.progress_] = d.addr_[n];
                if (d.addr_[n] == SYNC) {
                    rx_.state_ = PAYLOAD_SYNC;
                } else {
                    ++rx_.progress_;
                    if (rx_.progress_ ==  rx_.header_.len_) {
                        rx_.state_ = COMPLETE;
                    }
                }
                break;

            // still receiving payload, but next byte needs to be a sync
            case PAYLOAD_SYNC:
                if (d.addr_[n] == SYNC) {
                    ++rx_.progress_;
                    if (rx_.progress_ ==  rx_.header_.len_) {
                        rx_.state_ = COMPLETE;
                    } else {
                        rx_.state_ = PAYLOAD;
                    }
                } else {
                    // error, no second sync.  previous message was incomplete
                    // this is the second byte of a new message
                    rx_.header_.array()[0] = d.addr_[n];
                    rx_.state_ = HEADER;
                }
                break;

            case COMPLETE:
            default:
                ///\todo error here
                break;
        }
        ++n;
    }

    rxFifo_.contents_remove(n);
    if (rx_.state_ == COMPLETE) {
        rx_.state_ = INIT_SYNC;
        rx_.progress_ = 0;
        buf = rx_.buf_;
        len = rx_.header_.len_;
        return SUCCESS;
    } else {
        buf = NULL;
        len = 0;
        return SUCCESS;
    }
}

void SerialPort::receiveWork()
{
	///\todo consider switching rx to ping pong buffers instead of fifo

	// adjust rxFifo contents based on progress of DMA transfer
	if (rxXferCount_ != 0) {
		uint16_t b = __HAL_DMA_GET_COUNTER(huart2.hdmarx);
		rxFifo_.contents_add(rxXferCount_ - b);
		rxXferCount_ = b;
	}

	// start a DMA tranfer to the rx fifo if there isn't one running
	///\todo transfers might all need to be 32 bit aligned?
	if(huart2.RxState == HAL_UART_STATE_READY) {
		fifo<uint8_t, FIFO_SIZE>::carray d = rxFifo_.contents_carray();
		if (d.size_ == 0) return;
		HAL_UART_Receive_DMA(&huart2, d.addr_, d.size_);
		rxXferCount_ = d.size_;
	}
}
