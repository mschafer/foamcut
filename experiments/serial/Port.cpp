#include "Port.hpp"


Port::ErrorCode Port::open()
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	in_.clear();
	out_.clear();
	return PORT_SUCCESS;
}

Port::ErrorCode Port::close()
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	return PORT_SUCCESS;
}


Port::ErrorCode Port::send(const uint8_t *buff, size_t &size)
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	size_t n = std::min(sendAvailable(), size);
	for (size_t i=0; i<n; ++i) {
		out_.push_back(buff[i]);
	}
	size = n;
	return PORT_SUCCESS;
}

size_t Port::sendAvailable() const
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	return out_.capacity() - out_.size();
}

Port::ErrorCode Port::recv(uint8_t *buff, size_t &size)
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	size_t n = std::min(recvAvailable(), size);
	for (size_t i=0; i<n; ++i) {
		buff[i] = in_.front();
		in_.pop_front();
	}
	std::copy(in_.begin(), in_.begin()+n, buff);
	size = n;
	return PORT_SUCCESS;
}

size_t Port::recvAvailable() const
{
	boost::lock_guard<boost::mutex> guard(pp_.mutex());
	return in_.size();
}
