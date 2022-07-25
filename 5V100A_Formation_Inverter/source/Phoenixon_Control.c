#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"


//	Converter Parameter
float Vo_peak = 0., Iodss = 0., Ioqss = 0., Iodrs = 0., Ioqrs = 0., AC_Pow = 0;
float Vodc_integ = 0., Ioqrs_integ = 0., Iodrs_integ = 0.;
//float Vodc_Kp=1., Vodc_Ki=10., Io_Kp=1.5, Io_Ki=25., Vlimit = 0., Vdc_ref_step = 0.005, Vlimit_step = 0.5;//200128 old version
float Vodc_Kp = 2.5, Vodc_Ki = 15., Io_Kp = 1.0, Io_Ki = 50., Vlimit = 0., Vdc_ref_step = 0.005, Vlimit_step = 0.5;//DouB @420uH AC's inductor tune
float Iodrs_ref = 0., Ioqrs_ref = 0., Vodre = 0., Voqre = 0., Vodrep = 0., Voqrep = 0.;
float Idrs_ref = 0., Iqrs_ref = 0., Ioqrs_ref_offset = 0.;
float Iodrs_ff = 0., Ioqrs_ff = 0., Vodse = 0., Voqse = 0.;
float Voas = 0., Vobs = 0., Vocs = 0.;

//	Inverter Parameter
int Iqrs_offset_init_flag = 0;
float V_peak = 0., Idss = 0., Iqss = 0., Idrs = 0., Iqrs = 0., Ia_inv = 0., Ib_inv = 0., Ic_inv = 0.;
//float Iqrs_Kp = 1.5, Iqrs_Ki = 10., I_Kp = 1.5, I_Ki = 25.;//200128 old version
float Iqrs_Kp = 2.5, Iqrs_Ki = 15., I_Kp = 1.0, I_Ki = 50.;//DouB @420uH AC's inductor tune
float Iqrs_inv_integ = 0., Iqrs_ref_offt = 0., Iqrs_ref_PI = 0., Iqrs_ref_step = 0.0005;  //Iqrs_ref_step=0.0001;
float Iqrs_integ = 0., Idrs_integ = 0., Iqrs_ref_min = -5, Iqrs_ref_offset = 0., Iqrs_offset_const = 0.0016; // Iqrs_ref_min = -2, Iqrs_offset_const=0.0016;
float Vdre = 0., Vqre = 0., Vdrep = 0., Vqrep = 0.;
float Idrs_ff = 0., Iqrs_ff = 0., Vdse = 0., Vqse = 0.;
float Vas = 0., Vbs = 0., Vcs = 0.;

// Converter & Inverter Common Parameter
int Bidirection_flag = 0, Bidirection_mode = 0;
float Vdc_ref = 0., Vdc_ref_lim_Con = 30., Vdc_ref_lim_Inv = 50.; //Vdc_ref_lim_Con = 10., Vdc_ref_lim_Inv = 30 -> 50.; // 충전 후 Standy에서 모드 변경으로 FET 폴트 발생하여 값 변경
float Toa = 0., Tob = 0., Toc = 0., Ta = 0., Tb = 0., Tc = 0.;
float Toga = 0., Togb = 0., Togc = 0., Tga = 0., Tgb = 0., Tgc = 0.;
float Tomax = 0., Tomin = 0., Toe = 0., Too = 0., Tooff = 0.;
float Tmax = 0., Tmin = 0., Te = 0., To = 0., Toff = 0.;

//	SRC Parameter
int SRC_start_flag = 0, SRC_mode = 0, SRC_comm_flag = 0., SRC_Gate_int = 0, SRC_HR_Gate = 0;
float PWM_SRC_DT = 70., SRC_Gate = 0., SRC_temp = 0., SRC_Duty_half = 2222, SRC_Duty_step = 0.04;

//Differential CONTROL
// Common Parameter
int Differ_On_flag = 0, Differ_mode = 0, Diff_Gate_int = 0, Diff_HR_Gate = 0, Vbat_CV_cnt = 0;
float HR_temp = 0., PWM_Cut = 100., Idiff_ref_Cnt = 0., Idiff_ref_max = 0.;
float Diff_Duty = 0., Diff_Duty_add = 0., Diff_Init_Gate = 0.;
// CC Control
float Diff_Gate = 0., Idiff_limit_max = 0.;
//float Idiff_ref = 0., Idiff_integ = 0., Idiff_integ_val = 0., Idiff_Kp = 7.0, Idiff_Ki = 8000;
float Idiff_ref = 0., Idiff_integ = 0., Idiff_integ_val = 0., Idiff_Kp = 7.0, Idiff_Ki = 3000; // LGES 21LH528 50V50A

//CV Control
float Vdiff_CV_ref = 0., Vdiff_CV_integ = 0.;
//float Vdiff_CV_Kp = 0.25, Vdiff_CV_Ki = 30.;
float Vdiff_CV_Kp = 3.0, Vdiff_CV_Ki = 300.;//200604 removing CV overshoot
//float Vdiff_CV_Kp = 1.25, Vdiff_CV_Ki = 225.;//200609 removing CV overshoot
//float Vdiff_CV_Kp = 1., Vdiff_CV_Ki = 50.;
float Idiff_CV_ref = 0., Idiff_CV_integ = 0.;
//float Idiff_CV_Kp = 2.0, Idiff_CV_Ki = 3000.;
float Idiff_CV_Kp = 7.0, Idiff_CV_Ki = 8000.;//200604 removing CV overshoot
//float Idiff_CV_Kp = 3.75, Idiff_CV_Ki = 4000.;//200609 removing CV overshoot
float Idiff_ref_err = 0., Idiff_ref_p1 = 0., PWM1_CV_MAX = 0., PWM2_CV_MIN = 0., PWM_CV_CUT = 5.;

//Initial CV CONTROL
float Iint_CV_ref = 0., Vint_ref_Cnt = 0;
float Vint_CV_Kp = 2.5, Vint_CV_Ki = 30., Vint_CV_integ = 0., Iint_limit_max = 1.;
float Iint_CV_Kp = 3., Iint_CV_Ki = 700., Iint_CV_integ = 0.;
float Vpack_fix = 0., Vint_step_set = 0., Vint_step = 0., Step_int = 0.000091;

// Current change Control
//  4ms/91us=44->0.022, 5ms/91us=55->0.018, 8ms/91us=88->0.011, 9ms/91us=99->0.0101, 10ms/91us=109->0.0091, 18ms/91us=198->0.0051
// 50ms/91us=550->0.00181, 100ms/91us=1099->0.00091, 500ms/91us=5495->0.000181, 1s/91us=10989->0.000091, 5s/91us=55555->0.000018, 10s/91us=111111->0.0000091
int Err_Curr_flag = 0, Diff_Time_flag = 0, Curr_Change_flag = 0, Init_Stat_flag = 0, Cha_Dis_Start_flag=0;
float Rig_per = 0., Rig_per_F = 1.05, Rig_per_R = 0.95, Idiff_step = 0, Idiff_step2 = 0.01;
float Idiff_ref_View = 0., Diff_Change_Time = 0.;

