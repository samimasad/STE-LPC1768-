/**********************************************************************
* @file		decrypt.c
* @brief	interface functions for decrypting
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

extern ExpKeyBuffer 	expandedDecryptKey;
extern CypherConfigStr 	decryptEngineConfig;

void deCrypt(const cryptData_t* cryptedText, const uint32_t size, plainData_t* plainText) {

		uint32_t i = 0;
		uint32_t loops = (size/CRYPT_BSIZE); 
		
		#ifdef TEST_MODE
		if((size % CRYPT_BSIZE) != 0) while(1);
		#endif
		
		switch(decryptEngineConfig.cryptMode) {
		
			#ifdef AES128_SUPPORT	
			case AES_128:
			
				for(i=0;i<loops;i++) {
				
					deCryptKernel(	cryptedText,
									plainText, 
									expandedDecryptKey.aes128ExpKey,
									NR_AES128);
									
					#ifdef TEST_MODE
					checkAesDecryptData(decryptEngineConfig.referenceCryptData, cryptedText);
					#endif				
					
					plainText += PLAIN_BSIZE;
					cryptedText += CRYPT_BSIZE; 
				}	
				
				#ifdef TEST_MODE
				
				#endif
				
			break;
			#endif
			
			#ifdef AES192_SUPPORT	
			case AES_192:
				for(i=0;i<loops;i++) {
				
					deCryptKernel(	cryptedText,
									plainText, 
									expandedDecryptKey.aes192ExpKey,
									NR_AES192);
									
					#ifdef TEST_MODE
					checkAesDecryptData(decryptEngineConfig.referenceCryptData, cryptedText);
					#endif				

					
					plainText += PLAIN_BSIZE;
					cryptedText += CRYPT_BSIZE; 
				}	
				
				#ifdef TEST_MODE
				
				#endif
			break;
			#endif
			
			#ifdef AES256_SUPPORT	
			case AES_256:
				for(i=0;i<loops;i++) {
				
					deCryptKernel(	cryptedText,
									plainText, 
									expandedDecryptKey.aes256ExpKey,
									NR_AES256);
									
					#ifdef TEST_MODE
					checkAesDecryptData(decryptEngineConfig.referenceCryptData, cryptedText);
					#endif				

					
					plainText += PLAIN_BSIZE;
					cryptedText += CRYPT_BSIZE; 
				}	
				
				#ifdef TEST_MODE
				
				#endif
			break;
			#endif

		
		};

}


