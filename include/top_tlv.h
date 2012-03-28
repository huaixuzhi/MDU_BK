#ifndef _TOP_TLV_H
#define _TOP_TLV_H

G_BEGIN_DECLS


#pragma pack (1) // Note: �����#pragma pack ()�ɶ�ʹ��
#define  DEFAULT_TLV_TAG_LEN    1
#define  PU_STR_ID_LEN          20
/////////////////////////////////////////////////
// COMMAND ID DEFINE
/////////////////////////////////////////////////
#define  CMD_LOGIN	                0xf001
#define  CMD_LOGOUT	                0xf002
#define  CMD_KEEPALIVE	            0xf003
#define  CMD_ALARM	                0xf004
#define  CMD_VERSION	            0xf005
#define  CMD_TIME_SYNC	            0xf006
#define  CMD_CSG_PTZ	            0x5001
#define  CMD_CSG_PTZ_Transparent	0x5002
#define  CMD_CSG_PU_GPS	            0x5003
#define  CMD_CSG_PU_HARD_VERSION	0x5004
#define  CMD_CSG_PU_VIDEOPARAM	    0x5005
#define  CMD_CSG_PU_AUDIOPARAM	    0x5006
#define  CMD_CSG_PU_MESSAGE	        0x5007
#define  CMD_CSG_PTZ_STOP           0x5008
#define  CMD_CSG_PU_CHANGEWIN       0x5009

#define  CMD_CSG_RECORD_QUERY       0x500a
#define  CMD_CSG_PLAYBACK_OPEN      0x500b
#define  CMD_CSG_PLAYBACK_STOP      0x500c
#define  CMD_CSG_PLAYBACK_PAUSE     0x500d
#define  CMD_CSG_PLAYBACK_RESUME    0x500e
#define  CMD_CSG_PLAYBACK_LOCATION          0x500f  // ��ʱ����
#define  CMD_CSG_PLAYBACK_AUDIO_OPEN        0x5010
#define  CMD_CSG_PLAYBACK_AUDIO_CLOSE       0x5011
#define  CMD_CSG_PU_UPGRADE                 0x50fd
#define  CMD_CSG_XML_MESSAGE                0x50fe
#define  CMD_CSG_XML_COMMAND                0x50ff

#define  CMD_CSG_VOD_OPEN	        0x4001
#define  CMD_CSG_VOD_CLOSE	        0x4002
#define  CMD_CSG_AOD_OPEN	        0x4003
#define  CMD_CSG_AOD_CLOSE	        0x4004
#define  CMD_CSG_TALKBACK_OPEN	    0x4005
#define  CMD_CSG_TALKBACK_CLOSE	    0x4006
#define  CMD_CSG_BROADCAST_OPEN	    0x4007
#define  CMD_CSG_BROADCAST_CLOSE	0x4008
#define  CMD_CSG_STREAM_BREAK	    0x4009

/////////////////////////////////////////////////
// END OF COMMAND ID DEFINE
/////////////////////////////////////////////////


/////////////////////////////////////////////////
// TAG DEFINE
////////////////////////////////////////////////
#define  TAG_CUID                       0x51
#define  TAG_PUID                       0x52
#define  TAG_PUSTRID                    0x53
#define  TAG_ChannelID                  0x54
#define  TAG_CSGID                      0x55
#define  TAG_MDUID                      0x56
#define  TAG_MSUID                      0x57
#define  TAG_CMUID                      0x58
#define  TAG_CUINFO                     0x59
#define  TAG_MDUINFO                    0x5a
#define  TAG_CSGINFO                    0x5b
#define  TAG_MDUROUTE                   0x5c
#define  TAG_AlarmINFO                  0x5d
#define  TAG_AlarmInputID               0x5e
#define  TAG_AlarmOutputID              0x5f
#define  TAG_PTZControl                 0x60
#define  TAG_PTZTransparentData         0x61
#define  TAG_VideoParam                 0x62
#define  TAG_AudioParam                 0x63
#define  TAG_PUMessage                  0x64
#define  TAG_StreamID                   0x65
#define  TAG_PUINFO                     0x66
#define  TAG_StreamType                 0x67
#define  TAG_ProductName                0x68
#define  TAG_IPAddress                  0x69
#define  TAG_IPMask                     0x6a
#define  TAG_MACAddress                 0x6b
#define  TAG_GateWay                    0x6c
#define  TAG_DNS                        0x6d
#define  TAG_Name                       0x6e


