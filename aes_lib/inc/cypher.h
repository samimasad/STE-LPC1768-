/**********************************************************************
* @file		cypher.h
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
#ifndef __CYPHER__H
#define __CYPHER__H


/******************************************************************************
 * Comment / Uncomment the following definitions to remove / add support in 
 * the library build
 *****************************************************************************/
#define AES128_SUPPORT
#define AES192_SUPPORT
#define AES256_SUPPORT

/******************************************************************************
 * Incude the algorithm specifics
 *****************************************************************************/
#include "cypher_parameters.h"

/******************************************************************************
 * Configuration of the algorithm type
 * Allowed values:
 * 	- AES_128 for a 128 bit key
 * 	- AES_192 for a 192 bit key
 * 	- AES_256 for a 256 bit key
 *****************************************************************************/
 typedef enum CRYPT_MODE {
 
	AES_128,
	AES_192,
	AES_256
 
 } CRYPT_MODE;
 
 
typedef struct CypherConfigStr {

	CRYPT_MODE		cryptMode;
	aeskey_t*			keyLocation;
#ifdef TEST_MODE
	const expKey_t* 	referenceKey;
	const cryptData_t*  referenceCryptData;
#endif	
	
} CypherConfigStr;
 
typedef CypherConfigStr* CypherConfig;

void initCrypt(const CypherConfig configuration);
void initDeCrypt(const CypherConfig configuration);

void crypt(const plainData_t* plainText, const uint32_t size, cryptData_t* cryptedText);
void deCrypt(const cryptData_t* cryptedText, const uint32_t size, plainData_t* plainText);


/******************************************************************************
 * AES 128 functions
 *****************************************************************************/
#ifdef AES128_SUPPORT
	#include "aes_128.h"
#endif

/******************************************************************************
 * AES 192 functions
 *****************************************************************************/
#ifdef AES192_SUPPORT
	#include "aes_192.h"
#endif

/******************************************************************************
 * AES 256 functions
 *****************************************************************************/
#ifdef AES256_SUPPORT
	#include "aes_256.h"
#endif


/******************************************************************************
 * Utility functions
 *****************************************************************************/
typedef enum ERROR_CODE {
	
	OK_AES = 0,
	AES128_CRYPTKEY_FAILURE = 0x1,
	AES128_DECRYPTKEY_FAILURE = 0x2,
	AES192_CRYPTKEY_FAILURE = 0x10,
	AES192_DECRYPTKEY_FAILURE = 0x20,
	AES256_CRYPTKEY_FAILURE = 0x100,
	AES256_DECRYPTKEY_FAILURE = 0x200,
	AES_CRYPT_FAILURE = 0x400,
	AES_DECRYPT_FAILURE = 0x800,	

} ERROR_CODE;


ERROR_CODE checkErrors(void);



#ifdef TARGET_LPC11A
	#include "lpc11axx.h"
#endif

#ifdef TARGET_LPC1769
	#include "lpc17xx.h"
#endif

#ifdef TARGET_LPC1768
	#include "LPC17xx.h"
#endif


#include "cypher_internal.h"



#endif
