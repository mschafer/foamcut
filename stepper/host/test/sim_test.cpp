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
#include <TCPLink.hpp>
#include <Simulator.hpp>
#include <Dictionary.hpp>

void rtest(const boost::system::error_code &error)
{
	std::cout << "rtest" << std::endl;
}


BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	using namespace stepper;
	using namespace stepper::device;

	Simulator sim;
	std::cout << "sim port " << sim.port() << std::endl;
	TCPLink host("localhost", sim.port());

    boost::posix_time::time_duration d = boost::posix_time::milliseconds(1000);
    boost::this_thread::sleep(d);

    MessageBuffer *mb = host.alloc(0);
    PingMsg::init(*mb);
    host.send(mb);
    mb = host.alloc(0);
    PingMsg::init(*mb);
    host.send(mb);
    mb = host.alloc(0);
    PingMsg::init(*mb);
    host.send(mb);
    boost::this_thread::sleep(d);

#if 0
    std::cout << "available " << sim.socket().available() << std::endl;

    device::MessageHeader recvHeader;
    size_t r = boost::asio::read(sim.socket(),
		boost::asio::buffer(&recvHeader, sizeof(device::MessageHeader)));
    std::cout << "read " << r << std::endl;
	boost::asio::async_read(sim.socket(),
		boost::asio::buffer(&recvHeader, sizeof(device::MessageHeader)),
		boost::bind(&rtest, boost::asio::placeholders::error));

    std::cout << "sim send" << std::endl;
    mb = sim.alloc(0);
    PingMsg::init(*mb);
    sim.sendMessage(mb);
#endif

    BOOST_CHECK(host.receive() != nullptr);

	BOOST_CHECK(host.connected());
}
