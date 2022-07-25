/*
 * app_safety.c
 *
 *  Created on: 2021. 12. 5.
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "bsp_io.h"
#include "app_settings.h"

#include "drv_pwm.h"

#include "app_ctrl_power.h"
#include "app_op.h"
#include "drv_timer.h"

#include "app_safety.h"
#include "app_constant.h"
#include "drv_can.h"

#include "app_calibration.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(app_RunCheckSafety, ".TI.ramfunc");
#endif
#endif

uint16_t fault_pri_in_f_volt_max   = 0;
uint16_t fault_pri_in_f_volt_min   = 0;
uint16_t fault_pri_in_r_volt_max   = 0;
uint16_t fault_pri_in_r_volt_min   = 0;
uint16_t fault_pri_out_volt_max    = 0;
uint16_t fault_pri_out_volt_min    = 0;
uint16_t fault_sec_out_volt_max    = 0;
uint16_t fault_sec_out_volt_min    = 0;
uint16_t fault_battery_volt_max    = 0;
uint16_t fault_battery_volt_min    = 0;
uint16_t fault_output_current1_max = 0;
uint16_t fault_output_current2_max = 0;

uint16_t emergency = 0;
uint16_t fault = 0;

extern float sense_dc_link_pri_v;
extern float sense_dc_link_sec_v;
extern float sense_dc_out_cap_v;

extern uint16_t seq_acdc_mode;
extern uint16_t seq_llc_mode;
extern uint16_t seq_dcdc_mode;

extern float sense_dc_link_pri_f_v_emavg;
extern float sense_dc_link_pri_r_v_emavg;
extern float sense_dc_link_sec_v_emavg;
extern float sense_dc_out_cap_v_emavg;
extern float sense_batt_v_emavg;
extern float sense_dc_bb1_i_emavg;
extern float sense_dc_bb2_i_emavg;
extern float sense_sys_pw_p_15va;
extern float sense_sys_pw_p_5va;
extern float sense_sys_pw_12vd;
extern float sense_fan_current;
extern float sense_batt_neg_v;
extern float sense_temp_dsp;
extern float sense_temp_out_1;
extern float sense_temp_out_2;
extern float sense_temp_out_3;
extern float sense_temp_board_1;
extern float sense_temp_board_2;
extern float sense_batt_t_emavg;
extern float calc_dc_out_i_emavg;

extern appFormationSeq seq_mode;
extern AppCommCanOpHandle operation;

Uint16 warning_alarm = 0;
Uint16 dc_alarm = 0;
Uint16 battery_alarm = 0;
Uint16 fault_alarm = 0;

extern float sense_dc_link_pri_v;
extern float sense_dc_link_sec_v;
extern float sense_dc_out_cap_v;

extern Uint16 cmd_error[8];

void app_EmoSwitchCheck(void)
{
    if (bsp_IoFaultEMO() == SET) {
        operation.op.alarm.fault.bit.Emergency_Switch_Operation = TRUE;
    }
}

void app_BoardPowerErrorCheck(void)
{
    if ((sense_sys_pw_p_15va >= 15.0 + 1.0) && (sense_sys_pw_p_15va <= 15.0 - 1.0)) {
        SetBit(fault_alarm, INTERNAL_POWER_SUPPLY_15VA_FAULT);
        operation.op.alarm.fault.bit.Internal_power_supply_15V_A_Fault = TRUE;
    }

    if ((sense_sys_pw_p_5va >= 5.0 + 1.0) && (sense_sys_pw_p_5va <= 5.0 - 1.0)) {
        SetBit(fault_alarm, INTERNAL_POWER_SUPPLY_5VA_FAULT);
        operation.op.alarm.fault.bit.Internal_power_supply_5V_A_Fault = TRUE;
    }

    if ((sense_sys_pw_12vd >= 12.0 + 1.0) && (sense_sys_pw_12vd <= 12.0 - 1.0)) {
        SetBit(fault_alarm, INTERNAL_POWER_SUPPLY_12VD_FAULT);
        operation.op.alarm.fault.bit.Internal_power_supply_12V_D_Fault = TRUE;
    }
}

void app_FanStopCheck(void)
{
    if ((sense_fan_current >= 0.0) && (sense_fan_current <= 0.0)) {
        SetBit(fault_alarm, INTERNAL_FAN_STOP);
        operation.op.alarm.fault.bit.Internal_Fan_stop = TRUE;
    }
}

void app_BatteryReverseCheck(AppCommCanOpHandle *op)
{
    static Uint16 check_cnt[BAT_REVERSE_INDEX] = { 0, };
    int i;

    if (op->op.power_stage.V384_on_complete_flag == TRUE) {
        if (sense_batt_neg_v >= 0.5) {        // 현재 회로상 배터리 역삽하면 0.75V 정도 센싱됨..
            check_cnt[0]++;
            if (check_cnt[0] > 10) {
                check_cnt[0] = 0;
                SetBit(warning_alarm, BATTERY_REVERSED);
                operation.op.alarm.warning.bit.Battery_Reversed = TRUE;
            }
        }
    } else {
        for (i = 0; i < BAT_REVERSE_INDEX; i++) {
            check_cnt[i] = 0;
        }
    }
}

void app_InnerTempCheck(void)
{
    if ((sense_temp_dsp >= 0.0) && (sense_temp_dsp <= 60.0)) {
        SetBit(fault_alarm, DSP_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.DSP_over_temperature = TRUE;
    }
    if ((sense_temp_out_1 >= 0.0) && (sense_temp_out_1 <= 0.0)) {     // Secondary Top Fan Heatsink
        SetBit(fault_alarm, SECONDARY_SIDE_TOP_HEATSINK_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.Secondary_Side_Top_heatsink_over_temperature = TRUE;
    }
    if ((sense_temp_out_2 >= 0.0) && (sense_temp_out_2 <= 0.0)) {     // Secondary Bot Fan Heatsink
        SetBit(fault_alarm, SECONDARY_SIDE_BOT_HEATSINK_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.Secondary_Side_Bot_heatsink_over_temperature = TRUE;
    }
    if ((sense_temp_out_3 >= 0.0) && (sense_temp_out_3 <= 0.0)) {     // Primary Fan Heatsink
        SetBit(fault_alarm, PRIMARY_SIDE_HEATSINK_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.Primary_Side_heatsink_over_temperature = TRUE;
    }
    if ((sense_temp_board_1 >= 0.0) && (sense_temp_board_1 <= 0.0)) {  // Primary Board temp
        SetBit(fault_alarm, PRIMARY_SIDE_BOARD_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.Primary_Side_Board_over_temperature = TRUE;
    }
    if ((sense_temp_board_2 >= 0.0) && (sense_temp_board_2 <= 0.0)) {  // Secondary Board temp
        SetBit(fault_alarm, SECONDARY_SIDE_BOARD_OVER_TEMPERATURE);
        operation.op.alarm.fault.bit.Secondary_Side_Board_over_temperature = TRUE;
    }
}

void app_PrimaryFrontPowerCheck(AppCommCanOpHandle* op)
{
    static Uint16 check_cnt[PRIMARY_FRONT_POWER_INDEX] = { 0, };
    int i;

    if (op->op.state >= op_384V_on_progress) {
// Pri Front Volt(384V) max
        if (sense_dc_link_pri_f_v_emavg >= set_calc_dc_link_pri_volts_max) {
            check_cnt[0]++;
            if (check_cnt[0] >= 50) {
                check_cnt[0] = 0;
                SetBit(dc_alarm, PRIMARY_SIDE_INPUT_FRONT_OVERVOLTAGE);
                operation.op.alarm.dc.bit.Primary_Side_Input_Front_Overvoltage = TRUE;
            }
        }

// Pri Front Volt(384V) min
        if (operation.op.power_stage.V384_on_complete_flag == TRUE) {
            if (sense_dc_link_pri_f_v_emavg <= set_calc_dc_link_pri_volts_min) {
                check_cnt[1]++;
                if (check_cnt[1] >= 50) {
                    check_cnt[1] = 0;
                    SetBit(dc_alarm, PRIMARY_SIDE_INPUT_FRONT_UNDERVOLTAGE);
                    operation.op.alarm.dc.bit.Primary_Side_Input_Front_Undervoltage = TRUE;
                }
            }
        }
    } else {
        for (i = 0; i < PRIMARY_FRONT_POWER_INDEX; i++) {
            check_cnt[i] = 0;
        }
    }
}

void app_PrimaryRearPowerCheck(AppCommCanOpHandle* op)  //300ms
{
    static Uint16 check_cnt[PRIMARY_REAR_POWER_INDEX] = { 0, };
    int i;

    if (op->op.power_stage.V384_on_complete_flag == TRUE) {
// Pri Rear Volt(384V) max
        if (sense_dc_link_pri_r_v_emavg >= set_calc_dc_link_pri_volts_max) {
            check_cnt[0]++;

            if (check_cnt[0] >= 30) {
                check_cnt[0] = 0;
                SetBit(dc_alarm, PRIMARY_SIDE_INPUT_REAR_OVERVOLTAGE);
                operation.op.alarm.dc.bit.Primary_Side_Input_Rear_Overvoltage = TRUE;
            }
        }
#if 0
// Pri Rear Volt(384V) min
            if(sense_dc_link_pri_r_v_emavg <= set_calc_dc_link_pri_volts_min) {
                SetBit(dc_alarm, PRIMARY_SIDE_INPUT_REAR_UNDERVOLTAGE);
                operation.op.alarm.dc.bit.Primary_Side_Input_Rear_Undervoltage = TRUE;
            }
#endif
    } else {
        for (i = 0; i < PRIMARY_REAR_POWER_INDEX; i++) {
            check_cnt[i] = 0;
        }
    }
}

void app_SecondaryPowerCheck(AppCommCanOpHandle *op)
{
    static Uint16 check_cnt[SECONDARY_POWER_INDEX] = { 0, };
    int i;

    if (op->op.power_stage.V384_on_complete_flag == TRUE) {
// Sec Volt(12V) max
        if (sense_dc_link_sec_v_emavg >= set_calc_dc_link_sec_volts_max) {
            check_cnt[0]++;

            if (check_cnt[0] >= 30) {
                check_cnt[0] = 0;

                SetBit(dc_alarm, SECONDARY_SIDE_INPUT_OVERVOLTAGE);
                operation.op.alarm.dc.bit.Secondary_Side_Input_Overvoltage = TRUE;
            }
        }
#if 0
// Sec Volt(12V) min
        if (sense_dc_link_sec_v_emavg <= set_calc_dc_link_sec_volts_min) {
            SetBit(dc_alarm, SECONDARY_SIDE_INPUT_UNDERVOLTAGE);
            operation.op.alarm.dc.bit.Secondary_Side_Input_Undervoltage = TRUE;
        }
#endif
// Sec Out Cap Volt(5V) max
        if (sense_dc_out_cap_v_emavg >= set_calc_dc_out_cap_volts_max) {
            check_cnt[1]++;

            if (check_cnt[1] >= 30) {
                check_cnt[1] = 0;
                SetBit(dc_alarm, SECONDARY_SIDE_OUTPUT_OVERVOLTAGE);
                operation.op.alarm.dc.bit.Secondary_Side_Output_Overvoltage = TRUE;
            }
        }
    } else {
        for (i = 0; i < SECONDARY_POWER_INDEX; i++) {
            check_cnt[i] = 0;
        }
    }
}

void app_BatteryErrorCheck(AppCommCanOpHandle* op)
{
    static Uint16 check_cnt[BAT_ERROR_INDEX] = { 0, };
    int i;

    if (op->op.power_stage.bat_connect_complete_flag == TRUE) {
#if 0
        if(sense_batt_t_emavg >= 70.0) {
            SetBit(battery_alarm, BATTERY_OVER_TEMPRATURE);
            operation.op.alarm.bat.bit.Battery_Over_temprature = TRUE;
        }

        if(sense_batt_t_emavg <= -20.0) {
            SetBit(battery_alarm, BATTERY_UNDER_TEMPRATURE);
            operation.op.alarm.bat.bit.Battery_Under_temprature = TRUE;
        }
#endif
        if (sense_batt_v_emavg >= set_calc_battary_volts_max) {
            check_cnt[2]++;
            if (check_cnt[2] >= 10) {
                check_cnt[2] = 0;
                SetBit(battery_alarm, BATTERY_OVER_VOLTAGE);
                operation.op.alarm.bat.bit.Battery_Over_voltage = TRUE;
            }
        }

        if (sense_batt_v_emavg <= set_calc_battary_volts_min) {
            check_cnt[3]++;
            if (check_cnt[3] >= 10) {
                check_cnt[3] = 0;
                SetBit(battery_alarm, BATTERY_UNDER_VOLTAGE);
                operation.op.alarm.bat.bit.Battery_Under_voltage = TRUE;
            }
        }

        if (calc_dc_out_i_emavg >= set_calc_output_current_max) {
            check_cnt[4]++;
            if (check_cnt[4] >= 10) {
                check_cnt[4] = 0;
                SetBit(battery_alarm, BATTERY_OVER_CURRENT);
                operation.op.alarm.bat.bit.Battery_Over_current = TRUE;
            }
        }

        if (fabsf(sense_dc_bb1_i_emavg - sense_dc_bb2_i_emavg) >= 20.0) {    // 10A
            check_cnt[5]++;
            if (check_cnt[5] >= 10) {
                check_cnt[5] = 0;
                SetBit(battery_alarm, BATTERY_CURRENT_UNBALANCE);
                operation.op.alarm.bat.bit.Battery_current_unbalance = TRUE;
            }
        }
    } else {
        for (i = 0; i < BAT_ERROR_INDEX; i++) {
            check_cnt[i] = 0;
        }
    }
}

void app_ScheduleFaultCheck(void)
{
// 이후 추가 필요
/*
 *  Set_over_the_Maximum_test_current
 *  Set_over_the_Maximum_test_Voltage
 *  Set_below_the_minimum_test_voltage
 *  Charge_Setting_Voltage_error
 *  Discharge_Setting_Voltage_error
 *
 */
}

