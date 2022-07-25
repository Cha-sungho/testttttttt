//###########################################################################
//
// FILE:   Phoenixon_GlobalFunc.c
//
//###########################################################################
#include "F28x_Project.h"
//#include "F2837xD_device.h"   ???
//#include "F2837xD_Gpio_defines.h"   ???

//################################################################################################
//bgkim-2020-01-29
//Trip On gpio
//Start Line
//
#define TRIP_ON_GPIO 0 //value 1 set Trip gpio setting
//
//bgkim-2020-01-29
//Trip On gpio
//End Line
//################################################################################################

void Init_Global_Gpio(void)
{
    EALLOW;

//******************  	GPA   *************************//
//	GPIO_SetupPinMux(31, GPIO_MUX_CPU1, 0);
    //GPAMUX1 Register (GPIO0 to 15)

    //GPAMUX2 Register (GPIO16 to 31)
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;   //DRV_Fault_4
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;   //DRV_Fault_5
//	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;   //PWM_CHA
//	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;   //PWM_DISCHA
//	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;   //DRV_Fault_1
//	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;   //DRV_Fault_2
//	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;   //DRV_Fault_3
//	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;   //Inverter PWM Enable
    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;   //DRV_Fault_1
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;   //DRV_Fault_2
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;   //DRV_Fault_3
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;   //DRV_Fault_6
//	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;   //LED 임시

    //GPADIR (GPIO0 to 31)
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 0;   //DRV_Fault_4
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;   //DRV_Fault_5
//	GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;   //PWM_CHA
//	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;   //PWM_DISCHA
//	GpioCtrlRegs.GPADIR.bit.GPIO20 = 0;   //DRV_Fault_1
//	GpioCtrlRegs.GPADIR.bit.GPIO21 = 0;   //DRV_Fault_2
//	GpioCtrlRegs.GPADIR.bit.GPIO22 = 0;   //DRV_Fault_3
//	GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;	  //Inverter PWM Enable
    GpioCtrlRegs.GPADIR.bit.GPIO24 = 0;   //DRV_Fault_1
    GpioCtrlRegs.GPADIR.bit.GPIO25 = 0;   //DRV_Fault_2
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;   //DRV_Fault_3
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;   //DRV_Fault_6
//	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;   //LED 임시

    //******************  	GPB   *************************//
    //GPBMUX1 Register (GPIO32 to 47)
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 2;	// /XWE0   /EM1CS0
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;	//
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	//RESET_AD
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 0;	//DSP LED
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;	//
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0;	// DIN01
    GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;	//DIN02
    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;	//DIN03
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;	//DOUT01
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;	//
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;	//AC_ON_LAMP
    GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;	//DOUT02
    GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0;	//DOUT03
    GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;	//MUX_A0
    GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0;	//Inverter PWM Enable

//	GPBMUX2 Register (GPIO48 to 63)
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;   	//Fuse_DC_Fault
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;   	//Fuse_AC_Fault
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;   	//Vreverce_Check
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;	//RS422_ENA
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;   //AC_TR_FAN
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;   //Pack_Sensing
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;   //DC_MAIN_MC
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;   //DRV_Fault_7
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;   //DRV_Fault_8
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;   //DRV_RDY_View
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;   //DRV_RST

    //GPBDIR (GPIO32 to 63)
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0;		//
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;		//RESET_AD
    GpioCtrlRegs.GPBDIR.bit.GPIO35 = 1;		//DSP LED
    GpioCtrlRegs.GPBDIR.bit.GPIO36 = 0;		//
    GpioCtrlRegs.GPBDIR.bit.GPIO38 = 0;		//DIN01
    GpioCtrlRegs.GPBDIR.bit.GPIO39 = 0;		//DIN02
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 0;		//DIN03
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1;		//DOUT01
    GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;		//
    GpioCtrlRegs.GPBDIR.bit.GPIO43 = 1;		//AC_ON_LAMP
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;		//DOUT02
    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;		//DOUT03
    GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;		//MUX_A0
    GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;		//Inverter PWM Enable
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 0;		//Fuse_DC_Fault
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0;		//Fuse_AC_Fault
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;   	//Vreverce_Check
    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;	    //RS422_ENA
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;   	//AC_TR_FAN
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;   	//Pack_Sensing
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;   	//DC_MAIN_MC
    GpioCtrlRegs.GPBDIR.bit.GPIO58 = 0;   	//DRV_Fault_7
    GpioCtrlRegs.GPBDIR.bit.GPIO59 = 0;   	//DRV_Fault_8
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;   	//DRV_RDY_View
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;   	//DRV_RST

    //******************  	GPC   *************************//
    //GPCMUX1 Register (GPIO64 to 79)
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;   //Grid_Soft_MC
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;   //Grid_Main_MC
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;   //LAMP_FAULT

    //GPCMUX2 Register (GPIO80 to 95)
    GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;  //HEAT_SRP_Fault
    GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;  //HEAT_SRS_Fault
    GpioCtrlRegs.GPCMUX2.bit.GPIO88 = 0;  //HEAT_DCDC_Fault & Fuse_DC_Fault
    GpioCtrlRegs.GPCMUX2.bit.GPIO89 = 0;  //Emergency Stop
    GpioCtrlRegs.GPCMUX2.bit.GPIO90 = 0;  //LAMP_DISCHARGE
    GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 0;  //SRC PWM Enable
    GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 0;  //DCDC PWM Enable

    //GPCDIR (GPIO64 to 95)
    GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;   //Grid_Soft_MC
    GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;   //Grid_Main_MC
    GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;   //LAMP_FAULT
    GpioCtrlRegs.GPCDIR.bit.GPIO86 = 0;   //HEAT_SRP_Fault
    GpioCtrlRegs.GPCDIR.bit.GPIO87 = 0;   //HEAT_SRS_Fault
    GpioCtrlRegs.GPCDIR.bit.GPIO88 = 0;   //HEAT_DCDC_Fault & Fuse_DC_Fault
    GpioCtrlRegs.GPCDIR.bit.GPIO89 = 0;   //Emergency Stop
    GpioCtrlRegs.GPCDIR.bit.GPIO90 = 1;   //LAMP_DISCHARGE
    GpioCtrlRegs.GPCDIR.bit.GPIO91 = 1;   //SRC PWM Enable
    GpioCtrlRegs.GPCDIR.bit.GPIO92 = 1;   //DCDC PWM Enable

    //******************  	GPD   *************************//
    //GPDMUX1 Register (GPIO96 to 111)
    GpioCtrlRegs.GPDMUX1.bit.GPIO99 = 0;  //LAMP_CAHRGE

    //GPDMUX2 Register (GPIO112 to 127)

    //GPDDIR (GPIO96 to 127)
    GpioCtrlRegs.GPDDIR.bit.GPIO99 = 1;  //LAMP_CAHRGE

    //******************  	GPE   *************************//
    //GPEMUX1 Register (GPIO128 to 143)
    GpioCtrlRegs.GPEMUX1.bit.GPIO133 = 0;  //LAMP_RUN

    //GPEMUX2 Register (GPIO144 to 159)

    //GPEDIR (GPIO128 to 159)
    GpioCtrlRegs.GPEDIR.bit.GPIO133 = 1;  //LAMP_RUN

    //******************  	GPF   *************************//
    //GPFMUX1 Register (GPIO160 to 168)

    //GPFDIR (GPIO160 to 168)

    EDIS;

//################################################################################################
//bgkim-2020-01-29
//Trip input port & xbar epwm mux setting
//Start Line
//
#if TRIP_ON_GPIO
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3;  // Asynch input

    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;  // Asynch input

    GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 3;  // Asynch input

    GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 3;  // Asynch input

    GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;  // Asynch input

    GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;    // Enable pull-up on
    GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;  // Asynch input

    InputXbarRegs.INPUT1SELECT = 16;// /DRV_SRP_FAULT
    InputXbarRegs.INPUT2SELECT = 17;// /DRV_SRS_FAULT
    InputXbarRegs.INPUT3SELECT = 24;// /DRV_R_FAULT
    InputXbarRegs.INPUT4SELECT = 25;// /DRV_S_FAULT
    InputXbarRegs.INPUT5SELECT = 26;// /DRV_T_FAULT
    InputXbarRegs.INPUT6SELECT = 27;// /DRV_DC1_FAULT

    EPwmXbarRegs.TRIP4MUXENABLE.bit.MUX1 = 1;
    EPwmXbarRegs.TRIP5MUXENABLE.bit.MUX3 = 1;
    EPwmXbarRegs.TRIP7MUXENABLE.bit.MUX5 = 1;
    EPwmXbarRegs.TRIP8MUXENABLE.bit.MUX7 = 1;
    EPwmXbarRegs.TRIP9MUXENABLE.bit.MUX9 = 1;
    EPwmXbarRegs.TRIP10MUXENABLE.bit.MUX11 = 1;

    EPwmXbarRegs.TRIP4MUX0TO15CFG.bit.MUX1 = 1;
    EPwmXbarRegs.TRIP5MUX0TO15CFG.bit.MUX3 = 1;
    EPwmXbarRegs.TRIP7MUX0TO15CFG.bit.MUX5 = 1;
    EPwmXbarRegs.TRIP8MUX0TO15CFG.bit.MUX7 = 1;
    EPwmXbarRegs.TRIP9MUX0TO15CFG.bit.MUX9 = 1;
    EPwmXbarRegs.TRIP10MUX0TO15CFG.bit.MUX11 = 1;
    EDIS;
#endif
//
//bgkim-2020-01-29
//Trip input port & xbar epwm mux setting
//End Line
//################################################################################################
}

void Init_Ext_Bus(void)
{
    // Make sure the XINTF clock is enabled

}




