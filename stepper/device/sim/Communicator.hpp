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
#ifndef stepper_device_Communicator_hpp
#define stepper_device_Communicator_hpp

#include <stddef.h>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "Lock.hpp"
#include "CommunicatorBase.hpp"

namespace stepper { namespace device {

template <typename link_type> class ASIOImpl;

/**
 * Communicator implementation for boost based target.
 */
class Communicator : public CommunicatorBase<Communicator>
{
public:
    enum {
        HIGH_QUEUE_SIZE = 100,
        MEDIUM_QUEUE_SIZE = 100,
        LOW_QUEUE_SIZE = 10000
    };

    Communicator();
    ~Communicator();

    /**
     * Send a message to the host.
     * Communicator takes ownership of message and will delete it when finished.
     * \return true if the message can be sent and false if there is a problem.
     * If false is returned, then ownership of the message reverts to the callee.
     * The Communicator will not delete the message.
     */
    bool sendMessage(Message *message, Priority priority=LOW_PRIORITY);

    /** Call to start the connection process. */
    void initialize();

    /** Runs the background function once. */
    void operator()() { CommunicatorBase<Communicator>::operator()(); }

    /** \return the tcp/ip port used for connections. */
    uint16_t port() const;

private:
    friend class ASIOImpl<Communicator>;
	typedef std::list<device::Message*> MessageList;

    Lock lock_;
    MessageList hiList_;
    MessageList medList_;
    MessageList lowList_;
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::socket socket_;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::unique_ptr<ASIOImpl<Communicator> > impl_;
    std::unique_ptr<boost::thread> thread_;
    uint16_t port_;
    uint16_t testCount_, testSize_;
    TestState testState_;
    volatile bool running_;

    bool pushSendQueue(Message *message, Priority priority);
    Message *popSendQueue();
    
    boost::asio::ip::tcp::socket &socket() { return socket_; }
    boost::asio::io_service &ios() { return ios_; }
    void handleError(const boost::system::error_code &error);
    void handleMessage(Message *message);
    void run();
    void acceptComplete(const boost::system::error_code &error);

    /** Don't implement, this class is not copyable. */
	Communicator(const Communicator &cpy);

	/** Don't implement, this class is not assignable. */
	Communicator &operator=(const Communicator &cpy);
};

}}
#endif
