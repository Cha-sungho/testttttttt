/*
 * I2C_TCA9535.c
 *
 *  Created on: 2017. 10. 2.
 *      Author: BGKim
 */
#include "F2837xD_device.h"
#include "F2837xD_Examples.h"

void I2CA_TCA9535_Init(void)
{
	EALLOW;
	GPIO_SetupPinMux(91, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinMux(92, GPIO_MUX_CPU1, 6);

	I2caRegs.I2CPSC.all = 19;          // Prescaler - need 7-12 Mhz on module clk

	I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
	I2caRegs.I2CCLKH = 10;             // NOTE: must be non zero
	I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts

	I2caRegs.I2CMDR.bit.IRS = 1;     // Take I2C out of reset

	I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
	//I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,
	EDIS;

//	while(!I2CA_TCA9535_WriteData(0x02, 0));
//	while(!I2CA_TCA9535_WriteData(0x03, 0));
//	while(!I2CA_TCA9535_WriteData(0x06, 0));
//	while(!I2CA_TCA9535_WriteData(0x07, 0));
}

int I2CA_TCA9535_WriteData(unsigned int ui_Address, unsigned int ui_Value)
{
//	I2caRegs.I2CMDR.bit.IRS = 0;     // Take I2C out of reset
//	I2caRegs.I2CMDR.bit.IRS = 1;     // Take I2C out of reset

	if(I2caRegs.I2CMDR.bit.STP == 1)
   {
      return (0);
   }

   I2caRegs.I2CSAR.all = I2C_TCA9535_ADDRESS;

   if(I2caRegs.I2CSTR.bit.BB == 1)
   {
      return (0);
   }

   I2caRegs.I2CCNT = 2;

   I2caRegs.I2CDXR.all = (0xFF & ui_Address);
   I2caRegs.I2CDXR.all = (0xFF & ui_Value);

   I2caRegs.I2CMDR.all = 0x6E20;
   //DELAY_US(20);

   return (1);
}

unsigned int TCA9535_IOPort_Setting(unsigned int ui_PortNumber, unsigned int Value)
{
	static unsigned int ui_OldPortValue = 0;
	unsigned int ui_NewPortValue = 0;
	unsigned int ui_LowBit = 0;
	unsigned int ui_HighBit = 0;

	unsigned int ui_Temp = 0;

	if(ui_PortNumber > 16)
	{
		return (ui_OldPortValue);
	}

	if(ui_PortNumber == 0)
	{
		if(Value)
		{
			if(ui_OldPortValue == 0xFFFF)
			{
				return (ui_OldPortValue);
			}
			else
			{
				while(!I2CA_TCA9535_WriteData(0x02, 0xFF));
				while(!I2CA_TCA9535_WriteData(0x03, 0xFF));
				ui_OldPortValue = 0xFFFF;
			}
		}
		else
		{
			if(ui_OldPortValue == 0x0)
			{
				return (ui_OldPortValue);
			}
			else
			{
				while(!I2CA_TCA9535_WriteData(0x02, 0x0));
				while(!I2CA_TCA9535_WriteData(0x03, 0x0));
				ui_OldPortValue = 0x0;
			}
		}
	}
	else
	{
		ui_NewPortValue = ui_OldPortValue;

		ui_Temp = 1 << (ui_PortNumber - 1);

		if(Value)
		{
			ui_NewPortValue |= ui_Temp;
		}
		else
		{
			ui_NewPortValue &= (~ui_Temp);
		}

		if(ui_NewPortValue == ui_OldPortValue)
		{
			return 0;
		}
		else
		{
			ui_OldPortValue = ui_NewPortValue;
		}

		ui_LowBit  = (0xFF & ui_OldPortValue);
		ui_HighBit = (0xFF & (ui_OldPortValue >> 8));

		if(ui_PortNumber >= 1 && ui_PortNumber <= 8)
		{
			while(!I2CA_TCA9535_WriteData(0x02, ui_LowBit));
			//I2CA_TCA9535_WriteData(0x02, ui_LowBit);
		}
		else
		{
			while(!I2CA_TCA9535_WriteData(0x03, ui_HighBit));
			//I2CA_TCA9535_WriteData(0x03, ui_HighBit);
		}
	}

	return (ui_OldPortValue);
}

//
// End of file
//

