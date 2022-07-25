/*
 * drv_timer.h
 *
 *  Created on: Nov 24, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_TIMER_H_
#define PHOENIXON_DRV_DRV_TIMER_H_

#ifdef CPU1
#define DSP_TIMER0_TIME     32      // * 1us    32us -> 31.25kHz
#define DSP_TIMER1_TIME     500000  // * 1us
#define DSP_TIMER2_TIME     1000    // * 1us
#endif

#ifdef CPU2
#define DSP_TIMER0_TIME     32      // * 1us
#define DSP_TIMER1_TIME     1000000 // * 1us
#define DSP_TIMER2_TIME     1000000 // * 1us
#endif

#define USE_TIMER0_CNT      6
#define USE_TIMER1_CNT      5
#define USE_TIMER2_CNT      5

typedef struct
{
    void (*process)(void);
} CBTimer;

extern volatile CBTimer drv_timer0_register[USE_TIMER0_CNT];
extern volatile CBTimer drv_timer1_register[USE_TIMER1_CNT];
extern volatile CBTimer drv_timer2_register[USE_TIMER2_CNT];

void drv_InitTimer(uint32_t timer1, uint32_t timer2, uint32_t timer3);

__interrupt void drv_irq_Timer0(void);
__interrupt void drv_irq_Timer1(void);
__interrupt void drv_irq_Timer2(void);

uint32_t get_time_ms(void);
uint32_t get_time_diff(uint32_t time_start, uint32_t time_end);

#endif /* PHOENIXON_DRV_DRV_TIMER_H_ */