/*
 * drv_emif.c
 *
 *  Created on: 2021. 6. 9.
 *      Author: KHJ
 */

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#pragma CODE_SECTION(DSP_EmifRead, ".TI.ramfunc");
#pragma CODE_SECTION(DSP_EmifWrite, ".TI.ramfunc");

// Function Prototypes
extern void setup_emif1_pinmux_async_16bit(Uint16);
extern void setup_emif1_pinmux_sdram_16bit(Uint16);

void drv_InitEmif(void)
{
    EMIF_SyncConfig sdConfig;
    EMIF_SyncTimingParams tParam;
    EMIF_AsyncTimingParams tparam;

    setup_emif1_pinmux_async_16bit(GPIO_MUX_CPU1);
    setup_emif1_pinmux_sdram_16bit(GPIO_MUX_CPU1);

    // Configure to run EMIF1 on half Rate. (EMIF1CLK = CPU1SYSCLK/2)
    SysCtl_setEMIF1ClockDivider(SYSCTL_EMIF1CLK_DIV_2);

    // Grab EMIF1 For CPU1.
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);

    // Disable Access Protection. (CPU_FETCH/CPU_WR/DMA_WR)
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    // Commit the configuration related to protection. Till this bit remains set content of EMIF1ACCPROT0 register can't be changed.
    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    // Lock the configuration so that EMIF1COMMIT register can't be changed any more.
    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);

    // Configure SDRAM control registers. Needs to be
    // programmed based on SDRAM Data-Sheet. For this example:
    // T_RFC = 60ns = 0x6; T_RP  = 18ns = 0x1;
    // T_RCD = 18ns = 0x1; T_WR  = 1CLK + 6 ns = 0x1;
    // T_RAS = 42ns = 0x4; T_RC  = 60ns = 0x6;
    // T_RRD = 12ns = 0x1.
    tParam.tRfc = 0x1FU;
    tParam.tRp = 0x7U;
    tParam.tRcd = 0x7U;
    tParam.tWr = 0x7U;
    tParam.tRas = 0xFU;
    tParam.tRc = 0xFU;
    tParam.tRrd = 0x7U;

    EMIF_setSyncTimingParams(EMIF1_BASE, &tParam);

    // Configure Self Refresh exit timing.
    // Txsr = 70ns = 0x7.
    EMIF_setSyncSelfRefreshExitTmng(EMIF1_BASE, 0x7U);

    // Configure Refresh Rate.
    // Tref = 64ms for 8192 ROW, RR = 64000*100(Tfrq)/8192 = 781.25 (0x30E).
    EMIF_setSyncRefreshRate(EMIF1_BASE, 781);

    sdConfig.casLatency = EMIF_SYNC_CAS_LAT_2;
    sdConfig.iBank = EMIF_SYNC_BANK_4;
    sdConfig.narrowMode = EMIF_SYNC_NARROW_MODE_TRUE;
    sdConfig.pageSize = EMIF_SYNC_COLUMN_WIDTH_11;
    EMIF_setSyncMemoryConfig(EMIF1_BASE, &sdConfig);

    // Configures Normal Asynchronous Mode of Operation.
    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, EMIF_ASYNC_NORMAL_MODE);

    // Disables Extended Wait Mode.
    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET);

    // Configure EMIF1 Data Bus Width.
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, EMIF_ASYNC_DATA_WIDTH_16);

    // Configure the access timing for CS2 space.
    tparam.rSetup = 0;
    tparam.rStrobe = 3;
    tparam.rHold = 0;
    tparam.turnArnd = 0;
    tparam.wSetup = 0;
    tparam.wStrobe = 1;
    tparam.wHold = 0;

    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, &tparam);

}

void setupEMIF1PinmuxAsync16Bit()
{
    Uint16 i;
    Uint16 cpu_sel = 0;

    for (i = 28; i <= 52; i++) {
        if ((i != 42) && (i != 43)) {
            GPIO_SetupPinMux(i, cpu_sel, 2);
        }
    }
    for (i = 63; i <= 87; i++) {
        if (i != 84) {
            GPIO_SetupPinMux(i, cpu_sel, 2);
        }
    }

    GPIO_SetupPinMux(88, cpu_sel, 3);
    GPIO_SetupPinMux(89, cpu_sel, 3);
    GPIO_SetupPinMux(90, cpu_sel, 3);
    GPIO_SetupPinMux(91, cpu_sel, 3);
    GPIO_SetupPinMux(92, cpu_sel, 3);
    GPIO_SetupPinMux(93, cpu_sel, 3);
    GPIO_SetupPinMux(94, cpu_sel, 2);

    //
    //setup async mode and enable pull-ups for Data pins
    //
    for (i = 69; i <= 85; i++) {
        if (i != 84) {
            GPIO_SetupPinOptions(i, 0, 0x31);  // GPIO_ASYNC||GPIO_PULLUP
        }
    }
}

void setupEMIF1PinmuxSync16Bit()
{
    Uint16 i;
    Uint16 cpu_sel = 0;
    for (i = 28; i <= 87; i++) {
        if ((i != 42) && (i != 43) && (i != 84)) {
            GPIO_SetupPinMux(i, cpu_sel, 2);
        }
    }

    GPIO_SetupPinMux(88, cpu_sel, 3);
    GPIO_SetupPinMux(89, cpu_sel, 3);
    GPIO_SetupPinMux(90, cpu_sel, 3);
    GPIO_SetupPinMux(91, cpu_sel, 3);
    GPIO_SetupPinMux(92, cpu_sel, 3);
    GPIO_SetupPinMux(93, cpu_sel, 3);
    GPIO_SetupPinMux(94, cpu_sel, 2);

    //
    //setup async mode for Data pins
    //
    for (i = 53; i <= 85; i++) {
        if (i != 84) {
            GPIO_SetupPinOptions(i, 0, 0x31);   // GPIO_ASYNC||GPIO_PULLUP
        }
    }
}

Uint16 DSP_EmifRead(Uint32 ul_Address, Uint16 *ui_Data)
{

    if (ul_Address < EMIF_CS2_BASE_ADDRESS || ul_Address > EMIF_CS2_END_ADDRESS) {
        return (FAIL);  //SUCCESS Or FAIL
    }

    *ui_Data = *((Uint32*) (ul_Address));
    return (SUCCESS);  //SUCCESS Or FAIL
}

Uint16 DSP_EmifWrite(Uint32 ul_Address, Uint16 ui_Data)
{

    if (ul_Address < EMIF_CS2_BASE_ADDRESS || ul_Address > EMIF_CS2_END_ADDRESS) {
        return (FAIL);  //SUCCESS Or FAIL
    }

    (*(Uint16*) (ul_Address)) = ui_Data;
    return (SUCCESS);
}
