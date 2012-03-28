/*****************************************************
FileName:mduav.c
Author:huaixuzhi
Description:Dealing with audio and video,including open
	close/streamstop/transfer and so on
Fuction List:
	@
History:
 *****************************************************/
#include <glib.h>
#include "toplib.h"
#include "mdulist.h"
#include "top_tlv.h"

#define	NONEROUTE	-1

/*
Description:Parse MDUROUTE from buf's tlv
@argv:
	buf:data buffer
	pval:MDUROUTE message
@return:
	SUCCESS	length of the tlv
	FAIL	0
 */
gint    parseMDUROUTE2(gchar* buf , void* pval)
{
	/*length of T*/
	gushort    g_nTagLen = DEFAULT_TLV_TAG_LEN;
	gushort	   frontLen = 0;
	frontLen = g_nTagLen + sizeof(gushort);
    g_assert((guchar)buf[0] == TAG_MDUROUTE);
	/*Get the length of value from L*/
	gushort uslen = g_htons(*((gushort*)( &buf[g_nTagLen] )));
	if(uslen == sizeof(MDUINFO) + frontLen)
	{
		/*Have only one mduinfo in value*/
		return NONEROUTE;
	}
	else
	{
		/*Get the first node from mduroute*/
		g_assert((guchar)buf[frontLen] == TAG_MDUINFO);
		gushort mduslen = g_htons(*((gushort *)(&buf[frontLen + g_nTagLen] )));
		g_assert(mduslen == sizeof(MDUINFO));

		return parseTLV_str(buf, pval, mduslen);
	}
}

/*
Description:Get audio/video open/close message from cmu,and puinfo
	exists in mduList ,then use this function to send feedback
	message to cmu
@argv:

@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
@CREATED 2012-2-2
 */

guint	avFBMsg(struct _cmu_data_tag	*cmuMsg,
		gushort	ntype)
{
	MSGHEAD	msgHead;
	struct mduSrvTag *srv = NULL; 
    guint	tmpMduID,rc = 0;
	guint	lentmp = 0,len = 0;
	gssize	sentlen;
	guchar	opResult;
	gchar	*logStr = NULL;
	gchar	replyMsg[MAXLINE] = {0};

	srv = m_CmuSockSrv; 
	memset(&msgHead,0,sizeof(MSGHEAD));
	opResult = 1;
	lentmp = sizeof(MSGHEAD);
    /*Group Message information in replyMsg and send RegMsg to CMU*/
	switch(ntype)
	{
		case CSG_VOD_OPEN:
			msgHead.nsMsgType = htons(MDU_VOD_OPEN);
			logStr = "VOD_OPEN";
			break;
		case CSG_VOD_CLOSE:
			msgHead.nsMsgType = htons(MDU_VOD_CLOSE);
			logStr = "VOD_CLOSE";
			break;
		case CSG_AOD_OPEN:
			msgHead.nsMsgType = htons(MDU_AOD_OPEN);
			logStr = "AOD_OPEN";
			break;
		case CSG_AOD_CLOSE:
			msgHead.nsMsgType = htons(MDU_AOD_CLOSE);
			logStr = "AOD_CLOSE";
			break;
		default:
			break;
	}
    g_message("%s success and pu info was in list",logStr);
   	top_log(MDU_COMMUNIT_DOMAIN,"%s success and pu info was in list",logStr);
    len = buildOPRESULT(&replyMsg[lentmp],&opResult);
    lentmp = lentmp + len;
    len = buildPUID(&replyMsg[lentmp],&cmuMsg->puid);
    lentmp = lentmp + len;
    len = buildChannelID(&replyMsg[lentmp],&cmuMsg->channelid);
    lentmp = lentmp + len;
    if(cmuMsg->info_type == MEDIATYPEVOD)
    {
        len = buildStreamID(&replyMsg[lentmp],&cmuMsg->streamid);
        lentmp += len;
    }
    len = buildCUID(&replyMsg[lentmp],&cmuMsg->cuinfotag.uid);
    lentmp += len;

    tmpMduID = htonl(m_CmuSockSrv->mduId);
    len = buildMDUID(&replyMsg[lentmp],&tmpMduID);
    lentmp += len;

    msgHead.nsTotalLen = htons(lentmp);
    memcpy(&replyMsg,&msgHead,sizeof(MSGHEAD));

    sentlen = (gssize)lentmp;

    rc = activeSock_sendto(srv->asock,&srv->send_key,replyMsg,&sentlen,
    				0,&srv->addr,sizeof(top_sockaddr_in));
    top_log(MDU_COMMUNIT_DOMAIN,"%s Send to cmu feedback message success",logStr);
    top_log(MDU_COMMUNIT_DOMAIN,"Hava PU in List ,feedback CUID is %x",
    			cmuMsg->cuinfotag.uid);
    if(rc != TOP_SUCCESS)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,"Send message from CSG to cmu Failed");
    }
	return TOP_SUCCESS;
}

