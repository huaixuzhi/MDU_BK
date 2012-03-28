/*****************************************************
Filename:mdutalkback.c            
Author:huaixuzhi
Description:about function of talking back
Version:
Function List:
    @  
History:
    <author>    <time>  <version>   <desc>
****************************************************/

#include <glib.h>
#include "toplib.h"
#include "mdusock.h"
#include "mdulist.h"

#define		CSGRTPPORT	10102

/*
Description:Recieve talkbackopen message from cmu,then parse the message.
@argv:  
    data:Message from cmu
@return:
 */
gint talkBackOpen(void *dataMsg)
{
    gint len = 0,lentmp = 0;
    gint addrlen,rc,status;
    guint sTmp;
    gchar opResultTmp;
    gchar msgData[MAX_CALLS] = {0};    //keeping message send to csg

    top_sockaddr_in    csgSock;
 	struct	_cmu_data_tag	cmu_data_tag;
	struct	mduSrvTag		*srv = NULL;

    CULINK       cuKey;
    CSGKEYTAG    csgKey;
    STREAMKEY    dataKey;//Including cu infomation
    MSGHEAD 	 msg_head; 

    g_message("Get message TALK_BACK_OPEN from cmu");
    top_log(MDU_COMMUNIT_DOMAIN,"Get message TALK_BACK_OPEN from cmu");

	memset(&cuKey, 0, sizeof(CULINK));
	memset(&csgKey, 0, sizeof(CSGKEYTAG));
	memset(&dataKey, 0, sizeof(STREAMKEY));
	memset(&cmu_data_tag, 0, sizeof(struct _cmu_data_tag));
    len = sizeof(MSGHEAD);
	srv = m_CsgSockSrv;

    /*Parse message body and insert info into cmu_data_tag*/
    lentmp = parseOPRESULT((char *)dataMsg + len, &cmu_data_tag.opresult);
    if( 0 == lentmp)
    {
        g_error("TalkBack Data open parse opresult");
    }
    len = len + lentmp;
    lentmp = parsePUID((char *)dataMsg + len, &cmu_data_tag.puid);
    if( 0 == lentmp)
    {
        g_error("TalkBack Data open parse puid");
    }
    len = len + lentmp;
    lentmp = parseChannelID((char *)dataMsg + len, &cmu_data_tag.channelid);
    if( 0 == lentmp )
    {
        g_error("TalkBack Data open parse channelid");
    }
    len = len + lentmp;
    lentmp = parseCUINFO((char *)dataMsg + len, &cmu_data_tag.cuinfotag);
    if( 0 == len )
    {
        g_error("TalkBack Data open parse CUINFO");
    }
    len = len + lentmp;
    lentmp = parseCSGINFO((char *)dataMsg + len, &cmu_data_tag.csginfotag);
    if( 0 == len )
    {
        g_error("TalkBack Data open parse CSGINFO");
    }
    len = len + lentmp;
//2012-3-1 I don't think mduroute is in need in talkback
//  lentmp += len;
//  len = parseMDUROUTE(&data[lentmp],&cmu_data_tag->mduroute);

//Added by huaixuzhi in 2011-11-17
	/*get socket information of csg*/
    csgSock.sin_family = AF_INET;
    sTmp = ntohl(cmu_data_tag.csginfotag.uport);
    csgSock.sin_port = htons(sTmp);
    csgSock.sin_addr.s_addr = cmu_data_tag.csginfotag.uip;

    /*Create cuKey including CU information*/
    cuKey.mCUId = cmu_data_tag.cuinfotag.uid;
	cuKey.transAddr.sin_family = AF_INET;
	cuKey.transAddr.sin_addr.s_addr = cmu_data_tag.cuinfotag.uip;
//	sTmp = ntohl(cmu_data_tag.cuinfotag.uport);
//	cuKey.transAddr.sin_port = htons(sTmp);
	cuKey.transAddr.sin_port = htons(CSGRTPPORT);

    /*Create csgKey including CSG information*/
	/*maybe the csgid does not in use LOL*/
    csgKey.uid        = cmu_data_tag.csginfotag.uid;
	csgKey.nextAddr   = csgSock;	

    dataKey.mpuid          = cmu_data_tag.puid;
    dataKey.mchannelid     = cmu_data_tag.channelid;
    dataKey.mstreamtype    = cmu_data_tag.streamid;        //sub/main stream
    dataKey.mmediatype     = MEDIATALK;                //media type is vod/aod

    /*data from csg to cu*/
    avInsertList(&dataKey,&cuKey,&csgSock,0);
    /*Insert CU and CSG info into list;data from cu to csg*/
    rc = talkBackAddList(&cuKey,&csgKey);
    if(rc != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"ERROR:talk Back Add List fail");
		return rc;
    }

    /*Send talkback message to csg*/
    memset(&msg_head,0,sizeof(MSGHEAD));
    msg_head.nsMsgType = htons(CSG_TALKBACK_OPEN);
    len = sizeof(MSGHEAD);
    
	opResultTmp = 0;
    lentmp = buildOPRESULT(msgData + len,&opResultTmp);
    len = len + lentmp;
    lentmp = buildPUID(msgData + len,&cmu_data_tag.puid);
    len = len + lentmp;
    lentmp = buildChannelID(msgData + len,&cmu_data_tag.channelid);
    len = len + lentmp;
    lentmp = buildCUID(msgData + len,&cmu_data_tag.cuinfotag.uid);
    len = len + lentmp;
    lentmp = buildMDUINFO(msgData + len,&srv->mduinfotag);
    len = len + lentmp;
    msg_head.nsTotalLen = g_htons((gushort)len);
    memcpy(msgData,&msg_head,sizeof(MSGHEAD));

    addrlen = sizeof(top_sockaddr_in);
    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    status = activeSock_sendto(srv->asock,&srv->send_key,msgData,&len,0,
            &csgSock,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"ERROR:Send talkback message to csg");
		return status;
    }
    g_message("Send talkback message to csg success");
    top_log(MDU_COMMUNIT_DOMAIN,"Send talkback message to csg success");
	return TOP_SUCCESS;
	
}

