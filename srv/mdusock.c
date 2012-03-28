/*****************************************************
Filename:mdusock.c
Author:huaixuzhi
Description:
    Parse configure file,initliazie socket
Version:
Function List:
    @
History:
    <author>    <time>  <version>   <desc>
             2012-1-9               created
******************************************************/
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toplib.h"
#include "mdusock.h"
#include "mduav.h"

struct    mduSrvTag *m_CsgSockSrv = NULL;
struct    mduSrvTag *m_CsgDataSrv = NULL;
struct    mduSrvTag	*m_CmuSockSrv = NULL;
struct    mduSrvTag	*m_CUSockSrv = NULL;

/*
 Description:Parsing the configuration file of mdu
 buf:Path of mbu configuration
 @return:keyfile of parsing
*/
GKeyFile *mduParser(gchar *buf)
{
    GKeyFile        *keyfile;
    GKeyFileFlags   flags;
    GError  *error = NULL;
    keyfile = g_key_file_new();
    flags=G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if(!g_key_file_load_from_file(keyfile,buf,flags,&error))
    {
        g_error( error->message );
        return NULL;
    }
    return keyfile;
}

/*
Description:Analysis of the configuation file
	parse MDU.conf,get the information of cmuip,cmuport,
    mduid,cascade infos and so on    
m_conf:As return
created:2012-1-9
 */
guint mduConfAnal(MDUCONFTAG	**m_conf)
{
   /**/
    GKeyFile        *keyfile;
    MDUCONFTAG		*conf;          //structure keeping configure of MBU

    /*parse information thougth PATH of CONFIGURE*/
    keyfile = mduParser(CONF_PATH);
    conf = g_slice_new (MDUCONFTAG);   /*initalize*/

    conf->localid = g_key_file_get_integer(keyfile, MDUCONF, LOCALID, NULL);
    if(conf->localid == PRASE_FALSE)
    {
        g_error("Get mduLocalID from MDU.conf");
        g_key_file_free(keyfile);
    }

    conf->cmdport = g_key_file_get_integer(keyfile, MDUCONF, CMDPORT, NULL);
    if(conf->cmdport == PRASE_FALSE)
    {
        g_error("Get cmdport from MDU.conf");
        g_key_file_free(keyfile);
    }
    conf->rtpport = g_key_file_get_integer(keyfile, MDUCONF, RTPPORT, NULL);
    if(conf->rtpport == PRASE_FALSE)
    {
        g_error("Get RTPPort from MDU.conf");
        g_key_file_free(keyfile);
    }
    conf->rtpdevport = g_key_file_get_integer(keyfile, MDUCONF, RTPDEVPORT, NULL);
    if(conf->rtpdevport == PRASE_FALSE)
    {
        g_error("Get RTPDEVPort from MDU.conf");
        g_key_file_free(keyfile);
    }
    conf->csgmsgport = g_key_file_get_integer(keyfile, MDUCONF, CSGMSGPORT, NULL);
    if(conf->csgmsgport == PRASE_FALSE)
    {
        g_error("Get CSGMSGPort from MDU.conf");
        g_key_file_free(keyfile);
    }

    conf->cmuip = g_key_file_get_string(keyfile, CMUCONF, CMUIPCONF, NULL);
    if(conf->cmuip == NULL)
    {
        g_error("Get CMUIP from MDU.conf");
        g_key_file_free(keyfile);
    }
    conf->cmuport = g_key_file_get_integer(keyfile, CMUCONF, CMUPORT, NULL);
    if(conf->cmuport == PRASE_FALSE)
    {
        g_error("Get cmuPort from MDU.conf");
        g_key_file_free(keyfile);
    }

    conf->cfrontport = g_key_file_get_integer(keyfile, CASCADE, CFRONTPORT, NULL);
    if(conf->cfrontport == PRASE_FALSE)
    {
        g_error("Get cfrontport from MDU.conf");
        g_key_file_free(keyfile);
    }

    g_message("Parse from MDU.conf....");
    top_log(MDU_COMMUNIT_DOMAIN,"Parse from MDU.conf....");
	*m_conf = conf;

	return TOP_SUCCESS;
}

/*
Description:update the basic time of keeping alive
@void:void
@return:void
*/
void updateKeepAlive(void)
{
    GTimeVal tnow;
    g_get_current_time(&tnow);
    m_stGlobe.LastActiveTime = tnow.tv_sec;
}

/*
Description:Recieve reply message about login,then update MDUID
    and the basic time of keeping alive
@argv:
    dataMsg:Message from cmu
    size:srv
@return:
*/
void updateLoginInfo(void *dataMsg,gssize size)
{
    MDUINFO	stMduInfo;
    guint	nlen = 0,lentmp = 0;		//len of message head
    guint	mduIDTmp = 0;
    guchar	opResultTmp;
	guchar  *pData;

	g_message("Get regist feedback from CMU ");
    top_log(MDU_COMMUNIT_DOMAIN,"Get regist feedback from CMU ");
    /*set register success */
    m_stGlobe.bRegister = TRUE;

    /*Get mduinfo from the message*/
    memset(&stMduInfo,0,sizeof(MDUINFO));
    nlen = sizeof(MSGHEAD);
    lentmp = parseOPRESULT((gchar *)dataMsg + nlen,&opResultTmp);
    if(0 == lentmp)
	{
    	g_error("mupdateLoginInf parse OPRESULT");
	}
    nlen = nlen + lentmp;
    lentmp = parseMDUINFO((gchar *)dataMsg + nlen,&stMduInfo);
    if(0 == lentmp)
    {
    	g_error("updateLoginInfo parse MDUINFO");
    }

    mduIDTmp = ntohl(stMduInfo.uid);

    if ( mduIDTmp != m_CmuSockSrv->mduId)
    {
    	/*Write new mduID into the MDU.conf*/
    	FILE	*fp = NULL;
    	GKeyFile	*keyfile = NULL;
    	gsize	nDataLen = 0;

    	/*Read configuration file*/
    	keyfile = g_key_file_new();
    	g_key_file_load_from_file(keyfile,CONF_PATH,G_KEY_FILE_NONE,NULL);

    	/*updata the conf in MDU.conf*/
    	g_key_file_set_integer(keyfile,MDUCONF,LOCALID,mduIDTmp);
    	/*Transfer data into string*/
    	pData = (guchar*)g_key_file_to_data(keyfile,&nDataLen,NULL);

    	fp = fopen(CONF_PATH,"w+");
    	fwrite(pData,nDataLen,1,fp);

    	fclose(fp);
    }

    top_log(MDU_COMMUNIT_DOMAIN,"updateLoginInfo MDUID IS %d",
    		htonl(mduIDTmp));
    /*updata globle structure*/
    memcpy(&m_CmuSockSrv->mduinfotag,&stMduInfo,sizeof(MDUINFO));
    m_CmuSockSrv->mduId	= mduIDTmp;

	m_CsgSockSrv->mduinfotag = m_CmuSockSrv->mduinfotag;
    updateKeepAlive();
}

/*
Description:keepalive to cmu is timeout,destory the List
	of cus
@argv:
    NULL
@return:
    sucess		TOP_SUCCESS
    fail		others
@date:Added by huaixuzhi in 2011-9-16
 */
gint    emputyList()
{
    struct _cu_link	*pCUTmp	=	NULL;
    gboolean	bFind = FALSE;
    struct	_mdutransferentry	*pTransfer = NULL;
    GList	*puFirst = NULL,*puCurrent = NULL,
			*cuFirst = NULL,*cuCurrent = NULL;

    puFirst = g_list_first(m_pTransferList);
    puCurrent = puFirst;
    /*Search information of PU from m_pTransferList
      	list->data can get _mdutransferentry
     	_mdutransferentry->mStreamKey get info of PUs*/
    top_log(MDU_COMMUNIT_DOMAIN,
    		"BECAUSE of feedback from cmu timeout,remove currentInfo from LIST");
    g_mutex_lock(m_transferListMutex);
    while(puCurrent != NULL)
    {
    	bFind = TRUE;
    	pTransfer = (struct _mdutransferentry*)puCurrent->data;
    	if(pTransfer == NULL)
    	{
    		//put out information of error
    	}
    	else
    	{
    		/*find one pu in list*/
    		/*Delete all the nodes of cu about the pu*/
    		cuFirst = g_list_first(pTransfer->mCUList);
    		cuCurrent = cuFirst;

    		g_mutex_lock(m_subscribeListMutex);
    		while(cuCurrent != NULL)
    		{
    			pCUTmp = (CULINK *)cuCurrent->data;
    			if(pCUTmp == NULL)
    			{
    				//error
    			}
    			/*Delete cu node and set 0*/
         	    pTransfer->mCUList = g_list_remove(pTransfer->mCUList,(void *)pCUTmp);

    			pCUTmp->CURefCnt--;
    			if(pCUTmp->CURefCnt == 0){
    				m_pCUList = g_list_remove(m_pCUList,(void *)pCUTmp);
    				memset(pCUTmp,0,sizeof(struct _cu_link));
    			}
    			puCurrent = g_list_next(puCurrent);
    		}
    		g_mutex_unlock(m_subscribeListMutex);
    	}
    	/*Delete pu node*/
     	m_pTransferList = g_list_remove(m_pTransferList,(void*)pTransfer);
        memset(pTransfer,0,sizeof(struct _mdutransferentry));

    	puCurrent = g_list_next(puCurrent);
    }
    if(! bFind ){
    	/*There is no information of PU in the list.
    	  return delete success*/
    	top_log(MDU_COMMUNIT_DOMAIN,"show log after time out to cmu");
    	g_mutex_unlock(m_transferListMutex);
    	showListLog();
    	return TOP_SUCCESS;
    }
    g_mutex_unlock(m_transferListMutex);
    /*show log after destory pu list*/

    return TOP_SUCCESS;
}

/*
Description:Recieve keeping alive form cu time over ,and need sened a
    warning to cmu.this is the func building the message and sending
    to cmu.
tcuid:The ID of Cu from the List
return: success     TOP_SUCCESS
    	fail		others
 */
