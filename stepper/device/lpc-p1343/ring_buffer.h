#ifndef ring_buffer_h
#define ring_buffer_h

#include <stdint.h>
#include <stdbool.h>

/// max of rb_size_t needs to be at least 2x size to prevent overflow
typedef uint16_t rb_size_t;

/** 
 * A ring buffer.
 */
typedef struct {
    /// Offset of head
    rb_size_t head;

    /// Offset of tail
    rb_size_t tail;

    /// Size of data
    rb_size_t size;

    /// A pointer to the ring buffer memory.
    uint8_t *data;
} RingBuffer;

/// Construct a new ring buffer.
inline void rb_construct(RingBuffer *rb, rb_size_t size, uint8_t *data)
{
	rb->head = rb->tail = 0;
	rb->size = size;
	rb->data = data;
}

#define RB_INCREMENT(p)            \
	if ((p)+1 == rb->size) (p) = 0; \
    else (p) = (p)+1;

/// Remove all contents of ring buffer.
inline void rb_clear(RingBuffer *rb)
{
	rb->head = rb->tail = 0;
}

inline bool rb_empty(const RingBuffer *rb)
{
	return(rb->head == rb->tail);
}

inline bool rb_full(const RingBuffer *rb)
{
	return ((rb->tail == rb->head + 1) ||
			(rb->tail == 0 && rb->head == rb->size-1));
}

inline uint8_t rb_peek(const RingBuffer *rb)
{
	return rb->data[rb->tail];
}

inline uint8_t rb_read(RingBuffer *rb)
{
	uint8_t ret = rb->data[rb->tail];
	RB_INCREMENT(rb->tail);
	return ret;
}

inline void rb_write(RingBuffer *rb, uint8_t val)
{
	rb->data[rb->head] = val;
	RB_INCREMENT(rb->head);
}

inline rb_size_t free(const RingBuffer *rb)
{
	rb_size_t count = rb->size + rb->tail - rb->head - 1;
	count = (count < rb->size) ? count : count - rb->size;
	return count;
}

inline rb_size_t available(const RingBuffer *rb)
{
	rb_size_t count = rb->size + rb->head - rb->tail;
	count = (count < rb->size) ? count : count - rb->size;
	return count;
}

#endif
