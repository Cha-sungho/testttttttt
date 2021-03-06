/*
 * app_can_protocol.c
 *
 *  Created on: 2021. 8. 22.
 *      Author: JOO
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"
#include "wizchip_conf.h"
#include "ring_buffer.h"


#pragma CODE_SECTION(app_CommCanOpRun, ".TI.ramfunc");
#pragma CODE_SECTION(app_CommCanOpRecvProc, ".TI.ramfunc");
#pragma CODE_SECTION(app_CommCanOpPowerUpdateVar, ".TI.ramfunc");
#pragma CODE_SECTION(app_CommCanTimer, ".TI.ramfunc");

static Uint16 app_CommCanOpMsgSeqUpdateEndCondition(Uint16 channel);
static Uint16 app_CommCanOpMsgSeqCheckEndCondition(Uint16 channel);

static Uint16 app_CommCanOpRecvProc(Uint16 channel);
static void app_CommCanOpPowerUpdateVar(Uint16 channel);

static Uint16 app_CommCanOpMainProc(Uint16 channel);
static Uint16 app_CommCanOpRecipeUpdate(Uint16 channel);

static Uint16 app_CommCanOpUiEvt(Uint16 channel);

static Uint16 app_CommCanOpMsgSeqReady(Uint16 *data, CommCanRxCMDList exp_cmd, CommCanRxOPModeList exp_op, BOOL can, Uint16 channel);
static Uint16 app_CommCanOpMsgSeqSend(Uint16 channel);
static Uint16 app_CommCanOpMsgSeqComplete(Uint16 channel);

static Uint16 app_CommCanOpMsgAlarmClear(Uint16 channel);
static Uint16 app_CommCanOpMsgInputMCOff(Uint16 channel);

static Uint16 app_StartSeq(Uint16 channel);



////////////////////////////////////////////////////////////////////// Recipe to CAN TX Re-map Tables
typedef struct                                                      //
{                                                                   //
    CommOpDReqCmdList recipe;                                       //
    CommCanTxCMDList can_tx;                                        //
} RemapRecipeTxCmdTable;                                            //
//
const RemapRecipeTxCmdTable remap_cmd_recipe_tx[5] =                //
{                                                                   //
                                                                    { d_req_cmd_none,       can_tx_cmd_stop },                      //
                                                                    { d_req_cmd_rest,       can_tx_cmd_stop },                      //
                                                                    { d_req_cmd_charge,     can_tx_cmd_cha },                       //
                                                                    { d_req_cmd_discharge,  can_tx_cmd_dis },                        //
                                                                    { d_req_cmd_contact_resistance_measure,  can_tx_cmd_contact_resistance_measure }                        //
}; //
//
typedef struct                                                      //
{                                                                   //
    CommOpDStepMode recipe;                                         //
    CommCanTxParallelModeList can_tx;                               //
} RemapRecipeTxStepModeTable;                                       //
//
const RemapRecipeTxStepModeTable remap_step_recipe_tx[5] =          //
{                                                                   //
                                                                    { d_step_mode_none,             can_tx_single },                //
                                                                    { d_step_mode_normal,           can_tx_single },                //
                                                                    { d_step_mode_normal_pattern,   can_tx_single },                //
                                                                    { d_step_mode_paralle,          can_tx_parallel },              //
                                                                    { d_step_mode_parallel_pattern, can_tx_parallel }               //
};                                                                  //
//
typedef struct                                                      //
{                                                                   //
    CommOpDReqOperationMode recipe;                                 //
    CommCanTxOPModeList can_tx;                                     //
} RemapRecipeTxOpModeTable;                                         //
//
const RemapRecipeTxOpModeTable remap_op_recipe_tx[11] =             //
{                                                                   //
                                                                    { d_req_op_mode_none,       can_tx_op_spare2 },                 //
                                                                    { d_req_op_mode_cha_cc,     can_tx_op_cha_cc },                 //
                                                                    { d_req_op_mode_cha_cccv,   can_tx_op_cha_cv },                 //
                                                                    { d_req_op_mode_cha_cp,     can_tx_op_cha_cp },                 //
                                                                    { d_req_op_mode_cha_cpcv,   can_tx_op_cha_cp },                 //
                                                                    { d_req_op_mode_dis_cc,     can_tx_op_dis_cc },                 //
                                                                    { d_req_op_mode_dis_cccv,   can_tx_op_dis_cv },                 //
                                                                    { d_req_op_mode_dis_cp,     can_tx_op_dis_cp },                 //
                                                                    { d_req_op_mode_dis_cpcv,   can_tx_op_dis_cp },                 //
                                                                    { d_req_op_mode_cha_cr,     can_tx_op_cha_cc },                 //
                                                                    { d_req_op_mode_dis_cr,     can_tx_op_dis_cc }                  //
};                                                                  //
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////// CAN TX to CAN RX Re-map Tables Start
typedef struct                                                      //
{                                                                   //
    CommCanTxCMDList can_tx;                                        //
    CommCanRxCMDList can_rx;                                        //
} RemapTxRxCmdTable;                                                //
//
const RemapTxRxCmdTable remap_cmd_tx_rx[9] =                        //
{                                                                   //
                                                                    { can_tx_cmd_input_relay_on,    can_rx_cmd_input_relay_on },    //
                                                                    { can_tx_cmd_output_relay_on,   can_rx_cmd_output_relay_on },   //
                                                                    { can_tx_cmd_cha,               can_rx_cmd_cha },               //
                                                                    { can_tx_cmd_dis,               can_rx_cmd_dis },               //
                                                                    { can_tx_cmd_output_relay_off,  can_rx_cmd_output_relay_off },  //
                                                                    { can_tx_cmd_input_relay_off,   can_rx_cmd_input_relay_off },   //
                                                                    { can_tx_cmd_stop,              can_rx_cmd_stop },              //
                                                                    { can_tx_cmd_contact_resistance_measure,       can_rx_cmd_contact_resistance_measure },        //
                                                                    { can_tx_cmd_error_clear,       can_rx_cmd_error_clear }        //
};                                                                  //
//
typedef struct                                                      //
{                                                                   //
    CommCanTxOPModeList can_tx;                                     //
    CommCanRxOPModeList can_rx;                                     //
} RemapTxRxOpTable;                                                 //
//
const RemapTxRxOpTable remap_op_tx_rx[9] =                         //
{                                                                   //
                                                                    { can_tx_op_idle,   can_rx_op_idle },                           //
                                                                    { can_tx_op_spare1, can_rx_op_input_ready },                    //
                                                                    { can_tx_op_spare2, can_rx_op_all_ready },                      //
                                                                    { can_tx_op_cha_cc, can_rx_op_cha_cc },                         //
//                                                                    { can_tx_op_cha_cv, can_rx_op_cha_cv },                         //
                                                                    { can_tx_op_cha_cv, can_rx_op_cha_cv_set },                         //
                                                                    { can_tx_op_dis_cc, can_rx_op_dis_cc },                         //
//                                                                    { can_tx_op_dis_cv, can_rx_op_dis_cv },                         //
                                                                    { can_tx_op_dis_cv, can_rx_op_dis_cv_set },                         //
                                                                    { can_tx_op_cha_cp, can_rx_op_cha_cp },                         //
                                                                    { can_tx_op_dis_cp, can_rx_op_dis_cp }                          //
};                                                                  //
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////// CMD CAN RX to Log Re-map Tables Start
typedef struct                                                      //
{                                                                   //
    CommCanRxCMDList can_rx;                                        //
    CommOpDResCmdList log;                                          //
} RemapLogRxCmdTable;                                               //
//
const RemapLogRxCmdTable remap_cmd_rx_log[9] =                      //
{                                                                   //
                                                                    { can_rx_cmd_input_relay_on,    d_res_cmd_input_relay_on },     //
                                                                    { can_rx_cmd_output_relay_on,   d_res_cmd_output_relay_on },    //
                                                                    { can_rx_cmd_cha,               d_res_cmd_charge },             //
                                                                    { can_rx_cmd_dis,               d_res_cmd_discharge },          //
                                                                    { can_rx_cmd_output_relay_off,  d_res_cmd_output_relay_off },   //
                                                                    { can_rx_cmd_input_relay_off,   d_res_cmd_input_relay_off },    //
                                                                    { can_rx_cmd_stop,              d_res_cmd_rest },               //
                                                                    { can_rx_cmd_contact_resistance_measure,       d_res_cmd_contact_resistance_measure },         //
                                                                    { can_rx_cmd_error_clear,       d_res_cmd_alarm_clear }         //
};                                                                  //
//
typedef struct                                                      //
{                                                                   //
    CommCanRxOPModeList can_rx;                                     //
    CommOpDResOperationMode log;                                    //
} RemapLogRxOpTable;                                                //
//
const RemapLogRxOpTable remap_op_rx_log[12] =                       //
{                                                                   //
                                                                    { can_rx_op_idle,           d_res_op_mode_none },               //
                                                                    { can_rx_op_input_ready,    d_res_op_mode_input_on },           //
                                                                    { can_rx_op_all_ready,      d_res_op_mode_output_on },          //
                                                                    { can_rx_op_cha_cc,         d_res_op_mode_cha_cc },             //
                                                                    { can_rx_op_cha_cv,         d_res_op_mode_cha_cc },             //
                                                                    { can_rx_op_cha_cv_set,     d_res_op_mode_cha_cv },             //
                                                                    { can_rx_op_dis_cc,         d_res_op_mode_dis_cc },             //
                                                                    { can_rx_op_dis_cv,         d_res_op_mode_dis_cc },             //
                                                                    { can_rx_op_dis_cv_set,     d_res_op_mode_dis_cv },             //
                                                                    { can_rx_op_cha_cp,         d_res_op_mode_cha_cp },             //
                                                                    { can_rx_op_dis_cp,         d_res_op_mode_dis_cp },             //
                                                                    { can_rx_op_complete,       d_res_op_mode_none }                //
};                                                                  //
//
typedef struct                                                      //
{                                                                   //
    CommCanRxParallelModeList can_rx;                               //
    CommOpDStepMode log;                                            //
} RemapLogRxModeTable;                                              //
//
const RemapLogRxModeTable remap_par_rx_log[3] =                     //
{                                                                   //
                                                                    { can_rx_single,            d_step_mode_normal },               //
                                                                    { can_rx_parallel_ready,    d_step_mode_normal },               //
                                                                    { can_rx_parallel,          d_step_mode_paralle }               //
};                                                                  //
//////////////////////////////////////////////////////////////////////

//static Uint16 cmd_in_rly_on[8]      = { can_tx_cmd_input_relay_on, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_out_rly_on[8]     = { can_tx_cmd_output_relay_on, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_out_rly_off[8]    = { can_tx_cmd_output_relay_off, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_in_rly_off[8]     = { can_tx_cmd_input_relay_off, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_stop[8]           = { can_tx_cmd_stop, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_error_clear[8]    = { can_tx_cmd_error_clear, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_recipe[8]         = { can_tx_cmd_stop, 0, 0, 0, 0, 0, 0, 0 };
//static Uint16 cmd_null[8]           = { 0, 0, 0, 0, 0, 0, 0, 0 };

Uint16 cmd_in_rly_on[8]      = { can_tx_cmd_input_relay_on, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_out_rly_on[8]     = { can_tx_cmd_output_relay_on, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_out_rly_off[8]    = { can_tx_cmd_output_relay_off, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_in_rly_off[8]     = { can_tx_cmd_input_relay_off, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_stop[8]           = { can_tx_cmd_stop, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_error_clear[8]    = { can_tx_cmd_error_clear, 0, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_recipe[8]         = { can_tx_cmd_stop, can_tx_op_spare2, 0, 0, 0, 0, 0, 0 };
Uint16 cmd_null[8]           = { 0, 0, 0, 0, 0, 0, 0, 0 };

//AppCommCanOpHandle can_msg;
AppCommCanOpHandle can_msg[MAX_CHANNEL];

Uint16 cc_cut = 0;
Uint16 cv_cut = 0;
Uint16 time_cut = 0;
Uint16 end_condition_check = 0;

Uint16 app_CommCanOpInit(Uint16 channel, BOOL use_for_alarm)
{
    can_msg[channel].op.state = can_op_ui_connect;
    can_msg[channel].tx.seq = can_tx_seq_ready;
    if(can_msg[channel].power_ready_state.bit.output_on == TRUE)
    {
        can_msg[channel].power_ready_state.bit.output_on = FALSE;
        drv_WriteCAN(drv_can_msg.obj[TX_ID_100+(OBJ_MAX_1CH*channel)].obj_id, cmd_out_rly_off, 8);
    }
    if(can_msg[channel].power_ready_state.bit.input_on == TRUE)
    {
        can_msg[channel].power_ready_state.all = 0;
        drv_WriteCAN(drv_can_msg.obj[TX_ID_100+(OBJ_MAX_1CH*channel)].obj_id, cmd_in_rly_off, 8);
    }

    can_msg[channel].ui_evt.start.flag = FALSE;
    can_msg[channel].ui_evt.start.ignore_flag = FALSE;
    can_msg[channel].ui_evt.start.cnt = ui_msg_rx.recv.start[channel].cnt;

    can_msg[channel].ui_evt.jump.flag = FALSE;
    can_msg[channel].ui_evt.jump.cnt = ui_msg_rx.recv.jump[channel].cnt;

    can_msg[channel].ui_evt.command.flag = FALSE;
    can_msg[channel].ui_evt.command.cnt = ui_msg_rx.recv.command[channel].cnt;

    can_msg[channel].ui_evt.pause.flag = FALSE;
    can_msg[channel].ui_evt.pause.cnt = ui_msg_rx.recv.pause[channel].cnt;

    gp_sdram[channel]->total_log.step_idx = 0;
    can_msg[channel].recipe.step_idx = 0;
    memset(&can_msg[channel].recipe, 0,sizeof(can_msg[channel].recipe));


    can_msg[channel].recipe.timer = 0;
    can_msg[channel].recipe.step_change_flag = FALSE;
    can_msg[channel].end_cond.state = d_res_end_cond_none;


    if(use_for_alarm==FALSE)
    {
        can_msg[channel].recipe.cycle_loop_cnt=0;
        ui_msg_tx[channel].send.start.cnt = can_msg[channel].ui_evt.start.cnt;
    }

    return 0;
}

Uint16 app_CommCanOpRun(Uint16 channel)
{
    if (ui_msg_rx.sock_state == SOCK_ESTABLISHED) {
        app_CommCanOpMainProc(channel);
        app_CommCanOpUiEvt(channel);
        app_CommCanOpMsgSeqSend(channel);
        app_CommCanOpMsgAlarmClear(channel);
    } else {
        app_CommCanOpInit(channel,FALSE);
    }
    app_CommCanOpRecvProc(channel);

    return 0;
}

static Uint16 app_CommCanOpMsgSeqSend(Uint16 channel)
{
    //send tx 100ms interval
    switch (can_msg[channel].tx.seq)
    {
    case can_tx_seq_send:
        //send
        if (can_msg[channel].tx.timer >= TX_INTERVAL) {
            can_msg[channel].tx.timer = 0;
            if (can_msg[channel].tx.sen_flag == TRUE)
                drv_WriteCAN(drv_can_msg.obj[TX_ID_AA+(OBJ_MAX_1CH*channel)].obj_id, can_msg[channel].tx.buf, 8);
            else
                drv_WriteCAN(drv_can_msg.obj[TX_ID_100+(OBJ_MAX_1CH*channel)].obj_id, can_msg[channel].tx.buf, 8);
        }

        //receive check, if received change tx state complete
        if (can_msg[channel].tx.cnt != can_msg[channel].rx.op_report_cnt) {
            can_msg[channel].tx.cnt = can_msg[channel].rx.op_report_cnt;
            can_msg[channel].tx.seq = can_tx_seq_complete;
        }

        //time out check
        //        if (can_msg[channel].rx.time_out >= RX_TIMEOUT) {
        //            can_msg[channel].rx.time_out = 0;
        //            can_msg[channel].op.comm_error = TRUE;
        //            printf("%s-%s-%d CAN RX TIEMOUT\r\n", __FILE__, __FUNCTION__, __LINE__);
        //        }
        break;
    }

    return 0;
}

/*
 * @param   *data:      CAN TX data to be copied
 *          exp_cmd:    expected response cmd
 *          exp_op:     expected response op
 *          can:        change CAN ID flag
 */
