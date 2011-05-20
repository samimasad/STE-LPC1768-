/*
 * @file:    EthDev_LPC17xx.c
 * @purpose: Ethernet Device Functions for NXP LPC17xx
 * @version: V0.01
 * @date:    14. May 2009
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M3
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */


#include <LPC17xx.h>
#include "EthDev.h"
#include "EthDev_LPC17xx.h"

/*----------------------------------------------------------------------------
  Ethernet Device local functions
 *----------------------------------------------------------------------------*/
static int           EthDev_Init        (void);
static int           EthDev_UnInit      (void);
static int           EthDev_SetMCFilter (int NumHwAddr, unsigned char *pHwAddr);
static int           EthDev_TxFrame     (void *pData, int size);
static void          EthDev_Lock        (void);
static void          EthDev_UnLock      (void);
static EthDev_STATUS EthDev_LinkChk     (void);

static void *        EthDev_RxFrame     (int size);
static void          EthDev_RxFrameReady(int size);

static int           phy_Rd             (unsigned int PhyReg);
static int           phy_Wr             (unsigned int PhyReg, unsigned short Data);

static void          TxDscrInit         (void);
static void          RxDscrInit         (void);


/*----------------------------------------------------------------------------
  Ethernet Device IO Block
 *----------------------------------------------------------------------------*/

#define ETHDEV     eth0

EthDev_IOB ETHDEV = { 
                      EthDev_MODE_AUTO,
                      0,0,0,0,0,0,
                      EthDev_RxFrame,      /* Place holder, initialized by the user */
                      EthDev_RxFrameReady, /* Place holder, initialized by the user */

                      EthDev_Init,
                      EthDev_UnInit,
                      EthDev_SetMCFilter,
                      EthDev_TxFrame,
                      EthDev_Lock,
                      EthDev_UnLock,
                      EthDev_LinkChk
                    };

static EthDev_STATUS Status;


uint32_t TxErrCnt = 0;                                 /* testing purpose */
uint32_t RxErrCnt = 0; 								   /* testing purpose */
uint16_t phyId1, phyId2;							   /* testing purpose */


/*----------------------------------------------------------------------------
  Ethernet Device place holder function for user callback 
 *----------------------------------------------------------------------------*/
static void *EthDev_RxFrame (int size) {
  return (NULL);                         /* do nothing */
}


/*----------------------------------------------------------------------------
  Ethernet Device place holder function for user callback 
 *----------------------------------------------------------------------------*/
static void EthDev_RxFrameReady (int size) {
  ;                                      /* do nothing */
}


/*----------------------------------------------------------------------------
  Ethernet Device initialize
 *----------------------------------------------------------------------------*/
