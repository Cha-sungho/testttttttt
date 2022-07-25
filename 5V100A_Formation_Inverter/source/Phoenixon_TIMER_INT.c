#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"
#include "../main.h"

#include "PhoenixonControls_COMMUNICATION.h"

#pragma CODE_SECTION(cpu_timer0_isr, ".TI.ramfunc");
#pragma CODE_SECTION(cpu_timer2_isr, ".TI.ramfunc");
#pragma CODE_SECTION(check_command, ".TI.ramfunc");

unsigned long led_cnt = 0;

int sys_mode = 0;
float ON_time = 1.;

//Adc
float ADC_Read_Val[8] = { 0., 0., 0., 0., 0., 0., 0., 0. };

// Unit Status & Control
//int Pack_Cycler_comm=0, CV_ON_comm=0;
int SCI_Grid_ON_flag = 0, SCI_Pack_ON_flag = 0, SCI_Pack_OFF_flag = 0, SCI_Standby_flag = 0, SCI_All_Stop_flag = 0, SCI_Err_Clear_flag = 0;
int SCI_Charge_flag = 0, SCI_Discharge_flag = 0, SCI_CCCV_CP_CR_flag = 0, SCI_Comm_flag = 1, SCI_Parallel_flag = 0, Bidirection_On_flag = 0;
int SCI_TX_cccv = 0, Err_Clear_Comm = 0, freq_check_ok = 0, Vreverce_Chk_Cofm = 0, Vreverce_Chk_flag = 0;
int CP_confirm_flag = 0, Ibat_decre_flag = 0;

//Power Status & Control
int Converter_On_flag = 0, Inverter_On_flag = 0, Direct_CHA_DIS_flag = 0, Mode_I_change_flag = 0, Mode_V_change_flag = 0;
float PWM_half = 0., PWM_BB_half=0., SCI_Voltage_ref = 0., SCI_Current_ref = 0., SCI_CP_Power = 0., Soft_Per = 0.85;
float Ibat_decre_val = 0., Vbat_CV_Volt = 0., Discha_End_Volt = 0., Ibat_min = 0.5;

// Count
int Ibat_decre_cnt = 0, Fan_cnt2_Max = 4, Decre_Max = 0, AC_Fan_On_flag = 0;
unsigned long ti_cnt = 0, Err_Clear_cnt = 0, charge_cnt = 0, Chk_cnt = 0, MC_cnt = 0, Fan_cnt = 0, Fan_cnt2 = 0;
unsigned long Protection_V_cnt = 0, Protection_I_cnt = 0, Relay1_cnt = 0, Relay2_cnt = 0, soft_cnt = 0;

// DIO Check
int Parll_Sync_IN = 0, parallel_start_flag = 0, Parll_Pattern_Sync_OUT = 0, Parll_Pattern_Sync_IN = 0;
int Parallel_Cofirm = 0, Parll_CV_IN = 0, Parll_Fault_IN = 0;

//Communication
unsigned long comm_chk_cnt = 0, can_cnt = 0;

//extern test value line
unsigned long ul_Extern_One_time_Cycle_Cnt = 0;
float f_Real_Time = 0;

unsigned int t_flag = 0, ui_Pattern_Read_flag = 1;
//=========================================

extern Uint16 get_Master_DSP(void);
// prd의 값을 카운터에 넣으면 다시 카운트 리셋됨!!
extern float Recti_Voltage;
//

static uint32_t count_1ms = 0; // kwk 추가 190206
#if ( _TIMER2_1ms_ == 1)
    static Uint16 can_send_count_1ms = 0;
#elif (_TIMER2_10ms_ == 1)
    static Uint16 can_send_count_10ms = 0;
#endif

uint32_t get_count_1ms(void)
{
    return count_1ms;
}
void clear_count_1ms(void)
{
    //StopCpuTimer2();
    count_1ms = 0;
    //if( can_send_count_10ms >= 5)
    {
        //	can_send_count_10ms -= 5;
    }
    //can_send_count_1ms = 50;
    //StartCpuTimer2();
}

void clear_step_sync_n_send_sync(void)
{
    {//200225 kimwonkyum
//        count_1ms = 0;
//
//        #if(_TIMER2_10ms_ == 1)
//            can_send_count_10ms = 5;	// 중간값으로~~
//        #elif( _TIMER2_1ms_ == 1)
//            can_send_count_1ms = 50;	// 10 or 50
//        #endif
    }
    {//200225 fixed this for a timing TX
        count_1ms = 0;
        if (get_Master_DSP() > 0)   { can_send_count_10ms = 5; }
        else                        { can_send_count_10ms = 6; }
    }
}

#if( TIMER2_ENABLE == 1)
void Timer2_count_clear(void)
{
    {//200225 kimwonkyum
//        StopCpuTimer2();
//        #if ( CAN_PACKET_SEND_PRIORITY == 0)  // 0 이면 마스터가 더빠르다..
//            if( get_Master_DSP() == 1)	// Master
//            {
//                ReadCpuTimer2Counter() = ReadCpuTimer2Period() - (200 * 1000U * 5); ///원래 100ms 인터럽트 시간..초기화.
//            }
//            else
//            {
//                ReadCpuTimer2Counter() = (ReadCpuTimer2Period()); // 5ms  먼저 인터럽트가 발생한다..고로 슬레이브가 더 먼저 패킷보냄!!
//            }
//        #elif ( CAN_PACKET_SEND_PRIORITY == 1)
//            if (get_Master_DSP() == 1) // Master
//            {
//                ReadCpuTimer2Counter() = ReadCpuTimer2Period(); ///원래 100ms 인터럽트 시간..초기화.
//            }
//            else
//            {
//                ReadCpuTimer2Counter() = (ReadCpuTimer2Period() - (200 * 1000U * 5)); // 5ms  먼저 인터럽트가 발생한다..고로 슬레이브가 더 먼저 패킷보냄!!
//            }
//        #endif
//
//        ReloadCpuTimer2();
//        count_1ms = 0;
//
//        #if(_TIMER2_10ms_ == 1)
//            can_send_count_10ms = 0; // 타이머 변수들 초기화.
//        #elif(_TIMER2_1ms_ == 1)
//            can_send_count_1ms = 0;
//        #endif
//        StartCpuTimer2();
    }
    {//200225 fixed this for a timing TX
        StopCpuTimer2();
        count_1ms = 0;
        can_send_count_10ms = 0;
        ReadCpuTimer2Counter() = ReadCpuTimer2Period();

        StartCpuTimer2();
    }

}
#endif

#if( TIMER2_ENABLE == 1)
    // 슬레이브가 먼저 pc로 쏘도록 수정...
    void Timer2_count_clear_2(void)
    {
        {//200225 kimwonkyum
//            StopCpuTimer2();
//
//            #if ( CAN_PACKET_SEND_PRIORITY == 0)  // 0 이면 마스터가 더빠르다..
//                if( get_Master_DSP() == 1) // Master
//                {
//                    ReadCpuTimer2Counter() = (200 * 1000U * 5); // 10ms
//                }
//                else
//                {
//                    ReadCpuTimer2Counter() = (200 * 1000U * 10); //5ms   슬레이브가 먼저  인터럽트를 발생시킴...
//                }
//            #elif ( CAN_PACKET_SEND_PRIORITY == 1)
//                if (get_Master_DSP() == 1) // Master
//                {
//                    ReadCpuTimer2Counter() = (200 * 1000U * 10); // 10ms
//                }
//                else
//                {
//                    ReadCpuTimer2Counter() = (200 * 1000U * 5); //5ms   슬레이브가 먼저  인터럽트를 발생시킴...
//                }
//            #endif
//
//            ReloadCpuTimer2();
//            StartCpuTimer2();
        }
        {//200225 fixed this for a timing TX
            StopCpuTimer2();
            CAN_all_receive = 0;
            count_1ms = 0;

            ReadCpuTimer2Counter() = ReadCpuTimer2Period();
            if (get_Master_DSP() > 0)   { can_send_count_10ms = 5; }
            else                        { can_send_count_10ms = 6; }

            StartCpuTimer2();
        }
    }
