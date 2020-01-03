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
extern CL_RRUMODE rrumode;

extern SQ_RFCHSTA rfchsta;
extern SQ_RFCHANS rfchans;
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
extern SQ_RAYSTA raysta;
extern SQ_RAYANS rayans;

extern PQ_SYSTIME systimeque;
extern PQ_CPURATE cpurate;
extern PQ_CPURATEANS cpurateans;
extern PQ_RATECYC ratecyc;
extern PQ_RATECYCANS ratecycans;
extern PQ_RRUTEM rrutem;
extern PQ_SWRSTAANS swrstaans;
extern PQ_TEMTHR temthr;
extern PQ_TEMTHRANS temthrans;
extern PQ_STAMACH stamach;
extern PQ_STAMACHANS;

extern AR_ALAQUE alaque;
extern AR_ALAREP alarep;

#endif

