/***************************************************************

Date: August 28th, 2007.
Copyright (c) 2007 Cyan Technology Limited. All Rights Reserved.

Cyan Technology Limited, hereby grants a non-exclusive license
under Cyan Technology Limited's copyright to copy, modify and
distribute this software for any purpose and without fee,
provided that the above copyright notice and the following
paragraphs appear on all copies.

Cyan Technology Limited makes no representation that this
source code is correct or is an accurate representation of any
standard.

In no event shall Cyan Technology Limited have any liability
for any direct, indirect, or speculative damages, (including,
without limiting the forgoing, consequential, incidental and
special damages) including, but not limited to infringement,
loss of use, business interruptions, and loss of profits,
respective of whether Cyan Technology Limited has advance
notice of the possibility of any such damages.

Cyan Technology Limited specifically disclaims any warranties 
including, but not limited to, the implied warranties of
merchantability, fitness for a particular purpose, and non-
infringement. The software provided hereunder is on an "as is"
basis and Cyan Technology Limited has no obligation to provide
maintenance, support, updates, enhancements, or modifications.

****************************************************************/

/*=============================================================================
Cyan Technology Limited

FILE - Connections.c

DESCRIPTION
    Performs the actual FTP Daemon connection handling.

MODIFICATION DETAILS
=============================================================================*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>



#include "ff.h"
#include "FTP_Strings.h"

#include "lpc_types.h"
#include "usbhost_lpc17xx.h"

#include "uip.h"
#include "psock.h"

#include "FTPD.h"
#include "ff.h"

#include "Connections.h"
#define NUM_COMMANDS (sizeof(Cmd_dictionary)/sizeof(Cmd_dict_struct))

typedef PT_THREAD((*FP)(ftpd_state_t *s));

typedef struct
{
	char    *Name;
	FP      Function;
} Cmd_dict_struct;


/* Forward Function Definitions */
PT_THREAD(Cmd_ukwn(ftpd_state_t *s));
PT_THREAD(Cmd_user(ftpd_state_t *s));
PT_THREAD(Cmd_pass(ftpd_state_t *s));
PT_THREAD(Cmd_quit(ftpd_state_t *s));
PT_THREAD(Cmd_noop(ftpd_state_t *s));
PT_THREAD(Cmd_pasv(ftpd_state_t *s));
PT_THREAD(Cmd_port(ftpd_state_t *s));
PT_THREAD(Cmd_list(ftpd_state_t *s));
PT_THREAD(Cmd_retr(ftpd_state_t *s));
PT_THREAD(Cmd_stor(ftpd_state_t *s));
PT_THREAD(Cmd_pwd(ftpd_state_t *s));
PT_THREAD(Cmd_cwd(ftpd_state_t *s));
PT_THREAD(Cmd_cdup(ftpd_state_t *s));
PT_THREAD(Cmd_type(ftpd_state_t *s));
PT_THREAD(Cmd_abor(ftpd_state_t *s));
PT_THREAD(Cmd_mode(ftpd_state_t *s));
PT_THREAD(Cmd_stru(ftpd_state_t *s));
PT_THREAD(Cmd_dele(ftpd_state_t *s));
PT_THREAD(Cmd_mkd(ftpd_state_t *s));
PT_THREAD(Cmd_rmd(ftpd_state_t *s));
PT_THREAD(Cmd_help(ftpd_state_t *s));


/* Local static variables */
Cmd_dict_struct const Cmd_dictionary[] =
{
    {"USER", Cmd_user},
    {"QUIT", Cmd_quit},
    {"NOOP", Cmd_noop},
    {"PASV", Cmd_pasv},
    {"PORT", Cmd_port},
    {"LIST", Cmd_list},
    {"RETR", Cmd_retr},
    {"STOR", Cmd_stor},
    {"PWD", Cmd_pwd},
    {"XPWD", Cmd_pwd},
    {"CWD", Cmd_cwd},
    {"CDUP", Cmd_cdup},
    {"TYPE", Cmd_type},
    {"ABOR", Cmd_abor},
    {"MODE", Cmd_mode},
    {"STRU", Cmd_stru},
    {"DELE", Cmd_dele},
    {"MKD", Cmd_mkd},
    {"XMKD", Cmd_mkd},
    {"RMD", Cmd_rmd},
    {"XRMD", Cmd_rmd},
    {"HELP", Cmd_help},
    // Unknown command response must always be the last in the list
    {"UNKN", Cmd_ukwn}
};


// Run though the given string and convert all the letters to upper case
void Convert_to_upper(char *s)
{
    while (*s)
    {
        //*s = toupper(*s);
        s++;
    }
}


PT_THREAD(handle_cmd_input(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsin);

    // This is a new connection so send the 220 response
    PSOCK_SEND_STR(&s->cmdpsin, Resp220);

    while ((s->state != QUITTING) && (s->state != FTP_UNUSED))
    {
        // Read the line from the client
        PSOCK_READTO(&s->cmdpsin, '\n');
        
        // Replace the \n with a \0
        s->cmdinputbuffer[PSOCK_DATALEN(&s->cmdpsin) - 2] = '\0';
        
        PRINT_Log("\r\nReceived: \"%s\"", s->cmdinputbuffer);
        
        // Find the space that delimits the parameters from the command
        if (NULL == (s->params = strchr(s->cmdinputbuffer, ' ')))
        {
            // No parameters, so make the parameter pointer point to the null at the end of the command
            s->params = s->cmdinputbuffer + strlen(s->cmdinputbuffer);
        }
        else
        {
            // Replace the space with a null and move the parameter pointer up by one
            *(s->params++) = '\0';
        }
        
        // Convert all the letters in the parameters to upper case
        Convert_to_upper(s->params);
        
        // Wait for the previous output to complete before looking up this one
        PSOCK_WAIT_UNTIL(&s->cmdpsin, (CMD_INDEX_IDLE == s->cmd_index));
        
        for (s->cmd_index = 0; s->cmd_index < (NUM_COMMANDS - 1); s->cmd_index++)
        {
            if (0 == strcmp(s->cmdinputbuffer, Cmd_dictionary[s->cmd_index].Name))
            {
                break;
            }
        }
    }
    
    PSOCK_CLOSE(&s->cmdpsin);

    PSOCK_END(&s->cmdpsin);
}


