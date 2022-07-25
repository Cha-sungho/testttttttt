/*
 * HR_PWM.h
 *
 *  Created on: 2016. 11. 15.
 *      Author: KJ
 */

#ifndef HEADER_HR_PWM_H_
#define HEADER_HR_PWM_H_


#define	HRPWM_PERIOD	15000

#define	SYSTEM_CLOCK		200E6	// 200MHz
#define	TBCLK				200E6	// 200MHz
#define	PWM_CARRIER			10E3	// 20kHz
#define	PWM_DUTY_RATIO_A	2E-1	// 0.2, 20%


#define	SW_CH1		0
#define	SW_CH2		1
#define	SW_CH3		2
#define	SW_CH4		3
#define	SW_CH5		4
#define	SW_CH6		5
#define	SW_CH7		6
#define	SW_CH8		7
#define	SW_CH9		8
#define	SW_CH10		9
#define	SW_CH11		10
#define	SW_CH12		11
#define	SW_CH13		12
#define	SW_CH14		13
#define	SW_CH15		14
#define	SW_CH16		15

#define	HR_PWM_CH1		1
#define	HR_PWM_CH2		2
#define	HR_PWM_CH3		3
#define	HR_PWM_CH4		4
#define	HR_PWM_CH5		5
#define	HR_PWM_CH6		6
#define	HR_PWM_CH7		7
#define	HR_PWM_CH8		8
#define	HR_PWM_CH9		9
#define	HR_PWM_CH10		10
#define	HR_PWM_CH11		11
#define	HR_PWM_CH12		12
#define	HR_PWM_CH13		13
#define	HR_PWM_CH14		14
#define	HR_PWM_CH15		15
#define	HR_PWM_CH16		16

#define	EN_PWM1			16
#define	EN_PWM2			17
#define	EN_PWM3			18
#define	EN_PWM4			19
#define	EN_PWM5			20
#define	EN_PWM6			21
#define	EN_PWM7			22
#define	EN_PWM8			23
#define	EN_PWM9			24
#define	EN_PWM10		25
#define	EN_PWM11		26
#define	EN_PWM12		27
#define	EN_PWM13		28
#define	EN_PWM14		29
#define	EN_PWM15		30
#define	EN_PWM16		31

#define	CH_DIS_SEL1		91
#define	CH_DIS_SEL2		92
#define	CH_DIS_SEL3		93
#define	CH_DIS_SEL4		94

#define	HRPWM_GLOBAL	32

#define	SET_EN			1
#define	OFF_EN			0

#define	CHARGE			1
#define	DISCHARGE		0



#define	SET_EN_PWM1()		{GPIO_WritePin(EN_PWM1, SET_EN);}
#define	SET_EN_PWM2()		{GPIO_WritePin(EN_PWM2, SET_EN);}
#define	SET_EN_PWM3()		{GPIO_WritePin(EN_PWM3, SET_EN);}
#define	SET_EN_PWM4()		{GPIO_WritePin(EN_PWM4, SET_EN);}
#define	SET_EN_PWM5()		{GPIO_WritePin(EN_PWM5, SET_EN);}
#define	SET_EN_PWM6()		{GPIO_WritePin(EN_PWM6, SET_EN);}
#define	SET_EN_PWM7()		{GPIO_WritePin(EN_PWM7, SET_EN);}
#define	SET_EN_PWM8()		{GPIO_WritePin(EN_PWM8, SET_EN);}
#define	SET_EN_PWM9()		{GPIO_WritePin(EN_PWM9, SET_EN);}
#define	SET_EN_PWM10()		{GPIO_WritePin(EN_PWM10, SET_EN);}
#define	SET_EN_PWM11()		{GPIO_WritePin(EN_PWM11, SET_EN);}
#define	SET_EN_PWM12()		{GPIO_WritePin(EN_PWM12, SET_EN);}
#define	SET_EN_PWM13()		{GPIO_WritePin(EN_PWM13, SET_EN);}
#define	SET_EN_PWM14()		{GPIO_WritePin(EN_PWM14, SET_EN);}
#define	SET_EN_PWM15()		{GPIO_WritePin(EN_PWM15, SET_EN);}
#define	SET_EN_PWM16()		{GPIO_WritePin(EN_PWM16, SET_EN);}

#define	OFF_EN_PWM1()		{GPIO_WritePin(EN_PWM1, OFF_EN);}
#define	OFF_EN_PWM2()		{GPIO_WritePin(EN_PWM2, OFF_EN);}
#define	OFF_EN_PWM3()		{GPIO_WritePin(EN_PWM3, OFF_EN);}
#define	OFF_EN_PWM4()		{GPIO_WritePin(EN_PWM4, OFF_EN);}
#define	OFF_EN_PWM5()		{GPIO_WritePin(EN_PWM5, OFF_EN);}
#define	OFF_EN_PWM6()		{GPIO_WritePin(EN_PWM6, OFF_EN);}
#define	OFF_EN_PWM7()		{GPIO_WritePin(EN_PWM7, OFF_EN);}
#define	OFF_EN_PWM8()		{GPIO_WritePin(EN_PWM8, OFF_EN);}
#define	OFF_EN_PWM9()		{GPIO_WritePin(EN_PWM9, OFF_EN);}
#define	OFF_EN_PWM10()		{GPIO_WritePin(EN_PWM10, OFF_EN);}
#define	OFF_EN_PWM11()		{GPIO_WritePin(EN_PWM11, OFF_EN);}
#define	OFF_EN_PWM12()		{GPIO_WritePin(EN_PWM12, OFF_EN);}
#define	OFF_EN_PWM13()		{GPIO_WritePin(EN_PWM13, OFF_EN);}
#define	OFF_EN_PWM14()		{GPIO_WritePin(EN_PWM14, OFF_EN);}
#define	OFF_EN_PWM15()		{GPIO_WritePin(EN_PWM15, OFF_EN);}
#define	OFF_EN_PWM16()		{GPIO_WritePin(EN_PWM16, OFF_EN);}

