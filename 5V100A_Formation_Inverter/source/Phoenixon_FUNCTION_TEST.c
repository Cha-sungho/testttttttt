#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"
#include "../main.h"

int PWM_test_flag = 0, pwm_AC_test = 0, pwm_SRC_test = 0, pwm_HR_test = 0, pwm_open_flag = 0, MC_test_flag = 0, Lamp_test_flag = 0, Etc_test_flag = 0;
int Mc_On_value = 0, Lamp_On_value = 0, Etc_On_value = 0;
int Gate1_int = 0, Gate2_int = 0, Gate3_int = 0, Gate4_int = 0, Gate5_int = 0, Gate6_int = 0;
//int MEP_ScaleFactor = 55;//too big "##.99 > ##.00 at the Duty"
int MEP_ScaleFactor = 32, HR1_Gate = 0, HR2_Gate = 0, HR3_Gate = 0, HR4_Gate = 0, HR5_Gate = 0, HR6_Gate = 0;
float HR1_temp = 0, HR2_temp = 0, HR3_temp = 0, HR4_temp = 0, HR5_temp = 0, HR6_temp = 0;
float Gate_test1 = 0., Gate_test2 = 0., Gate_test3 = 0., Gate_test4 = 0., Gate_test5 = 0., Gate_test6 = 0.;
float Gate_test7 = 0., Gate_test8 = 0., Gate_test10 = 0., Gate_test11 = 0., Gate_test12 = 0.;
int D_Input_flag = 0, D_Input2_flag = 0, D_Input3_flag = 0;

//***   DC_Soft_Charge  ***//
//int soft_mode =0, DC_Soft_Charge_comm=0;
//float Vpack_ref=0., Vpack_step=0.01, I_lim=1., I_lim_max=1., I_lim_step = 0.0001, V_Kp=0.5, V_Ki=10., C_Kp=6., C_Ki=3500., PWM_CV_MAX=1500.;
//float V_CV_integ =0., Soft_CV_ref =0, Soft_CV_integ=0.;

