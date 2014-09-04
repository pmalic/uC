#include <boost/format.hpp>

#include <xbee/XBee.h>
#include <wsan/DiscoverMsg.h>
#include <wsan/NumValMsg.h>
#include <wsan/BoolValMsg.h>
#include <wsan/StrValMsg.h>

using namespace std;
using namespace wsan;

typedef unique_ptr<Msg> msg_ptr_type;
typedef list<msg_ptr_type> msg_ptr_list_type;

void sendDiscovery (XBee& xbee)
{
	cerr << "Sending discovery msg..." << endl;

	XBeeAddress64 addr = XBeeAddress64(0x0, 0xffff);

	DiscoverMsg msg("CRDPM");

	ZBTxRequest tx = ZBTxRequest(addr, const_cast<uint8_t*>(msg.getData()), msg.getDataSize());

	xbee.send(tx);
}

void readDiscovery (XBee& xbee, msg_ptr_list_type& vals)
{
	cerr << "Waiting for msgs..." << endl;

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

		if (!Msg::isPreambleOk(data))
			continue;

		switch (data[PREAMBLE_SIZE])
		{
			case NumValMsg::TYPE:
			{
				msg_ptr_type msg(new NumValMsg(data, data_size));
				vals.push_back(std::move(msg));
				break;
			}

			case BoolValMsg::TYPE:
			{
				msg_ptr_type msg(new BoolValMsg(data, data_size));
				vals.push_back(std::move(msg));
				break;
			}

			case StrValMsg::TYPE:
			{
				msg_ptr_type msg(new StrValMsg(data, data_size));
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
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	SERIAL serial = SERIAL(argv[1]);
	serial.begin(115200);

	XBee xbee = XBee();
	xbee.setSerial(serial);

	msg_ptr_list_type vals;

	while (true)
	{
		sendDiscovery(xbee);

		readDiscovery(xbee, vals);

		cerr << ">>> RECEIVED VALUES <<<" << endl;

		for (msg_ptr_list_type::const_iterator it = vals.begin(), it_end = vals.end(); it != it_end; ++it)
		{
			cerr << (*it)->getNodeName() << ' ' << (*it)->getDesc() << ": ";

			switch ((*it)->getType())
			{
				case 'N':
				{
					const NumValMsg* msg = reinterpret_cast<const NumValMsg*>(it->get());
					cerr << boost::format("%.2f") % msg->getValue();
					break;
				}

				case 'B':
				{
					const BoolValMsg* msg = reinterpret_cast<const BoolValMsg*>(it->get());
					cerr << (msg->getValue() ? "TRUE" : "FALSE");
					break;
				}

				case 'S':
				{
					const StrValMsg* msg = reinterpret_cast<const StrValMsg*>(it->get());
					cerr << '"' << msg->getValue() << '"';
					break;
				}

				default:
					break;
			}

			cerr << endl;
		}

		cerr << endl << endl;

		vals.clear();
	}

	return 0;
}