#endif

void Init_Timer()
{
    EALLOW;
        PieVectTable.TIMER0_INT = &cpu_timer0_isr;
        //PieVectTable.TIMER1_INT = &cpu_timer1_isr;
        #if( TIMER2_ENABLE == 1)
            PieVectTable.TIMER2_INT = &cpu_timer2_isr;
        #endif
    EDIS;

    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0, 200, Fix_Tsamp);			// 66.66666us period
    //ConfigCpuTimer(&CpuTimer1, 200, 1000000);

    #if (TIMER2_ENABLE == 1)
        #if ( _TIMER2_100ms_ == 1)
            ConfigCpuTimer(&CpuTimer2, 200, 100000); // 100ms Timer
        #elif ( _TIMER2_1ms_ == 1)
            ConfigCpuTimer(&CpuTimer2, 200, 1000); // 1ms Timer
        #elif (_TIMER2_10ms_ == 1 )
            ConfigCpuTimer(&CpuTimer2, 200, 10000); //10ms Timer
        #endif
    #endif

    CpuTimer0Regs.TCR.all = 0x4001;				// Interrupt EN
    //CpuTimer1Regs.TCR.all = 0x4000;
    //CpuTimer2Regs.TCR.all = 0x4000;//DouB - set to interrupt enable

    IER |= M_INT1;
    //IER |= M_INT13;
    #if( TIMER2_ENABLE == 1)
        IER |= M_INT14;
    #endif

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    #if ( TIMER2_ENABLE == 1)
        StopCpuTimer2(); //Timer2 stop;
    #endif
}

void Control_ALL_reset(void)
{
    Bidirection_flag = 0;
    Bidirection_mode = 0;
    Bidirection_On_flag = 0;

    PWM_half = (PWM_MAX_INV * 0.5);
    EPwm10Regs.CMPA.bit.CMPA = (int)PWM_half;
    EPwm11Regs.CMPA.bit.CMPA = (int)PWM_half;
    EPwm12Regs.CMPA.bit.CMPA = (int)PWM_half;

    SRC_mode = 0;
    SRC_start_flag = 0;
    SRC_comm_flag = 0;

    EPwm7Regs.CMPA.bit.CMPA = 0;
    EPwm7Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - 1.0);//DouB - origin

    EPwm8Regs.CMPA.bit.CMPA = 0;
    EPwm8Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - 1.0);//DouB - origin

    Differ_On_flag = 0;
    Differ_mode = 0;

#if (Formation_Inverter == 0)
    PWM_BB_half = (PWM_MAX_BB * 0.5f);

    EPwm1Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm1Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm1Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB

    EPwm2Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm2Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm2Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB

    EPwm3Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm3Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm3Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB

    EPwm4Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm4Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm4Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB

    EPwm5Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm5Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm5Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB

    EPwm6Regs.CMPA.bit.CMPA = (int)PWM_BB_half;
    EPwm6Regs.CMPA.bit.CMPAHR = 256;//256 == "(1.5 << 8)" ; DouB
    EPwm6Regs.DBFEDHR.all = 512;//512 == "(1.5 << 9)" ; DouB
#endif
}

void Control_INV_reset(void)
{
    Bidirection_flag = 0;
    Bidirection_mode = 0;
    Bidirection_On_flag = 0;

    PWM_half = (PWM_MAX_INV * 0.5);
    EPwm10Regs.CMPA.bit.CMPA = (int)PWM_half;
    EPwm11Regs.CMPA.bit.CMPA = (int)PWM_half;
    EPwm12Regs.CMPA.bit.CMPA = (int)PWM_half;

    SRC_mode = 0;
    SRC_start_flag = 0;
    SRC_comm_flag = 0;

    EPwm7Regs.CMPA.bit.CMPA = 0;
    EPwm7Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - 1.0);//DouB
    EPwm8Regs.CMPA.bit.CMPA = 0;
    EPwm8Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - 1.0);//DouB
}

void Parallel_DIO_Check(void)
{
    if (sys_mode == 7)
    {
        if (Parallel_Cofirm == 1)//병렬운전일 때만..
        {
            if (Master_DSP == 1)
            {
                D_Out_ON();

                if (!GpioDataRegs.GPBDAT.bit.GPIO38)
                {
                    D_Input_flag = 1;
                }
                else
                {
                    D_Input_flag = 0;
                }

                if (D_Input_flag == 1)
                {
                    Parll_Sync_IN = 1;
                    can_cnt = 0;
                    ul_Extern_One_time_Cycle_Cnt = 0;

                    //clear_count_1ms();// 190203 kwk 추가
                }
            }
            else if (Slave_DSP == 1)
            {
                if (!GpioDataRegs.GPBDAT.bit.GPIO38)
                {
                    D_Input_flag = 1;
                    D_Out_ON();
                }
                else
                {
                    D_Input_flag = 0;
                    D_Out_OFF();
                }

                if (D_Input_flag == 1)
                {
                    Parll_Sync_IN = 1;
                    can_cnt = 0;
                    ul_Extern_One_time_Cycle_Cnt = 0;

                    //clear_count_1ms(); // 190203 kwk 추가
                }
            }
        }
    }

    if (Etc_test_flag == 0)
    {
        if ((sys_mode <= 3) || (Ibat_decre_flag == 1))
        {
            D_Out_OFF();

            D_Input_flag = 0;
            Parll_Sync_IN = 0;
        }
    }
}

