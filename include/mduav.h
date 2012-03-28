/*****************************************************
Name:transfer.h
Author:huaixuzhi
Description:definition of transfer_link structures
Version:
Function List:
    @
History:
    <author>    <time>  <version>   <desc>
 ****************************************************/

#ifndef	_TRANSFER_H_
#define	_TRANSFER_H_
#include "mdulist.h"

gint    parseEXTRAMSG(gchar* buf , void* pval);
gint 	stopMsgToCSG(struct  _mdutransferentry  *transfer);
void odDataOpen(void *data, gshort type, guint len, guint flag);

void odDataClose(void *data,gshort type);

#endif
