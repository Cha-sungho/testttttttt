#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "SFO_V8.h"
#include "../main.h"

//int MEP_ScaleFactor=55; // Global variable used by the SFO library
                     // Result can be used for all HRPWM channels
                     // This variable is also copied to HRMSTEP
                     // register by SFO() function.

float f_Hr_duty=0;

int i_Value=0;
float f_Hr_data=0, f_Hr1_temp=0;

//
// Array of pointers to EPwm register structures:
// *ePWM[0] is defined as dummy value not used in the example
//
//volatile struct EPWM_REGS *ePWM[PWM_CH] =
//             {  &EPwm5Regs, &EPwm5Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs,
//                &EPwm5Regs, &EPwm6Regs, &EPwm7Regs, &EPwm8Regs};

volatile struct EPWM_REGS *ePWM[3] =
             { &EPwm7Regs, &EPwm7Regs, &EPwm8Regs};

//
// Function Prototypes
//
void HRPWM_Config(int period)
{
 //   Uint16 j;

    //
    // ePWM channel register configuration with HRPWM
    // ePWMxA / ePWMxB toggle low/high with MEP control on Rising edge
    //
/*    for (j=1;j<PWM_CH;j++)
    {
        (*ePWM[j]).TBCTL.bit.PRDLD = TB_SHADOW;  // set Immediate load             //ok
        (*ePWM[j]).TBPRD = period-1;             // PWM frequency = 1 / period     //ok
        (*ePWM[j]).CMPA.bit.CMPA = 0;   		 // set duty 0% initially
        (*ePWM[j]).CMPA.bit.CMPAHR = (1 << 8);   // initialize HRPWM extension         //ok
        (*ePWM[j]).CMPB.bit.CMPB = 0;		     // set duty 0% initially
        (*ePWM[j]).CMPB.all |= (1 << 8);         // initialize HRPWM extension
        (*ePWM[j]).TBPHS.all = 0;
        (*ePWM[j]).TBCTR = 0;    													 //ok

        (*ePWM[j]).TBCTL.bit.CTRMODE = TB_COUNT_UP;            //ok
        (*ePWM[j]).TBCTL.bit.PHSEN = TB_DISABLE;       			 //ok
        (*ePWM[j]).TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;        //ok
        (*ePWM[j]).TBCTL.bit.HSPCLKDIV = TB_DIV1;              //ok
        (*ePWM[j]).TBCTL.bit.CLKDIV = TB_DIV1;                 //ok
        (*ePWM[j]).TBCTL.bit.FREE_SOFT = 0x3;

        (*ePWM[j]).CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;                 //ok
        (*ePWM[j]).CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;                 //ok
        (*ePWM[j]).CMPCTL.bit.SHDWAMODE = CC_SHADOW;                 //ok
        (*ePWM[j]).CMPCTL.bit.SHDWBMODE = CC_SHADOW;                 //ok


        (*ePWM[j]).AQCTLA.bit.ZRO = AQ_SET;      // PWM toggle high/low
        (*ePWM[j]).AQCTLA.bit.CAU = AQ_CLEAR;                //ok
        (*ePWM[j]).AQCTLB.bit.ZRO = AQ_SET;
        (*ePWM[j]).AQCTLB.bit.CBU = AQ_CLEAR;                //ok

        EALLOW;
        (*ePWM[j]).HRCNFG.all = 0x0; 																//ok
        (*ePWM[j]).HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge			//ok
        (*ePWM[j]).HRCNFG.bit.CTLMODE = HR_CMP;											//ok
        (*ePWM[j]).HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;									//ok
        (*ePWM[j]).HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
        (*ePWM[j]).HRCNFG.bit.CTLMODEB = HR_CMP;
        (*ePWM[j]).HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
        #if (AUTOCONVERT)
        (*ePWM[j]).HRCNFG.bit.AUTOCONV = 1;      // Enable auto-conversion
                                                 // logic
        #endif
        (*ePWM[j]).HRPCTL.bit.HRPE = 0; // Turn off high-resolution period
                                        // control.
        EDIS;
    }
    HRPWM_GLOBAL_DIS();*/
}