static Uint16 app_CommCanOpMsgSeqReady(Uint16 *data, CommCanRxCMDList exp_cmd, CommCanRxOPModeList exp_op, BOOL can, Uint16 channel)
{
    //tx data copy and change tx sequence send and etc...
    switch (can_msg[channel].tx.seq)
    {
    case can_tx_seq_ready:
        memcpy(can_msg[channel].tx.buf, data, 8);    //update tx data
        can_msg[channel].rx.exp_cmd = exp_cmd;       //update expected cmd
        can_msg[channel].rx.exp_op = exp_op;         //update expected op
        can_msg[channel].tx.seq = can_tx_seq_send;   //change tx sequence send
        //init exception value
        can_msg[channel].rx.time_out = 0;
        can_msg[channel].rx.invalid_cnt = 0;
        can_msg[channel].tx.sen_flag = can;
        break;
    }

    return 0;
}

static Uint16 app_CommCanOpMsgSeqComplete(Uint16 channel)
{
    Uint16 complete = 0;

    switch (can_msg[channel].tx.seq)
    {
    case can_tx_seq_complete:
        //check expected cmd, op value. if all matched complete is 2.
        if (can_msg[channel].rx.exp_cmd == NULL
                || (can_msg[channel].rx.cmd) == (can_msg[channel].rx.exp_cmd)) complete++;

        if (can_msg[channel].rx.exp_op == NULL
                || ((can_msg[channel].rx.op & 0xf0) == (can_msg[channel].rx.exp_op & 0xf0))
                || ((can_msg[channel].rx.op & 0xf0) == ((can_msg[channel].rx.exp_op & 0xf0) + 0x10))) complete++;

        //if all matched change tx sequence ready, else tx again
        if (complete == 2) can_msg[channel].tx.seq = can_tx_seq_ready;
        else {
            can_msg[channel].tx.seq = can_tx_seq_send;

            //invalid check
            if(can_msg[channel].rx.invalid_cnt++ > RX_INVALID_CNT) {
                can_msg[channel].op.comm_error = TRUE;
                //printf("%s-%s-%d CAN RX INVALID\r\n", __FILE__, __FUNCTION__, __LINE__);
            }
        }
        break;
    }

    return complete;
}

