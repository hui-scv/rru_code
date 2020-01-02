#ifndef ____str_pc_init_h__
#define ____str_pc_init_h__


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


/*******************定义各种结构体***************/
CL_PROID_PC porid_pc =
{
	.ie_id = 1,
	.ie_size = 100,
};

CL_LINKTYPE_PC linktype_pc =
{
	.ie_id = 2,
	.ie_size = 9,
};

CL_RRUCAPA_PC rrucapa_pc =
{
	.ie_id = 3,
	.ie_size = 35,
};

CL_RRULV_PC rrulv_pc =
{
	.ie_id = 4,
	.ie_size = 5,
};

CL_RRUINFO_PC rruinfo_pc =
{
	.ie_id = 5,
	.ie_size = 52,
};

CL_RRUSOFTINFO_PC rrusoftinfo_pc =
{
	.ie_id = 6,
	.ie_size = 84,
};

CL_RRUFRE_PC rrufre_pc =
{
	.ie_id = 7,
	.ie_size = 14,
};

CL_RRURF_PC rrurf_pc =
{
	.ie_id = 8,
	.ie_size = 17,
};

CL_RRUCIR_PC rrucir_pc =
{
	.ie_id = 9,
	.ie_size = 23,
};

CL_SYSTIME_PC systime_pc =
{
	.ie_id = 11,
	.ie_size = 11,
};

CL_RRUMODE_PC rrumode_pc =
{
	.ie_id = 13,
	.ie_size = 6,
};

SQ_RFCHSTA_PC rfchsta_pc =
{
	.ie_id = 302,
	.ie_size = 5,
};

SQ_RFCHANS_PC rfchans_pc =
{
	.ie_id = 352,
	.ie_size = 13,
};

SQ_CIRSTA_PC cirsta_pc =
{
	.ie_id = 303,
	.ie_size = 4,
};

SQ_CIRANS_PC cirans_pc =
{
	.ie_id = 353,
	.ie_size = 10,
};

SQ_OSCSTA_PC oscsta_pc =
{
	.ie_id = 304,
	.ie_size = 4,
};

SQ_OSCANS_PC oscans_pc =
{
	.ie_id = 354,
	.ie_size = 12,
};

SQ_RTCSTA_PC rtcsta_pc =
{
	.ie_id = 305,
	.ie_size = 4,
};

SQ_RTCANS_PC rtcans_pc =
{
	.ie_id = 355,
	.ie_size = 8,
};

SQ_RUNSTA_PC runsta_pc =
{
	.ie_id = 306,
	.ie_size = 4,
};

SQ_RUNANS_PC runans_pc =
{
	.ie_id = 356,
	.ie_size = 8,
};

SQ_IRMODESTA_PC irmodesta_pc =
{
	.ie_id = 307,
	.ie_size = 4,
};

SQ_IRMODEANS_PC irmodeans_pc =
{
	.ie_id = 357,
	.ie_size = 8,
};

SQ_RAYSTA_PC raysta_pc =
{
	.ie_id = 309,
	.ie_size = 5,
};

SQ_RAYANS_PC rayans_pc =
{
	.ie_id = 359,
	.ie_size = 37,
};

PQ_SYSTIME_PC systimeque_pc =
{
	.ie_id = 401,
	.ie_size = 4,
};

PQ_CPURATE_PC cpurate_pc =
{
	.ie_id = 402,
	.ie_size = 4,
};

PQ_CPURATEANS_PC cpurateans_pc =
{
	.ie_id = 451,
	.ie_size = 8,
};

PQ_RATECYC_PC ratecyc_pc =
{
	.ie_id = 403,
	.ie_size = 4,
};

PQ_RATECYCANS_PC ratecycans_pc =
{
	.ie_id = 452,
	.ie_size = 8,
};

PQ_RRUTEM_PC rrutem_pc =
{
	.ie_id = 404,
	.ie_size = 4,
};

PQ_SWRSTAANS_PC swrstaans_pc =
{
	.ie_id = 453,
	.ie_size = 13,
};

PQ_TEMTHR_PC temthr_pc =
{
	.ie_id = 407,
	.ie_size = 4,
};

PQ_TEMTHRANS_PC temthrans_pc =
{
	.ie_id = 456,
	.ie_size = 12,
};

PQ_STAMACH_PC stamach_pc =
{
	.ie_id = 409,
	.ie_size = 4,
};

PQ_STAMACHANS_PC stamachans_pc =
{
	.ie_id = 458,
	.ie_size = 5,
};

AR_ALAQUE_PC alaque_pc =
{
	.ie_id = 1101,
	.ie_size = 12,
};

AR_ALAREP_PC alarep_pc =
{
	.ie_id = 1001,
	.ie_size = 138,
};

TS_RFTSEN_PC rftsen_pc =
{
	.ie_id = 11000,
	.ie_size = 5,
};

TS_SENCHSET_PC senchset_pc =
{
	.ie_id = 11001,
	.ie_size = 6,
};

TS_RECCHSET_PC recchset_pc =
{
	.ie_id = 11002,
	.ie_size = 6,
};

TS_SENCIRSET_PC sencirset_pc =
{
	.ie_id = 11003,
	.ie_size = 219,
};

TS_SENCIRANS_PC sencirans_pc =
{
	.ie_id = 11009,
	.ie_size = 14,
};

TS_SENDIMSET_PC sendimset_pc =
{
	.ie_id = 11004,
	.ie_size = 6,
};

TS_CIRDATAQUE_PC cirdataque_pc =
{
	.ie_id = 11005,
	.ie_size = 5,
};

TS_CIRDATAANS_PC cirdataans_pc =
{
	.ie_id = 11008,
	.ie_size = 221,
};

TS_RECCIRDATA_PC reccirdata_pc =
{
	.ie_id = 11006,
	.ie_size = 14,
};

TS_RECDIMSET_PC recdimset_pc =
{
	.ie_id = 11007,
	.ie_size = 6,
};

MSG_HEAD_PC head_pc =
{
	.msg_head = 0x7EAA,
};
/***********************************************/

#endif