void HR_PWM_DATA(float f_data, int i_pwm_num)
{
//		i_Value = (int)(f_data);
//		f_Hr1_temp = (f_data - i_Value);
//		f_Hr_data = ((int)(f_Hr1_temp * MEP_ScaleFactor + 0.5 ) << 8);
//		f_Hr_data += 0x0180;

	/*	switch (i_pwm_num)
		{
        case HR_PWM_CH1 :
            (*ePWM[1]).CMPA.bit.CMPA = (int)(f_data);
            (*ePWM[1]).CMPA.bit.CMPAHR = f_Hr_data;
            break;

        case HR_PWM_CH2 :
        	(*ePWM[1]).CMPB.bit.CMPB = (int)(f_data);
            (*ePWM[1]).CMPB.bit.CMPBHR = f_Hr_data;
            break;

        case HR_PWM_CH3 :
        	(*ePWM[2]).CMPA.bit.CMPA = (int)(f_data);
            (*ePWM[2]).CMPA.bit.CMPAHR = f_Hr_data;
            break;

        default :
            break;
		}  */
}


void InitEPwm_ALL(void)
{
	EALLOW;
        CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

#if (Formation_Inverter == 0)
        InitEPwm1Module();//buck boost sourcing side 1
        InitEPwm2Module();//buck boost sourcing side 2
        InitEPwm3Module();//buck boost sourcing side 3

        InitEPwm4Module();//buck boost sinking side 1
        InitEPwm5Module();//buck boost sinking side 2
        InitEPwm6Module();//buck boost sinking side 3
#endif

        InitEPwm7Module();//SRC based on full bridge
        InitEPwm8Module();//SRC based on full bridge

        //InitEPwm9Module();//reserved

        InitEPwm10Module();//inverter R
        InitEPwm11Module();//inverter S
        InitEPwm12Module();//inverter T

        CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

void InitEPwm1Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(1);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	// Setup Counter Mode and Clock
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		 // Count Up (Asymmetric)
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		 // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	//add s 190826
	EPwm1Regs.TBCTL.bit.FREE_SOFT = 0x3;

	EPwm1Regs.CMPA.bit.CMPA = 0;   			 // set duty 0% initially
	EPwm1Regs.CMPA.bit.CMPAHR = (1 << 8);   		 // initialize HRPWM extension
	EPwm1Regs.CMPB.bit.CMPB = 0;		     	 // set duty 0% initially
	EPwm1Regs.CMPB.bit.CMPBHR = (1 << 8);      		 // initialize HRPWM extension
	EPwm1Regs.TBPHS.all = 0;
	//add e 190826

	// Setup Phase
	EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
	EPwm1Regs.TBPHS.bit.TBPHS = 0;					// Phase is 0
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;			// Disable phase loading

	// Setup Period (Carrier Frequency)
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
	EPwm1Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
	EPwm1Regs.TBCTR = 0;							// Clear Counter

	// Set Compare Value
	EPwm1Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);	// Set Compare A Value to 20%

	// Setup shadowing
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

	// Set actions
	EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm1Regs.HRCNFG.all = 0x0;                                                                 //ok
    EPwm1Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                            //ok
    EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm1Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm1Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm1Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

	// Set Dead-time
	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;				// EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm1Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm1Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm1Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm1Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;		// Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

	EPwm1Regs.DBFED.bit.DBFED = 80;			   // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
	EPwm1Regs.DBRED.bit.DBRED = 80;			   // 200*5ns= 1.0us@33.33us

    //high resolution for a deadtime ; DouB
    EPwm1Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm1Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm1Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

	// Set Interrupts
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;		//ET_CTR_PRD	// Select INT on CNTR=Zero
}

