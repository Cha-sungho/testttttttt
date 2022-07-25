/*
 * app_comm.h
 *
 *  Created on: 2021. 6. 14.
 *      Author: KHJ
 */

#ifndef PHOENIXON_APP_COMM_UI_OP_PROTOCOL_H_
#define PHOENIXON_APP_COMM_UI_OP_PROTOCOL_H_

#ifdef PNX_DEBUG
#define STEP_ARRAY_LENGTH 20
#else
#define STEP_ARRAY_LENGTH 1000
#endif

typedef enum
{
    head_init,
    head_data       = 0x4044,    //ASCII   @D = 0x4044
    head_jump       = 0x404A,    //Code    @J = 0x404A
    head_cmd        = 0x4043,    //        @C = 0x4043
    head_start      = 0x4053,    //        @S = 0x4053
    head_end        = 0x4045,    //        @E = 0x4045
    head_pause      = 0x4050,    //        @P = 0x4050
    head_alarm      = 0x4041,    //        @A = 0x4041
    head_none
} CommOpHeadList;

typedef enum
{
    channel_none    = 0x0,
    channel_01      = 0x1,
    channel_02      = channel_01 << 1,
    channel_03      = channel_01 << 2,
    channel_04      = channel_01 << 3
} CommOpChannel;

typedef enum
{
    d_req_cmd_none                          = 0x0,
    d_req_cmd_rest                          = 0x81,
    d_req_cmd_charge                        = 0x82,
    d_req_cmd_discharge                     = 0x83,
    d_req_cmd_contact_resistance_measure    = 0x93    // CHA 22.05.16
} CommOpDReqCmdList;

typedef enum
{
    d_res_cmd_none                          = 0x0,
    d_res_cmd_rest                          = 0x81,
    d_res_cmd_charge                        = 0x82,
    d_res_cmd_discharge                     = 0x83,
    d_res_cmd_alarm_clear                   = 0x84,
    d_res_cmd_input_relay_off               = 0x85,
    d_res_cmd_input_relay_on                = 0x86,
    d_res_cmd_output_relay_off              = 0x87,
    d_res_cmd_output_relay_on               = 0x88,
    d_res_cmd_all_relay_off                 = 0x89,
    d_res_cmd_contact_resistance_measure    = 0x93    // CHA 22.05.16
} CommOpDResCmdList;

typedef enum
{
    d_step_mode_none,
    d_step_mode_normal,
    d_step_mode_normal_pattern,
    d_step_mode_paralle,
    d_step_mode_parallel_pattern
} CommOpDStepMode;

typedef enum
{
    d_req_op_mode_none,
    d_req_op_mode_cha_cc,
    d_req_op_mode_cha_cccv,
    d_req_op_mode_cha_cp,
    d_req_op_mode_cha_cpcv,
    d_req_op_mode_dis_cc,
    d_req_op_mode_dis_cccv,
    d_req_op_mode_dis_cp,
    d_req_op_mode_dis_cpcv,
    d_req_op_mode_cha_cr,
    d_req_op_mode_dis_cr
} CommOpDReqOperationMode;

typedef enum
{
    d_res_op_mode_none,
    d_res_op_mode_input_on,
    d_res_op_mode_output_on,
    d_res_op_mode_cha_cc,
    d_res_op_mode_cha_cv,
    d_res_op_mode_dis_cc,
    d_res_op_mode_dis_cv,
    d_res_op_mode_cha_cp,
    d_res_op_mode_dis_cp,
    d_res_op_mode_reseved1,
    d_res_op_mode_cha_cr,
    d_res_op_mode_dis_cr
} CommOpDResOperationMode;

typedef enum
{
    d_sampling_1ms    = 1,
    d_sampling_10ms   = 2,
    d_sampling_100ms  = 3,
    d_sampling_1000ms = 4
} CommOpDLogInterval;

