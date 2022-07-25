//###########################################################################
//
// FILE:   Phoenixon_IO_Ext.h
//
//###########################################################################

#ifndef __Phoenixon_IO_EXT_H__
    #define __PhoenixonIO_EXT_H__

    #define ADC_CSA    0x100000
    #define ADC_CSB    0x100200
    #define ADC_CSC    0x100400
    #define ADC_CSD    0x100600
    #define ADC_CSE    0x100800
    #define ADC_CSF    0x100A00
    #define ADC_CSG    0x100C00
    #define ADC_CSH    0x100E00
    #define ADC_CSI    0x101000
    #define ADC_CSJ    0x101200
    #define ADC_CSK    0x101400
    #define ADC_CSL    0x101600

    //WARNING ERROR CODE
    #define Pack_Reverse_Fault		0  //1	  Vpack_Reverse
    #define Pack_contact_Fault		1  //2 	  Vpack_Unconnacted
    #define	Ibat_Over_Operating		2  //4	  Over Operating Current
    #define	Pack_Over_Operating		3  //8	  Over_Operating_Voltage
    #define Charge_CV_Under_Fault   4  //10   Charge CV under then Pack Voltage
    #define Discharge_CV_Over_Fault 5  //20   Discharge CV Over then Pack Voltage
//    #define	System_Mode_Fault		6  //40	  Over_Operating_Voltage

    //GRID ERROR CODE
    #define PLL_Error			    0   //1     Pll Error
    #define Initial_Charge_Error    1   //2     Init Charge Fault
    #define IA_OverCurrent 			2   //4     Grid A OC
    #define IB_OverCurrent 			3   //8     Grid B OC
    #define IC_OverCurrent 			4   //10    Grid C OC
    #define Vgrid_OverVoltage       5   //20    Input Voltage Unbalance
    #define Vgrid_UnderVoltage      6   //40    Input Voltage Unbalance
    #define Heat_TR_Fault           7   //80    Heat Over
    #define Fuse_AC_Fault           8   //100   Fan Broken
    #define INV_MOSFET_Overtemp     9   //200   Inverter MOSFET Overtemp
    #define Vqe_Unbalance          10   //400   Grid Unbalance
    #define Vde_Unbalance          11   //800   Grid Unbalance

    //DC ERROR CODE
    #define Vdc_OverVoltage         0   //1     DC Link OV
    #define Vdc_UnderVoltage        1   //2     DC Link UV
    #define Vsrc_OverVoltage        2   //4     Vsrc Link OV
    #define Vsrc_UnderVoltage       3   //8     Vsrc Link UV
    #define Vbat_OverVoltage        4   //10    Bat OV
    #define Vpack_OverVoltage       5   //20    Vpack OV
    #define Ibat_OverCurrent        6   //40    Ibat OC
    #define Fuse_DC_Fault           7   //80    Fuse Broken
    #define SRC_MOSFET_Overtemp     8   //100   SRC MOSFET Overtemp
    #define DCDC_MOSFET_Overtemp    9   //200   DC/DC MOSFET Overtemp

    //FAULT ERROR CODE
    #define Emergency_Stop          0  //1      Emergency Stop
    #define	Heat_Fan_Fault   		1  //2      Inverter heatsink overtemp
    #define comm_chk_Fault         11  //800    PC-DSP Communication check Fault

    //IGBT ERROR CODE
    #define Drive_Fault_R          0   //1      Drive_Fault_1
    #define Drive_Fault_S		   1   //2		Drive_Fault_2
    #define Drive_Fault_T		   2   //4		Drive_Fault_3
    #define Drive_Fault_SRCP	   3   //8		Drive_Fault_4
    #define Drive_Fault_SRCS	   4   //10  	Drive_Fault_5
    #define Drive_Fault_DCDC_T	   5   //20 	Drive_Fault_6
    #define Drive_Fault_DCDC_B	   6   //40 	Drive_Fault_A
    #define Drive_Fault_C		   7   //80 	Drive_Fault_B
