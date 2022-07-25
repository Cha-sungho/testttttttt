/*
 * U_CAN.c
 *
 *  Created on: 2017. 9. 20.
 *      Author: KJ
 */

#include "F2837xD_device.h"
#include "F2837xD_Examples.h"
#include "../main.h"

extern void Timer2_count_clear(void);
extern void Timer2_count_clear_2(void);

//====================== =====================전역 변수. 머지시, 삭제 필요
Uint16 m_nExtTotalDataCount = 0;
Uint32 m_nExtTotalTime = 0;
int Ex_Pattern_flag;

int Pack_sensing1_flag = 0, Pack_sensing2_flag = 0, Pack_sensing3_flag = 0, Pack_sensing4_flag = 0;
int Pattern_read_flag, Pattern_cnt = 0, R_100ms_flag = 0, Pattern_Rest_flag = 0;
int Pattern_CV_mode, CV_pattern_CV_flag, Rising_time, Charge_flag, Discharge_flag;
//float Ibat_UI_rms,Vpack_UI_rms;


//=========================================================================================	test value
volatile unsigned long g_ulTxMsgCount = 0;
volatile unsigned long g_ulRxMsgCount = 0;
unsigned long u32CanAErrorStatus;
volatile unsigned long g_bErrFlag = 0;

tCANMsgObject sTXCANMessage, sTXCANMessage2;
tCANMsgObject sRXCANMessage;
tCANMsgObject sRXCANMessage_KJ;