/*
Description:Recieve talkbackclose message from cmu,then parse the message.
@argv:  
    data:Message from cmu
    CmuData:structure keep information from cmu
@return:
 */
gint talkBackClose(void *dataMsg)
{
	gchar msgData[MAX_CALLS] = {0};    //keeping message send to csg
    gchar opResultTmp;
    gint  lentmp = 0 ,len = 0;
    gint  addrlen,rc,status;
    gint  sTmp;

    MSGHEAD    msg_head;
  	/*Added by huaixuzhi in 2011-9-20*/
    STREAMKEY  dataKey;
    CULINK     cuKey;
    top_sockaddr_in    CsgSock;
	struct _cmu_data_tag cmu_data_tag;
	struct mduSrvTag *srv = NULL;

    len = sizeof(MSGHEAD);
	srv = m_CsgSockSrv;
	memset(&cuKey, 0, sizeof(CULINK));
	memset(&dataKey, 0, sizeof(MSGHEAD));
	memset(&msg_head, 0, sizeof(MSGHEAD));
	memset(&cmu_data_tag, 0, sizeof(struct _cmu_data_tag));

    g_message("Get message TALK_BACK_CLOSE from cmu");
    top_log(MDU_COMMUNIT_DOMAIN,"Get message TALK_BACK_CLOSE from cmu");

    /*Parse message body and insert info into cmu_data_tag*/
    lentmp = parseOPRESULT((char *)dataMsg+len,&cmu_data_tag.opresult);
    if( 0 == lentmp)
    {
        g_error("TalkBack Data close parse opresult");
    }
    len = len + lentmp;
    lentmp = parsePUID((char *)dataMsg+len,&cmu_data_tag.puid);
    if( 0 == lentmp)
    {
        g_error("TalkBack Data close parse puid");
    }
    len = len + lentmp;
    lentmp = parseChannelID((char *)dataMsg+len,&cmu_data_tag.channelid);
    if( 0 == lentmp )
    {
        g_error("TalkBack Data close parse channelid");
    }
    len = len + lentmp;
    lentmp = parseCUID((char *)dataMsg+len,&cmu_data_tag.cuinfotag.uid);
    if( 0 == len )
    {
        g_error("TalkBack Data close parse CUINFO");
    }
    len = len + lentmp;
    lentmp = parseCSGINFO((char *)dataMsg+len,&cmu_data_tag.csginfotag);
    if( 0 == len )
    {
        g_error("TalkBack Data close parse CSGINFO");
    }
    len = len + lentmp;

    dataKey.mpuid         = cmu_data_tag.puid;
    dataKey.mchannelid    = cmu_data_tag.channelid;
    dataKey.mstreamtype   = 0;
    dataKey.mmediatype    = MEDIATALK;            //Stream type is vod

    cuKey.mCUId = cmu_data_tag.cuinfotag.uid;
    /*Remove CuInfo from the listl;if Cu List in one Pu is empty,remove the pu*/

    rc = removeTransfer(&dataKey,&cuKey);
    g_message("removeTransfer success");
	top_log(MDU_COMMUNIT_DOMAIN,"removeTransfer success");
    showListLog();

    /*Insert CU and CSG info into list*/
    rc = talkBackRemove(cmu_data_tag.cuinfotag.uid);
    if(rc != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"talk Back remove List fail");
		return rc;
    }

    /*Send talkback close  message to csg*/
    /*CSGINFO is from cmu*/
    msg_head.nsMsgType = htons(CSG_TALKBACK_CLOSE);
    len = sizeof(MSGHEAD);
    
    opResultTmp = 0;
    lentmp = buildOPRESULT(msgData+len,&opResultTmp);
    len = len + lentmp;
    lentmp = buildPUID(msgData+len,&cmu_data_tag.puid);
    len = len + lentmp;
    lentmp = buildChannelID(msgData+len,&cmu_data_tag.channelid);
    len = len + lentmp;
    lentmp = buildCUID(msgData + len,&cmu_data_tag.cuinfotag.uid);
    len = len + lentmp;
    lentmp = buildMDUINFO(msgData + len,&cmu_data_tag.mduinfotag);
    len = len + lentmp;
    msg_head.nsTotalLen = g_htons((gushort)len);
    memcpy(msgData,&msg_head,sizeof(MSGHEAD));

    CsgSock.sin_family = AF_INET;
    sTmp = ntohl(cmu_data_tag.csginfotag.uport);
    CsgSock.sin_port = htons(sTmp);
    CsgSock.sin_addr.s_addr = cmu_data_tag.csginfotag.uip;
    addrlen = sizeof(top_sockaddr_in);
    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    status = activeSock_sendto(srv->asock,&srv->send_key,msgData,&len,0,
            &CsgSock,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"ERROR:Send talkback message to csg");
        return status;
    }
    g_message("Send talkbackclose message to csg success");
    top_log(MDU_COMMUNIT_DOMAIN,"Send talkbackclose message to csg success");
	
	return TOP_SUCCESS;
}

