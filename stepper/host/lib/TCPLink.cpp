#include "TCPLink.hpp"

namespace stepper {

#include <ASIOImpl.hpp>


TCPLink::TCPLink(const char *hostName, uint16_t port) :
	hostName_(hostName), socket_(ios_), running_(true), connected_(false)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
	impl_.reset(new ASIOImpl<TCPLink>(*this));
    thread_.reset(new boost::thread(boost::bind(&TCPLink::run, this)));
}

TCPLink::~TCPLink()
{
	try {
		running_ = false;
		ios_.stop();
		thread_->join();
	} catch (...) {
		///\todo ?
	}
}

void TCPLink::send(Message *mb)
{
	{
		boost::lock_guard<boost::mutex> guard(mtx_);
		txList_.push_back(mb);
	}
	impl_->startSend();
}

void TCPLink::run()
{
    using namespace boost::asio::ip;
    try {
        while (running_) {
            tcp::resolver resolver(ios_);
            tcp::resolver::query query(tcp::v4(), hostName_.c_str(), portStr_.c_str());
            boost::system::error_code error;
            tcp::resolver::iterator it = resolver.resolve(query, error);
            if (error || it == tcp::resolver::iterator()) {
            	throw std::runtime_error("TCPLink : resolve failed");
            } else {
            	impl_.reset(new ASIOImpl<TCPLink>(*this));
                boost::asio::async_connect(socket_, it,
                boost::bind(&TCPLink::connectComplete, this,
	            boost::asio::placeholders::error));
                ios_.run();
                ios_.reset();
            }
            boost::posix_time::time_duration d = boost::posix_time::milliseconds(TRY_CONNECT_TIMEOUT);
            boost::this_thread::sleep(d);
        }
    }

    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip client thread dying" << std::endl;
        std::cerr << ex.what();
    }

    catch(...) {
        std::cerr << "Exception caught, tcp/ip client thread dying" << std::endl;
    }
}

void TCPLink::handleError(const boost::system::error_code &error)
{
    connected_ = false;
    socket_.close();
    /// \todo Application::connectionChanged(false);
    impl_->reset();
}

void TCPLink::handleMessage(Message *message)
{
    if (message->id() == COMMUNICATOR_ID) {
    	///\todo
        message = NULL;
    } else {
        /// \todo Application::dispatch(message);
        message = NULL;
    }
    if (message != NULL) delete message;
}

void TCPLink::connectComplete(const boost::system::error_code &error)
{
	if (!error) {
    	std::cout << "connect complete" << std::endl;
		connected_ = true;
		impl_->startReceive();
	} else {
		// run loop will keep trying
	}
}

}
