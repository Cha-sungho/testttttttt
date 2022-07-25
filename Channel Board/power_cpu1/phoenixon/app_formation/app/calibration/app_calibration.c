/*
 * app_calibration.c
 *
 *  Created on: 2022. 3. 8.
 *      Author: kms
 */
#include "f28x_project.h"
#include "driverlib.h"
#include "device.h"
#include "app_calibration.h"
#include "app_sense.h"
#include "app_op.h"
#include "bsp_io.h"
#include "drv_timer.h"

#define CALIBRATION_VOLATAGE_UNIT_VALUE        (1.0)
#define CALIBRATION_VOLATAGE_MIN_VALUE         (0.0)
#define CALIBRATION_VOLATAGE_MAX_VALUE         (5.0)
#define CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH  (Uint16)(((CALIBRATION_VOLATAGE_MAX_VALUE - CALIBRATION_VOLATAGE_MIN_VALUE) / (CALIBRATION_VOLATAGE_UNIT_VALUE)) + 1.0)

#define CALIBRATION_CURRENT_UNIT_VALUE         (5.0)
#define CALIBRATION_CURRENT_MIN_VALUE          (0.0)
#define CALIBRATION_CURRENT_MAX_VALUE          (60.0)
#define CALIBRATION_CURRENT_MAX_ARRAY_LENGTH   (Uint16)(((CALIBRATION_CURRENT_MAX_VALUE - CALIBRATION_CURRENT_MIN_VALUE) / (CALIBRATION_CURRENT_UNIT_VALUE)) + 1.0)

extern float calc_dc_out_i_emavg;
extern float sense_batt_v_emavg;
extern float sense_batt_v_cal;
extern float sense_batt_v;

extern float sense_dc_bb1_i;
extern float sense_dc_bb2_i;

extern float sense_dc_bb1_i_cal;
extern float sense_dc_bb2_i_cal;

// Voltage Calibration
float sum_voltage_value = 0.0;
float voltage_array[100] = { 0.0, };
float avr_voltage_value = 0.0;
extern float sense_batt_v_emavg;
extern float sense_batt_v_cal;
extern float sense_dc_out_cap_v_cal;
extern Uint16 voltage_calibration_tune_flag;
extern Uint16 voltage_calibration_vrf_flag;
extern Uint16 vout_calibration_flag;
float voltage_diff = 0.0;

float32 Bat_Voltage_ADC[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH] = {
                                                              0.0,
                                                              1.0,
                                                              2.0,
                                                              3.0,
                                                              4.0,
                                                              5.0

};

float32 Bat_Voltage_verify[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH] = {
                                                              0.0,
                                                              1.0,
                                                              2.0,
                                                              3.0,
                                                              4.0,
                                                              5.0
};

float32 Out_Voltage_ADC[CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH] = {
                                                              0.0,
                                                              1.0,
                                                              2.0,
                                                              3.0,
                                                              4.0,
                                                              5.0
};

////// Current Path 1 //////////////////////////////////////////////////////////////////////////////
float32 Current_Charge_Scale1[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH] = {
                                                /* 0.0,        */      20.58313179,
                                                /* 5.0,        */      20.58313179,
                                                /* 10.0,       */      20.58313179,
                                                /* 15.0,       */      20.58313179,
                                                /* 20.0,       */      20.58313179,
                                                /* 25.0,       */      20.58313179,
                                                /* 30.0,       */      20.58313179,
                                                /* 35.0,       */      20.58313179,
                                                /* 40.0,       */      20.58313179,
                                                /* 45.0,       */      20.58313179,
                                                /* 50.0,       */      20.58313179,
                                                /* 55.0,       */      20.58313179,
                                                /* 60.0,       */      20.58313179
};

float32 Current_Discharge_Scale1[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH] = {
                                                /* 0.0,        */      20.58313179,
                                                /* 5.0,        */      20.58313179,
                                                /* 10.0,       */      20.58313179,
                                                /* 15.0,       */      20.58313179,
                                                /* 20.0,       */      20.58313179,
                                                /* 25.0,       */      20.58313179,
                                                /* 30.0,       */      20.58313179,
                                                /* 35.0,       */      20.58313179,
                                                /* 40.0,       */      20.58313179,
                                                /* 45.0,       */      20.58313179,
                                                /* 50.0,       */      20.58313179,
                                                /* 55.0,       */      20.58313179,
                                                /* 60.0,       */      20.58313179
};

////// Current Path 2 //////////////////////////////////////////////////////////////////////////////
float32 Current_Charge_Scale2[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH] = {
                                                /* 0.0,        */      20.58313179,
                                                /* 5.0,        */      20.58313179,
                                                /* 10.0,       */      20.58313179,
                                                /* 15.0,       */      20.58313179,
                                                /* 20.0,       */      20.58313179,
                                                /* 25.0,       */      20.58313179,
                                                /* 30.0,       */      20.58313179,
                                                /* 35.0,       */      20.58313179,
                                                /* 40.0,       */      20.58313179,
                                                /* 45.0,       */      20.58313179,
                                                /* 50.0,       */      20.58313179,
                                                /* 55.0,       */      20.58313179,
                                                /* 60.0,       */      20.58313179
};

