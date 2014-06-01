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
#ifndef stepper_device_SimCommunicator_hpp
#define stepper_device_SimCommunicator_hpp

#include <stddef.h>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <deque>
#include <HAL.hpp>

namespace stepper { namespace device {

class Message;
class ASIOSender;
class ASIOReceiver;

/**
 * CommService implementation for boost based target.
 */
class SimCommunicator
{
public:
    enum {
        QUEUE_SIZE = 1000
    };

    SimCommunicator(uint16_t port=0);
    ~SimCommunicator();

    /**
     * Send a message to the host.
     * CommService takes ownership of message and will delete it when finished.
     * \return true if the message can be sent and false if there is a problem.
     * If false is returned, then ownership of the message reverts to the callee.
     * The CommService will not delete the message.
     */
    HAL::Status sendMessage(Message *message, HAL::Priority priority=HAL::NORMAL_PRIORITY);

    /** Call to start the connection process. */
    void initialize();

    /** \return the tcp/ip port used for connections. */
    uint16_t port() const;

    Message *receiveMessage();

private:
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::socket socket_;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::unique_ptr<ASIOSender> sender_;
    std::unique_ptr<ASIOReceiver> receiver_;
    std::unique_ptr<boost::thread> thread_;
    uint16_t port_;

    void handleError(const boost::system::error_code &error);
    void run();
    void acceptComplete(const boost::system::error_code &error);

    /** Don't implement, this class is not copyable. */
	SimCommunicator(const SimCommunicator &cpy);

	/** Don't implement, this class is not assignable. */
	SimCommunicator &operator=(const SimCommunicator &cpy);
};

}}

#endif
