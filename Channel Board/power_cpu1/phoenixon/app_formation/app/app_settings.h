/*
 * app_settings.h
 *
 *  Created on: 2021. 12. 6.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_APP_SETTINGS_H_
#define PHOENIXON_APP_APP_SETTINGS_H_

#define T_SAMP              (50e-6)
#define F_SAMP              ((float)(1. / T_SAMP))
#define GRID_FREQ           ((float)60.)

#define REF_S_TIME(time)    (unsigned long)((time * F_SAMP)-1)
#define REF_MS_TIME(time)   (unsigned long)((time * F_SAMP * 0.001)-1)

#define DEBUGGING   0

extern float set_pu_out_amps;
extern float set_pu_out_volts;

extern float set_pu_dc_link_pri_volts;
extern float set_pu_dc_link_sec_volts;

extern float set_pu_out_L;
extern float set_pu_out_buck_C;
extern float set_pu_out_boost_C;

extern float set_pu_ctrl_t;
extern float set_pu_ctrl_f;

extern float set_calc_system_out_power_kw;
extern float set_calc_dc_link_pri_volts_max;
extern float set_calc_dc_link_pri_volts_min;
extern float set_calc_dc_link_sec_volts_max;
extern float set_calc_dc_link_sec_volts_min;
extern float set_calc_dc_out_cap_volts_max;
extern float set_calc_dc_out_cap_volts_min;
extern float set_calc_battary_volts_max;
extern float set_calc_battary_volts_min;
extern float set_calc_output_current_max;
extern float set_calc_output_current_min;

void app_InitSettings(void);

#endif /* PHOENIXON_APP_APP_SETTINGS_H_ */