static int EthDev_Init (void) {
  int regv, tout;

  NVIC_DisableIRQ(ENET_IRQn);             /* Disable ETH Interrupt */

  SC->PCONP |= 0x40000000;                /* Power Up the EMAC controller. */

   
  PINCON->PINSEL2 = 0x50150105;           /* Enable P1 Ethernet Pins. */
  PINCON->PINSEL3 = (PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

   /* Reset all EMAC internal modules. */
  EMAC->MAC1    = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | 
                  MAC1_RES_MCS_RX | MAC1_SIM_RES | MAC1_SOFT_RES;
  EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;

  for (tout = 100; tout; tout--);         /* A short delay after reset. */

   EMAC->MAC1 = MAC1_PASS_ALL;            /* Initialize MAC control registers. */
   EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
   EMAC->MAXF = ETH_MAX_FLEN;
   EMAC->CLRT = CLRT_DEF;
   EMAC->IPGR = IPGR_DEF;

   EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM;  /* Enable Reduced MII interface. */

   
   EMAC->SUPP = SUPP_RES_RMII;           /* Reset Reduced MII Logic. */
   for (tout = 100; tout; tout--);       /* A short delay */
   EMAC->SUPP = 0;


  phy_Wr (PHY_REG_BMCR, 0x8000);          /* perform PHY reset */
  for (tout = 0x20000; ; tout--) {        /* Wait for hardware reset to end. */
    regv = phy_Rd (PHY_REG_BMCR);
    if (regv < 0 || tout == 0) {
       return (-1);                       /* Error */
    }
    if (!(regv & 0x8000)) {
       break;                            /* Reset complete. */
    }
  }

  phyId1 = phy_Rd (PHY_REG_IDR1);
  phyId2 = phy_Rd (PHY_REG_IDR2);

  switch (ETHDEV.Mode) {
    case EthDev_MODE_AUTO:
      phy_Wr (PHY_REG_BMCR, PHY_AUTO_NEG);
      break;

    case EthDev_MODE_10M_FULL:
      phy_Wr (PHY_REG_BMCR, PHY_FULLD_10M);
      break;

    case EthDev_MODE_10M_HALF:
      phy_Wr (PHY_REG_BMCR, PHY_HALFD_10M);
      break;

    case EthDev_MODE_100M_FULL:
      phy_Wr (PHY_REG_BMCR, PHY_FULLD_100M);
      break;

    case EthDev_MODE_100M_HALF:
      phy_Wr (PHY_REG_BMCR, PHY_HALFD_100M);
      break;

    default:
       return (-1);
  }

  /* Set the Ethernet MAC Address registers */
  EMAC->SA0 = ((uint32_t)ETHDEV.HwAddr[5] << 8) | (uint32_t)ETHDEV.HwAddr[4];
  EMAC->SA1 = ((uint32_t)ETHDEV.HwAddr[3] << 8) | (uint32_t)ETHDEV.HwAddr[2];
  EMAC->SA2 = ((uint32_t)ETHDEV.HwAddr[1] << 8) | (uint32_t)ETHDEV.HwAddr[0];

  TxDscrInit();                                         /* initialize DMA TX Descriptor */
  RxDscrInit();                                         /* initialize DMA RX Descriptor */

  EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;   /* Receive Broadcast, Perfect Match Packets */

  EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;          /* Enable EMAC interrupts. */
  EMAC->IntClear  = 0xFFFF;                             /* Reset all interrupts */

   
  EMAC->Command  |= (CR_RX_EN | CR_TX_EN);              /* Enable receive and transmit mode of MAC Ethernet core */
  EMAC->MAC1     |= MAC1_REC_EN;

  NVIC_EnableIRQ(ENET_IRQn);                            /* Enable ETH Interrupt */

  Status.Link = EthDev_LINK_DOWN;
  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Uninitialize
 *----------------------------------------------------------------------------*/
static int EthDev_UnInit (void) {

  NVIC_DisableIRQ(ENET_IRQn);                           /* Disable ETH Interrupt */

  EMAC->IntEnable &= ~(INT_RX_DONE | INT_TX_DONE);
  EMAC->IntClear   =  0xFFFF;

  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Interrupt Disable
 *----------------------------------------------------------------------------*/
static void EthDev_Lock (void) {
  NVIC_DisableIRQ(ENET_IRQn);                           /* Disable ETH Interrupt */
}


/*----------------------------------------------------------------------------
  Ethernet Device Interrupt Enable
 *----------------------------------------------------------------------------*/
static void EthDev_UnLock (void) {
  NVIC_EnableIRQ(ENET_IRQn);                           /* Enable ETH Interrupt */
}


/*----------------------------------------------------------------------------
  Set Multicast Filter Address to ethernet controller 
 *----------------------------------------------------------------------------*/
static int EthDev_SetMCFilter (int NumHwAddr, unsigned char *pHwAddr) {

   EMAC->RxFilterCtrl |= RFC_MCAST_EN;                /* to do: add code for multicast support */

  return 0;
}


/*----------------------------------------------------------------------------
  Send frame to ENET ethernet controller 
 *----------------------------------------------------------------------------*/
static int EthDev_TxFrame (void *pData, int size) {
  uint32_t *pDst, *pSrc; 
  uint32_t  idx, i;


  idx = EMAC->TxProduceIndex;
  pDst = (uint32_t *)TX_DESC(idx)->Packet;
  pSrc = (uint32_t *)pData;
  for (i = 0; i < (size+3)/4; i++) {
    *pDst++ = *pSrc++;
  }    	 

  TX_DESC(idx)->Ctrl = (size-1) | (TCTRL_INT | TCTRL_LAST);

  if (++idx == NUM_TX_FRAG) idx = 0;
  EMAC->TxProduceIndex = idx;                           /* Start frame transmission. */

  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Check the Link
 *----------------------------------------------------------------------------*/
static EthDev_STATUS EthDev_LinkChk (void) {
  int phyData;

  phyData = phy_Rd (PHY_REG_STS);
  if (phyData < 0) {
    return (Status);
  }

  if (Status.Link == EthDev_LINK_DOWN) {
    if (phyData & 0x0004) {
      Status.Link = EthDev_LINK_UP;
      phyData = phy_Rd (PHY_REG_STS);
      if (phyData < 0) {
        return (Status);
      }
      Status.Duplex = (phyData & 0x0004) ? EthDev_DUPLEX_FULL : EthDev_DUPLEX_HALF;
      Status.Speed  = (phyData & 0x0002) ? EthDev_SPEED_10M  : EthDev_SPEED_100M;
      if (Status.Speed  == EthDev_SPEED_100M)  {
        EMAC->SUPP = SUPP_SPEED;                        /* 100MBit mode enabled. */
      } else {
        EMAC->SUPP = 0;                                 /* 10 MBit mode enabled. */
      }
      if (Status.Duplex == EthDev_DUPLEX_FULL) {
        EMAC->MAC2    |= MAC2_FULL_DUP;                 /* Full duplex is enabled. */
        EMAC->Command |= CR_FULL_DUP;
        EMAC->IPGT     = IPGT_FULL_DUP;
      } else {
        EMAC->IPGT = IPGT_HALF_DUP;                     /* Half duplex mode. */
      }
    }
  }
  else {
    if ((phyData & 0x0004) == 0) {
      Status.Link   = EthDev_LINK_DOWN;
    }
  }
  return (Status);
}


/*----------------------------------------------------------------------------
  Ethernet Device Ethernet Controller Interrupt function.
  (not static because of Startup.s) 
 *----------------------------------------------------------------------------*/
void ENET_IRQHandler (void) {
  uint32_t  intStatus;
  int       RxLen;
  uint32_t  idx, info, i;
  uint32_t *pDst, *pSrc; 

//  intStatus = (EMAC->IntStatus & EMAC->IntEnable);
  while ((intStatus = (EMAC->IntStatus & EMAC->IntEnable)) != 0) {
    EMAC->IntClear = intStatus;
    if (intStatus & INT_RX_DONE) {                      /* Packet received, check if packet is valid. */
      idx = EMAC->RxConsumeIndex;
//      if (idx != EMAC->RxProduceIndex) {
      while (idx != EMAC->RxProduceIndex) {
        info = RX_STAT(idx)->Info;
        if (!(info & RINFO_LAST_FLAG)) {
          goto rel;
        }

        RxLen = (info & RINFO_SIZE) - 3;
        if (RxLen > EthDev_MTU_SIZE || (info & RINFO_ERR_MASK)) {/* Invalid frame, ignore it and free buffer. */
          RxErrCnt++;                                    /* ToDo: Error needs to be handled */
          goto rel;
        }
        pSrc = (uint32_t *)RX_DESC(idx)->Packet;
        pDst = (uint32_t *)ETHDEV.RxFrame (RxLen);       /* get user buffer */
        if (pDst != NULL) {                              /* check if Buffer available */
          for (i = 0; i < (RxLen+3)/4; i++) {
            *pDst++ = *pSrc++;
          }
          ETHDEV.RxFrameReady (RxLen);                   /* call RxFrameReady() if provided */
        }
rel:    if (++idx == NUM_RX_FRAG) idx = 0;               /* Release frame from EMAC buffer. */
        EMAC->RxConsumeIndex = idx;
      }
    }
    
    if (intStatus & INT_TX_DONE) {                      /* Frame transmit completed. */
      ;                                                 /* nothing to do */
    }
  }

}


/*----------------------------------------------------------------------------
  Write a data 'Data' to PHY register 'PhyReg'.  
 *----------------------------------------------------------------------------*/
static int phy_Wr (unsigned int PhyReg, unsigned short Data) {
  unsigned int timeOut;

  EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  EMAC->MWTD = Data;

  for (timeOut = 0; timeOut < MII_WR_TOUT; timeOut++) { /* Wait until operation completed */
    if ((EMAC->MIND & MIND_BUSY) == 0) {
      return (0);
    }
  }

  return (-1);
}


/*----------------------------------------------------------------------------
  Read a PHY register 'PhyReg'. 
 *----------------------------------------------------------------------------*/
static int phy_Rd (unsigned int PhyReg) {
  unsigned int timeOut;

  EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  EMAC->MCMD = MCMD_READ;

  for (timeOut = 0; timeOut < MII_RD_TOUT; timeOut++) { /* Wait until operation completed */
    if ((EMAC->MIND & MIND_BUSY) == 0) {
      EMAC->MCMD = 0;
      return (EMAC->MRDD);                              /* Return a 16-bit value. */
    }
  }

  return (-1);
}


/*----------------------------------------------------------------------------
  Initialize Tx DMA Descriptors (chained mode)
 *----------------------------------------------------------------------------*/
static void TxDscrInit (void) {
  int i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    TX_DESC(i)->Packet = (uint32_t)TX_BUF(i);
    TX_DESC(i)->Ctrl   = 0;
    TX_STAT(i)->Info   = 0;
  }

  EMAC->TxDescriptor       = (uint32_t)TX_DESC(0);    /* Set EMAC Transmit Descriptor Registers. */
  EMAC->TxStatus           = (uint32_t)TX_STAT(0);
  EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  EMAC->TxProduceIndex  = 0;                           /* Tx Descriptors Point to 0 */
}


/*----------------------------------------------------------------------------
  Initialize Rx DMA Descriptors (chained mode)
 *----------------------------------------------------------------------------*/
static void RxDscrInit (void) {
  int i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC(i)->Packet  = (uint32_t)RX_BUF(i);
    RX_DESC(i)->Ctrl    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT(i)->Info    = 0;
    RX_STAT(i)->HashCRC = 0;
  }

  EMAC->RxDescriptor       = (uint32_t)RX_DESC(0);    /* Set EMAC Receive Descriptor Registers. */
  EMAC->RxStatus           = (uint32_t)RX_STAT(0);
  EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  EMAC->RxConsumeIndex  = 0;                            /* Rx Descriptors Point to 0 */
}
