#include "F28x_Project.h"
#include "Phoenixon_Constant.h"
#include "Phoenixon_Control.h"
#include "Phoenixon_IO_Ext.h"
#include "Phoenixon_Extern.h"
#include "Phoenixon_ADC.h"
#include "math.h"
#include "../main.h"

//INIT ADC PARAM
float tmp_a = 0., tmp_b = 0.;
float La_V = 0., Lb_V = 0., Wc_V = 2. * PI * 0.5;

//definitions for selecting DAC reference
#define REFERENCE_VDAC     0
#define REFERENCE_VREF     1

//Scale & Offset
float Va_scale = 0., Vb_scale = 0.;
float Vdc_scale = 0., Vsrc_scale = 0., Vbat_scale = 0., Vpack_scale = 0.;
float Ia_scale = 0., Ib_scale = 0., Ic_scale = 0.;
float Ibat_scale = 0., Ifan_scale = 0.,Ibat_Dis_scale =0.;

float Va_offset = 0., Vb_offset = 0.;
float Vdc_offset = 0., Vsrc_offset = 0., Vbat_offset = 0., Vpack_offset = 0.;
float Ia_offset = 0., Ib_offset = 0., Ic_offset = 0.;
float Ibat_offset = 0.,Ibat_Dis_offset = 0., Ifan_offset = 0.;

// Sum & Result
double Va_sum = 0., Vb_sum = 0., Vdc_sum = 0.;
double Vbat_sum = 0., Vsrc_sum = 0., Vpack_sum = 0., Ia_sum = 0., Ib_sum = 0., Ic_sum = 0., Ibat_sum = 0., Ifan_sum = 0.;
double Va_rms_sum = 0., Vb_rms_sum = 0., Ia_rms_sum = 0., Ib_rms_sum = 0., Ic_rms_sum = 0., Ifan_rms_sum = 0.;

// Real Value
float Va = 0., Vb = 0., Vc = 0., Ia = 0., Ib = 0., Ic = 0.;
float Vdc = 0, Vbat = 0., Vsrc = 0., Vpack = 0., Ibat = 0., Ifan = 0.;
float Temp_INV_R = 0., Temp_SRC_P = 0., Temp_SPC_S = 0.;
float Temp_DC_T1 = 0., Temp_DC_T3 = 0., Temp_DC_B1 = 0., Temp_DC_B3 = 0., Temp_H_TR = 0.;

// Rms & Check
float in_rms_scale = 0., Va_rms = 0., Vb_rms = 0.;
float Vdc_rms = 0., Vsrc_rms = 0., Vbat_rms = 0., Vpack_rms = 0., Ifan_rms = 0.;
float Ia_rms = 0., Ib_rms = 0., Ic_rms = 0., Ibat_rms = 0.;
int in_unit = 0, in_unitp = 0, in_period = 0, in_sum_cnt = 0;
int in_unit_cnt = 0;

//Read AD Value
int ad_offset_read = 0, ad_offset_read_cnt = 0, adc_cnt = 0, adc_cnt_max = 15, Etc_Sensing_flag = 0;
long Va_offset_sum = 0., Vb_offset_sum = 0.;
long Vdc_offset_sum = 0., Vsrc_offset_sum = 0., Vbat_offset_sum = 0., Vpack_offset_sum = 0.;
long Ia_offset_sum = 0., Ib_offset_sum = 0., Ic_offset_sum = 0., Ifan_offset_sum = 0., Ibat_offset_sum = 0.;

//Read AD result
float Va_result = 0., Vb_result = 0.;
float Vdc_result = 0., Vsrc_result = 0., Vbat_result = 0., Vpack_result = 0;
float Ia_result = 0., Ib_result = 0., Ic_result = 0., Ifan_result = 0., Ibat_result = 0.;
float Temp_INV_R_result = 0., Temp_SRC_P_result = 0., Temp_SPC_S_result = 0.;
float Temp_DC_T1_result = 0., Temp_DC_T3_result = 0., Temp_DC_B1_result = 0., Temp_DC_B3_result = 0., Temp_H_TR_result = 0.;

//Frequency Check
float Freq = 0., Freq_Fix = 0., Freq_cnt = 0, Freq_cnt1 = 0, Freq_sum = 0., Freq_add = 0.;

