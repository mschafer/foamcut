#ifndef foamcut_Packet_hpp
#define foamcut_Packet_hpp

namespace foamcut {
    
    struct Packet {
    public:
        uint16_t length_;
        uint8_t counter_;
        uint8_t id_;
    };

    static_assert (sizeof(Packet) == 4, "Packet is not 4 bytes");

    /** Request from client to Server. */
    struct Request : public Packet {
        
    };
    
    /** Client Response to Request. */
    struct Response : public Packet {
        
    };
    
    
    struct Ping : public Request {
        enum {
            REQUEST_ID = 0
        };
        
    };
    
    struct PingResponse : public Response {
        enum {
            REQUEST_ID = Ping::REQUEST_ID
        };
    };
    

}


#endif
