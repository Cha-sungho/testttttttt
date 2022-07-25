/*
 * drv_timer.c
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#pragma CODE_SECTION(drv_irq_Timer0, ".TI.ramfunc");

Uint32 timer_count = 0;
Uint32 timer_count_2 = 0;
Uint32 timer_count_3 = 0;
Uint32 timer_count_4 = 0;
Uint16 LED_FLAG = 0;

typedef struct
{
    void (*process)(void);
} halTimerCB;

halTimerCB drv_timer_register[] =
{
    app_CommCanTimer,
    app_CommUIOpTimer,
    NULL
};

void drv_ConfigTimer(Uint32 Timer, float Frequency, float Period);

void drv_InitTimer(void)
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
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    // Reload all counter register with period value
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    drv_ConfigTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER0_TIME);       // 1ms
    drv_ConfigTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER1_TIME);
    drv_ConfigTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, DSP_TIMER2_TIME);

    // IRQ
    Interrupt_register(INT_TIMER0, &drv_irq_Timer0);
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    Interrupt_enable(INT_TIMER0);

    // Start
    CPUTimer_startTimer(CPUTIMER0_BASE);
}

void drv_ConfigTimer(Uint32 Timer, float Frequency, float Period)
{
    Uint32 ul_Temp;

    // Initialize timer period:
    ul_Temp = (Uint32) (Frequency / 1000000 * Period);
    CPUTimer_setPeriod(Timer, ul_Temp);

    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    CPUTimer_setPreScaler(Timer, 0);

    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    CPUTimer_stopTimer(Timer);
    CPUTimer_reloadTimerCounter(Timer);
    CPUTimer_setEmulationMode(Timer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(Timer);
}

__interrupt void drv_irq_Timer0(void)
{
    Uint16 i;

    for (i = 0; drv_timer_register[i].process != NULL; i++) {
        drv_timer_register[i].process();
    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    timer_count++;
    timer_count_2++;
    timer_count_3++;
    timer_count_4++;

    if(((timer_count%500)==0) && LED_FLAG) //SDRAM ¿Ï·áµÇ¾î¾ß ±ôºýÀÓ
    {
        GpioDataRegs.GPBTOGGLE.bit.GPIO35 = 1;
    }
}

__interrupt void drv_irq_Timer1(void)
{

}

__interrupt void drv_irq_Timer2(void)
{

}
