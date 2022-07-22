/*
 * drv_gpio.h
 *
 *  Created on: 2022. 1. 9.
 *      Author: kms
 */

#ifndef PHOENIXON_DRV_DRV_GPIO_H_
#define PHOENIXON_DRV_DRV_GPIO_H_

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

void drv_initLED(void);
Uint16 drv_initChannelNumber(void);

#endif /* PHOENIXON_DRV_DRV_GPIO_H_ */
