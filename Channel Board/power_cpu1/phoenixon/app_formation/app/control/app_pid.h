/*
 * app_pid.h
 *
 *  Created on: 2022. 1. 18.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_CONTROL_APP_PID_H_
#define PHOENIXON_APP_CONTROL_APP_PID_H_

typedef struct
{
    float p;
    float i;
    float d;
    float a;  // anti windup gain
} PidGain;

typedef struct
{
    float e;
    float sum;
    float past;
    float w;
} PidError;

#endif /* PHOENIXON_APP_CONTROL_APP_PID_H_ */
