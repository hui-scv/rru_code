#ifndef ____str_init_h__
#define ____str_init_h__


#include "chlink/chlink.h"

#include "misc/ala_rep.h"
#include "misc/delay_mea.h"
#include "misc/log.h"
#include "misc/plot.h"
#include "misc/reset.h"

#include "parameter/para_cfg.h"
#include "parameter/para_que.h"

#include "version/state_que.h"
#include "version/ver_act.h"
#include "version/ver_down.h"


/*******************定义各种结构体***************/
pthread_rwlock_t chlinkque_rwlock;

CL_PROID porid[] =		//由PC端提供此信息
{[0 ... 7] =
	{
	.ie_id = 1,
	.ie_size = 100,
	.rru_manu = {"TUOLAI"},
	.rru_pub = {"TUOLAI"},
	.pro_num = {"RRU-001-001"},
	.birth_date = {"2020-03-20"},
	.used_date = {"2020-03-20"},
	}
};

CL_LINKTYPE linktype[] =		//由RRU自己设置，并在通道建立之后，将建立原因改为上电
{[0 ... 7] = 
	{
	.ie_id = 2,
	.ie_size = 9,
	.link_type = 2,
	}
};

CL_RRUCAPA rrucapa[] =			//由PC端提供此信息
{[0 ... 7] = 
	{
	.ie_id = 3,
	.ie_size = 35,
	.td_num = 0,
	.lte_num = 0,
	._5g_num = 1,
	.ant_num = 1,
	.tra_power = -2560,
	.master_type = 1,
	.rru_dl_max = 33333,
	.rru_ul_max = 33333,
	.mode_sel = 4,
	.ant_type = 0,
	.mbbu_dl_sel = 0xff,
	.sbbu_dl_sel = 0xff,
	.mrru_ul_sel = 0xff,
	.srru_ul_sel = 0xff,
	}
};

CL_RRULV rrulv[] =			//由PC端提供此信息
{[0 ... 7] = 
	{
	.ie_id = 4,
	.ie_size = 5,
	.rru_lv = 1,
	}
};

CL_RRUINFO rruinfo[] =			//由PC端提供此信息
{[0 ... 7] = 
	{
	.ie_id = 5,
	.ie_size = 52,
	.type = {'T', 'U', 'O', 'L', 'A', 'I', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x01, 0x00, 0x01, 0x00, 0x00, 0xF6,},
	.version = {"TLHWV1.0"},
	}
};

CL_RRUSOFTINFO rrusoftinfo[] =	//一开始由PC端提供，后面进行版本下载时，要更新此信息
{[0 ... 7] = 
	{
	.ie_id = 6,
	.ie_size = 84,
	.soft_ver = {"TLSWV1.0"},
	.firm_ver = {"TLFWV1.0"},
	}
};

CL_RRUFRE rrufre[] =		//此信息每个cpri口可能不一样，所以不能使用同一个文件记录此信息
{[0 ... 7] = 
	{
	.ie_id = 7,
	.ie_size = 14,
	.fre_start = 1,
	.fre_end = 10,
	.td_wid = 0,
	.td_num = 0,
	.lte_num = 0,
	._5g_num = 1,
	.fre_num = 0,
	}
};

CL_RRURF rrurf[] =			//此信息每个cpri口可能不一样，所以不能使用同一个文件记录此信息
{[0 ... 7] = 
	{
	.ie_id = 8,
	.ie_size = 17,
	.rf_chal = 1,
	.ant_num = 1,
	.td_start = 0,
	.td_end = 0,
	.lte_start = 0,
	.lte_end = 0,
	._5g_start = 0xF0,
	._5g_end = 0xF0,
	.chal_max_rf = -2560,
	.rf_section = 0,
	.chal_mode = 3,
	}
};

CL_RRUCIR rrucir[] =
{[0 ... 7] = 
	{
	.ie_id = 9,
	.ie_size = 23,
	.rf_chal = 1,
	._400_num = 1,
	}
};

