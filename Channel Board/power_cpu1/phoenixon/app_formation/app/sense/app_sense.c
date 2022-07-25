/*
 * app_sense.c
 *
 *  Created on: Nov 9, 2021
 *      Author: JOO
 */

#include <math.h>

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

//TI DIGITAL POWER SDK
#include "power_meas_sine_analyzer.h"
#include "emavg.h"
#include "rampgen.h"

//Phoenixon
#include <math.h>
#include "util_adc.h"

#include "drv_adc.h"
#include "drv_timer.h"

#include "bsp_ad7606.h"
#include "bsp_io.h"

#include "app_sense.h"
#include "app_settings.h"
#include "app_constant.h"
#include "app_tick.h"

#include "ringbuffer.h"

#include "app_op.h"
#include "util_sort.h"
#include "app_calibration.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(app_ConvDataDspAdcMain, ".TI.ramfunc");
#pragma CODE_SECTION(app_ConvDataDspAdcSub, ".TI.ramfunc");
#pragma CODE_SECTION(app_ConvDataExternalAdc, ".TI.ramfunc");
#endif
#endif

#ifdef CPU2
#ifdef _FLASH
#pragma CODE_SECTION(app_GetDspAdcMain, ".TI.ramfunc");
#pragma CODE_SECTION(app_GetDspAdcSub, ".TI.ramfunc");
#pragma CODE_SECTION(app_GetExternalAdc, ".TI.ramfunc");
#endif
#endif

#define SAMPLING_MAX_DATA 30

Uint16 thermal_table[30] = { 30688, 37988, 43500, 47709, 50945, 53457, 55423,
                             56966, 58191, 59164, 59946, 60577, 61089, 61506, 61849, 62131,
                             62365, 62560, 62723, 62859, 62975, 63072, 63156, 63226, 63287,
                             63339, 63384, 63423, 63456, 63485 };

float battary_t_buf_val[30] = { 0, };
float t1_buf_val[30] = { 0, };
float t2_buf_val[30] = { 0, };
float t3_buf_val[30] = { 0, };

extern uint32_t gs_cpu2_adc_ad7606_raw[8];
extern uint32_t gs_cpu2_adc_dsp_raw[16];
extern float gs_cpu2_adc_ad7606_raw_v[8];
extern float gs_cpu2_adc_dsp_raw_v[16];

extern float Current_Charge_Scale1[];
extern float Current_Discharge_Scale1[];
extern float Current_Charge_Scale2[];
extern float Current_Discharge_Scale2[];
extern float current_ref;

uint16_t cpu2_adc_ad7606_raw[8];
uint32_t cpu2_adc_dsp_raw[16];

float adc_ad7606_raw_v[8] = { 0.0f, };
float adc_dsp_raw_v[16] = { 0.0f, };

//DC-Link Primary
float sense_dc_link_pri_f_v = 0.0f;
float sense_dc_link_pri_f_v_scale = 0.0f;
float sense_dc_link_pri_f_v_offset = 0.0f;
float sense_dc_link_pri_f_v_emavg = 0.0f;

float sense_dc_link_pri_r_v = 0.0f;
float sense_dc_link_pri_r_v_scale = 0.0f;
float sense_dc_link_pri_r_v_offset = 0.0f;
float sense_dc_link_pri_r_v_emavg = 0.0f;

//DC-Link Secondary
float sense_dc_link_sec_v = 0.0f;
float sense_dc_link_sec_v_scale = 0.0f;
float sense_dc_link_sec_v_offset = 0.0f;
float sense_dc_link_sec_v_emavg = 0.0f;

//Out-CAP
float sense_dc_out_cap_v = 0.0f;
float sense_dc_out_cap_v_cal = 0.0f;
float sense_dc_out_cap_v_scale = 0.0f;
float sense_dc_out_cap_v_offset = 0.0f;
float sense_dc_out_cap_v_emavg = 0.0f;
float UI_sense_dc_out_cap_v_emavg = 0.0f;

//Current
float sense_dc_bb1_i_raw_v_emavg = 0.0f;
float sense_dc_bb1_i = 0.0f;
float sense_dc_bb1_i_cal = 0.0f;
float sense_dc_bb1_i_scale = 0.0f;
float sense_dc_bb1_i_scale_charge = 0.0f;
float sense_dc_bb1_i_scale_discharge = 0.0f;
float sense_dc_bb1_i_offset = 0.0f;
float sense_dc_bb1_i_emavg = 0.0f;
float UI_sense_dc_bb1_i_emavg = 0.0f;

