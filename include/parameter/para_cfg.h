#ifndef ___para_cfg_
#define ___para_cfg_

#include "../msg_head.h"

#pragma pack(1)

typedef struct pc_systime		//系统时间IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char sec;		//秒
	unsigned char min;		//分
	unsigned char hour;		//时
	unsigned char day;		//日
	unsigned char month;		//月
	unsigned short year;		//年
} PC_SYSTIME;

typedef struct pc_iqdatach		//IQ数据通道配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char cir_num;		//载波号
	unsigned char ant_num;		//天线号
	unsigned char axv_num;		//AXC号
	unsigned char ray_num;		//光纤号
} PC_IQDATACH;

typedef struct pc_ratecyc		//CPU占用率统计周期配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int rate_cyc;		//CPU占用率统计周期数值
} PC_RATECYC;

typedef struct pc_swrthr		//驻波比门限配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int no1_thr;		//一级门限
	unsigned int no2_thr;		//二级门限
} PC_SWRTHR;

typedef struct pc_irmode		//Ir口工作模式配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int ir_mode;		//Ir口工作模式
} PC_IRMODE;

typedef struct pc_temthr		//过温门限配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
	unsigned int tem_max;		//温度上门限
	unsigned int tem_low;		//温度下门限
} PC_TEMTHR;

typedef struct pc_rfchsta		//射频通道状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_num;		//射频通道号
} PC_RFCHSTA;

typedef struct pc_antcfg		//天线配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rru_antmode;	//RRU天线模式
	unsigned char ant_num;		//天线组号
	unsigned short ul_antsta;	//上行天线状态
	unsigned short dl_antsta;	//下行天线状态
} PC_ANTCFG;

typedef struct pc_systimeans		//系统时间响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int res;		//返回结果
} PC_SYSTIMEANS;

typedef struct pc_iqdatachans		//IQ数据通道配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ray_num;		//光纤号
	unsigned char cir_num;		//载波号
	unsigned char ant_num;		//天线号
	unsigned char res;		//返回结果
	
} PC_IQDATACHANS;

typedef struct pc_ratecycans		//CPU占用率统计周期配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int res;		//返回结果
} PC_RATECYCANS;

typedef struct pc_swrthrans		//驻波比门限配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int res;		//返回结果
} PC_SWRTHRANS;

typedef struct pc_irmodeans		//Ir口工作模式配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char mray_num;		//主光纤号
	unsigned char sray_num;		//辅光纤号
	unsigned int res;		//返回结果
} PC_IRMODEANS;

typedef struct pc_temthrans		//过温门限配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
	unsigned int res;		//返回结果
} PC_TEMTHRANS;

typedef struct pc_rfchstaans		//射频通道状态配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ch_num;		//射频通道号
	unsigned int res;		//返回结果
} PC_RFCHSTAANS;

typedef struct pc_antcfgans		//天线配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char ant_num;		//天线组号
	unsigned int res;		//返回结果
} PC_ANTCFGANS;

#pragma pack()

#endif
