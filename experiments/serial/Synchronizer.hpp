#ifndef Synchronizer_hpp
#define Synchronizer_hpp

class Synchronizer
{
public:
    typedef std::shared_ptr<Synchronizer> handle;

    explicit Synchronizer(const char *pattern, size_t patternSize) :
    pattern_(pattern), patternSize_(patternSize), pos_(0)
    {
    }

	bool run(uint8_t c) {
		if (pattern_[pos_] == c) {
			++pos_;
		} else {
			pos_ = 0;
		}

		return (pos_ == patternSize_);
	}

    bool run(const uint8_t *in, size_t &inSize) {
        size_t i = 0;
    	while (inSize > 0 && pos_ < patternSize_) {
            if (in[i] == pattern_[pos_]) {
                ++pos_;
                ++i;
            } else {
                pos_ = 0;
            }
            --inSize;
        }
        if (pos_ == patternSize_) {
            return true;
        } else {
            return false;
        }
    }

    size_t position() const { return pos_; }
    size_t remaining() const { return patternSize_ - pos_; }
	void reset() { pos_ = 0; }

private:
    const char *pattern_;
    size_t patternSize_;
    size_t pos_;

    Synchronizer();
    Synchronizer(const Synchronizer &cpy);
    Synchronizer &operator=(const Synchronizer &rhs);

};

#endif
