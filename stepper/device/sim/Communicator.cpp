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
#include "Communicator.hpp"
#include <IDevice.hpp>
#include <LockGuard.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <iostream>

namespace stepper { namespace device {

#include "ASIOImpl.hpp"

Communicator::Communicator() : socket_(ios_), port_(0), testCount_(0), testSize_(0),
    testState_(NO_TEST), running_(true)
{
    impl_.reset(new ASIOImpl<Communicator>(*this));
}

Communicator::~Communicator()
{
    running_ = false;
    ios_.stop();
    thread_->join();
    hiList_.clear();
    medList_.clear();
    lowList_.clear();
}

void Communicator::initialize()
{
    using boost::asio::ip::tcp;
    if (port_ != 0) {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint(tcp::v4(), port_)));
    } else {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint()));
        port_ = acceptor_->local_endpoint().port();
    }

    thread_.reset(new boost::thread(boost::bind(&Communicator::run, this)));
}

uint16_t Communicator::port() const
{
    return port_;
}

void Communicator::run()
{
    using namespace boost::asio::ip;
    try {
        while (running_) {
            acceptor_->async_accept(socket_,
            boost::bind(&Communicator::acceptComplete, this,
                boost::asio::placeholders::error));
                ios_.run();
            ios_.reset();
        }
    }

    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
        std::cerr << ex.what();
    }

    catch(...) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
    }
}

void Communicator::acceptComplete(const boost::system::error_code &error)
{
    if (!error) {
        impl_->startReceive();
    }
}

bool Communicator::sendMessage(Message *message, Priority priority)
{
    bool ret = pushSendQueue(message, priority);
    impl_->startSend();
    if (!ret) {
    	getStatusFlags().set(StatusFlags::COMM_SEND_FAILED);
    }
    return ret;
}

void Communicator::handleError(const boost::system::error_code &error)
{
    connected_ = false;
    socket_.close();
    /// \todo Application::connectionChanged(false);
    impl_->reset();
}

void Communicator::handleMessage(Message *message)
{
    if (message->id() == COMMUNICATOR_ID) {
        handleCSMessage(message);
        message = NULL;
    } else {
        /// \todo Application::dispatch(message);
        message = NULL;
    }
    if (message != NULL) delete message;
}

bool Communicator::pushSendQueue(Message *message, Priority priority)
{
    assert(message != NULL);
    LockGuard guard(lock_);
    switch (priority) {
    case HIGH_PRIORITY:
        if (hiList_.size() >= HIGH_QUEUE_SIZE) return false;
        hiList_.push_back(message);
        break;

    case MEDIUM_PRIORITY:
        if (medList_.size() >= MEDIUM_QUEUE_SIZE) return false;
        medList_.push_back(message);
        break;

    case LOW_PRIORITY:
        if (lowList_.size() >= LOW_QUEUE_SIZE) return false;
        lowList_.push_back(message);
        break;

    default:
        assert(0);
    }
    return true;
}

Message *Communicator::popSendQueue()
{
    LockGuard guard(lock_);
    Message *ret = NULL;
    if (!hiList_.empty()) {
    	ret = hiList_.front();
        hiList_.pop_front();
    } else if (!medList_.empty()) {
    	ret = medList_.front();
        medList_.pop_front();
    } else if (!lowList_.empty()) {
    	ret = lowList_.front();
        lowList_.pop_front();
    }
    return ret;
}

}}
