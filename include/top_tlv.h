#ifndef _TOP_TLV_H
#define _TOP_TLV_H

G_BEGIN_DECLS


#pragma pack (1) // Note: 必须和#pragma pack ()成对使用
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
#define  CMD_CSG_PLAYBACK_LOCATION          0x500f  // 暂时废弃
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
// 通信协议报文头
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
// 通信数据TLV定义
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
    void*  pAddition;                   // 附加数据，一般为null
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
    guint BitRate;        //码率
    guint BitType;        //0 变码率，1 定码率
    guint FrameRate;      //帧率分子
    guint FrameRateDiv;   //帧率分母
    guint FrameWidth;     //视频宽度
    guint FrameHeight;    //视频高度
    guint Standard;       //制式：0 为NTSC，1 为PAL
    guint HttpPort;       //HTTP端口号
    guint RtpPort;        //RTSP端口号
    guint IpInterval;     //I帧间隔(1~999)
    guint Palete;         //像素格式
    guint QuantMethod;    //压缩方式
    guint nBlackEdge;     //处理黑边
    guint nInterlace;     //交错处理
    guint Streamtype;     //码流类型
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
NOTE: 关于经纬度，联通规范有误，需要确认
暂定如下
第一,二字节为度，第三和第四字节为度的小数位、
经纬度精确到分的小数点后4位。其中最高位是符号位，
对于经度："正"表示"东经"，"负"表示"西经"；
对于维度："正"表示"北纬"，"负"表示"南纬"
*/
typedef struct _gps_upload_data_
{
    gchar     longitude[4];         // 经度
    gchar     latitude[4];          // 纬度
    gshort    speed;            // 速度，单位为公里/小时
    gshort    angle;            // 方位角，000-359度，正北为0度，顺时针方向计数
    gchar     altitude;         // 高度
    gint      timestamp;        // 采集时间
    gint      distance;         // 里程数 单位米
}GPSDATA;
//
// set TLV tag length,default is DEFAULT_TLV_TAG_LEN
//
//
void    setTagLength(gshort nLen);
//
// 解析数据，从buf中按照TLV结构解析出32位整数
// 参数: buf 数据缓存
//       pnRet    解析出的32为整数
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseTLV_int32(gchar* buf , guint* pnRet);
//
// 解析数据，从buf中按照TLV结构解析出16位整数
// 参数: buf 数据缓存
//       pnRet    解析出的16为整数
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseTLV_int16(gchar* buf , gushort* pnRet);
//
// 解析数据，从buf中按照TLV结构解析出8位整数
// 参数: buf 数据缓存
//       pnRet    解析出的8为整数
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseTLV_int8(gchar* buf , guchar* pnRet);
//
// 解析数据，从buf中按照TLV结构解析出字符串
// 参数: buf 数据缓存
//       pStr    解析出的字符串
//       len     pStr缓冲区大小
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseTLV_str(gchar* buf , guchar* pStr,gint len);
//
// 解析数据，从buf中按照TLV结构解析出数据
// 参数: buf 数据缓存
//       ppval    解析出的数据在buf中的指针
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseTLV_void(gchar* buf , void** ppval);

//
// 解析数据，从buf中按照TLV结构解析出CUID
// 参数: buf 数据缓存
//       pval    解析出的CUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCUID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PUID
// 参数: buf 数据缓存
//       pval    解析出的PUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePUID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出CMUID
// 参数: buf 数据缓存
//       pval    解析出的CMUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCMUID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出MDUID
// 参数: buf 数据缓存
//       pval    解析出的MDUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMDUID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出MSUID
// 参数: buf 数据缓存
//       pval    解析出的MSUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMSUID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出CSGID
// 参数: buf 数据缓存
//       pval    解析出的CSGID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCSGID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出ChannelID
// 参数: buf 数据缓存
//       pval    解析出的ChannelID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseChannelID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PUSTRID
// 参数: buf 数据缓存
//       pval    解析出的PUSTRID
//       len     pval缓冲长度
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePUSTRID(gchar* buf , void* pval,gint len);

