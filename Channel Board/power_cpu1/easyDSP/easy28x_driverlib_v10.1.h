/***************************************************************
    easy28x_driverlib_v*.*.h
    v9.1 (Mar 2020) :   First release /w DriverLib based C2000Ware_2_01_00_00
    v9.2 (Apr 2020) :   1. supports F2838xS, F2838xDa and F28002x
                        2. enables pull-up for Rx pins to prevent unexpected Rx input when Rx pin is float (easyDSP is not connected)
                        3. MCU name change : F2837xD -> F2837xD_CPU1_CPU2, F2837xD_CPU1_ONLY -> F2837xD_CPU1
    v9.5 (Dec 2020) :   1. reduce uninitialized variables
                        2. CPU2 RAM booting for F2837xD and F2838xD
	v10.1 (Nov 2021) :  1. support 28003x
	                    2. support legacy definitions of pin mux info for gpio.c
	                    3. speed up for flashAPI wrapper (no change to normal easyDSP operation)
	                    4. support 32bit address width
****************************************************************/
#ifndef _EASY28X_DRIVERLIB_H__
#define _EASY28X_DRIVERLIB_H__

extern void	easyDSP_SCI_Init(void);
extern __interrupt void easy_RXINT_ISR(void);
extern void easyDSP_Boot_Sync(void);

/////////////////////////////////////////////////////////////////////////////////////////
// NOTICE : Please select or modify below MCU type, CPU_CLK, LSP_CLK, BAUDRATE,
//          interrupt nesting, ram function activation according to your target system
///////////////////////////////////////////////////////////////////////////////////////////
// select target MCU. only one.
// 1 = selected, 0 = not selected
#ifdef _28377D
#define F28002x                 0
#define F28003x                 0
#define F28004x                 0
#define F2807x                  0
#define F2837xS                 0
#define F2837xD_CPU1            1       // use only CPU1 (name changed from previous F2837xD_CPU1_ONLY)
#define F2837xD_CPU1_CPU2       0       // use both CPU1 and CPU2 (name changed from previous F2837xD)
#define F2838xS_CPU1            0       // use only CPU1
#define F2838xS_CPU1_CM         0       // use CPU1+CM
#define F2838xD_CPU1            0       // use only CPU1
#define F2838xD_CPU1_CPU2       0       // use CPU1+CPU2
#define F2838xD_CPU1_CM         0       // use CPU1+CM
#define F2838xD_CPU1_CPU2_CM    0       // use CPU1+CPU2+CM
#else
#define F28002x                 0
#define F28003x                 0
#define F28004x                 0
#define F2807x                  0
#define F2837xS                 0
#define F2837xD_CPU1            0       // use only CPU1 (name changed from previous F2837xD_CPU1_ONLY)
#define F2837xD_CPU1_CPU2       0       // use both CPU1 and CPU2 (name changed from previous F2837xD)
#define F2838xS_CPU1            0       // use only CPU1
#define F2838xS_CPU1_CM         0       // use CPU1+CM
#define F2838xD_CPU1            0       // use only CPU1
#define F2838xD_CPU1_CPU2       1       // use CPU1+CPU2
#define F2838xD_CPU1_CM         0       // use CPU1+CM
#define F2838xD_CPU1_CPU2_CM    0       // use CPU1+CPU2+CM
#endif
///////////////////////////////////////////////////////////////////////////////////////////
// Baudrate for easyDSP should be one of below
//#define BAUDRATE            9600L
//#define BAUDRATE            19200L
//#define BAUDRATE            38400L
//#define BAUDRATE            57600L
//#define BAUDRATE            86400L
#define BAUDRATE            115200L
//#define BAUDRATE            153600L
//#define BAUDRATE            192000L
//#define BAUDRATE            230400L
///////////////////////////////////////////////////////////////////////////////////////////
// interrupt nesting assuming easyDSP ISR has the lowest priority. If not, please change the code accordingly
#define INT_NESTING_START   {                                               \
    IER |= 0x0100;                                                          \
    HWREGH(PIECTRL_BASE + PIE_O_ACK) = 0xFFFF;                              \
    asm("       NOP");                                                      \
    EINT;                                                                   \
    }
#define INT_NESTING_END     DINT
//#define INT_NESTING_START                                                 // in case of no nesting
//#define INT_NESTING_END Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9)     // in case of no nesting
///////////////////////////////////////////////////////////////////////////////////////////
// if _FLASH is not predefined by CCS configuration, you can do it yourself when you run the program on flash
#ifndef _FLASH
//#define _FLASH            // please uncomment to use on-chip flash if _FLASH is not predefined.
#endif
////////////////////////////////////////////////////////////////////////////////////////////
// uncomment #pragma if fast SCI ISR run on the ram is required
#ifdef _FLASH
#pragma CODE_SECTION(easy_RXINT_ISR, ".TI.ramfunc");
#endif
#endif	// of _EASY28X_DRIVERLIB_H__