static Uint16 app_CommCanOpRecipeUpdate(Uint16 channel)
{
    Uint32 temp = 0;
    Uint16 i;

    if(can_msg[channel].recipe.restart.flag == TRUE)
    {
        can_msg[channel].recipe.step_idx = can_msg[channel].recipe.pause.step_index;
        can_msg[channel].recipe.timer = can_msg[channel].recipe.pause.time;
        can_msg[channel].recipe.restart.flag = FALSE;
    }
    else
    {
        can_msg[channel].recipe.step_idx = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].step_idx;
    }
    can_msg[channel].recipe.cmd = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].command;       // Byte #7-8    I/F Command
    can_msg[channel].recipe.step_mode = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].step;    // Byte #9-10   Step Mode
    can_msg[channel].recipe.op = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].operation;      // Byte #11-12  Operation Mode


    temp = (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].ref_v_16);
    temp = (temp << 16) & 0xFFFF0000;
    temp |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].ref_v_00 & (Uint32) 0x0000FFFF);
    can_msg[channel].recipe.vol = temp;

    temp = (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].ref_i_p_16);
    temp = (temp << 16) & 0xFFFF0000;
    temp |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].ref_i_p_00 & (Uint32) 0x0000FFFF);
    can_msg[channel].recipe.cur = temp;

    //re-map recipe to can tx cmd
