#ifndef stepper_Pool_hpp
#ifndef stepper_Pool_hpp

namespace foamcut { namespace device {

/**
 * Fixed size, statically configured pool allocator.
 * \tparam POOL_SIZE The total size of the memory block assigned to the pool.
 */
template<size_t POOL_SIZE>
class Pool
{
public:
	/** \param blockSize The size of a memory block returned by alloc.
	  *  The actual size will be adjusted upward to a multiple of sizeof(void*).
	  */
    Pool(size_t blockSize) {
    	size_t actualSizeWords = (blockSize + sizeof(void*) - 1) / sizeof(void*);
        size_t blockCount = POOL_SIZE / (actualSizeWords * sizeof(void*));
        Node *nodes = reinterpret_cast<Node*>(memory_);
        for (size_t i=0; i<blockCount-1; i++) {
        	nodes[i]->next_ = &nodes[i+1];
        }
		nodes[blockCount-1]->next_ = NULL;
		
        blockPtr[i] = NULL;
        head_ = &nodes[0];
        tail_ = &nodes[blockCount-1];
    }
    
    void *alloc() {
    	
    }
    
    void free(void *block) {
    }
    
    ~Pool() {}
    
private:
    struct Node {
        Node *next_;
    };

    uint8_t memory_[POOL_SIZE];
    size_t blockSize_;
    Node *head_;
    Node *tail_;
};

}

#endif
