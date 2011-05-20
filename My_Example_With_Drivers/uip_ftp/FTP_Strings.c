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

FILE - FTP_Strings.c

DESCRIPTION
    Definition of the strings used by the FTP Daemon.

MODIFICATION DETAILS
=============================================================================*/

char *Resp150 = "150 Opening data connection\n";
char *Resp200 = "200 Command OK\n";
char *Resp220 = "220 LPC1768 FTP Server\n";
char *Resp221 = "221 Closing control connection\n";
char *Resp226 = "226 Closing data connection\n";
char *Resp230 = "230 User logged in\n";
char *Resp227 = "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\n";
char *Resp250 = "250 Command complete\n";
char *Resp257 = "257 \"%s\"\n";
char *Resp421 = "421 Service Unavailable\n";
char *Resp425 = "425 Cannot open data connection\n";
char *Resp426 = "426 Transfer aborted\n";
char *Resp451 = "451 Aborted. Local error\n";
char *Resp500 = "500 Syntax error, command unrecognised\n";
char *Resp501 = "501 Syntax error in parameters or arguments\n";
char *Resp502 = "502 Command not implemented\n";
char *Resp550 = "550 Action not taken\n";
