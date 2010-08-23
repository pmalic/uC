extern "C" void __cxa_pure_virtual() { while (1); }

#include "XBee.h"

int main (void)
{

  XBee xbee = XBee();

  uint8_t cmd[] = { 'V', 'R' };

  AtCommandRequest atRequest = AtCommandRequest(cmd);
  AtCommandResponse atResponse = AtCommandResponse();

  xbee.begin(115200);
  xbee.send(atRequest);

  if (!xbee.readPacket(5000))
  {
//	  if (xbee.getResponse().isError())
//		  cerr << "Error reading packet. Error code " << static_cast<unsigned short>(xbee.getResponse().getErrorCode()) << endl;
//	  else
//		  cerr << "No response from radio." << endl;

	  return 1;
  }

  if (xbee.getResponse().getApiId() != AT_COMMAND_RESPONSE)
  {
	  //cerr << "Expected AT response but got " << static_cast<unsigned short>(xbee.getResponse().getApiId()) << endl;
	  return 1;
  }

  xbee.getResponse().getAtCommandResponse(atResponse);

  if (!atResponse.isOk())
  {
	  //cerr << "Command return error code: " << static_cast<unsigned short>(atResponse.getStatus()) << endl;
	  return 1;
  }

//  cerr << "Command [" << atResponse.getCommand()[0] << atResponse.getCommand()[1] << "] was successful!" << endl;

  unsigned short valueLen = static_cast<unsigned short>(atResponse.getValueLength());

  if (valueLen)
  {
//	  cerr << "Command value (length " << valueLen << "):" << endl;

//	  cerr << hex << uppercase;

	  char c, d;

	  for (int i = 0; i < valueLen; ++i)
	  {
		  c = static_cast<unsigned short>(atResponse.getValue()[i]);
		  d = c;
	   }

//	  cerr << dec << nouppercase << endl;
  }


  return 0;
}