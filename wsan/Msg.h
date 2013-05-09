
#ifndef MSG_H_
#define MSG_H_

namespace wsan
{

class Msg
{
protected:
	typedef struct
	{
		uint8_t preamble[4];
		uint8_t msg_type;
	} __attribute__((packed)) Header;

	Msg (Header& header, uint8_t msg_type)
	{
		static const uint8_t preamble[4] = { 19, 81, 9, 3 };

		memcpy(header.preamble, preamble, 4);
		
		header.msg_type = msg_type;
	}

public:
	static bool isPreambleOk (const uint8_t* data)
	{
		static const uint8_t preamble[4] = { 19, 81, 9, 3 };

		return memcmp(data, preamble, 4) == 0;
	}

	virtual uint8_t* getData () = 0;

	virtual uint8_t getDataSize () = 0;
};

}

#endif
