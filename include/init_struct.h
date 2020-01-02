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

CL_RRUMODE rrumode =
{
	.ie_id = 13,
	.ie_size = 6,
};

SQ_RFCHSTA rfchsta =
{
	.ie_id = 302,
	.ie_size = 5,
};

SQ_RFCHANS rfchans =
{
	.ie_id = 352,
	.ie_size = 13,
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

PQ_SWRSTAANS swrstaans =
{
	.ie_id = 453,
	.ie_size = 13,
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
/***********************************************/

#endif
