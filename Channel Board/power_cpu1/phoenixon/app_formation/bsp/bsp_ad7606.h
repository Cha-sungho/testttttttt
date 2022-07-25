/*
 * bsp_ad7606.h
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_BSP_BSP_AD7606_H_
#define PHOENIXON_BSP_BSP_AD7606_H_

#include "driverlib.h"
#include "drv_gpio.h"
#include "drv_emif.h"

#define AD7606_CS_PIN           34
#define AD7606_RD_PIN           37

#define AD7606_RST_PIN          35
#define AD7606_RANGE_PIN        40
#define AD7606_OS0_PIN          86
#define AD7606_OS1_PIN          87
#define AD7606_OS2_PIN          88
#define AD7606_BUSY_PIN         91

#define AD7606_BIT              16
#define AD7606_LSB              ((float)0.0000305)  //if input range 20V, 0 ~ 20V or -10V ~ 10V
#define AD7606_DIV2_LSB         ((float)0.0000152)   //if input range 10V, 0 ~ 10V or -5.0V ~ 5.0V

typedef enum
{
    ad7606_range_5v,
    ad7606_range_10v,
} Ad7606ORangeList;

typedef enum
{
    ad7606_ovs_none,    //4.2us
    ad7606_ovs_2,       //9.1us
    ad7606_ovs_4,       //18.8us
    ad7606_ovs_8,       //39us
    ad7606_ovs_16,      //78us
    ad7606_ovs_32,      //158us
    ad7606_ovs_64       //315us
} Ad7606OVSList;

#define AD7606_CONV_ADDR        (uint32_t)(0x100000)      //y1 = 100x1 = 10001
#define AD7606_READ_ADDR        (uint32_t)(0x100000)      //y0 = 100x0 = 10000

void bsp_InitAD7606(Ad7606ORangeList range, Ad7606OVSList ovs);
void bsp_ConvAD7606(void);
uint16_t bsp_GetAD7606(uint16_t *raw);
uint16_t bsp_GetFulSeqAD7606(uint16_t *raw);

#endif /* PHOENIXON_BSP_BSP_AD7606_H_ */