PT_THREAD(Closed_data_conn(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nClosed data connection");
    PSOCK_SEND_STR(&s->cmdpsout, Resp226);

    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Aborted_data_conn(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nAborted data connection");
    PSOCK_SEND_STR(&s->cmdpsout, Resp426);

    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(handle_cmd_output( ftpd_state_t *s ))
{
    PT_BEGIN(&s->cmdpt);

    if (CMD_INDEX_IDLE != s->cmd_index)
    {
        // Initialise the output Proto Socket
        PSOCK_INIT(&s->cmdpsout, s->cmdinputbuffer, CMD_BUFFER_SIZE);
        // Call the current output function
        PT_WAIT_THREAD(&s->cmdpt, (Cmd_dictionary[s->cmd_index].Function)(s));
        // Clear the command index down
        s->cmd_index = CMD_INDEX_IDLE;
    }
    else if (TRANSFER_DONE == s->state)
    {
        // Initialise the output Proto Socket
        PSOCK_INIT(&s->cmdpsout, s->cmdinputbuffer, CMD_BUFFER_SIZE);
        // Call the tranfer complete function
        PT_WAIT_THREAD(&s->cmdpt, Closed_data_conn(s));
        // Reset the state
        s->state = LOGGED_IN;
    }
    else if (TRANSFER_ABORT == s->state)
    {
        // Initialise the output Proto Socket
        PSOCK_INIT(&s->cmdpsout, s->cmdinputbuffer, CMD_BUFFER_SIZE);
        // Call the tranfer aborted function
        PT_WAIT_THREAD(&s->cmdpt, Aborted_data_conn(s));
        // Reset the state
        s->state = LOGGED_IN;
    }

    PT_END(&s->cmdpt);
}


PT_THREAD(Cmd_ukwn(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nUnknown Command");
    PSOCK_SEND_STR(&s->cmdpsout, Resp500);

    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_user(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nUSER Command");
    if (LOGGED_OUT != s->state)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp501);
    }
    else
    {
        s->state = LOGGED_IN;
        PSOCK_SEND_STR(&s->cmdpsout, Resp230);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_quit(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nQUIT Command");
    if (NULL == s->data_conn)
    {
        // Simple close of the control connection
        s->state = FTP_UNUSED;
    }
    else
    {
        // Flag to the data connection that it needs to close
        s->state = QUITTING;
    }

    PSOCK_SEND_STR(&s->cmdpsout, Resp221);

    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_noop(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nNOOP Command");
    PSOCK_SEND_STR(&s->cmdpsout, Resp200);
    
    PSOCK_END(&s->cmdpsout);
}


static unsigned short Send_PASV_response( void *state )
{
    uip_ipaddr_t hostip;
    ftpd_state_t *s = (ftpd_state_t *)state;

    uip_gethostaddr(hostip);
    return sprintf((char*)uip_appdata, Resp227, uip_ipaddr1(hostip), uip_ipaddr2(hostip),
                   uip_ipaddr3(hostip), uip_ipaddr4(hostip), uip_ipaddr3(s->pasvport), uip_ipaddr4(s->pasvport));
}


PT_THREAD(Cmd_pasv(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nPASV Command");

    if (!s->pasvflag)
    {
        // Listen...
        uip_listen(htons(s->pasvport));
        PRINT_Log("\r\nListening on port %d", s->pasvport);
        s->pasvflag = TRUE;
    }

    PSOCK_GENERATOR_SEND(&s->cmdpsout, Send_PASV_response, s);
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_port(ftpd_state_t *s))
{
    int i;
    int data[6];
    
    PSOCK_BEGIN(&s->cmdpsout);

    //f_getcwd(s->Current_path, _MAX_LFN);


    PRINT_Log("\r\nPORT Command");
    
    if (s->pasvflag)
    {
        uip_unlisten(htons(s->pasvport));
        s->pasvflag = FALSE;
    }

    // Extract the ip address and port from the parameters
    for (i = 0; i < 6; i++)
    {
        data[i] = 0;
        while ((*s->params >= '0') && (*s->params <= '9'))
        {
            data[i] = (data[i] * 10) + (*s->params - '0');
            s->params++;
        }
        if (*s->params != ',')
        {
            // syntax error or end of string
            break;
        }
        else
        {
            // skip over the comma
            s->params++;
        }
    }

    // Note that loop will terminate early when it hits the '\0'
    if (5 == i)
    {
        uip_ipaddr(s->clientip, data[0], data[1], data[2], data[3]);
        s->clientport = (data[4] << 8) + data[5];
        
        PRINT_Log("\r\nDecoded data port %d.%d.%d.%d:%d", uip_ipaddr1(s->clientip),
               uip_ipaddr2(s->clientip), uip_ipaddr3(s->clientip),
               uip_ipaddr4(s->clientip), s->clientport);
        
        PSOCK_SEND_STR(&s->cmdpsout, Resp200);
    }
    else
    {
    	PRINT_Log("\r\nSyntax error in PORT command");
        PSOCK_SEND_STR(&s->cmdpsout, Resp501);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_list(ftpd_state_t *s))
{

	static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
	FRESULT rc;
    FILINFO fno;
    //DIR dir;



    PSOCK_BEGIN(&s->cmdpsout);

    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);

    s->count = 0 ;

    PRINT_Log("\r\nLIST Command with path %s",s->Current_path);

    //if (0 == (s->count = fat_get_num_dir_entries(s->pCurrent_dir)))
		f_chdir(s->Current_path);
		rc = f_opendir(&s->dir,s->Current_path);
		f_readdir(&s->dir, NULL);

        fno.lfname = lfn;
        fno.lfsize = sizeof(lfn);
        //uint_fast8_t rc ;
        char *fn;
        //PRINT_Log("\n\rf_opendir s->Current_path %s return %x ", s->Current_path , rc);
        //print out the directory
        //then get inside one sub and print out the content
        do{
        rc = f_readdir(&s->dir, &fno);
        //PRINT_Log("\n\r f_readdir return %x , filename %s ", rc,fno.lfname);
        if(rc != 0)
        	break;
       	fn = *fno.lfname ? fno.lfname : fno.fname;

       	s->count++ ;

        }while(fno.fname[0] != 0);
        //rewind the f_readdir
        rc = f_opendir(&s->dir,s->Current_path);
        f_readdir(&s->dir, NULL);

    //PRINT_Log("\r\nReturned directory entries:%d ",s->count);
    if (0 == s->count)
    {
    	//PRINT_Log("\r\nFailed to get number of directory entries. ERROR ");
        PSOCK_SEND_STR(&s->cmdpsout, Resp451);
        PSOCK_EXIT(&s->cmdpsout);
    }

    if (s->pasvflag)
    {
        // Check if we already have a data connection
        if (s->data_conn)
        {
            s->state = CONNECT_OK;
            PSOCK_SEND_STR(&s->cmdpsout, Resp125);
        }
        else
        {
            // Wait for the client to open a data connection
            s->state = WAITING_FOR_CONNECT;
        }
    }
    else
    {
        if (s->data_conn)
        {
            s->state = CONNECT_OK;
            PSOCK_SEND_STR(&s->cmdpsout, Resp125);
        }
        else if (NULL == (s->data_conn = uip_connect(&s->clientip, htons(s->clientport))))
        {
            // Couldn't open the data connection
            PSOCK_SEND_STR(&s->cmdpsout, Resp425);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Wait for the data connection to open
            s->state = WAITING_FOR_CONNECT;
        }
    }
    
    // Wait for the connection to open
    if (WAITING_FOR_CONNECT == s->state)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp150);
        
        // Wait for the data connection to be opened
        PSOCK_WAIT_UNTIL(&s->cmdpsout, (WAITING_FOR_CONNECT != s->state))
    }
    
    if (s->state != CONNECT_OK)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp150);
        s->state = LOGGED_IN;
        s->data_conn = NULL;
        PSOCK_EXIT(&s->cmdpsout);
    }
    rc = f_opendir(&s->dir,s->Current_path);
    f_readdir(&s->dir, NULL);
    s->count--;

    // Set the state to transfer the directory listing
    s->state = TRANSFER_LIST;
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_retr(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);

    PRINT_Log("\r\nRETR Command");

    // Check if it is an absolute path
    if ('/' == *s->params)
    {
        strcpy(s->Current_file, s->params);
    }
    else
    {
        // Copy the current path into the current filename
        strcpy(s->Current_file, s->Current_path);
        if (strlen(s->Current_file) > 1)
        {
            strcat(s->Current_file, "/");
        }
        // Append the filename
        strcat(s->Current_file, s->params);
    }
    
    PRINT_Log("\r\nSending file: \"%s\"", s->Current_file);

    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "rb+")))
    if (0 != (f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_OPEN_EXISTING)))
    {
        // File open failed
    	PRINT_Log("\r\nFAILED: fat_fopen Error ");
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
        PSOCK_EXIT(&s->cmdpsout);
    }
    else
    {
        // Close the file
        //fat_fclose(s->fat_file);
    	f_close(&s->File);

    }

    if (s->pasvflag)
    {
        // Check if we already have a data connection
        if (s->data_conn)
        {
            s->state = CONNECT_OK;
            PSOCK_SEND_STR(&s->cmdpsout, Resp125);
        }
        else
        {
            // Wait for the client to open a data connection
            s->state = WAITING_FOR_CONNECT;
        }
    }
    else
    {
        if (s->data_conn)
        {
            s->state = CONNECT_OK;
            PSOCK_SEND_STR(&s->cmdpsout, Resp125);
        }
        else if (NULL == (s->data_conn = uip_connect(&s->clientip, htons(s->clientport))))
        {
            // Couldn't open the data connection
            PSOCK_SEND_STR(&s->cmdpsout, Resp425);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Wait for the data connection to open
            s->state = WAITING_FOR_CONNECT;
        }
    }
    
    // Wait for the connection to open
    if (WAITING_FOR_CONNECT == s->state)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp150);
        
        // Wait for the data connection to be opened
        PSOCK_WAIT_UNTIL(&s->cmdpsout, (WAITING_FOR_CONNECT != s->state))
    }
    
    if (s->state != CONNECT_OK)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp150);
        s->state = LOGGED_IN;
        s->data_conn = NULL;
        PSOCK_EXIT(&s->cmdpsout);
    }

    // Set the state to send the file
    s->state = TRANSFER_SEND;
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_stor(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);



    // Check if it is an absolute path
    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);

    PRINT_Log("\r\nSTOR Command with current path %s",s->Current_path);
    if ('/' == *s->params)
    {
        strcpy(s->Current_file, s->params);
    }
    else
    {
        // Copy the current path into the current filename
        strcpy(s->Current_file, s->Current_path);
        if (strlen(s->Current_file) > 3)
        {
        strcat(s->Current_file, "/");
        }
        // Append the filename
        strcat(s->Current_file, s->params);
    }
    
    PRINT_Log("\r\nReceiving file: \"%s\"", s->Current_file);
    //       PRINT_Log("\r\nOpening the File ");
    //       CreateFile(s,FALSE);
    // Check we can create a file of that name
    // this will also delete it if it already exists
    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "wb+")))
    if (0 != f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_CREATE_ALWAYS))
    {
        // File open failed
    	PRINT_Log("\r\nFAILED: fat_fopen Error");
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
        PSOCK_EXIT(&s->cmdpsout);
    }
    else
    {
        // Close the file
        //fat_fclose(s->fat_file);
    	//f_close(&s->File); do not close
    }

    // Check if we already have a data connection
    if (s->data_conn)
    {
        s->state = TRANSFER_RECV;
        PSOCK_SEND_STR(&s->cmdpsout, Resp125);
    }
    else if (s->pasvflag)
    {
        // Wait for the client to open a data connection
        s->state = TRANSFER_RECV;
        PSOCK_SEND_STR(&s->cmdpsout, Resp150);
    }
    else
    {
        if (NULL == (s->data_conn = uip_connect(&s->clientip, htons(s->clientport))))
        {
            // Couldn't open the data connection
            PSOCK_SEND_STR(&s->cmdpsout, Resp425);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Wait for the data connection to open
            s->state = TRANSFER_RECV;
            PSOCK_SEND_STR(&s->cmdpsout, Resp150);
        }
    }
    
    PSOCK_END(&s->cmdpsout);
}


