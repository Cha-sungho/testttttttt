/*
 * main_cpu1.c
 *
 *  Created on: 2021. 12. 15.
 *      Author: JOO
 */

#include <string.h>
#include <math.h>
#include <stdio.h>

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "easy28x_driverlib_v10.1.h"

#include "drv_emif.h"
#include "drv_adc.h"
#include "drv_pwm.h"
#include "drv_timer.h"
#include "drv_can.h"

#include "bsp_ad7606.h"
#include "bsp_io.h"

#include "app_sense.h"
#include "app_safety.h"
#include "app_settings.h"
#include "app_constant.h"
#include "app_tick.h"
#include "app_test.h"

#include "app_ctrl_dcdc.h"
#include "app_ctrl_power.h"

#include "app_comm.h"
#include "app_op.h"
#include "app_calibration.h"

//#define USE_DEBUGGER  // uncomment this if you use debugger for multicore cpu
#define VERSION_LEN     50
#define DEVNAME_LEN     50

static const char g_device_string[DEVNAME_LEN]      = "FORMATION_Channel";
static const char g_fw_version_string[VERSION_LEN]  = "V1.0.13\r\n";

uint32_t gs_cpu2_adc_ad7606_raw[8];
uint32_t gs_cpu2_adc_dsp_raw[16];
float gs_cpu2_adc_ad7606_raw_v[8];
float gs_cpu2_adc_dsp_raw_v[16];

#pragma DATA_SECTION(gs_cpu2_adc_ad7606_raw,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_ad7606_raw_v,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_dsp_raw,"SHARERAMGS0");
#pragma DATA_SECTION(gs_cpu2_adc_dsp_raw_v,"SHARERAMGS0");

uint16_t dbg_io_flag = 0;
uint16_t dbg_io_din1 = 0;
uint16_t dbg_io_din2 = 0;
uint16_t dbg_io_mux1 = 0;
uint16_t dbg_io_mux2 = 0;
uint16_t dbg_io_mux3 = 0;
uint16_t dbg_io_mux4 = 0;
uint16_t dbg_io_fuse = 0;
uint16_t dbg_io_emo = 0;

uint16_t dbg_pwm_flag = 0;
float dbg_pwm_duty_a = 0;
float dbg_pwm_duty_b = 0;
uint16_t board_number = 0;
uint16_t board_group = 0;

extern appFormationSeq seq_mode;
extern Uint16 stop_flag;

extern float sense_dc_bb1_i_offset;
extern float sense_dc_bb2_i_offset;
extern float calc_dc_out_i_emavg;

extern float sense_dc_bb1_i;
extern float sense_dc_bb1_i_scale;

extern float sense_dc_bb2_i;
extern float sense_dc_bb2_i_scale;

Uint16 offset_complete = 0;
Uint16 control_output_relay = 0;
Uint16 control_output2_relay = 0;

void value_init(void);
void offset_calibration(void);
void PrintSystemInfo(void);

Uint16 voltage_calibration_tune_flag = 0;
Uint16 voltage_calibration_vrf_flag = 0;
Uint16 voltage = 0;

void main(void)
{
    InitSysCtrl();                  // Initialize system control

    Interrupt_initModule();         // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initVectorTable();    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).

    EINT;
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    ERTM;

    //Register the callback function -----------------------------------------------start
    drv_timer0_register[0].process = app_CtrlPower;
    drv_timer0_register[1].process = NULL;

    drv_timer1_register[0].process = app_TickVal1;
    drv_timer1_register[1].process = NULL;
    //Register the callback function -----------------------------------------------end

    drv_InitEMIF();
    drv_InitADC();
    drv_InitPWM();

    bsp_InitIO(&board_number, &board_group);
    bsp_InitAD7606(ad7606_range_5v, ad7606_ovs_2);

    drv_InitCAN(board_number);    //Move to app_InitComm()

    MemCfg_setGSRAMMasterSel((MEMCFG_SECT_GS0 | MEMCFG_SECT_GS1 | MEMCFG_SECT_GS2), MEMCFG_GSRAMMASTER_CPU2);

    app_InitSettings();
    app_InitSense();

    drv_InitTimer(DSP_TIMER0_TIME, T_SAMP * 1000000, 1000);

#ifdef _STANDALONE
#ifdef _FLASH
    //
    // Send boot command to allow the CPU2 application to begin execution
    //
    Device_bootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
    //
    // Send boot command to allow the CPU2 application to begin execution
    //
    Device_bootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);

