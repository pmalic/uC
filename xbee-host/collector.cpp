#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "xbee/XBee.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/OfferMsg.h"

using namespace std;
using namespace wsan;

void sendDiscovery (XBee& xbee)
{
	cerr << "Sending discovery msg..." << endl;

	XBeeAddress64 addr = XBeeAddress64(0x0, 0xffff);

	DiscoverMsg msg;

	ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}

void readDiscovery (XBee& xbee, vector<OfferMsg>& offers)
{
	cerr << "Waiting for offer msgs..." << endl;

	Platform::Stopwatch stopwatch;

	while (stopwatch.read() < 3000)
	{
		xbee.readPacket();

		XBeeResponse& res = xbee.getResponse();

		if (!res.isAvailable() || res.getApiId() != ZB_RX_RESPONSE)
			continue;

		ZBRxResponse rx = ZBRxResponse();
		res.getZBRxResponse(rx);

		uint8_t len = rx.getDataLength();

		if (!len || rx.getData(0) != Msg::PREAMBLE || rx.getData(1) != OfferMsg::MSG_TYPE)
			continue;

		OfferMsg msg(rx.getData());

		offers.push_back(msg);

		usleep(10);
	}

}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	XBee xbee = XBee(Platform::SerialPortConf(argv[1]));

	xbee.begin(115200);

	vector<OfferMsg> offers;

	while (true)
	{
		sendDiscovery(xbee);

		readDiscovery(xbee, offers);

		cerr << "Received offers:" << endl;

		for (vector<OfferMsg>::const_iterator it = offers.begin(), it_end = offers.end(); it != it_end; ++it)
		{
			const OfferMsg& msg = *it;

			unsigned int decimals = msg.header.desc >> 1;
			unsigned int factor = 1;

			while (decimals--)
				factor *= 10;

			cerr << msg.header.node_name << ":" << (msg.header.desc & 0x1 ? '-' : '+') <<	(msg.header.val / factor) << '.' << (msg.header.val % factor) << " C" << endl;
		}

		offers.clear();
	}

	return 0;
}
