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
#ifndef stepper_device_ASIOSender_hpp
#define stepper_device_ASIOSender_hpp

#include <atomic>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Message.hpp>
#include <HAL.hpp>

namespace stepper { namespace device {

class ASIOSender
{
public:
    typedef boost::function<void (const boost::system::error_code &error)> ErrorCallback;

    ASIOSender(boost::asio::ip::tcp::socket &s, ErrorCallback ec);
    ~ASIOSender();

    ErrorCode enqueue(Message *msg);

private:
    enum {
        QUEUE_SIZE = 1<<10
    };

    typedef std::deque<Message*> MessageQueue;

    boost::mutex mtx_;
    boost::condition_variable full_;
    boost::asio::ip::tcp::socket &socket_;
    ErrorCallback errorCallback_;
    MessageQueue toSend_;
    MessageQueue beingSent_;
    std::atomic<ErrorCode> status_;
    std::atomic<int> waitingSenders_;
    std::atomic<bool> inProgress_;

    void startSending();
    void sendComplete(const boost::system::error_code &error);
    void clearQueues();
};

}}

#endif

