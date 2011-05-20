#include "diskio.h"
#include "LPC17xx_spi.h"
#include "LPC17xx_sd.h"

/* 512 bytes for each sector */
#define SD_SECTOR_SIZE	512

/* token for write operation */
#define TOKEN_SINGLE_BLOCK	0xFE
#define TOKEN_MULTI_BLOCK	0xFC
#define TOKEN_STOP_TRAN		0xFD

/* Local variables */
static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */
static volatile	BYTE Timer1, Timer2;	/* 100Hz decrement timer */
static uint8_t CardType;
static SDCFG SDCfg;

/* Local Function Prototypes */
static Bool 	LPC17xx_SD_Init (void);
static uint8_t  LPC17xx_SD_SendCmd     (uint8_t cmd, uint32_t arg);
static Bool 	LPC17xx_SD_ReadSector (uint32_t sector, uint8_t *buff, uint32_t count);
static Bool 	LPC17xx_SD_ReadDataBlock ( uint8_t *buff,	uint32_t cnt);
static Bool 	LPC17xx_SD_WriteSector (uint32_t sector, const uint8_t *buff, uint32_t count);
static Bool 	LPC17xx_SD_WirteDataBlock (const uint8_t *buff, uint8_t token);
static Bool 	LPC17xx_SD_ReadCfg (SDCFG *cfg);
static Bool 	LPC17xx_SD_WaitForReady (void);

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                  */
/*-----------------------------------------------------------------------*/
DSTATUS MMC_disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;			/* Supports only single drive */
//	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	if (LPC17xx_SD_Init() && LPC17xx_SD_ReadCfg(&SDCfg))
		Stat &= ~STA_NOINIT;

	return Stat;
}
/* SSEL: low */
void LPC17xx_SPI_Select ()
{
	LPC_GPIO0->FIOPIN &= ~(1<<16);
}
/* SSEL: high */
void LPC17xx_SPI_DeSelect ()
{
	LPC_GPIO0->FIOPIN |= (1<<16);
}
/* Release SSP0 */
void LPC17xx_SPI_Release (void)
{
	LPC17xx_SPI_DeSelect ();
	SPI_ReceiveData(LPC_SPI);
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL != 0
DRESULT MMC_disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, *ptr = buff;

	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {
	case CTRL_SYNC :		/* Make sure that no pending write process */
		if (LPC17xx_SD_WaitForReady() == TRUE)
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
		*(DWORD*)buff = SDCfg.sectorcnt;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
		*(WORD*)buff = SDCfg.sectorsize;	//512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		*(DWORD*)buff = SDCfg.blocksize;
		res = RES_OK;
		break;

	case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
		*ptr = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = SDCfg.csd[n]; 
		res = RES_OK;
		break;

	case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
		for (n=0;n<16;n++)
			*(ptr+n) = SDCfg.cid[n];
		res = RES_OK;
		break;

	case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
		for (n=0;n<4;n++)
			*(ptr+n) = SDCfg.ocr[n];
		res = RES_OK;
		break;

	case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
		if (LPC17xx_SD_SendCmd(SD_STATUS, 0) == 0) {	/* SD_STATUS */
			//LPC17xx_SPI_RecvByte();
			SPI_ReceiveData(LPC_SPI);
			if (LPC17xx_SD_ReadDataBlock(ptr, 64))
				res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	LPC17xx_SPI_Release ();
	return res;
}
#endif /* _USE_IOCTL != 0 */

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT MMC_disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (LPC17xx_SD_ReadSector (sector, buff, count) == TRUE)
		return RES_OK;
	else
		return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS MMC_disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */
void disk_timerproc (void)
{
	BYTE n;

	n = Timer1;						/* 100Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _READONLY == 0
DRESULT MMC_disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
//	if (Stat & STA_PROTECT) return RES_WRPRT;

	if ( LPC17xx_SD_WriteSector(sector, buff, count) == TRUE)
		return RES_OK;
	else
		return 	RES_ERROR;

}
#endif /* _READONLY == 0 */

/* wait until the card is not busy */
static Bool LPC17xx_SD_WaitForReady (void)
{
	uint8_t res;
	/* timeout should be large enough to make sure the write operaion can be completed. */
	uint32_t timeout = 400000;

	//LPC17xx_SPI_SendByte(0xFF);
	SPI_SendData(LPC_SPI, 0xFF);
	do {
		//res = LPC17xx_SPI_RecvByte();
		res = SPI_ReceiveData(LPC_SPI);
	} while ((res != 0xFF) && timeout--);

	return (res == 0xFF ? TRUE : FALSE);
}

/* Initialize SD/MMC card. */
static Bool LPC17xx_SD_Init (void)
{
	uint32_t i, timeout;
	uint8_t cmd, ct, ocr[4];
	Bool ret = FALSE;
	SPI_CFG_Type SPI_InitStruct;
	/* Initialize SPI interface and enable Flash Card SPI mode. */
	SPI_ConfigStructInit(&SPI_InitStruct);
	SPI_InitStruct.ClockRate = 20000000;
	SPI_Init(LPC_SPI,&SPI_InitStruct);

	/* At least 74 clock cycles are required prior to starting bus communication */
	for (i = 0; i < 80; i++) {	   /* 80 dummy clocks */
		//LPC17xx_SPI_SendByte (0xFF);
		SPI_SendData(LPC_SPI, 0xFF);
	}

	ct = CT_NONE;
	if (LPC17xx_SD_SendCmd (GO_IDLE_STATE, 0) == 0x1)
	{
		timeout = 50000;
		if (LPC17xx_SD_SendCmd(CMD8, 0x1AA) == 1) {	/* SDHC */
			/* Get trailing return value of R7 resp */
			//for (i = 0; i < 4; i++) ocr[i] = LPC17xx_SPI_RecvByte();
			for (i = 0; i < 4; i++) ocr[i] = SPI_ReceiveData(LPC_SPI);
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {	/* The card can work at vdd range of 2.7-3.6V */
				/* Wait for leaving idle state (ACMD41 with HCS bit) */
				while (timeout-- && LPC17xx_SD_SendCmd(SD_SEND_OP_COND, 1UL << 30));	
				/* Check CCS bit in the OCR */
				if (timeout && LPC17xx_SD_SendCmd(READ_OCR, 0) == 0) {		
					//for (i = 0; i < 4; i++) ocr[i] = LPC17xx_SPI_RecvByte();
					for (i = 0; i < 4; i++) ocr[i] = SPI_ReceiveData(LPC_SPI);
					ct = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			} else {  /* SDSC or MMC */
				if (LPC17xx_SD_SendCmd(SD_SEND_OP_COND, 0) <= 1) 	{
					ct = CT_SD1; cmd = SD_SEND_OP_COND;	/* SDSC */
				} else {
					ct = CT_MMC; cmd = SEND_OP_COND;	/* MMC */
				}
				/* Wait for leaving idle state */
				while (timeout-- && LPC17xx_SD_SendCmd(cmd, 0));			
				/* Set R/W block length to 512 */
				if (!timeout || LPC17xx_SD_SendCmd(SET_BLOCKLEN, SD_SECTOR_SIZE) != 0)	
					ct = CT_NONE;				
			}
		}
	}
	CardType = ct;
	LPC17xx_SPI_Release();

#if 0
	if (ct) {			/* Initialization succeeded */
		ret = TRUE;
		if ( ct == CT_MMC ) {
			LPC17xx_SPI_SetSpeed(SPI_SPEED_20MHz);
		} else {
			LPC17xx_SPI_SetSpeed(SPI_SPEED_20MHz);
		}
	} else {			/* Initialization failed */
		LPC17xx_SPI_Select ();
		LPC17xx_SD_WaitForReady ();
		LPC17xx_SPI_DeInit();
	}
#endif
	return ret;
}

/*****************************************************************************
	Send a Command to Flash card and get a Response
	cmd:  cmd index
	arg: argument for the cmd
	return the received response of the commond
*****************************************************************************/
static uint8_t LPC17xx_SD_SendCmd (uint8_t cmd, uint32_t arg) 
{
	uint32_t r1, n;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		r1 = LPC17xx_SD_SendCmd(APP_CMD, 0);   /* CMD55 */
		if (r1 > 1) return r1; /* cmd send failed */
	}

	/* Select the card and wait for ready */
	LPC17xx_SPI_DeSelect();
	LPC17xx_SPI_Select();
	if (LPC17xx_SD_WaitForReady() == FALSE ) return 0xFF;

//	LPC17xx_SPI_SendByte (0xFF);	 /* prepare 8 clocks */
//	LPC17xx_SPI_SendByte (cmd);
//	LPC17xx_SPI_SendByte (arg >> 24);
//	LPC17xx_SPI_SendByte (arg >> 16);
//	LPC17xx_SPI_SendByte (arg >> 8);
//	LPC17xx_SPI_SendByte (arg);

	SPI_SendData(LPC_SPI, 0xFF);
	SPI_SendData(LPC_SPI, cmd);
	SPI_SendData(LPC_SPI, arg >> 24);
	SPI_SendData(LPC_SPI, arg >> 16);
	SPI_SendData(LPC_SPI, arg >> 8);
	SPI_SendData(LPC_SPI, arg);

   /* Checksum, should only be valid for the first command.CMD0 */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == GO_IDLE_STATE) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	//LPC17xx_SPI_SendByte(n);
	SPI_SendData(LPC_SPI,n);

	//if (cmd == STOP_TRAN) LPC17xx_SPI_RecvByte ();		/* Skip a stuff byte when stop reading */
	if (cmd == STOP_TRAN) SPI_ReceiveData(LPC_SPI);

	n = 10;		/* Wait for a valid response in timeout of 10 attempts */
	do {
		//r1 = LPC17xx_SPI_RecvByte ();
		r1 = SPI_ReceiveData(LPC_SPI);
	} while ((r1 & 0x80) && --n);

	return (r1);		/* Return with the response value */
}

/*****************************************************************************
	Read "count" Sector(s) starting from sector index "sector",
	buff <- [sector, sector+1, ... sector+count-1]
	if success, return true, otherwise return false
*****************************************************************************/ 
static Bool LPC17xx_SD_ReadSector (uint32_t sector, uint8_t *buff, uint32_t count)
{
	/* Convert to byte address if needed */
	if (!(CardType & CT_BLOCK)) sector *= SD_SECTOR_SIZE;	

	if (count == 1) {	/* Single block read */
		if ((LPC17xx_SD_SendCmd(READ_BLOCK, sector) == 0)
			&& LPC17xx_SD_ReadDataBlock(buff, SD_SECTOR_SIZE))
			count = 0;
	} else {				/* Multiple block read */
		if (LPC17xx_SD_SendCmd(READ_MULT_BLOCK, sector) == 0) {
			do {
				if (!LPC17xx_SD_ReadDataBlock(buff, SD_SECTOR_SIZE)) break;
				buff += SD_SECTOR_SIZE;
			} while (--count);
			LPC17xx_SD_SendCmd(STOP_TRAN, 0);	/* STOP_TRANSMISSION */
		}
	}
	LPC17xx_SPI_Release();

	return count ? FALSE : TRUE;
}

/*****************************************************************************
	read specified number of data to specified buffer.
	buff:  Data buffer to store received data
	cnt:   Byte count (must be multiple of 4, normally 512)
*****************************************************************************/
static Bool LPC17xx_SD_ReadDataBlock ( uint8_t *buff,	uint32_t cnt)
{
	uint8_t token;
	uint32_t timeout;
	
	timeout = 20000;
	do {							/* Wait for data packet in timeout of 100ms */
		//token = LPC17xx_SPI_RecvByte();
		token = SPI_ReceiveData(LPC_SPI);

	} while ((token == 0xFF) && timeout--);
	if(token != 0xFE) return FALSE;	/* If not valid data token, return with error */

#if USE_FIFO
	LPC17xx_SPI_RecvBlock_FIFO (buff, cnt);
#else
	do {	/* Receive the data block into buffer */
		/**buff++ = LPC17xx_SPI_RecvByte ();
		*buff++ = LPC17xx_SPI_RecvByte ();
		*buff++ = LPC17xx_SPI_RecvByte ();
		*buff++ = LPC17xx_SPI_RecvByte ();*/

		*buff++ = SPI_ReceiveData(LPC_SPI);
		*buff++ = SPI_ReceiveData(LPC_SPI);
		*buff++ = SPI_ReceiveData(LPC_SPI);
		*buff++ = SPI_ReceiveData(LPC_SPI);
	} while (cnt -= 4);
#endif /* USE_FIFO */

	//LPC17xx_SPI_RecvByte ();						/* Discard CRC */
	//LPC17xx_SPI_RecvByte ();

	SPI_ReceiveData(LPC_SPI);
	SPI_ReceiveData(LPC_SPI);

	return TRUE;					/* Return with success */
}

#if _READONLY == 0
/*****************************************************************************
	Write "count" Sector(s) starting from sector index "sector",
	buff -> [sector, sector+1, ... sector+count-1]
	if success, return true, otherwise return false
*****************************************************************************/  
static Bool LPC17xx_SD_WriteSector (uint32_t sector, const uint8_t *buff, uint32_t count)
{
	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */	

	if (count == 1) {	/* Single block write */
		if ((LPC17xx_SD_SendCmd(WRITE_BLOCK, sector) == 0)
			&& LPC17xx_SD_WirteDataBlock(buff, TOKEN_SINGLE_BLOCK))
			count = 0;
	} else {				/* Multiple block write */
		if (CardType & CT_SDC) LPC17xx_SD_SendCmd(SET_WR_BLK_ERASE_COUNT, count);
		if (LPC17xx_SD_SendCmd(WRITE_MULT_BLOCK, sector) == 0) {
			do {
				if (!LPC17xx_SD_WirteDataBlock(buff, TOKEN_MULTI_BLOCK)) break;
				buff += 512;
			} while (--count);
		#if 1
			if (!LPC17xx_SD_WirteDataBlock(0, TOKEN_STOP_TRAN))	/* STOP_TRAN token */
				count = 1;
		#else
			LPC17xx_SPI_SendByte(TOKEN_STOP_TRAN);
		#endif
		}
	}
	LPC17xx_SPI_Release(); 
	return count ? FALSE : TRUE;
}

/*****************************************************************************
	Write 512 bytes
	buffer: 512 byte data block to be transmitted
	token:  0xFE -> single block
			0xFC -> multi block
			0xFD -> Stop
*****************************************************************************/ 
static Bool LPC17xx_SD_WirteDataBlock (const uint8_t *buff, uint8_t token)
{
	uint8_t resp, i;

	i = i; // avoid warning

	//LPC17xx_SPI_SendByte (token);		/* send data token first*/
	SPI_SendData(LPC_SPI, token);

	if (token != TOKEN_STOP_TRAN) {
#if USE_FIFO
		LPC17xx_SPI_SendBlock_FIFO (buff);
#else
      /* Send data. */
      for (i = 512/4; i ; i--) { 
//         LPC17xx_SPI_SendByte (*buff++);
//		 LPC17xx_SPI_SendByte (*buff++);
//		 LPC17xx_SPI_SendByte (*buff++);
//		 LPC17xx_SPI_SendByte (*buff++);
    	  SPI_SendData(LPC_SPI, *buff++);
    	  SPI_SendData(LPC_SPI, *buff++);
    	  SPI_SendData(LPC_SPI, *buff++);
    	  SPI_SendData(LPC_SPI, *buff++);


      }
#endif /* USE_FIFO */
//		LPC17xx_SPI_SendByte(0xFF);					/* 16-bit CRC (Dummy) */
//		LPC17xx_SPI_SendByte(0xFF);
	  SPI_SendData(LPC_SPI, 0xFF);
      SPI_SendData(LPC_SPI, 0xFF);

		//resp = LPC17xx_SPI_RecvByte();				/* Receive data response */
		resp = SPI_ReceiveData(LPC_SPI);
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;

		if ( LPC17xx_SD_WaitForReady() == FALSE)  /* Wait while Flash Card is busy. */
			return FALSE;
	}

	return TRUE;
}
#endif /* _READONLY */

/* Read MMC/SD Card device configuration. */
static Bool LPC17xx_SD_ReadCfg (SDCFG *cfg)
{	   
	uint8_t i;	
	uint16_t csize;
	uint8_t n, csd[16];
	Bool retv = FALSE;
	
	/* Read the OCR - Operations Condition Register. */
	if (LPC17xx_SD_SendCmd (READ_OCR, 0) != 0x00) goto x;
	//for (i = 0; i < 4; i++)	cfg->ocr[i] = LPC17xx_SPI_RecvByte ();
	for (i = 0; i < 4; i++)	cfg->ocr[i] = SPI_ReceiveData(LPC_SPI);
  	
	/* Read the CID - Card Identification. */
	if ((LPC17xx_SD_SendCmd (SEND_CID, 0) != 0x00) || 
		(LPC17xx_SD_ReadDataBlock (cfg->cid, 16) == FALSE))
		goto x;

    /* Read the CSD - Card Specific Data. */
	if ((LPC17xx_SD_SendCmd (SEND_CSD, 0) != 0x00) || 
		(LPC17xx_SD_ReadDataBlock (cfg->csd, 16) == FALSE))
		goto x;

	cfg -> sectorsize = SD_SECTOR_SIZE;

	/* Get number of sectors on the disk (DWORD) */
	if ((cfg->csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
		csize = cfg->csd[9] + ((uint16_t)cfg->csd[8] << 8) + 1;
		cfg -> sectorcnt = (uint32_t)csize << 10;
	} else {					/* SDC ver 1.XX or MMC*/
		n = (cfg->csd[5] & 15) + ((cfg->csd[10] & 128) >> 7) + ((cfg->csd[9] & 3) << 1) + 2;  // 19
		csize = (cfg->csd[8] >> 6) + ((uint16_t)cfg->csd[7] << 2) + ((uint16_t)(cfg->csd[6] & 3) << 10) + 1; // 3752
		cfg -> sectorcnt = (uint32_t)csize << (n - 9); // 3842048
	}

	cfg->size = cfg -> sectorcnt * cfg -> sectorsize; // 512*3842048=1967128576Byte (1.83GB)

	/* Get erase block size in unit of sector (DWORD) */
	if (CardType & CT_SD2) {			/* SDC ver 2.00 */
		if (LPC17xx_SD_SendCmd(SD_STATUS /*ACMD13*/, 0) == 0) {		/* Read SD status */
			//LPC17xx_SPI_RecvByte();
			SPI_ReceiveData(LPC_SPI);
			if (LPC17xx_SD_ReadDataBlock(csd, 16)) {				/* Read partial block */
				//for (n = 64 - 16; n; n--) LPC17xx_SPI_RecvByte();	/* Purge trailing data */
				for (n = 64 - 16; n; n--) SPI_ReceiveData(LPC_SPI);
				cfg->blocksize = 16UL << (csd[10] >> 4);
				retv = TRUE;
			}
		}
	} else {					/* SDC ver 1.XX or MMC */
		if ((LPC17xx_SD_SendCmd(SEND_CSD, 0) == 0) && LPC17xx_SD_ReadDataBlock(csd, 16)) {	/* Read CSD */
			if (CardType & CT_SD1) {			/* SDC ver 1.XX */
				cfg->blocksize = (((csd[10] & 63) << 1) + ((uint16_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
			} else {					/* MMC */
				// cfg->blocksize = ((uint16_t)((buf[10] & 124) >> 2) + 1) * (((buf[11] & 3) << 3) + ((buf[11] & 224) >> 5) + 1);
				cfg->blocksize = ((uint16_t)((cfg->csd[10] & 124) >> 2) + 1) * (((cfg->csd[10] & 3) << 3) + ((cfg->csd[11] & 224) >> 5) + 1);
			}
			retv = TRUE;
		}
	}

x:	LPC17xx_SPI_Release();
    return (retv);
}