Uint16 kwk_test = 0;
Uint16 kwk_test_SCI_Operation_Time = 0;
extern void  convert_sensing(void);
__interrupt void cpu_timer0_isr(void)
{
	//Led_High();//Timer interrupt Cycle 확인용  ; 4DEBUG

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

//    AC_RUN_ON();
//    LAMP_CHARGE_ON();       // Formation Inverter   AC Input On - Blue LED

    led_cnt++;
    if (led_cnt < REF_TIME(ON_time))            { Led_High(); }
    else if (led_cnt < REF_TIME(ON_time * 2))   { Led_Low(); }
    else                                        { led_cnt = 0; }

    Read_DSP_ad_16bit();
    Read_ad_value_18bit();
    DSP_ADC_Read(ADC_Read_Val);

    Read_ad_UI_vaule();

    Control_Sequence();         // 인버터, 컨버터 제어 루틴


    convert_sensing(); // for I/F




    if (sys_mode >= 4){
//        if ((CV_ON_flag == 1) && (Pack_Cycler_flag == 1)){  Unused SDI
//            SetBit(Warning_error_code, System_Mode_Fault);
//        }
//        if ((CV_ON_flag == 0) && (Pack_Cycler_flag == 0)){
//            SetBit(Warning_error_code, System_Mode_Fault);
//        }
    }

    if ((sys_mode <= 3) && (Direct_CHA_DIS_flag == 0)){
        if (Master_DSP == 1){
            if (Pack_sensing1_flag == 1){
                Relay1_cnt++;
                if (Relay1_cnt >= REF_TIME(0.25)){
                    Pack_Sensing_ON();

                    Relay1_cnt = 0;
                }
            }
            else{
                Pack_Sensing_OFF();
            }
        }

        if ((Slave_DSP == 1) && (SCI_Parallel_flag == 0)){
            if (Pack_sensing1_flag == 1){
                Relay1_cnt++;
                //Pack_Parall_Sensing_OFF();//DouB ; 200227

                if (Relay1_cnt >= REF_TIME(0.25)){
                    Pack_Sensing_ON();

                    Pack_Parall_Sensing_ON();//DouB ; 200227 not used parallel relay

                    Relay1_cnt = 0;
                    Relay2_cnt = 0;
                }
            }
            else{
                Pack_Sensing_OFF();
                Pack_Parall_Sensing_OFF();

                Relay1_cnt = 0;
                Relay2_cnt = 0;
            }
        }

        if (Pack_Cycler_flag == 1){
            if ((Slave_DSP == 1) && (SCI_Parallel_flag == 1)){
                if (Pack_sensing1_flag == 1){
                    Relay2_cnt++;
                    //Pack_Sensing_OFF();//DouB ; 200227

                    if (Relay2_cnt >= REF_TIME(0.25)){
                        Pack_Parall_Sensing_ON();

                        Pack_Sensing_ON();//DouB ; 200227 not used parallel relay

                        Relay1_cnt = 0;
                        Relay2_cnt = 0;
                    }
                }
                else{
                    Pack_Parall_Sensing_OFF();
                    Pack_Sensing_OFF();

                    Relay1_cnt = 0;
                    Relay2_cnt = 0;
                }
            }
        }
    }

    //================== extern control start  =================//
    if (SCI_Pattern_flag == 1){
//		Ex_Pattern_flag = 1;

//		if(parallel_start_flag == 1){

        if (ui_Pattern_Read_flag == 1){
            if (t_flag == 1){
                /////////////////////////////////////////////////////////////
                //clear_count_1ms();// kwk 추가 19.02.03
                clear_step_sync_n_send_sync();					//한번만 무조건
                // 0 and 5로...
                /////////////////////////////////////////////////////////////

                ul_Extern_One_time_Cycle_Cnt = 0;
                can_cnt = 0;
                t_flag = 0;
            }
            if (Master_DSP == 1){
                ReadBuffData_CH1();
            }
            else if (Slave_DSP == 1){
                ReadBuffData_CH2();
            }

            ui_Pattern_Read_flag = 0;
        }
#if 0 // original
        f_Real_Time = (float)(ul_Extern_One_time_Cycle_Cnt * 0.0909091); //0.1-> 100us,  0.909091-> 90.91us,   0.06666 -> 66.666us
        // 1ms 인지 확인해야함.
        if(f_Real_Time >= SCI_Operation_Time)
        {
            m_nStepCount_Buf++;
            ul_Extern_One_time_Cycle_Cnt = 0;
            f_Real_Time = 0;
            ui_Pattern_Read_flag = 1;
        }
#else

        f_Real_Time = (float)count_1ms;
        if (f_Real_Time >= SCI_Operation_Time)
        {
            clear_count_1ms(); // 카운트값 초기화.
            m_nStepCount_Buf++;
            ul_Extern_One_time_Cycle_Cnt = 0;
            f_Real_Time = 0;
            ui_Pattern_Read_flag = 1;
        }

#endif
        //	}
    }
    else
    {
//		Ex_Pattern_flag = 0;
        m_nStepCount_Buf = 1;
        ul_Extern_One_time_Cycle_Cnt = 0;
        ui_Pattern_Read_flag = 1;
        t_flag = 1;
    }
    //===================================================== extern control end

    switch (sys_mode){
    case 0:

        PWM_Block();
        Control_ALL_reset();
        RS422_Comm_ON();

        ti_cnt = 0;
        soft_cnt = 0;
        SCI_TX_cccv = 0;
        SCI_Charge_flag = 0;
        SCI_Discharge_flag = 0;
        SCI_Pack_ON_flag = 0;
        SCI_Pack_OFF_flag = 0;
        SCI_Standby_flag = 0;
        SCI_CCCV_CP_CR_flag = 0;
        Warning_fault_flag = 0;
        Vbat_over = Vbat_over_max;
        Vpack_over = Vpack_over_max;
        Ibat_over = Ibat_over_max;

        Bidirection_flag = 0;
        Bidirection_mode = 0;
        Bidirection_On_flag = 0;

        SRC_mode = 0;
        SRC_start_flag = 0;
        SRC_comm_flag = 0;

        Direct_CHA_DIS_flag = 0;
        Error_cnt = 0;

        Vreverce_Chk_flag = 0;
        Ibat_decre_flag = 0;
        AC_Fan_On_flag = 1;

        Vreverce_Chk_Cofm = 0;
        Chk_cnt = 0;

//			Ibat_Cha_offset = 0.;
//			Ibat_Dis_offset =0.;

//			Parll_Pattern_Sync_OFF();
//			Parll_Fault_Out_OFF();
//			Parll_CV_Out_OFF();

        Parll_Fault_IN = 0;
        Parll_CV_IN = 0;
        Parll_Pattern_Sync_IN = 0;

//			Grid_SOFT_MC_OFF();
//			Grid_MAIN_MC_OFF();
        LAMP_RUN_OFF();

        if ((Sys_start_flag == 1) && (SCI_Grid_ON_flag == 1))
        {
            SCI_Grid_ON_flag = 0;

            comm_chk_cnt = 0;
            Vbat_CV_Volt = 0.;
            SCI_Voltage_ref = 0.;
            SCI_Current_ref = 0.;

            sys_mode++;
        }

        if (SCI_Err_Clear_flag == 1)
        {
            if(Err_Clear_Comm ==0)
            {
                DRV_RST_Mode = 0;
            }
            Err_Clear_Comm = 1;
        }

        if ((Err_Clear_Comm == 1)&&(DRV_RST_Mode >= 3))
        {
            Err_Clear_cnt++;
            if (Err_Clear_cnt > REF_TIME(0.1))
            {
                LAMP_FAULT_OFF();
                Err_Clear_cnt = 0;
                Err_Clear_Comm = 0;
                SCI_Err_Clear_flag = 0;
                Warning_error_code = 0;
                Grid_error_code = 0;
                DC_error_code = 0;
                Fault_error_code = 0;
                IGBT_error_code = 0;
                comm_chk_err_flag = 0;
                comm_chk_cnt = 0;

                Sys_start_flag = 1;
            }
        }
        break;

    case 1:                                                     //SOFT CHARGING AND PLL CHECK
        ti_cnt++;
        Err_Clear_cnt = 0;
        Err_Clear_Comm = 0;
        AC_Fan_On_flag = 0;

        if ((ti_cnt > REF_TIME(1.)) && (Vde > Vde_over))
        {
            ti_cnt = 0;
            SetBit(Grid_error_code, PLL_Error);
        }

        if (ti_cnt > REF_TIME(2.))
        {
            if (freq_check_ok == 1)
            {
                Grid_SOFT_MC_ON();
                charge_cnt = 0;
                ti_cnt = 0;
                sys_mode++;
            }
        }

        if (ti_cnt > REF_TIME(10.))
        {
            ti_cnt = 0;
            SetBit(Grid_error_code, Vqe_Unbalance);
        }

        if ((freq_check_flag == 1) && (freq_check_ok == 0))
        {
            freq_check_ok = 1;
        }
        break;

    case 2:                                                     // GRID Initial Charge -> Main MC On
        if (Vdc_rms > ((root2 * Grid_Vline) * Soft_Per))
        {
            if (Vdc_rms > ((root2 * Va_rms) * Soft_Per))
            {
                ti_cnt++;
                if (ti_cnt > REF_TIME(1.))
                {
                    Grid_MAIN_MC_ON();

                    if (ti_cnt > REF_TIME(2.))
                    {
                        ti_cnt = 0;
                        Grid_SOFT_MC_OFF();
                        sys_mode++;
                    }
                }
            }
        }

        else if (++charge_cnt > REF_TIME(50.)){
            charge_cnt = 0;
            SetBit(Grid_error_code, Initial_Charge_Error);
        }

        break;

    case 3:                                                     // 인버터(AC/DC + SRC) 동작 시작
//        LAMP_RUN_ON(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
        LAMP_DISCHARGE_ON();        // Formation Inverter   Run - Green LED

        PWM_AC_Block();
        PWM_SRC_Block();
        Control_INV_reset();

        Vbat_over = Vbat_over_max;
        Vpack_over = Vpack_over_max;
        Ibat_over = 10.;

        Idiff_ref_max = 0.; //important 180503

        if (SCI_Standby_flag == 1)
        {
            ti_cnt = 0;
        }

        SCI_TX_cccv = 1;

       /*  NoLoad_Input_Test _ 0 : Total Charge/Discharge Operation
           NoLoad_Input_Test _ 1 : Until No Load AC Inverter Operation
           NoLoad_Input_Test _ 2 : Until AC Inverter + SRC Operation
           NoLoad_Input_Test _ 3 : Until AC Inverter + SRC + DC/DC CV Operation   */

        if (Direct_CHA_DIS_flag == 0){
            SCI_cccv_Cnfim = 0; SCI_cccv_Cnt = 0;
            if ((SCI_Pack_ON_flag == 1) && (Warning_fault_flag == 0)){
#if(DSP_COMMUNICATION_CHANNEL == 1)
                if (Pack_sensing1_flag == 1){
#else
                if (Pack_sensing2_flag == 1){
#endif
                	if (NoLoad_Input_Test == 0){
						if (Vreverce_Chk_Cofm == 0){
							Vreverce_Chk_flag = 1;

							Chk_cnt++;
							if (Chk_cnt >= REF_TIME(0.3)){
								if (Vpack_rms >= 1.){   //3 -> 1
									SetBit(Warning_error_code, Pack_Reverse_Fault);
									Vreverce_Chk_flag = 0;
									Chk_cnt = 0;
								}
								else {
									Vreverce_Chk_Cofm = 1;
									Vreverce_Chk_flag = 0;
									Chk_cnt = 0;
								}
							}
						}
						else if (Vreverce_Chk_Cofm == 1){
							Vreverce_Chk_flag = 0;

							if (Vreverce_Chk_Stat() == 0)//DouB ; start timing problem 200504
							{
                                ti_cnt++;
                                if (ti_cnt >= REF_TIME(0.3))
                                {
                                    if (Vdc > (Recti_Voltage * 0.9))
                                    {
                                        ti_cnt = 0;
                                        //Vdc_ref = Vdc;
                                        Vdc_ref = Vdc_rms;

                                        Bidirection_On_flag = 1;
                                        Iint_limit_max = 1.;
                                        sys_mode = 4;
                                    }
                                }
                                else
                                {
                                    //if (ti_cnt > REF_TIME(0.5))
                                    if (ti_cnt > REF_TIME(1.0))
                                    {
                                        ti_cnt = 0;
                                        SCI_Pack_ON_flag = 0;
                                        SetBit(Warning_error_code, Pack_contact_Fault); // 알람 바꿔야 함
                                    }
                                }
							}
						}
                    }

                	if (NoLoad_Input_Test >= 1)
                	{
						//Vdc_ref = Vdc;
						Vdc_ref = Vdc_rms;

						Bidirection_On_flag = 1;
						Iint_limit_max = 1.;
						sys_mode = 4;
                	}
                }
            }
        }

//        if (Direct_CHA_DIS_flag == 1){
//            sys_mode = 6;
//        }
        break;

    case 4:  //CONVERTER PARAMETA INITIALIZE & Output MC ON

        Idiff_ref_max = 0.; //important 180503

        if(NoLoad_Input_Test == 0){         // NoLoad_Input_Test _ 0 : Total Charge/Discharge Operation
            if (SRC_comm_flag == 1){                        // SRC_control이 시작하면
                if (Direct_CHA_DIS_flag == 0){
#if(DSP_COMMUNICATION_CHANNEL == 1)
                    if (Pack_sensing1_flag == 1){           // 0xAA 받으면
#else
                    if (Pack_sensing2_flag == 1){           // 0xBB 받으면
#endif
                        if (SCI_Pack_ON_flag == 1){         // 0x81 받으면

                            ti_cnt++;

                            if (Vreverce_Chk_Cofm == 1){
                                Vreverce_Chk_flag = 0;
//                                Differ_On_flag = 1;

                                ti_cnt++;

                                if (Chk_cnt++ >= REF_TIME(1.5)){ //3 -> 1.5
                                    ti_cnt = 0;
                                    Chk_cnt = 0;

                                    Vreverce_Chk_Cofm = 0;
                                    SCI_Pack_ON_flag = 0;

                                    CHAR_DC_MAIN_MC_ON();       // Formation Inverter 출력 MC On

                                    sys_mode = 5;
                                }


                                if (ti_cnt >= REF_TIME(15.)){  //15 -> 5
                                    ti_cnt = 0;
                                    Chk_cnt = 0;
                                    Vreverce_Chk_Cofm = 0;
                                    SCI_Pack_ON_flag = 0;
                                    SetBit(Warning_error_code, Pack_contact_Fault); //output_int_charge_fail
                                }
                            }
                        }
                    }
                }
            }
        }

#if 0
        if(NoLoad_Input_Test == 0){         // NoLoad_Input_Test _ 0 : Total Charge/Discharge Operation
			if (SRC_comm_flag == 1){
				if (Direct_CHA_DIS_flag == 0){
					if (Pack_sensing1_flag == 1){
						if (SCI_Pack_ON_flag == 1){
							if (Vreverce_Chk_Cofm == 1){
								Vreverce_Chk_flag = 0;
								Differ_On_flag = 1;

								ti_cnt++;
//                              if (fabsf(Vpack_rms - Vbat_rms) <= 0.5){ //1 -> 05 //2020.04.30 pkj
								if (fabsf(Vpack_rms - Vbat_rms) <= 2.5){ //1 -> 05
//									Iint_limit_max =  0.5; //0.1; //0.001;//DouB
								    Iint_limit_max =  1.2; //0.1; //0.001;//DouB

									if (Chk_cnt++ >= REF_TIME(1.5)){ //3 -> 1.5
										ti_cnt = 0;
										Chk_cnt = 0;

										Vreverce_Chk_Cofm = 0;
										SCI_Pack_ON_flag = 0;

										sys_mode = 5;
									}
								}

								if (ti_cnt >= REF_TIME(15.)){  //15 -> 5
									ti_cnt = 0;
									Chk_cnt = 0;
									Vreverce_Chk_Cofm = 0;
									SCI_Pack_ON_flag = 0;
									SetBit(Warning_error_code, Pack_contact_Fault); //output_int_charge_fail
								}
							}
						}
					}
				}
			}
        }
#endif
	/*  NoLoad_Input_Test _ 0 : Total Charge/Discharge Operation
		NoLoad_Input_Test _ 1 : Until No Load AC Inverter Operation
		NoLoad_Input_Test _ 2 : Until AC Inverter + SRC Operation
		NoLoad_Input_Test _ 3 : Until AC Inverter + SRC + DC/DC CV Operation   */
        if(NoLoad_Input_Test == 3){
			if(SRC_comm_flag == 1){
				if (Direct_CHA_DIS_flag == 0){
#if(DSP_COMMUNICATION_CHANNEL == 1)
					if (Pack_sensing1_flag == 1){
#else
                    if (Pack_sensing2_flag == 1){
#endif
						if (SCI_Pack_ON_flag == 1){
//							Differ_On_flag = 1;

							ti_cnt++;
							if (ti_cnt >= REF_TIME(3.))
							{
								Iint_limit_max =  0.5; //0.001;
                                ti_cnt = 0;
                                SCI_Pack_ON_flag = 0;

                                CHAR_DC_MAIN_MC_ON();       // Formation Inverter 출력 MC On

                                sys_mode = 5;
							}
						}
					}
				}
			}
        }
        if (SCI_Pack_OFF_flag == 1){
        	SCI_Pack_ON_flag =0;
        	sys_mode = 6;
        }
        break;

    case 5: //OutPut SoftCharge &  Vpack Connection check

#if (Formation_Inverter == 0)
        if(NoLoad_Input_Test == 0){
			if (Vpack_rms > 5.){  //4
//              if (fabsf(Vpack_rms - Vbat_rms) <= 0.5) // 2020.04.30 pkj
				if (fabsf(Vpack_rms - Vbat_rms) <= 2.5)
				{
//				    Iint_limit_max = 0.5; //0.1; //0.001; //DouB    // 2020.04.30 pkj
                    Iint_limit_max = 1.2; //0.1; //0.001; //DouB

					soft_cnt++;
					if (soft_cnt > REF_TIME(0.1))
					{
						CHAR_DC_MAIN_MC_ON();

						if (soft_cnt > REF_TIME(0.5))
						{
							soft_cnt = 0;
							ti_cnt = 0;
							sys_mode = 6;

							Iint_limit_max = 0.1;//200917 DouB 첫번째 REST 구간에서 전류발생 방지
							Cha_Dis_Start_flag = 1;//200917 DouB 첫번째 REST 구간에서 전류발생 방지
						}
					}
				}
				if (++ti_cnt > REF_TIME(15.)){   //15
					SetBit(Warning_error_code, Pack_contact_Fault);
				}
			}
			else{
				if (++ti_cnt > REF_TIME(2.)){
					ti_cnt = 0;
					SetBit(Warning_error_code, Pack_contact_Fault);
				}
			}
        }

        if (NoLoad_Input_Test == 3)
        {
			soft_cnt++;
			if (soft_cnt > REF_TIME(0.1))
			{
				CHAR_DC_MAIN_MC_ON();//DC MC ON

				if (soft_cnt > REF_TIME(0.5))
				{
					soft_cnt = 0;
					ti_cnt = 0;
					sys_mode = 6;
				}
			}
        }
#endif

        SCI_TX_cccv = 2;        //Standby, 22.03.21 Formation Inverter 추가
        Ibat_over = 35.;        // 22.03.19 Formation Inverter 추가

        if (SCI_Parallel_flag == 1){
            Parallel_Cofirm = 1;
        }
        else{
            Parallel_Cofirm = 0;
        }

        if (SCI_Pack_OFF_flag == 1){
            SCI_Pack_ON_flag =0;
            sys_mode = 6;
        }

        break;

    case 6: //CONVERTER START
        LAMP_CHARGE_OFF();
        LAMP_DISCHARGE_OFF();

        Protection_I_cnt = 0;
        Protection_V_cnt = 0;

        if ((SCI_Charge_flag == 1) || (SCI_Discharge_flag == 1)){
            comm_chk_cnt = 0;

            if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 3)){
                if (SCI_Current_ref <= 0){
                    SCI_Current_ref = -(SCI_Current_ref);
                }
            }
            else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 5)){
                if (SCI_Current_ref >= 0){
                    SCI_Current_ref = -(SCI_Current_ref);
                }
            }

            //*************   CC Reference check  Start    **************//
            if ((SCI_CCCV_CP_CR_flag == 3) || (SCI_CCCV_CP_CR_flag == 5)){
                if (SCI_Voltage_ref >= 0.001){
                    Vbat_CV_Volt = SCI_Voltage_ref;
                    Idiff_ref = SCI_Current_ref;
                    Idiff_ref_max = SCI_Current_ref;

                    // Protection_check
                    Ibat_over = fabsf(SCI_Current_ref) + Ibat_over_add;
                    if (Ibat_over >= Ibat_over_max)
                        Ibat_over = Ibat_over_max;

                    if ((SCI_Charge_flag == 1) && (SCI_Current_ref >= 0.001)){
                        if (SCI_Voltage_ref < Vpack_UI_rms){
                            SetBit(Warning_error_code, Charge_CV_Under_Fault);
                        }
                        else{
                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = SCI_Voltage_ref + Vpack_over_add;
                            if (Vbat_over >= Vbat_over_max){
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }

                            sys_mode = 7;
                        }
                    }
                    else if ((SCI_Discharge_flag == 1) && (SCI_Current_ref <= -0.001)){
                        if (SCI_Voltage_ref > Vpack_UI_rms){
                            SetBit(Warning_error_code, Discharge_CV_Over_Fault);
                        }
                        else{
                            Vbat_over = Vbat_over_max;
                            Vpack_over = Vpack_over_max;
                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem) Discha_End_Volt = Discha_End_rem;

                            sys_mode = 7;
                        }
                    }
                }
            }
            //*************   CC Reference check  End    **************//

            //*************   CP Reference check  Start  **************//
            if ((SCI_CCCV_CP_CR_flag == 7) || (SCI_CCCV_CP_CR_flag == 8)){
                if (SCI_Voltage_ref >= 0.001){
                    Vbat_CV_Volt = SCI_Voltage_ref;
                    SCI_CP_Power = SCI_Current_ref;

                    if (SCI_CCCV_CP_CR_flag == 7){
                        Idiff_ref_max = (SCI_CP_Power * 1000) / Vpack_UI_rms;
                        Idiff_ref = Idiff_ref_max;
                    }
                    else if (SCI_CCCV_CP_CR_flag == 8){
                        Idiff_ref_max = -((fabsf(SCI_CP_Power) * 1000) / Vpack_UI_rms);
                        Idiff_ref = Idiff_ref_max;
                    }

                    // Protection_check
                    Ibat_over = Ibat_over_max;

                    if ((SCI_Charge_flag == 1) && (SCI_Current_ref >= 0.001)){
                        if (SCI_Voltage_ref < Vpack_UI_rms){
                            SetBit(Warning_error_code, Charge_CV_Under_Fault);
                        }
                        else{
                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = Vbat_CV_Volt + Vpack_over_add;
                            if (Vbat_over >= Vbat_over_max){
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }
                            sys_mode = 7;
                        }
                    }
                    else if ((SCI_Discharge_flag == 1) && (SCI_Current_ref >= 0.001)){
                        if (SCI_Voltage_ref > Vpack_UI_rms){
                            SetBit(Warning_error_code, Discharge_CV_Over_Fault);
                        }
                        else{
                            Vbat_over = Vbat_over_max;
                            Vpack_over = Vpack_over_max;
                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem)Discha_End_Volt = Discha_End_rem;

                            sys_mode = 7;
                        }
                    }
                }
            }
            //*************   CP Reference check  End    **************//
        }

        if ((SCI_Pack_OFF_flag == 1) || (Warning_fault_flag == 1)){
        	++MC_cnt;
            if (MC_cnt >= REF_TIME(0.1)){  //1
                PWM_Block();
                Control_ALL_reset();
                if (MC_cnt >= REF_TIME(0.5)){
					CHAR_DC_MAIN_MC_OFF();
					CHAR_DC_OUT_MC_OFF();
					CHAR_DC_OUT_Parall_MC_OFF();CHAR_DC_SOFT_MC_OFF();
					SCI_Pack_OFF_flag = 0;
					Direct_CHA_DIS_flag = 0;
					MC_cnt = 0;
					sys_mode = 3;
                }
            }
        }

        break;

    case 7: //BUCK CONVERTER START

        if (Parallel_Cofirm == 1){
            if (Parll_Sync_IN == 1){
                sys_mode = 8;
            }
        }
        else{
            sys_mode = 8;
        }

        if (SCI_Standby_flag == 1){
            SCI_Pack_OFF_flag = 1;
            sys_mode = 8;
        }

        break;

    case 8: //STANDBY CHECK

        //LAMP_CHARGE_ON();//DouB ; changing a location

        D_Out_OFF();
        D_Input_flag = 0;
        Parll_Sync_IN = 0;

        if (SCI_Pattern_flag == 0){
            if (SCI_Comm_flag == 1){  // PC - DSP Communication fault check
                comm_chk_cnt++;
                if (comm_chk_cnt >= REF_TIME(10.)){
                    comm_chk_err_flag = 1;
                    comm_chk_cnt = 0;
                }
            }
        }

        if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 3)){
            LAMP_CHARGE_ON();//DouB ; changing a location
            LAMP_DISCHARGE_OFF();//DouB ; changing a location

            if (SCI_Current_ref <= 0){
                SCI_Current_ref = -(SCI_Current_ref);
            }
        }
        else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 5)){
            LAMP_CHARGE_OFF();//DouB ; changing a location
            LAMP_DISCHARGE_ON();//DouB ; changing a location

            if (SCI_Current_ref >= 0){
                SCI_Current_ref = -(SCI_Current_ref);
            }
        }

        if (Ibat_decre_flag == 0){ // 방전에서 -0.001 필요하여 UI 전류값 부호에 따라 변경 -최현수
            if (SCI_Standby_flag == 1){
                if (SCI_Current_ref >= 0){
                    SCI_Current_ref = 0.001;
                    SCI_Charge_flag = 0;
                }
                else if (SCI_Current_ref < 0){
                    SCI_Current_ref = -0.001;
                    SCI_Discharge_flag = 0;
                }
//                SCI_Current_ref = 0.001; // Standby에서 UI 전류값을 무시하고 0.001로 함 -한정호
            }
        }

        if (Ibat_decre_flag == 0){
            if ((SCI_Charge_flag == 1) || (SCI_Discharge_flag == 1) || (SCI_Standby_flag == 1)){ //add SCI_Standby_flag 190913
              //*************   CC Reference check  Start  **************//
                if ((SCI_CCCV_CP_CR_flag == 3) || (SCI_CCCV_CP_CR_flag == 5) || (SCI_Standby_flag == 1)){ //add SCI_Standby_flag 190913
                    if (Idiff_ref_max != SCI_Current_ref){
                        Mode_I_change_flag = 1;
                        Curr_Change_flag = 1;

                        Idiff_ref = SCI_Current_ref;
                        Idiff_ref_max = SCI_Current_ref;

                        if (Differ_mode != 3){ //Mode_I_change_flag : CV-> CC
                            Mode_I_change_flag = 0;
                        }
                    }

                    //fixed routine
				   if ((fabsf(Idiff_ref) + Ibat_over_add) < Ibat_over)
				   {
					   if (Protection_I_cnt++ >= REF_TIME(0.5))
					   {
						   Protection_I_cnt = 0;
						   Ibat_over = fabsf(Idiff_ref) + Ibat_over_add;

						   if (Ibat_over >= Ibat_over_max)
							   Ibat_over = Ibat_over_max;
					   }
				   }
				   else if ((fabsf(Idiff_ref) + Ibat_over_add) > Ibat_over)
				   {
					   Protection_I_cnt = 0;
					   Ibat_over = fabsf(Idiff_ref) + Ibat_over_add;

					   if (Ibat_over >= Ibat_over_max)
						   Ibat_over = Ibat_over_max;
				   }


                    if (Vbat_CV_Volt != SCI_Voltage_ref){
                        Mode_V_change_flag = 1;
                        Curr_Change_flag = 1;

                        Vbat_CV_Volt = SCI_Voltage_ref;
                        Vdiff_CV_ref = SCI_Voltage_ref;

                        if (Differ_mode != 3){
                            Mode_V_change_flag = 0;
                        }
                    }

                    if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 3)){
                        if (Protection_V_cnt++ >= REF_TIME(0.15)){
                            Protection_V_cnt = 0;
                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = Vbat_CV_Volt + Vpack_over_add;
                            if (Vbat_over >= Vbat_over_max){
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }
                        }
                    }
                    else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 5)){
                        if (Protection_V_cnt++ >= REF_TIME(0.15)){
                            Protection_V_cnt = 0;
                            //Vbat_CV_Volt = SCI_Voltage_ref;
                            //Vdiff_CV_ref = SCI_Voltage_ref;

                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem)
                                Discha_End_Volt = Discha_End_rem;
                        }
                    }
                }
                //*************   CC Reference check  End    **************//

                //*************   CP Reference check  Start  **************//
                if ((SCI_CCCV_CP_CR_flag == 7) || (SCI_CCCV_CP_CR_flag == 8) || (SCI_Standby_flag == 1)){
                    if (SCI_CP_Power != SCI_Current_ref){
                        Mode_I_change_flag = 1;
                        Curr_Change_flag = 1;

                        SCI_CP_Power = SCI_Current_ref;

                        if (Differ_mode != 3){
                            Mode_I_change_flag = 0;
                        }
                    }

                    if (SCI_CCCV_CP_CR_flag == 7){
                        if (SCI_Standby_flag == 0){
                            Idiff_ref_max = (SCI_CP_Power * 1000)
                                    / Vpack_UI_rms;
                            Idiff_ref = Idiff_ref_max;
                        }
                        else{
                            Idiff_ref_max = 0.001;
                            Idiff_ref = 0.001;
                        }
                    }
                    else if (SCI_CCCV_CP_CR_flag == 8){
                        if (SCI_Standby_flag == 0){
                            Idiff_ref_max = -((fabsf(SCI_CP_Power) * 1000)
                                    / Vpack_UI_rms);
                            Idiff_ref = Idiff_ref_max;
                        }
                        else{
                            Idiff_ref_max = -0.001;
                            Idiff_ref = -0.001;
                        }
                    }

                    if (Vbat_CV_Volt != SCI_Voltage_ref){
                        Mode_V_change_flag = 1;
                        Curr_Change_flag = 1;

                        Vbat_CV_Volt = SCI_Voltage_ref;
                        Vdiff_CV_ref = SCI_Voltage_ref;

                        if (Differ_mode != 3){
                            Mode_V_change_flag = 0;
                        }
                    }

                    if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 7)){
                        if (Protection_V_cnt++ >= REF_TIME(0.15)){
                            Protection_V_cnt = 0;
                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = Vbat_CV_Volt + Vpack_over_add;
                            if (Vbat_over >= Vbat_over_max){
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }
                        }
                    }
                    else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 8)){
                        if (Protection_V_cnt++ >= REF_TIME(0.15)){
                            Protection_V_cnt = 0;
                            //Vdiff_CV_ref = SCI_Voltage_ref;
                            //Vbat_CV_Volt = SCI_Voltage_ref;

                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem)
                                Discha_End_Volt = Discha_End_rem;
                        }
                    }
                }
                //*************   CP Reference check  End    **************//
            }
        }

        if ((SCI_Pack_OFF_flag == 1) || (Warning_fault_flag == 1)){
            Ibat_decre_flag = 1;
            Idiff_ref = 0.001;
            Idiff_ref_max = 0.001;
            Curr_Change_flag = 1;
        }

        if (Ibat_decre_flag == 1){
            Ibat_decre_cnt++;
            //16.6ms/100us=166 -> 0.006,   10ms/100us=100 -> 0.01,   7.5ms/66us=75 -> 0.0133,   5ms/66us=50 -> 0.02,
            if ((fabsf(Ibat) <= Ibat_min) || (Ibat_decre_cnt >= Decre_Max)){
                Ibat_decre_cnt = 0;
                PWM_Block();
                //HRPWM_Block();
                Control_ALL_reset();

                SCI_Standby_flag = 0;
                SCI_Charge_flag = 0;
                SCI_Discharge_flag = 0;
                Ibat_decre_flag = 0;

                Direct_CHA_DIS_flag = 1;

                sys_mode = 3;
            }
        }
        break;

    default:
        break;
    }
    //Case End

    if (Vreverce_Chk_flag == 1)
    {
        Vreverce_Chk_ON();//역극성 검사
    }
    else
    {
        Vreverce_Chk_OFF();
    }