//etc
int Rising_View = 0, Rising_cnt = 0., Diff_PI_test = 0, Idiff_ref_p1_cnt = 0, Standby_cnt = 0, SCI_cccv_Cnfim = 0;
unsigned long SCI_cccv_Cnt = 0;
float Ibat_max = 0., Rising_ms = 0., Ibat_lim = 0., Rig_per_View = 0.95, Vpack_init_max = -0.5, Vsen_Read=0.;

Uint16 delay_cnt_src = 0;//DouB

void Bidirection_control(void)
{
    AC_Pow = 1.732 * Va_rms * Ia_rms;

    //check a charging condition ; VDC is LOW
//    if (Vdc <= Vdc_ref_Max - Vdc_ref_lim_Con){//600 - 30
//        Bidirection_mode = 0;
//        if (Bidirection_flag == 1){
//            Bidirection_mode = 1;
//        }
//        if (Bidirection_flag == 2){
//            Bidirection_mode = 2;
//        }
//    }

//    //check a discharging condition ; VDC is HIGH
////	if((Vdc >= Vdc_ref_Max + Vdc_ref_lim_Inv)||(Boost1_On_flag ==1)){
//    if (Vdc >= Vdc_ref_Max + Vdc_ref_lim_Inv){//600 + 30
//        Bidirection_mode = 3;
//        if (Bidirection_flag == 4){
//            Bidirection_mode = 4;
//        }
//        if (Bidirection_flag == 5){
//            Bidirection_mode = 5;
//        }
//    }

    switch (Bidirection_mode)
    {//inverter controller
    case 0:
        //Converter control reset
        PWM_AC_Block();

        Voqre = 0.;
        Vodre = 0.;
        Ioqrs_ref = 0.;
        Iodrs_ref = 0.;

        delay_cnt_src = 0;

        Vodc_integ = 0.;
        Ioqrs_integ = 0.;
        Iodrs_integ = 0.;

        Vlimit = 0.;

        Bidirection_mode = 1;
        Bidirection_flag = 1;
        break;

    case 1:
        //PWM_reset
        PWM_half = (PWM_MAX_INV * 0.5);
        Toga = PWM_half;
        Togb = PWM_half;
        Togc = PWM_half;
        Tga = PWM_half;
        Tgb = PWM_half;
        Tgc = PWM_half;
        EPwm10Regs.CMPA.bit.CMPA = (int)PWM_half;
        EPwm11Regs.CMPA.bit.CMPA = (int)PWM_half;
        EPwm12Regs.CMPA.bit.CMPA = (int)PWM_half;

        PWM_AC_Open();//pwm enable

        Bidirection_mode = 2;
        Bidirection_flag = 2;
        break;

    case 2:
        Vo_peak = Grid_Vline * root2_inv_root3;

        //stationary
        Iodss = ((2. * Ia) - Ib - Ic) * inv3;//amplitude of current (==sin)
        Ioqss = (out_dir * inv_root3) * (Ia + (2. * Ib));//(==cos)

        //rotating
        Iodrs = (cos_pll * Iodss) + (sin_pll * Ioqss);
        Ioqrs = (cos_pll * Ioqss) - (sin_pll * Iodss);//amplitude of current

        //***** Algorithm #02 Start *****//
        //controller for DC Link voltage ; 600VDC
        DPICON(Ioqrs_ref, Vdc_ref, Vdc, Vodc_integ, Vodc_Kp, Vodc_Ki, Tsamp, Iqrs_max);//voltage control
        MaxMin(Ioqrs_ref, Vlimit);
        //***** Algorithm #02 End  *****//


        {//controller for a grid current
            DPICON(Voqrep, Ioqrs_ref, Ioqrs, Ioqrs_integ, Io_Kp, Io_Ki, Tsamp, PWM_MAX_INV);//current control
            Ioqrs_ff = (_2PI60 * AC_Reactor * Iodrs);
            Voqre = -(Vo_peak - (Voqrep + Ioqrs_ff));

            DPICON(Vodrep, Iodrs_ref, Iodrs, Iodrs_integ, Io_Kp, Io_Ki, Tsamp, PWM_MAX_INV);
            Iodrs_ff = (_2PI60 * AC_Reactor * Ioqrs);
            Vodre = -(-Vodrep + Iodrs_ff);
        }

        Vodse = (cos_pll * Vodre) - (sin_pll * Voqre);
        Voqse = (sin_pll * Vodre) + (cos_pll * Voqre);

        Voas = Vodse;
        Vobs = 0.5 * (((out_dir * root3) * Voqse) - Vodse);
        Vocs = -(Voas + Vobs);

        //SVPWM(Voas, Vobs, Vocs, PWM_MAX_INV, Vdc, Toa, Tob, Toc, Tomax, Tomin, Toe, Too, Tooff, Toga, Togb, Togc);
        SVPWM(Voas, Vobs, Vocs, PWM_MAX_INV, Vdc_rms, Toa, Tob, Toc, Tomax, Tomin, Toe, Too, Tooff, Toga, Togb, Togc);

        if ((PWM_MAX_INV - 5) < Toga) { Toga = (PWM_MAX_INV - 5); }
        if ((PWM_MAX_INV - 5) < Togb) { Togb = (PWM_MAX_INV - 5); }
        if ((PWM_MAX_INV - 5) < Togc) { Togc = (PWM_MAX_INV - 5); }

        EPwm10Regs.CMPA.bit.CMPA = (int)Toga;
        EPwm11Regs.CMPA.bit.CMPA = (int)Togb;
        EPwm12Regs.CMPA.bit.CMPA = (int)Togc;


        if (Vdc_ref < (Vdc_ref_Max - 0.1))  { Vdc_ref += Vdc_ref_step; }//0.001(4s)
        else                                { Vdc_ref = Vdc_ref_Max; }




        //***** Algorithm #02 Start *****//
        if (Vlimit < Iqrs_max)
            Vlimit += Vlimit_step; //0.05->0.005 -> 0.5 Very important
        //***** Algorithm #02 End   *****//


        if (NoLoad_Input_Test == 0)
        {
            if (SRC_start_flag == 0)
            {
                //if (Vdc >= Vdc_ref_Max - 5.)
                //if (Vdc_rms >= Vdc_ref_Max - 2.)//DouB 200313 fixed SRC under voltage trip
                if (Vdc_rms >= Vdc_ref_Max - 5.)//DouB 200509
                {
                    delay_cnt_src++;
                    //if (delay_cnt_src >= REF_TIME(3.))
                    if (delay_cnt_src >= REF_TIME(1.5))
                    {
                        delay_cnt_src = 0;
                        SRC_start_flag = 1;
                    }
                }
			}
        }
        else if (NoLoad_Input_Test == 2)
        {
			if (SRC_start_flag == 1)
			{
				SRC_start_flag = 1;
			}
		}
        break;

    case 3:
        //Inverter control reset
        PWM_AC_Block();

        Vqre = 0.;
        Vdre = 0.;
        Iqrs_ref = 0.;
        Idrs_ref = 0.;

        Iqrs_integ = 0.;
        Idrs_integ = 0.;
        Iqrs_inv_integ = 0.;

        Iqrs_offset_init_flag = 0;

        Bidirection_mode = 4;
        Bidirection_flag = 4;
        break;

    case 4:
        //PWM_reset
        PWM_half = (PWM_MAX_INV * 0.5);
        EPwm10Regs.CMPA.bit.CMPA = (int)PWM_half;
        EPwm11Regs.CMPA.bit.CMPA = (int)PWM_half;
        EPwm12Regs.CMPA.bit.CMPA = (int)PWM_half;

        PWM_AC_Open();//pwm enable

        Bidirection_mode = 5;
        Bidirection_flag = 5;

        break;

    case 5:
        V_peak = -Grid_Vline * root2_inv_root3;

        Ia_inv = -Ia;
        Ib_inv = -Ib;
        Ic_inv = -Ic;

        //stationary
        Idss = ((2. * Ia_inv) - Ib_inv - Ic_inv) * inv3;//amplitude of current (==sin)
        Iqss = (out_dir * inv_root3) * (Ia_inv + (2. * Ib_inv));//(==cos)

        //rotating
        Idrs = (cos_pll * Idss) + (sin_pll * Iqss);
        Iqrs = (cos_pll * Iqss) - (sin_pll * Idss);//amplitude of current

        //*************** Regenerating Algorithm #01 *****************//
        if (Iqrs_offset_init_flag == 0)
        {         //Vpack_rms
            Iqrs_ref_offset = fabsf(Idiff_ref_max) * (Vpack_rms * Iqrs_offset_const);// 0.3/70V = 0.0043 add 110531 Low Battery Voltage, Low Iqrs
            if (Iqrs_ref_offset <= 0)
            {
                Iqrs_ref_offset = 0.;
            }
            Iqrs_offset_init_flag = 1;

            Iqrs_ref = Iqrs_ref_offset;
            Iqrs_inv_integ = Iqrs_ref_offset;
        }

        //controller for DC Link voltage ; 600VDC
        DPICON4(Iqrs_ref, Vdc_ref_Max, Vdc, Iqrs_inv_integ, Iqrs_Kp, Iqrs_Ki, Tsamp, Iqrs_max);//voltage control

        if (Iqrs_ref <= Iqrs_ref_min)
        {
            Iqrs_ref = Iqrs_ref_min;
            Iqrs_inv_integ = Iqrs_ref_min;
        }

        if (Vdc <= Vdc_ref_Max - (Vdc_ref_lim_Con * 0.5))
        {//검토필요
            Iqrs_inv_integ -= Iqrs_ref_step;
        }

        {//controller for a grid current
            DPICON(Vqrep, Iqrs_ref, Iqrs, Iqrs_integ, I_Kp, I_Ki, Tsamp, PWM_MAX_INV);//current control
            Iqrs_ff = (_2PI60 * AC_Reactor * Idrs);
            Vqre = (V_peak - (Vqrep + Iqrs_ff));

            DPICON(Vdrep, Idrs_ref, Idrs, Idrs_integ, I_Kp, I_Ki, Tsamp, PWM_MAX_INV);
            Idrs_ff = (_2PI60 * AC_Reactor * Iqrs);
            Vdre = (-Vdrep + Idrs_ff);
        }

        Vdse = (cos_pll * Vdre) - (sin_pll * Vqre);
        Vqse = (sin_pll * Vdre) + (cos_pll * Vqre);

        Vas = Vdse;
        Vbs = 0.5 * (((out_dir * root3) * Vqse) - Vdse);
        Vcs = -(Vas + Vbs);

        //SVPWM(Vas, Vbs, Vcs, PWM_MAX_INV, Vdc, Ta, Tb, Tc, Tmax, Tmin, Te, To, Toff, Tga, Tgb, Tgc);
        SVPWM(Vas, Vbs, Vcs, PWM_MAX_INV, Vdc_rms, Ta, Tb, Tc, Tmax, Tmin, Te, To, Toff, Tga, Tgb, Tgc);

        if ((PWM_MAX_INV - 5) < Tga) { Tga = (PWM_MAX_INV - 5); }
        if ((PWM_MAX_INV - 5) < Tgb) { Tgb = (PWM_MAX_INV - 5); }
        if ((PWM_MAX_INV - 5) < Tgc) { Tgc = (PWM_MAX_INV - 5); }

        EPwm10Regs.CMPA.bit.CMPA = (int)Tga;
        EPwm11Regs.CMPA.bit.CMPA = (int)Tgb;
        EPwm12Regs.CMPA.bit.CMPA = (int)Tgc;
        break;

    default:
        break;
    }
}