//    for (i = 0; i < 4; i++) {
    for (i = 0; i < 5; i++) {
        if (remap_cmd_recipe_tx[i].recipe == can_msg[channel].recipe.cmd) {
            cmd_recipe[0] = remap_cmd_recipe_tx[i].can_tx;
            break;
        }
    }

    //re-map recipe to can tx op(OP MODE)
    for (i = 0; i < 11; i++) {
        if (remap_op_recipe_tx[i].recipe == can_msg[channel].recipe.op) {
            cmd_recipe[1] = remap_op_recipe_tx[i].can_tx;
            break;
        }
    }

    //re-map recipe to can tx step(STEP MODE)
    for (i = 0; i < 5; i++) {
        if (remap_step_recipe_tx[i].recipe == can_msg[channel].recipe.step_mode) {
            cmd_recipe[1] |= remap_step_recipe_tx[i].can_tx;
            break;
        }
    }

    temp = (can_msg[channel].recipe.vol>>16) & 0x000000FF;
    cmd_recipe[2] = temp;

    temp = (can_msg[channel].recipe.vol>>8) & 0x000000FF;
    cmd_recipe[3] = temp;

    temp = can_msg[channel].recipe.vol & 0x000000FF;
    cmd_recipe[4] = temp;

    temp = (can_msg[channel].recipe.cur>>16) & 0x000000FF;
    cmd_recipe[5] = temp;

    temp = (can_msg[channel].recipe.cur>>8) & 0x000000FF;
    cmd_recipe[6] = temp;

    temp = can_msg[channel].recipe.cur & 0x000000FF;
    cmd_recipe[7] = temp;


    //re-map tx cmd to rx cmd, expected rx cmd value update
//    for (i = 0; i < 8; i++) {
    for (i = 0; i < 9; i++) {
        if (remap_cmd_tx_rx[i].can_tx == cmd_recipe[0]) {
            can_msg[channel].remap_idx.cmd = i;
            break;
        }
    }

    //re-map tx op to rx op, expected rx op value update
    for (i = 0; i < 9; i++) {
        if (remap_op_tx_rx[i].can_tx == cmd_recipe[1]) {
            can_msg[channel].remap_idx.op = i;
            break;
        }
    }
    return 0;
}