unsigned char ucTXMsgData[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char ucTXMsgData2[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char ucRXMsgData[8];
unsigned char ucRXMsgData_KJ[8];

unsigned long u32CntTXMsgData = 0x12345678;

unsigned int tx_id = 0, rx_id = 0, rx_id_KJ = 2;

int a = 0, b = 0xff, eVOLT_LIMIT, eERR_CHK_CNT, m_nERR_VOLT_CNT,
        SCI_V_ref_Err_Chk, eCURR_LIMIT, SCI_I_ref_Err_Chk, eERR_CHK_CNT;
int m_nERR_CURR_CNT, SCI_I_ref_Err;
int SCI_Pattern_flag = 0;
//=========================================================================================	test value end


//DouB ; setting for MASTER & SLAVE
#define DSP_CH1//CH1 ; DouB - this is means a MASTER
//#define DSP_CH2//CH2 ; DouB - this is means a SLAVE


// for 1 pcan port connect multi channels
// CAN ID  : 1ch - 0x100, 2ch - 0x200, 3ch - 0x300, 4ch - 0x400
// only use single mode : DSP_COMMUNICATION_CHANNEL : device channel
// use parralel mode : DSP_COMMUNICATION_CHANNEL : 0 ( disable )

#if(DSP_COMMUNICATION_CHANNEL != 0)
#define CAN_ID ((0x100) * (((DSP_COMMUNICATION_CHANNEL - 1) % 4) + 1))
#endif

#define CC_CHARGE_OPCODE 3
#define CC_DISCHARGE_OPCODE 5

#define eIBAT 260	//2015.05.04 add

tCANMsgObject s_TX_MBOX1, s_TX_MBOX2, s_TX_MBOX3;
tCANMsgObject s_TX_MBOX5;
#ifdef DSP_CH2
    tCANMsgObject s_TX_MBOX4;
#endif

tCANMsgObject s_RX_MBOX3, s_RX_MBOX4, s_RX_MBOX5, s_RX_MBOX6, s_RX_MBOX7,
        s_RX_MBOX8, s_RX_MBOX9, s_RX_MBOX10, s_RX_MBOX11,
        s_RX_MBOX12, //
        s_RX_MBOX13, s_RX_MBOX14, s_RX_MBOX15, s_RX_MBOX16, s_RX_MBOX17,
        s_RX_MBOX18, s_RX_MBOX19, s_RX_MBOX20,  // s_RX_MBOX20  add 181108
        s_RX_MBOX21;

unsigned char uc_tx_mbox1_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char uc_tx_mbox2_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char uc_tx_mbox3_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char uc_tx_mbox5_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

unsigned char uc_rx_mbox3_data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

//struct ECAN_REGS ECanaShadow;
//struct ECAN_REGS ECanbShadow;

float fltoffset;	//2015.06.13
float fltDelta1;
float fltDelta2;

float m_fltCvTemp = 0;
float m_nCvChk = 0;
float m_fltCvCurrRef = 5.0f;
float m_fltCvDelta = 0.01;

int m_nPatFlag = 0;
int m_nPatEndFlag = 0;

int m_nInitRealTimeChk = 0;
unsigned char m_szCmdBuf;
unsigned char m_szRealCmdBuf;
unsigned long m_nStepCount_Buf = 1;

int m_nOp2_CurrZeroCnt = 0;

pack_data rx_can_frame;
pack_data tx_data;
pack_warn tx_warn;

igbt_warn tx_igbt_warn;

Uint32 tmp_volt;
Uint32 tmp_curr;
Uint32 tmp_limit_volt;
Uint32 SCI_Operation_Time;

int CAN_all_receive;
Uint16 m_uSSID;
Uint16 SSID;
Uint16 prev_SSID;

//외부패턴
Uint16 m_Patt_Flag;

Uint64 tmp_cp;
Uint64 tmp_time;

float m_fltVolt = 0.;
float m_fltCurr = 0.;
float m_fltCp = 0.;
float flt_Ibat_over = 0.;

int nSCI_CCCV_CP_CR_flag = 0;
int nSCI_Parallel_flag = 0;
int nCP_confirm_flag = 0, Dir_Charge_flag = 1, Dir_Discharge_flag = 0;

unsigned char CMD = 0x00;

unsigned char m_wCMD = 0x00;

extern int m_nCheckDataZero = 0;
extern int m_CanConnectCheck = 0; //2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가
unsigned long Real_Pattern_cnt = 0;	////2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가
int m_chkExtCount_105 = 0;
int m_chkExtCount_106 = 0;

int m_chk_100 = 0;
void ReadBuffData_CH1(void);
void ReadBuffData_CH2(void);
/*void ReadBuffData_PARA(void);*/
Uint32 m_nCVCharge_Curr = 0;
Uint32 m_nCVDisCharge_Curr = 0;
float m_fltCVCharge_Curr = 0;
float m_fltCVDisCharge_Curr = 0;
unsigned long Vbat_rms_Now_Volt = 0;

//test
unsigned long Ibat_rms_p0 = 0;
float I_pecnt = 0.5, Ibat_View = 0., err_com = 0.085;

typedef struct Pattern_save
{
    unsigned char szCmd;			//충방전 커맨드 0x98 ,0x99
    unsigned char szOp;			//OP MODE 0x1,0x2 ..
    int nOpFlag;		//cc_cv_cp상태.. 우선은 만들어놈
    int nCpFlag;		//cc_cv_cp상태.. 우선은 만들어놈
    unsigned char szPara;			//병렬
    int nParaFlag;		//병렬 FLAG.. 우선은 만들어놈
    Uint32 nTime;			//동작 time
    float dVolt;			//volt 연산값
    float dCurr;			//curr 연산값
    float dPow;			//power 연산값
    float dChargeCurr;	//20150604 cv 기능시 사용
    float dDisChargeCurr;	//20150604 cv 기능시 사용
} stPat;
//
//
stPat m_stPatBuff_CH1_1[50] = { 0, };	//CH1 버퍼 1
stPat m_stPatBuff_CH1_2[50] = { 0, };	//CH2 버퍼 2
//
stPat m_stPatBuff_CH2_1[50] = { 0, };	//CH1 버퍼 1
stPat m_stPatBuff_CH2_2[50] = { 0, };	//CH2 버퍼 2

stPat m_stPatData_CH1;			//dsp에서 사용할 함수
stPat m_stPatData_CH2;			//dsp에서 사용할 함수

Uint16 m_nBufCnt_105[2] = { 0, };		//ch1 버퍼 1 카운트
Uint16 m_nBufCnt_106[2] = { 0, };		//ch1 버퍼 2 카운트
Uint16 m_nBufCnt_107[2] = { 0, };		//ch1 버퍼 2 카운트
Uint16 m_nBufCnt_205[2] = { 0, };		//ch2 버퍼 1 카운트
Uint16 m_nBufCnt_206[2] = { 0, };		//ch2 버퍼 2 카운트
Uint16 m_nBufCnt_207[2] = { 0, };		//ch2 버퍼 2 카운트

Uint16 m_RevBufflag_105 = 0;	//CH1 RAED DATA BUFF 1 & 2 FLAG
Uint16 m_RevBufflag_106 = 0;	//CH1 RAED DATA BUFF 1 & 2 FLAG
Uint16 m_RevBufflag_107;	//CH1 RAED DATA BUFF 1 & 2 FLAG

Uint16 m_RevBufflag_205 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG
Uint16 m_RevBufflag_206 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG
Uint16 m_RevBufflag_207 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG
//
Uint16 m_SendBufflag_CH1 = 0;	//SEND DATA BUFF 1 & 2 FLAG
Uint16 m_SendBufflag_CH2 = 0;	//SEND DATA BUFF 1 & 2 FLAG
//
Uint16 m_nSendBufCnt_CH1[2] = { 0, };	//BUFF 1 COUNT100 CECHK
Uint16 m_nSendBufCnt_CH2[2] = { 0, };	//BUFF 2 COUNT100 CECHK
//
unsigned long m_nStepCount_CH1 = 0;
unsigned long m_nStepCount_CH2 = 0;

//2016.03.10 kys cv on ctrol add
unsigned int Pack_Cycler_flag = 1;		//베터리 충방전기모드일 경우 1 처음시작시에는 1
unsigned int CV_ON_flag = 0;				//CV_ON 모드일 경우 1


//
//////외부패턴
unsigned long Vbat_rms_tmp = 0, Ibat_rms_tmp = 0;
unsigned long pattern_cnt = 1000;

unsigned int can_rx_cnt = 0;

void clear_rx_frame()
{
    memset(&rx_can_frame, 0, sizeof(pack_data));
}

void clear_tx_frame()
{
    //	memset(&tx_can_frame, 0, sizeof(pack_info));
    memset(&tx_data, 0, sizeof(pack_data));
    memset(&tx_warn, 0, sizeof(pack_warn));

    memset(&tx_igbt_warn, 0, sizeof(igbt_warn));
}

void Init_Pack_Data()
{
//외부패턴 초기화

    memset(m_stPatBuff_CH1_1, 0, sizeof(m_stPatBuff_CH1_1));
    memset(m_stPatBuff_CH1_2, 0, sizeof(m_stPatBuff_CH1_2));
    memset(m_stPatBuff_CH2_1, 0, sizeof(m_stPatBuff_CH2_1));
    memset(m_stPatBuff_CH2_2, 0, sizeof(m_stPatBuff_CH2_2));

    m_nBufCnt_105[0] = 0;		//ch1 버퍼 1 카운트
    m_nBufCnt_106[0] = 0;		//ch1 버퍼 2 카운트
    m_nBufCnt_107[0] = 0;		//ch1 버퍼 2 카운트
    m_nBufCnt_205[0] = 0;		//ch2 버퍼 1 카운트
    m_nBufCnt_206[0] = 0;		//ch2 버퍼 2 카운트
    m_nBufCnt_207[0] = 0;		//ch2 버퍼 2 카운트

    m_nBufCnt_105[1] = 0;		//ch1 버퍼 1 카운트
    m_nBufCnt_106[1] = 0;		//ch1 버퍼 2 카운트
    m_nBufCnt_107[1] = 0;		//ch1 버퍼 2 카운트
    m_nBufCnt_205[1] = 0;		//ch2 버퍼 1 카운트
    m_nBufCnt_206[1] = 0;		//ch2 버퍼 2 카운트
    m_nBufCnt_207[1] = 0;		//ch2 버퍼 2 카운트

    m_RevBufflag_105 = 0;	//CH1 RAED DATA BUFF 1 & 2 FLAG
    m_RevBufflag_106 = 0;	//CH1 RAED DATA BUFF 1 & 2 FLAG
    m_RevBufflag_107 = 0;	//CH1 RAED DATA BUFF 1 & 2 FLAG

    m_RevBufflag_205 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG
    m_RevBufflag_206 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG
    m_RevBufflag_207 = 0;	//CH2 RAED DATA BUFF 1 & 2 FLAG

    m_nSendBufCnt_CH1[0] = 0;
    m_nSendBufCnt_CH1[1] = 0;

    m_nSendBufCnt_CH2[0] = 0;
    m_nSendBufCnt_CH2[1] = 0;

    m_SendBufflag_CH1 = 0;

    m_SendBufflag_CH2 = 0;

    m_Patt_Flag = 0;

#ifdef DSP_CH1
#if( DSP_COMMUNICATION_CHANNEL == 0 )
    s_TX_MBOX1.ui32MsgID = 0x110;                  // CAN message ID - use 0x110
    s_TX_MBOX1.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX1.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX1.ui32MsgLen = sizeof(uc_tx_mbox1_data);   // size of message is 8
    s_TX_MBOX1.pucMsgData = uc_tx_mbox1_data;          // ptr to message content

    s_TX_MBOX2.ui32MsgID = 0x120;                  // CAN message ID - use 0x120
    s_TX_MBOX2.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX2.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX2.ui32MsgLen = sizeof(uc_tx_mbox2_data);   // size of message is 8
    s_TX_MBOX2.pucMsgData = uc_tx_mbox2_data;          // ptr to message content

    /*
     s_TX_MBOX5.ui32MsgID = 0x121;
     s_TX_MBOX5.ui32MsgIDMask = 0;
     s_TX_MBOX5.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
     s_TX_MBOX5.ui32MsgLen = 8;
     s_TX_MBOX5.pucMsgData = uc_tx_mbox5_data;
     */
#else
    s_TX_MBOX1.ui32MsgID = CAN_ID + 0x10;                  // CAN message ID - use 0x110
    s_TX_MBOX1.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX1.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX1.ui32MsgLen = sizeof(uc_tx_mbox1_data);   // size of message is 8
    s_TX_MBOX1.pucMsgData = uc_tx_mbox1_data;          // ptr to message content

    s_TX_MBOX2.ui32MsgID = CAN_ID + 0x20;                  // CAN message ID - use 0x120
    s_TX_MBOX2.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX2.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX2.ui32MsgLen = sizeof(uc_tx_mbox2_data);   // size of message is 8
    s_TX_MBOX2.pucMsgData = uc_tx_mbox2_data;          // ptr to message content

    /*
     s_TX_MBOX5.ui32MsgID = 0x121;
     s_TX_MBOX5.ui32MsgIDMask = 0;
     s_TX_MBOX5.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
     s_TX_MBOX5.ui32MsgLen = 8;
     s_TX_MBOX5.pucMsgData = uc_tx_mbox5_data;
     */
#endif
#endif
#ifdef DSP_CH2
    s_TX_MBOX1.ui32MsgID = 0x210;                  // CAN message ID - use 0x210
    s_TX_MBOX1.ui32MsgIDMask = 0;// no mask needed for TX
    s_TX_MBOX1.ui32Flags = MSG_OBJ_TX_INT_ENABLE;// enable interrupt on TX
    s_TX_MBOX1.ui32MsgLen = sizeof(uc_tx_mbox1_data);// size of message is 8
    s_TX_MBOX1.pucMsgData = uc_tx_mbox1_data;// ptr to message content

    s_TX_MBOX2.ui32MsgID = 0x220;// CAN message ID - use 0x220
    s_TX_MBOX2.ui32MsgIDMask = 0;// no mask needed for TX
    s_TX_MBOX2.ui32Flags = MSG_OBJ_TX_INT_ENABLE;// enable interrupt on TX
    s_TX_MBOX2.ui32MsgLen = sizeof(uc_tx_mbox2_data);// size of message is 8
    s_TX_MBOX2.pucMsgData = uc_tx_mbox2_data;// ptr to message content
    /*
     s_TX_MBOX5.ui32MsgID = 0x221;
     s_TX_MBOX5.ui32MsgIDMask = 0;
     s_TX_MBOX5.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
     s_TX_MBOX5.ui32MsgLen = 8;
     s_TX_MBOX5.pucMsgData = uc_tx_mbox5_data;
     */
#endif


#if (DSP_COMMUNICATION_CHANNEL == 0)
    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX3.ui32MsgID = 0x100;                      // CAN message ID - use 1
    s_RX_MBOX3.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX3.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX3.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX3.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX4.ui32MsgID = 0x200;                      // CAN message ID - use 1
    s_RX_MBOX4.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX4.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX4.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX4.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX5.ui32MsgID = 0x300;                      // CAN message ID - use 1
    s_RX_MBOX5.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX5.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX5.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX5.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX6.ui32MsgID = 0x106;                      // CAN message ID - use 1
    s_RX_MBOX6.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX6.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX6.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX6.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX7.ui32MsgID = 0x205;                      // CAN message ID - use 1
    s_RX_MBOX7.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX7.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX7.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX7.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX8.ui32MsgID = 0x306;                      // CAN message ID - use 1
    s_RX_MBOX8.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX8.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX8.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX8.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX9.ui32MsgID = 0x305;                      // CAN message ID - use 1
    s_RX_MBOX9.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX9.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX9.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX9.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX10.ui32MsgID = 0x206;                     // CAN message ID - use 1
    s_RX_MBOX10.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX10.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX10.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX10.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX11.ui32MsgID = 0x107;                     // CAN message ID - use 1
    s_RX_MBOX11.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX11.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX11.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX11.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX12.ui32MsgID = 0x207;                     // CAN message ID - use 1
    s_RX_MBOX12.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX12.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX12.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX12.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX13.ui32MsgID = 0x307;                     // CAN message ID - use 1
    s_RX_MBOX13.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX13.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX13.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX13.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX14.ui32MsgID = 0xAA;                      // CAN message ID - use 1
    s_RX_MBOX14.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX14.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX14.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX14.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX15.ui32MsgID = 0xBB;                      // CAN message ID - use 1
    s_RX_MBOX15.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX15.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX15.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX15.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX16.ui32MsgID = 0xCC;                      // CAN message ID - use 1
    s_RX_MBOX16.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX16.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX16.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX16.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX17.ui32MsgID = 0xDD;                      // CAN message ID - use 1
    s_RX_MBOX17.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX17.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX17.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX17.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX18.ui32MsgID = 0xFF;                      // CAN message ID - use 1
    s_RX_MBOX18.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX18.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX18.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX18.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX19.ui32MsgID = 0x105;                     // CAN message ID - use 1
    s_RX_MBOX19.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX19.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX19.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX19.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

//    ///////
    *(unsigned long *) ucRXMsgData = 0;                   // add 181108
    s_RX_MBOX20.ui32MsgID = 0x101;                     // CAN message ID - use 1
    s_RX_MBOX20.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX20.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX20.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX20.pucMsgData = uc_rx_mbox3_data;         // ptr to message content
//    ///////////


    s_TX_MBOX3.ui32MsgID = 0x130;                  // CAN message ID - use 0x120
    s_TX_MBOX3.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX3.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX3.ui32MsgLen = sizeof(uc_tx_mbox3_data);   // size of message is 8
    s_TX_MBOX3.pucMsgData = uc_tx_mbox3_data;          // ptr to message content
#else
    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX3.ui32MsgID = CAN_ID;                      // CAN message ID - use 1
    s_RX_MBOX3.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX3.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX3.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX3.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX6.ui32MsgID = CAN_ID + 0x06;                      // CAN message ID - use 1
    s_RX_MBOX6.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX6.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX6.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX6.pucMsgData = uc_rx_mbox3_data;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX11.ui32MsgID = CAN_ID + 0x07;                     // CAN message ID - use 1
    s_RX_MBOX11.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX11.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX11.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX11.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX14.ui32MsgID = 0xAA;                      // CAN message ID - use 1
    s_RX_MBOX14.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX14.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX14.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX14.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX15.ui32MsgID = 0xBB;                      // CAN message ID - use 1
    s_RX_MBOX15.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX15.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX15.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX15.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX16.ui32MsgID = 0xCC;                      // CAN message ID - use 1
    s_RX_MBOX16.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX16.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX16.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX16.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX17.ui32MsgID = 0xDD;                      // CAN message ID - use 1
    s_RX_MBOX17.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX17.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX17.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX17.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX18.ui32MsgID = 0xFF;                      // CAN message ID - use 1
    s_RX_MBOX18.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX18.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX18.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX18.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    s_RX_MBOX19.ui32MsgID = CAN_ID + 0x05;                     // CAN message ID - use 1
    s_RX_MBOX19.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX19.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX19.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX19.pucMsgData = uc_rx_mbox3_data;         // ptr to message content

//    ///////
    *(unsigned long *) ucRXMsgData = 0;                   // add 181108
    s_RX_MBOX20.ui32MsgID = CAN_ID + 0x01;                     // CAN message ID - use 1
    s_RX_MBOX20.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_RX_MBOX20.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    s_RX_MBOX20.ui32MsgLen = sizeof(uc_rx_mbox3_data);   // size of message is 8
    s_RX_MBOX20.pucMsgData = uc_rx_mbox3_data;         // ptr to message content
//    ///////////

    s_TX_MBOX3.ui32MsgID = CAN_ID + 0x30;                  // CAN message ID - use 0x120
    s_TX_MBOX3.ui32MsgIDMask = 0;                  // no mask needed for TX
    s_TX_MBOX3.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    s_TX_MBOX3.ui32MsgLen = sizeof(uc_tx_mbox3_data);   // size of message is 8
    s_TX_MBOX3.pucMsgData = uc_tx_mbox3_data;          // ptr to message content
#endif

#ifdef DSP_CH2
    s_TX_MBOX4.ui32MsgID = 0x230;
    s_TX_MBOX4.ui32MsgIDMask = 0;
    s_TX_MBOX4.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    s_TX_MBOX4.ui32MsgLen = 8;
    s_TX_MBOX4.pucMsgData = uc_tx_mbox3_data;
#endif

    CANMessageSet(CANA_BASE, 3, &s_RX_MBOX3, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 4, &s_RX_MBOX4, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 5, &s_RX_MBOX5, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 6, &s_RX_MBOX6, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 7, &s_RX_MBOX7, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 8, &s_RX_MBOX8, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 9, &s_RX_MBOX9, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 10, &s_RX_MBOX10, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 11, &s_RX_MBOX11, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 12, &s_RX_MBOX12, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 13, &s_RX_MBOX13, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 14, &s_RX_MBOX14, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 15, &s_RX_MBOX15, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 16, &s_RX_MBOX16, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 17, &s_RX_MBOX17, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 18, &s_RX_MBOX18, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 19, &s_RX_MBOX19, MSG_OBJ_TYPE_RX);
////
    CANMessageSet(CANA_BASE, 20, &s_RX_MBOX20, MSG_OBJ_TYPE_RX);   // add 181108
/////

    clear_rx_frame();
    clear_tx_frame();
    CAN_all_receive = 0;
}

void CAN_Write(Uint16 ID, unsigned char *frame)
{
    /*---------------------------------------------------------------------------
     A. CAN 데이터 송신
     ---------------------------------------------------------------------------*/
    // CAN으로 송신할 데이터를 메일박스에 저장 DSP 1 사용일때는 ID 0x210, 0x220을 막는다.확인필요
#ifdef DSP_CH1
#if(DSP_COMMUNICATION_CHANNEL == 0)
    if (ID == 0x110)
    {
        uc_tx_mbox1_data[0] = frame[0];
        uc_tx_mbox1_data[1] = frame[1];
        uc_tx_mbox1_data[2] = frame[2];
        uc_tx_mbox1_data[3] = frame[3];
        uc_tx_mbox1_data[4] = frame[4];
        uc_tx_mbox1_data[5] = frame[5];
        uc_tx_mbox1_data[6] = frame[6];
        uc_tx_mbox1_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = 0x110;
        CANMessageSet(CANA_BASE, 1, &s_TX_MBOX1, MSG_OBJ_TYPE_TX);
    }
    else if (ID == 0x120)
    {
        uc_tx_mbox2_data[0] = frame[0];
        uc_tx_mbox2_data[1] = frame[1];
        uc_tx_mbox2_data[2] = frame[2];
        uc_tx_mbox2_data[3] = frame[3];
        uc_tx_mbox2_data[4] = frame[4];
        uc_tx_mbox2_data[5] = frame[5];
        uc_tx_mbox2_data[6] = frame[6];
        uc_tx_mbox2_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = 0x120;
        CANMessageSet(CANA_BASE, 2, &s_TX_MBOX2, MSG_OBJ_TYPE_TX);
    }
    else if (ID == 0x130)
    {
        uc_tx_mbox3_data[0] = frame[0];
        uc_tx_mbox3_data[1] = frame[1];
        uc_tx_mbox3_data[2] = frame[2];
        uc_tx_mbox3_data[3] = frame[3];
        uc_tx_mbox3_data[4] = frame[4];
        uc_tx_mbox3_data[5] = frame[5];
        uc_tx_mbox3_data[6] = frame[6];
        uc_tx_mbox3_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = 0x130;
        CANMessageSet(CANA_BASE, 20, &s_TX_MBOX3, MSG_OBJ_TYPE_TX);
    }

#else
    if (ID == CAN_ID + 0x10)
    {
        uc_tx_mbox1_data[0] = frame[0];
        uc_tx_mbox1_data[1] = frame[1];
        uc_tx_mbox1_data[2] = frame[2];
        uc_tx_mbox1_data[3] = frame[3];
        uc_tx_mbox1_data[4] = frame[4];
        uc_tx_mbox1_data[5] = frame[5];
        uc_tx_mbox1_data[6] = frame[6];
        uc_tx_mbox1_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = CAN_ID + 0x10;
        CANMessageSet(CANA_BASE, 1, &s_TX_MBOX1, MSG_OBJ_TYPE_TX);
    }
    else if (ID == CAN_ID + 0x20)
    {
        uc_tx_mbox2_data[0] = frame[0];
        uc_tx_mbox2_data[1] = frame[1];
        uc_tx_mbox2_data[2] = frame[2];
        uc_tx_mbox2_data[3] = frame[3];
        uc_tx_mbox2_data[4] = frame[4];
        uc_tx_mbox2_data[5] = frame[5];
        uc_tx_mbox2_data[6] = frame[6];
        uc_tx_mbox2_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = CAN_ID + 0x20;
        CANMessageSet(CANA_BASE, 2, &s_TX_MBOX2, MSG_OBJ_TYPE_TX);
    }
    else if (ID == CAN_ID + 0x30)
    {
        uc_tx_mbox3_data[0] = frame[0];
        uc_tx_mbox3_data[1] = frame[1];
        uc_tx_mbox3_data[2] = frame[2];
        uc_tx_mbox3_data[3] = frame[3];
        uc_tx_mbox3_data[4] = frame[4];
        uc_tx_mbox3_data[5] = frame[5];
        uc_tx_mbox3_data[6] = frame[6];
        uc_tx_mbox3_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = CAN_ID + 0x30;
        CANMessageSet(CANA_BASE, 20, &s_TX_MBOX3, MSG_OBJ_TYPE_TX);
    }

#endif
#endif

#ifdef DSP_CH2
    if(ID == 0x210)
    {
        uc_tx_mbox1_data[0] = frame[0];
        uc_tx_mbox1_data[1] = frame[1];
        uc_tx_mbox1_data[2] = frame[2];
        uc_tx_mbox1_data[3] = frame[3];
        uc_tx_mbox1_data[4] = frame[4];
        uc_tx_mbox1_data[5] = frame[5];
        uc_tx_mbox1_data[6] = frame[6];
        uc_tx_mbox1_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = 0x110;
        CANMessageSet(CANA_BASE, 1, &s_TX_MBOX1, MSG_OBJ_TYPE_TX);
    }
    else if(ID == 0x220)
    {
        uc_tx_mbox2_data[0] = frame[0];
        uc_tx_mbox2_data[1] = frame[1];
        uc_tx_mbox2_data[2] = frame[2];
        uc_tx_mbox2_data[3] = frame[3];
        uc_tx_mbox2_data[4] = frame[4];
        uc_tx_mbox2_data[5] = frame[5];
        uc_tx_mbox2_data[6] = frame[6];
        uc_tx_mbox2_data[7] = frame[7];

        sTXCANMessage.ui32MsgID = 0x120;
        CANMessageSet(CANA_BASE, 2, &s_TX_MBOX2, MSG_OBJ_TYPE_TX);
    }
    else if( ID == 0x221)
    {
        uc_tx_mbox5_data[0] = frame[0];
        uc_tx_mbox5_data[1] = frame[1];
        uc_tx_mbox5_data[2] = frame[2];
        uc_tx_mbox5_data[3] = frame[3];
        uc_tx_mbox5_data[4] = frame[4];
        uc_tx_mbox5_data[5] = frame[5];
        uc_tx_mbox5_data[6] = frame[6];
        uc_tx_mbox5_data[7] = frame[7];

        CANMessageSet(CANA_BASE,23,&s_TX_MBOX5,MSG_OBJ_TYPE_TX);

    }
    else if(ID == 0x230)
    {
        uc_tx_mbox3_data[0] = frame[0];
        uc_tx_mbox3_data[1] = frame[1];
        uc_tx_mbox3_data[2] = frame[2];
        uc_tx_mbox3_data[3] = frame[3];
        uc_tx_mbox3_data[4] = frame[4];
        uc_tx_mbox3_data[5] = frame[5];
        uc_tx_mbox3_data[6] = frame[6];
        uc_tx_mbox3_data[7] = frame[7];

        //sTXCANMessage.ui32MsgID = 0x110;
        CANMessageSet(CANA_BASE, 21, &s_TX_MBOX4, MSG_OBJ_TYPE_TX);
    }
#endif
}


void Write2CAN()
{
    //	CAN_Write(0x210, tx_can_frame.data.frame) ;
    //	CAN_Write(0x220, tx_can_frame.warning.frame) ;
    //2015.4.30 Parallel add , DSP 1 , 2 ADD
#ifdef DSP_CH1
#if (DSP_COMMUNICATION_CHANNEL == 0)
    if (m_uSSID == 0x100 || m_uSSID == 0x300)		//2015.4.30
    {
        //2015.05.04 ext pattern step count
        CAN_Write(0x110, tx_data.frame);
        CAN_Write(0x120, tx_warn.frame);

        //CAN_Write(0x121,tx_igbt_warn.frame);// 추가...190402

        //2016.03.10 kys cv on add
        if (Pack_Cycler_flag == 1 && CV_ON_flag == 0)       { tx_data.cv_bit.cv_flag = (Pack_Cycler_flag) & 0xff; }
        else if (Pack_Cycler_flag == 0 && CV_ON_flag == 1)  { tx_data.cv_bit.cv_flag = (CV_ON_flag + 1) & 0xff; }
        tx_data.cv_bit.spare2 = 0 & 0xff;
        tx_data.cv_bit.spare3 = 0 & 0xff;
        tx_data.cv_bit.spare4 = 0 & 0xff;
        tx_data.cv_bit.spare5 = 0 & 0xff;
        tx_data.cv_bit.spare6 = 0 & 0xff;
        tx_data.cv_bit.spare7 = 0 & 0xff;
        tx_data.cv_bit.spare8 = 0 & 0xff;
        CAN_Write(0x111, tx_data.frame);

//		if((SCI_Current_ref < 2) && (m_wCMD == 0x88))
//		{
            if (Ibat_rms_tmp < 0xEA60)
            {
                can_rx_cnt++;
            }
//		}

        if (Ex_Pattern_flag == 1 || m_wCMD == 0x88 || m_nPatFlag == 1)
        {
            tx_data.step_bit.step_H = (m_nStepCount_Buf >> 8) & 0xff;
            tx_data.step_bit.step_L = (m_nStepCount_Buf) & 0xff;
            tx_data.step_bit.Pattern_count_H = (m_nExtTotalDataCount >> 8) & 0xff;	// 20150527 데이터 전송 카운트 추가
            tx_data.step_bit.Pattern_count_L = (m_nExtTotalDataCount) & 0xff;
            tx_data.step_bit.Pattern_time_H = (m_nExtTotalTime >> 24) & 0xff;// 20150527 외부패턴 시간 추가
            tx_data.step_bit.Pattern_time_M_1 = (m_nExtTotalTime >> 16) & 0xff;
            tx_data.step_bit.Pattern_time_M_2 = (m_nExtTotalTime >> 8) & 0xff;
            tx_data.step_bit.Pattern_time_L = (m_nExtTotalTime) & 0xff;

            CAN_Write(0x130, tx_data.frame);

            if (m_nPatFlag == 1)
            {
                m_nPatFlag = 0;
            }
        }
    }
#else
    if (m_uSSID == CAN_ID)       //2021.11.30 to use ( 1pcan - 4channel modular )
    {
        //2015.05.04 ext pattern step count
        CAN_Write(CAN_ID + 0x10, tx_data.frame);
        CAN_Write(CAN_ID + 0x20, tx_warn.frame);

        //CAN_Write(0x121,tx_igbt_warn.frame);// 추가...190402

        //2016.03.10 kys cv on add
        if (Pack_Cycler_flag == 1 && CV_ON_flag == 0)       { tx_data.cv_bit.cv_flag = (Pack_Cycler_flag) & 0xff; }
        else if (Pack_Cycler_flag == 0 && CV_ON_flag == 1)  { tx_data.cv_bit.cv_flag = (CV_ON_flag + 1) & 0xff; }
        tx_data.cv_bit.spare2 = 0 & 0xff;
        tx_data.cv_bit.spare3 = 0 & 0xff;
        tx_data.cv_bit.spare4 = 0 & 0xff;
        tx_data.cv_bit.spare5 = 0 & 0xff;
        tx_data.cv_bit.spare6 = 0 & 0xff;
        tx_data.cv_bit.spare7 = 0 & 0xff;
        tx_data.cv_bit.spare8 = 0 & 0xff;
        CAN_Write(0x111, tx_data.frame);

//      if((SCI_Current_ref < 2) && (m_wCMD == 0x88))
//      {
            if (Ibat_rms_tmp < 0xEA60)
            {
                can_rx_cnt++;
            }
//      }

        if (Ex_Pattern_flag == 1 || m_wCMD == 0x88 || m_nPatFlag == 1)
        {
            tx_data.step_bit.step_H = (m_nStepCount_Buf >> 8) & 0xff;
            tx_data.step_bit.step_L = (m_nStepCount_Buf) & 0xff;
            tx_data.step_bit.Pattern_count_H = (m_nExtTotalDataCount >> 8) & 0xff;  // 20150527 데이터 전송 카운트 추가
            tx_data.step_bit.Pattern_count_L = (m_nExtTotalDataCount) & 0xff;
            tx_data.step_bit.Pattern_time_H = (m_nExtTotalTime >> 24) & 0xff;// 20150527 외부패턴 시간 추가
            tx_data.step_bit.Pattern_time_M_1 = (m_nExtTotalTime >> 16) & 0xff;
            tx_data.step_bit.Pattern_time_M_2 = (m_nExtTotalTime >> 8) & 0xff;
            tx_data.step_bit.Pattern_time_L = (m_nExtTotalTime) & 0xff;

            CAN_Write(CAN_ID + 0x30, tx_data.frame);

            if (m_nPatFlag == 1)
            {
                m_nPatFlag = 0;
            }
        }
    }
#endif
#endif

#ifdef DSP_CH2
    if(m_uSSID == 0x200 || m_uSSID == 0x300 )
    {
        m_chk_100 = 2;
        //2015.05.04 ext pattern step count
        CAN_Write(0x210, tx_data.frame);
        CAN_Write(0x220, tx_warn.frame);

        //CAN_Write(0x221,tx_igbt_warn.frame);// kwk 190402 추가. :: remove by kimwonkyum 200225

        //2016.03.10 kys cv on add
        if(Pack_Cycler_flag == 1 && CV_ON_flag == 0)        { tx_data.cv_bit.cv_flag = (Pack_Cycler_flag) & 0xff; }
        else if(Pack_Cycler_flag == 0 && CV_ON_flag == 1)   { tx_data.cv_bit.cv_flag = (CV_ON_flag+1) & 0xff; }
        tx_data.cv_bit.spare2 = 0 & 0xff;
        tx_data.cv_bit.spare3 = 0 & 0xff;
        tx_data.cv_bit.spare4 = 0 & 0xff;
        tx_data.cv_bit.spare5 = 0 & 0xff;
        tx_data.cv_bit.spare6 = 0 & 0xff;
        tx_data.cv_bit.spare7 = 0 & 0xff;
        tx_data.cv_bit.spare8 = 0 & 0xff;
        CAN_Write(0x211, tx_data.frame);

        if(Ex_Pattern_flag == 1 || m_wCMD == 0x88 || m_nPatFlag == 1 )
        {
            tx_data.step_bit.step_H = (m_nStepCount_Buf >> 8) & 0xff;
            tx_data.step_bit.step_L = (m_nStepCount_Buf ) & 0xff;

            tx_data.step_bit.Pattern_count_H = (m_nExtTotalDataCount >> 8) & 0xff;// 20150527 데이터 전송 카운트 추가
            tx_data.step_bit.Pattern_count_L = (m_nExtTotalDataCount ) & 0xff;
            tx_data.step_bit.Pattern_time_H = (m_nExtTotalTime >> 24) & 0xff;// 20150527 외부패턴 시간 추가
            tx_data.step_bit.Pattern_time_M_1 = (m_nExtTotalTime >> 16) & 0xff;
            tx_data.step_bit.Pattern_time_M_2 = (m_nExtTotalTime >> 8) & 0xff;
            tx_data.step_bit.Pattern_time_L = (m_nExtTotalTime ) & 0xff;

            CAN_Write(0x230, tx_data.frame);

            if(m_nPatFlag == 1 )
            {
                m_nPatFlag = 0;
                Pattern_read_flag = 1;
            }
        }
    }
#endif
}


void Write_Pack_Data()
{
    if (m_uSSID == 0x110 || m_uSSID == 0x120 || m_uSSID == 0x130
            || m_uSSID == 0x210 || m_uSSID == 0x220 || m_uSSID == 0x230
            || m_uSSID == 0x105 || m_uSSID == 0x106 || m_uSSID == 0x205
            || m_uSSID == 0x206 || m_uSSID == 0x305 || m_uSSID == 0x306
            || m_uSSID == 0x101)
        return;

    tx_data.bit.CMD = m_wCMD + 0x10;
    if (SCI_Pattern_flag == 1)
    {
//		if(SCI_Charge_flag == 1)		SCI_TX_cccv = 3;
//		if(SCI_Discharge_flag == 1)		SCI_TX_cccv = 5;

        tx_data.bit.OPCODE_PARACODE = ((SCI_TX_cccv << 4) & 0xf0)
                | (SCI_Parallel_flag & 0x0f);
    }
    else
        tx_data.bit.OPCODE_PARACODE = ((SCI_TX_cccv << 4) & 0xf0)
                | (SCI_Parallel_flag & 0x0f);		//OPCODE

    Vbat_rms_tmp = (unsigned long) (Vpack_UI_rms * 1000);
    tx_data.bit.pack_vol_H = (Vbat_rms_tmp >> 16) & 0xff;
    tx_data.bit.pack_vol_M = (Vbat_rms_tmp >> 8) & 0xff;
    tx_data.bit.pack_vol_L = (Vbat_rms_tmp) & 0xff;

    //*** CC ***//
//	if((Buck1_mode ==1)||(Buck1_mode ==3)){
//		if(Ibuck1_ref < 125){ err_com = 0.05;}  //0.1
//		else if(Ibuck1_ref >= 125){ err_com = 0.12;}  //0.1
//
//		if(Ibuck1_ref <= 5.){
//			I_pecnt = 0.5;
//		}
//		else if((Ibuck1_ref > 5.)&&(Ibuck1_ref <= 30.)){
//			I_pecnt = 0.75;
//		}
//		else if(Ibuck1_ref > 30.){
//			I_pecnt = 0.8 ;
//		}
//
//		if(Ibat_UI_rms >= (Ibuck1_ref * I_pecnt)){
//			if(Ibat_UI_rms >= (Ibuck1_ref + err_com)){  //50.1,    200.3  12.048
//				if(Ibuck1_ref >= 125){
//					fltoffset =  0.12;
//				}
//				else{
//					fltoffset = 0.05;
//				}
//				fltDelta1 = Ibat_UI_rms - (Ibuck1_ref + fltoffset);  //0.115    0.4   2.063
//				fltDelta2 = (Ibuck1_ref + fltoffset) - Ibuck1_ref;  //0.015   0.1
//
//				while(1)
//				{
//					if(fabsf(fltDelta1) > fabsf(fltDelta2)){
//						fltDelta1 = fltDelta1 * 0.0512;   //0.0079   0.536  0.0127(12t)    0.512;  -> 0.0512
//					}
//					else{
//						break;
//					}
//				}
//				Ibat_rms_tmp = ((unsigned long)((Ibuck1_ref+fltDelta1) * 1000));
//			}
//
//			else if(Ibat_UI_rms <= (Ibuck1_ref - err_com)){  //16.03.14 현재 레퍼런스값에서 err_com 뺀값보다  실제 전류값이 작을경우 보정 .
//				if(Ibuck1_ref >= 125)
//					fltoffset =  0.12;
//				else
//					fltoffset = 0.05;
//
//				fltDelta1 = Ibat_UI_rms - (Ibuck1_ref - fltoffset);  //0.115    0.4   2.063
//				fltDelta2 = (Ibuck1_ref - fltoffset) - Ibuck1_ref;  //0.015   0.1
//
//				while(1)
//				{
//					if(fabsf(fltDelta1) > fabsf(fltDelta2)){
//						fltDelta1 = fltDelta1 * 0.512;   //0.0079   0.536  0.0127(12t)
//					}
//					else{
//						break;
//					}
//				}
//				Ibat_rms_tmp = ((unsigned long)((Ibuck1_ref+fltDelta1) * 1000));
//			}
//
//			else{
//				Ibat_rms_tmp = ((unsigned long)(Ibat_UI_rms * 1000));
//			}
//		}
//		else{
//			Ibat_rms_tmp = ((unsigned long)(Ibat_UI_rms * 1000));
//		}
//	}
//
//	else{
//		Ibat_rms_tmp = ((unsigned long)(Ibat_UI_rms * 1000));
//	}

    Ibat_rms_tmp = ((unsigned long) (Ibat_UI_rms * 1000));   //test

    Ibat_View = (Ibat_rms_tmp * 0.001);

//	Ibat_rms_tmp = (unsigned long)(Ibat_UI_rms * 1000);

    tx_data.bit.pack_cur_H = (Ibat_rms_tmp >> 16) & 0xff;
    tx_data.bit.pack_cur_M = (Ibat_rms_tmp >> 8) & 0xff;
    tx_data.bit.pack_cur_L = (Ibat_rms_tmp) & 0xff;

    //tx_warn.bit.CMD = m_wCMD + 0x10;

    //tx_warn.bit.OPCODE_PARACODE = ((SCI_TX_cccv << 4) & 0xf0) | (SCI_Parallel_flag & 0x0f);

#ifdef DSP_CH1
#if (DSP_COMMUNICATION_CHANNEL == 0)
    if ((m_uSSID == 0x100) || (m_uSSID == 0x300))
#else
    if (m_uSSID == CAN_ID)
#endif
#endif
#ifdef DSP_CH2
    if((m_uSSID == 0x200)||(m_uSSID == 0x300))
#endif
    {
//		switch(m_wCMD)
//		{
//			case 0x80 :
//	//				Rx_request_data = 80;
//				SCI_Grid_ON_flag   = 1;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x81 :
//	//				Rx_request_data = 81;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 1;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x82 :
//	//				Rx_request_data = 82;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 1;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x83 :
//	//				Rx_request_data = 83;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 1;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x84 :
//	//				Rx_request_data = 84;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 1;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x85 :
//	//				Rx_request_data = 85;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 1;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x86 :
//	//				Rx_request_data = 86;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 1;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x88 :
//	//				Rx_request_data = 88;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
////				SCI_Charge_flag    = 0;
////				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 1;
//				SCI_Err_Clear_flag = 0;
//				break;
//			case 0x89 :
//	//				Rx_request_data = 89;
//				SCI_Grid_ON_flag   = 0;
//				SCI_Pack_ON_flag   = 0;
//				SCI_Charge_flag    = 0;
//				SCI_Discharge_flag = 0;
//				SCI_Pack_OFF_flag  = 0;
//				SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
//				SCI_Err_Clear_flag = 1;
//				break;
//
//			default :
//				tx_data.bit.CMD = m_wCMD;
//				//tx_can_frame.data.bit.CMD = 0x90;
//				break ;
//		}
    }

    if (Warning_error_code == 0)
    {
        tx_warn.bit.WAlarm1 = 0;
        tx_warn.bit.WG = tx_warn.bit.WG & 0x0f;   // low byte.
    }
    if (Grid_error_code == 0)
    {
        tx_warn.bit.WG = tx_warn.bit.WG & 0xf0;   // high byte.
        tx_warn.bit.GAlarm1 = 0;
    }

    if (DC_error_code == 0)
    {
        tx_warn.bit.DAlarm1 = 0;
        tx_warn.bit.DF = tx_warn.bit.DF & 0x0f;
    }
    if (Fault_error_code == 0)
    {
        tx_warn.bit.DF = tx_warn.bit.DF & 0xf0;
        tx_warn.bit.FAlarm1 = 0;
    }

    if (Warning_error_code > 0x0)
    {
        tx_warn.bit.WAlarm1 = (Warning_error_code >> 4) & 0xff;
        tx_warn.bit.WG = ((Warning_error_code) & 0x0f) << 4; //상위 4bit
    }

    if (Grid_error_code > 0x0)
    {
        tx_warn.bit.WG = tx_warn.bit.WG | ((Grid_error_code & 0xf00) >> 8);
        tx_warn.bit.GAlarm1 = (Grid_error_code) & 0xff;
    }
//
    if (DC_error_code > 0x0)
    {
        tx_warn.bit.DAlarm1 = (DC_error_code >> 4) & 0xff;
        tx_warn.bit.DF = ((DC_error_code) & 0x0f) << 4; //상위 4bit
    }

    if (Fault_error_code > 0x0)
    {
        tx_warn.bit.DF = tx_warn.bit.DF | ((Fault_error_code & 0xf00) >> 8);
        tx_warn.bit.FAlarm1 = (Fault_error_code) & 0xff;
    }

    // kwk 190409 추가
    if (IGBT_error_code == 0)
    {
        tx_warn.bit.CMD = 0;
        tx_warn.bit.OPCODE_PARACODE = 0;

    }
    else if (IGBT_error_code > 0x0)
    {
        tx_warn.bit.CMD = (IGBT_error_code & 0xF00) >> 8;
        tx_warn.bit.OPCODE_PARACODE = IGBT_error_code & 0xFF;
    }

    if ((m_wCMD == 0x82 || m_wCMD == 0x83)
            && (SCI_TX_cccv == 2 && Ibat_rms_tmp == 0)
            && (Warning_error_code == 0 && Grid_error_code == 0
                    && DC_error_code == 0 && Fault_error_code == 0)
            && Ex_Pattern_flag == 0)
    {
        m_nOp2_CurrZeroCnt++;
        if (5 < m_nOp2_CurrZeroCnt)
            Write2CAN();

    }
    else
    {
        m_nOp2_CurrZeroCnt = 0;
        Write2CAN();
    }

//	CAN_all_receive = 0;
}

interrupt void cana_rx_isr(void)
{
//	can_rx_cnt++;

//	Uint32 tmp = 0;
    unsigned char OPCODE;
    unsigned char PARALLELCODE;	//2015.4.30 Parallel add

    CANHandler();

#ifdef DSP_CH1

    if (SSID == 0xAA)
    {
        Pack_sensing1_flag = 1;
        Pack_sensing2_flag = 0;
        Pack_sensing3_flag = 0;
        Pack_sensing4_flag = 0;

    }
    if (SSID == 0xBB)
    {
        Pack_sensing1_flag = 0;
        Pack_sensing2_flag = 1;
        Pack_sensing3_flag = 0;
        Pack_sensing4_flag = 0;
    }
    if (SSID == 0xCC)
    {
        Pack_sensing1_flag = 0;
        Pack_sensing2_flag = 0;
        Pack_sensing3_flag = 1;
        Pack_sensing4_flag = 0;
    }
    if (SSID == 0xDD)
    {
        Pack_sensing1_flag = 0;
        Pack_sensing2_flag = 0;
        Pack_sensing3_flag = 0;
        Pack_sensing4_flag = 1;
    }
    if (SSID == 0xFF)
    {
        Pack_sensing1_flag = 0;
        Pack_sensing2_flag = 0;
        Pack_sensing3_flag = 0;
        Pack_sensing4_flag = 0;
    }

#if(DSP_COMMUNICATION_CHANNEL == 0)
    if (SSID == 0x100 || SSID == 0x300) //ksw :: ch1-> ID=100, ch2-> ID=200, ch1_2-> ID300 에서만 m_uSSID 을 셋팅 한다.
#else
    if (SSID == CAN_ID)
#endif
    {
        m_uSSID = SSID;
        m_wCMD = uc_rx_mbox3_data[0];
        /*
         tx_data.bit.CMD = m_wCMD + 0x10;
         //	tx_data.bit.OPCODE = SCI_TX_cccv & 0xf;	//20150427 Parallel add
         tx_data.bit.OPCODE_PARACODE = ((SCI_TX_cccv << 4) & 0xf0) | (SCI_Parallel_flag & 0x0f);		//OPCODE
         //	tx_data.bit.OPCODE_PARACODE = SCI_Parallel_flag & 0x0f;		//PARALLEL CODE
         Vbat_rms_tmp = (unsigned long)(Vpack_UI_rms * 1000);
         tx_data.bit.pack_vol_H = (Vbat_rms_tmp >> 16) & 0xff ;
         tx_data.bit.pack_vol_M = (Vbat_rms_tmp >>  8) & 0xff ;
         tx_data.bit.pack_vol_L = (Vbat_rms_tmp      ) & 0xff ;

         Ibat_rms_tmp = abs((long)(Ibat_UI_rms * 1000));
         tx_data.bit.pack_cur_H = (Ibat_rms_tmp >> 16) & 0xff ;
         tx_data.bit.pack_cur_M = (Ibat_rms_tmp >>  8) & 0xff ;
         tx_data.bit.pack_cur_L = (Ibat_rms_tmp      ) & 0xff ;
         */
        //m_CanConnectCheck = 1;	//2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가
        comm_chk_cnt = 0;
//		if((start_flag == 0 || SCI_Pattern_flag == 0) && ECanaMboxes.MBOX1.MDL.byte.BYTE0 == 0x88)
//			CAN_all_receive = 1;

        //	push_rx_buffer(SSID);

#if ( ORIGINAL_METHOD == 1)
        m_CanConnectCheck = 1;
#elif (TIMER2_METHOD == 1 )
        if (m_CanConnectCheck == 0) // 타이머 시작때만 한번만 시작만 살짝 빨리하기 위해서 하번만 체크한다 한번만...중요..
        {
            m_CanConnectCheck = 1;
            Timer2_count_clear(); // 내부적으로 살작 마스터가 미리 시작하도록 되어있다 타이머 시작을..
        }
#endif

#if (DSP_COMMUNICATION_CHANNEL == 0)
#if( TIMER2_SYNC_ENABLE == 1)
        // 실글이었다가 병렬데이타 0X300이 처음 들어올때만 타이머 싱크를 맞춰준다..오키?
        if ((prev_SSID != 0x300) && (SSID == 0x300))
        {
#if (TIMER2_ENABLE == 1)
            Timer2_count_clear();
#endif
            //check_timer_clear ++;
        }
        prev_SSID = SSID;

#endif
#endif
    }
#endif

#ifdef DSP_CH2

    if(SSID == 0xAA)
    {
        Pack_sensing1_flag= 1;
        Pack_sensing2_flag= 0;
        Pack_sensing3_flag= 0;
        Pack_sensing4_flag= 0;

    }
    if(SSID == 0xBB)
    {
        Pack_sensing1_flag= 0;
        Pack_sensing2_flag= 1;
        Pack_sensing3_flag= 0;
        Pack_sensing4_flag= 0;
    }
    if(SSID == 0xCC)
    {
        Pack_sensing1_flag= 0;
        Pack_sensing2_flag= 0;
        Pack_sensing3_flag= 1;
        Pack_sensing4_flag= 0;
    }
    if(SSID == 0xDD)
    {
        Pack_sensing1_flag= 0;
        Pack_sensing2_flag= 0;
        Pack_sensing3_flag= 0;
        Pack_sensing4_flag= 1;
    }
    if(SSID == 0xFF)
    {
        Pack_sensing1_flag= 0;
        Pack_sensing2_flag= 0;
        Pack_sensing3_flag= 0;
        Pack_sensing4_flag= 0;
    }

    if(SSID == 0x200 || SSID == 0x300) //ksw :: ch1-> ID=100, ch2-> ID=200, ch1_2-> ID300 에서만 m_uSSID 을 셋팅 한다.
    {
        m_uSSID = SSID;
        m_wCMD = uc_rx_mbox3_data[0];
        /*
         tx_data.bit.CMD = m_wCMD + 0x10;
         //	tx_data.bit.OPCODE = SCI_TX_cccv & 0xf;	//20150427 Parallel add
         tx_data.bit.OPCODE_PARACODE = ((SCI_TX_cccv << 4) & 0xf0) | (SCI_Parallel_flag & 0x0f);		//OPCODE
         //	tx_data.bit.OPCODE_PARACODE = SCI_Parallel_flag & 0x0f;		//PARALLEL CODE
         Vbat_rms_tmp = (unsigned long)(Vpack_UI_rms * 1000);
         tx_data.bit.pack_vol_H = (Vbat_rms_tmp >> 16) & 0xff ;
         tx_data.bit.pack_vol_M = (Vbat_rms_tmp >>  8) & 0xff ;
         tx_data.bit.pack_vol_L = (Vbat_rms_tmp      ) & 0xff ;

         Ibat_rms_tmp = abs((long)(Ibat_UI_rms * 1000));
         tx_data.bit.pack_cur_H = (Ibat_rms_tmp >> 16) & 0xff ;
         tx_data.bit.pack_cur_M = (Ibat_rms_tmp >>  8) & 0xff ;
         tx_data.bit.pack_cur_L = (Ibat_rms_tmp      ) & 0xff ;
         */
        //m_CanConnectCheck = 1;	//2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가 - 'remove by kim won kyum @ 200225'
        comm_chk_cnt = 0;
//		if((start_flag == 0 || SCI_Pattern_flag == 0) && ECanaMboxes.MBOX1.MDL.byte.BYTE0 == 0x88)
//			CAN_all_receive = 1;




        //not running at a SLAVE mode
        //fixed this 'kim won kyum' 200225
        #if ( ORIGINAL_METHOD == 1)
            m_CanConnectCheck = 1;
        #elif (TIMER2_METHOD == 1 )
            if (m_CanConnectCheck == 0) // 타이머 시작때만 한번만 시작만 살짝 빨리하기 위해서 하번만 체크한다 한번만...중요..
            {
                m_CanConnectCheck = 1;
                Timer2_count_clear(); // 내부적으로 살작 마스터가 미리 시작하도록 되어있다 타이머 시작을..
            }
        #endif

        #if( TIMER2_SYNC_ENABLE == 1)
            // 실글이었다가 병렬데이타 0X300이 처음 들어올때만 타이머 싱크를 맞춰준다..오키?
            if ((prev_SSID != 0x300) && (SSID == 0x300))
            {
                #if (TIMER2_ENABLE == 1)
                    Timer2_count_clear();
                #endif
                //check_timer_clear ++;
            }
            prev_SSID = SSID;
        #endif




    }
#endif
//	 comm_chk_cnt = 0;
//	 if(SSID == 0x110 || SSID == 0x120 || SSID == 0x130 || SSID == 0x210 || SSID == 0x220 || SSID == 0x230)
//	 {
//		 // CAN 메일박스에 데이터가 수신되면 해당 CANRMP 레지스터 비트를 클리어.
//		ECanaShadow.CANRMP.all = 0;
//		ECanaShadow.CANRMP.bit.RMP1 = 1;
//		ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;
//
//		PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;			// Acknowledge interrupt to PIE
//		return;
//	 }

#ifdef DSP_CH1
#if(DSP_COMMUNICATION_CHANNEL == 0)

    if (SSID == 0x101)		//2016.03.10 kys cv on add
#else
    if (SSID == CAN_ID + 0x01 )
#endif
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0]; //ECanaMboxes.MBOX1.MDL.byte.BYTE0;
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1]; //ECanaMboxes.MBOX1.MDL.byte.BYTE1;
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2]; //ECanaMboxes.MBOX1.MDL.byte.BYTE2;
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3]; //ECanaMboxes.MBOX1.MDL.byte.BYTE3;
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4]; //ECanaMboxes.MBOX1.MDH.byte.BYTE4;
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5]; //ECanaMboxes.MBOX1.MDH.byte.BYTE5;
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6]; //ECanaMboxes.MBOX1.MDH.byte.BYTE6;
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7]; //ECanaMboxes.MBOX1.MDH.byte.BYTE7;
        if (rx_can_frame.frame[0] == 0x1)
        {
//				Pack_Cycler_flag = 1;
//				CV_ON_flag = 0;
        }
        else if (rx_can_frame.frame[0] == 0x2)
        {
//				Pack_Cycler_flag = 0;
//				CV_ON_flag = 1;
        }
        else
        {
//				Pack_Cycler_flag = 1;
//				CV_ON_flag = 0;
        }

