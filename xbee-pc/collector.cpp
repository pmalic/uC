#include <boost/format.hpp>

#include <xbee/XBee.h>
#include <wsan/DiscoverMsg.h>
#include <wsan/NumValMsg.h>
#include <wsan/BoolValMsg.h>
#include <wsan/StrValMsg.h>

typedef std::unique_ptr<wsan::Msg> msg_ptr_type;
typedef std::list<msg_ptr_type> msg_ptr_list_type;

void readDiscovery (XBee& xbee, msg_ptr_list_type& vals)
{
	std::cerr << "Waiting for msgs..." << std::endl;

	Stopwatch stopwatch;

	while (stopwatch.read() < 2000)
	{
		xbee.readPacket();

		XBeeResponse& res = xbee.getResponse();

		if (!res.isAvailable() || res.getApiId() != ZB_RX_RESPONSE)
			continue;

		ZBRxResponse rx;
		res.getZBRxResponse(rx);

		const uint8_t data_size = rx.getDataLength();

		if (data_size < 5)
			continue;

		const uint8_t* data = rx.getData();

		if (!wsan::Msg::isPreambleOk(data))
			continue;

		switch (data[wsan::Msg::PREAMBLE_SIZE])
		{
			case wsan::NumValMsg::TYPE:
			{
				msg_ptr_type msg(new wsan::NumValMsg(data, data_size));
				vals.push_back(std::move(msg));
				break;
			}

			case wsan::BoolValMsg::TYPE:
			{
				msg_ptr_type msg(new wsan::BoolValMsg(data, data_size));
				vals.push_back(std::move(msg));
				break;
			}

			case wsan::StrValMsg::TYPE:
			{
				msg_ptr_type msg(new wsan::StrValMsg(data, data_size));
				vals.push_back(std::move(msg));
				break;
			}
		}

		usleep(10);
	}

}

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

	while (true)
	{
		std::cerr << "Sending discovery msg..." << std::endl;

		wsan::DiscoverMsg msg("CRDPM");

		XBeeAddress64 addr(0x0, 0xffff);
		ZBTxRequest tx(addr, const_cast<uint8_t*>(msg.getData()), msg.getDataSize());
		xbee.send(tx);

		msg_ptr_list_type vals;
		readDiscovery(xbee, vals);

		std::cerr << ">>> RECEIVED VALUES <<<" << std::endl;

		for (msg_ptr_list_type::const_iterator it = vals.begin(), it_end = vals.end(); it != it_end; ++it)
		{
			std::cerr << (*it)->getNode() << ' ' << (*it)->getDesc() << ": ";

			switch ((*it)->getType())
			{
				case 'N':
				{
					const wsan::NumValMsg* msg = reinterpret_cast<const wsan::NumValMsg*>(it->get());
					std::cerr << boost::format("%.2f") % msg->getValue();
					break;
				}

				case 'B':
				{
					const wsan::BoolValMsg* msg = reinterpret_cast<const wsan::BoolValMsg*>(it->get());
					std::cerr << (msg->getValue() ? "TRUE" : "FALSE");
					break;
				}

				case 'S':
				{
					const wsan::StrValMsg* msg = reinterpret_cast<const wsan::StrValMsg*>(it->get());
					std::cerr << '"' << msg->getValue() << '"';
					break;
				}

				default:
					break;
			}

			std::cerr << std::endl;
		}

		std::cerr << std::endl << std::endl;

		vals.clear();
	}

	return 0;
}
