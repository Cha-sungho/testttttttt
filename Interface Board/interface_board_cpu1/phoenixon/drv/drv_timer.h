/*
 * drv_timer.h
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#ifndef PHOENIXON_DRV_DRV_TIMER_H_
#define PHOENIXON_DRV_DRV_TIMER_H_

#define DSP_TIMER0_TIME 1000    //1ms = 1000 * 1us
#define DSP_TIMER1_TIME 1000000 //1s  = 1000000 * 1us
#define DSP_TIMER2_TIME 1000000 //1s  = 1000000 * 1us

void drv_InitTimer(void);

__interrupt void drv_irq_Timer0(void);
__interrupt void drv_irq_Timer1(void);
__interrupt void drv_irq_Timer2(void);

#endif /* PHOENIXON_DRV_DRV_TIMER_H_ */
