/*
 * drv_pwm.c
 *
 *  Created on: Nov 4, 2021
 *      Author: JOO
 */

#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"

#include <stdbool.h>

#include "drv_gpio.h"
#include "drv_pwm.h"

#include "app_ctrl_power.h"
#include "bsp_io.h"

#include "app_op.h"
#include "app_calibration.h"

float32_t dcdc_op_pwm_freq = 42500.0f;  //Hz
float32_t dcdc_in_pwm_freq;
float32_t dcdc_pwm_prd;
uint16_t dcdc_pwm_hr_use;

float dcdc_pwm_hr_percent;

//For TI SOF
uint16_t sfo_status;
uint16_t sfo_error_cnt;
int MEP_ScaleFactor;
volatile uint32_t ePWM[] = {0, };

//For monitoring
uint32_t view_dcdc_t_hr_duty_reg;
uint32_t view_dcdc_b_hr_duty_reg;

uint16_t view_dcdc_t_duty_reg;
uint16_t view_dcdc_b_duty_reg;

extern uint16_t seq_mode;
extern uint16_t g_debug_start_flag;
extern uint16_t cv_mode;
extern c_d_mode charge_discharge_mode;
extern AppCommCanOpHandle operation;

// Configuration full version
static void drv_Config1PWM(uint32_t base,
                           uint32_t master_base,
                           float32_t freq_In_hz,
                           EPWM_TimeBaseCountMode ctr_mode,
                           PwmOutType out_type,
                           PwmSwapPol swap_pol,
                           uint16_t red, uint16_t fed,
                           uint16_t phase,
                           EPWM_SyncOutPulseMode sync_in,
                           uint16_t use_hrpwm);
// No dead time
static void drv_Config2PWM(uint32_t base,
                           uint32_t master_base,
                           float32_t freq_In_hz,
                           EPWM_TimeBaseCountMode ctr_mode,
                           uint16_t phase,
                           EPWM_SyncOutPulseMode sync_in);

// EPWM_configureSignal() function modified version
static void drv_ConfigPWMSignal(uint32_t base, const EPWM_SignalParams *signalParams);
static void drv_ConfigPWMTripZone(uint32_t base, uint16_t tz_signal);

static void drv_InitDCDCPWM(void);

