/*
 * drv_usb.h
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#ifndef PHOENIXON_DRV_DRV_USB_H_
#define PHOENIXON_DRV_DRV_USB_H_

#include "fatfs/src/ff.h"

void drv_InitUsb(void);
void drv_InitFatfs(void);

FRESULT hal_f_write(uint16_t *f_buf, uint16_t f_size);

#endif /* PHOENIXON_DRV_DRV_USB_H_ */
