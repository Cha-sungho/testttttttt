/*
 * bsp_ad7606.c
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "bsp_ad7606.h"

#ifdef CPU2
#ifdef _FLASH
#pragma CODE_SECTION(bsp_ConvAD7606, ".TI.ramfunc");
#pragma CODE_SECTION(bsp_BusyAD7606, ".TI.ramfunc");
#pragma CODE_SECTION(bsp_GetAD7606, ".TI.ramfunc");
#pragma CODE_SECTION(bsp_GetFulSeqAD7606, ".TI.ramfunc");
#endif
#endif

static void bsp_ConfigAD7606GPIO(void);
static uint16_t bsp_SelVolRangeAD7606(Ad7606ORangeList range);
static uint16_t bsp_SelOVSAD7606(Ad7606OVSList ovs);
static uint16_t bsp_BusyAD7606(void);

static void bsp_ConfigAD7606GPIO(void)
{
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_CS_PIN, GPIO_DIR_MODE_OUT, GPIO_34_GPIO34, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);         // CS
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_RST_PIN, GPIO_DIR_MODE_OUT, GPIO_32_GPIO32, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);        // RST
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_RANGE_PIN, GPIO_DIR_MODE_OUT, GPIO_40_GPIO40, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);      // RANGE
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_OS0_PIN, GPIO_DIR_MODE_OUT, GPIO_86_GPIO86, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);        // OS0
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_OS1_PIN, GPIO_DIR_MODE_OUT, GPIO_87_GPIO87, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);        // OS1
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_OS2_PIN, GPIO_DIR_MODE_OUT, GPIO_88_GPIO88, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);        // OS2
    drv_ConfigGPIO(GPIO_CORE_CPU2, AD7606_BUSY_PIN, GPIO_DIR_MODE_IN, GPIO_91_GPIO91, GPIO_PIN_TYPE_PULLUP, GPIO_QUAL_3SAMPLE, 1);  // BUSY
}

void bsp_InitAD7606(Ad7606ORangeList range, Ad7606OVSList ovs)
{
#ifdef CPU1
    bsp_ConfigAD7606GPIO();
#endif

#ifdef CPU2
    drv_WriteGPIO(AD7606_CS_PIN, 1);
    //drv_WriteGPIO(AD7606_RD_PIN, 1);

    drv_WriteGPIO(AD7606_RST_PIN, 1);
    DELAY_US(1);
    drv_WriteGPIO(AD7606_RST_PIN, 0);
    DELAY_US(1);

    bsp_SelVolRangeAD7606(range);
    bsp_SelOVSAD7606(ovs);
#endif
}

static uint16_t bsp_SelVolRangeAD7606(Ad7606ORangeList range)
{
    if (bsp_BusyAD7606()) {
        if (range == ad7606_range_5v) drv_WriteGPIO(AD7606_RANGE_PIN, 0);
        else if (range == ad7606_range_10v) drv_WriteGPIO(AD7606_RANGE_PIN, 1);
        else drv_WriteGPIO(AD7606_RANGE_PIN, 0);

        asm(" RPT #10 || NOP");
        //60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
    } else {
        return (FAIL);  //SUCCESS Or FAIL
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

static uint16_t bsp_SelOVSAD7606(Ad7606OVSList ovs)
{
    if (bsp_BusyAD7606()) {
        if (ovs & 0x1) drv_WriteGPIO(AD7606_OS0_PIN, 1);
        else drv_WriteGPIO(AD7606_OS0_PIN, 0);

        if (ovs & 0x2) drv_WriteGPIO(AD7606_OS1_PIN, 1);
        else drv_WriteGPIO(AD7606_OS1_PIN, 0);

        if (ovs & 0x4) drv_WriteGPIO(AD7606_OS2_PIN, 1);
        else drv_WriteGPIO(AD7606_OS2_PIN, 0);

        asm(" RPT #10 || NOP");
        //60ns, Delay : 60ns * (10 + 2) = 60ns Max 200
    } else {
        return (FAIL);  //SUCCESS Or FAIL
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

static uint16_t bsp_BusyAD7606(void)
{
    uint32_t timeout = 0;

    while (1) {
        DELAY_US(1);
        if (!drv_ReadGPIO(AD7606_BUSY_PIN)) return (SUCCESS);
        timeout++;
        if (timeout > 500) return (FAIL);
    }
}

void bsp_ConvAD7606(void)
{
    drv_WriteEMIF(AD7606_CONV_ADDR, 0xFFFF);
}

uint16_t bsp_GetAD7606(uint16_t *raw)
{
    uint16_t i = 0;

    if (bsp_BusyAD7606()) {
        drv_WriteGPIO(AD7606_CS_PIN, 0);

        for (i = 0; i < 8; i++) {
            drv_ReadEMIF(AD7606_READ_ADDR, &raw[i]);
        }

        drv_WriteGPIO(AD7606_CS_PIN, 1);
    } else {
        return (FAIL);  //SUCCESS Or FAIL
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

uint16_t bsp_GetFulSeqAD7606(uint16_t *raw)
{
    uint16_t i = 0;

    bsp_ConvAD7606();

    if (bsp_BusyAD7606()) {
        drv_WriteGPIO(AD7606_CS_PIN, 0);

        for (i = 0; i < 8; i++) {
            drv_ReadEMIF(AD7606_READ_ADDR, &raw[i]);
        }
        drv_WriteGPIO(AD7606_CS_PIN, 1);
    } else {
        return (FAIL);  //SUCCESS Or FAIL
    }

    return (SUCCESS);  //SUCCESS Or FAIL
}

//
// End of File
//