gint sendCUWarning(guint tcuid)
{
    MSGHEAD stHead;
    gchar	msgCUBuf[MAXLINE] = {0};
    guint	ntlvLen = 0;
	gssize	nlen = 0;
    guint   rc = 0;
    guint	mCUID;

    if(tcuid == 0)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,"wrong cuid from list");
    	return TOP_SUCCESS;
    }
	g_message("send CU warning packet to cmu...");
    g_message("CUID is %d",tcuid);
    top_log(MDU_COMMUNIT_DOMAIN,"send CU warning packet to cmu...");
    top_log(MDU_COMMUNIT_DOMAIN,"CUID is %x",tcuid);

    /* packet head*/
    memset(&stHead, 0, sizeof(MSGHEAD));
    stHead.nsMsgType = g_htons(MDU_ALARM);
    mCUID = ntohl(tcuid);

    /*packet body*/
    nlen = sizeof(MSGHEAD);
    ntlvLen = buildCUID(msgCUBuf+nlen, &mCUID);

    // update packet head
    nlen += ntlvLen;
    stHead.nsTotalLen = g_htons((gushort)nlen);
    memcpy(msgCUBuf, &stHead, sizeof(MSGHEAD));

    /*send warning message to cmu*/
    rc = activeSock_sendto(m_CmuSockSrv->asock,&m_CmuSockSrv->send_key,msgCUBuf,
			&nlen,0,&m_CmuSockSrv->addr,sizeof(top_sockaddr_in));
    if(rc != TOP_SUCCESS)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,"Send warning message to cmu Failed");
    }
    return TOP_SUCCESS;
}

/*
Description:Send "stream stop" message to cmu
 */
gint streamStopToCmu(struct _stream_key *puTmpTag)
{
    MSGHEAD stHead;
    gchar	msgBuf[MAXLINE] = {0};
    gssize	nlen = 0,nlentmp= 0;
    guint rc = 0;
    guchar	opResultTmp;
    guint mduIDTmp;

    g_message("send PU Stream stop warning packet to cmu...");
    top_log(MDU_COMMUNIT_DOMAIN,"send PU Stream stop warning packet to cmu...");
    g_message("PU stop:PUID = %x;ChannelID = %d",
    		puTmpTag->mpuid,puTmpTag->mchannelid);

    top_log(MDU_COMMUNIT_DOMAIN,"PU stop:PUID = %x;ChannelID = %d,MEDIATYPE = %d",
    		puTmpTag->mpuid,puTmpTag->mchannelid,puTmpTag->mmediatype);
    /* packet head*/
    memset(&stHead , 0 , sizeof(MSGHEAD));
    stHead.nsMsgType = g_htons(MDU_STREAM_BREAK);

    /*packet body*/
	opResultTmp = 0;
    nlen = sizeof(MSGHEAD);
    nlentmp = buildOPRESULT(msgBuf+nlen,&opResultTmp);
    nlen = nlen + nlentmp;
    nlentmp = buildPUID(msgBuf+nlen, &puTmpTag->mpuid);
    nlen = nlen + nlentmp;
    nlentmp = buildChannelID(msgBuf+nlen, &puTmpTag->mchannelid);
    nlen = nlen + nlentmp;
    nlentmp = buildStreamID(msgBuf+nlen,&puTmpTag->mstreamtype);

    nlen = nlen + nlentmp;
/////////////////////////////////////////////////////////////
//  pending
//  guint streamTypeTmp = htonl(1);
    nlentmp = buildStreamType(msgBuf + nlen,&puTmpTag->mmediatype);
    nlen = nlen + nlentmp;
//////////////////////////////////////////////////////////////
    mduIDTmp = htonl(m_CmuSockSrv->mduId);
    nlentmp = buildMDUID(msgBuf+nlen,&mduIDTmp);
    nlen = nlen + nlentmp;

    /* update packet head */
    stHead.nsTotalLen = g_htons((gushort)nlen);
    memcpy(msgBuf,&stHead,sizeof(MSGHEAD));

    /*send warning message to cmu*/
    rc = activeSock_sendto(m_CmuSockSrv->asock,&m_CmuSockSrv->send_key,msgBuf,&nlen,
    				0,&m_CmuSockSrv->addr,sizeof(top_sockaddr_in));
    if(rc != TOP_SUCCESS)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,
    			"Send pu stream stop warning message to cmu Failed");
    }
    return TOP_SUCCESS;
}

/*
Description:Send Regist Message to CMU
 */
void SendReg(void)
{
    struct	mduSrvTag	*srvCmuTmp = m_CmuSockSrv;
    MSGHEAD *stHead;
    gint	nlen = 0,ntlvLen = 0;
    gint	status = 0;
	guchar OPResultInit;


    if(m_stGlobe.pRegPacket == NULL)		//first regist
    {
    	guint	LoginTmp = MDU_LOGIN;

    	m_stGlobe.pRegPacket = (gchar*)g_malloc0(MAX_CALLS);
    	stHead = (MSGHEAD *)g_malloc0(sizeof(MSGHEAD));
		OPResultInit = 0;
    	// packet head
    	stHead->nsMsgType = htons(LoginTmp);

    	// packet body
    	nlen = sizeof(MSGHEAD);
    	ntlvLen = buildOPRESULT(m_stGlobe.pRegPacket+nlen,&OPResultInit);
    	nlen = ntlvLen + nlen ;
    	ntlvLen = buildMDUINFO(m_stGlobe.pRegPacket+nlen,&srvCmuTmp->mduinfotag);

        // update packet head
        nlen = ntlvLen + nlen;
        stHead->nsTotalLen = htons(nlen);
        memcpy(m_stGlobe.pRegPacket,stHead,sizeof(MSGHEAD));

    	/*registe to CMU*/
    	status = activeSock_sendto(srvCmuTmp->asock,&srvCmuTmp->send_key,
    			m_stGlobe.pRegPacket,&nlen,0,&srvCmuTmp->addr,sizeof(top_sockaddr_in));
    	if( status != TOP_SUCCESS )
    	{
    		g_error("Registe to CMU.");
    	}
    	g_message("First regist to cmu sendto finished");
    	top_log(MDU_COMMUNIT_DOMAIN,"First regist to cmu sendto finished,MDUID IS %d",
    			m_CmuSockSrv->mduinfotag.uid);
    	g_free(stHead);
    }
    else
    {
    	nlen = sizeof(MSGHEAD);

    	OPResultInit = 0;
    	ntlvLen = buildOPRESULT(m_stGlobe.pRegPacket+nlen,&OPResultInit);
    	nlen = ntlvLen + nlen;
    	buildMDUINFO(m_stGlobe.pRegPacket + nlen,&m_CmuSockSrv->mduinfotag);
    	nlen = g_ntohs(((MSGHEAD*)(m_stGlobe.pRegPacket))->nsTotalLen);
    	status = activeSock_sendto(srvCmuTmp->asock,&srvCmuTmp->send_key,
    			m_stGlobe.pRegPacket,&nlen,0,&srvCmuTmp->addr,sizeof(top_sockaddr_in));
    	if( status != TOP_SUCCESS )
    	{
    		g_error("Registe to CMU.");
    	}
    	g_message("Regist to cmu failed or Recv feedback msg failed");
    	top_log(MDU_COMMUNIT_DOMAIN,"Regist to cmu failed or Recv feedback msg failed");
    	g_message("Regist to cmu again");
    	top_log(MDU_COMMUNIT_DOMAIN,"Regist to cmu again,MDUID IS %d",
    			m_CmuSockSrv->mduinfotag.uid);
    }
}

/*
Description:Send keep alive message to cmu
*/
void SendKeepAlive(void)
{
    gint	nlen = 0,ntlvLen = 0;
    gint	status = 0;
    MSGHEAD *stHeadKeep;
    struct	mduSrvTag *SrvHeartTmp;
	guchar OPResultInit;

    SrvHeartTmp = m_CmuSockSrv;
    if(m_stGlobe.pKeepAlivePkt == NULL)
    {
		guint	mduIDTmp = 0;
    	nlen = sizeof(MSGHEAD);
    	
        // first keepalive
        m_stGlobe.pKeepAlivePkt = (gchar*)g_malloc0(MAX_CALLS);
        // packet head
    	stHeadKeep = (MSGHEAD *)g_malloc0(sizeof(MSGHEAD));
       	stHeadKeep->nsMsgType = htons(MDU_KEEPALIVE);
        // packet body
        
		OPResultInit = 0;	
    	ntlvLen = buildOPRESULT(&m_stGlobe.pKeepAlivePkt[nlen],&OPResultInit);
    	nlen = ntlvLen + nlen ;
    	mduIDTmp = htonl(m_CmuSockSrv->mduId);
    	ntlvLen = buildMDUID(&m_stGlobe.pKeepAlivePkt[nlen],&mduIDTmp);
     	nlen = ntlvLen + nlen;

        // update packet head
        stHeadKeep->nsTotalLen = htons(nlen);
        memcpy(m_stGlobe.pKeepAlivePkt,stHeadKeep,sizeof(MSGHEAD));

        // send packet
    	activeSock_sendto(SrvHeartTmp->asock,&SrvHeartTmp->send_key,
    			m_stGlobe.pKeepAlivePkt,&nlen,0,
    			&SrvHeartTmp->addr,sizeof(top_sockaddr_in));

    	g_message("First time keeping alive to cmu");
    	top_log(MDU_COMMUNIT_DOMAIN,"First time keeping alive to cmu,MDUID IS %d",
    			mduIDTmp);
    	g_free(stHeadKeep);
    }
    else
    {
        // packet body
    	guint	mduIDTmp = 0;
        nlen = sizeof(MSGHEAD);

		OPResultInit = 0;
    	ntlvLen = buildOPRESULT(&m_stGlobe.pKeepAlivePkt[nlen],&OPResultInit);
    	nlen = ntlvLen + nlen ;

    	mduIDTmp = htonl(m_CmuSockSrv->mduId);

    	buildMDUID(&m_stGlobe.pKeepAlivePkt[nlen],&mduIDTmp);
    	nlen = htons( ((MSGHEAD*)(m_stGlobe.pKeepAlivePkt))->nsTotalLen );
    	status = activeSock_sendto(SrvHeartTmp->asock,&SrvHeartTmp->send_key,
    			m_stGlobe.pKeepAlivePkt,&nlen,0,
    			&SrvHeartTmp->addr,sizeof(top_sockaddr_in));
    	if( status != TOP_SUCCESS )
    	{
    		g_error("KEEPALIVE to CMU.");
    	}
    	g_message("Keeping Alive to cmu MDUID IS %d",mduIDTmp);
    	top_log(MDU_COMMUNIT_DOMAIN,"Keeping Alive to cmu,MDUID IS %d",
    			mduIDTmp);
    }
}