static unsigned short Send_257_response( void *string )
{
    return sprintf((char*)uip_appdata, Resp257, (char*)string);
}


PT_THREAD(Cmd_pwd(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);
    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);
    PRINT_Log("\r\nPWD Command");
    PSOCK_GENERATOR_SEND(&s->cmdpsout, Send_257_response, s->Current_path);
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_cwd(ftpd_state_t *s))
{
    //fat_fat_dir_entry_t *fat_dir_entry;
    int i;
    int len;

    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nCWD Command, dir = \"%s\"", s->params);

    // Check for a / at the end of the path
    len = strlen(s->params);
    if ((len > 1) && ('/' == s->params[len - 1]))
    {
        s->params[len - 1] = '\0';
    }

    // Check if it is an absolute path
    if ('/' == *s->params)
    {
        if (1 == strlen(s->params))
        {
        	PRINT_Log("\r\nReturning to root");
            // Selecting root directory
            //fat_dir_entry = NULL;
            f_chdir("/");

            //strcpy(s->Current_path, "/");
            //f_getcwd(s->Current_path,_MAX_LFN);
        }
        else
        {
        	PRINT_Log(" - Absolute path");
            // NOTE the lack of overrun checking!
            strcpy(s->Current_path, s->params);
            f_chdir(s->params);
            //f_getcwd(s->Current_path,_MAX_LFN);

#if 0
            if (NULL == (fat_dir_entry = fat_find_directory_entry(s->params, NULL)))
            {
                printf("\r\nFailed to find directory entry. ERROR %u", fat_get_error());
                PSOCK_SEND_STR(&s->cmdpsout, Resp550);
                PSOCK_EXIT(&s->cmdpsout);
            }
#endif

        }
    }
    else if (0 == strcmp(s->params, ".."))
        {
            //if (!fat_get_dir_parent(s->pCurrent_dir, &fat_dir_entry))
        	//if (!f_getcwd(s->pCurrent_dir, _MAX_LFN))
			if (f_chdir(s->params))
            {
        		PRINT_Log("\r\nFailed to parent directory entry. ERROR %u", f_chdir(s->params));
                PSOCK_SEND_STR(&s->cmdpsout, Resp550);
                PSOCK_EXIT(&s->cmdpsout);
            }




        }
        else
        {
        	PRINT_Log(" - Relative path");
            if (f_chdir(s->params))
            {
            	PRINT_Log("\r\nFailed to find directory entry. ERROR %u", f_chdir(s->params));
                PSOCK_SEND_STR(&s->cmdpsout, Resp550);
                PSOCK_EXIT(&s->cmdpsout);
            }
            // Append new directory on to the current path
#if 0
            if (strlen(s->Current_path) > 1)
            {
                // Add a slash is we're not at the root
                strcat(s->Current_path, "/");
            }
            strcat(s->Current_path, s->params);
#endif
        }

    //reset the directory list


