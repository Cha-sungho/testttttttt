/*
 * app_comm_ui.h
 *
 *  Created on: 2021. 12. 16.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_COMM_APP_COMM_H_
#define PHOENIXON_APP_COMM_APP_COMM_H_

#include "app_settings.h"

typedef enum
{
    link_rx_ui_batt_on = 1,
    link_rx_ui_batt_off,
    link_rx_ui_cmd_normal,
    link_tx_ui_state,
    link_tx_ui_error
} AppCommCanLinkIDList;

typedef enum
{
    can_rx_cmd_pc_connect                   = 0x00,
    can_rx_cmd_input_relay_on               = 0x80,
    can_rx_cmd_output_relay_on              = 0x81,
    can_rx_cmd_cha                          = 0x82,
    can_rx_cmd_dis                          = 0x83,
    can_rx_cmd_output_relay_off             = 0x84,
    can_rx_cmd_input_relay_off              = 0x85,
    can_rx_cmd_stop                         = 0x86,
    can_rx_cmd_contact_resistance_measure   = 0x87,  // CHA 22.05.16 
    can_rx_cmd_error_clear                  = 0x89
} CommCanRxCMDList;

typedef enum
{
    can_tx_cmd_AA                           = 0x55,
    can_tx_cmd_none                         = 0x10,
    can_tx_cmd_384V_relay_on                = 0x90,
    can_tx_cmd_output_relay_on              = 0x91,
    can_tx_cmd_cha                          = 0x92,
    can_tx_cmd_dis                          = 0x93,
    can_tx_cmd_output_relay_off             = 0x94,
    can_tx_cmd_input_relay_off              = 0x95,
    can_tx_cmd_stop                         = 0x96,
    can_tx_cmd_contact_resistance_measure   = 0x97,  // CHA 22.05.16
    can_tx_cmd_error_clear                  = 0x99
} CommCanTxCMDList;

typedef enum
{
    can_rx_op_idle          = 0x00,     // Include Rest
    can_rx_op_spare1        = 0x10,
    can_rx_op_spare2        = 0x20,
    can_rx_op_cha_cc        = 0x30,
    can_rx_op_cha_cv        = 0x40,
    can_rx_op_dis_cc        = 0x50,
    can_rx_op_dis_cv        = 0x60,
    can_rx_op_cha_cp        = 0x70,
    can_rx_op_dis_cp        = 0x80,
    can_rx_op_AA            = 0xFF,
} CommCanRxOPModeList;

typedef enum
{
    can_tx_op_idle          = 0x00,
    can_tx_op_input_ready   = 0x10,
    can_tx_op_all_ready     = 0x20,
    can_tx_op_cha_cc        = 0x30,
    can_tx_op_cha_cv        = 0x40,
    can_tx_op_dis_cc        = 0x50,
    can_tx_op_dis_cv        = 0x60,
    can_tx_op_cha_cp        = 0x70,
    can_tx_op_dis_cp        = 0x80,
    can_tx_op_complete      = 0x90
} CommCanTxOPModeList;

typedef enum
{
    can_rx_single      = 0x00,
    can_rx_parallel    = 0x01
} CommCanRxParallelModeList;

typedef enum
{
    can_tx_single           = 0x00,
    can_tx_parallel_ready   = 0x01,
    can_tx_parallel         = 0x02
} CommCanTxParallelModeList;

typedef struct
{
    CommCanRxCMDList cmd;
    CommCanRxOPModeList op;
    CommCanRxParallelModeList parallel;
    uint32_t volts;
    uint32_t amps;
    float conv_volts;
    float conv_amps;
} AppCommUiCommand;

typedef struct
{
    CommCanTxCMDList cmd;
    CommCanTxOPModeList op;
    CommCanTxParallelModeList parallel;
    uint32_t volts;
    uint32_t amps;
    float conv_volts;
    float conv_amps;
} AppCommUiReport;

typedef struct
{
    uint16_t error1;
    uint16_t error2;
    uint16_t error3;
    uint16_t error4;
    uint16_t error5;
    uint16_t error6;
    uint16_t error7;
    uint16_t error8;
} AppCommUiFault;

extern AppCommUiCommand app_comm_ui_cmd;

void app_InitComm(void);
void app_UpdateCommCmd(void);
void app_PushReport(void);

#endif /* PHOENIXON_APP_COMM_APP_COMM_H_ */
