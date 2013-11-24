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
#ifndef stepper_device_MessageQueue_hpp
#define stepper_device_MessageQueue_hpp

#include <stddef.h>
#include <assert.h>
#include "Message.hpp"
#include "LockGuard.hpp"
#include "SList.hpp"

namespace stepper { namespace device {

/** Queue of MessageBuffer, FIFO semantics and thread safe. */
template <typename lock_type>
class MessageQueue
{
public:
	MessageQueue()  {}
	~MessageQueue() {}

	/**
	 * Push \em v onto tail of list. */
	void push(Message *v) {
		LockGuard<lock_type> guard(mtx_);
		list_.pushBack(v);
	}

	/** Pop head off list. */
	Message *pop() {
		LockGuard<lock_type> guard(mtx_);
		if (list_.empty()) return NULL;
		return &(list_.popFront());
	}

	size_t size() const { return list_.size(); }

private:
	lock_type mtx_;
	SList<Message> list_;

};

}}

#endif