static Uint16 app_CommCanOpMainProc(Uint16 channel)
{
    static Uint16 temp_pause_cnt=0;

    if (can_msg[channel].tx.seq != can_tx_seq_ready
            && can_msg[channel].tx.seq != can_tx_seq_complete
            && can_msg[channel].rx.alarm.flag==TRUE)
        return 0;

    switch (can_msg[channel].op.state)
    {
    case can_op_ui_connect:
        app_CommCanOpMsgSeqReady(cmd_null, can_rx_cmd_none, can_rx_op_idle, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {                   //if success, change can_msg.op state
            can_msg[channel].op.state = can_op_input_ready;
            can_msg[channel].power_ready_state.bit.connect = TRUE;           //all state mask
        }
        break;

    case can_op_input_ready:
        app_CommCanOpMsgSeqReady(cmd_in_rly_on, can_rx_cmd_input_relay_on, can_rx_op_input_ready, FALSE,channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {                   //if success, change can_msg.op state
            can_msg[channel].op.state = can_op_sensing_ready;
            can_msg[channel].power_ready_state.bit.input_on = TRUE;          //all state mask
        }
        break;

    case can_op_sensing_ready:
        app_CommCanOpMsgSeqReady(cmd_null, can_rx_cmd_AA, can_rx_op_AA, TRUE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {                   //if success, change can_msg.op state
            can_msg[channel].op.state = can_op_output_off;
            can_msg[channel].power_ready_state.bit.sensing = TRUE;           //all state mask
        }
        break;

    case can_op_stop:
        app_CommCanOpMsgSeqReady(cmd_stop, can_rx_cmd_stop, can_rx_op_idle, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {
            if(can_msg[channel].ui_evt.start.flag == TRUE) {
                if(can_msg[channel].power_ready_state.bit.input_on == FALSE) {
                    can_msg[channel].op.state = can_op_input_ready;
                } else {
                    can_msg[channel].ui_evt.start.flag = FALSE;
                    can_msg[channel].op.state = can_op_output_ready;         //if detect 'start' flag, start schedule process
                    switch(channel)
                    {
                    case 0:
                        timer_count = 1;
                        break;
                    case 1:
                        timer_count_2 = 1;
                        break;
                    case 2:
                        timer_count_3 = 1;
                        break;
                    case 3:
                        timer_count_4 = 1;
                        break;
                    }
                }
            } else{
                if(can_msg[channel].recipe.schedule_end_flag == TRUE){
                    can_msg[channel].recipe.step_idx = 0;
                    can_msg[channel].op.state = can_op_output_off;           //if detect 'schedule end' flag, end schedule process
                    //                    can_msg[channel].rx.schedule_end_cnt = TRUE;
                    can_msg[channel].rx.schedule_end_cnt++;
                    can_msg[channel].ui_evt.start.ignore_flag = FALSE;

                    can_msg[channel].recipe.schedule_end_flag = FALSE;
                }
#if 0
                if(can_msg[channel].recipe.step_change_flag == TRUE){
                    can_msg[channel].recipe.timer = 0;
                    can_msg[channel].recipe.step_change_flag = FALSE;
                    can_msg[channel].recipe.step_idx = can_msg[channel].end_cond.move_index;
                    can_msg[channel].op.state = can_op_cha_dis;
                }
#endif
            }
        }
        app_CommCanOpMsgInputMCOff(channel);                               //if detect @C c_cmd_alarm_clear(0x85), Input MC Off
        break;

    case can_op_output_ready:
        app_CommCanOpMsgSeqReady(cmd_out_rly_on, can_rx_cmd_output_relay_on, can_rx_op_all_ready, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {
            can_msg[channel].op.state = can_op_cha_dis;
            can_msg[channel].power_ready_state.bit.output_on = TRUE;         //all state mask
            can_msg[channel].recipe.step_idx = 1;
            can_msg[channel].recipe.timer = 0;
        }
        break;

    case can_op_cha_dis:
        if(can_msg[channel].recipe.step_idx == 0xFFFF)
        {
#if REST_BY_STEP == 1
            can_msg[channel].op.state = can_op_stop;
            can_msg[channel].recipe.schedule_end_flag = TRUE;
            can_msg[channel].ui_evt.start.ignore_flag = FALSE;
            can_msg[channel].recipe.step_change_flag = FALSE;
#else
            can_msg[channel].recipe.step_idx = 0;
            can_msg[channel].op.state = can_op_output_off;
            can_msg[channel].rx.schedule_end_cnt = TRUE;
            can_msg[channel].recipe.schedule_end_flag = FALSE;
#endif
            break;
        }
        app_CommCanOpRecipeUpdate(channel);

        app_CommCanOpMsgSeqReady(cmd_recipe, remap_cmd_tx_rx[can_msg[channel].remap_idx.cmd].can_rx,
                                 remap_op_tx_rx[can_msg[channel].remap_idx.op].can_rx, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {
            app_CommCanOpMsgSeqUpdateEndCondition(channel);
            app_CommCanOpMsgSeqCheckEndCondition(channel);

            if (can_msg[channel].ui_evt.pause.flag == TRUE) {
                switch (can_msg[channel].ui_evt.pause.cmd)
                {
                case p_cmd_pause:
                    can_msg[channel].end_cond.state=d_res_Pause;
                    //                        can_msg[channel].recipe.step_idx = 0xffff;
                    can_msg[channel].op.state = can_op_stop;
                    can_msg[channel].recipe.schedule_end_flag = TRUE;
                    can_msg[channel].recipe.step_change_flag = FALSE;

                    can_msg[channel].recipe.pause.time = can_msg[channel].recipe.timer;
                    can_msg[channel].recipe.pause.step_index = can_msg[channel].recipe.step_idx;

                case p_cmd_complete:
#if 0
                    temp_pause_cnt++;
                    can_msg[channel].end_cond.state=d_res_Complete;
                    if(temp_pause_cnt>=2)
                    {
                        temp_pause_cnt=0;
                        can_msg[channel].recipe.step_idx = 0xffff;
                        //                        can_msg[channel].op.state = can_op_stop;
                        //                        can_msg[channel].recipe.schedule_end_flag = TRUE;
                        //                        can_msg[channel].recipe.step_change_flag = FALSE;
                    }
#else
                    can_msg[channel].end_cond.state=d_res_Complete;
                    //                        can_msg[channel].recipe.step_idx = 0xffff;
                    can_msg[channel].op.state = can_op_stop;
                    can_msg[channel].recipe.schedule_end_flag = TRUE;
                    can_msg[channel].recipe.step_change_flag = FALSE;
#endif
                    break;

                case p_cmd_restart:
                    can_msg[channel].recipe.restart.flag = TRUE;
                    can_msg[channel].op.state = can_op_output_off;
                    can_msg[channel].ui_evt.start.flag = TRUE;

                    break;
                }
                can_msg[channel].ui_evt.pause.flag = FALSE;
            }
        }
        break;

    case can_op_output_off:
        can_msg[channel].end_cond.state = d_res_end_cond_none;
        app_CommCanOpMsgInputMCOff(channel);
        app_CommCanOpMsgSeqReady(cmd_out_rly_off, can_rx_cmd_output_relay_off, can_rx_op_input_ready, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2)
        {
            app_StartSeq(channel);
        }
        break;

    case can_op_input_off:
        app_CommCanOpMsgSeqReady(cmd_out_rly_off, can_rx_cmd_input_relay_off, can_rx_op_idle, FALSE, channel);
        if (app_CommCanOpMsgSeqComplete(channel) == 2) {
            can_msg[channel].op.state = can_op_stop;
            can_msg[channel].power_ready_state.bit.input_on = FALSE;         //all state mask
            can_msg[channel].recipe.schedule_end_flag = 0;
        }
        break;
    }
    return 0;
}

static Uint16 app_StartSeq(Uint16 channel)
{
    can_msg[channel].op.state = can_op_output_off;
    if(can_msg[channel].ui_evt.start.flag == TRUE)
    {
        can_msg[channel].recipe.step_idx = 0;
        can_msg[channel].op.timer=0;
        if(can_msg[channel].power_ready_state.bit.input_on == FALSE)
        {
            can_msg[channel].op.state = can_op_input_ready;
        }
        else
        {
            can_msg[channel].ui_evt.start.flag = FALSE;
            can_msg[channel].op.state = can_op_output_ready;
        }
    }
    else
    {
        can_msg[channel].power_ready_state.bit.output_on = FALSE;
    }
}

static Uint16 app_CommCanOpUiEvt(Uint16 channel)
{

    if (can_msg[channel].ui_evt.start.cnt != ui_msg_rx.recv.start[channel].cnt) {
        if (can_msg[channel].ui_evt.start.flag == FALSE
                &&(can_msg[channel].ui_evt.start.ignore_flag == FALSE))
        {
            can_msg[channel].ui_evt.start.flag = TRUE;
            can_msg[channel].ui_evt.start.ignore_flag = TRUE;
        }
        can_msg[channel].ui_evt.start.cnt = ui_msg_rx.recv.start[channel].cnt;
    }

    if (can_msg[channel].ui_evt.pause.cnt != ui_msg_rx.recv.pause[channel].cnt) {
        if (can_msg[channel].ui_evt.pause.flag == FALSE && can_msg[channel].recipe.step_idx != 0) {
            can_msg[channel].ui_evt.pause.flag = TRUE;
            can_msg[channel].ui_evt.pause.cmd = ui_msg_rx.recv.pause[channel].cmd;
        }
        can_msg[channel].ui_evt.pause.cnt = ui_msg_rx.recv.pause[channel].cnt;
    }

    if (can_msg[channel].ui_evt.jump.cnt != ui_msg_rx.recv.jump[channel].cnt) {
        if (can_msg[channel].ui_evt.jump.flag == FALSE) {
            can_msg[channel].ui_evt.jump.flag = TRUE;
            can_msg[channel].ui_evt.jump.step_idx = ui_msg_rx.recv.jump[channel].step_idx;
            can_msg[channel].ui_evt.jump.cnt = ui_msg_rx.recv.jump[channel].cnt;
            can_msg[channel].recipe.cycle_loop_cnt=0;
        }
    }

    if (can_msg[channel].ui_evt.command.cnt != ui_msg_rx.recv.command[channel].cnt) {
        if (can_msg[channel].ui_evt.command.flag == FALSE) {
            can_msg[channel].ui_evt.command.flag = TRUE;
            can_msg[channel].ui_evt.command.cmd = ui_msg_rx.recv.command[channel].cmd;
            can_msg[channel].ui_evt.command.cnt = ui_msg_rx.recv.command[channel].cnt;
        }
    }
    return 0;
}

static Uint16 app_CommCanOpMsgSeqUpdateEndCondition(Uint16 channel)
{
    static Uint16 old_step_index[4] = {0,0,0,0};

    can_msg[channel].end_cond.time = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_time_16;
    can_msg[channel].end_cond.time = (can_msg[channel].end_cond.time << 16) & 0xFFFF0000;
    can_msg[channel].end_cond.time |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_time_00 & (Uint32) 0x0000FFFF);

    can_msg[channel].end_cond.vol = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_v_16;
    can_msg[channel].end_cond.vol = (can_msg[channel].end_cond.vol << 16) & 0xFFFF0000;
    can_msg[channel].end_cond.vol |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_v_00 & (Uint32) 0x0000FFFF);

    can_msg[channel].end_cond.cur = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_i_16;
    can_msg[channel].end_cond.cur = (can_msg[channel].end_cond.cur << 16) & 0xFFFF0000;
    can_msg[channel].end_cond.cur |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_i_00 & (Uint32) 0x0000FFFF);

    can_msg[channel].end_cond.capa = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_capa_16;
    can_msg[channel].end_cond.capa = (can_msg[channel].end_cond.capa << 16) & 0xFFFF0000;
    can_msg[channel].end_cond.capa |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_capa_00 & (Uint32) 0x0000FFFF);

    can_msg[channel].end_cond.watt = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_watt_16;
    can_msg[channel].end_cond.watt = (can_msg[channel].end_cond.watt << 16) & 0xFFFF0000;
    can_msg[channel].end_cond.watt |= (gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_watt_00 & (Uint32) 0x0000FFFF);

    if(old_step_index[channel] != can_msg[channel].recipe.step_idx)	 //22.07.10 initialize capacity, watt
    {
        old_step_index[channel]=can_msg[channel].recipe.step_idx;
        can_msg[channel].cumulative_capacity = 0;
        can_msg[channel].cumulative_watt = 0;
    }

    return 0;
}

static Uint16 app_GoToNextStep(Uint16 channel, CommOpDResEndCond end_cond_state)
{
    static CommOpDResEndCond temp_end_cond[4] = {d_res_end_cond_none,};

    can_msg[channel].recipe.step_change_flag = TRUE;
    can_msg[channel].end_cond.state = end_cond_state;

    if(temp_end_cond[channel]!=can_msg[channel].end_cond.state)
    {
        can_msg[channel].end_cond.state_old=temp_end_cond[channel];
        temp_end_cond[channel]=can_msg[channel].end_cond.state;
    }

    if(gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].cycle_loop_cnt <= 1)
    {
        switch(end_cond_state)
        {
        case d_res_end_cond_time:
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_time_move_idx;
            break;
        case d_res_end_cond_i:
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_i_move_idx;
            break;
        case d_res_end_cond_capa:
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_capa_move_idx;
            break;
        case d_res_end_cond_watt:
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_watt_move_idx;
            break;
        case d_res_end_cond_v:
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_v_move_idx;
            break;
        }
    }
    else //Loop Count ???? ????
    {
        can_msg[channel].recipe.cycle_loop_cnt++;

        if(can_msg[channel].recipe.cycle_loop_cnt == gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].cycle_loop_cnt)
        {
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].cycle_loop_go_idx;
            can_msg[channel].recipe.cycle_loop_cnt=0;
        }
        else
        {
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].cycle_loop_re_idx;
        }
    }

    return 0;
}

