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
#include "ASIOSender.hpp"
#include <boost/foreach.hpp>
#include <boost/iterator/transform_iterator.hpp>

namespace stepper { namespace device {

/** Functor to create a boost::asio::const_buffer from a Message*. */
struct MessageToBuffer {
    boost::asio::const_buffer operator()(const Message *msg) const {
        return boost::asio::const_buffer(msg->transmitStart(), msg->transmitSize());
    }
};

/**
 * Implementation of boost::asio::ConstBufferSequence for gather writes. 
 * T must be a sequnece of Message*
 */
template <typename T>
struct ConstBufferSequence {
    typedef boost::asio::const_buffer value_type;
    typedef boost::transform_iterator<MessageToBuffer, typename T::const_iterator, boost::asio::const_buffer> const_iterator;

    ConstBufferSequence(T &l) : list_(l) {}

    /// clears the list and deletes all the messages in it
    void clearAndDelete() {
        while (!list_.empty()) {
            delete(list_.front());
            list_.pop_front();
        }
    }

    const_iterator begin() const { return const_iterator(list_.begin()); }
    const_iterator end() const { return const_iterator(list_.end()); }

    T &list_;
};

template <typename ASIOWriteStream>
ASIOSender<ASIOWriteStream>::ASIOSender(ASIOWriteStream &s, ErrorCallback ec) : socket_(s),
    errorCallback_(ec), toSend_(QUEUE_SIZE, NULL), beingSent_(QUEUE_SIZE, NULL), status_(SUCCESS),
    waitingSenders_(0), inProgress_(false)
{
    toSend_.clear();
    beingSent_.clear();
}

template <typename ASIOWriteStream>
ASIOSender<ASIOWriteStream>::~ASIOSender()
{
    boost::unique_lock<boost::mutex> lock(mtx_);
    status_ = FATAL_ERROR;
    while (waitingSenders_ > 0) {
        lock.unlock();
        full_.notify_all();
        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
        lock.lock();
    }
    clearQueues();
}

template <typename ASIOWriteStream>
ErrorCode ASIOSender<ASIOWriteStream>::enqueue(Message *msg)
{
	boost::unique_lock<boost::mutex> lock(mtx_);
	while (toSend_.size() >= QUEUE_SIZE && status_ == SUCCESS) {
		++waitingSenders_;

		try {
			full_.wait(lock);
		}

		// don't send if the wait throws an error
		catch (...) {
			--waitingSenders_;
			return RESOURCE_UNAVAILABLE;
		}

		--waitingSenders_;
	}

	if (status_ == SUCCESS) {
		toSend_.push_back(msg);
		if (!inProgress_) {
			startSending();
		}
		return SUCCESS;
	}
	else {
		return FATAL_ERROR;
	}
}

/**
 * Starts another asynchronous send if there is data available.
 * Required preconditions:
 *  mtx_ must be locked
 *  inProgress_ must be false
 *  beingSent_ must be empty
 */
template <typename ASIOWriteStream>
void ASIOSender<ASIOWriteStream>::startSending()
{
    assert(inProgress_ == false);
    if (toSend_.empty()) return;

    assert(beingSent_.empty());
    beingSent_.swap(toSend_);
    full_.notify_all();

    ConstBufferSequence<MessageQueue> buffSeq(beingSent_);
    inProgress_ = true;
    boost::asio::async_write(socket_,
        buffSeq,
        boost::bind(&ASIOSender::sendComplete, this,
        boost::asio::placeholders::error));
}

template <typename ASIOWriteStream>
void ASIOSender<ASIOWriteStream>::sendComplete(const boost::system::error_code &error)
{
    BOOST_FOREACH(Message *m, beingSent_) {
        delete m;
    }
    beingSent_.clear();

	boost::lock_guard<boost::mutex> guard(mtx_);
	inProgress_ = false;
	if (error) {
		status_ = FATAL_ERROR;
		clearQueues();
		errorCallback_(error);
	}
	else {
		startSending();
	}
}

template <typename ASIOWriteStream>
void ASIOSender<ASIOWriteStream>::clearQueues()
{
    BOOST_FOREACH(Message *m, beingSent_) {
        delete m;
    }
    beingSent_.clear();

    BOOST_FOREACH(Message *m, toSend_) {
        delete m;
    }
    toSend_.clear();
}

template class ASIOSender<boost::asio::ip::tcp::socket>;
template class ASIOSender<boost::asio::serial_port>;

}}
