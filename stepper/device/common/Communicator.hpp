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

#include "MessageQueue.hpp"

namespace stepper { namespace device {

class Message;

template <typename lock_type>
class Communicator
{
public:
	Communicator();
	virtual ~Communicator();

	/** Initialize the Communicator and block until a link with the host is successfully created. */
	virtual void initialize() = 0;

	/**
	 * Called periodically from the Stepper background loop to give the Communicator a chance to do
	 * necessary work.
	 */
	virtual void operator()() = 0;

	/**
	 * Send a message if the necessary resources are available.
	 * \param msg The Message to send must have been allocated from the MemoryAllocator
	 * and will be freed upon completion.
	 * \return true if successful.  If the send is unsuccessful, then the caller retains ownership of \em msg.
	 */
	virtual bool send(Message *msg) = 0;

	/**
	 * Check for received Messages.
	 * \return A pointer to the next available Message or NULL if none are available.
	 * The caller assumes ownership of any returned Message and must free it using the
	 * global MemoryAllocator when finished.
	 */
	Message *receive();

	/** \return true if the Communicator is currently connected to the host. */
	bool connected();

private:
	MessageQueue<lock_type> rxQueue_;
	MessageQueue<lock_type> txQueue_;

};

}}

#endif
