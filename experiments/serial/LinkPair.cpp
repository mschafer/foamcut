#include "LinkPair.hpp"


Link::ErrorCode Link::open()
{
	boost::lock_guard<boost::recursive_mutex> guard(pp_.mutex());
	in_.clear();
	out_.clear();
	return LINK_SUCCESS;
}

Link::ErrorCode Link::close()
{
	boost::lock_guard<boost::recursive_mutex> guard(pp_.mutex());
	return LINK_SUCCESS;
}


Link::ErrorCode Link::send(const uint8_t byte)
{
	boost::lock_guard<boost::recursive_mutex> guard(pp_.mutex());
    if (out_.full()) {
        return LINK_WOULD_BLOCK;
    } else {
        out_.push_back(byte);
        return LINK_SUCCESS;
    }
}

Link::ErrorCode Link::recv(uint8_t *byte)
{
	boost::lock_guard<boost::recursive_mutex> guard(pp_.mutex());
    if (in_.empty()) {
        return LINK_WOULD_BLOCK;
    } else {
        *byte = in_.front();
        return LINK_SUCCESS;
    }
}
