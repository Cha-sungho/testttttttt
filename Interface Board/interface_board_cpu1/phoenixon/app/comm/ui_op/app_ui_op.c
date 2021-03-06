/*
 * app_ui_protocol.c
 *
 *  Created on: 2021. 8. 21.
 *      Author: JOO
 */
#include <stdio.h>
#include <string.h>
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include "socket.h"
#include "wizchip_conf.h"

#pragma CODE_SECTION(app_CommUIOpRun, ".TI.ramfunc");
#pragma CODE_SECTION(cb_TcpEstablished, ".TI.ramfunc");
#pragma CODE_SECTION(cb_TcpClosedWait, ".TI.ramfunc");
#pragma CODE_SECTION(cb_TcpListen, ".TI.ramfunc");
#pragma CODE_SECTION(cb_TcpClosed, ".TI.ramfunc");
#pragma CODE_SECTION(cb_Data, ".TI.ramfunc");
#pragma CODE_SECTION(cb_Jump, ".TI.ramfunc");
#pragma CODE_SECTION(cb_Command, ".TI.ramfunc");
#pragma CODE_SECTION(cb_Start, ".TI.ramfunc");
#pragma CODE_SECTION(cb_Pause, ".TI.ramfunc");
#pragma CODE_SECTION(OpParser, ".TI.ramfunc");
#pragma CODE_SECTION(OpSender, ".TI.ramfunc");
#pragma CODE_SECTION(CopyFromEtherBuffer, ".TI.ramfunc");
#pragma CODE_SECTION(CopyToEtherBuffer, ".TI.ramfunc");

static void cb_TcpEstablished(uint16_t sock, uint16_t type, uint16_t port);
static void cb_TcpClosedWait(uint16_t sock, uint16_t type, uint16_t port);
static void cb_TcpListen(uint16_t sock, uint16_t type, uint16_t port);
static void cb_TcpClosed(uint16_t sock, uint16_t type, uint16_t port);

static AppCommUIOpErrList cb_Data(uint8_t sock);
static AppCommUIOpErrList cb_Jump(uint8_t sock);
static AppCommUIOpErrList cb_Command(uint8_t sock);
static AppCommUIOpErrList cb_Start(uint8_t sock);
static AppCommUIOpErrList cb_Pause(uint8_t sock);

static void CommUIOpMsgHandleInit(void);
static AppCommUIOpErrList OpParser(uint16_t sock);
static AppCommUIOpErrList OpSender(uint16_t sock, uint16_t channel);
static AppCommUIOpErrList CopyFromEtherBuffer(uint8_t sock, uint8_t *buf, uint16_t len);
static AppCommUIOpErrList CopyToEtherBuffer(uint8_t sock, uint8_t *buf, uint16_t len);


const AppCommUIOpInitVal app_ui_tcp_init_val = {
                                                .sock = 0,
                                                .type = Sn_MR_TCP,
                                                .port = 5000
};

const AppCommUIOpSockState app_ui_tcp_state_table[] =
{
 { SOCK_ESTABLISHED, cb_TcpEstablished },
 { SOCK_CLOSE_WAIT,  cb_TcpClosedWait },
 { SOCK_INIT,        cb_TcpListen },
 { SOCK_CLOSED,      cb_TcpClosed },
 { 0,                NULL }
};

const AppUICmdState app_ui_cmd_table[] =
{
 { head_data,    cb_Data },
 { head_jump,    cb_Jump },
 { head_cmd,     cb_Command },
 { head_start,   cb_Start },
 { head_pause,   cb_Pause },
 { head_none,    NULL }
};

volatile TotalStepLog *gp_sdram[4];
__attribute__((far)) volatile TotalStepLog g_sdram[4];

AppCommUIOpHandle_RX ui_msg_rx;
AppCommUIOpHandle_TX ui_msg_tx[4];

void app_CommUIOpDataMapping(void)
{
    Uint16 i;
    Uint32 temp;

    for (i = 0; i < 4; i++) {
        temp = (Uint32) &(g_sdram[i]);
        temp |= EMIF_CS0_BASE_ADDRESS;
        gp_sdram[i] = (TotalStepLog*) (temp);
    }
}

static void CommUIOpMsgHandleInit(void)
{
    memset(&ui_msg_rx, NULL, sizeof(AppCommUIOpHandle_RX));
}

