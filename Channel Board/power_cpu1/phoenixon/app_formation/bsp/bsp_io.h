/*
 * bsp_io.h
 *
 *  Created on: Nov 19, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_BSP_BSP_IO_H_
#define PHOENIXON_BSP_BSP_IO_H_

#include "drv_gpio.h"

#include "app_settings.h"

// DSP Status LED
#define DSP_STATUS_LED1_OPIN            49
#define DSP_STATUS_LED2_OPIN            55
#define DSP_STATUS_LED3_OPIN            42
#define DSP_STATUS_LED4_OPIN            43

// Charger Status LED
#define DSP_384V_LED_OPIN               61
#define DSP_FAULT_LED_OPIN              64
#define DSP_RUN_LED_OPIN                67

// I/O
#define INPUT_EN_OPIN                   24
#define OUT_EN_OPIN                     25
#define CONV_EN_OPIN                    90
#define VBAT_SENSE_OPIN                 66
#define INPUT_PRE_CHARGE_EN_OPIN        133

#define DSP_DOUT_OPIN                   41

#define DSP_DIN1_IPIN                   38
#define DSP_DIN2_IPIN                   39

// CH NUM MUX
#define CH_MUX1_IPIN                    46
#define CH_MUX2_IPIN                    52
#define CH_MUX3_IPIN                    44
#define CH_MUX4_IPIN                    47

// Fault
#define OUT_FUSE_IPIN                   50
#define EMO_IPIN                        89

// SCI MUX
#define SCI_MUX_OPIN                    30

//ADC RANGE
#define ADC_RANGE                       40

#define bsp_IoOnDspLED1(val)            drv_WriteGPIO(DSP_STATUS_LED1_OPIN, ~val & 0x01)
#define bsp_IoOnDspLED2(val)            drv_WriteGPIO(DSP_STATUS_LED2_OPIN, ~val & 0x01)
#define bsp_IoOnDspLED3(val)            drv_WriteGPIO(DSP_STATUS_LED3_OPIN, val)
#define bsp_IoOnDspLED4(val)            drv_WriteGPIO(DSP_STATUS_LED4_OPIN, val)

#define bsp_IoRunDspLED1()              drv_ToggleGPIO(DSP_STATUS_LED1_OPIN)
#define bsp_IoRunDspLED2()              drv_ToggleGPIO(DSP_STATUS_LED2_OPIN)
#define bsp_IoRunDspLED3()              drv_ToggleGPIO(DSP_STATUS_LED3_OPIN)
#define bsp_IoRunDspLED4()              drv_ToggleGPIO(DSP_STATUS_LED4_OPIN)

#define bsp_IoOn384VLED(val)            drv_WriteGPIO(DSP_384V_LED_OPIN, ~val & 0x01)
#define bsp_IoFaultLED(val)             drv_WriteGPIO(DSP_FAULT_LED_OPIN, ~val & 0x01)
#define bsp_IoRunLED(val)               drv_WriteGPIO(DSP_RUN_LED_OPIN, ~val & 0x01)

#define bsp_IoEnableInput(val)          drv_WriteGPIO(INPUT_EN_OPIN, val)
#define bsp_sec_mosfet(val)             drv_WriteGPIO(OUT_EN_OPIN, val)
#define bsp_IoEnableConv(val)           drv_WriteGPIO(CONV_EN_OPIN, ~val & 0x01)
#define bsp_IoEnableBattSen(val)        drv_WriteGPIO(VBAT_SENSE_OPIN, val)
#define bsp_IoEnablePreCharge(val)      drv_WriteGPIO(INPUT_PRE_CHARGE_EN_OPIN, val)

#define bsp_IoDout(val)                 drv_WriteGPIO(DSP_DOUT_OPIN, val & 0x01)
#define bsp_IoDin1()                    drv_ReadGPIO(DSP_DIN1_IPIN)
#define bsp_IoDin2()                    drv_ReadGPIO(DSP_DIN2_IPIN)

#if(DEBUGGING != 2)
#define bsp_IoChMux1()                  drv_ReadGPIO(CH_MUX1_IPIN)
#define bsp_IoChMux2()                  drv_ReadGPIO(CH_MUX2_IPIN)
#endif
#define bsp_IoChMux3()                  drv_ReadGPIO(CH_MUX3_IPIN)
#define bsp_IoChMux4()                  drv_ReadGPIO(CH_MUX4_IPIN)

// TP For TEST
#define bsp_IoChMux1TP(val)             drv_WriteGPIO(CH_MUX1_IPIN, val)
#define bsp_IoChMux2TP(val)             drv_WriteGPIO(CH_MUX2_IPIN, val)

#define bsp_IoFaultOutFuse()            drv_ReadGPIO(OUT_FUSE_IPIN)
#define bsp_IoFaultEMO()                (~drv_ReadGPIO(EMO_IPIN)) & 0x01       // EMO NO -> Pre-Charger
//#define bsp_IoFaultEMO()                (drv_ReadGPIO(EMO_IPIN)) & 0x01      // EMO NC -> Other equipment except Pre-Charger.

#define bsp_IoSICMux(val)               drv_WriteGPIO(SCI_MUX_OPIN, val)

Uint16 bsp_InitIO(Uint16 *board_channel, Uint16 *board_group);

#endif /* PHOENIXON_BSP_BSP_IO_H_ */
