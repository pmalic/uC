
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
		Msg::Header common;
		uint8_t msg_type;
		char node_name[16];
		uint8_t desc;
		uint32_t val;
	} __attribute__((packed)) Header;

	Header header;

	OfferMsg (const char* node_name)
	{
		header.common.preamble = Msg::PREAMBLE;
		header.msg_type = MSG_TYPE;

		strncpy(header.node_name, node_name, 16);
		header.node_name[15] = 0;
	}

	OfferMsg (const uint8_t* frame)
	{
		memcpy(&header, frame, sizeof header);
	}

	uint8_t* getFrame ()
	{
		return reinterpret_cast<uint8_t*>(&header);
	}

	uint8_t getFrameLen ()
	{
		return static_cast<uint8_t>(sizeof header);
	}
};

}

#endif