static Uint16 app_CommCanOpMsgSeqCheckEndCondition(Uint16 channel)
{
    static Uint16 temp_end_cond = 0;
    Uint32 cumulative_capacity = 0;

#if REST_BY_STEP
    if(can_msg[channel].end_cond.vol != 0){     // Voltage cut
        if(can_msg[channel].recipe.cmd == d_req_cmd_charge){
            if(can_msg[channel].rx.vol >= can_msg[channel].end_cond.vol){
                app_GoToNextStep(channel, d_res_end_cond_v);
            }
        }
        else if (can_msg[channel].recipe.cmd == d_req_cmd_discharge){
            if(can_msg[channel].rx.vol <= can_msg[channel].end_cond.vol){
                app_GoToNextStep(channel, d_res_end_cond_v);
            }
        }
    }

    if(can_msg[channel].end_cond.cur != 0){     // Current cut
        if((can_msg[channel].rx.op == can_rx_op_cha_cv_set) || (can_msg[channel].rx.op == can_rx_op_dis_cv_set))
        {
            if( can_msg[channel].rx.curr <= can_msg[channel].end_cond.cur){
                app_GoToNextStep(channel, d_res_end_cond_i);
            }
        }
    }

    if(can_msg[channel].end_cond.capa != 0){    // Capacity(Ah) cut
        if(can_msg[channel].recipe.cmd == d_req_cmd_charge || can_msg[channel].recipe.cmd == d_req_cmd_discharge){
            cumulative_capacity = (Uint32) (can_msg[channel].cumulative_capacity * 1000);
            if(cumulative_capacity >= can_msg[channel].end_cond.capa){
                app_GoToNextStep(channel, d_res_end_cond_capa);
            }
        }
    }

    if(can_msg[channel].end_cond.watt != 0){    // WattHour(Wh) cut
//        if(can_msg[channel].recipe.cmd == d_req_cmd_charge || can_msg[channel].recipe.cmd == d_req_cmd_discharge){
//            if(can_msg[channel].cumulative_watt >= can_msg[channel].end_cond.watt){
//                app_GoToNextStep(channel, d_res_end_cond_watt);
//            }
//        }
    }

    if(can_msg[channel].end_cond.time != 0){    // Time cut
        if( can_msg[channel].recipe.timer >= can_msg[channel].end_cond.time - 1 ){
            app_GoToNextStep(channel, d_res_end_cond_time);
        }
    }

#else
    if(can_msg[channel].recipe.step_change_flag == TRUE){
        can_msg[channel].recipe.timer = 0;
        can_msg[channel].recipe.step_change_flag = FALSE;
        can_msg[channel].recipe.step_idx = can_msg[channel].end_cond.move_index;
    }

    if(can_msg[channel].end_cond.vol != 0){
        if(can_msg[channel].recipe.cmd == d_req_cmd_charge){
            if(can_msg[channel].rx.vol >= can_msg[channel].end_cond.vol){
                can_msg[channel].recipe.step_change_flag = TRUE;
                can_msg[channel].op.state = can_op_cha_dis;
                can_msg[channel].recipe.timer = 0;
                can_msg[channel].end_cond.state = d_res_end_cond_v;
                can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_v_move_idx;
            }
        }
        else if (can_msg[channel].recipe.cmd == d_req_cmd_discharge){
            if(can_msg[channel].rx.vol <= can_msg[channel].end_cond.vol){
                can_msg[channel].recipe.step_change_flag = TRUE;
                can_msg[channel].op.state = can_op_cha_dis;
                can_msg[channel].recipe.timer = 0;
                can_msg[channel].end_cond.state = d_res_end_cond_v;
                can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_v_move_idx;
            }
        }
    }
    if(can_msg[channel].end_cond.cur != 0){
        if( can_msg[channel].rx.curr <= can_msg[channel].end_cond.cur ){
            can_msg[channel].recipe.step_change_flag = TRUE;
            can_msg[channel].op.state = can_op_cha_dis;
            can_msg[channel].recipe.timer = 0;
            can_msg[channel].end_cond.state = d_res_end_cond_i;
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_i_move_idx;

        }
    }
    //TODO add CAPA
    if(can_msg[channel].end_cond.capa != 0){

    }
    if(can_msg[channel].end_cond.watt != 0){

    }
    if(can_msg[channel].end_cond.time != 0){
        if( can_msg[channel].recipe.timer >= can_msg[channel].end_cond.time ){
            can_msg[channel].recipe.step_change_flag = TRUE;
            can_msg[channel].op.state = can_op_cha_dis;
            can_msg[channel].end_cond.move_index = gp_sdram[channel]->total_step[can_msg[channel].recipe.step_idx].end_cond_time_move_idx;
            can_msg[channel].recipe.timer = 0;
            can_msg[channel].end_cond.state = d_res_end_cond_time;
        }
    }
#endif
    return 0;
}