//Read_ad_UI_vaule
int UI_sum_cnt = 0, Mux_mode = 0;
float Vpack_UI_sum = 0., Ibat_UI_sum = 0., Vpack_UI_rms = 0., Ibat_UI_rms = 0., DC_Pow = 0., DC_Pow2 = 0.;
float Vpack_UI_ref = 0., Vpack_UI_ref_sum = 0., Vpack_UI_p0 = 0., Vpack_UI_p0_Clamp = 0., Vpack_UI_Clamp = 0., Vpack_UI_Clamp_val = 0.;
float Ibat_UI_ref = 0., Ibat_UI_ref_sum = 0., Ibat_UI_p0 = 0., Ibat_UI_p0_Clamp = 0., Ibat_UI_Clamp = 0., Ibat_UI_Clamp_val = 0.;
float UI_Vpack_ref_cnt = 0, UI_Ibat_ref_cnt = 0;
unsigned long Etc_Sensing_cnt = 0, Fan_chk_cnt = 0;



//#pragma CODE_SECTION(DSP_ADCRead, ".TI.ramfunc");
void DSP_ADC_Init(void)
{
    Va_scale = -0.020752586;//680Vpk:1.5Vpk => 680/32767 = 0.020752586  LV25-P/SP2
    Vb_scale = -0.020752586;

    //LAH100P :: 121.9512Apk:1.5Vpk => 121.9512/32767 = 0.0037217689
//    Ia_scale = -0.0037217689;
//    Ib_scale = -0.0037217689;
//    Ic_scale = -0.0037217689;

    //XXX Check datasheet
    //LA55P :: 60.9756/32767 = 0.001860884
//    Ia_scale = -0.0030519;              // Formation Inverter 기준
//    Ib_scale = -0.0030519;
//    Ic_scale = -0.0030519;

    Ia_scale = -0.0015259;              // Formation Inverter 기준
    Ib_scale = -0.0015259;
    Ic_scale = -0.0015259;

//    //    LA55P :: 312.5/32767 = 0.00957034
//    Ia_scale = -0.00957034;
//    Ib_scale = -0.00957034;
//    Ic_scale = -0.00957034;

    Vdc_scale = 0.011444266;//750Vpk:3Vpk => 750/65535 = 0.011444266 LV25-P/SP2
//    Vdc_scale = 0.012512398;//820/65535 = 0.012512398
    //Vdc_scale = 0.01373312;//900/65535 = 0.01373312
    Vbat_scale = 0.0038417;//250Vpk:3Vpk => 250/65535 = 0.003814755 LV25-P/SP2      // 사용 안함

    //MUX Selection//
    Vsrc_scale = 0.010376135;//680Vpk:3Vpk => 680/65535 = 0.010376135 LV25-P/SP2
//    Vsrc_scale = 0.011444266;//750/65535 = 0.011444266

    //XXX Check datasheet
//    Ifan_scale = 0.000572222;//HXN20-P_18.75Apk:3.75Vpk, 18.75Apk::1.5Vpk => 18.75/32767 = 0.000572222
    Ifan_scale = 0.0001907;      // Formation Inverter 기준 HXN20-P 10kohm


    Va_offset = 32698;          // 32767
    Vb_offset = 32577;

    Ia_offset = 32640;
    Ib_offset = 32806;
    Ic_offset = 32798;

    Vdc_offset = 0;
    Vbat_offset = -128;

    Vsrc_offset = 0;
    Ifan_offset = 32655;



    //Set ADCCLK divider to /4
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_8_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_8_0);
    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_8_0);
    ADC_setPrescaler(ADCD_BASE, ADC_CLK_DIV_8_0);

    //Set resolution and signal mode (see #defines above) and load corresponding trims.
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
    ADC_setMode(ADCC_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
    ADC_setMode(ADCD_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);

    //Set pulse positions to late
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCD_BASE, ADC_PULSE_END_OF_CONV);

    //Power up the ADCs and then delay for 1 ms
    ADC_enableConverter(ADCA_BASE);
    ADC_enableConverter(ADCB_BASE);
    ADC_enableConverter(ADCC_BASE);
    ADC_enableConverter(ADCD_BASE);

    DELAY_US(1000);

    /*
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);    //adc window max 512
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4_ADCIN5, 511);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER2);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);

    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);

    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER2, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);

    ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);

    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);
     */

    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);    //adc window max 512
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4_ADCIN5, 511);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER2);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);

    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);

    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER2, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);

    ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);

    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 511);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 511);