float sense_dc_bb2_i_raw_v_emavg = 0.0f;
float sense_dc_bb2_i = 0.0f;
float sense_dc_bb2_i_cal = 0.0f;
float sense_dc_bb2_i_scale = 0.0f;
float sense_dc_bb2_i_scale_charge = 0.0f;
float sense_dc_bb2_i_scale_discharge = 0.0f;
float sense_dc_bb2_i_offset = 0.0f;
float sense_dc_bb2_i_emavg = 0.0f;
float UI_sense_dc_bb2_i_emavg = 0.0f;

float calc_dc_out_i = 0.0f;
float calc_dc_out_i_emavg = 0.0f;
float UI_calc_dc_out_i_emavg = 0.0f;

//Batt Voltage
float sense_batt_v = 0.0f;
float sense_batt_v_cal = 0.0f;
float sense_batt_v_scale = 0.0f;
float sense_batt_v_offset = 0.0f;
float sense_batt_v_emavg = 0.0f;
float UI_sense_batt_v_emavg = 0.0f;

float sense_batt_neg_v = 0.0f;
float sense_batt_neg_v_scale = 0.0f;
float sense_batt_neg_v_offset = 0.0f;
float sense_batt_neg_v_emavg = 0.0f;

//Batt Temp
float sense_batt_t = 0.0f;
float sense_batt_t_scale = 0.0f;
float sense_batt_t_offset = 0.0f;
float sense_batt_t_emavg = 0.0f;

//Internal System Power
float sense_sys_pw_p_15va = 0.0f;
float sense_sys_pw_p_15va_scale = 0.0f;
float sense_sys_pw_p_5va = 0.0f;
float sense_sys_pw_p_5va_scale = 0.0f;
float sense_sys_pw_12vd = 0.0f;
float sense_sys_pw_12vd_scale = 0.0f;

//Temperature
float sense_temp_out_1 = 0.0f;
float sense_temp_out_2 = 0.0f;
float sense_temp_out_3 = 0.0f;
float sense_temp_out_4 = 0.0f;
float sense_temp_out_5 = 0.0f;
float sense_temp_board_1 = 0.0f;
float sense_temp_board_2 = 0.0f;
float sense_temp_board_3 = 0.0f;

float sense_temp_out_1_emavg = 0.0f;
float sense_temp_out_2_emavg = 0.0f;
float sense_temp_out_3_emavg = 0.0f;
float sense_temp_out_4_emavg = 0.0f;
float sense_temp_out_5_emavg = 0.0f;
float sense_temp_board_1_emavg = 0.0f;
float sense_temp_board_2_emavg = 0.0f;
float sense_temp_board_3_emavg = 0.0f;

//Fan Current
float sense_fan_current = 0.0f;
float sense_fan_current_scale = 0.0f;
float sense_fan_current_offset = 0.0f;

//Contact Resistance
float contact_resistance_emavg[50] = { 0.0f, };
float contact_resistance_law[50] = { 0.0f, };
float avg_contact_resistance = 0.0;
float sum_contact_resistance = 0.0;
float contact_resistance = 0.0;
float sum = 0.0;

int16_t sense_temp_dsp = 0;

uint16_t reset_offset_seq = 0;
float reset_offset_buf[100];
float reset_offset_sum;
uint16_t reset_offset_buf_idx = 0;
float result = 0.0;

Uint16 stop_flag = 0;
Uint16 contact_resistance_calculate_flag = 0;