Uint16 delay_cnt_dcdc = 0;//DouB

void SRC_control(void)
{
    switch (SRC_mode){
    case 0:
        PWM_SRC_Block();

        SRC_Gate = 0.;
        SRC_mode = 1;
        SRC_Duty_half = PWM_MAX_SRC- PWM_SRC_DT;//PWM_SRC_DT 100_0.5us@22.22us  200_1.0us@22.22us
        SRC_Duty_step = SRC_Duty_half / (3.5f / Tsamp);//make to "seconds" ; DouB

        delay_cnt_dcdc = 0;//DouB
        break;

    case 1:
        PWM_SRC_Open();
        SRC_mode = 2;
        break;

    case 2:
        SRC_Gate_int = (int) (SRC_Gate);

        EPwm7Regs.CMPA.bit.CMPA = (int)(SRC_Gate_int);
        EPwm7Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - SRC_Gate_int);//DouB

        EPwm8Regs.CMPA.bit.CMPA = (int)(SRC_Gate_int);
        EPwm8Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - SRC_Gate_int);//DouB

        if (SRC_Gate < SRC_Duty_half)   { SRC_Gate += SRC_Duty_step; }
        else                            { SRC_Gate = SRC_Duty_half; }

		if(NoLoad_Input_Test == 0){
		    if (Vsrc_rms >= 150.){//DouB ; SRC under voltage trip
		        delay_cnt_dcdc++;
		        if (delay_cnt_dcdc >= REF_TIME(2.))
		        {
		            delay_cnt_dcdc = 0;
		            SRC_comm_flag = 1;
		        }
			}
		}
		else if(NoLoad_Input_Test >= 2){
			if(SRC_comm_flag == 1){
				SRC_comm_flag = 1;
			}
		}
        break;
    }
}


