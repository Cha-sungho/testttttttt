#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"

Uint16 get_Master_DSP(void)
{
    return Master_DSP;
}

long DSP_Brd_Version = 1.0;
long Program_Date = 200116;


//DouB ; setting for MATER & SLAVE
int Master_DSP = 1;
int Slave_DSP = 0;



float Grid_Vline = 0, Efficiency = 0., Power_Factor = 0., AC_Reactor = 0., Recti_Voltage=0;
float Output_Max_Voltage = 0, Output_Max_Current = 0, Input_Power_kW = 0., Output_Power_kW = 0, Vdc_ref_Max = 0., Vsrc_ref_Max = 0, DC_Link_Voltage = 0.;
float Igrid_over = 0., Vgrid_over = 0., Vgrid_under = 0., Vdc_over = 0., Vdc_under = 0., Vsrc_over =0., Vsrc_under =0, Vqe_over = 0., Vde_over = 0., Iqrs_max = 0.;
float Ibat_over = 0, Ibat_over_max = 0., Vbat_over = 0., Vbat_over_max = 0., Vpack_over = 0., Vpack_over_max = 0., PWM2_CV_Duty=0., PWM2_CV_MAX=0.;
float Ibat_over_add = 30., Vbat_over_add = 10., Vpack_over_add = 5., Discha_End_rem = 3., Vdc_init_max =0., Vsrc_init_max =0.;
//  4ms/91us=44->0.022, 5ms/91us=55->0.018, 8ms/91us=88->0.011, 9ms/91us=99->0.0101, 10ms/91us=109->0.0091, 18ms/91us=198->0.0051
// 50ms/91us=550->0.00181, 100ms/91us=1099->0.00091, 500ms/91us=5495->0.000181, 1s/91us=10989->0.000091, 5s/91us=55555->0.000018, 10s/91us=111111->0.0000091
float Step_R_F_ms =0.0101, Step_TR_ms =0.0051, Step_1ms = 0.091, Step_2ms = 0.18, Step_4ms = 0.022, Step_8ms = 0.011, Step_9ms = 0.0101, Step_18ms = 0.0051, Curr_decrease_Time = 0.0101;

int Fault_check_start = 0, Sys_start_flag = 1, comm_chk_err_flag = 0., Warning_fault_flag = 0, Warning_Clear_Comm = 0, flt_T = 7.;
int Vgrid_over_fault_cnt = 0, Vgrid_under_fault_cnt = 0, Vqe_over_fault_cnt = 0, Vde_over_fault_cnt = 0, IGBT_SOURCE_test = 0;
int DRV_RST_Mode = 0, DRV_RDY_View = 0, NoLoad_Input_Test = 0;
float Va_Err_view = 0, Vb_Err_view = 0;
float Ia_Err_view = 0, Ib_Err_view = 0, Ic_Err_view = 0, Vdc_Top_rms_View = 0., Vsrc_rms_View = 0.;
float Ibat_Err_view = 0., Vpack_Err_view =0, Vbat_Err_view =0;
float TR_Temp = 95.0, MOFET_Temp = 80.0;//TR_Temp = 0.3, MOFET_Temp = 0.3;
unsigned int Warning_error_code = 0, Grid_error_code = 0, DC_error_code = 0, Fault_error_code = 0, IGBT_error_code = 0, Fuse_AC_fault_cnt = 0, Warning_Clear_cnt = 0;
unsigned long F_Check_cnt = 0, Error_cnt = 0, IGBT_Pw_On_cnt = 0, IGBT_Pw_Off_cnt = 0, Ibat_fault_cnt = 0, Fuse_DC_fault_cnt = 0, Pack_Fault_cnt = 0, Pack_Fault_cnt2 = 0;
unsigned long Driver1_fault_cnt = 0, Driver2_fault_cnt = 0, Driver3_fault_cnt = 0, Driver4_fault_cnt = 0, Driver5_fault_cnt = 0, Driver6_fault_cnt = 0;
unsigned long DriverA_fault_cnt = 0, DriverB_fault_cnt = 0, DriverC_fault_cnt = 0, DriverD_fault_cnt = 0, DriverE_fault_cnt = 0, DriverF_fault_cnt = 0;
unsigned long Heat_AC_Fault_cnt = 0, Heat_DC_Fault_cnt = 0, Heat_Fan_fault_cnt = 0, Tr_Fan_fault_cnt = 0, Driver_cnt = 0;

