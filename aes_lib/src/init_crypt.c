/**********************************************************************
* @file		init_crypt.c
* @brief	functions to initialize the crypt engine
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

extern CypherConfigStr 	cryptEngineConfig;
extern ExpKeyBuffer 	expandedCryptKey;

#ifdef TEST_MODE
int checkAes128ExpKey(expKey_t expKey[KSCH_AES128_SIZE], const expKey_t referenceKey[KSCH_AES128_SIZE]);
#endif

void initCrypt(const CypherConfig configuration) {

		uint32_t i;
		
		/* save for later use */
		cryptEngineConfig.cryptMode = configuration->cryptMode;
		cryptEngineConfig.keyLocation = configuration->keyLocation;

		#ifdef TEST_MODE
		cryptEngineConfig.referenceKey = configuration->referenceKey;
		cryptEngineConfig.referenceCryptData = configuration->referenceCryptData;
		#endif
		
		switch(cryptEngineConfig.cryptMode) {
		
		#ifdef AES128_SUPPORT		
			case AES_128:
			
				for(i=0;i<KSCH_AES128_SIZE;i++)
					expandedCryptKey.aes128ExpKey[i] = 0;

				genAes128ExpKey(cryptEngineConfig.keyLocation, expandedCryptKey.aes128ExpKey);
				
				#ifdef TEST_MODE
				checkAes128ExpCryptKey(expandedCryptKey.aes128ExpKey, configuration->referenceKey );
				#endif
				
			break;
		#endif
		
		#ifdef AES192_SUPPORT		
			case AES_192:
				for(i=0;i<KSCH_AES192_SIZE;i++)
				expandedCryptKey.aes192ExpKey[i] = 0;
				
				genAes192ExpKey(cryptEngineConfig.keyLocation, expandedCryptKey.aes192ExpKey);
				
				#ifdef TEST_MODE
				checkAes192ExpCryptKey(expandedCryptKey.aes192ExpKey, configuration->referenceKey );
				#endif				
				
			break;
		#endif
		
		#ifdef AES256_SUPPORT		
			case AES_256:
				for(i=0;i<KSCH_AES256_SIZE;i++)
				expandedCryptKey.aes256ExpKey[i] = 0;
				
				genAes256ExpKey(cryptEngineConfig.keyLocation, expandedCryptKey.aes256ExpKey);
				
				#ifdef TEST_MODE
				checkAes256ExpCryptKey(expandedCryptKey.aes256ExpKey, configuration->referenceKey );
				#endif	
				
			break;
		#endif		
		
			
		};
};


