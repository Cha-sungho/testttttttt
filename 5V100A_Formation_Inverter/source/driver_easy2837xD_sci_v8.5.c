/***************************************************************
    easy2837xD_sci.c
	2014.10.28 : First release based TI header files v130
	2015.01.31 : based on TI header files v140
	             coding for CPU2 (CPU2 takes SCI-B)
	2015.12.15 : 8 Byte writing with auto address increasement
				 block data (max. 512 word) transfer function
				 SciaRegs.SCIRXBUF.all -> ScibRegs.SCIRXBUF.bit.RXDT
	by Daewoong Chung (정대웅)
****************************************************************/
#include <driver_easy2837xD_sci_v8.5.h>
#include <driver_RingBuff.h>
#include "F2837xD_device.h"
#include "F28x_Project.h"
#include "F2837xD_Ipc_drivers.h"

/////////////////////////////////////////////////////////////////
// NOTICE : Don't uncomment ! This is for internal use only
/////////////////////////////////////////////////////////////////
//#define FLASH_API_WRAPPER
#ifdef FLASH_API_WRAPPER
#include "easy2837xD_ipc_v8.4.h"
#endif
#pragma CODE_SECTION(easy_RXINT_ISR, ".TI.ramfunc");
#pragma CODE_SECTION(easy_TXINT_ISR, ".TI.ramfunc");


/////////////////////////////////////////////////////////////////
// NOTICE :
// When you run the code with flash, uncomment below and run the ISRs fast on the RAM
/////////////////////////////////////////////////////////////////
//#pragma CODE_SECTION(easy_RXINT_ISR, "ramfuncs");
//#pragma CODE_SECTION(easy_TXINT_ISR, "ramfuncs");

/////////////////////////////////////////////////////////////////
// NOTICE : Please change below CPU_CLK, LSP_CLK, BAUDRATE
//          according to your system
//          Don't change any other variables and source
/////////////////////////////////////////////////////////////////
#define CPU_CLK		200000000L			// 200MHz
//#define CPU_CLK		190000000L			// 190MHz
//#define CPU_CLK		150000000L			// 150MHz
//#define CPU_CLK		120000000L			// 120MHz
//#define CPU_CLK		100000000L			// 100MHz

//#define	LSP_CLK		(CPU_CLK/4)				// default at reset
#define	LSP_CLK		(CPU_CLK/1)				// default at reset

//#define	BAUDRATE	9600L
//#define	BAUDRATE	19200L
//#define	BAUDRATE	38400L
//#define	BAUDRATE	57600L
//#define	BAUDRATE	86400L
#define	BAUDRATE	115200L
//#define	BAUDRATE	153600L
//#define	BAUDRATE	192000L
//#define	BAUDRATE	230400L
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// NOTICE : CPU2 uses SCI-B with SCIRXDB = GPIO 87, SCITXDB = GPIO86
//          If you likes to change its GPIO pin # according to your system requirement,
//			please also change its relative coding part
/////////////////////////////////////////////////////////////////
/*
// Bit codes for Test Bit instruction (BIT)
#ifdef BIT0
#elif
#define BIT0		(1<<0)
#define BIT1		(1<<1)
#define BIT2		(1<<2)
#define BIT3		(1<<3)
#define BIT4		(1<<4)
#define BIT5		(1<<5)
#define BIT6		(1<<6)
#define BIT7		(1<<7)
#endif
*/
#define BIT0		(1<<0)
#define BIT1		(1<<1)
#define BIT2		(1<<2)
#define BIT3		(1<<3)
#define BIT4		(1<<4)
#define BIT5		(1<<5)
#define BIT6		(1<<6)
#define BIT7		(1<<7)

// easyDSP commands & states
#define STAT_INIT		0
#define STAT_ADDR		1
#define STAT_DATA2B		2
#define STAT_DATA4B		3
#define STAT_WRITE		4
#define STAT_DATA8B		5
#define STAT_CONFIRM	6
#define STAT_DATA_BLOCK	7				// v8.5