CL_SYSTIME systime[] =
{[0 ... 7] = 
	{
	.ie_id = 11,
	.ie_size = 11,
	}
};

CL_LINKADDR linkaddr[] =
{[0 ... 7] = 
	{
	.ie_id = 12,
	.ie_size = 8,
	}
};

CL_RRUMODE rrumode[] =
{[0 ... 7] = 
	{
	.ie_id = 13,
	.ie_size = 6,
	}
};

CL_SOFTCHK softchk[] =
{[0 ... 7] = 
	{
	.ie_id = 14,
	.ie_size = 289,
	}
};

CL_SOFTCHK firmchk[] =
{[0 ... 7] = 
	{
	.ie_id = 14,
	.ie_size = 289,
	}
};

CL_IRMODECFG irmodecfg[] =
{[0 ... 7] = 
	{
	.ie_id = 504,
	.ie_size = 8,
	}
};

CL_FTPINFO ftpinfo[] =
{[0 ... 7] = 
	{
	.ie_id = 505,
	.ie_size = 84,
	}
};

CL_CHLINKANS chlinkans[] =
{[0 ... 7] = 
	{
	.ie_id = 21,
	.ie_size = 8,
	.res = 1,
	}
};

CL_IRMODECFGANS irmodecfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 555,
	.ie_size = 10,
	}
};

CL_VERUPDATA verupdata[] =
{[0 ... 7] = 
	{
	.ie_id = 31,
	.ie_size = 9,
	}
};
/*************************************************************************/

VD_VERDOWNANS verdownans[] =
{[0 ... 7] = 
	{
	.ie_id = 101,
	.ie_size = 9,
	}
};

VD_VERDOWNRES verdownres[] =
{[0 ... 7] = 
	{
	.ie_id = 111,
	.ie_size = 9,
	}
};
/*************************************************************************/

VA_RRUVERACTANS rruveractans[] =
{[0 ... 7] = 
	{
	.ie_id = 201,
	.ie_size = 9,
	}
};
/*************************************************************************/

SQ_RFCHSTA rfchsta[] =
{[0 ... 7] = 
	{
	.ie_id = 302,
	.ie_size = 5,
	}
};

SQ_RFCHANS rfchans[] =
{[0 ... 7] = 
	{
	.ie_id = 352,
	.ie_size = 13,
	}
};

SQ_CIRSTA cirsta[] =
{[0 ... 7] = 
	{
	.ie_id = 303,
	.ie_size = 4,
	}
};

SQ_CIRANS cirans[] =
{[0 ... 7] = 
	{
	.ie_id = 353,
	.ie_size = 10,
	}
};

SQ_OSCSTA oscsta[] =
{[0 ... 7] = 
	{
	.ie_id = 304,
	.ie_size = 4,
	}
};

SQ_OSCANS oscans[] =
{[0 ... 7] = 
	{
	.ie_id = 354,
	.ie_size = 12,
	}
};

SQ_RTCSTA rtcsta[] =
{[0 ... 7] = 
	{
	.ie_id = 305,
	.ie_size = 4,
	}
};

SQ_RTCANS rtcans[] =
{[0 ... 7] = 
	{
	.ie_id = 355,
	.ie_size = 8,
	.sta = 1,
	}
};

SQ_RUNSTA runsta[] =
{[0 ... 7] = 
	{
	.ie_id = 306,
	.ie_size = 4,
	}
};

SQ_RUNANS runans[] =
{[0 ... 7] = 
	{
	.ie_id = 356,
	.ie_size = 8,
	}
};

SQ_IRMODESTA irmodesta[] =
{[0 ... 7] = 
	{
	.ie_id = 307,
	.ie_size = 4,
	}
};

SQ_IRMODEANS irmodeans[] =
{[0 ... 7] = 
	{
	.ie_id = 357,
	.ie_size = 8,
	}
};

SQ_INITCHK initchk[] =
{[0 ... 7] = 
	{
	.ie_id = 308,
	.ie_size = 4,
	}
};

SQ_INITCHKANS initchkans[] =
{[0 ... 7] = 
	{
	.ie_id = 358,
	.ie_size = 20,
	}
};

