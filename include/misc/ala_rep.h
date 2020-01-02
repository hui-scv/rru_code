#ifndef ___ala_rep_
#define ___ala_rep_

#include "../msg_head.h"


typedef struct ar_alarep		//告警上报请求IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned short ala_able;	//告警有效性
	unsigned int ala_code;		//告警码
	unsigned int ala_subcode;	//告警子码
	unsigned int ala_flag;		//告警清除标志
	unsigned char time_stamp[20];	//时间戳
	unsigned char addi_data[100];	//附加信息
} AR_ALAREP;

typedef struct ar_alaque		//告警查询请求IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int ala_code;		//告警码
	unsigned int ala_subcode;	//告警子码
} AR_ALAQUE;

#endif