/*
Description:Set the time stamp 0xFFFFFFFF and send 
	confirmation to cmu
@argv:
	msgHeadTmp:head message from cmu
@return:
	SUCCESS	TOP_SUCCESS
	FAIL	others
*/
gint sendConfirmTocmu(struct _msg_head_data *msgHeadTmp)
{
    MSGHEAD	*msg_head = NULL;
    guint status = 0;
    gssize nlen = sizeof(MSGHEAD);
	gchar confirmBuf[sizeof(MSGHEAD)];
    msg_head = msgHeadTmp;
    msg_head->nTimestamp = COMFIRM_MSG;
	/*Added by huaixuzhi in 2011-12-9*/
	msg_head->nsTotalLen = nlen;

    memcpy(confirmBuf,msg_head,nlen);

    status = activeSock_sendto(m_CmuSockSrv->asock,&m_CmuSockSrv->send_key,confirmBuf,
    			&nlen,0,&m_CmuSockSrv->addr,sizeof(top_sockaddr_in));
    if( status != TOP_SUCCESS )
    {
    	g_error("sendConfirm to CMU.");
    }
    return TOP_SUCCESS;
}

/*
Description:Get message from cmu/front mdu of order message,
	and judge message type
@argv:
	dataMsg:	order message
	size:	message size from poll
@return:
	SUCCESS	TOP_SUCCESS
	FAIL	others
@Created By huaixuzhi in 2012-2-7
*/
gint	judgeOrderMsgTpye(gchar *getData,gsize msgLen)
{
    MSGHEAD	msg_head;	//structure keeping head of message from cmu
    					//--including:length of message/type of message
    					//--SN/version information and so on
    gushort	msgtype;	//to judge type of message(EX:VOD/AOD and so on)
	gushort msgCsgType;
    guint	msgheadlen = 0;			//length of message head
	guint	nodeFlag = OTHER_MDU_NODE;
    gchar	dataMsg[MAXLINE];		//Message from cmu
	
 	memcpy(&dataMsg, getData, msgLen);

    /* parse data from cmu ,transmit information into msg_head*/
    msgheadlen = sizeof(MSGHEAD);
    parseMsgHead((gchar *)&dataMsg,msgLen,&msg_head);

    /*structure keep informatin from CMU*/
    g_message("Getting message from front mdu node..");
    top_log(MDU_COMMUNIT_DOMAIN,"Getting message from front mdu node..");

    msgtype = ntohs(msg_head.nsMsgType);

    switch(msgtype)
    {
   		/*The opening of video on demand(VoD)*/
    	case MDU_VOD_OPEN:
    		msgCsgType = CSG_VOD_OPEN;
			odDataOpen(&dataMsg, msgCsgType, msgLen, nodeFlag);
    		break;
    	/*The closing of video on demand(VoD)*/
    	case MDU_VOD_CLOSE:
			msgCsgType = CSG_VOD_CLOSE;
    		odDataClose(&dataMsg, msgCsgType);
    		break;
    	/*The opening of Audio on demand(AoD)*/
    	case MDU_AOD_OPEN:
			msgCsgType = CSG_AOD_OPEN;
    		odDataOpen(&dataMsg, msgCsgType, msgLen, nodeFlag);
    		break;
    	/*The closing of Audio on demand(AoD)*/
    	case MDU_AOD_CLOSE:
			msgCsgType = CSG_AOD_CLOSE;
    		odDataClose(&dataMsg, MDU_AOD_CLOSE);
    		break;
    	default:
    		g_message("Recv from cmu is %x ",msgtype);
			top_log(MDU_COMMUNIT_DOMAIN,
					"A useless message from front mdu %x",msgtype);
    		break;
    }
	return TOP_SUCCESS;
}

/*
Description:Because of stream stop,Destory list about designated pu
 */
gint destoryPUList(struct _stream_key	*puTmpTag)
{

    struct	_cu_link	*pCUTmp	=	NULL;
    gboolean	bFind = FALSE;
    struct	_mdutransferentry	*pTransfer = NULL;
    GList	*pFirst = NULL,*pCurrent = NULL;

    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;
    /*Search information of PU from m_pTransferList
      	list->data can get _mdutransferentry
     	_mdutransferentry->mStreamKey get info of PUs*/
    top_log(MDU_COMMUNIT_DOMAIN,
    		"Destory pu list because stream stop/connect fail");
    g_mutex_lock(m_transferListMutex);
    while(pCurrent != NULL)
    {
    	//Test
    	top_log(MDU_COMMUNIT_DOMAIN,"In DESTORY while(pCurrent != NULL)");
    	pTransfer = (struct _mdutransferentry*)pCurrent->data;
    	if(pTransfer == NULL)
    	{
    		//put out information of error
    	}
    	if(pTransfer->mStreamKey.mpuid == puTmpTag->mpuid&&
    			pTransfer->mStreamKey.mchannelid	==	puTmpTag->mchannelid&&
    			pTransfer->mStreamKey.mstreamtype	== puTmpTag->mstreamtype&&
    			pTransfer->mStreamKey.mmediatype	== puTmpTag->mmediatype)
    	{
    		/*find it*/
    		bFind	=	TRUE;
    		break;
    	}
    	pCurrent = g_list_next(pCurrent);
    }
    if(! bFind ){
    	top_log(MDU_COMMUNIT_DOMAIN,"Do not find the pu info to delete!!");
    	/*There is no information of PU in the list.
    	  return delete success*/
    	g_mutex_unlock(m_transferListMutex);
    	return TOP_SUCCESS;
    }

    /*Delete all the nodes of cu about the pu*/
    pFirst = g_list_first(pTransfer->mCUList);
    pCurrent = pFirst;

    g_mutex_lock(m_subscribeListMutex);
    while(pCurrent != NULL)
    {
    	pCUTmp = (CULINK *)pCurrent->data;
    	if(pCUTmp == NULL)
    	{
    		//error
    	}
    	/*Delete cu node and set 0*/
        pTransfer->mCUList = g_list_remove(pTransfer->mCUList,(void *)pCUTmp);

    	pCUTmp->CURefCnt--;
    	if(pCUTmp->CURefCnt == 0){
    		m_pCUList = g_list_remove(m_pCUList,(void *)pCUTmp);
    		memset(pCUTmp,0,sizeof(struct _cu_link));
    	}
    	pCurrent = g_list_next(pCurrent);
    }
    g_mutex_unlock(m_subscribeListMutex);


    /*Delete pu node*/
    m_pTransferList = g_list_remove(m_pTransferList,(void*)pTransfer);
    memset(pTransfer,0,sizeof(struct _mdutransferentry));

    g_mutex_unlock(m_transferListMutex);
    /*structure setting 0*/
    //memset(puTmpTag,0,sizeof(PUFLOW));
    top_log(MDU_COMMUNIT_DOMAIN,"show log after destory pu list");
	return TOP_SUCCESS;
}

/*
Description:send feedback about audio/video to cmu
@argv:
	msgStr:  feedback message from csg
	msgHead: message head from csg
	nMsgType:MDU_VOD_OPEN,MDU_AOD_OPEN and so on
@return:
	SUCCESS	TOP_SUCCESS
	FAIL	others
@Created By huaixuzhi in 2012-3-1
 */
