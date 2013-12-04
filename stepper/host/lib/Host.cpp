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
#include <Platform.hpp>
#include <SimComm.hpp>
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
	device::Platform::reset();
	device::SimComm *comm = reinterpret_cast<device::SimComm*>(&device::Platform::getCommunicator());
	link_.reset(new TCPLink("localhost", comm->port()));

	auto end = boost::chrono::steady_clock::now() + boost::chrono::milliseconds(500);
	while (boost::chrono::steady_clock::now() < end) {
		if (link_->connected()) break;
	}
	if (!link_->connected()) return false;

	TCPLink::HostMessage *mb;
	mb = TCPLink::HostMessage::alloc(device::PingMsg<HostMessageAllocator>::PAYLOAD_SIZE);
	link_->send(mb);

	end = boost::chrono::steady_clock::now() + boost::chrono::milliseconds(500);
	while (boost::chrono::steady_clock::now() < end) {
		if ((mb = link_->receive()) != NULL) {
			uint8_t mbid = mb->id0();
			delete mb;
			if (mbid == device::PONG_MSG) return true;
		}
		boost::this_thread::yield();
	}
	return false;
}

}
