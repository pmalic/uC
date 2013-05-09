
#ifndef OFFERMSG_H_
#define OFFERMSG_H_

#include "wsan/Msg.h"

namespace wsan
{

class OfferMsg : public Msg
{
public:
	static const uint8_t MSG_TYPE = 1;

	typedef struct
	{
		char node_name[16];
		uint8_t desc;
		uint32_t val;
	} __attribute__((packed)) Payload;

	typedef struct
	{
		Header header;
		Payload payload;
	} __attribute__((packed)) Frame;

	Frame frame;

	OfferMsg (const char* node_name)
	: Msg(frame.header, MSG_TYPE)
	{
		strncpy(frame.payload.node_name, node_name, 16);
		frame.payload.node_name[15] = 0;
	}

	OfferMsg (const uint8_t* data, const uint8_t data_size)
	: Msg(frame.header, MSG_TYPE)
	{
		if (data_size != sizeof frame || !isPreambleOk(data) || data[4] != MSG_TYPE)
			return;

		const size_t header_size = sizeof frame.header;
		const size_t payload_size = sizeof frame.payload;

		if (data_size - header_size == payload_size)
			memcpy(&frame.payload, data + header_size, payload_size);
	}

	uint8_t* getData ()
	{
		return reinterpret_cast<uint8_t*>(&frame);
	}

	uint8_t getDataSize ()
	{
		return static_cast<uint8_t>(sizeof frame);
	}

};

}

#endif