gint    sendodFBToCmu(gchar *msgStr,MSGHEAD *msgHead,gushort	nMsgType)
{
    struct	_cmu_data_tag	csgFBmsg;
    struct  mduSrvTag *srv;
	struct	_stream_key		puFlowTag;
    struct	_mdutransferentry	*pTransfer = NULL;
    GList	*pFirst = NULL,*pCurrent = NULL;
    gchar	MsgToCmu[MAXLINE];
    gssize	lentmp = 0,len = 0;
    guint	rc = 0,status = 0,mduIDTmp = 0;
    gboolean	existFlag = FALSE;

	srv = m_CmuSockSrv;
	memset(&csgFBmsg, 0, sizeof(struct _cmu_data_tag));
    /*Get feedback from csg*/
    lentmp = sizeof(MSGHEAD);
    len = parseOPRESULT(msgStr + lentmp,&csgFBmsg.opresult);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse OPRESULT");
    }
       lentmp += len;
    len = parsePUID(msgStr + lentmp,&csgFBmsg.puid);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse PUID");
    }
    lentmp += len;
    len = parseChannelID(msgStr + lentmp,&csgFBmsg.channelid);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse ChannelID");
    }
    lentmp += len;
	if(nMsgType == MDU_VOD_OPEN || nMsgType == MDU_VOD_CLOSE)
	{
    	len = parseStreamID(msgStr + lentmp,&csgFBmsg.streamid);
    	if(0 == len)
    	{
    		g_error("msgFeedBacktoCmu parse StreamID");
    	}
	}

    msgHead->nsMsgType = htons(nMsgType);

	/*Get cuid from List*/
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;
    g_mutex_lock(m_transferListMutex);
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutransferentry*)pCurrent->data;
        if(pTransfer == NULL)
        {
           	//put out information of error
        }
        if(pTransfer->mStreamKey.mpuid == csgFBmsg.puid &&
    			pTransfer->mStreamKey.mchannelid == csgFBmsg.channelid &&
    			pTransfer->mStreamKey.mstreamtype == csgFBmsg.streamid &&
    			pTransfer->mStreamKey.mmediatype == MEDIATYPEVOD)
        {
           	/*Get cuid from pulist*/
			csgFBmsg.cuinfotag.uid = pTransfer->firstcuid;
			existFlag = TRUE; 
			break;
		}
       	pCurrent = g_list_next(pCurrent);
	}
    g_mutex_unlock(m_transferListMutex);

	if( !existFlag )
	{
		top_log(MDU_COMMUNIT_DOMAIN,
				"RECEIVE FALSE message from csg,BUT pu+channel do not exist");
		return TOP_SUCCESS;
	}

	/*judge the feedback from csg is failure or not,if the recived 
	 information from csg is open_failure,then remove pu_cuinfo*/
    if((csgFBmsg.opresult == 2)&&(nMsgType == MDU_VOD_OPEN || nMsgType == MDU_AOD_OPEN))
    {
		memset(&puFlowTag,0,sizeof(STREAMKEY));
    	/* Deal with the disconnect infomation from csg*/
    	g_message("Fail Message from csg about vod open");
    	top_log(MDU_COMMUNIT_DOMAIN,"Fail Message from csg about vod open");
    	//memset(puFlowTag,0,sizeof(struct _pu_flow_tag));
    	puFlowTag.mpuid			= csgFBmsg.puid;
    	puFlowTag.mchannelid	= csgFBmsg.channelid;
    	puFlowTag.mstreamtype	= csgFBmsg.streamid;

    	/*Open pu fail and should destory the list*/
    	rc = destoryPUList(&puFlowTag);
    	if(rc != TOP_SUCCESS)
    	{
    		top_log(MDU_COMMUNIT_DOMAIN,"ERROR:Destory pu list because stream stop");
    	}
    }

    /*Group Message information in buf and send buf to CMU*/
    memset(MsgToCmu,0,MAXLINE);

    lentmp = sizeof(MSGHEAD);
    len = buildOPRESULT(MsgToCmu + lentmp,&csgFBmsg.opresult);
    lentmp += len;
    len = buildPUID(MsgToCmu + lentmp,&csgFBmsg.puid);
    lentmp += len;
    len = buildChannelID(MsgToCmu + lentmp,&csgFBmsg.channelid);
    lentmp += len;
	if(nMsgType == MDU_VOD_OPEN || nMsgType == MDU_VOD_CLOSE)
	{
		/*vod feedback need streamid to cmu*/
        len = buildStreamID(MsgToCmu + lentmp,&csgFBmsg.streamid);
        lentmp += len;
	}
    len = buildCUID(MsgToCmu + lentmp,&csgFBmsg.cuinfotag.uid);
    lentmp += len;

    mduIDTmp = htonl(m_CmuSockSrv->mduId);
    len = buildMDUID(MsgToCmu + lentmp,&mduIDTmp);
    lentmp += len;

    msgHead->nsTotalLen = htons(lentmp);

    memcpy(MsgToCmu,msgHead,sizeof(MSGHEAD));

    status = activeSock_sendto(srv->asock,&srv->send_key,MsgToCmu,&lentmp,
    		0,&srv->addr,sizeof(top_sockaddr_in));
    g_message("Send to cmu feedback message success");
    top_log(MDU_COMMUNIT_DOMAIN,"Send to cmu feedback message success");
    top_log(MDU_COMMUNIT_DOMAIN,"HAVE NO PU list ,feed back CUID is %x",
    		csgFBmsg.cuinfotag.uid);
    top_log(MDU_COMMUNIT_DOMAIN,"CMU IP is %x PORT is %d",
    		srv->addr.sin_addr,srv->addr.sin_port);
    if(status != TOP_SUCCESS)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,"ERROR:Send message from CSG Failed");
    }
    return TOP_SUCCESS;
}

/*
Description:send feedback about talkback to cmu
 */
gint    sendFBToCmu(gchar *msgStr,MSGHEAD *msgHead,gushort	nMsgType)
{
    struct	_cmu_data_tag csgmsg;
    struct  mduSrvTag *srv;

    gchar	datamsg[MAXLINE] = {0};
    gssize	lentmp = 0,len = 0;
    guint	status = 0,lmduid = 0;

    srv = m_CmuSockSrv;
	memset(&csgmsg, 0, sizeof(struct _cmu_data_tag));
    /*Get feedback from csg*/
    lentmp = sizeof(MSGHEAD);
    len = parseOPRESULT(msgStr + lentmp,&csgmsg.opresult);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse OPRESULT");
    }
       lentmp += len;
    len = parsePUID(msgStr + lentmp,&csgmsg.puid);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse PUID");
    }
    lentmp += len;
    len = parseChannelID(msgStr + lentmp,&csgmsg.channelid);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse ChannelID");
    }
    lentmp += len;
	if(TAG_CUID != msgStr[lentmp])
	{
		top_log(MDU_COMMUNIT_DOMAIN,"parsecuid fail,msgStr[lentmp] = %x",
				msgStr[lentmp]);
		g_message("parsecuid fail CUID_TAG = %x ;msgStr[lentmp] = %x",
				TAG_CUID,msgStr[lentmp]);
		return TOP_SUCCESS;
	}
    len = parseCUID(msgStr + lentmp,&csgmsg.cuinfotag.uid);
    if(0 == len)
    {
    	g_error("msgFeedBacktoCmu parse CUID");
    }

    msgHead->nsMsgType = htons(nMsgType);

    /*Group Message information in buf and send buf to CMU*/
    lentmp = sizeof(MSGHEAD);
    len = buildOPRESULT(datamsg + lentmp,&csgmsg.opresult);
    lentmp += len;
    len = buildPUID(datamsg + lentmp,&csgmsg.puid);
    lentmp += len;
    len = buildChannelID(datamsg + lentmp,&csgmsg.channelid);
    lentmp += len;
    len = buildCUID(datamsg + lentmp,&csgmsg.cuinfotag.uid);
    lentmp += len;
    lmduid = htonl(m_CmuSockSrv->mduId);
    len = buildMDUID(datamsg + lentmp,&lmduid);
    lentmp += len;

    msgHead->nsTotalLen = htons(lentmp);

    memcpy(datamsg,msgHead,sizeof(MSGHEAD));

    status = activeSock_sendto(srv->asock,&srv->send_key,datamsg,&lentmp,
    		0,&srv->addr,sizeof(top_sockaddr_in));
    g_message("Send to cmu feedback message success");
    top_log(MDU_COMMUNIT_DOMAIN,"Send to cmu feedback message success");
    top_log(MDU_COMMUNIT_DOMAIN,"HAVE NO PU list ,feed back CUID is %x",
    		csgmsg.cuinfotag.uid);
    top_log(MDU_COMMUNIT_DOMAIN,"CMU IP is %x PORT is %d",
    		srv->addr.sin_addr,srv->addr.sin_port);
    if(status != TOP_SUCCESS)
    {
    	top_log(MDU_COMMUNIT_DOMAIN,"ERROR:Send message from CSG Failed");
    }
	return TOP_SUCCESS;
}

/*
Description:a recv func,from CMU and parse data
@argv:
    asock:structure keeping key and so on
    data:data recieving from CMU
    size:size of data
    dst_addr:Address of CMU
    addr_len:length of the address
    status:The flag if recieve data success or not
@return:
    	success		TRUE
    	fail		FALSE
*/
static gboolean mdu_datafrom_cmu(ACTIVSOCK    *asock,
        void *data,
        gsize  size,
        const top_sockaddr_t *dst_addr,
        int addr_len,
        gint status)
{
    MSGHEAD	msg_head;	//structure keeping head of message from cmu
    					//--including:length of message/type of message
    					//--SN/version information and so on
    gushort	msgtype;	//to judge type of message(EX:VOD/AOD and so on)
	gushort msgCsgType;
    guint	msgheadlen = 0;			//length of message head
    gint	rc = 0;
	guint	nodeFlag = FIRST_MDU_NODE;//FIRST_NODE == 1;OTHER_NODE == 0
    gchar	dataMsg[MAXLINE];		//Message from cmu

	/*Judge reciving success or not*/
    if(status != TOP_SUCCESS)
    {
    	g_error("Recieve Message from cmu failed.");
    }
    /*Judage the message have recived is NULL or not*/
    if( 0 == size )
    {
    	g_error("The Message recieved from Cmu is NULL");
    }

 	memcpy(&dataMsg, data, size);
    /* parse data from cmu ,transmit information into msg_head*/
    msgheadlen = sizeof(MSGHEAD);
    parseMsgHead((gchar *)&dataMsg, size, &msg_head);

    /*structure keep informatin from CMU*/
    g_message("Getting message from cmu..");
    top_log(MDU_COMMUNIT_DOMAIN,"Getting message from cmu..");

    msgtype = ntohs(msg_head.nsMsgType);

    switch(msgtype)
    {
    	/*recv is reply of keeping alive*/
    	case MDU_KEEPALIVE:
    		g_message("keep alive feedback from cmu. *****");
    		top_log(MDU_COMMUNIT_DOMAIN,"keep alive feedback from cmu. *****");
    		updateKeepAlive();
    		break;
    	/*Feedback of regist from cmu*/
    	case MDU_LOGIN:
    		updateLoginInfo(&dataMsg,size);
    		break;
    	/*The opening of video on demand(VoD)*/
    	case MDU_VOD_OPEN:
    		/*Send confirmation message to cmu*/
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- VOD OPEN");
    		}
			msgCsgType = CSG_VOD_OPEN;
			odDataOpen(&dataMsg, msgCsgType, size, nodeFlag);
			g_message("odDataOpen function end");
    		break;

    	/*The closing of video on demand(VoD)*/
    	case MDU_VOD_CLOSE:
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- VOD CLOSE");
    		}
			msgCsgType = CSG_VOD_CLOSE;
    		odDataClose(&dataMsg, msgCsgType);
    		break;
    	/*The opening of Audio on demand(AoD)*/
    	case MDU_AOD_OPEN:
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- AOD OPEN");
    		}
			msgCsgType = CSG_AOD_OPEN;
    		odDataOpen(&dataMsg, msgCsgType, size, nodeFlag);
    		break;
    	/*The closing of Audio on demand(AoD)*/
    	case MDU_AOD_CLOSE:
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- AOD CLOSE");
    		}
			msgCsgType = CSG_AOD_CLOSE;
    	    odDataClose(&dataMsg,msgCsgType);
    		break;
    	/*The opening/closing of Talkback*/
    	case MDU_TALKBACK_OPEN:
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- VOD CLOSE");
    		}
    		rc = talkBackOpen(&dataMsg);
			if(rc != TOP_SUCCESS)
			{
				//TODO send talkback fail message to cmu 
			}
    		break;
    	case MDU_TALKBACK_CLOSE:
    		rc = sendConfirmTocmu(&msg_head);
    		if(rc != TOP_SUCCESS)
    		{
    			g_error("Send Confirm To cmu Failed -- VOD CLOSE");
    		}
    		rc = talkBackClose(&dataMsg);
			if(rc != TOP_SUCCESS)
			{
				//TODO send talkback fail message to cmu 
			}
    		break;

    	/*broadcast*/
    	case MDU_BROADCAST_OPEN:
    		rc = sendConfirmTocmu(&msg_head);
    		if( rc != TOP_SUCCESS )
    		{
    			g_error("Send Confirm To cmu Failed -- BROADCAST OPEN");
    		}
    		//broadcastOpen(&dataMsg,size);
    		break;
    	case MDU_BROADCAST_CLOSE:
    		rc = sendConfirmTocmu(&msg_head);
    		if ( rc != TOP_SUCCESS )
    		{
    			g_error("Send Confirm to cmu Failed -- BROADCAST CLOSE");
    		}
    		//broadcastClose(&dataMsg,size);
    		break;
    	case MDU_STREAM_BREAK:
    		break;
    	default:
    		g_message("Recv from cmu is %x ",msgtype);
    		break;
    }
	return TRUE;
}

