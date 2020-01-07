#ifndef __ch_lingk_h
#define __ch_lingk_h

#include "../msg_head.h"


typedef struct cl_proid			//RRU产品标识IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rru_manu[16];	//RRU制造商名称
	unsigned char rru_pub[16];	//RRU发行商名称
	unsigned char pro_num[16];	//产品序列号
	unsigned char birth_date[16];	//生产日期
	unsigned char used_date[16];	//最近服务日期
	unsigned char addi_data[16];	//附加信息
} CL_PROID;

typedef struct cl_linktype		//通道建立原因IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char link_type;	//通道建立原因
	unsigned int reboot_code;	//导致RRU侧发起软件重启动的告警码
} CL_LINKTYPE;

typedef struct cl_rrucapa		//RRU能力IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int td_num;		//支持的TD-SCDMA载波个数
	unsigned int lte_num;		//支持的LTE载波个数
	unsigned int _5g_num;		//支持的5GNR载波个数
	unsigned char ant_num;		//支持的天线个数
	unsigned short tra_power;	//天线最大发射功率
	unsigned char master_type;	//通信主从属性
	unsigned int rru_dl_max;	//RRU支持的下行时延补偿最大值(DL CalRRU_MAX)
	unsigned int rru_ul_max;	//RRU支持的上行时延补偿最大值(UL CalRRU_MAX)
	unsigned short mode_sel;	//支持的模式
	unsigned char ant_type;		//RRU支持独立做天线校准
	unsigned char mbbu_dl_sel;	//主发校正通道
	unsigned char sbbu_dl_sel;	//备发校正通道
	unsigned char mrru_ul_sel;	//主收校正通道
	unsigned char srru_ul_sel;	//备收校正通道
} CL_RRUCAPA;

typedef struct cl_rrulv			//RRU级数IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rru_lv;		//RRU所处级数
} CL_RRULV;

typedef struct cl_rruinfo		//RRU硬件类型及版本信息IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char type[32];		//RRU硬件类型
	unsigned char version[16];	//RRU硬件版本号
} CL_RRUINFO;

typedef struct cl_rrusoftinfo		//RRU软件版本信息IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_ver[40];	//RRU软件版本号
	unsigned char bios_ver[40];	//固件软件版本号
} CL_RRUSOFTINFO;

typedef struct cl_rrufre		//RRU频段能力IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned short fre_start;	//支持的频段区间起始频点
	unsigned short fre_end;		//支持的频段区间截止频点
	unsigned short td_wid;		//支持的TD-SCDMA频带宽度
	unsigned char td_num;		//支持的TD-SCDMA载波个数
	unsigned char lte_num;		//支持的LTE载波个数
	unsigned char _5g_num;		//支持的5GNR载波个数
	unsigned char fre_num;		//频段区间编号
} CL_RRUFRE;

typedef struct cl_rrurf			//RRU射频通道能力IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rf_chal;		//射频通道号
	unsigned char ant_num;		//天线号
	unsigned char td_start;		//TD-SCDMA载波起始编号
	unsigned char td_end;		//TD-SCDMA载波终止编号
	unsigned char lte_start;	//LTE载波起始编号
	unsigned char lte_end;		//LTE载波终止编号
	unsigned char _5g_start;	//5GNR载波起始编号
	unsigned char _5g_end;		//5GNR载波终止编号
	unsigned short chal_max_rf;	//通道最大发射功率
	unsigned short rf_section;	//支持的频段区间位图
	unsigned char chal_mode;	//通道收发属性
} CL_RRURF;

typedef struct cl_rrucir		//载波能力组合IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char rf_chal;		//对应射频通道编号
	unsigned char fre_num;		//对应频段能力编号
	unsigned char _1o4_num;		//1.4M载波个数
	unsigned char _3_num;		//3 M载波个数
	unsigned char _5_num;		//5 M载波个数
	unsigned char _10_num;		//10 M载波个数
	unsigned char _15_num;		//15 M载波个数
	unsigned char _20_num;		//20 M载波个数
	unsigned char _25_num;		//25 M载波个数
	unsigned char _30_num;		//30 M载波个数
	unsigned char _40_num;		//40 M载波个数
	unsigned char _50_num;		//50 M载波个数
	unsigned char _60_num;		//60 M载波个数
	unsigned char _70_num;		//70 M载波个数
	unsigned char _80_num;		//80 M载波个数
	unsigned char _90_num;		//90 M载波个数
	unsigned char _100_num;		//100 M载波个数
	unsigned char _200_num;		//200 M载波个数
	unsigned char _400_num;		//400 M载波个数
} CL_RRUCIR;

typedef struct cl_systime		//系统时间IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char sec;		//秒
	unsigned char min;		//分
	unsigned char hour;		//时
	unsigned char day;		//日
	unsigned char month;		//月
	unsigned short year;		//年
} CL_SYSTIME;

typedef struct cl_linkaddr		//接入应答地址IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char bbu_ip[4];	//BBU侧FTP服务器IP地址
} CL_LINKADDR;

typedef struct cl_rrumode		//RRU操作模式设置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int mode;		//模式状态值
} CL_RRUMODE;

typedef struct cl_softchk		//软件版本核对结果IE
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
} CL_SOFTCHK;

typedef struct cl_irmodecfg		//Ir口工作模式配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int ir_mode;		//Ir口工作模式
} CL_IRMODECFG;

typedef struct cl_ftpinfo		//FTP信息配置IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char usr[40];		//FTP用户名
	unsigned char pwd[40];		//FTP密码
} CL_FTPINFO;

typedef struct cl_chlinkans		//通道建立配置应答IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int res;		//返回结果
} CL_CHLINKANS;

typedef struct cl_irmodecfgans		//Ir口工作模式配置响应IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char mray_num;		//主光纤号
	unsigned char sray_num;		//辅光纤号
	unsigned int res;		//返回结果
} CL_IRMODECFGANS;

typedef struct cl_verupdata		//版本更新结果指示IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned char soft_type;	//软件版本类型
	unsigned int res;		//返回结果
} CL_VERUPDATA;

#endif