/*
Description:Send audio/video open message to csg.
	//2012-2-1 using csgsrv/queue to send the message to next hop
@argv:
	cmuMsg:message from cmu
	nextsock:top_sockaddr_in socket of csg. 
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
 */
gint nextSendcsg(struct _cmu_data_tag *cmuMsg,gushort msgtype)
{
	/*message head*/
	MSGHEAD	msgHead;
	top_sockaddr_in		 csgSock;
	struct mduSrvTag    *csgSrv;
	guint	len = 0,lentmp = 0,addrlen = 0;
	guint	status = 0;
	gint	csgPortTmp = 0;
	gchar	msgdata[MAXLINE] = {0};

	csgSrv = m_CsgSockSrv;
    //Added in 2011-10-20
    memset(&csgSock,0,sizeof(top_sockaddr_in));
    csgSock.sin_family		= AF_INET;
    csgPortTmp = ntohl(cmuMsg->csginfotag.uport);
    csgSock.sin_port 		= htons(csgPortTmp);
    csgSock.sin_addr.s_addr = cmuMsg->csginfotag.uip;

    lentmp = sizeof(MSGHEAD);
	memset(&msgHead,0,sizeof(MSGHEAD));
    msgHead.nsMsgType = htons(msgtype);            //fill msgType in msghead

    len = buildOPRESULT(&msgdata[lentmp], &cmuMsg->opresult);
    lentmp += len;
    len = buildPUID(&msgdata[lentmp],&cmuMsg->puid);
    lentmp += len;
    len = buildChannelID(&msgdata[lentmp],&cmuMsg->channelid);
    lentmp += len;
    if(cmuMsg->info_type == MEDIATYPEVOD)
    {
		/*If this is a video open message,then need the streamid*/
        len = buildStreamID(&msgdata[lentmp],&cmuMsg->streamid);
        lentmp += len;
    }
    len = buildMDUINFO(&msgdata[lentmp],&csgSrv->mduinfotag);
    lentmp += len;
    msgHead.nsTotalLen = g_htons((gushort)lentmp);
    memcpy(&msgdata,&msgHead,sizeof(MSGHEAD));

    addrlen = sizeof(top_sockaddr_in);
    //top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    status = activeSock_sendto(csgSrv->asock,&csgSrv->send_key,
			msgdata,(gssize *)&lentmp,0,&csgSock,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"Error:Send message to csg");
        return status;
    }
    top_log(MDU_COMMUNIT_DOMAIN,"Send message to next hop success");

    return TOP_SUCCESS;
}

/*
Description:Send audio/video open message to next mdu.
	//2012-2-1 using csgsrv/queue to send the message to next hop
@argv:
	cmuMsg:message from cmu
	nextsock:top_sockaddr_in socket of  next mdu.
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
 */
gint	nextOpenTomdu(struct _cmu_data_tag *cmuMsg,
		top_sockaddr_in	*nextsock)
{
	/*message head*/
	MSGHEAD	msgHead;
	struct mduSrvTag    *csgSrv;
	CUINFO	current_mduinfo;
	guint	len = 0,lentmp = 0,addrlen = 0;
	guint	status = 0;
	gushort msgtype = 0;
	gchar	msgdata[MAXLINE] = {0};

	csgSrv = m_CsgSockSrv;
	if(cmuMsg->info_type == MEDIATYPEVOD)
		msgtype = MDU_VOD_OPEN;
	else if (cmuMsg->info_type == MEDIATYPEAOD)
		msgtype = MDU_AOD_OPEN;

	memset(&current_mduinfo,0,sizeof(CUINFO));
	memset(&msgHead,0,sizeof(MSGHEAD));

	current_mduinfo.uip = csgSrv->mduinfotag.uip;//IP of current mdu
	g_message("current_mduinfo.uip = %d",current_mduinfo.uip);
	current_mduinfo.uport = htonl(6002);		//recvport of current mdu
	g_message("current_mduinfo.uport = %d",6002);
	current_mduinfo.uid = 0;					//0
	g_message("current_mduinfo.uid = 0");
    msgHead.nsMsgType = htons(msgtype);         //fill msgType in msghead

    len = buildOPRESULT(&msgdata[lentmp], &cmuMsg->opresult);
    lentmp += len;
    len = buildPUID(&msgdata[lentmp],&cmuMsg->puid);
    lentmp += len;
    len = buildChannelID(&msgdata[lentmp],&cmuMsg->channelid);
    lentmp += len;
    if(cmuMsg->info_type == MEDIATYPEVOD)
    {
		/*If this is a video open message,then need the streamid*/
        len = buildStreamID(&msgdata[lentmp],&cmuMsg->streamid);
        lentmp += len;
    }
    len = buildCUINFO(&msgdata[lentmp],&current_mduinfo);
    lentmp = len + lentmp;
	len = buildCSGINFO(&msgdata[lentmp],&cmuMsg->csginfotag);
	lentmp = len +lentmp;
	len = buildMDUROUTE(&msgdata[lentmp],&cmuMsg->mdubuf);
	lentmp = len + lentmp;

    msgHead.nsTotalLen = g_htons((gushort)lentmp);
    memcpy(&msgdata,&msgHead,sizeof(MSGHEAD));

    addrlen = sizeof(top_sockaddr_in);
    //top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    status = activeSock_sendto(csgSrv->asock,&csgSrv->send_key,
			msgdata,(gssize *)&lentmp,0,&nextsock,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"Error:Send message to next mdu");
        return status;
    }
    top_log(MDU_COMMUNIT_DOMAIN,"Send message to next hop success");

    return TOP_SUCCESS;
}