/*
 Description:receive from csg and parse data packet,then
    	sendto CU through RTPPort
@argv:
    asock:tmpbufACTIVSOCK
    data:data from csg
    size:size of data
    dst_addr:addr of recv from
    addr_len:size of addr
    status:the return value from receive function
@return:
    success	TRUE
    fail	others
*/
static	gboolean    mdu_datafrom_csg(ACTIVSOCK	*asock,
    	void *data,
    	gsize	size,
        const top_sockaddr_t *dst_addr,
    	int addr_len,
    	gint status)
{
    gint addrlen;
    gint rc = 0;
    gboolean    bFind = FALSE;		//Flag--search pu info
    GTimeVal	tnow;
    STREAMKEY	pkey;				//key to search pu in list
    /*stream transfer relationship table*/
    struct	_media_data_head	mediaDataHead;		//keep head of data from csg
    struct  _mdutransferentry   *pTransfer = NULL;
    struct  mduSrvTag	*srv	= NULL;
    struct  _cu_link	*pCuData= NULL;

    GList   *pFirst = NULL,*pCurrent = NULL;    //List keeping PU
    GList	*pSecond = NULL,*pCUCurrent = NULL;	//List keeping CU

    if( status != TOP_SUCCESS )
    {
    	g_error("Recive data from CSG failed");
    }
    if( 0 == size )
    {
    	g_error("The data recieved from Cmu is NULL");
    }

	srv	= m_CUSockSrv;
    memset(&pkey,0,sizeof(STREAMKEY));
    memset(&mediaDataHead,0,sizeof(struct _media_data_head));
    /*Get the head of the date*/
    memcpy(&mediaDataHead,data,sizeof(struct _media_data_head));

    /*Traverse List and Get CUs wishing the data*/
    /*Get pu info from datahead,and keep in pkey*/
    pkey.mpuid			=	mediaDataHead.puid;
    pkey.mchannelid		=	mediaDataHead.channelid;
    pkey.mstreamtype	=	mediaDataHead.streamtype;
    pkey.mmediatype		=	mediaDataHead.mediatype;

    /*Get the first element form m_pTransferList;
    This list keep information of _mdutransferentry;
    and we can operate PU&CU thougth the list*/
    /*And update the time of pu exist*/
    pFirst = g_list_first(m_pTransferList);
    pCurrent = pFirst;
    /*Search information of PU from m_pTransferList
    list->data can get _mdutransferentry
          _mdutransferentry->mStreamKey get info of PUs*/
    g_mutex_lock(m_transferListMutex);
    while(pCurrent != NULL)
    {
        pTransfer = (struct _mdutransferentry*)pCurrent->data;
        if(pTransfer == NULL)
        {
           	//put out information of error
        }
        if(pTransfer->mStreamKey.mpuid == pkey.mpuid &&
    			pTransfer->mStreamKey.mchannelid == pkey.mchannelid &&
    			pTransfer->mStreamKey.mstreamtype == pkey.mstreamtype &&
    			pTransfer->mStreamKey.mmediatype == pkey.mmediatype)
        {
           	//find it and transfer data
            bFind   =   TRUE;
    		pSecond	=	g_list_first(pTransfer->mCUList);
    		pCUCurrent=	pSecond;
    		g_mutex_lock(m_subscribeListMutex);
    		while (pCUCurrent != NULL)
    		{
    			pCuData = (CULINK	*)pCUCurrent->data;
    			if(pCuData == NULL)
    			{
    				//error,delete the pu node from List
    			}
    			addrlen = sizeof(top_sockaddr_in);

    			rc = activeSock_sendto(srv->asock,&srv->send_key,
    					data,(gssize *)&size,0,&pCuData->transAddr,addrlen);
    			if (rc != TOP_SUCCESS)
    			{
    				g_message("ERROR:Data sendto cu fail,status=%d",rc);
    			}
           	 	pCUCurrent = g_list_next(pCUCurrent);
    		}
    		g_mutex_unlock(m_subscribeListMutex);
			/*update time of the stream*/
			if(pTransfer->mduNode == FIRST_MDU_NODE)
			{
				/*The node connects to cu*/
    			g_get_current_time(&tnow);
    			pTransfer->puLastTime = tnow.tv_sec;
			}
   			break;
       	}
       	pCurrent = g_list_next(pCurrent);
    }
    g_mutex_unlock(m_transferListMutex);
    if(!bFind )
    {
    	//g_message("Get stream don't need");
   	}
    return TRUE;
}

/*
 Description:receive message from csg and parse data packet,
 	then send feedback to cmu
 @argv:
    asock:tmpbufACTIVSOCK
    data:data from csg
    size:size of data
    dst_addr:addr of recv from
    addr_len:size of addr
    status:the return value from receive function
@return:
    success	TRUE
    fail	others
*/
static	gboolean    mdu_msgfrom_csg(ACTIVSOCK	*asock,
    	void *data,
    	gsize	size,
        const top_sockaddr_t *dst_addr,
    	int addr_len,
    	gint status)
{
    guint	headType;
    guint	rc;				//Judge the message confirm or feedback
    MSGHEAD	msg_head;		//head of message from csg

    if( status != TOP_SUCCESS )
    {
    	g_error("Recieve Message from csg failed.");
    }
    if ( 0 == size )
    {
    	g_error("The Message recieved from Cmu is NULL");
    }
    g_message("The size of Message from Csg is %d ",size);
    top_log(MDU_COMMUNIT_DOMAIN,"The size of Message from Csg is %d ",size);

	memset(&msg_head,0,sizeof(MSGHEAD));

    parseMsgHead(data,size,&msg_head);
    rc = msg_head.nTimestamp;
    if(rc == COMFIRM_MSG)
    {
    	//get confirm message from csg
    	//--Csg have recived my message
    	g_message("Recv confirmation from csg rc = %x ",rc);
    }
    else
    {
   	    headType = ntohs(msg_head.nsMsgType);
    	g_message("Recv feedback from csg msgtype = %x",headType);
    	top_log(MDU_COMMUNIT_DOMAIN,
    		"Recv feedback from csg msgtype = %x",headType);

    	/*judgment of message type*/
    	switch(headType)
    	{
    		case CSG_VOD_OPEN:
    			sendodFBToCmu(data,&msg_head,MDU_VOD_OPEN);
    			break;
    		case CSG_VOD_CLOSE:
    			sendodFBToCmu(data,&msg_head,MDU_VOD_CLOSE);
    			break;
    		case CSG_AOD_OPEN:
    			sendodFBToCmu(data,&msg_head,MDU_AOD_OPEN);
    			break;
    		case CSG_AOD_CLOSE:
    			sendodFBToCmu(data,&msg_head,MDU_AOD_CLOSE);
    			break;
    		case CSG_TALKBACK_OPEN:
    			sendFBToCmu(data,&msg_head,MDU_TALKBACK_OPEN);
    			break;
    		case CSG_TALKBACK_CLOSE:
    			sendFBToCmu(data,&msg_head,MDU_TALKBACK_CLOSE);
    			break;
    		case CSG_BROADCAST_OPEN:
    			//TODO
    			//sendBroFBToCmu(data,&msg_head,MDU_BROADCAST_OPEN);
    			break;
    		case CSG_BROADCAST_CLOSE:
    			//TODO
    			//sendBroFBToCmu(data,&msg_head,MDU_BROADCAST_CLOSE);
    			break;
    		default:
    			break;
    	}
    }/* end of else*/
	return TRUE;
}

