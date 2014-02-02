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
#ifndef ASIOImpl_hpp
#define ASIOImpl_hpp

/** \file
 * This file contains a boost::asio based implementation of sending and 
 * receiving messages that is meant to be shared across host and target.
 * It should be included only in .cpp file for the Communicator using it
 * and inside the namespace declaration.  The same .cpp file must include
 * a header to bring messages into the same namespace and headers listed
 * below.
 *
 * #include <boost/asio.hpp>
 * #include <boost/asio/deadline_timer.hpp>
 * #include <boost/utility.hpp>
 * #include <boost/thread.hpp>
 */

template <typename link_type>
class ASIOImpl : public boost::noncopyable
{
public:
	enum {
		OUT_OF_MEM_WAIT=100
	};

	ASIOImpl(link_type &link) : link_(link),
        recvInProgress_(NULL), sendInProgress_(NULL)
	{
	}

	~ASIOImpl() {}

	void startReceive() {
    	boost::asio::async_read(link_.socket(),
    		boost::asio::buffer(&recvHeader_, sizeof(MessageHeader)),
    		boost::bind(&ASIOImpl::headerComplete, this,
    		boost::asio::placeholders::error));
	}

    bool sending() const { return sendInProgress_ != NULL; }

	void startSend()
	{
		if (sendInProgress_ == NULL) {
            sendInProgress_ = link_.popSendQueue();
			if (sendInProgress_ != NULL) {
				boost::asio::async_write(link_.socket(),
					boost::asio::buffer(sendInProgress_->transmitStart(), sendInProgress_->transmitSize()),
					boost::bind(&ASIOImpl::sendComplete, this,
					boost::asio::placeholders::error));
			}
		}
	}

    void reset()
    {
        Message *m;
        while((m = link_.popSendQueue()) != NULL) {
            delete m;
        }

        if (sendInProgress_) {
            delete sendInProgress_;
            sendInProgress_ = NULL;
        }
        if (recvInProgress_) {
            delete recvInProgress_;
            recvInProgress_ = NULL;
        }
    }

private:
	link_type &link_;

    Message *recvInProgress_;
	Message *sendInProgress_;
	std::auto_ptr<boost::asio::deadline_timer> oomTimer;
	MessageHeader recvHeader_;


	void headerComplete(const boost::system::error_code &error)
	{
		if (!error) {
			uint16_t s = recvHeader_.payloadSize_;
            recvInProgress_ = Message::alloc(s);
			if (recvInProgress_ == NULL) {
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
					link_.handleMessage(recvInProgress_);
					recvInProgress_ = NULL;
					startReceive();
				}
			}
		} else {
			link_.handleError(error);
		}
	}

	void bodyComplete(const boost::system::error_code &error)
	{
		if (!error) {
			link_.handleMessage(recvInProgress_);
			recvInProgress_ = NULL;
            startReceive();
		} else {
			link_.handleError(error);
		}
	}

	void sendComplete(const boost::system::error_code &error)
	{
		if (sendInProgress_ != NULL) {
			delete sendInProgress_;
			sendInProgress_ = NULL;
		}

		if (!error) {
			startSend();
		} else {
			link_.handleError(error);
		}
	}

};

#endif