#define  TAG_RegisterPort               0x6f
#define  TAG_CSGMessage                 0x70
#define  TAG_MDUMessage                 0x71
#define  TAG_MSUMessage                 0x72
#define  TAG_MSUINFO                    0x73
#define  TAG_WIN_NUM                    0x74
#define  TAG_MBUID                      0x75
#define  TAG_MBUINFO                    0x76
#define  TAG_StorageTypeID              0x77
#define  TAG_TimeSPAN                   0x78
#define  TAG_RelationInfo               0x79
#define  TAG_SPACEInfo                  0x7a
#define  TAG_CHANNELPlan                0x7b

#define  TAG_XML                        0xfb
#define  TAG_LIST                       0xfc
#define  TAG_VerINFO                    0xfd
#define  TAG_AdditionalInfo             0xfe
#define  TAG_OPRESULT                   0xff

#define  TAG_GPS                        0x0b

////////////////////////////////////////////////
// endof TAG DEFINE
////////////////////////////////////////////////
//
// ͨ��Э�鱨��ͷ
//
typedef struct    _msg_head_data
{
    gushort    nsTotalLen;        //
    gushort    nsMsgType;
    guint      nCseq;
    gushort    nVersion;
    guint      nTimestamp;
}MSGHEAD;
//
// ͨ������TLV����
//
typedef struct    _tlv_object
{
    gushort    nTag;
    gushort    nLen;
    void*      pValue;
}TLVOBJ;
// TLV  CUINFO data
typedef struct    _cu_info_data
{
    guint    uip;       // cu ip address
    guint    uport;     // cu port
    guint    uid;       // cu id assigned by cmu
}CUINFO;
// TLV MDUINFO data
typedef struct    _mdu_info_data
{
    guint    uip;         // ip address
    guint    uRevPort;    // receive stream port
    guint    uSendPort;   // send stream port
    guint    uCmdPort;    // command port
    guint    uid;         // mdu id
}MDUINFO;
// TLV 	CSGINFO data
typedef  CUINFO    CSGINFO;
// TLV PUINFO data
typedef struct    _pu_info_data
{
    guint csgid;                        // csg id
    guint puid;                         // pu  id
    guchar putype;                      // pu type 0:3511 1:8180
    guchar pustrid[PU_STR_ID_LEN];      // pu string id
    guint  puip;                        // pu ip address
    void*  pAddition;                   // �������ݣ�һ��Ϊnull
}PUINFO;
// TLV PTZCTRL data
typedef struct   _ptz_control_data
{
    guchar    type;
    gushort   value;
}PTZCTRL;
// TLV Video parameter data
typedef struct _video_param_data
{
    guint BitRate;        //����
    guint BitType;        //0 �����ʣ�1 ������
    guint FrameRate;      //֡�ʷ���
    guint FrameRateDiv;   //֡�ʷ�ĸ
    guint FrameWidth;     //��Ƶ���
    guint FrameHeight;    //��Ƶ�߶�
    guint Standard;       //��ʽ��0 ΪNTSC��1 ΪPAL
    guint HttpPort;       //HTTP�˿ں�
    guint RtpPort;        //RTSP�˿ں�
    guint IpInterval;     //I֡���(1~999)
    guint Palete;         //���ظ�ʽ
    guint QuantMethod;    //ѹ����ʽ
    guint nBlackEdge;     //����ڱ�
    guint nInterlace;     //������
    guint Streamtype;     //��������
}VideoParam;
// TLV audio parameter data
typedef struct _audio_param_data
{
    guint     wFormatTag;           /* format type */
    guint     nChannels;            /* number of channels (i.e. mono, stereo...) */
    guint     nSamplesPerSec;       /* sample rate */
    guint     nAvgBytesPerSec;      /* for buffer estimation */
    guint     nBlockAlign;          /* block size of data */
    guint     wBitsPerSample;       /* number of bits per sample of mono data */
    guint     cbSize;               /* the count in bytes of the size of */
}AudioParam;
/*
NOTE: ���ھ�γ�ȣ���ͨ�淶������Ҫȷ��
�ݶ�����
��һ,���ֽ�Ϊ�ȣ������͵����ֽ�Ϊ�ȵ�С��λ��
��γ�Ⱦ�ȷ���ֵ�С�����4λ���������λ�Ƿ���λ��
���ھ��ȣ�"��"��ʾ"����"��"��"��ʾ"����"��
����ά�ȣ�"��"��ʾ"��γ"��"��"��ʾ"��γ"
*/
typedef struct _gps_upload_data_
{
    gchar     longitude[4];         // ����
    gchar     latitude[4];          // γ��
    gshort    speed;            // �ٶȣ���λΪ����/Сʱ
    gshort    angle;            // ��λ�ǣ�000-359�ȣ�����Ϊ0�ȣ�˳ʱ�뷽�����
    gchar     altitude;         // �߶�
    gint      timestamp;        // �ɼ�ʱ��
    gint      distance;         // ����� ��λ��
}GPSDATA;
//
// set TLV tag length,default is DEFAULT_TLV_TAG_LEN
//
//
void    setTagLength(gshort nLen);
//
// �������ݣ���buf�а���TLV�ṹ������32λ����
// ����: buf ���ݻ���
//       pnRet    ��������32Ϊ����
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseTLV_int32(gchar* buf , guint* pnRet);
//
// �������ݣ���buf�а���TLV�ṹ������16λ����
// ����: buf ���ݻ���
//       pnRet    ��������16Ϊ����
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseTLV_int16(gchar* buf , gushort* pnRet);
//
// �������ݣ���buf�а���TLV�ṹ������8λ����
// ����: buf ���ݻ���
//       pnRet    ��������8Ϊ����
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseTLV_int8(gchar* buf , guchar* pnRet);
//
// �������ݣ���buf�а���TLV�ṹ�������ַ���
// ����: buf ���ݻ���
//       pStr    ���������ַ���
//       len     pStr��������С
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseTLV_str(gchar* buf , guchar* pStr,gint len);
//
// �������ݣ���buf�а���TLV�ṹ����������
// ����: buf ���ݻ���
//       ppval    ��������������buf�е�ָ��
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseTLV_void(gchar* buf , void** ppval);

