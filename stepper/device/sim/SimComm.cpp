#include "SimComm.hpp"

namespace stepper { namespace device {

SimComm::SimComm(uint16_t port) : socket_(ios_), port_(port), running_(true)
{
	using boost::asio::ip::tcp;
    if (port_ != 0) {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint(tcp::v4(), port_)));
    } else {
        acceptor_.reset(new tcp::acceptor(ios_, tcp::endpoint()));
        port_ = acceptor_->local_endpoint().port();
    }

	impl_.reset(new ASIOImpl<SimComm>(*this));
	thread_.reset(new boost::thread(boost::bind(&SimComm::run, this)));
}

void SimComm::run()
{
    using namespace boost::asio::ip;
    try {
        while (running_) {
            acceptor_->async_accept(socket_,
		    boost::bind(&SimComm::acceptComplete, this,
		        boost::asio::placeholders::error));
            ios_.run();
            ios_.reset();
        }
    }

    catch(std::exception &ex) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
        std::cerr << ex.what();
    }

    catch(...) {
        std::cerr << "Exception caught, tcp/ip server thread dying" << std::endl;
    }

}

void SimComm::initialize()
{
}

void SimComm::operator()()
{
}

bool SimComm::connected() const
{
	return false;
}

void SimComm::startSending()
{
    ios_.post(boost::bind(&ASIOImpl<SimComm>::startSending, impl_.get()));
}

void SimComm::acceptComplete(const boost::system::error_code &error)
	{
    if (!error) {
    	std::cout << "accept complete" << std::endl;
    	impl_->receiveOne();
    	impl_->startSending();
    } else {
    	///\todo handle error
    }
}


}}