/*
Desctiption:Recieve keeping alive message from CU,and parse the
    message head to compare with cuinfo in list.
@argv:
    asock:structure keeping key and so on
    data:data recieving from mdu
    size:size of data
    dst_addr:Address of mdu
    addr_len:length of the address
    status:The flag if recieve data success or not
@return:
    	success		TRUE
    	fail		FALSE
*/
gboolean    mdu_msgfrom_cu(ACTIVSOCK	*asock,
    	void *data,
    	gsize	size,
        const top_sockaddr_t *dst_addr,
    	int addr_len,
    	gint status)
{
    //compare with cuinfo in list;if different then update
    gchar keepaliveBuf[MAXLINE] = {0};
    gboolean	bFind = FALSE;
    gint	rc = 0;
    gint	len = 0,lentmp = 0;
	guint	transcuid = 0;		//CUID from cu's keepalive
    MSGHEAD	msg_head;
    top_sockaddr_in	*cuaddr = (top_sockaddr_in *)dst_addr;

    if( status != TOP_SUCCESS )
    {
    	g_error("Recieve FeedBack from cu failed.");
    }
    if( 0 == size )
    {
    	g_error("The FeedBack recieved from Cu is NULL");
    }
	/*keep message from cu in keepaliveBuf*/
    memcpy(keepaliveBuf,data,size);
	memset(&msg_head, 0, sizeof(MSGHEAD));
    parseMsgHead(keepaliveBuf, size + 1, &msg_head);

    rc = ntohs(msg_head.nsMsgType);
    if( rc == MDU_KEEPALIVE )
    {
    	//Have recieved talkback from cu
    	CULINK		*pSearch = NULL;
    	GList		*pFirst	 = NULL;	//HeadNode of the list
    	GList		*pCurrent= NULL;	//Current Node of the List
    	gchar		*cuip= NULL;
    	gchar		oPResultTmp;
    	gushort		cuport;
    	GTimeVal 	tnow;

    	/*Get CUID from cu's talkback message*/
    	lentmp = sizeof(MSGHEAD);
    	len = parseOPRESULT((gchar *)data + lentmp,&oPResultTmp);
    	if(0 == len || oPResultTmp != 0)
    	{
    		g_error("mdu_msgfrom_cu parse OPRESULT");
    	}
    	lentmp = len + lentmp;
    	len = parseCUID((gchar *)data + lentmp,&transcuid);
    	if(0 == len)
    	{
    		g_error("mdu_msgfrom_cu parseCUID");
    	}

    	/*Get Ip address of cu*/
    	cuip	= top_inet_ntoa(cuaddr->sin_addr);
    	cuport	= cuaddr->sin_port;
        g_message("IP and of CU from keepalive is %s ,%x",cuip,ntohs(cuport));

    	/*Search CUList for CUID*/
    	g_mutex_lock(m_subscribeListMutex);
    	pFirst = g_list_first(m_pCUList);
    	pCurrent = pFirst;
    	while( pCurrent != NULL )
    	{
    		pSearch = (CULINK *)pCurrent->data;
    		if(pSearch == NULL)
    		{
    			top_log(MDU_COMMUNIT_DOMAIN,"No CU information in List");
    			//send warning to cmu
    		}
    		if( pSearch->mCUId == transcuid)
    		{
				/*Find cuid from CULIST,If ip || port from cu isn't same
				 as the ip&&port from cmu,then replace*/
    			bFind = TRUE;
    			if(pSearch->transAddr.sin_port == cuport &&
    					pSearch->transAddr.sin_addr.s_addr == cuaddr->sin_addr.s_addr)
    			{
    				/*compare info between cu/cmu sending*/
    				g_message("CUid info is same as cum sended");
    				top_log(MDU_COMMUNIT_DOMAIN,"CUid info is same as cum sended");
    			}
    			else
    			{
    				/*Message is different and will be reset*/
    				g_message("CUID info isn't same as cmu sended");
    				top_log(MDU_COMMUNIT_DOMAIN,"CUID info isn't same as cmu sended");
					top_log(MDU_COMMUNIT_DOMAIN,"New CUADDR IS ip=%s,port=%d",
							cuip,cuport);
    				pSearch->transAddr.sin_port = cuport;
					pSearch->transAddr.sin_addr = cuaddr->sin_addr;
    			}

    			/*UpDate the Last active time of the cu*/
    			g_get_current_time(&tnow);
    			pSearch->cuLastTime = tnow.tv_sec;
				break;
    		}
    		pCurrent = g_list_next(pCurrent);
    	}
    	if ( !bFind )
    	{
    		/*There is no need cu in the list and do not dispose*/
    		g_message("No need CUID in List");
    		top_log(MDU_COMMUNIT_DOMAIN,"No need CUID in List");
    	}
    	g_mutex_unlock(m_subscribeListMutex);
    	return TRUE;
    }
    else
    {
		dataSendtoCsg((gchar *)data);
		return TRUE;
    }
    return FALSE;
}

/*
Description:a recv func,from  cascade and parse data
@argv:
    asock:structure keeping key and so on
    data:data recieving from mdu
    size:size of data
    dst_addr:Address of mdu
    addr_len:length of the address
    status:The flag if recieve data success or not
@return:
    	success		TRUE
    	fail		FALSE
*/
static gboolean mdu_msgfrom_cas(ACTIVSOCK    *asock,
        void *data,
        gsize  size,
        const top_sockaddr_t *dst_addr,
        int addr_len,
        gint status)
{
	guint	rc = 0;

    /*Judge reciving success or not*/
    if(status != TOP_SUCCESS)
    {
    	g_error("Recieve Message from cascade failed.");
    }
    /*Judage the message have recived is NULL or not*/
    if( 0 == size )
    {
    	g_error("The Message recieved from cascade is NULL");
    }

    /* parse data from mdu */
   	/*recv from front mdu(cmu message in data)*/
    g_message("Recive from front mdu of cmuMessage");
    top_log(MDU_COMMUNIT_DOMAIN,"Recive from front mdu of cmuMessage");
	rc = judgeOrderMsgTpye(data,size);
	if (rc != TOP_SUCCESS)
	{
		top_log(MDU_COMMUNIT_DOMAIN,"judgeOrderMsgType fail");
	}
    return TRUE;
}

/*
Descriprion:To judge time between sending message to csg and
    recieving data from csg,if the time greater than 15s,then
    destory the list about the pu,and set 0 to structure,and
    send messge to cmu
@argv:
@return:
 */
gpointer puExistTime(struct mduSrvTag *srvTmp)
{
    GTimeVal	tlast,tnow;
    gboolean	bFind = FALSE;
    struct	_mdutransferentry	*pTransfer = NULL;
	struct	_cu_link	*cuTransfer = NULL;
    GList	*pFirst = NULL,*pCurrent = NULL;
	GList	*cuFirst = NULL,*cuCurrent = NULL;

    g_get_current_time(&tlast);
    while(1)
    {
        pFirst = g_list_first(m_pTransferList);
		if(pFirst == NULL)
		{
			g_usleep(5000000);
		}
		else
		{
    		g_get_current_time(&tnow);
    		if(( tnow.tv_sec - tlast.tv_sec ) < 5)
    		{
    			g_usleep(5000000);
    			continue;
    		}
    		tlast.tv_sec = tnow.tv_sec;

			pCurrent = pFirst;
    		g_mutex_lock(m_transferListMutex);
   			while(pCurrent != NULL)
    		{
    			//Test
    			pTransfer = (struct _mdutransferentry*)pCurrent->data;
    			if(pTransfer == NULL)
    			{
    			//put out information of error
    			}
    			if(pTransfer->mduNode == FIRST_MDU_NODE)
				{
					if(tlast.tv_sec - pTransfer->puLastTime > 3*5)
					{
						/*the stream is timeout,remove pu list*/
						bFind = TRUE;
    					streamStopToCmu(&pTransfer->mStreamKey);
						top_log(MDU_COMMUNIT_DOMAIN,"");
						top_log(MDU_COMMUNIT_DOMAIN,
								"pu %x stream stop",pTransfer->mStreamKey.mpuid);
					}
				}
				if(bFind == TRUE)
				{
					/*Find one stream timeout ,and remove the info from list*/
					bFind = FALSE;
    				/*Delete all the nodes of cu about the pu*/
   	 				cuFirst = g_list_first(pTransfer->mCUList);
    				cuCurrent = cuFirst;

    				g_mutex_lock(m_subscribeListMutex);
    				while(cuCurrent != NULL)
    				{
    					cuTransfer = (CULINK*)cuCurrent->data;
    					if(cuTransfer == NULL)
    					{
    						//error
    					}
    					/*Delete cu node and set 0*/
        				pTransfer->mCUList = g_list_remove(pTransfer->mCUList,
								(void *)cuTransfer);
    					cuTransfer->CURefCnt--;
    					if(cuTransfer->CURefCnt == 0){
    						m_pCUList = g_list_remove(m_pCUList,(void *)cuTransfer);
    						memset(cuTransfer,0,sizeof(struct _cu_link));
    					}
    					cuCurrent = g_list_next(cuCurrent);
    				}		
    				g_mutex_unlock(m_subscribeListMutex);
    				/*Remove pu node*/
    				m_pTransferList = g_list_remove(m_pTransferList,(void*)pTransfer);
    				memset(pTransfer,0,sizeof(struct _mdutransferentry));
				}
    	    	pCurrent = g_list_next(pCurrent);
			    g_mutex_unlock(m_transferListMutex);
			}/*end of while*/
		}/*PUList isn't emputy*/
    }/*end of while(1)*/
}



/*
Description:regist and keep alive to cmu. Judge if time out and
    then regist again.
@argv:_MDU_conf---the Configuration information of MDU's file
*/
gpointer KeepAliveToCmu(struct mduSrvTag *srvTmp)
{
    GTimeVal tlast, tnow;    	//Time to judge time out
    gint rc = 0;

    g_get_current_time(&tlast);
    if( !m_stGlobe.bRegister )
    {
    	/*Send Regist message to cmu*/
    	SendReg();
    	g_usleep(10000000);
    }
    g_message("KeepAliveToCmu func");
    top_log(MDU_COMMUNIT_DOMAIN,"KeepAliveToCmu func");

    while(1)
    {
        g_get_current_time(&tnow);
        if(( tnow.tv_sec - tlast.tv_sec ) < 1)
    	{
           // time is not up
           g_usleep(1000000);
           continue;
        }
        // time up ,update last time
        tlast.tv_sec = tnow.tv_sec;
        // so long time not receive keep alive packet
        if( (tnow.tv_sec - m_stGlobe.LastActiveTime) >= 10*6){
    	//////////////////////////////////////////////////////////////////
    	//Modified by huaixuzhi in 2011-9-16
    	/*When keepalive from cmu time out to cmu ,destory the List */
			g_message("EXIT FROM MDU FUCNTION BECAUSE OF CMU'S TIMEOUT");
 			top_log(MDU_COMMUNIT_DOMAIN,
				"EXIT FROM MDU FUCNTION BECAUSE OF CMU'S TIMEOUT");
   			rc = emputyList();
    		if( rc != TOP_SUCCESS )
    		{
    			g_error("Clean The m_ and List");
    		}
    		//DEMAON
    		exit(0);
            m_stGlobe.bRegister = FALSE;
        }
        // register
        if( !m_stGlobe.bRegister)
    	{
    		SendReg();
    	}
    	else				//keeping alive
    	{
    		SendKeepAlive();
        //g_usleep(1000*m_stGlobe.nInterval);
        }
    	g_usleep(10000000);
    }
    return NULL;
}