void app_InitSense(void)
{
    sense_dc_bb1_i_offset           = 0.0  ;
    sense_dc_bb1_i_scale            = 20.58313179f;     // Out amps 1
    sense_dc_bb1_i_scale_charge     = 20.58313179f;     // Out amps 1
    sense_dc_bb1_i_scale_discharge  = 20.58313179f;     // Out amps 1

    sense_dc_bb2_i_offset           = 0.0;
    sense_dc_bb2_i_scale            = 20.58313179f;     // Out amps 2
    sense_dc_bb2_i_scale_charge     = 20.58313179f;     // Out amps 1
    sense_dc_bb2_i_scale_discharge  = 20.58313179f;     // Out amps 1

    sense_batt_v_scale              = 0.0000915527f;    // Bat volts

    sense_sys_pw_p_15va_scale       = 6.0f;             // +15VA
    sense_sys_pw_p_5va_scale        = 2.0f;             // +5VA
    sense_sys_pw_12vd_scale         = 5.0f;             // +12VD

    sense_dc_link_pri_f_v_scale     = 138.8424908f;     // 384V Front
    sense_dc_link_pri_r_v_scale     = 138.8424908f;     // 384V Rear
    sense_dc_link_sec_v_scale       = 4.527131783f;     // Converter 12V
    sense_dc_out_cap_v_scale        = 4.056497175f;     // 5V OUT CAP
    sense_batt_neg_v                = 1.0f;             // Vbat NEG

    sense_fan_current_scale         = 1.0f;
    sense_fan_current_offset        = 0.0f;

    sense_batt_t_scale              = 1.0f;             // Bat Temp
}

/*
 * Run in timer, CPU1
 */
void app_ConvDataDspAdcMain(void)
{
    static Uint16 array_cnt = 0;
// change to real scale
    if (operation.rx.cmd == can_rx_cmd_stop || operation.rx.cmd == can_rx_cmd_cha) {             // Rest, Charge Operation
        sense_dc_bb1_i_scale_charge = scale_select1(Current_Charge_Scale1, 5.0);
        sense_dc_bb1_i = (gs_cpu2_adc_dsp_raw_v[dsp_adc_out1_amps] - sense_dc_bb1_i_offset) * sense_dc_bb1_i_scale_charge;
        sense_dc_bb2_i_scale_charge = scale_select2(Current_Charge_Scale2, 5.0);
        sense_dc_bb2_i = (gs_cpu2_adc_dsp_raw_v[dsp_adc_out2_amps] - sense_dc_bb2_i_offset) * sense_dc_bb2_i_scale_charge;
    } else {        // Discharge Operation
        sense_dc_bb1_i_scale_discharge = scale_select1(Current_Discharge_Scale1, 5.0);
        sense_dc_bb1_i = (gs_cpu2_adc_dsp_raw_v[dsp_adc_out1_amps] - sense_dc_bb1_i_offset) * sense_dc_bb1_i_scale_discharge;
        sense_dc_bb2_i_scale_discharge = scale_select2(Current_Discharge_Scale2, 5.0);
        sense_dc_bb2_i = (gs_cpu2_adc_dsp_raw_v[dsp_adc_out2_amps] - sense_dc_bb2_i_offset) * sense_dc_bb2_i_scale_discharge;
    }
    sense_batt_v = (gs_cpu2_adc_dsp_raw_v[dsp_adc_batt_volts] - sense_batt_v_offset) * sense_batt_v_scale;

    sense_dc_link_pri_f_v   = (gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_pri_f] - sense_dc_link_pri_f_v_offset) * sense_dc_link_pri_f_v_scale;  // 384V front
    sense_dc_link_pri_r_v   = (gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_pri_r] - sense_dc_link_pri_r_v_offset) * sense_dc_link_pri_r_v_scale;  // 384V rear
    sense_dc_link_sec_v     = (gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_sec] - sense_dc_link_sec_v_offset) * sense_dc_link_sec_v_scale;      // Vicor 12V out
    sense_dc_out_cap_v      = (gs_cpu2_adc_dsp_raw_v[dsp_adc_out_cap_volts] - sense_dc_out_cap_v_offset) * sense_dc_out_cap_v_scale;       // 5V Output Cap
    sense_batt_neg_v        = (gs_cpu2_adc_dsp_raw_v[dsp_adc_batt_neg_volts] - sense_batt_neg_v_offset) * sense_batt_neg_v_scale;           // Reverse Bat

    sense_temp_dsp = ADC_getTemperatureC((uint16_t) gs_cpu2_adc_dsp_raw[dsp_adc_dsp_tempsense], (float32_t) 3.0f);

    calibration_run();

    EMAVG_MACRO(sense_dc_bb1_i, sense_dc_bb1_i_emavg, 0.03);            // for Control
    EMAVG_MACRO(sense_dc_bb2_i, sense_dc_bb2_i_emavg, 0.03);            // for Control
    calc_dc_out_i_emavg = sense_dc_bb1_i_emavg + sense_dc_bb2_i_emavg;
    EMAVG_MACRO(calc_dc_out_i_emavg, UI_calc_dc_out_i_emavg, 0.0005);   // for UI

    EMAVG_MACRO(sense_batt_v_cal, sense_batt_v_emavg, 0.02);            // for Control
    EMAVG_MACRO(sense_batt_v_cal, UI_sense_batt_v_emavg, 0.0005);       // for UI

    EMAVG_MACRO(sense_dc_link_pri_f_v, sense_dc_link_pri_f_v_emavg, 0.001);
    EMAVG_MACRO(sense_dc_link_pri_r_v, sense_dc_link_pri_r_v_emavg, 0.001);
    EMAVG_MACRO(sense_dc_link_sec_v, sense_dc_link_sec_v_emavg, 0.001);
    EMAVG_MACRO(sense_dc_out_cap_v_cal, sense_dc_out_cap_v_emavg, 0.02);	    // for Control
    EMAVG_MACRO(sense_dc_out_cap_v_cal, UI_sense_dc_out_cap_v_emavg, 0.0005);	// for UI	
    EMAVG_MACRO(sense_batt_neg_v, sense_batt_neg_v_emavg, 0.001);

    EMAVG_MACRO(gs_cpu2_adc_dsp_raw_v[dsp_adc_out1_amps], sense_dc_bb1_i_raw_v_emavg, 0.001);
    EMAVG_MACRO(gs_cpu2_adc_dsp_raw_v[dsp_adc_out2_amps], sense_dc_bb2_i_raw_v_emavg, 0.001);

    operation.op.sensing.s_vol = UI_sense_batt_v_emavg;
    operation.op.sensing.s_curr = UI_calc_dc_out_i_emavg;

    if (operation.rx.cmd == can_rx_cmd_contact_resistance_measure) {
        contact_resistance = (sense_dc_out_cap_v_emavg - sense_batt_v_emavg) / calc_dc_out_i_emavg;

        contact_resistance_emavg[array_cnt] = contact_resistance;
        contact_resistance_law[array_cnt] = (UI_sense_dc_out_cap_v_emavg - UI_sense_batt_v_emavg) / UI_calc_dc_out_i_emavg;

        sum_contact_resistance += contact_resistance;
        array_cnt++;
        if (array_cnt >= 50) {
            sum = sum_contact_resistance;
            avg_contact_resistance = sum_contact_resistance / 50.0;
            sum_contact_resistance = 0.0;

            array_cnt = 0;
        }
    }
}