#define CMD_ADDR			0xE7
#define	CMD_READ2B			0xDB
#define CMD_READ4B			0xC3
#define CMD_READ8B			0x8B
#define CMD_READ16B			0x28
#define	CMD_DATA2B			0xBD
#define	CMD_DATA4B			0x99
#define	CMD_DATA8B			0x64
#define CMD_DATA_BLOCK		0x55		// v8.5
#define	CMD_WRITE			0x7E
#define	CMD_FB_READ			0x0D
#define	CMD_FB_WRITE_OK		0x0D
#define CMD_FB_WRITE_NG		0x3C
#define CMD_CHANGE_CPU		0X5A
#define CMD_CONFIRM			0xA5

volatile struct SCI_REGS *ezDSP_pScixRegs;			// for both CPU1 and CPU2. For easyDSP monitoring operation

#define	TX_START	{	if(ezDSP_pScixRegs->SCICTL2.bit.TXRDY) {	\
								ezDSP_pScixRegs->SCICTL2.all = BIT1 | BIT0;	\
								ezDSP_pScixRegs->SCITXBUF.all = ExtractRing();}	\
						}
#define	TX_STOP	{ezDSP_pScixRegs->SCICTL2.all = BIT1;}	// enable only Rx int


#ifdef CPU1
//#define C1C2_BROM_IPC_EXECUTE_BOOTMODE_CMD	0x00000013
//#define C1C2_BROM_BOOTMODE_BOOT_FROM_SCI	0x00000001
//#define C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH	0x0000000B
void easyDSP_SCIBootCPU2(void)
{
    // SCIA connected to CPU2
    EALLOW;
	CpuSysRegs.PCLKCR7.bit.SCI_A = 0;
	DevCfgRegs.CPUSEL5.bit.SCI_A = 1;	// This register must be configured prior to enabling the peripheral clocks.
	CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
	EDIS;

	// GPIO 84, 85 to CPU2
	GPIO_SetupPinMux(84, GPIO_MUX_CPU2, 5);
	GPIO_SetupPinMux(85, GPIO_MUX_CPU2, 5);

    // Assign all GS RAMs to CPU2
    EALLOW;
	MemCfgRegs.GSxMSEL.all = 0xFFFF;
	EDIS;

	IpcRegs.IPCBOOTMODE = C1C2_BROM_BOOTMODE_BOOT_FROM_SCI; 	// CPU1 to CPU2 IPC Boot Mode Register
	IpcRegs.IPCSENDCOM = BROM_IPC_EXECUTE_BOOTMODE_CMD; // C1C2_BROM_IPC_EXECUTE_BOOTMODE_CMD;	// CPU1 to CPU2 IPC Command Register
	IpcRegs.IPCSET.all = 0x80000001; // CPU1 to CPU2 IPC flag register

	//wait until CPU2 is booted and set IPC5
	while(IpcRegs.IPCSTS.bit.IPC5 != 1);
	IpcRegs.IPCACK.bit.IPC5 = 1; //clearing the acknowledgement flag

	// Assign all GS RAMs to CPU1
    EALLOW;
	MemCfgRegs.GSxMSEL.all = 0x0000;
	EDIS;

    // SCIA connected to CPU1
	EALLOW;
	CpuSysRegs.PCLKCR7.bit.SCI_A = 0;
	DevCfgRegs.CPUSEL5.bit.SCI_A = 0;	// This register must be configured prior to enabling the peripheral clocks.
	CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
	EDIS;
}

void easyDSP_FlashBootCPU2(void)
{
    // Wait until CPU02 control system boot ROM is ready to receive CPU01 to CPU02 INT1 interrupts.
	uint32_t bootStatus;
    while(1) {
        bootStatus = IPCGetBootStatus() & 0x0000000F;
        if(bootStatus == C2_BOOTROM_BOOTSTS_SYSTEM_READY) break;
    }

    // Loop until CPU02 control system IPC flags 1 and 32 are available
    while ((IPCLtoRFlagBusy(IPC_FLAG0) == 1) || (IPCLtoRFlagBusy(IPC_FLAG31) == 1));

	IpcRegs.IPCBOOTMODE = C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH; 	// CPU1 to CPU2 IPC Boot Mode Register
	IpcRegs.IPCSENDCOM = BROM_IPC_EXECUTE_BOOTMODE_CMD; // C1C2_BROM_IPC_EXECUTE_BOOTMODE_CMD;	// CPU1 to CPU2 IPC Command Register
	IpcRegs.IPCSET.all = 0x80000001; // CPU1 to CPU2 IPC flag register
}
#endif