typedef enum
{
    d_res_end_cond_none             = 0,       // 0  [ None ],
    d_res_end_cond_time             = 1,       // 1  [ Time ],
    d_res_end_cond_i                = 2,       // 2  [ Ampere ],
    d_res_end_cond_capa             = 3,       // 3  [ Capacity ],
    d_res_end_cond_watt             = 4,       // 4  [ Watt ],
    d_res_end_cond_v                = 5,       // 5  [ Voltage ],
    d_res_Cycle_Loop_Complete       = 6,       //
    d_res_Cycle_Loop_Resume         = 7,       //
    d_res_Cycle_Voltage_Cut         = 8,       // 8  [ Cycle Voltage ],
    d_res_Cycle_Time_Cut            = 9,       // 9  [ Cycle Time ],
    d_res_Cycle_Capacity_Cut        = 10,      // 10 [ Cycle Capacity ],
    d_res_Cycle_Watt_Cut            = 11,      // 11 [ Cycle Watt ],
    d_res_Cycle_Low_Voltage_Cut     = 12,      // 12 [ Cycle Low Voltage ]
    d_res_Jump                      = 13,      // 13 [ Jump Echo ],
    d_res_Complete                  = 14,      // 14 [ Force Complete Echo ],
    d_res_Alarm                     = 15,      // 15 [ Alarm ],
    d_res_Pause                     = 16,      // 16 [ Pause Echo ],
    d_res_Dwell_Single              = 17,
    d_res_Dwell_Multi               = 18,
    d_res_Rising_Chamber_Single     = 19,
    d_res_Rising_Chamber_Multi      = 20,
    d_res_Step_Sync                 = 21
} CommOpDResEndCond;

typedef enum
{
    p_cmd_pause     = 0x01,
    p_cmd_complete  = 0x02,
    p_cmd_restart   = 0x03
} CommOpPCmd;

typedef enum
{
    c_cmd_none              = 0x00,
    c_cmd_rest              = 0x81,
    c_cmd_cha               = 0x82,
    c_cmd_dis               = 0x83,
    c_cmd_alarm_clear       = 0x84,
    c_cmd_input_relay_off   = 0x85,
    c_cmd_input_relay_on    = 0x86,
    c_cmd_output_relay_off  = 0x87,
    c_cmd_output_relay_on   = 0x88,
    c_cmd_all_relay_off     = 0x89,
    c_cmd_alarm_req         = 0x90,
    c_cmd_mode_parallel     = 0x91,
    c_cmd_mode_normal       = 0x92,
    c_cmd_cv                = 0x98
} CommOpCCmd;

typedef struct
{
    CommOpDReqCmdList command;

    CommOpDStepMode step;
    CommOpDReqOperationMode operation;

    CommOpDLogInterval sampling_time;
    CommOpDLogInterval send_time;

    Uint16 step_idx;

    Uint16 cycle_loop_cnt;
    Uint16 cycle_loop_go_idx;
    Uint16 cycle_loop_re_idx;

    Uint16 ref_v_16;
    Uint16 ref_v_00;

    Uint16 end_cond_v_16;
    Uint16 end_cond_v_00;

    Uint16 end_cond_v_move_idx;

    Uint16 ref_i_p_16;
    Uint16 ref_i_p_00;

    Uint16 end_cond_time_16;
    Uint16 end_cond_time_00;

    Uint16 cycle_idx;

    Uint16 end_cond_time_move_idx;

    Uint16 end_cond_i_16;
    Uint16 end_cond_i_00;

    Uint16 end_cond_i_move_idx;

    Uint16 end_cond_capa_16;
    Uint16 end_cond_capa_00;

    Uint16 end_cond_capa_move_idx;

    Uint16 end_cond_watt_16;
    Uint16 end_cond_watt_00;

    Uint16 end_cond_watt_move_idx;
} StepUnit;

typedef struct
{
    CommOpDLogInterval sampling_time;

    Uint16 step_idx;

    CommOpDResCmdList command;
    CommOpDResEndCond end_cond_state;

    CommOpDResOperationMode operation;
    CommOpDStepMode step;

    Uint16 sen_v_16;
    Uint16 sen_v_00;

    Uint16 sen_i_16;
    Uint16 sen_i_00;

    Uint16 time_64;
    Uint16 time_32;
    Uint16 time_16;
    Uint16 time_00;

    Uint16 end_cond_val_16;
    Uint16 end_cond_val_00;

    Uint16 cycle_loop_cnt;

    Uint16 sen_capa_16;
    Uint16 sen_capa_00;

    Uint16 sen_thermal_16;
    Uint16 sen_thermal_00;
} LogUnit;

typedef struct
{
    StepUnit total_step[STEP_ARRAY_LENGTH];

    CommOpHeadList head;
    uint16_t len;
    CommOpChannel channel;
    LogUnit total_log;
} TotalStepLog;

#endif /* PHOENIXON_APP_COMM_UI_OP_PROTOCOL_H_ */
