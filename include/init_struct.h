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
CL_PROID porid =
{
	.ie_id = 1,
	.ie_size = 100,
};

CL_LINKTYPE linktype =
{
	.ie_id = 2,
	.ie_size = 9,
};

CL_RRUCAPA rrucapa =
{
	.ie_id = 3,
	.ie_size = 35,
};

CL_RRULV rrulv =
{
	.ie_id = 4,
	.ie_size = 5,
};

CL_RRUINFO rruinfo =
{
	.ie_id = 5,
	.ie_size = 52,
};

CL_RRUSOFTINFO rrusoftinfo =
{
	.ie_id = 6,
	.ie_size = 84,
};

CL_RRUFRE rrufre =
{
	.ie_id = 7,
	.ie_size = 14,
};

CL_RRURF rrurf =
{
	.ie_id = 8,
	.ie_size = 17,
};

CL_RRUCIR rrucir =
{
	.ie_id = 9,
	.ie_size = 23,
};

CL_SYSTIME systime =
{
	.ie_id = 11,
	.ie_size = 11,
};

CL_LINKADDR linkaddr =
{
	.ie_id = 12,
	.ie_size = 8,
};

CL_RRUMODE rrumode =
{
	.ie_id = 13,
	.ie_size = 6,
};

CL_SOFTCHK softchk =
{
	.ie_id = 14,
	.ie_size = 289,
};

CL_IRMODECFG irmodecfg =
{
	.ie_id = 504,
	.ie_size = 8,
};

CL_FTPINFO ftpinfo =
{
	.ie_id = 505,
	.ie_size = 84,
};

CL_CHLINKANS chlinkans =
{
	.ie_id = 21,
	.ie_size = 8,
};

CL_IRMODECFGANS irmodecfgans =
{
	.ie_id = 555,
	.ie_size = 10,
};

CL_VERUPDATA verupdata =
{
	.ie_id = 31,
	.ie_size = 9,
};
/*************************************************************************/

VD_VERDOWNANS verdownans =
{
	.ie_id = 101,
	.ie_size = 9,
};

VD_VERDOWNRES verdownres =
{
	.ie_id = 111,
	.ie_size = 9,
};
/*************************************************************************/

VA_RRUVERACTANS rruveractans =
{
	.ie_id = 201,
	.ie_size = 9,
};
/*************************************************************************/

SQ_RFCHSTA rfchsta =
{
	.ie_id = 302,
	.ie_size = 5,
};

SQ_RFCHANS rfchans[] =
{[0 ... 7] = 
	{
	.ie_id = 352,
	.ie_size = 13,
	}
};

SQ_CIRSTA cirsta =
{
	.ie_id = 303,
	.ie_size = 4,
};

SQ_CIRANS cirans =
{
	.ie_id = 353,
	.ie_size = 10,
};

SQ_OSCSTA oscsta =
{
	.ie_id = 304,
	.ie_size = 4,
};

SQ_OSCANS oscans =
{
	.ie_id = 354,
	.ie_size = 12,
};

SQ_RTCSTA rtcsta =
{
	.ie_id = 305,
	.ie_size = 4,
};

SQ_RTCANS rtcans =
{
	.ie_id = 355,
	.ie_size = 8,
};

SQ_RUNSTA runsta =
{
	.ie_id = 306,
	.ie_size = 4,
};

SQ_RUNANS runans =
{
	.ie_id = 356,
	.ie_size = 8,
};

SQ_IRMODESTA irmodesta =
{
	.ie_id = 307,
	.ie_size = 4,
};

SQ_IRMODEANS irmodeans =
{
	.ie_id = 357,
	.ie_size = 8,
};

SQ_INITCHK initchk =
{
	.ie_id = 308,
	.ie_size = 4,
};

SQ_INITCHKANS initchkans =
{
	.ie_id = 358,
	.ie_size = 20,
};

SQ_RAYSTA raysta =
{
	.ie_id = 309,
	.ie_size = 5,
};

SQ_RAYANS rayans =
{
	.ie_id = 359,
	.ie_size = 37,
};

PQ_SYSTIME systimeque =
{
	.ie_id = 401,
	.ie_size = 4,
};

PQ_CPURATE cpurate =
{
	.ie_id = 402,
	.ie_size = 4,
};

PQ_CPURATEANS cpurateans =
{
	.ie_id = 451,
	.ie_size = 8,
};

PQ_RATECYC ratecyc =
{
	.ie_id = 403,
	.ie_size = 4,
};

PQ_RATECYCANS ratecycans =
{
	.ie_id = 452,
	.ie_size = 8,
};

PQ_RRUTEM rrutem =
{
	.ie_id = 404,
	.ie_size = 4,
};

