#include <boost/random.hpp>
#include <boost/format.hpp>

#include <xbee/XBee.h>
#include <wsan/DiscoverMsg.h>
#include <wsan/NumValMsg.h>
#include <wsan/BoolValMsg.h>
#include <wsan/StrValMsg.h>

using namespace std;
using namespace wsan;

int genRandomInt (const uint32_t seed)
{
	boost::variate_generator<boost::mt19937, boost::uniform_int<> > rng(boost::mt19937(seed), boost::uniform_int<>(-10000, 10000));

	return rng();
}

bool genRandomBool (const uint32_t seed)
{
	return genRandomInt(seed) > 0;
}

void reply (XBee &xbee, const DiscoverMsg& msg, XBeeAddress64& addr)
{
	cerr << "Replying to " << msg.getNodeName() << " ("<< hex << addr.getMsb() << ':' << addr.getLsb() << ")..." << endl;


	NumValMsg msg1("PM2");
	const double d = genRandomInt(time(0)) / 100.0;
	msg1.setValue(d, 2);
	msg1.setDesc("DBL");
	cerr << "\tDBL: " << boost::format("%.2f") % d << endl;

	ZBTxRequest tx1 = ZBTxRequest(addr, const_cast<uint8_t*>(msg1.getData()), msg1.getDataSize());
	xbee.send(tx1);


	BoolValMsg msg2("PM2");
	const bool b = genRandomBool(time(0));
	msg2.setValue(b);
	msg2.setDesc("BOOL");
	cerr << "\tBOOL: " << (b ? "TRUE" : "FALSE") << endl;

	ZBTxRequest tx2 = ZBTxRequest(addr, const_cast<uint8_t*>(msg2.getData()), msg2.getDataSize());
	xbee.send(tx2);


	StrValMsg msg3("PM2");
	const char* s = "Hello World!";
	msg3.setValue(s);
	msg3.setDesc("STR");
	cerr << "\tSTR: \"" << s << '"' << endl;

	ZBTxRequest tx3 = ZBTxRequest(addr, const_cast<uint8_t*>(msg3.getData()), msg3.getDataSize());
	xbee.send(tx3);
}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	SERIAL serial = SERIAL(argv[1]);
	serial.begin(115200);

	XBee xbee = XBee();
	xbee.setSerial(serial);

	ZBRxResponse rx = ZBRxResponse();

	while (true)
	{
		xbee.readPacket(5000);

		if (!xbee.getResponse().isAvailable() || xbee.getResponse().getApiId() != ZB_RX_RESPONSE)
			continue;

		xbee.getResponse().getZBRxResponse(rx);

		const uint8_t data_size = static_cast<unsigned short>(rx.getDataLength());

		if (data_size < 5)
			continue;

		uint8_t* data = rx.getData();

		DiscoverMsg msg(data, data_size);

		if (msg.getType() != DiscoverMsg::TYPE)
			continue;

		usleep(100);

		reply(xbee, msg, rx.getRemoteAddress64());
	}

	return 0;
}
