/*
 * Phoenixon_EMIF_AD7679.c
 *
 *  Created on: 2017. 11. 20.
 *      Author: BGKim
 */

#include "F2837xD_device.h"
#include "F2837xD_Examples.h"

#define ASRAM_CS2_START_ADDR 0x100000
#define ASRAM_CS2_END_ADDR   0x102800
#define AD7679_STEP_ADDRESS 0x200

void Emif1_AD7679_Init(void)
{
	  EALLOW;
	 // ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x0;
	  ClkCfgRegs.PERCLKDIVSEL.bit.EMIF1CLKDIV = 0x0;
	  Emif1ConfigRegs.EMIF1MSEL.all = 0x93A5CE71;
	  Emif1ConfigRegs.EMIF1ACCPROT0.all = 0x0;
	  Emif1ConfigRegs.EMIF1COMMIT.all = 0x1;
	  Emif1ConfigRegs.EMIF1LOCK.all = 0x1;
	  EDIS;
	//
	//Configure GPIO pins for EMIF1
	//

	//  GPIO_SetupPinMux(37,0,2);
	  GPIO_SetupPinMux(34,0,2);
	  GPIO_SetupPinMux(31,0,2);


	  GPIO_SetupPinMux(85,0,2);
	  GPIO_SetupPinMux(83,0,2);
	  GPIO_SetupPinMux(82,0,2);
	  GPIO_SetupPinMux(81,0,2);
	  GPIO_SetupPinMux(80,0,2);
	  GPIO_SetupPinMux(79,0,2);
	  GPIO_SetupPinMux(78,0,2);
	  GPIO_SetupPinMux(77,0,2);
	  GPIO_SetupPinMux(76,0,2);
	  GPIO_SetupPinMux(75,0,2);
	  GPIO_SetupPinMux(74,0,2);
	  GPIO_SetupPinMux(73,0,2);
	  GPIO_SetupPinMux(72,0,2);
	  GPIO_SetupPinMux(71,0,2);
	  GPIO_SetupPinMux(70,0,2);
	  GPIO_SetupPinMux(69,0,2);
	  GPIO_SetupPinMux(68,0,2);
	  GPIO_SetupPinMux(67,0,2);

	  GPIO_SetupPinMux(48,0,2);
	  GPIO_SetupPinMux(49,0,2);
	  GPIO_SetupPinMux(50,0,2);
//	  GPIO_SetupPinMux(51,0,2);

	  GPIO_SetupPinOptions(85,0,0x31);
	  GPIO_SetupPinOptions(83,0,0x31);
	  GPIO_SetupPinOptions(82,0,0x31);
	  GPIO_SetupPinOptions(81,0,0x31);
	  GPIO_SetupPinOptions(80,0,0x31);
	  GPIO_SetupPinOptions(79,0,0x31);
	  GPIO_SetupPinOptions(78,0,0x31);
	  GPIO_SetupPinOptions(77,0,0x31);
	  GPIO_SetupPinOptions(76,0,0x31);
	  GPIO_SetupPinOptions(75,0,0x31);
	  GPIO_SetupPinOptions(74,0,0x31);
	  GPIO_SetupPinOptions(73,0,0x31);
	  GPIO_SetupPinOptions(72,0,0x31);
	  GPIO_SetupPinOptions(71,0,0x31);
	  GPIO_SetupPinOptions(70,0,0x31);
	  GPIO_SetupPinOptions(69,0,0x31);
	  GPIO_SetupPinOptions(68,0,0x31);
	  GPIO_SetupPinOptions(67,0,0x31);

	  //Configure GPIO pins for EMIF1 END

	  GPIO_SetupPinMux(32,0,0); //ADC_RESET
	  GPIO_SetupPinMux(37,0,0); //ADC_READ
	  GPIO_SetupPinMux(31,0,0); //ADC_CONVERSION
	//  GPIO_SetupPinMux(34,0,0);
	  //GPIO_SetupPinOptions(35,GPIO_OUTPUT,GPIO_PUSHPULL);
	  GPIO_SetupPinOptions(32,GPIO_OUTPUT,GPIO_PUSHPULL); //ADC_RESET
	  GPIO_SetupPinOptions(37,GPIO_OUTPUT,GPIO_PUSHPULL); //ADC_READ
	  GPIO_SetupPinOptions(31,GPIO_OUTPUT,GPIO_PUSHPULL); //ADC_CONVERSION
	//  GPIO_SetupPinOptions(34,GPIO_OUTPUT,GPIO_PUSHPULL);

	//  Emif1Regs.ASYNC_CS2_CR.all =  (EMIF_ASYNC_ASIZE_32    | // 16Bit Memory
	//                                                          // Interface
	//                                 EMIF_ASYNC_TA_4        | // Turn Around time
	//                                                          // of 2 Emif Clock
	//                                 EMIF_ASYNC_RHOLD_4     | // Read Hold time
	//                                                          // of 1 Emif Clock
	//                                 EMIF_ASYNC_RSTROBE_10   | // Read Strobe time
	//                                                          // of 4 Emif Clock
	//                                 EMIF_ASYNC_RSETUP_4    | // Read Setup time
	//                                                          // of 1 Emif Clock
	//                                 EMIF_ASYNC_WHOLD_4     | // Write Hold time
	//                                                          // of 1 Emif Clock
	//                                 EMIF_ASYNC_WSTROBE_6   | // Write Strobe time
	//                                                          // of 1 Emif Clock
	//                                 EMIF_ASYNC_WSETUP_4    | // Write Setup time
	//                                                          // of 1 Emif Clock
	//                                 EMIF_ASYNC_EW_DISABLE  | // Extended Wait
	//                                                          // Disable.
	//                                 EMIF_ASYNC_SS_DISABLE    // Strobe Select Mode
	//                                                          // Disable.
	//                                );

	  Emif1Regs.ASYNC_CS2_CR.all =  (EMIF_ASYNC_ASIZE_32    | // 16Bit Memory
	                                                          // Interface
	                                 EMIF_ASYNC_TA_4        | // Turn Around time
	                                                          // of 2 Emif Clock
	                                 EMIF_ASYNC_RHOLD_8     | // Read Hold time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_RSTROBE_16   | // Read Strobe time
	                                                          // of 4 Emif Clock
	                                 EMIF_ASYNC_RSETUP_16    | // Read Setup time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WHOLD_8     | // Write Hold time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WSTROBE_16   | // Write Strobe time  64
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_WSETUP_16    | // Write Setup time
	                                                          // of 1 Emif Clock
	                                 EMIF_ASYNC_EW_DISABLE  | // Extended Wait
	                                                          // Disable.
	                                 EMIF_ASYNC_SS_DISABLE    // Strobe Select Mode
	                                                          // Disable.
	                                );


	  GPIO_WritePin(32,1);
	  DELAY_US(500000);
	  GPIO_WritePin(32,0);
}
int AD7679_Conversion(void)
{
	GPIO_WritePin(37,0);//ADC_READ
//	DELAY_US(1);
//	GPIO_WritePin(32,0);
//	DELAY_US(1);

	//*(unsigned long *)(ASRAM_CS2_END_ADDR) = 0;

	GPIO_WritePin(31,0);//ADC_CONVERSION
	DELAY_US(1);
	GPIO_WritePin(31,1);//ADC_CONVERSION
	DELAY_US(1);

//	GPIO_WritePin(32,1);
//	DELAY_US(1);
//	GPIO_WritePin(32,0);
//	DELAY_US(1);
//	*(unsigned long *)ASRAM_CS2_START_ADDR = 0;
//	DELAY_US(1);

	return (0);
}

float AD7679_Read(unsigned int ui_CS_Select)
{
	unsigned long ul_Temp = 0;
//	unsigned long ul_Temp2 = 0;

	float f_RealValue = 0.0;

	if(ui_CS_Select > 15)return(~ul_Temp);
//	GPIO_WritePin(34,0);
//	GPIO_WritePin(31,0);
//	GPIO_WritePin(37,0);
	ul_Temp = *(unsigned long *)(ASRAM_CS2_START_ADDR + (AD7679_STEP_ADDRESS * ui_CS_Select));


//	ul_Temp = *(unsigned long *)(ASRAM_CS2_START_ADDR + (AD7679_STEP_ADDRESS * ui_CS_Select));
//	ul_Temp = *(unsigned long *)(ASRAM_CS2_START_ADDR + (AD7679_STEP_ADDRESS * ui_CS_Select));
//	GPIO_WritePin(37,1);
//	GPIO_WritePin(31,1);
//	GPIO_WritePin(34,1);
	f_RealValue =(float)ul_Temp;
	ul_Temp = *(unsigned long *)(ASRAM_CS2_END_ADDR);
		return (f_RealValue);
}