//    if ((sys_mode >= 3) && (sys_mode < 90)){
    if ((sys_mode >= 1) && (sys_mode < 90)){
        Fan_cnt++;
        AC_HS_FAN_ON();

        if (Fan_cnt >= REF_TIME(0.5)){
            Fan_cnt = 0;
            Fan_cnt2 = 0;
            AC_Fan_On_flag = 1;
        }
    }
    else{
        if((AC_Fan_On_flag == 1)||(sys_mode <= 2)){
            Fan_cnt++;
            if (Fan_cnt >= REF_TIME(30.)){
                Fan_cnt = 0;

                Fan_cnt2++;
                if (Fan_cnt2 >= Fan_cnt2_Max){
                    AC_HS_FAN_OFF();

                    AC_Fan_On_flag = 0;
                    Fan_cnt = 0;
                    Fan_cnt2 = 0;
                }
            }
        }
    }

//    if (sys_mode >= 4) { AC_HS_FAN_ON(); }//DouB ; 4DEBUG

    if (PWM_test_flag){
        sys_mode = 99;
        Pwm_test();
    }

    if (MC_test_flag){
        sys_mode = 99;
        MC_test();
    }

    if (Lamp_test_flag){
        sys_mode = 99;
        Lamp_test();
    }

    if (Etc_test_flag){
        sys_mode = 99;
        Etc_test();
    }

    if (SCI_All_Stop_flag){
        System_fault();
    }

    Fault_check();
    Parallel_DIO_Check();

    if (m_CanConnectCheck == 1){
        if (can_cnt >= 1099)   //1500 -> 15kHz, 1099 -> 11kHz    1000 -> 10kHz
        {
#if ( ORIGINAL_METHOD == 1)
            CAN_all_receive = 1;
#endif
        }
        else
        {
            can_cnt++;
            ul_Extern_One_time_Cycle_Cnt++;
        }
    }
    else
    {
//		can_cnt = 0;
//		CAN_all_receive = 0;
    }

    SCI_Data_Send_Cnt++;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	//Led_Low();//Timer interrupt Cycle 확인용 ; 4DEBUG


