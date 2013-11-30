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
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vector>
#include <cfloat>
#include <cmath>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include "MemoryPool.hpp"


BOOST_AUTO_TEST_CASE( pool_test )
{
	using namespace stepper::device;
	const size_t poolSizes[2] = {16, 256};

	uint8_t block[1024];
	MemoryPool<2, size_t> mp(poolSizes, block, sizeof(block));

	void *b = mp.alloc(8);
}
