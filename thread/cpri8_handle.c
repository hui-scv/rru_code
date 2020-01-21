#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include "../include/thread/cpri8_handle.h"
#include "../include/struct.h"



/********************************************通信通道建立请求****************************************/
int cpri8_comch_req(int sk, BBU_HEAD cpri_ans)
{
	char send_msg[512];
	int count = 0, ret = 0;
	MSG_HEAD msg_head;

	msg_head.msg_id = CPRI_CHLINK_QUE;
	msg_head.msg_size = 354;
	msg_head.bbu_id = cpri_ans.bbu_id;
	msg_head.rru_id = cpri_ans.rru_id;
	msg_head.ray_id = cpri_ans.bbu_id & 0x0f;
	msg_head.inc_num = 0;

	memcpy(send_msg + count, &msg_head, sizeof(MSG_HEAD));
	count += sizeof(MSG_HEAD);
	memcpy(send_msg + count, &porid[7], sizeof(CL_PROID));
	count += sizeof(CL_PROID);
	memcpy(send_msg + count, &linktype[7], sizeof(CL_LINKTYPE));
	count += sizeof(CL_LINKTYPE);
	memcpy(send_msg + count, &rrucapa[7], sizeof(CL_RRUCAPA));
	count += sizeof(CL_RRUCAPA);
	memcpy(send_msg + count, &rrulv[7], sizeof(CL_RRULV));
	count += sizeof(CL_RRULV);
	memcpy(send_msg + count, &rruinfo[7], sizeof(CL_RRUINFO));
	count += sizeof(CL_RRUINFO);
	memcpy(send_msg + count, &rrusoftinfo[7], sizeof(CL_RRUSOFTINFO));
	count += sizeof(CL_RRUSOFTINFO);
	memcpy(send_msg + count, &rrufre[7], sizeof(CL_RRUFRE));
	count += sizeof(CL_RRUFRE);
	memcpy(send_msg + count, &rrurf[7], sizeof(CL_RRURF));
	count += sizeof(CL_RRURF);
	memcpy(send_msg + count, &rrucir[7], sizeof(CL_RRUCIR));
	count += sizeof(CL_RRUCIR);
	
	ret = send(sk, send_msg, count, 0);
	
	//printf("ret: %d, count: %d\n", ret, count);
	return ret;
}


/********************************************通信通道建立配置****************************************/
int cpri8_comch_cfg(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy((char *)&msg_head, msg, MSG_HEADSIZE);

	for(i = 0; i < 6; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);
		printf("ie_id : %d\n", ie_id);
		printf("count : %d\n", count);
		printf("size : %d\n", size);

		switch(ie_id)
		{
			case 11:
				memcpy((char *)(&systime[7]) + 4, (char *)src_addr, sizeof(CL_SYSTIME) - 4);
				break;
			case 12:
				memcpy((char *)(&linkaddr[7]) + 4, (char *)src_addr, sizeof(CL_LINKADDR) - 4);
				break;
			case 13:
				memcpy((char *)(&rrumode[7]) + 4, (char *)src_addr, sizeof(CL_RRUMODE) - 4);
				break;
			case 14:
				memcpy((char *)(&softchk[7]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				break;
			case 504:
				memcpy((char *)(&irmodecfg[7]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 
4);
				msg_head.msg_id = CPRI_CHLINK_ANS;
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[7]), sizeof(CL_IRMODECFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 505:
				memcpy((char *)(&ftpinfo[7]) + 4, (char *)src_addr, sizeof(CL_FTPINFO) - 4);
				break;
			default:
				ret += -1;
				return ret;
		}

		if(size == count)
			return 0;
	}
	
	return -1;
}

void cpri8_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans)
{
	char send_msg[512];
	int ret = 0, rec_num = 0;
	fd_set rdfds;
	struct timeval tv;

	cpri8_comch_req(sk, cpri_ans);

	while(1)
	{
		//接收BBU返回的通道建立配置，修改为等待10s，若没有响应则超时
		FD_ZERO(&rdfds);
		FD_SET(sk, &rdfds);
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		ret = select(sk + 1, &rdfds, NULL, NULL, &tv);
		if(ret < 0)
			printf("select erro!\n");
		else if(ret == 0)
		{
			printf("cpri8 chlink time out!\n");
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
			cpri8_comch_req(sk, cpri_ans);
			continue;
 		}else if(FD_ISSET(sk, &rdfds))
			rec_num += recv(sk, msg + rec_num, 512, 0);

		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			if(((MSG_HEAD *)msg)->msg_id == 2)
			{
				printf("ret\n");
				memcpy(msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
				printf("scv\n");
				//进行通信通道建立配置处理
				ret = cpri8_comch_cfg(sk, msg);
				printf("ret0\n");
				//返回配置响应
				if(ret >= 0)
				{
					msg_head->msg_id = CPRI_CHLINK_ANS;
					msg_head->msg_size = MSG_HEADSIZE + chlinkans[7].ie_size;
					memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
					memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[7]), sizeof(CL_CHLINKANS));
					printf("ret1 : %d\n", ret);
					ret = send(sk, send_msg, msg_head->msg_size, 0);
					printf("ret2 : %d\n", ret);
					break;
				}
				else
				{
					rec_num = 0;
					memset(msg, 0, sizeof(char) * 512);
				}
			}else
			{
				rec_num = 0;
				memset(msg, 0, sizeof(char) * 512);
			}
			msg_head->msg_id = CPRI_CHLINK_ANS;
			msg_head->msg_size = MSG_HEADSIZE + chlinkans[7].ie_size;
			memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[7]), sizeof(CL_CHLINKANS));
			send(sk, send_msg, msg_head->msg_size, 0);
			
			cpri8_comch_req(sk, cpri_ans);
		}
	}
}


