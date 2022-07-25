/*
 * drv_can.c
 *
 *  Created on: 2021. 8. 19.
 *      Author: JOO
 */

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include <string.h>

#pragma CODE_SECTION(drv_WriteCAN, ".TI.ramfunc");
#pragma CODE_SECTION(drv_irq_CANA, ".TI.ramfunc");

const DrvCanInitVal drv_can_init = {
                                    .module     = CANA_BASE,
                                    .bit_rate   = 500000,       //500000 = 500kbps, 1000000 = 1Mbps
};

extern AppBoardState op;

DrvCanMsg drv_can_msg;
DrvCanMsg drv_can_msg_dummy;

void drv_InitCANGpio(void);
void drv_InitCANMsgObj(void);

void drv_InitCANGpio(void)
{
    GPIO_setPinConfig(GPIO_5_CANRXA);
    GPIO_setPinConfig(GPIO_4_CANTXA);
}

void drv_InitCANMsgObj(void)
{
    int i;
    memset(&drv_can_msg, NULL, sizeof(DrvCanMsg));

    for(i=0; i<MAX_CHANNEL; i++) //up to object number:16
    {
    //I/F -> Power, command
        drv_can_msg.obj[1+i*5].obj_id = 1+i*5;
        drv_can_msg.obj[1+i*5].can_id = 0x100+(i*0x100);
        drv_can_msg.obj[1+i*5].type = CAN_MSG_OBJ_TYPE_TX;
        drv_can_msg.obj[1+i*5].flag = CAN_MSG_OBJ_NO_FLAGS;
        drv_can_msg.obj[1+i*5].cnt = 0;

    //I/F -> Power, command
        drv_can_msg.obj[2+i*5].obj_id = 2+i*5;
        drv_can_msg.obj[2+i*5].can_id = 0xAA+(i*0x10);;
        drv_can_msg.obj[2+i*5].type = CAN_MSG_OBJ_TYPE_TX;
        drv_can_msg.obj[2+i*5].flag = CAN_MSG_OBJ_NO_FLAGS;
        drv_can_msg.obj[2+i*5].cnt = 0;

    //Power -> I/F, stats
        drv_can_msg.obj[3+i*5].obj_id = 3+i*5;
        drv_can_msg.obj[3+i*5].can_id = 0x110+(i*0x100);
        drv_can_msg.obj[3+i*5].type = CAN_MSG_OBJ_TYPE_RX;
        drv_can_msg.obj[3+i*5].flag = CAN_MSG_OBJ_RX_INT_ENABLE;
        drv_can_msg.obj[3+i*5].cnt = 0;

    //Power -> I/F, erro
        drv_can_msg.obj[4+i*5].obj_id = 4+i*5;
        drv_can_msg.obj[4+i*5].can_id = 0x120+(i*0x100);
        drv_can_msg.obj[4+i*5].type = CAN_MSG_OBJ_TYPE_RX;
        drv_can_msg.obj[4+i*5].flag = CAN_MSG_OBJ_RX_INT_ENABLE;
        drv_can_msg.obj[4+i*5].cnt = 0;

    //Thermal
        drv_can_msg.obj[5+i*5].obj_id = 5+i*5;
        drv_can_msg.obj[5+i*5].can_id = 0x130+(i*0x100);
        drv_can_msg.obj[5+i*5].type = CAN_MSG_OBJ_TYPE_RX;
        drv_can_msg.obj[5+i*5].flag = CAN_MSG_OBJ_RX_INT_ENABLE;
        drv_can_msg.obj[5+i*5].cnt = 0;
    }
}

void drv_InitCAN(void)
{
    Uint16 i;

    drv_InitCANGpio();
    drv_InitCANMsgObj();

    CAN_initModule(drv_can_init.module);
    CAN_setBitRate(drv_can_init.module, DEVICE_SYSCLK_FREQ, drv_can_init.bit_rate, 20);

    CAN_enableInterrupt(CANA_BASE, CAN_INT_IE0 | CAN_INT_ERROR | CAN_INT_STATUS);
    Interrupt_register(INT_CANA0, &drv_irq_CANA);
    Interrupt_enable(INT_CANA0);
    CAN_enableGlobalInterrupt(drv_can_init.module, CAN_GLOBAL_INT_CANINT0);

    for (i = 1; i < 33; i++) {
        if(drv_can_msg.obj[i].obj_id != NULL) {
            CAN_setupMessageObject(drv_can_init.module,
                                   drv_can_msg.obj[i].obj_id,
                                   drv_can_msg.obj[i].can_id,
                                   CAN_MSG_FRAME_STD,
                                   drv_can_msg.obj[i].type,
                                   0,
                                   drv_can_msg.obj[i].flag,
                                   8);
        }
    }
    CAN_startModule(drv_can_init.module);
}

void drv_WriteCAN(uint32_t id, Uint16 *data, Uint16 size)
{
    Uint16 i;
    Uint16 data_temp[8] = { 0, };

    for (i = 0; i < size; i++) {
        data_temp[i] = data[i];
    }

    CAN_sendMessage(drv_can_init.module, id, 8, data_temp);
}

__interrupt void drv_irq_CANA(void)
{
    uint32_t status;
    Uint16 i;

    // Read the CAN interrupt status
    status = CAN_getInterruptCause(drv_can_init.module);

    // If the cause is a controller status interrupt, then get the status
    if (status == CAN_INT_INT0ID_STATUS) {
        status = CAN_getStatus(drv_can_init.module);

    // Check to see if an error occurred.
        if (((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7)
                && ((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0)) {
    // Set a flag to indicate some errors may have occurred.
            drv_can_msg.error_cnt++;
        }
    } else {
        for (i = 0; i < 32; i++) {
            if(status == drv_can_msg.obj[i].obj_id) {
                drv_can_msg.obj[i].cnt++;
                CAN_readMessage(drv_can_init.module, drv_can_msg.obj[i].obj_id, drv_can_msg.obj[i].data);
                CAN_clearInterruptStatus(drv_can_init.module, drv_can_msg.obj[i].obj_id);
                break;
            }
        }
    }

    // Clear the global interrupt flag for the CAN interrupt line
    CAN_clearGlobalInterruptStatus(drv_can_init.module, CAN_GLOBAL_INT_CANINT0);

    // Acknowledge this interrupt located in group 9
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
