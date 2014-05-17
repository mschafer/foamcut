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
#ifndef stepper_device_MessageStorage_hpp
#define stepper_device_MessageStorage_hpp

#include <boost/intrusive/list.hpp>

namespace stepper { namespace device {

struct MessageHeader
{
    /** Size of payload in bytes does not include header. */
	uint16_t payloadSize_;

    /** Id of the sending Instrumentation App. */
	uint8_t id_;

    /** For use by the Instrumentation App. */
	uint8_t function_;
};

class MessageStorage {

	boost::intrusive::list_base_hook<> listHook_;

	// reserve memory for header and preserve alignment for payload
    // header_ may not actually contain the header!
    // public only for storage layout, do not access directly
    union {
    	void *align_;
	    uint8_t reserved_[sizeof(void*)];
        MessageHeader fauxHeader_;
    };
};

typedef boost::instrusive::list<MessageStorage, boost::intrusive::list_member_hook<>, &MessageStorage::listHook_> MessageList;

}}

#endif
