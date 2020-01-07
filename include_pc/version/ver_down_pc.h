#ifndef ___ver_pc_down_
#define ___ver_pc_down_

#include "../msg_head_pc.h"


typedef struct vd_softchk		//软件版本核对结果IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_type;	//软件版本类型
	unsigned int res;		//返回结果
	unsigned char file_path[200];	//文件路径
	unsigned char file_name[16];	//文件名
	unsigned int file_length;	//文件长度
	unsigned char file_date[20];	//文件时间信息
	unsigned char file_ver[40];	//文件版本
} VD_SOFTCHK;

typedef struct vd_verdownans		//版本下载响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_type;	//软件版本类型
	unsigned int res;		//响应结果
} VD_VERDOWNANS;

typedef struct vd_verdownres		//版本下载传输完成指示IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_type;	//软件版本类型
	unsigned int res;		//版本下载传输完成指示
} VD_VERDOWNRES;

#endif
