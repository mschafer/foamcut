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
#ifndef stepper_HostMessageAllocator_hpp
#define stepper_HostMessageAllocator_hpp

#include <boost/pool/singleton_pool.hpp>

namespace stepper {

struct HostMessageAllocator
{
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef boost::singleton_pool<HostMessageAllocator, 512> pool_type;

	static void *malloc(const size_type bytes) {
		return pool_type::malloc();
	}

	static void free(void *const p) {
		pool_type::free(p);
	}
};

}

#endif