//    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER4, ADC_SOC_NUMBER1);
    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER4, ADC_SOC_NUMBER1);// differential
    ADC_enableInterrupt(ADCD_BASE, ADC_INT_NUMBER4);
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);
}

void Init_adc_param_18bit(void)
{
    //PLL
    tmp_a = Wc_V * Tsamp;
    tmp_b = 1. / (2. + tmp_a);
    La_V = (2. - tmp_a) * tmp_b;
    Lb_V = tmp_a * tmp_b;


    Vpack_scale = 0.0006889;//+180.9587/262144 = 0.00069030265       // Formation Inverter 기준 사용 안함

    //XXX Check datasheet
    //Ibat_scale = 0.0028488;//+300/86508 = 0.003467907 (@300A)
    //Ibat_Dis_scale = 0.00285253;

//    Ibat_scale = 0.0020402990;//200/80399 = 0.002487578 (@200A & 100A)
//    Ibat_Dis_scale = 0.0020393531;

    Ibat_scale = 0.0004783;              // Formation Inverter 기준  62.68748087 / ((2^17)-1)
    Ibat_Dis_scale = 0.0004783;


    Vpack_offset = 1292;
    Ibat_offset = 131009;//131072 (half of 18bit)
    Ibat_Dis_offset= 130999;


    GpioDataRegs.GPBSET.bit.GPIO34 = 1;//ADC reset signal
    for (adc_cnt = 0; adc_cnt < 20; adc_cnt++)
    {}
    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
}


Uint16 DSP_ADC_Read(float32 *Data)
{
    Uint16 ui_i = 0;
    Uint16 ui_ResultData[16] = { 0, };
    long l_TimeOut = 200000;

    //Convert, wait for completion, and store results
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER2);

    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER1);

    ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER0);

    ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER1);

    l_TimeOut = 200000;
    while (ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3) == false)
    {
        if (l_TimeOut-- <= 0)
            break;
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);

    l_TimeOut = 200000;
    while (ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2) == false)
    {
        if (l_TimeOut-- <= 0)
            break;
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    l_TimeOut = 200000;
    while (ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1) == false)
    {
        if (l_TimeOut-- <= 0)
            break;
    }
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);

    l_TimeOut = 200000;
    while (ADC_getInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4) == false)
    {
        if (l_TimeOut-- <= 0)
            break;
    }
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);

    //
    // Store results
    //
    ui_ResultData[0] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[1] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    ui_ResultData[2] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
    ui_ResultData[3] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[4] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);
    ui_ResultData[5] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[6] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    ui_ResultData[7] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);

    for (ui_i = 0; ui_i < 8; ui_i++)
    {
        Data[ui_i] = ui_ResultData[ui_i];
    }
    //
    // Software breakpoint. At this point, conversion results are stored in
    // adcAResult0, adcAResult1, adcBResult0, and adcBResult1.
    //
    // Hit run again to get updated conversions.
    //
    return (SUCCESS); //SUCCESS Or FAIL
}


//==============================================================
void Read_DSP_ad_16bit(void)
{
    Va_result = (float) ADC_Read_Val[0] - Va_offset;
    Vb_result = (float) ADC_Read_Val[1] - Vb_offset;

    Ia_result = (float) ADC_Read_Val[2] - Ia_offset;
    Ib_result = (float) ADC_Read_Val[3] - Ib_offset;
    Ic_result = (float) ADC_Read_Val[4] - Ic_offset;

    Vdc_result = (float) ADC_Read_Val[5] - Vdc_offset;
    Vbat_result = (float) ADC_Read_Val[6] - Vbat_offset;

    //MUX Selection//
    switch (Mux_mode)
    {
    case 0:
		Vsrc_result = (float)ADC_Read_Val[7]  - Vsrc_offset;
        MUX_A0_ON();
        Mux_mode = 1;
        break;

    case 1:
		Ifan_result = (float)ADC_Read_Val[7]  - Ifan_offset;
        MUX_A0_OFF();
        Mux_mode = 0;
        break;
    }

    Va = (float) (Va_result) * Va_scale;//real scaled value for ab phase
    Vb = (float) (Vb_result) * Vb_scale;//real scaled value for bc phase
    Vc = -(Va + Vb);//real scaled value for a ca phase

    Ia = (float) (Ia_result) * Ia_scale;
    Ib = (float) (Ib_result) * Ib_scale;
    Ic = (float) (Ic_result) * Ic_scale;
    //Ic = -(Ia + Ib);

    Vdc = (float) (Vdc_result) * Vdc_scale;
    Vbat = (float) (Vbat_result) * Vbat_scale;

    //MUX Selection//
    Vsrc = (float) (Vsrc_result) * Vsrc_scale;
    Ifan = (float) (Ifan_result) * Ifan_scale;
}


