#include <cstring>
#include <vector>
#include "ks0108/KS0108.h"
#include "xbee/XBee.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/NumValMsg.h"

#define MAX_TXT_SIZE	32

extern "C" void __cxa_pure_virtual () { while (1); }

extern "C" void abort (void) { while (1); }

using namespace wsan;
using namespace std;

void sendDiscovery (XBee& xbee)
{
	XBeeAddress64 addr = XBeeAddress64(0x0, 0xffff);

	DiscoverMsg msg;

	ZBTxRequest tx = ZBTxRequest(addr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}

void readDiscovery (XBee& xbee, vector<OfferMsg>& offers)
{
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
	}

}

int main (void)
{
	GLCD_Initalize();
	GLCD_ClearScreen();

	XBee xbee = XBee();

	xbee.begin(115200);

	vector<OfferMsg> offers;
	char txt[MAX_TXT_SIZE];

	while (true)
	{
		sendDiscovery(xbee);

		readDiscovery(xbee, offers);

		GLCD_ClearScreen();
		GLCD_GoTo(0, 0);

		snprintf(txt, MAX_TXT_SIZE, "FOUND %d SENSORS:", offers.size());
		GLCD_WriteString(txt);

		short cnt = 0;

		for (vector<OfferMsg>::const_iterator it = offers.begin(), it_end = offers.end(); it != it_end && ++cnt < 15; ++it)
		{
			GLCD_GoTo(cnt % 2 == 0 ? 64 : 0, (cnt - 1) / 2 + 1);

			const OfferMsg& msg = *it;

			unsigned int decimals = msg.header.desc >> 1;
			unsigned int factor = 1;

			while (decimals--)
				factor *= 10;

			snprintf(txt, MAX_TXT_SIZE, "%s:%c%02d.%02d", msg.header.node_name, (msg.header.desc & 0x1 ? '-' : '+'), msg.header.val / factor, msg.header.val % factor);
			GLCD_WriteString(txt);
		}

		offers.clear();
	}

	return 0;
}
