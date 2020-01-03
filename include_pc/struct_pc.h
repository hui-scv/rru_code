#ifndef ___str_pc_h__
#define ___str_pc_h__


#include "chlink/chlink_pc.h"

#include "misc/ala_rep_pc.h"
//#include "misc/delay_mea.h"
//#include "misc/log.h"
//#include "misc/plot.h"
//#include "misc/reset.h"

//#include "parameter/para_cfg.h"
#include "parameter/para_que_pc.h"

#include "version/state_que_pc.h"
//#include "version/ver_act.h"
//#include "version/ver_down.h"

#include "test/test_pc.h"


extern CL_PROID_PC porid_pc;
extern CL_LINKTYPE_PC linktype_pc;
extern CL_RRUCAPA_PC rrucapa_pc;
extern CL_RRULV_PC rrulv_pc;
extern CL_RRUINFO_PC rruinfo_pc;
extern CL_RRUSOFTINFO_PC rrusoftinfo_pc;
extern CL_RRUFRE_PC rrufre_pc;
extern CL_RRURF_PC rrurf_pc;
extern CL_RRUCIR_PC rrucir_pc;
extern CL_SYSTIME_PC systime_pc;
extern CL_RRUMODE_PC rrumode_pc;
extern SQ_RFCHSTA_PC rfchsta_pc;
extern SQ_RFCHANS_PC rfchans_pc[8];
extern SQ_CIRSTA_PC cirsta_pc;
extern SQ_CIRANS_PC cirans_pc;
extern SQ_OSCSTA_PC oscsta_pc;
extern SQ_OSCANS_PC oscans_pc;
extern SQ_RTCSTA_PC rtcsta_pc;
extern SQ_RTCANS_PC rtcans_pc;
extern SQ_RUNSTA_PC runsta_pc;
extern SQ_RUNANS_PC runans_pc;
extern SQ_IRMODESTA_PC irmodesta_pc;
extern SQ_IRMODEANS_PC irmodeans_pc;
extern SQ_RAYSTA_PC raysta_pc;
extern SQ_RAYANS_PC rayans_pc;
extern PQ_SYSTIME_PC systimeque_pc;
extern PQ_CPURATE_PC cpurate_pc;
extern PQ_CPURATEANS_PC cpurateans_pc;
extern PQ_RATECYC_PC ratecyc_pc;
extern PQ_RATECYCANS_PC ratecycans_pc;
extern PQ_RRUTEM_PC rrutem_pc;
extern PQ_SWRSTAANS_PC swrstaans_pc;
extern PQ_TEMTHR_PC temthr_pc;
extern PQ_TEMTHRANS_PC temthrans_pc;
extern PQ_STAMACH_PC stamach_pc;
extern PQ_STAMACHANS_PC stamachans_pc;
extern AR_ALAQUE_PC alaque_pc;
extern AR_ALAREP_PC alarep_pc;
extern TS_RFTSEN_PC rftsen_pc;
extern TS_SENCHSET_PC senchset_pc;
extern TS_RECCHSET_PC recchset_pc;
extern TS_SENCIRSET_PC sencirset_pc;
extern TS_SENCIRANS_PC sencirans_pc;
extern TS_SENDIMSET_PC sendimset_pc;
extern TS_CIRDATAQUE_PC cirdataque_pc;
extern TS_CIRDATAANS_PC cirdataans_pc;
extern TS_RECCIRDATA_PC reccirdata_pc;
extern TS_RECDIMSET_PC recdimset_pc;
extern MSG_HEAD_PC head_pc;

#endif
