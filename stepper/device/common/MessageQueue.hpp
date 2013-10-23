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

namespace stepper { namespace device {

/** Queue of MessageBuffer, FIFO semantics. */
template <typename lock_type>
class MessageQueue
{
public:
	MessageQueue() : head_(NULL), tail_(NULL), size_(0) {}
	~MessageQueue() {}

	/**
	 * Push \em v onto tail of list. */
	void push(MessageBuffer *v) {
		assert(v->next() == NULL);
		LockGuard<lock_type> guard(mtx_);
        if (tail_ != NULL) {
            tail_->next(v);
        }
        tail_ = v;

        if (head_ == NULL) head_ = v;
        ++size_;
	}

	/** Pop head off list. */
	MessageBuffer *pop() {
		LockGuard<lock_type> guard(mtx_);
        MessageBuffer *ret = head_;
        if (head_ != NULL) {
            head_ = head_->next();
            ret->next(NULL);
            if (head_ == NULL) tail_ = NULL;
        }
        --size_;
        return ret;
	}

	size_t size() const { return size_; }

private:
	MessageBuffer *head_;
	MessageBuffer *tail_;
	size_t size_;
	lock_type mtx_;

};

}}

#endif
