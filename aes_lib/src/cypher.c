/**********************************************************************
* @file		cypher.c
* @brief	several cypher functions
* @version	1.0
* @date		03.07.2012
* @author	nxp28536
*
* Copyright(C) 2012, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include "cypher.h"

/******************************************************************************
 * Variables used to hold the expanded key, the configuration and a 
 * pointer to the private key
 *****************************************************************************/
ExpKeyBuffer 		expandedCryptKey  	__attribute__ ((aligned (4)));
ExpKeyBuffer 		expandedDecryptKey  __attribute__ ((aligned (4)));
CypherConfigStr 	cryptEngineConfig;
CypherConfigStr 	decryptEngineConfig;


ERROR_CODE cryptLibError = OK_AES;


/******************************************************************************
 * Constant table used for the key expansion
 * in little endian format
 *****************************************************************************/
const uint32_t Rcon[10] __attribute__ ((section ("rcon_table"), aligned (4))) =
{
  0x00000001, 0x00000002, 0x00000004, 0x00000008,
  0x00000010, 0x00000020, 0x00000040, 0x00000080,
  0x0000001B, 0x00000036,
  
};


/******************************************************************************
 * Substitution table used for the bytes substitution subWord function 
 *****************************************************************************/
static const uint8_t S_BOX[16][16]  = {

{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 },
{ 0xca, 0x82, 0xc9, 0x7d, 0xfa,	0x59, 0x47, 0xf0, 0xad,	0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 },
{ 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34,	0xa5, 0xe5, 0xf1, 0x71,	0xd8, 0x31, 0x15 },
{ 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 },
{ 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 },
{ 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf },
{ 0xd0, 0xef, 0xaa, 0xfb, 0x43,	0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 },
{ 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 },
{ 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 },
{ 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb },
{ 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24,	0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 },
{ 0xe7, 0xc8, 0x37, 0x6d, 0x8d,	0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 },
{ 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a },
{ 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e },
{ 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf },
{ 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }

};

#define UPPER_NIBBLE(n) ((n & 0xF0) >> 4)
#define LOWER_NIBBLE(n) (n & 0x0F)

