
#ifndef MSG_H_
#define MSG_H_

#include <string>

namespace wsan
{

class Msg
{
public:
	static const size_t PREAMBLE_SIZE =		2;
	static const size_t NODE_SIZE =				8;
	static const size_t SESS_SIZE =				6;
	static const size_t DESC_SIZE =				8;

	typedef struct
	{
		char preamble[Msg::PREAMBLE_SIZE];
		char type;
		char node[Msg::NODE_SIZE];
		char sess[Msg::SESS_SIZE];
		char vnet;
	} __attribute__((packed)) Header;

	static bool isPreambleOk (const uint8_t* data)
	{
		static const char preamble[Msg::PREAMBLE_SIZE] = { 81, 93 };

		return memcmp(data, preamble, Msg::PREAMBLE_SIZE) == 0;
	}

	virtual char getType () const = 0;

	virtual std::string getNode () const = 0;

	virtual void setDesc (const std::string&) = 0;

	virtual std::string getDesc () const = 0;

	virtual const uint8_t* getData () const = 0;

	virtual uint8_t getDataSize () const = 0;

protected:
	Msg ()
	{
	}

	Msg (Header& header, const char type, const char* node, const char* sess = NULL, const char vnet = 0)
	{
		memset(&header, 0, sizeof header);

		static const char preamble[Msg::PREAMBLE_SIZE] = { 81, 93 };
		memcpy(header.preamble, preamble, Msg::PREAMBLE_SIZE);

		header.type = type;

		strncpy(header.node, node, Msg::NODE_SIZE);

		if (sess)
			strncpy(header.sess, sess, Msg::SESS_SIZE);

		header.vnet = vnet;
	}
};

}

#endif // MSG_H_
