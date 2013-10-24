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
#include "Host.hpp"
#include "TCPLink.hpp"
#include <Simulator.hpp>
#include <Dictionary.hpp>

namespace stepper {

Host::Host()
{

}

Host::~Host()
{

}

bool Host::connectToSimulator()
{
	sim_.reset(new device::Simulator());
	link_.reset(new TCPLink("localhost", sim_->port()));

	device::MessageBuffer *mb = link_->alloc(device::PingMsg::PAYLOAD_SIZE);
	device::PingMsg::init(*mb);
	link_->send(mb);

	auto end = boost::chrono::steady_clock::now() + boost::chrono::milliseconds(500);
	while (boost::chrono::steady_clock::now() < end) {

	}

	return true;
}

}
