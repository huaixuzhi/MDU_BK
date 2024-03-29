#ifndef _TOP_LOG_H_
#define _TOP_LOG_H_

G_BEGIN_DECLS
#pragma pack (1) // Note: 必须和#pragma pack ()成对使用

/*******************************************************************************
    函数名 :  top_log_init
    参  数 :  domain        日志域，不同模块使用不同的域
              path          日志文件存放路径，以'/'结尾
              filename      文件名
    返回值 :  void
    调  用 :  none
    被调用 :  none
    功  能 :  日志初始化

    修改历史
     1.修改描述: 创建函数
       日期  : 2011-8-4
       作者  : dengjh
*******************************************************************************/
void top_log_init(gchar* domain , gchar* path,gchar* filename);

/*******************************************************************************
    函数名 :  top_log
    参  数 :  log_domain      日志域，不同模块使用不同的域
              format          日志数据格式化信息
              ...             格式化参数
    返回值 :  void
    调  用 :  none
    被调用 :  none
    功  能 :  日志初始化

    修改历史
     1.修改描述: 创建函数
       日期  : 2011-8-4
       作者  : dengjh
*******************************************************************************/
void top_log (const gchar   *log_domain, const gchar   *format, ...);

#pragma pack() // Note
G_END_DECLS

#endif
