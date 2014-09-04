
#ifndef MSG_H_
#define MSG_H_

#define PREAMBLE_SIZE		4
#define NODE_NAME_SIZE	8
#define DESC_SIZE				8

namespace wsan
{

class Msg
{
public:
	static bool isPreambleOk (const uint8_t* data)
	{
		static const char preamble[PREAMBLE_SIZE] = { 19, 81, 9, 3 };

		return memcmp(data, preamble, PREAMBLE_SIZE) == 0;
	}

	virtual char getType () const = 0;

	virtual const char* getNodeName () const = 0;

	virtual void setDesc (const char* desc) = 0;

	virtual const char* getDesc () const = 0;

	virtual const uint8_t* getData () const = 0;

	virtual uint8_t getDataSize () const = 0;

protected:
	typedef struct
	{
		char preamble[PREAMBLE_SIZE];
		char type;
		char node_name[NODE_NAME_SIZE];
	} __attribute__((packed)) Header;

	static void setDesc (char* payload_desc, const char* desc)
	{
		strncpy(payload_desc, desc, DESC_SIZE - 1);
		payload_desc[DESC_SIZE - 1] = 0;
	}

	Msg ()
	{
	}

	Msg (Header& header, const char type, const char* node_name)
	{
		static const char preamble[PREAMBLE_SIZE] = { 19, 81, 9, 3 };
		memcpy(header.preamble, preamble, PREAMBLE_SIZE);

		header.type = type;

		strncpy(header.node_name, node_name, NODE_NAME_SIZE - 1);
		header.node_name[NODE_NAME_SIZE - 1] = 0;
	}
};

}

#endif
