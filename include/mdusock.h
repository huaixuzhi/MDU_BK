/*************************************************
Name:mdusock.h
Author:huaixuzhi
Description:
Function List:
 	#
History:
 	<author>	<time>	<version>	<desc>
 *************************************************/

#ifndef	_MDU_SOCK_H_
#define	_MDU_SOCK_H_

#pragma	pack (1)			//Type of alignment is 1 byte

#define FLAG_TURE   1       //flag for concurrent
#define FLAG_FALSE  0       //flag for notconcurrent
#define	TIMEINTEVAL	10
#define	MDULIST_IN	2		//There is pu in List
#define	MDULIST_OUT	3		//There is no pu in List
#define MAXLINE     2048
#define	MAX_CALLS	1024
#define	keepAliveTime	30
#define MAINSLEEPTIME   10
/////////////////////////////////////////////////////
//Flag signs the node if the first node in mduList,
//Only the First mdu connecting to cu opens pu's timing
#define	FIRST_MDU_NODE	1	//
#define	OTHER_MDU_NODE	0	//
////////////////////////////////////////////////////

//////////////////////////////////////////////////
//Stream Type  to distinguish vod or aod
//use  In mdu list and send message to csg
#define	MEDIATYPEVOD	0	//Stream Type is VOD
#define MEDIATYPEAOD	1	//StreanType is AOD
#define	MEDIATALK		2	//StreamType is TALKBACK
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// COMMAND ID DEFINE
/////////////////////////////////////////////////
#define  MDU_LOGIN	                0xf001
#define  MDU_LOGOUT	                0xf002
#define  MDU_KEEPALIVE	            0xf003
#define  MDU_ALARM	                0xf004
#define  MDU_VERSION	            0xf005
#define  MDU_TIME_SYNC	            0xf006
#define	 MDU_VOD_OPEN				0x1001
#define  MDU_VOD_CLOSE				0x1002
#define	 MDU_AOD_OPEN				0x1003
#define	 MDU_AOD_CLOSE				0x1004
#define  MDU_TALKBACK_OPEN			0x1005
#define	 MDU_TALKBACK_CLOSE			0X1006
#define	 MDU_BROADCAST_OPEN			0x1007
#define	 MDU_BROADCAST_CLOSE		0x1008
#define	 MDU_STREAM_BREAK			0x1009
/////////////////////////////////////////////////
// END OF COMMAND ID DEFINE
/////////////////////////////////////////////////
//COMMAND ID TO CSG
////////////////////////////////////////////////
#define	 CSG_VOD_OPEN				0x4001
#define	 CSG_VOD_CLOSE				0x4002
#define	 CSG_AOD_OPEN				0x4003
#define	 CSG_AOD_CLOSE				0x4004
#define	 CSG_TALKBACK_OPEN			0x4005
#define	 CSG_TALKBACK_CLOSE			0x4006
#define	 CSG_BROADCAST_OPEN			0x4007
#define	 CSG_BROADCAST_CLOSE		0x4008
#define	 CSG_STREAM_BREAK			0x4009
///////////////////////////////////////////////
//END OF COMMAND ID TO CSG
////////////////////////////////////////////////
//COMMAND ID TO CU
///////////////////////////////////////////////
#define	 CU_MDU_TALKBACK			0X0302
#define	 CU_MDU_BROADCAST			0x0303
/////////////////////////////////////////////////
//END OF COMMAND ID TO CU
///////////////////////////////////////////////

//COMFIRM MESSAGE AFTER SENDED MESSAGE
#define	COMFIRM_MSG					0xFFFFFFFF

////////////////////////////////////////////////
//EXTRA HEAD BETWEEN TWO MDUS IN ROUTE
//A short int added before the message head
//Added by huaixuzhi in 2012-2-7
#define	TAG_EXTRA					0xaa
#define	EXTRA_CMU_MSG				0x1001
#define	EXTRA_CSG_FEEDBACK			0x1002
//////////////////////////////////////////////

/////////////////////////////////////////////////
//Parse MDU.conf
/////////////////////////////////////////////////
#define PRASE_FALSE		0
#define	CONF_PATH		"../conf/MDU.conf"

#define	CMUCONF			"conf_of_cmu"
#define	MDUCONF			"conf_of_mdu"
#define	CASCADE			"conf_of_cascade"

