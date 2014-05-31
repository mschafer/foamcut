/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#include "ASIOReceiver.hpp"
#include <boost/foreach.hpp>

namespace stepper { namespace device {


ASIOReceiver::ASIOReceiver(boost::asio::ip::tcp::socket &s, ErrorCallback ec) :
    socket_(s), errorCallback_(ec), msg_(NULL), msgPos_(0), bufferPos_(0), bufferEnd_(0)
{
    socket_.async_read_some(boost::asio::buffer(buffer_, BUFFER_SIZE),
        boost::bind(&ASIOReceiver::readComplete, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

ASIOReceiver::~ASIOReceiver()
{
}

Message *ASIOReceiver::getMessage()
{
	boost::lock_guard<boost::mutex> guard(mtx_);
	if (!rxList_.empty()) {
		MessageList::auto_type r = rxList_.pop_front();
		return r.release();
	} else {
		return nullptr;
	}
}

void ASIOReceiver::readComplete(const boost::system::error_code &error, size_t bytesReceived)
{
    if (error) {
        errorCallback_(error);
    } else {
        assert(bufferPos_ == 0);
        assert(bytesReceived != 0);
        bufferEnd_ = bytesReceived;
        extract();
        socket_.async_read_some(boost::asio::buffer(buffer_, BUFFER_SIZE),
            boost::bind(&ASIOReceiver::readComplete, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
}

void ASIOReceiver::extract()
{
    while (bufferPos_ != bufferEnd_) {
        boost::this_thread::interruption_point();

        if (msg_ == NULL) {
            if (extractHeader()) {
                msg_ = Message::alloc(header_.payloadSize_);
                if (msg_ == NULL) {
                    throw std::bad_alloc();
                }
                msg_->header(header_);
            }

            // did we receive enough for a complete header?
            if (msg_ == NULL) {
                continue;
            }
        }

        assert(msg_ != NULL);
        size_t n = msg_->transmitSize();
        uint8_t *p = msg_->transmitStart();
        while (msgPos_ < n && bufferPos_ != bufferEnd_) {
            p[msgPos_++] = buffer_[bufferPos_++];
        }

        if (msgPos_ == msg_->transmitSize()) {
        	boost::lock_guard<boost::mutex> guard(mtx_);
        	rxList_.push_back(msg_);
            msg_ = NULL;
            msgPos_ = 0;
        }
    }
    bufferPos_ = bufferEnd_ = 0;
    return;
}

bool ASIOReceiver::extractHeader() {
    uint8_t *pHeader = reinterpret_cast<uint8_t*>(&header_);
    while (bufferPos_ != bufferEnd_ && msgPos_ < sizeof(MessageHeader)) {
        pHeader[msgPos_++] = buffer_[bufferPos_++];
    }
    if (msgPos_ == sizeof(MessageHeader)) {
        assert(header_.payloadSize_ <= Message::maxPayloadCapacity());
        return true;
    } else {
        return false;
    }
}


}}
