/****************************************************
Name:transfer.c
Author:huaixuzhi
Description:operation of transfer_link,about insert,del,
        look for and so on
Version:
Function List:
    @
History:
    <author>    <time>      <version>   <desc>
    huaixuhi   2011-6-17
 ****************************************************/
#include <glib.h>
#include "toplib.h"
#include "mdulist.h"

/*
 Description:add PU and CU information into list
@argv:
    argv[0]        structure can uniquely identify a stream
    argv[1]        structure of CULINK
@return:
    success
    fail
*/

gint    avInsertList(void *keytmp,void *culinktmp,
		top_sockaddr_in *csgaddr,guint	nodeFlag)
{
   	gboolean    bFind = FALSE;
	/*if pu is in list,flag = 0;if pu isn't in list;flag = 1*/
    gint ReFlag = 1, i = 0;    
    struct _mdutransferentry *pTransfer = NULL;
    struct _cu_link	*pculink	= (struct _cu_link *)culinktmp;
    struct _cu_link	*pculinkTag	= NULL;
	STREAMKEY   *pKey			= (STREAMKEY*) keytmp;
    GList    *pFirst = NULL,*pCurrent = NULL;
	GTimeVal tnow;

    g_mutex_lock(m_transferListMutex);
    /*Get the first element in m_pTransferList*/
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;

    /*Search stream from tansfer table*/
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutransferentry*)pCurrent->data;
        if(pTransfer == NULL)
        {
            //error
        }
        if(pTransfer->mStreamKey.mpuid == pKey->mpuid &&
                pTransfer->mStreamKey.mchannelid  == pKey->mchannelid &&
                pTransfer->mStreamKey.mstreamtype == pKey->mstreamtype &&
                pTransfer->mStreamKey.mmediatype  == pKey->mmediatype)
        {
            //find it
            bFind = TRUE;
            ReFlag = 0;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( !bFind )
    {
        pTransfer = NULL;
        //find free space and insert
        for(i = 0;i < MAX_CALLS;i++)
        {
            /*Judge free or not*/
            if(m_TransferTable[i].mCUList == NULL)
            {
                pTransfer = &m_TransferTable[i];
				/*keep the fisrt cuid -- this puinfo 2012-2-21*/
				pTransfer->firstcuid = pculink->mCUId;

                pTransfer->mStreamKey.mpuid = pKey->mpuid;
                pTransfer->mStreamKey.mchannelid  = pKey->mchannelid;
                pTransfer->mStreamKey.mstreamtype = pKey->mstreamtype;
                pTransfer->mStreamKey.mmediatype  = pKey->mmediatype;
                /*Added in 2011-10-20*/
                pTransfer->nextAddr = *csgaddr;
				/*Added in 2012-2-14,time about the pu*/
				if(nodeFlag == FIRST_MDU_NODE)
				{
					/*The first mdu in nodes*/
					g_get_current_time(&tnow);
					pTransfer->puLastTime	= tnow.tv_sec;
					pTransfer->mduNode		= FIRST_MDU_NODE;
				}
				else if (nodeFlag == OTHER_MDU_NODE)
					pTransfer->mduNode		= OTHER_MDU_NODE;
                break;
            }
        }
        if( pTransfer != NULL)
        {
            /*add pu in list*/
            m_pTransferList = g_list_append(m_pTransferList,(void*)pTransfer);
        }
    }

	/*search cu information from list*/
    bFind = FALSE;
    if(pTransfer != NULL)
    {
        //search CU information
        g_mutex_lock(m_subscribeListMutex);
        pFirst = g_list_first(pTransfer->mCUList);
        pCurrent = pFirst;
        while(pCurrent != NULL)
        {
            pculinkTag = (CULINK*)pCurrent->data;
            if(pculinkTag == NULL)
            {
                //error
            }
            if( pculinkTag->mCUId == pculink->mCUId )
            {
                //CU exist in List,and need not operate
                g_mutex_unlock(m_subscribeListMutex);
                g_mutex_unlock(m_transferListMutex);
                return ReFlag;
            }
            pCurrent = g_list_next(pCurrent);
        }

        /*search in CULINK,Judge if exist or not*/
        pFirst = g_list_first(m_pCUList);
        pCurrent = pFirst;
        while(pCurrent != NULL)
        {
            pculinkTag = (CULINK*)pCurrent->data;
            if(pculinkTag == NULL)
            {
                //error
            }
            if( pculinkTag->mCUId == pculink->mCUId )
            {
                bFind = TRUE;
                break;
            }
            pCurrent = g_list_next(pCurrent);
        }
        if( !bFind )
        {
			GTimeVal tnow;
            //CU infomation don't exist in table
            //To find free space to insert CU info
            pculinkTag = NULL;          
            //search free space to insert
            for( i = 0;i < MAX_CALLS;i++)
            {
                //Judge space free or not,if CURefCnt == 0 :free
                if(m_cuLinkArray[i].CURefCnt == 0)
                {
                    g_get_current_time(&tnow);
                    //Find free space and insert CU info into List
                    pculinkTag = &m_cuLinkArray[i];
                    pculinkTag->mCUId = pculink->mCUId;
					pculinkTag->transAddr = pculink->transAddr; 
                    pculinkTag->cuLastTime = tnow.tv_sec;

                    m_pCUList = g_list_append(m_pCUList,(void *)pculinkTag);
                    break;
                }
            }/*end of for()*/
        }/*end of if(!bFind)*/
        if(pculinkTag != NULL){
        //modify in 2011-8-22
            pculinkTag->CURefCnt++;
            pTransfer->mCUList = g_list_append(pTransfer->mCUList,(void*)pculinkTag);
         ///////////
        }
        g_mutex_unlock(m_subscribeListMutex);
    }
    g_mutex_unlock(m_transferListMutex);
    return ReFlag;
}

