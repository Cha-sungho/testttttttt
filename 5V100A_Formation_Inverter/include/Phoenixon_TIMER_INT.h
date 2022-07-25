/*
 * TIMER_INT.h
 *
 *  Created on: 2016. 11. 16.
 *      Author: KJ
 */

#ifndef HEADER_TIMER_INT_H_
#define HEADER_TIMER_INT_H_


__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

void Init_Timer();


extern int Daq_data_call_cnt;

#endif /* HEADER_TIMER_INT_H_ */
