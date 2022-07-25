/***************************************************************
    easy28x_driverlib_v*.c
****************************************************************/
#include "easy28x_driverlib_v10.1.h"
#include "driverlib.h"
#include "device.h"

// v10.1 : to support legacy definitions of pin mux info for gpio.c
#ifndef GPIO_28_SCIRXDA
#define GPIO_28_SCIRXDA     GPIO_28_SCIA_RX
#define GPIO_29_SCITXDA     GPIO_29_SCIA_TX
#define GPIO_85_SCIRXDA     GPIO_85_SCIA_RX
#define GPIO_84_SCITXDA     GPIO_84_SCIA_TX
#define GPIO_87_SCIRXDB     GPIO_87_SCIB_RX
#define GPIO_86_SCITXDB     GPIO_86_SCIB_TX
#define GPIO_15_SCIRXDB     GPIO_15_SCIB_RX
#define GPIO_14_SCITXDB     GPIO_14_SCIB_TX
#define GPIO_85_CMUARTRXA   GPIO_85_UARTA_RX
#define GPIO_84_CMUARTTXA   GPIO_84_UARTA_TX
#endif

//////////////////////////////////////////////////
// DON'T CHANGE THIS FILE
// 소스 파일이 아닌 헤더 파일을 변경하셔야 합니다.
//////////////////////////////////////////////////

// function declaration
__interrupt void easy_RXINT_ISR(void);
void easyDSP_SCIBootCPU2(void);
void easyDSP_SCIBootCM(void);                // for F2838xS and F2838xD
void easyDSP_FlashBootCPU2(void);
void easyDSP_2838x_Sci_Boot_Sync(void);

#define F2838xX_CPU1    (F2838xS_CPU1       || F2838xD_CPU1)
#define F2838xX_CPU1_CM (F2838xS_CPU1_CM    || F2838xD_CPU1_CM)
#define F2838xX_ALL     (F2838xS_CPU1       || F2838xS_CPU1_CM || F2838xD_CPU1 || F2838xD_CPU1_CPU2 || F2838xD_CPU1_CM || F2838xD_CPU1_CPU2_CM)

////////////////////////////////////////////////////////////
// SCI-A used except that SCI-B is used for CPU2
// caution : to use SCIC or SCID for CPU2, you have to change the source in easyDSP_SCI_Init() accordingly
//////////////////////////////////////////////////////////////
#if F2837xD_CPU1_CPU2 && defined(CPU2)
#define SCIx_BASE   SCIB_BASE
#elif (F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM) && defined(CPU2)
uint32_t SCIx_BASE = SCIB_BASE;
#else
uint32_t SCIx_BASE = SCIA_BASE;
#endif

// for internal use. Don't delete !!!!
// v10.1 : moved to here
#define VER_OF_THIS_FILE    1010    // version xx.y.z
unsigned int ezDSP_Version_SCI = VER_OF_THIS_FILE;
unsigned int ezDSP_uRead16BPossible = 1, ezDSP_uRead8BPossible = 1;
bool ezDSP_b32bitAddress = false;
float ezDSP_fFloat = 0;
#if defined(CPU1)
unsigned int ezDSP_uCPU = 1;
#elif defined(CPU2)
unsigned int ezDSP_uCPU = 2;
#endif

// auto baud implementation
void easyDSP_AutoBaud(void)
{
    uint16_t byteData;
    /////////////////////////////////////////////////////////////////////////////
    // Initialize the SCI-A port for communications with the host.
    /////////////////////////////////////////////////////////////////////////////
    // Enable the SCI-A clocks
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIA);      // called in CM_init()
    SysCtl_setLowSpeedClock(SYSCTL_LSPCLK_PRESCALE_4);    // called in CM_init()

    EALLOW;
    HWREGH(SCIA_BASE + SCI_O_FFTX) = SCI_FFTX_SCIRST;
    // 1 stop bit, No parity, 8-bit character, No loopback
    HWREGH(SCIA_BASE + SCI_O_CCR) = (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE);
    SCI_setParityMode(SCIA_BASE, SCI_CONFIG_PAR_NONE);
    // Enable TX, RX, Use internal SCICLK
    HWREGH(SCIA_BASE + SCI_O_CTL1) = (SCI_CTL1_TXENA | SCI_CTL1_RXENA);
    // Disable RxErr, Sleep, TX Wake, Rx Interrupt, Tx Interrupt
    HWREGB(SCIA_BASE + SCI_O_CTL2) = 0x0U;
    // Relinquish SCI-A from reset and enable TX/RX
    SCI_enableModule(SCIA_BASE);
    EDIS;
    /////////////////////////////////////////////////////////////////////////////
    // Perform autobaud lock with the host.
    /////////////////////////////////////////////////////////////////////////////
    SCI_lockAutobaud(SCIA_BASE);
    // Read data
    byteData = SCI_readCharBlockingNonFIFO(SCIA_BASE);
    // Write data to request key
    SCI_writeCharNonBlocking(SCIA_BASE, byteData);
}