void InitEPwm2Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(2);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
    // Setup Counter Mode and Clock
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;       // Count Up (Asymmetric)
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm2Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm2Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    EPwm2Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm2Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    EPwm2Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm2Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    //EPwm2Regs.TBPHS.bit.TBPHS = 0;
    EPwm2Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.3333f);
    //EPwm2Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.5f);//DouB 4DEBUG(2 phase)
    EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm2Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
    EPwm2Regs.TBCTR = 0;                            // Clear Counter

    // Set Compare Value
    EPwm2Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);   // Set Compare A Value to 20%

    // Setup shadowing
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm2Regs.HRCNFG.all = 0x0;                                                                 //ok
    EPwm2Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm2Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    EPwm2Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm2Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm2Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm2Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

    // Set Dead-time
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;              // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm2Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm2Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm2Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm2Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;      // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

    EPwm2Regs.DBFED.bit.DBFED = 80;               // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
    EPwm2Regs.DBRED.bit.DBRED = 80;               // 200*5ns= 1.0us@33.33us

    //high resolution for a deadtime ; DouB
    EPwm2Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm2Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm2Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

    // Set Interrupts
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm3Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(3);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
    // Setup Counter Mode and Clock
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;       // Count Up (Asymmetric)
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm3Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm3Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    EPwm3Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm3Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    EPwm3Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm3Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm3Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    //EPwm3Regs.TBPHS.bit.TBPHS = 0;
    EPwm3Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.6666f);
    EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm3Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
    EPwm3Regs.TBCTR = 0;                            // Clear Counter

    // Set Compare Value
    EPwm3Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);   // Set Compare A Value to 20%

    // Setup shadowing
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm3Regs.HRCNFG.all = 0x0;                                                                 //ok
    EPwm3Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm3Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    EPwm3Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm3Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm3Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm3Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

    // Set Dead-time
    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;              // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm3Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm3Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm3Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm3Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;      // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

    EPwm3Regs.DBFED.bit.DBFED = 80;               // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
    EPwm3Regs.DBRED.bit.DBRED = 80;               // 200*5ns= 1.0us@33.33us

    //high resolution for a deadtime ; DouB
    EPwm3Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm3Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm3Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

    // Set Interrupts
    EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm4Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(4);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	SyncSocRegs.SYNCSELECT.bit.EPWM4SYNCIN = 0;   //190318

    // Setup Counter Mode and Clock
    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;       // Count Up (Asymmetric)
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm4Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm4Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    EPwm4Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm4Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    EPwm4Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm4Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm4Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    //EPwm4Regs.TBPHS.bit.TBPHS = 0;
    EPwm4Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.1666f);
    //EPwm4Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.25f);//DouB 4DEBUG(2 phase)
    EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm4Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
    EPwm4Regs.TBCTR = 0;                            // Clear Counter

    // Set Compare Value
    EPwm4Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);   // Set Compare A Value to 20%

    // Setup shadowing
    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm4Regs.HRCNFG.all = 0x0;                                                                 //ok
    EPwm4Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm4Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    EPwm4Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm4Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm4Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm4Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

    // Set Dead-time
    EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;              // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm4Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm4Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm4Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm4Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;      // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

    EPwm4Regs.DBFED.bit.DBFED = 80;               // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
    EPwm4Regs.DBRED.bit.DBRED = 80;               // 200*5ns= 1.0us@33.33us
    EPwm4Regs.DBCTL.bit.OUTSWAP = 0x3;

    //high resolution for a deadtime ; DouB
    EPwm4Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm4Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm4Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

    // Set Interrupts
    EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm5Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(5);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
    // Setup Counter Mode and Clock
    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;       // Count Up (Asymmetric)
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm5Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm5Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    EPwm5Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm5Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    EPwm5Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm5Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm5Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    //EPwm5Regs.TBPHS.bit.TBPHS = 0;
    EPwm5Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.5f);
    //EPwm5Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.75f);//DouB 4DEBUG(2 phase)
    EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm5Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
    EPwm5Regs.TBCTR = 0;                            // Clear Counter

    // Set Compare Value
    EPwm5Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);   // Set Compare A Value to 20%

    // Setup shadowing
    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm5Regs.HRCNFG.all = 0x0;                                                                 //ok
    EPwm5Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm5Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    EPwm5Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm5Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm5Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm5Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

    // Set Dead-time
    EPwm5Regs.DBCTL.bit.IN_MODE = DBA_ALL;              // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm5Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm5Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm5Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm5Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;      // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

    EPwm5Regs.DBFED.bit.DBFED = 80;               // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
    EPwm5Regs.DBRED.bit.DBRED = 80;               // 200*5ns= 1.0us@33.33us
    EPwm5Regs.DBCTL.bit.OUTSWAP = 0x3;

    //high resolution for a deadtime ; DouB
    EPwm5Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm5Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm5Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

    // Set Interrupts
    EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm6Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(6);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
    // Setup Counter Mode and Clock
    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;       // Count Up (Asymmetric)
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm6Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm6Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    EPwm6Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm6Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    EPwm6Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm6Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm6Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    //EPwm6Regs.TBPHS.bit.TBPHS = 0;
    EPwm6Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.83333f);
    //EPwm6Regs.TBPHS.bit.TBPHS = (Uint16)(PWM_BB_SwitchingFrequency * 0.75f);//DouB 4DEBUG(2 phase)
    EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm6Regs.TBPRD = (int)(PWM_BB_SwitchingFrequency);// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
    EPwm6Regs.TBCTR = 0;                            // Clear Counter

    // Set Compare Value
    EPwm6Regs.CMPA.bit.CMPA = (int)(PWM_BB_SwitchingFrequency * 0.5);   // Set Compare A Value to 20%

    // Setup shadowing
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //add s 190826
    EPwm6Regs.HRCNFG.all = 0x0;                                                   //ok
    EPwm6Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    EPwm6Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    EPwm6Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    EPwm6Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    EPwm6Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    EPwm6Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;
    //add e 190826

    // Set Dead-time
    EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;              // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm6Regs.DBCTL.bit.SHDWDBREDMODE = 1;//set a shadow mode ; DouB
    EPwm6Regs.DBCTL.bit.SHDWDBFEDMODE = 1;//set a shadow mode ; DouB
    EPwm6Regs.DBCTL.bit.LOADREDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm6Regs.DBCTL.bit.LOADFEDMODE = 0;//update timing for a deadtime regi ; DouB
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active High Complementary (AHC). EPWMxB is inverted
    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;      // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB

    EPwm6Regs.DBFED.bit.DBFED = 80;               // 200*5ns= 1.0us@33.33us, 200*5ns=1.0us  450*5ns=2.25us, 600*5ns=3.0us  1000*5ns=5.0us
    EPwm6Regs.DBRED.bit.DBRED = 80;               // 200*5ns= 1.0us@33.33us
    EPwm6Regs.DBCTL.bit.OUTSWAP = 0x3;

    //high resolution for a deadtime ; DouB
    EPwm6Regs.HRCNFG2.bit.EDGMODEDB = 0x3;//both of rising and falling ; DouB
    EPwm6Regs.HRCNFG2.bit.CTLMODEDBRED = 0;//should match DBCTL[LOADREDMODE] ; DouB
    EPwm6Regs.HRCNFG2.bit.CTLMODEDBFED = 0;//should match DBCTL[LOADFEDMODE] ; DouB

    // Set Interrupts
    EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm7Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(7);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	SyncSocRegs.SYNCSELECT.bit.EPWM7SYNCIN = 0;   //190318

    // Setup Counter Mode and Clock
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm7Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm7Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    //EPwm7Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm7Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    //EPwm7Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm7Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm7Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    EPwm7Regs.TBPHS.bit.TBPHS = 0;
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm7Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm7Regs.TBPRD = (int)(PWM_SRC_SwitchingFrequency);
    EPwm7Regs.TBCTR = 0;                            // Clear Counter

    // Setup shadowing
    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBD = AQ_CLEAR;

    // Set Dead-time
    EPwm7Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;    // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;          // Active High Complementary (AHC). EPWMxB is inverted
    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;    // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
