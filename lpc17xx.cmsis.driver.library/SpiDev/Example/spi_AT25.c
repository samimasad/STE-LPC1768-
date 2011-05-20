/*----------------------------------------------------------------------------
 * Name:    SPI_AT25.c
 * Purpose: AT25 SPI EEPROM Driver
 * Version: V1.00
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

#include <stdint.h>                           /* Include standard types */

#include "SpiDev.h"

extern void SPI_SlaveSelect (unsigned char ss);
extern SpiDev_IOB *pSpi;

static unsigned char spiBuf[3];

/* SPI Instructions */
#define SPI_WREN        0x06
#define SPI_WRDI        0x04
#define SPI_RDSR        0x05
#define SPI_WRSR        0x01
#define SPI_READ        0x03
#define SPI_WRITE       0x02

/* Status Register Bits */
#define SR_NRDY         0x01
#define SR_WEN          0x02
#define SR_BP0          0x04
#define SR_BP1          0x08
#define SR_WPEN         0x80

#define PAGE_SZ         64


void SPI_MemRead (unsigned long adr, unsigned long sz, unsigned char *buf) {
//  unsigned int i;

  SPI_SlaveSelect(0);
  spiBuf[0] = SPI_READ;
  spiBuf[1] = (unsigned char)(adr >> 8);
  spiBuf[2] = (unsigned char)(adr >> 0);
  pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 3);
  
  pSpi->BufTxRx ((void *)buf, (void *)buf, sz);
//  for (i = 0; i < sz; i++) {
//    spiBuf[0] = 0xFF;
//    pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 1);
//    *buf++ = spiBuf[0];
//  }
  SPI_SlaveSelect(1);
}


void SPI_MemWrite (unsigned long adr, unsigned long sz, unsigned char *buf) {
  unsigned char sr;
  unsigned int  n;
//  unsigned int i;

  while (sz) {

    n = PAGE_SZ - (adr & (PAGE_SZ - 1));
    if (sz < n) n = sz;

    SPI_SlaveSelect(0);
    spiBuf[0] = SPI_WREN;
    pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 1);
    SPI_SlaveSelect(1);

    SPI_SlaveSelect(0);
    spiBuf[0] = SPI_WRITE;
    spiBuf[1] = (unsigned char)(adr >> 8);
    spiBuf[2] = (unsigned char)(adr >> 0);
    pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 3);

    pSpi->BufTxRx ((void *)buf, (void *)buf, sz);
//    for (i = 0; i < n; i++) {
//      spiBuf[0] = *buf++;
//      pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 1);
//    }
    SPI_SlaveSelect(1);

    do {
      SPI_SlaveSelect(0);
      spiBuf[0] = SPI_RDSR;
      spiBuf[1] = 0xFF;
      pSpi->BufTxRx ((void *)&spiBuf[0], (void *)&spiBuf[0], 2);
      sr = spiBuf[1];
      SPI_SlaveSelect(1);
    } while (sr & SR_NRDY);

    adr += n;
    sz  -= n;

  }
}