#ifdef CPU2
void easyDSP_SCIBootCPU2(void)
{
    // booting feedback still working even this moment
    // So no switch SCIA channel to CPU1 immediately
	DELAY_US(1000*100);

	// set IPC5 to inform to CPU1 that booting is done
    IpcRegs.IPCSET.bit.IPC5 = 1;
    while(IpcRegs.IPCFLG.bit.IPC5 == 1);
	asm(" NOP");
	asm(" NOP");

    // Wait until CPU1 is configuring SCI-B for CPU2 in easyDSP_SCI_Init()
	DELAY_US(1000*10);
}
#endif


// for internal use
unsigned int ezDSP_Version_SCI = 850;
unsigned int ezDSP_uRead16BPossible = 1, ezDSP_uRead8BPossible = 1;
float ezDSP_fFloat = 0;
#if defined(CPU1)
unsigned int ezDSP_uCPU = 1;
#elif defined(CPU2)
unsigned int ezDSP_uCPU = 2;
#endif

void easyDSP_SCI_Init(void)
{
	int i;

	#ifdef CPU1
	///////////////////////////////////////////
	// SCI-A assigned to CPU1
	///////////////////////////////////////////
	EALLOW;
	CpuSysRegs.PCLKCR7.bit.SCI_A = 0;		// SCI-A CLOCK Disabled
	DevCfgRegs.CPUSEL5.bit.SCI_A = 0;		// This register must be configured prior to enabling the peripheral clocks.
	EDIS;

	///////////////////////////////////////////
    // SCI-B assigned to CPU2
    ///////////////////////////////////////////
	EALLOW;
	CpuSysRegs.PCLKCR7.bit.SCI_B = 0;		// SCI-B CLOCK Disabled
	DevCfgRegs.CPUSEL5.bit.SCI_B = 1;
	EDIS;

	///////////////////////////////////////////
	// SCI-A GPIO setting : SCIRXDA = GPIO 85, SCITXDA = GPIO84
	///////////////////////////////////////////
//	GPIO_SetupPinMux(84, GPIO_MUX_CPU1, 5);
//	GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 5);
//	GPIO_SetupPinOptions(84, GPIO_OUTPUT, GPIO_ASYNC);
//	GPIO_SetupPinOptions(85, GPIO_INPUT, GPIO_ASYNC);
//	EALLOW;
//	GpioCtrlRegs.GPCPUD.bit.GPIO85 = 0;		// Enable pull-up for GPIO85 (SCIRXDA)
//	GpioCtrlRegs.GPCPUD.bit.GPIO84 = 0;	   	// Enable pull-up for GPIO84 (SCITXDA)
//	EDIS;

    ///////////////////////////////////////////
    // SCI-A GPIO setting : SCIRXDA = GPIO 28, SCITXDA = GPIO29
    ///////////////////////////////////////////

    GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_ASYNC);
    GPIO_SetupPinOptions(28, GPIO_INPUT, GPIO_ASYNC);
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;
    EDIS;

    // easyDSP 포트를 GPIO28/29에 연결 (GPIO30을 사용하여)
    GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(30, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_WritePin(30, 0);


	///////////////////////////////////////////
	// SCI-B GPIO Setting : SCIRXDB = GPIO 87, SCITXDB = GPIO86
	///////////////////////////////////////////
	GPIO_SetupPinMux(86, GPIO_MUX_CPU2, 5);
	GPIO_SetupPinMux(87, GPIO_MUX_CPU2, 5);
	GPIO_SetupPinOptions(86, GPIO_OUTPUT, GPIO_ASYNC);
	GPIO_SetupPinOptions(87, GPIO_INPUT, GPIO_ASYNC);
	EALLOW;
//	GpioCtrlRegs.GPCPUD.bit.GPIO86 = 0;   	// Enable pull-up
//	GpioCtrlRegs.GPCPUD.bit.GPIO87 = 0;  	// Enable pull-up
	EDIS;
	#endif

	EALLOW;
	#if defined (CPU1)
	CpuSysRegs.PCLKCR7.bit.SCI_A = 1;		// SCI-A CLOCK Enabled
	#elif defined (CPU2)
	CpuSysRegs.PCLKCR7.bit.SCI_B = 1;		// SCI-B CLOCK Enabled
	#endif
	EDIS;

	#if defined (CPU1)
    ezDSP_pScixRegs = &SciaRegs;
	#elif defined (CPU2)
	//#ifdef FLASH_API_WRAPPER
    //ezDSP_pScixRegs = &SciaRegs;
	//#endif
    ezDSP_pScixRegs = &ScibRegs;
	#endif

    // SCI Registers Settings : no action = no use
	//ezDSP_pScixRegs->SCIFFTX.all = 0xa000;		// FIFO reset
	//ezDSP_pScixRegs->SCIFFCT.all = 0x4000;		// Clear ABD
 	
    ezDSP_pScixRegs->SCICCR.all = 0x7;						// 1 stop & no parity & 8bit char, no loopback, idle-line
    ezDSP_pScixRegs->SCICTL1.all = BIT6 | BIT1 | BIT0;		// enable RX-ERR interrupt, TX and RX operation

	i = (int)(((float)LSP_CLK/(BAUDRATE*8.) - 1) + 0.5);
	ezDSP_pScixRegs->SCIHBAUD.all = i >> 8;
	ezDSP_pScixRegs->SCILBAUD.all = i & 0xFF;

	ezDSP_pScixRegs->SCICTL2.all = 0x3;			// enable RX/BK INT, TX INT
	ezDSP_pScixRegs->SCICTL1.all = BIT6 | BIT5 | BIT1 | BIT0;	// Relinquish SCI from Reset

    // Reassign ISR for easyDSP.
	EALLOW;
	#if defined (CPU1)
	PieVectTable.SCIA_RX_INT = &easy_RXINT_ISR;
	PieVectTable.SCIA_TX_INT = &easy_TXINT_ISR;
	#elif defined (CPU2)
	#ifdef FLASH_API_WRAPPER
	PieVectTable.SCIA_RX_INT = &easy_RXINT_ISR;
	PieVectTable.SCIA_TX_INT = &easy_TXINT_ISR;
	#endif
	PieVectTable.SCIB_RX_INT = &easy_RXINT_ISR;
	PieVectTable.SCIB_TX_INT = &easy_TXINT_ISR;
	#endif
	EDIS;

	// Enable interrupts required
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;		// Enable the PIE block
	#if defined (CPU1)
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;		// Enable SCI-A RX/TX INT in the PIE: Group 9 interrupt 1,2
	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
	IER |= M_INT9;							// Enable CPU INT9 for SCI-A  (M_INT9 = 0x100)
	#elif defined (CPU2)
	#ifdef FLASH_API_WRAPPER
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;		// Enable SCI-A RX/TX INT in the PIE: Group 9 interrupt 1,2
	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
	#endif
	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;		// Enable SCI-B RX/TX INT in the PIE: Group 9 interrupt 3,4
	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;
	IER |= M_INT9;							// Enable CPU INT9 for SCI-B  (M_INT9 = 0x100)
	#endif
	EINT;									// Enable Global interrupt INTM

	// init ring buffer
	ResetRing();
}