void app_ConvDataDspAdcSub(void)
{
//change to real scale
    sense_sys_pw_p_15va = gs_cpu2_adc_dsp_raw_v[dsp_adc_15va] * sense_sys_pw_p_15va_scale;
    sense_sys_pw_p_5va  = gs_cpu2_adc_dsp_raw_v[dsp_adc_5va] * sense_sys_pw_p_5va_scale;
    sense_sys_pw_12vd   = gs_cpu2_adc_dsp_raw_v[dsp_adc_12vd] * sense_sys_pw_12vd_scale;

    sense_fan_current   = (gs_cpu2_adc_dsp_raw_v[dsp_adc_ext_diff] - sense_fan_current_offset) * sense_fan_current_scale;
}

void app_ConvDataExternalAdc(void)
{
    sense_temp_out_4    = util_ConvTemperature(5000, gs_cpu2_adc_ad7606_raw_v[3], 3.3f, 3411, 298.15);
    sense_temp_out_5    = util_ConvTemperature(5000, gs_cpu2_adc_ad7606_raw_v[4], 3.3f, 3411, 298.15);
    sense_temp_board_1  = util_MapFloat(gs_cpu2_adc_ad7606_raw_v[5], -0.3f, 3.6f, -10.0f, 125.0f);
    sense_temp_board_2  = util_MapFloat(gs_cpu2_adc_ad7606_raw_v[6], -0.3f, 3.6f, -10.0f, 125.0f);
    sense_temp_board_3  = util_MapFloat(gs_cpu2_adc_ad7606_raw_v[7], -0.3f, 3.6f, -10.0f, 125.0f);
}

