/*
 * app_sense.h
 *
 *  Created on: Nov 9, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_SENSE_APP_SENSE_H_
#define PHOENIXON_APP_SENSE_APP_SENSE_H_

#define USE_DB_FL               0       //with db
#define USE_RB_MAVG_FL          1       //with array
#define USE_TI_EMAVG_FL         1       //with TI EMAVG
#define USE_TI_RAMPGEN_TEST     0       //with TI RAMPGEN
#define USE_TI_POWER_MEASURE    1       //with TI Power measurement

//Sense values for AC/DC
extern float calc_grid_a_v;        //L-N Calculation Value
extern float calc_grid_b_v;
extern float calc_grid_c_v;

extern float sense_grid_a_i;
extern float sense_grid_b_i;
extern float sense_grid_c_i;

extern float sense_dc_link_pri_v;

//Sense values for DC/DC
extern float sense_dc_link_sec_v;
extern float sense_dc_out_cap_v;
extern float sense_dc_inductor_h_i;
extern float sense_dc_inductor_l_i;
extern float sense_dc_inductor_h_l_i;
extern float sense_batt_1234_v;

extern float sense_dc_bb1_i_raw_v_emavg;
extern float sense_dc_bb2_i_raw_v_emavg;

extern float sense_dc_out_cap_v_cal;
extern float sense_batt_v_cal;
extern float sense_dc_bb1_i_cal;
extern float sense_dc_bb2_i_cal;

void app_InitSense(void);
float app_InitSenseOffset1(float *value);
float app_InitSenseOffset2(float *value);

void app_ConvDataDspAdcMain(void);
void app_ConvDataDspAdcSub(void);
void app_ConvDataExternalAdc(void);

void app_GetDspAdcMain(void);
void app_GetDspAdcSub(void);
void app_GetExternalAdc(void);

void ConfigureADC(void);
void SetupADCSoftware(void);
void app_ConvThermal(void);
float scale_select1(float32 *CAL_Scale_Data, float32 Unit);
float scale_select2(float32 *CAL_Scale_Data, float32 Unit);

#endif /* PHOENIXON_APP_SENSE_APP_SENSE_H_ */
