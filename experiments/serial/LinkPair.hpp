#ifndef LinkPair_hpp
#define LinkPair_hpp

#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>



class LinkPair;

class Link : private boost::noncopyable
{
public:

	enum ErrorCode {
		LINK_SUCCESS = 0,
		LINK_ERROR,
        LINK_WOULD_BLOCK
	};

    ErrorCode open();
	ErrorCode close();
	ErrorCode send(const uint8_t byte);
	ErrorCode recv(uint8_t *byte);

private:
    friend class LinkPair;
	Link(boost::circular_buffer<uint8_t> &in, boost::circular_buffer<uint8_t> &out, LinkPair &pp) :
        in_(in), out_(out), pp_(pp)
    {
    }

	boost::circular_buffer<uint8_t> &in_;
	boost::circular_buffer<uint8_t> &out_;
	class LinkPair &pp_;

};

class LinkPair : private boost::noncopyable
{
public:
	typedef boost::shared_ptr<LinkPair> Handle;

    enum {
        DEFAULT_BUFFER_SIZE = 256
    };

    LinkPair() : cbA_(DEFAULT_BUFFER_SIZE), cbB_(DEFAULT_BUFFER_SIZE),
        portA_(cbA_, cbB_, *this), portB_(cbB_, cbA_, *this)
    {
    }

    explicit LinkPair(size_t s) : cbA_(s), cbB_(s),
        portA_(cbA_, cbB_, *this), portB_(cbB_, cbA_, *this)
    {
    }


    Link &portA() { return portA_; }
    Link &portB() { return portB_; }

private:
    friend class Link;

	boost::circular_buffer<uint8_t> cbA_;
	boost::circular_buffer<uint8_t> cbB_;
	Link portA_;
	Link portB_;
	boost::recursive_mutex lock_;

	boost::recursive_mutex &mutex() { return lock_; }
};

#endif
