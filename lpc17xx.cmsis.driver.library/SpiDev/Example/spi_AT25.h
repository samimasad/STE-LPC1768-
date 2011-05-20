/*----------------------------------------------------------------------------
 * Name:    SPI_AT25.h
 * Purpose: AT25 SPI EEPROM prototypes
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#ifndef _SPI_AT25_H_
#define _SPI_AT25_H_

// SPI Memory
extern void SPI_MemRead (unsigned long adr, unsigned long sz, unsigned char *buf);
extern void SPI_MemWrite(unsigned long adr, unsigned long sz, unsigned char *buf);

#endif
