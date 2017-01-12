#ifndef fifo_hpp__
#define fifo_hpp__

#include <stddef.h>
#include <stdint.h>

/**
 * Circular buffer.
 * Additional access methods to access contiguous contents or free space as a
 * C style array.
 *
 * Should be safe for single producer, single consumer on a
 * single core processor w/ strict ordering.
 */
template<typename T, size_t N>
class fifo
{
public:

    typedef T value_type;

    struct carray {
        T *buf_;
        size_t len_;
    };

    fifo() : contents_(buff_), space_(buff_) {}

    bool empty() const { return space_ == contents_; }
    bool full() const { return contents_size() == N - 1; }
    void clear() { contents_ = buff_; space_ = buff_; }

    T front() const { return *contents_; }
    void pop() { contents_remove(1); }
    void push(const T &val) { 
        *space_ = val;
        contents_add(1);
    }

    T &operator[](ptrdiff_t idx) { return contents_at(idx); }

    size_t contents_size() const {
        return contents_wrapped() ?  N + space_ - contents_ : space_ - contents_;
    }

    T &contents_at(size_t idx) {
        T *p = increment(const_cast<T*>(contents_), idx);
        return *p;
    }

    carray contents_carray() {
        carray r;
        r.buf_ = const_cast<T*>(contents_);
        r.len_ = contents_wrapped() ? buff_end() - contents_ : space_ - contents_;
        return r;
    }

    /** converts count items at the front of contents to space. */
    void contents_remove(size_t count) {
        contents_ = increment(const_cast<T*>(contents_), count);
    }

    /** adds count items at the front of space to the back of contents. */
    void contents_add(size_t count) {
        space_ = increment(const_cast<T*>(space_), count);
    }

    size_t space_size() const { return N - contents_size() - 1; }

    T &space_at(size_t idx) {
        T *p = increment(const_cast<T*>(space_), idx);
        return *p;
    }

    carray space_carray() {
        carray r;
        r.buf_ = const_cast<T*>(space_);
        if (contents_wrapped()) {
            r.len_ = contents_ - space_ - 1;
        }
        else {
            r.len_ = buff_end() - space_;

            // if contents_ == buff_, then allowing the space array to go all the way to the end
            // would mean that contents_add(r.size_) would make empty() == true;
            if (contents_ == buff_) --r.len_;
        }
        return r;
    }

private:
    volatile T *contents_;
    volatile T *space_;
    T buff_[N];

    T *increment(T *p, size_t i=1) {
        p += i;
        p = p >= buff_end() ? p - N : p;
        return p;
    }

    T *buff_end() { return buff_ + N; }
    bool contents_wrapped() const { return space_ < contents_; }

};

#endif
