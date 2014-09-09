
#ifndef STRVALMSG_H_
#define STRVALMSG_H_

#include <wsan/Msg.h>

namespace wsan
{

class StrValMsg : public Msg
{
public:
	static const char TYPE = 'S';

	static const size_t VAL_SIZE = 	52;

	typedef struct
	{
		char val[VAL_SIZE];
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

	virtual char getType () const
	{
		return _frame.header.type;
	}

	virtual std::string getNodeName () const
	{
		return std::string(_frame.header.node_name, NODE_NAME_SIZE);
	}

	virtual void setDesc (const std::string& desc)
	{
		strncpy(_frame.payload.desc, desc.c_str(), DESC_SIZE);
	}

	virtual std::string getDesc () const
	{
		return std::string(_frame.payload.desc, DESC_SIZE);
	}

	virtual const uint8_t* getData () const
	{
		return reinterpret_cast<const uint8_t*>(&_frame);
	}

	virtual uint8_t getDataSize () const
	{
		return static_cast<uint8_t>(sizeof _frame);
	}

	void setValue (const std::string& value)
	{
		strncpy(_frame.payload.val, value.c_str(), VAL_SIZE);
	}

	const std::string getValue () const
	{
		return std::string(_frame.payload.val, VAL_SIZE);
	}

private:
	Frame _frame;
};

}

#endif // STRVALMSG_H_
