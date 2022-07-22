
#include "F28x_Project.h"
#include <string.h>
#include <stdio.h>
#include "flash_programming_dcsm_c28.h"
#include "F021_F2837xD_C28x.h"
//#include "Types.h"

#define _FLASH_DEBUG_ 1

#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000 
        #define ramFuncSection ".TI.ramfunc"
    #else
        #define ramFuncSection "ramfuncs"
    #endif
#endif

static uint32 u32Index;
static uint16 i;
static uint16 miniBuffer[8]; //useful for 8-word access to flash with
static uint16 j;
static uint16 old_len;

Fapi_StatusType Flash_write(uint32 sector, uint16 *buf, uint16 len)
{
    old_len = len;

    Fapi_StatusType oReturnCheck = Fapi_Status_Success;
    volatile Fapi_FlashStatusType oFlashStatus;
    Fapi_FlashStatusWordType oFlashStatusWord;

    SeizeFlashPump();   // Gain pump semaphore
    EALLOW;             // protect clear

#if _FLASH_DEBUG_==1
    printf("[Debug]Flash write Start, Size : %u\r\n", len);
#endif

    // This function is required to initialize the Flash API based on System frequency before any other Flash API operation can be performed
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 200);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_initializeAPI Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    // Fapi_setActiveFlashBank function sets the Flash bank and FMC for further Flash operations to be performed on the bank
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_setActiveFlashBank Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    for (i = 0, u32Index = sector; (u32Index < (sector + len)) && (oReturnCheck == Fapi_Status_Success); i += 8, u32Index += 8) {

        if (old_len >= 8) {
            old_len -= 8;
            miniBuffer[0] = buf[i + 0];
            miniBuffer[1] = buf[i + 1];
            miniBuffer[2] = buf[i + 2];
            miniBuffer[3] = buf[i + 3];
            miniBuffer[4] = buf[i + 4];
            miniBuffer[5] = buf[i + 5];
            miniBuffer[6] = buf[i + 6];
            miniBuffer[7] = buf[i + 7];
        } else {
            memset(miniBuffer, 0xFFFF, sizeof(miniBuffer));
            for(j = 0; j < old_len; j++) {
                miniBuffer[j] = buf[i + j];
            }
        }

        oReturnCheck = Fapi_issueProgrammingCommand((uint32*) u32Index, miniBuffer, 8, 0, 0, Fapi_AutoEccGeneration);

        // Wait until FSM is done with write sector operation
        while (Fapi_checkFsmForReady() == Fapi_Status_FsmBusy) {
        }

        if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
            printf("[Debug]Fapi_issueProgrammingCommand Func Error : %u\r\n", oReturnCheck);
#endif
            return oReturnCheck;
        }

        // Read FMSTAT register contents to know the status of FSM after program command for any debug
        oFlashStatus = Fapi_getFsmStatus();
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_getFsmStatus : %lu, u32Index : 0x%lx\r\n", oFlashStatus, u32Index);
#endif
#if 0
        // Verify the values programmed.  The Program step itself does a verify as it goes.  This verify is a 2nd verification that can be done.
        oReturnCheck = Fapi_doVerify((uint32*) u32Index, 4, buf + (i / 2), &oFlashStatusWord);

        if (oReturnCheck != Fapi_Status_Success) {
            printf("Flash_write Fapi_doVerify : %lu\r\n", oReturnCheck);
            return oReturnCheck;
        }
#endif
    }

    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;  // Enable ECC

    EDIS;                   // protect set
    ReleaseFlashPump();     // Leave control over flash pump
#if _FLASH_DEBUG_==1
    printf("[Debug]Flash write Finish\r\n");
#endif
    return oReturnCheck;
}