uint32_t subWord(uint32_t word) {

	uint8_t byte1, byte2, byte3, byte4;
	uint32_t result;
	
	byte1 = word & 0xFF;
	byte2 = (word >> 8) & 0xFF;
	byte3 = (word >> 16) & 0xFF;	
	byte4 = (word >> 24) & 0xFF;	
	
	byte1 = S_BOX[UPPER_NIBBLE(byte1)][LOWER_NIBBLE(byte1)];
	byte2 = S_BOX[UPPER_NIBBLE(byte2)][LOWER_NIBBLE(byte2)];
	byte3 = S_BOX[UPPER_NIBBLE(byte3)][LOWER_NIBBLE(byte3)];
	byte4 = S_BOX[UPPER_NIBBLE(byte4)][LOWER_NIBBLE(byte4)];
	
	result = (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
	
	return(result);
}

/******************************************************************************
 * Substitution table used for the bytes replacing subWord function 
 *****************************************************************************/
static const uint8_t INV_S_BOX[16][16]  = {

{ 0x52,	0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5,	0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb },
{ 0x7c, 0xe3, 0x39,	0x82, 0x9b,	0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb },
{ 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e },
{ 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25 },
{ 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92 },
{ 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84 },
{ 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06 },
{ 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b },
{ 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73 },
{ 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e },
{ 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b },
{ 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4 },
{ 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f },
{ 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef },
{ 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61 },
{ 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d }

};

uint32_t invSubWord(uint32_t word) {

	uint8_t byte1, byte2, byte3, byte4;
	uint32_t result;
	
	byte1 = word & 0xFF;
	byte2 = (word >> 8) & 0xFF;
	byte3 = (word >> 16) & 0xFF;	
	byte4 = (word >> 24) & 0xFF;	
	
	byte1 = INV_S_BOX[UPPER_NIBBLE(byte1)][LOWER_NIBBLE(byte1)];
	byte2 = INV_S_BOX[UPPER_NIBBLE(byte2)][LOWER_NIBBLE(byte2)];
	byte3 = INV_S_BOX[UPPER_NIBBLE(byte3)][LOWER_NIBBLE(byte3)];
	byte4 = INV_S_BOX[UPPER_NIBBLE(byte4)][LOWER_NIBBLE(byte4)];
	
	result = (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
	
	return(result);
}




void shiftRows(uint8_t state[4*NB]) {

	uint8_t i;
	uint32_t result;
	
	for(i=1;i<NB;i++) {
	
		result = 0;
		
		result |= state[i];
		result |= state[i+NB] << 8;
		result |= state[i+(2*NB)] << 16;
		result |= state[i+(3*NB)] << 24;

		result = rotRow(result, i*8);

		state[i] = (result & 0xFF);
		state[i+NB] = (result >> 8) & 0xFF;
		state[i+(2*NB)] = (result >> 16) & 0xFF;
		state[i+(3*NB)] = (result >> 24) & 0xFF;
	};
}

void invShiftRows(uint8_t state[4*NB]) {

	uint8_t i;
	uint32_t result;
	
	for(i=1;i<NB;i++) {
	
		result = 0;
		result |= state[i];
		result |= state[i+NB] << 8;
		result |= state[i+(2*NB)] << 16;
		result |= state[i+(3*NB)] << 24;

		result = rotRow(result, 32-(i*8));

		state[i] = (result & 0xFF);
		state[i+NB] = (result >> 8) & 0xFF;
		state[i+(2*NB)] = (result >> 16) & 0xFF;
		state[i+(3*NB)] = (result >> 24) & 0xFF;
	};
}


void mixColumns(uint32_t* statePtr) {

	#define BYTE1	0
	#define BYTE2	1
	#define BYTE3	2
	#define BYTE4	3
	
	uint8_t byte1, byte2, byte3, byte4;
	uint32_t mixedColumn;
	uint8_t mult1, mult2, mult3, mult4;
	uint32_t* ptr;
	uint8_t i;
	uint8_t xtime_result[4];
	
	for(i=0, ptr=statePtr;i<NB;i++,ptr++) {		
		
		byte1 = (*ptr) & 0xFF;
		byte2 = ((*ptr) >> 8) & 0xFF;
		byte3 = ((*ptr) >> 16) & 0xFF;
		byte4 = ((*ptr) >> 24) & 0xFF;	
		
		xtime_result[BYTE1] = xtime(byte1);
		xtime_result[BYTE2] = xtime(byte2);
		xtime_result[BYTE3] = xtime(byte3);		
		xtime_result[BYTE4] = xtime(byte4);				

		/* first column */	
		mult1 = xtime_result[0] ^ xtime(byte2) ^ byte2 ^ byte3 ^ byte4;

		/* second column */
		mult2 = (byte1 ^ xtime_result[BYTE2] ^ xtime_result[BYTE3] ^ byte3 ^ byte4) ;
		
		/* third column */
		mult3 = byte1 ^ byte2 ^ xtime_result[BYTE3] ^ xtime_result[BYTE4] ^ byte4;

		/* fourth column */
		mult4 = xtime_result[BYTE1] ^ byte1 ^ byte2 ^ byte3 ^ xtime_result[BYTE4];
		
		mixedColumn = mult1 | (mult2 << 8) | (mult3 << 16) | (mult4 << 24);
		
		*ptr = mixedColumn;
	}
}

void invMixColumns(uint32_t* statePtr) {

	#define BYTE1	0
	#define BYTE2	1
	#define BYTE3	2
	#define BYTE4	3

	uint8_t byte1, byte2, byte3, byte4;
	uint8_t result1, result2, result3, result4;	
	uint32_t mixedColumn;
	uint8_t mult1, mult2, mult3, mult4;
	uint32_t* ptr;
	uint8_t i;

	uint8_t xtime_result[4];	
	uint8_t xtime2_result[4];	
	uint8_t xtime3_result[4];	
	
	for(i=0, ptr=statePtr;i<NB;i++,ptr++) {		
		
		byte1 = (*ptr) & 0xFF;
		byte2 = ((*ptr) >> 8) & 0xFF;
		byte3 = ((*ptr) >> 16) & 0xFF;
		byte4 = ((*ptr) >> 24) & 0xFF;	

		/* prepare the expansion results */
		xtime_result[BYTE1] = xtime(byte1);
		xtime_result[BYTE2] = xtime(byte2);
		xtime_result[BYTE3] = xtime(byte3);		
		xtime_result[BYTE4] = xtime(byte4);	

		xtime2_result[BYTE1] = xtime(xtime_result[BYTE1]);
		xtime2_result[BYTE2] = xtime(xtime_result[BYTE2]);
		xtime2_result[BYTE3] = xtime(xtime_result[BYTE3]);		
		xtime2_result[BYTE4] = xtime(xtime_result[BYTE4]);			

		xtime3_result[BYTE1] = xtime(xtime2_result[BYTE1]);
		xtime3_result[BYTE2] = xtime(xtime2_result[BYTE2]);
		xtime3_result[BYTE3] = xtime(xtime2_result[BYTE3]);		
		xtime3_result[BYTE4] = xtime(xtime2_result[BYTE4]);
		

		/* first column */	
		/* 0E */
		result1 = xtime3_result[BYTE1] ^ xtime2_result[BYTE1] ^ xtime_result[BYTE1];
		
		/* 0B */
		result2 = xtime3_result[BYTE2] ^ xtime_result[BYTE2] ^ byte2;

		/* 0D */
		result3 = xtime3_result[BYTE3] ^ xtime2_result[BYTE3] ^ byte3;

		/* 09 */
		result4 = xtime3_result[BYTE4] ^ byte4;
		
		mult1 = result1 ^ result2 ^ result3 ^ result4;

		/* second column */
		/* 0x09 */
		result1 = xtime3_result[BYTE1] ^ byte1;

		/* 0x0E */
		result2 = xtime3_result[BYTE2] ^ xtime2_result[BYTE2] ^ xtime_result[BYTE2];
		
		/* 0B */	
		result3 = xtime3_result[BYTE3] ^ xtime_result[BYTE3] ^ byte3;
		
		/* 0D */
		result4 = xtime3_result[BYTE4] ^ xtime2_result[BYTE4] ^ byte4;

		mult2 = (result1 ^ result2 ^ result3 ^ result4) ;
		
		/* third column */
		/* 0D */
		result1 = xtime3_result[BYTE1] ^ xtime2_result[BYTE1] ^ byte1;
		
		/* 0x09 */
		result2 = xtime3_result[BYTE2] ^ byte2;

		/* 0x0E */
		result3 = xtime3_result[BYTE3] ^ xtime2_result[BYTE3] ^ xtime_result[BYTE3];		

		/* 0B */	
		result4 = xtime3_result[BYTE4] ^ xtime_result[BYTE4] ^ byte4;

		mult3 = result1 ^ result2 ^ result3 ^ result4;

		/* fourth column */
		/* 0B */
		result1 = xtime3_result[BYTE1] ^ xtime_result[BYTE1] ^ byte1;

		/* 0D */
		result2 = xtime3_result[BYTE2] ^ xtime2_result[BYTE2] ^ byte2;
		
		/* 0x09 */
		result3 = xtime3_result[BYTE3] ^ byte3;		

		/* 0x0E */
		result4 = xtime3_result[BYTE4] ^ xtime2_result[BYTE4] ^ xtime_result[BYTE4];	
		
		mult4 = result1 ^ result2 ^ result3 ^ result4;
		
		mixedColumn = mult1 | (mult2 << 8) | (mult3 << 16) | (mult4 << 24);
		
		*ptr = mixedColumn;
	}
}

void cryptKernel(	const plainData_t plainText[PLAIN_BSIZE], 
					cryptData_t cryptedText[CRYPT_BSIZE], 
					expKey_t* expKey, const uint8_t numRounds) {
					
	uint8_t state[4*NB];
	uint32_t* ptr, *statePtr;
	uint8_t i, round;

	/* copy plain input into state to initialize */
    ptr = (uint32_t*) &plainText[0];
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++)
		*statePtr++ = *ptr++;

	/* first iteration, round 0 */		
	addRoundkey((uint32_t*) &state[0], expKey);
	
	
	/* Nround - 1 iterations */
	for(round=1;round<numRounds;round++) {
	
		statePtr = (uint32_t*) &state[0];
		for(i=0;i<NB;i++) 
			*statePtr++ = subWord(*statePtr);
		shiftRows(state);
		mixColumns((uint32_t*) &state[0]);
		addRoundkey((uint32_t*) &state[0], expKey +(round*NB));
	}
	
	/* last iteration, round 11 */
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++) 
			*statePtr++ = subWord(*statePtr);
	shiftRows(state);
	addRoundkey((uint32_t*) &state[0], expKey +(numRounds*NB));

	/* now copy back the crypted text into the buffer */
    ptr = (uint32_t*) &cryptedText[0];
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++)
		*ptr++ = *statePtr++;	
	
}