static Uint16 app_CommCanOpMsgAlarmClear(Uint16 channel)
{
    if (can_msg[channel].ui_evt.command.flag == TRUE) {
        switch (can_msg[channel].ui_evt.command.cmd)
        {
        case c_cmd_alarm_clear:
            drv_WriteCAN(drv_can_msg.obj[TX_ID_100+(OBJ_MAX_1CH*channel)].obj_id, cmd_error_clear, 8);
            break;
        }
        can_msg[channel].ui_evt.command.flag = FALSE;
    }
    return 0;
}

static Uint16 app_CommCanOpMsgInputMCOff(Uint16 channel)
{
    if (can_msg[channel].ui_evt.command.flag == TRUE) {
        switch (can_msg[channel].ui_evt.command.cmd)
        {
        case c_cmd_input_relay_off:
            can_msg[channel].power_ready_state.all = 0;
            drv_WriteCAN(drv_can_msg.obj[TX_ID_100+(OBJ_MAX_1CH*channel)].obj_id, cmd_in_rly_off, 8);
            break;
        }
        //        can_msg[channel].ui_evt.command.flag = FALSE;
    }
    return 0;
}

static Uint16 app_CommCanOpRecvProc(Uint16 channel)
{
    Uint32 temp16=0, temp00=0;

    //Report #1, Power State
    if (can_msg[channel].rx.op_report_cnt != drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].cnt) {    // Every 100ms
        app_CommCanOpPowerUpdateVar(channel);
        can_msg[channel].rx.op_report_cnt = drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].cnt;
        if(can_msg[channel].recipe.step_change_flag == TRUE)
        {
            can_msg[channel].recipe.timer = 0;
            can_msg[channel].recipe.step_change_flag = FALSE;
            can_msg[channel].recipe.step_idx = can_msg[channel].end_cond.move_index;
            can_msg[channel].op.state = can_op_cha_dis;
            can_msg[channel].end_cond.state = d_res_end_cond_none;
        }
    }

    //Report #2, Power Alarm
    if (drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].cnt != can_msg[channel].rx.alarm_report_cnt) {
        can_msg[channel].rx.alarm_report_cnt = drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].cnt;

        //        can_msg[channel].rx.alarm[0] = 0x0000;           //Not check. Only Command&Operation mode
        can_msg[channel].rx.alarm.warning.all = 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[0];
        can_msg[channel].rx.alarm.warning.all <<= 8;
        can_msg[channel].rx.alarm.warning.all |= 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[1];

        can_msg[channel].rx.alarm.dc.all = 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[2];
        can_msg[channel].rx.alarm.dc.all <<= 8;
        can_msg[channel].rx.alarm.dc.all |= 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[3];

        can_msg[channel].rx.alarm.bat.all = 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[4];
        can_msg[channel].rx.alarm.bat.all <<= 8;
        can_msg[channel].rx.alarm.bat.all |= 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[5];

        can_msg[channel].rx.alarm.fault.all = 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[6];
        can_msg[channel].rx.alarm.fault.all <<= 8;
        can_msg[channel].rx.alarm.fault.all |= 0x00FF & drv_can_msg.obj[RX_ID_120+(OBJ_MAX_1CH*channel)].data[7];

        can_msg[channel].op.power_error = can_msg[channel].rx.alarm.warning.all;
        can_msg[channel].op.power_error |= can_msg[channel].rx.alarm.dc.all;
        can_msg[channel].op.power_error |= can_msg[channel].rx.alarm.bat.all;
        can_msg[channel].op.power_error |= can_msg[channel].rx.alarm.fault.all;

        if(can_msg[channel].op.power_error != 0)
        {

            can_msg[channel].rx.alarm.flag = TRUE;
            app_CommCanOpInit(channel, TRUE);
        }
        else
        {
            can_msg[channel].rx.alarm.flag = FALSE;
        }
    }

/*      DAQ?? ????
    //    Report #3, Thermal
    if (drv_can_msg.obj[RX_ID_130+(OBJ_MAX_1CH*channel)].cnt != can_msg[channel].rx.thermal_report_cnt) {
        can_msg[channel].rx.thermal_report_cnt = drv_can_msg.obj[RX_ID_130+(OBJ_MAX_1CH*channel)].cnt;

        temp16 = 0x00FF & drv_can_msg.obj[RX_ID_130+(OBJ_MAX_1CH*channel)].data[2];
        temp00 = 0x00FF & drv_can_msg.obj[RX_ID_130+(OBJ_MAX_1CH*channel)].data[3];
        temp00 <<= 8;
        temp00 |= 0x00FF & drv_can_msg.obj[RX_ID_130+(OBJ_MAX_1CH*channel)].data[4];

        can_msg[channel].rx.thermal = ((temp16 << 16) | temp00);

        gp_sdram[channel]->total_log.sen_thermal_16 = temp16;
        gp_sdram[channel]->total_log.sen_thermal_00 = temp00;


    }
*/

    return 0;
}
Uint16 g_debug_op_cnt=0;