//    EPwm7Regs.DBFED.bit.DBFED = 0.;                  // 100*5ns= 0.5us@22.22us@4444    dead_time:2%
//    EPwm7Regs.DBRED.bit.DBRED = 0.;                  // 100*5ns= 0.5us@22.22us@4444    dead_time:2%

    //EPwm7Regs.HRCNFG.all = 0x0;                                                                 //ok
    //EPwm7Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge          //ok
    //EPwm7Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                          //ok
    //EPwm7Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                 //ok
    //EPwm7Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    //EPwm7Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    //EPwm7Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;

    // Set Interrupts
    EPwm7Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm8Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(8);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
    // Setup Counter Mode and Clock
    EPwm8Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  //SYNCOSEL(Synchronization Output Select)   190911 Very important

    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;         // TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    //add s 190826
    EPwm8Regs.TBCTL.bit.FREE_SOFT = 0x3;

    EPwm8Regs.CMPA.bit.CMPA = 0;             // set duty 0% initially
    //EPwm8Regs.CMPA.bit.CMPAHR = (1 << 8);            // initialize HRPWM extension
    EPwm8Regs.CMPB.bit.CMPB = 0;                 // set duty 0% initially
    //EPwm8Regs.CMPB.bit.CMPBHR = (1 << 8);            // initialize HRPWM extension
    EPwm8Regs.TBPHS.all = 0;
    //add e 190826

    // Setup Phase
    EPwm8Regs.TBCTL.bit.PHSDIR = TB_UP;              //PHSDIR(Phase Direction bit) only up-down count mode
    EPwm8Regs.TBPHS.bit.TBPHS = 0;
    EPwm8Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading

    // Setup Period (Carrier Frequency)
    EPwm8Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
    EPwm8Regs.TBPRD = (int)(PWM_SRC_SwitchingFrequency);
    EPwm8Regs.TBCTR = 0;                            // Clear Counter

    // Setup shadowing
    EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Compare A Register is loaded from its shadow when CNTR=Zero
    EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;   //
    EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   //

    // Set actions
    EPwm8Regs.AQCTLA.bit.ZRO = AQ_CLEAR;            // Set PWM1A on Zero
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.CBD = AQ_CLEAR;

    // Set Dead-time
    EPwm8Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;    // EPWMxA is the source for both falling-edge & rising-edge delay
    EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;          // Active High Complementary (AHC). EPWMxB is inverted
    EPwm8Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;    // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