#if 0
    else if (0 == strcmp(s->params, ".."))
    {
        //if (!fat_get_dir_parent(s->pCurrent_dir, &fat_dir_entry))
    	if (!f_getcwd(s->pCurrent_dir, _MAX_LFN))
        {
            //printf("\r\nFailed to parent directory entry. ERROR %u", fat_get_error());
            PSOCK_SEND_STR(&s->cmdpsout, Resp550);
            PSOCK_EXIT(&s->cmdpsout);
        }

        // The parent may be indicated by a NULL pointer
        if (NULL == fat_dir_entry)
        {
            s->pCurrent_dir = NULL;
            strcpy(s->Current_path, "/");
        }
        else
        {
            // Make a copy of the directory entry
            s->pCurrent_dir = (fat_fat_dir_entry_t*)memcpy(&s->Current_dir_entry, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
            // Cut the directory name off the end of the current path
            i = strlen(s->Current_path);
            while(i && ('/' != s->Current_path[i]))
            {
                i--;
            }
            // Check if we've chopped down to the root
            if (0 == i)
            {
                s->Current_path[1] = '\0';
            }
            else
            {
                s->Current_path[i] = '\0';
            }
        }

    }
    else
    {
        printf(" - Relative path");
        if (NULL == (fat_dir_entry = fat_find_directory_entry(s->params, s->pCurrent_dir)))
        {
            printf("\r\nFailed to find directory entry. ERROR %u", fat_get_error());
            PSOCK_SEND_STR(&s->cmdpsout, Resp550);
            PSOCK_EXIT(&s->cmdpsout);
        }
        // Append new directory on to the current path
        if (strlen(s->Current_path) > 1)
        {
            // Add a slash is we're not at the root
            strcat(s->Current_path, "/");
        }
        strcat(s->Current_path, s->params);
    }
    
    // Make a copy of the directory entry
    if (fat_dir_entry)
    {
        s->pCurrent_dir = (fat_fat_dir_entry_t*)memcpy(&s->Current_dir_entry, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
    }
    else
    {
        s->pCurrent_dir = NULL;
    }
#endif
    f_getcwd(s->Current_path, _MAX_LFN);
    PRINT_Log("\r\nCurrent Dir %s",s->Current_path);
    // Send the completion response
    PSOCK_SEND_STR(&s->cmdpsout, Resp250);
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_cdup(ftpd_state_t *s))
{
    //fat_fat_dir_entry_t *fat_dir_entry;
	Bool have_parent = FALSE;
    int i;
	FRESULT rc;
    FILINFO fno;
    //DIR dir;
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];

    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nCDUP Command");

    rc = f_opendir(&s->dir,s->Current_path);
    f_readdir(&s->dir, NULL);
/*
	FRESULT rc;
    FILINFO fno;
    DIR dir;
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
*/
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
    uint_fast8_t rc ;
    char *fn;
    PRINT_Log("\n\rDisplay Directory info for Dir %s :\n\r",s->Current_path);
    //print out the directory
    //then get inside one sub and print out the content
    do{
    rc = f_readdir(&s->dir, &fno);
    if(rc != 0)
    	break;
   	fn = *fno.lfname ? fno.lfname : fno.fname;


   	if(fn[0] == '.' )
   	{
   		have_parent = TRUE;
   		break;
   	}


   	s->count++ ;

    }while(fno.fname[0] != 0);

    //if (!fat_get_dir_parent(s->pCurrent_dir, &fat_dir_entry))
    if (!have_parent)
    {
    	PRINT_Log("\r\nFailed to parent directory entry. ERROR ");
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
        PSOCK_EXIT(&s->cmdpsout);
    }

#if 0
    // The parent may be indicated by a NULL pointer
    if (NULL == fat_dir_entry)
    {
        s->pCurrent_dir = NULL;
        strcpy(s->Current_path, "/");
    }
    else
    {
        // Make a copy of the directory entry
        s->pCurrent_dir = (fat_fat_dir_entry_t*)memcpy(&s->Current_dir_entry, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
        // Cut the directory name off the end of the current path
        i = strlen(s->Current_path);
        while(i && ('/' != s->Current_path[i]))
        {
            i--;
        }
        s->Current_path[i] = '\0';
    }
#else
    f_chdir("..");
    f_getcwd(s->Current_path,_MAX_LFN);


#endif

    PSOCK_SEND_STR(&s->cmdpsout, Resp250);
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_type(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nTYPE Command");

    // Support Image and ASCII.
    if(('I' == *s->params) || ('A' == *s->params))
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp200);
    }
    else
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp504);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_abor(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->dataps);

    PRINT_Log("\r\nABOR Command");

    if (TRANSFER_LIST == s->state)
    {
        s->state = TRANSFER_ABORT;
    }

    PSOCK_SEND_STR(&s->cmdpsout, Resp226);

    PSOCK_END(&s->dataps);
}


