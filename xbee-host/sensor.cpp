#include <boost/random.hpp>
#include <boost/format.hpp>

#include <xbee/XBee.h>
#include <wsan/DiscoverMsg.h>
#include <wsan/NumValMsg.h>
#include <wsan/BoolValMsg.h>
#include <wsan/StrValMsg.h>

boost::variate_generator<boost::mt19937, boost::uniform_int<> > rndIntGen(boost::mt19937(time(0)), boost::uniform_int<>(-10000, 10000));

int main (int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Not enough parameters!" << std::endl;
		return 1;
	}

	SERIAL serial(argv[1]);
	serial.begin(115200);

	XBee xbee;
	xbee.setSerial(serial);

	ZBRxResponse rx;

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

		wsan::DiscoverMsg msg(data, data_size);

		if (msg.getType() != wsan::DiscoverMsg::TYPE)
			continue;

		usleep(100);

		XBeeAddress64& addr = rx.getRemoteAddress64();

		std::cerr << "Replying to " << msg.getNodeName() << " ("<< std::hex << addr.getMsb() << ':' << addr.getLsb() << ")..." << std::endl;

		{
			wsan::NumValMsg msg("PMSENSOR");
			const double d = rndIntGen() / 100.0;
			msg.setValue(d, 2);
			msg.setDesc("DBL DESC");
			std::cerr << "\t" << msg.getDesc() << ": " << boost::format("%.2f") % d << std::endl;

			ZBTxRequest tx(addr, const_cast<uint8_t*>(msg.getData()), msg.getDataSize());
			xbee.send(tx);
		}

		{
			wsan::BoolValMsg msg("PMSENSOR");
			const bool b = rndIntGen() % 2 == 0;
			msg.setValue(b);
			msg.setDesc("BOOL DSC");
			std::cerr << "\t" << msg.getDesc() << ": " << (b ? "TRUE" : "FALSE") << std::endl;

			ZBTxRequest tx(addr, const_cast<uint8_t*>(msg.getData()), msg.getDataSize());
			xbee.send(tx);
		}

		{
			wsan::StrValMsg msg("PMSENSOR");
			const char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor...";
			msg.setValue(s);
			msg.setDesc("STR DESC");
			std::cerr << "\t" << msg.getDesc() << ": \"" << s << '"' << std::endl;

			ZBTxRequest tx(addr, const_cast<uint8_t*>(msg.getData()), msg.getDataSize());
			xbee.send(tx);
		}
	}

	return 0;
}
