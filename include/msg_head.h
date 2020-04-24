/*
 * 文件名：msg_head.h
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：记录了BBU和RRU之间通信时，使用的消息头的格式以及消息编号表。
 */

#ifndef __msg_head_
#define __msg_head_

//消息头的大小
#define MSG_HEADSIZE 15

/*
 * 消息编号表:
 * 消息编号	消息名称
 * 1	通道建立请求
 * 2	通道建立配置
 * 3	通道建立配置应答
 * 4	版本更新结果指示
 * 5	版本更新结果指示应答
 * 11	RRU版本查询请求
 * 12	RRU版本查询请求应答
 * 21	版本下载请求
 * 22	版本下载应答
 * 23	版本下载结果指示
 * 31	RRU版本激活指示
 * 32	RRU版本激活应答
 * 41	RRU状态查询
 * 42	RRU状态查询响应
 * 51	RRU参数查询
 * 52	RRU参数查询响应
 * 61	RRU参数配置
 * 62	RRU参数配置响应
 * 71	初始化校准结果上报
 * 81	校准参数配置请求
 * 82	校准参数配置请求响应
 * 83	RRU天线校准开关设置
 * 84	RRU天线校准开关设置应答
 * 85	RRU天线校准请求
 * 86	RRU天线校准指示
 * 87	RRU天线校准指示应答
 * 88	RRU天线校准结束指示
 * 89	RRU天线校准结束指示应答
 * 91	天线校准结果查询请求
 * 92	天线校准结果查询请求应答
 * 101	时延测量请求
 * 102	时延测量响应
 * 103	时延配置命令
 * 104	时延配置命令响应
 * 111	告警上报请求
 * 121	告警查询请求
 * 122	 * 1查询请求应答
 * 131	日志上传请求
 * 132	日志上传应答
 * 133	日志上传结果指示
 * 141	复位指示
 * 151	远程RRU复位指示
 * 161	环回请求
 * 162	环回请求应答
 * 171	RRU在位心跳消息
 * 181	BBU在位心跳消息
 * 191	TD-SCDMA小区配置
 * 192	TD-SCDMA小区配置响应
 * 193	TD-LTE小区配置
 * 194	TD-LTE小区配置响应
 * 201	周期性校准结果上报
 * 211	天线校准请求
 * 212	天线校准请求应答
 */
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

#pragma pack(1)

typedef struct msg_head		//BBU和RRU进行正常通信时，使用的消息头
{
	unsigned int msg_id;		//消息编号
	unsigned int msg_size;		//消息长度
	unsigned char rru_id;		//RRU ID 
	unsigned char bbu_id;		//BBU ID
	unsigned char ray_id;		//光纤端口号
	unsigned int inc_num;		//流水号
} MSG_HEAD;

typedef struct rru_head		//RRU向BBU发送UDP广播信息的格式
{
	unsigned char bbu_num;		//BBU侧光口号
	unsigned char rru_id;		//RRU的ID号
	unsigned char rru_mac[6];	//RRU的mac地址
} RRU_HEAD;

typedef struct bbu_head		//BBU向RRU应答的UDP广播信息的格式
{
	unsigned char bbu_num;		//BBU侧光口号
	unsigned char rru_id;		//RRU的ID号
	unsigned char bbu_id;		//BBU的ID号
	unsigned char rru_mac[6];	//RRU的mac地址
	unsigned char rru_ip[4];	//RRU的ip地址
	unsigned char bbu_ip[4];	//BBU的ip地址
	unsigned char subnet_mask[4];	//子网掩码
} BBU_HEAD;

#pragma pack()

#endif