/*
Description:Judge if Cu time out and then send waring to cmu.
@argv:_MDU_conf---the Configuration information of MDU's file
*/
gpointer KeepAliveFromCu(struct    mduSrvTag	*srv)
{
    GTimeVal tlast, tnow;    	//Time to judge time out
    CULINK		*pSearch = NULL;
    GList		*pFirst	 = NULL;
    GList		*pCurrent= NULL;
    guint		rc = 0,tcuid = 0;
    gboolean	bFind = FALSE;

    g_get_current_time(&tlast);
    g_message("KeepAliveFromCu	func");
    top_log(MDU_COMMUNIT_DOMAIN,"KeepAliveFromCu	func");

    while(1)
    {
    	bFind = FALSE;
        g_get_current_time(&tnow);
        if(( tnow.tv_sec - tlast.tv_sec ) < 10)
    	{
           /*time is not up*/
    		g_usleep(10000000);
            continue;
        }
        /* time up ,update last time*/
        tlast.tv_sec = tnow.tv_sec;
    	/*traverse the list and Judge timeout or not*/
    	g_mutex_lock(m_subscribeListMutex);
    	pFirst = g_list_first(m_pCUList);
    	pCurrent = pFirst;
    	while( pCurrent != NULL )
    	{
    		pSearch = (CULINK *)pCurrent->data;
    		if(pSearch == NULL)
    		{
    			top_log(MDU_COMMUNIT_DOMAIN,"No CU information in List");
    		}
    		if( tlast.tv_sec - pSearch->cuLastTime > 3*10)
    		{
				/*Timeout---keepalive from cu*/
    			rc = sendCUWarning(pSearch->mCUId);
    			if (rc != TOP_SUCCESS)
    			{
    				g_error("Send Cu Warning message failed %d",rc);
    			}
				tcuid = pSearch->mCUId;
    			bFind = TRUE;
    			break;
    		}
    		pCurrent = g_list_next(pCurrent);
    	}
    	g_mutex_unlock(m_subscribeListMutex);

    	if(bFind)
    	{
			top_log(MDU_COMMUNIT_DOMAIN,
					"CU KEEPALIVE TIME OUT,REMOVE CUINFO FROM LIST");
			removeCUInfo(tcuid);
			/*The below function maybe used if we need to do so*/
    		//showListLog();
    	}
    }/*End of while(1)*/
}


/*
Description:create socket,add handle to cb,Associated sockfd with
    key,op_key and cb.
####the socket is used as a command port communicating with CMU
@argv:
    argv[0]:The queue for socket
    argv[1]:structure keeping information from MDU_conf
@return:    success TOP_SUCCESS
    		fail 	others
 */

gint cmuSrvCreate(top_ioqueue_t    *ioqueue,
    	struct _mdu_conf *data)
{
    struct mduSrvTag	*srv;
    top_sockaddr	cmdAddr;
    top_sockaddr_in	cmuAddr;		//Addr keeping cmu info
    int addr_len;
    ACTIVSOCK_CB	cmuMsgcb;
    int status;
    struct	top_in_addr MduIPAddr;
    GString* CmuIpBuf;

    /*init conf*/
    CmuIpBuf = g_string_new(data->cmuip);

    /*add func to cb structure*/
    memset(&cmuMsgcb,0, sizeof(ACTIVSOCK_CB));
    cmuMsgcb.on_data_recvfrom = &mdu_datafrom_cmu;

    /*init srv*/
    srv = (struct mduSrvTag *)g_malloc0(sizeof(struct mduSrvTag));
    memset(&cmuAddr,0,sizeof(top_sockaddr_in));

    /*Add MDUINFO in srv*/
    srv->mduinfotag.uip			=	MduIPAddr.s_addr;
    srv->mduinfotag.uRevPort	= 	htonl(data->rtpdevport);
    srv->mduinfotag.uSendPort	= 	htonl(data->rtpport);
    srv->mduinfotag.uCmdPort	= 	htonl(data->cmdport);
    srv->mduinfotag.uid			=	htonl(data->localid);

    srv->mduId	=	data->localid;

   /*socket creat*/
    top_sockaddr_in_init(&cmdAddr.ipv4,NULL,(unsigned short)data->cmdport);
    addr_len = sizeof(cmdAddr);

    /*Sokect of cmu info*/
    cmuAddr.sin_family = AF_INET;
    cmuAddr.sin_port = htons(data->cmuport);
    top_inet_aton(CmuIpBuf,&cmuAddr.sin_addr);
    srv->addr = cmuAddr;

    g_message("activeSock_create_udp of CmuSrvCreate....");
    top_log(MDU_COMMUNIT_DOMAIN,"activeSock_create_udp of CmuSrvCreate....");
    status = activeSock_create_udp(&cmdAddr, NULL, ioqueue, &cmuMsgcb,
                      srv, &srv->asock, &cmdAddr,65536);
    if (status != TOP_SUCCESS) {
        g_error("activeSock_create() %d",status);
    }

    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));
    g_message("MduToCmu send_key init....");
    top_log(MDU_COMMUNIT_DOMAIN,"MduToCmu send_key init....");

    /*CmuSockSrv as a globle structure*/
    m_CmuSockSrv = srv;
    top_log(MDU_COMMUNIT_DOMAIN,"CMU IP IS %s CMU PORT IS %d",
        data->cmuip,data->cmdport);

    status = activeSock_start_recvfrom(srv->asock,MAXLINE, 0);
    if (status != TOP_SUCCESS) {
        activeSock_close(srv->asock);
        g_error("activesock_start_recvfrom()");
        return status;
    }

    /*create a thread and register/heart to CMU*/
    g_thread_create((GThreadFunc)KeepAliveToCmu,srv,FALSE,NULL);
    /*If Get Keeping alive message from cu has something
     wrong ,send message to cmu*/
//    g_thread_create((GThreadFunc)KeepAliveFromCu,srv,FALSE,NULL);
    /*Judge timeout of pu's data*/
    g_thread_create((GThreadFunc)puExistTime,m_CmuSockSrv,FALSE,NULL);

    g_string_free(CmuIpBuf,TRUE);
    return TOP_SUCCESS;
}

/*
 Description:create socket listening rtpdevport(port to CSG),
    		add handle to cb,associated sockfd with cb,key
    		and op_key
    	and create socket for rtpport(Port to CU)
@argv:
    argv[0]:ioqueue
    argv[1]:TURE/FALSE
    argv[2]:structure keeping information about key/op_key and so on
    argv[3]:Monitor port
@return:
    	success		TOP_SUCCESS
    	fail		other
*/
gint    csgDataSrvCreate(top_ioqueue_t	*ioqueue,
    	guint	port)
{
    struct mduSrvTag	*srv;
    top_sockaddr	addr;
    int addr_len;
    ACTIVSOCK_CB	MduToCsgCb;
    int status	= -1;
    guint Port	=	port;
    gint nRecvbuf=8*1024*1024;
    /*init srv*/
    srv = (struct mduSrvTag *)g_malloc0(sizeof(struct mduSrvTag));

    /*add func to cb structure*/
    memset(&MduToCsgCb, 0,sizeof(ACTIVSOCK_CB));
    MduToCsgCb.on_data_recvfrom = &mdu_datafrom_csg;

   /*socket creat*/
    g_message("csgDataSrvCreate.Creating socket....");
    top_log(MDU_COMMUNIT_DOMAIN,"csgDataSrvCreate.Creating socket....");
    top_sockaddr_in_init(&addr.ipv4,NULL,Port);
    addr_len = sizeof(addr);

    g_message("activeSock_create_udp of MduToCsg...");
    top_log(MDU_COMMUNIT_DOMAIN,"activeSock_create_udp of MduToCsg...");
    status = activeSock_create_udp(&addr, NULL, ioqueue, &MduToCsgCb,
                      srv, &srv->asock, &addr,nRecvbuf);
    if (status != TOP_SUCCESS) {
        g_error("activeSock_create()");
        return status;
    }

    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));

    //g_thread_create(DataSendtoCsg,srv,FALSE,NULL);


    status = activeSock_start_recvfrom(srv->asock,MAXLINE, 0);
    if (status != TOP_SUCCESS) {
        activeSock_close(srv->asock);
        g_error("activesock_start_recvfrom()");
        return status;
    }

	m_CsgDataSrv = srv;

    return TOP_SUCCESS;
}


/*
 Description:create socket listening CsgMsgPort(port to Csg),
    		add handle to cb,associated sockfd with cb,key
    		and op_key
@argv:
    argv[0]:ioqueue
    argv[1]:TURE/FALSE
    argv[2]:structure keeping information about key/op_key and so on
    argv[3]:Monitor port
@return:
    	success		TOP_SUCCESS
    	fail		other
*/
gint    csgMsgSrvCreate(top_ioqueue_t	*ioqueue,
    	guint	port)
{
    struct mduSrvTag	*srv;
    top_sockaddr	addr;
    int addr_len;
    ACTIVSOCK_CB	MduToCsgMsgCb;
    int status	= -1;
    guint Port	=	port;

    /*init srv*/
    srv = (struct mduSrvTag *)g_malloc0(sizeof(struct mduSrvTag));

    /*add func to cb structure*/
    memset(&MduToCsgMsgCb,0, sizeof(ACTIVSOCK_CB));
    MduToCsgMsgCb.on_data_recvfrom = &mdu_msgfrom_csg;

   /*socket creat*/
    g_message("csgMsgSrvCreate.Creating socket....");
    top_log(MDU_COMMUNIT_DOMAIN,"csgMsgSrvCreate.Creating socket....");
    top_sockaddr_in_init(&addr.ipv4, NULL ,Port);
	addr_len = sizeof(addr);

    g_message("activeSock_create_udp of MduToCsgMsg..");
    top_log(MDU_COMMUNIT_DOMAIN,"activeSock_create_udp of MduToCsgMsg..");
    status = activeSock_create_udp(&addr, NULL, ioqueue, &MduToCsgMsgCb,
                      srv, &srv->asock, &addr,65536);
    if (status != TOP_SUCCESS) {
//  top_sock_close(sock_fd);
        g_error("activeSock_create()");
        return status;
    }

    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));

	/*Global*/
    m_CsgSockSrv= srv;

    status = activeSock_start_recvfrom(srv->asock,MAXLINE, 0);
    if (status != TOP_SUCCESS) {
        activeSock_close(srv->asock);
        g_error("activesock_start_recvfrom()");
        return status;
    }

    return TOP_SUCCESS;
}