#define	LOCALID			"localid"
#define CMDPORT			"cmdport"
#define RTPPORT			"rtpport"
#define	RTPDEVPORT		"rtpdevport"
#define CSGMSGPORT		"csgmsgport"
#define CMUIPCONF		"cmuip"
#define	CMUPORT			"cmuport"
#define	CFRONTPORT		"cfrontport"
//////////////////////////////////////////////
//LOG
/////////////////////////////////////////////////
#define   MDU_COMMUNIT_DOMAIN        "MDUCOMMUNIT"
#define   MDU_COMMUNIT_LOG_PATH      "../log/"
#define   MDU_COMMUNIT_LOG_FILE      "mdulog"
////////////////////////////////////////////////

/*
 Description:structure of MDU configuration
 @include:ID,IP,three ports.
          and cascade ports(three)
 @if need ,can be added
*/
typedef struct _mdu_conf{
        guint localid;		//unique ID of MDU;get from CMU;Default value is 0
        gchar *cmuip; 	   //cmuip and port r used for registe and heart
        guint cmuport;
        guint cmdport;		//default port to get cmu command
        guint rtpport;		//Default port to communicate with cu
        guint rtpdevport;	//Default port to communicate with csg
		guint csgmsgport;	//Default port to transmit message with csg
		guint cfrontport; 	//cascade message port from mdu to mdu(cmu msg)
		guint cnextport;	//cascade message port from mdu (from csg)
		guint csendport;	//cascade data port from (cu)mdu->mdu
		guint crecvport;	//cascade data port from (csg)mdu->mdu
}MDUCONFTAG;


//
//using information of mdusock
//
struct mduSrvTag
{
    ACTIVSOCK           *asock;         //structure keeping key and so on
    top_ioqueue_op_key_t     send_key;    //using for operating send/recv
    guint               mduId;          //unique ID from CMU;the first is 0
    MDUINFO             mduinfotag;     //mdu infomation,getting from conf
                                        //the structure get from top_tlv.h
	top_sockaddr_in		addr;			//socket info
};
//
// Description:keep information from CMU(after TLV analyze)
//
struct _cmu_data_tag
{
    guchar      info_type;          //type of information from cmu
    guchar   	opresult;           //
    guint   	puid;
    guchar   	channelid;
    guchar   	streamid;
    CUINFO  	cuinfotag;          //struct keeping cuinfo,from top_tlv.h
                                	//include uip(cu ip address)/uport/uid
    CSGINFO 	csginfotag;         //struct keeping csginfo like cuinfo
	MDUINFO		mduinfotag;			//struct keeping current mduinfo
	guint		bropuid[MAX_CALLS];	//Using in broadcast,keeping pus from cmu
    //a structure keep router of MDU
	MDUINFO		mduroute;			//keep message of the next mdu
	gchar		mdubuf[MAX_CALLS];	//keep all mduroute info except the next one
};

//
//Registe and keepalive to cmu
//
typedef	struct _cmu_regist_tag
{
	gboolean	bRegister;		//regist OK or not
	guint		LastActiveTime;	//active time
	gchar		*pRegPacket;	//Regist packet
	gchar		*pKeepAlivePkt;	//KeepAlive	packet
}CMUREGIST;

CMUREGIST	m_stGlobe;

//
// Globe block keeping data from csg
//
typedef	struct	_csgdatatag
{
	gssize	datasize;
	gchar	csgdata[MAXLINE];
}CSGDATATAG;

struct	_csgdatatag		m_csgdataGlobe[MAXLINE];

//
//Globe block keeping data from cu
//
typedef	struct _cudatatag
{
	gssize	datasize;
	gchar	cudata[MAXLINE];
}CUDATATAG;

struct	_cudatatag	m_cudataGlobe[MAXLINE];

//
//Globe block keeping message from csg
//This is not in use
typedef	struct	_csgmsgtag
{
	gssize	msgsize;
	gchar	csgmsg[MAXLINE];
}CSGMSGTAG;

struct	_csgmsgtag		m_csgmsgGlobe[MAX_CALLS];

//
//Numeration pool
//
typedef	struct _count_tag
{
	guint	recvCountCsg;
	guint	sendCountCu;
	guint	upuCount;
}COUNTTAG;

struct	_count_tag	countNum;

//
//pu information and cu id,using for send feedback (reciving
//from csg) to cmu
typedef	struct	_keepMsgInfo
{
	guint	m_uid;			//CU ID
	guint	m_pid;			//PU ID
	guint	m_channelid;	//DEV Channel ID
	guchar	m_streamid;		//code stream
	guchar	m_mediatype;	//Media type
}KEEPMSGINFO;

struct	_keepMsgInfo	m_keepMsgInfo[MAX_CALLS];


gint mdusockfunc(void);

#pragma	pack ()			//Type of alignment is 1 byte
#endif
