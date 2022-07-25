/*
 * Phoenixon_I2C_TCA9535.h
 *
 *  Created on: 2017. 10. 2.
 *      Author: BGKim
 */

#ifndef PHOENIXON_I2C_TCA9535_H_
#define PHOENIXON_I2C_TCA9535_H_

#define I2C_TCA9535_ADDRESS 0X20


void I2CA_TCA9535_Init(void);
int I2CA_TCA9535_WriteData(unsigned int ui_Address, unsigned int ui_Value);
unsigned int TCA9535_IOPort_Setting(unsigned int ui_PortNumber, unsigned int Value);

#endif /* PHOENIXON_I2C_TCA9535_H_ */
