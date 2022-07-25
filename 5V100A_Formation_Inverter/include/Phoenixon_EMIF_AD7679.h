/*
 * Phoenixon_EMIF_AD7679.h
 *
 *  Created on: 2017. 11. 20.
 *      Author: BGKim
 */

#ifndef PHOENIXON_EMIF_AD7679_H_
#define PHOENIXON_EMIF_AD7679_H_

void Emif1_AD7679_Init(void);
int AD7679_Conversion(void);
void ADC_MUX_SEL(unsigned int ui_sel);
float AD7679_Read(unsigned int ui_CS_Select);

#endif /* PHOENIXON_EMIF_AD7679_H_ */