SQ_RAYSTA raysta[] =
{[0 ... 7] = 
	{
	.ie_id = 309,
	.ie_size = 5,
	}
};

SQ_RAYANS rayans[] =
{[0 ... 7] = 
	{
	.ie_id = 359,
	.ie_size = 37,
	}
};

PQ_SYSTIME systimeque[] =
{[0 ... 7] = 
	{
	.ie_id = 401,
	.ie_size = 4,
	}
};

PQ_CPURATE cpurate[] =
{[0 ... 7] = 
	{
	.ie_id = 402,
	.ie_size = 4,
	}
};

PQ_CPURATEANS cpurateans[] =
{[0 ... 7] = 
	{
	.ie_id = 451,
	.ie_size = 8,
	}
};

PQ_RATECYC ratecyc[] =
{[0 ... 7] = 
	{
	.ie_id = 403,
	.ie_size = 4,
	}
};

PQ_RATECYCANS ratecycans[] =
{[0 ... 7] = 
	{
	.ie_id = 452,
	.ie_size = 8,
	.rate_cyc = 10,
	}
};

PQ_RRUTEM rrutem[] =
{[0 ... 7] = 
	{
	.ie_id = 404,
	.ie_size = 4,
	}
};

PQ_RRUTEMANS rrutemans[] =
{[0 ... 7] = 
	{
	.ie_id = 453,
	.ie_size = 13,
	}
};

PQ_SWRSTA swrsta[] =
{[0 ... 7] = 
	{
	.ie_id = 405,
	.ie_size = 5,
	}
};

PQ_SWRSTAANS swrstaans[] =
{[0 ... 7] = 
	{
	.ie_id = 454,
	.ie_size = 9,
	}
};

PQ_SWRTHR swrthr[] =
{[0 ... 7] = 
	{
	.ie_id = 406,
	.ie_size = 4,
	}
};

PQ_SWRTHRANS swrthrans[] =
{[0 ... 7] = 
	{
	.ie_id = 455,
	.ie_size = 12,
	.no1_thr = 10,
	.no2_thr = 10,
	}
};

PQ_TEMTHR temthr[] =
{[0 ... 7] = 
	{
	.ie_id = 407,
	.ie_size = 4,
	}
};

PQ_TEMTHRANS temthrans[] =
{[0 ... 7] = 
	{
	.ie_id = 456,
	.ie_size = 12,
	}
};

PQ_OUTPOWER outpower[] =
{[0 ... 7] = 
	{
	.ie_id = 408,
	.ie_size = 5,
	}
};

PQ_OUTPOWERANS outpowerans[] =
{[0 ... 7] = 
	{
	.ie_id = 457,
	.ie_size = 7,
	}
};

PQ_STAMACH stamach[] =
{[0 ... 7] = 
	{
	.ie_id = 409,
	.ie_size = 4,
	}
};

PQ_STAMACHANS stamachans[] =
{[0 ... 7] = 
	{
	.ie_id = 458,
	.ie_size = 5,
	}
};
/*************************************************************************/

PC_SYSTIME systimecfg[] =	//系统时间配置IE
{[0 ... 7] = 
	{
	.ie_id = 11,
	.ie_size = 11,
	}
};

PC_IQDATACH iqdatachcfg[] =
{[0 ... 7] = 
	{
	.ie_id = 501,
	.ie_size = 8,
	}
};

PC_RATECYC ratecyccfg[] =
{[0 ... 7] = 
	{
	.ie_id = 502,
	.ie_size = 8,
	}
};

PC_SWRTHR swrthrcfg[] =
{[0 ... 7] = 
	{
	.ie_id = 503,
	.ie_size = 12,
	}
};

PC_TEMTHR temthrcfg[] =
{[0 ... 7] = 
	{
	.ie_id = 505,
	.ie_size = 16,
	}
};

PC_RFCHSTA rfchstacfg[] =
{[0 ... 7] = 
	{
	.ie_id = 302,
	.ie_size = 5,
	}
};

