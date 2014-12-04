#include "AppPacket.hpp"

AppPacketFactory::AppPacketFactory()
{

}

AppPacketFactory::~AppPacketFactory()
{

}

size_t AppPacketFactory::operator()(const uint8_t *data, size_t size)
{
	return size;
}

AppPacket *AppPacketFactory::receive()
{
	if (!recvQueue_.empty()) {
		AppPacket *r = &recvQueue_.back();
		recvQueue_.pop_back();
		return r;
	}
	else {
		return nullptr;
	}
}