PT_THREAD(Cmd_mode(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nMODE Command");

    // Support only Stream.
    if('S' == *s->params)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp200);
    }
    else
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp504);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_stru(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nSTRU Command");

    // Support only File.
    if('F' == *s->params)
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp200);
    }
    else
    {
        PSOCK_SEND_STR(&s->cmdpsout, Resp504);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_dele(ftpd_state_t *s))
{
    //fat_fat_dir_entry_t *fat_dir_entry = NULL;
    //fat_fat_dir_entry_t fat_dir_local;
    int i;
	FRESULT rc;
    FILINFO fno;
    //DIR dir;
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    char file_name[_MAX_LFN];
    char org_path[_MAX_LFN];

    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);
    f_getcwd(org_path, _MAX_LFN);
    PRINT_Log("\r\nStrating Dir %s ",org_path);

    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nDELE Command, file = \"%s\"", s->params);

    // Check if it is an absolute directory
    if ('/' == *s->params)
    {
        strcpy(s->Current_file, s->params);
    }
    else
    {
        // Concatinate the current path and the given file
        strcpy(s->Current_file, s->Current_path);
        //if (strlen(s->Current_file) > 3 )
        //{
        strcat(s->Current_file, "/");
        //}
        strcat(s->Current_file, s->params);
    }
    
    // Search backwards for the last slash
    for (i = strlen(s->Current_file) - 1; i >= 0; i--)
    {
        if ('/' == s->Current_file[i])
        {
            s->Current_file[i] = '\0';
            break;
        }
    }

    strcpy(file_name,&s->Current_file[i+1]);
    PRINT_Log("\n\rFile Name %s :\n\r",file_name);

    f_chdir( s->Current_file);
    rc = f_opendir(&s->dir,s->Current_file);


    // Check if the file is in the root
    if (i)
    {

        fno.lfname = lfn;
        fno.lfsize = sizeof(lfn);
        uint_fast8_t rc ;
        char *fn;
        Bool found_file = FALSE ;
        PRINT_Log("\n\rDisplay Directory info for Dir %s :\n\r",s->Current_file);
        f_readdir(&s->dir, NULL);
        //print out the directory
        //then get inside one sub and print out the content
        do{
        rc = f_readdir(&s->dir, &fno);
        if(rc != 0)
        	break;
       	fn = *fno.lfname ? fno.lfname : fno.fname;
       	PRINT_Log("\n\rcompare file name %s to the directory item %s:\n\r",s->Current_file,fn);


       	if(!strcmp(file_name,fn))
       	{
       		found_file = TRUE;
       		PRINT_Log("File %s Found ", fn);
       		break;
       	}






        }while(fno.fname[0] != 0);

    	//if (NULL == (fat_dir_entry = fat_find_directory_entry(s->Current_file, NULL)))
        if (!found_file)
        {
        	PRINT_Log("\r\nFailed to find directory. ERROR ");
            PSOCK_SEND_STR(&s->cmdpsout, Resp550);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Make a copy of the directory entry
            //fat_dir_entry = (fat_fat_dir_entry_t *)memcpy(&fat_dir_local, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
        	PRINT_Log("\r\nFound directory ok");
        }        
    }
        
    // Delete the file
    //if (fat_delete_file(&s->Current_file[i + 1], fat_dir_entry))
    //if (!f_unlink (&s->Current_file[i + 1]))
    if (!f_unlink (file_name))
    {
    	PRINT_Log("\r\nDeleted OK");
        PSOCK_SEND_STR(&s->cmdpsout, Resp250);
    }
    else
    {
    	PRINT_Log("\r\nFailed to delete file. ERROR %d",f_unlink (file_name));
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
    }
    
    //return back to original dir
    PRINT_Log("\r\nGoing back to %s ",org_path);

    f_chdir(s->Current_path);
   	rc = f_opendir(&s->dir,s->Current_path);
   	//f_getcwd(s->Current_path, _MAX_LFN);

    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_mkd(ftpd_state_t *s))
{
    //fat_fat_dir_entry_t *fat_dir_entry = NULL;
    //fat_fat_dir_entry_t fat_dir_local;
    int i;
    Bool found_dir = FALSE ;
	FRESULT rc;
    FILINFO fno;
    //DIR dir;
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];

    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nMKD Command, directory = \"%s\"", s->params);

    // Check if it is an absolute path
    if ('/' == *s->params)
    {
        strcpy(s->Current_file, s->params);
    }
    else
    {
        // Concatinate the current path and the given directory
        strcpy(s->Current_file, s->Current_path);
        if (strlen(s->Current_file) > 1)
        {
            strcat(s->Current_file, "/");
        }
        strcat(s->Current_file, s->params);
    }
    
    // Search backwards for the last slash
    for (i = strlen(s->Current_file) - 1; i >= 0; i--)
    {
        if ('/' == s->Current_file[i])
        {
            s->Current_file[i] = '\0';
            break;
        }
    }

    // Check if the directory is in the root
    if (i)
    {
        // Find the directory
    	rc = f_opendir(&s->dir,s->Current_path);
    	f_readdir(&s->dir, NULL);
    /*
    	FRESULT rc;
        FILINFO fno;
        DIR dir;
        static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    */
        fno.lfname = lfn;
        fno.lfsize = sizeof(lfn);
        uint_fast8_t rc ;
        char *fn;
        Bool found_file = FALSE ;
        PRINT_Log("\n\rDisplay Directory info for Dir %s :\n\r",s->Current_path);
        //print out the directory
        //then get inside one sub and print out the content
        found_dir = FALSE ;
        do{
        rc = f_readdir(&s->dir, &fno);
        if(rc != 0)
        	break;
       	fn = *fno.lfname ? fno.lfname : fno.fname;


       	if(!strcmp(s->Current_file,fn))
       	{

       		if(fno.fattrib & AM_DIR)
       		{
       			found_dir = TRUE;
       			PRINT_Log("Directory %s Found ", fn);
       			break;
       		}
       	}
   }while(fno.fname[0] != 0);


#if 0
        //if (NULL == (fat_dir_entry = fat_find_directory_entry(s->Current_file, NULL)))
        if (!found_dir)
        {
        	PRINT_Log("\r\nFailed to find directory. ERROR ");
            PSOCK_SEND_STR(&s->cmdpsout, Resp550);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Make a copy of the directory entry
            //fat_dir_entry = (fat_fat_dir_entry_t *)memcpy(&fat_dir_local, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
        	PRINT_Log("\r\nFound directory ok");
        }
#endif
    }
        
    // Create the directory
    //if (fat_create_directory(&s->Current_file[i + 1], fat_dir_entry))
    if (!f_mkdir(&s->Current_file[i + 1]))

    {
    	PRINT_Log("\r\nCreated OK");
        // Put the slash back in so as we can print the path
        s->Current_file[i] = '/';
        PSOCK_GENERATOR_SEND(&s->cmdpsout, Send_257_response, s->Current_file);
    }
    else
    {
    	PRINT_Log("\r\nFailed to create directory. ERROR %u");
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
    }
    
    PSOCK_END(&s->cmdpsout);
}