/*
Description:Send audio/video open message to next mdu.
	//2012-2-1 using csgsrv/queue to send the message to next hop
@argv:
	cmuMsg:message from cmu
	nextsock:top_sockaddr_in socket of  next mdu.
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
 */
gint nextCloseTomdu(struct _cmu_data_tag *cmuMsg,top_sockaddr_in	*nextsock)
{	
	/*message head*/
	MSGHEAD	msgHead;
	struct mduSrvTag    *csgSrv;
	guint	len = 0,lentmp = 0,addrlen = 0;
	guint	status = 0;
	gushort msgtype= 0;
	gchar	msgdata[MAXLINE] = {0};

	csgSrv = m_CsgSockSrv;
	if(cmuMsg->info_type == MEDIATYPEVOD)
		msgtype = MDU_VOD_CLOSE;
	else if (cmuMsg->info_type == MEDIATYPEAOD)
		msgtype = MDU_AOD_CLOSE;

	memset(&msgHead,0,sizeof(MSGHEAD));
    msgHead.nsMsgType = htons(msgtype);         //fill msgType in msghead

    len = buildOPRESULT(&msgdata[lentmp], &cmuMsg->opresult);
    lentmp += len;
    len = buildPUID(&msgdata[lentmp],&cmuMsg->puid);
    lentmp += len;
    len = buildChannelID(&msgdata[lentmp],&cmuMsg->channelid);
    lentmp += len;
    if(cmuMsg->info_type == MEDIATYPEVOD)
    {
		/*If this is a video open message,then need the streamid*/
        len = buildStreamID(&msgdata[lentmp],&cmuMsg->streamid);
        lentmp += len;
    }
    len = buildCUID(&msgdata[lentmp],&cmuMsg->cuinfotag.uid);
    lentmp = len + lentmp;
	len = buildCSGINFO(&msgdata[lentmp],&cmuMsg->csginfotag);
	lentmp = len +lentmp;
	len = buildMDUROUTE(&msgdata[lentmp],&cmuMsg->mdubuf);
	lentmp = len + lentmp;

    msgHead.nsTotalLen = g_htons((gushort)lentmp);
    memcpy(&msgdata,&msgHead,sizeof(MSGHEAD));

    addrlen = sizeof(top_sockaddr_in);
    //top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    status = activeSock_sendto(csgSrv->asock,&csgSrv->send_key,
			msgdata,(gssize *)&lentmp,0,&nextsock,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"Error:Send message to next mdu");
        return status;
    }
    top_log(MDU_COMMUNIT_DOMAIN,"Send message to next hop success");

    return TOP_SUCCESS;
}

