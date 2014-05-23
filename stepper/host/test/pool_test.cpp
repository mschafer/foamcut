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
#include "Message.hpp"
#include "MemoryAllocator.hpp"


BOOST_AUTO_TEST_CASE( pool_test )
{
	using namespace stepper::device;
	MemoryAllocator &ma = MemoryAllocator::instance();

	void *p = ma.alloc(ma.maxCapacity());

	BOOST_CHECK(p != nullptr);
	BOOST_CHECK_EQUAL(ma.capacity(p), ma.maxCapacity());
	BOOST_CHECK_LT(Message::maxPayloadCapacity(), ma.maxCapacity());

	Message *m = Message::alloc(Message::maxPayloadCapacity());
	BOOST_CHECK_EQUAL(m->maxPayloadCapacity(), Message::maxPayloadCapacity());
}
