#include <stdio.h>

extern "C" void __cxa_pure_virtual() { while (1); }

#include "xbee/XBee.h"
#include "ks0108/KS0108.h"
#include "onewire/OneWire.h"

#define MAX_MSG_SIZE	32

int main (void)
{
	GLCD_Initalize();
	GLCD_ClearScreen();

	XBee xbee = XBee();

	uint8_t cmd[] = { 'N', 'I' };

	AtCommandRequest atRequest = AtCommandRequest(cmd);
	AtCommandResponse atResponse = AtCommandResponse();

	xbee.begin(115200);
	xbee.send(atRequest);

	char msg[MAX_MSG_SIZE];
	GLCD_GoTo(0, 0);

	if (!xbee.readPacket(5000))
	{
		if (xbee.getResponse().isError())
			snprintf(msg, MAX_MSG_SIZE, "Read err code: %d", static_cast<unsigned short>(xbee.getResponse().getErrorCode()));
		else
			snprintf(msg, MAX_MSG_SIZE, "No response");

		GLCD_WriteString(msg);

		return 1;
	}

	if (xbee.getResponse().getApiId() != AT_COMMAND_RESPONSE)
	{
		snprintf(msg, MAX_MSG_SIZE, "Got wrong API ID: %d", static_cast<unsigned short>(xbee.getResponse().getApiId()));
		GLCD_WriteString(msg);

		return 1;
	}

	xbee.getResponse().getAtCommandResponse(atResponse);

	if (!atResponse.isOk())
	{
		snprintf(msg, MAX_MSG_SIZE, "Cmd err code: %d", static_cast<unsigned short>(atResponse.getStatus()));
		GLCD_WriteString(msg);

		return 1;
	}

	snprintf(msg, MAX_MSG_SIZE, "Cmd [%c%c] succeeded", atResponse.getCommand()[0], atResponse.getCommand()[1]);
	GLCD_WriteString(msg);

	unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

	if (valueLen)
	{
		GLCD_GoTo(0, 1);
		snprintf(msg, MAX_MSG_SIZE, "SCmd val (len %d):", valueLen);
		GLCD_WriteString(msg);

		GLCD_GoTo(0, 2);
		for (int i = 0; i < valueLen; ++i)
		{
			snprintf(msg, MAX_MSG_SIZE, "%c ", static_cast<unsigned short>(atResponse.getValue()[i]));
			GLCD_WriteString(msg);
		}
	}

	return 0;
}
