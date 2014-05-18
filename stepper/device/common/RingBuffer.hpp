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
#ifndef foamcut_device_RingBuffer_hpp
#define foamcut_device_RingBuffer_hpp

#include <stdint.h>

namespace stepper { namespace device {

template <typename T, uint16_t N>
class RingBuffer
{
public:
	RingBuffer() : head_(0), tail_(0) {
	}

	bool empty() const {
		return (head_ == tail_);
	}

	bool full() const {
		return ( head_+1 == tail_ || (tail_ == 0 && head_ == N-1));
	}

	void clear() {
		head_ = 0;
		tail_ = 0;
	}

	/// \return The number of elements in the RingBuffer.
	uint16_t count() const {
		int32_t d = head_ - tail_;
		d = d<0 ? d+N : d;
		return d;
	}
	
	/// \return space available in RingBuffer == number of pushes that will succeed.
	uint16_t space() const {
		return N - count() - 1;
	}

	/**
	 * Write a new value into the RingBuffer.
	 * This method will overwrite the oldest data if the RingBuffer is full.
	 */
	void push(const T &val) {
		buff_[head_] = val;
		increment(head_);
	}
	
	/**
	 * Removes the oldest element.
	 * It is an error or bug to call this on an empty RingBuffer.
	 */
	void pop() {
		increment(tail_);
	}
	
	/**
	 * Returns the oldest element.
	 */
	T front() {
		return buff_[tail_];
	}

private:
	void increment(uint16_t &idx) {
		uint16_t i = idx+1;
		idx = i == N ? 0 : i;
	}

	T buff_[N];
	uint16_t head_;  ///\< push new items here
	uint16_t tail_;  ///\< pop from here
};

}}

#endif
