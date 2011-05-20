#include "diskio.h"
#include "usbhost_inc.h"
#include "lpc17xx_rtc.h"
#include "ff.h"
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
//static  BOOT_SEC    FAT_BootSec;
static USB_INT16U  boot_sig;
#define    FAT_16                   1
#define	   FAT_32					2
extern FATFS USBFs[_VOLUMES];		/* File system object for each logical drive */
/* 31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
							/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */
DWORD get_fattime ()
{
	RTC_TIME_Type RTCFullTime;
	RTC_GetFullTime (LPC_RTC, &RTCFullTime);


	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(RTCFullTime.YEAR  - 1980) << 25)
			| ((DWORD)RTCFullTime.MONTH << 21)
			| ((DWORD)RTCFullTime.DOM << 16)
			| ((DWORD)RTCFullTime.HOUR << 11)
			| ((DWORD)RTCFullTime.MIN << 5)
			| ((DWORD)RTCFullTime.SEC >> 1);
}


/*
**************************************************************************************************************
*                                         GET FILE SYSTEM TYPE
*
* Description: This function returns the file system type with which the disk is formatted
*
* Arguments  : None
*
* Returns    : FAT16           On Success
*              ERR_FAT_TYPE    On Failure
*
**************************************************************************************************************
*/

USB_INT08U  FAT_GetFATType_USB (void)
{
//    USB_INT08U  fat_type;
//    USB_INT32U  tot_sec;
//    USB_INT32U  tot_data_clus;

//
//    if (FAT_BootSec.TotSec16 != 0) {                             /* Get total sectors in the disk           */
//        tot_sec = FAT_BootSec.TotSec16;
//    } else {
//        tot_sec = FAT_BootSec.TotSec32;
//    }

//
//
//
//    tot_data_clus = tot_sec / (FAT_BootSec.SecPerClus);          /* Get total data clusters in the disk     */
//                                                                 /* If total data clusters >= 4085 and      */
//                                                                 /* < 65525, then it is FAT16 file system   */
//    if (tot_data_clus >= 4085 && tot_data_clus < 65525) {
//        fat_type = FAT_16;
//    } else {
//        fat_type = FAT_32;
//    }





    return (USBFs[USB].fs_type);
}


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                  */
/*-----------------------------------------------------------------------*/
DSTATUS USB_disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{


	CHAR debug_msg[100];
    USB_INT32S  rc = OK;
	USB_INT32U  numBlks, blkSize;
	USB_INT08U  inquiryResult[INQUIRY_LENGTH];

    USB_INT32S  i;
    static Bool host_init = FALSE ;
    //FILE_ENTRY	*entry;

    //only do it if the host is not being init before

    if(!host_init)
    {
    	host_init = TRUE;

	Host_Init();               /* Initialize the lpc17xx host controller                                    */
	    rc = Host_EnumDev();       /* Enumerate the device connected                                            */
	    if (rc == OK)
	    {
			/* Initialize the mass storage and scsi interfaces */
	        rc = MS_Init( &blkSize, &numBlks, inquiryResult );

	    }
    }


	MS_BulkRecv(0, 1, FATBuffer);

	RTC_Init(LPC_RTC);
	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Set current time for RTC */
	// Current time is 8:00:00PM, 2009-04-24
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 20);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 4);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2009);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 24);

#if 0
	boot_sig = ReadLE16U(&FATBuffer[510]);
	if (boot_sig != 0xAA55) {
		rc = ERR_INVALID_BOOT_SIG;
	} else {

		if (FATBuffer[0] != 0xEB && FATBuffer[0] != 0xE9) {
			FAT_BootSec.BootSecOffset = ReadLE32U(&FATBuffer[454]);
			MS_BulkRecv(FAT_BootSec.BootSecOffset, 1, FATBuffer);
		}
		FAT_BootSec.BytsPerSec = ReadLE16U(&FATBuffer[11]); /* Bytes per cluster              */
		FAT_BootSec.SecPerClus = FATBuffer[13]; /* Sectors per cluster            */
		/* Reserved sector count          */
		FAT_BootSec.RsvdSecCnt = ReadLE16U(&FATBuffer[14])
				+ FAT_BootSec.BootSecOffset;
		FAT_BootSec.NumFATs = FATBuffer[16]; /* Number of FAT copies           */
		FAT_BootSec.RootEntCnt = ReadLE16U(&FATBuffer[17]); /* Root entry count               */
		FAT_BootSec.TotSec16 = ReadLE16U(&FATBuffer[19]); /* Total FAT16 sectors            */
		FAT_BootSec.TotSec32 = ReadLE32U(&FATBuffer[32]); /* Total FAT32 sectors            */
		FAT_BootSec.FATSz16 = ReadLE16U(&FATBuffer[22]); /* Size of the FAT table          */
		/* Bytes per cluster              */
		FAT_BootSec.BytsPerClus = (FAT_BootSec.BytsPerSec
				* FAT_BootSec.SecPerClus);
		/* Root directory starting sector */
		FAT_BootSec.RootDirStartSec = FAT_BootSec.RsvdSecCnt
				+ (FAT_BootSec.FATSz16 * FAT_BootSec.NumFATs);
		/* Sectors occupied by root directory */
		FAT_BootSec.RootDirSec = ((FAT_BootSec.RootEntCnt * 32)
				+ (FAT_BootSec.BytsPerSec - 1)) / (FAT_BootSec.BytsPerSec);
		/* First data sector              */
		FAT_BootSec.FirstDataSec = FAT_BootSec.RootDirStartSec
				+ FAT_BootSec.RootDirSec;
		FAT_BootSec.FATType = FAT_GetFATType_USB(); /* Type of FAT                    */


		rc = OK ;
	}

#endif
	rc = OK ;
	return rc;


}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL != 0
DRESULT USB_disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{



	DRESULT res;
	BYTE n, *ptr = buff;
	res = RES_ERROR;

	switch (ctrl) {
	case CTRL_SYNC :		/* Make sure that no pending write process */
		if (MS_TestUnitReady() == OK)
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
#if 0
		if(USBFs[USB].fs_type == FAT_16)
		{
		*(WORD*)buff = FAT_BootSec.TotSec16;
		res = RES_OK;
		}else if(FAT_BootSec.FATType == FAT_32)
		{
		*(DWORD*)buff = FAT_BootSec.TotSec32;
		res = RES_OK;
		}
#endif
		res = RES_OK;
		*(WORD*)buff = (USBFs[USB].n_fatent - 2) * USBFs[USB].csize ;



		break;

	case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */

		//*(WORD*)buff = FAT_BootSec.BytsPerSec;
		*(WORD*)buff = _MAX_SS ;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		//*(WORD*)buff = FAT_BootSec.BytsPerClus;

		res = RES_OK;
		break;

	return res;
	}
}
#endif /* _USE_IOCTL != 0 */

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT USB_disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{

	DWORD rc;
	rc = MS_BulkRecv(sector, count, buff);

	return rc;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS USB_disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{


	if (drv) return STA_NOINIT;		/* Supports only single drive */
	if(boot_sig != 0xAA55)
		return Stat;
	else
		return OK;
}

/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */
void USB_disk_timerproc (void)
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
DRESULT USB_disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	MS_BulkSend(sector,count,buff);



	return RES_OK;

}
#endif /* _READONLY == 0 */

/* Initialize SD/MMC card. */
