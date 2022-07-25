//#############################################################################
//
//  FILE:   abc_dq0_pos.h
//
//  TITLE:  ABC to DQ0 Positive Sequence Transform Module
//
//#############################################################################
// $TI Release: C2000Ware DigitalPower SDK v3.03.00.00 $
// $Release Date: Fri Sep 24 11:50:54 CDT 2021 $
// $Copyright:
// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// ALL RIGHTS RESERVED
// $
//#############################################################################

#ifndef ABC_DQ0_POS_H
#define ABC_DQ0_POS_H

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
//! \addtogroup ABC_DQ0_POSNEG
//! @{
//
//*****************************************************************************

//
// Included Files
//
#include <stdint.h>
#ifndef __TMS320C28XX_CLA__
#include <math.h>
#else
#include <CLAmath.h>
#endif

//#############################################################################
//
// Macro Definitions
//
//#############################################################################
#ifndef C2000_IEEE754_TYPES
#define C2000_IEEE754_TYPES
#ifdef __TI_EABI__
typedef float         float32_t;
typedef double        float64_t;
#else // TI COFF
typedef float         float32_t;
typedef long double   float64_t;
#endif // __TI_EABI__
#endif // C2000_IEEE754_TYPES

//
// Typedefs
//

//! \brief          Defines the ABC_DQ0_POS transform structure
//!
//! \details        This block stores variables used to transforms
//!                 ABC domain variables to dq domain
//!
typedef struct{
    float32_t alpha;   //!< Output: Alpha component (abc-> alpha beta)
    float32_t beta;    //!< Output: Beta component (abc-> alpha beta)
    float32_t d;       //!< Output: D axis component (alpha beta -> d,q,z)
    float32_t q;       //!< Output: Q axis component (alpha beta -> d,q,z)
    float32_t z;       //!< Output: Z axis component (alpha beta -> d,q,z)
}ABC_DQ0_POS;

//! \brief       Resets internal data to zero
//! \param *v    The ABC_DQ0_POS structure pointer
//! \return None
//!
static inline void ABC_DQ0_POS_reset(ABC_DQ0_POS *v)
{
    v->alpha = 0;
    v->beta = 0;
    v->d = 0;
    v->q = 0;
    v->z = 0;
}

//! \brief             Run ABC_DQ0_POS routine
//! \param *v          The ABC_DQ0_POS structure pointer
//! \param a           Phase a value
//! \param b           Phase b value
//! \param c           Phase c value
//! \param sine_val    sine value of the grid angle
//! \param cosine_val  cosine value of the grid angle
//! \return None
//!
static inline void ABC_DQ0_POS_run(ABC_DQ0_POS *v,
                                   float32_t a, float32_t b, float32_t c,
                                   float32_t sine_val, float32_t cosine_val)
{
    v->alpha = (0.66666666677f) * (a - 0.5f * (b + c));
    v->beta  = (0.57735026913f) * (b - c);
    v->z     = (0.57735026913f) * (a + b + c);
    v->d     =  v->alpha * cosine_val + v->beta * sine_val;
    v->q     = -v->alpha * sine_val   + v->beta * cosine_val;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of  _ABC_DQ0_POS_H_ definition

//
// End of File
//