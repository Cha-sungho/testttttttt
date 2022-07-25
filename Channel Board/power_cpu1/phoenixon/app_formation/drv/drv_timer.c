/*
 * drv_timer.c
 *
 *  Created on: Nov 24, 2021
 *      Author: JOO
 */

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include "drv_timer.h"
#include "bsp_io.h"

#include <stdlib.h>

#if _FLASH
#pragma CODE_SECTION(drv_irq_Timer0, ".TI.ramfunc");
#pragma CODE_SECTION(drv_irq_Timer1, ".TI.ramfunc");
#pragma CODE_SECTION(drv_irq_Timer2, ".TI.ramfunc");
#endif

volatile CBTimer drv_timer0_register[USE_TIMER0_CNT] = { NULL, };
volatile CBTimer drv_timer1_register[USE_TIMER1_CNT] = { NULL, };
volatile CBTimer drv_timer2_register[USE_TIMER2_CNT] = { NULL, };

volatile Uint32 timer2_count = 0;

void drv_ConfigTimer(uint32_t cpuTimer, uint32_t period);

void drv_InitTimer(uint32_t timer1, uint32_t timer2, uint32_t timer3)
{
    // Initialize timer period to maximum
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    // Make sure timer is stopped
    drv_ConfigTimer(CPUTIMER0_BASE, timer1);
    drv_ConfigTimer(CPUTIMER1_BASE, timer2);
    drv_ConfigTimer(CPUTIMER2_BASE, timer3);

    // Reload all counter register with period value
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    // IRQ
    Interrupt_register(INT_TIMER0, &drv_irq_Timer0);
    Interrupt_register(INT_TIMER1, &drv_irq_Timer1);
    Interrupt_register(INT_TIMER2, &drv_irq_Timer2);

    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);
    Interrupt_enable(INT_TIMER2);

    // Start
#ifdef CPU1
    if (timer1) CPUTimer_startTimer(CPUTIMER0_BASE);
    if (timer2) CPUTimer_startTimer(CPUTIMER1_BASE);
    if (timer3) CPUTimer_startTimer(CPUTIMER2_BASE);
#endif
#ifdef CPU2
    CPUTimer_startTimer(CPUTIMER0_BASE);
#endif
}

void drv_ConfigTimer(uint32_t cpuTimer, uint32_t period)
{
    uint32_t temp, freq = DEVICE_SYSCLK_FREQ;

    // Initialize timer period:
    temp = ((freq / 1000000) * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    CPUTimer_setPreScaler(cpuTimer, 0);

    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);
}

__interrupt void drv_irq_Timer0(void)
{
#ifdef CPU1
    bsp_IoChMux1TP(1);
#endif
#ifdef CPU2
//    bsp_IoChMux2TP(1);
#endif
    Uint16 i;

    for (i = 0; drv_timer0_register[i].process != NULL && i <= USE_TIMER0_CNT; i++) {
        drv_timer0_register[i].process();
    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
#ifdef CPU1
    bsp_IoChMux1TP(0);
#endif
#ifdef CPU2
//    bsp_IoChMux2TP(0);
#endif
}

__interrupt void drv_irq_Timer1(void)
{
    Uint16 i;

    for (i = 0; drv_timer1_register[i].process != NULL && i <= USE_TIMER1_CNT; i++) {
        drv_timer1_register[i].process();
    }
}

__interrupt void drv_irq_Timer2(void)
{
    Uint16 i;
    timer2_count++;

    for (i = 0; drv_timer2_register[i].process != NULL && i <= USE_TIMER2_CNT; i++) {
        drv_timer2_register[i].process();
    }
}

uint32_t get_time_ms(void)
{
    return timer2_count;
}

uint32_t get_time_diff(uint32_t time_start, uint32_t time_end)
{
    uint32_t time_diff = 0;

    time_diff = abs((sizeof(uint32_t) - time_end) - (sizeof(uint32_t) - time_start));

    return time_diff;
}

//
// End of File
//
