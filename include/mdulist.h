/****************************************************
Name:mdulist.h
Author:huaixuzhi
Description:definition of transfer_link structures
Version:
Function List:
    @
History:
    <author>    <time>  <version>   <desc>
 ****************************************************/

#ifndef	_MDU_LIST_H_
#define	_MDU_LIST_H
#include <string.h>
#include "mdusock.h"

/*size of table space;allocate for structure*/
#define	MAX_CALLS	1024
#define	DEL_SUCCESS	0
#define DEL_FALSE	1

/*using in tlv*/

#pragma	pack (1)			//Type of alignment is 1 byte
//
//CSG-->MDU	media data head
//receiving data from CSG:data head + rtp
//
struct	_media_data_head
{
	guchar	Magic[2];	//magic code always = cddc
	guint	puid;		//device id
	guchar	channelid;	//channel	id
	guchar	streamtype;	//main/sub	stream
	guchar	mediatype;	//mediatype
	gushort	length;		//rtp packet length
	gchar	Resrv[1];
};

//
// in the process of the transger streaming
// one stream can be uniquely determined acconding to the following
//
typedef	struct	_stream_key
{
	guint	mpuid;		//device id cmu distribute
	guchar	mchannelid;		//channel id
	guchar	mstreamtype;	//main/sub	stream
	guchar	mmediatype;		//media	type
}STREAMKEY;
//transfer table entry
//create instance base on stream
//relationship of CUs and PUs
struct	_mdutransferentry
{
	gint		firstcuid;		//First cuid connect to the stream
	STREAMKEY	mStreamKey;		//Uniquely identify a stream
	top_sockaddr_in	nextAddr;	//next hop:using send message when stream stop
	guint		puLastTime;		//judge pu online or not
	guint		mduNode;		//Fisrt or other node in List
	GList*		mCUList;		//List of CU
};
//
//List of CU
//
typedef	struct	_cu_link
{
	gint		mCUId;					//uniquely ID from CMU
	/*alter by huaixuzhi in 2012-1-31:IP+port-->addr*/
	top_sockaddr_in	transAddr;			//cu/frontMdu socket info
	guint		cuLastTime;				//Last active time
	gint		CURefCnt;				//reference count
}CULINK;

//
//Added in 2011-7-27
//The information of CSG/MDU.
//Recieve data from cu and send to this
typedef	struct	_csg_key_tag{
	guint		uid;				//Uniquely identifier ID
	guint		refCnt;				//reference	
	top_sockaddr_in nextAddr;		//csg/next mdu socket information
}CSGKEYTAG;

typedef	CSGKEYTAG	MDUKEYTAG;

//Added in 2011-7-19/27
//relationship of CUs and CSG/MDUs
//Using in TALKBACK and BROADCAST
struct	_mdutalkback
{
	CULINK		mCUStreamKey;	//Uniquely	identify a stream from cu
	guint		mStatus;		//Judge	a stream on/off
	GList		*mCSGList;		//List of csg
};

//
//Added in 2011-7-19
//Using for getting data from cu and sending to pu
//Using in TALKBACK	and BROADCAST
struct	_mdutalkback	m_TalkBackTable[MAX_CALLS];
struct	_csg_key_tag	m_CSGLink[MAX_CALLS];
GList	*m_pTalkBackList;		//cu List
GList	*m_pCSGList;			//csg List


//
//stream transfer relationship table
//built --to add or update stream
//destory --to delete stream
struct	_mdutransferentry	m_TransferTable[MAX_CALLS];

//
//CU list
//built is to add or update stream
//destory is to delete stream
struct _cu_link		m_cuLinkArray[MAX_CALLS];

//
//transfer,to operate m_transferTable
//
GList *m_pTransferList;
//
//CU,to operate m_cuLinkArray 
//
GList *m_pCUList;

//
//lock of the list,cuList and puList
GMutex *m_subscribeListMutex;
GMutex *m_transferListMutex;
struct  mduSrvTag *m_CsgSockSrv;                                                       
struct  mduSrvTag *m_CmuSockSrv;                                                       
struct  mduSrvTag *m_CUSockSrv;  
struct  mduSrvTag *m_CsgDataSrv;

//
gint avInsertList(void *key,void *culink,
		top_sockaddr_in *addr,guint flag);

gint removeTransfer(void *key,void *culink);
gint talkBackAddList(void *key,void *insertLink);
gint talkBackRemove(gint cuid);
gint removeCUInfo(gint cuid);
gint showListLog(void);

#pragma	pack ()			//Type of alignment is 1 byte
#endif
