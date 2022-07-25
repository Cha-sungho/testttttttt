/*
 * drv_can.c
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include <string.h>

#include "drv_gpio.h"
#include "drv_can.h"
#include "app_op.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(drv_WriteCAN, ".TI.ramfunc");
#pragma CODE_SECTION(drv_Write8CAN, ".TI.ramfunc");
#pragma CODE_SECTION(drv_irq_CANA, ".TI.ramfunc");
#endif
#endif

#define USE_DMA         0

#if USE_DMA == 1
#define MSG_DATA_LENGTH 4
#define TX_MSG_OBJ_ID   1
#define RX_MSG_OBJ_ID   2
#define DMA_BURST       2
#define DMA_TRANSFER    1
#define CAN_IF2         2
uint16_t drv_rx_dma_buf[8] = { 0, };
#pragma DATA_SECTION(drv_rx_dma_buf,"SHARERAMGS1");
#endif

DrvSetCanMsg drv_can_msg_set_table[33];

volatile DrvCanMsg drv_can_msg;

static void drv_ConfigCANGpio(void);
static void drv_InitCANMsgObj(Uint16 channel);
__interrupt void drv_irq_CANA(void);

static void drv_InitDMACANA(void);
__interrupt void drv_irq_DMA_CANA(void);

static void drv_ConfigCANGpio(void)
{
    //CAN-A RX
    drv_ConfigGPIO(GPIO_CORE_CPU1, 62, GPIO_DIR_MODE_OUT, GPIO_62_CANRXA, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);
    //CAN-A TX
    drv_ConfigGPIO(GPIO_CORE_CPU1, 63, GPIO_DIR_MODE_OUT, GPIO_63_CANTXA, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);
}

static void drv_InitCANMsgObj(Uint16 channel)
{
    Uint16 i = 0;

    memset((void*) &drv_can_msg, NULL, sizeof(DrvCanMsg));

    //CAN ID       | channel 1  | channel 2   | channel 3   | channel 4  |
    //RX_ID_0x100  | 0x100   (1)| 0x200   (6) | 0x300   (11)| 0x400  (16)|
    //RX_ID_0xAA   | 0xAA    (2)| 0xBA    (7) | 0xCA    (12)| 0xDA   (17)|
    //TX_ID_110    | 0x110   (3)| 0x210   (8) | 0x310   (13)| 0x410  (18)|
    //TX_ID_120    | 0x120   (4)| 0x220   (9) | 0x320   (14)| 0x420  (19)|
    //TX_ID_130    | 0x130   (5)| 0x230   (10)| 0x330   (15)| 0x430  (20)|

    drv_can_msg_set_table[1 + channel * 5].can_id = RX_ID_0x100 + (0x100 * channel);
    drv_can_msg_set_table[1 + channel * 5].type = CAN_MSG_OBJ_TYPE_RX;
    drv_can_msg_set_table[1 + channel * 5].flag = CAN_MSG_OBJ_RX_INT_ENABLE;

    drv_can_msg_set_table[2 + channel * 5].can_id = RX_ID_0xAA + (0x10 * channel);
    drv_can_msg_set_table[2 + channel * 5].type = CAN_MSG_OBJ_TYPE_RX;
    drv_can_msg_set_table[2 + channel * 5].flag = CAN_MSG_OBJ_RX_INT_ENABLE;

    drv_can_msg_set_table[3 + channel * 5].can_id = TX_ID_110 + (0x100 * channel);
    drv_can_msg_set_table[3 + channel * 5].type = CAN_MSG_OBJ_TYPE_TX;
    drv_can_msg_set_table[3 + channel * 5].flag = CAN_MSG_OBJ_NO_FLAGS;

    drv_can_msg_set_table[4 + channel * 5].can_id = TX_ID_120 + (0x100 * channel);
    drv_can_msg_set_table[4 + channel * 5].type = CAN_MSG_OBJ_TYPE_TX;
    drv_can_msg_set_table[4 + channel * 5].flag = CAN_MSG_OBJ_NO_FLAGS;

    drv_can_msg_set_table[5 + channel * 5].can_id = TX_ID_130 + (0x100 * channel);
    drv_can_msg_set_table[5 + channel * 5].type = CAN_MSG_OBJ_TYPE_TX;
    drv_can_msg_set_table[5 + channel * 5].flag = CAN_MSG_OBJ_NO_FLAGS;

    for (i = 1 + channel * 5; i < (5 + channel * 5) + 1; i++) {
        drv_can_msg.obj[i].obj_id = i;
        drv_can_msg.obj[i].can_id = drv_can_msg_set_table[i].can_id;
        drv_can_msg.obj[i].type = drv_can_msg_set_table[i].type;
        drv_can_msg.obj[i].flag = drv_can_msg_set_table[i].flag;
    }
}

void drv_InitCAN(Uint16 channel)
{
    Uint16 i;

#ifdef CPU1
    drv_ConfigCANGpio();
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL8_CAN, 1, SYSCTL_CPUSEL_CPU1);
#endif

#ifdef CPU1
    drv_InitCANMsgObj(channel);

    CAN_initModule(USE_CAN);
    CAN_setBitRate(USE_CAN, DEVICE_SYSCLK_FREQ, CAN_BITRATE, 20);

    CAN_enableInterrupt(CANA_BASE, CAN_INT_IE0 | CAN_INT_ERROR | CAN_INT_STATUS);
    Interrupt_register(INT_CANA0, &drv_irq_CANA);

#if USE_DMA == 1
    Interrupt_register(INT_DMA_CH5, &drv_irq_DMA_CANA);
    drv_InitDMACANA();
    CAN_enableDMARequests(CANA_BASE);
    Interrupt_enable(INT_DMA_CH5);
    DMA_startChannel(DMA_CH5_BASE);
#endif

    Interrupt_enable(INT_CANA0);
    CAN_enableGlobalInterrupt(USE_CAN, CAN_GLOBAL_INT_CANINT0);

    for (i = 1; i < 32; i++) {
        if(drv_can_msg.obj[i].can_id != NULL) {
            CAN_setupMessageObject(USE_CAN,                     //base
                                   drv_can_msg.obj[i].obj_id,   //obj ID
                                   drv_can_msg.obj[i].can_id,   //msg ID
                                   CAN_MSG_FRAME_STD,           //frame
                                   drv_can_msg.obj[i].type,     //msg type
                                   drv_can_msg.obj[i].can_id,                           //msg IDMask
                                   drv_can_msg.obj[i].flag,     //flag
                                   8);                          //msg len
        }
    }

    CAN_startModule(USE_CAN);
#endif
}

void drv_WriteCAN(uint32_t obj_id, Uint16 *data, Uint16 size)
{
    Uint16 i;
    Uint16 data_temp[8] = { 0, };

    for (i = 0; i < size; i++) {
        data_temp[i] = data[i];
    }

    CAN_sendMessage(USE_CAN, obj_id, 8, data_temp);
}

void drv_Write8CAN(uint32_t obj_id, Uint16 *data)
{
    CAN_sendMessage(USE_CAN, obj_id, 8, data);
}

__interrupt void drv_irq_CANA(void)
{
    uint32_t status;
    Uint16 i;

    // Read the CAN interrupt status to find the cause of the interrupt
    status = CAN_getInterruptCause(USE_CAN);

    // If the cause is a controller status interrupt, then get the status
    if (status == CAN_INT_INT0ID_STATUS) {
        // Read the controller status.
        status = CAN_getStatus(USE_CAN);
        // Check to see if an error occurred.
        if (((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7)
                && ((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0)) {
            // Set a flag to indicate some errors may have occurred.
            drv_can_msg.error_cnt++;
        }
    } else {
        for (i = 1; i < 33; i++) {
            if (status == drv_can_msg.obj[i].obj_id) {
#if USE_DMA == 1
                drv_can_msg.obj[i].irq_cnt++;
                CAN_transferMessage(USE_CAN,
                                    CAN_IF2,                    //Interface
                                    drv_can_msg.obj[i].obj_id,  //objID
                                    false ,                     //Direction, False : RAM to IF, True : IF to RAM
                                    true                        //DMA request
                );
#else
                operation.op.keepalive.cnt++;
                drv_can_msg.obj[i].irq_cnt++;
                CAN_readMessage(USE_CAN, drv_can_msg.obj[i].obj_id, (uint16_t*) &drv_can_msg.obj[i].data);
#endif
                CAN_clearInterruptStatus(USE_CAN, drv_can_msg.obj[i].obj_id);
                break;
            }
        }
    }

    // Clear the global interrupt flag for the CAN interrupt line
    CAN_clearGlobalInterruptStatus(USE_CAN, CAN_GLOBAL_INT_CANINT0);

    // Acknowledge this interrupt located in group 9
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

static void drv_InitDMACANA(void)
{
#if USE_DMA == 1
    // Initialize DMA
    DMA_initController();

    // Configure DMA Ch5 for RX. When the CAN asserts the DMA line, the DMA
    // will transfer the contents of the IF2 data register into the RX buffer
    DMA_configAddresses(DMA_CH5_BASE, (uint16_t*) drv_rx_dma_buf, (uint16_t*) (CANA_BASE + CAN_O_IF2DATA));
    DMA_configBurst(DMA_CH5_BASE, 8, 1, 1);
    DMA_configTransfer(DMA_CH5_BASE, 8, 0, 0);
    //DMA_configWrap(DMA_CH5_BASE, 8, 1, 8, 1);
    DMA_configMode(DMA_CH5_BASE, DMA_TRIGGER_CANAIF2, DMA_CFG_ONESHOT_DISABLE | DMA_CFG_CONTINUOUS_ENABLE | DMA_CFG_SIZE_16BIT);

    // Configure DMA Ch5 interrupts
    DMA_setInterruptMode(DMA_CH5_BASE, DMA_INT_AT_END);
    DMA_enableInterrupt(DMA_CH5_BASE);
    DMA_enableTrigger(DMA_CH5_BASE);
#endif
}

__interrupt void drv_irq_DMA_CANA(void)
{
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);
}

//
// End of File
//
