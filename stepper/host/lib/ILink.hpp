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
#ifndef stepper_ILink_hpp
#define stepper_ILink_hpp

#include <stdint.h>
#include <deque>
#include <vector>
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

namespace stepper {

/**
 * Interface definition for the communications link between the host and device.
 * The implementation must support writing and reading from different threads.
 */
class ILink : public boost::noncopyable
{
public:
	typedef boost::shared_ptr<ILink> handle;

	ILink() {}
	virtual ~ILink() {}

	/**
	 * Write a sequence of bytes to the device.
	 * This method returns immediately and the write is handled asynchronously.
	 */
	virtual void write(const std::deque<uint8_t> &bytes) = 0;

	/**
	 * Read a sequence of bytes from the device.
	 * \return A deque containing all available bytes from the device.
	 */
	virtual std::deque<uint8_t> read() = 0;

	/**
	 * Read a single byte from the device.
	 * \return Optional containing the byte read or empty if nothing is available.
	 */
	virtual boost::optional<uint8_t> readByte() = 0;
};

}

#endif
