#include <cstring>
#include "xbee/XBee.h"
#include "onewire/OneWire.h"
#include "wsan/DiscoverMsg.h"
#include "wsan/NumValMsg.h"

extern "C" void __cxa_pure_virtual() { while (1); }

extern "C" void abort (void) { while (1); }

using namespace wsan;
using namespace std;

bool findSensor (OneWire& ow, uint8_t* owaddr)
{
	uint8_t retries = 10;

	while (true)
		if (ow.search(owaddr))
			break;
		else if (--retries == 0)
			return false;

	if (OneWire::crc8(owaddr, 7) != owaddr[7])
		return false;

	return true;
}

void readTemp (OneWire& ow, uint8_t* owaddr, OfferMsg& msg)
{
	ow.reset();
	ow.select(owaddr);
	ow.write(0x44, 1);

	delay(750);

	ow.reset();
	ow.select(owaddr);
	ow.write(0xBE);

	uint8_t i, data[12];

	for (i = 0; i < 9; ++i)
		data[i] = ow.read();

	int temp = (data[1] << 8) + data[0];
	int sign = temp & 0x8000;

	if (sign)
	{
		temp = (temp ^ 0xffff) + 1;
		++msg.header.desc;
	}

	msg.header.val = owaddr[0] == 0x10 ? (temp * 100 / 2) : (temp * 6 + temp / 4);
}

void sendOffer (XBee &xbee, XBeeAddress64& xbaddr, OneWire &ow, uint8_t* owaddr)
{
	OfferMsg msg("PM1");

	msg.header.desc = 2 << 1;
	readTemp(ow, owaddr, msg);

	ZBTxRequest tx = ZBTxRequest(xbaddr, msg.getFrame(), msg.getFrameLen());

	xbee.send(tx);
}

int main (void)
{
	OneWire ow(30);
	uint8_t owaddr[8];

	if (!findSensor(ow, owaddr))
		return 1;

	XBee xbee = XBee();

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

		sendOffer(xbee, rx.getRemoteAddress64(), ow, owaddr);
	}

	return 0;
}