/*
Description:add cu--csg information into list and send 
	feedback message to cmu and send open/close message 
	to csg -_-
@argv:
	cmuMsgTag:Information from cmu
	nodeFlag:Flag to show if the node is the one connecting 
		to cu,if it is,open pu timing.
@return:
	SUCCESS		TOP_TRUE;TOP_FALSE
	FAIL		others
*/
gint	nodeToCSGInsert(struct _cmu_data_tag *cmuMsgTag,
		guint	nodeFlag)
{
	guint rc = 0;
	guint cuPortTmp = 0,csgPortTmp = 0;
	gchar *logStr = NULL;
    struct top_in_addr   sin_addr_tmp;
	top_sockaddr_in		 csgSock;
	struct _cu_link		 cuLinkTag;
	struct _stream_key	 datakey;

	memset(&sin_addr_tmp,0,sizeof(struct top_in_addr));
	memset(&cuLinkTag,0,sizeof(CULINK));
	memset(&datakey,0,sizeof(STREAMKEY));

    /*Juadge the message type from cmu */
    if(cmuMsgTag->info_type == MEDIATYPEVOD)
    {
        /*Get the message : VOD_OPEN*/
        logStr = "VOD_OPEN";
    }
    else if(cmuMsgTag->info_type == MEDIATYPEAOD)
    {
        /*Get the message : AOD_OPEN*/
        logStr = "AOD_OPEN";
    }

    memset(&csgSock,0,sizeof(top_sockaddr_in));
    csgSock.sin_family		= AF_INET;
    csgPortTmp = ntohl(cmuMsgTag->csginfotag.uport);
    csgSock.sin_port 		= htons(csgPortTmp);
    csgSock.sin_addr.s_addr = cmuMsgTag->csginfotag.uip;

	cuLinkTag.mCUId	= cmuMsgTag->cuinfotag.uid;
	cuLinkTag.transAddr.sin_family = AF_INET;
	sin_addr_tmp.s_addr = cmuMsgTag->cuinfotag.uip;
	cuLinkTag.transAddr.sin_addr = sin_addr_tmp;
	cuPortTmp = ntohl(cmuMsgTag->cuinfotag.uport);
	cuLinkTag.transAddr.sin_port = htons(cuPortTmp);

	datakey.mpuid		   = cmuMsgTag->puid;
    datakey.mchannelid     = cmuMsgTag->channelid;
    datakey.mstreamtype    = cmuMsgTag->streamid;        //sub/main stream
    datakey.mmediatype     = cmuMsgTag->info_type;       //media type is vod/aod

    top_log(MDU_COMMUNIT_DOMAIN,"%s open PUID is %x ",logStr,cmuMsgTag->puid);
	top_log(MDU_COMMUNIT_DOMAIN,"next Send to csg open message");
	rc = avInsertList(&datakey,&cuLinkTag,&csgSock,nodeFlag);
    showListLog();

	if(rc != TOP_TRUE && rc != TOP_FALSE)
	{
		top_log(MDU_COMMUNIT_DOMAIN,
				"avInsertList fail! %s",logStr);
	}
	return rc;
}

/*
Description:add cu--mdu information into list and send 
	feedback message to cmu and send open/close message 
	to the next mdu-_-
@argv:
	cmuMsgTag:Information from cmu
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
@date:
	Create  2012-2-1
*/
gint	nodeToMDUInsert(struct _cmu_data_tag *cmuMsgTag,
		guint	nodeFlag)
{	
	guint rc = 0;
	guint portTmp = 0;
	gushort	nsMsgType = 0;
	gchar	*logStr	= NULL;
	top_sockaddr_in		 nextmduSock;
    struct top_in_addr   sin_addr_tmp;
	struct _cu_link		 cuLinkTag;		//Will send to
	struct _stream_key	 datakey;

    memset(&nextmduSock,0,sizeof(top_sockaddr_in));
	memset(&sin_addr_tmp,0,sizeof(struct top_in_addr));
	memset(&cuLinkTag,0,sizeof(CULINK));
	memset(&datakey,0,sizeof(STREAMKEY));

    /*Juadge the message type from cmu */
    if(cmuMsgTag->info_type == MEDIATYPEVOD)
    {
        /*Get the message : VOD_OPEN*/
        logStr        = "VOD_OPEN";
		nsMsgType	  = CSG_VOD_OPEN;
    }
    else if(cmuMsgTag->info_type == MEDIATYPEAOD)
    {
        /*Get the message : AOD_OPEN*/
        logStr        = "AOD_OPEN";
		nsMsgType	  = CSG_AOD_OPEN;
    }

	cuLinkTag.mCUId	= cmuMsgTag->cuinfotag.uid;
	cuLinkTag.transAddr.sin_family = AF_INET;
	sin_addr_tmp.s_addr = cmuMsgTag->cuinfotag.uip;
	cuLinkTag.transAddr.sin_addr = sin_addr_tmp;
	portTmp = ntohl(cmuMsgTag->cuinfotag.uport);
	cuLinkTag.transAddr.sin_port = htons(portTmp);

    //Added in 2011-10-20
    nextmduSock.sin_family		= AF_INET;
    nextmduSock.sin_port 		= htons(6004);
    nextmduSock.sin_addr.s_addr = m_CmuSockSrv->mduinfotag.uip;

	datakey.mpuid		   = cmuMsgTag->puid;
    datakey.mchannelid     = cmuMsgTag->channelid;
    datakey.mstreamtype    = cmuMsgTag->streamid;        //sub/main stream
    datakey.mmediatype     = cmuMsgTag->info_type;       //media type is vod/aod

    top_log(MDU_COMMUNIT_DOMAIN,"%s open PUID is %x ",logStr,cmuMsgTag->puid);
	top_log(MDU_COMMUNIT_DOMAIN,"next Send to csg open message");
	rc = avInsertList((void *)&datakey,(void *)&cuLinkTag,
			&nextmduSock,nodeFlag);
    showListLog();

    /*There is no need pu existing in list,then organize message and
     send to csg/next mdu,recving request from csg later*/
    if (rc == TOP_TRUE)
	{
		/*send open message to csg*/
        top_log(MDU_COMMUNIT_DOMAIN,
				"%s success,but there is no pu in list",logStr);
		nextOpenTomdu(cmuMsgTag,&nextmduSock);
	}
	else if (rc == TOP_FALSE)
	{
		/*Pu existing in list,and send feedback to cmu*/
		avFBMsg(cmuMsgTag,nsMsgType);
	}

	return TOP_SUCCESS;
}