//    EPwm8Regs.DBFED.bit.DBFED = 0.;                  // 100*5ns= 0.5us@22.22us@4444    dead_time:2%
//    EPwm8Regs.DBRED.bit.DBRED = 0.;                  // 100*5ns= 0.5us@22.22us@4444    dead_time:2%
    EPwm8Regs.DBCTL.bit.OUTSWAP = 0x3;

    //EPwm8Regs.HRCNFG.all = 0x0;                                                  //ok
    //EPwm8Regs.HRCNFG.bit.EDGMODE = HR_FEP;  // MEP control on falling edge       //ok
    //EPwm8Regs.HRCNFG.bit.CTLMODE = HR_CMP;                                       //ok
    //EPwm8Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;                                  //ok
    //EPwm8Regs.HRCNFG.bit.EDGMODEB = HR_FEP; // MEP control on falling edge
    //EPwm8Regs.HRCNFG.bit.CTLMODEB = HR_CMP;
    //EPwm8Regs.HRCNFG.bit.HRLOADB  = HR_CTR_ZERO;

    // Set Interrupts
    EPwm8Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;        //ET_CTR_PRD    // Select INT on CNTR=Zero
}

void InitEPwm9Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
//    InitEpwmTrip(9);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
//	EPwm9Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;  //SYNCOSEL(Syncronization Output Select)	     add============
//
//	EPwm9Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// Count Up (Asymmetric)
//	EPwm9Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
//	EPwm9Regs.TBCTL.bit.CLKDIV = TB_DIV1;
//
//	// Setup Phase
//  EPwm9Regs.TBCTL.bit.PHSDIR = TB_UP;        //PHSDIR(Phase Direction bit) only up-down count mode     add    =========   ===
//	EPwm9Regs.TBPHS.bit.TBPHS = 0;			// Phase is 0
//	EPwm9Regs.TBCTL.bit.PHSEN = TB_DISABLE;			// Disable phase loading
//
//	// Setup Period (Carrier Frequency)
//  EPwm9Regs.TBCTL.bit.PRDLD = TB_SHADOW;          // Period Register is loaded from its shadow when CNTR=Zero
//	EPwm9Regs.TBPRD = (int)(PWM_SwitchingFrequency);	// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
//	EPwm9Regs.TBCTR = 0;						// Clear Counter
//
//	// Set Compare Value
//	EPwm9Regs.CMPA.bit.CMPA = (int)(PWM_SwitchingFrequency * 0.5);	// Set Compare A Value to 20%   확인필요
//
//	// Setup shadowing
//	EPwm9Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
//	EPwm9Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
//	EPwm9Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;    //ok
//	EPwm9Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;    //ok
//
//	// Set actions
//	EPwm9Regs.AQCTLA.bit.CAU = AQ_CLEAR;
//	EPwm9Regs.AQCTLA.bit.CAD = AQ_CLEAR;
//	EPwm9Regs.AQCTLB.bit.CBU = AQ_CLEAR;
//	EPwm9Regs.AQCTLB.bit.CBD = AQ_CLEAR;
//
//	// Set Dead-time
//	EPwm9Regs.DBCTL.bit.IN_MODE = 0;		// EPWMxA is the source for both falling-edge & rising-edge delay
//	EPwm9Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;		// Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
//	EPwm9Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;	// Active High Complementary (AHC). EPWMxB is inverted
//	EPwm9Regs.DBFED.bit.DBFED = 600;			// 0sec, Zero
//	EPwm9Regs.DBRED.bit.DBRED = 600;			// 0sec, Zero
//
//	// Set Interrupts
//	EPwm9Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;		//ET_CTR_PRD	// Select INT on CNTR=Zero
}

