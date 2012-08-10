/**********************************************************************
* $Id$		nxp28536			2012-06-19
*//**
* @file		decrypt_aes128.c
* @brief	configuration modules for single phase mode 
* @version	1.0
* @date		19. June. 2012
* @author	NXP MCU SW Application Team
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
				
void deCrypt_AES128(const cryptData_t cryptedText[CRYPT_BSIZE],
					plainData_t plainText[PLAIN_BSIZE], 
					expKey_t expKey[KSCH_AES128_SIZE]) {
					
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
	addRoundkey((uint32_t*) &state[0], &expKey[NB*NR_AES128]);
	
	
	/* Go from the next downto Nround - 1 iterations */
	for(round=(NR_AES128 - 1); round > 0; round--) {
	
		statePtr = (uint32_t*) &state[0];
		for(i=0;i<NB;i++) 
			*statePtr++ = invSubWord(*statePtr);
		invShiftRows(state);
		invMixColumns((uint32_t*) &state[0]);
		addRoundkey((uint32_t*) &state[0], &expKey[round*NB]);
	}
	
	/* last iteration, round 0 */
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++) 
			*statePtr++ = invSubWord(*statePtr);
	invShiftRows(state);
	addRoundkey((uint32_t*) &state[0], &expKey[0]);

	/* now copy back the crypted text into the buffer */
    ptr = (uint32_t*) &plainText[0];
	statePtr = (uint32_t*) &state[0];
	for(i=0;i<NB;i++)
		*ptr++ = *statePtr++;						
					
};