/*
Description:remove cu--csg information from list and send 
	feedback message to cmu and send close message 
	to csg -_-
@argv:
	cmuMsgTag:Information from cmu
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
*/
gint	nodeToCSGRemove(struct _cmu_data_tag *cmuMsgTag)
{
	guint rc = 0;
    gchar *logStr = NULL;

	struct _cu_link		 cuLinkTag;		//Will send to
	struct _stream_key	 datakey;

    /*Juadge the message type from cmu */
    if(cmuMsgTag->info_type == MEDIATYPEVOD)
    {
        /*Get the message : VOD_CLOSE*/
        logStr = "VOD_CLOSE";
    }
    else if(cmuMsgTag->info_type == MEDIATYPEAOD)
    {
        /*Get the message : AOD_CLOSE*/
        logStr = "AOD_CLOSE";
    }

	memset(&cuLinkTag,0,sizeof(CULINK));
	memset(&datakey,0,sizeof(STREAMKEY));

	datakey.mpuid		   = cmuMsgTag->puid;
    datakey.mchannelid     = cmuMsgTag->channelid;
    datakey.mstreamtype    = cmuMsgTag->streamid;        //sub/main stream
    datakey.mmediatype     = cmuMsgTag->info_type;       //media type is vod/aod

    cuLinkTag.mCUId = cmuMsgTag->cuinfotag.uid;

    /*Remove CuInfo from the listl;if Cu List in one Pu is empty,remove the pu*/
    rc = removeTransfer(&datakey,&cuLinkTag);

    g_message("removeTransfer success");
    top_log(MDU_COMMUNIT_DOMAIN,"%s rmemoveTransfer success",logStr);
    showListLog(); 

	if(rc != TOP_TRUE && rc != TOP_FALSE)
	{
		top_log(MDU_COMMUNIT_DOMAIN,
				"avInsertList fail! %s",logStr);
	}
	return rc;
}

/*
Description:Remove cu--mdu information from list and send 
	feedback message to cmu and send open/close message 
	to the next mdu-_-
@argv:
	cmuMsgTag:Information from cmu
@return:
	SUCCESS		TOP_SUCCESS
	FAIL		others
@date:
	Create  2012-2-1
*/
gint	nodeToMDURemove(struct _cmu_data_tag *cmuMsgTag)
{
	guint rc = 0;
	gushort nsMsgType = 0;
	top_sockaddr_in		 nextmduSock;

    memset(&nextmduSock,0,sizeof(top_sockaddr_in));
    nextmduSock.sin_family		= AF_INET;
    nextmduSock.sin_port 		= htons(6004);
    nextmduSock.sin_addr.s_addr = cmuMsgTag->mduinfotag.uip;

	rc = nodeToCSGRemove(cmuMsgTag);
	if(rc == TOP_TRUE)
	{
		/*There is no cu in the pu ; Send message to mdu to drop*/
		nextCloseTomdu(cmuMsgTag,&nextmduSock);
		return rc;
	}
	else if (rc == TOP_FALSE)
	{
    	/*Juadge the message type from cmu */
   		if(cmuMsgTag->info_type == MEDIATYPEVOD)
        	/*Get the message : VOD_CLOSE*/
			nsMsgType = CSG_VOD_CLOSE;
    	else if(cmuMsgTag->info_type == MEDIATYPEAOD)
        	/*Get the message : AOD_CLOSE*/
			nsMsgType = CSG_AOD_CLOSE;

		avFBMsg(cmuMsgTag,nsMsgType);
	}
	return TOP_SUCCESS;
}