/********************************************RRU版本下载请求分割线***************************************/
int cpri8_verdown_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count = 0;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 14 && count == size)
	{
		memcpy((char *)(&softchk[7]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		if(softchk[7].res == 0)
		{
			verdownans[7].soft_type = softchk[7].soft_type;
			verdownans[7].res = 1;
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[7].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[7]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);
			ret = 0;
		}else
		{
			verdownans[7].soft_type = softchk[7].soft_type;
			verdownans[7].res = 0;
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[7].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[7]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);
			/*******************************
			进行ftp下载
			*******************************/
			verdownres[7].soft_type = softchk[7].soft_type;
			verdownres[7].res = 0;
			msg_head.msg_id = CPRI_VERDOWN_IND;
			msg_head.msg_size = MSG_HEADSIZE + verdownres[7].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownres[7]), sizeof(VD_VERDOWNRES));
			send(sk, send_msg, msg_head.msg_size, 0);
			ret = 1;
		}
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU版本激活指示分割线***************************************/
int cpri8_veract_ind(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 6 && count == size)
	{
		memcpy((char *)(&rrusoftinfo[7]) + 4, (char *)src_addr, sizeof(CL_RRUSOFTINFO) - 4);
		/***********************************
		进行激活操作
		***********************************/
		msg_head.msg_id = CPRI_VERACT_ANS;
		msg_head.msg_size = MSG_HEADSIZE + rruveractans[7].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&rruveractans[7]), sizeof(VA_RRUVERACTANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU状态查询分割线***************************************/
int cpri8_state_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, ch, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_STATE_ANS;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);

		switch(ie_id)
		{
			case 302:		//射频通道状态查询
				//ch = ((SQ_RFCHSTA *)(msg + MSG_HEADSIZE))->ch_num;
				msg_head.msg_size = MSG_HEADSIZE + rfchans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchans[7]), sizeof(SQ_RFCHANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 303:
				msg_head.msg_size = MSG_HEADSIZE + cirans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cirans[7]), sizeof(SQ_CIRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 304:
				msg_head.msg_size = MSG_HEADSIZE + oscans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&oscans[7]), sizeof(SQ_OSCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 305:
				msg_head.msg_size = MSG_HEADSIZE + rtcans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rtcans[7]), sizeof(SQ_RTCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 306:
				msg_head.msg_size = MSG_HEADSIZE + runans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&runans[7]), sizeof(SQ_RUNANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 307:
				msg_head.msg_size = MSG_HEADSIZE + irmodeans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodeans[7]), sizeof(SQ_IRMODEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 308:
				msg_head.msg_size = MSG_HEADSIZE + initchkans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&initchkans[7]), sizeof(SQ_INITCHKANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 309:
				msg_head.msg_size = MSG_HEADSIZE + rayans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rayans[7]), sizeof(SQ_RAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			default:
				ret += -1;
				return ret;
		}
		
		if(size == count)
			return 0;
	}
	
	return -1;
}


/********************************************RRU参数查询分割线***************************************/
int cpri8_para_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_PARA_ANS;

	for(i = 0; i < 9; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);

		switch(ie_id)
		{
			case 401:
				msg_head.msg_size = MSG_HEADSIZE + systime[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systime[7]), sizeof(CL_SYSTIME));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 402:
				msg_head.msg_size = MSG_HEADSIZE + cpurateans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cpurateans[7]), sizeof(PQ_CPURATEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 403:
				msg_head.msg_size = MSG_HEADSIZE + ratecycans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecycans[7]), sizeof(PQ_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 404:
				msg_head.msg_size = MSG_HEADSIZE + rrutemans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rrutemans[7]), sizeof(PQ_RRUTEMANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 405:
				msg_head.msg_size = MSG_HEADSIZE + swrstaans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrstaans[7]), sizeof(PQ_SWRSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 406:
				msg_head.msg_size = MSG_HEADSIZE + swrthrans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrans[7]), sizeof(PQ_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 407:
				msg_head.msg_size = MSG_HEADSIZE + temthrans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrans[7]), sizeof(PQ_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 408:
				msg_head.msg_size = MSG_HEADSIZE + outpowerans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&outpowerans[7]), sizeof(PQ_OUTPOWERANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 409:
				msg_head.msg_size = MSG_HEADSIZE + stamachans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&stamachans[7]), sizeof(PQ_STAMACHANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			default:
				ret += -1;
				return ret;
		}
		
		if(size == count)
			return 0;

	}
	
	return -1;
}