//
// 解析数据，从buf中按照TLV结构解析出CUINFO
// 参数: buf 数据缓存
//       pval    解析出的CUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCUINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PUINFO
// 参数: buf 数据缓存
//       pval    解析出的PUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePUINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出MDUINFO
// 参数: buf 数据缓存
//       pval    解析出的MDUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMDUINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出CSGINFO
// 参数: buf 数据缓存
//       pval    解析出的CSGINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCSGINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出MDUROUTE
// 参数: buf 数据缓存
//       pval    解析出的MDUROUTE
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
//gint    parseMDUROUTE(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出AlarmINFO
// 参数: buf 数据缓存
//       pval    解析出的AlarmINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseAlarmINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出AlarmInputID
// 参数: buf 数据缓存
//       pval    解析出的AlarmInputID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseAlarmInputID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出AlarmOutputID
// 参数: buf 数据缓存
//       pval    解析出的AlarmOutputID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseAlarmOutputID(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PTZControl
// 参数: buf 数据缓存
//       pval    解析出的PTZControl
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePTZControl(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PTZTransparentData
// 参数: buf 数据缓存
//       pval    解析出的PTZTransparentData
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePTZTransparentData(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出VideoParam
// 参数: buf 数据缓存
//       pval    解析出的VideoParam
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseVideoParam(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出AudioParam
// 参数: buf 数据缓存
//       pval    解析出的AudioParam
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseAudioParam(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出PUMessage
// 参数: buf 数据缓存
//       pval    解析出的PUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parsePUMessage(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出StreamID
// 参数: buf 数据缓存
//       pval    解析出的StreamID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseStreamID(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出StreamType
// 参数: buf 数据缓存
//       pval    解析出的StreamType
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseStreamType(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出ProductName
// 参数: buf 数据缓存
//       pval    解析出的ProductName
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseProductName(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出IPAddress
// 参数: buf 数据缓存
//       pval    解析出的IPAddress
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseIPAddress(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出IPMask
// 参数: buf 数据缓存
//       pval    解析出的IPMask
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseIPMask(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出MACAddress
// 参数: buf 数据缓存
//       pval    解析出的MACAddress
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMACAddress(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出GateWay
// 参数: buf 数据缓存
//       pval    解析出的GateWay
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseGateWay(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出DNS
// 参数: buf 数据缓存
//       pval    解析出的DNS
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseDNS(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出Name
// 参数: buf 数据缓存
//       pval    解析出的Name
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseName(gchar* buf , void* pval);


//
// 解析数据，从buf中按照TLV结构解析出RegisterPort
// 参数: buf 数据缓存
//       pval    解析出的RegisterPort
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseRegisterPort(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出CSGMessage
// 参数: buf 数据缓存
//       pval    解析出的CSGMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseCSGMessage(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出MDUMessage
// 参数: buf 数据缓存
//       pval    解析出的MDUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMDUMessage(gchar* buf , void* pval);
//
// 解析数据，从buf中按照TLV结构解析出MSUMessage
// 参数: buf 数据缓存
//       pval    解析出的MSUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseMSUMessage(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出VerINFO
// 参数: buf 数据缓存
//       pval    解析出的VerINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseVerINFO(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出AdditionalInfo
// 参数: buf 数据缓存
//       pval    解析出的AdditionalInfo
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseAdditionalInfo(gchar* buf , void* pval);

//
// 解析数据，从buf中按照TLV结构解析出OPRESULT
// 参数: buf 数据缓存
//       pval    解析出的OPRESULT
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    parseOPRESULT(gchar* buf , void* pval);


/////////////////////////////////////////////////////////
// Build
/////////////////////////////////////////////////////////
//
// 构造TLV数据，将数据存入buf中，32位整数
// 参数: buf 数据缓存
//       tag 对应的tag
//       val 对应的数据
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildTLV_int32(gchar* buf , guchar tag, guint val);
//
// 构造TLV数据，将数据存入buf中，16位整数
// 参数: buf 数据缓存
//       tag 对应的tag
//       val 对应的数据
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildTLV_int16(gchar* buf , guchar tag, gushort val);
//
// 构造TLV数据，将数据存入buf中，8位整数
// 参数: buf 数据缓存
//       tag 对应的tag
//       val 对应的数据
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildTLV_int8(gchar* buf , guchar tag, guchar val);
//
// 构造TLV数据，将数据存入buf中，字符串
// 参数: buf 数据缓存
//       tag 对应的tag
//       pStr    字符串
//       len     pStr缓冲区大小
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildTLV_str(gchar* buf , guchar tag, guchar* pStr,gint len);
//
// 构造TLV数据，将数据存入buf中，数据
// 参数: buf 数据缓存
//       tag 对应的tag
//       pval 对应的数据
//       len  数据长度
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildTLV_void(gchar* buf , guchar tag, void* pval,gint len);

//
// 构造TLV数据，将数据存入buf中，CUID
// 参数: buf 数据缓存
//       pval    CUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCUID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PUID
// 参数: buf 数据缓存
//       pval    解析出的PUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPUID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，CMUID
// 参数: buf 数据缓存
//       pval    CMUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCMUID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，MDUID
// 参数: buf 数据缓存
//       pval    MDUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMDUID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，MSUID
// 参数: buf 数据缓存
//       pval    MSUID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMSUID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，CSGID
// 参数: buf 数据缓存
//       pval    CSGID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCSGID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，ChannelID
// 参数: buf 数据缓存
//       pval    ChannelID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildChannelID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PUSTRID
// 参数: buf 数据缓存
//       pval    PUSTRID
//       len     pval缓冲长度
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPUSTRID(gchar* buf , void* pval,gint len);

//
// 构造TLV数据，将数据存入buf中，CUINFO
// 参数: buf 数据缓存
//       pval    CUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCUINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PUINFO
// 参数: buf 数据缓存
//       pval    PUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPUINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，MDUINFO
// 参数: buf 数据缓存
//       pval    MDUINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMDUINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，CSGINFO
// 参数: buf 数据缓存
//       pval    CSGINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCSGINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，MDUROUTE
// 参数: buf 数据缓存
//       pval    MDUROUTE
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMDUROUTE(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，AlarmINFO
// 参数: buf 数据缓存
//       pval    AlarmINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildAlarmINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，AlarmInputID
// 参数: buf 数据缓存
//       pval    AlarmInputID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildAlarmInputID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，AlarmOutputID
// 参数: buf 数据缓存
//       pval    AlarmOutputID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildAlarmOutputID(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PTZControl
// 参数: buf 数据缓存
//       pval    PTZControl
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPTZControl(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PTZTransparentData
// 参数: buf 数据缓存
//       pval    PTZTransparentData
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPTZTransparentData(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，VideoParam
// 参数: buf 数据缓存
//       pval   VideoParam
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildVideoParam(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，AudioParam
// 参数: buf 数据缓存
//       pval    AudioParam
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildAudioParam(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，PUMessage
// 参数: buf 数据缓存
//       pval    PUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildPUMessage(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，StreamID
// 参数: buf 数据缓存
//       pval    StreamID
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildStreamID(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，StreamType
// 参数: buf 数据缓存
//       pval    StreamType
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildStreamType(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，ProductName
// 参数: buf 数据缓存
//       pval    ProductName
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildProductName(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，IPAddress
// 参数: buf 数据缓存
//       pval    IPAddress
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildIPAddress(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，IPMask
// 参数: buf 数据缓存
//       pval    IPMask
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildIPMask(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，MACAddress
// 参数: buf 数据缓存
//       pval    MACAddress
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMACAddress(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，GateWay
// 参数: buf 数据缓存
//       pval    GateWay
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildGateWay(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，DNS
// 参数: buf 数据缓存
//       pval    DNS
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildDNS(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，Name
// 参数: buf 数据缓存
//       pval    Name
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildName(gchar* buf , void* pval);


//
// 构造TLV数据，将数据存入buf中，RegisterPort
// 参数: buf 数据缓存
//       pval    RegisterPort
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildRegisterPort(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，CSGMessage
// 参数: buf 数据缓存
//       pval    CSGMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildCSGMessage(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，MDUMessage
// 参数: buf 数据缓存
//       pval    MDUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMDUMessage(gchar* buf , void* pval);
//
// 构造TLV数据，将数据存入buf中，MSUMessage
// 参数: buf 数据缓存
//       pval    MSUMessage
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildMSUMessage(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，VerINFO
// 参数: buf 数据缓存
//       pval    VerINFO
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildVerINFO(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，AdditionalInfo
// 参数: buf 数据缓存
//       pval    AdditionalInfo
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildAdditionalInfo(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，OPRESULT
// 参数: buf 数据缓存
//       pval    OPRESULT
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildOPRESULT(gchar* buf , void* pval);

//
// 构造TLV数据，将数据存入buf中，GPS
// 参数: buf 数据缓存
//       pval    GPSDATA数据
// 返回值:
//        成功:  TLV数据在buf中的数据长度
//        失败:  0
//
gint    buildGPS(gchar* buf , void* pval);

//////////////////////////////////////////////////////////////
// common
//////////////////////////////////////////////////////////////
//
// 解析数据，将二进制数据转换为TLV结构的数据
//
//
gint    parseTLV(gchar* buf , TLVOBJ* pstRet);
//
// 将TLV结构数据转换为二进制数据
//
gint    buildTLV(TLVOBJ stTlv , gchar* buf , gint nBufLen);
///////////////////////////////////////////////////////////////
// Message Head
//////////////////////////////////////////////////////////////
//
// 从buf中解析出报文头
//
void    parseMsgHead(gchar* buf,gint len,MSGHEAD* pHead);
//
// 构造报文，将报文头数据写入buf
//
void    buildMsgHead(gchar* buf,gint len,MSGHEAD* pHead);
#pragma pack() // Note

G_END_DECLS


#endif
