/*
 * drv_can.h
 *
 *  Created on: Nov 8, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_CAN_H_
#define PHOENIXON_DRV_DRV_CAN_H_

#include "driverlib.h"

#define USE_CAN             CANA_BASE
#define CAN_BITRATE         500000      //500000 = 500kbps, 1000000 = 1Mbps
#define OBJ_MAX_1CH         5

typedef struct
{
    uint32_t can_id;
    CAN_MsgObjType type;
    uint32_t flag;
} DrvSetCanMsg;

typedef struct
{
    struct {
        uint64_t irq_cnt;
        uint16_t obj_id;
        uint32_t can_id;
        CAN_MsgObjType type;
        uint32_t flag;
        uint16_t data[8];
    } obj[32];
    uint64_t error_cnt;
} DrvCanMsg;

typedef enum
{
    RX_ID_0x100 = 0x100,
    RX_ID_0xAA  = 0xAA,
    TX_ID_110   = 0x110,
    TX_ID_120   = 0x120,
    TX_ID_130   = 0x130,
}CanID_Number_Format;

typedef enum
{
    CAN_RX_ID_0x100 = 1,
    CAN_RX_ID_0xAA  = 2,
    CAN_TX_ID_110   = 3,
    CAN_TX_ID_120   = 4,
    CAN_TX_ID_130   = 5
}CanID_Obj_Matching;

extern DrvSetCanMsg drv_can_msg_set_table[33];
extern volatile DrvCanMsg drv_can_msg;

void drv_InitCAN(Uint16 channel);
void drv_WriteCAN(uint32_t obj_id, Uint16 *data, Uint16 size);
void drv_Write8CAN(uint32_t obj_id, Uint16 *data);

#endif /* PHOENIXON_DRV_DRV_CAN_H_ */