/*
Description:Recieve a/vod open message from cmu,then parse the message.
    if there is no corresponding pu in List ,send request to Csg,and
    add cu info into List.After that,send feedback to cmu
@argv:
    dataMsg:Message from cmu
    nsHeadType:The head type of message from cmu(vod/aod_open/close)
@return:
 */
void odDataOpen(void *dataMsg, gshort nsMsgType, 
		guint totalen, guint nodeFlag)
{
	gushort    g_nTagLen = DEFAULT_TLV_TAG_LEN;
	gushort    routeFrontLen = 0;
    gint	len = 0,lentmp = 0,uslen = 0,copylen = 0;
    gint	rc = 0;
    gchar	*logStr = NULL;
	gchar	groupmsg[MAXLINE] = {0};
    struct mduSrvTag *srv = NULL;
	struct _cmu_data_tag cmuDataTag;

    lentmp = sizeof(MSGHEAD);
    srv = m_CmuSockSrv;
	memset(&cmuDataTag,0,sizeof(struct _cmu_data_tag));
    /*Judage the type from cmu,and use the string in log below*/
    if(nsMsgType == CSG_VOD_OPEN)
    {
    	logStr = "VOD_OPEN";
    	cmuDataTag.info_type = MEDIATYPEVOD;		//Media type is vod
    }
    else if(nsMsgType == CSG_AOD_OPEN)
    {
    	logStr = "AOD_OPEN";
    	cmuDataTag.info_type = MEDIATYPEAOD;		//Meida type is aod
    }

    top_log(MDU_COMMUNIT_DOMAIN,"Get message %s from cmu",logStr);

    /*Parse message body and insert info into cmuDataTag*/
    len = parseOPRESULT((char *)dataMsg + lentmp,&cmuDataTag.opresult);
    if(0 == len)
    {
		top_log(MDU_COMMUNIT_DOMAIN,"%s parse opresult",logStr);
    	g_error("%s parse opresult",logStr);
    }
    lentmp += len;
    len = parsePUID((char *)dataMsg + lentmp,&cmuDataTag.puid);
    if(0 == len)
    {
		top_log(MDU_COMMUNIT_DOMAIN,"%s parse puid",logStr);
    	g_error("%s parse puid",logStr);
    }
    lentmp += len;
    len = parseChannelID((char *)dataMsg + lentmp,&cmuDataTag.channelid);
    if(0 == len)
    {	
		top_log(MDU_COMMUNIT_DOMAIN,"%s parse channelid",logStr);
    	g_error("%s parse channelid",logStr);
    }
    lentmp += len;
    if(nsMsgType == CSG_VOD_OPEN)
    {
    	len = parseStreamID((char *)dataMsg + lentmp,&cmuDataTag.streamid);
    	if(0 == len)
    	{
			top_log(MDU_COMMUNIT_DOMAIN,"%s parse streamid",logStr);
    		g_error("%s parse streamid",logStr);
    	}
    	lentmp += len;
    }
    len = parseCUINFO((char *)dataMsg + lentmp,&cmuDataTag.cuinfotag);
    if(0 == len)
    {
		top_log(MDU_COMMUNIT_DOMAIN,"%s parse cuinfo",logStr);
    	g_error("%s parse cuinfo",logStr);
    }
    lentmp += len;
    len = parseCSGINFO((char *)dataMsg + lentmp,&cmuDataTag.csginfotag);
    if(0 == len)
    {
		top_log(MDU_COMMUNIT_DOMAIN,"%s parse csginfo",logStr);
    	g_error("%s parse csginfo",logStr);
    }
    lentmp += len;

    len = parseMDUROUTE2((char *)dataMsg + lentmp,&cmuDataTag.mduroute);
	if(NONEROUTE == len)
	{
		/*Have no mduroute in the open message from cmu*/
		top_log(MDU_COMMUNIT_DOMAIN,"Have no route in open message");
    	rc = nodeToCSGInsert(&cmuDataTag,nodeFlag);
    	if (rc == TOP_TRUE)
		{
			/*send open message to csg*/
        	top_log(MDU_COMMUNIT_DOMAIN,
					"%s success,but there is no pu in list",logStr);
			nextSendcsg(&cmuDataTag,nsMsgType);
		}
		else if (rc == TOP_FALSE)
		{
			/*Pu existing in list,and send feedback to cmu*/
			avFBMsg(&cmuDataTag,nsMsgType);
		}
	}
	else if(0 == len)
	{
	    top_log(MDU_COMMUNIT_DOMAIN,"%s parse mduroute",logStr);
    	g_error("%s parse mduroute",logStr);
	}
	else
	{
		/*Have mdu route in the open message from cmu*/
		g_message("Have mdu route in open message");
		top_log(MDU_COMMUNIT_DOMAIN,"Have mdu route in open message");
		routeFrontLen = 2*g_nTagLen + 2*sizeof(gushort) + sizeof(MDUINFO);
		uslen = lentmp + g_nTagLen + sizeof(gushort);
		copylen = totalen - g_nTagLen - sizeof(gushort) - sizeof(MDUINFO);

		/*copy head(including mduroute's T and L) */
		memcpy(&groupmsg, (gchar *)dataMsg, uslen);
		/*copy the rest mdus into groupmsg*/
		memcpy(&groupmsg[uslen],(gchar *)dataMsg + routeFrontLen,
				totalen- lentmp - routeFrontLen);

		memcpy(&cmuDataTag.mdubuf,&groupmsg,copylen);
		rc = nodeToMDUInsert(&cmuDataTag,nodeFlag);
	}

    top_log(MDU_COMMUNIT_DOMAIN,
			"CUID=%x,PUID=%x,CHANNELID=%d,STREAMID=%d,mediatype=%d",
			cmuDataTag.cuinfotag.uid,
    		cmuDataTag.puid,cmuDataTag.channelid,
    		cmuDataTag.streamid,cmuDataTag.info_type);
}

