/*
 * Phoenixon_SCID.c
 *
 *  Created on: 2017. 12. 2.
 *      Author: BGKim
 */

#include "..\include\F2837xD_device.h"
#include "..\include\F2837xD_Examples.h"

unsigned int ASCIIToUInt(unsigned int ui_Cipher, unsigned int *aui_ASCIIData)
{
	unsigned int ui_i = 0;
	unsigned int ui_ConversionData = 0;
	unsigned int ui_CipherValue = 1;

	ui_ConversionData = 0;

	for(ui_i = ui_Cipher - 1; ;ui_i--)
	{
		ui_ConversionData += (aui_ASCIIData[ui_i] - 0x30) * ui_CipherValue;
		ui_CipherValue *= 10;
		if(ui_i == 0)break;
	}
	return (unsigned int)(ui_ConversionData);
}

void UIntToASCII(unsigned int ui_Cipher, unsigned int ui_Data,  unsigned int *aui_ASCIIData)
{
	unsigned int ui_i = 0;
	unsigned int ui_ConversionData = 0;
	unsigned int ui_CipherValue = 1;

	ui_ConversionData = 0;

	for(ui_i = ui_Cipher - 1; ;ui_i--)
	{
		ui_ConversionData = ui_Data % (ui_CipherValue * 10);
		ui_ConversionData /= ui_CipherValue;
		ui_ConversionData += 0x30;
		aui_ASCIIData[ui_i] = ui_ConversionData;

		ui_CipherValue *= 10;
		if(ui_i == 0)break;
	}
}

void FloatToArray(unsigned int ui_Cipher, float f_Data,  unsigned int *aui_ArrayData)
{
	unsigned int ui_i = 0;
	unsigned long ul_ConversionData = 0;
	unsigned long ul_CipherValue = 1;
	unsigned long ul_Data = 0;

	ul_Data = (unsigned long)(f_Data);

	ul_ConversionData = 0;

	for(ui_i = ui_Cipher - 1; ;ui_i--)
	{
		ul_ConversionData = ul_Data % (ul_CipherValue * 10);
		ul_ConversionData /= ul_CipherValue;
		aui_ArrayData[ui_i] = ul_ConversionData;

		ul_CipherValue *= 10;
		if(ui_i == 0)break;
	}
}

void ScidSetup(void)
{
	//
	// For this example, only init the pins for the SCI-B port.
	//  GPIO_SetupPinMux() - Sets the GPxMUX1/2 and GPyMUX1/2 register bits
	//  GPIO_SetupPinOptions() - Sets the direction and configuration of the GPIOS
	// These functions are found in the F2837xD_Gpio.c file.

	GPIO_SetupPinMux(93, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(93, GPIO_INPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(94, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(94, GPIO_OUTPUT, GPIO_ASYNC);

	//
	// Note: Clocks were turned on to the SCIA peripheral
	// in the InitSysCtrl() function
	//

	ScidRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
									// No parity,8 char bits,
									// async mode, idle-line protocol
	ScidRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
									// Disable RX ERR, SLEEP, TXWAKE
	ScidRegs.SCICTL2.all = 0x0003;
	ScidRegs.SCICTL2.bit.TXINTENA = 1;
	ScidRegs.SCICTL2.bit.RXBKINTENA = 1;

	//
	// SCIA at 9600 baud
	// @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
	// @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
	//
	ScidRegs.SCIHBAUD.all = 0x0002;
	ScidRegs.SCILBAUD.all = 0x008B;

	ScidRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset

	// Initialize the SCI FIFO

	ScidRegs.SCIFFTX.all = 0xE040;
	ScidRegs.SCIFFRX.all = 0x2044;
	ScidRegs.SCIFFCT.all = 0x0;
}

unsigned int ScidPut(unsigned int ui_Character)
{
	unsigned long ul_TimeOut = 0;
	ul_TimeOut = ~ul_TimeOut;

    while (ScidRegs.SCIFFTX.bit.TXFFST != 0)
    {
    	if(!(ul_TimeOut--))
    	{
    		ScidRegs.SCICTL1.bit.SWRESET = 0;
    		DELAY_US(1);
    		ScidRegs.SCICTL1.bit.SWRESET = 1;

    		return (SCI_TX_ERROR);
    	}
    }

    ScidRegs.SCITXBUF.all =(unsigned char)(0xFF & ui_Character);

    return (ui_Character);
}

unsigned int ScidWirte(unsigned int *ui_String, unsigned int ui_Length)
{
    unsigned int ui_i = 0;
    unsigned int ui_ReturnValue = 0;

	for(ui_i = ui_Length - 1; ;ui_i--)
//    for(ui_i = 0; ui_i < ui_Length; ui_i++)
    {
    	ui_ReturnValue = ScidPut(ui_String[ui_i]);
    	if(ui_ReturnValue == SCI_TX_ERROR)
    	{
    		return (ui_ReturnValue);
    	}
    	if(ui_i == 0)break;
    }

	return (ui_ReturnValue);
}

unsigned int ScidGet(void)
{
	unsigned long ul_TimeOut = 0;
	ul_TimeOut = ~ul_TimeOut;

    while(ScidRegs.SCIFFRX.bit.RXFFST == 0)
    {
    	if(ScidRegs.SCIRXST.bit.RXERROR)
    	{
    		ScidRegs.SCICTL1.bit.SWRESET = 0;
    		DELAY_US(1);
    		ScidRegs.SCICTL1.bit.SWRESET = 1;

    		return (SCI_RX_ERROR);
    	}
    	if(!(ul_TimeOut--))
    	{
    		return (SCI_RX_ERROR);
    	}
    }

    return ((unsigned int)(0xFF & ScidRegs.SCIRXBUF.all));
}
//===========================================================================
// End of file.
//===========================================================================