void InitEPwm10Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(10);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	SyncSocRegs.SYNCSELECT.bit.EPWM10SYNCIN = 0;   //190318

	EPwm10Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;  //SYNCOSEL(Syncronization Output Select)	     add============

	EPwm10Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// Count Up (Asymmetric)
	EPwm10Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
	EPwm10Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup Phase
	EPwm10Regs.TBCTL.bit.PHSDIR = TB_UP;        //PHSDIR(Phase Direction bit) only up-down count mode     add    =========   ===
	EPwm10Regs.TBPHS.bit.TBPHS = 0;			// Phase is 0
	EPwm10Regs.TBCTL.bit.PHSEN = TB_DISABLE;			// Disable phase loading

	// Setup Period (Carrier Frequency)
	EPwm10Regs.TBCTL.bit.PRDLD = TB_SHADOW;         // Period Register is loaded from its shadow when CNTR=Zero
	EPwm10Regs.TBPRD = (int)(PWM_INV_SwitchingFrequency);	// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
	EPwm10Regs.TBCTR = 0;						// Clear Counter

	// Set Compare Value
	EPwm10Regs.CMPA.bit.CMPA = (int)(PWM_INV_SwitchingFrequency * 0.5);	// Set Compare A Value to 20%   확인필요
	EPwm10Regs.CMPB.bit.CMPB = 0;

	// Setup shadowing
	EPwm10Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm10Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm10Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;    //ok
	EPwm10Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;    //ok

	// Set actions
	EPwm10Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm10Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm10Regs.AQCTLB.bit.CBU = AQ_CLEAR;
	EPwm10Regs.AQCTLB.bit.CBD = AQ_CLEAR;

	// Set Dead-time
	EPwm10Regs.DBCTL.bit.IN_MODE = DBA_ALL;		// EPWMxA is the source for both falling-edge & rising-edge delay
	EPwm10Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;	// Active High Complementary (AHC). EPWMxB is inverted
	EPwm10Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;     // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
	EPwm10Regs.DBFED.bit.DBFED = 70;			// 300*5ns= 1.5us@33.33us
	EPwm10Regs.DBRED.bit.DBRED = 70;			// 300*5ns= 1.5us@33.33us

	// Set Interrupts
	EPwm10Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;		//ET_CTR_PRD	// Select INT on CNTR=Zero
}

