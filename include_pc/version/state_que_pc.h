#ifndef ___state_que_pc_
#define ___state_que_pc_

#include "../msg_head_pc.h"


typedef struct sq_rfchsta_pc		//射频通道状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_num;		//射频通道号
} SQ_RFCHSTA_PC;

typedef struct sq_cirsta_pc		//载波状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_CIRSTA_PC;

typedef struct sq_oscsta_pc		//本振状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_OSCSTA_PC;

typedef struct sq_rtcsta_pc		//时钟状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_RTCSTA_PC;

typedef struct sq_runsta_pc		//RRU运行状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_RUNSTA_PC;

typedef struct sq_irmodesta_pc		//Ir口工作模式查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_IRMODESTA_PC;

typedef struct sq_initchk_pc		//初始化校准结果IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} SQ_INITCHK_PC;

typedef struct sq_raysta_pc		//光口信息查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤端口号
} SQ_RAYSTA_PC;

typedef struct sq_rfchans_pc		//射频通道状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_num;		//射频通道号
	unsigned int ul_sta;		//上行通道状态
	unsigned int dl_sta; 		//下行通道状态
} SQ_RFCHANS_PC;

typedef struct sq_cirans_pc		//载波状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ant_num;		//天线组号
	unsigned char cir_num;		//载波号
	unsigned int sta; 		//状态
} SQ_CIRANS_PC;

typedef struct sq_oscans_pc		//本振状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int osc_fre; 		//本振频率
	unsigned int sta; 		//状态
} SQ_OSCANS_PC;

typedef struct sq_rtcans_pc		//时钟状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int sta; 		//状态
} SQ_RTCANS_PC;

typedef struct sq_runans_pc		//RRU运行状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int sta; 		//状态
} SQ_RUNANS_PC;

typedef struct sq_irmodeans_pc		//Ir口工作模式查询响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int res; 		//返回结果
} SQ_IRMODEANS_PC;

typedef struct sq_initchkans_pc		//初始化校准结果响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tr_num;		//发射通道号
	unsigned int trchk_sta;		//发射通道校准状态
	unsigned int rec_num;		//接收通道号
	unsigned int recchk_sta;	//接收通道校准状态
} SQ_INITCHKANS_PC;

typedef struct sq_rayans_pc		//光口信息查询响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤端口号
	unsigned short rec_power;	//收功率
	unsigned short tr_power;	//发功率
	unsigned char is_flag;		//在位信息
	unsigned char ray_manu[16];	//光模块厂商
	unsigned short ray_speed;	//光模块传输bit速率
	unsigned char tem;		//温度
	unsigned short volt;		//电压
	unsigned short curr;		//电流
	unsigned char _9_max_long;	//9um单模光纤支持的最大长度
	unsigned char _9o_max_long;	//9um单模光纤支持的最大长度
	unsigned char _50_max_long;	//50um单模光纤支持的最大长度
	unsigned char _62o5_max_long;	//62.5um单模光纤支持的最大长度
} SQ_RAYANS_PC;

#endif