// Configuration full version
static void drv_Config1PWM(uint32_t base,
                           uint32_t master_base,
                           float32_t freq_In_hz,
                           EPWM_TimeBaseCountMode ctr_mode,
                           PwmOutType out_type,
                           PwmSwapPol swap_pol,
                           uint16_t red, uint16_t fed,
                           uint16_t phase,
                           EPWM_SyncOutPulseMode sync_in,
                           uint16_t use_hrpwm)
{
    EPWM_SignalParams pwm_signal;
    uint32_t read_prd_val, phase_reg_val;

    EPWM_setEmulationMode(base, EPWM_EMULATION_FREE_RUN);

    pwm_signal.freqInHz = freq_In_hz;
    pwm_signal.dutyValA = 0.0f;
    pwm_signal.dutyValB = 0.0f;
    pwm_signal.invertSignalB = true;
    pwm_signal.sysClkInHz = DEVICE_SYSCLK_FREQ;
    pwm_signal.epwmClkDiv = SYS_EPWM_CLK_DIV;
    pwm_signal.tbCtrMode = ctr_mode;
    pwm_signal.tbClkDiv = EPWM_CLK_DIV;
    pwm_signal.tbHSClkDiv = EPWM_HS_CLK_DIV;

    drv_ConfigPWMSignal(base, &pwm_signal);

    // Use the delayed signals instead of the original signals
    EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, true);
    EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);

    EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

    // Configure deadband delay polarity
    switch (out_type)
    {
        case out_active_high:
            // Do not invert the delayed outputs (AH)
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_HIGH);
            break;
        case out_active_high_complementary:
            // Invert only the Falling Edge delayed output (AHC)
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
            break;
        case out_active_low:
            // INVERT the delayed outputs (AL)
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_LOW);
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
            break;
        case out_active_low_complementary:
            // Invert only the Rising Edge delayed output (ALC)
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_LOW);
            EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_HIGH);
            break;
    }

    if (swap_pol == swap_true) {
        // Switch Output A with Output B
        EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, true);
        EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, true);
    } else {
        // DO NOT Switch Output A with Output B
        EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_A, false);
        EPWM_setDeadBandOutputSwapMode(base, EPWM_DB_OUTPUT_B, false);
    }

    // Use EPWMA as the input for both RED and FED
    EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);
    EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);

    // Set the RED and FED values
    EPWM_setRisingEdgeDelayCount(base, red);        //if 200MHz : 1 = 5ns
    EPWM_setFallingEdgeDelayCount(base, fed);

    if (master_base == 0xFFFF) {
        EPWM_disablePhaseShiftLoad(base);
        EPWM_setPhaseShift(base, 0U);
        // base SYNCO is generated on CTR=0
        // XXX
        EPWM_setPeriodLoadMode(base, EPWM_PERIOD_SHADOW_LOAD);
        EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);

    } else {
        // Calculate phase register values based on Time Base counter mode
        read_prd_val = EPWM_getTimeBasePeriod(master_base);
        if ((HWREGH(base + EPWM_O_TBCTL) & 0x3U) == EPWM_COUNTER_MODE_UP_DOWN) {
            phase_reg_val = (2U * read_prd_val * phase) / 360U;
        } else if ((HWREGH(base + EPWM_O_TBCTL) & 0x3U) < EPWM_COUNTER_MODE_UP_DOWN) {
            phase_reg_val = (read_prd_val * phase) / 360U;
            phase_reg_val = read_prd_val - phase_reg_val;
        }
        // Initialize base PWM with phase shift of phase_reg_val
        EPWM_selectPeriodLoadEvent(base, EPWM_SHADOW_LOAD_MODE_SYNC);
        EPWM_setPhaseShift(base, phase_reg_val);
        EPWM_setCountModeAfterSync(base, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);

        EPWM_setTimeBaseCounter(base, phase_reg_val);

        // base PWM uses the sync_in.
        // XXX
        EPWM_setPeriodLoadMode(base, EPWM_PERIOD_SHADOW_LOAD);
        EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_EPWMxSYNCIN);

        // Enable all phase shifts.
        EPWM_enablePhaseShiftLoad(base);
    }

    if (use_hrpwm) {
        // Align HRPWM CMPA value and shadow mode with EPWM
        HRPWM_setCounterCompareShadowLoadEvent(base, HRPWM_CHANNEL_A, HRPWM_LOAD_ON_CNTR_ZERO);
        HRPWM_setCounterCompareShadowLoadEvent(base, HRPWM_CHANNEL_B, HRPWM_LOAD_ON_CNTR_ZERO);

        // Align the HRPWM CMPA behavior with EPWM
        HRPWM_setMEPEdgeSelect(base, HRPWM_CHANNEL_A, HRPWM_MEP_CTRL_FALLING_EDGE);
        HRPWM_setMEPEdgeSelect(base, HRPWM_CHANNEL_B, HRPWM_MEP_CTRL_FALLING_EDGE);
        HRPWM_setMEPControlMode(base, HRPWM_CHANNEL_A, HRPWM_MEP_DUTY_PERIOD_CTRL);
        HRPWM_setMEPControlMode(base, HRPWM_CHANNEL_B, HRPWM_MEP_DUTY_PERIOD_CTRL);

        HRPWM_enableAutoConversion(base);
    }
}

// No dead time
static void drv_Config2PWM(uint32_t base,
                           uint32_t master_base,
                           float32_t freq_In_hz,
                           EPWM_TimeBaseCountMode ctr_mode,
                           uint16_t phase,
                           EPWM_SyncOutPulseMode sync_in)
{
    EPWM_SignalParams pwm_signal;
    uint32_t read_prd_val, phase_reg_val;

    EPWM_setEmulationMode(base, EPWM_EMULATION_FREE_RUN);

    pwm_signal.freqInHz = freq_In_hz;
    pwm_signal.dutyValA = 0.5f;
    pwm_signal.dutyValB = 0.5f;
    pwm_signal.sysClkInHz = DEVICE_SYSCLK_FREQ;
    pwm_signal.epwmClkDiv = SYS_EPWM_CLK_DIV;
    pwm_signal.tbCtrMode = ctr_mode;
    pwm_signal.tbClkDiv = EPWM_CLK_DIV;
    pwm_signal.tbHSClkDiv = EPWM_HS_CLK_DIV;

    // caution, Sets up the Action Qualifier output on ePWM A or ePWMB is included in this function.
    // Set-up TBCLK
    // Set ePWM clock pre-scaler
    // Set up shadowing
    // Set-up compare
    // Set actions
    drv_ConfigPWMSignal(base, &pwm_signal);

    if (master_base == 0xFFFF) {
        EPWM_disablePhaseShiftLoad(base);
        EPWM_setPhaseShift(base, 0U);
        // base SYNCO is generated on CTR=0
        // XXX
        EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);

    } else {
        // Calculate phase register values based on Time Base counter mode
        read_prd_val = EPWM_getTimeBasePeriod(master_base);
        if ((HWREGH(base + EPWM_O_TBCTL) & 0x3U) == EPWM_COUNTER_MODE_UP_DOWN) {
            phase_reg_val = (2U * read_prd_val * phase) / 360U;
        } else if ((HWREGH(base + EPWM_O_TBCTL) & 0x3U) < EPWM_COUNTER_MODE_UP_DOWN) {
            phase_reg_val = (read_prd_val * phase) / 360U;
            phase_reg_val = read_prd_val - phase_reg_val;
        }
        // Initialize base PWM with phase shift of phase_reg_val
        EPWM_selectPeriodLoadEvent(base, EPWM_SHADOW_LOAD_MODE_SYNC);
        EPWM_setPhaseShift(base, phase_reg_val);
        EPWM_setTimeBaseCounter(base, phase_reg_val);
        // base PWM uses the sync_in.
        // XXX
        EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_EPWMxSYNCIN);
        // Enable all phase shifts.
        EPWM_enablePhaseShiftLoad(base);
    }
}

