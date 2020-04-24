#ifndef ___reset_h
#define ___reset_h

#include "../msg_head.h"

#pragma pack(1)

typedef struct ret_reset		//复位指示IE
{
	unsigned short ie_id;		//IE标志
	unsigned short ie_size;		//IE长度
	unsigned int ret_type;		//复位类型
} RET_RESET;

#pragma pack()

#endif
