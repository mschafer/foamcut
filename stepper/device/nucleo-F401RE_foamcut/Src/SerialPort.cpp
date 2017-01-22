#include "SerialPort.hpp"
#include "usart.h"
#include "stm32f4xx_hal_uart.h"


SerialPort::Status
SerialPort::startSending(const uint8_t *buf, uint16_t len)
{
	Status ss = sendState();
	if (ss != SERIAL_OK) {
		return ss;
	}

    tx_.buf_ = const_cast<uint8_t*>(buf);
    tx_.header_.len_ = len;
    tx_.progress_ = 0;
    tx_.state_ = INIT_SYNC;

    sendBytes();
    if (tx_.state_ == ERROR) {
    	return SERIAL_ERROR;
    } else {
    	return SERIAL_OK;
    }
}

SerialPort::Status
SerialPort::sendState() const {
	switch (tx_.state_) {
	case COMPLETE:
		return SERIAL_OK;
		break;

	case ERROR:
		return SERIAL_ERROR;
		break;

	default:
		return SERIAL_BUSY;
	}
}

SerialPort::Status
SerialPort::startReceiving(uint8_t *buf, uint16_t len)
{
	Status ss = sendState();
	if (ss != SERIAL_OK || ss != SERIAL_OVERRUN) {
		return ss;
	}

    tx_.buf_ = buf;
    tx_.bufLen_ = len;
    tx_.progress_ = 0;
    tx_.state_ = INIT_SYNC;
    receiveBytes();
    return SERIAL_OK;
}

SerialPort::Status
SerialPort::receiveState(uint16_t &len) {
	switch (rx_.state_) {
	case COMPLETE:
		len = rx_.header_.len_;
		return SERIAL_OK;
		break;

	case RX_OVERRUN:
		len = rx_.bufLen_;
		return SERIAL_OVERRUN;
		break;

	default:
		return SERIAL_BUSY;
	}
}

void
SerialPort::runOnce()
{
	///\todo need to check that right thing happens when nothing to send or receive

	receiveDriver();
	receiveBytes();

	sendDriver();
	sendBytes();
}

