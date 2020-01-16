#ifndef __msg_head_
#define __msg_head_

#pragma pack(1)

#define MSG_HEADSIZE 15

#define CPRI_CHLINK_QUE 1
#define CPRI_CHLINK_CFG 2
#define CPRI_CHLINK_ANS 3
#define CPRI_VERUP_IND 4
#define CPRI_VERUP_ANS 5
#define CPRI_VERDOWN_QUE 21
#define CPRI_VERDOWN_ANS 22
#define CPRI_VERDOWN_IND 23
#define CPRI_VERACT_IND 31
#define CPRI_VERACT_ANS 32
#define CPRI_STATE_QUE 41
#define CPRI_STATE_ANS 42
#define CPRI_PARA_QUE 51
#define CPRI_PARA_ANS 52
#define CPRI_PARACFG_QUE 61
#define CPRI_PARACFG_ANS 62
#define CPRI_DELAYMSE_QUE 101
#define CPRI_DELAYMSE_ANS 102
#define CPRI_DELAYCFG_CMD 103
#define CPRI_DELAYCFG_ANS 104
#define CPRI_ALAREP_QUE 111
#define CPRI_ALA_QUE 121
#define CPRI_ALA_ANS 122
#define CPRI_LOGUP_QUE 131
#define CPRI_LOGUP_ANS 132
#define CPRI_LOGUP_IND 133
#define CPRI_RESET_IND 141
#define CPRI_RRUBEAT_MSG 171
#define CPRI_BBUBEAT_MSG 181
#define CPRI_SCDMA_CFG 191
#define CPRI_SCDMA_ANS 192
#define CPRI_LTE_CFG 193
#define CPRI_LTE_ANS 194


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
