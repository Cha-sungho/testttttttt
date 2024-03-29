/*
 * drv_emif.h
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_EMIF_H_
#define PHOENIXON_DRV_DRV_EMIF_H_

#define EMIF_CS2_BASE_ADDRESS   0x00100000
#define EMIF_CS2_END_ADDRESS    0x002FFFFF
#define EMIF_CS2_LENGTH         ((EMIF_CS2_END_ADDRESS - EMIF_CS2_END_ADDRESS) + 1)

void drv_InitEMIF(void);
uint16_t drv_WriteEMIF(uint32_t addr, uint16_t data);
uint16_t drv_ReadEMIF(uint32_t addr, uint16_t *data);
uint16_t drv_Read16MULEMIF(uint32_t addr, uint16_t *data);

#endif /* PHOENIXON_DRV_DRV_EMIF_H_ */