void Pwm_test(void){
    if (pwm_AC_test){
        if (pwm_open_flag){
            PWM_AC_Open();
            EPwm10Regs.CMPA.bit.CMPA = (int) (Gate_test10);
            EPwm11Regs.CMPA.bit.CMPA = (int) (Gate_test11);
            EPwm12Regs.CMPA.bit.CMPA = (int) (Gate_test12);
        }
        else{
            PWM_AC_Block();
            PWM_INV_Disable();
        }
    }

    else if (pwm_SRC_test){
        if (pwm_open_flag){
            PWM_SRC_Open();
            if(Gate_test7>= PWM_MAX_SRC - PWM_SRC_DT) Gate_test7 =PWM_MAX_SRC -PWM_SRC_DT;
            EPwm7Regs.CMPA.bit.CMPA = (int)(Gate_test7);
            EPwm7Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - Gate_test7);//DouB

            if(Gate_test8>= PWM_MAX_SRC - PWM_SRC_DT) Gate_test8 =PWM_MAX_SRC -PWM_SRC_DT;
            EPwm8Regs.CMPA.bit.CMPA = (int)(Gate_test8);
            EPwm8Regs.CMPB.bit.CMPB = (int)(PWM_MAX_SRC - Gate_test8);//DouB
        }
        else{
            PWM_SRC_Block();
        }
    }

    else if (pwm_HR_test){
        if (pwm_open_flag){
            HRPWM_Open();

            Gate1_int = (int) (Gate_test1);
            HR1_temp = (Gate_test1 - Gate1_int);
            HR1_Gate = ((int) (HR1_temp * MEP_ScaleFactor + 1.5) << 8);

            Gate2_int = (int) (Gate_test2);
            HR2_temp = (Gate_test2 - Gate2_int);
            HR2_Gate = ((int) (HR2_temp * MEP_ScaleFactor + 1.5) << 8);

            Gate3_int = (int) (Gate_test3);
            HR3_temp = (Gate_test3 - Gate3_int);
            HR3_Gate = ((int) (HR3_temp * MEP_ScaleFactor + 1.5) << 8);

            Gate4_int = (int) (Gate_test4);
            HR4_temp = (Gate_test4 - Gate4_int);
            HR4_Gate = ((int) (HR4_temp * MEP_ScaleFactor + 1.5) << 8);

            Gate5_int = (int) (Gate_test5);
            HR5_temp = (Gate_test5 - Gate5_int);
            HR5_Gate = ((int) (HR5_temp * MEP_ScaleFactor + 1.5) << 8);

            Gate6_int = (int) (Gate_test6);
            HR6_temp = (Gate_test6 - Gate6_int);
            HR6_Gate = ((int) (HR6_temp * MEP_ScaleFactor + 1.5) << 8);

            EPwm1Regs.CMPA.bit.CMPA = (int) (Gate_test1);
            EPwm1Regs.CMPA.bit.CMPAHR = HR1_Gate;
            EPwm1Regs.DBFEDHR.all = (HR1_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm1Regs.CMPB.bit.CMPB = 0;

            EPwm2Regs.CMPA.bit.CMPA = (int) (Gate_test2);
            EPwm2Regs.CMPA.bit.CMPAHR = HR2_Gate;
            EPwm2Regs.DBFEDHR.all = (HR2_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm2Regs.CMPB.bit.CMPB = 0;

            EPwm3Regs.CMPA.bit.CMPA = (int) (Gate_test3);
            EPwm3Regs.CMPA.bit.CMPAHR = HR3_Gate;
            EPwm3Regs.DBFEDHR.all = (HR3_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm3Regs.CMPB.bit.CMPB = 0;

            EPwm4Regs.CMPA.bit.CMPA = (int) (Gate_test4);
            EPwm4Regs.CMPA.bit.CMPAHR = HR4_Gate;
            EPwm4Regs.DBFEDHR.all = (HR4_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm4Regs.CMPB.bit.CMPB = 0;

            EPwm5Regs.CMPA.bit.CMPA = (int) (Gate_test5);
            EPwm5Regs.CMPA.bit.CMPAHR = HR5_Gate;
            EPwm5Regs.DBFEDHR.all = (HR5_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm5Regs.CMPB.bit.CMPB = 0;

            EPwm6Regs.CMPA.bit.CMPA = (int) (Gate_test6);
            EPwm6Regs.CMPA.bit.CMPAHR = HR6_Gate;
            EPwm6Regs.DBFEDHR.all = (HR6_Gate << 1);//for a keeping during of a deadtime ; DouB
            EPwm6Regs.CMPB.bit.CMPB = 0;
        }
        else{
            HRPWM_Block();
        }
    }
}


void MC_test(void)
{
    switch (Mc_On_value)
    {
    case 0:
        All_MC_off();
        break;
    case 1:
        Grid_SOFT_MC_ON();
        break;
    case 2:
        Grid_MAIN_MC_ON();
        break;
    case 3:
//        CHAR_DC_SOFT_MC_ON();
        break;
    case 4:
        CHAR_DC_MAIN_MC_ON();			  // dc main
        break;
    case 5:
//        CHAR_DC_OUT_MC_ON();			  // dc both
        break;
    case 6:
//        CHAR_DC_OUT_Parall_MC_ON();      // dc both Parallel
        break;
    case 7:
        Pack_Sensing_ON();
        break;
    case 8:
        Pack_Parall_Sensing_ON();
        break;
    case 9:
        IGBT_SOURCE_ON();
        break;
    case 10:
        CV_Resistor_ON();
        break;
    case 11:
        CV_OFF_Resistor_ON();
        break;
    }
}


void Lamp_test(void)
{
    switch (Lamp_On_value)
    {
    case 0:
        AC_RUN_OFF(); LAMP_RUN_OFF(); LAMP_FAULT_OFF(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
        break;

    case 1:
        AC_RUN_ON(); LAMP_RUN_OFF(); LAMP_FAULT_OFF(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
        break;

    case 2:
        AC_RUN_OFF(); LAMP_RUN_ON(); LAMP_FAULT_OFF(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
        break;

    case 3:
        AC_RUN_OFF(); LAMP_RUN_OFF(); LAMP_FAULT_ON(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_OFF();
        break;

    case 4:
        AC_RUN_OFF(); LAMP_RUN_OFF(); LAMP_FAULT_OFF(); LAMP_CHARGE_ON(); LAMP_DISCHARGE_OFF();
        break;

    case 5:
        AC_RUN_OFF(); LAMP_RUN_OFF(); LAMP_FAULT_OFF(); LAMP_CHARGE_OFF(); LAMP_DISCHARGE_ON();
        break;

    case 6:
        AC_RUN_ON(); LAMP_RUN_ON(); LAMP_FAULT_ON(); LAMP_CHARGE_ON(); LAMP_DISCHARGE_ON();
        break;

    default:
        break;
    }
}


void Etc_test(void)
{
    switch (Etc_On_value){
    case 0:
        D_Out_OFF(); D_Out2_OFF(); D_Out3_OFF(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF(); Driver_Set();
        break;

    case 1:
        D_Out_ON(); D_Out2_OFF(); D_Out3_OFF(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF();
        break;

    case 2:
        D_Out_OFF(); D_Out2_ON(); D_Out3_OFF(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF();
        break;

    case 3:
        D_Out_OFF(); D_Out2_OFF(); D_Out3_ON(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF();
        break;

    case 4:
        D_Out_OFF(); D_Out2_OFF(); D_Out3_OFF(); Vreverce_Chk_ON(); AC_HS_FAN_OFF();
        break;

    case 5:
        D_Out_OFF(); D_Out2_OFF(); D_Out3_OFF(); Vreverce_Chk_OFF(); AC_HS_FAN_ON();
        break;

    case 6:
        D_Out_ON(); D_Out2_ON(); D_Out3_ON(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF();
        break;

    case 7:
        D_Out_ON(); D_Out2_ON(); D_Out3_ON(); Vreverce_Chk_OFF(); AC_HS_FAN_OFF(); Driver_Reset();
        break;

    default:
        break;
    }

    //D_Input#01
    if (!GpioDataRegs.GPBDAT.bit.GPIO38)    { D_Input_flag = 1; }
    else                                    { D_Input_flag = 0; }

    //D_Input#02
    if (!GpioDataRegs.GPBDAT.bit.GPIO39)    { D_Input2_flag = 1; }
    else                                    { D_Input2_flag = 0; }

    //D_Input#03
    if (!GpioDataRegs.GPBDAT.bit.GPIO40)    { D_Input3_flag = 1; }
    else                                    { D_Input3_flag = 0; }

}


void PWM_AC_Open(void)
{
    //PWM_INV_Enable();

    EPwm10Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm10Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm10Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm10Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm10Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm10Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm11Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm11Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm11Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm11Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm11Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm11Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm12Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm12Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm12Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm12Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm12Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm12Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    PWM_INV_Enable();
}


void PWM_AC_Block(void)
{
	//PWM_INV_Disable();

    EPwm10Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm10Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm10Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm10Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm10Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm10Regs.AQCTLB.bit.CBU = AQ_CLEAR;


    EPwm11Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm11Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm11Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm11Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm11Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm11Regs.AQCTLB.bit.CBU = AQ_CLEAR;


    EPwm12Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm12Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm12Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm12Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm12Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm12Regs.AQCTLB.bit.CBU = AQ_CLEAR;
}


void PWM_SRC_Open(void)
{
    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_SET;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.PRD = AQ_SET;
    EPwm7Regs.AQCTLB.bit.CBD = AQ_CLEAR;

    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
    EPwm8Regs.AQCTLA.bit.ZRO = AQ_SET;
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.PRD = AQ_SET;
    EPwm8Regs.AQCTLB.bit.CBD = AQ_CLEAR;

    PWM_SRC_Enable();
}


void PWM_SRC_Block(void)
{
    PWM_SRC_Disable();

    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBD = AQ_CLEAR;

    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
    EPwm8Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.CBD = AQ_CLEAR;
}


void HRPWM_Open(void)
{
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm1Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm2Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm3Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm4Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm5Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm6Regs.AQCTLA.bit.PRD = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    PWM_DCDC_Enable();
}


void HRPWM_Block(void)
{
    PWM_DCDC_Disable();

    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm4Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm5Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
    EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
    EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;
}


void PWM_Block(void)
{
    PWM_INV_Disable();
    PWM_SRC_Disable();
    PWM_DCDC_Disable();
#if (Formation_Inverter == 0)
    {//Buck Boost side PWM port
        EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm4Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm5Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm5Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;
    }
#endif
    {//SRC side PWM port
        EPwm7Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
        EPwm7Regs.AQCTLA.bit.ZRO = AQ_CLEAR;            // Set PWM1A on Zero
        EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
        EPwm7Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm7Regs.AQCTLB.bit.CBD = AQ_CLEAR;

        EPwm8Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; // Active Hi complementary, EPWM1B is inverted
        EPwm8Regs.AQCTLA.bit.ZRO = AQ_CLEAR;            // Set PWM1A on Zero
        EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
        EPwm8Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm8Regs.AQCTLB.bit.CBD = AQ_CLEAR;
    }
    {//Inverter side PWM port
        EPwm10Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm10Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm10Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm10Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm10Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm10Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm11Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm11Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm11Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm11Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm11Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm11Regs.AQCTLB.bit.CBU = AQ_CLEAR;

        EPwm12Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; // disable Dead-band module
        EPwm12Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary, EPWM1B is inverted
        EPwm12Regs.AQCTLA.bit.PRD = AQ_CLEAR; // Time-base counter equal to the period value(PRD)
        EPwm12Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Counter equal to CMPA on up-count(CAU)
        EPwm12Regs.AQCTLB.bit.PRD = AQ_CLEAR;
        EPwm12Regs.AQCTLB.bit.CBU = AQ_CLEAR;
    }
}






