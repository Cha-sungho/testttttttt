/*
 * app_constant.h
 *
 *  Created on: Nov 10, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_APP_APP_CONSTANT_H_
#define PHOENIXON_APP_APP_CONSTANT_H_

#if 1
#define PI2         (1.570796327)
#define PI          (3.141592654)
#define SQRT3       (1.732050808)
#define INV_SQRT3   (0.577350269)
#define SQRT2       (1.414213562)
#define INV_SQRT2   (0.707106781)
#define INV3        (0.333333333333333)

#define F2          ((float)(1./2.))
#define F3          ((float)(F2/3.))
#define F4          ((float)(F3/4.))
#define F5          ((float)(F4/5.))
#define F6          ((float)(F5/6.))
#define F7          ((float)(F6/7.))
#define F8          ((float)(F7/8.))
#define F9          ((float)(F8/9.))
#define F10         ((float)(F9/10.))
#define F11         ((float)(F10/11.))
#define F12         ((float)(F11/12.))
#define F13         ((float)(F12/13.))
#define F14         ((float)(F13/14.))
#define F15         ((float)(F14/15.))
#else
#define PI2         (1.57079637)
#define PI          (3.14159274)
#define SQRT3       (1.73205078)
#define INV_SQRT3   (0.577350259)
#define SQRT2       (1.41421354)
#define INV_SQRT2   (0.707106769)

#define F2          ((float)(0.5))
#define F3          ((float)(0.166666672))
#define F4          ((float)(0.0416666679))
#define F5          ((float)(0.00833333377))
#define F6          ((float)(0.00138888892))
#define F7          ((float)(0.000198412701))
#define F8          ((float)(2.48015876e-05))
#define F9          ((float)(2.75573188e-06))
#define F10         ((float)(2.755732e-07))
#define F11         ((float)(2.50521097e-08))
#define F12         ((float)(2.08767581e-09))
#define F13         ((float)(1.60590444e-10))
#define F14         ((float)(1.14707462e-11))
#define F15         ((float)(7.64716415e-13))
#endif

#define BOUND_PI(x)    ((x) - (6.283185307)*(int)((x + PI) * (float)1.591549431E-01))

#define PI_CON(out, ref, in, iterm, kp, ki, tsamp, limit)       \
{                                                               \
    float err, tmp;                                             \
                                                                \
    err = ref - in;                                             \
    iterm += ki*tsamp*err;                                      \
    tmp = kp*err + iterm;                                       \
    out = BOUND(tmp, limit);                                    \
    tmp = out - kp*err;                                         \
    iterm = BOUND(tmp, limit);                                  \
}                                                               \

#define BOUND(in,lim)   ((in > (lim)) ? (lim) : ((in < -(lim)) ? -(lim) : in))
#define SIN(x,x2)       ((x)*((float)1.-(x2)*(F3-(x2)*(F5-(x2)*(F7-(x2)*(F9-(x2)*(F11-(x2)*(F13-(x2)*F15))))))))
#define COS(x2)         ((float)1.-(x2)*(F2-(x2)*(F4-(x2)*(F6-(x2)*(F8-(x2)*(F10-(x2)*(F12-(x2)*F14)))))))

#define BYTE            unsigned char
#define WORD            unsigned int
#define LONG            unsigned long
#define LLONG           unsigned long long

#define PBYTE(X)        *(volatile BYTE *)(X)
#define PWORD(X)        *(volatile WORD *)(X)
#define PLONG(X)        *(volatile LONG *)(X)
#define PLLONG(X)       *(volatile LLONG *)(X)

#define BIT_MASK(bit)           (1 << (bit))
#define GET_BIT(val, bit)       (((val) & BIT_MASK(bit)) >> (bit))
#define SET_BIT(val, bit)       (val |= BIT_MASK(bit))
#define CLR_BIT(val, bit)       (val &= ~BIT_MASK(bit))
#define TOGGLE_BIT(val, bit)    (val ^= BIT_MASK(bit))

//#define EMAVG_MACRO(in, out, multiplier) out = ((in - out)*multiplier) + out;

#endif /* PHOENIXON_APP_APP_CONSTANT_H_ */