void InitEPwm11Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(11);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	EPwm11Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;  //SYNCOSEL(Syncronization Output Select)	     add============

	EPwm11Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// Count Up (Asymmetric)
	EPwm11Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
	EPwm11Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup Phase
	EPwm11Regs.TBCTL.bit.PHSDIR = TB_UP;        //PHSDIR(Phase Direction bit) only up-down count mode     add    =========   ===
	EPwm11Regs.TBPHS.bit.TBPHS = 0;			// Phase is 0
	EPwm11Regs.TBCTL.bit.PHSEN = TB_DISABLE;			// Disable phase loading

	// Setup Period (Carrier Frequency)
	EPwm11Regs.TBCTL.bit.PRDLD = TB_SHADOW;         // Period Register is loaded from its shadow when CNTR=Zero
	EPwm11Regs.TBPRD = (int)(PWM_INV_SwitchingFrequency);	// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
	EPwm11Regs.TBCTR = 0;						// Clear Counter

	// Set Compare Value
	EPwm11Regs.CMPA.bit.CMPA = (int)(PWM_INV_SwitchingFrequency * 0.5);	// Set Compare A Value to 20%   확인필요
	EPwm11Regs.CMPB.bit.CMPB = 0;

	// Setup shadowing
	EPwm11Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm11Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm11Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;    //ok
	EPwm11Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;    //ok

	// Set actions
	EPwm11Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm11Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm11Regs.AQCTLB.bit.CBU = AQ_CLEAR;
	EPwm11Regs.AQCTLB.bit.CBD = AQ_CLEAR;

	// Set Dead-time
	EPwm11Regs.DBCTL.bit.IN_MODE = DBA_ALL;		// EPWMxA is the source for both falling-edge & rising-edge delay
	EPwm11Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;	// Active High Complementary (AHC). EPWMxB is inverted
	EPwm11Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;     // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
	EPwm11Regs.DBFED.bit.DBFED = 70;			// 300*5ns= 1.5us@33.33us
	EPwm11Regs.DBRED.bit.DBRED = 70;			// 300*5ns= 1.5us@33.33us

	// Set Interrupts
	EPwm11Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;		//ET_CTR_PRD	// Select INT on CNTR=Zero
}

