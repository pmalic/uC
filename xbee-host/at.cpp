#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "xbee/XBee.h"

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

	uint8_t cmd[] = { argv[2][0], argv[2][1] };

	AtCommandRequest atRequest = AtCommandRequest(cmd);
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

	cerr << "Command [" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "] was successful!" << endl;

	unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

	if (valueLen)
	{
		cerr << "Command value (length " << valueLen << "):" << endl;

		cerr << hex << uppercase;

		for (int i = 0; i < valueLen; ++i)
			cerr << static_cast<char>(atResponse.getValue()[i]) << " ";

		cerr << dec << nouppercase << endl;
	}

	return 0;
}
