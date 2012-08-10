/*
**************************************************************************************************************
*                                                 NXP USB Host Stack
*
*                                     (c) Copyright 2008, NXP SemiConductors
*                                     (c) Copyright 2008, OnChip  Technologies LLC
*                                                 All Rights Reserved
*
*                                                  www.nxp.com
*                                               www.onchiptech.com
*
* File           : usbhost_main.c
* Programmer(s)  : Ravikanth.P
* Version        :
*
**************************************************************************************************************
*/

/*
**************************************************************************************************************
*                                       INCLUDE HEADER FILES
**************************************************************************************************************
*/

#include  "usbhost_main.h"
#ifdef FATFS_FLAG
#include "diskio.h"
#include "ff.h"
#include "lpc17xx_rtc.h"
#include "ff.h"
#endif

#ifdef AES_ENABLE
#include "cypher.h"
#endif
/*
**************************************************************************************************************
*                                          MAIN FUNCTION
*
* Description: This function is the main function where the execution begins
*
* Arguments  : None
*
* Returns    : 
*
**************************************************************************************************************
*/
#ifdef FATFS_FLAG
FIL File1, File2;			/* File objects */
FATFS USBFs[_VOLUMES];		/* File system object for each logical drive */
#endif
int usb_host_main( void *pvParameters )
{
    USB_INT32S  rc;
	USB_INT32U  numBlks, blkSize;
	USB_INT08U  inquiryResult[INQUIRY_LENGTH];

	//SystemInit();                               /* initialize clocks */
 

    UART_Init_usb(115200);         /* Initialize the serial port to view the log messages                       */
#ifdef FATFS_FLAG
    uint8_t read_buffer[80];
    uint32_t bytes_read ;
    FILINFO fno;
    DIR dir;
    uint8_t file_attr[8];
    uint8_t dir_name[32];


     //PRINT_Log("usb_host_main init the USB FS \n\r");
    //disk_initialize(USB);

    //PRINT_Log("usb_host_main f_mount \n\r");
    rc = f_mount(USB,&USBFs[USB]);

    PRINT_Log("f_mount return %x \n\r",rc);
    //change driver to USB
    rc = f_chdrive(USB);
    PRINT_Log("f_chdrive return %x \n\r",rc);
    vTaskDelay(500);
/*
    //open the file and check if we have errors or not
    PRINT_Log("Open the file \n\r");
    rc = f_open(&File1,FILENAME_R,FA_READ);
    PRINT_Log("f_open return %x \n\r",rc);


    //read the file and print it in the UART
    do
    {
    	f_read(&File1,read_buffer,79,&bytes_read);
    	read_buffer[bytes_read]='\0';
    	if(bytes_read>1)
    		PRINT_Log(read_buffer);

    }while(bytes_read>0);

    f_close(&File1);
    rc = f_opendir(&dir, "");
*/
    //static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    static char lfn[_MAX_LFN * (0 ? 2 : 1) + 1]; //need to change from hard coded 0
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
    char *fn;
    int counter = 0;
    PRINT_Log("\n\rDisplay Directory info:\n\r");
    //print out the directory
    //then get inside one sub and print out the content
    //open dir

    rc = f_opendir(&dir,"/");
    PRINT_Log("f_opendir return %x \n\r",rc);

    do{
		rc = f_readdir(&dir, &fno);

		if(rc != 0)
		{
			PRINT_Log("f_readdir return %x \n\r",rc);
			break;
		}
		fn = *fno.lfname ? fno.lfname : fno.fname;
		//check what is the type of it
		switch(fno.fattrib & AM_DIR)
		{
		case AM_DIR:
			sprintf(file_attr,"Dir");
			sprintf(dir_name,"/%s",fn);
			break;
		default:
			sprintf(file_attr,"File");

		}
		if(fno.fname[0] != '\0')
			PRINT_Log("%s: %s Attrib %x\n\r",file_attr,fn,fno.fattrib );

    }while(fno.fname[0] != 0);

    //check AES


#if 0
    //change directory
	rc = f_chdir(dir_name);
    rc = f_opendir(&dir, dir_name);
	PRINT_Log("changed directory to %s returned %x\n\r",dir_name ,rc);
	//find the first file and just open it and send it out
	do{
		rc = f_readdir(&dir, &fno);
		if(rc != 0)
		    break;
		fn = *fno.lfname ? fno.lfname : fno.fname;
		PRINT_Log("%s: %s Attrib %x\n\r",file_attr,fn,fno.fattrib );
	}//while(fno.fname[0] != 0);
	while(fno.fattrib != 0x20);

/*
	rc = f_open(&File1, fn, FA_READ);
	PRINT_Log("f_open opening %s returned %x\n\r",fn ,rc);
	do {
		f_read(&File1, read_buffer, 79, &bytes_read);
		read_buffer[bytes_read] = '\0';
		if (bytes_read > 1)
			PRINT_Log(read_buffer);

	} while (bytes_read > 0);

*/

#endif




#else

    Host_Init();               /* Initialize the lpc17xx host controller                                    */
    rc = Host_EnumDev();       /* Enumerate the device connected                                            */

    if (rc == OK) {
		/* Initialize the mass storage and scsi interfaces */
        rc = MS_Init( &blkSize, &numBlks, inquiryResult );
        if (rc == OK) {
            rc = FAT_Init();   /* Initialize the FAT16 file system                                          */
            if (rc == OK) {
                Main_Copy();   /* Call the application                                                      */
            } else {
				while ( 1 );
                return (0);
            }
        } else {
			while ( 1 );
            return (0);
        }
    } else {							
		while ( 1 );
        return (0);
    }
#endif
    while(1)
    {
    	vTaskDelay(100);

    }
}

