/*
 * PhoenixonControls_ADC.h
 *
 *  Created on: 2018. 5. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_ADC_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_ADC_H_
//#include "cpu.h"
#include "debug.h"
//#include <stdbool.h>
//#include <stdint.h>
//#include "F28x_Project.h"

#define DSP_REFERENCE_VOLTAGE (float)(3.0)
#define DSP_RESOLUTION (float)(4096.0)
//#define DSP_ADC_GAIN (float)(0.000732421875) //(DSP_REFERENCE_VOLTAGE / DSP_RESOLUTION)
#define DSP_ADC_GAIN (float)(0.0000457763671875) //(DSP_REFERENCE_VOLTAGE / DSP_RESOLUTION)

#define ADCARESULT_BASE             0x00000B00U // ADCA Result Registers
#define ADCBRESULT_BASE             0x00000B20U // ADCB Result Registers
#define ADCCRESULT_BASE             0x00000B40U // ADCC Result Registers
#define ADCDRESULT_BASE             0x00000B60U // ADCD Result Registers

#define ADC_SOCxCTL_OFFSET_BASE     ADC_O_SOC0CTL
#define ADC_RESULTx_OFFSET_BASE     ADC_O_RESULT0
#define ADC_INTSELxNy_OFFSET_BASE   ADC_O_INTSEL1N2
#define ADC_PPBxRESULT_OFFSET_BASE  ADC_O_PPB1RESULT

//*****************************************************************************
//
//! Values that can be passed to ADC_setMode() as the \e resolution
//! parameter.
//
//*****************************************************************************
typedef enum
{
    ADC_RESOLUTION_12BIT = 0x00,    //!< 12-bit conversion resolution
    ADC_RESOLUTION_16BIT = 0x40     //!< 16-bit conversion resolution
} ADC_Resolution;

//*****************************************************************************
//
//! Values that can be passed to ADC_enableInterrupt(), ADC_disableInterrupt(),
//! and ADC_getInterruptStatus() as the \e adcIntNum parameter.
//
//*****************************************************************************
typedef enum
{
    ADC_INT_NUMBER1,        //!< ADCINT1 Interrupt
    ADC_INT_NUMBER2,        //!< ADCINT2 Interrupt
    ADC_INT_NUMBER3,        //!< ADCINT3 Interrupt
    ADC_INT_NUMBER4         //!< ADCINT4 Interrupt
} ADC_IntNumber;

//*****************************************************************************
//
//! Values that can be passed in as the \e ppbNumber parameter for several
//! functions.
//
//*****************************************************************************
typedef enum
{
    ADC_PPB_NUMBER1,        //!< Post-processing block 1
    ADC_PPB_NUMBER2,        //!< Post-processing block 2
    ADC_PPB_NUMBER3,        //!< Post-processing block 3
    ADC_PPB_NUMBER4         //!< Post-processing block 4
} ADC_PPBNumber;
//*****************************************************************************
//
//! Values that can be passed to ADC_setupSOC() as the \e trigger
//! parameter to specify the event that will trigger a conversion to start.
//! It is also used with ADC_setBurstModeConfig().
//
//*****************************************************************************
typedef enum
{
    ADC_TRIGGER_SW_ONLY = 0,     //!< Software only
    ADC_TRIGGER_CPU1_TINT0 = 1,     //!< CPU1 Timer 0, TINT0
    ADC_TRIGGER_CPU1_TINT1 = 2,     //!< CPU1 Timer 1, TINT1
    ADC_TRIGGER_CPU1_TINT2 = 3,     //!< CPU1 Timer 2, TINT2
    ADC_TRIGGER_GPIO = 4,     //!< GPIO, ADCEXTSOC
    ADC_TRIGGER_EPWM1_SOCA = 5,     //!< ePWM1, ADCSOCA
    ADC_TRIGGER_EPWM1_SOCB = 6,     //!< ePWM1, ADCSOCB
    ADC_TRIGGER_EPWM2_SOCA = 7,     //!< ePWM2, ADCSOCA
    ADC_TRIGGER_EPWM2_SOCB = 8,     //!< ePWM2, ADCSOCB
    ADC_TRIGGER_EPWM3_SOCA = 9,     //!< ePWM3, ADCSOCA
    ADC_TRIGGER_EPWM3_SOCB = 10,    //!< ePWM3, ADCSOCB
    ADC_TRIGGER_EPWM4_SOCA = 11,    //!< ePWM4, ADCSOCA
    ADC_TRIGGER_EPWM4_SOCB = 12,    //!< ePWM4, ADCSOCB
    ADC_TRIGGER_EPWM5_SOCA = 13,    //!< ePWM5, ADCSOCA
    ADC_TRIGGER_EPWM5_SOCB = 14,    //!< ePWM5, ADCSOCB
    ADC_TRIGGER_EPWM6_SOCA = 15,    //!< ePWM6, ADCSOCA
    ADC_TRIGGER_EPWM6_SOCB = 16,    //!< ePWM6, ADCSOCB
    ADC_TRIGGER_EPWM7_SOCA = 17,    //!< ePWM7, ADCSOCA
    ADC_TRIGGER_EPWM7_SOCB = 18,    //!< ePWM7, ADCSOCB
    ADC_TRIGGER_EPWM8_SOCA = 19,    //!< ePWM8, ADCSOCA
    ADC_TRIGGER_EPWM8_SOCB = 20,    //!< ePWM8, ADCSOCB
    ADC_TRIGGER_EPWM9_SOCA = 21,    //!< ePWM9, ADCSOCA
    ADC_TRIGGER_EPWM9_SOCB = 22,    //!< ePWM9, ADCSOCB
    ADC_TRIGGER_EPWM10_SOCA = 23,   //!< ePWM10, ADCSOCA
    ADC_TRIGGER_EPWM10_SOCB = 24,   //!< ePWM10, ADCSOCB
    ADC_TRIGGER_EPWM11_SOCA = 25,   //!< ePWM11, ADCSOCA
    ADC_TRIGGER_EPWM11_SOCB = 26,   //!< ePWM11, ADCSOCB
    ADC_TRIGGER_EPWM12_SOCA = 27,   //!< ePWM12, ADCSOCA
    ADC_TRIGGER_EPWM12_SOCB = 28,   //!< ePWM12, ADCSOCB
    ADC_TRIGGER_CPU2_TINT0 = 29,    //!< CPU2 Timer 0, TINT0
    ADC_TRIGGER_CPU2_TINT1 = 30,    //!< CPU2 Timer 1, TINT1
    ADC_TRIGGER_CPU2_TINT2 = 31     //!< CPU2 Timer 2, TINT2
} ADC_Trigger;

//*****************************************************************************
//
//! Values that can be passed in as the \e socNumber parameter for several
//! functions. This value identifies the start-of-conversion (SOC) that a
//! function is configuring or accessing. Note that in some cases (for example,
//! ADC_setInterruptSource()) \e socNumber is used to refer to the
//! corresponding end-of-conversion (EOC).
//
//*****************************************************************************
typedef enum
{
    ADC_SOC_NUMBER0,        //!< SOC/EOC number 0
    ADC_SOC_NUMBER1,        //!< SOC/EOC number 1
    ADC_SOC_NUMBER2,        //!< SOC/EOC number 2
    ADC_SOC_NUMBER3,        //!< SOC/EOC number 3
    ADC_SOC_NUMBER4,        //!< SOC/EOC number 4
    ADC_SOC_NUMBER5,        //!< SOC/EOC number 5
    ADC_SOC_NUMBER6,        //!< SOC/EOC number 6
    ADC_SOC_NUMBER7,        //!< SOC/EOC number 7
    ADC_SOC_NUMBER8,        //!< SOC/EOC number 8
    ADC_SOC_NUMBER9,        //!< SOC/EOC number 9
    ADC_SOC_NUMBER10,       //!< SOC/EOC number 10
    ADC_SOC_NUMBER11,       //!< SOC/EOC number 11
    ADC_SOC_NUMBER12,       //!< SOC/EOC number 12
    ADC_SOC_NUMBER13,       //!< SOC/EOC number 13
    ADC_SOC_NUMBER14,       //!< SOC/EOC number 14
    ADC_SOC_NUMBER15        //!< SOC/EOC number 15
} ADC_SOCNumber;

//*****************************************************************************
//
//! Values that can be passed to ADC_setInterruptPulseMode() as the
//! \e pulseMode parameter.
//
//*****************************************************************************
typedef enum
{
    //! Occurs at the end of the acquisition window
    ADC_PULSE_END_OF_ACQ_WIN = 0x00,
    //! Occurs at the end of the conversion
    ADC_PULSE_END_OF_CONV = 0x04
} ADC_PulseMode;

//*****************************************************************************
//
//! Values that can be passed to ADC_setMode() as the \e signalMode
//! parameter.
//
//*****************************************************************************
typedef enum
{
    ADC_MODE_SINGLE_ENDED = 0x00,   //!< Sample on single pin with VREFLO
    ADC_MODE_DIFFERENTIAL = 0x80    //!< Sample on pair of pins
} ADC_SignalMode;

//*****************************************************************************
//
//! Values that can be passed to ADC_setPrescaler() as the \e clkPrescale
//! parameter.
//
//*****************************************************************************
typedef enum
{
    ADC_CLK_DIV_1_0 = 0,            //!< ADCCLK = (input clock) / 1.0
    ADC_CLK_DIV_2_0 = 2,            //!< ADCCLK = (input clock) / 2.0
    ADC_CLK_DIV_2_5 = 3,            //!< ADCCLK = (input clock) / 2.5
    ADC_CLK_DIV_3_0 = 4,            //!< ADCCLK = (input clock) / 3.0
    ADC_CLK_DIV_3_5 = 5,            //!< ADCCLK = (input clock) / 3.5
    ADC_CLK_DIV_4_0 = 6,            //!< ADCCLK = (input clock) / 4.0
    ADC_CLK_DIV_4_5 = 7,            //!< ADCCLK = (input clock) / 4.5
    ADC_CLK_DIV_5_0 = 8,            //!< ADCCLK = (input clock) / 5.0
    ADC_CLK_DIV_5_5 = 9,            //!< ADCCLK = (input clock) / 5.5
    ADC_CLK_DIV_6_0 = 10,           //!< ADCCLK = (input clock) / 6.0
    ADC_CLK_DIV_6_5 = 11,           //!< ADCCLK = (input clock) / 6.5
    ADC_CLK_DIV_7_0 = 12,           //!< ADCCLK = (input clock) / 7.0
    ADC_CLK_DIV_7_5 = 13,           //!< ADCCLK = (input clock) / 7.5
    ADC_CLK_DIV_8_0 = 14,           //!< ADCCLK = (input clock) / 8.0
    ADC_CLK_DIV_8_5 = 15            //!< ADCCLK = (input clock) / 8.5
} ADC_ClkPrescale;

//*****************************************************************************
//
//! Values that can be passed to ADC_setupSOC() as the \e channel
//! parameter. This is the input pin on which the signal to be converted is
//! located.
//
//*****************************************************************************
typedef enum
{
    ADC_CH_ADCIN0 = 0,           //!< single-ended, ADCIN0
    ADC_CH_ADCIN1 = 1,           //!< single-ended, ADCIN1
    ADC_CH_ADCIN2 = 2,           //!< single-ended, ADCIN2
    ADC_CH_ADCIN3 = 3,           //!< single-ended, ADCIN3
    ADC_CH_ADCIN4 = 4,           //!< single-ended, ADCIN4
    ADC_CH_ADCIN5 = 5,           //!< single-ended, ADCIN5
    ADC_CH_ADCIN6 = 6,           //!< single-ended, ADCIN6
    ADC_CH_ADCIN7 = 7,           //!< single-ended, ADCIN7
    ADC_CH_ADCIN8 = 8,           //!< single-ended, ADCIN8
    ADC_CH_ADCIN9 = 9,           //!< single-ended, ADCIN9
    ADC_CH_ADCIN10 = 10,         //!< single-ended, ADCIN10
    ADC_CH_ADCIN11 = 11,         //!< single-ended, ADCIN11
    ADC_CH_ADCIN12 = 12,         //!< single-ended, ADCIN12
    ADC_CH_ADCIN13 = 13,         //!< single-ended, ADCIN13
    ADC_CH_ADCIN14 = 14,         //!< single-ended, ADCIN14
    ADC_CH_ADCIN15 = 15,         //!< single-ended, ADCIN15
    ADC_CH_ADCIN0_ADCIN1 = 0,    //!< differential, ADCIN0 and ADCIN1
    ADC_CH_ADCIN2_ADCIN3 = 2,    //!< differential, ADCIN2 and ADCIN3
    ADC_CH_ADCIN4_ADCIN5 = 4,    //!< differential, ADCIN4 and ADCIN5
    ADC_CH_ADCIN6_ADCIN7 = 6,    //!< differential, ADCIN6 and ADCIN7
    ADC_CH_ADCIN8_ADCIN9 = 8,    //!< differential, ADCIN8 and ADCIN9
    ADC_CH_ADCIN10_ADCIN11 = 10, //!< differential, ADCIN10 and ADCIN11
    ADC_CH_ADCIN12_ADCIN13 = 12, //!< differential, ADCIN12 and ADCIN13
    ADC_CH_ADCIN14_ADCIN15 = 14  //!< differential, ADCIN14 and ADCIN15
} ADC_Channel;

//*****************************************************************************
//
//! Configures a start-of-conversion (SOC) in the ADC.
//!
//! \param base is the base address of the ADC module.
//! \param socNumber is the number of the start-of-conversion.
//! \param trigger the source that will cause the SOC.
//! \param channel is the number associated with the input signal.
//! \param sampleWindow is the acquisition window duration.
//!
//! This function configures the a start-of-conversion (SOC) in the ADC module.
//!
//! The \e socNumber number is a value \b ADC_SOC_NUMBERX where X is a number
//! from 0 to 15 specifying which SOC is to be configured on the ADC module
//! specified by \e base.
//!
//! The \e trigger specifies the event that causes the SOC such as software, a
//! timer interrupt, an ePWM event, or an ADC interrupt. It should be a value
//! in the format of \b ADC_TRIGGER_XXXX where XXXX is the event such as
//! \b ADC_TRIGGER_SW_ONLY, \b ADC_TRIGGER_CPU1_TINT0, \b ADC_TRIGGER_GPIO,
//! \b ADC_TRIGGER_EPWM1_SOCA, and so on.
//!
//! The \e channel parameter specifies the channel to be converted. In
//! single-ended mode this is a single pin given by \b ADC_CH_ADCINx where x is
//! the number identifying the pin between 0 and 15 inclusive. In differential
//! mode, two pins are used as inputs and are passed in the \e channel
//! parameter as \b ADC_CH_ADCIN0_ADCIN1, \b ADC_CH_ADCIN2_ADCIN3, ..., or
//! \b ADC_CH_ADCIN14_ADCIN15.
//!
//! The \e sampleWindow parameter is the acquisition window duration in SYSCLK
//! cycles. It should be a value between 1 and 512 cycles inclusive. The
//! selected duration must be at least as long as one ADCCLK cycle. Also, the
//! datasheet will specify a minimum window duration requirement in
//! nanoseconds.
//!
//! \return None.
//
//*****************************************************************************

static inline void ADC_setupSOC(uint32_t base, ADC_SOCNumber socNumber,
                                ADC_Trigger trigger, ADC_Channel channel,
                                uint32_t sampleWindow)
{
    uint32_t ctlRegAddr;

    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base)); ASSERT((sampleWindow >= 1U) && (sampleWindow <= 512U));

    //
    // Calculate address for the SOC control register.
    //
    ctlRegAddr = base + ADC_SOCxCTL_OFFSET_BASE + ((uint32_t) socNumber * 2U);

    //
    // Set the configuration of the specified SOC.
    //
    EALLOW;
    HWREG(ctlRegAddr) = ((uint32_t) channel << ADC_SOC0CTL_CHSEL_S)
            | ((uint32_t) trigger << ADC_SOC0CTL_TRIGSEL_S)
            | (sampleWindow - 1U);
    EDIS;
}

//*****************************************************************************
//
//! Sets the source EOC for an analog-to-digital converter interrupt.
//!
//! \param base is the base address of the ADC module.
//! \param adcIntNum is interrupt number within the ADC wrapper.
//! \param socNumber is the number of the start-of-conversion.
//!
//! This function sets which conversion is the source of an ADC interrupt.
//!
//! The \e socNumber number is a value \b ADC_SOC_NUMBERX where X is a number
//! from 0 to 15 specifying which EOC is to be configured on the ADC module
//! specified by \e base.
//!
//! \e adcIntNum can take the value \b ADC_INT_NUMBER1, \b ADC_INT_NUMBER2,
//! \b ADC_INT_NUMBER3, \b or ADC_INT_NUMBER4 to express which of the four
//! interrupts of the ADC module is being configured.
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_setInterruptSource(uint32_t base,
                                          ADC_IntNumber adcIntNum,
                                          ADC_SOCNumber socNumber)
{
    uint32_t intRegAddr;
    uint16_t shiftVal;

    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Each INTSEL register manages two interrupts. If the interrupt number is
    // even, we'll be accessing the upper byte and will need to shift.
    //
    intRegAddr = base + ADC_INTSELxNy_OFFSET_BASE + ((uint32_t) adcIntNum >> 1);
    shiftVal = ((uint16_t) adcIntNum & 0x1U) << 3U;

    //
    // Set the specified ADC interrupt source.
    //
    EALLOW;

    HWREGH(intRegAddr) = (HWREGH(intRegAddr)
            & ~(ADC_INTSEL1N2_INT1SEL_M << shiftVal))
            | ((uint16_t) socNumber << shiftVal);

    EDIS;
}

//*****************************************************************************
//
//! Sets the timing of the end-of-conversion pulse
//!
//! \param base is the base address of the ADC module.
//! \param pulseMode is the generation mode of the EOC pulse.
//!
//! This function configures the end-of-conversion (EOC) pulse generated by the
//! ADC. This pulse will be generated either at the end of the acquisition
//! window (pass \b ADC_PULSE_END_OF_ACQ_WIN into \e pulseMode) or at the end
//! of the voltage conversion, one cycle prior to the ADC result latching into
//! its result register (pass \b ADC_PULSE_END_OF_CONV into \e pulseMode).
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_setInterruptPulseMode(uint32_t base,
                                             ADC_PulseMode pulseMode)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Set the position of the pulse.
    //
    EALLOW;
    HWREGH(base + ADC_O_CTL1) = (HWREGH(base + ADC_O_CTL1)
            & ~ADC_CTL1_INTPULSEPOS) | (uint16_t) pulseMode;
    EDIS;
}

//*****************************************************************************
//
//! Enables an ADC interrupt source.
//!
//! \param base is the base address of the ADC module.
//! \param adcIntNum is interrupt number within the ADC wrapper.
//!
//! This function enables the indicated ADC interrupt source.  Only the
//! sources that are enabled can be reflected to the processor interrupt.
//! Disabled sources have no effect on the processor.
//!
//! \e adcIntNum can take the value \b ADC_INT_NUMBER1, \b ADC_INT_NUMBER2,
//! \b ADC_INT_NUMBER3, \b or ADC_INT_NUMBER4 to express which of the four
//! interrupts of the ADC module should be enabled.
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_enableInterrupt(uint32_t base, ADC_IntNumber adcIntNum)
{
    uint32_t intRegAddr;
    uint16_t shiftVal;

    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Each INTSEL register manages two interrupts. If the interrupt number is
    // even, we'll be accessing the upper byte and will need to shift.
    //
    intRegAddr = base + ADC_INTSELxNy_OFFSET_BASE + ((uint32_t) adcIntNum >> 1);
    shiftVal = ((uint16_t) adcIntNum & 0x1U) << 3U;

    //
    // Enable the specified ADC interrupt.
    //
    EALLOW;

    HWREGH(intRegAddr) |= ADC_INTSEL1N2_INT1E << shiftVal;

    EDIS;
}

//*****************************************************************************
//
//! Powers up the analog-to-digital converter core.
//!
//! \param base is the base address of the ADC module.
//!
//! This function powers up the analog circuitry inside the analog core.
//!
//! \note Allow at least a 500us delay before sampling after calling this API.
//! If you enable multiple ADCs, you can delay after they all have begun
//! powering up.
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_enableConverter(uint32_t base)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Set the bit that powers up the analog circuitry.
    //
    EALLOW;
    HWREGH(base + ADC_O_CTL1) |= ADC_CTL1_ADCPWDNZ;
    EDIS;
}

//*****************************************************************************
//
//! Clears ADC interrupt sources.
//!
//! \param base is the base address of the ADC module.
//! \param adcIntNum is interrupt number within the ADC wrapper.
//!
//! This function clears the specified ADC interrupt sources so that they no
//! longer assert. If not in continuous mode, this function must be called
//! before any further interrupt pulses may occur.
//!
//! \e adcIntNum takes a one of the values \b ADC_INT_NUMBER1,
//! \b ADC_INT_NUMBER2, \b ADC_INT_NUMBER3, \b or ADC_INT_NUMBER4 to express
//! which of the four interrupts of the ADC module should be cleared
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_clearInterruptStatus(uint32_t base,
                                            ADC_IntNumber adcIntNum)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Clear the specified interrupt.
    //
    HWREGH(base + ADC_O_INTFLGCLR) |= 1U << (uint16_t) adcIntNum;
}

//*****************************************************************************
//
//! Configures the analog-to-digital converter module prescaler.
//!
//! \param base is the base address of the ADC module.
//! \param clkPrescale is the ADC clock prescaler.
//!
//! This function configures the ADC module's ADCCLK.
//!
//! The \e clkPrescale parameter specifies the value by which the input clock
//! is divided to make the ADCCLK.  The value can be specified with the value
//! \b ADC_CLK_DIV_1_0, \b ADC_CLK_DIV_2_0, \b ADC_CLK_DIV_2_5, ...,
//! \b ADC_CLK_DIV_7_5, \b ADC_CLK_DIV_8_0, or \b ADC_CLK_DIV_8_5.
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_setPrescaler(uint32_t base, ADC_ClkPrescale clkPrescale)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Set the configuration of the ADC module prescaler.
    //
    EALLOW;
    HWREGH(base + ADC_O_CTL2) = (HWREGH(base + ADC_O_CTL2)
            & ~ADC_CTL2_PRESCALE_M) | (uint16_t) clkPrescale;
    EDIS;
}

//*****************************************************************************
//
//! Reads the conversion result.
//!
//! \param resultBase is the base address of the ADC results.
//! \param socNumber is the number of the start-of-conversion.
//!
//! This function returns the conversion result that corresponds to the base
//! address passed into \e resultBase and the SOC passed into \e socNumber.
//!
//! The \e socNumber number is a value \b ADC_SOC_NUMBERX where X is a number
//! from 0 to 15 specifying which SOC's result is to be read.
//!
//! \note Take care that you are using a base address for the result registers
//! (ADCxRESULT_BASE) and not a base address for the control registers.
//!
//! \return Returns the conversion result.
//
//*****************************************************************************
static inline uint16_t ADC_readResult(uint32_t resultBase,
                                      ADC_SOCNumber socNumber)
{
    uint16_t* resAddr;

    //
    // Check the arguments.
    //
    ASSERT((resultBase == ADCARESULT_BASE) ||
            (resultBase == ADCBRESULT_BASE) ||
            (resultBase == ADCCRESULT_BASE) ||
            (resultBase == ADCDRESULT_BASE));

    //
    // Get the address to the appropriate result.
    //
    resAddr = (uint16_t*) (resultBase + ADC_RESULTx_OFFSET_BASE);

    //
    // Return the result found at that address and offset.
    //
    return (resAddr[socNumber]);
}

//*****************************************************************************
//
//! Gets the current ADC interrupt status.
//!
//! \param base is the base address of the ADC module.
//! \param adcIntNum is interrupt number within the ADC wrapper.
//!
//! This function returns the interrupt status for the analog-to-digital
//! converter.
//!
//! \return \b true if the interrupt flag for the specified interrupt number is
//! set and \b false if it is not.
//
//*****************************************************************************
static inline bool ADC_getInterruptStatus(uint32_t base,
                                          ADC_IntNumber adcIntNum)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Get the specified ADC interrupt status.
    //
    return ((HWREGH(base + ADC_O_INTFLG) & (1U << (uint16_t) adcIntNum)) != 0U);
}

//*****************************************************************************
//
//! Forces a SOC flag to a 1 in the analog-to-digital converter.
//!
//! \param base is the base address of the ADC module.
//! \param socNumber is the number of the start-of-conversion.
//!
//! This function forces the SOC flag associated with the SOC specified by
//! \e socNumber. This initiates a conversion once that SOC is given
//! priority. This software trigger can be used whether or not the SOC has been
//! configured to accept some other specific trigger.
//!
//! \return None.
//
//*****************************************************************************
static inline void ADC_forceSOC(uint32_t base, ADC_SOCNumber socNumber)
{
    //
    // Check the arguments.
    //
    ASSERT(ADC_isBaseValid(base));

    //
    // Write to the register that will force a 1 to the corresponding SOC flag
    //
    HWREGH(base + ADC_O_SOCFRC1) |= (1U << (uint16_t) socNumber);
}

//*****************************************************************************
//
//! Configures the analog-to-digital converter resolution and signal mode.
//!
//! \param base is the base address of the ADC module.
//! \param resolution is the resolution of the converter (12 or 16 bits).
//! \param signalMode is the input signal mode of the converter.
//!
//! This function configures the ADC module's conversion resolution and input
//! signal mode and ensures that the corresponding trims are loaded.
//!
//! The \e resolution parameter specifies the resolution of the conversion.
//! It can be 12-bit or 16-bit specified by \b ADC_RESOLUTION_12BIT
//! or \b ADC_RESOLUTION_16BIT.
//!
//! The \e signalMode parameter specifies the signal mode. In single-ended
//! mode, which is indicated by \b ADC_MODE_SINGLE_ENDED, the input voltage is
//! sampled on a single pin referenced to VREFLO. In differential mode, which
//! is indicated by \b ADC_MODE_DIFFERENTIAL, the input voltage to the
//! converter is sampled on a pair of input pins, a positive and a negative.
//!
//! \return None.
//
//*****************************************************************************
extern void ADC_setMode(uint32_t base, ADC_Resolution resolution, ADC_SignalMode signalMode);

void DSP_ADC_Init(void);
Uint16 DSP_ADC_Read();

#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_PHOENIXONCONTROLS_ADC_H_ */