void InitEPwm12Module(void)
{
//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
    InitEpwmTrip(12);
//
//bgkim-2020-01-29
//Trip Init function
//end Line
//################################################################################################
	EPwm12Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;  //SYNCOSEL(Syncronization Output Select)	     add============

	EPwm12Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;		// Count Up (Asymmetric)
	EPwm12Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;		// TBCLK = (SYSCLKOUT / EPWMCLKDIV) / (HSPCLKDIV * CLKDIV) = 200MHz
	EPwm12Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup Phase
	EPwm12Regs.TBCTL.bit.PHSDIR = TB_UP;        //PHSDIR(Phase Direction bit) only up-down count mode     add    =========   ===
	EPwm12Regs.TBPHS.bit.TBPHS = 0;			// Phase is 0
	EPwm12Regs.TBCTL.bit.PHSEN = TB_DISABLE;			// Disable phase loading

	// Setup Period (Carrier Frequency)
	EPwm12Regs.TBCTL.bit.PRDLD = TB_SHADOW;         // Period Register is loaded from its shadow when CNTR=Zero
	EPwm12Regs.TBPRD = (int)(PWM_INV_SwitchingFrequency);	// Set Timer Period, (200MHz/20KHz)-1 = 9,999 (0x270F)
	EPwm12Regs.TBCTR = 0;						// Clear Counter

	// Set Compare Value
	EPwm12Regs.CMPA.bit.CMPA = (int)(PWM_INV_SwitchingFrequency * 0.5);	// Set Compare A Value to 20%   확인필요
	EPwm12Regs.CMPB.bit.CMPB = 0;

	// Setup shadowing
	EPwm12Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm12Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;		// Compare A Register is loaded from its shadow when CNTR=Zero
	EPwm12Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;    //ok
	EPwm12Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;    //ok

	// Set actions
	EPwm12Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm12Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm12Regs.AQCTLB.bit.CBU = AQ_CLEAR;
	EPwm12Regs.AQCTLB.bit.CBD = AQ_CLEAR;

	// Set Dead-time
	EPwm12Regs.DBCTL.bit.IN_MODE = DBA_ALL;		// EPWMxA is the source for both falling-edge & rising-edge delay
	EPwm12Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;	// Active High Complementary (AHC). EPWMxB is inverted
	EPwm12Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;     // Dead-band is fully enabled for both rising-edge delay on EPWMxA and falling-edge delay on EPWMxB
	EPwm12Regs.DBFED.bit.DBFED = 70;			// 300*5ns= 1.5us@33.33us
	EPwm12Regs.DBRED.bit.DBRED = 70;			// 300*5ns= 1.5us@33.33us

	// Set Interrupts
	EPwm12Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;		//ET_CTR_PRD	// Select INT on CNTR=Zero
}
//################################################################################################
//bgkim-2020-01-29
//Trip Flag Cler function
//Start Line
//
void ClaerTripFlag(void)
{
    unsigned int i = 0;
    volatile struct EPWM_REGS *s_Temp;

    s_Temp = (volatile struct EPWM_REGS *)(0x00004000);

    for(i=0; i <12 ; i++)
    {
        EALLOW;
        if((*s_Temp).TZFLG.bit.OST)
        {
            (*s_Temp).TZCLR.bit.OST = 1;
        }
        EDIS;
        s_Temp++;
    }
}
//
//bgkim-2020-01-29
//Trip Flag Cler function
//End Line
//################################################################################################

//################################################################################################
//bgkim-2020-01-29
//Trip Init function
//Start Line
//
int InitEpwmTrip(unsigned int ui_Number)
{
#if TRIP_ON_EPWM
    unsigned int i = 0;
    volatile struct EPWM_REGS *s_Temp;

    s_Temp = (volatile struct EPWM_REGS *)(0x00004000);

    if( ui_Number <= 0  || 12 < ui_Number )return (0);
    else
    {
        for(i = 1; i < ui_Number; i++)
        {
            s_Temp++;
        }

//        EALLOW;
        (*s_Temp).TZSEL.bit.DCAEVT1 = 1; // Oneshut mode
        (*s_Temp).TZCTL.bit.TZA = TZ_FORCE_LO;//Trip after epwmA action of Lo signal
        (*s_Temp).TZCTL.bit.TZB = TZ_FORCE_LO;

        (*s_Temp).DCACTL.bit.EVT1SRCSEL = 0; // DCAEVT1
        (*s_Temp).DCACTL.bit.EVT1FRCSYNCSEL = 1; //Async Set

        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT4  = 1; //Select input4 Trip signal
        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT5  = 1; //Select input5 Trip signal
        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT6  = 1; //Select input6 Trip signal
        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT7  = 1; //Select input7 Trip signal
        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT8  = 1; //Select input8 Trip signal
        (*s_Temp).DCAHTRIPSEL.bit.TRIPINPUT10 = 1; //Select input10 Trip signal

        (*s_Temp).DCTRIPSEL.bit.DCAHCOMPSEL   = 0xF; //input Trip signal all combine
        (*s_Temp).TZDCSEL.bit.DCAEVT1         = 1; //compare action (001: DCAH = low, DCAL = don't care)
//        EDIS;
    }
#endif
    return (1);
}
//
//bgkim-2020-01-29
//Trip Init function
//End Line
//################################################################################################