/*
 Description:Create socket listening the cuport,
	add handle to cb,associated sockfd with cb,key
    and op_key
@argv:
    ioqueue:ioqueue have been created for socket
    port:Monitor port
@return:
    	success		TOP_SUCCESS
    	fail		other
*/
gint	cuSrvCreate(top_ioqueue_t	*ioqueue,
    	guint	port)
{
    struct mduSrvTag	*srv;
    top_sock_t	sock_fd = -1;
    top_sockaddr	addr;
    int addr_len;
    ACTIVSOCK_CB	cuDataCb;
    int status	= -1;
    guint Port	=	port;

    gint nRecvbuf=8*1024*1024;
    /*init srv*/
    srv = (struct mduSrvTag *)g_malloc0(sizeof(struct mduSrvTag));
    memset(srv,0,sizeof(srv));

    /*add func to cb structure*/
    memset(&cuDataCb,0,sizeof(ACTIVSOCK_CB));
    cuDataCb.on_data_recvfrom = &mdu_msgfrom_cu;

   /*socket creat*/
    g_message("cuSrvCreate,Creating socket....");
    top_log(MDU_COMMUNIT_DOMAIN,"cuSrvCreate,Creating socket....");
    top_sock_socket(top_AF_INET(), top_SOCK_DGRAM(), 0, &sock_fd);
    top_sockaddr_in_init(&addr.ipv4, NULL ,Port);
    addr_len = sizeof(addr);

    g_message("activeSock_create_udp of MduToCu...");
    top_log(MDU_COMMUNIT_DOMAIN,"activeSock_create_udp of MduToCu...");
    status = activeSock_create_udp(&addr, NULL, ioqueue, &cuDataCb,
                      srv, &srv->asock, &addr,nRecvbuf);
    if (status != TOP_SUCCESS) {
        top_sock_close(sock_fd);
        g_error("activeSock_create()");
        return status;
    }

    g_message("CsgSockSrv assignment");
    top_log(MDU_COMMUNIT_DOMAIN,"CsgSockSrv assignment");

    top_ioqueue_op_key_init (&srv->send_key,sizeof(top_ioqueue_op_key_t));

//    g_thread_create(DataSendtoCu,srv,FALSE,NULL);

    status = activeSock_start_recvfrom(srv->asock,MAXLINE, 0);
    if (status != TOP_SUCCESS) {
        activeSock_close(srv->asock);
        g_error("activesock_start_recvfrom()");
        return status;
    }

    m_CUSockSrv = srv;
    return TOP_SUCCESS;
}

/*
 Description:Create socket listening the cascade msgport,
	add handle to cb,associated sockfd with cb,key
    and op_key
@argv:
    ioqueue:ioqueue have been created for socket
    port:Monitor port
@return:
   success		TOP_SUCCESS
   fail		other
*/
gint    casMsgSrvCreate(top_ioqueue_t	*ioqueue,
    	guint	port)
{
    struct mduSrvTag	*srv;
    top_sock_t	sock_fd = -1;
    top_sockaddr	addr;
    int addr_len;
    ACTIVSOCK_CB	casMsgCb;
    int status	= -1;

    gint nRecvbuf=8*1024*1024;
    /*init srv*/
    srv = (struct mduSrvTag *)g_malloc0(sizeof(struct mduSrvTag));

    /*add func to cb structure*/
    memset(&casMsgCb,0,sizeof(ACTIVSOCK_CB));
    casMsgCb.on_data_recvfrom = &mdu_msgfrom_cas;

   /*socket creat*/
    g_message("casSrvCreate,Creating socket....");
    top_log(MDU_COMMUNIT_DOMAIN,"casSrvCreate,Creating socket....");
    top_sock_socket(top_AF_INET(), top_SOCK_DGRAM(), 0, &sock_fd);
    top_sockaddr_in_init(&addr.ipv4, NULL, port);
    addr_len = sizeof(top_sockaddr);

    g_message("activeSock_create_udp of mdu to next mdu...");
    top_log(MDU_COMMUNIT_DOMAIN,"activeSock_create_udp of MduToMdu...");
    status = activeSock_create_udp(&addr, NULL, ioqueue, &casMsgCb,
                      srv, &srv->asock, &addr,nRecvbuf);
    if (status != TOP_SUCCESS) {
        top_sock_close(sock_fd);
        g_error("activeSock_create()");
        return status;
    }

    top_ioqueue_op_key_init(&srv->send_key,sizeof(top_ioqueue_op_key_t));

    status = activeSock_start_recvfrom(srv->asock,MAXLINE, 0);
    if (status != TOP_SUCCESS) {
        activeSock_close(srv->asock);
        g_error("activesock_start_recvfrom()");
        return status;
    }
    return TOP_SUCCESS;
}

gpointer DePoll(top_ioqueue_t    *ioqueue)
{
	GTimeVal	delay = {0,1};

    while(1)
    {
    	top_ioqueue_poll(ioqueue,&delay);
    }
    g_message("DePoll");
    top_log(MDU_COMMUNIT_DOMAIN,"DePoll");
}
/*
Description:Analysis of the configuration file;
    		create ioqueue,add socket into ioqueue,ioqueue cycle
@argv:
@return:
    	success		TOP_SUCCESS
    	fail		others
 */
gint mdusockfunc(void)
{
	guint  rc = 0; 
 	/*parse MDU.conf,get the information of CMU_ip,cmuPort,
     	MDU_IP,LocalID and so on*/
    MDUCONFTAG		*mduconf;          //structure keeping configure of MDU

    top_ioqueue_t *ioqueue	= NULL;
	m_subscribeListMutex    = g_mutex_new();
	m_transferListMutex     = g_mutex_new();

    /*parse information thougth PATH of CONFIGURE*/
	rc = mduConfAnal(&mduconf);
	if(rc != TOP_SUCCESS)
	{
		g_error("mduConfAnal error!rc = %d",rc);
	}

    g_message("Create ioqueue.....");
    top_log(MDU_COMMUNIT_DOMAIN,"Create ioqueue.....");
    rc = top_ioqueue_create(8,&ioqueue);		//ioqueue create
    if (rc != TOP_SUCCESS)
    {
    	g_error("top_ioqueue_create.rc = %d",rc);
    	goto	on_return;
    }

    /*Socket set for concurrent mode:FLAG_TURE == 1
    NOTE:if FLAG_FALSE---Not concurrent*/
    g_message("Set concurrency.....");
    top_log(MDU_COMMUNIT_DOMAIN,"Set concurrency.....");
    rc = top_ioqueue_set_default_concurrency(ioqueue,FLAG_TURE);
    if (rc != TOP_SUCCESS)
    {
    	g_error("top_ioqueue_set_default_concurrency");
    	goto	on_return;
    }

    /*create socket and insert into ioqueue;
    information of CMU come from the MDU.conf and not receive from CMU*/
    top_log(MDU_COMMUNIT_DOMAIN,"CmuSrvCreate...");
    rc = cmuSrvCreate(ioqueue,mduconf);
    if(rc != TOP_SUCCESS)
    {
    	g_error("CmuSrvCreate");
    	goto on_return;
    }

    /*Create socket and insert into ioqueue
       Get data from CSG and then feed back sth to CSG*/
    g_message("csgDataSrvCreate....");
    top_log(MDU_COMMUNIT_DOMAIN,"csgDataSrvCreate....");
    rc = csgDataSrvCreate(ioqueue,mduconf->rtpdevport);
    if(rc != TOP_SUCCESS)
    {
    	g_error("csgDataSrvCreate");
    	goto on_return;
    }

    g_message("csgMsgSrvCreate....");
    top_log(MDU_COMMUNIT_DOMAIN,"csgMsgSrvCreate....");
    rc = csgMsgSrvCreate(ioqueue,mduconf->csgmsgport);
    if(rc != TOP_SUCCESS)
    {
    	g_error("csgMsgSrvCreate");
    	goto on_return;
    }

    /*create socket and insert into ioqueue*/
    g_message("cuSrvCreate....");
    top_log(MDU_COMMUNIT_DOMAIN,"cuSrvCreate....");
    rc = cuSrvCreate(ioqueue,mduconf->rtpport);
    if(rc != TOP_SUCCESS)
    {
    	g_error("cuSrvCreate");
    	goto on_return;
    }

	/*cascade control Sockets will be created in below functions*/
	g_message("casMsgSrvCreate...");
	top_log(MDU_COMMUNIT_DOMAIN,"casMsgSrvCreate..");
	rc = casMsgSrvCreate(ioqueue,mduconf->cfrontport);
	if(rc != TOP_SUCCESS)
	{
		g_error("casMsgSrvCreate..");
		goto on_return;
	}

    /*epoll loop*/
    g_message("g_thread_create Depoll");
    top_log(MDU_COMMUNIT_DOMAIN,"g_thread_create Depoll");
    g_thread_create((GThreadFunc)DePoll,ioqueue,FALSE,NULL);

    return TOP_SUCCESS;

on_return:
    if (ioqueue)
    	top_ioqueue_destroy(ioqueue);

}
