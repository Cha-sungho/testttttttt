/*
 * main_cpu2.c
 *
 *  Created on: 2021. 12. 15.
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "drv_emif.h"
#include "drv_adc.h"
#include "drv_timer.h"
#include "drv_can.h"

#include "bsp_ad7606.h"
#include "bsp_io.h"

#include "app_sense.h"
#include "app_settings.h"
#include "app_tick.h"

uint32_t gs_cpu2_adc_ad7606_raw[8];
uint32_t gs_cpu2_adc_dsp_raw[16];
float gs_cpu2_adc_ad7606_raw_v[8];
float gs_cpu2_adc_dsp_raw_v[16];

extern float sense_dc_bb1_i_offset;
extern float sense_dc_bb2_i_offset;

#pragma DATA_SECTION(gs_cpu2_adc_ad7606_raw,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_ad7606_raw_v,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_dsp_raw,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_dsp_raw_v,"SHARERAMGS0");

void main(void)
{
    InitSysCtrl();                  // Initialize system control

    Interrupt_initModule();         // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initVectorTable();    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).

    EINT;
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    ERTM;

    //Register the callback function -----------------------------------------------start
    drv_timer0_register[0].process = app_GetDspAdcMain;
    drv_timer0_register[1].process = app_TickVal1;
    drv_timer0_register[2].process = NULL;

    drv_timer1_register[0].process = NULL;
    drv_timer1_register[1].process = NULL;
    //Register the callback function -----------------------------------------------end

    drv_InitEMIF();
    drv_InitADC();

    bsp_InitAD7606(ad7606_range_5v, ad7606_ovs_2);

    drv_InitTimer(T_SAMP * 1000000, 0, 0);

    while (1) {
        app_GetDspAdcSub();
        app_GetExternalAdc();

        if (cpu2_tick_timer_led > REF_MS_TIME(500)) {
            bsp_IoRunDspLED3();
            cpu2_tick_timer_led = 0;
        }

    }
}

//
// End of File
//
