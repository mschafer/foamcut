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
#include <Host.hpp>
#include <Dictionary.hpp>

using boost::asio::ip::tcp;

char bufft[10];
char buffr[10];

void sendComplete(const boost::system::error_code &error)
{
	if (!error) {
		std::cout << "send completed" << std::endl;
	} else {
		std::cout << "send: " << error.message() << std::endl;
	}
}

void receiveComplete(const boost::system::error_code &error)
{
	if (!error) {
		std::cout << "receive completed" << std::endl;
	} else {
		std::cout << "receive: " << error.message() << std::endl;
	}
}

void connectComplete(boost::asio::ip::tcp::socket &s, const boost::system::error_code &error) {
	if (!error) {
		std::cout << "connection completed" << std::endl;
        s.async_send(boost::asio::buffer(bufft, sizeof(bufft)), boost::bind(&sendComplete, boost::asio::placeholders::error));
        s.async_receive(boost::asio::buffer(buffr, sizeof(buffr)), boost::bind(&receiveComplete, boost::asio::placeholders::error));
	} else {
		std::cout << "connect: " << error.message() << std::endl;
	}
}

void acceptComplete(boost::asio::ip::tcp::socket &s, const boost::system::error_code &error) {
	if (!error) {
		std::cout << "accept completed with " << s.available() << " bytes available" << std::endl;
	} else {
		std::cout << "accept: " << error.message() << std::endl;
	}
}


BOOST_AUTO_TEST_CASE( sim_ping_test )
{
	boost::asio::io_service ios;
    tcp::acceptor acc(ios, tcp::endpoint());
    uint16_t port = acc.local_endpoint().port();
    tcp::socket server(ios);

    std::ostringstream oss;
    oss << port;
    std::string portStr = oss.str();
    tcp::resolver resolver(ios);
    tcp::resolver::query q(tcp::v4(), "localhost", portStr.c_str());
    boost::system::error_code error;
    tcp::resolver::iterator it = resolver.resolve(q, error);
    tcp::socket client(ios);
    boost::asio::async_connect(client, it, boost::bind(&connectComplete, boost::ref(client),
   		boost::asio::placeholders::error));

    for (int iter=0; iter<100; iter++) {
    	boost::system::error_code ec;
    	ios.poll(ec);
    	if (ec) {
    		std::cout << ec.message() << std::endl;
    	}
        if (iter == 10) {
            std::cout << "trying to accept" << std::endl;
            acc.async_accept(server, boost::bind(&acceptComplete, boost::ref(server), boost::asio::placeholders::error));
        }
    	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

	using namespace stepper;
	using namespace stepper::device;


	Host host;

	BOOST_CHECK(host.connectToSimulator());
}
