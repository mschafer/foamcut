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

#include <boost/chrono.hpp>
#include <Simulator.hpp>
#include <Device.hpp>
#include "Host.hpp"
#include "TCPLink.hpp"

namespace stepper {

Host::Host()
{

}

Host::~Host()
{

}

bool Host::connectToSimulator()
{
	device::Simulator::reset();
	uint16_t port = device::Simulator::instance().port();

	link_.reset(new TCPLink("localhost", port));
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	device::PingMsg *pm = new device::PingMsg();
	int tries = 10;
	while (link_->send(pm) != device::HAL::SUCCESS && tries > 0) {
		--tries;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
	if (tries == 0) return false;

	device::Message *rx;
	tries = 10;
	while ((rx = link_->receive()) == NULL && tries > 0) {
		--tries;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
	if (tries == 0) return false;

	if (rx->function() == device::PingResponseMsg::FUNCTION) {
		return true;
	} else {
		return false;
	}
}

}