//			if(Pack_Cycler_flag == 1){
//				Vbat_CV_Under =10;
//			}
//			else if(CV_ON_flag == 1){
//				tmp_limit_volt = (((Uint32)rx_can_frame.frame[2]) << 16) | (rx_can_frame.frame[3]<< 8 )	//CV_under_limit_Volt(v)  181107
//						| rx_can_frame.frame[4] ;
//				Vbat_CV_Under = (float)tmp_limit_volt * 0.001 ;
//			}

    }
#endif
#ifdef DSP_CH2
    if(SSID == 0x201)		//2016.03.10 kys cv on add
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0]; //ECanaMboxes.MBOX1.MDL.byte.BYTE0;
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];//ECanaMboxes.MBOX1.MDL.byte.BYTE1;
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];//ECanaMboxes.MBOX1.MDL.byte.BYTE2;
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];//ECanaMboxes.MBOX1.MDL.byte.BYTE3;
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];//ECanaMboxes.MBOX1.MDH.byte.BYTE4;
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];//ECanaMboxes.MBOX1.MDH.byte.BYTE5;
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];//ECanaMboxes.MBOX1.MDH.byte.BYTE6;
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];//ECanaMboxes.MBOX1.MDH.byte.BYTE7;
        if(rx_can_frame.frame[0] == 0x1)
        {
            Pack_Cycler_flag = 1;
            CV_ON_flag = 0;
        }
        else if(rx_can_frame.frame[0] == 0x2)
        {
            Pack_Cycler_flag = 0;
            CV_ON_flag = 1;
        }
        else
        {
            Pack_Cycler_flag = 1;
            CV_ON_flag = 0;
        }