/*
Description:Recieve vod close message from cmu,then parse the message.
@argv:
    data:Message from cmu
    CmuData:structure keep information from cmu
@return:
 */
void odDataClose(void *dataMsg,gshort	nsMsgType)
{
    gint	len = 0,lentmp = 0,uslen = 0,routeFrontLen = 0;
    gint	rc = 0, copylen = 0,totalen = 0;
    gchar	*logStr = NULL;
	gchar	groupmsg[MAXLINE];
	gushort    g_nTagLen = DEFAULT_TLV_TAG_LEN;
    struct	_cmu_data_tag	cmuDataTag;

    /*Judage the type from cmu,and use the string in log below*/
    if(nsMsgType== CSG_VOD_CLOSE)
    {
    	logStr = "VOD_CLOSE";
    	cmuDataTag.info_type = MEDIATYPEVOD;		//Media type is vod
    }
    else if(nsMsgType == CSG_AOD_CLOSE)
    {
    	logStr = "AOD_CLOSE";
    	cmuDataTag.info_type = MEDIATYPEAOD;		//Meida type is aod
    }

    g_message("Get message %s from cmu",logStr);
    top_log(MDU_COMMUNIT_DOMAIN,"Get message %s from cmu",logStr);

    lentmp = sizeof(MSGHEAD);
    /*Parse message body and insert info into cmuDataTag*/
    len = parseOPRESULT((gchar *)dataMsg + lentmp,&cmuDataTag.opresult);
    if( 0 == len)
    {
    	g_error("%s open parse opresult",logStr);
    }
    lentmp += len;
    len = parsePUID((gchar *)dataMsg + lentmp,&cmuDataTag.puid);
    if( 0 == len)
    {
    	g_error("%s open parse puid",logStr);
    }
    lentmp += len;
    len = parseChannelID((gchar *)dataMsg + lentmp,&cmuDataTag.channelid);
    if( 0 == len )
    {
    	g_error("%s open parse channelid",logStr);
    }
    lentmp += len;
    if(nsMsgType == CSG_VOD_CLOSE)
    {
    	len = parseStreamID((char *)dataMsg + lentmp,&cmuDataTag.streamid);
    	if( 0 == len )
    	{
    		g_error("%s open parse StreamID",logStr);
    	}
    	lentmp += len;
    }
    len = parseCUID((char *)dataMsg + lentmp,&cmuDataTag.cuinfotag.uid);
    if( 0 == len )
    {
    	g_error("%s open parse CuInfo",logStr);
    }
    lentmp += len;
    len = parseCSGINFO((char *)dataMsg + lentmp,&cmuDataTag.csginfotag);
    if( 0 == len )
    {
    	g_error("%s open parse CSGINFO",logStr);
    }
//    len = parseMDUROUTE2((char *)dataMsg + lentmp,&cmuDataTag.mduroute);
//	if(NONEROUTE == len)
//	{
		/*Have no mduroute in the open message from cmu*/
		top_log(MDU_COMMUNIT_DOMAIN,"Have no route in close message");
    	rc = nodeToCSGRemove(&cmuDataTag); 
 		if (rc == TOP_TRUE)
    	{
       	 	/*There is no cu in the pu ; Send message to csg to drop*/
			top_log(MDU_COMMUNIT_DOMAIN,
					"No cu ever in the puList,Send message to csg to drop");
			nextSendcsg(&cmuDataTag,nsMsgType);
		}
		else if (rc == TOP_FALSE)
    	{
        	/*Remove success and have other cus in the list*/
			top_log(MDU_COMMUNIT_DOMAIN,
					"Remove success and have other cus in the list");
			/*send feedback message to front*/
			avFBMsg(&cmuDataTag,nsMsgType);
		}
#if 0
	}
	else if(0 == len)
	{
	    top_log(MDU_COMMUNIT_DOMAIN,"%s parse mduroute",logStr);
    	g_error("%s parse mduroute",logStr);
	}
	else
	{
		/*Have mdu route in the open message from cmu*/
		top_log(MDU_COMMUNIT_DOMAIN,"Have mdu route in close message");

		routeFrontLen = 2*g_nTagLen + 2*sizeof(gushort) + sizeof(MDUINFO);
		uslen = lentmp + g_nTagLen + sizeof(gushort);
		copylen = totalen - g_nTagLen - sizeof(gushort) - sizeof(MDUINFO);

		/*copy head(including mduroute's T and L) */
		memcpy(&groupmsg, (gchar *)dataMsg, uslen);
		/*copy the rest mdus into groupmsg*/
		memcpy(&groupmsg[uslen],(gchar *)dataMsg + routeFrontLen,
				totalen- lentmp - routeFrontLen);

		memcpy(&cmuDataTag.mdubuf,&groupmsg,copylen);

		rc = nodeToMDURemove(&cmuDataTag);
		if(rc != TOP_SUCCESS)
		{
			top_log(MDU_COMMUNIT_DOMAIN,"Remove node in route's list fail");
		}
	}
