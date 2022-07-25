/*
 * main.h
 *
 *  Created on: 2019. 1. 31.
 *      Author: kwk
 */

#ifndef MAIN_H_
#define MAIN_H_


//  Formation Inverter 추가
#define Formation_Inverter      1


// 토글로 바꿔준다...
#define ORIGINAL_METHOD    0
#define TIMER2_METHOD      1


#define _TIMER2_100ms_   0
#define _TIMER2_1ms_     0
#define _TIMER2_10ms_    1


#define TIMER2_SYNC_ENABLE   1



#define CAN_PACKET_SEND_PRIORITY  1// 0 이면 마스터가 먼저 보내고 1이면 슬레이브가 먼저보낸다.



#if( ORIGINAL_METHOD == 1)   // 오리지날 방법은 타이머 2를 죽여준다..
	#define TIMER2_ENABLE   0
#else
     #define TIMER2_ENABLE  1
#endif


//#define DSP_CH1   1



#endif /* MAIN_H_ */