//    #define Drive_Fault_D		   8   //100	Drive_Fault_C
//    #define Drive_Fault_E		   9   //200	Drive_Fault_D
//    #define Drive_Fault_F          10  //400    Drive_Fault_E


    /****************  Control  Check   ***********************/
    #define Vreverce_Chk_ON()  GpioDataRegs.GPBSET.bit.GPIO53 = 1;  //Reverce_check
    #define Vreverce_Chk_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;
    #define Vreverce_Chk_Stat() GpioDataRegs.GPBDAT.bit.GPIO53

    #define Spaer_Chk_ON()  GpioDataRegs.GPBSET.bit.GPIO59 = 1;   //Reverce_check, Discharge
    #define Spaer_Chk_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;

    #define Current_H_Ctrl_ON()  GpioDataRegs.GPBSET.bit.GPIO60 = 1;   //Discharge
    #define Current_H_Ctrl_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;

    #define Current_L_Ctrl_ON()  GpioDataRegs.GPBSET.bit.GPIO61 = 1;
    #define Current_L_Ctrl_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;

    #define PWM_INV_Enable()  GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;
    #define PWM_INV_Disable() GpioDataRegs.GPBSET.bit.GPIO47 = 1;

    #define PWM_SRC_Enable()  GpioDataRegs.GPCCLEAR.bit.GPIO91 = 1;
    #define PWM_SRC_Disable() GpioDataRegs.GPCSET.bit.GPIO91 = 1;

    #define PWM_DCDC_Enable()  GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1;
    #define PWM_DCDC_Disable() GpioDataRegs.GPCSET.bit.GPIO92 = 1;

    /****************  Digital OUTPUT      ***********************/

    //#define Parll_Pattern_Sync_ON()  GpioDataRegs.GPBSET.bit.GPIO41 = 1;
    //#define Parll_Pattern_Sync_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO41 = 1;

    //#define Parll_CV_Out_ON()  GpioDataRegs.GPBSET.bit.GPIO44 = 1;
    //#define Parll_CV_Out_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;

    //#define Parll_Fault_Out_ON()  GpioDataRegs.GPBSET.bit.GPIO45 = 1;
    //#define Parll_Fault_Out_OFF()  GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;

    #define D_Out_ON()  GpioDataRegs.GPBSET.bit.GPIO41 = 1;
    #define D_Out_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO41 = 1;

    #define D_Out2_ON()  GpioDataRegs.GPBSET.bit.GPIO44 = 1;
    #define D_Out2_OFF() GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;

    #define D_Out3_ON()  GpioDataRegs.GPBSET.bit.GPIO45 = 1;
    #define D_Out3_OFF()  GpioDataRegs.GPBCLEAR.bit.GPIO45 = 1;

    /****************  Input/Output MC     ***********************/

    //#define All_MC_OFF() {TCA9535_IOPort_Setting(0, 0);}

    #define Grid_MAIN_MC_ON()      GpioDataRegs.GPCSET.bit.GPIO65 = 1;
    #define Grid_MAIN_MC_OFF()     GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;

    #define Grid_SOFT_MC_ON()      GpioDataRegs.GPCSET.bit.GPIO64 = 1;
    #define Grid_SOFT_MC_OFF()     GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;

    #define CHAR_DC_MAIN_MC_ON()   GpioDataRegs.GPBSET.bit.GPIO57 = 1;
    #define CHAR_DC_MAIN_MC_OFF()  GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;

    #define Pack_Sensing_ON()      GpioDataRegs.GPBSET.bit.GPIO56 = 1;
    #define Pack_Sensing_OFF()     GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;

    #define AC_HS_FAN_ON()         GpioDataRegs.GPBSET.bit.GPIO55 = 1;
    #define AC_HS_FAN_OFF()        GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;

    #define RS422_Comm_ON()         GpioDataRegs.GPBSET.bit.GPIO54 = 1;
    #define RS422_Comm_OFF()        GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;

    #define Pack_Parall_Sensing_ON()    GpioDataRegs.GPBSET.bit.GPIO56 = 1;
    #define Pack_Parall_Sensing_OFF()   GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;

    #define CHAR_DC_OUT_MC_ON()
    #define CHAR_DC_OUT_MC_OFF()

    #define CHAR_DC_SOFT_MC_ON()
    #define CHAR_DC_SOFT_MC_OFF()

    #define IGBT_SOURCE_ON()
    #define IGBT_SOURCE_OFF()

    #define CHAR_DC_OUT_Parall_MC_ON()
    #define CHAR_DC_OUT_Parall_MC_OFF()

    #define CV_OFF_Resistor_ON()
    #define CV_OFF_Resistor_OFF()

    #define CV_Resistor_ON()
    #define CV_Resistor_OFF()

    /****************  Etc     ***********************/
    #define Led_Low()      			GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;
    #define Led_High()     			GpioDataRegs.GPBSET.bit.GPIO35 = 1;

    //AC input status - 43
    #define AC_RUN_ON()			    GpioDataRegs.GPBSET.bit.GPIO43 = 1;
    #define AC_RUN_OFF()			GpioDataRegs.GPBCLEAR.bit.GPIO43 = 1;

    //READY status - 133
    #define LAMP_RUN_ON()			GpioDataRegs.GPESET.bit.GPIO133 = 1;
    #define LAMP_RUN_OFF()			GpioDataRegs.GPECLEAR.bit.GPIO133 = 1;

    //FAULT status - 66
//    #define LAMP_FAULT_ON() 		GpioDataRegs.GPDSET.bit.GPIO99 = 1;
//    #define LAMP_FAULT_OFF()		GpioDataRegs.GPDCLEAR.bit.GPIO99 = 1;
    #define LAMP_FAULT_ON()         GpioDataRegs.GPCSET.bit.GPIO66 = 1;         // Formation Inverter   Fault - RED LED
    #define LAMP_FAULT_OFF()        GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;

    //CHARGE status - 99
//    #define LAMP_CHARGE_ON() 		GpioDataRegs.GPCSET.bit.GPIO90 = 1;
//    #define LAMP_CHARGE_OFF()		GpioDataRegs.GPCCLEAR.bit.GPIO90 = 1;
    #define LAMP_CHARGE_ON()        GpioDataRegs.GPDSET.bit.GPIO99 = 1;         // Formation Inverter   AC Input On - Blue LED
    #define LAMP_CHARGE_OFF()       GpioDataRegs.GPDCLEAR.bit.GPIO99 = 1;

    //DISCHARGE status - 90
//    #define LAMP_DISCHARGE_ON()		GpioDataRegs.GPCSET.bit.GPIO66 = 1;
//    #define LAMP_DISCHARGE_OFF()	GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;
    #define LAMP_DISCHARGE_ON()     GpioDataRegs.GPCSET.bit.GPIO90 = 1;         // Formation Inverter   Run - Green LED
    #define LAMP_DISCHARGE_OFF()    GpioDataRegs.GPCCLEAR.bit.GPIO90 = 1;

    //ADC MUX
    #define MUX_A0_ON()				GpioDataRegs.GPBSET.bit.GPIO46 = 1;
    #define MUX_A0_OFF()			GpioDataRegs.GPBCLEAR.bit.GPIO46 = 1;

    //MOSFET DRV Rest signal
    #define Driver_Set()  			GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;
    #define Driver_Reset() 			GpioDataRegs.GPBSET.bit.GPIO61 = 1;
#endif




