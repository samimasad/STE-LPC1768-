/**********************************************************************
* @file		cypher_internal.h
* @brief	configuration modules for single phase mode 
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
#ifndef __CYPHER_INTERNAL__H
#define __CYPHER_INTERNAL__H


#ifdef CORE_M3

#endif

#define rotWord(word) 		__ROR(word, 8)
#define rotRow(word, bits) 	__ROR(word, bits)
#define swapBytes(word)		__REV(word)

extern const uint32_t Rcon[10];

extern uint32_t subWord(uint32_t word);
extern void shiftRows(uint8_t state[4*NB]);
extern void mixColumns(uint32_t* statePtr);

extern void invMixColumns(uint32_t* statePtr);
extern void invShiftRows(uint8_t state[4*NB]);
extern uint32_t invSubWord(uint32_t word);



/* this function computes the input multiplicated by two, for GF(2^8) */
static uint8_t xtime(uint8_t value)  __attribute__((always_inline));
static uint8_t xtime(uint8_t value) {

	uint8_t result;
	
	/* shift by one, multiply by two */
	result = value << 1;
	
	/* if MSB was one, XOR with 1B before returning  */
	if(value & 0x80) 
		return(result ^ 0x1B); 
	else return(result);

}


extern ERROR_CODE cryptLibError;

static void setErrorCode(ERROR_CODE code) __attribute__((always_inline));
static void setErrorCode(ERROR_CODE code) {

		cryptLibError |= code;
}



/* this function is used to XOR the expanded key columns with the state */
static void addRoundkey(uint32_t* state, expKey_t* expKey) __attribute__((always_inline));
static void addRoundkey(uint32_t* state, expKey_t* expKey) {

	uint8_t i;
	
	for(i=0;i<NB;i++)
		*state++ ^= (*expKey++);

}

/* used for priming the expanded key for the equivalent inverse cypher */
#ifdef AES128_SUPPORT
extern void eqInvKeyShuffle128(expKey_t expKey[KSCH_AES128_SIZE]);
#endif
#ifdef AES192_SUPPORT
extern void eqInvKeyShuffle192(expKey_t expKey[KSCH_AES192_SIZE]);
#endif
#ifdef AES256_SUPPORT
extern void eqInvKeyShuffle256(expKey_t expKey[KSCH_AES256_SIZE]);
#endif

extern void cryptKernel(	const plainData_t plainText[PLAIN_BSIZE], 
					cryptData_t cryptedText[CRYPT_BSIZE], 
					expKey_t* expKey, const uint8_t numRounds);
					
extern void deCryptKernel(const cryptData_t cryptedText[CRYPT_BSIZE],
					plainData_t plainText[PLAIN_BSIZE], 
					expKey_t* expKey, const uint8_t numRounds);					
					
#ifdef TEST_MODE

extern void runFips197Test(void);

#ifdef AES128_SUPPORT
extern void checkAes128ExpCryptKey(expKey_t expKey[KSCH_AES128_SIZE], const expKey_t referenceKey[KSCH_AES128_SIZE]);
extern void checkAes128ExpDecryptKey(expKey_t expKey[KSCH_AES128_SIZE], const expKey_t referenceKey[KSCH_AES128_SIZE]);
#endif

#ifdef AES192_SUPPORT
extern void checkAes192ExpCryptKey(expKey_t expKey[KSCH_AES192_SIZE], const expKey_t referenceKey[KSCH_AES192_SIZE]);
extern void checkAes192ExpDecryptKey(expKey_t expKey[KSCH_AES192_SIZE], const expKey_t referenceKey[KSCH_AES192_SIZE]);
#endif

#ifdef AES256_SUPPORT
extern void checkAes256ExpCryptKey(expKey_t expKey[KSCH_AES256_SIZE], const expKey_t referenceKey[KSCH_AES256_SIZE]);
extern void checkAes256ExpDecryptKey(expKey_t expKey[KSCH_AES256_SIZE], const expKey_t referenceKey[KSCH_AES256_SIZE]);
#endif


void checkAesCryptData(const cryptData_t cryptedText[CRYPT_BSIZE], const cryptData_t referenceText[CRYPT_BSIZE]); 
void checkAesDecryptData(const plainData_t deCryptedText[PLAIN_BSIZE], const plainData_t referenceText[PLAIN_BSIZE]);

#endif




#endif
