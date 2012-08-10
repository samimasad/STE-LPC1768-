/**********************************************************************
* $Id$		nxp28536			2012-06-19
*//**
* @file		crypt_aes192.c
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

#ifdef AES192_SUPPORT	

void crypt_AES192(	const plainData_t plainText[PLAIN_BSIZE], 
					cryptData_t cryptedText[CRYPT_BSIZE], 
					expKey_t expKey[KSCH_AES192_SIZE]) {
					
					
					
}


#endif

