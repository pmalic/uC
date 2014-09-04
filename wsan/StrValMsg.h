
#ifndef STRVALMSG_H_
#define STRVALMSG_H_

#include <wsan/Msg.h>

#define STR_VAL_SIZE		52

namespace wsan
{

class StrValMsg : public Msg
{
public:
	static const char TYPE = 'S';

	typedef struct
	{
		char val[STR_VAL_SIZE];
		char desc[DESC_SIZE];
	} __attribute__((packed)) Payload;

	typedef struct
	{
		Header header;
		Payload payload;
	} __attribute__((packed)) Frame;

	StrValMsg (const char* node_name)
		: Msg(_frame.header, TYPE, node_name)
	{
		memset(&_frame.payload, 0, sizeof _frame.payload);
	}

	StrValMsg (const uint8_t* data, const uint8_t data_size)
	{
		if (data_size != sizeof _frame || !isPreambleOk(data) || data[PREAMBLE_SIZE] != TYPE)
		{
			memset(&_frame, 0, sizeof _frame);
			return;
		}

		memcpy(&_frame, data, data_size);
	}

	char getType () const
	{
		return _frame.header.type;
	}

	const char* getNodeName () const
	{
		return _frame.header.node_name;
	}

	void setDesc (const char* desc)
	{
		Msg::setDesc(_frame.payload.desc, desc);
	}

	const char* getDesc () const
	{
		return _frame.payload.desc;
	}

	const uint8_t* getData () const
	{
		return reinterpret_cast<const uint8_t*>(&_frame);
	}

	uint8_t getDataSize () const
	{
		return static_cast<uint8_t>(sizeof _frame);
	}

	void setValue (const char* value)
	{
		strncpy(_frame.payload.val, value, STR_VAL_SIZE - 1);
		_frame.payload.val[STR_VAL_SIZE - 1] = 0;
	}

	const char* getValue () const
	{
		return _frame.payload.val;
	}

private:
	Frame _frame;
};

}

#endif