Fapi_StatusType Flash_read(uint32 sector, uint16 *buf, uint16 len)
{
    old_len = len;

    Fapi_StatusType oReturnCheck = Fapi_Status_Success;
    volatile Fapi_FlashStatusType oFlashStatus;

    SeizeFlashPump();   // Gain pump semaphore
    EALLOW;             // protect clear
#if _FLASH_DEBUG_==1
    printf("[Debug]Flash read Start, Size : %u\r\n", len);
#endif

    // This function is required to initialize the Flash API based on System frequency before any other Flash API operation can be performed
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 200);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_initializeAPI Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    // Fapi_setActiveFlashBank function sets the Flash bank and FMC for further Flash operations to be performed on the bank
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_setActiveFlashBank Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    for (i = 0, u32Index = sector; (u32Index < (sector + len)) && (oReturnCheck == Fapi_Status_Success); i += 8, u32Index += 8) {

        oReturnCheck = Fapi_doMarginRead((uint32*) u32Index, miniBuffer, 8, Fapi_NormalRead);
        if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
            printf("[Debug]Fapi_doMarginRead Func Error : %u\r\n", oReturnCheck);
#endif
            return oReturnCheck;
        }

        if (old_len >= 8) {
            old_len -= 8;
            buf[i + 0] = miniBuffer[0];
            buf[i + 1] = miniBuffer[1];
            buf[i + 2] = miniBuffer[2];
            buf[i + 3] = miniBuffer[3];
            buf[i + 4] = miniBuffer[4];
            buf[i + 5] = miniBuffer[5];
            buf[i + 6] = miniBuffer[6];
            buf[i + 7] = miniBuffer[7];
        } else {
            for (j = 0; j < old_len; j++) {
                buf[i + j] = miniBuffer[j];
            }
        }

        // Read FMSTAT register contents to know the status of FSM after program command for any debug
        oFlashStatus = Fapi_getFsmStatus();
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_getFsmStatus : %lu\r\n", oFlashStatus);
#endif
    }
    //Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;    // Enable ECC

    EDIS;                   // protect set
    ReleaseFlashPump();     // Leave control over flash pump

    printf("[Debug]Flash read Finish\r\n");

    return oReturnCheck;
}

Fapi_StatusType Flash_erase(void)
{
    Fapi_StatusType oReturnCheck = Fapi_Status_Success;
    volatile Fapi_FlashStatusType oFlashStatus;
    Fapi_FlashStatusWordType oFlashStatusWord;

    SeizeFlashPump();   // Gain pump semaphore
    EALLOW;             // protect clear
#if _FLASH_DEBUG_==1
    printf("[Debug]Flash erase Start\r\n");
#endif
    // This function is required to initialize the Flash API based on System frequency before any other Flash API operation can be performed
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 200);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_initializeAPI Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    // Fapi_setActiveFlashBank function sets the Flash bank and FMC for further Flash operations to be performed on the bank
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if (oReturnCheck != Fapi_Status_Success) {
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_setActiveFlashBank Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    // Erase Sector M
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, (uint32*) Bzero_SectorM_start);
    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady) {
    }
    // Verify that SectorL is erased.  The Erase step itself does a verify as it goes.  This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32*) Bzero_SectorM_start, Bzero_64KSector_u32length, &oFlashStatusWord); //Bzero_16KSector_u32length
    if (oReturnCheck != Fapi_Status_Success) {

        // Read FMSTAT register contents to know the status of FSM after program command for any debug
        oFlashStatus = Fapi_getFsmStatus();
#if _FLASH_DEBUG_==1
        printf("[Debug]Fapi_getFsmStatus : %lu\r\n", oFlashStatus);
        printf("[Debug]Fapi_doBlankCheck Func Error : %u\r\n", oReturnCheck);
#endif
        return oReturnCheck;
    }

    EDIS;                   // protect set
    ReleaseFlashPump();     // Leave control over flash pump
#if _FLASH_DEBUG_==1
    printf("[Debug]Flash erase Finish\r\n");
#endif
    return oReturnCheck;
}