void
SerialPort::sendBytes()
{
    fifo<uint8_t, FIFO_SIZE>::carray d = txFifo_.space_carray();
    size_t n = 0;
    while (n < d.len_ && tx_.state_ != COMPLETE) {
        switch (tx_.state_) {
            case INIT_SYNC:
                d.buf_[n] = SYNC;
                tx_.state_ = HEADER;
                tx_.progress_ = 0;
                break;

            case HEADER:
                d.buf_[n] = tx_.header_.array()[tx_.progress_];
                if (d.buf_[n] == SYNC) {
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
                d.buf_[n] = SYNC;
                ++tx_.progress_;
                if (tx_.progress_ == sizeof(Header)) {
                    tx_.state_ = PAYLOAD;
                    tx_.progress_ = 0;
                } else {
                    tx_.state_ = HEADER;
                }
                break;

            case PAYLOAD:
                d.buf_[n] = tx_.buf_[tx_.progress_];
                if (d.buf_[n] == SYNC) {
                    tx_.state_ = PAYLOAD_SYNC;
                } else {
                    ++tx_.progress_;
                    if (tx_.progress_ == tx_.header_.len_) {
                        tx_.state_ = COMPLETE;
                    }
                }
                break;

            case PAYLOAD_SYNC:
                d.buf_[n] = SYNC;
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

void
SerialPort::receiveBytes()
{
	fifo<uint8_t, FIFO_SIZE>::carray d = rxFifo_.contents_carray();
    size_t n = 0;
    while (n < d.len_ && rx_.state_ != COMPLETE) {
        switch (rx_.state_) {

            // looking for a sync
            case INIT_SYNC:
                if (d.buf_[n] == SYNC) {
                    rx_.state_ = HEADER;
                    rx_.progress_ = 0;
                }
                break;

            case HEADER:
                rx_.header_.array()[rx_.progress_] = d.buf_[n];
                if (d.buf_[n] == SYNC) {
                    rx_.state_ = HEADER_SYNC;
                } else {
                    ++rx_.progress_;
                    if (rx_.progress_ == sizeof(Header)) {
                        rx_.state_ = PAYLOAD;
                        rx_.progress_ = 0;
                    }
                }
                break;

            // still receiving length, but next byte needs to be a sync
            case HEADER_SYNC:
                if (d.buf_[n] == SYNC) {
                    ++rx_.progress_;
                    if (rx_.progress_ == sizeof(Header)) {
                        rx_.state_ = PAYLOAD;
                        rx_.progress_ = 0;
                    } else {
                        rx_.state_ = HEADER;
                    }
                } else {
                    // error, no second sync.  previous message was incomplete
                    // this is the first header byte of a new message
                    rx_.header_.array()[0] = d.buf_[n];
                    rx_.state_ = HEADER;
                }
                break;

            case PAYLOAD:
                rx_.buf_[rx_.progress_] = d.buf_[n];
                if (d.buf_[n] == SYNC) {
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
                if (d.buf_[n] == SYNC) {
                    ++rx_.progress_;
                    if (rx_.progress_ ==  rx_.header_.len_) {
                        rx_.state_ = COMPLETE;
                    } else {
                        rx_.state_ = PAYLOAD;
                    }
                } else {
                    // error, no second sync.  previous message was incomplete
                    // this is the second byte of a new message
                    rx_.header_.array()[0] = d.buf_[n];
                    rx_.state_ = HEADER;
                }
                break;

            case COMPLETE:
            	break;

            case RX_OVERRUN:
            	break;

            default:
                ///\todo error here
                break;
        }

        // check for rx buffer overflow
        if ((rx_.state_ == PAYLOAD || rx_.state_ == PAYLOAD_SYNC) &&
        	rx_.progress_ >= rx_.bufLen_) {
        	rx_.state_ = RX_OVERRUN;
        }
        ++n;
    }

    rxFifo_.contents_remove(n);
}

void SerialPort::receiveDriver()
{
	// adjust rxFifo contents based on progress of DMA transfer
	if (rxXferCount_ != 0) {
		uint16_t b = __HAL_DMA_GET_COUNTER(huart2.hdmarx);
		rxFifo_.contents_add(rxXferCount_ - b);
		rxXferCount_ = b;
	}

	// start a DMA transfer to the rx fifo if there isn't one running
	///\todo transfers might all need to be 32 bit aligned?
	if(huart2.RxState == HAL_UART_STATE_READY) {
		if (rxXferCount_ != 0) {
			rxFifo_.contents_add(rxXferCount_);
			rxXferCount_ = 0;
		}

		fifo<uint8_t, FIFO_SIZE>::carray d = rxFifo_.space_carray();
		if (d.len_ == 0) return;
		HAL_UART_Receive_DMA(&huart2, d.buf_, d.len_);
		rxXferCount_ = d.len_;
	}
}

void SerialPort::sendDriver()
{
    // adjust rxFifo contents based on progress of DMA transfer
    if (txXferCount_ != 0) {
        uint16_t b = __HAL_DMA_GET_COUNTER(huart2.hdmatx);
        txFifo_.contents_remove(txXferCount_ - b);
        txXferCount_ = b;
    }

    // start a DMA transfer from the tx fifo if there isn't one running
    if(huart2.gState == HAL_UART_STATE_READY) {
        txFifo_.contents_remove(txXferCount_);
        txXferCount_ = 0;
        fifo<uint8_t, FIFO_SIZE>::carray d = txFifo_.contents_carray();
        if (d.len_ == 0) return;
        HAL_UART_Transmit_DMA(&huart2, d.buf_, d.len_);
        txXferCount_ = d.len_;
    }
}
