/*
 * drv_emif.h
 *
 *  Created on: 2021. 6. 9.
 *      Author: KHJ
 */

#ifndef PHOENIXON_DRV_DRV_EMIF_H_
#define PHOENIXON_DRV_DRV_EMIF_H_

#define EMIF_CS0_BASE_ADDRESS 0x80000000
#define EMIF_CS0_END_ADDRESS  0x8FFFFFFF
#define EMIF_CS0_LENGTH       ((EMIF_CS0_END_ADDRESS - EMIF_CS0_END_ADDRESS) + 1)

#define EMIF_CS2_BASE_ADDRESS 0x00100000
#define EMIF_CS2_END_ADDRESS  0x002FFFFF
#define EMIF_CS2_LENGTH       ((EMIF_CS2_END_ADDRESS - EMIF_CS2_END_ADDRESS) + 1)

void drv_InitEmif(void);

#ifdef CPU1
// Function Prototypes
void setupEMIF1PinmuxSync16Bit(void);
void setupEMIF1PinmuxAsync16Bit(void);
#endif

Uint16 DSP_EmifRead(Uint32 ul_Address, Uint16 *ui_Data);
Uint16 DSP_EmifWrite(Uint32 ul_Address, Uint16 ui_Data);

#endif /* PHOENIXON_DRV_DRV_EMIF_H_ */
