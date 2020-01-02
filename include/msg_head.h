#ifndef __msg_head_
#define __msg_head_

typedef struct msg_head
{
	unsigned int msg_id;		//消息编号
	unsigned int msg_size;		//消息长度
	unsigned char rru_id;		//RRU ID 
	unsigned char bbu_id;		//BBU ID
	unsigned char ray_id;		//光纤端口号
	unsigned int inc_num;		//流水号
} MSG_HEAD;

#endif
