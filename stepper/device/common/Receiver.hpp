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
#ifndef stepper_device_Receiver_hpp
#define stepper_device_Receiver_hpp

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

namespace stepper { namespace device {

class Message;

class Receiver
{
public:

    enum {
    	MAX_ID = 3
    };
	
    Receiver() {}
    virtual ~Receiver() {}

	/**
	 * \return The unique identifier for this Receiver.
	 */
    virtual uint8_t id() const = 0;

    /**
     * Called when a message comes in for this Receiver.
     * \param message The message to be handled by the application.  It must
     * be deleted when handling is complete.
     */
    virtual void receiveMessage(Message *message) = 0;

    /**
     * Called when the state of the connection changes.
	 */
	virtual void connected(bool c) = 0;

    /**
     * Lookup the Application instance by its id.
     * \return Pointer to the Application with the specified id.
     * There can be only one.  NULL if nothing exists.
     * The Application must be enabled to be found.
     */
    static Receiver *findById(uint8_t id);

    /** Called by Communicator when it receives a message. */
    static void dispatch(Message *message);

    /** Called by Communicator when it connects or disconnects. */
    static void connectionChanged(bool connected);


protected:
    void enable() {
        assert(id() <= MAX_ID);
        assert(registry_[id()] == NULL);
        registry_[id()] = this;
    }

    void disable() {
        assert(id() <= MAX_ID);
        assert(registry_[id()] != NULL);
        registry_[id()] = NULL;
    }

private:
    static Receiver *registry_[MAX_ID + 1];

	Receiver(const Receiver &);
	const Receiver &operator=(const Receiver &);
};

}}

#endif
