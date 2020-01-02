#ifndef ___log_
#define ___log_

#include "../msg_head.h"


typedef struct log_upque		//日志上传请求IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int log_type;		//日志类型
	unsigned char bbu_path[200];	//BBU侧存储路径
	unsigned char bbu_file[16];	//BBU侧存储文件名
} LOG_UPQUE;

typedef struct log_upans		//日志上传应答IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int log_type;		//日志类型
	unsigned int res;		//返回结果
} LOG_UPANS;

typedef struct log_upres		//日志上传结果IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned short log_type;	//日志类型
	unsigned short res;		//上传结果
} LOG_UPRES;

#endif
