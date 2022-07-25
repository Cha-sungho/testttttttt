/*
 * drv_gpio.c
 *
 *  Created on: 2022. 1. 9.
 *      Author: kms
 */



#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

void drv_initLED(void)
{
    GPIO_setDirectionMode(35, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(35, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_35_GPIO35);
    GPIO_setMasterCore(35, GPIO_CORE_CPU1);
}

Uint16 drv_initChannelNumber(void)
{
    // CH Number |  CH 1  | CH 0
    //   0       |   0    |  0
    //   1       |   0    |  1
    //   2       |   1    |  0
    //   3       |   1    |  1
    Uint16 channel_number=0;

    //CH0
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(16, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setMasterCore(16, GPIO_CORE_CPU1);

    //CH1
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(16, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setMasterCore(16, GPIO_CORE_CPU1);

    channel_number|=GPIO_ReadPin(16);
    channel_number|=(GPIO_ReadPin(17)<<1);


    return channel_number;

}


