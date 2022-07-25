/*
 * app_ctrl_power.c
 *
 *  Created on: 2022. 1. 5.
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "app_ctrl_dcdc.h"

#include "app_comm.h"
#include "app_sense.h"
#include "app_op.h"
#include "app_settings.h"

#include "app_ctrl_power.h"
#include "bsp_io.h"
#include "drv_pwm.h"
#include "drv_timer.h"

#include "app_safety.h"
#include "app_calibration.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(app_CtrlPower, ".TI.ramfunc");
#endif
#endif

///////// BUCK Control Part ////////////////////////////////////////////////////////////////////////////////////////////////////////////
float V_BUCK_ref = 0.;
float Err_BUCK_V = 0., Up_BUCK_V = 0., Ui_BUCK_V = 0., Kp_BUCK_V = 6.25, Ki_BUCK_V = 10000., Ka_BUCK_V = 0.16;
float Kp_BUCK_V_PRE = 3.25, Ki_BUCK_V_PRE = 5000;
float I_BUCK_ref = 0., I_BUCK_ref_1 = 0., I_BUCK_ref_2 = 0.;
float Err_BUCK_I_1 = 0., Up_BUCK_I_1 = 0., Ui_BUCK_I_1 = 0., Kp_BUCK_I_1 = 0.0005, Ki_BUCK_I_1 = 1., Ka_BUCK_I = 2000., D_BUCK_1 = 0.;
float Ki_BUCK_I_1_PRE = 1., Kp_BUCK_I_1_PRE = 0.0005, Ka_BUCK_I_PRE = 2000;
float Err_BUCK_I_2 = 0., Up_BUCK_I_2 = 0., Ui_BUCK_I_2 = 0., D_BUCK_2 = 0.;
float I_BUCK_comp = 0., I_BUCK_comp2 = 0.;
float D_BUCK_1_comp = 0., D_BUCK_1_comp2 = 0., D_BUCK_2_comp = 0., D_BUCK_2_comp2 = 0.;
float ANTI_BUCK_V = 0.;
float ANTI_BUCK_I_1 = 0., ANTI_BUCK_I_2 = 0.;

float Del_t = ((float) DSP_TIMER0_TIME * 1.e-6);  // 0.000032 Sampling Time(31.25kHz)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern float sense_batt_v_emavg;
extern float sense_dc_bb1_i_emavg;
extern float sense_dc_bb2_i_emavg;
extern float sense_dc_out_cap_v_emavg;

extern float sense_batt_v;
extern float sense_dc_bb1_i;
extern float sense_dc_bb2_i;
extern float sense_dc_out_cap_v;

extern uint16_t fault;

float voltage_ref = 0;
float current_ref = 0;
float g_debug_target_voltage = 3.5;
float g_debug_target_current = 0.1;

uint16_t g_debug_start_flag = 0;
uint16_t cv_mode = 0;

c_d_mode charge_discharge_mode = charge_mode;

Uint16 controlseqeunse_cnt = 0;
extern float set_pu_dc_link_pri_volts;
Uint16 controlseqeunse_cnt_new = 0;
extern AppCommCanOpHandle operation;
extern float sense_dc_link_pri_f_v_emavg;

float cv_voltage_margin = 0.0015;

Uint16 *cnt_control;
Uint16 controlseqeunse_cnt1 = 0;
Uint16 controlseqeunse_cnt2 = 0;
Uint16 pre_cnt = 10;
extern float set_calc_output_current_max;

appFormationSeq seq_mode = no_operation;
appFormationSeq seq_mode_old = no_operation;

float start_current = 4.0;  // After calibration

Uint16 precharge_end_cnt = 0;
Uint16 target_time = 1;
Uint16 charge_cv_voltage_check_time = 100;
Uint16 discharge_cv_voltage_check_time = 100;
float dischar_volt = 0.0;
float char_volt = 0.0;
float increase_vol_value = 0.001;

void app_CtrlPower(void)
{
    app_ConvDataDspAdcMain();
    app_controlseqeunse();

    if (seq_mode == output_precharge) {
        buck_boost_control_precharge(voltage_ref, current_ref);
    } else if (seq_mode == operation_control || seq_mode == output_precharge_end || seq_mode == control_idle) {
        buck_boost_control(charge_discharge_mode, voltage_ref, current_ref);
    }
#if(DEBUGGING == 1)
    else if (seq_mode >= output_mosfet_on && seq_mode <= basic_charge)
    {
        buck_boost_control(charge_discharge_mode, voltage_ref, current_ref);
    }

    if(g_debug_test_flag == 1) {            // ��Ƽ �׽�Ʈ��
        drv_SetDCDCPWMDuty(g_debug_test_CMPA, g_debug_test_CMPA, 1);      // HR_PWM
        drv_EnablePWM();
    }
    if(g_debug_test_flag == 2) {            // ��Ƽ �׽�Ʈ��
        bsp_IoEnableOut(1); 				// Secondary MOSFET On
    }
#endif
}

void app_controlseqeunse(void)
{
    switch (seq_mode)
    {
        case output_precharge:
            if ((sense_dc_out_cap_v_emavg >= (sense_batt_v_emavg * 0.95))) {        // 95%
                current_ref = 0.001;
                precharge_end_cnt++;
                if (precharge_end_cnt >= CONTROL_SEQUENCE_MS_COUNT(target_time)) {
                    seq_mode = output_precharge_end;
                }
            } else {
                voltage_ref = sense_batt_v_emavg - 0.005;
                current_ref = start_current;
                precharge_end_cnt = 0;
                drv_EnablePWM();
            }
            break;

        case output_precharge_end:
            operation.op.power_stage.bat_connect_complete_flag = TRUE;
            bsp_sec_mosfet(1);  // Secondary MOSFET On
            voltage_ref = sense_batt_v_emavg;
            current_ref = 0.0;
            break;

        case control_idle:
            charge_discharge_mode = idle;
            voltage_ref = sense_batt_v_emavg;
            current_ref = 0.0;
            operation.op.cv_mode_flag = FALSE;
            break;

        case operation_control:
            if (operation.rx.op == can_rx_op_cha_cv || operation.rx.op == can_rx_op_dis_cv) {
                appCvModeCheck(operation.rx.vol, cv_voltage_margin);
            } else {
                operation.op.cv_mode_flag = FALSE;
            }
            break;

#if(DEBUGGING == 1)
    case basic_discharge:   // Discharge
        charge_discharge_mode = 0;     // Discharge;
        voltage_ref = g_debug_target_voltage;        //
        current_ref = g_debug_target_current;        //
        break;

    case basic_charge:      // Charge
        charge_discharge_mode = 1;     // Charge;
        voltage_ref = g_debug_target_voltage;        //
        current_ref = g_debug_target_current;        //
        break;
#endif
        default:
            break;
    }
}

void appCvModeCheck(uint32_t voltage, float margin)
{
    static Uint16 cv_cnt = 0;

    if (charge_discharge_mode == charge_mode) {
        char_volt = ((float) voltage / 1000.0) - margin;
        if (sense_batt_v_emavg >= char_volt) {
            cv_cnt++;
            if (cv_cnt >= charge_cv_voltage_check_time) {
                operation.op.cv_mode_flag = TRUE;
                cv_cnt = 0;
            }
        }
    } else if (charge_discharge_mode == discharge_mode) {
        dischar_volt = ((float) voltage / 1000.0) + margin;
        if (sense_batt_v_emavg <= dischar_volt) {
            cv_cnt++;
            if (cv_cnt >= discharge_cv_voltage_check_time) {
                operation.op.cv_mode_flag = TRUE;
                cv_cnt = 0;
            }
        }
    }
}

void buck_boost_control(c_d_mode char_dischar, float vol_ref, float cur_ref)
{
    if (vol_ref >= 5.5) vol_ref = 5.5;
    if (cur_ref >= 105.0) cur_ref = 105.0;

    if ((char_dischar == idle) || (char_dischar == charge_mode)) {
///////////////////// Voltage Control /////////////////////////////////////////////////////////
        V_BUCK_ref += 0.001;
        if (V_BUCK_ref >= vol_ref) V_BUCK_ref = vol_ref;

        Err_BUCK_V = V_BUCK_ref - sense_batt_v_emavg;

        Up_BUCK_V = Kp_BUCK_V * Err_BUCK_V;
        Ui_BUCK_V = Ui_BUCK_V + Ki_BUCK_V * (Err_BUCK_V - ANTI_BUCK_V * Ka_BUCK_V) * Del_t;

        I_BUCK_comp = Up_BUCK_V + Ui_BUCK_V;
        I_BUCK_comp2 = (I_BUCK_comp <= 0) ? 0. : (I_BUCK_comp >= cur_ref) ? cur_ref : I_BUCK_comp;

        ANTI_BUCK_V = I_BUCK_comp - I_BUCK_comp2;
        I_BUCK_ref = I_BUCK_comp2;
///////////////////// Current Control /////////////////////////////////////////////////////////
        I_BUCK_ref_1 = I_BUCK_ref / 2;
        I_BUCK_ref_2 = I_BUCK_ref / 2;

        Err_BUCK_I_1 = I_BUCK_ref_1 - sense_dc_bb1_i_emavg;

        Up_BUCK_I_1 = Kp_BUCK_I_1 * Err_BUCK_I_1;
        Ui_BUCK_I_1 = Ui_BUCK_I_1 + Ki_BUCK_I_1 * (Err_BUCK_I_1 - ANTI_BUCK_I_1 * Ka_BUCK_I) * Del_t;

        D_BUCK_1_comp = Up_BUCK_I_1 + Ui_BUCK_I_1;
        D_BUCK_1_comp2 = (D_BUCK_1_comp <= 0) ? 0. : (D_BUCK_1_comp >= 0.9) ? 0.9 : D_BUCK_1_comp;

        ANTI_BUCK_I_1 = D_BUCK_1_comp - D_BUCK_1_comp2;
        D_BUCK_1 = D_BUCK_1_comp2;

        Err_BUCK_I_2 = I_BUCK_ref_2 - sense_dc_bb2_i_emavg;

        Up_BUCK_I_2 = Kp_BUCK_I_1 * Err_BUCK_I_2;
        Ui_BUCK_I_2 = Ui_BUCK_I_2 + Ki_BUCK_I_1 * (Err_BUCK_I_2 - ANTI_BUCK_I_2 * Ka_BUCK_I) * Del_t;

        D_BUCK_2_comp = Up_BUCK_I_2 + Ui_BUCK_I_2;
        D_BUCK_2_comp2 = (D_BUCK_2_comp <= 0) ? 0. : (D_BUCK_2_comp >= 0.9) ? 0.9 : D_BUCK_2_comp;

        ANTI_BUCK_I_2 = D_BUCK_2_comp - D_BUCK_2_comp2;
        D_BUCK_2 = D_BUCK_2_comp2;

        drv_SetDCDCPWMDuty(D_BUCK_1, D_BUCK_2, 1);      // HR_PWM
    } else   // char_dischar = 1;
    {
        if (cv_mode == 0) {
            I_BUCK_ref = (-1.0) * (cur_ref);  //CC Control
        } else {
///////////////////// Voltage Control /////////////////////////////////////////////////////////
            V_BUCK_ref -= 0.001;

            if (V_BUCK_ref <= vol_ref) V_BUCK_ref = vol_ref;

            Err_BUCK_V = -V_BUCK_ref + sense_batt_v_emavg;

            Up_BUCK_V = Kp_BUCK_V * Err_BUCK_V;
            Ui_BUCK_V = Ui_BUCK_V + Ki_BUCK_V * (Err_BUCK_V - ANTI_BUCK_V * Ka_BUCK_V) * Del_t;

            I_BUCK_comp = Up_BUCK_V + Ui_BUCK_V;
            I_BUCK_comp2 = (I_BUCK_comp <= 0) ? 0. : (I_BUCK_comp >= cur_ref) ? cur_ref : I_BUCK_comp;  // ���� ���� ���� ��� ����

            ANTI_BUCK_V = I_BUCK_comp - I_BUCK_comp2;
            I_BUCK_ref = I_BUCK_comp2;
            I_BUCK_ref = (-1.0) * I_BUCK_ref;
        }
///////////////////// Current Control /////////////////////////////////////////////////////////
        I_BUCK_ref_1 = I_BUCK_ref / 2;
        I_BUCK_ref_2 = I_BUCK_ref / 2;

        Err_BUCK_I_1 = I_BUCK_ref_1 - sense_dc_bb1_i_emavg;

        Up_BUCK_I_1 = Kp_BUCK_I_1 * Err_BUCK_I_1;
        Ui_BUCK_I_1 = Ui_BUCK_I_1 + Ki_BUCK_I_1 * (Err_BUCK_I_1 - ANTI_BUCK_I_1 * Ka_BUCK_I) * Del_t;

        D_BUCK_1_comp = Up_BUCK_I_1 + Ui_BUCK_I_1;
        D_BUCK_1_comp2 = (D_BUCK_1_comp <= -0.9) ? -0.9 : (D_BUCK_1_comp >= 0.9) ? 0.9 : D_BUCK_1_comp;

        ANTI_BUCK_I_1 = D_BUCK_1_comp - D_BUCK_1_comp2;

        if (D_BUCK_1_comp >= 0.0) {
            D_BUCK_1 = D_BUCK_1_comp2;
        } else {
            D_BUCK_1 = 1 + D_BUCK_1_comp2;
        }

        Err_BUCK_I_2 = I_BUCK_ref_2 - sense_dc_bb2_i_emavg;

        Up_BUCK_I_2 = Kp_BUCK_I_1 * Err_BUCK_I_2;
        Ui_BUCK_I_2 = Ui_BUCK_I_2 + Ki_BUCK_I_1 * (Err_BUCK_I_2 - ANTI_BUCK_I_2 * Ka_BUCK_I) * Del_t;

        D_BUCK_2_comp = Up_BUCK_I_2 + Ui_BUCK_I_2;
        D_BUCK_2_comp2 = (D_BUCK_2_comp <= -0.9) ? -0.9 : (D_BUCK_2_comp >= 0.9) ? 0.9 : D_BUCK_2_comp;

        ANTI_BUCK_I_2 = D_BUCK_2_comp - D_BUCK_2_comp2;

        if (D_BUCK_2_comp2 >= 0.0) {
            D_BUCK_2 = D_BUCK_2_comp2;
        } else {
            D_BUCK_2 = 1 + D_BUCK_2_comp2;
        }

        drv_SetDCDCPWMDuty(D_BUCK_1, D_BUCK_2, 1);
    }
}

void buck_boost_control_precharge(float vol_ref, float cur_ref)
{
    if (vol_ref >= 5.5) vol_ref = 5.5;
    if (cur_ref >= 15.0) cur_ref = 15.0;
///////////////////// Voltage Control /////////////////////////////////////////////////////////
    pre_cnt++;
    if (pre_cnt >= 10) {
        V_BUCK_ref += increase_vol_value;
        pre_cnt = 0;
    }
    if (V_BUCK_ref >= vol_ref) V_BUCK_ref = vol_ref;

    Err_BUCK_V = V_BUCK_ref - sense_dc_out_cap_v_cal;

    Up_BUCK_V = Kp_BUCK_V_PRE * Err_BUCK_V;
    Ui_BUCK_V = Ui_BUCK_V + Ki_BUCK_V_PRE * (Err_BUCK_V - ANTI_BUCK_V * Ka_BUCK_V) * Del_t;

    I_BUCK_comp = Up_BUCK_V + Ui_BUCK_V;
    I_BUCK_comp2 = (I_BUCK_comp <= 0) ? 0. : (I_BUCK_comp >= cur_ref) ? cur_ref : I_BUCK_comp;  // ���� ���� ���� ��� ����

    ANTI_BUCK_V = I_BUCK_comp - I_BUCK_comp2;
    I_BUCK_ref = I_BUCK_comp2;

///////////////////// Current Control /////////////////////////////////////////////////////////
    I_BUCK_ref_1 = I_BUCK_ref / 2;
    I_BUCK_ref_2 = I_BUCK_ref / 2;

    Err_BUCK_I_1 = I_BUCK_ref_1 - sense_dc_bb1_i_emavg;

    Up_BUCK_I_1 = Kp_BUCK_I_1_PRE * Err_BUCK_I_1;
    Ui_BUCK_I_1 = Ui_BUCK_I_1 + Ki_BUCK_I_1_PRE * (Err_BUCK_I_1 - ANTI_BUCK_I_1 * Ka_BUCK_I_PRE) * Del_t;

    D_BUCK_1_comp = Up_BUCK_I_1 + Ui_BUCK_I_1;
    D_BUCK_1_comp2 = (D_BUCK_1_comp <= 0) ? 0. : (D_BUCK_1_comp >= 0.9) ? 0.9 : D_BUCK_1_comp;

    ANTI_BUCK_I_1 = D_BUCK_1_comp - D_BUCK_1_comp2;
    D_BUCK_1 = D_BUCK_1_comp2;

    Err_BUCK_I_2 = I_BUCK_ref_2 - sense_dc_bb2_i_emavg;

    Up_BUCK_I_2 = Kp_BUCK_I_1_PRE * Err_BUCK_I_2;
    Ui_BUCK_I_2 = Ui_BUCK_I_2 + Ki_BUCK_I_1_PRE * (Err_BUCK_I_2 - ANTI_BUCK_I_2 * Ka_BUCK_I_PRE) * Del_t;

    D_BUCK_2_comp = Up_BUCK_I_2 + Ui_BUCK_I_2;
    D_BUCK_2_comp2 = (D_BUCK_2_comp <= 0) ? 0. : (D_BUCK_2_comp >= 0.9) ? 0.9 : D_BUCK_2_comp;

    ANTI_BUCK_I_2 = D_BUCK_2_comp - D_BUCK_2_comp2;
    D_BUCK_2 = D_BUCK_2_comp2;

    drv_SetDCDCPWMDuty(D_BUCK_1, D_BUCK_2, 1);
}

void control_value_init(void)
{
    V_BUCK_ref = 0.;
    Err_BUCK_V = 0.;
    Up_BUCK_V = 0.;
    Ui_BUCK_V = 0.;
    Kp_BUCK_V = 6.25;
    Ki_BUCK_V = 10000.;
    Ka_BUCK_V = 0.16;

    I_BUCK_ref = 0.;
    I_BUCK_ref_1 = 0.;
    I_BUCK_ref_2 = 0.;
    Err_BUCK_I_1 = 0.;
    Up_BUCK_I_1 = 0.;
    Ui_BUCK_I_1 = 0.;
    Kp_BUCK_I_1 = 0.0005;
    Ki_BUCK_I_1 = 1.;
    Ka_BUCK_I = 2000.;
    D_BUCK_1 = 0.;
    Err_BUCK_I_2 = 0.;
    Up_BUCK_I_2 = 0.;
    Ui_BUCK_I_2 = 0.;
    D_BUCK_2 = 0.;
    I_BUCK_comp = 0.;
    I_BUCK_comp2 = 0.;
    D_BUCK_1_comp = 0.;
    D_BUCK_1_comp2 = 0.;
    D_BUCK_2_comp = 0.;
    D_BUCK_2_comp2 = 0.;
    ANTI_BUCK_V = 0.;
    ANTI_BUCK_I_1 = 0.;
    ANTI_BUCK_I_2 = 0.;

    pre_cnt = 10;
    char_volt = 0;
    dischar_volt = 0;
}

//
// End of File
//