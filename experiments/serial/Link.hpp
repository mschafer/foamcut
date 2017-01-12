#ifndef foamcut_SerialLink_hpp
#define foamcut_SerialLink_hpp

#include <stdint.h>
#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>


namespace foamcut {

struct Link {

    enum ErrorCode {
        SUCCESS = 0,
        FATAL_ERROR = 1,
        RESOURCE_TEMPORARILY_UNAVAILABLE = 2
    };
    
    Link() {}
    ~Link() {}
    
    ErrorCode send(uint8_t b);
    ErrorCode receive(uint8_t &b);
    
};
    
struct LinkPair {

    
    
};
    

}


#endif