void Read_ad_value_18bit(void)
{
    float dump;

    AD7679_Conversion();

//	//for(adc_cnt = 0; adc_cnt < adc_cnt_max; adc_cnt++){}// add 180119 8  ADC Settling Time , 10 timer count can be saved by doing that.
//	DELAY_US(2);

    Vpack_result = (float) (AD7679_Read(0)) - Vpack_offset; //PACK ADC_CS0

    //Charge, Discharge에 따라 offset 따로 적용 최현수
    if (SCI_Charge_flag == 1)           { Ibat_result = (float) (AD7679_Read(1)) - Ibat_offset; }//IBAT ADC_CS1
    else if(SCI_Discharge_flag == 1)    { Ibat_result = (float) (AD7679_Read(1)) - Ibat_Dis_offset; }
    else                                { Ibat_result = (float) (AD7679_Read(1)) - Ibat_offset; }


    Etc_Sensing_cnt++;

    if (Etc_Sensing_cnt >= REF_TIME(0.2))
    {
        Etc_Sensing_flag = 1;
        Etc_Sensing_cnt = 0;
    }

    if (Etc_Sensing_flag == 1)
    {
        Temp_INV_R_result = (float) (AD7679_Read(2));
        Temp_SRC_P_result = (float) (AD7679_Read(2));
        Temp_SPC_S_result = (float) (AD7679_Read(2));
        Temp_DC_T1_result = (float) (AD7679_Read(2));
        Temp_DC_T3_result = (float) (AD7679_Read(2));
        Temp_DC_B1_result = (float) (AD7679_Read(2));
        Temp_DC_B3_result = (float) (AD7679_Read(2));
        Temp_H_TR_result = (float) (AD7679_Read(2));

        Etc_Sensing_flag = 0;
    }

    Vpack = (float) (Vpack_result) * Vpack_scale;

    //Charge, Discharge에 따라  scale 따로 적용 최현수
    if(SCI_Charge_flag == 1)            { Ibat = (float) (Ibat_result) * Ibat_scale; }//IBAT ADC_CS1}
    else if(SCI_Discharge_flag == 1)    { Ibat = (float) (Ibat_result) * Ibat_Dis_scale; }
    else                                { Ibat = (float) (Ibat_result) * Ibat_scale; }


    {//termperature - DouB
//        Temp_INV_R = Temp_INV_R_result * 0.0001525925;
//        Temp_SRC_P = Temp_SRC_P_result * 0.0001525925;
//        Temp_SPC_S = Temp_SPC_S_result * 0.0001525925;
//        Temp_DC_T1 = Temp_DC_T1_result * 0.0001525925;
//        Temp_DC_T3 = Temp_DC_T3_result * 0.0001525925;
//        Temp_DC_B1 = Temp_DC_B1_result * 0.0001525925;
//        Temp_DC_B3 = Temp_DC_B3_result * 0.0001525925;
//        Temp_H_TR = Temp_H_TR_result * 0.0001525925;


            dump = Temp_INV_R_result * 0.0001525925;//converted to voltage
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;//converted to resister
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;//converted to temperature
        Temp_INV_R += (dump - Temp_INV_R) * 0.2;//filtering
            dump = Temp_SRC_P_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_SRC_P += (dump - Temp_SRC_P) * 0.2;
            dump = Temp_SPC_S_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_SPC_S += (dump - Temp_SPC_S) * 0.2;
            dump = Temp_DC_T1_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_DC_T1 += (dump - Temp_DC_T1) * 0.2;
            dump = Temp_DC_T3_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_DC_T3 += (dump - Temp_DC_T3) * 0.2;
            dump = Temp_DC_B1_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_DC_B1 += (dump - Temp_DC_B1) * 0.2;
            dump = Temp_DC_B3_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_DC_B3 += (dump - Temp_DC_B3) * 0.2;
            dump = Temp_H_TR_result * 0.0001525925;
            dump = ((5000 * dump) / (3.3 - dump)) / 10000.0;
            dump = (1 / (0.003354017 + (0.000291121 * logf(dump)))) - 273.15;
        Temp_H_TR += (dump - Temp_H_TR) * 0.2;
    }

//*****   Init_AD OFFSET READ   ******//
    if (ad_offset_read)
    {
        if (ad_offset_read_cnt++ <= 4096)
        {
            Va_offset_sum += Va_result;
            Vb_offset_sum += Vb_result;
            Vdc_offset_sum += Vdc_result;
            Vsrc_offset_sum += Vsrc_result;
            Vbat_offset_sum += Vbat_result;
            Vpack_offset_sum += Vpack_result;

            Ia_offset_sum += Ia_result;
            Ib_offset_sum += Ib_result;
            Ic_offset_sum += Ic_result;
            Ibat_offset_sum += Ibat_result;
            Ifan_offset_sum += Ifan_result;
        }
        else
        {
            Va_offset = ((long) (Va_offset_sum) >> 12);
            Vb_offset = ((long) (Vb_offset_sum) >> 12);
            Vdc_offset = ((long) (Vdc_offset_sum) >> 12);
            Vsrc_offset = ((long) (Vsrc_offset_sum) >> 12);
            Vbat_offset = ((long) (Vbat_offset_sum) >> 12);
            Vpack_offset = ((long) (Vpack_offset_sum) >> 12);

            Ia_offset = ((long) (Ia_offset_sum) >> 12);
            Ib_offset = ((long) (Ib_offset_sum) >> 12);
            Ic_offset = ((long) (Ic_offset_sum) >> 12);
            Ibat_offset = ((long) (Ibat_offset_sum) >> 12);
            Ifan_offset = ((long) (Ifan_offset_sum) >> 12);

            Va_offset_sum = 0.;
            Vb_offset_sum = 0.;
            Vdc_offset_sum = 0.;
            Vsrc_offset_sum = 0.;
            Vbat_offset_sum = 0.;
            Vpack_offset_sum = 0.;

            Ia_offset_sum = 0.;
            Ib_offset_sum = 0.;
            Ic_offset_sum = 0.;
            Ibat_offset_sum = 0.;
            Ifan_offset_sum =0.;

            ad_offset_read = 0;
            ad_offset_read_cnt = 0;
        }
    }
}

