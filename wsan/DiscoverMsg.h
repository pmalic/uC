
#ifndef DISCOVERMSG_H_
#define DISCOVERMSG_H_

#include <wsan/Msg.h>

namespace wsan
{

class DiscoverMsg : public Msg
{
public:
	static const char TYPE = 'D';

	typedef struct
	{
		char desc[Msg::DESC_SIZE];
	} __attribute__((packed)) Payload;

	typedef struct
	{
		Header header;
		Payload payload;
	} __attribute__((packed)) Frame;

	DiscoverMsg (const char* node, const char* sess = NULL, const char vnet = 0)
		: Msg(_frame.header, DiscoverMsg::TYPE, node, sess, vnet)
	{
		memset(&_frame.payload, 0, sizeof _frame.payload);
	}

	DiscoverMsg (const uint8_t* data, const uint8_t data_size)
	{
		if (data_size != sizeof _frame || !isPreambleOk(data) || data[Msg::PREAMBLE_SIZE] != DiscoverMsg::TYPE)
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

private:
	Frame _frame;
};

}

#endif // DISCOVERMSG_H_
