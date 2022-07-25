/*
 * drv_gpio.h
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_GPIO_H_
#define PHOENIXON_DRV_DRV_GPIO_H_

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#define drv_WriteGPIO(pin, val)     GPIO_writePin(pin, val)
#define drv_ReadGPIO(pin)           GPIO_readPin(pin)
#define drv_ToggleGPIO(pin)         GPIO_togglePin(pin)

void drv_ConfigGPIO(GPIO_CoreSelect cpu, uint16_t pin, GPIO_Direction dir, uint32_t mux, uint16_t pad, GPIO_QualificationMode qualification, uint32_t divider);

#endif /* PHOENIXON_DRV_DRV_GPIO_H_ */