PQ_RRUTEMANS rrutemans =
{
	.ie_id = 453,
	.ie_size = 13,
};

PQ_SWRSTA swrsta =
{
	.ie_id = 405,
	.ie_size = 5,
};

PQ_SWRSTAANS swrstaans =
{
	.ie_id = 454,
	.ie_size = 9,
};

PQ_SWRTHR swrthr =
{
	.ie_id = 406,
	.ie_size = 4,
};

PQ_SWRTHRANS swrthrans =
{
	.ie_id = 455,
	.ie_size = 12,
};

PQ_TEMTHR temthr =
{
	.ie_id = 407,
	.ie_size = 4,
};

PQ_TEMTHRANS temthrans =
{
	.ie_id = 456,
	.ie_size = 12,
};

PQ_OUTPOWER outpower =
{
	.ie_id = 408,
	.ie_size = 5,
};

PQ_OUTPOWERANS outpowerans =
{
	.ie_id = 457,
	.ie_size = 7,	
};

PQ_STAMACH stamach =
{
	.ie_id = 409,
	.ie_size = 4,
};

PQ_STAMACHANS stamachans =
{
	.ie_id = 458,
	.ie_size = 5,
};
/*************************************************************************/

PC_SYSTIME systimecfg =	//系统时间配置IE
{
	.ie_id = 11,
	.ie_size = 11,
};

PC_IQDATACH iqdatachcfg =
{
	.ie_id = 501,
	.ie_size = 8,
};

PC_RATECYC ratecyccfg =
{
	.ie_id = 502,
	.ie_size = 8,
};

PC_SWRTHR swrthrcfg =
{
	.ie_id = 503,
	.ie_size = 12,
};

PC_TEMTHR temthrcfg =
{
	.ie_id = 505,
	.ie_size = 16,
};

PC_RFCHSTA rfchstacfg =
{
	.ie_id = 302,
	.ie_size = 5,
};

PC_ANTCFG antcfg =
{
	.ie_id = 507,
	.ie_size = 10,
};

PC_SYSTIMEANS systimecfgans =
{
	.ie_id = 551,
	.ie_size = 8,
};

PC_IQDATACHANS iqdatachcfgans =
{
	.ie_id = 552,
	.ie_size = 8,
};

PC_RATECYCANS ratecyccfgans =
{
	.ie_id = 553,
	.ie_size = 8,
};

PC_SWRTHRANS swrthrcfgans =
{
	.ie_id = 554,
	.ie_size = 8,
};

/*PC_IRMODEANS irmodecfgans =
{
	.ie_id = 555,
	.ie_size = 10,
};*/

PC_TEMTHRANS temthrcfgans =
{
	.ie_id = 556,
	.ie_size = 12,
};

PC_RFCHSTAANS rfchstacfgans =
{
	.ie_id = 557,
	.ie_size = 9,
};

PC_ANTCFGANS antcfgans =
{
	.ie_id = 559,
	.ie_size = 9,
};
/*************************************************************************/

DM_CYCDELAY cycdelaycfg =
{
	.ie_id = 901,
	.ie_size = 5,
};

DM_RAYDELAYANS raydelayans =
{
	.ie_id = 911,
	.ie_size = 29,
};

DM_DELAYCFG delaycfg =
{
	.ie_id = 921,
	.ie_size = 25,
};

DM_DELAYCFGANS delaycfgans =
{
	.ie_id = 931,
	.ie_size = 6,
};
/*************************************************************************/

AR_ALAQUE alaque =
{
	.ie_id = 1101,
	.ie_size = 12,
};

AR_ALAREP alarep =
{
	.ie_id = 1001,
	.ie_size = 138,
};
/*************************************************************************/

LOG_UPQUE upque =
{
	.ie_id = 1201,
	.ie_size = 224,
};

LOG_UPANS upans =
{
	.ie_id = 1211,
	.ie_size = 12,
};

LOG_UPRES upres =
{
	.ie_id = 1221,
	.ie_size = 8,
};
/*************************************************************************/

RET_RESET reset =
{
	.ie_id = 1301,
	.ie_size = 8,
};
/*************************************************************************/

PLT_CFG pltcfg =
{
	.ie_id = 1501,
	.ie_size = 13,
};

PLT_FREPIT_FDD frepit_fdd =
{
	.ie_id = 1503,
	.ie_size = 31,
};

PLT_CFGANS pltcfgans =
{
	.ie_id = 1511,
	.ie_size = 12,
};

PLT_FREPITANS frepitans =
{
	.ie_id = 1512,
	.ie_size = 13,
};
/***********************************************/

#endif

