#ifndef ___ala_rep_
#define ___ala_rep_

#include "../msg_head.h"

#pragma pack(1)

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

typedef struct ar_alaexl
{
	unsigned int a_00001;
	unsigned int a_10001;
	unsigned int a_20001;
	unsigned int a_30001;
	unsigned int a_40001;
	unsigned int a_50001;
	unsigned int a_60001;
	unsigned int a_70001;
	unsigned int a_80001;
	unsigned int a_90001;
	unsigned int a_100001;
	unsigned int a_120001;
	unsigned int a_130001;
	unsigned int a_140001;
	unsigned int a_150001;
	unsigned int a_160001;
	unsigned int a_170001;
	unsigned int a_180001;
	unsigned int a_190001;
	unsigned int a_200001;
	unsigned int a_210001;
	unsigned int a_220001;
	unsigned int a_230001;
}AR_ALAEXL;

#pragma pack()

#endif
