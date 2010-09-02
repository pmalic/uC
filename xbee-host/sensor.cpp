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

void sendOffer (XBee &xbee, XBeeAddress64& addr)
{
	cerr << "Sending offer msg to " << hex << addr.getMsb() << ":" << addr.getLsb() << "..." << endl;

	OfferMsg msg("PM2");
	msg.header.val = 2550;

	ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	XBee xbee = XBee(Platform::SerialPortConf(argv[1]));

	XBeeResponse response = XBeeResponse();
	ZBRxResponse rx = ZBRxResponse();

	xbee.begin(115200);

	while (true)
	{
		xbee.readPacket(5000);

		if (!xbee.getResponse().isAvailable() || xbee.getResponse().getApiId() != ZB_RX_RESPONSE)
			continue;

		xbee.getResponse().getZBRxResponse(rx);

		unsigned short frameLen = static_cast<unsigned short>(rx.getDataLength());

		if (!frameLen || rx.getData(0) != Msg::PREAMBLE || rx.getData(1) != DiscoverMsg::MSG_TYPE)
			continue;

		usleep(100);

		sendOffer(xbee, rx.getRemoteAddress64());
	}

	return 0;
}