int32_t app_CommUIOpRun(void)
{
    uint8_t sock_state;
    uint8_t i;

    sock_state = getSn_SR(app_ui_tcp_init_val.sock);

    for (i = 0; app_ui_tcp_state_table[i].process != NULL; i++) {
        if (app_ui_tcp_state_table[i].state == sock_state) {
            app_ui_tcp_state_table[i].process(app_ui_tcp_init_val.sock, app_ui_tcp_init_val.type, app_ui_tcp_init_val.port);
            ui_msg_rx.sock_state = sock_state;
            break;
        }
    }
    return 0;
}

static void cb_TcpEstablished(uint16_t sock, uint16_t type, uint16_t port)
{
    uint8_t destip[4];
    uint16_t destport;
    AppCommUIOpErrList ret = op_error_none;
    uint16_t i;

    if (getSn_IR(sock) & Sn_IR_CON) {
        getSn_DIPR(sock, destip);
        destport = getSn_DPORT(sock);

        printf("%s-%s-%d %d: UI TCP server Connected - %d.%d.%d.%d : %u\r\n", __FILE__, __FUNCTION__, __LINE__, sock, destip[0], destip[1], destip[2], destip[3], destport);
        setSn_IR(sock, Sn_IR_CON);
        send(sock, '\0', 1);
    }

    ret = OpParser(sock);
    if(ret != op_error_none) {
        printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
        CommUIOpMsgHandleInit();
    }

    //XXX Sender channel ???? ???????????? ?
    for(i=0;i<MAX_CHANNEL;i++)
    {
        ret = OpSender(sock, i);
        if(ret != op_error_none) {
//            printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

static void cb_TcpClosedWait(uint16_t sock, uint16_t type, uint16_t port)
{
    int32_t ret;

    printf("%s-%s-%d %d: UI TCP server close wait\r\n", __FILE__, __FUNCTION__, __LINE__, sock);

    if ((ret = disconnect(sock)) != SOCK_OK)
        printf("%s-%s-%d %d: UI TCP server close error %ld\r\n", __FILE__, __FUNCTION__, __LINE__, sock, ret);
    else
        printf("%s-%s-%d %d: UI TCP server closed\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
}

static void cb_TcpListen(uint16_t sock, uint16_t type, uint16_t port)
{
    int32_t ret;

    printf("%s-%s-%d %d: UI TCP server listen\r\n", __FILE__, __FUNCTION__, __LINE__, sock);

    if ((ret = listen(sock)) != SOCK_OK)
        printf("%s-%s-%d %d: UI TCP server listen error %ld\r\n", __FILE__, __FUNCTION__, __LINE__, sock, ret);
    else
        printf("%s-%s-%d %d: UI TCP server listened\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
}

static void cb_TcpClosed(uint16_t sock, uint16_t type, uint16_t port)
{
    int32_t ret;

    printf("%s-%s-%d %d: UI TCP server start\r\n", __FILE__, __FUNCTION__, __LINE__, sock);

    if ((ret = socket(sock, type, port, 0x00)) != sock)
        printf("%s-%s-%d %d: UI TCP server open error %ld\r\n", __FILE__, __FUNCTION__, __LINE__, sock, ret);
    else {
        CommUIOpMsgHandleInit();
        //setSn_KPALVTR(sock, 1);
        printf("%s-%s-%d %d: UI TCP server opened\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
    }
}

static AppCommUIOpErrList OpParser(uint16_t sock)
{
    AppCommUIOpErrList ret = op_error_none;
    uint16_t size = getSn_RX_RSR(sock);
    uint16_t i;

    if (size > 0) {
        //printf("%s-%s-%d %d: recv size - %u\r\n", __FILE__, __FUNCTION__, __LINE__, sock, size);

        if (ui_msg_rx.recv.remain_step_cnt == 0) {
            //Header, Channel, Len copy&check
            ret = CopyFromEtherBuffer(sock, &ui_msg_rx.recv.head, UI_HEADER_SIZE);
            if (ret != op_error_none) return ret;
            if (ui_msg_rx.recv.head != head_data &&
                    ui_msg_rx.recv.head != head_jump &&
                    ui_msg_rx.recv.head != head_cmd &&
                    ui_msg_rx.recv.head != head_start &&
                    ui_msg_rx.recv.head != head_pause) return op_error_header;
        }

        //Remain data copy&check
        for (i = 0; app_ui_cmd_table[i].process != NULL; i++) {
            if (app_ui_cmd_table[i].head == ui_msg_rx.recv.head) {
                ret = app_ui_cmd_table[i].process(sock);
                if (ret != op_error_none) return ret;
                break;
            }
        }
    }

    //step loading exception & complete check
    if(ui_msg_rx.recv.remain_step_cnt != 0) {
        if(ui_msg_rx.recv.remain_step_cnt_old != ui_msg_rx.recv.remain_step_cnt) {
            ui_msg_rx.recv.remain_step_cnt_old = ui_msg_rx.recv.remain_step_cnt;
            ui_msg_rx.step_load_time_out = 0;
        } else {
            if(ui_msg_rx.step_load_time_out > 2000) {
                ui_msg_rx.recv.act_step_cnt = 0;
                ui_msg_rx.recv.remain_step_cnt = 0;
                ui_msg_rx.recv.remain_step_cnt_old = 0;
                ui_msg_rx.recv.step_loaded = FALSE;
            }
        }
    }

    return ret;
}

static AppCommUIOpErrList cb_Data(uint8_t sock)
{
    AppCommUIOpErrList ret = op_error_none;
    StepUnit step_temp;
    uint16_t i;

    if(ui_msg_rx.recv.remain_step_cnt == 0) {
        ui_msg_rx.recv.act_step_cnt = 0;
        ui_msg_rx.recv.remain_step_cnt = ui_msg_rx.recv.len;
        ui_msg_rx.recv.remain_step_cnt_old = 0;
        ui_msg_rx.recv.step_loaded = FALSE;
        ui_msg_rx.step_load_time_out = 0;
    }

    ret = CopyFromEtherBuffer(sock, (uint8_t*) &step_temp, sizeof(StepUnit));
    if (ret != op_error_none) return ret;

    for (i = 0; i < 4; i++) {
        if((ui_msg_rx.recv.channel >> i) & 0x01) {
            memcpy_fast_far(&gp_sdram[i]->total_step[ui_msg_rx.recv.act_step_cnt], &step_temp, sizeof(StepUnit));
        }
    }

    ui_msg_rx.recv.act_step_cnt++;
    ui_msg_rx.recv.remain_step_cnt = ui_msg_rx.recv.len - ui_msg_rx.recv.act_step_cnt;

    if(ui_msg_rx.recv.act_step_cnt == ui_msg_rx.recv.len) {
        ui_msg_rx.recv.step_loaded = TRUE;
    }

    return ret;
}

static AppCommUIOpErrList cb_Jump(uint8_t sock)
{
    uint16_t i;
    AppCommUIOpErrList ret = op_error_none;
    uint16_t cal_size = ui_msg_rx.recv.len - (UI_HEADER_SIZE * 2);

    if (cal_size < UI_JUMP_SIZE) return op_error_len;

    for (i = 0; i < 4; i++) {
        if(ui_msg_rx.recv.channel>>i & 0x01) {
            ret = CopyFromEtherBuffer(sock, (uint8_t*) &ui_msg_rx.recv.jump[i], UI_JUMP_SIZE);
            if (ret != op_error_none) return ret;
            ui_msg_rx.recv.jump[i].cnt++;
        }
    }
    printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
    return ret;
}

static AppCommUIOpErrList cb_Command(uint8_t sock)
{
    uint16_t i;
    AppCommUIOpErrList ret = op_error_none;
    uint16_t cal_size = ui_msg_rx.recv.len - (UI_CMD_SIZE * 2);

    if (cal_size < UI_CMD_SIZE) return op_error_len;

    for (i = 0; i < 4; i++) {
        if(ui_msg_rx.recv.channel>>i & 0x01) {
            ret = CopyFromEtherBuffer(sock, (uint8_t*) &ui_msg_rx.recv.command[i], UI_CMD_SIZE);
            if (ret != op_error_none) return ret;
            ui_msg_rx.recv.command[i].cnt++;
            //    printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
    printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
    return ret;
}

static AppCommUIOpErrList cb_Pause(uint8_t sock)
{
    uint16_t i,j;
    AppCommUIOpErrList ret = op_error_none;
    uint16_t cal_size = ui_msg_rx.recv.len - (UI_PAUSE_SIZE * 2);


    if (cal_size < UI_PAUSE_SIZE) return op_error_len;

    for (i = 0; i < 4; i++) {
        if(ui_msg_rx.recv.channel>>i & 0x01) {
            ret = CopyFromEtherBuffer(sock, (uint8_t*) &ui_msg_rx.recv.pause[i], UI_PAUSE_SIZE);
            if (ret != op_error_none) return ret;
            ui_msg_rx.recv.pause[i].cnt++;
#ifdef PNX_DEBUG
            printf("head-0x%2x\r\n", ui_msg_rx.recv.head);
            printf("len-0x%2x\r\n", ui_msg_rx.recv.len);
            printf("channel-0x%2x\r\n", ui_msg_rx.recv.channel);
            printf("data[%d]-0x%02x\r\n", i, ui_msg_rx.recv.pause[i].cmd);
#endif
            //            printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
    printf("%s-%s-%d\r\n", __FILE__, __FUNCTION__, __LINE__);
    return ret;
}

static AppCommUIOpErrList cb_Start(uint8_t sock)
{
    uint16_t i;
    AppCommUIOpErrList ret = op_error_none;
    uint16_t cal_size = ui_msg_rx.recv.len - (UI_START_SIZE * 2);

    if (cal_size < UI_START_SIZE) return op_error_len;

    for (i = 0; i < 4; i++) {
        if((ui_msg_rx.recv.channel >> i) & 0x01) {
            ret = CopyFromEtherBuffer(sock, (uint8_t*) &ui_msg_rx.recv.start[i], UI_START_SIZE);
            if (ret != op_error_none) return ret;

            printf("%s-%s-%d-[channel : %d]\r\n", __FILE__, __FUNCTION__, __LINE__,i);
            ui_msg_rx.recv.start[i].cnt++;
        }
    }


    return ret;
}

static AppCommUIOpErrList OpSender(uint16_t sock, uint16_t channel)
{
    AppCommUIOpErrList ret = op_error_none;

    if(ui_msg_tx[channel].send.data.cnt != can_msg[channel].rx.op_report_cnt) {
        ui_msg_tx[channel].send.data.cnt = can_msg[channel].rx.op_report_cnt;

        gp_sdram[channel]->head = head_data;
        gp_sdram[channel]->len = (UI_HEADER_SIZE + sizeof(LogUnit)) * 2;
        switch(channel)
        {
        case 0:
            gp_sdram[channel]->channel = channel_01;
            break;
        case 1:
            gp_sdram[channel]->channel = channel_02;
            break;
        case 2:
            gp_sdram[channel]->channel = channel_03;
            break;
        case 3:
            gp_sdram[channel]->channel = channel_04;
            break;
        default :
            break;
        }

        ret = CopyToEtherBuffer(sock, (uint8_t*) &gp_sdram[channel]->head, UI_HEADER_SIZE + sizeof(LogUnit));
        if (ret != op_error_none) return ret;
    }

    if(ui_msg_tx[channel].send.alarm.cnt != can_msg[channel].rx.alarm_report_cnt) {

        ui_msg_tx[channel].send.alarm.cnt = can_msg[channel].rx.alarm_report_cnt;
        ui_msg_tx[channel].send.alarm.head = head_alarm;
        ui_msg_tx[channel].send.alarm.len = (sizeof(ui_msg_tx[channel].send.alarm) - 1) * 2;
        switch(channel)
        {
        case 0:
            ui_msg_tx[channel].send.alarm.channel = channel_01;
            break;
        case 1:
            ui_msg_tx[channel].send.alarm.channel = channel_02;
            break;
        case 2:
            ui_msg_tx[channel].send.alarm.channel = channel_03;
            break;
        case 3:
            ui_msg_tx[channel].send.alarm.channel = channel_04;
            break;
        default :
            break;
        }
        ui_msg_tx[channel].send.alarm.step_idx = can_msg[channel].recipe.step_idx;

        ui_msg_tx[channel].send.alarm.data[0] = can_msg[channel].rx.alarm.warning.all;
        ui_msg_tx[channel].send.alarm.data[1] = can_msg[channel].rx.alarm.dc.all;
        ui_msg_tx[channel].send.alarm.data[2] = can_msg[channel].rx.alarm.bat.all;
        ui_msg_tx[channel].send.alarm.data[3] = can_msg[channel].rx.alarm.fault.all;
        //        if(can_msg[channel].op.power_error != 0){
        ret = CopyToEtherBuffer(sock, (uint8_t*) &ui_msg_tx[channel].send.alarm, sizeof(ui_msg_tx[channel].send.alarm) - 1);
        if (ret != op_error_none) return ret;
        //        }
    }

    if(ui_msg_tx[channel].send.end.cnt != can_msg[channel].rx.schedule_end_cnt) {
        //can_msg[channel].rx.schedule_end_cnt = ui_msg_tx[channel].send.end.cnt;
        ui_msg_tx[channel].send.end.cnt = can_msg[channel].rx.schedule_end_cnt;
        ui_msg_tx[channel].send.end.head = head_end;
        ui_msg_tx[channel].send.end.len = (sizeof(ui_msg_tx[channel].send.end) - 1) * 2;
        ui_msg_tx[channel].send.end.channel = channel_01<<channel;
        //        ui_msg_tx[channel].send.end.step_idx = 0;
        ret = CopyToEtherBuffer(sock, (uint8_t*) &ui_msg_tx[channel].send.end, sizeof(ui_msg_tx[channel].send.end) - 1);

        if (ret != op_error_none) return ret;
    }

    //start
    if(ui_msg_tx[channel].send.start.cnt != can_msg[channel].ui_evt.start.cnt) {
        ui_msg_tx[channel].send.start.cnt = can_msg[channel].ui_evt.start.cnt;
        ui_msg_tx[channel].send.start.head = head_start;
        ui_msg_tx[channel].send.start.len = (sizeof(ui_msg_tx[channel].send.start) - 1) * 2;
        ui_msg_tx[channel].send.start.channel = channel_01<<channel;
        //            g_debug_start=1;
        ret = CopyToEtherBuffer(sock, (uint8_t*) &ui_msg_tx[channel].send.start, sizeof(ui_msg_tx[channel].send.start) - 1);

        if (ret != op_error_none) return ret;
    }

    return ret;
}

static AppCommUIOpErrList CopyFromEtherBuffer(uint8_t sock, uint8_t *buf, uint16_t len)
{
    uint16_t size = getSn_RX_RSR(sock);
    int32_t ret;


    //    printf("%s-%s-%d %d: input size - %u, recv size - %u\r\n", __FILE__, __FUNCTION__, __LINE__, sock, len, size);

    if (len) {
        if (size > 0) {
            ret = recv(sock, buf, len * 2);
#if 0
            int i;
            printf("--------------------------------------------------\r\n");
            for(i=0;i<len;i++)
            {
                printf("data[%d] : 0x%04x   len - %u, recv size - %u\r\n",i, buf[i], len, size);
            }
            `("--------------------------------------------------\r\n");
#endif
            //Size check
            if (ret == len * 2) {
//                printf("%s-%s-%d %d: Size Valid\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
            } else if (ret < 0) {
                printf("%s-%s-%d %d: Sock error\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
                return op_error_sock;
            } else {
                printf("%s-%s-%d %d: Size Invalid\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
                return op_error_len;
            }
        } else {
            printf("%s-%s-%d %d: Data size zero - %u\r\n", __FILE__, __FUNCTION__, __LINE__, sock, size);
            return op_error_len;
        }
    }

    return op_error_none;
}

static AppCommUIOpErrList CopyToEtherBuffer(uint8_t sock, uint8_t *buf, uint16_t len)
{
    int32_t ret;
    int i;
    ret = send(sock, buf, len * 2);
    //    if(g_debug_start)
    //    {
    //        g_debug_start=0;
    //        printf("------start----------\r\n");
    //        for(i=0; i<len; i++)
    //        {
    //            printf("start data[%d]==0x%04x\r\n", i, buf[i]);
    //        }
    //        printf("------start----------\r\n");
    //    }
    //printf("%s-%s-%d %d: input size - %u, send size - %lu\r\n", __FILE__, __FUNCTION__, __LINE__, sock, len, ret);

    if (ret == len * 2) {
        //printf("%s-%s-%d %d: Size Valid\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
    } else if (ret < 0) {
        //printf("%s-%s-%d %d: Sock error\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
        return op_error_sock;
    } else {
        printf("%s-%s-%d %d: Size Invalid\r\n", __FILE__, __FUNCTION__, __LINE__, sock);
        return op_error_len;
    }

    return op_error_none;
}

void app_CommUIOpTimer(void)
{
    ui_msg_rx.step_load_time_out++;
}



