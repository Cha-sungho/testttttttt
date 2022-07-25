/*
 * app_tick.h
 *
 *  Created on: 2022. 2. 5.
 *      Author: boomw
 */

#ifndef PHOENIXON_APP_FORMATION_APP_TICK_APP_TICK_H_
#define PHOENIXON_APP_FORMATION_APP_TICK_APP_TICK_H_

#ifdef CPU1
extern volatile uint32_t cpu1_tick_timer_led;
extern volatile uint32_t cpu1_comm_ui_push_timer;
extern volatile uint32_t cpu1_reset_offset_timer;
#else
extern volatile uint32_t cpu2_tick_timer_led;
extern volatile uint32_t cpu2_offset_timer;
#endif

void app_TickVal1(void);
void app_TickVal2(void);
void app_TickVal3(void);

#endif /* PHOENIXON_APP_FORMATION_APP_TICK_APP_TICK_H_ */
