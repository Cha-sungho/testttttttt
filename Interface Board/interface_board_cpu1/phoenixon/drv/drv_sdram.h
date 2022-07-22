/*
 * drv_sdram.h
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#ifndef PHOENIXON_DRV_DRV_SDRAM_H_
#define PHOENIXON_DRV_DRV_SDRAM_H_

//ISSI IS42S86400D 512Mb SDRAM
//16M * 8(bit) * 4(Bank) = 64M(8bit)
//IS42S86400D * 2EA = 64M(8bit) + 64M(8bit) = 64M(16bit)
//64M(16bit) = Uint16 * 64M
#define SDRAM_CAPACITY          536870912  // bits
#define SDRAM_DATA_BANDWIDTH    8          // 8bit
#define SDRAM_BASE_ADDRESS      EMIF_CS0_BASE_ADDRESS //CS0 End Address 0x0x8000 0000, End Address 0x8FFF FFFF
#ifdef PNX_DEBUG
#define SDRAM_LENGTH            0x1000 // 64M(16bit) (((SDRAM_CAPACITY / SDRAM_DATA_BANDWIDTH) + SDRAM_BASE_ADDRESS) + 1)
#else
#define SDRAM_LENGTH            0x2000000 // 64M(16bit) (((SDRAM_CAPACITY / SDRAM_DATA_BANDWIDTH) + SDRAM_BASE_ADDRESS) + 1)
#endif
#define SDRAM_MEM_BUFFER_SIZE   0x500

Uint32 drv_InitSdram(void);
void drv_ClearSdram(Uint32 Ul_Address, Uint32 ul_Size);
Uint32 drv_CheckSdram(Uint32 Ul_Address, Uint32 ul_Size);

#endif /* PHOENIXON_DRV_DRV_SDRAM_H_ */