PT_THREAD(Cmd_rmd(ftpd_state_t *s))
{
    //fat_fat_dir_entry_t *fat_dir_entry = NULL;
    //fat_fat_dir_entry_t fat_dir_local;
    int i;
	FRESULT rc;
    FILINFO fno;
    //DIR dir;
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    Bool found_dir = FALSE;
    char file_name[_MAX_LFN];
    char org_path[_MAX_LFN];

    f_chdir(s->Current_path);
    f_getcwd(s->Current_path, _MAX_LFN);
    f_getcwd(org_path, _MAX_LFN);

    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nRMD Command, directory = \"%s\"", s->params);

    // Check if it is an absolute path

    if ('/' == *s->params)
    {
        strcpy(s->Current_file, s->params);
    }
    else
    {
        // Concatinate the current path and the given directory
        strcpy(s->Current_file, s->Current_path);
        //if (strlen(s->Current_file) > 3)
        //{
        strcat(s->Current_file, "/");
        //}
        strcat(s->Current_file, s->params);
    }
    
    // Search backwards for the last slash
    for (i = strlen(s->Current_file) - 1; i >= 0; i--)
    {
        if ('/' == s->Current_file[i])
        {
            s->Current_file[i] = '\0';

            break;
        }
    }

    strcpy(file_name,&s->Current_file[i+1]);
    PRINT_Log("\n\rFile Name %s :\n\r",file_name);


    f_chdir( s->Current_file);
    rc = f_opendir(&s->dir,s->Current_file);

    //now take only the file name


    // Check if the directory is in the root
    if (i)
    {

    	//rc = f_opendir(&s->dir,s->Current_path);
    	f_readdir(&s->dir, NULL);
        /*
        	FRESULT rc;
            FILINFO fno;
            DIR dir;
            static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
            Bool found_file = FALSE;
        */
            fno.lfname = lfn;
            fno.lfsize = sizeof(lfn);
            uint_fast8_t rc ;
            char *fn;
            Bool found_file = FALSE ;
            PRINT_Log("\n\rDisplay Directory info for Dir %s :\n\r",s->Current_path);
            //print out the directory
            //then get inside one sub and print out the content
            found_dir = FALSE ;
            do{
            rc = f_readdir(&s->dir, &fno);
            if(rc != 0)
            	break;
           	fn = *fno.lfname ? fno.lfname : fno.fname;

           	PRINT_Log("\n\rFound item %s  check if it is Directory %i:\n\r",fn,fno.fattrib );
           	if(!strcmp(file_name,fn))
           	{


           		if(fno.fattrib & AM_DIR)
           		{
           			found_dir = TRUE;
           			PRINT_Log("Directory %s Found ", fn);
           			break;
           		}
           	}
       }while(fno.fname[0] != 0);


        // Find the directory
        //if (NULL == (fat_dir_entry = fat_find_directory_entry(s->Current_file, NULL)))
        if (!found_dir)
        {
        	PRINT_Log("\r\nFailed to find directory. ERROR ");
            PSOCK_SEND_STR(&s->cmdpsout, Resp550);
            PSOCK_EXIT(&s->cmdpsout);
        }
        else
        {
            // Make a copy of the directory entry
            //fat_dir_entry = (fat_fat_dir_entry_t *)memcpy(&fat_dir_local, fat_dir_entry, sizeof(fat_fat_dir_entry_t));
        	PRINT_Log("\r\nFound directory ok");
        }
    }
        
    // Delete the directory
    //if (fat_delete_directory(&s->Current_file[i + 1], fat_dir_entry))
    //if (!f_unlink(&s->Current_file[i + 1]))
    if (!f_unlink(file_name))
    {
    	PRINT_Log("\r\nDeleted OK");
        PSOCK_SEND_STR(&s->cmdpsout, Resp250);
    }
    else
    {
    	PRINT_Log("\r\nFailed to delete dir. ERROR %d",f_unlink (file_name));
        PSOCK_SEND_STR(&s->cmdpsout, Resp550);
    }
    //return back to original dir
    f_chdir(s->Current_path);
   	rc = f_opendir(&s->dir,s->Current_path);
   	//f_getcwd(s->Current_path, _MAX_LFN);

    
    PSOCK_END(&s->cmdpsout);
}


static unsigned short Generate_help_data( void *state )
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    int len;

    if ((7 == (s->count & 7)) || (NUM_COMMANDS - 2 == s->count))
    {
        len = sprintf((char*)uip_appdata, "    %-4s\r\n", Cmd_dictionary[s->count].Name);
    }
    else
    {
        len = sprintf((char*)uip_appdata, "    %-4s", Cmd_dictionary[s->count].Name);
    }        
    return len;
}


PT_THREAD(Cmd_help(ftpd_state_t *s))
{
    PSOCK_BEGIN(&s->cmdpsout);

    PRINT_Log("\r\nHELP Command");

    PSOCK_SEND_STR(&s->cmdpsout, Resp214A);

    for (s->count = 0; s->count < (NUM_COMMANDS - 1); s->count++)
    {
        PSOCK_GENERATOR_SEND(&s->cmdpsout, Generate_help_data, s);
    }
    
    PSOCK_SEND_STR(&s->cmdpsout, Resp214B);

    PSOCK_END(&s->cmdpsout);
}


