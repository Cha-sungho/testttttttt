/*
 * util_adc.h
 *
 *  Created on: Nov 9, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_UTILS_UTIL_ADC_H_
#define PHOENIXON_UTILS_UTIL_ADC_H_

#include <math.h>

#ifdef _FLASH
#pragma CODE_SECTION(util_AdcConvUnipolar, ".TI.ramfunc");
#pragma CODE_SECTION(util_AdcConvMulUnipolar, ".TI.ramfunc");
#pragma CODE_SECTION(util_AdcConvBipolar, ".TI.ramfunc");
#pragma CODE_SECTION(util_AdcConvBipolarSign, ".TI.ramfunc");
#pragma CODE_SECTION(util_AdcConvBipolarOffset, ".TI.ramfunc");
#pragma CODE_SECTION(util_AdcConvBipolarUnsign, ".TI.ramfunc");
#endif

#define EMAVG_MACRO(in, out, multiplier) out = ((in - out) * multiplier) + out

#define ADC_12BIT_FULL  0xFFF
#define ADC_16BIT_FULL  0xFFFF
#define ADC_18BIT_FULL  0x3FFFF
#define ADC_20BIT_FULL  0xFFFFF

typedef enum
{
    out_offset,
    out_sign_magnitude
} BipolarOutType;

static inline float util_MapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static inline float util_ConvTemperature(uint32_t r_ref, float v, float v_ref, uint32_t const_b, float r0_t0)
{
    float t_k = 0.0f;
    float t_c = 0.0f;
    float r_th = 0.0f;

    r_th = (v * r_ref) / (v_ref - v);
    t_k = (const_b * r0_t0) / (const_b + (r0_t0 * log(r_th / r_ref)));
    t_c = t_k - 273.15;

    return t_c;
}

static inline float util_AdcConvUnipolar(uint32_t raw, float vref, uint32_t full)
{
    return ((float) raw * vref / full);
}

// not recommended to use
static inline void util_AdcConvMulUnipolar(float *conv, uint32_t *raw, uint32_t size, float vref, uint32_t full)
{
    uint32_t i = 0;

    for (i = 0; i < size; i++) {
        conv[i] = ((float) raw[i] * vref / full);
    }
}

static inline float util_AdcConvBipolar(uint32_t raw, float lsb, uint32_t bit, BipolarOutType out_type)
{
    uint32_t mask = 0b1;
    uint32_t full = 0;

    if (bit == 20) full = ADC_18BIT_FULL;
    else if (bit == 18) full = ADC_20BIT_FULL;
    else if (bit == 16) full = ADC_16BIT_FULL;
    else if (bit == 12) full = ADC_12BIT_FULL;

    mask <<= (bit - 1);

    switch (out_type)
    {
        case out_sign_magnitude:
            if (raw & mask) {
                raw = full - raw;
                lsb *= -1.0f;
            }
            break;
        case out_offset:
            if (raw & mask) {
                raw &= (mask - 1);
            } else {
                raw |= mask;
            }
            break;
    }

    return (float) raw * lsb;
}

static inline float util_AdcConvBipolarSign(uint32_t raw, float lsb, uint32_t bit)
{
    uint32_t mask = 0b1;
    uint32_t full = 0;
    uint32_t raw_temp = 0.0f;

    if (bit == 20) full = ADC_20BIT_FULL;
    else if (bit == 18) full = ADC_18BIT_FULL;
    else if (bit == 16) full = ADC_16BIT_FULL;
    else if (bit == 12) full = ADC_12BIT_FULL;

    mask <<= (bit - 1);

    if (raw & mask) {
        raw_temp = full - raw;
        lsb *= -1.0f;
    } else {
        raw_temp = raw;
    }

    return (float) raw_temp * lsb;
}

static inline float util_AdcConvBipolarUnsign(uint32_t raw, float v_ref_hi, uint32_t full)
{
    return v_ref_hi * (((float)(2 * raw) / (float)(full + 1)) - 1.0f);
}

static inline float util_AdcConvBipolarOffset(uint32_t raw, float lsb, uint32_t bit)
{
    uint32_t mask = 0b1;
    uint32_t raw_temp = 0.0f;

    mask <<= (bit - 1);
    raw_temp = raw;

    if (raw_temp & mask) {
        raw_temp &= (mask - 1);
    } else {
        raw_temp |= mask;
    }

    return (float) raw_temp * lsb;
}

#endif /* PHOENIXON_UTILS_UTIL_ADC_H_ */