void app_RunCheckSafety(Uint16 channel)
{
    static Uint32 old_time = 0, new_time = 0;
    static Uint32 old_time_alarm[10] = { 0, };
    static Uint32 new_time_alarm[10] = { 0, };

    app_EmoSwitchCheck();

    new_time_alarm[0] = get_time_ms();
    if (get_time_diff(old_time_alarm[0], new_time_alarm[0]) >= 100) {
        old_time_alarm[0] = new_time_alarm[0];
        app_BoardPowerErrorCheck();
    }

    new_time_alarm[1] = get_time_ms();
    if (get_time_diff(old_time_alarm[1], new_time_alarm[1]) >= 10) {
        old_time_alarm[1] = new_time_alarm[1];

        if (operation.op.state >= op_384V_on_progress) {
            app_PrimaryFrontPowerCheck(&operation);
        }

        if (operation.op.power_stage.V384_on_complete_flag == TRUE) {
            app_PrimaryRearPowerCheck(&operation);      //0.1s
            app_SecondaryPowerCheck(&operation);        //0.1s
            app_BatteryReverseCheck(&operation);
        }

        if (operation.op.power_stage.bat_connect_complete_flag == TRUE) {
            app_BatteryErrorCheck(&operation);          //0.1s
        }
    }

    if (operation.op.alarm.warning.all != 0 || operation.op.alarm.dc.all != 0
            || operation.op.alarm.bat.all != 0 || operation.op.alarm.fault.all != 0) {
        operation.op.alarm.alarm_flag = TRUE;
        operation.op.state = op_None;
        seq_mode = no_operation;
    }

    new_time = get_time_ms();
    if (get_time_diff(old_time, new_time) >= 100)  //@100ms
    {
        old_time = new_time;
        cmd_error[0] = (operation.op.alarm.warning.all >> 8) & 0x00FF;
        cmd_error[1] = (operation.op.alarm.warning.all) & 0x00FF;
        cmd_error[2] = (operation.op.alarm.dc.all >> 8) & 0x00FF;
        cmd_error[3] = (operation.op.alarm.dc.all) & 0x00FF;
        cmd_error[4] = (operation.op.alarm.bat.all >> 8) & 0x00FF;
        cmd_error[5] = (operation.op.alarm.bat.all) & 0x00FF;
        cmd_error[6] = (operation.op.alarm.fault.all >> 8) & 0x00FF;
        cmd_error[7] = (operation.op.alarm.fault.all) & 0x00FF;
        drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_120 + OBJ_MAX_1CH * channel].obj_id, cmd_error, 8);
    }
}

void app_ResetSafetyVal(void)
{
    drv_value_reset();
}

Uint16 app_KeepAliveCheck(Uint16 *cnt, Uint16 *old_cnt, BOOL *flag)
{
    static Uint32 old_t = 0;
    static Uint32 new_t = 0;

    new_t = get_time_ms();
    if (get_time_diff(old_t, new_t) >= 5000)  //@5s timeout
    {
        old_t = new_t;
        *flag = TRUE;
        return FALSE;
    } else {
        if (*old_cnt != *cnt) {
            old_t = new_t;
            *old_cnt = *cnt;
        }
        *flag = FALSE;
    }
    return TRUE;
}

//
// End of File
//
