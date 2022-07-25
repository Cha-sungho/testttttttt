/*
 * pack_data.h
 *
 *  Created on: 2017. 11. 16.
 *      Author: KJ
 */

#ifndef USER_SOURCE_USER_HEAD_PACK_DATA_H_
#define USER_SOURCE_USER_HEAD_PACK_DATA_H_

#define _USE_OTHER_ADC_D_
#define _USE_CAN_

struct PACK_DATA_BIT            // Pack Voltage/Current Frame for Command/Report Frame
{
	unsigned char CMD;
	unsigned char OPCODE_PARACODE;	//2015.4.30 Parallel add
	unsigned char pack_vol_H;
	unsigned char pack_vol_M;
	unsigned char pack_vol_L;
	unsigned char pack_cur_H;
	unsigned char pack_cur_M;
	unsigned char pack_cur_L;
};

struct EXT_DATA_BIT            // Pack Voltage/Current Frame for Command/Report Frame
{
	unsigned char ext_pow_H;
	unsigned char ext_pow_M_1;
	unsigned char ext_pow_M_2;
	unsigned char ext_pow_L;
	unsigned char ext_time_H;
	unsigned char ext_time_M_1;
	unsigned char ext_time_M_2;
	unsigned char ext_time_L;
};

struct EXT_CV_CURR_BIT            // 20150624 kys cv charge curr & cv discharge curr
{
	unsigned char ext_Charge_Curr_H;
	unsigned char ext_Charge_Curr_M_1;
	unsigned char ext_Charge_Curr_M_2;
	unsigned char ext_Charge_Curr_L;
	unsigned char ext_DisCharge_Curr_H;
	unsigned char ext_DisCharge_Curr_M_1;
	unsigned char ext_DisCharge_Curr_M_2;
	unsigned char ext_DisCharge_Curr_L;
};

struct EXT_DATA_STEP            //2015.05.04  ext pattern step count
{
	unsigned char step_H;
	unsigned char step_L;
	unsigned char Pattern_count_H;	// 20150527 데이터 전송 카운트 추가
	unsigned char Pattern_count_L;
	unsigned char Pattern_time_H;	// 20150527 외부패턴 시간 추가
	unsigned char Pattern_time_M_1;
	unsigned char Pattern_time_M_2;
	unsigned char Pattern_time_L;
};

struct CV_ON            //2016.03.10 kys cv on add
{
	unsigned char cv_flag;
	unsigned char spare2;
	unsigned char spare3;	// 20150527 데이터 전송 카운트 추가
	unsigned char spare4;
	unsigned char spare5;	// 20150527 외부패턴 시간 추가
	unsigned char spare6;
	unsigned char spare7;
	unsigned char spare8;
};

union pack_data_
{
	unsigned char frame[8] ;
	struct PACK_DATA_BIT bit;
	struct EXT_DATA_BIT ext_bit;
	struct EXT_CV_CURR_BIT ext_Curr_bit;
	struct EXT_DATA_STEP step_bit;
	struct CV_ON cv_bit;				//2016.03.10 kys cv on add
};

struct PACK_WARN_BIT            // Pack Warning Info for Report Frame
{
	unsigned char CMD;
	unsigned char OPCODE_PARACODE;	//2015.4.30 Parallel add

	unsigned char WAlarm1;
	unsigned char WG;
	unsigned char GAlarm1;


	unsigned char DAlarm1;
	unsigned char DF;
	unsigned char FAlarm1;
};

struct IGBT_WARN_BIT
{
	unsigned char CMD;
	unsigned char OPCODE_PARACODE;

	unsigned char Reserved0;
	unsigned char Reserved1;

	unsigned char Reserved2;
	unsigned char Reserved3;

	unsigned char igbt_alarm_H;
	unsigned char igbt_alarm_L;
};

/*

struct PACK_WARN_BIT             // Pack Warning Info for Report Frame
{
	Uint16 CMD:8;
	Uint16 OPCODE:8;
	Uint16 WAlarm3:4;
	Uint16 WAlarm2:4;
	Uint16 WAlarm1:4;
	Uint16 GAlarm3:4;
	Uint16 GAlarm2:4;
	Uint16 GAlarm1:4;
	Uint16 DAlarm3:4;
	Uint16 DAlarm2:4;
	Uint16 DAlarm1:4;
	Uint16 FAlarm3:4;
	Uint16 FAlarm2:4;
	Uint16 FAlarm1:4;
};


 */
union pack_warn_
{
	unsigned char frame[8];
	struct   PACK_WARN_BIT bit;
};

#if 1// igbt err add kwk
union igbt_warn_
{
	unsigned char frame[8];
	struct IGBT_WARN_BIT bit;
};
#endif

typedef union pack_data_ pack_data;
typedef union pack_warn_ pack_warn;

typedef union igbt_warn_ igbt_warn;// kwk 190402

struct PACK_INFO_
{
	pack_data data;
	pack_warn warning;
};

typedef struct PACK_INFO_ pack_info ;

void Init_Pack_Data();
void Write_Pack_Data();



#endif /* USER_SOURCE_USER_HEAD_PACK_DATA_H_ */
