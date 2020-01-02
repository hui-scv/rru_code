#ifndef ___test_pc_h_
#define ___test_pc_h_


#include "../msg_head_pc.h"


typedef struct ts_rftsen_pc		//测试模式设置
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char enable;		//测试使能
} TS_RFTSEN_PC;

typedef struct ts_senchset_pc		//发射通道开启设置
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//发射通道
	unsigned char ch_enable;	//发射使能
} TS_SENCHSET_PC;

typedef struct ts_recchset_pc		//接收通道开启设置
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//接收通道
	unsigned char ch_enable;	//接收使能
} TS_RECCHSET_PC;

typedef struct ts_sencirset_pc		//发射载波数据设置（PC下发RRU）
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//发射通道
	unsigned int total_size;	//文件总大小
	unsigned int packet_num;	//当前包序号
	unsigned short data_size;	//当前包数据长度
	unsigned char data[200];	//当前包数据
	unsigned int checksum;		//校验和
} TS_SENCIRSET_PC;

typedef struct ts_sencirans_pc		//发射载波数据响应（RRU响应PC）
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//发射通道
	unsigned int total_size;	//文件总大小
	unsigned int packet_num;	//当前包序号
	unsigned char res;		//消息响应
} TS_SENCIRANS_PC;

typedef struct ts_sendimset_pc		//发射衰减器设置
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//发射通道
	unsigned char dim_val;		//发射衰减值
} TS_SENDIMSET_PC;

typedef struct ts_cirdataque_pc		//请求获取载波数据（PC下发RRU）
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//接收通道
} TS_CIRDATAQUE_PC;

typedef struct ts_cirdataans_pc		//获取接收载波数据(RRU上发PC)
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//接收通道
	unsigned int total_size;	//文件总大小
	unsigned int packet_num;	//当前包序号
	unsigned int data_size;		//当前包数据长度
	unsigned char data[200];	//当前包数据
	unsigned int checksum;		//校验和
} TS_CIRDATAANS_PC;

typedef struct ts_reccirdata_pc		//接收载波数据响应(PC响应RRU)
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//接收通道
	unsigned int total_size;	//文件总大小
	unsigned int packet_num;	//当前包序号
	unsigned char res;		//消息响应
} TS_RECCIRDATA_PC;

typedef struct ts_recdimset_pc		//接收衰减器设置
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_sel;		//接收通道
	unsigned char dim_val;		//接收衰减值
} TS_RECDIMSET_PC;

#endif