//			if(Pack_Cycler_flag == 1){
//				Vbat_CV_Under =10;
//			}
//			else if(CV_ON_flag == 1){
//				tmp_limit_volt = (((Uint32)rx_can_frame.frame[2]) << 16) | (rx_can_frame.frame[3]<< 8 )	//CV_under_limit_Volt(v)  181107
//						| rx_can_frame.frame[4] ;
//				Vbat_CV_Under = (float)tmp_limit_volt * 0.001 ;
//			}

    }
#endif
#ifdef DSP_CH1

#if (DSP_COMMUNICATION_CHANNEL == 0)
//		if((SSID == 0x100 || SSID == 0x300) && CAN_all_receive == 0)		//2015.4.30 DSP1 에서는 0x200을 막는다.
    if ((SSID == 0x100 || SSID == 0x300))		//2015.4.30 DSP1 에서는 0x200을 막는다.
#else
    if (SSID == CAN_ID)
#endif
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];
        /*			if(rx_can_frame.frame[0] == 0x80)	//20150610
         {
         int k;
         for(k = 0; k < 50; k++)
         {
         m_stPatBuff_CH1_1[k].szCmd = 0;
         m_stPatBuff_CH1_1[k].szOp = 0;
         m_stPatBuff_CH1_1[k].nOpFlag = 0;
         m_stPatBuff_CH1_1[k].nCpFlag= 0;
         m_stPatBuff_CH1_1[k].szPara = 0;
         m_stPatBuff_CH1_1[k].nParaFlag= 0;
         m_stPatBuff_CH1_1[k].nTime = 0;
         m_stPatBuff_CH1_1[k].dVolt = 0;
         m_stPatBuff_CH1_1[k].dCurr = 0;
         m_stPatBuff_CH1_1[k].dPow = 0;
         }
         for(k = 0; k < 50; k++)
         {
         m_stPatBuff_CH1_2[k].szCmd = 0;
         m_stPatBuff_CH1_2[k].szOp = 0;
         m_stPatBuff_CH1_2[k].nOpFlag = 0;
         m_stPatBuff_CH1_2[k].nCpFlag= 0;
         m_stPatBuff_CH1_2[k].szPara = 0;
         m_stPatBuff_CH1_2[k].nParaFlag= 0;
         m_stPatBuff_CH1_2[k].nTime = 0;
         m_stPatBuff_CH1_2[k].dVolt = 0;
         m_stPatBuff_CH1_2[k].dCurr = 0;
         m_stPatBuff_CH1_2[k].dPow = 0;
         }
         }
         */
        if (rx_can_frame.frame[0] == 0x88)	//0x88 외부패턴일때
        {
            if (m_nStepCount_CH1 == 0)		//전체스텝카운터가 0이면 버퍼저장 카운터를 0으로 초기화.
            {
                m_nBufCnt_105[0] = 0;
                m_nBufCnt_105[1] = 0;
                m_nBufCnt_106[0] = 0;
                m_nBufCnt_106[1] = 0;
                m_nBufCnt_107[0] = 0;
                m_nBufCnt_107[1] = 0;

                m_nSendBufCnt_CH1[0] = 0;
                m_nSendBufCnt_CH1[1] = 0;
                Pattern_read_flag = 1;
            }
            m_Patt_Flag = 1;
            Real_Pattern_cnt = 0; //2015.06.14 kys 외부패턴 모드가 아니면서 PC로부터 CAN 데이터를 수신받으면 시작한다.
        }
        else if (rx_can_frame.frame[0] == 0x84 || rx_can_frame.frame[0] == 0x86) // 84,86 커맨드 입력받을시 버퍼 초기화
        {
            m_RevBufflag_105 = 0;
            m_RevBufflag_106 = 0;
            m_RevBufflag_107 = 0;
            m_nBufCnt_105[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_105[1] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[1] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[1] = 0;		//pc->dsp  time data count init
            Pattern_cnt = 0;
            R_100ms_flag = 0;

            m_nSendBufCnt_CH1[0] = 0;
            m_nSendBufCnt_CH1[1] = 0;

            int k;
            for (k = 0; k < 50; k++)
            {
                /*					m_stPatBuff_CH1_1[k].szCmd = 0;
                 m_stPatBuff_CH1_1[k].szOp = 0;
                 m_stPatBuff_CH1_1[k].nOpFlag = 0;
                 m_stPatBuff_CH1_1[k].nCpFlag= 0;
                 m_stPatBuff_CH1_1[k].szPara = 0;
                 m_stPatBuff_CH1_1[k].nParaFlag= 0;*/
                m_stPatBuff_CH1_1[k].nTime = 0;
                /*					m_stPatBuff_CH1_1[k].dVolt = 0;
                 m_stPatBuff_CH1_1[k].dCurr = 0;
                 m_stPatBuff_CH1_1[k].dPow = 0;*/

                /*					m_stPatBuff_CH1_2[k].szCmd = 0;
                 m_stPatBuff_CH1_2[k].szOp = 0;
                 m_stPatBuff_CH1_2[k].nOpFlag = 0;
                 m_stPatBuff_CH1_2[k].nCpFlag= 0;
                 m_stPatBuff_CH1_2[k].szPara = 0;
                 m_stPatBuff_CH1_2[k].nParaFlag= 0;*/
                m_stPatBuff_CH1_2[k].nTime = 0;
                /*					m_stPatBuff_CH1_2[k].dVolt = 0;
                 m_stPatBuff_CH1_2[k].dCurr = 0;
                 m_stPatBuff_CH1_2[k].dPow = 0;*/
            }

            m_nStepCount_CH1 = 0;	//CMD가 0x88이 아닐때 외부패턴 스텝 카운트 버퍼 초기화
            m_SendBufflag_CH1 = 0;
            m_Patt_Flag = 0;
        }
        else
        {
            m_nStepCount_CH1 = 0;	//CMD가 0x88이 아닐때 외부패턴 스텝 카운트 버퍼 초기화
            m_SendBufflag_CH1 = 0;
            m_Patt_Flag = 0;
        }

        if (rx_can_frame.frame[0] == 0x86)	//2015.07.10 pattern
        {
            Pattern_read_flag = 1;
            R_100ms_flag = 0;
        }

        if (rx_can_frame.frame[0] != 0x88)
        {
            switch (m_wCMD)
            {
            case 0x80:
                //				Rx_request_data = 80;
                SCI_Grid_ON_flag = 1;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x81:
                //				Rx_request_data = 81;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 1;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x82:
                //				Rx_request_data = 82;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 1;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x83:
                //				Rx_request_data = 83;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 1;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x84:
                //				Rx_request_data = 84;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 1;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x85:
                //				Rx_request_data = 85;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 1;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x86:
                //				Rx_request_data = 86;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 1;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x88:
                //				Rx_request_data = 88;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                //				SCI_Charge_flag    = 0;
                //				SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 1;
                SCI_Err_Clear_flag = 0;
                break;
            case 0x89:
                //				Rx_request_data = 89;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 1;
                break;

            default:
                tx_data.bit.CMD = m_wCMD;
                //tx_can_frame.data.bit.CMD = 0x90;
                break;
            }

            //0x88 외부패턴일때
            OPCODE = (rx_can_frame.bit.OPCODE_PARACODE >> 4) & 0xf;

            if (OPCODE == 0x1)
                SCI_CCCV_CP_CR_flag = 1;   //CC/CV
            else if (OPCODE == 0x2)
                SCI_CCCV_CP_CR_flag = 2;   //CP
            else if (OPCODE == 0x3)
                SCI_CCCV_CP_CR_flag = 3;
            else if (OPCODE == 0x4)
                SCI_CCCV_CP_CR_flag = 4;
            else if (OPCODE == 0x5)
                SCI_CCCV_CP_CR_flag = 5;
            else if (OPCODE == 0x6)
                SCI_CCCV_CP_CR_flag = 6;
            else if (OPCODE == 0x7)
                SCI_CCCV_CP_CR_flag = 7;
            else if (OPCODE == 0x8)
                SCI_CCCV_CP_CR_flag = 8;
            else if (OPCODE == 0x9)
                SCI_CCCV_CP_CR_flag = 9;
            else
                SCI_CCCV_CP_CR_flag = 0;

            PARALLELCODE = rx_can_frame.bit.OPCODE_PARACODE & 0xf; //20150427 Parallel add

            if ((sys_mode <= 3) && (Direct_CHA_DIS_flag == 0))
            {
                if (PARALLELCODE == 0x0)
                    SCI_Parallel_flag = 0;  //PARALLEL OFF & SINGLE MODE ON 확인필요
                else if (PARALLELCODE == 0x1)
                    SCI_Parallel_flag = 1;   //PARALLEL ON  & SINGLE MODE OFF
            }

            m_szRealCmdBuf = rx_can_frame.frame[0];
            if (m_nInitRealTimeChk == 0)
            {
                m_szCmdBuf = rx_can_frame.frame[0];
                Real_Pattern_cnt = 0;
                m_nInitRealTimeChk = 1;
            }

            if (m_szRealCmdBuf != m_szCmdBuf)
            {
                m_szCmdBuf = rx_can_frame.frame[0];
                Real_Pattern_cnt = 0;
            }

            if (rx_can_frame.frame[0] == 0x82 || rx_can_frame.frame[0] == 0x83)
            {	//0x88 외부패턴일때 //20150522 add

                tmp_volt = (((Uint32) rx_can_frame.bit.pack_vol_H) << 16)
                        | (rx_can_frame.bit.pack_vol_M << 8)	//Volt(v)
                        | rx_can_frame.bit.pack_vol_L;
                SCI_Voltage_ref = (float) tmp_volt * 0.001;

                tmp_curr = (((Uint32) rx_can_frame.bit.pack_cur_H) << 16)
                        | (rx_can_frame.bit.pack_cur_M << 8)	//Curr(c)
                        | rx_can_frame.bit.pack_cur_L;

                SCI_Current_ref = (float) tmp_curr * 0.001;

                if (SCI_CCCV_CP_CR_flag == 7 || SCI_CCCV_CP_CR_flag == 8)//Power(w)
                {
                    CP_confirm_flag = 1;
                    //				SCI_CP_Power = tmp_curr;
                    SCI_Current_ref = (float) tmp_curr * 0.001;
                }
                else
                    CP_confirm_flag = 0;

                //			if((SCI_CCCV_CP_CR_flag == 2) && (CP_confirm_flag == 1))										//2015.05.04 확인필요
                //			{
                //				Ibat_over = SCI_Current_ref + 10. ;
                //				if(Ibat_over >=110) Ibat_over = 110;
                //			}

                tmp_volt = 0;
                tmp_curr = 0;
            }		//20150522 add
                    //20150522 add
//				CAN_all_receive = 1 ;//2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 막음
        }
        //		Rx_all_receive = 1;
        //		CAN_all_receive = 1 ;
        if (rx_can_frame.frame[0] == 0x88)	//0x88 외부패턴일때
        {
            CMD = rx_can_frame.bit.CMD;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 0;
            SCI_Pattern_flag = 1;	// 처음 스타트 되는곳.
            SCI_Err_Clear_flag = 0;
        }

        if (rx_can_frame.frame[0] == 0x86)	//0x86 외부패턴 정지
        {
            CMD = rx_can_frame.bit.CMD;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
        }
    }
#endif
#ifdef DSP_CH2
//	if((SSID == 0x200 || SSID == 0x300 ) && CAN_all_receive == 0)		//2015.4.30 DSP2에서는 0x100을 막는다.//현재 dsp 1 사용이다.
    if((SSID == 0x200 || SSID == 0x300 ))//2015.4.30 DSP2에서는 0x100을 막는다.//현재 dsp 1 사용이다.
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];
        /*
         if(rx_can_frame.frame[0] == 0x81)	//20150610
         {
         int k;
         for(k = 0; k < 50; k++)
         {
         m_stPatBuff_CH2_1[k].szCmd = 0;
         m_stPatBuff_CH2_1[k].szOp = 0;
         m_stPatBuff_CH2_1[k].nOpFlag = 0;
         m_stPatBuff_CH2_1[k].nCpFlag= 0;
         m_stPatBuff_CH2_1[k].szPara = 0;
         m_stPatBuff_CH2_1[k].nParaFlag= 0;
         m_stPatBuff_CH2_1[k].nTime = 0;
         m_stPatBuff_CH2_1[k].dVolt = 0;
         m_stPatBuff_CH2_1[k].dCurr = 0;
         m_stPatBuff_CH2_1[k].dPow = 0;
         }
         for(k = 0; k < 50; k++)
         {
         m_stPatBuff_CH2_2[k].szCmd = 0;
         m_stPatBuff_CH2_2[k].szOp = 0;
         m_stPatBuff_CH2_2[k].nOpFlag = 0;
         m_stPatBuff_CH2_2[k].nCpFlag= 0;
         m_stPatBuff_CH2_2[k].szPara = 0;
         m_stPatBuff_CH2_2[k].nParaFlag= 0;
         m_stPatBuff_CH2_2[k].nTime = 0;
         m_stPatBuff_CH2_2[k].dVolt = 0;
         m_stPatBuff_CH2_2[k].dCurr = 0;
         m_stPatBuff_CH2_2[k].dPow = 0;
         }
         }
         */
        if(rx_can_frame.frame[0] == 0x88)	//0x88 외부패턴일때
        {
            if(m_nStepCount_CH2 == 0)	//전체스텝카운터가 0이면 버퍼저장 카운터를 0으로 초기화.
            {
                m_nBufCnt_205[0] = 0;
                m_nBufCnt_205[1] = 0;
                m_nBufCnt_206[0] = 0;
                m_nBufCnt_206[1] = 0;
                m_nBufCnt_207[0] = 0;
                m_nBufCnt_207[1] = 0;

                m_nSendBufCnt_CH2[0] = 0;
                m_nSendBufCnt_CH2[1] = 0;
                Pattern_read_flag = 1;
            }
            m_Patt_Flag = 1;
            Real_Pattern_cnt = 0; //2015.06.14 kys 외부패턴 모드가 아니면서 PC로부터 CAN 데이터를 수신받으면 시작한다.
        }
        else if(rx_can_frame.frame[0] == 0x84 || rx_can_frame.frame[0] == 0x86)	// 84,86 커맨드 입력받을시 버퍼 초기화
        {
            m_RevBufflag_205 = 0;
            m_RevBufflag_206 = 0;
            m_RevBufflag_207 = 0;
            m_nBufCnt_205[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_206[0] = 0;//pc->dsp  time data count init
            m_nBufCnt_207[0] = 0;//pc->dsp  time data count init
            m_nBufCnt_205[1] = 0;//pc->dsp  value data count init
            m_nBufCnt_206[1] = 0;//pc->dsp  time data count init
            m_nBufCnt_207[1] = 0;//pc->dsp  time data count init
            Pattern_cnt = 0;
            R_100ms_flag =0;

            m_nSendBufCnt_CH2[0] = 0;
            m_nSendBufCnt_CH2[1] = 0;

            int k;
            for(k = 0; k < 50; k++)
            {
                /*				m_stPatBuff_CH2_1[k].szCmd = 0;
                 m_stPatBuff_CH2_1[k].szOp = 0;
                 m_stPatBuff_CH2_1[k].nOpFlag = 0;
                 m_stPatBuff_CH2_1[k].nCpFlag= 0;
                 m_stPatBuff_CH2_1[k].szPara = 0;
                 m_stPatBuff_CH2_1[k].nParaFlag= 0;*/
                m_stPatBuff_CH2_1[k].nTime = 0;
                /*				m_stPatBuff_CH2_1[k].dVolt = 0;
                 m_stPatBuff_CH2_1[k].dCurr = 0;
                 m_stPatBuff_CH2_1[k].dPow = 0;

                 m_stPatBuff_CH2_2[k].szCmd = 0;
                 m_stPatBuff_CH2_2[k].szOp = 0;
                 m_stPatBuff_CH2_2[k].nOpFlag = 0;
                 m_stPatBuff_CH2_2[k].nCpFlag= 0;
                 m_stPatBuff_CH2_2[k].szPara = 0;
                 m_stPatBuff_CH2_2[k].nParaFlag= 0;*/
                m_stPatBuff_CH2_2[k].nTime = 0;
                /*				m_stPatBuff_CH2_2[k].dVolt = 0;
                 m_stPatBuff_CH2_2[k].dCurr = 0;
                 m_stPatBuff_CH2_2[k].dPow = 0;*/
            }

            m_nStepCount_CH2 = 0;	//CMD가 0x88이 아닐때 외부패턴 스텝 카운트 버퍼 초기화
            m_SendBufflag_CH2 = 0;
            m_Patt_Flag = 0;
        }
        else
        {
            m_nStepCount_CH2 = 0;	//CMD가 0x88이 아닐때 외부패턴 스텝 카운트 버퍼 초기화
            m_SendBufflag_CH2 = 0;
            m_Patt_Flag = 0;
        }

        if(rx_can_frame.frame[0] == 0x86)	//2015.07.10 pattern
        {
            Pattern_read_flag = 1;
            R_100ms_flag =0;
        }

        if(rx_can_frame.frame[0] != 0x88)
        {	//0x88 외부패턴일때

            switch(m_wCMD)
            {
                case 0x80 :
                //				Rx_request_data = 80;
                SCI_Grid_ON_flag = 1;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x81 :
                //				Rx_request_data = 81;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 1;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x82 :
                //				Rx_request_data = 82;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 1;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x83 :
                //				Rx_request_data = 83;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 1;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x84 :
                //				Rx_request_data = 84;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 1;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x85 :
                //				Rx_request_data = 85;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 1;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x86 :
                //				Rx_request_data = 86;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 1;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x88 :
                //				Rx_request_data = 88;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                //				SCI_Charge_flag    = 0;
                //				SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 1;
                SCI_Err_Clear_flag = 0;
                break;
                case 0x89 :
                //				Rx_request_data = 89;
                SCI_Grid_ON_flag = 0;
                SCI_Pack_ON_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                SCI_Pack_OFF_flag = 0;
                SCI_All_Stop_flag = 0;
                SCI_Standby_flag = 0;
                SCI_Pattern_flag = 0;
                SCI_Err_Clear_flag = 1;
                break;

                default :
                tx_data.bit.CMD = m_wCMD;
                //tx_can_frame.data.bit.CMD = 0x90;
                break;
            }

            OPCODE = (rx_can_frame.bit.OPCODE_PARACODE >> 4) & 0xf;

            if(OPCODE == 0x1) SCI_CCCV_CP_CR_flag = 1;   //CC/CV
            else if(OPCODE == 0x2) SCI_CCCV_CP_CR_flag = 2;//CP
            else if(OPCODE == 0x3) SCI_CCCV_CP_CR_flag = 3;
            else if(OPCODE == 0x4) SCI_CCCV_CP_CR_flag = 4;
            else if(OPCODE == 0x5) SCI_CCCV_CP_CR_flag = 5;
            else if(OPCODE == 0x6) SCI_CCCV_CP_CR_flag = 6;
            else if(OPCODE == 0x7) SCI_CCCV_CP_CR_flag = 7;
            else if(OPCODE == 0x8) SCI_CCCV_CP_CR_flag = 8;
            else if(OPCODE == 0x9) SCI_CCCV_CP_CR_flag = 9;
            else SCI_CCCV_CP_CR_flag = 0;

            PARALLELCODE = rx_can_frame.bit.OPCODE_PARACODE & 0xf;//20150427 Parallel add

            if((sys_mode <=3)&&(Direct_CHA_DIS_flag == 0))
            {
                if(PARALLELCODE == 0x0) SCI_Parallel_flag = 0; //PARALLEL OFF & SINGLE MODE ON 확인필요
                else if(PARALLELCODE == 0x1) SCI_Parallel_flag = 1;//PARALLEL ON  & SINGLE MODE OFF
            }

            m_szRealCmdBuf = rx_can_frame.frame[0];
            if(m_nInitRealTimeChk == 0)
            {
                m_szCmdBuf = rx_can_frame.frame[0];
                Real_Pattern_cnt = 0;
                m_nInitRealTimeChk = 1;
            }

            if(m_szRealCmdBuf != m_szCmdBuf)
            {
                m_szCmdBuf = rx_can_frame.frame[0];
                Real_Pattern_cnt = 0;
            }

            if(rx_can_frame.frame[0] == 0x82 || rx_can_frame.frame[0] == 0x83)
            {	//0x88 외부패턴일때 //20150522 add

                tmp_volt = (((Uint32)rx_can_frame.bit.pack_vol_H) << 16) | (rx_can_frame.bit.pack_vol_M << 8 )//Volt(v)
                | rx_can_frame.bit.pack_vol_L;

                SCI_Voltage_ref = (float)tmp_volt * 0.001;

                tmp_curr = (((Uint32)rx_can_frame.bit.pack_cur_H) << 16) | (rx_can_frame.bit.pack_cur_M << 8 )//Curr(c)
                | rx_can_frame.bit.pack_cur_L;

                SCI_Current_ref = (float)tmp_curr * 0.001;

                if(SCI_CCCV_CP_CR_flag == 7 || SCI_CCCV_CP_CR_flag == 8)//Power(w)
                {
                    CP_confirm_flag = 1;
                    //	SCI_CP_Power = tmp_curr;
                    SCI_Current_ref = (float)tmp_curr * 0.001;
                }
                else
                CP_confirm_flag = 0;

                //		if((SCI_CCCV_CP_CR_flag == 2) && (CP_confirm_flag == 1))										//2015.05.04 확인필요
                //		{
                //			Ibat_over = SCI_Current_ref + 10. ;
                //			if(Ibat_over >=110) Ibat_over = 110;
                //		}

                tmp_volt = 0;
                tmp_curr = 0;
            }	//20150522 add
//			CAN_all_receive = 1 ;//2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 막음
        }
//		Rx_all_receive = 1;
//		CAN_all_receive = 1 ;

        if(rx_can_frame.frame[0] == 0x88)//0x88 외부패턴일때
        {
            CMD = rx_can_frame.bit.CMD;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 0;
            SCI_Pattern_flag = 1;
            SCI_Err_Clear_flag = 0;
        }

        if(rx_can_frame.frame[0] == 0x86)	//0x86 외부패턴 정지
        {
            CMD = rx_can_frame.bit.CMD;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
        }
    }
