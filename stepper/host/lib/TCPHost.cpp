#include "TCPHost.hpp"

namespace stepper {

TCPHost::TCPHost(const char *hostName, uint16_t port) :
	hostName_(hostName), socket_(ios_), running_(true), connected_(false)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
    pool_.reset(new pool_type(messageBlock_, sizeof(messageBlock_)));
	impl_.reset(new device::ASIOImpl<TCPHost>(*this));
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
            	throw std::runtime_error("TCPHost : resolve failed");
            } else {
            	impl_.reset(new device::ASIOImpl<TCPHost>(*this));
                boost::asio::async_connect(socket_, it,
                boost::bind(&TCPHost::connectComplete, this,
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

void TCPHost::handler(device::MessageBuffer *msg, const boost::system::error_code &error)
{
	if (msg != nullptr) {
		rxQueue_.push(msg);
		impl_->receiveOne();
	}

	if (error) {
		connected_ = false;
		socket_.close();
		ios_.stop();
		///\todo indicate error
    	std::cout << "TCPHost error " << error << std::endl;
	}
}

void TCPHost::connectComplete(const boost::system::error_code &error)
{
	if (!error) {
    	std::cout << "connect complete" << std::endl;
		connected_ = true;
		impl_->receiveOne();
		impl_->startSending();
	} else {
		// run loop will keep trying
	}
}

}
