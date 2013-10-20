#include "TCPHost.hpp"

namespace stepper {

TCPHost::TCPHost(const char *hostName, uint16_t port) :
	hostName_(hostName), socket_(ios_), running_(true)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
    thread_.reset(new boost::thread(boost::bind(&TCPHost::run, this)));
}

TCPHost::~TCPHost()
{
	try {
		running_ = false;
		ios_.stop();
		thread_->join();
	} catch (...) {
		///\todo ?
	}
}

void TCPHost::send(device::MessageBuffer *mb)
{
	txQueue_.push(mb);
	impl_->startSending();
}

device::MessageBuffer *TCPHost::receive()
{
	return rxQueue_.pop();
}

void TCPHost::run()
{
    using namespace boost::asio::ip;
    try {
        while (running_) {
            tcp::resolver resolver(ios_);
            tcp::resolver::query query(hostName_.c_str(), portStr_.c_str());
            boost::system::error_code error;
            tcp::resolver::iterator it = resolver.resolve(query, error);
            if (error || it == tcp::resolver::iterator()) {
                boost::posix_time::time_duration d = boost::posix_time::milliseconds(TRY_CONNECT_TIMEOUT);
                boost::this_thread::sleep(d);
            } else {
            	impl_.reset(new device::ASIOImpl<TCPHost>(*this));
                boost::asio::async_connect(socket_, it,
                boost::bind(&TCPHost::connectComplete, this,
	            boost::asio::placeholders::error));
                ios_.run();
                ios_.reset();
            }
            ///\todo error happened here
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

void TCPHost::handler(device::MessageBuffer *msg, const boost::system::error_code &error)
{
	if (msg != nullptr) {
		rxQueue_.push(msg);
		impl_->receiveOne();
	}

	if (error) {
		///\todo handle error
	}
}

void TCPHost::connectComplete(const boost::system::error_code &error)
{
	if (!error) {
		impl_->receiveOne();
		impl_->startSending();
	} else {
		///\todo handle error
	}
}

}
