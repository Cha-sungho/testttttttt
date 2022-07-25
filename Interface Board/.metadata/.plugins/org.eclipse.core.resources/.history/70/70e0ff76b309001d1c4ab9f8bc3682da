/*
 * can_op_comm_protocol.h
 *
 *  Created on: 2021. 8. 24.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_COMM_CAN_OP_PROTOCOL_H_
#define PHOENIXON_APP_COMM_CAN_OP_PROTOCOL_H_

#define MAX_CHANNEL 4
#define DUMMY_CHANNEL 1
#define DUMMY_CHANNEL_INDEX 4

//#define TX_ID_100       0
//#define TX_ID_AA        1
//#define RX_ID_110       2
//#define RX_ID_120       3

#define TX_INTERVAL     100 //100ms
#define RX_TIMEOUT      500 //500ms
#define RX_INVALID_CNT  150 //150 = 15s

typedef enum
{
    //channel 1
    TX_ID_100 = 1,
    TX_ID_AA,
    RX_ID_110,
    RX_ID_120,
    RX_ID_130,

    //channel 2
    TX_ID_200,
    TX_ID_BA,
    RX_ID_210,
    RX_ID_220,
    RX_ID_230,

    //channel 3
    TX_ID_300,
    TX_ID_CA,
    RX_ID_310,
    RX_ID_320,
    RX_ID_330,

    //channel 4
    TX_ID_400,
    TX_ID_DA,
    RX_ID_410,
    RX_ID_420,
    RX_ID_430,

}Object_number;


typedef enum
{
    can_tx_cmd_input_relay_on      = 0x80,
    can_tx_cmd_output_relay_on     = 0x81,
    can_tx_cmd_cha                 = 0x82,
    can_tx_cmd_dis                 = 0x83,
    can_tx_cmd_output_relay_off    = 0x84,
    can_tx_cmd_input_relay_off     = 0x85,
    can_tx_cmd_stop                = 0x86,
    can_tx_cmd_contact_resistance_measure     = 0x87,    // CHA 22.05.16 
    can_tx_cmd_error_clear         = 0x89
} CommCanTxCMDList;

typedef enum
{
    can_rx_cmd_none                = 0x00,
    can_rx_cmd_input_relay_on      = 0x90,
    can_rx_cmd_output_relay_on     = 0x91,
    can_rx_cmd_cha                 = 0x92,
    can_rx_cmd_dis                 = 0x93,
    can_rx_cmd_output_relay_off    = 0x94,
    can_rx_cmd_input_relay_off     = 0x95,
    can_rx_cmd_stop                = 0x96,
    can_rx_cmd_contact_resistance_measure     = 0x97,    // CHA 22.05.16 
    can_rx_cmd_error_clear         = 0x99,
    can_rx_cmd_AA                  = 0x55
} CommCanRxCMDList;

typedef enum
{
    can_tx_op_idle      = 0x00,
    can_tx_op_spare1    = 0x10,
    can_tx_op_spare2    = 0x20,
    can_tx_op_cha_cc    = 0x30,
    can_tx_op_cha_cv    = 0x40,
    can_tx_op_dis_cc    = 0x50,
    can_tx_op_dis_cv    = 0x60,
    can_tx_op_cha_cp    = 0x70,
    can_tx_op_dis_cp    = 0x80
} CommCanTxOPModeList;

typedef enum
{
    can_rx_op_idle          = 0x00,
    can_rx_op_input_ready   = 0x10,
    can_rx_op_all_ready     = 0x20,
    can_rx_op_cha_cc        = 0x30,
    can_rx_op_cha_cv        = 0x40,
    can_rx_op_cha_cv_set    = 0x48,
    can_rx_op_dis_cc        = 0x50,
    can_rx_op_dis_cv        = 0x60,
    can_rx_op_dis_cv_set    = 0x68,
    can_rx_op_cha_cp        = 0x70,
    can_rx_op_dis_cp        = 0x80,
    can_rx_op_complete      = 0x90,
    can_rx_op_AA            = 0xFF
} CommCanRxOPModeList;

typedef enum
{
    can_tx_single      = 0x00,
    can_tx_parallel    = 0x01
} CommCanTxParallelModeList;

typedef enum
{
    can_rx_single           = 0x00,
    can_rx_parallel_ready   = 0x01,
    can_rx_parallel         = 0x02
} CommCanRxParallelModeList;


#endif /* PHOENIXON_APP_COMM_CAN_OP_PROTOCOL_H_ */