Uint16 fault_check_flag = 1;//DouB


void Init_Unit_Setting(void)
{
//    //Input_Setting
//    Grid_Vline = 220;
//    Recti_Voltage = Grid_Vline * 1.414;
//    Efficiency = 0.9;//Fixed 효율
//    Power_Factor = 0.95;//Fixed 역율
////    AC_Reactor = 0.55e-3;//mH ;; DouB
//    AC_Reactor = 1.1e-3;//mH ;; DouB 21LH528
//
//    //Output_Setting
//    Output_Max_Voltage = 100;    //Vbat_over, MAX referenced
//    Output_Max_Current = 50;
//    Output_Power_kW = Output_Max_Voltage * Output_Max_Current;
//
//    Vdc_ref_Max = 400;
//    Vdc_under = Recti_Voltage * 0.95;//-5%
//
//    Vsrc_ref_Max = 160;
//    Vsrc_over = Vsrc_ref_Max * 1.1;//+10%
//    Vsrc_under = Vsrc_ref_Max * 0.85;//-15%
//
//
//    {//***** Function_Setting   *****//  //rising falling time control
//        Step_R_F_ms = Step_9ms;
//        Step_TR_ms = Step_18ms;
//        Curr_decrease_Time = Step_9ms;
//
////        Step_R_F_ms = Step_4ms;
////        Step_TR_ms = Step_8ms;
////        Curr_decrease_Time = Step_4ms;
//
////        Step_R_F_ms = Step_1ms;
////        Step_TR_ms = Step_2ms;
////        Curr_decrease_Time = Step_1ms;
//    }
//
//    {//***** Protection_Auto_Setting  Start *****//
//        Ibat_over_add = 10.;  //30.;
//        Vbat_over_add = 10.;
//        Vpack_over_add = 5.;
//        Discha_End_rem = 3.;
//
//        Ibat_over = Output_Max_Current;
//        Ibat_over_max = Ibat_over + Ibat_over_add;
//        Vbat_over = Output_Max_Voltage + Vbat_over_add;
//        Vbat_over_max = Output_Max_Voltage + Vbat_over_add;
//        Vpack_over = Output_Max_Voltage + Vpack_over_add;
//        Vpack_over_max = Output_Max_Voltage + Vpack_over_add;
//
//        //***** Protection_Auto_Setting  Start *****//
//        Input_Power_kW = Output_Power_kW / (Efficiency * Power_Factor);
//        DC_Link_Voltage = sqrtf(2.) * Grid_Vline;
//
//        Igrid_over = Input_Power_kW / (sqrtf(3.) * Grid_Vline) * 1.1;
//        Iqrs_max = sqrtf(2.) * Igrid_over;
//
//        Vgrid_over = Grid_Vline * 1.1;//+10%
//        Vgrid_under = Grid_Vline * 0.9;//-10%
//
//        Vdc_over = Vdc_ref_Max * 1.15;//+15%
//        Vdc_init_max = Vdc_ref_Max * 0.95;//-5%
//        Vsrc_init_max = Vsrc_ref_Max * 0.9;//-10%
//
//        Vqe_over = DC_Link_Voltage * 1.1;//+10%
//        if (Grid_Vline >= 0.)
//        {
//            Vde_over = 20.;
//        }
//        if ((Grid_Vline > 150) && (Grid_Vline <= 250))
//        {
//            Vde_over = 35.;
//        }
//        if ((Grid_Vline > 250) && (Grid_Vline <= 400))
//        {
//            Vde_over = 50.; //40 20205 hs
//        }
//        if ((Grid_Vline > 400) && (Grid_Vline <= 600))
//        {
//            Vde_over = 100.;  //80  -> 100
//        }
//        if ((Grid_Vline > 600) && (Grid_Vline <= 750))
//        {
//            Vde_over = 120.;  //100  -> 120
//        }
//        if (Grid_Vline > 750)
//        {
//            Vde_over = 150.;
//        }
//
//        PWM2_CV_Duty = (Vsrc_ref_Max + Output_Max_Voltage) / (2 * Vsrc_ref_Max);
//        PWM2_CV_MAX = (PWM2_CV_Duty * PWM_MAX_BB);
// //***** Protection_Auto_Setting  End *****//

//22.01.12  Formation Inverter

    //Input_Setting
//    Grid_Vline = 390;                       //22.01.12  테스트 기준  (거의 400V임)
    Grid_Vline = 400;                       //22.03.25  노스볼트 스웨덴 기준  (거의 400V임)
    Recti_Voltage = Grid_Vline * 1.414;
    Efficiency = 0.9;//Fixed 효율
    Power_Factor = 0.95;//Fixed 역율
    AC_Reactor = 1.3e-3;//mH                //22.01.12  Formation Inverter

//22.01.12  Formation Inverter
    Output_Max_Voltage = 386;               //22.01.12  Vbat 삭제
    Output_Max_Current = 45;                //22.01.12  Ibat은 SRC 출력 전류를 센싱함. 인버터 기준 최대 44A
    Output_Power_kW = Output_Max_Voltage * Output_Max_Current;      //22.01.12  인버터 기준 무시

    Vdc_ref_Max = 587;                      //22.01.12  DC Link1 600V
    Vdc_under = Recti_Voltage * 0.95;//-5%

    Vsrc_ref_Max = 386;                     //22.01.12  DC Link2 386V
    Vsrc_over = Vsrc_ref_Max * 1.15;//+15%
    Vsrc_under = Vsrc_ref_Max * 0.85;//-15%


    {//***** Function_Setting   *****//  //rising falling time control
        Step_R_F_ms = Step_9ms;
        Step_TR_ms = Step_18ms;
        Curr_decrease_Time = Step_9ms;
    }

    {//***** Protection_Auto_Setting  Start *****//
        Ibat_over_add = 5.;             //22.01.12  임의의 값
        Vbat_over_add = 10.;            //22.01.12  Vbat 사용 안함
        Vpack_over_add = 10.;           //22.01.12  Vpack 사용 안함
        Discha_End_rem = 3.;            //22.01.12  뭔지 모름...

        Ibat_over = Output_Max_Current;
        Ibat_over_max = Ibat_over + Ibat_over_add;
        Vbat_over = Output_Max_Voltage + Vbat_over_add;
        Vbat_over_max = Output_Max_Voltage + Vbat_over_add;
        Vpack_over = Output_Max_Voltage + Vpack_over_add;
        Vpack_over_max = Output_Max_Voltage + Vpack_over_add;

        //***** Protection_Auto_Setting Start *****//
        Input_Power_kW = Output_Power_kW / (Efficiency * Power_Factor);
        DC_Link_Voltage = sqrtf(2.) * Grid_Vline;

        Igrid_over = Input_Power_kW / (sqrtf(3.) * Grid_Vline) * 1.1;
        Iqrs_max = sqrtf(2.) * Igrid_over;

        Vgrid_over = Grid_Vline * 1.15;//+15%
        Vgrid_under = Grid_Vline * 0.85;//-15%

        Vdc_over = Vdc_ref_Max * 1.1;//+10%
        Vdc_init_max = Vdc_ref_Max * 0.9;//-10%
        Vsrc_init_max = Vsrc_ref_Max * 0.9;//-10%

        Vqe_over = DC_Link_Voltage * 1.15;//+15%
        if (Grid_Vline >= 0.)
        {
            Vde_over = 20.;
        }
        if ((Grid_Vline > 150) && (Grid_Vline <= 250))
        {
            Vde_over = 35.;
        }
        if ((Grid_Vline > 250) && (Grid_Vline <= 400))
        {
//            Vde_over = 50.; //40 20205 hs
            Vde_over = 100.; // 22.03.30 Formation Inverter 스웨덴 현장 환경 고려 변경
        }
        if ((Grid_Vline > 400) && (Grid_Vline <= 600))
        {
            Vde_over = 100.;  //80  -> 100
        }
        if ((Grid_Vline > 600) && (Grid_Vline <= 750))
        {
            Vde_over = 120.;  //100  -> 120
        }
        if (Grid_Vline > 750)
        {
            Vde_over = 150.;
        }

        PWM2_CV_Duty = (Vsrc_ref_Max + Output_Max_Voltage) / (2 * Vsrc_ref_Max);
        PWM2_CV_MAX = (PWM2_CV_Duty * PWM_MAX_BB);
    }//***** Protection_Auto_Setting  End *****//
}


