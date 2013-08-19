#ifndef foamcut_device_RingBuffer_hpp
#define foamcut_device_RingBuffer_hpp

#include <stdint.h>

namespace foamcut { namespace device {

template <typename T, uint16_t N>
class RingBuffer
{
public:
	RingBuffer() : head_(0), tail_(0) {
	}

	bool empty() const {
		return (head_ == tail_);
	}

	bool full() const {
		return ( head_+1 == tail_ || 
				(tail_ == 0 && head_ == N-1));
	}

	/// \return The number of elements in the RingBuffer.
	uint16_t count() const {
		int32_t d = head_ - tail_;
		d = d<0 ? d+N : d;
	}
	
	/// \return space available in RingBuffer == number of pushes that will succeed.
	uint16_t space() const {
	}
	
	bool push(const T &val) {
		if (full()) return false;
		buff_[head_] = val;
		increment(head_);
		return true;
	}
	
	///\todo optimize with contiguous memcpy's
	bool push(const T *vals, uint16_t count) {
		if (space() < count) return false;
		uint16_t i = 0;
		for (uint16_t i=0; i<count; ++i) {
			push(vals[i]);
		}
	}
	
	bool pop(T &out) {
		if (empty()) return false;
		out = buff_[tail_];
		increment(tail_);
		return true;
	}

	///\todo optimize with contiguous memcpy's
	bool pop(T *vals, uint16_t count) {
		if (count() < count) return false;
		uint16_t i = 0;
		for (uint16_t i=0; i<count; ++i) {
			pop(vals[i]);
		}
	}
	
private:
	void increment(uint16_t &idx) {
		uint16_t i = idx+1;
		idx = i == N ? 0 : i;
	}

	T buff_[N];
	uint16_t head_;  ///\< push new items here
	uint16_t tail_;  ///\< pop from here
};

}}

#endif