// error counter
unsigned int ezDSP_uBRKDTCount = 0, ezDSP_uFECount = 0, ezDSP_uOECount = 0, ezDSP_uPECount = 0;
unsigned int ezDSP_uWrongISRCount = 0;

// for easyDSP
unsigned char ezDSP_ucRx = 0;
unsigned int ezDSP_uState = STAT_INIT, ezDSP_uData = 0, ezDSP_uChksum = 0;
unsigned long ezDSP_ulData = 0, ezDSP_ulAddr = 0;
unsigned int ezDSP_uAddrRdCnt = 0, ezDSP_uDataRdCnt = 0;
unsigned long long ezDSP_ullData = 0;
unsigned int ezDSP_uCommand = 0;
#ifdef FLASH_API_WRAPPER
unsigned int ezDSP_uBlockTransfer = 1, ezDSP_uBlockSize = 0, ezDSP_uBlockIndex = 0, ezDSP_uChkSumCalculated = 0; // v8.5
#endif
interrupt void easy_RXINT_ISR(void)
{
	///////////////////////////////////////////////////////////////////////////
	// interrupt nesting to allow another interrupts before the completion of this ISR
	// Please kindly use your own code for this purpose if necessary
	///////////////////////////////////////////////////////////////////////////
	//EINT;
	//IER |= M_INT9;
	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	///////////////////////////////////////////////////////////////////////////

	Uint16 uIndex;

	#ifdef CPU2
	#ifdef FLASH_API_WRAPPER
	ezDSP_pScixRegs = &SciaRegs;
	#endif
	#endif

	// check RX Error
	if(ezDSP_pScixRegs->SCIRXST.bit.RXERROR) {
		if(ezDSP_pScixRegs->SCIRXST.bit.BRKDT)	ezDSP_uBRKDTCount++;	// Break Down
		if(ezDSP_pScixRegs->SCIRXST.bit.FE) 	ezDSP_uFECount++;		// FE
		if(ezDSP_pScixRegs->SCIRXST.bit.OE) 	ezDSP_uOECount++;		// OE
		if(ezDSP_pScixRegs->SCIRXST.bit.PE)		ezDSP_uPECount++;		// PE

		// 'Break down' stops further Rx operation.
		// software reset is necessary to clear its status bit and proceed further rx operation
		ezDSP_pScixRegs->SCICTL1.bit.SWRESET = 0;
		ezDSP_pScixRegs->SCICTL1.bit.SWRESET = 1;

		PieCtrlRegs.PIEACK.all |= 0x100;
		return;
	}

	// Receive Char
	if(ezDSP_pScixRegs->SCIRXST.bit.RXRDY) {
		ezDSP_ucRx = ezDSP_pScixRegs->SCIRXBUF.bit.SAR;
		// RXRDY is cleared by a reading of the SCIRXBUF register, by an active SW RESET, or by a system reset.

		// loop back for test
		//ezDSP_pScixRegs->SCITXBUF.all = ezDSP_ucRx;
		//PieCtrlRegs.PIEACK.all |= 0x100;
		//return;
	}
	else {
		ezDSP_uWrongISRCount++;
		PieCtrlRegs.PIEACK.all |= 0x100;
		return;
	}

	////////////////////////////////////////////
	// Parsing by state
	////////////////////////////////////////////
	
	if(ezDSP_uState == STAT_INIT) {
		if(ezDSP_ucRx == CMD_ADDR) {
			ezDSP_uState = STAT_ADDR;
			ezDSP_uAddrRdCnt = 0;
		}
		else if(ezDSP_ucRx == CMD_READ2B) {
			ezDSP_ulAddr++;	// auto increment
			ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;
			
			AddRing(ezDSP_uData >> 8);	// MSB
			AddRing(ezDSP_uData);		// LSB
			AddRing(CMD_FB_READ);
			
			ezDSP_uState = STAT_INIT;
			TX_START;
		}
		else if(ezDSP_ucRx == CMD_READ16B) {
			ezDSP_ulAddr += 8;
			for(uIndex = 0; uIndex < 8; uIndex++) {
				// Since this is not for variable, addresss is increased sequentially
				ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + uIndex);
				AddRing(ezDSP_uData >> 8);		// MSB
				AddRing(ezDSP_uData);			// LSB
			}
			AddRing(CMD_FB_READ);
	
			ezDSP_uState = STAT_INIT;
			TX_START;
		}
		else if(ezDSP_ucRx == CMD_DATA2B) {
			ezDSP_ulAddr++;	// auto increment

			ezDSP_uState = STAT_DATA2B;
			ezDSP_uDataRdCnt = 0;
		}
		else if(ezDSP_ucRx == CMD_DATA4B) {
			ezDSP_ulAddr += 2;	// auto increment

			ezDSP_uState = STAT_DATA4B;
			ezDSP_uDataRdCnt = 0;
		}
		// v8.5
		else if(ezDSP_ucRx == CMD_DATA8B) {
			ezDSP_ulAddr += 4;	// auto increment

			ezDSP_uState = STAT_DATA8B;
			ezDSP_uDataRdCnt = 0;
		}
		else if(ezDSP_ucRx == CMD_CHANGE_CPU) {
			ezDSP_uCommand = CMD_CHANGE_CPU;
			ezDSP_uState = STAT_CONFIRM;
		}
	}
	else if(ezDSP_uState == STAT_ADDR) {
		ezDSP_uAddrRdCnt++;
		if(ezDSP_uAddrRdCnt == 1) {
			ezDSP_ulAddr = ezDSP_ucRx; 			// MSB
			ezDSP_ulAddr <<= 16; 				// MSB
		}
		else if(ezDSP_uAddrRdCnt == 2)
			ezDSP_ulAddr |= (ezDSP_ucRx << 8);

		else if(ezDSP_uAddrRdCnt == 3)
			ezDSP_ulAddr |= ezDSP_ucRx;			// LSB

		else if(ezDSP_uAddrRdCnt == 4) {
			if(ezDSP_ucRx == CMD_READ2B) {
				ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;
				
				AddRing(ezDSP_uData >> 8);	// MSB
				AddRing(ezDSP_uData);		// LSB
				AddRing(CMD_FB_READ);
				
				ezDSP_uState = STAT_INIT;
				TX_START;
			}
			else if(ezDSP_ucRx == CMD_READ4B) {
				ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + 1);
				AddRing(ezDSP_uData >> 8);	// MSB
				AddRing(ezDSP_uData);		

				ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;
				AddRing(ezDSP_uData >> 8);	
				AddRing(ezDSP_uData);		// LSB

				AddRing(CMD_FB_READ);
				
				ezDSP_uState = STAT_INIT;
				TX_START;
			}
			else if(ezDSP_ucRx == CMD_READ8B) {
				ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + 3);
				AddRing(ezDSP_uData >> 8);	// MSB
				AddRing(ezDSP_uData);		

				ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + 2);
				AddRing(ezDSP_uData >> 8);	
				AddRing(ezDSP_uData);

				ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + 1);
				AddRing(ezDSP_uData >> 8);
				AddRing(ezDSP_uData);		

				ezDSP_uData = *(unsigned int*)ezDSP_ulAddr;
				AddRing(ezDSP_uData >> 8);	
				AddRing(ezDSP_uData);		// LSB

				AddRing(CMD_FB_READ);
				
				ezDSP_uState = STAT_INIT;
				TX_START;
			}
			else if(ezDSP_ucRx == CMD_READ16B) {
				for(uIndex = 0; uIndex < 8; uIndex++) {
					// Since this is not for variable, addresss is increased sequentially
					ezDSP_uData = *(unsigned int*)(ezDSP_ulAddr + uIndex);
					AddRing(ezDSP_uData >> 8);		// MSB
					AddRing(ezDSP_uData);			// LSB
				}
				AddRing(CMD_FB_READ);
				
				ezDSP_uState = STAT_INIT;
				TX_START;
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
			// v8.5
			else if(ezDSP_ucRx == CMD_DATA_BLOCK) {
				ezDSP_uState = STAT_DATA_BLOCK;
				ezDSP_uDataRdCnt = 0;
			}
			else ezDSP_uState = STAT_INIT;
		}
		else 
			ezDSP_uState = STAT_INIT;
	}
	
	else if(ezDSP_uState == STAT_DATA2B) {
		ezDSP_uDataRdCnt++;
		if(ezDSP_uDataRdCnt == 1)
			ezDSP_uData = ezDSP_ucRx << 8; 		// MSB
		else if(ezDSP_uDataRdCnt == 2)
			ezDSP_uData |= ezDSP_ucRx; 			// LSB
		else if(ezDSP_uDataRdCnt == 3) 		
			ezDSP_uChksum = ezDSP_ucRx << 8;	// MSB
		else if(ezDSP_uDataRdCnt == 4)
			ezDSP_uChksum |= ezDSP_ucRx;		// LSB
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
				TX_START;
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
			ezDSP_ulData = ezDSP_ucRx; 		// MSB
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
			ezDSP_uChksum = ezDSP_ucRx << 8;	// MSB
		else if(ezDSP_uDataRdCnt == 6)
			ezDSP_uChksum |= ezDSP_ucRx;		// LSB
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
				TX_START;
			}
			else
				ezDSP_uState = STAT_INIT;
		}
		else
			ezDSP_uState = STAT_INIT;
	}
	// new
	else if(ezDSP_uState == STAT_DATA8B) {
		ezDSP_uDataRdCnt++;
		if(ezDSP_uDataRdCnt == 1) {
			ezDSP_ullData = ezDSP_ucRx; 		// MSB
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
			ezDSP_uChksum = ezDSP_ucRx << 8;	// MSB
		else if(ezDSP_uDataRdCnt == 10)
			ezDSP_uChksum |= ezDSP_ucRx;		// LSB
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
				TX_START;
			}
			else
				ezDSP_uState = STAT_INIT;
		}
		else
			ezDSP_uState = STAT_INIT;
	}
