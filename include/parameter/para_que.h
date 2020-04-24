#ifndef ___para_que_
#define ___para_que_

#include "../msg_head.h"

#pragma pack(1)

typedef struct pq_systime		//系统时间查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_SYSTIME;

typedef struct pq_cpurate		//CPU占用率IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_CPURATE;

typedef struct pq_ratecyc		//CPU占用率统计周期查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_RATECYC;

typedef struct pq_rrutem		//RRU温度IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
} PQ_RRUTEM;

typedef struct pq_swrsta		//驻波比状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
} PQ_SWRSTA;

typedef struct pq_swrthr		//驻波比门限查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_SWRTHR;

typedef struct pq_temthr		//过温门限查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_TEMTHR;

typedef struct pq_outpower		//输出功率IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
} PQ_OUTPOWER;

typedef struct pq_stamach		//状态机查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_STAMACH;

typedef struct pq_systimeans		//系统时间IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char sec;		//秒
	unsigned char min;		//分
	unsigned char hour;		//时
	unsigned char day;		//日
	unsigned char month;		//月
	unsigned short year;		//年
} PQ_SYSTIMEANS;

typedef struct pq_cpurateans		//CPU占用率响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int cpu_rate;		//CPU占用率
} PQ_CPURATEANS;

typedef struct pq_ratecycans		//CPU占用率统计周期响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int rate_cyc;		//CPU占用率统计周期数值
} PQ_RATECYCANS;

typedef struct pq_rrutemans		//RRU温度响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
	unsigned char rfch_num;		//射频通道号
	unsigned int tem_val;		//温度数值
} PQ_RRUTEMANS;

typedef struct pq_swrstaans		//驻波比状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
	unsigned int swr_val;		//数值
} PQ_SWRSTAANS;

typedef struct pq_swrthrans		//驻波比门限响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int no1_thr;		//一级门限
	unsigned int no2_thr;		//二级门限
} PQ_SWRTHRANS;

typedef struct pq_temthrans		//过温门限响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_max;		//温度上门限
	unsigned int tem_low;		//温度下门限
} PQ_TEMTHRANS;

typedef struct pq_outpowerans		//输出功率响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
	unsigned short value;		//数值
} PQ_OUTPOWERANS;

typedef struct pq_stamachans		//状态机查询响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char state;		//状态机的状态
} PQ_STAMACHANS;

#pragma pack()

#endif