void Differ_control(void)
{
    switch (Differ_mode)
    {
    case 0:
        //Reference Setting
        Idiff_ref = Idiff_ref_max;
        Vdiff_CV_ref = Vbat_CV_Volt;
        PWM_Cut = PWM_MAX_BB * 0.01;
        Vsen_Read = Vbat;

        //PWM Initialization
        Diff_Gate = 0.;
        Idiff_integ = 0.;
        Vdiff_CV_integ = 0.;
        Idiff_CV_integ = 0.;

        Iint_CV_ref = 0;
        Vint_CV_integ = 0.;
        Iint_CV_integ = 0.;

        Mode_I_change_flag = 0;
        Mode_V_change_flag = 0;

        //Current Change Initialization
        Curr_Change_flag = 1;
        Init_Stat_flag = 0;
        Err_Curr_flag = 0;
        Diff_Time_flag = 0;
        Idiff_ref_Cnt = 0.;
        Idiff_ref_err = 0;
        Idiff_ref_p1 = 0;

        //Etc Initialization
        Cha_Dis_Start_flag =0;
        Standby_cnt =0;

        Vint_ref_Cnt = Vbat_rms;
        Vpack_fix = Vpack_UI_rms;
        Iint_limit_max = 1.;

        if ((Vdc_rms >= Vdc_init_max) && (Vsrc_rms >= Vsrc_init_max) && (Vbat_rms >= Vpack_init_max))
        {
        	if (Vbat_rms <= 0.) Vbat_rms = 0.1;

            Diff_Duty = (Vsrc_rms + Vbat_rms) / (2 * Vsrc_rms);
            Diff_Init_Gate = (Diff_Duty * PWM_MAX_BB) + Diff_Duty_add;
            Diff_Gate = Iint_CV_integ = Diff_Init_Gate;

            Vint_step_set = (Vpack_UI_rms - Vbat_rms) * Step_int; //0.000091
            Vint_step = Vint_step_set;

            HRPWM_Open();

            Vpack_Range_PI_Ctrl();

            Differ_mode = 1;
        }
        break;

    case 1: //* Initial CV Control Start *//  Differ_mode => 1 : Initial CV Mode

        if (SCI_cccv_Cnfim == 0){
            if (++SCI_cccv_Cnt >= REF_TIME(3.)){
                SCI_cccv_Cnfim = 1;
                SCI_cccv_Cnt = 0;
            }
        }
        else{
            SCI_TX_cccv = 2;  //Standby
        }

        //precharging controller before DCMC ON
		DPICON(Iint_CV_ref, Vint_ref_Cnt, Vsen_Read, Vint_CV_integ, Vint_CV_Kp, Vint_CV_Ki, Tsamp, Iint_limit_max);
		DPICON2(Diff_Gate, Iint_CV_ref, Ibat, Iint_CV_integ, Iint_CV_Kp,Iint_CV_Ki, Tsamp, (PWM_MAX_BB - PWM_Cut));

        if (Cha_Dis_Start_flag == 0)
        {
        	Vsen_Read = Vbat;
			if (Vint_ref_Cnt <= Vpack_fix)  { Vint_ref_Cnt += Vint_step; }
			else                            { Vint_ref_Cnt -= Vint_step; }

			if (Vbat >= Vpack_UI_rms * 0.99)    { Vint_step = 0.01; }
        }
        else
        {
        	Vsen_Read = Vpack;
        	Vint_ref_Cnt = Vpack_UI_rms;
        }

        if (Diff_Gate < 10) Diff_Gate = 0.;   // 0 -> 10
        Diff_Gate_int = (int) (Diff_Gate);
        HR_temp = (Diff_Gate - Diff_Gate_int);
        Diff_HR_Gate = ((int) (HR_temp * MEP_ScaleFactor + 1.5) << 8); //Shift CMPAHR High bit
        //Diff_HR_Gate += 0x0180;//중복느낌... DouB

        EPwm1Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm1Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm1Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm2Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm2Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm2Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm3Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm3Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm3Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm4Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm4Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm4Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm5Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm5Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm5Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm6Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm6Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm6Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        if ((SCI_Charge_flag == 1) || (SCI_Discharge_flag == 1))
        {

            //*******  CC Control Setting Start *******//
            if ((SCI_CCCV_CP_CR_flag == 3) || (SCI_CCCV_CP_CR_flag == 5))
            {   //CC
                if (SCI_Voltage_ref >= 0.001)
                {
                    if ((SCI_Charge_flag == 1) && (SCI_Current_ref >= 0.001))
                    {
                        if (SCI_Voltage_ref < Vpack_UI_rms)
                        {
                            SetBit(Warning_error_code, Charge_CV_Under_Fault);
                        }
                        else
                        {
                            Idiff_integ = Iint_CV_integ;

                            Vbat_CV_Volt = SCI_Voltage_ref; //190915 important
                            Vdiff_CV_ref = Vbat_CV_Volt;    //190915 important

                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = SCI_Voltage_ref + Vpack_over_add;

                            if (Vbat_over >= Vbat_over_max)
                            {
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }

                            // Mini Scope Reset
                            Rising_View = 0;
                            Rising_cnt = 0;
                            Rising_ms = 0.;
                            Ibat_max = 0.;
                            Ibat_lim = 0.;

                            Init_Stat_flag = 0;
                            Differ_mode = 2;

                        }
                    }

                    else if ((SCI_Discharge_flag == 1) && (SCI_Current_ref <= -0.001))
                    {
                        if (SCI_Voltage_ref > Vpack_UI_rms)
                        {
                            SetBit(Warning_error_code, Discharge_CV_Over_Fault);
                        }
                        else
                        {
                            Idiff_integ = Iint_CV_integ;

                            Vbat_CV_Volt = SCI_Voltage_ref; //190915 important
                            Vdiff_CV_ref = Vbat_CV_Volt;    //190915 important

                            Vbat_over = Vbat_over_max;
                            Vpack_over = Vpack_over_max;
                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem)
                            {
                                Discha_End_Volt = Discha_End_rem;
                            }

                            // Mini Scope Reset
                            Rising_View = 0;
                            Rising_cnt = 0;
                            Rising_ms = 0.;
                            Ibat_max = 0.;
                            Ibat_lim = 0.;

                            Init_Stat_flag = 0;
                            Differ_mode = 2;
                        }
                    }
                }
            }
            //*******  CC Control Setting End   *******//

            //*******  CP Control Setting Start *******//
            if ((SCI_CCCV_CP_CR_flag == 7) || (SCI_CCCV_CP_CR_flag == 8))
            {   //CP
                if (SCI_Voltage_ref >= 0.001)
                {
                    if ((SCI_Charge_flag == 1) && (SCI_Current_ref >= 0.001))
                    {

                        if (SCI_Voltage_ref < Vpack_UI_rms)
                        {
                            SetBit(Warning_error_code, Charge_CV_Under_Fault);
                        }
                        else
                        {
                            Idiff_integ = Iint_CV_integ;

                            Vbat_CV_Volt = SCI_Voltage_ref; //190915 important
                            Vdiff_CV_ref = Vbat_CV_Volt;    //190915 important

                            Vbat_over = SCI_Voltage_ref + Vbat_over_add;
                            Vpack_over = SCI_Voltage_ref + Vpack_over_add;
                            if (Vbat_over >= Vbat_over_max)
                            {
                                Vbat_over = Vbat_over_max;
                                Vpack_over = Vpack_over_max;
                            }
                            // Mini Scope Reset
                            Rising_View = 0;
                            Rising_cnt = 0;
                            Rising_ms = 0.;
                            Ibat_max = 0.;
                            Ibat_lim = 0.;

                            Init_Stat_flag = 0;
                            Differ_mode = 4;
                        }
                    }

                    else if ((SCI_Discharge_flag == 1) && (SCI_Current_ref >= 0.001)){
                        if (SCI_Voltage_ref > Vpack_UI_rms)
                        {
                            SetBit(Warning_error_code, Discharge_CV_Over_Fault);
                        }
                        else
                        {
                            Idiff_integ = Iint_CV_integ;

                            Vbat_CV_Volt = SCI_Voltage_ref; //190915 important
                            Vdiff_CV_ref = Vbat_CV_Volt;    //190915 important

                            Vbat_over = Vbat_over_max;
                            Vpack_over = Vpack_over_max;
                            Discha_End_Volt = SCI_Voltage_ref - Discha_End_rem;
                            if (Discha_End_Volt <= Discha_End_rem)
                                Discha_End_Volt = Discha_End_rem;

                            // Mini Scope Reset
                            Rising_View = 0;
                            Rising_cnt = 0;
                            Rising_ms = 0.;
                            Ibat_max = 0.;
                            Ibat_lim = 0.;

                            Init_Stat_flag = 0;
                            Differ_mode = 4;
                        }
                    }
                }
            }
            //*******  CP Control Setting End *******//
        }

        break; //* Initial CV Control END *//

    case 2:  //* CC  Control    Start *//  Differ_mode => 2 : CC Mode

        Cha_Dis_Start_flag =1;
        if (SCI_Charge_flag == 1)
        {
            SCI_TX_cccv = 3;  // Charge CC
        }
        else if (SCI_Discharge_flag == 1)
        {
            SCI_TX_cccv = 5; // Discharge CC
        }

        Vpack_Range_PI_Ctrl();

        //******   Mini Scope(Ibat_max, Rising_Time)  ******//

        if (Ibat > Ibat_max)
        {// only_view
            Ibat_max = Ibat;
        }

        if (Rising_View == 0)
        {
            Rising_cnt++;
            if (fabsf(Ibat) >= fabsf(Idiff_ref * Rig_per_View))//0.95
            {
                Rising_View = 1;
                Rising_ms = Rising_cnt * 0.11;
            }
            if (Rising_cnt >= 454)//454 -> 50ms
            {
                Rising_View = 1;
                Rising_ms = Rising_cnt * 0.11;
            }
        }

        //******      Ibat Limit Set      *******//
        Ibat_lim = Output_Power_kW / fabsf(Vpack_UI_rms);

        if (Ibat_lim >= Output_Max_Current)
        {
            Ibat_lim = Output_Max_Current;
        }

        if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 3))
        {
            if (Idiff_ref >= Ibat_lim)
            {
                Idiff_ref = Ibat_lim;
            }
        }
        else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 5))
        {
            if (Idiff_ref <= -Ibat_lim)
            {
                Idiff_ref = -Ibat_lim;
            }
        }

        //******   Current Change Control  ******//
        if (Curr_Change_flag == 1)      { Err_Curr_flag = 1; }// 위치 확인 필요    case 1
        else                            { Err_Curr_flag = 0; }


        if (Err_Curr_flag == 1)
        {
            if (Diff_Time_flag == 0)
            {
                if (Init_Stat_flag == 1)
                {
                    if ((Idiff_ref >= 0) && (Idiff_ref_p1 >= 0))
                    {
                        Diff_Change_Time = Step_R_F_ms;
                    }
                    else if ((Idiff_ref < 0) && (Idiff_ref_p1 < 0))
                    {
                        Diff_Change_Time = Step_R_F_ms;
                    }
                    else
                    {
                        Diff_Change_Time = Step_TR_ms;
                    }
                }
                else
                {
                    Diff_Change_Time = Step_R_F_ms;
                    Init_Stat_flag = 1;
                }

                Idiff_ref_err = fabsf(Idiff_ref - Idiff_ref_p1);
                if (Idiff_ref_err <= 0.1)
                {
                    Idiff_ref_err = 0.1;
                }

                Idiff_ref_p1 = Idiff_ref;
                Idiff_ref_p1_cnt++; // 190915 Only test
                Diff_Time_flag = 1;
            }
        }

        if (Curr_Change_flag == 1)
        {
            Idiff_step = Idiff_ref_err * Diff_Change_Time;
            //if(Idiff_step <= 0.01666){Idiff_step = 0.01666;} 확인후 적용
            Curr_Change_flag = 0;  //add 190913
        }

        //******   Only PI Tuning Code  Start  ******//
        if (Diff_PI_test == 0)
        {
            DPICON2(Diff_Gate, Idiff_ref_Cnt, Ibat, Idiff_integ, Idiff_Kp, Idiff_Ki, Tsamp, (PWM_MAX_BB - PWM_Cut));
        }
        else if (Diff_PI_test == 1)
        {
            DPICON2(Diff_Gate, Idiff_ref, Ibat, Idiff_integ, Idiff_Kp, Idiff_Ki,Tsamp, (PWM_MAX_BB - PWM_Cut));
        }

