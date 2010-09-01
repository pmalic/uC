#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "xbee/XBee.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/OfferMsg.h"

void sendOffer (XBee &xbee, XBeeAddress64& addr)
{
	std::cerr << "Sending offer msg to " << std::hex << addr.getMsb() << ":" << addr.getLsb() << "..." << std::endl;

	wsan::OfferMsg msg("PM2");

	ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}


int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Not enough parameters!" << std::endl;
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

		if (!frameLen || rx.getData(0) != wsan::Msg::PREAMBLE || rx.getData(1) != wsan::DiscoverMsg::MSG_TYPE)
			continue;

		usleep(100);

		sendOffer(xbee, rx.getRemoteAddress64());
	}

	return 0;
}