void app_GetDspAdcMain(void)
{
    drv_GetFullSeqDspAdcMain(cpu2_adc_dsp_raw);

    //convert raw to voltage
    gs_cpu2_adc_dsp_raw_v[dsp_adc_out1_amps]        = util_AdcConvBipolarUnsign(cpu2_adc_dsp_raw[dsp_adc_out1_amps], 3.0f, ADC_16BIT_FULL);     //Out amps 1
    gs_cpu2_adc_dsp_raw_v[dsp_adc_batt_temp]        = cpu2_adc_dsp_raw[dsp_adc_batt_temp];   //Bat Temp

    gs_cpu2_adc_dsp_raw_v[dsp_adc_out2_amps]        = util_AdcConvBipolarUnsign(cpu2_adc_dsp_raw[dsp_adc_out2_amps], 3.0f, ADC_16BIT_FULL);     //Out amps 2
    gs_cpu2_adc_dsp_raw_v[dsp_adc_batt_volts]       = cpu2_adc_dsp_raw[dsp_adc_batt_volts];   //Bat volts

    gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_pri_f]    = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_dc_link_pri_f], 3.0f, ADC_12BIT_FULL);      //384V Front
    gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_pri_r]    = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_dc_link_pri_r], 3.0f, ADC_12BIT_FULL);      //384V Rear
    gs_cpu2_adc_dsp_raw_v[dsp_adc_dc_link_sec]      = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_dc_link_sec], 3.0f, ADC_12BIT_FULL);        //Converter 12V
    gs_cpu2_adc_dsp_raw_v[dsp_adc_out_cap_volts]    = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_out_cap_volts], 3.0f, ADC_12BIT_FULL);      //5V OUT CAP
    gs_cpu2_adc_dsp_raw_v[dsp_adc_batt_neg_volts]   = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_batt_neg_volts], 3.0f, ADC_12BIT_FULL);     //Vbat NEG

    gs_cpu2_adc_dsp_raw[dsp_adc_dsp_tempsense]      = cpu2_adc_dsp_raw[dsp_adc_dsp_tempsense];
    gs_cpu2_adc_dsp_raw[dsp_adc_batt_temp]          = cpu2_adc_dsp_raw[dsp_adc_batt_temp];
}

void app_GetDspAdcSub(void)
{
    drv_GetFullSeqDspAdcSub(cpu2_adc_dsp_raw);

//convert raw to voltage
    gs_cpu2_adc_dsp_raw_v[dsp_adc_15va]             = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_15va], 3.0f, ADC_12BIT_FULL);       //+15VA
    gs_cpu2_adc_dsp_raw_v[dsp_adc_5va]              = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_5va], 3.0f, ADC_12BIT_FULL);        //+5VA
    gs_cpu2_adc_dsp_raw_v[dsp_adc_12vd]             = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_12vd], 3.0f, ADC_12BIT_FULL);       //+12VD

    gs_cpu2_adc_dsp_raw_v[dsp_adc_ext1_single]      = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_ext1_single], 3.0f, ADC_12BIT_FULL);
    gs_cpu2_adc_dsp_raw_v[dsp_adc_ext2_single]      = util_AdcConvUnipolar(cpu2_adc_dsp_raw[dsp_adc_ext2_single], 3.0f, ADC_12BIT_FULL);
}

void app_GetExternalAdc(void)
{
    bsp_GetFulSeqAD7606(cpu2_adc_ad7606_raw);

//convert raw to voltage
    gs_cpu2_adc_ad7606_raw_v[0] = cpu2_adc_ad7606_raw[0];
    gs_cpu2_adc_ad7606_raw_v[1] = cpu2_adc_ad7606_raw[1];
    gs_cpu2_adc_ad7606_raw_v[2] = cpu2_adc_ad7606_raw[2];
    gs_cpu2_adc_ad7606_raw_v[3] = util_AdcConvBipolarOffset(cpu2_adc_ad7606_raw[3], AD7606_DIV2_LSB, 16);
    gs_cpu2_adc_ad7606_raw_v[4] = util_AdcConvBipolarOffset(cpu2_adc_ad7606_raw[4], AD7606_DIV2_LSB, 16);
    gs_cpu2_adc_ad7606_raw_v[5] = util_AdcConvBipolarOffset(cpu2_adc_ad7606_raw[5], AD7606_DIV2_LSB, 16);
    gs_cpu2_adc_ad7606_raw_v[6] = util_AdcConvBipolarOffset(cpu2_adc_ad7606_raw[6], AD7606_DIV2_LSB, 16);
    gs_cpu2_adc_ad7606_raw_v[7] = util_AdcConvBipolarOffset(cpu2_adc_ad7606_raw[7], AD7606_DIV2_LSB, 16);
}

