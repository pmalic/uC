#include <xbee/XBee.h>

int main (int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Not enough parameters!" << std::endl;
		return 1;
	}

	SERIAL serial(argv[1]);
	serial.begin(115200);

	XBee xbee;
	xbee.setSerial(serial);

	char cmd[] = { argv[2][0], argv[2][1] };

	AtCommandRequest atRequest(reinterpret_cast<uint8_t*>(cmd));
	AtCommandResponse atResponse;

	xbee.send(atRequest);

	if (!xbee.readPacket(5000))
	{
		if (xbee.getResponse().isError())
			std::cerr << "Error reading packet. Error code " << static_cast<unsigned short>(xbee.getResponse().getErrorCode()) << std::endl;
		else
			std::cerr << "No response from radio." << std::endl;

		return 1;
	}

	if (xbee.getResponse().getApiId() != AT_COMMAND_RESPONSE)
	{
		std::cerr << "Expected AT response but got " << static_cast<unsigned short>(xbee.getResponse().getApiId()) << std::endl;
		return 1;
	}

	xbee.getResponse().getAtCommandResponse(atResponse);

	if (!atResponse.isOk())
	{
		std::cerr << "Command returned error code: " << static_cast<unsigned short>(atResponse.getStatus()) << std::endl;
		return 1;
	}

	std::cerr << "Command '" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "' was successful, ";

	const unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

	if (!valueLen)
		std::cerr << "no return value." << std::endl;
	else
	{
		std::cerr << "return value is " << valueLen << " byte(s) long:" << std::endl;

		const uint8_t* value = atResponse.getValue();

		std::cerr << std::hex << std::uppercase;

		for (int i = 0; i < valueLen; ++i)
			std::cerr << ' ' << std::setfill('0') << std::setw(2) << static_cast<unsigned short>(value[i]);

		std::cerr << std::dec << std::nouppercase << ' ';

		for (int i = 0; i < valueLen; ++i)
				std::cerr << (value[1] >= 0x20 && value[i] <= 0x7e ? static_cast<char>(value[i]) : '.');

		std::cerr << std::endl;

	}

	return 0;
}
