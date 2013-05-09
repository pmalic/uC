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

	ZBTxRequest tx = ZBTxRequest(addr, msg.getData(), msg.getDataSize());

	xbee.send(tx);
}

void readDiscovery (XBee& xbee, vector<OfferMsg>& offers)
{
	cerr << "Waiting for offer msgs..." << endl;

	Stopwatch stopwatch;

	while (stopwatch.read() < 3000)
	{
		xbee.readPacket();

		XBeeResponse& res = xbee.getResponse();

		if (!res.isAvailable() || res.getApiId() != ZB_RX_RESPONSE)
			continue;

		ZBRxResponse rx = ZBRxResponse();
		res.getZBRxResponse(rx);

		const uint8_t data_size = rx.getDataLength();

		if (data_size < 5)
			continue;

		uint8_t* data = rx.getData();

		if (!Msg::isPreambleOk(data) || data[4] != OfferMsg::MSG_TYPE)
			continue;

		OfferMsg msg(data, data_size);

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

	SERIAL serial = SERIAL(argv[1]);
	serial.begin(115200);

	XBee xbee = XBee();
	xbee.setSerial(serial);

	vector<OfferMsg> offers;

	while (true)
	{
		sendDiscovery(xbee);

		readDiscovery(xbee, offers);

		cerr << ">>> RECEIVED OFFERS <<<" << endl;

		for (vector<OfferMsg>::const_iterator it = offers.begin(), it_end = offers.end(); it != it_end; ++it)
		{
			const OfferMsg& msg = *it;

			unsigned int decimals = msg.frame.payload.desc >> 1;
			unsigned int factor = 1;

			while (decimals--)
				factor *= 10;

			cerr << msg.frame.payload.node_name << ":" << (msg.frame.payload.desc & 0x1 ? '-' : '+') <<	(msg.frame.payload.val / factor) << '.' << (msg.frame.payload.val % factor) << "C ";
		}

		cerr << endl << endl;

		offers.clear();
	}

	return 0;
}