void Read_ad_UI_vaule(void)
{
    DC_Pow = Ibat * Vpack;
    DC_Pow2 = Ibat_UI_rms * Vpack_UI_rms;

    Vpack_UI_sum += Vpack;
    Ibat_UI_sum += Ibat;
    UI_sum_cnt++;

    if (UI_sum_cnt >= 50)
    {
        Vpack_UI_p0 = Vpack_UI_sum * 0.02;
        Vpack_UI_sum = 0.;
        Ibat_UI_p0 = Ibat_UI_sum * 0.02;
        Ibat_UI_sum = 0.;
        UI_sum_cnt = 0;

        //##### Vbat1_UI_rms_Clamping ##### 0.05, 0.01, 0.01 //
        if ((Vpack_UI_p0 > Vpack_UI_ref - 0.8) && (Vpack_UI_p0 < Vpack_UI_ref + 0.8))
        {   //1.
            Vpack_UI_p0_Clamp = 0.5;	// 0.1;

            if ((Vpack_UI_p0 > Vpack_UI_ref - 0.4) && (Vpack_UI_p0 < Vpack_UI_ref + 0.4))
            {  //0.6
                Vpack_UI_p0_Clamp = 0.2; // 0.05;

                if ((Vpack_UI_p0 > Vpack_UI_ref - 0.2) && (Vpack_UI_p0 < Vpack_UI_ref + 0.2))
                {  //0.2
                    Vpack_UI_p0_Clamp = 0.01; // 0.01;  -> 0.005;  0.01;
                }
            }

            UI_Vpack_ref_cnt++;

            Vpack_UI_Clamp = Vpack_UI_ref + ((Vpack_UI_p0 - Vpack_UI_ref) * Vpack_UI_p0_Clamp);
            Vpack_UI_Clamp_val = Vpack_UI_Clamp;
            Vpack_UI_ref_sum += Vpack_UI_Clamp;

            if (UI_Vpack_ref_cnt >= 2)
            {
                Vpack_UI_ref = Vpack_UI_ref_sum * 0.5;
                UI_Vpack_ref_cnt = 0;
                Vpack_UI_ref_sum = 0;
            }
        }
        else
        {
            UI_Vpack_ref_cnt = 0;
            Vpack_UI_ref_sum = 0;
            Vpack_UI_Clamp_val = Vpack_UI_p0;
            Vpack_UI_ref = Vpack_UI_p0;
        }

        //##### Ibat1_UI_rms_Clamping ##### 0.01, 0.05, 0.2 //
        if ((Ibat_UI_p0 > Ibat_UI_ref - 0.8) && (Ibat_UI_p0 < Ibat_UI_ref + 0.8))
        { //0.5: +-1~+-0.4 => Value*0.5(+-0.5~+-0.2)
            Ibat_UI_p0_Clamp = 0.5;

            if ((Ibat_UI_p0 > Ibat_UI_ref - 0.4) && (Ibat_UI_p0 < Ibat_UI_ref + 0.4))
            { //0.625: +-0.4~+-0.2 => Value*0.625(+-0.25~+-0.125)
                Ibat_UI_p0_Clamp = 0.2;

                if ((Ibat_UI_p0 > Ibat_UI_ref - 0.2) && (Ibat_UI_p0 < Ibat_UI_ref + 0.2))
                {
                    Ibat_UI_p0_Clamp = 0.05;  //0.1;
                }
            }

            UI_Ibat_ref_cnt++;

            Ibat_UI_Clamp = Ibat_UI_ref + ((Ibat_UI_p0 - Ibat_UI_ref) * Ibat_UI_p0_Clamp);
            Ibat_UI_Clamp_val = Ibat_UI_Clamp;
            Ibat_UI_ref_sum += Ibat_UI_Clamp;

            if (UI_Ibat_ref_cnt >= 5)
            {
                Ibat_UI_ref = Ibat_UI_ref_sum * 0.2;
                UI_Ibat_ref_cnt = 0;
                Ibat_UI_ref_sum = 0;
            }
        }
        else
        {
            UI_Ibat_ref_cnt = 0;
            Ibat_UI_ref_sum = 0;
            Ibat_UI_Clamp_val = Ibat_UI_p0;
            Ibat_UI_ref = Ibat_UI_p0;
        }

        Ibat_UI_rms = fabsf(Ibat_UI_Clamp_val);
        Vpack_UI_rms = Vpack_UI_Clamp_val;

        if (sys_mode <= 3)
        {
            if (Vpack_UI_Clamp_val < 3.)
            {
                Vpack_UI_Clamp_val = 0.;
            }

            Vpack_UI_rms = Vpack_UI_Clamp_val;
        }

        if (sys_mode <= 7)
        {
            if (Ibat_UI_Clamp_val < 2.)
            {
                Ibat_UI_Clamp_val = 0.;
            }  //1 -> 2

            Ibat_UI_rms = Ibat_UI_Clamp_val;
        }
    }
}


