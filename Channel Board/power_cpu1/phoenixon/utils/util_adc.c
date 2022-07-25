/*
 * util.adc.c
 *
 *  Created on: Nov 10, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "util_adc.h"

#if 0

#ifdef CPU2
#ifdef _FLASH
#pragma CODE_SECTION(util_ConvInSingleSense, ".TI.ramfunc");
#pragma CODE_SECTION(util_ConvMultiInSingleSense, ".TI.ramfunc");
#pragma CODE_SECTION(util_ConvInDiffSense, ".TI.ramfunc");
#endif
#endif

float util_ConvInSingleSense(uint32_t raw, float vref, uint32_t bit)
{
    return ((float) raw * vref / bit);
}

void util_ConvMultiInSingleSense(float *conv, uint32_t *raw, uint32_t size, float vref, uint32_t bit)
{
    uint32_t i = 0;

    for (i = 0; i < size; i++) {
        conv[i] = ((float) raw[i] * vref / bit);
    }
}

float util_ConvInDiffSense(uint32_t data, float lsb, uint32_t bit, DiffOutType out_type)
{
    float ret_temp = 0.0f;
    uint32_t mask = 0b1;
    uint32_t full = 0;
#if 0
    full = pow(2, bit);
#else
    if(bit == 20) full = ADC_20BIT_FULL;
    if(bit == 18) full = ADC_18BIT_FULL;
    if(bit == 16) full = ADC_16BIT_FULL;
    if(bit == 12) full = ADC_12BIT_FULL;
#endif
    mask <<= (bit - 1);

    switch (out_type)
    {
        case signed_output:
            if (data & mask) {
                data = full - data;
                lsb *= -1.0f;
            }

            ret_temp = (float) data * lsb;
            break;
        case unsigned_output:
            if (data & mask) {
                data &= (mask - 1);
            } else {
                data |= mask;
            }

            ret_temp = (float) data * lsb;
            break;
    }

    return ret_temp;
}

#endif
