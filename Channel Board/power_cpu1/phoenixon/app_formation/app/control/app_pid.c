/*
 * app_pid.c
 *
 *  Created on: 2022. 1. 18.
 *      Author: JOO
 */

#include "app_pid.h"

float app_pi(float ref, float in, PidError *error, PidGain k, float sample_t)
{
    float out;

    error->e = ref - in;

    error->sum += (error->e - error->w) * k.i * sample_t;
    out = (k.p * error->e) + error->sum;

    return out;
}

float app_pid(float ref, float in, PidError *error, PidGain k, float sample_t)
{
    float out, error_delta;

    error->e = ref - in;

    error->sum += (error->e - error->w) * k.i * sample_t;
    error_delta = (error->e - error->past) / sample_t;
    error->past = error->e;

    out = (k.p * error->e) + error->sum + (k.d * error_delta);

    return out;
}

void app_antiwindup(float pi_out, float limiter_out, PidError *error, PidGain k)
{
    float windup_error;

    windup_error = pi_out - limiter_out;
    error->w = k.a * windup_error;
}

float app_limiter(float in, float upper, float lower)
{
    if (in > upper)         return upper;
    else if (in < lower)    return lower;
    else                    return in;
}

//
// End of File
//