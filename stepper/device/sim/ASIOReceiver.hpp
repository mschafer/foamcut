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
#include <Message.hpp>

namespace stepper { namespace device {

/**
 * Hold buffer(s) to receive raw bytes and then extract individual messages
 * for processing.
 */
class ASIOReceiver
{
public:
    typedef boost::function<void (Message*)> Handler;
    typedef boost::function<void (const boost::system::error_code &error)> ErrorCallback;

    ASIOReceiver(boost::asio::ip::tcp::socket &s, Handler h, ErrorCallback ec);
    ~ASIOReceiver();

private:
    enum {
        BUFFER_SIZE = 1<<22
    };

    boost::asio::ip::tcp::socket &socket_;
    Handler handler_;
    ErrorCallback errorCallback_;
    boost::mutex mtx_;
    boost::condition_variable data_;
    std::unique_ptr<boost::thread> thread_;

    Message *msg_;
    MessageHeader header_;
    size_t msgPos_;
    size_t bufferPos_;
    volatile size_t bufferEnd_; ///\todo std::atomic
    uint8_t buffer_[BUFFER_SIZE];

    void operator()();
    void readComplete(const boost::system::error_code &error, size_t bytesReceived);
    void extract();
    bool extractHeader();
};

}}

#endif
