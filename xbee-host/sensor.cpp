#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <boost/random.hpp>
#include "xbee/XBee.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/NumValMsg.h"

using namespace std;
using namespace wsan;

void genRandom (OfferMsg& msg)
{
	boost::mt19937 seed(time(0));
	boost::uniform_int<> bounds(0, 10000);
	boost::variate_generator<boost::mt19937, boost::uniform_int<> > rng(seed, bounds);

	msg.frame.payload.val = rng();
	msg.frame.payload.desc += msg.frame.payload.val & 0x1;
}

void sendOffer (XBee &xbee, XBeeAddress64& addr)
{
	cerr << "Sending offer msg to " << hex << addr.getMsb() << ":" << addr.getLsb() << "..." << endl;

	OfferMsg msg("PM2");
	msg.frame.payload.desc = 2 << 1;
	genRandom(msg);

	ZBTxRequest tx = ZBTxRequest(addr, msg.getData(), msg.getDataSize());

	xbee.send(tx);
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

	XBeeResponse response = XBeeResponse();
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

		if (!Msg::isPreambleOk(data) || data[4] != DiscoverMsg::MSG_TYPE)
			continue;

		usleep(100);

		sendOffer(xbee, rx.getRemoteAddress64());
	}

	return 0;
}
