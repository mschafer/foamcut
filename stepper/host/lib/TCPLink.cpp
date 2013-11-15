#include "TCPLink.hpp"

namespace stepper {

TCPLink::TCPLink(const char *hostName, uint16_t port) :
	hostName_(hostName), socket_(ios_), running_(true), connected_(false)
{
    std::ostringstream oss;
    oss << port;
    portStr_ = oss.str();
    pool_.reset(new pool_type(messageBlock_, sizeof(messageBlock_)));
	impl_.reset(new device::ASIOImpl<TCPLink>(*this));
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

void TCPLink::send(device::MessageBuffer *mb)
{
	txQueue_.push(mb);
    ios_.post(boost::bind(&device::ASIOImpl<TCPLink>::startSending, impl_.get()));
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
            	impl_.reset(new device::ASIOImpl<TCPLink>(*this));
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

void TCPLink::handler(device::MessageBuffer *msg, const boost::system::error_code &error)
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
    	std::cout << "TCPLink error " << error.message() << std::endl;
	}
}

void TCPLink::connectComplete(const boost::system::error_code &error)
{
	if (!error) {
    	std::cout << "connect complete" << std::endl;
		connected_ = true;
		impl_->receiveOne();
        ios_.post(boost::bind(&device::ASIOImpl<TCPLink>::startSending, impl_.get()));
	} else {
		// run loop will keep trying
	}
}

}
