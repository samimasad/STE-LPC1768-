/**********************************************************************
* @file		cypher_parameters.h
* @brief	paramters for the AES algorithm 
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
#ifndef __CYPHER_PARAMETERS__H
#define __CYPHER_PARAMETERS__H

#include <stdint.h>

/******************************************************************************
 * These parameters are defined accorging to FIPS-197
 * http://csrc.nist.gov/publications
 *****************************************************************************/


/******************************************************************************
 * State columns
 * fixed for all allowed configurations
 *****************************************************************************/
#define NB	4	


#ifdef AES128_SUPPORT
/******************************************************************************
 * AES 128 specific
 *****************************************************************************/
/* Number of 32-bit words comprising the cypher key */
#define NK_AES128	4

/* Number of bytes comprising the cypher key */
#define AES128_KEYSIZE	(4*NK_AES128)
	
/* Number of rounds defined for the combination of NB and NK */
/* (number of loops within the crypt procedure )*/
#define NR_AES128	10

/* 176 bytes for key expansion on AES 128 */
/* 44 words */
#define KSCH_AES128_SIZE	(NB*(NR_AES128+1))

#endif /* AES128_SUPPORT */



#ifdef AES192_SUPPORT
/******************************************************************************
 * AES 192 specific
 *****************************************************************************/
#define NK_AES192	6

/* Number of bytes comprising the cypher key */
#define AES192_KEYSIZE	(4*NK_AES192)

#define NR_AES192	12

/* 208 bytes for key expansion on AES 192 */
/* 52 words */
#define KSCH_AES192_SIZE	(NB*(NR_AES192+1))

#endif /* AES192_SUPPORT */



#ifdef AES256_SUPPORT
/******************************************************************************
 * AES 256 specific
 *****************************************************************************/
#define NK_AES256	8

/* Number of bytes comprising the cypher key */
#define AES256_KEYSIZE	(4*NK_AES256)

#define NR_AES256	14

/* 240 bytes for key expansion on AES 256 */
#define KSCH_AES256_SIZE	(NB*(NR_AES256+1))

#endif /* AES256_SUPPORT */


/******************************************************************************
 * size of the cypher items in bytes
 *****************************************************************************/
/* input items are in bytes */
typedef uint8_t plainData_t;
typedef uint8_t cryptData_t;
typedef uint8_t data_t;

/* state matrix is expressed in bytes */
typedef uint8_t State_t;

/* input and output buffer blocks processed every turn */
#define PLAIN_BSIZE			(4*NB) 
#define CRYPT_BSIZE			(4*NB) 
#define BLOCK_BSIZE			(4*NB) 

/* key is specified in bytes */
typedef uint8_t aeskey_t;

/* expanded key is specified in words */
typedef uint32_t expKey_t;

 
typedef union ExpKeyBuffer {
	
	#ifdef AES128_SUPPORT	
		expKey_t	aes128ExpKey[KSCH_AES128_SIZE];
	#endif
	#ifdef AES192_SUPPORT			
		expKey_t	aes192ExpKey[KSCH_AES192_SIZE];
	#endif
	#ifdef AES256_SUPPORT		
		expKey_t	aes256ExpKey[KSCH_AES256_SIZE];	
	#endif

} ExpKeyBuffer;




#endif