#endif
// 외부패턴 add 2015.05.02
#ifdef DSP_CH1
#if(DSP_COMMUNICATION_CHANNEL == 0)
    if ((SSID == 0x105 || SSID == 0x305))	//2015.4.30 Parallel add
#else
    if  (SSID == CAN_ID + 0x05)
#endif
    {
//		can_rx_cnt++;
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];

        m_chkExtCount_105++;

        OPCODE = (rx_can_frame.bit.OPCODE_PARACODE >> 4) & 0xf;

        if (OPCODE == 0x1)
            nSCI_CCCV_CP_CR_flag = 1;   //CC/CV
        else if (OPCODE == 0x2)
            nSCI_CCCV_CP_CR_flag = 2;   //CP
        else if (OPCODE == 0x3)
            nSCI_CCCV_CP_CR_flag = 3;
        else if (OPCODE == 0x4)
            nSCI_CCCV_CP_CR_flag = 4;
        else if (OPCODE == 0x5)
            nSCI_CCCV_CP_CR_flag = 5;
        else if (OPCODE == 0x6)
            nSCI_CCCV_CP_CR_flag = 6;
        else if (OPCODE == 0x7)
            nSCI_CCCV_CP_CR_flag = 7;
        else if (OPCODE == 0x8)
            nSCI_CCCV_CP_CR_flag = 8;
        else if (OPCODE == 0x9)
            nSCI_CCCV_CP_CR_flag = 9;
        else
            nSCI_CCCV_CP_CR_flag = 0;

        PARALLELCODE = rx_can_frame.bit.OPCODE_PARACODE & 0xf; //20150427 Parallel add

        if (PARALLELCODE == 0x0)
            nSCI_Parallel_flag = 0;   //PARALLEL OFF & SINGLE MODE ON 확인필요
        else if (PARALLELCODE == 0x1)
            nSCI_Parallel_flag = 1;   //PARALLEL ON  & SINGLE MODE OFF

        tmp_volt = (((Uint32) rx_can_frame.bit.pack_vol_H) << 16)
                | (rx_can_frame.bit.pack_vol_M << 8)	//Volt(v)
                | rx_can_frame.bit.pack_vol_L;
        m_fltVolt = (float) tmp_volt * 0.001;

        tmp_curr = (((Uint32) rx_can_frame.bit.pack_cur_H) << 16)
                | (rx_can_frame.bit.pack_cur_M << 8)	//Curr(c)
                | rx_can_frame.bit.pack_cur_L;
        m_fltCurr = (float) tmp_curr * 0.001;

        if (nSCI_CCCV_CP_CR_flag == 7 || nSCI_CCCV_CP_CR_flag == 8)	//Power(w)
        {
            nCP_confirm_flag = 1;
            m_fltCp = tmp_curr;
        }
        else
            nCP_confirm_flag = 0;

//		if((SCI_CCCV_CP_CR_flag == 2) && (CP_confirm_flag == 1))										//2015.05.04 확인필요
//		{
//			Ibat_over = SCI_Current_ref + 10. ;
//			if(Ibat_over >=110) Ibat_over = 110;
//		}

        if (m_RevBufflag_105 == 0)
        {
            if (m_nBufCnt_105[0] == 0)
            {
//				memset(m_stPatBuff_CH1_1,0,sizeof(m_stPatBuff_CH1_1));
                int k;
                for (k = 0; k < 50; k++)
                {
                    m_stPatBuff_CH1_1[k].szCmd = 0;
                    m_stPatBuff_CH1_1[k].szOp = 0;
                    m_stPatBuff_CH1_1[k].nOpFlag = 0;
                    m_stPatBuff_CH1_1[k].nCpFlag = 0;
                    m_stPatBuff_CH1_1[k].szPara = 0;
                    m_stPatBuff_CH1_1[k].nParaFlag = 0;
                    m_stPatBuff_CH1_1[k].nTime = 0;
                    m_stPatBuff_CH1_1[k].dVolt = 0;
                    m_stPatBuff_CH1_1[k].dCurr = 0;
                    m_stPatBuff_CH1_1[k].dPow = 0;
                }
            }
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].szCmd = rx_can_frame.bit.CMD;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].szOp = OPCODE;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].nOpFlag = nSCI_CCCV_CP_CR_flag;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].nCpFlag = nCP_confirm_flag;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].szPara = PARALLELCODE;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].nParaFlag = nSCI_Parallel_flag;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].dVolt = m_fltVolt;
            m_stPatBuff_CH1_1[m_nBufCnt_105[0]].dCurr = m_fltCurr;
            m_nBufCnt_105[0]++;
            if (m_nBufCnt_105[0] == 50)
            {
                m_nBufCnt_105[0] = 0;
                m_RevBufflag_105 = 1;
            }
//			if(m_stPatBuff_CH1_1[(m_nBufCnt_105[0]-1)].szCmd == 0x86)
//				m_RevBufflag_105 = 0;
        }
        else if (m_RevBufflag_105 == 1)
        {
            if (m_nBufCnt_105[1] == 0)
            {
//				memset(m_stPatBuff_CH1_2,0,sizeof(m_stPatBuff_CH1_2));
                int k;
                for (k = 0; k < 50; k++)
                {
                    m_stPatBuff_CH1_2[k].szCmd = 0;
                    m_stPatBuff_CH1_2[k].szOp = 0;
                    m_stPatBuff_CH1_2[k].nOpFlag = 0;
                    m_stPatBuff_CH1_2[k].nCpFlag = 0;
                    m_stPatBuff_CH1_2[k].szPara = 0;
                    m_stPatBuff_CH1_2[k].nParaFlag = 0;
                    m_stPatBuff_CH1_2[k].nTime = 0;
                    m_stPatBuff_CH1_2[k].dVolt = 0;
                    m_stPatBuff_CH1_2[k].dCurr = 0;
                    m_stPatBuff_CH1_2[k].dPow = 0;
                }
            }
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].szCmd = rx_can_frame.bit.CMD;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].szOp = OPCODE;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].nOpFlag = nSCI_CCCV_CP_CR_flag;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].nCpFlag = nCP_confirm_flag;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].szPara = PARALLELCODE;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].nParaFlag = nSCI_Parallel_flag;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].dVolt = m_fltVolt;
            m_stPatBuff_CH1_2[m_nBufCnt_105[1]].dCurr = m_fltCurr;
            m_nBufCnt_105[1]++;
            /*			if(m_nBufCnt_105[1] == 50 || m_stPatBuff_CH1_2[(m_nBufCnt_105[1]-1)].szCmd == 0x86)
             {
             m_nBufCnt_105[1] = 0;
             m_RevBufflag_105 = 0;
             }*/
            if (m_nBufCnt_105[1] == 50)
            {
                m_nBufCnt_105[1] = 0;
                m_RevBufflag_105 = 0;
            }
        }
        tmp_volt = 0;
        tmp_curr = 0;
//		CAN_all_receive = 1 ;	//150526
    }
#if(DSP_COMMUNICATION_CHANNEL == 0)
    if ((SSID == 0x106 || SSID == 0x306))   //2015.4.30 Parallel add
#else
    if  (SSID == CAN_ID + 0x06)
#endif
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];

        m_chkExtCount_106++;
        //cp
        tmp_cp = (((Uint32) rx_can_frame.ext_bit.ext_pow_H) << 24)
                | ((Uint32) (rx_can_frame.ext_bit.ext_pow_M_1) << 16)
                | (rx_can_frame.ext_bit.ext_pow_M_2 << 8)
                | rx_can_frame.ext_bit.ext_pow_L;

        m_fltCp = (float) tmp_cp * 0.001;

        //time
        tmp_time = (((Uint32) rx_can_frame.ext_bit.ext_time_H) << 24)
                | ((Uint32) (rx_can_frame.ext_bit.ext_time_M_1) << 16)
                | (rx_can_frame.ext_bit.ext_time_M_2 << 8)
                | rx_can_frame.ext_bit.ext_time_L;

        if ((tmp_time % 10) != 0)//20150527 만약 시간이 100일때 99 이런식으로 저장될때 1을 더해준다.
            tmp_time += 1;

        if (m_RevBufflag_106 == 0)
        {

            m_stPatBuff_CH1_1[m_nBufCnt_106[0]].dPow = m_fltCp;
            m_stPatBuff_CH1_1[m_nBufCnt_106[0]].nTime = tmp_time;
            m_nBufCnt_106[0]++;
            if (m_nBufCnt_106[0] == 50)
            {
                m_nBufCnt_106[0] = 0;
                m_RevBufflag_106 = 1;
            }
        }
        else if (m_RevBufflag_106 == 1)
        {
            m_stPatBuff_CH1_2[m_nBufCnt_106[1]].dPow = m_fltCp;
            m_stPatBuff_CH1_2[m_nBufCnt_106[1]].nTime = tmp_time;

            m_nBufCnt_106[1]++;
            if (m_nBufCnt_106[1] == 50)
            {
                m_nBufCnt_106[1] = 0;
                m_RevBufflag_106 = 0;
            }
        }
        tmp_volt = 0;
        tmp_curr = 0;
//		CAN_all_receive = 1 ;
    }
#if(DSP_COMMUNICATION_CHANNEL == 0)
    if ((SSID == 0x107 || SSID == 0x307))// 20150624 kys cv charge curr & cv discharge curr
#else
    if  (SSID == CAN_ID + 0x07)
#endif
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];

        // charge curr
        m_nCVCharge_Curr =
                (((Uint32) rx_can_frame.ext_Curr_bit.ext_Charge_Curr_H) << 24)
                        | ((Uint32) rx_can_frame.ext_Curr_bit.ext_Charge_Curr_M_1
                                << 16)	//Volt(v)
                        | (rx_can_frame.ext_Curr_bit.ext_Charge_Curr_M_2 << 8)
                        | rx_can_frame.ext_Curr_bit.ext_Charge_Curr_L;
        m_fltCVCharge_Curr = (float) m_nCVCharge_Curr * 0.001;
        // discharge curr
        m_nCVDisCharge_Curr =
                (((Uint32) rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_H) << 24)
                        | ((Uint32) rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_M_1
                                << 16)	//Curr(c)
                        | (rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_M_2 << 8)
                        | rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_L;
        m_fltCVDisCharge_Curr = (float) m_nCVDisCharge_Curr * 0.001;

        if (m_RevBufflag_107 == 0)
        {
            m_stPatBuff_CH1_1[m_nBufCnt_107[0]].dChargeCurr =
                    m_fltCVCharge_Curr;
            m_stPatBuff_CH1_1[m_nBufCnt_107[0]].dDisChargeCurr =
                    m_fltCVDisCharge_Curr;
            m_nBufCnt_107[0]++;
            if (m_nBufCnt_107[0] == 50)
            {
                m_nBufCnt_107[0] = 0;
                m_RevBufflag_107 = 1;
            }
        }
        else if (m_RevBufflag_107 == 1)
        {
            m_stPatBuff_CH1_2[m_nBufCnt_107[1]].dChargeCurr =
                    m_fltCVCharge_Curr;
            m_stPatBuff_CH1_2[m_nBufCnt_107[1]].dDisChargeCurr =
                    m_fltCVDisCharge_Curr;

            m_nBufCnt_107[1]++;
            if (m_nBufCnt_107[1] == 50)
            {
                m_nBufCnt_107[1] = 0;
                m_RevBufflag_107 = 0;
            }
        }
        m_nCVCharge_Curr = 0;
        m_nCVDisCharge_Curr = 0;
    }
#endif
#ifdef DSP_CH2
    if((SSID == 0x205 || SSID == 0x305))	//2015.4.30 Parallel add
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];

        OPCODE = (rx_can_frame.bit.OPCODE_PARACODE >> 4) & 0xf;

        if(OPCODE == 0x1) nSCI_CCCV_CP_CR_flag = 1;   //CC/CV
        else if(OPCODE == 0x2) nSCI_CCCV_CP_CR_flag = 2;//CP
        else if(OPCODE == 0x3) nSCI_CCCV_CP_CR_flag = 3;
        else if(OPCODE == 0x4) nSCI_CCCV_CP_CR_flag = 4;
        else if(OPCODE == 0x5) nSCI_CCCV_CP_CR_flag = 5;
        else if(OPCODE == 0x6) nSCI_CCCV_CP_CR_flag = 6;
        else if(OPCODE == 0x7) nSCI_CCCV_CP_CR_flag = 7;
        else if(OPCODE == 0x8) nSCI_CCCV_CP_CR_flag = 8;
        else if(OPCODE == 0x9) nSCI_CCCV_CP_CR_flag = 9;
        else nSCI_CCCV_CP_CR_flag = 0;

        PARALLELCODE = rx_can_frame.bit.OPCODE_PARACODE & 0xf;//20150427 Parallel add

        if(PARALLELCODE == 0x0) nSCI_Parallel_flag = 0;//PARALLEL OFF & SINGLE MODE ON 확인필요
        else if(PARALLELCODE == 0x1) nSCI_Parallel_flag = 1;//PARALLEL ON  & SINGLE MODE OFF

        tmp_volt = (((Uint32)rx_can_frame.bit.pack_vol_H) << 16) | (rx_can_frame.bit.pack_vol_M << 8 )//Volt(v)
        | rx_can_frame.bit.pack_vol_L;
        m_fltVolt = (float)tmp_volt * 0.001;

        tmp_curr = (((Uint32)rx_can_frame.bit.pack_cur_H) << 16) | (rx_can_frame.bit.pack_cur_M << 8 )//Curr(c)
        | rx_can_frame.bit.pack_cur_L;
        m_fltCurr = (float)tmp_curr * 0.001;

        if(nSCI_CCCV_CP_CR_flag == 7 || nSCI_CCCV_CP_CR_flag == 8)//Power(w)
        {
            nCP_confirm_flag = 1;
            m_fltCp = tmp_curr;
        }
        else
        nCP_confirm_flag = 0;

//		if((SCI_CCCV_CP_CR_flag == 2) && (CP_confirm_flag == 1))										//2015.05.04 확인필요
//		{
//			Ibat_over = SCI_Current_ref + 10. ;
//			if(Ibat_over >=110) Ibat_over = 110;
//		}

        if(m_RevBufflag_205 == 0)
        {
            if(m_nBufCnt_205[0] == 0)
            {
//				memset(m_stPatBuff_CH2_1,0,sizeof(m_stPatBuff_CH2_1));
                int k;
                for(k = 0; k < 50; k++)
                {
                    m_stPatBuff_CH2_1[k].szCmd = 0;
                    m_stPatBuff_CH2_1[k].szOp = 0;
                    m_stPatBuff_CH2_1[k].nOpFlag = 0;
                    m_stPatBuff_CH2_1[k].nCpFlag= 0;
                    m_stPatBuff_CH2_1[k].szPara = 0;
                    m_stPatBuff_CH2_1[k].nParaFlag= 0;
                    m_stPatBuff_CH2_1[k].nTime = 0;
                    m_stPatBuff_CH2_1[k].dVolt = 0;
                    m_stPatBuff_CH2_1[k].dCurr = 0;
                    m_stPatBuff_CH2_1[k].dPow = 0;
                }
            }
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].szCmd = rx_can_frame.bit.CMD;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].szOp = OPCODE;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].nOpFlag = nSCI_CCCV_CP_CR_flag;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].nCpFlag = nCP_confirm_flag;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].szPara = PARALLELCODE;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].nParaFlag = nSCI_Parallel_flag;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].dVolt = m_fltVolt;
            m_stPatBuff_CH2_1[m_nBufCnt_205[0]].dCurr = m_fltCurr;
            m_nBufCnt_205[0]++;
            if(m_nBufCnt_205[0] == 50)
            {
                m_nBufCnt_205[0] = 0;
                m_RevBufflag_205 = 1;
            }
        }
        else if(m_RevBufflag_205 == 1)
        {
            if(m_nBufCnt_205[1] == 0)
            {
//				memset(m_stPatBuff_CH2_2,0,sizeof(m_stPatBuff_CH2_2));
                int k;
                for(k = 0; k < 50; k++)
                {
                    m_stPatBuff_CH2_2[k].szCmd = 0;
                    m_stPatBuff_CH2_2[k].szOp = 0;
                    m_stPatBuff_CH2_2[k].nOpFlag = 0;
                    m_stPatBuff_CH2_2[k].nCpFlag= 0;
                    m_stPatBuff_CH2_2[k].szPara = 0;
                    m_stPatBuff_CH2_2[k].nParaFlag= 0;
                    m_stPatBuff_CH2_2[k].nTime = 0;
                    m_stPatBuff_CH2_2[k].dVolt = 0;
                    m_stPatBuff_CH2_2[k].dCurr = 0;
                    m_stPatBuff_CH2_2[k].dPow = 0;
                }
            }
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].szCmd = rx_can_frame.bit.CMD;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].szOp = OPCODE;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].nOpFlag = nSCI_CCCV_CP_CR_flag;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].nCpFlag = nCP_confirm_flag;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].szPara = PARALLELCODE;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].nParaFlag = nSCI_Parallel_flag;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].dVolt = m_fltVolt;
            m_stPatBuff_CH2_2[m_nBufCnt_205[1]].dCurr = m_fltCurr;
            m_nBufCnt_205[1]++;
            if(m_nBufCnt_205[1] == 50)
            {
                m_nBufCnt_205[1] = 0;
                m_RevBufflag_205 = 0;
            }
        }
        tmp_volt = 0;
        tmp_curr = 0;
