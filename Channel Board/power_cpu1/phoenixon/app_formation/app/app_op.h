/*
 * app_op.h
 *
 *  Created on: 2022. 2. 4.
 *      Author: kms
 */

#ifndef PHOENIXON_APP_FORMATION_APP_APP_OP_H_
#define PHOENIXON_APP_FORMATION_APP_APP_OP_H_

#include "app_comm.h"

typedef enum
{
    FALSE = 0,
    TRUE
} BOOL;

#define MAX_CHANNEL 4
#define OBJ_MAX_ 4

#if 0
typedef enum
{
    //channel 1
    RX_ID_100 = 0,
    RX_ID_AA,
    TX_ID_110,
    TX_ID_120,

    //channel 2
    RX_ID_200,
    RX_ID_BA,
    TX_ID_210,
    TX_ID_220,

    //channel 3
    RX_ID_300,
    RX_ID_CA,
    TX_ID_310,
    TX_ID_320,

    //channel 4
    RX_ID_400,
    RX_ID_DA,
    TX_ID_410,
    TX_ID_420,

}Object_number;
#endif

typedef enum
{
    op_None,
    op_If_connect,
    op_384V_on_progress,
    op_sensing_start_progress,
    op_output_connect,
    op_idle,        // rest
    op_cha_dis,
    op_output_off,
    op_384V_off_progress,
} AppCommCanOpStateList;

typedef enum
{
    can_tx_seq_ready,
    can_tx_seq_send,
    can_tx_seq_complete
} AppCommCanTxSeq;

typedef struct
{
    BOOL connect        :1;     //Bit00
    BOOL input_on       :1;     //Bit01
    BOOL sensing        :1;     //Bit02
    BOOL output_on      :1;     //Bit03
    BOOL reserved4      :1;     //Bit04
    BOOL reserved5      :1;     //Bit05
    BOOL reserved6      :1;     //Bit06
    BOOL reserved7      :1;     //Bit07
    BOOL reserved8      :1;     //Bit08
    BOOL reserved9      :1;     //Bit09
    BOOL reserved10     :1;     //Bit10
    BOOL reserved11     :1;     //Bit11
    BOOL reserved12     :1;     //Bit12
    BOOL reserved13     :1;     //Bit13
    BOOL reserved14     :1;     //Bit14
    BOOL reserved15     :1;     //Bit15
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
    struct
    {
        AppCommCanOpStateList state;
        AppCommCanOpStateList old_state;

        BOOL cv_mode_flag;
        Uint16 cv_mode_cnt;

        struct
        {
            BOOL V384_primary_mosfet_flag;
            BOOL offset_calibration_flag;
            BOOL V384_on_complete_flag;
            BOOL bat_connect_complete_flag;
        }power_stage;

        struct
        {
            float s_vol;
            float s_curr;
            float s_bat_t;
        }sensing;

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
            BOOL alarm_flag;
            BOOL alarm_clear_flag;
            Uint16 clear_cnt;
        }alarm;

        struct
        {
            Uint16 cnt;
            Uint16 cnt_old;
            BOOL fault_flag;
        }keepalive;

    } op;

    struct
    {
        AppCommCanTxSeq seq;
        Uint16 cnt;
        Uint16 buf[8];
        BOOL sen_flag;
        Uint32 tx_s_vol;
        Uint32 tx_s_curr;
        Uint16 tx_alarm[4];
    } tx;

    struct
    {
        uint16_t op_0x100_cnt;
        uint16_t op_0xAA_cnt;
        CommCanRxCMDList cmd;
        CommCanRxOPModeList op;
        CommCanRxParallelModeList par;
        uint32_t vol;
        uint32_t curr;

        uint16_t alarm_report_cnt;
        Uint16 alarm[4];
        Uint64 time_out;
        Uint64 time;
        uint16_t schedule_end_cnt;
    } rx;
} AppCommCanOpHandle;

extern AppCommCanOpHandle operation;
Uint16 app_operation(Uint16 channel);

#endif /* PHOENIXON_APP_FORMATION_APP_APP_OP_H_ */
