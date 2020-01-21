#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include "../include/thread/cpri7_handle.h"
#include "../include/struct.h"



/********************************************通信通道建立请求****************************************/
int cpri7_comch_req(int sk, BBU_HEAD cpri_ans)
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
	memcpy(send_msg + count, &porid[6], sizeof(CL_PROID));
	count += sizeof(CL_PROID);
	memcpy(send_msg + count, &linktype[6], sizeof(CL_LINKTYPE));
	count += sizeof(CL_LINKTYPE);
	memcpy(send_msg + count, &rrucapa[6], sizeof(CL_RRUCAPA));
	count += sizeof(CL_RRUCAPA);
	memcpy(send_msg + count, &rrulv[6], sizeof(CL_RRULV));
	count += sizeof(CL_RRULV);
	memcpy(send_msg + count, &rruinfo[6], sizeof(CL_RRUINFO));
	count += sizeof(CL_RRUINFO);
	memcpy(send_msg + count, &rrusoftinfo[6], sizeof(CL_RRUSOFTINFO));
	count += sizeof(CL_RRUSOFTINFO);
	memcpy(send_msg + count, &rrufre[6], sizeof(CL_RRUFRE));
	count += sizeof(CL_RRUFRE);
	memcpy(send_msg + count, &rrurf[6], sizeof(CL_RRURF));
	count += sizeof(CL_RRURF);
	memcpy(send_msg + count, &rrucir[6], sizeof(CL_RRUCIR));
	count += sizeof(CL_RRUCIR);
	
	ret = send(sk, send_msg, count, 0);
	
	//printf("ret: %d, count: %d\n", ret, count);
	return ret;
}


/********************************************通信通道建立配置****************************************/
int cpri7_comch_cfg(int sk, char *msg)
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
				memcpy((char *)(&systime[6]) + 4, (char *)src_addr, sizeof(CL_SYSTIME) - 4);
				break;
			case 12:
				memcpy((char *)(&linkaddr[6]) + 4, (char *)src_addr, sizeof(CL_LINKADDR) - 4);
				break;
			case 13:
				memcpy((char *)(&rrumode[6]) + 4, (char *)src_addr, sizeof(CL_RRUMODE) - 4);
				break;
			case 14:
				memcpy((char *)(&softchk[6]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				break;
			case 504:
				memcpy((char *)(&irmodecfg[6]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 
4);
				msg_head.msg_id = CPRI_CHLINK_ANS;
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[6]), sizeof(CL_IRMODECFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 505:
				memcpy((char *)(&ftpinfo[6]) + 4, (char *)src_addr, sizeof(CL_FTPINFO) - 4);
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

void cpri7_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans)
{
	char send_msg[512];
	int ret = 0, rec_num = 0;
	fd_set rdfds;
	struct timeval tv;

	cpri7_comch_req(sk, cpri_ans);

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
			printf("cpri7 chlink time out!\n");
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
			cpri7_comch_req(sk, cpri_ans);
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
				ret = cpri7_comch_cfg(sk, msg);
				printf("ret0\n");
				//返回配置响应
				if(ret >= 0)
				{
					msg_head->msg_id = CPRI_CHLINK_ANS;
					msg_head->msg_size = MSG_HEADSIZE + chlinkans[6].ie_size;
					memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
					memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[6]), sizeof(CL_CHLINKANS));
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
			msg_head->msg_size = MSG_HEADSIZE + chlinkans[6].ie_size;
			memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[6]), sizeof(CL_CHLINKANS));
			send(sk, send_msg, msg_head->msg_size, 0);
			
			cpri7_comch_req(sk, cpri_ans);
		}
	}
}