//			if(Curr_Change_flag == 1){
        if (Diff_Time_flag == 1)//change 190913
        {
            if (Idiff_ref_Cnt <= Idiff_ref)
            {
                if (Idiff_ref >= 0)     { Rig_per = Rig_per_R; }
                else                    { Rig_per = Rig_per_F; }

                Idiff_ref_View = Idiff_ref * Rig_per;
                Idiff_ref_Cnt += Idiff_step;   //검토 필요

                if (Idiff_ref_Cnt >= Idiff_ref_View)//Rig_per_R = 0.95, Rig_per_F = 1.05
                {
                    Curr_Change_flag = 0;
                    Err_Curr_flag = 0;
                    Diff_Time_flag = 0;
                }
            }
            else if (Idiff_ref_Cnt >= Idiff_ref)
            {
                if (Idiff_ref >= 0)     { Rig_per = Rig_per_F; }
                else                    { Rig_per = Rig_per_R; }

                Idiff_ref_View = Idiff_ref * Rig_per;
                Idiff_ref_Cnt -= Idiff_step;

                if (Idiff_ref_Cnt <= Idiff_ref_View)//Rig_per_R = 0.95, Rig_per_F = 1.05
                {
                    Curr_Change_flag = 0;
                    Err_Curr_flag = 0;
                    Diff_Time_flag = 0;
                }
            }
        }
        else
        {
            if (Idiff_ref_Cnt <= Idiff_ref)     { Idiff_ref_Cnt += Idiff_step2; }  //검토 필요
            else                                { Idiff_ref_Cnt -= Idiff_step2; }
        }

        if (SCI_Standby_flag == 1){  // important
            Standby_cnt++;
            if (Standby_cnt >= REF_ms_TIME(50)){
                Iint_CV_integ = Idiff_integ;
                Init_Stat_flag = 0;
                Standby_cnt = 0;
                SCI_Standby_flag = 0;
                Differ_mode = 1;
            }
        }

        //******   Only PI Tuning Code  End    ******//

        //***   CV Mode Detection Start       ***//
        if (Master_DSP == 1){
            if (((Vpack_UI_rms >= Vbat_CV_Volt + 0.005) && (SCI_Charge_flag == 1))|| ((Vpack_UI_rms <= Vbat_CV_Volt - 0.005) && (SCI_Discharge_flag == 1))){
                Vbat_CV_cnt++;
                if (Vbat_CV_cnt > 50){
                    Vdiff_CV_integ = Ibat_UI_rms;
                    Idiff_CV_integ = Idiff_integ;
                    Idiff_limit_max = fabsf(Idiff_ref);
                    PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;  //PWM_CV_CUT =5
                    PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                    Vbat_CV_cnt = 0;
                    Differ_mode = 3;
                }
            }
            else{
                Vbat_CV_cnt = 0;
            }
        }
        else if (Slave_DSP == 1){
            if (SCI_Parallel_flag == 1){
                if ((D_Input2_flag == 1)||((Vpack_UI_rms >= Vbat_CV_Volt + 0.5) && (SCI_Charge_flag == 1))||((Vpack_UI_rms <= Vbat_CV_Volt - 0.5) && (SCI_Discharge_flag == 1))){
                    if (SCI_Charge_flag == 1){
                        Vdiff_CV_ref = Vpack_UI_rms - 0.005;
                    }
                    else if (SCI_Discharge_flag == 1){
                        Vdiff_CV_ref = Vpack_UI_rms + 0.005;
                    }
                    Vdiff_CV_integ = Ibat_UI_rms;
                    Idiff_CV_integ = Idiff_integ;
                    Idiff_limit_max = fabsf(Idiff_ref);
                    PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;
                    PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                    Vbat_CV_cnt = 0;
                    Differ_mode = 3;
                }
            }
            else{
                if (((Vpack_UI_rms >= Vbat_CV_Volt + 0.005) && (SCI_Charge_flag == 1))||((Vpack_UI_rms <= Vbat_CV_Volt - 0.005) && (SCI_Discharge_flag == 1))){
                    Vbat_CV_cnt++;
                    if (Vbat_CV_cnt > 50){
                        Vdiff_CV_integ = Ibat_UI_rms;
                        Idiff_CV_integ = Idiff_integ;
                        Idiff_limit_max = fabsf(Idiff_ref);
                        PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;
                        PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                        Vbat_CV_cnt = 0;
                        Differ_mode = 3;
                    }
                }
                else{
                    Vbat_CV_cnt = 0;
                }
            }
        }
        //***   CV Mode Detection End      ***//

        if (Diff_Gate < 10) Diff_Gate = 0.;   // 0 -> 10
        Diff_Gate_int = (int) (Diff_Gate);
        HR_temp = (Diff_Gate - Diff_Gate_int);
        Diff_HR_Gate = ((int) (HR_temp * MEP_ScaleFactor + 1.5) << 8); //Shift CMPAHR High bit
        //Diff_HR_Gate += 0x0180;//중복느낌... DouB

        EPwm1Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm1Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm1Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm2Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm2Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm2Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm3Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm3Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm3Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm4Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm4Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm4Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm5Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm5Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm5Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        EPwm6Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm6Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm6Regs.DBFEDHR.all = (Diff_HR_Gate << 1);//for a keeping during of a deadtime ; DouB

        break;   //* CC  Control  End *//

    case 3:   //* CV  Control  Start *//   Differ_mode => 3 : CV Mode

        Cha_Dis_Start_flag =1;

        if (Ibat_decre_flag == 0)
        {
            if (SCI_Charge_flag == 1)
            {
                SCI_TX_cccv = 4;	// Charge CV
                DPICON2(Idiff_CV_ref, Vdiff_CV_ref, Vpack, Vdiff_CV_integ, Vdiff_CV_Kp, Vdiff_CV_Ki, Tsamp, Idiff_limit_max);
                DPICON2(Diff_Gate, Idiff_CV_ref, Ibat, Idiff_CV_integ, Idiff_CV_Kp, Idiff_CV_Ki, Tsamp, PWM1_CV_MAX);

                Idiff_ref_Cnt = Idiff_CV_ref;
                Idiff_ref_p1 = Idiff_CV_ref;
                Idiff_ref_err = fabsf(Idiff_CV_ref) - 0.1;
            }
            else if (SCI_Discharge_flag == 1)
            {
                SCI_TX_cccv = 6;	// Discharge CV
                DPICON3(Idiff_CV_ref, Vdiff_CV_ref, Vpack, Vdiff_CV_integ, Vdiff_CV_Kp, Vdiff_CV_Ki, Tsamp, Idiff_limit_max); //DPICON3
                DDPICON2(Diff_Gate, -Idiff_CV_ref, Ibat, Idiff_CV_integ,Idiff_CV_Kp, Idiff_CV_Ki, Tsamp, PWM2_CV_MIN, PWM2_CV_MAX);

                Idiff_ref_Cnt = -Idiff_CV_ref;
                Idiff_ref_p1 = -Idiff_CV_ref;
                Idiff_ref_err = fabsf(Idiff_CV_ref) - 0.1;
            }
        }
        else
        {
            Idiff_step = Idiff_ref_err * Curr_decrease_Time; //0.01666 ->60   0.0333 ->30  112 -> 0.08928  수정필요
            if (Idiff_step <= 0.08928)
            {
                Idiff_step = 0.08928;
            }

            if (SCI_Charge_flag == 1)
            {
                DPICON2(Diff_Gate, Idiff_ref_Cnt, Ibat, Idiff_CV_integ, Idiff_Kp, Idiff_Ki, Tsamp, PWM1_CV_MAX);

                if (Idiff_ref_Cnt >= Idiff_ref)
                    Idiff_ref_Cnt -= Idiff_step;
                else
                {
                    Idiff_ref_Cnt = Idiff_ref;
                }
            }
            else if (SCI_Discharge_flag == 1)
            {
                DPICON2(Diff_Gate, Idiff_ref_Cnt, Ibat, Idiff_CV_integ, Idiff_Kp, Idiff_Ki, Tsamp, PWM1_CV_MAX);

                if (Idiff_ref_Cnt <= Idiff_ref)
                    Idiff_ref_Cnt += Idiff_step;
                else
                {
                    Idiff_ref_Cnt = Idiff_ref;
                }
            }

        }

        if (Ibat_decre_flag == 0)
        {
            if ((Mode_I_change_flag == 1) || (Mode_V_change_flag == 1))
            {
                if ((SCI_CCCV_CP_CR_flag == 3) || (SCI_CCCV_CP_CR_flag == 5))
                {
                    Idiff_integ = Idiff_CV_integ;
                    Differ_mode = 2;
                    Mode_I_change_flag = 0;
                    Mode_V_change_flag = 0;
                }
                else if ((SCI_CCCV_CP_CR_flag == 7) || (SCI_CCCV_CP_CR_flag == 8))
                {
                    Idiff_integ = Idiff_CV_integ;
                    Differ_mode = 4;
                    Mode_I_change_flag = 0;
                    Mode_V_change_flag = 0;
                }
                else
                {
                    Differ_mode = 3;
                }
            }
        }

        if (SCI_Standby_flag == 1)
        { //DouB :: 200226
          //changed to standby mode when a CV condition
          //not working 'zero current control'
            Standby_cnt++;
            if (Standby_cnt >= REF_ms_TIME(50))
            {
                Iint_CV_integ = Idiff_integ;
                Init_Stat_flag = 0;
                Standby_cnt = 0;
                SCI_Standby_flag = 0;
                Differ_mode = 1;
            }
        }

        if (Diff_Gate < 10) { Diff_Gate = 0; }
        Diff_Gate_int = (int) (Diff_Gate);
        HR_temp = (Diff_Gate - Diff_Gate_int);
        Diff_HR_Gate = ((int) (HR_temp * MEP_ScaleFactor + 1.5) << 8);
        //Diff_HR_Gate += 0x0180;//중복느낌... DouB

        EPwm1Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm1Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm1Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm2Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm2Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm2Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm3Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm3Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm3Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm4Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm4Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm4Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm5Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm5Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm5Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm6Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm6Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm6Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB
        break;   //* CV  Control  End *//

    case 4:  //* CP  Control  Start *//  Differ_mode => 4 : CP Mode
        Cha_Dis_Start_flag = 1;

        if (SCI_Charge_flag == 1)
        {
            SCI_TX_cccv = 7;  // Charge CP
        }
        else if (SCI_Discharge_flag == 1)
        {
            SCI_TX_cccv = 8; // Discharge CP
        }

        Vpack_Range_PI_Ctrl();

        //******   Mini Scope(Ibat_max, Rising_Time)  ******//

        if (Ibat > Ibat_max)
        {   // only_view
            Ibat_max = Ibat;
        }

        if (Rising_View == 0)
        {
            Rising_cnt++;
            if (fabsf(Ibat) >= fabsf(Idiff_ref * Rig_per_View))
            {  //0.95
                Rising_View = 1;
                Rising_ms = Rising_cnt * 0.11;
            }
            if (Rising_cnt >= 454)
            {  //454 -> 50ms
                Rising_View = 1;
                Rising_ms = Rising_cnt * 0.11;
            }
        }

        //******      Ibat Limit Set      *******//
        Ibat_lim = Output_Power_kW / fabsf(Vpack_UI_rms);
        if (Ibat_lim >= Output_Max_Current)
            Ibat_lim = Output_Max_Current;

        if ((SCI_Charge_flag == 1) && (SCI_CCCV_CP_CR_flag == 7))
        {
            if (Idiff_ref >= Ibat_lim)
            {
                Idiff_ref = Ibat_lim;
            }
        }
        else if ((SCI_Discharge_flag == 1) && (SCI_CCCV_CP_CR_flag == 8))
        {
            if (Idiff_ref <= -Ibat_lim)
            {
                Idiff_ref = -Ibat_lim;
            }
        }

        //******   Current Change Control  ******//

        if (Curr_Change_flag == 1)
        {  // 위치 확인 필요    case 1
            Err_Curr_flag = 1;
        }
        else
        {
            Err_Curr_flag = 0;
        }

        if (Err_Curr_flag == 1)
        {
            if (Diff_Time_flag == 0)
            {
                if (Init_Stat_flag == 1)
                {
                    if ((Idiff_ref >= 0) && (Idiff_ref_p1 >= 0))
                    {
                        Diff_Change_Time = Step_R_F_ms;
                    }
                    else if ((Idiff_ref < 0) && (Idiff_ref_p1 < 0))
                    {
                        Diff_Change_Time = Step_R_F_ms;
                    }
                    else
                    {
                        Diff_Change_Time = Step_TR_ms;
                    }
                }
                else
                {
                    Diff_Change_Time = Step_R_F_ms;
                    Init_Stat_flag = 1;
                }

                Idiff_ref_err = fabsf(Idiff_ref - Idiff_ref_p1);
                if (Idiff_ref_err <= 0.1)
                {
                    Idiff_ref_err = 0.1;
                }
                Idiff_ref_p1 = Idiff_ref;
                Idiff_ref_p1_cnt++; // 190915 Only test
                Diff_Time_flag = 1;
            }
        }

        if (Curr_Change_flag == 1)
        {
            Idiff_step = Idiff_ref_err * Diff_Change_Time;
            //if(Idiff_step <= 0.01666){Idiff_step = 0.01666;} 확인후 적용
            Curr_Change_flag = 0;  //add 190913
        }

        //******   Only PI Tuning Code  Start  ******//
        if (Diff_PI_test == 0)
        {
            //Idiff_ref
            DPICON2(Diff_Gate, Idiff_ref_Cnt, Ibat, Idiff_integ, Idiff_Kp, Idiff_Ki, Tsamp, (PWM_MAX_BB - PWM_Cut));
        }
        else if (Diff_PI_test == 1)
        {
            DPICON2(Diff_Gate, Idiff_ref, Ibat, Idiff_integ, Idiff_Kp, Idiff_Ki, Tsamp, (PWM_MAX_BB - PWM_Cut));
        }

