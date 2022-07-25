/*
 * app_comm_ui.c
 *
 *  Created on: 2021. 12. 15.
 *      Author: JOO
 */

//TODO CAN_enableRetry
//TODO CAN_getErrorCount

#include <string.h>
#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "drv_can.h"
#include "app_comm.h"
#include "app_settings.h"

#define CHANNEL 1

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(app_UpdateCommCmd, ".TI.ramfunc");
#pragma CODE_SECTION(app_PushReport, ".TI.ramfunc");
#endif
#endif

#define app_UpdateCmdVal(idx)                                                  \
{                                                                              \
    app_comm_ui_cmd.cmd        = drv_can_msg.obj[idx].data[0];                 \
    app_comm_ui_cmd.op         = (drv_can_msg.obj[idx].data[1] >> 4) & 0x0F;   \
    app_comm_ui_cmd.parallel   = drv_can_msg.obj[idx].data[1] & 0x0F;          \
    app_comm_ui_cmd.volts      = drv_can_msg.obj[idx].data[2] & 0x00FF;        \
    app_comm_ui_cmd.volts      <<= 8;                                          \
    app_comm_ui_cmd.volts      |= drv_can_msg.obj[idx].data[3] & 0x00FF;       \
    app_comm_ui_cmd.volts      <<= 8;                                          \
    app_comm_ui_cmd.volts      |= drv_can_msg.obj[idx].data[4] & 0x00FF;       \
    app_comm_ui_cmd.conv_volts = (float) app_comm_ui_cmd.volts * 0.001f;       \
    app_comm_ui_cmd.amps       = drv_can_msg.obj[idx].data[5] & 0x00FF;        \
    app_comm_ui_cmd.amps       <<= 8;                                          \
    app_comm_ui_cmd.amps       |= drv_can_msg.obj[idx].data[6] & 0x00FF;       \
    app_comm_ui_cmd.amps       <<= 8;                                          \
    app_comm_ui_cmd.amps       |= drv_can_msg.obj[idx].data[7] & 0x00FF;       \
    app_comm_ui_cmd.conv_amps  = (float) app_comm_ui_cmd.amps * 0.001f;        \
}                                                                              \

DrvSetCanMsg app_can_msg_set_table[33] = {
    { .can_id = NULL, },

    //From UI
    { .can_id = 0xAA,               .type = CAN_MSG_OBJ_TYPE_RX, .flag = CAN_MSG_OBJ_RX_INT_ENABLE },

    { .can_id = 0xFF,               .type = CAN_MSG_OBJ_TYPE_RX, .flag = CAN_MSG_OBJ_RX_INT_ENABLE },
    { .can_id = 0x100 * CHANNEL,    .type = CAN_MSG_OBJ_TYPE_RX, .flag = CAN_MSG_OBJ_RX_INT_ENABLE },

    //To UI
    { .can_id = 0x110 * CHANNEL,    .type = CAN_MSG_OBJ_TYPE_TX, .flag = CAN_MSG_OBJ_NO_FLAGS },
    { .can_id = 0x120 * CHANNEL,    .type = CAN_MSG_OBJ_TYPE_TX, .flag = CAN_MSG_OBJ_NO_FLAGS },

    { .can_id = NULL, },
};

AppCommUiCommand app_comm_ui_cmd;
AppCommUiReport app_comm_ui_report1;
AppCommUiFault app_comm_ui_report2;

uint16_t dbg_push_report1[8];
uint16_t dbg_push_report2[8];

uint32_t comm_ui_push_timer = 0;

void app_InitComm(void)
{
    memcpy(drv_can_msg_set_table, app_can_msg_set_table, sizeof(drv_can_msg_set_table));
}

void app_UpdateCommCmd(void)
{
    comm_ui_push_timer++;

    app_UpdateCmdVal(link_rx_ui_cmd_normal);
}

void app_PushReport(void)
{
    if (comm_ui_push_timer >= REF_MS_TIME(100)) {

        drv_Write8CAN(link_tx_ui_state, dbg_push_report1);
        drv_Write8CAN(link_tx_ui_error, dbg_push_report1);
        dbg_push_report1[0]++;

        comm_ui_push_timer = 0;
    }
}

//
// End of File
//