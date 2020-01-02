#ifndef ___para_que_pc_
#define ___para_que_pc_

#include "../msg_head_pc.h"


typedef struct pq_systime_pc		//系统时间查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_SYSTIME_PC;

typedef struct pq_cpurate_pc		//CPU占用率IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_CPURATE_PC;

typedef struct pq_ratecyc_pc		//CPU占用率统计周期查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_RATECYC_PC;

typedef struct pq_rrutem_pc		//RRU温度IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
} PQ_RRUTEM_PC;

typedef struct pq_swrsta_pc		//驻波比状态IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
} PQ_SWRSTA_PC;

typedef struct pq_swrthr_pc		//驻波比门限查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_SWRTHR_PC;

typedef struct pq_temthr_pc		//过温门限查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_TEMTHR_PC;

typedef struct pq_outpower_pc		//输出功率IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
} PQ_OUTPOWER_PC;

typedef struct pq_stamach_pc		//状态机查询IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
} PQ_STAMACH_PC;

typedef struct pq_systimeans_pc		//系统时间IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char sec;		//秒
	unsigned char min;		//分
	unsigned char hour;		//时
	unsigned char day;		//日
	unsigned char month;		//月
	unsigned short year;		//年
} PQ_SYSTIMEANS_PC;

typedef struct pq_cpurateans_pc		//CPU占用率响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int cpu_rate;		//CPU占用率
} PQ_CPURATEANS_PC;

typedef struct pq_ratecycans_pc		//CPU占用率统计周期响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int rate_cyc;		//CPU占用率统计周期数值
} PQ_RATECYCANS_PC;

typedef struct pq_rrutemans_pc		//RRU温度响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_type;		//RRU温度类型
	unsigned char rfch_num;		//射频通道号
	unsigned int tem_val;		//温度数值
} PQ_RRUTEMANS_PC;

typedef struct pq_swrstaans_pc		//驻波比状态响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
	unsigned int swr_val;		//数值
} PQ_SWRSTAANS_PC;

typedef struct pq_swrthrans_pc		//驻波比门限响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int no1_thr;		//一级门限
	unsigned int no2_thr;		//二级门限
} PQ_SWRTHRANS_PC;

typedef struct pq_temthrans_pc		//过温门限响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int tem_max;		//温度上门限
	unsigned int tem_low;		//温度下门限
} PQ_TEMTHRANS_PC;

typedef struct pq_outpowerans_pc	//输出功率响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rfch_num;		//射频通道号
	unsigned short value;		//数值
} PQ_OUTPOWERANS_PC;

typedef struct pq_stamachans_pc		//状态机查询响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char state;		//状态机的状态
} PQ_STAMACHANS_PC;

#endif
