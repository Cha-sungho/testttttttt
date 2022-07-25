/*
 * Phoenixon_SCIB.h
 *
 *  Created on: 2017. 12. 2.
 *      Author: BGKim
 */

#ifndef PHOENIXON_SCID_H
#define PHOENIXON_SCID_H

#define SCI_STX 0x02
#define SCI_ETX 0x03

#define SCI_RX_DATA_LENGTH 7
#define SCI_TX_DATA_LENGTH 7

#define SCI_TX_ERROR 0xFFFF
#define SCI_RX_ERROR 0xFFFF

unsigned int ASCIIToUInt(unsigned int ui_Cipher, unsigned int *aui_ASCIIData);
void UIntToASCII(unsigned int ui_Cipher, unsigned int ui_Data,  unsigned int *aui_ASCIIData);
void FloatToArray(unsigned int ui_Cipher, float f_Data,  unsigned int *aui_ArrayData);
void ScidSetup(void);
unsigned ScidPut(unsigned int ui_Character);
unsigned int ScidWirte(unsigned int *ui_String, unsigned int ui_Length);
unsigned int ScidGet(void);

#endif /* INCLUDE_PHOENIXON_SCIB_H_ */