/*
**************************************************************************************************************
*                                      READ DATA FROM DISK
*
* Description: This function is used by the user to read data from the disk
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/
#ifndef FATFS_FLAG
void  Main_Read (void)
{
    USB_INT32S  fdr;
    USB_INT32U  bytes_read;
    

    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        PRINT_Log("Reading from %s...\n", FILENAME_R);
        do {
            bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
        } while (bytes_read);

        FILE_Close(fdr);
        PRINT_Log("Read Complete\n");
    } else {
        PRINT_Log("Could not open file %s\n", FILENAME_R);
        return;
    }
}

/*
**************************************************************************************************************
*                                      WRITE DATA TO DISK
*
* Description: This function is used by the user to write data to disk
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

void  Main_Write (void)
{
    USB_INT32S  fdw;
    USB_INT32S  fdr;
    USB_INT32U  tot_bytes_written;
    USB_INT32U  bytes_written;


    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) {
            tot_bytes_written = 0;
            PRINT_Log("Writing to %s...\n", FILENAME_W);
            do {
                bytes_written = FILE_Write(fdw, UserBuffer, MAX_BUFFER_SIZE);
                tot_bytes_written += bytes_written;
            } while (tot_bytes_written < WRITE_SIZE);
            FILE_Close(fdw);
            PRINT_Log("Write completed\n");
        } else {
            PRINT_Log("Could not open file %s\n", FILENAME_W);
            return;
        }
        FILE_Close(fdr);
    } else {
        PRINT_Log("Could not open file %s\n", FILENAME_R);
        return;
    }
}

/*
**************************************************************************************************************
*                                    COPYING A FILE
*
* Description: This function is used by the user to copy a file
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

void  Main_Copy (void)
{
    USB_INT32S  fdr;
    USB_INT32S  fdw;
    USB_INT32U  bytes_read;


    fdr = FILE_Open(FILENAME_R, RDONLY);
    if (fdr > 0) {
        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) {
            PRINT_Log("Copying from %s to %s...\n", FILENAME_R, FILENAME_W);
            do {
                bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
                FILE_Write(fdw, UserBuffer, bytes_read);
            } while (bytes_read);
            FILE_Close(fdw);
        } else {
            PRINT_Log("Could not open file %s\n", FILENAME_W);
            return;
        }

        //try to read the directories



        FILE_Close(fdr);
        PRINT_Log("Copy completed\n");
    } else {
        PRINT_Log("Could not open file %s\n", FILENAME_R);
        return;
    }
}
#endif
