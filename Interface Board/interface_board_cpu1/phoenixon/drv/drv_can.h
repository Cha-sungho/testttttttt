/*
 * drv_can.h
 *
 *  Created on: 2021. 8. 20.
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_CAN_H_
#define PHOENIXON_DRV_DRV_CAN_H_

#include "driverlib.h"

#define OBJ_MAX_1CH     5

typedef struct
{
    uint32_t module;
    uint32_t bit_rate;
} DrvCanInitVal;

typedef struct
{
    struct {
        uint32_t obj_id;
        uint32_t can_id;
        CAN_MsgObjType type;
        uint32_t flag;
        uint16_t data[8];
        uint16_t cnt;
    } obj[32];
    uint32_t error_cnt;
} DrvCanMsg;

extern DrvCanMsg drv_can_msg;

void drv_InitCAN(void);
void drv_WriteCAN(uint32_t id, Uint16 *data, Uint16 size);
__interrupt void drv_irq_CANA(void);

#endif /* PHOENIXON_DRV_DRV_CAN_H_ */
