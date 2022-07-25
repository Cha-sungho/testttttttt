/*
 * app_calibrtion.h
 *
 *  Created on: 2022. 3. 8.
 *      Author: kms
 */

#ifndef PHOENIXON_APP_FORMATION_APP_CALIBRATION_APP_CALIBRATION_H_
#define PHOENIXON_APP_FORMATION_APP_CALIBRATION_APP_CALIBRATION_H_

float32 CALIBRATION_ConversionADC(float32 ADC_Data, float32 *CAL_Data, float32 InitialValue, float32 Unit, Uint16 ArrayLength);
void calibration_run(void);
float bat_voltage_calibration_function(void);
float bat_voltage_calibration_verify(void);
float out_voltage_calibration(void);
void InsertionSort(float *Data, int Number);
void voltage_calibration(Uint16 volt);
void voltage_calibration_verify(Uint16 volt);

#endif /* PHOENIXON_APP_FORMATION_APP_CALIBRATION_APP_CALIBRATION_H_ */