void Fault_check(void)
{
    if (GpioDataRegs.GPBDAT.bit.GPIO60)
    {
        DRV_RDY_View = 0;   //Disable
    }
    else
    {
        DRV_RDY_View = 1;
    }  //Enable

    switch (DRV_RST_Mode)
    {
    case 0:
        Driver_Set();//HIGH
        if (++Driver_cnt > REF_TIME(1.0))
        {  //0.5
            DRV_RST_Mode = 1;
            Driver_cnt = 0;
        }
        break;

    case 1:
        Driver_Reset();//LOW
        if (++Driver_cnt > REF_TIME(1.0))
        {  //0.5
            DRV_RST_Mode = 2;
            Driver_cnt = 0;
        }
        break;

    case 2:
        Driver_Set();//HIGH
        DRV_RST_Mode = 3;
        Driver_cnt = 0;
        break;

    default:
        break;
    }

    F_Check_cnt++;
    if (F_Check_cnt > REF_TIME(5.))
    {
        F_Check_cnt = 0;
        Fault_check_start = 1;
    }

    if (IGBT_SOURCE_test == 0)
    {
        if (sys_mode > 0)
        {
            if (IGBT_Pw_On_cnt++ > REF_TIME(1.))
            {
                if (sys_mode != 99)
                    IGBT_SOURCE_ON();
                IGBT_Pw_On_cnt = 0;
            }
            IGBT_Pw_Off_cnt = 0;
        }
        else
        {
            if (IGBT_Pw_Off_cnt++ > REF_TIME(0.5))
            {   // 0.1 -> 0.5
                if (sys_mode != 99)
                    IGBT_SOURCE_OFF();
                IGBT_Pw_Off_cnt = 0;
            }
            IGBT_Pw_On_cnt = 0;
        }
    }
    else
    {
        IGBT_SOURCE_ON();  //test
    }

    if (Fault_check_start == 1)
    {
        F_Check_cnt = 0;

        if (fault_check_flag)
        {//should be disable this block when a debug & test ; DouB
            Warning_Error_check();
            Grid_Error_check();
            DC_Error_check();
            Fault_Error_check();
            //IGBT_Error_check();
            Emergerncy_check();
        }


        if ((Warning_error_code != 0) || (Grid_error_code != 0) || (DC_error_code != 0) ||(Fault_error_code !=0)|| (IGBT_error_code != 0)){
//        if ((Grid_error_code != 0) || (DC_error_code != 0) || (Fault_error_code != 0) || (IGBT_error_code != 0))
//        {
            Sys_start_flag = 0;
            System_fault();
            LAMP_RUN_OFF();
            LAMP_FAULT_ON();
            LAMP_CHARGE_OFF();
            LAMP_DISCHARGE_OFF();

            Driver1_fault_cnt = 0; Driver2_fault_cnt = 0; Driver3_fault_cnt = 0;
            Driver4_fault_cnt = 0; Driver5_fault_cnt = 0; Driver6_fault_cnt = 0;
            DriverA_fault_cnt = 0; DriverB_fault_cnt = 0; DriverC_fault_cnt = 0;
            DriverD_fault_cnt = 0; DriverE_fault_cnt = 0; DriverF_fault_cnt = 0;
        }

//        if (Warning_error_code != 0)
//        {
//            Warning_fault_flag = 1;
//
//            System_Warning_fault();
//            LAMP_RUN_ON(); LAMP_FAULT_ON();
//            LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
//        }
    }
}

