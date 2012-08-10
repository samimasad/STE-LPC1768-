/**********************************************************************
* @file		aes_256.h
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
#ifndef __AES_256__H
#define __AES_256__H

/******************************************************************************
 * AES 256 functions
 *****************************************************************************/
void genAes256ExpKey(const aeskey_t key[NK_AES256], expKey_t expKey[KSCH_AES256_SIZE]);

void crypt_AES256(	const plainData_t plainText[PLAIN_BSIZE], 
					cryptData_t cryptedText[CRYPT_BSIZE], 
					expKey_t expKey[KSCH_AES256_SIZE]);
					
void deCrypt_AES256(const cryptData_t cryptedText[CRYPT_BSIZE],
					plainData_t plainText[PLAIN_BSIZE], 
					expKey_t expKey[KSCH_AES256_SIZE]);



#endif
