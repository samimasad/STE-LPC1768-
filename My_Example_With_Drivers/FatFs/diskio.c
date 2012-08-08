/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "lpc17xx_sd.h"
#include "usbhost_lpc17xx.h"


#define TRUE 1
#define FALSE 0



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT;
	int result;
	PRINT_Log("disk_initialize for disk %x \n\r",drv);

	switch (drv) {
	case ATA :
		//result = ATA_disk_initialize();
		// translate the reslut code here

		return stat;

	case MMC :
		result = MMC_disk_initialize(0);
		// translate the reslut code here
		stat = RES_OK ;
		driveAttached = TRUE ;
		return stat;

	case USB :
		result = USB_disk_initialize();
		// translate the reslut code here
		stat = RES_OK ;
		driveAttached = TRUE ;
		return stat;
	}
	PRINT_Log("disk_initialize return problem");
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat = STA_NOINIT ;
	int result;

	switch (drv) {
	case ATA :
		//result = ATA_disk_status();
		// translate the reslut code here

		return stat;

	case MMC :
		stat = MMC_disk_status(0);
		// translate the reslut code here

		return stat;

	case USB :
		stat = USB_disk_status();
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	DRESULT res;


	switch (drv) {
	case ATA :
		//result = ATA_disk_read(buff, sector, count);
		// translate the reslut code here

		return res;

	case MMC :
		res = MMC_disk_read(0,buff, sector, count);
		// translate the reslut code here

		return res;

	case USB :
		res = USB_disk_read(0,buff, sector, count);
		// translate the reslut code here

		return res;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res;


	switch (drv) {
	case ATA :
		//result = ATA_disk_write(buff, sector, count);
		// translate the reslut code here

		return res;

	case MMC :
		res = MMC_disk_write(0,buff, sector, count);
		// translate the reslut code here

		return res;

	case USB :
		res = USB_disk_write(0,buff, sector, count);
		// translate the reslut code here

		return res;
	}
	return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;


	switch (drv) {
	case ATA :
		// pre-process here

		//result = ATA_disk_ioctl(ctrl, buff);
		// post-process here

		return res;

	case MMC :
		// pre-process here

		//result = MMC_disk_ioctl(ctrl, buff);
		res =  MMC_disk_ioctl (
			0,		/* Physical drive number (0) */
			ctrl,		/* Control code */
			buff		/* Buffer to send/receive control data */
		);
		// post-process here

		return res;

	case USB :
		// pre-process here

		res = USB_disk_ioctl(ctrl, buff);
		// post-process here

		return res;
	}
	return RES_PARERR;
}

