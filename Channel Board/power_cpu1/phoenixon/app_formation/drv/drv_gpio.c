/*
 * drv_gpio.c
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "drv_gpio.h"

void drv_ConfigGPIO(GPIO_CoreSelect cpu, uint16_t pin, GPIO_Direction dir, uint32_t mux, uint16_t pad, GPIO_QualificationMode qualification, uint32_t divider)
{
    EALLOW;

    if (cpu != 0xFFFF) GPIO_setMasterCore(pin, cpu);
    if (dir != 0xFFFF) GPIO_setDirectionMode(pin, dir);
    if (mux != 0xFFFF) GPIO_setPinConfig(mux);
    if (pad != 0xFFFF) GPIO_setPadConfig(pin, pad);
    if (qualification != 0xFFFF) GPIO_setQualificationMode(pin, qualification);
    if (divider != 0xFFFF) GPIO_setQualificationPeriod(pin, divider);

    EDIS;
}

//
// End of File
//