//		CAN_all_receive = 1 ; //150526
    }

    if((SSID == 0x206 || SSID == 0x306))	//2015.4.30 Parallel add
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];
        //cp
        tmp_cp = (((Uint32)rx_can_frame.ext_bit.ext_pow_H) << 24) | ((Uint32)(rx_can_frame.ext_bit.ext_pow_M_1) << 16)
        | (rx_can_frame.ext_bit.ext_pow_M_2 << 8 ) | rx_can_frame.ext_bit.ext_pow_L;

        m_fltCp = (float)tmp_cp * 0.001;

        //time
        tmp_time = (((Uint32)rx_can_frame.ext_bit.ext_time_H) << 24) | ((Uint32)(rx_can_frame.ext_bit.ext_time_M_1) << 16)
        | (rx_can_frame.ext_bit.ext_time_M_2 << 8 ) | rx_can_frame.ext_bit.ext_time_L;

        if((tmp_time % 10) != 0)//20150527 만약 시간이 100일때 99 이런식으로 저장될때 1을 더해준다.
        tmp_time += 1;

        if(m_RevBufflag_206 == 0)
        {

            m_stPatBuff_CH2_1[m_nBufCnt_206[0]].dPow = m_fltCp;
            m_stPatBuff_CH2_1[m_nBufCnt_206[0]].nTime = tmp_time;
            m_nBufCnt_206[0]++;
            if(m_nBufCnt_206[0] == 50)
            {
                m_nBufCnt_206[0] = 0;
                m_RevBufflag_206 = 1;
            }
        }
        else if(m_RevBufflag_206 == 1)
        {
            m_stPatBuff_CH2_2[m_nBufCnt_206[1]].dPow = m_fltCp;
            m_stPatBuff_CH2_2[m_nBufCnt_206[1]].nTime = tmp_time;

            m_nBufCnt_206[1]++;
            if(m_nBufCnt_206[1] == 50 )
            {
                m_nBufCnt_206[1] = 0;
                m_RevBufflag_206 = 0;
            }
        }
        tmp_volt = 0;
        tmp_curr = 0;
//		CAN_all_receive = 1 ;
    }

    if((SSID == 0x207 || SSID == 0x307))// 20150624 kys cv charge curr & cv discharge curr
    {
        rx_can_frame.frame[0] = uc_rx_mbox3_data[0];
        rx_can_frame.frame[1] = uc_rx_mbox3_data[1];
        rx_can_frame.frame[2] = uc_rx_mbox3_data[2];
        rx_can_frame.frame[3] = uc_rx_mbox3_data[3];
        rx_can_frame.frame[4] = uc_rx_mbox3_data[4];
        rx_can_frame.frame[5] = uc_rx_mbox3_data[5];
        rx_can_frame.frame[6] = uc_rx_mbox3_data[6];
        rx_can_frame.frame[7] = uc_rx_mbox3_data[7];

        // charge curr
        m_nCVCharge_Curr = (((Uint32)rx_can_frame.ext_Curr_bit.ext_Charge_Curr_H) << 24) | ((Uint32)rx_can_frame.ext_Curr_bit.ext_Charge_Curr_M_1 << 16 )//Volt(v)
        | (rx_can_frame.ext_Curr_bit.ext_Charge_Curr_M_2 << 8 ) | rx_can_frame.ext_Curr_bit.ext_Charge_Curr_L;
        m_fltCVCharge_Curr = (float)m_nCVCharge_Curr * 0.001;
        // discharge curr
        m_nCVDisCharge_Curr = (((Uint32)rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_H) << 24) | ((Uint32)rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_M_1 << 16 )//Curr(c)
        | (rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_M_2 << 8 ) | rx_can_frame.ext_Curr_bit.ext_DisCharge_Curr_L;
        m_fltCVDisCharge_Curr = (float)m_nCVDisCharge_Curr * 0.001;

        if(m_RevBufflag_207 == 0)
        {
            m_stPatBuff_CH2_1[m_nBufCnt_207[0]].dChargeCurr = m_fltCVCharge_Curr;
            m_stPatBuff_CH2_1[m_nBufCnt_207[0]].dDisChargeCurr = m_fltCVDisCharge_Curr;
            m_nBufCnt_207[0]++;
            if(m_nBufCnt_207[0] == 50)
            {
                m_nBufCnt_207[0] = 0;
                m_RevBufflag_207 = 1;
            }
        }
        else if(m_RevBufflag_207 == 1)
        {
            m_stPatBuff_CH2_2[m_nBufCnt_207[1]].dChargeCurr = m_fltCVCharge_Curr;
            m_stPatBuff_CH2_2[m_nBufCnt_207[1]].dDisChargeCurr = m_fltCVDisCharge_Curr;

            m_nBufCnt_207[1]++;
            if(m_nBufCnt_207[1] == 50 )
            {
                m_nBufCnt_207[1] = 0;
                m_RevBufflag_207 = 0;
            }
        }
        m_nCVCharge_Curr = 0;
        m_nCVDisCharge_Curr = 0;
    }
#endif
// CAN 메일박스에 데이터가 수신되면 해당 CANRMP 레지스터 비트를 클리어.

    SSID = 0;

    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);
//    CANIntClear(CANA_BASE, CANA_BASE);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void ReadBuffData_CH1(void)	//CH1
{
//	memset(&m_stPatData_CH1,0,sizeof(&m_stPatData_CH1));
    m_stPatData_CH1.szCmd = 0;
    m_stPatData_CH1.szOp = 0;
    m_stPatData_CH1.nOpFlag = 0;
    m_stPatData_CH1.nCpFlag = 0;
    m_stPatData_CH1.szPara = 0;
    m_stPatData_CH1.nParaFlag = 0;
    m_stPatData_CH1.nTime = 0;
    m_stPatData_CH1.dVolt = 0;
    m_stPatData_CH1.dCurr = 0;
    m_stPatData_CH1.dPow = 0;
    m_stPatData_CH1.dChargeCurr = 0;
    m_stPatData_CH1.dDisChargeCurr = 0;

    if (m_SendBufflag_CH1 == 0)
    {

        m_stPatData_CH1.szCmd = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].szCmd;
        m_stPatData_CH1.szOp = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].szOp;
        m_stPatData_CH1.nOpFlag =
                m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].nOpFlag;
        m_stPatData_CH1.nCpFlag =
                m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].nCpFlag;
        m_stPatData_CH1.szPara = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].szPara;
        m_stPatData_CH1.nParaFlag =
                m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].nParaFlag;
        m_stPatData_CH1.dVolt = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].dVolt;
        m_stPatData_CH1.dCurr = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].dCurr;
        m_stPatData_CH1.dPow = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].dPow;
        m_stPatData_CH1.nTime = m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].nTime;
        m_stPatData_CH1.dChargeCurr =
                m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].dChargeCurr;
        m_stPatData_CH1.dDisChargeCurr =
                m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].dDisChargeCurr;
        /*
         if(m_stPatData_CH1.nTime <= 0.)	//20150610 안전조건 , 만약 time =0 이 나오면 정지;
         {
         m_stPatBuff_CH1_1[m_nSendBufCnt_CH1[0]].nTime =0x86;
         m_stPatData_CH1.szCmd = 0x86;
         }
         */
        CMD = m_stPatData_CH1.szCmd;

        switch (CMD)
        {
        case 0x79:			// 20150624 kys cv charge curr & cv discharge curr
//				Rx_request_data = 79;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
//				Charge_flag    = 0;
//				Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;
        case 0x82:
            //				Rx_request_data = 82;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 1;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 1;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x83:
            //				Rx_request_data = 83;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 1;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 1;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x85:
            //				Rx_request_data = 85;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 1;
            SCI_Standby_flag = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x86:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x87:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
//				Pattern_Rest_flag = 1;
            break;

        default:
            //	tx_data.bit.CMD = 0x90;
            /*				SCI_Grid_ON_flag   = 0;
             SCI_Pack_ON_flag   = 0;
             Charge_flag    = 0;
             Discharge_flag = 0;
             SCI_Charge_flag    = 0;
             SCI_Discharge_flag = 0;
             SCI_Pack_OFF_flag  = 0;
             SCI_All_Stop_flag  = 0;
             SCI_Standby_flag   = 0;
             Standby_flag = 1;  //add
             SCI_Pattern_flag   = 0;
             SCI_Err_Clear_flag = 0;*/
            break;
        }

//		SCI_CCCV_CP_CR_flag  = m_stPatData_CH1.nOpFlag;
        CP_confirm_flag = m_stPatData_CH1.nCpFlag;
        SCI_Parallel_flag = m_stPatData_CH1.nParaFlag;
        SCI_CP_Power = m_stPatData_CH1.dPow;
        CP_confirm_flag = m_stPatData_CH1.nCpFlag;
        SCI_Operation_Time = m_stPatData_CH1.nTime;
        SCI_Voltage_ref = m_stPatData_CH1.dVolt;
//		SCI_Current_ref = m_stPatData_CH1.dCurr;

        if (m_stPatData_CH1.szCmd == 0x79)
        {	// 20150624 kys cv charge curr & cv discharge curr

            Pattern_CV_mode = 1;

            if (Dir_Charge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                {  //0.025  -> 0.05  -> 0.07  -> 0.12
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 1;
                    Dir_Discharge_flag = 0;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }
            else if (Dir_Discharge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                { //0.025  -> 0.05  -> 0.07  -> 0.1
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 1;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }

            if (((SCI_Voltage_ref >= Vpack_UI_rms) && (Dir_Discharge_flag == 0))
                    || (Dir_Charge_flag == 1))
            {	//SCI_Voltage_ref : 사용자 입력 전압 , Vpack_UI_rms : 현재 베터리 전압
                Charge_flag = 1;
                Discharge_flag = 0;
                Dir_Charge_flag = 1;
                Dir_Discharge_flag = 0;
                SCI_CCCV_CP_CR_flag = CC_CHARGE_OPCODE;
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);

                SCI_Current_ref = m_stPatData_CH1.dChargeCurr;
                if (m_fltCvTemp < 0.5)
                {  // 0.25
                   //	Iboost1_ctrl_ref =0;
                    R_100ms_flag = 1;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {  //0.025
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {  //0.015
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //	SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }

            if (((SCI_Voltage_ref < Vpack_UI_rms) && (Dir_Charge_flag == 0))
                    || (Dir_Discharge_flag == 1))
            {
                Charge_flag = 0;
                Discharge_flag = 1;
                Dir_Charge_flag = 0;
                Dir_Discharge_flag = 1;
                SCI_CCCV_CP_CR_flag = CC_DISCHARGE_OPCODE;

                m_fltCvTemp = fabsf(Vpack_UI_rms - SCI_Voltage_ref);

                SCI_Current_ref = m_stPatData_CH1.dDisChargeCurr;
                if (m_fltCvTemp < 0.5)
                {
                    R_100ms_flag = 1;
                    //	Ibuck1_ctrl_ref =0;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }
        }
        else
        {
            SCI_CCCV_CP_CR_flag = m_stPatData_CH1.nOpFlag;
            SCI_Current_ref = m_stPatData_CH1.dCurr;
            CV_pattern_CV_flag = 0;
            Pattern_CV_mode = 0;
        }

        m_nSendBufCnt_CH1[0]++;
        if (m_nSendBufCnt_CH1[0] == 50)
        {
            m_nSendBufCnt_CH1[0] = 0;
            m_SendBufflag_CH1 = 1;
        }
        if (m_stPatData_CH1.szCmd == 0x86)
        {
            m_RevBufflag_105 = 0;
            m_RevBufflag_106 = 0;
            m_RevBufflag_107 = 0;
            m_nBufCnt_105[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[0] = 0;		//pc->dsp  CV data count init
            m_nBufCnt_105[1] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[1] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[1] = 0;		//pc->dsp  CV data count init
            m_SendBufflag_CH1 = 0;
            m_chkExtCount_105 = 0;
            m_chkExtCount_106 = 0;
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;

            int k;
            for (k = 0; k < 50; k++)
            {
                m_stPatBuff_CH1_1[k].szCmd = 0;
                m_stPatBuff_CH1_1[k].szOp = 0;
                m_stPatBuff_CH1_1[k].nOpFlag = 0;
                m_stPatBuff_CH1_1[k].nCpFlag = 0;
                m_stPatBuff_CH1_1[k].szPara = 0;
                m_stPatBuff_CH1_1[k].nParaFlag = 0;
                m_stPatBuff_CH1_1[k].nTime = 0;
                m_stPatBuff_CH1_1[k].dVolt = 0;
                m_stPatBuff_CH1_1[k].dCurr = 0;
                m_stPatBuff_CH1_1[k].dPow = 0;
                m_stPatBuff_CH1_1[k].dChargeCurr = 0;
                m_stPatBuff_CH1_1[k].dDisChargeCurr = 0;

                m_stPatBuff_CH1_2[k].szCmd = 0;
                m_stPatBuff_CH1_2[k].szOp = 0;
                m_stPatBuff_CH1_2[k].nOpFlag = 0;
                m_stPatBuff_CH1_2[k].nCpFlag = 0;
                m_stPatBuff_CH1_2[k].szPara = 0;
                m_stPatBuff_CH1_2[k].nParaFlag = 0;
                m_stPatBuff_CH1_2[k].nTime = 0;
                m_stPatBuff_CH1_2[k].dVolt = 0;
                m_stPatBuff_CH1_2[k].dCurr = 0;
                m_stPatBuff_CH1_2[k].dPow = 0;
                m_stPatBuff_CH1_2[k].dChargeCurr = 0;
                m_stPatBuff_CH1_2[k].dDisChargeCurr = 0;
            }
        }
    }
    else if (m_SendBufflag_CH1 == 1)
    {
        m_stPatData_CH1.szCmd = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].szCmd;
        m_stPatData_CH1.szOp = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].szOp;
        m_stPatData_CH1.nOpFlag =
                m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].nOpFlag;
        m_stPatData_CH1.nCpFlag =
                m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].nCpFlag;
        m_stPatData_CH1.szPara = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].szPara;
        m_stPatData_CH1.nParaFlag =
                m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].nParaFlag;
        m_stPatData_CH1.dVolt = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].dVolt;
        m_stPatData_CH1.dCurr = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].dCurr;
        m_stPatData_CH1.dPow = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].dPow;
        m_stPatData_CH1.nTime = m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].nTime;
        m_stPatData_CH1.dChargeCurr =
                m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].dChargeCurr;
        m_stPatData_CH1.dDisChargeCurr =
                m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].dDisChargeCurr;
        /*
         if(m_stPatData_CH1.nTime <= 0.)
         {
         m_stPatBuff_CH1_2[m_nSendBufCnt_CH1[1]].nTime =0x86;
         m_stPatData_CH1.szCmd = 0x86;
         }
         */
        CMD = m_stPatData_CH1.szCmd;

        switch (CMD)
        {
        case 0x79:			// 20150624 kys cv charge curr & cv discharge curr
//				Rx_request_data = 79;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
//				Charge_flag    = 0;
//				Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;
        case 0x82:
            //				Rx_request_data = 82;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 1;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x83:
            //				Rx_request_data = 83;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 1;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x85:
            //				Rx_request_data = 85;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 1;
            SCI_Standby_flag = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x86:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
//				m_nStepCount_Buf++;

            break;
        case 0x87:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        default:
            //	tx_data.bit.CMD = 0x90;
            /*				SCI_Grid_ON_flag   = 0;
             SCI_Pack_ON_flag   = 0;
             Charge_flag    = 0;
             Discharge_flag = 0;
             SCI_Charge_flag    = 0;
             SCI_Discharge_flag = 0;
             SCI_Pack_OFF_flag  = 0;
             SCI_All_Stop_flag  = 0;
             SCI_Standby_flag   = 0;
             Standby_flag = 1;  //add
             SCI_Pattern_flag   = 0;
             SCI_Err_Clear_flag = 0;*/
            break;
        }

//		SCI_CCCV_CP_CR_flag  = m_stPatData_CH1.nOpFlag;
        CP_confirm_flag = m_stPatData_CH1.nCpFlag;
        SCI_Parallel_flag = m_stPatData_CH1.nParaFlag;
        SCI_CP_Power = m_stPatData_CH1.dPow;
        CP_confirm_flag = m_stPatData_CH1.nCpFlag;
        SCI_Operation_Time = m_stPatData_CH1.nTime;
        SCI_Voltage_ref = m_stPatData_CH1.dVolt;
