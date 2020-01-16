#ifndef ___str_h__
#define ___str_h__


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


extern CL_PROID porid;

extern CL_LINKTYPE linktype;

extern CL_RRUCAPA rrucapa;

extern CL_RRULV rrulv;

extern CL_RRUINFO rruinfo;

extern CL_RRUSOFTINFO rrusoftinfo;

extern CL_RRUFRE rrufre;

extern CL_RRURF rrurf;

extern CL_RRUCIR rrucir;

extern CL_SYSTIME systime;

extern CL_LINKADDR linkaddr;

extern CL_RRUMODE rrumode;

extern CL_SOFTCHK softchk;

extern CL_IRMODECFG irmodecfg;

extern CL_FTPINFO ftpinfo;

extern CL_CHLINKANS chlinkans;

extern CL_IRMODECFGANS irmodecfgans;

extern CL_VERUPDATA verupdata;
/*************************************************************************/

extern VD_VERDOWNANS verdownans;

extern VD_VERDOWNRES verdownres;
/*************************************************************************/

extern VA_RRUVERACTANS rruveractans;
/*************************************************************************/

extern SQ_RFCHSTA rfchsta;

extern SQ_RFCHANS rfchans[8];

extern SQ_CIRSTA cirsta;

extern SQ_CIRANS cirans;

extern SQ_OSCSTA oscsta;

extern SQ_OSCANS oscans;

extern SQ_RTCSTA rtcsta;

extern SQ_RTCANS rtcans;

extern SQ_RUNSTA runsta;

extern SQ_RUNANS runans;

extern SQ_IRMODESTA irmodesta;

extern SQ_IRMODEANS irmodeans;

extern SQ_INITCHK initchk;

extern SQ_INITCHKANS initchkans;

extern SQ_RAYSTA raysta;

extern SQ_RAYANS rayans;

extern PQ_SYSTIME systimeque;

extern PQ_CPURATE cpurate;

extern PQ_CPURATEANS cpurateans;

extern PQ_RATECYC ratecyc;

extern PQ_RATECYCANS ratecycans;

extern PQ_RRUTEM rrutem;

extern PQ_RRUTEMANS rrutemans;

extern PQ_SWRSTA swrsta;

extern PQ_SWRSTAANS swrstaans;

extern PQ_SWRTHR swrthr;

extern PQ_SWRTHRANS swrthrans;

extern PQ_TEMTHR temthr;

extern PQ_TEMTHRANS temthrans;

extern PQ_OUTPOWER outpower;

extern PQ_OUTPOWERANS outpowerans;

extern PQ_STAMACH stamach;

extern PQ_STAMACHANS stamachans;
/*************************************************************************/

extern PC_SYSTIME systimecfg;	//系统时间配置IE

extern PC_IQDATACH iqdatachcfg;

extern PC_RATECYC ratecyccfg;

extern PC_SWRTHR swrthrcfg;

extern PC_TEMTHR temthrcfg;

extern PC_RFCHSTA rfchstacfg;

extern PC_ANTCFG antcfg;

extern PC_SYSTIMEANS systimecfgans;

extern PC_IQDATACHANS iqdatachcfgans;

extern PC_RATECYCANS ratecyccfgans;

extern PC_SWRTHRANS swrthrcfgans;

/*PC_IRMODEANS irmodecfgans =
{
	.ie_id = 555,
	.ie_size = 10,
};*/

extern PC_TEMTHRANS temthrcfgans;

extern PC_RFCHSTAANS rfchstacfgans;

extern PC_ANTCFGANS antcfgans;
/*************************************************************************/

extern DM_CYCDELAY cycdelaycfg;

extern DM_RAYDELAYANS raydelayans;

extern DM_DELAYCFG delaycfg;

extern DM_DELAYCFGANS delaycfgans;
/*************************************************************************/

extern AR_ALAQUE alaque;

extern AR_ALAREP alarep;
/*************************************************************************/

extern LOG_UPQUE upque;

extern LOG_UPANS upans;

extern LOG_UPRES upres;
/*************************************************************************/

extern RET_RESET reset;
/*************************************************************************/

extern PLT_CFG pltcfg;

extern PLT_FREPIT_FDD frepit_fdd;

extern PLT_CFGANS pltcfgans;

extern PLT_FREPITANS frepitans;
/***********************************************/

#endif

