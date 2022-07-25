/*
 * drv_adc.c
 *
 *  Created on: Nov 8, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "drv_adc.h"

#include "util_adc.h"

#ifdef CPU2
#ifdef _FLASH
#pragma CODE_SECTION(drv_SocDspAdcAll, ".TI.ramfunc");
#pragma CODE_SECTION(drv_GetDspAdcAll, ".TI.ramfunc");
#pragma CODE_SECTION(drv_GetFullSeqDspAdcAll, ".TI.ramfunc");
#pragma CODE_SECTION(drv_GetFullSeqDspAdcMain, ".TI.ramfunc");
#pragma CODE_SECTION(drv_GetFullSeqDspAdcSub, ".TI.ramfunc");
#endif
#endif

void drv_InitADC(void)
{
#ifdef CPU1
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 1, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 2, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 3, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 4, SYSCTL_CPUSEL_CPU2);

    //DSP internal temperature sensor
    ASysCtl_enableTemperatureSensor();
#endif
#ifdef CPU2
    //Set ADCCLK divider to /4
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCD_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode (see #defines above) and load corresponding trims.
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
    ADC_setMode(ADCC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setMode(ADCD_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

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

    //ADC A
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 64);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 64);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4_ADCIN5, 64);
    //DSP internal temperature sensor
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN13, 140);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER3);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //ADC B
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0_ADCIN1, 64);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2_ADCIN3, 64);

    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER2, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    //ADC C
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 15);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 15);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4, 15);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN14, 15);
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER6, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN15, 15);

    ADC_setInterruptSource(ADCC_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER6);
    ADC_enableInterrupt(ADCC_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3);

    //ADC D
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 15);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN1, 15);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN2, 15);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN3, 15);
    ADC_setupSOC(ADCD_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN4, 15);

    ADC_setInterruptSource(ADCD_BASE, ADC_INT_NUMBER4, ADC_SOC_NUMBER4);
    ADC_enableInterrupt(ADCD_BASE, ADC_INT_NUMBER4);
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);
#endif
}

void drv_SocDspAdcAll(void)
{
    //ADC A
    ADC_forceMultipleSOC(ADCA_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3));
    //ADC B
    ADC_forceMultipleSOC(ADCB_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));
    //ADC C
    ADC_forceMultipleSOC(ADCC_BASE, (ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5 | ADC_FORCE_SOC6));
    //ADC D
    ADC_forceMultipleSOC(ADCD_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4));
}

void drv_GetDspAdcAll(uint32_t *raw)
{
    uint32_t timeout = 200000;

    //ADC A
    while (ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //ADC B
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    //ADC C
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3);

    //ADC D
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);

    //ADC A
    raw[dsp_adc_out1_amps]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_temp]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_ext_diff]       = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);

    //ADC B
    raw[dsp_adc_out2_amps]      = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_volts]     = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);

    //ADC C
    raw[dsp_adc_15va]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_5va]            = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_12vd]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC D
    raw[dsp_adc_dc_link_pri_f]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_dc_link_pri_r]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_dc_link_sec]    = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_out_cap_volts]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_batt_neg_volts] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC A
    raw[dsp_adc_dsp_tempsense]  = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3); //DSP internal temperature sensor

    //ADC C
    raw[dsp_adc_ext1_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5); //Ext ADC 1
    raw[dsp_adc_ext2_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER6); //Ext ADC 2
}

void drv_GetFullSeqDspAdcAll(uint32_t *raw)
{
    uint32_t timeout = 200000;

    //ADC A
    ADC_forceMultipleSOC(ADCA_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3));
    //ADC B
    ADC_forceMultipleSOC(ADCB_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));
    //ADC C
    ADC_forceMultipleSOC(ADCC_BASE, (ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5 | ADC_FORCE_SOC6));
    //ADC D
    ADC_forceMultipleSOC(ADCD_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4));

    //ADC A
    while (ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //ADC B
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    //ADC C
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3);

    //ADC D
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);

    //ADC A
    raw[dsp_adc_out1_amps]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_temp]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_ext_diff]       = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);

    //ADC B
    raw[dsp_adc_out2_amps]      = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_volts]     = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);

    //ADC C
    raw[dsp_adc_15va]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_5va]            = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_12vd]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC D
    raw[dsp_adc_dc_link_pri_f]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_dc_link_pri_r]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_dc_link_sec]    = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_out_cap_volts]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_batt_neg_volts] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC A
    raw[dsp_adc_dsp_tempsense]  = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3); //DSP internal temperature sensor
    //ADC D
    raw[dsp_adc_ext1_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5); //Ext ADC 1
    raw[dsp_adc_ext2_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER6); //Ext ADC 2
}

void drv_GetFullSeqDspAdcMain(uint32_t *raw)
{
    uint32_t timeout = 200000;

    //ADC A
    ADC_forceMultipleSOC(ADCA_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3));
    //ADC B
    ADC_forceMultipleSOC(ADCB_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1));
    //ADC D
    ADC_forceMultipleSOC(ADCD_BASE, (ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4));

    //ADC A
    while (ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //ADC B
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER2);

    //ADC D
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER4);

    //ADC A
    raw[dsp_adc_out1_amps]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_temp]      = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_ext_diff]       = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);

    //ADC B
    raw[dsp_adc_out2_amps]      = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_batt_volts]     = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);

    //ADC D
    raw[dsp_adc_dc_link_pri_f]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    raw[dsp_adc_dc_link_pri_r]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    raw[dsp_adc_dc_link_sec]    = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_out_cap_volts]  = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_batt_neg_volts] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC A
    raw[dsp_adc_dsp_tempsense]  = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3); //DSP internal temperature sensor
}

void drv_GetFullSeqDspAdcSub(uint32_t *raw)
{
    uint32_t timeout = 200000;

    //ADC C
    ADC_forceMultipleSOC(ADCC_BASE, (ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5 | ADC_FORCE_SOC6));

    //ADC C
    timeout = 200000;
    while (ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3) == false && timeout--) {
    }
    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER3);

    //ADC C
    raw[dsp_adc_15va]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    raw[dsp_adc_5va]            = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    raw[dsp_adc_12vd]           = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);

    //ADC C
    raw[dsp_adc_ext1_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5); //Ext ADC 1
    raw[dsp_adc_ext2_single]    = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER6); //Ext ADC 2
}

//
// End of File
//