//    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
//    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);
//    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER2);
//
//    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
//    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER1);
//
//    ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER0);
//
//    ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER0);
//    ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER1);

}

//
// cpu_timer1_isr - CPU Timer1 ISR
//
__interrupt void cpu_timer1_isr(void)
{

}

//
// cpu_timer2_isr CPU Timer2 ISR
//

Uint16 checking = 0;
__interrupt void cpu_timer2_isr(void)
{
#if ( _TIMER2_100ms_ == 1 )
#if( TIMER2_ENABLE == 1)
    if(m_CanConnectCheck == 1)
    {
        CAN_all_receive = 1;
    }
#endif
#elif ( _TIMER2_1ms_ == 1)
    count_1ms ++;
    can_send_count_1ms ++;

    if (can_send_count_1ms >= 100)
    {
        can_send_count_1ms = 0;

        if (m_CanConnectCheck == 1)
        {
            CAN_all_receive = 1;
        }
    }
#elif (_TIMER2_10ms_ == 1)
    count_1ms += 0x0A;   // 10ms 이므로 10개씩..

    can_send_count_10ms++;
    if (can_send_count_10ms >= 10)
    {
        can_send_count_10ms = 0;

        if (m_CanConnectCheck == 1)
        {
            CAN_all_receive = 1;
        }
    }

    checking++;
#endif
}


