#ifndef ___str_h__
#define ___str_h__

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

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


//cpri网口名称
#define ETH0 "eth0"
#define ETH1 "eth1"
#define ETH2 "eth2"
#define ETH3 "eth3"
#define ETH4 "eth4"
#define ETH5 "eth5"
#define ETH6 "eth6"
#define ETH7 "eth7"


extern pthread_rwlock_t chlinkque_rwlock;

extern CL_PROID porid[8];

extern CL_LINKTYPE linktype[8];

extern CL_RRUCAPA rrucapa[8];

extern CL_RRULV rrulv[8];

extern CL_RRUINFO rruinfo[8];

extern CL_RRUSOFTINFO rrusoftinfo[8];

extern CL_RRUFRE rrufre[8];

extern CL_RRURF rrurf[8];

extern CL_RRUCIR rrucir[8];

extern CL_SYSTIME systime[8];

extern CL_LINKADDR linkaddr[8];

extern CL_RRUMODE rrumode[8];

extern CL_SOFTCHK softchk[8];

extern CL_SOFTCHK bioschk[8];

extern CL_IRMODECFG irmodecfg[8];

extern CL_FTPINFO ftpinfo[8];

extern CL_CHLINKANS chlinkans[8];

extern CL_IRMODECFGANS irmodecfgans[8];

extern CL_VERUPDATA verupdata[8];
/*************************************************************************/

extern VD_VERDOWNANS verdownans[8];

extern VD_VERDOWNRES verdownres[8];
/*************************************************************************/

extern VA_RRUVERACTANS rruveractans[8];
/*************************************************************************/

extern SQ_RFCHSTA rfchsta[8];

extern SQ_RFCHANS rfchans[8];

extern SQ_CIRSTA cirsta[8];

extern SQ_CIRANS cirans[8];

extern SQ_OSCSTA oscsta[8];

extern SQ_OSCANS oscans[8];

extern SQ_RTCSTA rtcsta[8];

extern SQ_RTCANS rtcans[8];

extern SQ_RUNSTA runsta[8];

extern SQ_RUNANS runans[8];

extern SQ_IRMODESTA irmodesta[8];

extern SQ_IRMODEANS irmodeans[8];

extern SQ_INITCHK initchk[8];

extern SQ_INITCHKANS initchkans[8];

extern SQ_RAYSTA raysta[8];

extern SQ_RAYANS rayans[8];

extern PQ_SYSTIME systimeque[8];

extern PQ_CPURATE cpurate[8];

extern PQ_CPURATEANS cpurateans[8];

extern PQ_RATECYC ratecyc[8];

extern PQ_RATECYCANS ratecycans[8];

extern PQ_RRUTEM rrutem[8];

extern PQ_RRUTEMANS rrutemans[8];

extern PQ_SWRSTA swrsta[8];

extern PQ_SWRSTAANS swrstaans[8];

extern PQ_SWRTHR swrthr[8];

extern PQ_SWRTHRANS swrthrans[8];

extern PQ_TEMTHR temthr[8];

extern PQ_TEMTHRANS temthrans[8];

extern PQ_OUTPOWER outpower[8];

extern PQ_OUTPOWERANS outpowerans[8];

extern PQ_STAMACH stamach[8];

extern PQ_STAMACHANS stamachans[8];
/*************************************************************************/

extern PC_SYSTIME systimecfg[8];	//系统时间配置IE

extern PC_IQDATACH iqdatachcfg[8];

extern PC_RATECYC ratecyccfg[8];

extern PC_SWRTHR swrthrcfg[8];

extern PC_TEMTHR temthrcfg[8];

extern PC_RFCHSTA rfchstacfg[8];

extern PC_ANTCFG antcfg[8];

extern PC_SYSTIMEANS systimecfgans[8];

extern PC_IQDATACHANS iqdatachcfgans[8];

extern PC_RATECYCANS ratecyccfgans[8];

extern PC_SWRTHRANS swrthrcfgans[8];

/*PC_IRMODEANS irmodecfgans =
{
	.ie_id = 555,
	.ie_size = 10,
};*/

extern PC_TEMTHRANS temthrcfgans[8];

extern PC_RFCHSTAANS rfchstacfgans[8];

extern PC_ANTCFGANS antcfgans[8];
/*************************************************************************/

extern DM_CYCDELAY cycdelaycfg[8];

extern DM_RAYDELAYANS raydelayans[8];

extern DM_DELAYCFG delaycfg[8];

extern DM_DELAYCFGANS delaycfgans[8];
/*************************************************************************/

extern AR_ALAQUE alaque[8];

extern AR_ALAREP alarep[8];

extern AR_ALAEXL alaexl[8];
/*************************************************************************/

extern LOG_UPQUE upque[8];

extern LOG_UPANS upans[8];

extern LOG_UPRES upres[8];
/*************************************************************************/

extern RET_RESET reset[8];
/*************************************************************************/

extern PLT_CFG pltcfg[8];

extern PLT_FREPIT_FDD frepit_fdd[8];

extern PLT_CFGANS pltcfgans[8];

extern PLT_FREPITANS frepitans[8];
/***********************************************/

#endif

