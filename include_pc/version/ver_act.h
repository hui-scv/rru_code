#ifndef ___ver_down_
#define ___ver_down_

#include "../msg_head.h"


typedef struct va_rrusoftinfo		//RRU软件版本信息IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_ver[40];	//RRU软件版本号
	unsigned char bios_ver[40];	//固件软件版本号
} VA_RRUSOFTINFO;

typedef struct va_rruveractans		//RRU版本激活响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_type;	//软件版本类型
	unsigned int res;		//软件包激活返回结果
} VA_RRUVERACTANS;

#endif