static void drv_ConfigPWMTripZone(uint32_t base, uint16_t tz_signal)
{
    EPWM_enableTripZoneSignals(base, tz_signal);  // Emulator Stop
    EPWM_disableTripZoneAdvAction(base);
    EPWM_setTripZoneAction(base, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);
}

static void drv_InitDCDCPWM(void)
{
    EPWM_TimeBaseCountMode ctr_mode;
    uint16_t hr_use;

    //GPIO
    drv_ConfigGPIO(GPIO_CORE_CPU1, DCDC_A_T_PWM_PIN, GPIO_DIR_MODE_OUT, GPIO_0_EPWM1A, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);
    drv_ConfigGPIO(GPIO_CORE_CPU1, DCDC_A_B_PWM_PIN, GPIO_DIR_MODE_OUT, GPIO_1_EPWM1B, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);
    drv_ConfigGPIO(GPIO_CORE_CPU1, DCDC_B_T_PWM_PIN, GPIO_DIR_MODE_OUT, GPIO_2_EPWM2A, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);
    drv_ConfigGPIO(GPIO_CORE_CPU1, DCDC_B_B_PWM_PIN, GPIO_DIR_MODE_OUT, GPIO_3_EPWM2B, GPIO_PIN_TYPE_STD, GPIO_QUAL_SYNC, 1);

    //PWM
    drv_Config1PWM(
                  DCDC_T_PWM,                               // PWM Base
                  0xFFFF,                                   // Master Base
                  dcdc_op_pwm_freq,                         // PWM Frequency
                  ctr_mode = EPWM_COUNTER_MODE_UP_DOWN,     // Count Mode
                  out_active_high_complementary,            // Out Mode
                  swap_false,                               // Out Swap
                  24, 24,                                   // Dead-band
                  0,                                        // Phase Shift
                  0,                                        // Sync Source
                  hr_use = 1                                // HRPWM Use
                  );    //DCDC A
    drv_Config1PWM(
                  DCDC_B_PWM,                               // PWM Base
                  DCDC_T_PWM,                               // Master Base
                  dcdc_op_pwm_freq,                         // PWM Frequency
                  ctr_mode = EPWM_COUNTER_MODE_UP_DOWN,     // Count Mode
                  out_active_high_complementary,            // Out Mode
                  swap_false,                                // Out Swap
                  24, 24,                                   // Dead-band
                  180,                                      // Phase Shift
                  0,                                        // Sync Source
                  hr_use = 1                                // HRPWM Use
                  );    //DCDC B

    dcdc_in_pwm_freq = ((float32_t) DEVICE_SYSCLK_FREQ / (1U << ((uint16_t) SYS_EPWM_CLK_DIV + (uint16_t) EPWM_CLK_DIV)));
    dcdc_in_pwm_freq /= (1U << (uint16_t) EPWM_HS_CLK_DIV);

    if (ctr_mode == EPWM_COUNTER_MODE_UP_DOWN) {
        dcdc_pwm_prd = (uint16_t) (dcdc_in_pwm_freq / (2.0f * (float32_t) dcdc_op_pwm_freq));  //Up-Down
    } else {
        dcdc_pwm_prd = (uint16_t) ((dcdc_in_pwm_freq / (float32_t) dcdc_op_pwm_freq) - 1.0f);   //Up or Down
    }

    if (hr_use == 1) {
        dcdc_pwm_hr_percent = 4.0 / ((float32_t) dcdc_pwm_prd);

        while (sfo_status == SFO_INCOMPLETE) {
            sfo_status = SFO();
            if (sfo_status == SFO_ERROR) {
                sfo_error_cnt++;
            }
        }
    }

    drv_DisablePWM();
}

