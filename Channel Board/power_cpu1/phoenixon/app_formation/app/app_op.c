/*
 * app_op.c
 *
 *  Created on: 2022. 2. 4.
 *      Author: kms
 */

#include <math.h>

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "app_op.h"
#include "drv_can.h"
#include "drv_timer.h"
#include "drv_pwm.h"
#include <string.h>

#include "app_ctrl_power.h"
#include "bsp_io.h"
#include "app_safety.h"

extern float sense_batt_v_emavg;
extern float sense_dc_bb1_i_emavg;
extern float sense_dc_bb2_i_emavg;
extern float sense_dc_out_cap_v_emavg;

extern float sense_batt_v;
extern float sense_dc_bb1_i;
extern float sense_dc_bb2_i;
extern float sense_dc_out_cap_v;

extern float voltage_ref;
extern float current_ref;

extern float sense_dc_link_pri_f_v;
extern float sense_dc_link_pri_f_v_scale;
extern float sense_dc_link_pri_f_v_offset;
extern float sense_dc_link_pri_f_v_emavg;

extern float sense_dc_bb1_i_offset;
extern float sense_dc_bb2_i_offset;

Uint16 cmd_AA[8]                        = { can_tx_cmd_AA, can_rx_op_AA, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_none[8]                      = { can_tx_cmd_none, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_384V_rly_on[8]               = { can_tx_cmd_384V_relay_on, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_384V_rly_on_complete[8]      = { can_tx_cmd_384V_relay_on, can_rx_op_spare1, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_out_rly_on[8]                = { can_tx_cmd_output_relay_on, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_out_rly_off[8]               = { can_tx_cmd_output_relay_off, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_in_rly_off[8]                = { can_tx_cmd_input_relay_off, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_stop[8]                      = { can_tx_cmd_stop, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_error_clear[8]               = { can_tx_cmd_error_clear, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_recipe[8]                    = { can_tx_cmd_stop, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_null[8]                      = { 0, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_error[8]                     = { 0, 0, 0, 0, 0, 0, 0, 0 };

AppCommCanOpHandle operation;

extern appFormationSeq seq_mode;
extern c_d_mode charge_discharge_mode;
extern uint16_t cv_mode;

extern Uint16 stop_flag;
extern float avg_contact_resistance;

extern void offset_calibration(void);
extern Uint16 voltage_calibration_tune_flag;
extern Uint16 voltage_calibration_vrf_flag;

void can_tx_data_conversion(Uint16 *data, CommCanTxCMDList cmd, CommCanTxOPModeList op_mode, float vol, float curr)
{
    Uint32 temp_voltage = 0, temp_current = 0;

    data[0] = cmd;
    data[1] = op_mode;

    temp_voltage = (Uint32) (vol * 10000);

    data[2] = (temp_voltage >> 16) & 0xff;
    data[3] = (temp_voltage >> 8) & 0xff;
    data[4] = (temp_voltage >> 0) & 0xff;

    if (curr < 0) curr = curr * (float) (-1.0);
    temp_current = (Uint32) (curr * 10000);

    data[5] = (temp_current >> 16) & 0xff;
    data[6] = (temp_current >> 8) & 0xff;
    data[7] = (temp_current >> 0) & 0xff;
}

Uint16 app_paser(AppCommCanOpStateList state, Uint16 channel)
{
    static Uint16 cmd_old = 0;
    Uint16 obj_index_0x100 = channel * OBJ_MAX_1CH + 1;
    Uint16 obj_index_0xAA = channel * OBJ_MAX_1CH + 2;

    if (operation.rx.op_0xAA_cnt != drv_can_msg.obj[obj_index_0xAA].irq_cnt) {
        operation.rx.op_0xAA_cnt = drv_can_msg.obj[obj_index_0xAA].irq_cnt;
        operation.op.state = op_sensing_start_progress;
    }

    if (operation.rx.op_0x100_cnt != drv_can_msg.obj[obj_index_0x100].irq_cnt) {
        operation.rx.op_0x100_cnt = drv_can_msg.obj[obj_index_0x100].irq_cnt;

        memset(&operation.rx.cmd, 0, sizeof(CommCanRxCMDList));
        operation.rx.cmd = (CommCanRxCMDList) drv_can_msg.obj[obj_index_0x100].data[0];

        if (cmd_old != operation.rx.cmd) {
            cmd_old = operation.rx.cmd;
            memset(&operation.rx.op, 0, sizeof(CommCanRxOPModeList));
            memset(&operation.rx.vol, 0, sizeof(uint32_t));
            memset(&operation.rx.curr, 0, sizeof(uint32_t));
        }

        switch (operation.rx.cmd)
        {
            case can_rx_cmd_pc_connect:         //0x00
                operation.op.state = op_If_connect;
                break;

            case can_rx_cmd_input_relay_on:     //0x80
                operation.op.state = op_384V_on_progress;
                break;

            case can_rx_cmd_output_relay_on:    //0x81
                operation.op.state = op_output_connect;
                break;

            case can_rx_cmd_cha:                //0x82
            case can_rx_cmd_dis:                //0x83
            case can_rx_cmd_contact_resistance_measure:  // 0x87     CHA 22.05.16
                switch (drv_can_msg.obj[obj_index_0x100].data[1])
                {
                    case can_rx_op_cha_cc:
                        operation.rx.op = can_rx_op_cha_cc;
                        break;
                    case can_rx_op_cha_cv:
                        operation.rx.op = can_rx_op_cha_cv;
                        break;
                    case can_rx_op_cha_cp:
                        operation.rx.op = can_rx_op_cha_cp;
                        break;
                    case can_rx_op_dis_cc:
                        operation.rx.op = can_rx_op_dis_cc;
                        break;
                    case can_rx_op_dis_cv:
                        operation.rx.op = can_rx_op_dis_cv;
                        break;
                    case can_rx_op_dis_cp:
                        operation.rx.op = can_rx_op_dis_cp;
                        break;
                }
                operation.rx.vol = (uint32_t) (drv_can_msg.obj[obj_index_0x100].data[2] & 0xff) << 16
                                            | (drv_can_msg.obj[obj_index_0x100].data[3] & 0xff) << 8
                                            | (drv_can_msg.obj[obj_index_0x100].data[4] & 0xff);

                operation.rx.curr = (uint32_t) (drv_can_msg.obj[obj_index_0x100].data[5] & 0xff) << 16
                                            | (drv_can_msg.obj[obj_index_0x100].data[6] & 0xff) << 8
                                            | (drv_can_msg.obj[obj_index_0x100].data[7] & 0xff);
                operation.op.state = op_cha_dis;
                break;

            case can_rx_cmd_output_relay_off:   //0x84
                operation.op.state = op_output_off;
                break;

            case can_rx_cmd_input_relay_off:    //0x85
                operation.op.state = op_384V_off_progress;
                break;

            case can_rx_cmd_stop:               //0x86
                operation.op.state = op_idle;
                break;

            case can_rx_cmd_error_clear:        //0x89
                operation.op.alarm.warning.all  = 0;
                operation.op.alarm.bat.all      = 0;
                operation.op.alarm.dc.all       = 0;
                operation.op.alarm.fault.all    = 0;
                operation.op.alarm.alarm_flag   = FALSE;
                operation.op.alarm.clear_cnt++;
                break;
        }
    }
    return FALSE;
}

Uint16 app_alarm_paser(AppCommCanOpStateList state, Uint16 channel)
{
    Uint16 obj_index_0x100 = channel * OBJ_MAX_1CH + 1;
    if (operation.rx.op_0x100_cnt != drv_can_msg.obj[obj_index_0x100].irq_cnt) {
        operation.rx.op_0x100_cnt = drv_can_msg.obj[obj_index_0x100].irq_cnt;

        memset(&operation.rx.cmd, 0, sizeof(CommCanRxCMDList));
        operation.rx.cmd = (CommCanRxCMDList) drv_can_msg.obj[obj_index_0x100].data[0];

        switch (operation.rx.cmd)
        {
            case can_rx_cmd_error_clear:        //0x89
                operation.op.alarm.warning.all  = 0;
                operation.op.alarm.bat.all      = 0;
                operation.op.alarm.dc.all       = 0;
                operation.op.alarm.fault.all    = 0;
                operation.op.alarm.alarm_flag   = FALSE;
                operation.op.alarm.clear_cnt++;
                break;
        }
    }
    return 0;
}

Uint16 app_operation(Uint16 channel)
{
    static Uint32 old_time = 0, new_time = 0;
    static Uint32 old_time_control = 0, new_time_control = 0;
    static Uint32 old_time_control_2 = 0, new_time_control_2 = 0;
    static Uint32 old_time_control_3 = 0, new_time_control_3 = 0;
    static Uint32 old_time_thermal = 0, new_time_thermal = 0;
    static AppCommCanOpStateList old_state = op_None;
    static Uint16 first_in_flag = 0;
    static Uint16 wait_cnt = 0;
    static float voltage_ref_old = 0;

    Uint16 temp_data[8] = { 0, };
    Uint32 temp_thermal;

    if (operation.op.keepalive.fault_flag == FALSE && operation.op.alarm.alarm_flag == FALSE) {
        app_paser(operation.op.state, channel);

        new_time_thermal = get_time_ms();
        if (get_time_diff(old_time_thermal, new_time_thermal) >= 100) {        //@100ms
            old_time_thermal = new_time_thermal;
            temp_thermal = (Uint32) (operation.op.sensing.s_bat_t * 10000);

            temp_data[2] = (temp_thermal >> 16) & 0xff;
            temp_data[3] = (temp_thermal >> 8) & 0xff;
            temp_data[4] = (temp_thermal >> 0) & 0xff;

            drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_130 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
        }
    } else {
        app_alarm_paser(operation.op.state, channel);
    }

    if (old_state != operation.op.state) {  //check for state change
        operation.op.old_state = old_state;
        old_state = operation.op.state;
        old_time_control = new_time_control;
        old_time_control_2 = new_time_control_2;
        old_time_control_3 = new_time_control_3;
        seq_mode = no_operation;
        first_in_flag = 0;
    }

    switch (operation.op.state)
    {
        case op_None:
//TODO ADD turn off all controllers for alarm
            drv_DisablePWM();
            bsp_sec_mosfet(0);          // Secondary MOSFET Off
            bsp_IoEnableConv(0);        // Vicor Converter(BCM6123) Disable

            if (voltage_calibration_tune_flag == 0 || voltage_calibration_vrf_flag == 0) {
                bsp_IoEnableBattSen(0); // Calibration. Battery Sensing Relay Off
            }
            bsp_IoEnablePreCharge(0);   // Precharge Relay Off
            bsp_IoEnableInput(0);       // Primary MOSFET Off

            operation.op.power_stage.bat_connect_complete_flag = FALSE;
            operation.op.power_stage.V384_on_complete_flag = FALSE;
            operation.op.power_stage.V384_primary_mosfet_flag = FALSE;
            operation.op.power_stage.offset_calibration_flag = FALSE;
            drv_value_reset();
            break;

        case op_If_connect:
            new_time = get_time_ms();

            if (first_in_flag == 0) {
                first_in_flag = 1;
                if (get_time_diff(old_time, new_time) >= 50) {  //@100ms
                    old_time = new_time;
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, cmd_none, 8);
                }
            }
            if (get_time_diff(old_time, new_time) >= 100) {     //@100ms
                old_time = new_time;
                drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, cmd_none, 8);
            }
            old_time_control = new_time_control;
            old_time_control_3 = new_time_control_3;
            break;

        case op_384V_on_progress:
            new_time = get_time_ms();

            if (get_time_diff(old_time, new_time) >= 100) {     //@100ms
                old_time = new_time;
                if (operation.op.power_stage.V384_on_complete_flag == FALSE) {
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, cmd_384V_rly_on, 8);
                } else {
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, cmd_384V_rly_on_complete, 8);
                }
            }
            if (operation.op.power_stage.V384_primary_mosfet_flag == FALSE) {
                bsp_sec_mosfet(0);              // Secondary MOSFET Off
                bsp_IoEnableInput(0);           // Primary MOSFET Off
                bsp_IoEnablePreCharge(1);       // Precharge Relay On

                new_time_control_3 = get_time_ms();
                if (get_time_diff(old_time_control_3, new_time_control_3) >= 500) {
                    bsp_IoEnableInput(1);
                    new_time_control = get_time_ms();

                    if ((sense_dc_link_pri_f_v_emavg >= set_pu_dc_link_pri_volts * 0.9)
                            && (get_time_diff(old_time_control, new_time_control) >= 1000)) {
                        old_time_control = new_time_control;
                        old_time_control_3 = new_time_control_3;
                        operation.op.power_stage.V384_primary_mosfet_flag = TRUE;
                    }
                }
                old_time_control_2 = new_time_control_2;
            } else {
                if (operation.op.power_stage.offset_calibration_flag == TRUE) {
                    offset_calibration();
                } else {
                    new_time_control_2 = get_time_ms();
                    if (get_time_diff(old_time_control_2, new_time_control_2) >= 500) {  //@500ms
                        old_time_control_2 = new_time_control_2;
                        bsp_IoEnablePreCharge(0);   // Precharge Relay Off
                        bsp_IoEnableConv(1);        // Vicor Converter(BCM6123) Enable

                        wait_cnt++;
                        if (wait_cnt >= 2) {
                            wait_cnt = 0;
                            stop_flag = 0;
                            operation.op.power_stage.offset_calibration_flag = TRUE;
                        }
                    }
                }
            }
            break;

        case op_sensing_start_progress:
            bsp_IoEnableBattSen(1);     // Battery Sensing Relay On
            drv_DisablePWM();
            bsp_sec_mosfet(0);          // Secondary MOSFET Off

            app_ResetSafetyVal();

            new_time = get_time_ms();
            if (get_time_diff(old_time, new_time) >= 100) { //@100ms
                old_time = new_time;
                drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, cmd_AA, 8);
            }
            break;

        case op_output_connect:
            new_time = get_time_ms();
            if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                old_time = new_time;

                if (operation.op.power_stage.bat_connect_complete_flag == TRUE) {
                    can_tx_data_conversion(temp_data, can_tx_cmd_output_relay_on, can_tx_op_all_ready,
                                           operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                } else {
                    can_tx_data_conversion(temp_data, can_tx_cmd_output_relay_on, can_tx_op_input_ready,
                                           operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                }
                drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
            }
            if (first_in_flag == 0) {
                first_in_flag = 1;
                seq_mode = output_precharge;
            }
            break;

        case op_idle:  // include rest operation.
//TODO ADD turn off all controllers for alram
            seq_mode = control_idle;
            new_time = get_time_ms();
            if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                old_time = new_time;
                can_tx_data_conversion(temp_data, can_tx_cmd_stop, can_tx_op_all_ready,
                                       operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
            }
            break;

        case op_cha_dis:
            voltage_ref = ((float) operation.rx.vol / 1000.0f);
            current_ref = ((float) operation.rx.curr / 1000.0f);
            if (voltage_ref_old != voltage_ref) {
                voltage_ref_old = voltage_ref;
                operation.op.cv_mode_flag = FALSE;
            }

            if (operation.rx.cmd == can_rx_cmd_cha || operation.rx.cmd == can_rx_cmd_contact_resistance_measure) {
                charge_discharge_mode = charge_mode;
                if (operation.rx.op == can_rx_op_cha_cv) {
                    cv_mode = 1;
                } else {
                    cv_mode = 0;
                }
            } else if (operation.rx.cmd == can_rx_cmd_dis) {
                charge_discharge_mode = discharge_mode;
                if (operation.rx.op == can_rx_op_dis_cv) {
                    cv_mode = 1;
                } else {
                    cv_mode = 0;
                }
            }
            seq_mode = operation_control;

            if (operation.op.cv_mode_flag == TRUE) {  //xxx complete flag CC or CP
                new_time = get_time_ms();
                if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                    old_time = new_time;
                    if (operation.rx.cmd == can_rx_cmd_contact_resistance_measure) {
                        can_tx_data_conversion(temp_data, (CommCanTxCMDList) (operation.rx.cmd + 0x10), (CommCanTxOPModeList) (operation.rx.op + 0x08),
                                               operation.op.sensing.s_vol, avg_contact_resistance);
                    } else {
                        can_tx_data_conversion(temp_data, (CommCanTxCMDList) (operation.rx.cmd + 0x10), (CommCanTxOPModeList) (operation.rx.op + 0x08),
                                               operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                    }
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
                }
            } else {
                new_time = get_time_ms();
                if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                    old_time = new_time;
                    if (operation.rx.cmd == can_rx_cmd_contact_resistance_measure) {
                        can_tx_data_conversion(temp_data, (CommCanTxCMDList) (operation.rx.cmd + 0x10), (CommCanTxOPModeList) (operation.rx.op),
                                               operation.op.sensing.s_vol, avg_contact_resistance);
                    } else {
                        can_tx_data_conversion(temp_data, (CommCanTxCMDList) (operation.rx.cmd + 0x10), (CommCanTxOPModeList) (operation.rx.op),
                                               operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                    }
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
                }
            }
            break;

        case op_output_off:
            if (operation.op.power_stage.bat_connect_complete_flag == FALSE) {  //if battery disconnect complete
                new_time = get_time_ms();
                if (get_time_diff(old_time, new_time) >= 100)  //@100ms
                {
                    old_time = new_time;
                    can_tx_data_conversion(temp_data, can_tx_cmd_output_relay_off, can_tx_op_input_ready,
                                           operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
                }
            } else {
//xxx Operation output off
                drv_DisablePWM();
                bsp_sec_mosfet(0);  // Secondary MOSFET Off
                app_ResetSafetyVal();
                operation.op.power_stage.bat_connect_complete_flag = FALSE;

                new_time = get_time_ms();
                if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                    old_time = new_time;
                    can_tx_data_conversion(temp_data, can_tx_cmd_output_relay_off, can_tx_op_all_ready,
                                           operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                    drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
                }
            }
            break;

        case op_384V_off_progress:
            new_time = get_time_ms();
            if (get_time_diff(old_time, new_time) >= 100) {  //@100ms
                old_time = new_time;
                can_tx_data_conversion(temp_data, can_tx_cmd_input_relay_off, can_tx_op_idle,
                                       operation.op.sensing.s_vol, operation.op.sensing.s_curr);
                drv_WriteCAN(drv_can_msg.obj[CAN_TX_ID_110 + OBJ_MAX_1CH * channel].obj_id, temp_data, 8);
            }

            new_time_control = get_time_ms();
            if (operation.op.power_stage.V384_primary_mosfet_flag == TRUE) {
                if (get_time_diff(old_time_control, new_time_control) >= 500) {
                    old_time_control = new_time_control;
                    bsp_IoEnableInput(0);
                    operation.op.power_stage.V384_primary_mosfet_flag = FALSE;
                }
                old_time_control_2 = new_time_control_2;
            } else {
                if ((get_time_diff(old_time_control_2, new_time_control_2) >= 500)) {
                    old_time_control_2 = new_time_control_2;
                    bsp_IoEnablePreCharge(0);       // Precharge Relay Off
                    operation.op.power_stage.V384_on_complete_flag = FALSE;
                }

            }
            break;
    }

    return TRUE;
}

//
// End of File
//