#ifdef FLASH_API_WRAPPER
	// v8.5
	else if(ezDSP_uState == STAT_DATA_BLOCK) {
		ezDSP_uDataRdCnt++;
		// block size
		if(ezDSP_uDataRdCnt == 1) {
			ezDSP_uBlockSize = ezDSP_ucRx; 		// MSB. size in word
			ezDSP_uBlockSize <<= 8;
		}
		else if(ezDSP_uDataRdCnt == 2) {
			ezDSP_uBlockSize |= ezDSP_ucRx;		// LSB
			ezDSP_uChkSumCalculated = ezDSP_uBlockSize + ezDSP_ulAddr;
			ezDSP_uBlockIndex = 0;
		}
		// data transfer : ezDSP_uBlockSize = in words
		else if((ezDSP_uDataRdCnt >= 3) && (ezDSP_uDataRdCnt < (3 + ezDSP_uBlockSize*2))) {
			if(ezDSP_uBlockIndex == 0) {
				ezDSP_uData =  ezDSP_ucRx;
				ezDSP_uBlockIndex = 1;
			}
			else if(ezDSP_uBlockIndex == 1 ) {
				ezDSP_uData <<= 8;
				ezDSP_uData |=  ezDSP_ucRx;
				ezDSP_uBlockIndex = 0;

				// transfer
				*(unsigned int*)ezDSP_ulAddr = ezDSP_uData;
				ezDSP_uChkSumCalculated += ezDSP_uData;
				ezDSP_ulAddr++;
			}
		}
		else if(ezDSP_uDataRdCnt == (3 + ezDSP_uBlockSize*2 + 0)) 	ezDSP_uChksum = ezDSP_ucRx << 8;	// MSB
		else if(ezDSP_uDataRdCnt == (3 + ezDSP_uBlockSize*2 + 1))	ezDSP_uChksum |= ezDSP_ucRx;		// LSB
		else if(ezDSP_uDataRdCnt == (3 + ezDSP_uBlockSize*2 + 2)) {
			if(ezDSP_ucRx == CMD_WRITE) {
				if(ezDSP_uChksum == ezDSP_uChkSumCalculated)	AddRing(CMD_FB_WRITE_OK);
				else 											AddRing(CMD_FB_WRITE_NG);
				ezDSP_uState = STAT_INIT;
				TX_START;
			}
			else
				ezDSP_uState = STAT_INIT;
		}
		else
			ezDSP_uState = STAT_INIT;
	}
