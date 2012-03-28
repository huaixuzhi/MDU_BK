/*****************************************************
Name:mdutalkback.h
Author:huaixuzhi
Description:mdutalkback function
Version:
Function List:
    @
History:
    <author>    <time>  <version>   <desc>
 ****************************************************/

#ifndef _MDUTALKBACK_H_
#define _MDUTALKBACK_H_

#include "mdusock.h"

gint talkBackOpen(void *data);
gint talkBackClose(void *data);

gint broadcastOpen(void *data,gssize len);
gint broadcastClose(void *data,gssize len);

gint DataSendtoCsg(gchar *data);

#endif

