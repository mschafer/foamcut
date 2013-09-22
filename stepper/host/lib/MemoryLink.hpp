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
#ifndef stepper_MemoryLink_hpp
#define stepper_MemoryLink_hpp

#include <boost/thread.hpp>
#include "ILink.hpp"

namespace stepper {

/**
 * Interface definition for the communications link between the host and device.
 * The implementation must support writing and reading from different threads.
 */
class MemoryLink : public ILink
{
public:
	typedef std::deque<uint8_t> queue_t;
	typedef boost::shared_ptr<queue_t> queue_ptr;
	typedef boost::shared_ptr<boost::mutex> mutex_ptr;
	typedef boost::shared_ptr<MemoryLink> handle;

	MemoryLink(queue_ptr in, queue_ptr out, mutex_ptr m) :
		in_(in), out_(out), pmutex_(m) {
	}

	virtual ~MemoryLink() {
	}

	/**
	 * Write a sequence of bytes to the device.
	 * This method returns immediately and the write is handled asynchronously.
	 * Note that vector is used instead of deque because some implementations require contiguous memory.
	 */
	void write(const std::vector<uint8_t> &bytes) {
		boost::lock_guard<boost::mutex> guard(*pmutex_);
		out_->insert(out_->end(), bytes.begin(), bytes.end());
	}

	/**
	 * Read a sequence of bytes from the device.
	 * \return A deque containing all available bytes from the device.
	 */
	std::deque<uint8_t> read() {
		boost::lock_guard<boost::mutex> guard(*pmutex_);
		std::deque<uint8_t> ret;
		ret.swap(*in_);
		return ret;
	}

	/**
	 * Read a single byte from the device.
	 * \return Optional containing the byte read or empty if nothing is available.
	 */
	boost::optional<uint8_t> readByte() {
		boost::lock_guard<boost::mutex> guard(*pmutex_);
		boost::optional<uint8_t> ret;
		if (!in_->empty()) {
			ret = in_->front();
			in_->pop_front();
		}
		return ret;
	}

	/**
	 * Create a pair of MemoryLinks that are connected to one another.
	 * Data written to one can be read from the other and vice versa.
	 */
	static std::pair<handle, handle> createPair() {
		mutex_ptr pmutex(new boost::mutex());
		queue_ptr qa(new queue_t());
		queue_ptr qb(new queue_t());

		handle a(new MemoryLink(qa, qb, pmutex));
		handle b(new MemoryLink(qb, qa, pmutex));
		return std::make_pair(a, b);
	}

private:
	MemoryLink();

	queue_ptr in_;
	queue_ptr out_;
	mutex_ptr pmutex_;
};

}

#endif