void System_Warning_fault(void)
{

    Vreverce_Chk_flag = 0;
    SCI_Grid_ON_flag = 0;
    SCI_Pack_ON_flag =0;
    SCI_Charge_flag = 0;
    SCI_Discharge_flag = 0;
    SCI_CCCV_CP_CR_flag = 0;

    if (sys_mode <= 7){
        if (Sys_start_flag == 1){
            Sys_start_flag = 0;
            sys_mode = 7;
        }
    }

    if (SCI_Err_Clear_flag == 1){
        Warning_Clear_Comm = 1;
    }

    if (Warning_Clear_Comm == 1){
        Warning_Clear_cnt++;
        if (Warning_Clear_cnt > REF_TIME(0.5)){
            LAMP_FAULT_OFF();
            Warning_Clear_cnt = 0;
            Warning_Clear_Comm = 0;

            Sys_start_flag = 1;
            Warning_fault_flag = 0;

            SCI_Err_Clear_flag = 0;
            Warning_error_code = 0;

            comm_chk_err_flag = 0;
            comm_chk_cnt = 0;
        }
    }
}

void System_fault(void)
{

    SCI_Standby_flag = 1;
    Ibat_decre_flag = 1;
    Idiff_ref = 0.1;

    PWM_Block();
    Control_ALL_reset();

    if (Error_cnt++ >= REF_ms_TIME(50))
    {
        Error_cnt = 0;
        All_MC_off();
        sys_mode = 0;
    }
}

