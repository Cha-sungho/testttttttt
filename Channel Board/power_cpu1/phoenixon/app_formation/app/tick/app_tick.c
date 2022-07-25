/*
 * app_tick.c
 *
 *  Created on: 2022. 2. 5.
 *      Author: boomw
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "app_tick.h"

#ifdef _FLASH
#pragma CODE_SECTION(app_TickVal1, ".TI.ramfunc");
#pragma CODE_SECTION(app_TickVal2, ".TI.ramfunc");
#pragma CODE_SECTION(app_TickVal3, ".TI.ramfunc");
#endif

#ifdef CPU1
volatile uint32_t cpu1_tick_timer_led = 0;
volatile uint32_t cpu1_comm_ui_push_timer = 0;
volatile uint32_t cpu1_reset_offset_timer = 0;
#else
volatile uint32_t cpu2_tick_timer_led = 0;
volatile uint32_t cpu2_offset_timer = 0;
#endif

void app_TickVal1(void)
{
#ifdef CPU1
    cpu1_tick_timer_led++;
    cpu1_comm_ui_push_timer++;
    cpu1_reset_offset_timer++;
#else
    cpu2_tick_timer_led++;
    cpu2_offset_timer++;
#endif
}

void app_TickVal2(void)
{

}

void app_TickVal3(void)
{

}

//
// End of File
//