void drv_InitPWM(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    drv_InitDCDCPWM();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void drv_DisablePWM(void)
{
    EPWM_forceTripZoneEvent(DCDC_T_PWM, EPWM_TZ_FORCE_EVENT_OST);
    EPWM_forceTripZoneEvent(DCDC_B_PWM, EPWM_TZ_FORCE_EVENT_OST);
}

void drv_EnablePWM(void)
{
    EPWM_clearTripZoneFlag(DCDC_T_PWM, EPWM_TZ_FLAG_OST);
    EPWM_clearTripZoneFlag(DCDC_B_PWM, EPWM_TZ_FLAG_OST);
}

// TI EPWM_configureSignal() function modified version
static void drv_ConfigPWMSignal(uint32_t base, const EPWM_SignalParams *signalParams)
{
    float32_t tbClkInHz = 0.0F;
    uint16_t tbPrdVal = 0U, cmpAVal = 0U, cmpBVal = 0U;

    // Check the arguments.
    ASSERT(EPWM_isBaseValid(base));

    // Configure EPWM clock Divider
    SysCtl_setEPWMClockDivider(signalParams->epwmClkDiv);

    // Configure Time Base counter Clock
    EPWM_setClockPrescaler(base, signalParams->tbClkDiv, signalParams->tbHSClkDiv);

    // Configure Time Base Counter Mode
    EPWM_setTimeBaseCounterMode(base, signalParams->tbCtrMode);

    // Calculate TBCLK, TBPRD and CMPx values to be configured for
    // achieving desired signal
    tbClkInHz = ((float32_t) signalParams->sysClkInHz / (1U << ((uint16_t) signalParams->epwmClkDiv + (uint16_t) signalParams->tbClkDiv)));

    if (signalParams->tbHSClkDiv <= EPWM_HSCLOCK_DIVIDER_4) {
        tbClkInHz /= (1U << (uint16_t) signalParams->tbHSClkDiv);
    } else {
        tbClkInHz /= (2U * (uint16_t) signalParams->tbHSClkDiv);
    }

    if (signalParams->tbCtrMode == EPWM_COUNTER_MODE_UP) {
        tbPrdVal = (uint16_t) ((tbClkInHz / signalParams->freqInHz) - 1.0f);
        cmpAVal = (uint16_t) ((float32_t) signalParams->dutyValA * (tbPrdVal + 1U));
        cmpBVal = (uint16_t) ((float32_t) signalParams->dutyValB * (tbPrdVal + 1U));
    } else if (signalParams->tbCtrMode == EPWM_COUNTER_MODE_DOWN) {
        tbPrdVal = (uint16_t) ((tbClkInHz / signalParams->freqInHz) - 1.0f);
        cmpAVal = (uint16_t) ((tbPrdVal + 1U) - ((float32_t) signalParams->dutyValA * (tbPrdVal + 1U)));
        cmpBVal = (uint16_t) ((tbPrdVal + 1U) - ((float32_t) signalParams->dutyValB * (tbPrdVal + 1U)));
    } else if ((signalParams->tbCtrMode == EPWM_COUNTER_MODE_UP_DOWN)) {
        tbPrdVal = (uint16_t) (tbClkInHz / (2.0f * signalParams->freqInHz));
        cmpAVal = (uint16_t) (((float32_t) tbPrdVal - ((float32_t) (signalParams->dutyValA * tbPrdVal))) + 0.5f);
        cmpBVal = (uint16_t) (((float32_t) tbPrdVal - ((float32_t) (signalParams->dutyValB * tbPrdVal))) + 0.5f);
    }

    // Configure TBPRD value
    EPWM_setTimeBasePeriod(base, tbPrdVal);

    // Default Configurations.
    EPWM_disablePhaseShiftLoad(base);
    EPWM_setPhaseShift(base, 0U);
    EPWM_setTimeBaseCounter(base, 0U);

    // Setup shadow register load on ZERO
    EPWM_setCounterCompareShadowLoadMode(base, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(base, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);

    // Set Compare values
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, cmpAVal);
    EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_B, cmpBVal);
}


void drv_value_reset(void)
{
    cv_mode = 0;
    operation.op.cv_mode_flag = FALSE;
    charge_discharge_mode = idle;
    drv_DisablePWM();

    bsp_sec_mosfet(0);  // Secondary MOSFET Off

    control_value_init();
}

//
// End of File
//