#endif // _FLASH
#endif // _STANDALONE

    // SCI initialization for easyDSP communication
    easyDSP_SCI_Init();
    PrintSystemInfo();

    while (1) {
#if 0
        app_TestIO();
#endif

        if (voltage_calibration_tune_flag != 0) {
            switch (voltage_calibration_tune_flag)
            {
                case 1:
                    voltage_calibration(1);
                    break;
                case 2:
                    voltage_calibration(2);
                    break;
                case 3:
                    voltage_calibration(3);
                    break;
                case 4:
                    voltage_calibration(4);
                    break;
                case 5:
                    voltage_calibration(5);
                    break;
                default:
                    break;
            }
        }
        if (voltage_calibration_vrf_flag != 0) {
            switch (voltage_calibration_vrf_flag)
            {
                case 1:
                    voltage_calibration_verify(1);
                    break;
                case 2:
                    voltage_calibration_verify(2);
                    break;
                case 3:
                    voltage_calibration_verify(3);
                    break;
                case 4:
                    voltage_calibration_verify(4);
                    break;
                case 5:
                    voltage_calibration_verify(5);
                    break;
                default:
                    break;
            }
        }

        app_operation(board_number);
        app_ConvDataDspAdcSub();
        app_ConvDataExternalAdc();
        app_ConvThermal();
        app_RunCheckSafety(board_number);

        if (app_KeepAliveCheck(&operation.op.keepalive.cnt, &operation.op.keepalive.cnt_old,
                               &operation.op.keepalive.fault_flag) == FALSE) {
            operation.op.state = op_None;
        }

        if (cpu1_tick_timer_led > REF_MS_TIME(500)) {
            bsp_IoRunDspLED1();
            cpu1_tick_timer_led = 0;
        }
    }
}

void PrintSystemInfo(void)
{
    printf("\r\n************************************************");
    printf("\r\nDevice Type: %s", g_device_string);
    printf("\r\nNow FW Version: %s", g_fw_version_string);
    printf("************************************************\r\n");
}

float app_InitSenseOffset1(float *value)
{
    static Uint16 offset_cnt1 = 0;
    static float ret_value_1 = 0;
    float ret_1 = 0;

    offset_cnt1++;
    ret_value_1 += *value;
    if (offset_cnt1 >= 100) {
        ret_value_1 = ret_value_1 / 100;
        offset_cnt1 = 0;
        ret_1 = ret_value_1;
        ret_value_1 = 0;
        return ret_1;
    }
    return 0;
}

float app_InitSenseOffset2(float *value)
{
    static Uint16 offset_cnt2 = 0;
    static float ret_value_2 = 0;
    float ret_2 = 0;

    offset_cnt2++;
    ret_value_2 += *value;
    if (offset_cnt2 >= 100) {
        ret_value_2 = ret_value_2 / 100;
        offset_cnt2 = 0;
        stop_flag = 1;

        ret_2 = ret_value_2;
        ret_value_2 = 0;
        return ret_2;
    }
    return 0;
}

void offset_calibration(void)
{
    if (stop_flag == 1) {
        operation.op.power_stage.V384_on_complete_flag = TRUE;
        operation.op.power_stage.offset_calibration_flag = FALSE;
    } else {
        stop_flag = 1;

    }
}

//
// End of File
//
