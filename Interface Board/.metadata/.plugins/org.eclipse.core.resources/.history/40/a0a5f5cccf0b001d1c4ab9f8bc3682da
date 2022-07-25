/*
 * main_cpu01.c
 *
 *  Created on: 2021. 6. 9.
 *      Author: KHJ
 */

// Included Files
#include <stdio.h>
#include <string.h>

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"
#include "loopback.h"
#include "Types.h"

#define RACK_NUMBER     2           // Need to change this number

#define VERSION_LEN     50
#define DEVNAME_LEN     50

static const char g_device_string[DEVNAME_LEN] = "FORMATION_Interface";
static const char g_fw_version_string[VERSION_LEN] = "V1.0.20\r\n";

uint16 size[12];
extern Uint16 LED_FLAG;
AppBoardState op;

void PrintSystemInfo(void);

void main(void)
{

    InitSysCtrl();                  // Initialize system control

    Device_initGPIO();              // Initialize GPIO and configure GPIO pins for USB.
    Interrupt_initModule();         // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initVectorTable();    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).

    drv_InitEmif();
    op.channel = drv_initChannelNumber();
    drv_InitUsb();
    drv_InitTimer();
    PrintSystemInfo();
    drv_InitNet(op.channel, RACK_NUMBER);
    drv_InitCAN();
    drv_initLED();

    Interrupt_enableMaster();       // Enable Interrupts

    EINT;                           // Enable Global Interrupt (INTM)

    ERTM;                           // Enable realtime interrupt (DBGM)

    LED_FLAG = drv_InitSdram();     // SDRAM initialize(30s)

    app_CommUIOpDataMapping();

    size[0] = UI_HEADER_SIZE;
    size[1] = UI_JUMP_SIZE;
    size[2] = UI_START_SIZE;
    size[3] = UI_CMD_SIZE;
    size[4] = UI_PAUSE_SIZE;
    size[5] = sizeof(StepUnit);
    size[6] = sizeof(LogUnit);

    app_CanOpVarInit();

    int i;

    while (1) {
        app_CommUIOpRun();
        for (i = 0; i < MAX_CHANNEL; i++) {
            op.control_board_channel = i;
            app_CommCanOpRun(i);
        }
    }
}

void PrintSystemInfo(void)
{
    printf("\r\n************************************************");
    printf("\r\nDevice Type: %s", g_device_string);
    printf("\r\nNow FW Version: %s", g_fw_version_string);
    printf("************************************************\r\n");
}