// Data connection handling from here on
static unsigned short Generate_list_data( void *state )
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    //fat_fat_dir_entry_t *fat_dir_entry;
    u16_t result = 0;
    char filename[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    unsigned int wrt_date;
    char *data = (char*)uip_appdata;
    static FILINFO fno;
    //static DIR dir ;
    uint32_t rc ;

    //f_chdir(s->Current_path);

    //if(fno.fsize == 0 && fno.fattrib == 0)
    if(s->dir.fs == NULL && fno.fsize == 0 && fno.fattrib == 0)
    {
    	PRINT_Log("\n\r Generate_list_data init directory structure");
    	rc = f_opendir(&s->dir,s->Current_path);
    	f_readdir(&s->dir, NULL);
    }

    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
    //s->count = 0 ;
    char *fn;

    //print out the directory
    //then get inside one sub and print out the content
    //f_readdir(&dir, NULL);


    //do{
    rc = f_readdir(&s->dir, &fno);
    PRINT_Log("\n\r f_readdir return %x , filename %s ", rc,fno.lfname);
   //if(rc != 0)
   // 	break;
   	fn = *fno.lfname ? fno.lfname : fno.fname;

    if (s->count == 0 )
    {
    	PRINT_Log("\r\nFailed to get directory entry %u. ERROR ", s->count);
        s->state = TRANSFER_ABORT;
    }

   	wrt_date = fno.fdate;
   	        result = sprintf(data, "%c--------- 1 ftp ftp %10lu %s %2d  %d %s\r\n",
   	            ((fno.fattrib & AM_DIR) ? 'd' : '-'),
   	            fno.fsize,
   	            months[((wrt_date >> 5) & 0x0f) - 1],
   	            (wrt_date & 0x1f),
   	            ((wrt_date >> 9) + 1980),
   	         fn);

//finished
   	     if (s->count == 0 )
   	     {
   	     	PRINT_Log("\r\nFinished List" );
   	         s->state = TRANSFER_DONE;
   	         //reset the f_readdir
   	    	f_readdir(&s->dir, NULL);
   	     }

   	//s->count++;

    //}while(fno.fname[0] != 0);



    
#if 0
    else if (!fat_is_long_filename_entry(fat_dir_entry))
    {
        fat_fatfilename_to_filename(filename, fat_dir_entry->DIR_Name);
        wrt_date = (unsigned int)swap16(fat_dir_entry->DIR_WrtDate);
        result = sprintf(data, "%c--------- 1 ftp ftp %10lu %s %2d  %d %s\r\n",
            ((fat_dir_entry->DIR_Attr & FAT_FILEATTR_DIRECTORY) ? 'd' : '-'),
            swap32(fat_dir_entry->DIR_FileSize),
            months[((wrt_date >> 5) & 0x0f) - 1],
            (wrt_date & 0x1f),
            ((wrt_date >> 9) + 1980),
            filename);
    }
#endif
    return result;
}

#if 0
static unsigned short Generate_part_of_file(void *state)
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    unsigned int error;
    DIR dir;
    FRESULT rc;

    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "rb+")))
    f_opendir(&dir, s->Current_path);

    //s->File.fs = &USBFs[USB];

    if (0 != f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_OPEN_EXISTING))
    {
        // File open failed (but it must have worked before otherwsie we wouldn't be here!)
        // Set len to 0 and stop the sending process
        s->len = 0;
        PRINT_Log("\r\nFAILED: fat_fopen Error ");
        s->state = TRANSFER_ABORT;
    }
    else
    {
        // Seek to the correct position
        //error = fat_fseek(s->fat_file, s->pos, FAT_SEEK_SET);
    	error = f_lseek (&s->File, s->pos);
        if (0 != error)
        {
            // Seek failed so set the len to 0 and stop the sending process
            s->len = 0;
            PRINT_Log("\r\nFAILED: fat_fseek %d", error);
            s->state = TRANSFER_ABORT;
        }
        else
        {
        	//PRINT_Log("\r\nfread to %d ",uip_mss());
            //s->len = fat_fread((char*)uip_appdata, sizeof(char), uip_mss(), s->fat_file);
        	rc = f_read(&s->File,(char*)uip_appdata,uip_mss(),&s->len);
        	//f_truncate(&s->File);
        	if(s->len < uip_mss() || rc != 0)
        	{
        		PRINT_Log("\r\nFinished reading file with current s->len %d rc %d",s->len,rc);


        	}
        }
        
        // Close the file
        //fat_fclose(s->fat_file);
        f_close(&s->File);
    }
    
    return s->len;
}


static u16_t Append_part_of_file( void *state, u8_t *data, u16_t len)
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    unsigned int error;
    DIR dir;
    FRESULT rc ;


    //f_opendir(&dir, s->Current_path);

    // Open the file for read and append (which allows writing)
    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "rb+")))
    //PRINT_Log("\r\nf_open: %s  file handler %x",s->Current_file , &s->File);
    rc = f_open(&s->File, s->Current_file, FA_WRITE | FA_OPEN_ALWAYS) ;
    //PRINT_Log("\r\nf_open: %s  file handler %x size of the file %d" ,s->Current_file , &s->File,s->File.fsize);
    if(rc == 0)
    {
    	rc = f_lseek(&s->File, s->File.fsize);
    }


    //if (0 != f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_CREATE_ALWAYS))
    if (0 != rc)
    {

        // File open failed (but it must have worked before otherwsie we wouldn't be here!)
        // Stop the receiving process
    	PRINT_Log("\r\nFAILED: fat_fopen Error %d",rc);
        s->state = TRANSFER_ABORT;
        s->len = 0;
    }
    else
    {
        // Seek to the end of the file
        //error = fat_fseek(s->fat_file, s->pos, FAT_SEEK_SET);

    	//error = f_lseek (&s->File, s->pos);
    	error = 0 ;
        if (0 != error)
        {
            // Seek failed so stop the receiving process

            s->state = TRANSFER_ABORT;
            s->len = 0;
        }
        else
        {
            //s->len = fat_fwrite((char*)data, sizeof(char), len, s->fat_file);

        	rc = f_write(&s->File,(char*)data,len,&s->len);
        	f_truncate(&s->File);
        	//PRINT_Log("\r\nf_write: file size %d" ,s->File.fsize);

            // Check all the data was written
            if (s->len != len || rc != 0)
            {
            	PRINT_Log("\r\nFAILED: fat_fwrite Error %d",rc);
                s->state = TRANSFER_ABORT;
            }
        }
        
        // Close the file
        //fat_fclose(s->fat_file);
        f_close(&s->File);
    }
    
    return s->len;
}
#else
static unsigned short Generate_part_of_file(void *state)
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    unsigned int error;
    //DIR dir;
    FRESULT rc;

    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "rb+")))
    //f_opendir(&dir, s->Current_path);

    //s->File.fs = &USBFs[USB];

    if (&s->File == NULL)
       {
           // File open failed (but it must have worked before otherwsie we wouldn't be here!)
           // Stop the receiving process
       	PRINT_Log("\r\nFAILED: File object Error ");
           s->state = TRANSFER_ABORT;
           s->len = 0;
       }
    else
    {
       	//PRINT_Log("\r\nfread to %d ",uip_mss());
       //s->len = fat_fread((char*)uip_appdata, sizeof(char), uip_mss(), s->fat_file);
      	rc = f_read(&s->File,(char*)uip_appdata,uip_mss(),&s->len);
       	//f_truncate(&s->File);
       	if(s->len < uip_mss() || rc != 0)
       	{
       		PRINT_Log("\r\nFinished reading file with current s->len %d rc %d",s->len,rc);
       	}
    }

    // Close the file
    //fat_fclose(s->fat_file);
    //f_close(&s->File);


    return s->len;
}
static u16_t Append_part_of_file( void *state, u8_t *data, u16_t len)
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    unsigned int error;
    //DIR dir;
    FRESULT rc;


    //f_opendir(&dir, s->Current_path);

    // Open the file for read and append (which allows writing)
    //if (NULL == (s->fat_file = fat_fopen(s->Current_file, "rb+")))

    if (&s->File == NULL)
    {
        // File open failed (but it must have worked before otherwsie we wouldn't be here!)
        // Stop the receiving process
    	PRINT_Log("\r\nFAILED: File object Error ");
        s->state = TRANSFER_ABORT;
        s->len = 0;
    }
    else
    {

			rc = f_write(&s->File,(char*)data,len,&s->len);
			f_truncate(&s->File);

            // Check all the data was written
            if (s->len != len || rc != 0)
            {
            	PRINT_Log("\r\nFAILED: fat_fwrite Error with %d",rc);
                s->state = TRANSFER_ABORT;
            }



    }

    return s->len;
}