//		SCI_Current_ref = m_stPatData_CH1.dCurr;

        if (m_stPatData_CH1.szCmd == 0x79)
        {	// 20150624 kys cv charge curr & cv discharge curr

            Pattern_CV_mode = 1;

            if (Dir_Charge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                {  //0.025  -> 0.05  -> 0.07  -> 0.12
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 1;
                    Dir_Discharge_flag = 0;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }
            else if (Dir_Discharge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                { //0.025  -> 0.05  -> 0.07  -> 0.1
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 1;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }

            if (((SCI_Voltage_ref >= Vpack_UI_rms) && (Dir_Discharge_flag == 0))
                    || (Dir_Charge_flag == 1))
            {	//SCI_Voltage_ref : 사용자 입력 전압 , Vpack_UI_rms : 현재 베터리 전압
                Charge_flag = 1;
                Discharge_flag = 0;
                Dir_Charge_flag = 1;
                Dir_Discharge_flag = 0;
                SCI_CCCV_CP_CR_flag = CC_CHARGE_OPCODE;
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);

                SCI_Current_ref = m_stPatData_CH1.dChargeCurr;
                if (m_fltCvTemp < 0.5)
                {  // 0.25
                   //	Iboost1_ctrl_ref =0;
                    R_100ms_flag = 1;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {  //0.025
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {  //0.015
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //	SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }

            if (((SCI_Voltage_ref < Vpack_UI_rms) && (Dir_Charge_flag == 0))
                    || (Dir_Discharge_flag == 1))
            {
                Charge_flag = 0;
                Discharge_flag = 1;
                Dir_Charge_flag = 0;
                Dir_Discharge_flag = 1;
                SCI_CCCV_CP_CR_flag = CC_DISCHARGE_OPCODE;

                m_fltCvTemp = fabsf(Vpack_UI_rms - SCI_Voltage_ref);

                SCI_Current_ref = m_stPatData_CH1.dDisChargeCurr;
                if (m_fltCvTemp < 0.5)
                {
                    R_100ms_flag = 1;
                    //	Ibuck1_ctrl_ref =0;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }
        }
        else
        {
            SCI_CCCV_CP_CR_flag = m_stPatData_CH1.nOpFlag;
            SCI_Current_ref = m_stPatData_CH1.dCurr;
            CV_pattern_CV_flag = 0;
            Pattern_CV_mode = 0;
        }
        m_nSendBufCnt_CH1[1]++;
        if (m_nSendBufCnt_CH1[1] == 50)
        {
            m_nSendBufCnt_CH1[1] = 0;
            m_SendBufflag_CH1 = 0;
            //dsp -> can data 100개 전송 요청
        }

        if (m_stPatData_CH1.szCmd == 0x86)
        {
            m_RevBufflag_105 = 0;
            m_RevBufflag_106 = 0;
            m_RevBufflag_107 = 0;
            m_nBufCnt_105[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_105[1] = 0;		//pc->dsp  value data count init
            m_nBufCnt_106[1] = 0;		//pc->dsp  time data count init
            m_nBufCnt_107[1] = 0;		//pc->dsp  time data count init
            m_SendBufflag_CH1 = 0;
//			Pattern_cnt = 0;
            m_chkExtCount_105 = 0;
            m_chkExtCount_106 = 0;
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;

            int k;
            for (k = 0; k < 50; k++)
            {
                m_stPatBuff_CH1_1[k].szCmd = 0;
                m_stPatBuff_CH1_1[k].szOp = 0;
                m_stPatBuff_CH1_1[k].nOpFlag = 0;
                m_stPatBuff_CH1_1[k].nCpFlag = 0;
                m_stPatBuff_CH1_1[k].szPara = 0;
                m_stPatBuff_CH1_1[k].nParaFlag = 0;
                m_stPatBuff_CH1_1[k].nTime = 0;
                m_stPatBuff_CH1_1[k].dVolt = 0;
                m_stPatBuff_CH1_1[k].dCurr = 0;
                m_stPatBuff_CH1_1[k].dPow = 0;
                m_stPatBuff_CH1_1[k].dChargeCurr = 0;
                m_stPatBuff_CH1_1[k].dDisChargeCurr = 0;

                m_stPatBuff_CH1_2[k].szCmd = 0;
                m_stPatBuff_CH1_2[k].szOp = 0;
                m_stPatBuff_CH1_2[k].nOpFlag = 0;
                m_stPatBuff_CH1_2[k].nCpFlag = 0;
                m_stPatBuff_CH1_2[k].szPara = 0;
                m_stPatBuff_CH1_2[k].nParaFlag = 0;
                m_stPatBuff_CH1_2[k].nTime = 0;
                m_stPatBuff_CH1_2[k].dVolt = 0;
                m_stPatBuff_CH1_2[k].dCurr = 0;
                m_stPatBuff_CH1_2[k].dPow = 0;
                m_stPatBuff_CH1_2[k].dChargeCurr = 0;
                m_stPatBuff_CH1_2[k].dDisChargeCurr = 0;
            }
        }
    }
    if (m_stPatData_CH1.szCmd != 0x00)
    {
        m_nStepCount_CH1++;
        if (m_stPatData_CH1.szCmd == 0x86)
        {
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;
        }
    }

}

void ReadBuffData_CH2(void)	//CH2
{
//	memset(&m_stPatData_CH2,0,sizeof(&m_stPatData_CH2));

    m_stPatData_CH2.szCmd = 0;
    m_stPatData_CH2.szOp = 0;
    m_stPatData_CH2.nOpFlag = 0;
    m_stPatData_CH2.nCpFlag = 0;
    m_stPatData_CH2.szPara = 0;
    m_stPatData_CH2.nParaFlag = 0;
    m_stPatData_CH2.nTime = 0;
    m_stPatData_CH2.dVolt = 0;
    m_stPatData_CH2.dCurr = 0;
    m_stPatData_CH2.dPow = 0;
    m_stPatData_CH2.dChargeCurr = 0;
    m_stPatData_CH2.dDisChargeCurr = 0;

    if (m_SendBufflag_CH2 == 0)
    {
        m_stPatData_CH2.szCmd = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].szCmd;
        m_stPatData_CH2.szOp = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].szOp;
        m_stPatData_CH2.nOpFlag =
                m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].nOpFlag;
        m_stPatData_CH2.nCpFlag =
                m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].nCpFlag;
        m_stPatData_CH2.szPara = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].szPara;
        m_stPatData_CH2.nParaFlag =
                m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].nParaFlag;
        m_stPatData_CH2.dVolt = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].dVolt;
        m_stPatData_CH2.dCurr = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].dCurr;
        m_stPatData_CH2.dPow = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].dPow;
        m_stPatData_CH2.nTime = m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].nTime;
        m_stPatData_CH2.dChargeCurr =
                m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].dChargeCurr;
        m_stPatData_CH2.dDisChargeCurr =
                m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].dDisChargeCurr;
        /*
         if(m_stPatData_CH2.nTime <= 0.)	//20150610 안전조건 , 만약 time =0 이 나오면 정지;
         {
         m_stPatBuff_CH2_1[m_nSendBufCnt_CH2[0]].nTime =0x86;
         m_stPatData_CH2.szCmd = 0x86;
         }
         */
        CMD = m_stPatData_CH2.szCmd;

        switch (CMD)
        {
        case 0x79:			// 20150624 kys cv charge curr & cv discharge curr
//				Rx_request_data = 79;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
//				Charge_flag    = 0;
//				Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;
        case 0x82:
            //				Rx_request_data = 82;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 1;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 1;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x83:
            //				Rx_request_data = 83;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 1;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 1;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x85:
            //				Rx_request_data = 85;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 1;
            SCI_Standby_flag = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x86:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x87:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
//				Pattern_Rest_flag = 1;
            break;

        default:
            //	tx_data.bit.CMD = 0x90;
            /*				SCI_Grid_ON_flag   = 0;
             SCI_Pack_ON_flag   = 0;
             Charge_flag    = 0;
             Discharge_flag = 0;
             SCI_Charge_flag    = 0;
             SCI_Discharge_flag = 0;
             SCI_Pack_OFF_flag  = 0;
             SCI_All_Stop_flag  = 0;
             SCI_Standby_flag   = 0;
             Standby_flag = 1;  //add
             SCI_Pattern_flag   = 0;
             SCI_Err_Clear_flag = 0;*/
            break;
        }

//		SCI_CCCV_CP_CR_flag  = m_stPatData_CH2.nOpFlag;
        CP_confirm_flag = m_stPatData_CH2.nCpFlag;
        SCI_Parallel_flag = m_stPatData_CH2.nParaFlag;
//		Ibat_over = m_stPatData_CH2.dIbat_over;
        SCI_CP_Power = m_stPatData_CH2.dPow;
        CP_confirm_flag = m_stPatData_CH2.nCpFlag;
        SCI_Operation_Time = m_stPatData_CH2.nTime;
        SCI_Voltage_ref = m_stPatData_CH2.dVolt;
//		SCI_Current_ref = m_stPatData_CH2.dCurr;

        if (m_stPatData_CH2.szCmd == 0x79)
        {	// 20150624 kys cv charge curr & cv discharge curr

            Pattern_CV_mode = 1;

            if (Dir_Charge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                {  //0.025  -> 0.05  -> 0.07  -> 0.12
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 1;
                    Dir_Discharge_flag = 0;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }
            else if (Dir_Discharge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp <= 0.12)
                { //0.025  -> 0.05  -> 0.07  -> 0.1
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 1;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }

            if (((SCI_Voltage_ref >= Vpack_UI_rms) && (Dir_Discharge_flag == 0))
                    || (Dir_Charge_flag == 1))
            {	//SCI_Voltage_ref : 사용자 입력 전압 , Vpack_UI_rms : 현재 베터리 전압
                Charge_flag = 1;
                Discharge_flag = 0;
                Dir_Charge_flag = 1;
                Dir_Discharge_flag = 0;
                SCI_CCCV_CP_CR_flag = CC_CHARGE_OPCODE;
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);

                SCI_Current_ref = m_stPatData_CH2.dChargeCurr;
                if (m_fltCvTemp < 0.5)
                {  // 0.25
                   //	Iboost1_ctrl_ref =0;
                    R_100ms_flag = 1;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {  //0.025
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {  //0.015
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //	SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }

            if (((SCI_Voltage_ref < Vpack_UI_rms) && (Dir_Charge_flag == 0))
                    || (Dir_Discharge_flag == 1))
            {
                Charge_flag = 0;
                Discharge_flag = 1;
                Dir_Charge_flag = 0;
                Dir_Discharge_flag = 1;
                SCI_CCCV_CP_CR_flag = CC_DISCHARGE_OPCODE;

                m_fltCvTemp = fabsf(Vpack_UI_rms - SCI_Voltage_ref);

                SCI_Current_ref = m_stPatData_CH2.dDisChargeCurr;
                if (m_fltCvTemp < 0.5)
                {
                    R_100ms_flag = 1;
                    //	Ibuck1_ctrl_ref =0;
                    CV_pattern_CV_flag = 0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }
        }
        else
        {
            SCI_CCCV_CP_CR_flag = m_stPatData_CH2.nOpFlag;
            SCI_Current_ref = m_stPatData_CH2.dCurr;
            CV_pattern_CV_flag = 0;
            Pattern_CV_mode = 0;
        }

        m_nSendBufCnt_CH2[0]++;
        if (m_nSendBufCnt_CH2[0] == 50)
        {
            m_nSendBufCnt_CH2[0] = 0;
            m_SendBufflag_CH2 = 1;
            //dsp -> can data 100개 전송 요청
        }

        if (m_stPatData_CH2.szCmd == 0x86)
        {
            m_RevBufflag_205 = 0;
            m_RevBufflag_206 = 0;
            m_RevBufflag_207 = 0;
            m_nBufCnt_205[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_206[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_207[0] = 0;		//pc->dsp  CV data count init
            m_nBufCnt_205[1] = 0;		//pc->dsp  value data count init
            m_nBufCnt_206[1] = 0;		//pc->dsp  time data count init
            m_nBufCnt_207[1] = 0;		//pc->dsp  CV data count init
            m_SendBufflag_CH2 = 0;
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;

            int k;
            for (k = 0; k < 50; k++)
            {
                m_stPatBuff_CH2_1[k].szCmd = 0;
                m_stPatBuff_CH2_1[k].szOp = 0;
                m_stPatBuff_CH2_1[k].nOpFlag = 0;
                m_stPatBuff_CH2_1[k].nCpFlag = 0;
                m_stPatBuff_CH2_1[k].szPara = 0;
                m_stPatBuff_CH2_1[k].nParaFlag = 0;
                m_stPatBuff_CH2_1[k].nTime = 0;
                m_stPatBuff_CH2_1[k].dVolt = 0;
                m_stPatBuff_CH2_1[k].dCurr = 0;
                m_stPatBuff_CH2_1[k].dPow = 0;
                m_stPatBuff_CH2_1[k].dChargeCurr = 0;
                m_stPatBuff_CH2_1[k].dDisChargeCurr = 0;

                m_stPatBuff_CH2_2[k].szCmd = 0;
                m_stPatBuff_CH2_2[k].szOp = 0;
                m_stPatBuff_CH2_2[k].nOpFlag = 0;
                m_stPatBuff_CH2_2[k].nCpFlag = 0;
                m_stPatBuff_CH2_2[k].szPara = 0;
                m_stPatBuff_CH2_2[k].nParaFlag = 0;
                m_stPatBuff_CH2_2[k].nTime = 0;
                m_stPatBuff_CH2_2[k].dVolt = 0;
                m_stPatBuff_CH2_2[k].dCurr = 0;
                m_stPatBuff_CH2_2[k].dPow = 0;
                m_stPatBuff_CH2_2[k].dChargeCurr = 0;
                m_stPatBuff_CH2_2[k].dDisChargeCurr = 0;
            }
        }

    }
    else if (m_SendBufflag_CH2 == 1)
    {
        m_stPatData_CH2.szCmd = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].szCmd;
        m_stPatData_CH2.szOp = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].szOp;
        m_stPatData_CH2.nOpFlag =
                m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].nOpFlag;
        m_stPatData_CH2.nCpFlag =
                m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].nCpFlag;
        m_stPatData_CH2.szPara = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].szPara;
        m_stPatData_CH2.nParaFlag =
                m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].nParaFlag;
        m_stPatData_CH2.dVolt = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].dVolt;
        m_stPatData_CH2.dCurr = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].dCurr;
        m_stPatData_CH2.dPow = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].dPow;
        m_stPatData_CH2.nTime = m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].nTime;
        m_stPatData_CH2.dChargeCurr =
                m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].dChargeCurr;
        m_stPatData_CH2.dDisChargeCurr =
                m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].dDisChargeCurr;
        /*
         if(m_stPatData_CH2.nTime <= 0.)	//20150610 안전조건 , 만약 time =0 이 나오면 정지;
         {
         m_stPatBuff_CH2_2[m_nSendBufCnt_CH2[1]].nTime =0x86;
         m_stPatData_CH2.szCmd = 0x86;
         }
         */
        CMD = m_stPatData_CH2.szCmd;

        switch (CMD)
        {
        case 0x79:			// 20150624 kys cv charge curr & cv discharge curr
//				Rx_request_data = 79;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
//				Charge_flag    = 0;
//				Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;
        case 0x82:
            //				Rx_request_data = 82;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 1;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x83:
            //				Rx_request_data = 83;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 1;
            SCI_Pack_OFF_flag = 0;
//	 			SCI_All_Stop_flag  = 0;
//				SCI_Standby_flag   = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x85:
            //				Rx_request_data = 85;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 1;
            SCI_Standby_flag = 0;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        case 0x86:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
            SCI_Pattern_flag = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
//				m_nStepCount_Buf++;

            break;
        case 0x87:
            //				Rx_request_data = 86;
            SCI_Grid_ON_flag = 0;
            SCI_Pack_ON_flag = 0;
            Charge_flag = 0;
            Discharge_flag = 0;
            SCI_Charge_flag = 0;
            SCI_Discharge_flag = 0;
            SCI_Pack_OFF_flag = 0;
            SCI_All_Stop_flag = 0;
            SCI_Standby_flag = 1;
//				SCI_Pattern_flag   = 0;
            SCI_Err_Clear_flag = 0;
            Pattern_Rest_flag = 0;
            break;

        default:
            //	tx_data.bit.CMD = 0x90;
            /*				SCI_Grid_ON_flag   = 0;
             SCI_Pack_ON_flag   = 0;
             Charge_flag    = 0;
             Discharge_flag = 0;
             SCI_Charge_flag    = 0;
             SCI_Discharge_flag = 0;
             SCI_Pack_OFF_flag  = 0;
             SCI_All_Stop_flag  = 0;
             SCI_Standby_flag   = 0;
             Standby_flag = 1;  //add
             SCI_Pattern_flag   = 0;
             SCI_Err_Clear_flag = 0;*/
            break;
        }

//		SCI_CCCV_CP_CR_flag  = m_stPatData_CH2.nOpFlag;
        CP_confirm_flag = m_stPatData_CH2.nCpFlag;
        SCI_Parallel_flag = m_stPatData_CH2.nParaFlag;
        SCI_CP_Power = m_stPatData_CH2.dPow;
        CP_confirm_flag = m_stPatData_CH2.nCpFlag;
        SCI_Operation_Time = m_stPatData_CH2.nTime;
        SCI_Voltage_ref = m_stPatData_CH2.dVolt;
//		SCI_Current_ref = m_stPatData_CH2.dCurr;

        if (m_stPatData_CH2.szCmd == 0x79)
        {	// 20150624 kys cv charge curr & cv discharge curr

            Pattern_CV_mode = 1;

            if (Dir_Charge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp < 0.12)
                {  //0.025  -> 0.05  -> 0.07  -> 0.12
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 1;
                    Dir_Discharge_flag = 0;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }
            else if (Dir_Discharge_flag == 1)
            {
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);
                if (m_fltCvTemp < 0.12)
                {  //0.025  -> 0.05  -> 0.07
                    R_100ms_flag = 1;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 1;
                }
                else
                {
                    R_100ms_flag = 0;
                    Dir_Charge_flag = 0;
                    Dir_Discharge_flag = 0;
                }
            }

            if (((SCI_Voltage_ref >= Vpack_UI_rms) && (Dir_Discharge_flag == 0))
                    || (Dir_Charge_flag == 1))
            {	//SCI_Voltage_ref : 사용자 입력 전압 , Vpack_UI_rms : 현재 베터리 전압
                Charge_flag = 1;
                Discharge_flag = 0;
                Dir_Charge_flag = 1;
                Dir_Discharge_flag = 0;
                SCI_CCCV_CP_CR_flag = CC_CHARGE_OPCODE;
                m_fltCvTemp = fabsf(SCI_Voltage_ref - Vpack_UI_rms);

                SCI_Current_ref = m_stPatData_CH2.dChargeCurr;
                if (m_fltCvTemp < 0.5)
                {  // 0.25){
                   //	Iboost1_ctrl_ref =0;
                    R_100ms_flag = 1;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    CV_pattern_CV_flag = 0;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //	SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }

            if (((SCI_Voltage_ref < Vpack_UI_rms) && (Dir_Charge_flag == 0))
                    || (Dir_Discharge_flag == 1))
            {
                Charge_flag = 0;
                Discharge_flag = 1;
                Dir_Charge_flag = 0;
                Dir_Discharge_flag = 1;
                SCI_CCCV_CP_CR_flag = CC_DISCHARGE_OPCODE;

                m_fltCvTemp = fabsf(Vpack_UI_rms - SCI_Voltage_ref);

                SCI_Current_ref = m_stPatData_CH2.dDisChargeCurr;
                if (m_fltCvTemp < 0.5)
                {  //0.25
                    R_100ms_flag = 1;
                    CV_pattern_CV_flag = 0;
                    //	Ibuck1_ctrl_ref =0;
                    Rising_time = 0.0001;  //Rising_time = 0.0001;
                    if (m_fltCvTemp < 0.1)
                    {
                        Rising_time = 0.00005;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.035)
                    {
                        Rising_time = 0.00002;
                        CV_pattern_CV_flag = 1;
                    }
                    if (m_fltCvTemp < 0.025)
                    {
                        Rising_time = 0.00001;
                        CV_pattern_CV_flag = 1;
                        //SCI_Current_ref = 10.; //SCI_Current_ref * 0.5;
                    }
                }
                else
                {
                    //	Iboost1_ctrl_ref =0;
                    //	Ibuck1_ctrl_ref =0;
                    R_100ms_flag = 0;
                    Rising_time = 0.001;
                    CV_pattern_CV_flag = 0;
                }
            }
        }
        else
        {
            SCI_CCCV_CP_CR_flag = m_stPatData_CH2.nOpFlag;
            SCI_Current_ref = m_stPatData_CH2.dCurr;
            CV_pattern_CV_flag = 0;
            Pattern_CV_mode = 0;
        }

        m_nSendBufCnt_CH2[1]++;
        if (m_nSendBufCnt_CH2[1] == 50)
        {
            m_nSendBufCnt_CH2[1] = 0;
            m_SendBufflag_CH2 = 0;
            //dsp -> can data 100개 전송 요청
        }
        if (m_stPatData_CH2.szCmd == 0x86)
        {
            m_RevBufflag_205 = 0;
            m_RevBufflag_206 = 0;
            m_RevBufflag_207 = 0;
            m_nBufCnt_205[0] = 0;		//pc->dsp  value data count init
            m_nBufCnt_206[0] = 0;		//pc->dsp  time data count init
            m_nBufCnt_207[0] = 0;		//pc->dsp  CV data count init
            m_nBufCnt_205[1] = 0;		//pc->dsp  value data count init
            m_nBufCnt_206[1] = 0;		//pc->dsp  time data count init
            m_nBufCnt_207[1] = 0;		//pc->dsp  CV data count init
            m_SendBufflag_CH2 = 0;
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;

            int k;
            for (k = 0; k < 50; k++)
            {
                m_stPatBuff_CH2_1[k].szCmd = 0;
                m_stPatBuff_CH2_1[k].szOp = 0;
                m_stPatBuff_CH2_1[k].nOpFlag = 0;
                m_stPatBuff_CH2_1[k].nCpFlag = 0;
                m_stPatBuff_CH2_1[k].szPara = 0;
                m_stPatBuff_CH2_1[k].nParaFlag = 0;
                m_stPatBuff_CH2_1[k].nTime = 0;
                m_stPatBuff_CH2_1[k].dVolt = 0;
                m_stPatBuff_CH2_1[k].dCurr = 0;
                m_stPatBuff_CH2_1[k].dPow = 0;
                m_stPatBuff_CH2_1[k].dChargeCurr = 0;
                m_stPatBuff_CH2_1[k].dDisChargeCurr = 0;

                m_stPatBuff_CH2_2[k].szCmd = 0;
                m_stPatBuff_CH2_2[k].szOp = 0;
                m_stPatBuff_CH2_2[k].nOpFlag = 0;
                m_stPatBuff_CH2_2[k].nCpFlag = 0;
                m_stPatBuff_CH2_2[k].szPara = 0;
                m_stPatBuff_CH2_2[k].nParaFlag = 0;
                m_stPatBuff_CH2_2[k].nTime = 0;
                m_stPatBuff_CH2_2[k].dVolt = 0;
                m_stPatBuff_CH2_2[k].dCurr = 0;
                m_stPatBuff_CH2_2[k].dPow = 0;
                m_stPatBuff_CH2_2[k].dChargeCurr = 0;
                m_stPatBuff_CH2_2[k].dDisChargeCurr = 0;
            }
        }
    }
    if (m_stPatData_CH2.szCmd != 0x00)
    {
        m_nStepCount_CH2++;
        if (m_stPatData_CH2.szCmd == 0x86)
        {
            m_nPatFlag = 1;
            m_nPatEndFlag = 1;
        }
    }
