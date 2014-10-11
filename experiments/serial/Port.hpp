#ifndef Port_hpp
#define Port_hpp

#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>



class PortPair;

class Port : private boost::noncopyable
{
public:

	enum ErrorCode {
		PORT_SUCCESS = 0,
		PORT_ERROR
	};

	ErrorCode open();
	ErrorCode close();
	ErrorCode send(const uint8_t *buff, size_t &size);
    size_t sendAvailable() const;
	ErrorCode recv(uint8_t *buff, size_t &size);
	size_t recvAvailable() const;

private:
    friend class PortPair;
	Port(boost::circular_buffer<uint8_t> &in, boost::circular_buffer<uint8_t> &out, PortPair &pp) :
        in_(in), out_(out), pp_(pp)
    {
    }

	boost::circular_buffer<uint8_t> &in_;
	boost::circular_buffer<uint8_t> &out_;
	class PortPair &pp_;

};

class PortPair : private boost::noncopyable
{
public:
	typedef boost::shared_ptr<PortPair> Handle;

    enum {
        DEFAULT_BUFFER_SIZE = 256
    };

    PortPair() : cbA_(DEFAULT_BUFFER_SIZE), cbB_(DEFAULT_BUFFER_SIZE),
        portA_(cbA_, cbB_, *this), portB_(cbB_, cbA_, *this)
    {
    }

    explicit PortPair(size_t s) : cbA_(s), cbB_(s),
        portA_(cbA_, cbB_, *this), portB_(cbB_, cbA_, *this)
    {
    }


    Port &portA() { return portA_; }
    Port &portB() { return portB_; }

private:
    friend class Port;

	boost::circular_buffer<uint8_t> cbA_;
	boost::circular_buffer<uint8_t> cbB_;
	Port portA_;
	Port portB_;
	boost::recursive_mutex lock_;

	boost::recursive_mutex &mutex() { return lock_; }
};

#endif
