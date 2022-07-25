/*
 * drv_adc.h
 *
 *  Created on: Nov 8, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_ADC_H_
#define PHOENIXON_DRV_DRV_ADC_H_

typedef enum
{
    dsp_adc_out1_amps,        //0
    dsp_adc_batt_temp,        //1
    dsp_adc_ext_diff,         //2
    dsp_adc_out2_amps,        //3
    dsp_adc_batt_volts,       //4
    dsp_adc_15va,             //5
    dsp_adc_5va,              //6
    dsp_adc_12vd,             //7
    dsp_adc_dc_link_pri_f,    //8
    dsp_adc_dc_link_pri_r,    //9
    dsp_adc_dc_link_sec,      //10
    dsp_adc_out_cap_volts,    //11
    dsp_adc_batt_neg_volts,   //12
    dsp_adc_dsp_tempsense,    //13
    dsp_adc_ext1_single,      //14
    dsp_adc_ext2_single       //15
} DspAdcList;

void drv_InitADC(void);
void drv_SocDspAdcAll(void);
void drv_GetDspAdcAll(uint32_t *raw);
void drv_GetFullSeqDspAdcAll(uint32_t *data);
void drv_GetFullSeqDspAdcMain(uint32_t *data);
void drv_GetFullSeqDspAdcSub(uint32_t *data);

#endif /* PHOENIXON_DRV_DRV_ADC_H_ */
