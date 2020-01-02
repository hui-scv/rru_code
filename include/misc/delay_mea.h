#ifndef ___delay_mea_
#define ___delay_mea_

#include "../msg_head.h"


typedef struct dm_cycdelay		//周期性时延测量参数配置请求IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤号
} DM_CYCDELAY;

typedef struct dm_raydelayans		//光纤时延测量响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤号
	unsigned int toffset;		//RRU板级时延
	unsigned int dl_delay;		//下行转发时延
	unsigned int ul_delay;		//上行转发时延
	unsigned int t2a;		//
	unsigned int ta3;		//
	unsigned int n;			//
} DM_RAYDELAYANS;

typedef struct dm_delaycfg		//时延配置命令IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤号
	unsigned int t12;		//
	unsigned int t34;		//
	unsigned int dl_offset;		//帧定时提前量
	unsigned int dl_cal;		//RRU下行补偿值
	unsigned int ul_cal;		//RRU上行补偿值
} DM_DELAYCFG;

typedef struct dm_delaycfgans		//时延测量结果响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤号
	unsigned char res;		//返回结果
} DM_DELAYCFGANS;

#endif
