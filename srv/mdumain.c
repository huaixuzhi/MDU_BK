/**************************************************************
Name:mdumain.c
Description:main function,As an entrance of the function
Author;huaixuzhi
Function List:
	    @main               main
    Initialize log of mdu,and the thread,and as the 
    entrance of mdu_function
History:
    <author>    <time>  <version>   <desc>
			 2012-1-9				framework
  *************************************************************/

#include <glib.h>
#include "toplib.h"
#include "mdusock.h"

gint main(void)
{
	guint	rc;

	/*Start log*/
	top_log_init(MDU_COMMUNIT_DOMAIN,MDU_COMMUNIT_LOG_PATH,MDU_COMMUNIT_LOG_FILE);
	top_init_sock();

	if( !g_thread_supported() )
	{
		g_thread_init(NULL);
	}

	rc = mdusockfunc();
	if(rc != TOP_SUCCESS)
	{
		g_error("mdusockfunc fail,rc = %d ",rc);
		return rc;
	}
	while(1)
	{
		g_usleep(MAINSLEEPTIME * 1000000);
	}
	return TOP_SUCCESS;
}