void Warning_Error_check(void){
    if (NoLoad_Input_Test == 0){
        if ((sys_mode >= 7) && (sys_mode < 90)){
            if (SCI_CCCV_CP_CR_flag == 3){
                if (Idiff_ref_max >= Output_Max_Current + 1.){
                    SetBit(Warning_error_code, Ibat_Over_Operating);
                }
                if (Vbat_CV_Volt >= Output_Max_Voltage + 1.){
                    SetBit(Warning_error_code, Pack_Over_Operating);
                }
            }
            if (SCI_CCCV_CP_CR_flag == 5){
                if (fabsf(Idiff_ref_max) >= Output_Max_Current + 1.){
                    SetBit(Warning_error_code, Ibat_Over_Operating);
                }
            }
            if (SCI_CCCV_CP_CR_flag == 7){
                if (Vbat_CV_Volt >= Output_Max_Voltage + 1.){
                    SetBit(Warning_error_code, Pack_Over_Operating);
                }
            }
            if (Vpack <= 5.){
                if (Pack_Fault_cnt++ > 10){
                    SetBit(Warning_error_code, Pack_contact_Fault);
                }
            }
            else if (Pack_Fault_cnt > 0)
                Pack_Fault_cnt--;

            if (fabsf(Vpack - Vbat) >= 5.){
                if (Pack_Fault_cnt2++ > 10){
                    SetBit(Warning_error_code, Pack_contact_Fault);
                }
            }
            else if (Pack_Fault_cnt2 > 0)
                Pack_Fault_cnt2 = 0;
        }
    }
}

// Formation Inverter 알람 재정리 및 추가 필요


