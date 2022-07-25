/*
 * drv_sdram.c
 *
 *  Created on: 2021. 6. 10.
 *      Author: KHJ
 */

#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#pragma CODE_SECTION(drv_ClearSdram, ".TI.ramfunc");
#pragma CODE_SECTION(drv_CheckSdram, ".TI.ramfunc");

unsigned long ErrCount_local = 0;
unsigned int ErrCount = 0;
unsigned long Counter = 0;

Uint32 drv_InitSdram(void)
{
    Uint16 ui_Check = 0;

    drv_ClearSdram(SDRAM_BASE_ADDRESS, SDRAM_LENGTH);
    ui_Check = 1;
    return (ui_Check);
}

void drv_ClearSdram(Uint32 Ul_Address, Uint32 ul_Size)
{
    Uint32 ul_i;
    Uint32 memWdl = 0x0;

    for (ul_i = 0; ul_i < ul_Size; ul_i++) {
        memcpy_fast_far((Uint32*) Ul_Address + ul_i, &memWdl, 2);
    }
}

Uint32 drv_CheckSdram(Uint32 Ul_Address, Uint32 ul_Size)
{
    unsigned int mem_rdl;
    unsigned int mem_wdl;
    unsigned long ErrorCount = 0;
    Uint32 XMEM_p;
    Uint32 i;

    XMEM_p = Ul_Address;

    //Fill memory
    mem_wdl = 0;
    for (i = 0; i < ul_Size; i++) {
        __addr32_write_uint16(XMEM_p, 0);
        XMEM_p = XMEM_p + 1;
    }

    XMEM_p = Ul_Address;
    for (i = 0; i < ul_Size; i++) {
        __addr32_write_uint16(XMEM_p, mem_wdl);
        XMEM_p = XMEM_p + 1;
        mem_wdl += 1;
    }

    //Verify memory
    mem_wdl = 0;
    XMEM_p = Ul_Address;

    for (i = 0; i < ul_Size; i++) {
        mem_rdl = __addr32_read_uint16(XMEM_p);

        if (mem_rdl != mem_wdl) {
            ErrorCount++;
        }
        XMEM_p = XMEM_p + 1;
        mem_wdl += 1;
    }
    return (ErrorCount);
}