float temp_Voltage_data = 0;
float temp_Current_data = 0;
unsigned long _long_temp_Voltage_data = 0;
signed long _long_temp_Current_data = 0;

extern volatile Uint16 share_vol_high;
extern volatile Uint16 share_vol_low;
extern volatile int16 share_cur_high;
extern volatile int16 share_cur_low;

extern volatile int32 share_int32_cur_data;
extern volatile float share_float64_capacity;

extern volatile Uint16 share_IGBT_error_code;
extern volatile Uint16 share_Warning_error_code;
extern volatile Uint16 share_Grid_error_code;
extern volatile Uint16 share_DC_error_code;
extern volatile Uint16 share_Fault_error_code;
extern volatile Uint16 share_cmd_data;
extern volatile Uint16 share_op_paracode_data;
extern volatile Uint16 share_gpio_hw_sync;
extern volatile Uint16 share_step_index_cpu2;

extern volatile Uint16 share_pause_flag;

extern unsigned char m_wCMD;
extern void check_command(Uint16 id,volatile Uint16 * rx_buffer);

extern unsigned int Schedule_refresh_CPU1;
extern unsigned int Schedule_refresh_CPU2;

extern Uint16 command_id;
extern volatile Uint16 share_can_data[8];

Uint16 timer_check = 0;



