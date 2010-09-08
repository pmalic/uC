#include <cstring>
#include <vector>
#include <targets/LPC21xx.h>
#include "ks0108/KS0108.h"
#include "onewire/OneWire.h"

#define MAX_TXT_SIZE	32

using namespace std;

int main (void)
{
	GLCD_Initalize();
	GLCD_ClearScreen();

	GLCD_GoTo(0, 0);

	char txt[MAX_TXT_SIZE];

	OneWire ds(30);
	uint8_t addr[8];

	uint8_t retries = 10;

	while (true)
		if (ds.search(addr))
			break;
		else if (--retries == 0)
		{
			GLCD_WriteString("NO DS18x20 FOUND!");
			return 1;
		}

	if (OneWire::crc8(addr, 7) != addr[7])
	{
		GLCD_WriteString("DS18x20 CRC INVALID!");
		return 1;
	}

	snprintf(txt, MAX_TXT_SIZE, "FOUND DS18%c20:", addr[0] == 0x10 ? 'S' : 'B');
	GLCD_WriteString(txt);

	uint8_t i, data[12];

	int temp, sign, whole, fract;

	while (true)
	{
		ds.reset();
		ds.select(addr);
		ds.write(0x44, 1);

		delay(100);

		ds.reset();
		ds.select(addr);
		ds.write(0xBE);

		for (i = 0; i < 9; ++i)
			data[i] = ds.read();

		temp = (data[1] << 8) + data[0];
		sign = temp & 0x8000;

		if (sign)
			temp = (temp ^ 0xffff) + 1;

		temp = addr[0] == 0x10 ? (temp * 100 / 2) : (temp * 6 + temp / 4);

		whole = temp / 100;
		fract = temp % 100;

		GLCD_GoTo(0, 1);

		snprintf(txt, MAX_TXT_SIZE, "%c%d.%02d C", sign ? '-' : '+', whole, fract < 10 ? 0 : fract);

		GLCD_WriteString(txt);
	}

	return 0;
}