/********************************************RRU参数配置分割线***************************************/
int cpri8_paracfg_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_PARACFG_ANS;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);

		switch(ie_id)
		{
			case 11:
				/************************************
				系统时间配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + systimecfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systimecfgans[7]), sizeof(PC_SYSTIMEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 501:
				/************************************
				IQ数据通道配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + iqdatachcfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&iqdatachcfgans[7]), sizeof(PC_IQDATACHANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 502:
				/************************************
				CPU占用率统计周期配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + ratecyccfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecyccfgans[7]), sizeof(PC_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 503:
				/************************************
				驻波比门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + swrthrcfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrcfgans[7]), sizeof(PC_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 504:
				/************************************
				驻波比门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[7]), sizeof(PC_IRMODEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 505:
				/************************************
				过温门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + temthrcfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrcfgans[7]), sizeof(PC_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 302:
				/************************************
				射频通道状态配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + rfchstacfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchstacfgans[7]), sizeof(PC_RFCHSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 507:
				/************************************
				天线配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + antcfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&antcfgans[7]), sizeof(PC_ANTCFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			default:
				ret += -1;
				return ret;
		}

		if(size == count)
			return 0;
	}
	
	return -1;
}


/********************************************RRU时延测量分割线***************************************/
int cpri8_delaymse_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);

	for(i = 0; i < 2; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);

		switch(ie_id)
		{
			case 901:
				/************************************
				时延测量
				************************************/
				msg_head.msg_id = CPRI_DELAYMSE_ANS;
				msg_head.msg_size = MSG_HEADSIZE + raydelayans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&raydelayans[7]), sizeof(DM_RAYDELAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 921:
				/************************************
				时延配置
				************************************/
				msg_head.msg_id = CPRI_DELAYCFG_ANS;
				msg_head.msg_size = MSG_HEADSIZE + delaycfgans[7].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&delaycfgans[7]), sizeof(DM_DELAYCFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			default:
				ret += -1;
				return ret;
		}

		if(size == count)
			return 0;
	}
	
	return -1;
}


/********************************************RRU报警查询分割线***************************************/
int cpri8_ala_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 1101 && count == size)
	{
		/************************************
		报警查询
		************************************/
		msg_head.msg_id = CPRI_ALA_ANS;
		msg_head.msg_size = MSG_HEADSIZE + alarep[7].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&alarep[7]), sizeof(AR_ALAREP));
		send(sk, send_msg, msg_head.msg_size, 0);

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU日志上传请求分割线***************************************/
int cpri8_logup_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 1201 && count == size)
	{
		/************************************
		日志上传请求
		************************************/
		msg_head.msg_id = CPRI_LOGUP_ANS;
		msg_head.msg_size = MSG_HEADSIZE + upans[7].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&upans[7]), sizeof(LOG_UPANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		if(upans[7].res == 0)
		{
			/*******************************
			进行ftp上传日志
			*******************************/
			msg_head.msg_id = CPRI_LOGUP_IND;
			msg_head.msg_size = MSG_HEADSIZE + upres[7].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&upres[7]), sizeof(LOG_UPRES));
			send(sk, send_msg, msg_head.msg_size, 0);
		}

				ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU复位指示分割线***************************************/
int cpri8_reset_ind(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 1301 && count == size)
	{
		/*******************************
		进行RRU的复位操作
		*******************************/

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************BBU心跳包消息分割线***************************************/
int cpri8_bbubeat_msg(int sk, char *msg, int *num)
{
	int ret = 0;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_RRUBEAT_MSG;
	msg_head.msg_size = MSG_HEADSIZE;

	*num = 0;
	/*******************************
	进行BBU在位监测
	*******************************/
	ret = send(sk, &msg_head, msg_head.msg_size, 0);
	
	return ret;
}


/********************************************RRUlte小区配置分割线***************************************/
int cpri8_lte_cfg(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);

	for(i = 0; i < 2; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);

		switch(ie_id)
		{
			case 1501:
				/*******************************
				先空置
				*******************************/
				break;
			case 1502:
				/*******************************
				先空置
				*******************************/
				break;
			default:
				ret += -1;
				return ret;
		}

		if(size == count)
			return 0;

	}
	
	return -1;
}


/***********************************************************************************/
int cpri8_write_str(char *msg, int i)
{
	int fd, ret;

	fd = open("./rru_info.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd < 0)
		goto fd_error;

	ret = lseek(fd, (i - 1) * 510, SEEK_SET);
	if(ret < 0)
	{
		goto error;
	}

	ret = write(fd, msg, i * 510);
	if(ret < 0)
	{
		goto error;
	}
	
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	printf("write_str error!\n");
	return -1;
}