//Uint16 test_flag = 0;
//Uint16 test_send = 0;
//static Uint16 prev_test_send = 0;


volatile int16 temp_share_vol_cur_high = 0;
volatile int16 temp_share_vol_cur_low = 0;


static Uint16 prev_cmd_data = 0;
static Uint16 prev_pause_flag = 0;
static Uint16 prev_step_index_cpu2 = 0;








extern volatile S_COMMUNICATION_GSRAM_CPU2 GSRAM_DATA[];

volatile Uint16 original_count = 0;
volatile Uint16 index = 0;
extern volatile Uint16 bank;
extern Uint16 get_current_COMMAND(void);
extern unsigned int Now_step;
extern unsigned int prev_Comp_Loop_Cnt;
extern volatile Uint16 stop_flag;
//Uint16 count98 = 0;
//Uint16 count99 = 0;

extern Uint16 pause_flag;
extern Uint16 display_flag;

extern float64 VoltageAVG;
extern float64 CurrentAVG;
extern float64 WattAVG;

void convert_sensing(void)
{
#if 0
    temp_Voltage_data = Vpack;
#else
   // temp_Voltage_data = Vpack_rms; // CPU1에서 이미 평균 내줌!!
    temp_Voltage_data = Vpack_UI_rms;
#endif
#if 0
    temp_Current_data = Ibat;
#else
  //  temp_Current_data = Ibat_rms;
    temp_Current_data = Ibat_UI_rms;
#endif
#if 0
    temp_Voltage_data = count_0 ++;
    temp_Current_data = count_1 ++;
#endif





    //timer_check ++;
    _long_temp_Voltage_data = (unsigned long)(temp_Voltage_data * MULTI);
    _long_temp_Current_data = (signed long)(temp_Current_data * MULTI);

    share_vol_high = (_long_temp_Voltage_data >> 16 ) & 0xFFFF;
    share_vol_low  = (_long_temp_Voltage_data ) & 0xFFFF;

    share_cur_high = (_long_temp_Current_data >> 16 ) & 0xFFFF;
    share_cur_low  = (_long_temp_Current_data ) & 0xFFFF;

#if 1
    share_int32_cur_data = _long_temp_Current_data;
#else
    share_int32_cur_data = temp_Current_data;
#endif
   // temp_share_vol_cur_high = 0;
   // temp_share_vol_cur_low = (int16)(temp_Current_data);

    share_Warning_error_code = Warning_error_code;
    share_Grid_error_code = Grid_error_code;
    share_DC_error_code = DC_error_code;
    share_Fault_error_code = Fault_error_code;
    share_IGBT_error_code = IGBT_error_code;


#if 1
    share_cmd_data = m_wCMD + 0x10;
    share_op_paracode_data  = ((SCI_TX_cccv << 4 ) & 0xF0 ) | ( SCI_Parallel_flag & 0x0F);
#else
    share_cmd_data = count_0;
    share_op_paracode_data  = count_1;

#endif


    	if( stop_flag == 0 )
    	{

			original_count ++;
			if( original_count >= 11)//
			{
				original_count = 0;


		    		if((( share_op_paracode_data >> 4 ) & 0x0F) >= 2 )
				         {
				      		if(( share_cmd_data == 0x92 ) || ( share_cmd_data == 0x93) || ( share_cmd_data == 0x96))
				      		{
				      			if ( prev_cmd_data != share_cmd_data)//여기서 초기화를 할것이냐 안할것이냐 결정.
				      			{

				      				if (prev_pause_flag != pause_flag)
				      				{
				      					if(( prev_pause_flag == 1 ) && (pause_flag == 0 ))
				      					{
				      						;
				      					//	check_6 ++;
				      					}
				      					prev_pause_flag = pause_flag;

				      					//check_5 ++;
				      				}
				      				else
				      				{
				      					if (( prev_pause_flag == 0 ) && ( pause_flag == 0 ))
				      					{
				      						share_float64_capacity = 0;
				      					}
				      				}

				      			//	check ++;
				      				prev_cmd_data = share_cmd_data;

				      			}
				      			else
				      			{

				      			}

				      			if( pause_flag == 0 )
				      			{
				      				if( share_cmd_data == 0x96 )
				      				{
				      					temp_Current_data = 0;
				      				//	check_7 ++;
				      				}

				      				if( prev_step_index_cpu2 != Now_step)
				      				{
				      					share_float64_capacity = 0;
				      					prev_step_index_cpu2 = Now_step;

				      				//	check_8 ++;
				      				}
				      				//share_float64_capacity += fabsf(temp_Current_data) * (float)(1.0/3600.0) * (float)(0.00009090909091);
				        			//	share_float64_capacity += fabsf(temp_Current_data) * 0.000000025252525252525 ;
				      				share_float64_capacity += fabsf(temp_Current_data) * (float)(1.0/3600.0) * (float)(0.001);
				      				//check_7 ++;

				      			}
				      			else if( pause_flag == 1)
				      			{
				      				;
				      				//check_3 ++;
				      			}

				      		}
				      	}
				      	else
				      	{
				      		share_float64_capacity = 0;
				      		//check_4 ++;
				      	}

				if( index < 100)
				{
					Uint16 bank_offset = bank + 1;
					GSRAM_DATA[bank_offset].CPU2_DATA.ChannelIndex = CH_NUMBER;//default;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].VOLTAGEx16 = share_vol_high;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].VOLTAGEx00 = share_vol_low;

					if( share_int32_cur_data < 0 ) share_int32_cur_data *= - 1;
					int32 temp_Cur_2 = share_int32_cur_data;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CURRENTx16 = (((Uint32)temp_Cur_2) >> 16) & 0xFFFF;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CURRENTx00 = ((Uint16)temp_Cur_2 & 0xFFFF);

					Uint32 temp32 = (Uint32)(share_float64_capacity * MULTI);
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CAPACITYx16 = (Uint16)( temp32 >> 16 ) & 0xFFFF;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].CAPACITYx00 = (Uint16)( temp32 ) & 0xFFFF;


					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].COMMAND = get_current_COMMAND();
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].SAMPLING_TIME = 1;//default;

					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].STEP_INDEX = Now_step;

					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].OPERATIN = ((share_op_paracode_data >> 4 ) & 0xFF);
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].MODE = 1;
					GSRAM_DATA[bank_offset].CPU2_DATA.DATA[ index ].LOOP_COUNT = prev_Comp_Loop_Cnt - 1;


#if 1
					//if( display_flag == 1){
					//Uint32 temp_Vol = (((Uint32)share_vol_high << 16 ) | share_vol_low);
					//Uint32 temp_Cur = (((Uint32)share_cur_high << 16 ) | share_cur_low);
					VoltageAVG +=  Vpack_UI_rms ;
					CurrentAVG +=  Ibat_UI_rms ;
					WattAVG    += ( Vpack_UI_rms * Ibat_UI_rms);
					//}
#endif

				//	if( index == 98 ) count98 ++;
				//	if( index == 99 ) count99 ++;
				}
				index ++;


			}
    	}

#if 0
    if (Schedule_refresh_CPU1 != Schedule_refresh_CPU2)
    {
        check_command(command_id,share_can_data);

        Schedule_refresh_CPU1 = Schedule_refresh_CPU2;

        checking_2 ++;
    }

#endif
}