void Grid_Error_check(void)
{
    if ((sys_mode > 0) && (sys_mode < 90))
    {
        if (fabsf(Ia_rms) > Igrid_over)
        {
            SetBit(Grid_error_code, IA_OverCurrent);
            Ia_Err_view = Ia_rms;
        }

        if (fabsf(Ib_rms) > Igrid_over)
        {
            SetBit(Grid_error_code, IB_OverCurrent);
            Ib_Err_view = Ib_rms;
        }

        if (fabsf(Ic_rms) > Igrid_over)
        {
            SetBit(Grid_error_code, IC_OverCurrent);
            Ic_Err_view = Ic_rms;
        }

        if ((Va_rms > Vgrid_over) || (Vb_rms > Vgrid_over))
        {
            if (Vgrid_over_fault_cnt++ > 20)
            {
                SetBit(Grid_error_code, Vgrid_OverVoltage);
                Va_Err_view = Va_rms;
                Vb_Err_view = Vb_rms;
            }
        }
        else if (Vgrid_over_fault_cnt > 0)
        {
            Vgrid_over_fault_cnt--;
        }

        if ((Va_rms < Vgrid_under) || (Vb_rms < Vgrid_under))
        {
            if (Vgrid_under_fault_cnt++ > 20)
            {
                SetBit(Grid_error_code, Vgrid_UnderVoltage);
            }
        }
        else if (Vgrid_under_fault_cnt > 0)
        {
            Vgrid_under_fault_cnt--;
        }

        if (Vqe > Vqe_over)
        {
            if (Vqe_over_fault_cnt++ > 1000)
            {
                SetBit(Grid_error_code, Vqe_Unbalance);//66us * 5000 = 0.33s
            }
        }
        else if (Vqe_over_fault_cnt > 0)
        {
            Vqe_over_fault_cnt--;
        }

        if (Vde > Vde_over)
        {
            if (Vde_over_fault_cnt++ > 1000)
            {
                SetBit(Grid_error_code, Vde_Unbalance);
            }
        }
        else if (Vde_over_fault_cnt > 0)
        {
            Vde_over_fault_cnt--;
        }
    }

    //if (Temp_INV_R < MOFET_Temp)
    if (Temp_INV_R >= MOFET_Temp)//DouB
    {
        SetBit(Grid_error_code, INV_MOSFET_Overtemp);
    }

    //if (Temp_H_TR < TR_Temp)
    if (Temp_H_TR >= TR_Temp)//DouB
    {
        SetBit(Grid_error_code, Heat_TR_Fault);
    }

    if (GpioDataRegs.GPBDAT.bit.GPIO52)
    {
        if (Fuse_AC_fault_cnt++ > REF_TIME(0.5))
        {
            SetBit(Grid_error_code, Fuse_AC_Fault);
        }
    }
    else if(Fuse_AC_fault_cnt > 0)Fuse_AC_fault_cnt = 0;

}

Uint32 cnt_vdc_ov = 0, cnt_vsrc_ov = 0;
void DC_Error_check(void)
{
    if (sys_mode > 0)
    {
        if (Vdc_rms > Vdc_over)
        {
            //if (cnt_vdc_ov++ > REF_TIME(0.25))
            //if (cnt_vdc_ov++ > 5)
            //{
                SetBit(DC_error_code, Vdc_OverVoltage);
                Vdc_Top_rms_View = Vdc_rms;
            //}
        }
        else
        {
            cnt_vdc_ov = 0;
        }

        if (sys_mode >= 6)
        {
            if (Vdc_rms <= Vdc_under)
            {
                SetBit(DC_error_code, Vdc_UnderVoltage);
                Vdc_Top_rms_View = Vdc_rms;
            }
        }

        if ((sys_mode >= 4) && (NoLoad_Input_Test == 0) && (SRC_comm_flag == 1))
        {
            if (Vsrc_rms > Vsrc_over)
            {
                //if (cnt_vsrc_ov++ > REF_TIME(0.25))
                //if (cnt_vsrc_ov++ > 5)
                //{
                    SetBit(DC_error_code, Vsrc_OverVoltage);
                    Vsrc_rms_View = Vsrc_rms;
                //}
            }
            else
            {
                cnt_vsrc_ov = 0;
            }

            if (Vsrc_rms <= Vsrc_under)
            {
                SetBit(DC_error_code, Vsrc_UnderVoltage);
                Vsrc_rms_View = Vsrc_rms;
            }
        }

        if (Vbat_rms > Vbat_over)
        {
            SetBit(DC_error_code, Vbat_OverVoltage);
            Vbat_Err_view = Vbat;
        }

        if (Vpack_rms > Vpack_over)
        {
            SetBit(DC_error_code, Vpack_OverVoltage);
            Vpack_Err_view = Vpack;
        }

        if (fabsf(Ibat) > Ibat_over)
        {
            if (Ibat_fault_cnt++ > 30){
                SetBit(DC_error_code, Ibat_OverCurrent);
                Ibat_Err_view = Ibat;
                Ibat_fault_cnt = 0;
            }
        }
        else if (Ibat_fault_cnt > 0) Ibat_fault_cnt--;


        //if(Temp_SRC_P < MOFET_Temp){
        if(Temp_SRC_P >= MOFET_Temp){//DouB
            SetBit(DC_error_code, SRC_MOSFET_Overtemp);
        }
        //if(Temp_SPC_S < MOFET_Temp){
        if(Temp_SPC_S >= MOFET_Temp){//DouB
            SetBit(DC_error_code, SRC_MOSFET_Overtemp);
        }
        //if(Temp_DC_T1 < MOFET_Temp){
        if(Temp_DC_T1 >= MOFET_Temp){//DouB
            SetBit(DC_error_code, DCDC_MOSFET_Overtemp);
        }
        //if(Temp_DC_T3 < MOFET_Temp){
        if(Temp_DC_T3 >= MOFET_Temp){
            SetBit(DC_error_code, DCDC_MOSFET_Overtemp);
        }
        //if(Temp_DC_B1 < MOFET_Temp){
        if(Temp_DC_B1 >= MOFET_Temp){
            SetBit(DC_error_code, DCDC_MOSFET_Overtemp);
        }
        //if(Temp_DC_B3 < MOFET_Temp){
        if(Temp_DC_B3 >= MOFET_Temp){
            SetBit(DC_error_code, DCDC_MOSFET_Overtemp);
        }
    }

    if (GpioDataRegs.GPBDAT.bit.GPIO51){
          if (Fuse_DC_fault_cnt++ > REF_TIME(0.5)){
              SetBit(DC_error_code, Fuse_DC_Fault);
          }
    }
    else if (Fuse_DC_fault_cnt > 0) Fuse_DC_fault_cnt--;


}

