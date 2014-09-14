
#ifndef BOOLMSG_H_
#define BOOLMSG_H_

#include <wsan/Msg.h>

namespace wsan
{

class BoolValMsg : public Msg
{
public:
	static const char TYPE = 'B';

	typedef struct
	{
		uint8_t val;
		char desc[Msg::DESC_SIZE];
	} __attribute__((packed)) Payload;

	typedef struct
	{
		Header header;
		Payload payload;
	} __attribute__((packed)) Frame;

	BoolValMsg (const char* node, const char* sess = NULL, const char vnet = 0)
		: Msg(_frame.header, BoolValMsg::TYPE, node, sess, vnet)
	{
		memset(&_frame.payload, 0, sizeof _frame.payload);
	}

	BoolValMsg (const uint8_t* data, const uint8_t data_size)
	{
		if (data_size != sizeof _frame || !isPreambleOk(data) || data[Msg::PREAMBLE_SIZE] != BoolValMsg::TYPE)
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

	virtual std::string getNode () const
	{
		const char* node = _frame.header.node;
		return node[Msg::NODE_SIZE - 1] ? std::string(node, Msg::NODE_SIZE) : std::string(node);
	}

	virtual void setDesc (const std::string& desc)
	{
		strncpy(_frame.payload.desc, desc.c_str(), Msg::DESC_SIZE);
	}

	virtual std::string getDesc () const
	{
		const char* desc = _frame.payload.desc;
		return desc[Msg::DESC_SIZE - 1] ? std::string(desc, Msg::DESC_SIZE) : std::string(desc);
	}

	virtual const uint8_t* getData () const
	{
		return reinterpret_cast<const uint8_t*>(&_frame);
	}

	virtual uint8_t getDataSize () const
	{
		return static_cast<uint8_t>(sizeof _frame);
	}

	void setValue (const bool value)
	{
		_frame.payload.val = value ? 1 : 0;
	}

	bool getValue () const
	{
		return _frame.payload.val == 1;
	}

private:
	Frame _frame;
};

}

#endif // BOOLMSG_H_
