class Synchronizer
{
public:
    typedef std::shared_ptr<Synchronizer> handle;

    explicit Synchronizer(const char *pattern, size_t patternSize) :
    pattern_(pattern), patternSize_(patternSize), pos_(0)
    {
    }

    size_t run(const uint8_t *in, size_t inSize) {
        size_t i;
        for (i=0; i<patternSize_ - pos_; ++i) {
            if (in[i] == pattern_[pos_]) {
                ++pos_;
            } else {
                return 0;
            }
        }
        if (pos_ == patternSize_) {
            return i;
        } else {
            return 0;
        }
    }

    size_t position() const { return pos_; }
    size_t remaining() const { return patternSize_ - pos_; }

private:
    const char *pattern_;
    size_t patternSize_;
    size_t pos_;

    Synchronizer();
    Synchronizer(const Synchronizer &cpy);
    Synchronizer &operator=(const Synchronizer &rhs);

};
