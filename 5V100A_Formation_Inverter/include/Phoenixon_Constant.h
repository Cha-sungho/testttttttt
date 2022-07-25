//###########################################################################
//
// FILE:   Phoenixon_Constant.h
//
//###########################################################################

#ifndef __CONSTANT_H__
    #define __CONSTANT_H__

#include "../main.h"

    //#define Tsamp	66.6666666e-6	//15kHz
    //#define Tsamp	80.e-6	//12.5kHz
    #define Tsamp	90.90909091e-6	//11kHz
    //#define Tsamp	100.e-6	//10kHz

    #define Fsamp       (float)(1. / Tsamp)
    #define Fix_Tsamp   (Tsamp * 1.e6)
    #define Fix_Fsamp   (Fsamp * 1.e-3)

    #define REF_TIME(time)      (unsigned long)((time * Fsamp) + 0.5)
    #define REF_ms_TIME(time)   (unsigned long)((time * Fix_Fsamp) + 0.5)

    //#define PWM_INV_SwitchingFrequency  ((200.e6 / 30.e3) + 0.5)//30kHz, 6666
    #define PWM_INV_SwitchingFrequency  ((200.e6 / 35.e3) + 0.5)//35kHz, 5714
    //#define PWM_INV_SwitchingFrequency  ((200.e6 / 40.e3) + 0.5)//40kHz, 5000
#if (Formation_Inverter == 0)
//    #define PWM_SRC_SwitchingFrequency  ((200.e6 / 64.e3) + 0.5)//32kHz, 6250
    #define PWM_SRC_SwitchingFrequency  ((200.e6 / 52e3) + 0.5)//26kHz, 7692
#else
//#define PWM_SRC_SwitchingFrequency  ((200.e6 / 70.26e3) + 0.5)//35.13kHz            // 22.01.12     Formation Inverter (2.8uF, 7uH)
//#define PWM_SRC_SwitchingFrequency  ((200.e6 / 50.48e3) + 0.5)//25.24kHz            // 22.03.19     Trans Sample Test (2.8uF, 14.2uH)
#define PWM_SRC_SwitchingFrequency  ((200.e6 / 54.91e3) + 0.5)//27.456kHz            // 22.06.20     Apply New Trans (2.8uF, 12uH)
#endif
    //#define PWM_BB_SwitchingFrequency   ((200.e6 / 30.e3) + 0.5)//30kHz, 6666
    #define PWM_BB_SwitchingFrequency   ((200.e6 / 40.e3) + 0.5)//40kHz, 5000


    #define PWM_MAX_INV PWM_INV_SwitchingFrequency
    #define PWM_MAX_SRC PWM_SRC_SwitchingFrequency
    #define PWM_MAX_BB  (PWM_BB_SwitchingFrequency-2)



    #define PI          3.141592653589793
    #define _2PI        (2. * PI)
    #define	_PI2        (PI * 0.5)
    #define _2PI2       (_2PI / 2.)
    #define _2PI3       (_2PI / 3.)
    #define _2PI4       (_2PI / 4.)
    #define _2PI5       (_2PI / 5.)
    #define _2PI6       (_2PI / 6.)
    #define _2PI7       (_2PI / 7.)

    #define _radianTotable  (2048./360.)

    #define PI_inv6		0.523598775598298/* pi / 6. */
    #define PI_inv2		1.570796326794896/* pi / 2. */
    #define	root2       1.414213562373095/* sqrt(2) */
    #define	root3       1.732050807568877 /* sqrt(3) */
    #define	inv_root3	0.577350269189625 /* 1 / sqrt(3) */
    #define	inv3        0.333333333333333 /* 1 / 3. */
    #define root2_inv3  0.47140452
    #define root3_inv3  0.577350269
    #define two_inv3    0.666666666666667
    #define root2_inv_root3 0.81664966

    /*******************************/
    /* Related to Sine Table Value */
    /*******************************/
    #define	TF            325.949323452201647
    #define _PI2_TF       (_PI2 * TF)
    #define _2PI60        376.991118430775188
    #define AngToRad      57.29577951 /* 360./(2.*PI) */
    #define inv_AngToRad  0.017453292 /* (2.*PI)/360. */


    /**************************************/
    /* Useful define(made by Phoenixon)   */
    /**************************************/
    #define BYTE			unsigned char
    #define WORD			unsigned int
    #define LONG			unsigned long
    #define LLONG			unsigned long long

    #define PBYTE(X)		*(volatile BYTE *)(X)
    #define PWORD(X)		*(volatile WORD *)(X)
    #define PLONG(X)		*(volatile LONG *)(X)
    #define PLLONG(X)		*(volatile LLONG *)(X)

    #define BIT_MASK(bit)				(1 << (bit))
    #define GetBit(val, bit)			(((val) & BIT_MASK(bit)) >> (bit))
    #define SetBit(val, bit)			(val |= BIT_MASK(bit))
    #define ClearBit(val, bit)			(val &= ~BIT_MASK(bit))
    #define ToggleBit(val, bit)			(val ^= BIT_MASK(bit))

    #define key_ESC   0x01
    #define key_ENT   0x03
    #define key_UP    0x05
    #define key_DOWN  0x07
    #define key_LEFT  0x09
    #define key_RIGHT 0x0b
    #define key_RUN   0x0d
    #define key_STOP  0x0f

#endif





