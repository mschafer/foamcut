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
#ifndef stepper_device_ASIOReceiver_hpp
#define stepper_device_ASIOReceiver_hpp

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <Message.hpp>

namespace stepper { namespace device {

/**
 * Hold buffer(s) to receive raw bytes and then extract individual messages
 * for processing.
 */
template <typename ASIOWriteStream>
class ASIOReceiver
{
public:
    typedef boost::function<void (const boost::system::error_code &error)> ErrorCallback;

	ASIOReceiver(ASIOWriteStream &s, ErrorCallback ec);
    ~ASIOReceiver();

    Message *getMessage();

private:
	typedef boost::ptr_list<Message> MessageList;
    enum {
        BUFFER_SIZE = 1<<22
    };

	ASIOWriteStream &socket_;
    ErrorCallback errorCallback_;
    boost::mutex mtx_;
    MessageList rxList_;

    Message *msg_;
    MessageHeader header_;
    size_t msgPos_;
    size_t bufferPos_;
    size_t bufferEnd_;
    uint8_t buffer_[BUFFER_SIZE];

    void readComplete(const boost::system::error_code &error, size_t bytesReceived);
    void extract();
    bool extractHeader();
};

}}

#endif