float Vdc_r = 0;

//void Rms_value(void)
//{
//	Vdc_r = (Vdc_r * 0.842519) + (Vdc * 0.157481);//DouB ; LPF (300Hz @ 11kHz)
//
//    Va_sum += (double) (Va * Va);
//    Vb_sum += (double) (Vb * Vb);
//    Vdc_sum += (double) Vdc;
//    Vsrc_sum += (double) Vsrc;
//    Vbat_sum += (double) Vbat;
//    Vpack_sum += (double) Vpack;
//
//    Ia_sum += (double) (Ia * Ia);
//    Ib_sum += (double) (Ib * Ib);
//    Ic_sum += (double) (Ic * Ic);
//    Ibat_sum += (double) Ibat;
//
//    Ifan_sum += (double) (Ifan * Ifan);
//
//    in_unitp = in_unit;
//
//    if (sin_pll > 0.)   { in_unit = 1; }//전반 반주기동안 SET
//    else                { in_unit = 0; }//후반 반주기동안 CLEAR
//
//    if (in_unit * (!in_unitp))//'1&0'조건 == 한주기의 시작점
//    {
//        in_period = in_unit_cnt;
//        in_unit_cnt = 0;
//        in_sum_cnt++;
//    }
//
//    switch (in_unit_cnt++)
//    {
//    case 1:
//        in_rms_scale = 1. / (float) in_period;
//        break;
//
//    case 2:
//        Va_rms_sum += sqrt(Va_sum * in_rms_scale);
//        Va_sum = 0.;
//        break;
//
//    case 3:
//        Vb_rms_sum += sqrt(Vb_sum * in_rms_scale);
//        Vb_sum = 0.;
//        break;
//
//    case 4:
//        if (in_sum_cnt >= 10)
//        {
//            Va_rms = Va_rms_sum * 0.1;
//            Va_rms_sum = 0.;
//            Vb_rms = Vb_rms_sum * 0.1;
//            Vb_rms_sum = 0.;
//        }
//        break;
//
//    case 5:
//        Vdc_rms = (Vdc_sum * in_rms_scale);
//        Vdc_sum = 0.;
//        break;
//
//    case 6:
//        Vbat_rms = (Vbat_sum * in_rms_scale);
//        Vbat_sum = 0.;
//        break;
//
//    case 7:
//        Vsrc_rms = (Vsrc_sum * in_rms_scale);
//        Vsrc_sum = 0.;
//        break;
//
//    case 8:
//        Vpack_rms = (Vpack_sum * in_rms_scale);
//        Vpack_sum = 0.;
//        break;
//
//    case 9:
//        Ia_rms_sum += sqrt(Ia_sum * in_rms_scale);
//        Ia_sum = 0.;
//        break;
//
//    case 10:
//        Ib_rms_sum += sqrt(Ib_sum * in_rms_scale);
//        Ib_sum = 0.;
//        break;
//
//    case 11:
//        Ic_rms_sum += sqrt(Ic_sum * in_rms_scale);
//        Ic_sum = 0.;
//        break;
//
//    case 12:
//        Ifan_rms_sum += sqrt(Ifan_sum * in_rms_scale);
//        Ifan_sum = 0.;
//        break;
//
//    case 13:
//        if (in_sum_cnt >= 10)
//        {
//            Ia_rms = Ia_rms_sum * 0.1;
//            Ia_rms_sum = 0.;
//            Ib_rms = Ib_rms_sum * 0.1;
//            Ib_rms_sum = 0.;
//            Ic_rms = Ic_rms_sum * 0.1;
//            Ic_rms_sum = 0.;
//            Ifan_rms = Ifan_rms_sum * 0.1;
//            Ifan_rms_sum = 0.;
//            in_sum_cnt = 0;
//        }
//        break;
//
//    case 14:
//        Ibat_rms = (Ibat_sum * in_rms_scale);
//        Ibat_sum = 0.;
//        break;
//
//    case 15:
//        Freq_sum += 1. / (in_period * (Tsamp));
//
//        Freq_cnt++;
//        if (Freq_cnt >= 3)
//        {
//            Freq = Freq_sum * inv3;
//
//            Freq_sum = 0.;
//            Freq_cnt = 0;
//
//            if ((sys_mode == 1) && (freq_check_flag == 0))
//            {
//                Freq_cnt1++;
//
//                Freq_add += Freq;
//
//                if (Freq_cnt1 >= 50)
//                {//  initial Freq Check(0.8X3 => 2.4s)
//                    Freq_Fix = Freq_add * 0.02;
//
//                    if (Freq_Fix >= 57)
//                    {
//                        freq_60Hz = 1;
//                        freq_check_flag = 1;
//                        Freq_cnt1 = 0;
//                        Freq_add = 0.;
//                    }
//                    else if (Freq_Fix <= 53)
//                    {
//                        freq_50Hz = 1;
//                        freq_check_flag = 1;
//                        Freq_cnt1 = 0;
//                        Freq_add = 0.;
//                    }
//                }
//            }
//        }
//        break;
//
//    default:
//        break;
//    }
//}