/********************************************RRU版本下载请求分割线***************************************/
int cpri7_verdown_que(int sk, char *msg)
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
		memcpy((char *)(&softchk[6]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		if(softchk[6].res == 0)
		{
			verdownans[6].soft_type = softchk[6].soft_type;
			verdownans[6].res = 1;
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[6].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[6]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);
			ret = 0;
		}else
		{
			verdownans[6].soft_type = softchk[6].soft_type;
			verdownans[6].res = 0;
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[6].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[6]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);
			/*******************************
			进行ftp下载
			*******************************/
			verdownres[6].soft_type = softchk[6].soft_type;
			verdownres[6].res = 0;
			msg_head.msg_id = CPRI_VERDOWN_IND;
			msg_head.msg_size = MSG_HEADSIZE + verdownres[6].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownres[6]), sizeof(VD_VERDOWNRES));
			send(sk, send_msg, msg_head.msg_size, 0);
			ret = 1;
		}
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU版本激活指示分割线***************************************/
int cpri7_veract_ind(int sk, char *msg)
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
		memcpy((char *)(&rrusoftinfo[6]) + 4, (char *)src_addr, sizeof(CL_RRUSOFTINFO) - 4);
		/***********************************
		进行激活操作
		***********************************/
		msg_head.msg_id = CPRI_VERACT_ANS;
		msg_head.msg_size = MSG_HEADSIZE + rruveractans[6].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&rruveractans[6]), sizeof(VA_RRUVERACTANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU状态查询分割线***************************************/
int cpri7_state_que(int sk, char *msg)
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
				msg_head.msg_size = MSG_HEADSIZE + rfchans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchans[6]), sizeof(SQ_RFCHANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 303:
				msg_head.msg_size = MSG_HEADSIZE + cirans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cirans[6]), sizeof(SQ_CIRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 304:
				msg_head.msg_size = MSG_HEADSIZE + oscans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&oscans[6]), sizeof(SQ_OSCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 305:
				msg_head.msg_size = MSG_HEADSIZE + rtcans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rtcans[6]), sizeof(SQ_RTCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 306:
				msg_head.msg_size = MSG_HEADSIZE + runans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&runans[6]), sizeof(SQ_RUNANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 307:
				msg_head.msg_size = MSG_HEADSIZE + irmodeans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodeans[6]), sizeof(SQ_IRMODEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 308:
				msg_head.msg_size = MSG_HEADSIZE + initchkans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&initchkans[6]), sizeof(SQ_INITCHKANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 309:
				msg_head.msg_size = MSG_HEADSIZE + rayans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rayans[6]), sizeof(SQ_RAYANS));
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
int cpri7_para_que(int sk, char *msg)
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
				msg_head.msg_size = MSG_HEADSIZE + systime[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systime[6]), sizeof(CL_SYSTIME));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 402:
				msg_head.msg_size = MSG_HEADSIZE + cpurateans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cpurateans[6]), sizeof(PQ_CPURATEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 403:
				msg_head.msg_size = MSG_HEADSIZE + ratecycans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecycans[6]), sizeof(PQ_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 404:
				msg_head.msg_size = MSG_HEADSIZE + rrutemans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rrutemans[6]), sizeof(PQ_RRUTEMANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 405:
				msg_head.msg_size = MSG_HEADSIZE + swrstaans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrstaans[6]), sizeof(PQ_SWRSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 406:
				msg_head.msg_size = MSG_HEADSIZE + swrthrans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrans[6]), sizeof(PQ_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 407:
				msg_head.msg_size = MSG_HEADSIZE + temthrans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrans[6]), sizeof(PQ_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 408:
				msg_head.msg_size = MSG_HEADSIZE + outpowerans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&outpowerans[6]), sizeof(PQ_OUTPOWERANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 409:
				msg_head.msg_size = MSG_HEADSIZE + stamachans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&stamachans[6]), sizeof(PQ_STAMACHANS));
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
int cpri7_paracfg_que(int sk, char *msg)
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
				msg_head.msg_size = MSG_HEADSIZE + systimecfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systimecfgans[6]), sizeof(PC_SYSTIMEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 501:
				/************************************
				IQ数据通道配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + iqdatachcfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&iqdatachcfgans[6]), sizeof(PC_IQDATACHANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 502:
				/************************************
				CPU占用率统计周期配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + ratecyccfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecyccfgans[6]), sizeof(PC_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 503:
				/************************************
				驻波比门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + swrthrcfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrcfgans[6]), sizeof(PC_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 504:
				/************************************
				驻波比门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[6]), sizeof(PC_IRMODEANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 505:
				/************************************
				过温门限配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + temthrcfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrcfgans[6]), sizeof(PC_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 302:
				/************************************
				射频通道状态配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + rfchstacfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchstacfgans[6]), sizeof(PC_RFCHSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 507:
				/************************************
				天线配置
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + antcfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&antcfgans[6]), sizeof(PC_ANTCFGANS));
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
int cpri7_delaymse_que(int sk, char *msg)
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
				msg_head.msg_size = MSG_HEADSIZE + raydelayans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&raydelayans[6]), sizeof(DM_RAYDELAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 921:
				/************************************
				时延配置
				************************************/
				msg_head.msg_id = CPRI_DELAYCFG_ANS;
				msg_head.msg_size = MSG_HEADSIZE + delaycfgans[6].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&delaycfgans[6]), sizeof(DM_DELAYCFGANS));
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
int cpri7_ala_que(int sk, char *msg)
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
		msg_head.msg_size = MSG_HEADSIZE + alarep[6].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&alarep[6]), sizeof(AR_ALAREP));
		send(sk, send_msg, msg_head.msg_size, 0);

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU日志上传请求分割线***************************************/
int cpri7_logup_que(int sk, char *msg)
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
		msg_head.msg_size = MSG_HEADSIZE + upans[6].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&upans[6]), sizeof(LOG_UPANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		if(upans[6].res == 0)
		{
			/*******************************
			进行ftp上传日志
			*******************************/
			msg_head.msg_id = CPRI_LOGUP_IND;
			msg_head.msg_size = MSG_HEADSIZE + upres[6].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&upres[6]), sizeof(LOG_UPRES));
			send(sk, send_msg, msg_head.msg_size, 0);
		}

				ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/********************************************RRU复位指示分割线***************************************/
int cpri7_reset_ind(int sk, char *msg)
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
int cpri7_bbubeat_msg(int sk, char *msg, int *num)
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
int cpri7_lte_cfg(int sk, char *msg)
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
int cpri7_write_str(char *msg, int i)
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


