/*
 * main.h
 *
 *  Created on: 2019. 1. 31.
 *      Author: kwk
 */

#ifndef MAIN_H_
#define MAIN_H_


//  Formation Inverter �߰�
#define Formation_Inverter      1


// ��۷� �ٲ��ش�...
#define ORIGINAL_METHOD    0
#define TIMER2_METHOD      1


#define _TIMER2_100ms_   0
#define _TIMER2_1ms_     0
#define _TIMER2_10ms_    1


#define TIMER2_SYNC_ENABLE   1



#define CAN_PACKET_SEND_PRIORITY  1// 0 �̸� �����Ͱ� ���� ������ 1�̸� �����̺갡 ����������.



#if( ORIGINAL_METHOD == 1)   // �������� ����� Ÿ�̸� 2�� �׿��ش�..
	#define TIMER2_ENABLE   0
#else
     #define TIMER2_ENABLE  1
#endif


//#define DSP_CH1   1



#endif /* MAIN_H_ */



