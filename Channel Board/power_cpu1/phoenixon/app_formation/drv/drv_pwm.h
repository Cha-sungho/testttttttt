/*
 * drv_pwm.h
 *
 *  Created on: Nov 18, 2021
 *      Author: JOO
 */

#ifndef PHOENIXON_DRV_DRV_PWM_H_
#define PHOENIXON_DRV_DRV_PWM_H_

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include "sfo_v8.h"

#ifdef CPU1
#ifdef _FLASH
#pragma CODE_SECTION(drv_SetDCDCPWMDuty, ".TI.ramfunc");
#endif
#endif

#define DCDC_T_PWM          EPWM1_BASE
#define DCDC_B_PWM          EPWM2_BASE

#define DCDC_A_T_PWM_PIN    0     //EPWM 1
#define DCDC_A_B_PWM_PIN    1
#define DCDC_B_T_PWM_PIN    2     //EPWM 2
#define DCDC_B_B_PWM_PIN    3

#define SYS_EPWM_CLK_DIV    SYSCTL_EPWMCLK_DIV_1
#define EPWM_CLK_DIV        EPWM_CLOCK_DIVIDER_1
#define EPWM_HS_CLK_DIV     EPWM_HSCLOCK_DIVIDER_1

#define HRPWM_SCALE         256

typedef enum
{
    swap_true,
    swap_false
} PwmSwapPol;

typedef enum
{
    out_active_high,
    out_active_high_complementary,
    out_active_low,
    out_active_low_complementary
} PwmOutType;

typedef enum
{
    block_acdc,
    block_llc,
    block_dcdc,
    block_all
} PwmPowerBlock;

extern float32_t dcdc_pwm_prd;

extern uint16_t sfo_status;
extern uint16_t sfo_error_cnt;
extern int MEP_ScaleFactor;

extern uint32_t view_dcdc_t_hr_duty_reg;
extern uint32_t view_dcdc_b_hr_duty_reg;

extern uint16_t view_dcdc_t_duty_reg;
extern uint16_t view_dcdc_b_duty_reg;

// For configuration full version
#pragma FUNC_ALWAYS_INLINE(drv_SetDCDCPWMDuty)
static inline void drv_SetDCDCPWMDuty(float32_t duty_a, float32_t duty_b, uint16_t hr_use)
{
    uint16_t cmp_val_a = 0, cmp_val_b = 0;
    uint32_t hr_cmp_val_a = 0, hr_cmp_val_b = 0;

    if (hr_use) {
#if 0
        view_dcdc_t_hr_duty_reg = hr_cmp_val_a = (uint32_t) (duty_a * (dcdc_pwm_prd + 1U) * HRPWM_SCALE);
        view_dcdc_b_hr_duty_reg = hr_cmp_val_b = (uint32_t) (duty_b * (dcdc_pwm_prd + 1U) * HRPWM_SCALE);
#else
        view_dcdc_t_hr_duty_reg = hr_cmp_val_a = (uint32_t) ((duty_a * (float32_t) ((uint32_t) dcdc_pwm_prd << 8)));
        view_dcdc_b_hr_duty_reg = hr_cmp_val_b = (uint32_t) ((duty_b * (float32_t) ((uint32_t) dcdc_pwm_prd << 8)));
#endif
        HRPWM_setCounterCompareValue(DCDC_T_PWM, HRPWM_COUNTER_COMPARE_A, hr_cmp_val_a);
        HRPWM_setCounterCompareValue(DCDC_B_PWM, HRPWM_COUNTER_COMPARE_A, hr_cmp_val_b);

        sfo_status = SFO();  // in background, MEP calibration module continuously updates MEP_ScaleFactor
        if (sfo_status == SFO_ERROR) {
            sfo_error_cnt++;
        }
    } else {
        view_dcdc_t_duty_reg = cmp_val_a = (uint16_t) (duty_a * (dcdc_pwm_prd + 1U));
        view_dcdc_b_duty_reg = cmp_val_b = (uint16_t) (duty_b * (dcdc_pwm_prd + 1U));

        EPWM_setCounterCompareValue(DCDC_T_PWM, EPWM_COUNTER_COMPARE_A, cmp_val_a);
        EPWM_setCounterCompareValue(DCDC_B_PWM, EPWM_COUNTER_COMPARE_A, cmp_val_b);
    }
}

#pragma FUNC_ALWAYS_INLINE(drv_SetBB1PWMDuty)
static inline void drv_SetBB1PWMDuty(float32_t duty, uint16_t hr_use)
{
    uint16_t cmp_val = 0;
    uint32_t hr_cmp_val = 0;

    if (hr_use) {
#if 0
        view_dcdc_t_hr_duty_reg = hr_cmp_val = (uint32_t) (duty_a * (dcdc_pwm_prd + 1U) * HRPWM_SCALE);
#else
        view_dcdc_t_hr_duty_reg = hr_cmp_val = (uint32_t) ((duty * (float32_t) ((uint32_t) dcdc_pwm_prd << 8)));
#endif
        HRPWM_setCounterCompareValue(DCDC_T_PWM, HRPWM_COUNTER_COMPARE_A, hr_cmp_val);

        sfo_status = SFO();  // in background, MEP calibration module continuously updates MEP_ScaleFactor
        if (sfo_status == SFO_ERROR) {
            sfo_error_cnt++;
        }
    } else {
        view_dcdc_t_duty_reg = cmp_val = (uint16_t) (duty * (dcdc_pwm_prd + 1U));

        EPWM_setCounterCompareValue(DCDC_T_PWM, EPWM_COUNTER_COMPARE_A, cmp_val);
    }
}

#pragma FUNC_ALWAYS_INLINE(drv_SetBB2PWMDuty)
static inline void drv_SetBB2PWMDuty(float32_t duty, uint16_t hr_use)
{
    uint16_t cmp_val = 0;
    uint32_t hr_cmp_val = 0;

    if (hr_use) {
#if 0
        view_dcdc_b_hr_duty_reg = hr_cmp_val = (uint32_t) (duty * (dcdc_pwm_prd + 1U) * HRPWM_SCALE);
#else
        view_dcdc_b_hr_duty_reg = hr_cmp_val = (uint32_t) ((duty * (float32_t) ((uint32_t) dcdc_pwm_prd << 8)));
#endif
        HRPWM_setCounterCompareValue(DCDC_B_PWM, HRPWM_COUNTER_COMPARE_A, hr_cmp_val);

        sfo_status = SFO();  // in background, MEP calibration module continuously updates MEP_ScaleFactor
        if (sfo_status == SFO_ERROR) {
            sfo_error_cnt++;
        }
    } else {
        view_dcdc_b_duty_reg = cmp_val = (uint16_t) (duty * (dcdc_pwm_prd + 1U));

        EPWM_setCounterCompareValue(DCDC_B_PWM, EPWM_COUNTER_COMPARE_A, cmp_val);
    }
}

void drv_InitPWM(void);
void drv_DisablePWM(void);
void drv_EnablePWM(void);
void drv_value_reset(void);
void value_init(void);

#endif /* PHOENIXON_DRV_DRV_PWM_H_ */
