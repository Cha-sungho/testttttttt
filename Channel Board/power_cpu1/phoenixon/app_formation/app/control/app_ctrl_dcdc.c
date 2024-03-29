/*
 * app_ctrl_dcdc.c
 *
 *  Created on: Nov 10, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

//Phoenixon
#include "drv_pwm.h"
#include "bsp_io.h"
#include "app_settings.h"
#include "app_ctrl_dcdc.h"
#include "app_sense.h"
#include "app_safety.h"
#include "app_constant.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(app_CtrlDCDC, ".TI.ramfunc");
#endif
#endif

#if 0
#define DCDC_DUTY_MIN    (0.02f)
#define DCDC_DUTY_MAX    (0.98f)
#else
float DCDC_DUTY_MIN = 0.015f;
float DCDC_DUTY_MAX = 0.985f;
#endif

//Gain
float gain_kpc = 0.0f;
float gain_kic = 0.0f;
float gain_kac = 0.0f;

//For DOF Control
float gain_dof_fcc20 = 0.0f;

float gain_dof_wcc = 0.0f;
float gain_dof_ktc = 0.0f;
float gain_dof_kic = 0.0f;
float gain_dof_kfc = 0.0f;
float gain_dof_kac = 0.0f;

float gain_dof_wcv = 0.0f;
float gain_dof_ktv = 0.0f;
float gain_dof_kiv = 0.0f;
float gain_dof_kfv = 0.0f;
float gain_dof_kav = 0.0f;

//Voltage, Current Control Parameter, feed-forward duty
float duty_ff = 0.0f;
float duty_ffw = 0.0f;

float i_top_ref = 0.0f;
float i_top_err = 0.0f;
float Upc_top = 0.0f;
float i_comp_top = 0.0f;
float Uic_top = 0.0f;
float ii_comp_top = 0.0f;
float anti_i_top_comp = 0.0f;

//For DOF Control
float D_Utc_top = 0.0f;
float D_Uic_top = 0.0f;
float D_Ufc_top = 0.0f;

float i_bottom_ref = 0.0f;
float i_bottom_err = 0.0f;
float Upc_bottom = 0.0f;
float i_comp_bottom = 0.0f;
float Uic_bottom = 0.0f;
float ii_comp_bottom = 0.0f;
float anti_i_bottom_comp = 0.0f;

//For DOF Control
float D_Utc_bottom = 0.0f;
float D_Uic_bottom = 0.0f;
float D_Ufc_bottom = 0.0f;

float i_out_inc_val = 0.01f;
float i_out_set = 0.0f;

//PWM parameter
float dcdc_duty_t = 0.0;
float dcdc_duty_b = 0.0;
float dcdc_duty_test = 0.0;

//Operate
uint16_t seq_dcdc_mode = 0;
uint32_t seq_dcdc_timer = 0;

uint16_t dcdc_pwm_flag = 0;
uint16_t dcdc_open_loop_test = 1;

static void app_UpdateDCDCGain(void);

static void app_UpdateDCDCGain(void)
{
    gain_dof_fcc20 = set_pu_ctrl_f / 20.0f;
    gain_dof_wcc = 2 * gain_dof_fcc20 * PI;
    gain_dof_ktc = gain_dof_wcc * (set_pu_out_L / set_pu_dc_link_sec_volts);
    gain_dof_kic = gain_dof_wcc * gain_dof_wcc * (set_pu_out_L / set_pu_dc_link_sec_volts);
    gain_dof_kfc = 2.0f * gain_dof_wcc * (set_pu_out_L / set_pu_dc_link_sec_volts);
    gain_dof_kac = 1.0f / gain_dof_ktc;

    gain_dof_wcv = gain_dof_wcc / 20.0f;
    gain_dof_ktv = gain_dof_wcv * set_pu_out_buck_C;
    gain_dof_kiv = gain_dof_wcv * gain_dof_wcv * set_pu_out_buck_C;
    gain_dof_kfv = 2.0f * gain_dof_wcv * set_pu_out_buck_C;
    gain_dof_kav = 1.0f / gain_dof_ktv;
}

#if 0
void app_CtrlDCDC(void)
{
    seq_dcdc_timer++;

    if (fault) seq_dcdc_mode = 0;

    switch (seq_dcdc_mode)
    {
        case 0:  //All stop
            drv_DisablePWM();
            bsp_IoEnableDCDCPWM(0);
            bsp_IoOnOutRly(0);
            app_UpdateDCDCGain();

            seq_dcdc_mode++;
            break;
        case 1:  //Stop idle
            app_UpdateDCDCGain();
            break;
        case 2:  //Relay ON
            bsp_IoOnOutRly(1);
            seq_dcdc_timer = 0;
            seq_dcdc_mode++;
            break;
        case 3:  //Relay ON wait, PWM ON
            if(seq_dcdc_timer >= 4000) {
                D_Uic_top = 0.0f;
                D_Uic_bottom = 0.0f;
                dcdc_pwm_flag = 1;
                seq_dcdc_mode++;
            }
            break;
        case 4:  //DCDC idle
            break;
    }

#if 0
    duty_ff = sense_dc_out_cap_v / sense_dc_link_sec_v;
    duty_ffw = (duty_ff <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (duty_ff >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : duty_ff;  // duty limiter

    i_top_ref = (i_out_set > i_top_ref) ? (i_top_ref + i_out_inc_val) : (i_out_set < i_top_ref) ? (i_top_ref - i_out_inc_val) : (i_top_ref);
    i_top_err = i_top_ref - sense_dc_inductor_h_i;
    Upc_top = i_top_err * gain_kpc;
    Uic_top += (i_top_err - (anti_i_top_comp * gain_kac)) * gain_kic * 0.00005;
    i_comp_top = Upc_top + Uic_top + duty_ffw;  //Feed-forward
    ii_comp_top = (i_comp_top <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_top >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_top;  // duty limiter
    anti_i_top_comp = i_comp_top - ii_comp_top;
    dcdc_duty_t = ii_comp_top;

    i_bottom_ref = (i_out_set > i_bottom_ref) ? (i_bottom_ref + i_out_inc_val) : (i_out_set < i_bottom_ref) ? (i_bottom_ref - i_out_inc_val) : (i_bottom_ref);
    i_bottom_err = i_bottom_ref - sense_dc_inductor_l_i;
    Upc_bottom = i_bottom_err * gain_kpc;
    Uic_bottom += (i_bottom_err - (anti_i_bottom_comp * gain_kac)) * gain_kic * 0.00005;
    i_comp_bottom = Upc_bottom + Uic_bottom + duty_ffw;    //Feed-forward
    ii_comp_bottom = (i_comp_bottom <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_bottom >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_bottom;  // duty limiter
    anti_i_bottom_comp = i_comp_bottom - ii_comp_bottom;
    dcdc_duty_b = ii_comp_bottom;
#endif

#if 0   //DOF control
    duty_ff = sense_dc_out_cap_v / sense_dc_link_sec_v;
    duty_ffw = (duty_ff <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (duty_ff >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : duty_ff;  // duty limiter

    i_top_ref = (i_out_set > i_top_ref) ? (i_top_ref + i_out_inc_val) : (i_out_set < i_top_ref) ? (i_top_ref - i_out_inc_val) : (i_top_ref);
    i_top_err = i_top_ref - sense_dc_inductor_h_i;
    D_Utc_top = i_top_ref * gain_dof_ktc;
    D_Uic_top += ((i_top_err - (anti_i_top_comp * gain_dof_kac)) * gain_dof_kic * set_pu_ctrl_t);
    D_Ufc_top = sense_dc_inductor_h_i * gain_dof_kfc;
    i_comp_top = D_Utc_top + D_Uic_top - D_Ufc_top + duty_ffw;
    ii_comp_top = (i_comp_top <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_top >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_top;  // duty limiter
    anti_i_top_comp = i_comp_top - ii_comp_top;
    dcdc_duty_t = ii_comp_top;

    i_bottom_ref = (i_out_set > i_bottom_ref) ? (i_bottom_ref + i_out_inc_val) : (i_out_set < i_bottom_ref) ? (i_bottom_ref - i_out_inc_val) : (i_bottom_ref);
    i_bottom_err = i_bottom_ref - sense_dc_inductor_l_i;
    D_Utc_bottom = i_bottom_ref * gain_dof_ktc;
    D_Uic_bottom += ((i_bottom_err - (anti_i_bottom_comp * gain_dof_kac)) * gain_dof_kic * set_pu_ctrl_t);
    D_Ufc_bottom = sense_dc_inductor_l_i * gain_dof_kfc;
    i_comp_bottom = D_Utc_bottom + D_Uic_bottom - D_Ufc_bottom + duty_ffw;
    ii_comp_bottom = (i_comp_bottom <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_bottom >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_bottom;  // duty limiter
    anti_i_bottom_comp = i_comp_bottom - ii_comp_bottom;
    dcdc_duty_b = ii_comp_bottom;
#endif

#if 1   //PID TECH
    duty_ff = sense_dc_out_cap_v / sense_dc_link_sec_v;
    duty_ffw = (duty_ff <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (duty_ff >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : duty_ff;  // duty limiter

    i_top_ref = (i_out_set > i_top_ref) ? (i_top_ref + i_out_inc_val) : (i_out_set < i_top_ref) ? (i_top_ref - i_out_inc_val) : (i_top_ref);
    i_top_err = i_top_ref - sense_dc_inductor_h_i;
    D_Utc_top = i_top_ref * gain_dof_ktc;
    D_Uic_top += ((i_top_err - (anti_i_top_comp * gain_dof_kac)) * gain_dof_kic * set_pu_ctrl_t);
    D_Ufc_top = sense_dc_inductor_h_i * gain_dof_kfc;
    i_comp_top = D_Utc_top + D_Uic_top - D_Ufc_top + duty_ffw;
    ii_comp_top = (i_comp_top <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_top >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_top;  // duty limiter
    anti_i_top_comp = i_comp_top - ii_comp_top;
    dcdc_duty_t = ii_comp_top;

    i_bottom_ref = (i_out_set > i_bottom_ref) ? (i_bottom_ref + i_out_inc_val) : (i_out_set < i_bottom_ref) ? (i_bottom_ref - i_out_inc_val) : (i_bottom_ref);
    i_bottom_err = i_bottom_ref - sense_dc_inductor_l_i;
    D_Utc_bottom = i_bottom_ref * gain_dof_ktc;
    D_Uic_bottom += ((i_bottom_err - (anti_i_bottom_comp * gain_dof_kac)) * gain_dof_kic * set_pu_ctrl_t);
    D_Ufc_bottom = sense_dc_inductor_l_i * gain_dof_kfc;
    i_comp_bottom = D_Utc_bottom + D_Uic_bottom - D_Ufc_bottom + duty_ffw;
    ii_comp_bottom = (i_comp_bottom <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (i_comp_bottom >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : i_comp_bottom;  // duty limiter
    anti_i_bottom_comp = i_comp_bottom - ii_comp_bottom;
    dcdc_duty_b = ii_comp_bottom;
#endif

#if 1   //DCDC control
    //XXX Final duty
    dcdc_duty_t = (dcdc_duty_t <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (dcdc_duty_t >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : dcdc_duty_t;  // duty limiter
    dcdc_duty_b = (dcdc_duty_b <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (dcdc_duty_b >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : dcdc_duty_b;  // duty limiter

    dcdc_duty_test = (dcdc_duty_test <= DCDC_DUTY_MIN) ? DCDC_DUTY_MIN : (dcdc_duty_test >= DCDC_DUTY_MAX) ? DCDC_DUTY_MAX : dcdc_duty_test;  // duty limiter

    if (dcdc_open_loop_test == 1) {
        drv_SetDCDCPWMDuty(dcdc_duty_test, dcdc_duty_test, 0);
    } else {
        drv_SetDCDCPWMDuty(dcdc_duty_t, dcdc_duty_b, 0);
    }

    // Because, the pwm register value is initially incorrect.
    if (seq_dcdc_mode == 4 && dcdc_pwm_flag == 2) {
        //bsp_IoEnableDCDCPWM(1);     // Second turn on transceiver
        dcdc_pwm_flag = 0;
    }

    if (seq_dcdc_mode == 4 && dcdc_pwm_flag == 1) {
        drv_EnablePWM();  // First turn on PWM
        dcdc_pwm_flag = 2;
    }
#endif
}

#endif

//
// End of File
//
