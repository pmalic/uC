
#ifndef NUMVALMSG_H_
#define NUMVALMSG_H_

#include <wsan/Msg.h>

namespace wsan
{

class NumValMsg : public Msg
{
public:
	static const char TYPE = 'N';

	typedef struct
	{
		uint8_t spec;
		uint8_t val1;
		uint8_t val2;
		uint8_t val3;
		uint8_t val4;
		uint8_t val5;
		uint8_t val6;
		uint8_t val7;
		uint8_t val8;
		char desc[DESC_SIZE];
	} __attribute__((packed)) Payload;

	typedef struct
	{
		Header header;
		Payload payload;
	} __attribute__((packed)) Frame;

	NumValMsg (const char* node_name)
		: Msg(_frame.header, TYPE, node_name)
	{
		memset(&_frame.payload, 0, sizeof _frame.payload);
	}

	NumValMsg (const uint8_t* data, const uint8_t data_size)
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

	void setValue (const double value, uint8_t decimals)
	{
		_frame.payload.spec = decimals << 1;

		if (value < 0)
			_frame.payload.spec |= 1;

		unsigned int factor = 1;

		while (decimals--)
			factor *= 10;

		packValue(static_cast<uint64_t>(value * (value < 0 ? -1 : 1) * factor));
	}

	void setValue (const int64_t value)
	{
		_frame.payload.spec = value < 0 ? 1 : 0;

		packValue(static_cast<uint64_t>(value));
	}

	void setValue (const int32_t value)
	{
		_frame.payload.spec = value < 0 ? 1 : 0;

		packValue(static_cast<uint64_t>(value));
	}

	double getValue () const
	{
		uint8_t decimals = _frame.payload.spec >> 1;
		double factor = 1.0;

		while (decimals--)
			factor *= 10.0;

		return unpackValue() / factor * (_frame.payload.spec & 1 ? -1 : 1);
	}

	int64_t getInt64 () const
	{
		int64_t value = static_cast<int64_t>(unpackValue());

		if (_frame.payload.spec & 1)
			value *= -1;

		return value;
	}

	int32_t getInt32 () const
	{
		int32_t value = static_cast<int32_t>(unpackValue());

		if (_frame.payload.spec & 1)
			value *= -1;

		return value;
	}

private:
	void packValue (const uint64_t value)
	{
		_frame.payload.val1 = value;
		_frame.payload.val2 = value >> 8;
		_frame.payload.val3 = value >> 16;
		_frame.payload.val4 = value >> 24;
		_frame.payload.val5 = value >> 32;
		_frame.payload.val6 = value >> 40;
		_frame.payload.val7 = value >> 48;
		_frame.payload.val8 = value >> 56;
	}

	uint64_t unpackValue () const
	{
		uint64_t value;

		value = _frame.payload.val8;
		value <<= 8;

		value |= _frame.payload.val7;
		value <<= 8;

		value |= _frame.payload.val6;
		value <<= 8;

		value |= _frame.payload.val5;
		value <<= 8;

		value |= _frame.payload.val4;
		value <<= 8;

		value |= _frame.payload.val3;
		value <<= 8;

		value |= _frame.payload.val2;
		value <<= 8;

		value |= _frame.payload.val1;

		return value;
	}

	Frame _frame;
};

}

#endif // NUMVALMSG_H_