////////////////////////////////////////////////////////////
unsigned int ezDSP_uOnChipFlash = 0;
void easyDSP_SCI_Init(void)
{
    // v10.1 : make sure that these variables will survive even after optimization
    ezDSP_Version_SCI = VER_OF_THIS_FILE;
    ezDSP_uRead16BPossible = 1;
    ezDSP_uRead8BPossible = 1;
    ezDSP_fFloat = 0;
    #if defined(CPU1)
    ezDSP_uCPU = 1;
    #elif defined(CPU2)
    ezDSP_uCPU = 2;
    #endif


#if F28002x || F28003x || F28004x
    // GPIO28 is the SCI Rx pin.
    #if F28004x
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    #endif
    #if F28003x
    GPIO_setPinConfig(GPIO_28_SCIA_RX);
    #else
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    #endif
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCI Tx pin.
    #if F28004x
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    #endif
    #if F28003x
    GPIO_setPinConfig(GPIO_29_SCIA_TX);
    #else
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    #endif
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);
#elif F2807x || F2837xS || F2837xD_CPU1
    // GPIO85 is the SCI Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    GPIO_setMasterCore(30, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_30_GPIO30);
    GPIO_setDirectionMode(30, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(30, GPIO_PIN_TYPE_PULLUP);
    GPIO_writePin(30, 0);

    // GPIO84 is the SCI Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);
#elif F2837xD_CPU1_CPU2
#ifdef CPU1
    // GPIO85 is the SCI Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO84 is the SCI Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

#if 0
    // SCI-B connected to CPU2
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_B;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) |= SYSCTL_CPUSEL5_SCI_B;  // 0 = CPU1, 1 = CPU2 : This register must be configured prior to enabling the peripheral clocks
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_B;
    EDIS;

    ///////////////////////////////////////////////////////////
    // modify below to change GPIOx for SCI-B
    ///////////////////////////////////////////////////////////
    // GPIO87 is the SCI-B Rx pin for CPU2.
    GPIO_setMasterCore(87, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_87_SCIRXDB);
    GPIO_setDirectionMode(87, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(87, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(87, GPIO_QUAL_ASYNC);

    // GPIO86 is the SCI-B Tx pin for CPU2.
    GPIO_setMasterCore(86, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_86_SCITXDB);
    GPIO_setDirectionMode(86, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(86, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(86, GPIO_QUAL_ASYNC);
#endif
#endif

#elif F2838xX_ALL
#ifdef CPU1
    // SCI-A connected to CPU1
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_B;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) &= ~SYSCTL_CPUSEL5_SCI_A;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_B;
    EDIS;

    // GPIO28 is the SCI Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCI Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
    // SCI-B connected to CPU2
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_B;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) |= SYSCTL_CPUSEL5_SCI_B;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_B;
    EDIS;
    ///////////////////////////////////////////////////////////
    // modify below to change GPIOx for SCI-B
    ///////////////////////////////////////////////////////////
    // GPIO15 is the SCI-B Rx pin for CPU2.
    GPIO_setMasterCore(15, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_15_SCIRXDB);
    GPIO_setDirectionMode(15, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(15, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(15, GPIO_QUAL_ASYNC);
    // GPIO14 is the SCI-B Tx pin for CPU2.
    GPIO_setMasterCore(14, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_14_SCITXDB);
    GPIO_setDirectionMode(14, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(14, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(14, GPIO_QUAL_ASYNC);
#endif
#if F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
    ///////////////////////////////////////////////////////////
    // modify below to change GPIOx for UART-0
    ///////////////////////////////////////////////////////////
    // GPIO85 is the UART-A Rx pin for CM
    GPIO_setMasterCore(85, GPIO_CORE_CM);
    GPIO_setPinConfig(GPIO_85_CMUARTRXA);
    GPIO_setDirectionMode(85, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(85, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(85, GPIO_QUAL_ASYNC);
    // GPIO84 is the UART-A Tx pin for CM
    GPIO_setMasterCore(84, GPIO_CORE_CM);
    GPIO_setPinConfig(GPIO_84_CMUARTTXA);
    GPIO_setDirectionMode(84, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(84, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(84, GPIO_QUAL_ASYNC);
#endif
#endif

#else   // MCU is not defined
    this will create compiler error intentionally
#endif

    // Map the ISR to the wake interrupt.
    if(SCIx_BASE == SCIA_BASE) {
        Interrupt_register(INT_SCIA_RX, easy_RXINT_ISR);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIA);    // Enable the SCI clocks
    }
    #if !F28002x
    else if(SCIx_BASE == SCIB_BASE) {
        Interrupt_register(INT_SCIB_RX, easy_RXINT_ISR);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIB);
    }
    #endif

    // Configure SCI
    SCI_setConfig(SCIx_BASE, DEVICE_LSPCLK_FREQ, BAUDRATE, (SCI_CONFIG_WLEN_8 |
                                            SCI_CONFIG_STOP_ONE |
                                             SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIx_BASE);
    //SCI_enableLoopback(SCIx_BASE);
    SCI_resetChannels(SCIx_BASE);
    SCI_enableFIFO(SCIx_BASE);

    // RX and TX FIFO Interrupts Enabled
    SCI_enableInterrupt(SCIx_BASE, SCI_INT_RXFF);
    SCI_disableInterrupt(SCIx_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIx_BASE, SCI_FIFO_TX1, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIx_BASE);

    SCI_resetTxFIFO(SCIx_BASE);
    SCI_resetRxFIFO(SCIx_BASE);

    // Enable the interrupts in the PIE: Group 9 interrupts 1 & 2.
    if(SCIx_BASE == SCIA_BASE)      Interrupt_enable(INT_SCIA_RX);
    #if !F28002x
    else if(SCIx_BASE == SCIB_BASE) Interrupt_enable(INT_SCIB_RX);
    #endif

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    EINT;
    ERTM;

    // others
#ifdef _FLASH
    ezDSP_uOnChipFlash = 1;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
// boot and sync for multi-core MCU
/////////////////////////////////////////////////////////////////////////////////////////////
#if F2837xD_CPU1_CPU2 || F2838xS_CPU1_CM || F2838xD_CPU1_CM || F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
// boot and synch for multi-core MCU like 2837xD, 2338xD
// note : For 2838xD, you can modify flash sector location if sector 0 is not preferred
void easyDSP_Boot_Sync(void)
{
#ifdef _FLASH
#ifdef CPU1
#if F2837xD_CPU1_CPU2
    easyDSP_FlashBootCPU2();
#endif
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
    Device_bootCPU2(BOOTMODE_BOOT_TO_FLASH_SECTOR0);
    //Device_bootCPU2(BOOTMODE_BOOT_TO_FLASH_SECTOR4);  // other options for CPU2
    //Device_bootCPU2(BOOTMODE_BOOT_TO_FLASH_SECTOR8);
    //Device_bootCPU2(BOOTMODE_BOOT_TO_FLASH_SECTOR13);
#endif
#if F2838xS_CPU1_CM || F2838xD_CPU1_CM || F2838xD_CPU1_CPU2_CM
    Device_bootCM(BOOTMODE_BOOT_TO_FLASH_SECTOR0);
    //Device_bootCM(BOOTMODE_BOOT_TO_FLASH_SECTOR4);    // other options for CM
    //Device_bootCM(BOOTMODE_BOOT_TO_FLASH_SECTOR8);
    //Device_bootCM(BOOTMODE_BOOT_TO_FLASH_SECTOR13);
#endif
#endif // CPU1
#else   // if not flash
#if defined(CPU1) || defined(CPU2)
#if F2837xD_CPU1_CPU2
    easyDSP_SCIBootCPU2();
#endif
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CM || F2838xS_CPU1_CM || F2838xD_CPU1_CPU2_CM
    easyDSP_2838x_Sci_Boot_Sync();
#endif
#endif  // CPU1 and CPU2
#endif  // _FLASH
}
#endif

#if F2838xX_ALL
#define CPU1_CPU2_SYNC_FLAG         IPC_FLAG5
#define CPU1_CPU2_2ND_SYNC_FLAG     IPC_FLAG6
#define CPU1_CM_SYNC_FLAG           IPC_FLAG31
#define CPU1_CM_COMM_FLAG           IPC_FLAG30
void easyDSP_2838x_Sci_Boot_Sync(void)
{
#if F2838xD_CPU1_CPU2 || F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
#ifdef CPU1
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
    easyDSP_SCIBootCPU2();
#endif
#if F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
    easyDSP_SCIBootCM();
#endif
#if F2838xD_CPU1_CPU2_CM
    IPC_sync(IPC_CPU1_L_CPU2_R, CPU1_CPU2_2ND_SYNC_FLAG);   // second sync with CPU2 after CM boot
#endif
#endif  // CPU1
#ifdef CPU2
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
    easyDSP_SCIBootCPU2();
    DEVICE_DELAY_US(1000);    // Wait until CPU1 is configuring SCI-B for CPU2 in easyDSP_SCI_Init()
#endif
#if F2838xD_CPU1_CPU2_CM
    IPC_sync(IPC_CPU2_L_CPU1_R, CPU1_CPU2_2ND_SYNC_FLAG);   // second sync with CPU1 after CM boot
    DEVICE_DELAY_US(1000);    // Wait until CPU1 is configuring SCI-B for CPU2 in easyDSP_SCI_Init()
#endif
#endif  // CPU2
#endif  // F2838xD_CPU1_CPU2 || F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
}
#endif

#if F2838xD_CPU1_CPU2 || F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
#ifdef CPU1
uint32_t ezDSP_u32CPU2BlockWordSize = 0, ezDSP_u32CMBlockWordSize = 0;  // only for monitoring
uint32_t ezDSP_u32CPU2BootStatus = 0, ezDSP_u32CMBootStatus = 0;

uint16_t easyDSP_GetWordData(void)
{
    uint16_t wordData, byteData;
    wordData = byteData = 0x0000;

    // Fetch the LSB and verify back to the host
    while(!SCI_isDataAvailableNonFIFO(SCIA_BASE));
    wordData =  SCI_readCharNonBlocking(SCIA_BASE);
    SCI_writeCharNonBlocking(SCIA_BASE, wordData);

    // Fetch the MSB and verify back to the host
    while(!SCI_isDataAvailableNonFIFO(SCIA_BASE));
    byteData =  SCI_readCharNonBlocking(SCIA_BASE);
    SCI_writeCharNonBlocking(SCIA_BASE, byteData);

    // form the wordData from the MSB:LSB
    wordData |= (byteData << 8);

    return wordData;
}

uint32_t easyDSP_GetLongData(void)
{
    uint32_t longData;
    longData = ( (uint32_t)easyDSP_GetWordData() << 16);    // Fetch the upper 1/2 of the 32-bit value
    longData |= (uint32_t)easyDSP_GetWordData();            // Fetch the lower 1/2 of the 32-bit value
    return longData;
}

void easyDSP_CopyData(uint16_t uMCU)
{
    struct HEADER {
        uint32_t DestAddr;
        uint16_t BlockSize;
    } BlockHeader;

    uint16_t wordData;
    uint16_t i;

   BlockHeader.BlockSize = easyDSP_GetWordData();
   if(uMCU == 3) BlockHeader.BlockSize /= 2;

   // monitoring
   if(uMCU == 2)        ezDSP_u32CPU2BlockWordSize    += BlockHeader.BlockSize;
   else if(uMCU == 3)   ezDSP_u32CMBlockWordSize      += BlockHeader.BlockSize;

   while(BlockHeader.BlockSize != 0)
   {
      BlockHeader.DestAddr = easyDSP_GetLongData();
      if(uMCU == 2) {
          BlockHeader.DestAddr -= 0x400;
          BlockHeader.DestAddr += CPU1TOCPU2MSGRAM1_BASE;
      }
      else if(uMCU == 3) {
          BlockHeader.DestAddr -= 0x20000800;
          BlockHeader.DestAddr /= 2;    // CM address
          BlockHeader.DestAddr += CPUXTOCMMSGRAM1_BASE;
      }

      for(i = 0; i < BlockHeader.BlockSize; i++) {
          wordData = easyDSP_GetWordData();
          if(uMCU == 2)         *(uint16_t *)BlockHeader.DestAddr = wordData;
          else if(uMCU == 3)    *(uint16_t *)BlockHeader.DestAddr = (wordData >> 8) | (wordData << 8);
          BlockHeader.DestAddr++;
      }

      BlockHeader.BlockSize = easyDSP_GetWordData();
      if(uMCU == 3) BlockHeader.BlockSize /= 2;

      // monitoring
      if(uMCU == 2)        ezDSP_u32CPU2BlockWordSize += BlockHeader.BlockSize;
      else if(uMCU == 3)   ezDSP_u32CMBlockWordSize += BlockHeader.BlockSize;
   }
}

#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
void easyDSP_SCIBootCPU2(void)
{
    // GPIO28 is the SCI Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCI Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    // copy SCI booting agent code to CPU1TOCPU2MSGRAM1
    uint16_t i;
    easyDSP_AutoBaud();
    if(easyDSP_GetWordData() != 0x08AA) while(1);   // KeyValue
    for(i = 0; i < 8; i++) easyDSP_GetWordData();   // 8 reserved words
    easyDSP_GetWordData();                          // entry point
    easyDSP_GetWordData();                          // entry point
    easyDSP_CopyData(2);

    // secure the last echo byte
    DEVICE_DELAY_US(1000*10);

    // SCIA connected to CPU2
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_A;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) |= SYSCTL_CPUSEL5_SCI_A;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_A;
    EDIS;

    //Allows CPU02 bootrom to take control of clock configuration registers
    EALLOW;
    HWREG(CLKCFG_BASE + SYSCTL_O_CLKSEM) = 0xA5A50000;
    HWREG(CLKCFG_BASE + SYSCTL_O_LOSPCP) = 0x0002;
    EDIS;

    // GPIO28 SCIA-RX to CPU2
    GPIO_setMasterCore(28, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 SCIA-TX to CPU2
    GPIO_setMasterCore(29, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    // Assign all GS RAMs to CPU2
    // NOTE : don't assign GSxRAM to CPU2 where .text of CPU1 is located !!!!!!!
    EALLOW;
    HWREG(MEMCFG_BASE + MEMCFG_O_GSXMSEL) = 0xFFFF;
    EDIS;

    // Boot CPU2 /w agent. max. (1000) copy
    IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Boot_Pump_Reg->IPC_BOOTMODE = (BOOT_KEY | CPU2_BOOT_FREQ_200MHZ | BOOTMODE_IPC_MSGRAM_COPY_LENGTH_1000W | BOOTMODE_IPC_MSGRAM_COPY_BOOT_TO_M1RAM);
    IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Flag_Ctr_Reg->IPC_SET = IPC_FLAG0;
    uint32_t clearvalue;
    EALLOW;
    clearvalue = HWREG(DEVCFG_BASE + SYSCTL_O_CPU2RESCTL);
    clearvalue &= ~SYSCTL_CPU2RESCTL_RESET;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPU2RESCTL) = (SYSCTL_REG_KEY & SYSCTL_CPU2RESCTL_KEY_M) | clearvalue;
    EDIS;
    while((HWREGH(DEVCFG_BASE + SYSCTL_O_RSTSTAT) &
            SYSCTL_RSTSTAT_CPU2RES) == 0U);

    DEVICE_DELAY_US(1000*1);    // wait CPU2 boot
    ezDSP_u32CPU2BootStatus = IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Boot_Pump_Reg->IPC_BOOTSTS;

    ///////////////////////////////////////////////////////////////////////////////////
    // download target CPU2 code via SCI-A. This is done by agent program in CPU2
    ///////////////////////////////////////////////////////////////////////////////////

    // Synchronize both the cores : wait until CPU2 is booted and set CPU1_CPU2_SYNC_FLAG
    IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Flag_Ctr_Reg->IPC_SET = CPU1_CPU2_SYNC_FLAG;
    while((IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Flag_Ctr_Reg->IPC_STS & CPU1_CPU2_SYNC_FLAG) == 0U);
    IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Flag_Ctr_Reg->IPC_ACK = CPU1_CPU2_SYNC_FLAG;
    while((IPC_Instance[IPC_CPU1_L_CPU2_R].IPC_Flag_Ctr_Reg->IPC_FLG & CPU1_CPU2_SYNC_FLAG) != 0U);

    // Assign all GS RAMs to CPU1
    EALLOW;
    HWREG(MEMCFG_BASE + MEMCFG_O_GSXMSEL) = 0x0000;
    EDIS;

    // SCIA connected to CPU1
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_A;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) &= ~SYSCTL_CPUSEL5_SCI_A;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_A;
    EDIS;
}

#endif
#if F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM
void easyDSP_SendWordToCM(uint16_t wordData)
{
    IPC_sendResponse(IPC_CPU1_L_CM_R, wordData);
    IPC_sync(IPC_CPU1_L_CM_R, CPU1_CM_COMM_FLAG);
    IPC_sync(IPC_CPU1_L_CM_R, CPU1_CM_COMM_FLAG);
}

void easyDSP_SendAppToCM(void)
{
    struct HEADER {
        uint32_t DestAddr;
        uint16_t BlockSize;
    } BlockHeader;
    uint16_t wordData;
    uint16_t i;

    BlockHeader.BlockSize = easyDSP_GetWordData();
    BlockHeader.BlockSize /= 2; // byte to word
    easyDSP_SendWordToCM(BlockHeader.BlockSize);

    while(BlockHeader.BlockSize != 0)
    {
        BlockHeader.DestAddr = easyDSP_GetLongData();
        easyDSP_SendWordToCM((uint16_t)(BlockHeader.DestAddr >> 16));   // MSW
        easyDSP_SendWordToCM((uint16_t)BlockHeader.DestAddr);           // LSW

        for(i = 1; i <= BlockHeader.BlockSize; i++) {
            wordData = easyDSP_GetWordData();
            easyDSP_SendWordToCM(wordData);
        }
        BlockHeader.BlockSize = easyDSP_GetWordData();
        BlockHeader.BlockSize /= 2;
        easyDSP_SendWordToCM(BlockHeader.BlockSize);
    }
}

bool ezDSP_bIsCMResetBeforeBooting;
void easyDSP_SCIBootCM(void)
{
    uint16_t i;

    // GPIO28 is the SCI Rx pin.
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    // GPIO29 is the SCI Tx pin.
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    // clear message ram for test purpose
    //for(i = 0; i < (1000/2); i++) HWREG(CPUXTOCMMSGRAM1_BASE + i) = 0;  // 1000x16bit = 500x32bit.

    // copy SCI booting agent code to CPU1TOCMMSGRAM1
    easyDSP_AutoBaud();
    if(easyDSP_GetWordData() != 0x08AA) while(1);   // KeyValue
    for(i = 0; i < 8; i++) easyDSP_GetWordData();   // 8 reserved words
    easyDSP_GetWordData();                          // entry point
    easyDSP_GetWordData();                          // entry point
    easyDSP_CopyData(3);

    // secure the last echo
    DEVICE_DELAY_US(1000*1);

    // CM should be in reset
    ezDSP_bIsCMResetBeforeBooting = SysCtl_isCMReset();

    // Boot CM /w agent: copy max. size (1000 word)
    Device_bootCM(BOOTMODE_IPC_MSGRAM_COPY_LENGTH_1000W | BOOTMODE_IPC_MSGRAM_COPY_BOOT_TO_S0RAM);
    DEVICE_DELAY_US(1000*1);    // wait CM boot
    ezDSP_u32CMBootStatus = IPC_getBootStatus(IPC_CPU1_L_CM_R);

    // download target CM code via SCI-A. This is done by agent program in CM
    if(easyDSP_GetWordData() != 0x08AA) while(1);   // KeyValue
    for(i = 0; i < 8; i++) easyDSP_GetWordData();   // 8 reserved words
    easyDSP_SendWordToCM(easyDSP_GetWordData());    // entry point
    easyDSP_SendWordToCM(easyDSP_GetWordData());    // entry point
    easyDSP_SendAppToCM();
    // secure the last echo byte
    DEVICE_DELAY_US(1000*1);

    // Synchronize both the cores
    IPC_sync(IPC_CPU1_L_CM_R, CPU1_CM_SYNC_FLAG);
}
#endif
#endif  // CPU1

#ifdef CPU2
#if F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
// easyDSP_SCIBootCPU2 for F2838xD CPU2
void easyDSP_SCIBootCPU2(void)
{
    // booting echo feedback still ongoing even this moment
    // So no switch SCIA channel to CPU1 immediately
    DEVICE_DELAY_US(1000*10);

    // Synchronize both the cores.
    IPC_sync(IPC_CPU2_L_CPU1_R, CPU1_CPU2_SYNC_FLAG);

    asm(" NOP");
    asm(" NOP");

    // Wait until CPU1 is configuring SCI-B for CPU2 in easyDSP_SCI_Init()
    //DEVICE_DELAY_US(1000*10);
}
#endif  // F2838xD_CPU1_CPU2 || F2838xD_CPU1_CPU2_CM
#endif  // CPU2
#endif  // F2838xD_CPU1_CPU2 || F2838xX_CPU1_CM || F2838xD_CPU1_CPU2_CM


#if F2837xD_CPU1_CPU2
#ifdef CPU1
//*****************************************************************************
// The following are values that can be passed into the
// IPCBootControlSystem() function in the ulBootMode parameter.
//*****************************************************************************
#define BROM_IPC_EXECUTE_BOOTMODE_CMD     0x00000013
// Below are the values programmed into IPCBOOTMODE register
#define C1C2_BROM_BOOTMODE_BOOT_FROM_PARALLEL                  0x00000000
#define C1C2_BROM_BOOTMODE_BOOT_FROM_SCI                       0x00000001
#define C1C2_BROM_BOOTMODE_BOOT_FROM_SPI                       0x00000004
#define C1C2_BROM_BOOTMODE_BOOT_FROM_I2C                       0x00000005
#define C1C2_BROM_BOOTMODE_BOOT_FROM_CAN                       0x00000007
#define C1C2_BROM_BOOTMODE_BOOT_FROM_RAM                       0x0000000A
#define C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH                     0x0000000B
//*****************************************************************************
// The following values report on the CPU02 boot ROM status at all times while
// the CPU02 is booting, and wil reside in IPCBOOTSTS[11:0].
//*****************************************************************************
#define C2_BOOTROM_BOOTSTS_C2TOC1_IGNORE                             0x00000000 // CPU02 has not filled in a valid value yet
#define C2_BOOTROM_BOOTSTS_SYSTEM_START_BOOT                         0x00000001 // CPU02 has started to boot, but not completed the boot process yet
#define C2_BOOTROM_BOOTSTS_SYSTEM_READY                              0x00000002 // CPU02 has completed the boot and is ready for CPU01 TO CPU02 IPC commands
#define C2_BOOTROM_BOOTSTS_C2TOC1_BOOT_CMD_ACK                       0x00000003 // CPU02 ACKs the command in CPU01 TO CPU01 BOOTMODE register
#define C2_BOOTROM_BOOTSTS_C2TOC1_BOOT_CMD_NAK_STATUS_NOT_SUPPORTED  0x00000004 // CPU02 un-supported command in CPU01 TO CPU01 BOOTMODE register
#define C2_BOOTROM_BOOTSTS_C2TOC1_BOOT_CMD_NAK_STATUS_BUSY_WITH_BOOT 0x00000005 // CPU2 NAKs the current boot command in CPU01 TO CPU01 BOOTMODE register
// Flags
#define IPC_FLAG0                   0x00000001  // IPC FLAG 0
#define IPC_FLAG31                  0x80000000  // IPC FLAG 31
#include "inc/hw_ipc.h"
uint32_t ezDSP_u32C1BROMSTATUS = 0, ezDSP_u32BootStatus = 0;
void easyDSP_SCIBootCPU2(void)
{
    ezDSP_u32C1BROMSTATUS = HWREG(0x0000002C);

    // Wait until CPU02 control system boot ROM is ready to receive CPU01 to CPU02 INT1 interrupts.
    /*
    while(1) {
        ezDSP_u32BootStatus = HWREG(IPC_BASE + IPC_O_STS) & 0x0000000F;
        if(ezDSP_u32BootStatus == C2_BOOTROM_BOOTSTS_SYSTEM_READY) break;
    }
    */

    // Loop until CPU02 control system IPC flags 1 and 32 are available
    while((HWREG(IPC_BASE + IPC_O_FLG) & IPC_FLAG0) || (HWREG(IPC_BASE + IPC_O_FLG) & IPC_FLAG31));

    // SCIA connected to CPU2
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_A;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) |= SYSCTL_CPUSEL5_SCI_A;  // 0 = CPU1, 1 = CPU2 : This register must be configured prior to enabling the peripheral clocks
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_A;
    EDIS;

    //Allows CPU02 bootrom to take control of clock configuration registers
    EALLOW;
    HWREG(CLKCFG_BASE + SYSCTL_O_CLKSEM) = 0xA5A50000;
    HWREG(CLKCFG_BASE + SYSCTL_O_LOSPCP) = 0x0002;
    EDIS;

    // GPIO85=Rx, GPIO84=Tx to CPU2
    GPIO_setMasterCore(85, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_85_SCIRXDA);
    GPIO_setDirectionMode(85, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(85, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(85, GPIO_QUAL_ASYNC);

    GPIO_setMasterCore(84, GPIO_CORE_CPU2);
    GPIO_setPinConfig(GPIO_84_SCITXDA);
    GPIO_setDirectionMode(84, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(84, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(84, GPIO_QUAL_ASYNC);

    // Assign all GS RAMs to CPU2
    //MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GSX_ALL, MEMCFG_GSRAMMASTER_CPU2);   // no use from v9.5
    EALLOW;
    HWREG(MEMCFG_BASE + MEMCFG_O_GSXMSEL) = 0xFFFF;    // NOTE : don't assign GSxRAM to CPU2 where .text of CPU1 is located !!!!!!!
    EDIS;

    HWREG(IPC_BASE + IPC_O_BOOTMODE)    = C1C2_BROM_BOOTMODE_BOOT_FROM_SCI;     // CPU1 to CPU2 IPC Boot Mode Register
    HWREG(IPC_BASE + IPC_O_SENDCOM)     = BROM_IPC_EXECUTE_BOOTMODE_CMD;        // CPU1 to CPU2 IPC Command Register
    HWREG(IPC_BASE + IPC_O_SET)         = 0x80000001;                           // CPU1 to CPU2 IPC flag register

    //wait until CPU2 is booted and set IPC5
    while(!(HWREG(IPC_BASE + IPC_O_STS) & IPC_STS_IPC5));
    HWREG(IPC_BASE + IPC_O_ACK) |= IPC_ACK_IPC5;    //clearing the acknowledgement flag

    // Assign all GS RAMs to CPU1
    //MemCfg_setGSRAMMasterSel(MEMCFG_SECT_GSX_ALL, MEMCFG_GSRAMMASTER_CPU1);   // no use from v9.5
    EALLOW;
    HWREG(MEMCFG_BASE + MEMCFG_O_GSXMSEL) = 0x0000;
    EDIS;

    // SCIA connected to CPU1
    EALLOW;
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) &= ~SYSCTL_PCLKCR7_SCI_A;
    HWREG(DEVCFG_BASE + SYSCTL_O_CPUSEL5) &= ~SYSCTL_CPUSEL5_SCI_A;  // 0 = CPU1, 1 = CPU2 : This register must be configured prior to enabling the peripheral clocks
    HWREG(CPUSYS_BASE + SYSCTL_O_PCLKCR7) |= SYSCTL_PCLKCR7_SCI_A;
    EDIS;
}

void easyDSP_FlashBootCPU2(void)
{
    /*
    // Wait until CPU02 control system boot ROM is ready to receive CPU01 to CPU02 INT1 interrupts.
    //uint32_t bootStatus;
    while(1) {
       // bootStatus = HWREG(IPC_BASE + IPC_O_STS) & 0x0000000F;
        if((HWREG(IPC_BASE + IPC_O_STS) & 0x0000000F) == C2_BOOTROM_BOOTSTS_SYSTEM_READY) break;
    }
    */

    // Loop until CPU02 control system IPC flags 1 and 32 are available
    while((HWREG(IPC_BASE + IPC_O_FLG) & IPC_FLAG0) || (HWREG(IPC_BASE + IPC_O_FLG) & IPC_FLAG31));

    HWREG(IPC_BASE + IPC_O_BOOTMODE)    = C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH;   // CPU1 to CPU2 IPC Boot Mode Register
    HWREG(IPC_BASE + IPC_O_SENDCOM)     = BROM_IPC_EXECUTE_BOOTMODE_CMD;        // CPU1 to CPU2 IPC Command Register
    HWREG(IPC_BASE + IPC_O_SET)         = 0x80000001;                           // CPU1 to CPU2 IPC flag register
}

#endif

#ifdef CPU2
#include "inc/hw_ipc.h"
void easyDSP_SCIBootCPU2(void)
{
    // booting echo feedback still ongoing even this moment
    // So no switch SCIA channel to CPU1 immediately
    DEVICE_DELAY_US(1000*30);
    DEVICE_DELAY_US(1000*30);
    DEVICE_DELAY_US(1000*30);
    DEVICE_DELAY_US(1000*10);

    // set IPC5 to inform to CPU1 that booting is done
    HWREG(IPC_BASE + IPC_O_SET) |= IPC_SET_IPC5;
    while(HWREG(IPC_BASE + IPC_O_FLG) & IPC_FLG_IPC5);
    asm(" NOP");
    asm(" NOP");

    // Wait until CPU1 is configuring SCI-B for CPU2 in easyDSP_SCI_Init()
    DEVICE_DELAY_US(1000*10);
}
#endif
#endif

// easyDSP commands & states
#define STAT_INIT       0
#define STAT_ADDR       1
#define STAT_DATA2B     2
#define STAT_DATA4B     3
#define STAT_WRITE      4
#define STAT_DATA8B     5
#define STAT_CONFIRM    6
#define STAT_DATA_BLOCK 7

#define CMD_ADDR            0xE7
#define CMD_ADDR_32BIT      0x41    // 32bit address from v10.1
//#define CMD_READ1B        0x12  not used
#define CMD_READ2B          0xDB
#define CMD_READ4B          0xC3
#define CMD_READ8B          0x8B
#define CMD_READ16B         0x28
//#define CMD_DATA1B        0x30  not used
#define CMD_DATA2B          0xBD
#define CMD_DATA4B          0x99
#define CMD_DATA8B          0x64
#define CMD_WRITE           0x7E
#define CMD_FB_READ         0x0D
#define CMD_FB_WRITE_OK     0x0D
#define CMD_FB_WRITE_NG     0x3C
#define CMD_DATA_BLOCK      0x55
#define CMD_CHANGE_CPU      0X5A
#define CMD_CONFIRM         0xA5

// error counter
unsigned int ezDSP_uBRKDTCount = 0, ezDSP_uFECount = 0, ezDSP_uOECount = 0, ezDSP_uPECount = 0;
unsigned int ezDSP_uWrongISRCount = 0;

// for easyDSP
unsigned char ezDSP_ucRx = 0;
unsigned int ezDSP_uState = STAT_INIT, ezDSP_uData = 0, ezDSP_uChksum = 0;
unsigned long ezDSP_ulData = 0, ezDSP_ulAddr = 0;
unsigned int ezDSP_uAddrRdCnt = 0, ezDSP_uDataRdCnt = 0;
unsigned long long ezDSP_ullData = 0;
unsigned int ezDSP_uBlockSize = 0, ezDSP_uBlockIndex = 0, ezDSP_uChkSumCalculated = 0;
unsigned int ezDSP_uCommand = 0;

unsigned int ezDSP_uISRRxCount = 0, ezDSP_uISRTxCount = 0;
unsigned int ezDSP_uRxFifoCnt = 0, ezDSP_uMaxRxFifoCnt = 0, ezDSP_uTxFifoCnt = 0, ezDSP_uMaxTxFifoCnt = 0;

unsigned int ezDSP_uTxFifoFullCnt = 0;      // something wrong if not zero
inline void AddRing(char y) {
    if(SCI_getTxFIFOStatus(SCIx_BASE) != SCI_FIFO_TX16)     SCI_writeCharNonBlocking(SCIx_BASE, y);
    else                                                    ezDSP_uTxFifoFullCnt++;

    // counting after input to Tx
    ezDSP_uTxFifoCnt = SCI_getTxFIFOStatus(SCIx_BASE);
    if(ezDSP_uTxFifoCnt > ezDSP_uMaxTxFifoCnt) ezDSP_uMaxTxFifoCnt = ezDSP_uTxFifoCnt;
}

__interrupt void easy_RXINT_ISR(void)
{
    INT_NESTING_START;

    unsigned int uIndex;
    unsigned int u16X;

    ezDSP_uISRRxCount++;

    // check RX Error
    u16X = SCI_getRxStatus(SCIx_BASE);
    if(u16X & SCI_RXSTATUS_ERROR) {
        if(u16X & SCI_RXSTATUS_BREAK)   ezDSP_uBRKDTCount++;    // Break Down
        if(u16X & SCI_RXSTATUS_FRAMING) ezDSP_uFECount++;       // FE
        if(u16X & SCI_RXSTATUS_OVERRUN) ezDSP_uOECount++;       // OE
        if(u16X & SCI_RXSTATUS_PARITY)  ezDSP_uPECount++;       // PE

        // 'Break down' stops further Rx operation.
        // software reset is necessary to clear its status bit and proceed further rx operation
        SCI_performSoftwareReset(SCIx_BASE);

        SCI_clearOverflowStatus(SCIx_BASE);
        SCI_clearInterruptStatus(SCIx_BASE, SCI_INT_RXFF);
        INT_NESTING_END;
        return;
    }

    // monitoring
    ezDSP_uRxFifoCnt = SCI_getRxFIFOStatus(SCIx_BASE);
    if(ezDSP_uRxFifoCnt > ezDSP_uMaxRxFifoCnt) ezDSP_uMaxRxFifoCnt = ezDSP_uRxFifoCnt;

    if(SCI_getRxFIFOStatus(SCIx_BASE) == SCI_FIFO_RX0) {
        ezDSP_uWrongISRCount++;
        SCI_clearOverflowStatus(SCIx_BASE);
        SCI_clearInterruptStatus(SCIx_BASE, SCI_INT_RXFF);
        INT_NESTING_END;
        return;
    }

    // FIFO
    while(SCI_getRxFIFOStatus(SCIx_BASE) != SCI_FIFO_RX0) {
        ezDSP_ucRx = SCI_readCharNonBlocking(SCIx_BASE);

        // loop back for test
        //SCI_writeCharNonBlocking(SCIx_BASE, ezDSP_ucRx);
        //SCI_clearOverflowStatus(SCIx_BASE);
        //SCI_clearInterruptStatus(SCIx_BASE, SCI_INT_RXFF);
        //INT_NESTING_END;
        //return;

        ////////////////////////////////////////////
        // Parsing by state
        ////////////////////////////////////////////

        if(ezDSP_uState == STAT_INIT) {
            if(ezDSP_ucRx == CMD_ADDR || ezDSP_ucRx == CMD_ADDR_32BIT) {
                ezDSP_uState = STAT_ADDR;
                ezDSP_uAddrRdCnt = 0;
                ezDSP_b32bitAddress = (ezDSP_ucRx == CMD_ADDR_32BIT);
            }
            else if(ezDSP_ucRx == CMD_READ2B) {
                ezDSP_ulAddr++; // auto increment
                ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;

                AddRing(ezDSP_uData >> 8);  // MSB
                AddRing(ezDSP_uData);       // LSB
                AddRing(CMD_FB_READ);

                ezDSP_uState = STAT_INIT;
            }
            else if(ezDSP_ucRx == CMD_READ16B) {
                ezDSP_ulAddr += 8;
                for(uIndex = 0; uIndex < 8; uIndex++) {
                    // Since this is not for variable, address is increased sequentially
                    ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + uIndex);
                    AddRing(ezDSP_uData >> 8);      // MSB
                    AddRing(ezDSP_uData);           // LSB
                }
                AddRing(CMD_FB_READ);

                ezDSP_uState = STAT_INIT;
            }
            else if(ezDSP_ucRx == CMD_DATA2B) {
                ezDSP_ulAddr++; // auto increment

                ezDSP_uState = STAT_DATA2B;
                ezDSP_uDataRdCnt = 0;
            }
            else if(ezDSP_ucRx == CMD_DATA4B) {
                ezDSP_ulAddr += 2;  // auto increment

                ezDSP_uState = STAT_DATA4B;
                ezDSP_uDataRdCnt = 0;
            }
            else if(ezDSP_ucRx == CMD_DATA8B) {
                ezDSP_ulAddr += 4;  // auto increment

                ezDSP_uState = STAT_DATA8B;
                ezDSP_uDataRdCnt = 0;
            }
        }
        else if(ezDSP_uState == STAT_ADDR) {
            ezDSP_uAddrRdCnt++;
            if(ezDSP_uAddrRdCnt == 1) {
                ezDSP_ulAddr = ezDSP_ucRx;          // MSB
            }
            else if(ezDSP_uAddrRdCnt == 2 || ezDSP_uAddrRdCnt == 3 || (ezDSP_b32bitAddress && ezDSP_uAddrRdCnt == 4 )) {
                ezDSP_ulAddr <<= 8;
                ezDSP_ulAddr |= ezDSP_ucRx;
            }
            else if((!ezDSP_b32bitAddress && ezDSP_uAddrRdCnt == 4) || (ezDSP_b32bitAddress && ezDSP_uAddrRdCnt == 5)) {
                if(ezDSP_ucRx == CMD_READ2B) {
                    ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;

                    AddRing(ezDSP_uData >> 8);  // MSB
                    AddRing(ezDSP_uData);       // LSB
                    AddRing(CMD_FB_READ);

                    ezDSP_uState = STAT_INIT;
                }
                else if(ezDSP_ucRx == CMD_READ4B) {
                    ezDSP_ulData = *(unsigned long *)ezDSP_ulAddr;
                    AddRing(ezDSP_ulData >> 24);  // MSB
                    AddRing(ezDSP_ulData >> 16);
                    AddRing(ezDSP_ulData >> 8);
                    AddRing(ezDSP_ulData);        // LSB
                    AddRing(CMD_FB_READ);
                    ezDSP_uState = STAT_INIT;
                }
                else if(ezDSP_ucRx == CMD_READ8B) {
                    ezDSP_ullData = *(unsigned long long *)ezDSP_ulAddr;
                    AddRing(ezDSP_ullData >> (8*7));   // MSB
                    AddRing(ezDSP_ullData >> (8*6));
                    AddRing(ezDSP_ullData >> (8*5));
                    AddRing(ezDSP_ullData >> (8*4));
                    AddRing(ezDSP_ullData >> (8*3));
                    AddRing(ezDSP_ullData >> (8*2));
                    AddRing(ezDSP_ullData >> (8*1));
                    AddRing(ezDSP_ullData);            // LSB
                    AddRing(CMD_FB_READ);
                    ezDSP_uState = STAT_INIT;
                }
                else if(ezDSP_ucRx == CMD_READ16B) {
                    for(uIndex = 0; uIndex < 8; uIndex++) {
                        // Since this is not for variable, address is increased sequentially
                        ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + uIndex);
                        AddRing(ezDSP_uData >> 8);      // MSB
                        AddRing(ezDSP_uData);           // LSB
                    }
                    AddRing(CMD_FB_READ);
                    ezDSP_uState = STAT_INIT;
                }
                else if(ezDSP_ucRx == CMD_DATA2B) {
                    ezDSP_uState = STAT_DATA2B;
                    ezDSP_uDataRdCnt = 0;
                }
                else if(ezDSP_ucRx == CMD_DATA4B) {
                    ezDSP_uState = STAT_DATA4B;
                    ezDSP_uDataRdCnt = 0;
                }
                else if(ezDSP_ucRx == CMD_DATA8B) {
                    ezDSP_uState = STAT_DATA8B;
                    ezDSP_uDataRdCnt = 0;
                }
                else ezDSP_uState = STAT_INIT;
            }
            else
                ezDSP_uState = STAT_INIT;
        }
        else if(ezDSP_uState == STAT_DATA2B) {
            ezDSP_uDataRdCnt++;
            if(ezDSP_uDataRdCnt == 1)       ezDSP_uData = ezDSP_ucRx << 8;      // MSB
            else if(ezDSP_uDataRdCnt == 2)  ezDSP_uData |= ezDSP_ucRx;          // LSB
            else if(ezDSP_uDataRdCnt == 3)  ezDSP_uChksum = ezDSP_ucRx << 8;    // MSB
            else if(ezDSP_uDataRdCnt == 4)  ezDSP_uChksum |= ezDSP_ucRx;        // LSB
            else if(ezDSP_uDataRdCnt == 5) {
                if(ezDSP_ucRx == CMD_WRITE) {
                    if(ezDSP_uChksum == ((ezDSP_ulAddr + ezDSP_uData) & 0xFFFF)) {
                        *(unsigned int*)ezDSP_ulAddr = ezDSP_uData;
                        AddRing(CMD_FB_WRITE_OK);
                        ezDSP_uState = STAT_INIT;
                    }
                    else {
                        AddRing(CMD_FB_WRITE_NG);
                        ezDSP_uState = STAT_INIT;
                    }
                }
                else
                    ezDSP_uState = STAT_INIT;
            }
            else
                ezDSP_uState = STAT_INIT;
        }
        else if(ezDSP_uState == STAT_DATA4B) {
            ezDSP_uDataRdCnt++;
            if(ezDSP_uDataRdCnt == 1) {
                ezDSP_ulData = ezDSP_ucRx;      // MSB
                ezDSP_ulData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 2) {
                ezDSP_ulData |= ezDSP_ucRx;
                ezDSP_ulData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 3) {
                ezDSP_ulData |= ezDSP_ucRx;
                ezDSP_ulData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 4) {
                ezDSP_ulData |= ezDSP_ucRx;
            }
            else if(ezDSP_uDataRdCnt == 5)
                ezDSP_uChksum = ezDSP_ucRx << 8;    // MSB
            else if(ezDSP_uDataRdCnt == 6)
                ezDSP_uChksum |= ezDSP_ucRx;        // LSB
            else if(ezDSP_uDataRdCnt == 7) {
                if(ezDSP_ucRx == CMD_WRITE) {
                    if(ezDSP_uChksum == ((ezDSP_ulAddr + ezDSP_ulData) & 0xFFFF)) {
                        *(unsigned long*)ezDSP_ulAddr = ezDSP_ulData;
                        AddRing(CMD_FB_WRITE_OK);
                        ezDSP_uState = STAT_INIT;
                    }
                    else {
                        AddRing(CMD_FB_WRITE_NG);
                        ezDSP_uState = STAT_INIT;
                    }
                }
                else
                    ezDSP_uState = STAT_INIT;
            }
            else
                ezDSP_uState = STAT_INIT;
        }
        else if(ezDSP_uState == STAT_DATA8B) {
            ezDSP_uDataRdCnt++;
            if(ezDSP_uDataRdCnt == 1) {
                ezDSP_ullData = ezDSP_ucRx;         // MSB
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 2) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 3) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 4) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 5) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 6) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 7) {
                ezDSP_ullData |= ezDSP_ucRx;
                ezDSP_ullData <<= 8;
            }
            else if(ezDSP_uDataRdCnt == 8) {
                ezDSP_ullData |= ezDSP_ucRx;
            }
            else if(ezDSP_uDataRdCnt == 9)
                ezDSP_uChksum = ezDSP_ucRx << 8;    // MSB
            else if(ezDSP_uDataRdCnt == 10)
                ezDSP_uChksum |= ezDSP_ucRx;        // LSB
            else if(ezDSP_uDataRdCnt == 11) {
                if(ezDSP_ucRx == CMD_WRITE) {
                    if(ezDSP_uChksum == ((ezDSP_ulAddr + ezDSP_ullData) & 0xFFFF)) {
                        *(unsigned long long*)ezDSP_ulAddr = ezDSP_ullData;
                        AddRing(CMD_FB_WRITE_OK);
                        ezDSP_uState = STAT_INIT;
                    }
                    else {
                        AddRing(CMD_FB_WRITE_NG);
                        ezDSP_uState = STAT_INIT;
                    }
                }
                else
                    ezDSP_uState = STAT_INIT;
            }
            else
                ezDSP_uState = STAT_INIT;
        }
        else {
            ezDSP_uState = STAT_INIT;
        }
    }

    SCI_clearOverflowStatus(SCIx_BASE);
    SCI_clearInterruptStatus(SCIx_BASE, SCI_INT_RXFF);

    INT_NESTING_END;
}