void Fault_Error_check(void){

    if((sys_mode >= 7)&&(NoLoad_Input_Test == 0)){
        if((Ifan_rms <= 0.2)||(Ifan_rms >= 6.)){
            if(Heat_Fan_fault_cnt++ > REF_TIME(5.)) SetBit(Fault_error_code, Heat_Fan_Fault);
        }
        else if(Heat_Fan_fault_cnt >0) Heat_Fan_fault_cnt = 0;
    }
    else{
        Heat_Fan_fault_cnt =0;
    }

    if (comm_chk_err_flag == 1){
        SetBit(Fault_error_code, comm_chk_Fault);
    }
}

void IGBT_Error_check(void){
    if (sys_mode > 0){
        if (sys_mode < 3){
            if (GpioDataRegs.GPADAT.bit.GPIO24){
                if (Driver1_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_R);
            }
            else if (Driver1_fault_cnt > 0) Driver1_fault_cnt = 0;

            if (GpioDataRegs.GPADAT.bit.GPIO25){
                if (Driver2_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_S);
            }
            else if (Driver2_fault_cnt > 0) Driver2_fault_cnt = 0;

            if (GpioDataRegs.GPADAT.bit.GPIO26){
                if (Driver3_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_T);
            }
            else if (Driver3_fault_cnt > 0) Driver3_fault_cnt = 0;

            if (GpioDataRegs.GPADAT.bit.GPIO16){
                if (Driver4_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_SRCP);
            }
            else if (Driver4_fault_cnt > 0)Driver4_fault_cnt = 0;

            if (GpioDataRegs.GPADAT.bit.GPIO17){
                if (Driver5_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_SRCS);
            }
            else if (Driver5_fault_cnt > 0) Driver5_fault_cnt = 0;

            if (GpioDataRegs.GPADAT.bit.GPIO27){
                if (DriverA_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_DCDC_T);
            }
            else if (DriverA_fault_cnt > 0) DriverA_fault_cnt = 0;

            if (GpioDataRegs.GPBDAT.bit.GPIO58){
                if (DriverB_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_DCDC_B);
            }
            else if (DriverB_fault_cnt > 0) DriverB_fault_cnt = 0;

//            if (GpioDataRegs.GPBDAT.bit.GPIO59){
//                if (DriverC_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code, Drive_Fault_C);
//            }
//            else if (DriverC_fault_cnt > 0) DriverC_fault_cnt = 0;
//
//          if(GpioDataRegs.GPBDAT.bit.GPIO42){
//              if(DriverD_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code,Drive_Fault_D);
//          }
//          else if(DriverD_fault_cnt >0) DriverD_fault_cnt =0;
//
//          if(GpioDataRegs.GPBDAT.bit.GPIO36){
//              if(DriverE_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code,Drive_Fault_E);
//          }
//          else if(DriverE_fault_cnt >0) DriverE_fault_cnt =0;
//
//          if(GpioDataRegs.GPBDAT.bit.GPIO33){
//              if(DriverF_fault_cnt++ > REF_TIME(flt_T)) SetBit(IGBT_error_code,Drive_Fault_F);
//          }
//          else if(DriverF_fault_cnt >0) DriverF_fault_cnt =0;
        }
        else{
            if (GpioDataRegs.GPADAT.bit.GPIO24){
                SetBit(IGBT_error_code, Drive_Fault_R);
            }
            if (GpioDataRegs.GPADAT.bit.GPIO25){
                SetBit(IGBT_error_code, Drive_Fault_S);
            }
            if (GpioDataRegs.GPADAT.bit.GPIO26){
                SetBit(IGBT_error_code, Drive_Fault_T);
            }

            if (GpioDataRegs.GPADAT.bit.GPIO16){
                SetBit(IGBT_error_code, Drive_Fault_SRCP);
            }
            if (GpioDataRegs.GPADAT.bit.GPIO17){
                SetBit(IGBT_error_code, Drive_Fault_SRCS);
            }

            if (GpioDataRegs.GPADAT.bit.GPIO27){
                SetBit(IGBT_error_code, Drive_Fault_DCDC_T);
            }
            if (GpioDataRegs.GPBDAT.bit.GPIO58){
                SetBit(IGBT_error_code, Drive_Fault_DCDC_B);
            }
//            if (GpioDataRegs.GPBDAT.bit.GPIO59){
//                SetBit(IGBT_error_code, Drive_Fault_C);
//            }

//          if(GpioDataRegs.GPBDAT.bit.GPIO42){ SetBit(IGBT_error_code,Drive_Fault_D); }
//          if(GpioDataRegs.GPBDAT.bit.GPIO36){ SetBit(IGBT_error_code,Drive_Fault_E); }
//          if(GpioDataRegs.GPBDAT.bit.GPIO33){ SetBit(IGBT_error_code,Drive_Fault_F); }

            Driver1_fault_cnt = 0; Driver2_fault_cnt = 0; Driver3_fault_cnt = 0;
            Driver4_fault_cnt = 0; Driver5_fault_cnt = 0; Driver6_fault_cnt = 0;
            DriverA_fault_cnt = 0; DriverB_fault_cnt = 0; DriverC_fault_cnt = 0;
            DriverD_fault_cnt = 0; DriverE_fault_cnt = 0; DriverF_fault_cnt = 0;
        }
    }
}

void All_MC_off(void)
{
    Grid_SOFT_MC_OFF();
    Grid_MAIN_MC_OFF();
    CHAR_DC_SOFT_MC_OFF();
    CHAR_DC_MAIN_MC_OFF();
    CHAR_DC_OUT_MC_OFF();
    CHAR_DC_OUT_Parall_MC_OFF();
    CV_Resistor_OFF();
    CV_OFF_Resistor_OFF();
}

void Emergerncy_check(void)
{
    if (Fault_check_start == 1)
    {
//        if (GpioDataRegs.GPCDAT.bit.GPIO89)       // 22.03.23 Formation Inverter  EMO NC 기준
        if (GpioDataRegs.GPCDAT.bit.GPIO89 != 1)    // 22.03.23 Formation Inverter  EMO NO 기준
        {
            SetBit(Fault_error_code, Emergency_Stop);
        }
    }
}




