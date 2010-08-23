//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// NXP LPC2000 MCU low-level driver
// (c) Rados�aw Kwiecie�, radek@dxp.pl
//-------------------------------------------------------------------------------------------------
#include <targets/LPC210x.h>
#include <targets/LPC2148.h>

// data bus
#define KS0108_DATA_DIR		IO0DIR
#define KS0108_DATA_PIN		IO0PIN
#define KS0108_DATA_SET		IO0SET
#define KS0108_DATA_CLR		IO0CLR

#define KS0108_D0			0

// control bus
#define KS0108_CTRL_DIR		IO0DIR
#define KS0108_CTRL_SET		IO0SET
#define KS0108_CTRL_CLR		IO0CLR

#define KS0108_RS			(1 << 17)
#define KS0108_RW			(1 << 16)
#define KS0108_EN			(1 << 19)

#define KS0108_CS1			(1 << 15)
#define KS0108_CS2			(1 << 18)
#define KS0108_CS3			(1 << 31)

#define DISPLAY_STATUS_BUSY	0x80

extern unsigned char screen_x;
extern unsigned char screen_y;

//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
__asm("nop");
__asm("nop");
}
//-------------------------------------------------------------------------------------------------
// Enalbe Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_CLR = KS0108_CS1; break;
	case 1 : KS0108_CTRL_CLR = KS0108_CS2; break;
	case 2 : KS0108_CTRL_CLR = KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-2)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
switch(controller){
	case 0 : KS0108_CTRL_SET = KS0108_CS1; break;
	case 1 : KS0108_CTRL_SET = KS0108_CS2; break;
	case 2 : KS0108_CTRL_SET = KS0108_CS3; break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status byte from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
unsigned char status;
KS0108_DATA_DIR &= ~(0xFF << KS0108_D0);
KS0108_CTRL_SET = KS0108_RW;
KS0108_CTRL_CLR = KS0108_RS;
GLCD_EnableController(controller);
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
status = (KS0108_DATA_PIN >> KS0108_D0);
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(controller);
return status;
}
//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
while(GLCD_ReadStatus(controller)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR |= (0xFF << KS0108_D0);
KS0108_CTRL_CLR = (KS0108_RW | KS0108_RS);
GLCD_EnableController(controller);

KS0108_DATA_SET = (commandToWrite << KS0108_D0);
commandToWrite ^= 0xFF;
KS0108_DATA_CLR = (commandToWrite << KS0108_D0);

KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(controller);
}
//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
unsigned char data;
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR &= ~(0xFF << KS0108_D0);
KS0108_CTRL_SET = (KS0108_RW | KS0108_RS);
GLCD_EnableController(screen_x / 64);
//GLCD_Delay();
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
data = (KS0108_DATA_PIN >>  KS0108_D0);
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
return data;
}
//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite)
{
while(GLCD_ReadStatus(screen_x / 64)&DISPLAY_STATUS_BUSY);
KS0108_DATA_DIR |= (0xFF << KS0108_D0);
KS0108_CTRL_CLR = KS0108_RW;
KS0108_CTRL_SET = KS0108_RS;

KS0108_DATA_SET = (dataToWrite << KS0108_D0);
dataToWrite ^= 0xFF;
KS0108_DATA_CLR = (dataToWrite << KS0108_D0);

GLCD_EnableController(screen_x / 64);
KS0108_CTRL_SET = KS0108_EN;
GLCD_Delay();
KS0108_CTRL_CLR = KS0108_EN;
GLCD_DisableController(screen_x / 64);
screen_x++;
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitalizePorts(void)
{
VPBDIV = 1;
KS0108_CTRL_DIR |= (KS0108_CS1 | KS0108_CS2 | KS0108_CS3 | KS0108_RS | KS0108_RW | KS0108_EN);
KS0108_CTRL_SET = (KS0108_CS1 | KS0108_CS2 | KS0108_CS3);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
return *(ptr);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