//
}

void CANA_GPIO_INIT()
{
    EALLOW;
    DevCfgRegs.CPUSEL8.bit.CAN_A = 0;				// select CPU1(0) or CPU2(1)

    GPIO_SetupPinMux(62, GPIO_MUX_CPU1, 6); 				// GPIO30 - CANRXA
    GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 6); 				// GPIO31 - CANTXA
    GPIO_SetupPinOptions(62, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinOptions(63, GPIO_OUTPUT, GPIO_PUSHPULL);

    CANInit(CANA_BASE);
    CANClkSourceSelect(CANA_BASE, 0);
    CANBitRateSet(CANA_BASE, 200000000, 1000000);
    CANIntEnable(CANA_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    //	CANIntEnable(CANA_BASE, CAN_INT_STATUS);
    // TODO Add code to allow configuration of GPADIR from CPU02 using IPC
    EDIS;
}

void CANA_INIT_INIT()
{
    EALLOW;
    PieVectTable.CANA0_INT = cana_rx_isr;
    EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
    IER |= 0x0100;   // M_INT9
    EINT;
}

//==================================================================================== Example Code.

void CAN_A_Mailbox_Init()
{
    sTXCANMessage.ui32MsgID = 0x110;                   // CAN message ID - use 1
    sTXCANMessage.ui32MsgIDMask = 0;                  // no mask needed for TX
    sTXCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // size of message is 8
    sTXCANMessage.pucMsgData = ucTXMsgData;           // ptr to message content

    sTXCANMessage2.ui32MsgID = 0x120;                  // CAN message ID - use 1
    sTXCANMessage2.ui32MsgIDMask = 0;                  // no mask needed for TX
    sTXCANMessage2.ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    sTXCANMessage2.ui32MsgLen = sizeof(ucTXMsgData2);   // size of message is 8
    sTXCANMessage2.pucMsgData = ucTXMsgData2;          // ptr to message content

    *(unsigned long *) ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = 0x200;                   // CAN message ID - use 1
    sRXCANMessage.ui32MsgIDMask = 0;                  // no mask needed for TX
    sRXCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE;  // enable interrupt on RX
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   // size of message is 8
    sRXCANMessage.pucMsgData = ucRXMsgData;           // ptr to message content

    *(unsigned long *) ucRXMsgData_KJ = 0;
    sRXCANMessage_KJ.ui32MsgID = 0x300;                // CAN message ID - use 1
    sRXCANMessage_KJ.ui32MsgIDMask = 0;                 // no mask needed for TX
    sRXCANMessage_KJ.ui32Flags = MSG_OBJ_RX_INT_ENABLE; // enable interrupt on RX
    sRXCANMessage_KJ.ui32MsgLen = sizeof(ucRXMsgData_KJ); // size of message is 8
    sRXCANMessage_KJ.pucMsgData = ucRXMsgData_KJ;      // ptr to message content

    CANMessageSet(CANA_BASE, 3, &sRXCANMessage, MSG_OBJ_TYPE_RX);
    CANMessageSet(CANA_BASE, 4, &sRXCANMessage_KJ, MSG_OBJ_TYPE_RX);
}

void CANHandler(void)
{
    unsigned long ulStatus;

    ulStatus = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    if (ulStatus == CAN_INT_INT0ID_STATUS)
    {
        ulStatus = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);

        if (((ulStatus & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 7)
                && ((ulStatus & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 0))
        {
            g_bErrFlag = 1;
        }
    }

    else if (ulStatus == 1)
    {
        CANIntClear(CANA_BASE, 1);
        g_ulTxMsgCount++;
        g_bErrFlag = 0;

        SSID = 0;
    }
    else if (ulStatus == 2)
    {
        CANIntClear(CANA_BASE, 2);
        g_ulTxMsgCount++;
        g_bErrFlag = 0;

        SSID = 0;
    }
    else if (ulStatus == 3)
    {
        CANMessageGet(CANA_BASE, 3, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 3);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX3.ui32MsgID;
    }
    else if (ulStatus == 4)
    {
        CANMessageGet(CANA_BASE, 4, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 4);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX4.ui32MsgID;
    }
    else if (ulStatus == 5)
    {
        CANMessageGet(CANA_BASE, 5, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 5);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX5.ui32MsgID;
    }
    else if (ulStatus == 6)
    {
        CANMessageGet(CANA_BASE, 6, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 6);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX6.ui32MsgID;
    }
    else if (ulStatus == 7)
    {
        CANMessageGet(CANA_BASE, 7, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 7);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX7.ui32MsgID;
    }
    else if (ulStatus == 8)
    {
        CANMessageGet(CANA_BASE, 8, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 8);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX8.ui32MsgID;
    }
    else if (ulStatus == 9)
    {
        CANMessageGet(CANA_BASE, 9, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 9);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX9.ui32MsgID;
    }
    else if (ulStatus == 10)
    {
        CANMessageGet(CANA_BASE, 10, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 10);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX10.ui32MsgID;
    }
    else if (ulStatus == 11)
    {
        CANMessageGet(CANA_BASE, 11, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 11);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX11.ui32MsgID;
    }
    else if (ulStatus == 12)
    {
        CANMessageGet(CANA_BASE, 12, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 12);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX12.ui32MsgID;
    }
    else if (ulStatus == 13)
    {
        CANMessageGet(CANA_BASE, 13, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 13);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX13.ui32MsgID;
    }
    else if (ulStatus == 14)
    {
        CANMessageGet(CANA_BASE, 14, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 14);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX14.ui32MsgID;
    }
    else if (ulStatus == 15)
    {
        CANMessageGet(CANA_BASE, 15, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 15);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX15.ui32MsgID;
    }
    else if (ulStatus == 16)
    {
        CANMessageGet(CANA_BASE, 16, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 16);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX16.ui32MsgID;
    }
    else if (ulStatus == 17)
    {
        CANMessageGet(CANA_BASE, 17, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 17);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX17.ui32MsgID;
    }
    else if (ulStatus == 18)
    {
        CANMessageGet(CANA_BASE, 18, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 18);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX18.ui32MsgID;
    }
    else if (ulStatus == 19)
    {
        CANMessageGet(CANA_BASE, 19, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 19);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX19.ui32MsgID;
    }

//  ///////
    else if (ulStatus == 20)          // add 181108
    {
        CANMessageGet(CANA_BASE, 20, &s_RX_MBOX3, true);
        CANIntClear(CANA_BASE, 20);
        g_ulRxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX20.ui32MsgID;
    }
//  ///////////

    else if (ulStatus == 21)
    {
        CANMessageGet(CANA_BASE, 21, &s_RX_MBOX3, true);

        CANIntClear(CANA_BASE, 21);
        g_ulTxMsgCount++;
        g_bErrFlag = 0;

        SSID = s_RX_MBOX21.ui32MsgID;
    }
    else
    {

    }
}

//interrupt void
//CANIntHandler(void)
//{
//    unsigned long ulStatus;
//
//    ulStatus = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);
//
//    if(ulStatus == CAN_INT_INT0ID_STATUS)
//    {
//        ulStatus = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);
//
//        if(((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 7) &&
//           ((ulStatus  & ~(CAN_ES_TXOK | CAN_ES_RXOK)) != 0))
//        {
//            g_bErrFlag = 1;
//        }
//    }
//
//    else if(ulStatus == 1)
//    {
//        CANIntClear(CANA_BASE, 1);
//        g_ulTxMsgCount++;
//        g_bErrFlag = 0;
//    }
//    else if(ulStatus == 2)
//    {
//        CANIntClear(CANA_BASE, 2);
//        g_ulTxMsgCount++;
//        g_bErrFlag = 0;
//    }
//    else if(ulStatus == 3)
//    {
//        CANMessageGet(CANA_BASE, 3, &s_RX_MBOX3, true);
//        CANIntClear(CANA_BASE, 3);
//        g_ulRxMsgCount++;
//        g_bErrFlag = 0;
//    }
//    else if(ulStatus == 4)
//    {
//        CANMessageGet(CANA_BASE, 4, &sRXCANMessage_KJ, true);
//        CANIntClear(CANA_BASE, 4);
//        g_ulRxMsgCount++;
//        g_bErrFlag = 0;
//    }
//    else
//    {
//
//    }
//
//    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
//}

void Can_Data_Send_Test()
{
    sTXCANMessage.ui32MsgID = 0x110;
    CANMessageSet(CANA_BASE, 1, &s_TX_MBOX1, MSG_OBJ_TYPE_TX);

    sTXCANMessage2.ui32MsgID = 0x120;
    CANMessageSet(CANA_BASE, 2, &s_TX_MBOX2, MSG_OBJ_TYPE_TX);

    ucTXMsgData[0] = a;
    ucTXMsgData[1] = a + 1;
    ucTXMsgData[2] = a + 2;
    ucTXMsgData[3] = a + 3;
    ucTXMsgData[4] = a + 4;
    ucTXMsgData[5] = a + 5;
    ucTXMsgData[6] = a + 6;
    ucTXMsgData[7] = a + 7;

    ucTXMsgData2[0] = b;
    ucTXMsgData2[1] = b - 1;
    ucTXMsgData2[2] = b - 2;
    ucTXMsgData2[3] = b - 3;
    ucTXMsgData2[4] = b - 4;
    ucTXMsgData2[5] = b - 5;
    ucTXMsgData2[6] = b - 6;
    ucTXMsgData2[7] = b - 7;
//===============================================
    uc_tx_mbox1_data[0] = a;
    uc_tx_mbox1_data[1] = a + 1;
    uc_tx_mbox1_data[2] = a + 2;
    uc_tx_mbox1_data[3] = a + 3;
    uc_tx_mbox1_data[4] = a + 4;
    uc_tx_mbox1_data[5] = a + 5;
    uc_tx_mbox1_data[6] = a + 6;
    uc_tx_mbox1_data[7] = a + 7;

    uc_tx_mbox2_data[0] = b;
    uc_tx_mbox2_data[1] = b - 1;
    uc_tx_mbox2_data[2] = b - 2;
    uc_tx_mbox2_data[3] = b - 3;
    uc_tx_mbox2_data[4] = b - 4;
    uc_tx_mbox2_data[5] = b - 5;
    uc_tx_mbox2_data[6] = b - 6;
    uc_tx_mbox2_data[7] = b - 7;

    a++;
    b--;
}
//==================================================================================== Example Code End


static Uint16 rx_ctrl_data[8] = {0,};




Uint16 m_wCMD_flag = 0;
extern volatile Uint16 share_can_data[8];



void check_command(Uint16 id, volatile Uint16 * rx_buffer)
{


	comm_chk_cnt = 0;
    unsigned char OPCODE ;
  //  unsigned char PARALLELCODE ;    //2015.4.30 Parallel add


    id = 0x100;

    SSID = id;// 변수에 그냥 넣어벌니다!
    Uint16 k = 0;
    //for( k = 0; k < 8 ; k ++) uc_rx_mbox3_data[k] = rx_buffer[k] & 0xFF; // ID 및 데이타 변수에 카피




    for( k = 0; k < 8 ; k ++) {
    	rx_ctrl_data[k] = rx_buffer[k] & 0xFF; // ID 및 데이타 변수에 카피

    	//rx_ctrl_data[k] = share_can_data[k];
    }

/*
    rx_ctrl_data[0] = share_can_data[0];
    rx_ctrl_data[1] = share_can_data[1];
    rx_ctrl_data[2] = share_can_data[2];
    rx_ctrl_data[3] = share_can_data[3];
    rx_ctrl_data[4] = share_can_data[4];
    rx_ctrl_data[5] = share_can_data[5];
    rx_ctrl_data[6] = share_can_data[6];
    rx_ctrl_data[7] = share_can_data[7];
*/
    /*
    ctrl_data_0 =  rx_ctrl_data[0];
    ctrl_data_1 =  rx_ctrl_data[1];
    ctrl_data_2 =  rx_ctrl_data[2];
    ctrl_data_3 =  rx_ctrl_data[3];
    ctrl_data_4 =  rx_ctrl_data[4];
    ctrl_data_5 =  rx_ctrl_data[5];
    ctrl_data_6 =  rx_ctrl_data[6];
    ctrl_data_7 =  rx_ctrl_data[7];

*/

    if( Pack_sensing1_flag == 0 )
      {
      	Pack_sensing1_flag = 1;
      }

	   switch(rx_ctrl_data[0])
	   {
		   case 0x80 :
//              Rx_request_data = 80;
			   SCI_Grid_ON_flag   = 1;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;

			   break;
		   case 0x81 :
//              Rx_request_data = 81;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 1;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;

			   break;
		   case 0x82 :
//              Rx_request_data = 82;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 1;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;




	            tmp_volt = (((Uint32)rx_ctrl_data[2]) << 16) | (rx_ctrl_data[3] << 8 )  //Volt(v)
						| rx_ctrl_data[4];
				SCI_Voltage_ref = (float)tmp_volt * 0.001 ;

				tmp_curr = (((Uint32)rx_ctrl_data[5]) << 16) | (rx_ctrl_data[6] << 8 )  //Curr(c)
						| rx_ctrl_data[7] ;

				SCI_Current_ref = (float)tmp_curr * 0.001 ;


				 tmp_volt = 0;
				 tmp_curr = 0;

			   break;
		   case 0x83 :
//              Rx_request_data = 83;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 1;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;

	            tmp_volt = (((Uint32)rx_ctrl_data[2]) << 16) | (rx_ctrl_data[3] << 8 )  //Volt(v)
						| rx_ctrl_data[4];
				SCI_Voltage_ref = (float)tmp_volt * 0.001 ;

				tmp_curr = (((Uint32)rx_ctrl_data[5]) << 16) | (rx_ctrl_data[6] << 8 )  //Curr(c)
						| rx_ctrl_data[7] ;

				SCI_Current_ref = (float)tmp_curr * 0.001 ;

				 tmp_volt = 0;
				 tmp_curr = 0;

			   break;
		   case 0x84 :
//              Rx_request_data = 84;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 1;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;

			   break;
		   case 0x85 :
//              Rx_request_data = 85;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 1;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;
			   break;
		   case 0x86 :
//              Rx_request_data = 86;


			  // SCI_Current_ref = 0;



			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 1;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 0;



			   break;
		   case 0x88 :
//              Rx_request_data = 88;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
//              SCI_Charge_flag    = 0;
//              SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 1;
			   SCI_Err_Clear_flag = 0;
			   break;
		   case 0x89 :
//              Rx_request_data = 89;
			   SCI_Grid_ON_flag   = 0;
			   SCI_Pack_ON_flag   = 0;
			   SCI_Charge_flag    = 0;
			   SCI_Discharge_flag = 0;
			   SCI_Pack_OFF_flag  = 0;
			   SCI_All_Stop_flag  = 0;
			   SCI_Standby_flag   = 0;
			   SCI_Pattern_flag   = 0;
			   SCI_Err_Clear_flag = 1;
			   break;

		   default :
			   tx_data.bit.CMD = m_wCMD;
			   //tx_can_frame.data.bit.CMD = 0x90;
			   break ;
	   }



		OPCODE = ( rx_ctrl_data[1] >> 4) & 0xf ;

		if(OPCODE == 0x1)      SCI_CCCV_CP_CR_flag = 1;   //CC/CV
		else if(OPCODE == 0x2) SCI_CCCV_CP_CR_flag = 2;   //CP
		else if(OPCODE == 0x3) SCI_CCCV_CP_CR_flag = 3;
		else if(OPCODE == 0x4) SCI_CCCV_CP_CR_flag = 4;
		else if(OPCODE == 0x5) SCI_CCCV_CP_CR_flag = 5;
		else if(OPCODE == 0x6) SCI_CCCV_CP_CR_flag = 6;
		else if(OPCODE == 0x7) SCI_CCCV_CP_CR_flag = 7;
		else if(OPCODE == 0x8) SCI_CCCV_CP_CR_flag = 8;
		else if(OPCODE == 0x9) SCI_CCCV_CP_CR_flag = 9;
		else                   SCI_CCCV_CP_CR_flag = 0;


    if(SSID == 0x100 || SSID == 0x300) //ksw :: ch1-> ID=100, ch2-> ID=200, ch1_2-> ID300 에서만 m_uSSID 을 셋팅 한다.
    {
        m_uSSID = SSID;
        m_wCMD = rx_ctrl_data[0];
        comm_chk_cnt = 0;

    }






            if((rx_ctrl_data[0] == 0x80 ) ||
              ( rx_ctrl_data[1] == 0x81 ))
            {
                Pack_sensing1_flag = 1;
            }




}