#endif
	else if(ezDSP_uState == STAT_CONFIRM) {
		if(ezDSP_ucRx == CMD_CONFIRM) {
			if(ezDSP_uCommand == CMD_CHANGE_CPU) {
				// change CPU
				#if defined (CPU1)
				#ifdef FLASH_API_WRAPPER
				SCIA_TO_CPU2();
				#endif
				#elif defined (CPU2)
				#ifdef FLASH_API_WRAPPER
				IpcRegs.IPCSET.all = 0x8;			// CPU2 to CPU1 IPC4 interrupt
				#endif
				#endif
			}
		}
		ezDSP_uState = STAT_INIT;
	}
	else {
		ezDSP_uState = STAT_INIT;
	}

	// Acknowledge this interrupt to recieve more interrupts from group 9
	PieCtrlRegs.PIEACK.all |= 0x100;
}
                                              
interrupt void easy_TXINT_ISR(void)
{
	///////////////////////////////////////////////////////////////////////////
	// interrupt nesting to allow another interrupts before the completion of this ISR
	// Please kindly use your own code for this purpose if necessary
	///////////////////////////////////////////////////////////////////////////
	//EINT;
	//IER |= M_INT9;
	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	///////////////////////////////////////////////////////////////////////////

	// buffer => sio
	if(!IsRingEmpty()) {
		if(ezDSP_pScixRegs->SCICTL2.bit.TXRDY)	// check TXRDY
			ezDSP_pScixRegs->SCITXBUF.all = ExtractRing();
		else 
			ezDSP_uWrongISRCount++;
	}
	else {
		TX_STOP;
	}

	PieCtrlRegs.PIEACK.all |= 0x100;
}

