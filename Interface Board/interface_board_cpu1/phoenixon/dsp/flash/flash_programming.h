/*
 * flash_programming.h
 *
 *  Created on: 2020. 5. 21.
 *      Author: boomw
 */

#ifndef SRC_FLASH_FLASH_PROGRAMMING_H_
#define SRC_FLASH_FLASH_PROGRAMMING_H_

#include "Types.h"

Fapi_StatusType Flash_write(uint32 sector, uint16 *buf, uint16 len);
Fapi_StatusType Flash_read(uint32 sector, uint16 *buf, uint16 len);
Fapi_StatusType Flash_erase(void);

#endif /* SRC_FLASH_FLASH_PROGRAMMING_H_ */
