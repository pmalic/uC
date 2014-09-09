
#ifndef MSG_H_
#define MSG_H_

namespace wsan
{

class Msg
{
public:
	static const size_t PREAMBLE_SIZE =		4;
	static const size_t NODE_NAME_SIZE =	8;
	static const size_t DESC_SIZE =				8;

	static bool isPreambleOk (const uint8_t* data)
	{
		static const char preamble[PREAMBLE_SIZE] = { 19, 81, 9, 3 };

		return memcmp(data, preamble, PREAMBLE_SIZE) == 0;
	}

	virtual char getType () const = 0;

	virtual std::string getNodeName () const = 0;

	virtual void setDesc (const std::string&) = 0;

	virtual std::string getDesc () const = 0;

	virtual const uint8_t* getData () const = 0;

	virtual uint8_t getDataSize () const = 0;

protected:
	typedef struct
	{
		char preamble[PREAMBLE_SIZE];
		char type;
		char node_name[NODE_NAME_SIZE];
	} __attribute__((packed)) Header;

	Msg ()
	{
	}

	Msg (Header& header, const char type, const char* node_name)
	{
		static const char preamble[PREAMBLE_SIZE] = { 19, 81, 9, 3 };
		memcpy(header.preamble, preamble, PREAMBLE_SIZE);

		header.type = type;

		strncpy(header.node_name, node_name, NODE_NAME_SIZE);
	}
};

}

#endif // MSG_H_
