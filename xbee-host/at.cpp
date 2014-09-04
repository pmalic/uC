#include <xbee/XBee.h>

int main (int argc, char* argv[])
{
	using namespace std;

	if (argc < 3)
	{
		cerr << "Not enough parameters!" << endl;
		return 1;
	}

	SERIAL serial = SERIAL(argv[1]);
	XBee xbee = XBee();

	char cmd[] = { argv[2][0], argv[2][1] };

	AtCommandRequest atRequest = AtCommandRequest(reinterpret_cast<uint8_t*>(cmd));
	AtCommandResponse atResponse = AtCommandResponse();

	serial.begin(115200);
	xbee.setSerial(serial);

	xbee.send(atRequest);

	if (!xbee.readPacket(5000))
	{
		if (xbee.getResponse().isError())
			cerr << "Error reading packet. Error code " << static_cast<unsigned short>(xbee.getResponse().getErrorCode()) << endl;
		else
			cerr << "No response from radio." << endl;

		return 1;
	}

	if (xbee.getResponse().getApiId() != AT_COMMAND_RESPONSE)
	{
		cerr << "Expected AT response but got " << static_cast<unsigned short>(xbee.getResponse().getApiId()) << endl;
		return 1;
	}

	xbee.getResponse().getAtCommandResponse(atResponse);

	if (!atResponse.isOk())
	{
		cerr << "Command returned error code: " << static_cast<unsigned short>(atResponse.getStatus()) << endl;
		return 1;
	}

	cerr << "Command '" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "' was successful, ";

	const unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

	if (!valueLen)
		cerr << "no return value." << endl;
	else
	{
		cerr << "return value is " << valueLen << " byte(s) long:" << endl;

		const uint8_t* value = atResponse.getValue();

		cerr << hex << uppercase;

		for (int i = 0; i < valueLen; ++i)
			cerr << ' ' << setfill('0') << setw(2) << static_cast<unsigned short>(value[i]);

		cerr << dec << nouppercase << ' ';

		for (int i = 0; i < valueLen; ++i)
				cerr << (value[1] >= 0x20 && value[i] <= 0x7e ? static_cast<char>(value[i]) : '.');

		cerr << endl;

	}

	return 0;
}