/*
Description:Recieve broadcastopen message from cmu,then parse the message.
@argv:  
    data:Message from cmu
    CmuData:structure keep information from cmu
@return:
 */
void broadcastOpen(void *dataMsg,gssize    dataLen)
{ 
}
/*
Description:Recieve broadcastclose message from cmu,then parse the message.
@argv:  
    data:Message from cmu
    CmuData:structure keep information from cmu
@return:
 */
void broadcastClose(void *dataMsg,gssize    dataLen)
{ 
}

/*
Description:recieve data from cu and judge if there are csg(s) -- cu
    in list,then send data to csg
@argv:
	cudata:data buffer from cu to csg
@return:
	success		TOP_SUCCESS
	fail		others
*/
gint	dataSendtoCsg(gchar *cudata)
{
    guint    lcuid,status = 0;
    gssize	 buflen = 0;
    gboolean    bFind = FALSE;
    GList   *pFirst = NULL,*pCurrent = NULL;          //List keeping cu
    GList   *pSecond = NULL,*csgCurrent = NULL;      //List keeping CSG  
    CSGKEYTAG    *csgKey = NULL;
    struct  _media_data_head    mediaDataHead;        //keep head of data from cu
    struct  _mdutalkback        *pTransfer;
	struct  mduSrvTag           *csgSrv;

    bFind = FALSE;
	csgSrv = m_CsgDataSrv;
    /*keeping data head*/
	memset(&mediaDataHead,0,sizeof(struct _media_data_head));
    /*parse datahead from databuf*/
    memcpy(&mediaDataHead,cudata,sizeof(struct _media_data_head));
	/*puid keeping cuid in data from cu*/
    lcuid = mediaDataHead.puid;        
    /*get the length of the stream*/
    buflen = ntohs(mediaDataHead.length) + sizeof(struct _media_data_head);

    /*Search list and send data to csg*/
    pFirst = g_list_first(m_pTalkBackList);
    pCurrent = pFirst;
    while(pCurrent != NULL)
    {
    	pTransfer = (struct _mdutalkback *)pCurrent->data;
        if(pTransfer == NULL)
        {
            //put out information of error 
        }
        if(pTransfer->mCUStreamKey.mCUId == lcuid)
        {
                    /*Find cuinfo in list*/
            bFind = TRUE;
            break;
        }
        pCurrent = g_list_next(pCurrent);
    }
    if( bFind )
    {
        /*have cu in the list*/
        pSecond = g_list_first(pTransfer->mCSGList);
        csgCurrent = pSecond;
        while(csgCurrent != NULL)
        {
            /*have csg--cu*/
            csgKey = (CSGKEYTAG *)csgCurrent->data;
            if(csgKey == NULL)
            {
                //error
            }
            status = activeSock_sendto(csgSrv->asock,&csgSrv->send_key,
					cudata,&buflen,0,&csgKey->nextAddr,sizeof(top_sockaddr_in));
            if( status != TOP_SUCCESS )
            {
                top_log(MDU_COMMUNIT_DOMAIN,"ERROR:data send to csg fail");
            }
            //top_log(MDU_COMMUNIT_DOMAIN,"CU DATA TO CSG %d",i++);
            //g_message("Have found one csg IP = %s ",csgInfoTag->uRemoteAddr);
            csgCurrent = g_list_next(csgCurrent);
        }
        /*Set the csg block datasize = 0 and reset the block to 0*/
    }
    else if ( !bFind )
    {
        /*There is no information of csg in the list.*/
        g_message("Get stream from cu don't need");
        //send warning to CMU
    }/*end of else */
	return TOP_SUCCESS;
}