int cnt_rms = 0;

void Rms_value(void)
{
	    Vdc_rms = (Vdc_rms * 0.95) + (Vdc * 0.05);
		Vsrc_rms = (Vsrc_rms * 0.95) + (Vsrc * 0.05);

		//--condition #1
#if (Formation_Inverter == 0)
		Vbat_rms = (Vbat_rms * 0.98) + (Vbat * 0.02);
		Vpack_rms = (Vpack_rms * 0.98) + (Vpack * 0.02);
#else
		Vbat_rms = 10.0;        // Formation Inverter용. 임의의 값
		Vpack_rms = 0.5;        // Formation Inverter용. 임의의 값
#endif
		Ibat_rms = (Ibat_rms * 0.98) + (Ibat * 0.02);   // Formation Inverter에서는 SRC 출력 전류로 사용

		//--condition #2
//		Vbat_rms = (Vbat_rms * 0.9725) + (Vbat * 0.0275);
//		Vpack_rms = (Vpack_rms * 0.9725) + (Vpack * 0.0275);
//		Ibat_rms = (Ibat_rms * 0.9725) + (Ibat * 0.0275);

	    Va_sum += (double)(Va * Va);
	    Vb_sum += (double)(Vb * Vb);

	    Ia_sum += (double)(Ia * Ia);
	    Ib_sum += (double)(Ib * Ib);
	    Ic_sum += (double)(Ic * Ic);

		Ifan_sum += (double)(Ifan * Ifan);

		Va_rms = (Va_rms * 0.95) + (Va_rms_sum * 0.05);
		Vb_rms = (Vb_rms * 0.95) + (Vb_rms_sum * 0.05);

		Ia_rms = (Ia_rms * 0.95) + (Ia_rms_sum * 0.05);
		Ib_rms = (Ib_rms * 0.95) + (Ib_rms_sum * 0.05);
		Ic_rms = (Ic_rms * 0.95) + (Ic_rms_sum * 0.05);

		Ifan_rms = (Ifan_rms * 0.95) + (Ifan_rms_sum * 0.05);

	switch (cnt_rms++)
	{
	case 1:
		in_rms_scale = 1. / 270.;
		break;

	case 2:
		Va_rms_sum = sqrt(Va_sum * in_rms_scale);
		Va_sum = 0.;
		break;

	case 3:
		Vb_rms_sum = sqrt(Vb_sum * in_rms_scale);
		Vb_sum = 0.;
		break;

	case 4:
		Ia_rms_sum = sqrt(Ia_sum * in_rms_scale);
		Ia_sum = 0.;
		break;

	case 5:
		Ib_rms_sum = sqrt(Ib_sum * in_rms_scale);
		Ib_sum = 0.;
		break;

	case 6:
		Ic_rms_sum = sqrt(Ic_sum * in_rms_scale);
		Ic_sum = 0.;
		break;

	case 7:
		Ifan_rms_sum = sqrt(Ifan_sum * in_rms_scale);
		Ifan_sum = 0.;
		break;

	case 270:
		cnt_rms = 0;
		in_sum_cnt++;
		break;
	}


    in_unitp = in_unit;
    in_unit = (sin_pll > 0.);
    if (in_unit * (!in_unitp))//'1&0'조건 == 한주기의 시작점
    {
        in_period = in_unit_cnt;
        in_unit_cnt = 0;
        in_sum_cnt++;
    }

    switch (in_unit_cnt++)
    {
    case 100:
        Freq_sum += 1. / (in_period * (Tsamp));

        Freq_cnt++;
        if (Freq_cnt >= 3)
        {
            Freq = Freq_sum * inv3;

            Freq_sum = 0.;
            Freq_cnt = 0;

            if ((sys_mode == 1) && (freq_check_flag == 0))
            {
                Freq_cnt1++;

                Freq_add += Freq;

                if (Freq_cnt1 >= 50)
                {//  initial Freq Check(0.8X3 => 2.4s)
                    Freq_Fix = Freq_add * 0.02;

                    if (Freq_Fix >= 57)
                    {
                        freq_60Hz = 1;
                        freq_check_flag = 1;
                        Freq_cnt1 = 0;
                        Freq_add = 0.;
                    }
                    else if (Freq_Fix <= 53)
                    {
                        freq_50Hz = 1;
                        freq_check_flag = 1;
                        Freq_cnt1 = 0;
                        Freq_add = 0.;
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}




