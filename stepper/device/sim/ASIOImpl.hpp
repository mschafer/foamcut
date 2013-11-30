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
#ifndef stepper_device_ASIOImpl_hpp
#define stepper_device_ASIOImpl_hpp

#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include "MessageQueue.hpp"

namespace stepper { namespace device {

template <typename link_type>
class ASIOImpl : public boost::noncopyable
{
public:
	enum {
		OUT_OF_MEM_WAIT=100
	};

	ASIOImpl(link_type &link) : link_(link), recvInProgress_(NULL),
	    sendInProgress_(NULL)
	{
	}

	~ASIOImpl() {}

	void receiveOne() {
    	boost::asio::async_read(link_.socket(),
    		boost::asio::buffer(&recvHeader_, sizeof(device::MessageHeader)),
    		boost::bind(&ASIOImpl::headerComplete, this,
    		boost::asio::placeholders::error));
	}

	void startSending()
	{
		if (sendInProgress_ == nullptr) {
			sendInProgress_ = link_.popTx();
			if (sendInProgress_ != nullptr) {
				boost::asio::async_write(link_.socket(),
					boost::asio::buffer(sendInProgress_->transmitStart(), sendInProgress_->transmitSize()),
					boost::bind(&ASIOImpl::sendComplete, this,
					boost::asio::placeholders::error));
			}
		}
	}

private:
	link_type &link_;
	device::MessageHeader recvHeader_;
	typename link_type::Message_type *recvInProgress_;
	typename link_type::Message_type *sendInProgress_;
	std::unique_ptr<boost::asio::deadline_timer> oomTimer;


	void headerComplete(const boost::system::error_code &error)
	{
		if (!error) {
			uint16_t s = recvHeader_.payloadSize_;
			recvInProgress_ = link_type::Message_type::alloc(s);
			if (recvInProgress_ == NULL) {
				 ///\todo indicate out of memory error
				oomTimer.reset(new boost::asio::deadline_timer(link_.ios(), boost::posix_time::milliseconds(OUT_OF_MEM_WAIT)));
		        oomTimer->async_wait(boost::bind(&ASIOImpl::headerComplete, this, error));

			} else {
				recvInProgress_->header(recvHeader_);
				if (s > 0) {
					boost::asio::async_read(link_.socket(),
						boost::asio::buffer(recvInProgress_->payload(), s),
						boost::bind(&ASIOImpl::bodyComplete, this,
						boost::asio::placeholders::error));
				} else {
					link_.handler(recvInProgress_, error);
					recvInProgress_ = nullptr;
					receiveOne();
				}
			}
		} else {
			link_.handler(nullptr, error);
		}
	}

	void bodyComplete(const boost::system::error_code &error)
	{
		if (!error) {
			link_.handler(recvInProgress_, error);
			recvInProgress_ = nullptr;
	    	boost::asio::async_read(link_.socket(),
	    		boost::asio::buffer(&recvHeader_, sizeof(device::MessageHeader)),
	    		boost::bind(&ASIOImpl::headerComplete, this,
	    		boost::asio::placeholders::error));
		} else {
			link_.handler(nullptr, error);
		}
	}

	void sendComplete(const boost::system::error_code &error)
	{
		if (sendInProgress_ != nullptr) {
			delete sendInProgress_;
			sendInProgress_ = nullptr;
		}

		if (!error) {
			startSending();
		} else {
			link_.handler(nullptr, error);
		}
	}

};

}}

#endif