void deCryptKernel(const cryptData_t cryptedText[CRYPT_BSIZE],
					plainData_t plainText[PLAIN_BSIZE], 
					expKey_t* expKey, const uint8_t numRounds) {
					
	uint8_t state[4*NB];				
	uint32_t* ptr, *statePtr;
	uint8_t i, round;

	/* copy plain input into state to initialize */
    ptr = (uint32_t*) &cryptedText[0];
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++)
		*statePtr++ = *ptr++;

	/* first iteration, round 0 */
	/* take the key from the bottom of the array */
	addRoundkey((uint32_t*) &state[0], expKey + (NB*numRounds));
	
	
	/* Go from the next downto Nround - 1 iterations */
	for(round=(numRounds - 1); round > 0; round--) {
	
		statePtr = (uint32_t*) &state[0];
		for(i=0;i<NB;i++) 
			*statePtr++ = invSubWord(*statePtr);
		invShiftRows(state);
		invMixColumns((uint32_t*) &state[0]);
		addRoundkey((uint32_t*) &state[0], expKey + (round*NB));
	}
	
	/* last iteration, round 0 */
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++) 
			*statePtr++ = invSubWord(*statePtr);
	invShiftRows(state);
	addRoundkey((uint32_t*) &state[0], expKey);

	/* now copy back the crypted text into the buffer */
    ptr = (uint32_t*) &plainText[0];
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++)
		*ptr++ = *statePtr++;						
					
};


void checkAesCryptData(const cryptData_t cryptedText[CRYPT_BSIZE], const cryptData_t referenceText[CRYPT_BSIZE]) {

	uint32_t i = 0;
	
	for(i=0;i<CRYPT_BSIZE;i++) {
	
			if((*cryptedText++) != (*referenceText++)) {
				setErrorCode(AES_CRYPT_FAILURE);
				return;
			};
	}
}

void checkAesDecryptData(const plainData_t deCryptedText[PLAIN_BSIZE], const plainData_t referenceText[PLAIN_BSIZE]) {

	uint32_t i = 0;
	
	for(i=0;i<PLAIN_BSIZE;i++) {
	
			if((*deCryptedText++) != (*referenceText++)) {
				setErrorCode(AES_DECRYPT_FAILURE);
				return;
			};
	}
}


ERROR_CODE checkErrors(void) {

	return(cryptLibError);
}




