/*
 * User_CAN.h
 *
 *  Created on: 2017. 9. 20.
 *      Author: KJ
 */

#ifndef USER_SOURCE_USER_HEAD_U_CAN_H_
#define USER_SOURCE_USER_HEAD_U_CAN_H_


interrupt void CANIntHandler(void);

extern	void Can_Data_Send_Test();
extern	void CAN_A_Mailbox_Init();

extern	void Init_Pack_Data();
extern  void CANA_GPIO_INIT();
extern  void CANA_INIT_INIT();
extern  void CANHandler(void);
extern  void Write_Pack_Data();

extern int Pack_sensing1_flag, Pack_sensing2_flag,  Pack_sensing3_flag, Pack_sensing4_flag;

#define DSP_COMMUNICATION_CHANNEL   1

#endif /* USER_SOURCE_USER_HEAD_U_CAN_H_ */