/*
   *Delete PU/CU information from the transfer table
   *NOTE:CU is to compare address
@argv:
    argv[0]        structure can uniquely identify a stream
    argv[1]        structure of CULINK
@return:
    success
    fail
*/
int    removeTransfer(void *key,void *culinkTmp)
{
    STREAMKEY	*pKey	= (STREAMKEY*)key;
    CULINK	*pculink	= (CULINK*)culinkTmp;
    CULINK	*pculinkTag = NULL;
    gboolean    bFind	= FALSE;
    gint        ReFlag	= 1;            //To judge if the CU List of one pu
                                    //is empty. 1  not;0  do empty
    struct    _mdutransferentry    *pTransfer = NULL;
    GList    *pFirst = NULL,*pCurrent = NULL;

    /*Get the first element form m_pTransferList;
     This list keep information of _mdutransferentry;
     and we can operate PU&CU thougth the list*/
    g_mutex_lock(m_transferListMutex);
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;
    /*Search information of PU from m_pTransferList
          list->data can get _mdutransferentry
         _mdutransferentry->mStreamKey get info of PUs*/
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutransferentry*)pCurrent->data;
        if(pTransfer == NULL)
        {
            //put out information of error
        }
        if(pTransfer->mStreamKey.mpuid == pKey->mpuid &&
                pTransfer->mStreamKey.mchannelid ==    pKey->mchannelid &&
                pTransfer->mStreamKey.mstreamtype == pKey->mstreamtype &&
                pTransfer->mStreamKey.mmediatype == pKey->mmediatype)
        {
            //find it
            bFind    =    TRUE;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if(! bFind ){
        /*There is no information of PU in the list.
          return delete success*/
           g_mutex_unlock(m_transferListMutex);
        return ( DEL_SUCCESS + 2);
    }

    /***********************************************/
    /*Search in CULINK,and judge if info exits or not*/
    /*************************************************/
    bFind = FALSE;
    g_mutex_lock(m_subscribeListMutex);
    pFirst = g_list_first(pTransfer->mCUList);
    pCurrent = pFirst;
    while(pCurrent != NULL)
    {
        pculinkTag = (CULINK*)pCurrent->data;
        if(pculinkTag == NULL)
        {
            //error
        }
        if( pculinkTag->mCUId == pculink->mCUId )
        {
            //CU exist in List
            bFind = TRUE;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( !bFind)
    {
        /*The information of CU don't exist in the list;
          and return success*/
        g_mutex_unlock(m_transferListMutex);
        g_mutex_unlock(m_subscribeListMutex);
        return (DEL_SUCCESS + 2);
    }

    /****modify by huaixuzhi 2011-8-12 *****/
    /*Remove*/
    /*Search for node of CU*/
    if( g_list_find(pTransfer->mCUList,pculinkTag) != NULL)
    {
        pTransfer->mCUList = g_list_remove(pTransfer->mCUList,(void *)pculinkTag);

        if( g_list_length(pTransfer->mCUList) == 0 ){
            // lock
            m_pTransferList = g_list_remove(m_pTransferList,(void*)pTransfer);
            memset(pTransfer,0,sizeof(struct _mdutransferentry));
            ReFlag = 0;
        }
        pculinkTag->CURefCnt--;
        if(pculinkTag->CURefCnt == 0){
            // lock
            m_pCUList = g_list_remove(m_pCUList,(void*)pculinkTag);
            memset(pculinkTag,0,sizeof(CULINK));
        }
        g_mutex_unlock(m_transferListMutex);
        g_mutex_unlock(m_subscribeListMutex);
        if ( !ReFlag )
        {
            /*The Cu List of one PU is empty*/
            return TOP_TRUE;
        }
        else
        {
            return TOP_FALSE;
        }
    }
    g_mutex_unlock(m_transferListMutex);
    g_mutex_unlock(m_subscribeListMutex);
    return 3;
}


/*
 Description:add CSG/MDU and CU information into list,use for talkback
    and broadcast.One CU corresponds one or more CSG/MDU.
@argv:
    key            structure can uniquely identify a stream
    insertLink    structure of csgLink or next mduLink.keeping useful info
@return:
    success        TOP_SUCCESS
    fail        others
*/
gint    talkBackAddList(void *key,void *insertLink)
{
    gboolean    bFind         = FALSE;
    guint i;            //Using for loop

    CULINK     *pKey          = (CULINK *)key;
    CSGKEYTAG  *csgKey        = (CSGKEYTAG *)insertLink;
    CSGKEYTAG  *csgLinkTag    = NULL;
    GList      *pFirst = NULL,*pCurrent = NULL;
    struct      _mdutalkback  *pTransfer = NULL;

    /*Get the first element in m_pTransferList*/
    pFirst = g_list_first(m_pTalkBackList);
    pCurrent = pFirst;
    /*Search stream from transfer table*/
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutalkback *)pCurrent->data;
        if(pTransfer == NULL)
        {
            //error
        }
        if(pTransfer->mCUStreamKey.mCUId == pKey->mCUId)
        {
            /*find stream from cu in the list */
            bFind = TRUE;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( !bFind )
    {
        /*No cu info in list and should insert*/
        pTransfer = NULL;
        /*find free space and insert*/
        for(i = 0;i < MAX_CALLS;i++)
        {
            /*Judge free or not*/
            if(m_TalkBackTable[i].mCSGList == NULL)
            {
                pTransfer = &m_TalkBackTable[i];
                pTransfer->mCUStreamKey.mCUId = pKey->mCUId;
				pTransfer->mCUStreamKey.transAddr = pKey->transAddr;
                break;
            }
        }
    }
    if( pTransfer != NULL)
    {
        m_pTalkBackList = g_list_append(m_pTalkBackList,(void*)pTransfer);
    }

    /*cu in list and search for csg/mdu*/
    bFind = FALSE;
    if(pTransfer != NULL)
    {
        //search pu information
        pFirst = g_list_first(pTransfer->mCSGList);
        pCurrent = pFirst;
        while(pCurrent != NULL)
        {
            csgLinkTag = (CSGKEYTAG *)pCurrent->data;
            if( csgLinkTag == NULL)
            {
                //error
            }
            if(csgLinkTag->uid == csgKey->uid)
            {
                //csg exist in List,and need not operate
                 bFind = TRUE;
                 break;
            }
            pCurrent = g_list_next(pCurrent);
        }
        if( !bFind )
        {
            //csg infomation doesn't exist in table
            //To find free space to insert csg info
            csgLinkTag = NULL;
            //search free space to insert
            for( i = 0;i < MAX_CALLS;i++)
            {
                //Judge space free or not,if refCnt == 0 :free
                if(m_CSGLink[i].refCnt == 0)
                {
                    //Find free space and insert csg info into List
                    csgLinkTag = &m_CSGLink[i];
					csgLinkTag->nextAddr = csgKey->nextAddr;
                    csgLinkTag->uid = csgKey->uid;
                    break;
                }
            }
        }
        if( csgLinkTag != NULL){
            csgLinkTag->refCnt++;
            pTransfer->mCSGList = g_list_append(pTransfer->mCSGList,(void*)csgLinkTag);
        }
    }
    return TOP_SUCCESS;
}

/*
   *Delete CU/CSG information from the transfer table
@argv:
    key         structure can uniquely identify a stream
        This is CU information
@return:
    success
    fail
*/
gint    talkBackRemove(gint cuid)
{
    gboolean    bFind = FALSE;
    gint        ReFlag = 1;            //To judge if the CU List of one pu
                                    //is empty. 1  not;0  do empty
    struct    _mdutalkback    *pTransfer = NULL;
    CSGKEYTAG   *csgLinkTag = NULL;
    GList    *pFirst = NULL,*pCurrent = NULL;

    /*Get the first element form m_pTalkBackList;
     This list keep information of _mdutalkback;
     and we can operate CU&CSG/MDU thougth the list*/
    pFirst = g_list_first(m_pTalkBackList);
    pCurrent = pFirst;
    /*Search information of CU from m_pTalkBackList
          list->data can get _mdutalkback
         _mdutalkback->mCUStreamKey get info of CUs*/
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutalkback *)pCurrent->data;
        if(pTransfer == NULL)
        {
            //put out information of error
        }
        if(pTransfer->mCUStreamKey.mCUId == cuid)
        {
            //find it
            bFind = TRUE;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( !bFind ){
        /*There is no information of CU in the list.
          return delete success*/
        return (DEL_SUCCESS + 1);
    }

    /*There is cu in the list and then to search for
     csg information.*/
    bFind = FALSE;
    pFirst = g_list_first(pTransfer->mCSGList);
    pCurrent = pFirst;
    while(pCurrent != NULL)
    {
        bFind = TRUE;
        csgLinkTag    = (CSGKEYTAG *)pCurrent->data;
        if(csgLinkTag == NULL)
        {
            //error
        }
        /*Delete csg info from list*/
        pTransfer->mCSGList = g_list_remove(pTransfer->mCSGList,(void *)m_pCSGList);
        if( g_list_length(pTransfer->mCSGList) == 0 ){
            /*judge whether csg list emputy or not*/
            m_pTalkBackList = g_list_remove(m_pTalkBackList,(void*)pTransfer);
            memset(pTransfer,0,sizeof(struct _mdutalkback));
            ReFlag = 0;
        }
        csgLinkTag->refCnt--;
        if(csgLinkTag->refCnt == 0){
            /*NOTE:Need lock */
            m_pCSGList = g_list_remove(m_pCSGList,(void*)csgLinkTag);
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( !bFind)
    {
        /*The information of csg don't exist in the list;
          and return success*/
        return (DEL_SUCCESS);
    }
    return TOP_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////
//Added by huaixuzhi in 2011-10-20
//Description:when Cu's heartback is time out,then we use this function to
//..delete CU infos in the list,if PU has no CU after delete,destory PU info-
//..rmation.
gint    removeCUInfo(gint mcuid)
{
    CULINK        *pculinkTag = NULL;
    gboolean    bFind = FALSE;

    struct    _mdutransferentry    *pTransfer = NULL;
    GList    *pFirst = NULL,*pCurrent = NULL;
    GList    *pcuFirst = NULL,*pcuCurrent = NULL;

    /*Get the first element form m_pTransferList;
     This list keep information of _mdutransferentry;
     and we can operate PU&CU thougth the list*/
    g_mutex_lock(m_transferListMutex);
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;
    /*Search information of PU from m_pTransferList
          list->data can get _mdutransferentry
         _mdutransferentry->mStreamKey get info of PUs*/
    while(pCurrent != NULL)
    {
        bFind = FALSE;
        pTransfer = (struct _mdutransferentry*)pCurrent->data;

        //g_mutex_lock(m_subscribeListMutex);
        pcuFirst = g_list_first(pTransfer->mCUList);
        pcuCurrent = pcuFirst;

        /*Search CUs in the PU*/
        while(pcuCurrent != NULL)
        {
            pculinkTag = (CULINK*)pcuCurrent->data;
            if(pculinkTag != NULL)
            {
            }
            if( pculinkTag->mCUId == mcuid )
            {
                pcuCurrent = g_list_next(pcuCurrent);
                //CU exist in List
                bFind = TRUE;
                top_log(MDU_COMMUNIT_DOMAIN,"In mdulist.c 550");
                if( g_list_find(pTransfer->mCUList,pculinkTag) != NULL)
                {
                    /*delete cu information*/
                    pTransfer->mCUList = g_list_remove(pTransfer->mCUList,
                            (void *)pculinkTag);
                    pculinkTag->CURefCnt--;
                }
                if(pculinkTag->CURefCnt == 0)
                {
                       // lock
                    m_pCUList = g_list_remove(m_pCUList,(void*)pculinkTag);
                    memset(pculinkTag,0,sizeof(CULINK));
                }
                break;
            }
            else 
                pcuCurrent = g_list_next(pcuCurrent);
        }
        if( !bFind)
        {
            /*The information of CU don't exist in the list;
              and return success*/
            //g_mutex_unlock(m_subscribeListMutex);
            //continue;
        }
        if( g_list_length(pTransfer->mCUList) == 0 )
        {
            //If there is no cu in the pu,remove the pu from List
            /*Send pu stop message to csg*/
            if (TOP_SUCCESS != stopMsgToCSG(pTransfer))
            {
                g_message("Remove Fail because cu timeout");
            }
            top_log(MDU_COMMUNIT_DOMAIN,"g_list_length(pTransfer->mCUList) == 0");
            m_pTransferList = g_list_remove(m_pTransferList,(void*)pTransfer);
            memset(pTransfer,0,sizeof(struct _mdutransferentry));
        }
        //g_mutex_unlock(m_subscribeListMutex);
        /*Remove*/
        /*Search for node of CU*/
        pCurrent = g_list_next(pCurrent);
    }
    g_mutex_unlock(m_transferListMutex);

    top_log(MDU_COMMUNIT_DOMAIN,"THAT'S REMOVE ALL THE TIMEOUT CUS");
    return TOP_SUCCESS;
}

//add by huaixuzhi in 8-23
//when increase a content in the list,write all the list in log
//
gint showListLog(void)
{

	gchar	*addrbuf = NULL;
	gint	cuport = 0;
    struct    _mdutransferentry    *pTransfer = NULL;
    CULINK    *pculinkTag = NULL;
    GList    *pFirst = NULL,*pCurrent = NULL;
    GList   *pSecond = NULL,*pCUCurrent = NULL;                 //List keeping CU

    g_mutex_lock(m_transferListMutex);
    /*Get the first element in m_pTransferList*/
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;

    /*Search stream from tansfer table*/
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutransferentry*)pCurrent->data;
        if(pTransfer != NULL)
        {
            g_message("##################The List##################");
            g_message("PU ID is %x CHANNEL ID is %x",
                    pTransfer->mStreamKey.mpuid,pTransfer->mStreamKey.mchannelid);
            g_message("##########CU LIST CORRESPONDING PU##########");
            top_log(MDU_COMMUNIT_DOMAIN,"################The List################");
            top_log(MDU_COMMUNIT_DOMAIN,"PUID=%x CHANNELID=%x STREAMID=%x MEDIA=%d",
                    pTransfer->mStreamKey.mpuid,pTransfer->mStreamKey.mchannelid,
                    pTransfer->mStreamKey.mstreamtype,pTransfer->mStreamKey.mmediatype);
            top_log(MDU_COMMUNIT_DOMAIN,"#######CU LIST CORRESPONDING PU#######");

            g_mutex_lock(m_subscribeListMutex);
            pSecond =   g_list_first(pTransfer->mCUList);
            pCUCurrent = pSecond;
            while(pCUCurrent != NULL)
            {
                pculinkTag = (CULINK *)pCUCurrent->data;
                if(pculinkTag != NULL)
                {
					addrbuf = top_inet_ntoa(pculinkTag->transAddr.sin_addr);
					cuport = ntohs(pculinkTag->transAddr.sin_port);
					g_message("CU IP %s PORT IS %d ID IS %x",
							addrbuf,cuport,pculinkTag->mCUId);
                    top_log(MDU_COMMUNIT_DOMAIN,"CU IP %s PORT IS %d ID IS %x",
							addrbuf,cuport,pculinkTag->mCUId);
                }
                pCUCurrent = g_list_next(pCUCurrent);
            }
            g_message("###############END OF ONE PU################");
            top_log(MDU_COMMUNIT_DOMAIN,"#############END OF ONE PU##############");
            g_mutex_unlock(m_subscribeListMutex);
        }
        pCurrent = g_list_next(pCurrent);
    }
    g_mutex_unlock(m_transferListMutex);
    return TOP_SUCCESS;
}

