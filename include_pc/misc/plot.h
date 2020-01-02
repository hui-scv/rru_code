#ifndef ___reset_
#define ___reset_

#include "../msg_head.h"


typedef struct plt_cfg			//小区配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char plot_cfgflag;	//小区配置标识
	unsigned int plot_locflag;	//本地小区标识
	unsigned short plot_power;	//小区功率
	unsigned char ant_num;		//天线组号
	unsigned char fre_point;	//频点数
} PLT_CFG;

typedef struct plt_frepit_fdd		//频点配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char frepit_flag;	//频点配置标识
	unsigned int plot_locflag;	//本地小区标识
	unsigned char cir_num;		//载波号
	unsigned int dl_frepit;		//下行中心频点
	unsigned int ul_frepit;		//上行中心频点
	unsigned int frepit_type;	//频点的主辅特性
	unsigned int dl_cirbw;		//下行载波带宽
	unsigned int ul_cirbw;		//上行载波带宽
	unsigned char loop_sel;		//循环前缀长度选择
} PLT_FREPIT_FDD;

typedef struct plt_cfgans		//小区配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int plot_locflag;	//本地小区标识
	unsigned int res;		//返回结果
} PLT_CFGANS;

typedef struct plt_frepitans		//频点配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int plot_locflag;	//本地小区标识
	unsigned char cir_num;		//载波号
	unsigned int res;		//返回结果
} PLT_FREPITANS;

#endif