#define	HRPWM_GLOBAL_EN()		{GPIO_WritePin(HRPWM_GLOBAL, SET_EN);}
#define	HRPWM_GLOBAL_DIS()		{GPIO_WritePin(HRPWM_GLOBAL, OFF_EN);}

#define	HRPWM_CHARGE_SEL1()		{GPIO_WritePin(CH_DIS_SEL1, CHARGE);}
#define	HRPWM_CHARGE_SEL2()		{GPIO_WritePin(CH_DIS_SEL2, CHARGE);}
#define	HRPWM_CHARGE_SEL3()		{GPIO_WritePin(CH_DIS_SEL3, CHARGE);}
#define	HRPWM_CHARGE_SEL4()		{GPIO_WritePin(CH_DIS_SEL4, CHARGE);}

#define	HRPWM_DIS_SEL1()		{GPIO_WritePin(CH_DIS_SEL1, DISCHARGE);}
#define	HRPWM_DIS_SEL2()		{GPIO_WritePin(CH_DIS_SEL2, DISCHARGE);}
#define	HRPWM_DIS_SEL3()		{GPIO_WritePin(CH_DIS_SEL3, DISCHARGE);}
#define	HRPWM_DIS_SEL4()		{GPIO_WritePin(CH_DIS_SEL4, DISCHARGE);}


#define	SW1_ON()				{GPIO_WritePin(SW_CH1, 1);}
#define	SW2_ON()				{GPIO_WritePin(SW_CH2, 1);}
#define	SW3_ON()				{GPIO_WritePin(SW_CH3, 1);}
#define	SW4_ON()				{GPIO_WritePin(SW_CH4, 1);}
#define	SW5_ON()				{GPIO_WritePin(SW_CH5, 1);}
#define	SW6_ON()				{GPIO_WritePin(SW_CH6, 1);}
#define	SW7_ON()				{GPIO_WritePin(SW_CH7, 1);}
#define	SW8_ON()				{GPIO_WritePin(SW_CH8, 1);}
#define	SW9_ON()				{GPIO_WritePin(SW_CH9, 1);}
#define	SW10_ON()				{GPIO_WritePin(SW_CH10, 1);}
#define	SW11_ON()				{GPIO_WritePin(SW_CH11, 1);}
#define	SW12_ON()				{GPIO_WritePin(SW_CH12, 1);}
#define	SW13_ON()				{GPIO_WritePin(SW_CH13, 1);}
#define	SW14_ON()				{GPIO_WritePin(SW_CH14, 1);}
#define	SW15_ON()				{GPIO_WritePin(SW_CH15, 1);}
#define	SW16_ON()				{GPIO_WritePin(SW_CH16, 1);}

#define	SW1_OFF()				{GPIO_WritePin(SW_CH1, 0);}
#define	SW2_OFF()				{GPIO_WritePin(SW_CH2, 0);}
#define	SW3_OFF()				{GPIO_WritePin(SW_CH3, 0);}
#define	SW4_OFF()				{GPIO_WritePin(SW_CH4, 0);}
#define	SW5_OFF()				{GPIO_WritePin(SW_CH5, 0);}
#define	SW6_OFF()				{GPIO_WritePin(SW_CH6, 0);}
#define	SW7_OFF()				{GPIO_WritePin(SW_CH7, 0);}
#define	SW8_OFF()				{GPIO_WritePin(SW_CH8, 0);}
#define	SW9_OFF()				{GPIO_WritePin(SW_CH9, 0);}
#define	SW10_OFF()				{GPIO_WritePin(SW_CH10, 0);}
#define	SW11_OFF()				{GPIO_WritePin(SW_CH11, 0);}
#define	SW12_OFF()				{GPIO_WritePin(SW_CH12, 0);}
#define	SW13_OFF()				{GPIO_WritePin(SW_CH13, 0);}
#define	SW14_OFF()				{GPIO_WritePin(SW_CH14, 0);}
#define	SW15_OFF()				{GPIO_WritePin(SW_CH15, 0);}
#define	SW16_OFF()				{GPIO_WritePin(SW_CH16, 0);}


void HRPWM_Config(int period);
void HR_PWM_DATA(float f_data, int i_pwm_num);
void PWM_EN_INIT();
void SET_EN_PWM(unsigned int ui_en);
void OFF_EN_PWM(unsigned int ui_off);

void SWITCH_CH_INIT();

void InitEPwm_ALL(void);

void InitEPwm1Module(void);
void InitEPwm2Module(void);
void InitEPwm3Module(void);
void InitEPwm4Module(void);
void InitEPwm5Module(void);
void InitEPwm6Module(void);
void InitEPwm7Module(void);
void InitEPwm8Module(void);
void InitEPwm9Module(void);
void InitEPwm10Module(void);
void InitEPwm11Module(void);
void InitEPwm12Module(void);

//################################################################################################
//bgkim-2020-01-29
//Add function
//Start Line
//
int InitEpwmTrip(unsigned int ui_Number);
void ClaerTripFlag(void);
//
//bgkim-2020-01-29
//Add function
//End Line
//################################################################################################

//################################################################################################
//bgkim-2020-01-29
//Trip On EPWM
//Start Line
//
#define TRIP_ON_EPWM 0//value 1 set Trip EPWM setting
//
//bgkim-2020-01-29
//Trip On EPWM
//End Line
//################################################################################################
extern float f_Hr_duty;

#endif /* HEADER_HR_PWM_H_ */
