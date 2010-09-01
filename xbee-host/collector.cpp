#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "xbee/XBee.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/OfferMsg.h"

void sendDiscovery (XBee &xbee)
{
	std::cerr << "Sending discovery msg..." << std::endl;

	XBeeAddress64 addr = XBeeAddress64(0x0, 0xffff);

	wsan::DiscoverMsg msg;

	ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}

void readDiscovery (XBee &xbee)
{
	std::cerr << "Waiting for offer msgs..." << std::endl;

	Platform::Stopwatch stopwatch;

	while (stopwatch.read() < 3000)
	{
		if (!xbee.readPacket(500))
			continue;

		XBeeResponse& res = xbee.getResponse();

		if (res.getApiId() != ZB_RX_RESPONSE)
			continue;

		ZBRxResponse rx = ZBRxResponse();
		res.getZBRxResponse(rx);

		uint8_t len = rx.getDataLength();

		if (!len || rx.getData(0) != wsan::Msg::PREAMBLE || rx.getData(1) != wsan::OfferMsg::MSG_TYPE)
			continue;

		wsan::OfferMsg msg(rx.getData());

		std::cerr << "Got offer from node: " << msg.header.node_name << std::endl;
	}

}

int main (int argc, char* argv[])
{
	using namespace std;
	using namespace wsan;

	if (argc < 2)
	{
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	XBee xbee = XBee(Platform::SerialPortConf(argv[1]));

	xbee.begin(115200);

	while (true)
	{
		sendDiscovery(xbee);

		readDiscovery(xbee);
	}

	return 0;
}
