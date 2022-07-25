//###########################################################################
//
// FILE:   Phoenixon_Extern.h
//
//###########################################################################

#ifndef __Phoenixon_EXTERN_H__
#define __Phoenixon_EXTERN_H__


//ADC.c
extern void Init_adc_param_18bit(void);
extern void Read_ad_value_18bit(void);
extern void Read_DSP_ad_16bit(void);
extern void Init_adc_16bit(void);
extern void Rms_value(void);
extern void Read_ad_UI_vaule(void);
extern float Va, Vb, Vc, Ia, Ib, Ic, Ibat, Ibat_H, Ibat_L, Vdc, Vbat, Vpack, Ibat_EV;
extern float Va_rms, Vb_rms, Vdc_rms, Ibat_rms, Vbat_rms, Vpack_rms, Ia_rms, Ib_rms, Ic_rms, Vdc_Top_rms, Vdc_Bot_rms, Vsrc_rms;
extern float La_V, Lb_V, Ifan_rms;
extern float Temp_INV_R, Temp_SRC_P, Temp_SPC_S, Temp_DC_T1, Temp_DC_T3, Temp_DC_B1, Temp_DC_B3, Temp_H_TR;
extern float Ibat_Cha_scale, Ibat_Dis_scale, Ibat_Cha_offset,Ibat_Dis_offset, Ibat_View, DC_Pow, DC_Pow2;
extern float  AC2_5V_ref, DC2_5V_ref, AVDD_5V_ref, DVDD_5V_ref, Rly_24V_ref, Comm422_5V_ref;

//Setting  Error Chk.c
extern void Init_Unit_Setting(void);
extern void Warning_Error_check(void);
extern void Grid_Error_check(void);
extern void DC_Error_check(void);
extern void Fault_Error_check(void);
extern void IGBT_Error_check(void);
extern void Fault_check(void);
extern void All_MC_off(void);
extern void System_fault(void);
extern void Emergerncy_check(void);
extern void System_Warning_fault(void);
extern unsigned int DC_error_code, Grid_error_code, Fault_error_code, Warning_error_code, Etc_error_code, IGBT_error_code;
extern int Master_DSP, Slave_DSP, Sys_start_flag, comm_chk_err_flag, DRV_RST_Mode, NoLoad_Input_Test;
extern float Vde_over, Ibat_over, Ibat_over_max, Vbat_over, Vbat_over_max, Vpack_over, Vpack_over_max;
extern float Grid_Vline, Iqrs_max, Vdc_ref_Max, AC_Reactor, Output_Max_Current, Output_Power_kW;
extern float Ibat_over_add, Vbat_over_add, Vpack_over_add, Discha_End_rem, Vdc_init_max;
extern float Step_R_F_ms, Step_TR_ms, Curr_decrease_Time, Vsrc_init_max, PWM2_CV_MAX;

//PLL.C
extern void Pll(void);
extern int freq_check_flag, freq_50Hz, freq_60Hz;
extern float sine_table[], sin_pll, cos_pll, theta, Vde, Vqe, out_dir;


//TIMER_INT.c
extern void Control_reset(void);
extern void Control_ALL_reset(void);
extern void Standby_End_Ctrl(void);
extern int sys_mode, Converter_On_flag, Inverter_On_flag, Ibat_decre_flag, Warning_fault_flag;
extern int Mode_V_change_flag,  Mode_I_change_flag, Vreverce_Chk_flag;
extern unsigned long Error_cnt, SCI_cccv_Cnt;
extern float Iqrs_ref, Idrs_ref, Vqre, Vdre, Ioqrs_ref, Iodrs_ref, Voqre, Vodre, Vlimit;
extern float Idiff_ref_max, Vbat_CV_Volt;
extern float ADC_Read_Val[8];

//MAIN.C
extern void Init_Global_Gpio(void);
extern unsigned int SCI_Data_Send_Cnt;

//Control.c
extern void Control_Sequence(void);
extern void Vpack_Range_PI_Ctrl(void);

