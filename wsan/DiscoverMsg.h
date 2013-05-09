
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
		Header header;
	} __attribute__((packed)) Frame;

	Frame frame;

	DiscoverMsg ()
	: Msg(frame.header, MSG_TYPE)
	{
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