//Create File
u8_t CreateFile(void *state , BOOL_8 read_only)
{
    ftpd_state_t *s = (ftpd_state_t *)state;
    FRESULT rc = 0 ;
    if(read_only)
    {
    	rc = f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_OPEN_EXISTING) ;
    }
    else
    {
    	//we need to check if this is reopen or creating new
    	if(s->len == 0)
    	{
    		rc = f_open(&s->File, s->Current_file, FA_READ |FA_WRITE| FA_OPEN_ALWAYS) ;
    	}


    }


    if (0 != rc)
        {
            // File open failed (but it must have worked before otherwsie we wouldn't be here!)
            // Stop the receiving process
        	PRINT_Log("\r\nFAILED: fat_fopen Error %d",rc);
            s->state = TRANSFER_ABORT;
        }

    if(read_only)
    {
    	//seek to the position required
    	f_lseek(&s->File,s->pos);

    }
    else
    {
		//seek the file pointer to the end of the file to make sure it is appended
		if(s->File.fsize > 0)
		{
			PRINT_Log("\r\nAppending File with size %d",s->File.fsize);
			f_lseek(&s->File,s->File.fsize);
		}

    }
    return rc ;


}

//Close file
u8_t CloseFile(void *state)
{
	PRINT_Log("\r\nClose File ");
	FRESULT rc;
    ftpd_state_t *s = (ftpd_state_t *)state;
    rc = f_close(&s->File);
    return rc;
}
#endif

PT_THREAD(handle_data_connection( ftpd_state_t *s ))
{
    PSOCK_BEGIN(&s->dataps);

    PRINT_Log("\r\nhandle_data_connection ");

    // Check if we should close this connection
    if (QUITTING == s->state)
    {
        s->state = FTP_UNUSED;
        s->data_conn = NULL;
        PSOCK_CLOSE_EXIT(&s->dataps);
    }
    else if (TRANSFER_LIST == s->state)
    {
        while ((s->count--) && (TRANSFER_LIST == s->state))
        {
            PSOCK_GENERATOR_SEND(&s->dataps, Generate_list_data, s);
        }
        // Check from an error
        if (TRANSFER_LIST == s->state)
        {
            s->state = TRANSFER_DONE;
        }
        // Close the data connection
        PSOCK_CLOSE_EXIT(&s->dataps);
    }
    else if (TRANSFER_SEND == s->state)
    {
    	PRINT_Log("\r\nSending data ");
        // Set the file position to the start
#if 0
        s->pos = 0;
        do
        {
            PSOCK_GENERATOR_SEND(&s->dataps, Generate_part_of_file, s);
            s->pos += (long)s->len;

        } while(s->len == uip_mss() );
        // Check from an error
        if (TRANSFER_SEND == s->state)
        {
            s->state = TRANSFER_DONE;
        }
#else
        s->pos = 0;
        PRINT_Log("\r\nOpening the File for sending");
        CreateFile(s,TRUE); //reading only
        do
        {
            PSOCK_GENERATOR_SEND(&s->dataps, Generate_part_of_file, s);
            s->pos += (long)s->len;

        } while(s->len == uip_mss() );
        // Check from an error
        if (TRANSFER_SEND == s->state)
        {
            s->state = TRANSFER_DONE;
        }
       	PRINT_Log("\r\nClosing the File for Sending");
       	CloseFile(s);

#endif
        // Close the data connection
        PSOCK_CLOSE_EXIT(&s->dataps);
    }
    else if (TRANSFER_RECV == s->state)
    {
    	PRINT_Log("\r\nReceiveing data ");
#if 0
        // Set the file position to the start
        s->pos = 0;
        do
        {
            PSOCK_SINK_RECEIVE(&s->dataps, Append_part_of_file, s);
            s->pos += (long)s->len;
        } while (TRANSFER_RECV == s->state);
 #else
        // Set the file position to the start
        //Create the file first

       s->pos = 0;
//       PRINT_Log("\r\nOpening the File ");
//       CreateFile(s,FALSE);


        do
        {
        	// PRINT_Log("\r\nLooping with s->len %d with state  = %d",s->len,s->state);
            PSOCK_SINK_RECEIVE(&s->dataps, Append_part_of_file, s);
            s->pos += (long)s->len;
        } while (TRANSFER_RECV == s->state);


       	//PRINT_Log("\r\nClosing the File");
       	//CloseFile(s);
#endif
        // Close the data connection
         PSOCK_CLOSE_EXIT(&s->dataps);

    }

    PSOCK_END(&s->dataps);



}


