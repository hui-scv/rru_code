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

typedef struct rru_head
{
	unsigned char bbu_num;
	unsigned char rru_id;
	unsigned char rru_mac[6];
} RRU_HEAD;

typedef struct bbu_head
{
	unsigned char bbu_num;
	unsigned char rru_id;
	unsigned char bbu_id;
	unsigned char rru_mac[6];
	unsigned char rru_ip[4];
	unsigned char bbu_ip[4];
	unsigned char subnet_mask[4];
} BBU_HEAD;

#endif