static void app_CommCanOpPowerUpdateVar(Uint16 channel)     
{
    Uint32 temp16 = 0;
    Uint32 temp00 = 0;
    Uint16 i;
//    static Uint16 old_step_index[4] = {0,0,0,0};
    static CommCanRxCMDList temp_old_cmd = can_rx_cmd_none;
    static CommCanRxOPModeList temp_old_op = can_rx_op_idle;

    if(temp_old_cmd != can_msg[channel].rx.cmd)
    {
        can_msg[channel].rx.cmd_old = temp_old_cmd;
        temp_old_cmd = can_msg[channel].rx.cmd;
    }


    gp_sdram[channel]->total_log.sampling_time = d_sampling_100ms;
    gp_sdram[channel]->total_log.step_idx = can_msg[channel].recipe.step_idx;
    //    gp_sdram[channel]->total_log.end_cond_state = d_res_end_cond_none;

    //    //for test
    //    if(can_msg[channel].recipe.step_change_cnt >= 2)
    //    {
    //
    //        can_msg[channel].end_cond.debug_end_cnt++;
    //        gp_sdram[channel]->total_log.end_cond_state = (CommOpDResEndCond)(can_msg[channel].end_cond.state_old);
    //    }
    //    else
    //    {
    //        gp_sdram[channel]->total_log.end_cond_state = (CommOpDResEndCond)(can_msg[channel].end_cond.state);
    //    }

    gp_sdram[channel]->total_log.end_cond_state = (CommOpDResEndCond)(can_msg[channel].end_cond.state);

#if 0 //for debugging
    if(can_msg[channel].end_cond.logging_flag==TRUE)
    {
        can_msg[channel].end_cond.logging_flag=FALSE;
        //        gp_sdram[channel]->total_log.end_cond_state = (CommOpDResEndCond)(can_msg[channel].end_cond.state_old);
        if(temp_old_op != can_msg[channel].rx.op)
        {
            can_msg[channel].rx.op_old=temp_old_op;
            temp_old_op=can_msg[channel].rx.op;
            for (i = 0; i < 10; i++) {
                if (remap_op_rx_log[i].can_rx == can_msg[channel].rx.op_old) {
                    gp_sdram[channel]->total_log.operation = remap_op_rx_log[i].log;
                    break;
                }
            }
        }
        else
        {
            can_msg[channel].rx.op = (CommCanRxOPModeList) (drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[1] & 0xff);
            for (i = 0; i < 10; i++) {
                if (remap_op_rx_log[i].can_rx == can_msg[channel].rx.op) {
                    g_debug_op_cnt++;
                    gp_sdram[channel]->total_log.operation = remap_op_rx_log[i].log;
                    break;
                }
            }
        }
    }
    else
#endif
    {
        can_msg[channel].rx.op = (CommCanRxOPModeList) (drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[1] & 0xff);
        for (i = 0; i < 10; i++) {
            if (remap_op_rx_log[i].can_rx == can_msg[channel].rx.op) {
                g_debug_op_cnt++;
                gp_sdram[channel]->total_log.operation = remap_op_rx_log[i].log;
                break;
            }
        }
    }

    //Re-map can rx cmd to log cmd & update log
    can_msg[channel].rx.cmd = (CommCanRxCMDList) drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[0];
    for (i = 0; i < 9; i++) {
        if (remap_cmd_rx_log[i].can_rx == can_msg[channel].rx.cmd) {
            gp_sdram[channel]->total_log.command = remap_cmd_rx_log[i].log;
            break;
        }
    }

    //Can RX OP Mode to Log OP & update log


    //Can RX MODE Mode to Log MODE & update log
    can_msg[channel].rx.par = (CommCanRxParallelModeList) (drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[1] & 0x0F);
    for (i = 0; i < 3; i++) {
        if (remap_par_rx_log[i].can_rx == can_msg[channel].rx.par) {
            gp_sdram[channel]->total_log.step = remap_par_rx_log[i].log;
            break;
        }
    }

    //voltage update log
    temp16 = 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[2];
    temp00 = 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[3];
    temp00 <<= 8;
    temp00 |= 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[4];

    gp_sdram[channel]->total_log.sen_v_16 = temp16;
    gp_sdram[channel]->total_log.sen_v_00 = temp00;

    can_msg[channel].rx.vol = ((temp16 << 16) | temp00) * 0.1;

    //current update log
    temp16 = 0;
    temp00 = 0;
    temp16 = 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[5];
    temp00 = 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[6];
    temp00 <<= 8;
    temp00 |= 0x00FF & drv_can_msg.obj[RX_ID_110+(OBJ_MAX_1CH*channel)].data[7];

    gp_sdram[channel]->total_log.sen_i_16 = temp16;
    gp_sdram[channel]->total_log.sen_i_00 = temp00;

    can_msg[channel].current = (float)((temp16 << 16) | temp00) / 10000.0;	// 22.07.10
    can_msg[channel].rx.curr = ((temp16 << 16) | temp00) * 0.1;

    //time update log
    gp_sdram[channel]->total_log.time_64 = (can_msg[channel].op.timer >> 48) & 0x000000000000FFFF;
    gp_sdram[channel]->total_log.time_32 = (can_msg[channel].op.timer >> 32) & 0x000000000000FFFF;
    gp_sdram[channel]->total_log.time_16 = (can_msg[channel].op.timer >> 16) & 0x000000000000FFFF;
    gp_sdram[channel]->total_log.time_00 = (can_msg[channel].op.timer >> 0)  & 0x000000000000FFFF;

    //value at the end update log
    gp_sdram[channel]->total_log.end_cond_val_16 = 1;
    gp_sdram[channel]->total_log.end_cond_val_00 = 1;

    //cycle update log
    gp_sdram[channel]->total_log.cycle_loop_cnt = can_msg[channel].recipe.cycle_loop_cnt;

    //Clear @Each Step
//    if(old_step_index[channel] != can_msg[channel].recipe.step_idx)
//    {
//        old_step_index[channel]=can_msg[channel].recipe.step_idx;
//        can_msg[channel].cumulative_capacity = 0;
//        can_msg[channel].cumulative_watt = 0;
//    }

    //capacity log @Each Step
    if(can_msg[channel].recipe.cmd == d_req_cmd_charge || can_msg[channel].recipe.cmd == d_req_cmd_discharge)
    {
        can_msg[channel].cumulative_capacity += can_msg[channel].current * (1.0 / 3600.0) * 0.1;  //Ah, cumulative capacity during 100ms.
    }

    can_msg[channel].rx.capa = (Uint32)((can_msg[channel].current * (1.0 / 3600.0) * 0.1) * 10000000000);  // 0?? 10?? ?????? ????.
    gp_sdram[channel]->total_log.sen_capa_16 = (can_msg[channel].rx.capa>>16) &0xffff;
    gp_sdram[channel]->total_log.sen_capa_00 = (can_msg[channel].rx.capa>>0) &0xffff;

    //Watt log
    can_msg[channel].cumulative_watt = (Uint32)((can_msg[channel].rx.vol * can_msg[channel].rx.curr) * (1.0 / 3600.0) * 0.1);  // cumulative watt during 100ms.
    can_msg[channel].cumulative_watt += can_msg[channel].cumulative_watt;
}

void app_CanOpVarInit(void)
{
    memset(&can_msg, NULL, sizeof(AppCommCanOpHandle)*4);
}

Uint16 app_PowerInitSeq(void)
{
    //TODO
    return 0;
}

Uint16 app_PowerOffSeq(void)
{
    //TODO
    return 0;
}

void app_CommCanTimer(void)
{
    int i;
    for(i=0;i<MAX_CHANNEL;i++)
    {
        can_msg[i].rx.time++;           //Rx time
        //        can_msg[i].op.timer++;          //Total system run time
        can_msg[i].rx.time_out++;       //Rx Check
        can_msg[i].tx.timer++;          //Tx Interval

        //        if ((timer_count % 100) == 0) {
        //            can_msg[i].recipe.timer++;  //Recipe cond
        //        }
        if ((++can_msg[i].op.timer % 100) == 0) {
            can_msg[i].recipe.timer++;  //Recipe cond
        }
    }
}
