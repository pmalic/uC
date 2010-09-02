
#ifndef MSG_H_
#define MSG_H_

namespace wsan
{

class Msg
{
public:
	static const uint8_t PREAMBLE = 81;

	typedef struct
	{
		uint8_t preamble;
	} __attribute__((packed)) Header;

	virtual uint8_t* getFrame () = 0;

	virtual uint8_t getFrameLen () = 0;

};

}

#endif
