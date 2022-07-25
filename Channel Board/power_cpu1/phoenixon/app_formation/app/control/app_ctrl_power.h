/*
 * app_ctrl_power.h
 *
 *  Created on: 2022. 1. 5.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_CONTROL_APP_CTRL_POWER_H_
#define PHOENIXON_APP_CONTROL_APP_CTRL_POWER_H_

#define CONTROL_SEQUENCE_MS_COUNT(x)         (Uint32)(x / DSP_TIMER0_TIME * 1000)  // ms

typedef enum
{
    no_operation,
    output_precharge_start,
    output_precharge,
    output_precharge_end,
    control_idle,
    output_mosfet_on,
    operation_control,
    calibration_current,
    calibration_voltage
#if(DEBUGGING == 1)
    basic_discharge,
    basic_charge,
#endif
} appFormationSeq;

typedef enum
{
    idle,
    charge_mode,
    discharge_mode
} c_d_mode;

void app_CtrlPower(void);
void app_controlseqeunse(void);
void control_value_init(void);
void buck_boost_control_precharge(float vol_ref, float cur_ref);
void buck_boost_control(Uint16 char_dischar, float vol_ref, float cur_ref);
void appCvModeCheck(uint32_t voltage, float margin);
#endif /* PHOENIXON_APP_CONTROL_APP_CTRL_POWER_H_ */
