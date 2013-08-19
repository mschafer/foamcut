#ifndef stepper_Pin_hpp
#define stepper_Pin_hpp

#include <assert.h>

namespace stepper {

/**
 * This class encapsulates the functionality of a single digital i/o pin.
 */
struct Pin
{
public:
	Pin() : val_(UNASSIGNED) {}

	enum Function {
		OUTPUT,
		INPUT_PULLUP,
		INPUT_PULLDOWN
	};

	enum {
		UNASSIGNED = 0xFFFF
	};

    /**
     * An unassigned may be used for functions that are not implemented like
     * limit switches that are not connected.
     * The results of all other operations on an unassigned pin are undefined.
     */
	bool unassigned() const { return val_ = UNASSIGNED; }

	/**
	 * If the pin is inverted, then Platform::setPin(true) will result in a logic
	 * low value.  Platform::getPin() will also return true if level is low.
	 */
	bool inverted() const {
		assert(val_ != UNASSIGNED);
		return bits_.invert_ == 1;
	}
	void inverted(bool v) { bits_.invert_ = v ? 1 : 0; }

	uint8_t port() const {
		assert(val_ != UNASSIGNED);
		return bits_.port_;
	}
	void port(uint8_t v) { bits_.port_ = v;}

	uint8_t pin() const {
		assert(val_ != UNASSIGNED);
		return bits_.pin_;
	}
	void pin(uint8_t v) {bits_.pin_ = v; }

private:
	union {
		struct {
			uint16_t invert_ : 1;
			uint16_t port_ : 7;
			uint16_t pin_ : 8;
		} bits_;
		uint16_t val_;
	};
};

}

#endif