//			if(Curr_Change_flag == 1){
        if (Diff_Time_flag == 1)
        {	  //change 190913
            if (Idiff_ref_Cnt <= Idiff_ref)
            {
                if (Idiff_ref >= 0)
                    Rig_per = Rig_per_R;
                else
                {
                    Rig_per = Rig_per_F;
                }

                Idiff_ref_View = Idiff_ref * Rig_per;
                Idiff_ref_Cnt += Idiff_step;   //검토 필요

                if (Idiff_ref_Cnt >= Idiff_ref_View)
                {  //Rig_per_R = 0.95, Rig_per_F = 1.05
                    Curr_Change_flag = 0;
                    Err_Curr_flag = 0;
                    Diff_Time_flag = 0;
                }
            }
            else if (Idiff_ref_Cnt >= Idiff_ref)
            {
                if (Idiff_ref >= 0)
                    Rig_per = Rig_per_F;
                else
                {
                    Rig_per = Rig_per_R;
                }

                Idiff_ref_View = Idiff_ref * Rig_per;
                Idiff_ref_Cnt -= Idiff_step;

                if (Idiff_ref_Cnt <= Idiff_ref_View)
                {   //Rig_per_R = 0.95, Rig_per_F = 1.05
                    Curr_Change_flag = 0;
                    Err_Curr_flag = 0;
                    Diff_Time_flag = 0;
                }
            }
        }
        else
        {
            if (Idiff_ref_Cnt <= Idiff_ref)
                Idiff_ref_Cnt += Idiff_step2;   //검토 필요
            else
            {
                Idiff_ref_Cnt -= Idiff_step2;
            }
        }

        if (SCI_Standby_flag == 1)
        {  // important
            Standby_cnt++;
            if (Standby_cnt >= REF_ms_TIME(50))
            {
                Iint_CV_integ = Idiff_integ;
                Init_Stat_flag = 0;
                Standby_cnt = 0;
                SCI_Standby_flag = 0;
                Differ_mode = 1;
            }
        }

        //******   Only PI Tuning Code  End    ******//

        //***   CV Mode Detection Start       ***//
        if (Master_DSP == 1)
        {
            if (((Vpack_UI_rms >= Vbat_CV_Volt + 0.005)
                    && (SCI_Charge_flag == 1))
                    || ((Vpack_UI_rms <= Vbat_CV_Volt - 0.005)
                            && (SCI_Discharge_flag == 1)))
            {
                Vbat_CV_cnt++;
                if (Vbat_CV_cnt > 50)
                {
                    Vdiff_CV_integ = Ibat_UI_rms;
                    Idiff_CV_integ = Idiff_integ;
                    Idiff_limit_max = fabsf(Idiff_ref);
                    PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;
                    PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                    Vbat_CV_cnt = 0;
                    Differ_mode = 3;
                }
            }
            else
            {
                Vbat_CV_cnt = 0;
            }
        }
        else if (Slave_DSP == 1)
        {
            if (SCI_Parallel_flag == 1)
            {
                if ((D_Input2_flag == 1)
                        || ((Vpack_UI_rms >= Vbat_CV_Volt + 0.5)
                                && (SCI_Charge_flag == 1))
                        || ((Vpack_UI_rms <= Vbat_CV_Volt - 0.5)
                                && (SCI_Discharge_flag == 1)))
                {
                    if (SCI_Charge_flag == 1)
                    {
                        Vdiff_CV_ref = Vpack_UI_rms - 0.005;
                    }
                    else if (SCI_Discharge_flag == 1)
                    {
                        Vdiff_CV_ref = Vpack_UI_rms + 0.005;
                    }
                    Vdiff_CV_integ = Ibat_UI_rms;
                    Idiff_CV_integ = Idiff_integ;
                    Idiff_limit_max = fabsf(Idiff_ref);
                    PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;
                    PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                    Vbat_CV_cnt = 0;
                    Differ_mode = 3;
                }
            }
            else
            {
                if (((Vpack_UI_rms >= Vbat_CV_Volt + 0.005)
                        && (SCI_Charge_flag == 1))
                        || ((Vpack_UI_rms <= Vbat_CV_Volt - 0.005)
                                && (SCI_Discharge_flag == 1)))
                {
                    Vbat_CV_cnt++;
                    if (Vbat_CV_cnt > 50)
                    {
                        Vdiff_CV_integ = Ibat_UI_rms;
                        Idiff_CV_integ = Idiff_integ;
                        Idiff_limit_max = fabsf(Idiff_ref);
                        PWM1_CV_MAX = Diff_Gate + PWM_CV_CUT;
                        PWM2_CV_MIN = Diff_Gate - PWM_CV_CUT;
                        Vbat_CV_cnt = 0;
                        Differ_mode = 3;
                    }
                }
                else
                {
                    Vbat_CV_cnt = 0;
                }
            }
        }
        //***   CV Mode Detection End      ***//

        if (Diff_Gate < 10)
            Diff_Gate = 0.;   // 0 -> 10
        Diff_Gate_int = (int) (Diff_Gate);
        HR_temp = (Diff_Gate - Diff_Gate_int);
        Diff_HR_Gate = ((int) (HR_temp * MEP_ScaleFactor + 1.5) << 8); //Shift CMPAHR High bit
        //Diff_HR_Gate += 0x0180;//중복느낌... DouB

        EPwm1Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm1Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm1Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm2Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm2Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm2Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm3Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm3Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm3Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm4Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm4Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm4Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm5Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm5Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm5Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        EPwm6Regs.CMPA.bit.CMPA = (int) (Diff_Gate_int);
        EPwm6Regs.CMPA.bit.CMPAHR = Diff_HR_Gate;
        EPwm6Regs.DBFEDHR.all = (Diff_HR_Gate << 1); //for a keeping during of a deadtime ; DouB

        break;  //* CP  Control  End *//
    }
}

void Control_Sequence(void)
{
    Pll();

    if (Bidirection_On_flag){
        Bidirection_control();
		if (SRC_start_flag == 1){
			SRC_control();
        }
    }

    if (Differ_On_flag){
        Differ_control();

        if (Differ_mode == 3){
            D_Out2_ON();
        }
        else{
            D_Out2_OFF();
        }
    }
    else{ D_Out2_OFF(); }

    if (!GpioDataRegs.GPBDAT.bit.GPIO39){  //D_Input#02
        D_Input2_flag = 1;
    }
    else{
        D_Input2_flag = 0;
    }

    if (sys_mode <= 7){
        D_Out2_OFF();
    }

    Rms_value();
}

void Vpack_Range_PI_Ctrl(void)
{

}





