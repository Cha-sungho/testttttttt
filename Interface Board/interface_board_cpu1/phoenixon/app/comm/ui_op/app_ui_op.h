/*
 * app_ui_protocol.h
 *
 *  Created on: 2021. 8. 21.
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_COMM_UI_OP_APP_UI_OP_H_
#define PHOENIXON_APP_COMM_UI_OP_APP_UI_OP_H_

#define UI_HEADER_SIZE  (sizeof(CommOpHeadList)+sizeof(uint16_t)+sizeof(CommOpChannel))
#define UI_JUMP_SIZE    (sizeof(uint16_t))
#define UI_START_SIZE   (sizeof(uint16_t)*4)
#define UI_CMD_SIZE     (sizeof(CommOpCCmd))
#define UI_PAUSE_SIZE   (sizeof(uint16_t)+sizeof(CommOpPCmd))

typedef struct
{
    uint16_t sock;
    uint16_t type;
    uint16_t port;
} AppCommUIOpInitVal;

typedef enum
{
    op_error_none,
    op_error_header,
    op_error_sock,
    op_error_len,
    op_error_timeout
} AppCommUIOpErrList;

typedef struct
{
    uint16_t sock_state;

    struct
    {
        CommOpHeadList head;
        uint16_t len;
        CommOpChannel channel;

        struct
        {
            uint16_t reserved[4];
            uint16_t cnt;
        } start[4];

        struct
        {
            uint16_t step_idx;
            uint16_t cnt;
        } jump[4];

        struct
        {
            CommOpCCmd cmd;
            uint16_t cnt;
        } command[4];

        struct
        {
            uint16_t step_idx;
            CommOpPCmd cmd;
            uint16_t cnt;
        } pause[4];

        uint16_t act_step_cnt;
        uint16_t remain_step_cnt;
        uint16_t remain_step_cnt_old;
        BOOL step_loaded;
    } recv;

   Uint64 step_load_time_out;

} AppCommUIOpHandle_RX;

typedef struct
{
    struct
    {
        CommOpHeadList head;
        uint16_t len;
        CommOpChannel channel;

        struct
        {
            uint16_t cnt;
        } data;

        struct
        {
            CommOpHeadList head;
            uint16_t len;
            CommOpChannel channel;
            uint16_t reserved[4];
            uint16_t cnt;
        } start;

        struct
        {
            uint16_t step_idx;
            uint16_t cnt;
        } jump;

        struct
        {
            CommOpCCmd cmd;
            uint16_t cnt;
        } command;

        struct
        {
            CommOpPCmd cmd;
            uint16_t cnt;
        } pause;

        struct
        {
            CommOpHeadList head;
            uint16_t len;
            CommOpChannel channel;
            uint16_t step_idx;
            uint16_t data[8];
            uint16_t cnt;
        } alarm;

        struct
        {
            CommOpHeadList head;
            uint16_t len;
            CommOpChannel channel;
            uint16_t cnt;
        } end;
    } send;

} AppCommUIOpHandle_TX;

typedef struct
{
    uint16_t state;
    void (*process)(uint16_t sock, uint16_t type, uint16_t port);
} AppCommUIOpSockState;

typedef struct
{
    CommOpHeadList head;
    AppCommUIOpErrList (*process)(uint8_t sock);
} AppUICmdState;

typedef struct
{
    uint16_t channel;
    uint16_t control_board_channel;
} AppBoardState;

extern AppCommUIOpHandle_RX ui_msg_rx;
extern AppCommUIOpHandle_TX ui_msg_tx[4];
extern volatile TotalStepLog *gp_sdram[4];

void app_CommUIOpDataMapping(void);
int32_t app_CommUIOpRun(void);
void app_CommUIOpTimer(void);

#endif /* PHOENIXON_APP_COMM_UI_OP_APP_UI_OP_H_ */