float32 Current_Discharge_Scale2[CALIBRATION_CURRENT_MAX_ARRAY_LENGTH] = {
                                                /* 0.0,        */      20.58313179,
                                                /* 5.0,        */      20.58313179,
                                                /* 10.0,       */      20.58313179,
                                                /* 15.0,       */      20.58313179,
                                                /* 20.0,       */      20.58313179,
                                                /* 25.0,       */      20.58313179,
                                                /* 30.0,       */      20.58313179,
                                                /* 35.0,       */      20.58313179,
                                                /* 40.0,       */      20.58313179,
                                                /* 45.0,       */      20.58313179,
                                                /* 50.0,       */      20.58313179,
                                                /* 55.0,       */      20.58313179,
                                                /* 60.0,       */      20.58313179
};


float32 CALIBRATION_ConversionADC(float32 ADC_Data, float32 *CAL_Data, float32 InitialValue, float32 Unit, Uint16 ArrayLength)
{
    Uint16 ArrayAddressValue = 0.0;
    Uint16 ui_i = 0;

    float Result = 0.0;
    float CalGain = 0.0;
    float CalOffset = 0.0;

    if (ADC_Data > CAL_Data[0]) {
        for (ui_i = 0; ui_i < ArrayLength; ui_i++) {
            if (CAL_Data[ui_i] >= ADC_Data) {
                break;
            }
        }
        if (ui_i < ArrayLength) ArrayAddressValue = ui_i;
        else ArrayAddressValue = ui_i - 1;
    } else {
        return (ADC_Data);
    }

    CalGain = Unit / (CAL_Data[ArrayAddressValue] - CAL_Data[ArrayAddressValue - 1]);       //CalGain = (xH - xL)/(yH - yL)
    CalOffset = (CAL_Data[ArrayAddressValue - 1] * CalGain)
                    - ((ArrayAddressValue - ((InitialValue / Unit) * (-1) + 1)) * Unit);    //CalOffset = yL * CalGain - xL
    Result = ADC_Data * CalGain - CalOffset;                                                //x = y * CalGain - CalOffset

    return (Result);
}

void calibration_run(void)
{
    sense_batt_v_cal = CALIBRATION_ConversionADC(sense_batt_v, Bat_Voltage_ADC,
    CALIBRATION_VOLATAGE_MIN_VALUE, CALIBRATION_VOLATAGE_UNIT_VALUE,
    CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);

    sense_dc_out_cap_v_cal = CALIBRATION_ConversionADC(sense_dc_out_cap_v, Out_Voltage_ADC,
    CALIBRATION_VOLATAGE_MIN_VALUE, CALIBRATION_VOLATAGE_UNIT_VALUE,
    CALIBRATION_VOLATAGE_MAX_ARRAY_LENGTH);
}

void voltage_calibration(Uint16 volt)
{
    switch (volt)
    {
        case 1:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_function();
            Bat_Voltage_ADC[1] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 2:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_function();
            Bat_Voltage_ADC[2] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 3:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_function();
            Bat_Voltage_ADC[3] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 4:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_function();
            Bat_Voltage_ADC[4] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 5:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_function();
            Bat_Voltage_ADC[5] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        default:
            break;
    }
}

void voltage_calibration_verify(Uint16 volt)
{
    switch (volt)
    {
        case 1:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_verify();
            Bat_Voltage_verify[1] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 2:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_verify();
            Bat_Voltage_verify[2] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 3:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_verify();
            Bat_Voltage_verify[3] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 4:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_verify();
            Bat_Voltage_verify[4] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        case 5:
            bsp_IoEnableBattSen(1);  // Battery Sensing Relay On
            avr_voltage_value = bat_voltage_calibration_verify();
            Bat_Voltage_verify[5] = avr_voltage_value;
            sum_voltage_value = 0.0;
            break;
        default:
            break;
    }
}

float bat_voltage_calibration_function(void)
{
    static Uint16 old_t = 0;
    static Uint16 new_t = 0;
    static Uint16 cnt = 0;
    Uint32 i, j = 0;

    new_t = get_time_ms();

    if (get_time_diff(old_t, new_t) >= 10)  //@10ms timeout
    {
        old_t = new_t;
        voltage_array[cnt] = sense_batt_v;
        cnt++;
    }
    if (cnt >= 100) {
        InsertionSort(voltage_array, 99);
        cnt = 0;

        for (i = 35; i < 75; i++, j++) {
            sum_voltage_value += voltage_array[i];
        }
        sum_voltage_value = sum_voltage_value / j;
        voltage_calibration_tune_flag = 0;
        bsp_IoEnableBattSen(0);

        return sum_voltage_value;
    }
    return 0;
}

float bat_voltage_calibration_verify(void)
{
    static Uint16 old_t = 0;
    static Uint16 new_t = 0;
    static Uint16 cnt = 0;
    Uint32 i, j = 0;

    new_t = get_time_ms();

    if (get_time_diff(old_t, new_t) >= 10)  //@10ms timeout
    {
        old_t = new_t;
        voltage_array[cnt] = sense_batt_v_cal;
        cnt++;
    }
    if (cnt >= 100) {
        InsertionSort(voltage_array, 99);
        cnt = 0;

        for (i = 35; i < 75; i++, j++) {
            sum_voltage_value += voltage_array[i];
        }
        sum_voltage_value = sum_voltage_value / j;
        voltage_calibration_vrf_flag = 0;
        bsp_IoEnableBattSen(0);

        return sum_voltage_value;
    }
    return 0;
}

void InsertionSort(float *Data, int Number)
{
    int i, j;
    float Key;

    for (i = 1; i <= Number; i++) {
        Key = Data[i];
        j = i - 1;
        while ((j >= 0) && (Data[j] > Key)) {
            Data[j + 1] = Data[j];
            j--;
        }
        Data[j + 1] = Key;
    }
}

//
// End of File
//