PC_ANTCFG antcfg[] =
{[0 ... 7] = 
	{
	.ie_id = 507,
	.ie_size = 10,
	}
};

PC_SYSTIMEANS systimecfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 551,
	.ie_size = 8,
	}
};

PC_IQDATACHANS iqdatachcfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 552,
	.ie_size = 8,
	}
};

PC_RATECYCANS ratecyccfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 553,
	.ie_size = 8,
	}
};

PC_SWRTHRANS swrthrcfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 554,
	.ie_size = 8,
	}
};

/*PC_IRMODEANS irmodecfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 555,
	.ie_size = 10,
	}
};*/

PC_TEMTHRANS temthrcfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 556,
	.ie_size = 12,
	}
};

PC_RFCHSTAANS rfchstacfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 557,
	.ie_size = 9,
	}
};

PC_ANTCFGANS antcfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 559,
	.ie_size = 9,
	}
};
/*************************************************************************/

DM_CYCDELAY cycdelaycfg[] =
{[0 ... 7] = 
	{
	.ie_id = 901,
	.ie_size = 5,
	}
};

DM_RAYDELAYANS raydelayans[] =
{[0 ... 7] = 
	{
	.ie_id = 911,
	.ie_size = 29,
	.dl_delay = 0,
	.ul_delay = 0,
	.n = 1,
	}
};

DM_DELAYCFG delaycfg[] =
{[0 ... 7] = 
	{
	.ie_id = 921,
	.ie_size = 25,
	}
};

DM_DELAYCFGANS delaycfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 931,
	.ie_size = 6,
	}
};
/*************************************************************************/

AR_ALAQUE alaque[] =
{[0 ... 7] = 
	{
	.ie_id = 1101,
	.ie_size = 12,
	}
};

AR_ALAREP alarep[] =
{[0 ... 7] = 
	{
	.ie_id = 1001,
	.ie_size = 138,
	}
};

AR_ALAEXL alaexl[] =
{[0 ... 7] =
	{
	.a_00001 = 00001,
	.a_10001 = 10001,
	.a_20001 = 20001,
	.a_30001 = 30001,
	.a_40001 = 40001,
	.a_50001 = 50001,
	.a_60001 = 60001,
	.a_70001 = 70001,
	.a_80001 = 80001,
	.a_90001 = 90001,
	.a_100001 = 100001,
	.a_120001 = 120001,
	.a_130001 = 130001,
	.a_140001 = 140001,
	.a_150001 = 150001,
	.a_160001 = 160001,
	.a_170001 = 170001,
	.a_180001 = 180001,
	.a_190001 = 190001,
	.a_200001 = 200001,
	.a_210001 = 210001,
	.a_220001 = 220001,
	.a_230001 = 230001,
	}
};
/*************************************************************************/

LOG_UPQUE upque[] =
{[0 ... 7] = 
	{
	.ie_id = 1201,
	.ie_size = 224,
	}
};

LOG_UPANS upans[] =
{[0 ... 7] = 
	{
	.ie_id = 1211,
	.ie_size = 12,
	}
};

LOG_UPRES upres[] =
{[0 ... 7] = 
	{
	.ie_id = 1221,
	.ie_size = 8,
	}
};
/*************************************************************************/

RET_RESET reset[] =
{[0 ... 7] = 
	{
	.ie_id = 1301,
	.ie_size = 8,
	}
};
/*************************************************************************/

PLT_CFG pltcfg[] =
{[0 ... 7] = 
	{
	.ie_id = 1501,
	.ie_size = 13,
	}
};

PLT_FREPIT_FDD frepit_fdd[] =
{[0 ... 7] = 
	{
	.ie_id = 1503,
	.ie_size = 31,
	}
};

PLT_CFGANS pltcfgans[] =
{[0 ... 7] = 
	{
	.ie_id = 1511,
	.ie_size = 12,
	}
};

PLT_FREPITANS frepitans[] =
{[0 ... 7] = 
	{
	.ie_id = 1512,
	.ie_size = 13,
	}
};
/***********************************************/

#endif

