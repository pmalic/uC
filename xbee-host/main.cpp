#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "XBee.h"

int main (int argc, char* argv[])
{
	using namespace std;

	if (argc < 2)
	{
		cerr << "No command!" << endl;
		return 1;
	}

	XBee xbee = XBee(); //Platform::SerialPortConf("/dev/tty.usbserial-A700eX8n"));

	uint8_t cmd[] = { argv[1][0], argv[1][1] };

	AtCommandRequest atRequest = AtCommandRequest(cmd);
	AtCommandResponse atResponse = AtCommandResponse();

	xbee.begin(115200);
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
		cerr << "Command return error code: " << static_cast<unsigned short>(atResponse.getStatus()) << endl;
		return 1;
	}

	cerr << "Command [" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "] was successful!" << endl;

	unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

	if (valueLen)
	{
		cerr << "Command value (length " << valueLen << "):" << endl;

		cerr << hex << uppercase;

		for (int i = 0; i < valueLen; ++i)
			cerr << static_cast<unsigned short>(atResponse.getValue()[i]) << " ";

		cerr << dec << nouppercase << endl;
	}

	return 0;
}