//
// �������ݣ���buf�а���TLV�ṹ������CUID
// ����: buf ���ݻ���
//       pval    ��������CUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCUID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PUID
// ����: buf ���ݻ���
//       pval    ��������PUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePUID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������CMUID
// ����: buf ���ݻ���
//       pval    ��������CMUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCMUID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������MDUID
// ����: buf ���ݻ���
//       pval    ��������MDUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMDUID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������MSUID
// ����: buf ���ݻ���
//       pval    ��������MSUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMSUID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������CSGID
// ����: buf ���ݻ���
//       pval    ��������CSGID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCSGID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������ChannelID
// ����: buf ���ݻ���
//       pval    ��������ChannelID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseChannelID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PUSTRID
// ����: buf ���ݻ���
//       pval    ��������PUSTRID
//       len     pval���峤��
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePUSTRID(gchar* buf , void* pval,gint len);

//
// �������ݣ���buf�а���TLV�ṹ������CUINFO
// ����: buf ���ݻ���
//       pval    ��������CUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCUINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PUINFO
// ����: buf ���ݻ���
//       pval    ��������PUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePUINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������MDUINFO
// ����: buf ���ݻ���
//       pval    ��������MDUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMDUINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������CSGINFO
// ����: buf ���ݻ���
//       pval    ��������CSGINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCSGINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������MDUROUTE
// ����: buf ���ݻ���
//       pval    ��������MDUROUTE
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
//gint    parseMDUROUTE(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������AlarmINFO
// ����: buf ���ݻ���
//       pval    ��������AlarmINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseAlarmINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������AlarmInputID
// ����: buf ���ݻ���
//       pval    ��������AlarmInputID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseAlarmInputID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������AlarmOutputID
// ����: buf ���ݻ���
//       pval    ��������AlarmOutputID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseAlarmOutputID(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PTZControl
// ����: buf ���ݻ���
//       pval    ��������PTZControl
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePTZControl(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PTZTransparentData
// ����: buf ���ݻ���
//       pval    ��������PTZTransparentData
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePTZTransparentData(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������VideoParam
// ����: buf ���ݻ���
//       pval    ��������VideoParam
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseVideoParam(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������AudioParam
// ����: buf ���ݻ���
//       pval    ��������AudioParam
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseAudioParam(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������PUMessage
// ����: buf ���ݻ���
//       pval    ��������PUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parsePUMessage(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������StreamID
// ����: buf ���ݻ���
//       pval    ��������StreamID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseStreamID(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������StreamType
// ����: buf ���ݻ���
//       pval    ��������StreamType
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseStreamType(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������ProductName
// ����: buf ���ݻ���
//       pval    ��������ProductName
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseProductName(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������IPAddress
// ����: buf ���ݻ���
//       pval    ��������IPAddress
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseIPAddress(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������IPMask
// ����: buf ���ݻ���
//       pval    ��������IPMask
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseIPMask(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������MACAddress
// ����: buf ���ݻ���
//       pval    ��������MACAddress
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMACAddress(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������GateWay
// ����: buf ���ݻ���
//       pval    ��������GateWay
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseGateWay(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������DNS
// ����: buf ���ݻ���
//       pval    ��������DNS
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseDNS(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������Name
// ����: buf ���ݻ���
//       pval    ��������Name
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseName(gchar* buf , void* pval);


//
// �������ݣ���buf�а���TLV�ṹ������RegisterPort
// ����: buf ���ݻ���
//       pval    ��������RegisterPort
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseRegisterPort(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������CSGMessage
// ����: buf ���ݻ���
//       pval    ��������CSGMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseCSGMessage(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������MDUMessage
// ����: buf ���ݻ���
//       pval    ��������MDUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMDUMessage(gchar* buf , void* pval);
//
// �������ݣ���buf�а���TLV�ṹ������MSUMessage
// ����: buf ���ݻ���
//       pval    ��������MSUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseMSUMessage(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������VerINFO
// ����: buf ���ݻ���
//       pval    ��������VerINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseVerINFO(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������AdditionalInfo
// ����: buf ���ݻ���
//       pval    ��������AdditionalInfo
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseAdditionalInfo(gchar* buf , void* pval);

//
// �������ݣ���buf�а���TLV�ṹ������OPRESULT
// ����: buf ���ݻ���
//       pval    ��������OPRESULT
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    parseOPRESULT(gchar* buf , void* pval);


/////////////////////////////////////////////////////////
// Build
/////////////////////////////////////////////////////////
//
// ����TLV���ݣ������ݴ���buf�У�32λ����
// ����: buf ���ݻ���
//       tag ��Ӧ��tag
//       val ��Ӧ������
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildTLV_int32(gchar* buf , guchar tag, guint val);
//
// ����TLV���ݣ������ݴ���buf�У�16λ����
// ����: buf ���ݻ���
//       tag ��Ӧ��tag
//       val ��Ӧ������
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildTLV_int16(gchar* buf , guchar tag, gushort val);
//
// ����TLV���ݣ������ݴ���buf�У�8λ����
// ����: buf ���ݻ���
//       tag ��Ӧ��tag
//       val ��Ӧ������
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildTLV_int8(gchar* buf , guchar tag, guchar val);
//
// ����TLV���ݣ������ݴ���buf�У��ַ���
// ����: buf ���ݻ���
//       tag ��Ӧ��tag
//       pStr    �ַ���
//       len     pStr��������С
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildTLV_str(gchar* buf , guchar tag, guchar* pStr,gint len);
//
// ����TLV���ݣ������ݴ���buf�У�����
// ����: buf ���ݻ���
//       tag ��Ӧ��tag
//       pval ��Ӧ������
//       len  ���ݳ���
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildTLV_void(gchar* buf , guchar tag, void* pval,gint len);

//
// ����TLV���ݣ������ݴ���buf�У�CUID
// ����: buf ���ݻ���
//       pval    CUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCUID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PUID
// ����: buf ���ݻ���
//       pval    ��������PUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPUID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�CMUID
// ����: buf ���ݻ���
//       pval    CMUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCMUID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�MDUID
// ����: buf ���ݻ���
//       pval    MDUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMDUID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�MSUID
// ����: buf ���ݻ���
//       pval    MSUID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMSUID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�CSGID
// ����: buf ���ݻ���
//       pval    CSGID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCSGID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�ChannelID
// ����: buf ���ݻ���
//       pval    ChannelID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildChannelID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PUSTRID
// ����: buf ���ݻ���
//       pval    PUSTRID
//       len     pval���峤��
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPUSTRID(gchar* buf , void* pval,gint len);

//
// ����TLV���ݣ������ݴ���buf�У�CUINFO
// ����: buf ���ݻ���
//       pval    CUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCUINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PUINFO
// ����: buf ���ݻ���
//       pval    PUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPUINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�MDUINFO
// ����: buf ���ݻ���
//       pval    MDUINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMDUINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�CSGINFO
// ����: buf ���ݻ���
//       pval    CSGINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCSGINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�MDUROUTE
// ����: buf ���ݻ���
//       pval    MDUROUTE
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMDUROUTE(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�AlarmINFO
// ����: buf ���ݻ���
//       pval    AlarmINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildAlarmINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�AlarmInputID
// ����: buf ���ݻ���
//       pval    AlarmInputID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildAlarmInputID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�AlarmOutputID
// ����: buf ���ݻ���
//       pval    AlarmOutputID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildAlarmOutputID(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PTZControl
// ����: buf ���ݻ���
//       pval    PTZControl
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPTZControl(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PTZTransparentData
// ����: buf ���ݻ���
//       pval    PTZTransparentData
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPTZTransparentData(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�VideoParam
// ����: buf ���ݻ���
//       pval   VideoParam
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildVideoParam(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�AudioParam
// ����: buf ���ݻ���
//       pval    AudioParam
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildAudioParam(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�PUMessage
// ����: buf ���ݻ���
//       pval    PUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildPUMessage(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�StreamID
// ����: buf ���ݻ���
//       pval    StreamID
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildStreamID(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�StreamType
// ����: buf ���ݻ���
//       pval    StreamType
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildStreamType(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�ProductName
// ����: buf ���ݻ���
//       pval    ProductName
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildProductName(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�IPAddress
// ����: buf ���ݻ���
//       pval    IPAddress
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildIPAddress(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�IPMask
// ����: buf ���ݻ���
//       pval    IPMask
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildIPMask(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�MACAddress
// ����: buf ���ݻ���
//       pval    MACAddress
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMACAddress(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�GateWay
// ����: buf ���ݻ���
//       pval    GateWay
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildGateWay(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�DNS
// ����: buf ���ݻ���
//       pval    DNS
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildDNS(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�Name
// ����: buf ���ݻ���
//       pval    Name
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildName(gchar* buf , void* pval);


//
// ����TLV���ݣ������ݴ���buf�У�RegisterPort
// ����: buf ���ݻ���
//       pval    RegisterPort
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildRegisterPort(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�CSGMessage
// ����: buf ���ݻ���
//       pval    CSGMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildCSGMessage(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�MDUMessage
// ����: buf ���ݻ���
//       pval    MDUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMDUMessage(gchar* buf , void* pval);
//
// ����TLV���ݣ������ݴ���buf�У�MSUMessage
// ����: buf ���ݻ���
//       pval    MSUMessage
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildMSUMessage(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�VerINFO
// ����: buf ���ݻ���
//       pval    VerINFO
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildVerINFO(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�AdditionalInfo
// ����: buf ���ݻ���
//       pval    AdditionalInfo
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildAdditionalInfo(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�OPRESULT
// ����: buf ���ݻ���
//       pval    OPRESULT
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildOPRESULT(gchar* buf , void* pval);

//
// ����TLV���ݣ������ݴ���buf�У�GPS
// ����: buf ���ݻ���
//       pval    GPSDATA����
// ����ֵ:
//        �ɹ�:  TLV������buf�е����ݳ���
//        ʧ��:  0
//
gint    buildGPS(gchar* buf , void* pval);

//////////////////////////////////////////////////////////////
// common
//////////////////////////////////////////////////////////////
//
// �������ݣ�������������ת��ΪTLV�ṹ������
//
//
gint    parseTLV(gchar* buf , TLVOBJ* pstRet);
//
// ��TLV�ṹ����ת��Ϊ����������
//
gint    buildTLV(TLVOBJ stTlv , gchar* buf , gint nBufLen);
///////////////////////////////////////////////////////////////
// Message Head
//////////////////////////////////////////////////////////////
//
// ��buf�н���������ͷ
//
void    parseMsgHead(gchar* buf,gint len,MSGHEAD* pHead);
//
// ���챨�ģ�������ͷ����д��buf
//
void    buildMsgHead(gchar* buf,gint len,MSGHEAD* pHead);
#pragma pack() // Note

G_END_DECLS


#endif