float RootMeanSquare(float *f_Source, unsigned ui_Length)
{
    double d_SumValue = 0.0;
    double d_Result = 0.0;
    double d_Sing = 0.0;

    unsigned int ui_i = 0;

    for (ui_i = 0; ui_i < ui_Length; ui_i++) {
        d_SumValue += (f_Source[ui_i] * f_Source[ui_i]);
        d_Sing += f_Source[ui_i];
    }
    d_Result = sqrt(d_SumValue / ui_Length);
    d_Result *= ((d_Sing >= 0) ? 1 : -1);
    return ((float) (d_Result));
}

float ThermistorCONV(float *buf, unsigned length)
{
    Uint16 i;
    float offset = 0;
    float return_val = 0;
    float rms_val = 0;

    rms_val = RootMeanSquare(buf, length);
    for (i = 0; i < length; i++) {
        if (rms_val < thermal_table[i]) break;
    }
    if (rms_val == thermal_table[i - 1]) {
        offset = 5.0;
    } else {
        offset = (5.0 * (rms_val - thermal_table[i - 1])) / (thermal_table[i] - thermal_table[i - 1]);
    }
    return_val = -20.0 + ((float) (i - 1) * 5.0) + offset;

    return return_val;
}

void app_ConvThermal(void)
{
    static Uint16 index_temp = 0;
    float temp_f = 0;

    battary_t_buf_val[index_temp] = gs_cpu2_adc_dsp_raw[dsp_adc_batt_temp];
    index_temp++;

    if (index_temp > SAMPLING_MAX_DATA) {
        index_temp = 0;
        sense_batt_t = ThermistorCONV(battary_t_buf_val, SAMPLING_MAX_DATA);
        operation.op.sensing.s_bat_t = sense_batt_t;
    }

    temp_f = gs_cpu2_adc_ad7606_raw_v[0] * 0.0001525925;                    //converted to voltage
    temp_f = ((5000 * temp_f) / (3.3 - temp_f)) / 10000.0;                  //converted to resister
    temp_f = (1 / (0.003354017 + (0.0002911208 * logf(temp_f)))) - 273.15;  //converted to temperature
    sense_temp_out_1 += (temp_f - sense_temp_out_1) * 0.2;                  //filtering

    temp_f = gs_cpu2_adc_ad7606_raw_v[1] * 0.0001525925;
    temp_f = ((5000 * temp_f) / (3.3 - temp_f)) / 10000.0;
    temp_f = (1 / (0.003354017 + (0.0002911208 * logf(temp_f)))) - 273.15;
    sense_temp_out_2 += (temp_f - sense_temp_out_2) * 0.2;

    temp_f = gs_cpu2_adc_ad7606_raw_v[2] * 0.0001525925;
    temp_f = ((5000 * temp_f) / (3.3 - temp_f)) / 10000.0;
    temp_f = (1 / (0.003354017 + (0.0002911208 * logf(temp_f)))) - 273.15;
    sense_temp_out_3 += (temp_f - sense_temp_out_3) * 0.2;
}

float scale_select1(float32 *CAL_Scale_Data, float32 Unit)
{
    Uint16 ArrayAddressValue = 0;
    float current = 0.0;
    float result = 0.0;

    if (operation.op.cv_mode_flag == TRUE) {    // CV
        current = sense_dc_bb1_i_emavg;         // ex 4.5A
    } else {    // CC
        current = (current_ref / 2.0);
        current = current + 0.1;
    }
    ArrayAddressValue = (Uint16) (current / Unit);
    result = CAL_Scale_Data[ArrayAddressValue];

    return (result);
}

float scale_select2(float32 *CAL_Scale_Data, float32 Unit)
{
    Uint16 ArrayAddressValue = 0;
    float current = 0.0;
    float result = 0.0;

    if (operation.op.cv_mode_flag == TRUE) {    // CV
        current = sense_dc_bb2_i_emavg;        // ex 4.5A
    } else {        // CC
        current = (current_ref / 2.0);
        current = current + 0.1;
    }
    ArrayAddressValue = (Uint16) (current / Unit);
    result = CAL_Scale_Data[ArrayAddressValue];

    return (result);
}

//
// End of File
//