extern int D_Input_flag, D_Input2_flag, Ctrl_testm, SCI_cccv_Cnfim;
extern int Curr_Change_flag, Bidirection_flag, Bidirection_mode, Bidirection_On_flag;
extern int SRC_start_flag, SRC_mode,  SRC_comm_flag;
extern int Iqrs_offset_init_flag, Differ_mode, Differ_On_flag;
extern int Cha_Dis_Start_flag;
extern float Vdc_ref, Vodc_integ, Ioqrs_integ, Iodrs_integ, PWM_half;
extern float Iqrs_integ, Idrs_integ, Iqrs_inv_integ, Vbat_CV_Vol_Fix;
extern float Vdiff_CV_ref, Idiff_ref,Vbuck1_CV_ref, Idiff_ref_p1, Iint_limit_max;
extern float Idrs_ref, Idrs_integ, Ictrl_CP_ref, Ibat_CV_ref, PWM_SRC_DT;

//Function_test.c
extern void Pwm_test(void);
extern void MC_test(void);
extern void Lamp_test(void);
extern void Etc_test(void);
extern void PWM_AC_Open(void);
extern void PWM_AC_Block(void);
extern void PWM_SRC_Open(void);
extern void PWM_SRC_Block(void);
extern void HRPWM_Open(void);
extern void HRPWM_Block(void);
extern void PWM_Block(void);
extern int PWM_test_flag, MC_test_flag, Lamp_test_flag, Etc_test_flag, MEP_ScaleFactor;

//Can.C
extern void Init_scib(void);
extern void Scib_tx_isr(void);
extern void Scib_rx_isr(void);
extern void Tx_data_write(void);
extern void Rx_Check_sum(int Rx_array_num);
extern int  Scib_tx_pos, Scib_tx_end, Scib_rx_pos, Scib_rx_end, Tx_data_write_ok, SCI_100ms_flag;
extern int  Rx_all_receive,Charge_flag, Discharge_flag;
extern int  SCI_Charge_flag, SCI_Discharge_flag, SCI_Standby_flag, SCI_TX_cccv, SCI_All_Stop_flag, CP_confirm_flag;
extern int  comm_chk_err_flag, SCI_Grid_ON_flag, SCI_Pack_ON_flag, SCI_Pack_OFF_flag, SCI_Comm_flag, SCI_Parallel_flag;
extern int  SCI_CCCV_CP_CR_flag, SCI_Pattern_flag, SCI_Err_Clear_flag;
extern unsigned long comm_chk_cnt, Ibat_rms_tmp, Ibat_rms_p0, can_cnt;
extern float SCI_Voltage_ref, SCI_Current_ref, SCI_CP_Power, Ibat_err, Discha_End_Volt;
extern float Vpack_UI_rms, Ibat_UI_rms,Ibat_UI_ref, Ibat_UI_sum;

//Can_prcs.c
extern void  ReadBuffData_CH1(void);
extern void  ReadBuffData_CH2(void);
extern void ReadBuffData_PARA(void);
extern int 	CAN_all_receive;
extern unsigned int Pack_Cycler_flag, CV_ON_flag, CV_Rest_cnt;
extern int m_nExtTime;
extern Uint16 SSID ;
extern Uint32 SCI_Operation_Time;

extern Uint16 m_nExtTotalDataCount; // 20150527 외부패턴 시간,데이터 전송 카운트 추가
extern Uint32 m_nExtTotalTime;
extern Uint16 m_uSSID;
extern float fltoffset, fltDelta1,  fltDelta2, err_com;
extern int m_CanConnectCheck, Ex_Pattern_flag;//2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가
extern unsigned long Real_Pattern_cnt;	////2015.06.14 kys 100ms dsp-> pc 자동응답 위하여 추가
extern unsigned char m_szRealCmdBuf;
extern unsigned long m_nStepCount_CH1;
extern unsigned long m_nStepCount_CH2;

extern unsigned long m_nStepCount_Buf;
extern int m_nPatFlag, m_nPatEndFlag, Dir_Charge_flag, Dir_Discharge_flag, CAN_interrupt_flag;

extern float m_fltChargeV_ref;
extern float m_fltDisChargeV_ref;

extern int m_nCAN_Rcvflag;


extern int Direct_CHA_DIS_flag, Pattern_read_flag, Pattern_cnt, R_100ms_flag, Pattern_Rest_flag;


#endif