#endif
    top_log(MDU_COMMUNIT_DOMAIN,
			"CUID=%x,PUID=%x,CHANNELID=%d,STREAMID=%d,mediatype=%d",
			cmuDataTag.cuinfotag.uid,
    		cmuDataTag.puid,cmuDataTag.channelid,
    		cmuDataTag.streamid,cmuDataTag.info_type);
}

/*Because cu's timeout,have remove all the cus in one PUList,
	then Send STOP MESSAGE TO CSG*/
guint stopMsgToCSG(struct  _mdutransferentry  *pTransfer)
{
    guint 		len = 0, addrlen = 0, status = 0;
	gssize		lentmp;
    gushort		msgtypetmp;
    gchar 		msgdata[MAX_CALLS] = {0};    //keeping message send to csg
    gchar       *logStr = NULL;
    gchar		opResult;
    MSGHEAD     msg_head;
    struct mduSrvTag *srv = NULL;

    memset(&msg_head,0,sizeof(MSGHEAD));
	opResult = 0;
	srv = m_CsgSockSrv;
    /*Juadge the message type from cmu */
    if(pTransfer->mStreamKey.mmediatype == MEDIATYPEVOD)
    {
        /*Get the message : VOD_OPEN*/
        logStr        = "VOD_CLOSE";
        msgtypetmp    = CSG_VOD_CLOSE;
    }
    else if(pTransfer->mStreamKey.mmediatype == MEDIATYPEAOD)
    {
        /*Get the message : AOD_OPEN*/
        logStr        = "AOD_CLOSE";
        msgtypetmp    = CSG_AOD_CLOSE;
    }

    msg_head.nsMsgType = htons(msgtypetmp);            //fill msgType in msghead
    lentmp = sizeof(MSGHEAD);
    len = buildOPRESULT(msgdata + lentmp,&opResult);
    lentmp += len;
    len = buildPUID(msgdata + lentmp,&pTransfer->mStreamKey.mpuid);
    lentmp += len;
    len = buildChannelID(msgdata + lentmp,&pTransfer->mStreamKey.mchannelid);
    lentmp += len;
    if( msgtypetmp == CSG_VOD_CLOSE )
    {
        len = buildStreamID(msgdata + lentmp,&pTransfer->mStreamKey.mstreamtype);
        lentmp += len;
    }
    len = buildMDUINFO(msgdata + lentmp,&srv->mduinfotag);
    lentmp += len;
    msg_head.nsTotalLen = g_htons((gushort)lentmp);
    memcpy(msgdata,&msg_head,sizeof(MSGHEAD));

    addrlen = sizeof(top_sockaddr_in);
    /*Send close message to csg*/
    status = activeSock_sendto(srv->asock,&srv->send_key,msgdata,&lentmp,0,
            &pTransfer->nextAddr,addrlen);
    if (status != TOP_SUCCESS)
    {
        top_log(MDU_COMMUNIT_DOMAIN,"Error:Send message to csg");
        return status;
    }
    top_log(MDU_COMMUNIT_DOMAIN,
            "BECAUSE CU timeout,Send CLOSE message to csg");

	return TOP_SUCCESS;
}
