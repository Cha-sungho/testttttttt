/*
 * app_can_protocol.h
 *
 *  Created on: 2021. 8. 22.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_COMM_EQUIP_CAN_APP_CAN_OP_H_
#define PHOENIXON_APP_COMM_EQUIP_CAN_APP_CAN_OP_H_

#define REST_BY_STEP    1

typedef enum
{
    can_op_ui_connect,
    can_op_input_ready,
    can_op_sensing_ready,
    can_op_stop,
    can_op_output_ready,
    can_op_cha_dis,
    can_op_output_off,
    can_op_input_off,
} AppCommCanOpStateList;

typedef enum
{
    can_tx_seq_ready,
    can_tx_seq_send,
    can_tx_seq_complete
} AppCommCanTxSeq;

typedef struct
{
    BOOL connect            :1;      //bit 0
    BOOL input_on           :1;     //bit 1
    BOOL sensing            :1;      //bit 2
    BOOL output_on          :1;    //bit 3
    BOOL reserved4          :1;    //bit 4
    BOOL reserved5          :1;    //bit 5
    BOOL reserved6          :1;    //bit 6
    BOOL reserved7          :1;    //bit 7
    BOOL reserved8          :1;    //bit 8
    BOOL reserved9          :1;    //bit 9
    BOOL reserved10         :1;   //bit 10
    BOOL reserved11         :1;   //bit 11
    BOOL reserved12         :1;   //bit 12
    BOOL reserved13         :1;   //bit 13
    BOOL reserved14         :1;   //bit 14
    BOOL reserved15         :1;   //bit 15
} AppCommCanPowerState;

typedef struct
{
    BOOL Battery_Reversed                     :1;   //Bit00
    BOOL bad_battery_contact                  :1;   //Bit01
    BOOL Set_over_the_Maximum_test_current    :1;   //Bit02
    BOOL Set_over_the_Maximum_test_Voltage    :1;   //Bit03
    BOOL Set_below_the_minimum_test_voltage   :1;   //Bit04
    BOOL Charge_Setting_Voltage_error         :1;   //Bit05
    BOOL Discharge_Setting_Voltage_error      :1;   //Bit06
    BOOL reserved7                            :1;   //Bit07
    BOOL reserved8                            :1;   //Bit08
    BOOL reserved9                            :1;   //Bit09
    BOOL reserved10                           :1;   //Bit10
    BOOL reserved11                           :1;   //Bit11
    BOOL reserved12                           :1;   //Bit12
    BOOL reserved13                           :1;   //Bit13
    BOOL reserved14                           :1;   //Bit14
    BOOL reserved15                           :1;   //Bit15
} WarningAlarmList;

typedef struct
{
    BOOL Primary_Side_Input_Pre_Charge_ON_Failure   :1;   //Bit00
    BOOL Primary_Side_Input_OFF_Failure             :1;   //Bit01
    BOOL Primary_Side_Input_Front_Overvoltage       :1;   //Bit02
    BOOL Primary_Side_Input_Front_Undervoltage      :1;   //Bit03
    BOOL Primary_Side_Input_Rear_Overvoltage        :1;   //Bit04
    BOOL Primary_Side_Input_Rear_Undervoltage       :1;   //Bit05
    BOOL Secondary_Side_Input_Overvoltage           :1;   //Bit06
    BOOL Secondary_Side_Input_Undervoltage          :1;   //Bit07
    BOOL Secondary_Side_Output_Overvoltage          :1;   //Bit08
    BOOL Battery_Output_Pre_Charge_ON_Failure       :1;   //Bit09
    BOOL Output_Line_Fault                          :1;   //Bit10
    BOOL reserved11                                 :1;   //Bit11
    BOOL reserved12                                 :1;   //Bit12
    BOOL reserved13                                 :1;   //Bit13
    BOOL reserved14                                 :1;   //Bit14
    BOOL reserved15                                 :1;   //Bit15
} DcAlarmList;

typedef struct
{
    BOOL Battery_Over_temprature      :1;   //Bit00
    BOOL Battery_Under_temprature     :1;   //Bit01
    BOOL Battery_Over_voltage         :1;   //Bit02
    BOOL Battery_Under_voltage        :1;   //Bit03
    BOOL Battery_Over_current         :1;   //Bit04
    BOOL Battery_current_unbalance    :1;   //Bit05
    BOOL reserved6                    :1;   //Bit06
    BOOL reserved7                    :1;   //Bit07
    BOOL reserved8                    :1;   //Bit08
    BOOL reserved9                    :1;   //Bit09
    BOOL reserved10                   :1;   //Bit10
    BOOL reserved11                   :1;   //Bit11
    BOOL reserved12                   :1;   //Bit12
    BOOL reserved13                   :1;   //Bit13
    BOOL reserved14                   :1;   //Bit14
    BOOL reserved15                   :1;   //Bit15
} BatteryAlarmList;

typedef struct
{
    BOOL Emergency_Switch_Operation                     :1;   //Bit00
    BOOL Internal_Fan_stop                              :1;   //Bit01
    BOOL Internal_power_supply_15V_A_Fault              :1;   //Bit02
    BOOL Internal_power_supply_5V_A_Fault               :1;   //Bit03
    BOOL Internal_power_supply_12V_D_Fault              :1;   //Bit04
    BOOL Primary_Side_heatsink_over_temperature         :1;   //Bit05
    BOOL Primary_Side_Board_over_temperature            :1;   //Bit06
    BOOL Secondary_Side_Board_over_temperature          :1;   //Bit07
    BOOL Secondary_Side_Top_heatsink_over_temperature   :1;   //Bit08
    BOOL Secondary_Side_Bot_heatsink_over_temperature   :1;   //Bit09
    BOOL DSP_over_temperature                           :1;   //Bit10
    BOOL reserved11                                     :1;   //Bit11
    BOOL reserved12                                     :1;   //Bit12
    BOOL reserved13                                     :1;   //Bit13
    BOOL reserved14                                     :1;   //Bit14
    BOOL reserved15                                     :1;   //Bit15
} FaultAlarmList;

typedef struct
{
    union
    {
        Uint16 all;
        AppCommCanPowerState bit;
    } power_ready_state;

    struct
    {
        AppCommCanOpStateList state;
        Uint64 timer;
        BOOL comm_error;
        Uint16 power_error;
    } op;

    struct
    {
        AppCommCanTxSeq seq;
        Uint16 cnt;
        Uint16 buf[8];
        BOOL sen_flag;
        Uint64 timer;
    } tx;

    struct
    {
        CommOpDReqCmdList cmd;          // Byte #7-8    I/F Command
        CommOpDStepMode step_mode;      // Byte #9-10   Step Mode
        CommOpDReqOperationMode op;     // Byte #11-12  Operation Mode
        Uint16 step_idx;
        Uint16 cycle_loop_cnt;
        Uint32 vol;
        Uint32 cur;
        Uint64 timer;
        Uint16 schedule_end_flag;
        Uint16 step_change_flag;
        struct
        {
            Uint64 time;
            Uint16 step_index;
        }pause;
        struct
        {
            BOOL flag;
        }restart;
    } recipe;

    struct
    {
        Uint32 vol;
        Uint32 cur;
        Uint32 capa;
        Uint32 watt;
        Uint32 time;
        Uint32 move_index;
        CommOpDResEndCond state;
        CommOpDResEndCond state_old;
        float32 value;
    } end_cond;

    struct
    {
        Uint16 cmd;
        Uint16 op;
    } remap_idx;

    float cumulative_capacity;		// 22.07.10 
    float cumulative_watt;
    float current;

    struct
    {
        struct
        {
            uint16_t cnt;
            BOOL flag;
            BOOL ignore_flag;
        } start;

        struct
        {
            uint16_t step_idx;
            uint16_t cnt;
            BOOL flag;
        } jump;

        struct
        {
            CommOpCCmd cmd;
            uint16_t cnt;
            BOOL flag;
        } command;

        struct
        {
            CommOpPCmd cmd;
            uint16_t cnt;
            BOOL flag;
        } pause;
    } ui_evt;

    struct
    {
        uint16_t op_report_cnt;
        CommCanRxCMDList cmd_old;
        CommCanRxCMDList cmd;
        CommCanRxCMDList exp_cmd;
        CommCanRxOPModeList op_old;
        CommCanRxOPModeList op;
        CommCanRxOPModeList exp_op;
        CommCanRxParallelModeList par;
        CommCanRxParallelModeList exp_par;
        uint16_t invalid_cnt;
        uint32_t vol;
        uint32_t curr;
        uint32_t capa;
        uint32_t thermal;
        uint16_t alarm_report_cnt;
        uint16_t thermal_report_cnt;

        struct
        {
            union
            {
                Uint16 all;
                WarningAlarmList bit;
            }warning;
            union
            {
                Uint16 all;
                DcAlarmList bit;
            }dc;
            union
            {
                Uint16 all;
                BatteryAlarmList bit;
            }bat;
            union
            {
                Uint16 all;
                FaultAlarmList bit;
            }fault;
            BOOL flag;
        }alarm;

        Uint64 time_out;
        Uint64 time;
        uint16_t schedule_end_cnt;
    } rx;
} AppCommCanOpHandle;

extern AppCommCanOpHandle can_msg[MAX_CHANNEL];
extern Uint32 timer_count;
extern Uint32 timer_count_2;
extern Uint32 timer_count_3;
extern Uint32 timer_count_4;

Uint16 app_CommCanOpRun(Uint16 channel);
Uint16 app_CommCanOpInit(Uint16 channel, BOOL use_for_alarm);
void app_CanOpVarInit(void);
void app_CommCanTimer(void);

#endif /* PHOENIXON_APP_COMM_EQUIP_CAN_APP_CAN_OP_H_ */
