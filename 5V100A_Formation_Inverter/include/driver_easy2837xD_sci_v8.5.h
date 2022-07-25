/***************************************************************
    easy2837xD_sci.h
	copyright (c) 2015 by Dae-Woong Chung
	All Rights Reserved.
****************************************************************/
#ifndef _EASY2837XD_SCI_H__
#define _EASY2837XD_SCI_H__

extern void	easyDSP_SCI_Init(void);
extern interrupt void easy_RXINT_ISR(void);
extern interrupt void easy_TXINT_ISR(void);

extern void easyDSP_SCIBootCPU2(void);
extern void easyDSP_FlashBootCPU2(void);
#endif	// of _EASY2837XD_SCI_H__