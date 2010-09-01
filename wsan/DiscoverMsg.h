
#ifndef DISCOVERMSG_H_
#define DISCOVERMSG_H_

#include "wsan/Msg.h"

namespace wsan
{

class DiscoverMsg : public Msg
{
public:
	static const uint8_t MSG_TYPE = 0;

	typedef struct
	{
		Msg::Header common;
		uint8_t msg_type;
	} Header;

	Header header;

	DiscoverMsg ()
	{
		header.common.preamble = Msg::PREAMBLE;
		header.msg_type = MSG_TYPE;
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
