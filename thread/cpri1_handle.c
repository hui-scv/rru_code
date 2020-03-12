/*
 * 文件名：cpri1_handle.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：cpri1接口各个消息体的实际处理函数合集。
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>

#include "thread/cpri1_handle.h"
#include "struct.h"
#include "ftp_client.h"
#include "usr.h"



extern unsigned int cpri1_ala_flag;
extern char cpri1_bbu_ip[20];

/*
 * 函数名：int cpri1_comch_req(int sk, BBU_HEAD cpri_ans)
 * 功能描述：cpri向BBU端口发送所有关于通道建立请求的消息结构体。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、cpri_ans，BBU应答RRU的广播信息结构体。
 * output：
 *		ret：发送数据的大小
 */
int cpri1_comch_req(int sk, BBU_HEAD cpri_ans)
{
	char send_msg[512];
	int count = 0, ret = 0;
	MSG_HEAD msg_head;

	//初始化通道建立请求的消息头
	msg_head.msg_id = CPRI_CHLINK_QUE;
	msg_head.msg_size = 354;
	msg_head.bbu_id = cpri_ans.bbu_id;
	msg_head.rru_id = cpri_ans.rru_id;
	msg_head.ray_id = cpri_ans.bbu_id & 0x0f;
	msg_head.inc_num = 0;

	//将消息头和通道建立请求的所有消息体都拷贝到send_msg数组中
	memcpy(send_msg + count, &msg_head, sizeof(MSG_HEAD));
	count += sizeof(MSG_HEAD);
	memcpy(send_msg + count, &porid[0], sizeof(CL_PROID));
	count += sizeof(CL_PROID);
	memcpy(send_msg + count, &linktype[0], sizeof(CL_LINKTYPE));
	count += sizeof(CL_LINKTYPE);
	memcpy(send_msg + count, &rrucapa[0], sizeof(CL_RRUCAPA));
	count += sizeof(CL_RRUCAPA);
	memcpy(send_msg + count, &rrulv[0], sizeof(CL_RRULV));
	count += sizeof(CL_RRULV);
	memcpy(send_msg + count, &rruinfo[0], sizeof(CL_RRUINFO));
	count += sizeof(CL_RRUINFO);
	memcpy(send_msg + count, &rrusoftinfo[0], sizeof(CL_RRUSOFTINFO));
	count += sizeof(CL_RRUSOFTINFO);
	memcpy(send_msg + count, &rrufre[0], sizeof(CL_RRUFRE));
	count += sizeof(CL_RRUFRE);
	memcpy(send_msg + count, &rrurf[0], sizeof(CL_RRURF));
	count += sizeof(CL_RRURF);
	memcpy(send_msg + count, &rrucir[0], sizeof(CL_RRUCIR));
	count += sizeof(CL_RRUCIR);
	
	//发送给BBU端口
	ret = send(sk, send_msg, count, 0);
	
	//printf("ret: %d, count: %d\n", ret, count);
	return ret;
}


/*
 * 函数名：int cpri1_comch_cfg(int sk, char *msg)
 * 功能描述：cpri进行通道建立配置的操作。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，BBU端口应答cpri的通道建立配置消息体。
 * output：
 *		失败：-1；
 *		成功：0
 */
int cpri1_comch_cfg(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;
	int ver_ret = -1;	//软件版本是否相同标识

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	//计算得到除去消息头的长度，有效消息体的总长度是多少
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	memcpy((char *)&msg_head, msg, MSG_HEADSIZE);

	for(i = 0; i < 7; i++)
	{
		//得到有效消息体ie的标识号
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		//除去消息头和有效消息体ie中的标识号和大小成员，得到的实际通道建立配置信息的地址
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		//累计ie的长度，每执行一次增加一个ie的长度，最后会得到一个有效消息体的总长度
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);
		printf("ie_id : %d\n", ie_id);
		printf("count : %d\n", count);
		printf("size : %d\n", size);

		//通过标识号去匹配相应ie需要执行的相应操作
		switch(ie_id)
		{
			case 11:	//系统时间配置
				memcpy((char *)(&systime[0]) + 4, (char *)src_addr, sizeof(CL_SYSTIME) - 4);
				cpri1_set_systime((PC_SYSTIME *)&systime[0]);
				break;
			case 12:	//保存BBU侧的FTP服务器的地址信息
				memcpy((char *)(&linkaddr[0]) + 4, (char *)src_addr, sizeof(CL_LINKADDR) - 4);
				break;
			case 13:	//RRU操作模式设置，目前固定为正常模式
				memcpy((char *)(&rrumode[0]) + 4, (char *)src_addr, sizeof(CL_RRUMODE) - 4);
				break;
			case 14:	//软件版本核对结果，这个结果是由BBU得出的
				if(*(char *)src_addr == 0)	//将软件的版本核对结果赋值
					memcpy((char *)(&softchk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				else						//将固件的版本核对结果赋值
					memcpy((char *)(&bioschk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				break;
			case 504:	//Ir口工作模式，目前固定为普通模式（Ir红外接口）
				//完成Ir口工作模式配置并应答BBU
				memcpy((char *)(&irmodecfg[0]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 
4);
				irmodeans[0].res = irmodecfg[0].ir_mode;		//这里将设置状态查询中ir口工作模式查询的内容
				irmodecfgans[0].res = 0;		//设置参数配置成功
				msg_head.msg_id = CPRI_CHLINK_ANS;
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[0]), sizeof(CL_IRMODECFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 505:	//保存FTP的用户名和密码
				memcpy((char *)(&ftpinfo[0]) + 4, (char *)src_addr, sizeof(CL_FTPINFO) - 4);
				break;
			default:
				ret += -1;
				return ret;
		}

		//当消息头中的ie总大小和实际计算出的ie总大小相等时，则成功
		if(size == count)
			return 0;
	}
	
	return -1;
}


/*
 * 函数名：void cpri1_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans)
 * 功能描述：cpri1将不断向BBU端口发送通信链路建立请求消息，直到BBU正确应答。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，用于接受BBU端口回复的信息；
 * 		3、msg_head，消息头结构体；
 * 		4、cpri_ans，BBU应答RRU的广播信息结构体。
 * output：void
 */
void cpri1_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans)
{
	char send_msg[512], server_file[220];
	int ret = 0, rec_num = 0;
	fd_set rdfds;
	struct timeval tv;

	memset(server_file, 0, sizeof(char)*220);
	//cpri发送通信链路建立的消息集合请求
	cpri1_comch_req(sk, cpri_ans);

	FD_ZERO(&rdfds);
	FD_SET(sk, &rdfds);

	while(1)
	{
		//接收BBU返回的通道建立配置信息，等待10s，若没有接收到则超时
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		ret = select(sk + 1, &rdfds, NULL, NULL, &tv);
		if(ret < 0)
			printf("select erro!\n");
		else if(ret == 0)
		{
			//当select返回0时，说明等待超时，此时就重新发送通道建立请求
			printf("cpri1 chlink time out!\n");
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
			cpri1_comch_req(sk, cpri_ans);
			continue;
 		}else if(FD_ISSET(sk, &rdfds))
			rec_num += recv(sk, msg + rec_num, 512, 0);		//等到应答后，进行接收数据。数据有可能不是1包发完，所以需要进行+=，累积之前接收的数据

		//当接收的数据总大小和返回的消息头中指明数据的大小相等之后，说明数据接收完毕
		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			//然后再判断是否是通道建立配置消息体
			if(((MSG_HEAD *)msg)->msg_id == 2)
			{
				printf("ret\n");
				memcpy(msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
				printf("scv\n");
				//进行通信通道建立配置处理
				ret = cpri1_comch_cfg(sk, msg);
				printf("ret0\n");
				//返回配置响应
				if(ret >= 0)
				{
					//发送配置成功的响应，并退出循环
					msg_head->msg_id = CPRI_CHLINK_ANS;		//设置消息头类型
					msg_head->msg_size = MSG_HEADSIZE + chlinkans[0].ie_size;	//设置消息体的总大小
					chlinkans[0].res = 0;		//设置通道建立配置应答的结果为成功
					memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
					memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[0]), sizeof(CL_CHLINKANS));
					printf("ret1 : %d\n", ret);
					ret = send(sk, send_msg, msg_head->msg_size, 0);	//发送通道建立配置应答
					printf("ret2 : %d\n", ret);

					if(softchk[0].res != 0)	//得出版本不一致，需要更新软件
					{
						strcat(server_file, softchk[0].file_path);
						strcat(server_file, softchk[0].file_name);
						ret = ftp_down(ETH0, linkaddr[0].bbu_ip, softchk[0].file_name, server_file, 0);
						verupdata[0].soft_type = softchk[0].soft_type;
						if(ret == -5)
							verupdata[0].res = 1;		//给出版本下载的结果
						else
							verupdata[0].res = 0;
						
						msg_head->msg_id = CPRI_VERUP_IND;
						msg_head->msg_size = MSG_HEADSIZE + softchk[0].ie_size;
						memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
						memcpy(send_msg + MSG_HEADSIZE, (char *)(&softchk[0]), sizeof(CL_SOFTCHK));
						send(sk, send_msg, msg_head->msg_size, 0);
					}
					if(bioschk[0].res != 0)	//得出版本不一致，需要更新软件
					{
						strcat(server_file, bioschk[0].file_path);
						strcat(server_file, bioschk[0].file_name);
						ret = ftp_down(ETH0, linkaddr[0].bbu_ip, bioschk[0].file_name, server_file, 0);
						verupdata[0].soft_type = bioschk[0].soft_type;
						if(ret == -5)
							verupdata[0].res = 1;		//给出版本下载的结果
						else
							verupdata[0].res = 0;
						
						msg_head->msg_id = CPRI_VERUP_IND;
						msg_head->msg_size = MSG_HEADSIZE + bioschk[0].ie_size;
						memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
						memcpy(send_msg + MSG_HEADSIZE, (char *)(&bioschk[0]), sizeof(CL_SOFTCHK));
						send(sk, send_msg, msg_head->msg_size, 0);
					}
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

			//发送配置失败的响应，并重新请求建立
			msg_head->msg_id = CPRI_CHLINK_ANS;
			msg_head->msg_size = MSG_HEADSIZE + chlinkans[0].ie_size;
			chlinkans[0].res = 1;		//设置通道建立配置应答的结果为失败
			memcpy(send_msg, (char *)msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[0]), sizeof(CL_CHLINKANS));
			send(sk, send_msg, msg_head->msg_size, 0);
			
			cpri1_comch_req(sk, cpri_ans);
		}
	}
}


/*
 * 函数名：int cpri1_verdown_que(int sk, char *msg)
 * 功能描述：RRU版本下载请求处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、软件版本一致，不需要下载；
 * 		1、软件版本不一致，需要下载；
 * 		-1、接收到的消息体有误。
 */
int cpri1_verdown_que(int sk, char *msg)
{
	char *src_addr, send_msg[512], server_file[220];
	unsigned short ie_id;
	int size = 0, ret = 0, count = 0;
	MSG_HEAD msg_head;

	memset(server_file, 0, sizeof(char)*220);
	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);	//将接收到的消息头信息存入msg_head结构体
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;	//得到所有ie消息体的总大小
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);	//得到版本下载请求ie消息体的大小
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);	//得到此ie的id号
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 14 && count == size)
	{
		memcpy((char *)(&softchk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		/*if(*(char *)src_addr == 0)	//将软件的版本核对结果赋值
			memcpy((char *)(&softchk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		else						//将固件的版本核对结果赋值
			memcpy((char *)(&bioschk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		break;*/
		if(softchk[0].res == 0)		//判断接收到的版本下载请求ie，看软件版本是否一致
		{
			verdownans[0].soft_type = softchk[0].soft_type;
			verdownans[0].res = 1;		//拒绝版本下载，在这里给出拒绝原因，因为软件版本一致
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[0].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[0]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载应答消息体
			ret = 0;
		}else
		{
			verdownans[0].soft_type = softchk[0].soft_type;
			verdownans[0].res = 0;		//可以进行版本下载
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[0].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[0]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载应答消息体

			//进行ftp下载文件
			strcat(server_file, softchk[0].file_path);
			strcat(server_file, softchk[0].file_name);
			ret = ftp_down(ETH0, linkaddr[0].bbu_ip, softchk[0].file_name, server_file, 0);
			verdownres[0].soft_type = softchk[0].soft_type;
			if(ret == -5)
				verdownres[0].res = 1;		//给出版本下载的结果
			else
				verdownres[0].res = 0;
			msg_head.msg_id = CPRI_VERDOWN_IND;
			msg_head.msg_size = MSG_HEADSIZE + verdownres[0].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownres[0]), sizeof(VD_VERDOWNRES));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载结果指示的消息体
			ret = 1;
		}
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri1_veract_ind(int sk, char *msg)
 * 功能描述：RRU版本激活指示处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的激活操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_veract_ind(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id;
	int size = 0, ret = 0, count, flag[2] = {0, 0};
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);	//将接收到的消息头信息存入msg_head结构体
	
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;
	size = *(unsigned short *)(msg + 2 + MSG_HEADSIZE);
	ie_id = *(unsigned short *)(msg + MSG_HEADSIZE);
	src_addr = msg + 4 + MSG_HEADSIZE;

	if(ie_id == 6 && count == size)
	{
		memcpy((char *)(&rrusoftinfo[0]) + 4, (char *)src_addr, sizeof(CL_RRUSOFTINFO) - 4);
		/***********************************
		进行激活操作
		***********************************/
		msg_head.msg_id = CPRI_VERACT_ANS;
		msg_head.msg_size = MSG_HEADSIZE + 2 * rruveractans[0].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		rruveractans[0].soft_type = 0;	//设置激活应答的类型是软件
		rruveractans[0].res = flag[0];		//设置软件激活的结果
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&rruveractans[0]), sizeof(VA_RRUVERACTANS));
		rruveractans[0].soft_type = 1;	//设置激活应答的类型是固件
		rruveractans[0].res = flag[1];		//设置固件激活的结果
		memcpy(send_msg + MSG_HEADSIZE + rruveractans[0].ie_size, (char *)(&rruveractans[0]), sizeof(VA_RRUVERACTANS));
		send(sk, send_msg, msg_head.msg_size, 0);	//发送版本激活结果指示的消息体
		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri1_state_que(int sk, char *msg)
 * 功能描述：RRU状态查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的状态查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_state_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, ch, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;
	STATUS_S status;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;	//得到所有ie消息体的总大小
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_STATE_ANS;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 302:		//射频通道状态查询
				memcpy((char *)(&rfchsta[0]) + 4, (char *)src_addr, sizeof(SQ_RFCHSTA) - 4);	//取得查询请求的消息体内容
				msg_head.msg_size = MSG_HEADSIZE + rfchans[0].ie_size;
				status_read(rfchsta[0].ch_num/3, &status);
				rfchans[0].ul_sta = status.ad_work_state;
				rfchans[0].dl_sta = status.da_work_state;
				rfchans[0].ch_num = rfchsta[0].ch_num;
				//rfchans[0].ch_num = rfchsta[0].ch_num;		//设置要查询的射频通道号
				//rfchans[0].ul_sta += 0;		//设置上行通道状态响应，这里不需要设置，因为在参数配置时会进行设置它。
				//rfchans[0].dl_sta += 0;		//设置下行通道状态响应，这里不需要设置，因为在参数配置时会进行设置它。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchans[0]), sizeof(SQ_RFCHANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送射频通道状态查询响应消息体
				break;
			case 303:		//载波状态查询
				msg_head.msg_size = MSG_HEADSIZE + cirans[0].ie_size;
				cirans[0].ant_num += 0;		//设置天线组号，这里不需要设置，因为在参数配置时会进行设置它。
				cirans[0].cir_num = 1;		//设置载波号
				cirans[0].sta = rfchans[0].dl_sta;		//设置载波使能或非使能状态，通过读取射频通道状态来获取。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cirans[0]), sizeof(SQ_CIRANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送载波状态查询响应消息体
				break;
			case 304:		//本振状态查询
				msg_head.msg_size = MSG_HEADSIZE + oscans[0].ie_size;
				//对DA的本振状态查询
				status_read(0, &status);
				oscans[0].osc_fre = device_read(0, 0x06, 0);
				oscans[0].sta = status.da_clk_pll_state;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&oscans[0]), sizeof(SQ_OSCANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送本振状态查询响应消息体
				break;
			case 305:		//时钟状态查询
				msg_head.msg_size = MSG_HEADSIZE + rtcans[0].ie_size;
							//接口目前还没有提供
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rtcans[0]), sizeof(SQ_RTCANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送时钟状态查询响应消息体
				break;
			case 306:		//RRU运行状态查询
				msg_head.msg_size = MSG_HEADSIZE + runans[0].ie_size;
				runans[0].sta += 0;		//设置RRU运行状态，这里不需要设置，因为在通道建立配置和小区配置时会进行设置它。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&runans[0]), sizeof(SQ_RUNANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//RRU运行状态查询响应消息体
				break;
			case 307:		//Ir口工作模式查询
				msg_head.msg_size = MSG_HEADSIZE + irmodeans[0].ie_size;
				irmodeans[0].res += 0;	//Ir口工作模式目前固定为普通模式。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodeans[0]), sizeof(SQ_IRMODEANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//Ir口工作模式查询响应消息体
				break;
			case 308:		//初始化校准结果查询
				msg_head.msg_size = MSG_HEADSIZE + initchkans[0].ie_size;
							//5G没有初始化校准，所以应该没有初始化校准结果的查询
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&initchkans[0]), sizeof(SQ_INITCHKANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//初始化校准结果查询响应消息体
				break;
			case 309:		//光口信息查询
				msg_head.msg_size = MSG_HEADSIZE + rayans[0].ie_size;
							//没有找到哪里有对光口信息的设置，可能是默认初始化的，待确定
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rayans[0]), sizeof(SQ_RAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//光口信息查询响应消息体
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


/*
 * 函数名：void cpri1_get_systime(CL_SYSTIME *cl_systime)
 * 功能描述：系统时间获取函数。
 * input：
 * 		1、cl_systime，系统时间结构体的指针。
 * output：void
 */
void cpri1_get_systime(CL_SYSTIME *cl_systime)
{
	time_t t;
	struct tm area;

	//获取系统时间，并转换成struct tm结构体的格式
	t = time(NULL);
	localtime_r(&t, &area);

	//对系统时间结构体进行赋值
	cl_systime->year = area.tm_year + 1900;
	cl_systime->month = area.tm_mon;
	cl_systime->day = area.tm_mday;
	cl_systime->hour = area.tm_hour;
	cl_systime->min = area.tm_min;
	cl_systime->sec = area.tm_sec;
}


/*
 * 函数名：int cpri1_para_que(int sk, char *msg)
 * 功能描述：RRU参数查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_para_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;
	STATUS_S status;
	CPRI_STATUS_S cpri_status;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;	//得到所有ie消息体的总大小
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_PARA_ANS;

	for(i = 0; i < 9; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 401:		//系统时间查询
				cpri1_get_systime(&systime[0]);	//获取本地系统时间到系统时间查询响应结构体
				msg_head.msg_size = MSG_HEADSIZE + systime[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systime[0]), sizeof(CL_SYSTIME));
				send(sk, send_msg, msg_head.msg_size, 0);	//系统时间查询响应消息体
				break;
			case 402:		//CPU占用率查询
				msg_head.msg_size = MSG_HEADSIZE + cpurateans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);		//此功能已实现，开了一个定时器信号，每隔一段时间就运算出CPU占用率，并填写到cpurateans[0]结构体中
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&cpurateans[0]), sizeof(PQ_CPURATEANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//CPU占用率查询响应消息体
				break;
			case 403:		//CPU占用率统计周期查询
				msg_head.msg_size = MSG_HEADSIZE + ratecycans[0].ie_size;
				ratecycans[0].rate_cyc += 0;		//设置CPU占用率统计周期，这里不需要设置，因为在参数配置时已经对它进行了设置。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecycans[0]), sizeof(PQ_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//CPU占用率统计周期查询响应消息体
				break;
			case 404:		//RRU温度查询
				msg_head.msg_size = MSG_HEADSIZE + rrutemans[0].ie_size;
				rrutemans[0].tem_type = 0;		//设置温度查询类型为射频通道温度，因为只有这一个选项
				//FPGA的温度读取，射频通道温度待定
				status_read(0, &status);
				rrutemans[0].rfch_num = 0;
				rrutemans[0].tem_val = status.fpga_tempatrue;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rrutemans[0]), sizeof(PQ_RRUTEMANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//RRU温度查询响应消息体
				break;
			case 405:		//驻波比状态查询
				memcpy((char *)(&swrsta[0]) + 4, (char *)src_addr, sizeof(PQ_SWRSTA) - 4);	//取得查询请求的消息体内容
				msg_head.msg_size = MSG_HEADSIZE + swrstaans[0].ie_size;
				swrstaans[0].rfch_num = swrsta[0].rfch_num;
							//驻波比状态目前不用管
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrstaans[0]), sizeof(PQ_SWRSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//驻波比状态查询响应消息体
				break;
			case 406:		//驻波比门限查询
				msg_head.msg_size = MSG_HEADSIZE + swrthrans[0].ie_size;
				swrthrans[0].no1_thr += 0;		//设置驻波比一级门限，这里不需要设置，因为在参数配置时已经对它进行了设置。
				swrthrans[0].no2_thr += 0;		//设置驻波比二级门限，这里不需要设置，因为在参数配置时已经对它进行了设置。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrans[0]), sizeof(PQ_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//驻波比门限查询响应消息体
				break;
			case 407:		//过温门限查询
				msg_head.msg_size = MSG_HEADSIZE + temthrans[0].ie_size;
				temthrans[0].tem_max += 0;		//设置过温上门限，这里不需要设置，因为在参数配置时已经对它进行了设置。
				temthrans[0].tem_low += 0;		//设置过温下门限，这里不需要设置，因为在参数配置时已经对它进行了设置。
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrans[0]), sizeof(PQ_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//过温门限查询响应消息体
				break;
			case 408:		//输出功率查询
				memcpy((char *)(&outpower[0]) + 4, (char *)src_addr, sizeof(PQ_OUTPOWER) - 4);	//取得查询请求的消息体内容
				msg_head.msg_size = MSG_HEADSIZE + outpowerans[0].ie_size;
				outpowerans[0].rfch_num = outpower[0].rfch_num;
							//目前只有DA的增益设置，还没有增益的读取
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&outpowerans[0]), sizeof(PQ_OUTPOWERANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//输出功率查询响应消息体
				break;
			case 409:		//状态机查询
				msg_head.msg_size = MSG_HEADSIZE + stamachans[0].ie_size;
				//状态机查询应该是读取cpri状态来的
				cpri_status_read(0, 0, &cpri_status);
				stamachans[0].state = cpri_status.state;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&stamachans[0]), sizeof(PQ_STAMACHANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//状态机查询响应消息体
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


/*
 * 函数名：int cpri1_set_systime(PC_SYSTIME *cl_systime)
 * 功能描述：系统时间设置函数。
 * input：
 * 		1、cl_systime，系统时间结构体的指针。
 * output：
 * 		成功：0
 * 		失败：1
 */
int cpri1_set_systime(const PC_SYSTIME *cl_systime)
{
	struct timeval tv;
	struct tm area;

	//对系统时间结构体进行赋值
	area.tm_year = cl_systime->year - 1900;
	area.tm_mon = cl_systime->month;
	area.tm_mday = cl_systime->day;
	area.tm_hour = cl_systime->hour;
	area.tm_min = cl_systime->min;
	area.tm_sec = cl_systime->sec;

	//将tm结构体转化为一共有多少秒
	if((tv.tv_sec = mktime(&area)) < 0)
	{
		printf("11: mktime is error!\n");
		return 1;
	}else
		tv.tv_usec = 0;

	//设置系统时间
	if(settimeofday(&tv, NULL) < 0)
	{
		perror("11");
		return 1;
	}else
		return 0;
}


/*
 * 函数名：int cpri1_paracfg_que(int sk, char *msg)
 * 功能描述：RRU参数配置处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_paracfg_que(int sk, char *msg)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, i;
	int size = 0, ret = 0, count;
	MSG_HEAD msg_head;
	struct itimerval new_value;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;	//得到所有ie消息体的总大小
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_PARACFG_ANS;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 11:		//参数配置-系统时间
				memcpy((char *)(&systimecfg[0]) + 4, (char *)src_addr, sizeof(PC_SYSTIME) - 4);	//取得参数配置的消息体内容
				systimecfgans[0].res = cpri1_set_systime(&systimecfg[0]);		//调用set_systime函数来配置系统时间，成功返回0，失败返回1
				msg_head.msg_size = MSG_HEADSIZE + systimecfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&systimecfgans[0]), sizeof(PC_SYSTIMEANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 501:		//参数配置-IQ数据通道配置
				memcpy((char *)(&iqdatachcfg[0]) + 4, (char *)src_addr, sizeof(PC_IQDATACH) - 4);	//取得参数配置的消息体内容
				/************************************
				IQ数据通道配置，目前不知道怎么配，待定。所以配置结果也没有设置。
				************************************/
				msg_head.msg_size = MSG_HEADSIZE + iqdatachcfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&iqdatachcfgans[0]), sizeof(PC_IQDATACHANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 502:		//参数配置-CPU占用率统计周期配置
				memcpy((char *)(&ratecyccfg[0]) + 4, (char *)src_addr, sizeof(PC_RATECYC) - 4);	//取得参数配置的消息体内容
				ratecycans[0].rate_cyc = ratecyccfg[0].rate_cyc;		//这里将配置参数查询中CPU占用率统计周期的查询内容
				cpri_write_str(ratecycans, 10);					//将CPU占用率统计周期参数写入到RRU的信息记录文件中
				//CPU占用率统计周期配置，采用ratecycans[0].rate_cyc这个值去设置。
				new_value.it_interval.tv_sec = ratecycans[0].rate_cyc;
				new_value.it_interval.tv_usec = 0;
				if(setitimer(ITIMER_REAL, &new_value, NULL) == -1)		//正式设置统计周期，每隔此时间段便计算一次
				{
					//设置参数配置失败
					perror("502");
					ratecyccfgans[0].res = 1;
				}else
					ratecyccfgans[0].res = 0;
				msg_head.msg_size = MSG_HEADSIZE + ratecyccfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&ratecyccfgans[0]), sizeof(PC_RATECYCANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 503:		//参数配置-驻波比门限配置
				memcpy((char *)(&swrthrcfg[0]) + 4, (char *)src_addr, sizeof(PC_SWRTHR) - 4);	//取得参数配置的消息体内容
				swrthrans[0].no1_thr = swrthrcfg[0].no1_thr;		//这里将配置参数查询中驻波比门限的查询内容
				swrthrans[0].no2_thr = swrthrcfg[0].no2_thr;		//这里将配置参数查询中驻波比门限的查询内容
				/************************************
				驻波比门限配置，目前暂时不用管
				************************************/
				swrthrcfgans[0].res = 0;		//设置参数配置成功
				msg_head.msg_size = MSG_HEADSIZE + swrthrcfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&swrthrcfgans[0]), sizeof(PC_SWRTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 504:		//参数配置-Ir口工作模式配置
				memcpy((char *)(&irmodecfg[0]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 4);	//取得参数配置的消息体内容
				irmodeans[0].res = irmodecfg[0].ir_mode;		//这里将设置状态查询中ir口工作模式查询的内容
				/************************************
				Ir口工作模式配置，目前固定配置无普通模式
				************************************/
				irmodecfgans[0].res = 0;		//设置参数配置成功
				msg_head.msg_size = MSG_HEADSIZE + irmodecfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&irmodecfgans[0]), sizeof(CL_IRMODECFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 505:		//参数配置-过温门限配置
				memcpy((char *)(&temthrcfg[0]) + 4, (char *)src_addr, sizeof(PC_TEMTHR) - 4);	//取得参数配置的消息体内容
				temthrans[0].tem_max = temthrcfg[0].tem_max;		//这里将设置参数查询中过温门限查询的内容
				temthrans[0].tem_low = temthrcfg[0].tem_low;		//这里将设置参数查询中过温门限查询的内容
				/************************************
				过温门限配置，这里也不知道怎么配置
				************************************/
				temthrcfgans[0].tem_type = temthrcfg[0].tem_type;
				temthrcfgans[0].res = 0;		//设置参数配置成功
				msg_head.msg_size = MSG_HEADSIZE + temthrcfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&temthrcfgans[0]), sizeof(PC_TEMTHRANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 302:		//参数配置-射频通道状态配置
				memcpy((char *)(&rfchans[*(char *)src_addr - 1]) + 4, (char *)src_addr, sizeof(SQ_RFCHANS) - 4);	//取得参数配置的消息体内容
				//射频通道状态配置
				if(rfchans[*(char *)src_addr - 1].dl_sta != 0)	//射频通道使能
				{	
					da_recall_mode_set((*(char *)src_addr-1)/2, (*(char *)src_addr-1)%2, 0);
					rfchstacfgans[0].res = da_recall_enable((*(char *)src_addr-1)/2, (*(char *)src_addr-1)%2, 1);
				}else											//射频通道失能
				{
					da_recall_mode_set((*(char *)src_addr-1)/2, (*(char *)src_addr-1)%2, 0);
					rfchstacfgans[0].res = da_recall_enable((*(char *)src_addr-1)/2, (*(char *)src_addr-1)%2, 0);
				}
				rfchstacfgans[0].ch_num = *(char *)src_addr;
				msg_head.msg_size = MSG_HEADSIZE + rfchstacfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&rfchstacfgans[0]), sizeof(PC_RFCHSTAANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 507:		//参数配置-天线配置
				memcpy((char *)(&antcfg[0]) + 4, (char *)src_addr, sizeof(PC_ANTCFG) - 4);	//取得参数配置的消息体内容
				/************************************
				天线配置，这里也不知道怎么去配
				************************************/
				antcfgans[0].ant_num = antcfg[0].ant_num;
				antcfgans[0].res = 0;		//设置参数配置成功
				msg_head.msg_size = MSG_HEADSIZE + antcfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&antcfgans[0]), sizeof(PC_ANTCFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
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


/*
 * 函数名：int cpri1_delaymse_que(int sk, char *msg)
 * 功能描述：RRU时延测量处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_delaymse_que(int sk, char *msg)
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
				时延测量，先缓一缓
				************************************/
				msg_head.msg_id = CPRI_DELAYMSE_ANS;
				msg_head.msg_size = MSG_HEADSIZE + raydelayans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&raydelayans[0]), sizeof(DM_RAYDELAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 921:
				/************************************
				时延配置，先缓一缓
				************************************/
				msg_head.msg_id = CPRI_DELAYCFG_ANS;
				msg_head.msg_size = MSG_HEADSIZE + delaycfgans[0].ie_size;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&delaycfgans[0]), sizeof(DM_DELAYCFGANS));
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


/*
 * 函数名：void cpri1_get_ala(AR_ALAQUE *ala_que, AR_ALAREP *ala_rep)
 * 功能描述：针对查询指定的告警码是否被置位的功能函数。
 * input：
 * 		1、ala_que，告警查询请求消息体；
 * 		2、ala_rep，告警查询应答消息体；
 * output：void
 */
void cpri1_get_ala(AR_ALAQUE *ala_que, AR_ALAREP *ala_rep)
{
	int i = 0;
	time_t t;
	struct tm *area;

	//获取系统时间，并转换成struct tm结构体的格式
	t = time(NULL);
	area = localtime(&t);

	for(i = 0; i < 23; i++)
	{
		if(ala_que->ala_code == 0)
		{

		}else
		{
			//判断请求的告警码是否有效，即此告警码是否存在
			if(ala_que->ala_code == *(unsigned int *)((char *)(&alaexl[0]) + i*4))
			{
				ala_rep->ala_able = 0;		//告警码存在

				//告警码是否已经被置位
				if((cpri1_ala_flag & (1 << i)) != 0)
				{
					ala_rep->ala_flag = 0;	//已经被置位
				}else
				{
					ala_rep->ala_flag = 1;
				}
				break;
			}else
			{
				ala_rep->ala_able = 1;
			}
		}
	}

	//获取时间戳，包括年月日时分秒
	sprintf(ala_rep->time_stamp, "%d%02d%02d%02d%02d%02d", area->tm_year + 1900, area->tm_mon, area->tm_mday, area->tm_hour, area->tm_min, area->tm_sec);
	ala_rep->ala_code = ala_que->ala_code;
	ala_rep->ala_subcode = ala_que->ala_subcode;
	//ala_rep->addi_data = "";
}


/*
 * 函数名：int cpri1_ala_que(int sk, char *msg)
 * 功能描述：RRU报警查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_ala_que(int sk, char *msg)
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
		memcpy((char *)(&alaque[0]) + 4, (char *)src_addr, sizeof(AR_ALAQUE) - 4);	//取得参数配置的消息体内容
		cpri1_get_ala(&alaque[0], &alarep[0]);		//查询告警号是否存在，并且判断其是否被置位
		msg_head.msg_id = CPRI_ALA_ANS;
		msg_head.msg_size = MSG_HEADSIZE + alarep[0].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&alarep[0]), sizeof(AR_ALAREP));
		send(sk, send_msg, msg_head.msg_size, 0);

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri1_logup_que(int sk, char *msg)
 * 功能描述：RRU日志上传请求处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_logup_que(int sk, char *msg)
{
	char *src_addr, send_msg[512], server_file[220];
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
		memcpy((char *)(&upque[0]) + 4, (char *)src_addr, sizeof(LOG_UPQUE) - 4);	//取得参数配置的消息体内容
		//日志上传请求，这里判断日志是否存在
		upans[0].log_type = upque[0].log_type;
		if(upans[0].log_type == 0)
			upans[0].res = access("ala.txt", F_OK);
		else if(upque[0].log_type == 1)
			upans[0].res = access("usr.txt", F_OK);
		else
			upans[0].res = access("sys.txt", F_OK);
		if(upans[0].res == -1)
			upans[0].res = 1;
		msg_head.msg_id = CPRI_LOGUP_ANS;
		msg_head.msg_size = MSG_HEADSIZE + upans[0].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&upans[0]), sizeof(LOG_UPANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		if(upans[0].res == 0)	//如果日志文件存在，开始上传
		{
			upres[0].log_type = upque[0].log_type;
			//进行ftp上传日志，然后记录上传结果
			strcat(server_file, upque[0].bbu_path);
			strcat(server_file, upque[0].bbu_file);
			if(upres[0].log_type == 0)
				upres[0].res = ftp_up(ETH0, linkaddr[0].bbu_ip, "ala.txt", server_file, 0);
			else if(upque[0].log_type == 1)
				upres[0].res = ftp_up(ETH0, linkaddr[0].bbu_ip, "usr.txt", server_file, 0);
			else
				upres[0].res = ftp_up(ETH0, linkaddr[0].bbu_ip, "sys.txt", server_file, 0);
			msg_head.msg_id = CPRI_LOGUP_IND;
			msg_head.msg_size = MSG_HEADSIZE + upres[0].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&upres[0]), sizeof(LOG_UPRES));
			send(sk, send_msg, msg_head.msg_size, 0);
		}

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri1_reset_ind(int sk, char *msg)
 * 功能描述：RRU复位指示处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_reset_ind(int sk, char *msg)
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
		进行RRU的复位操作，这里的复位是指整机复位还是单个cpri口进行复位？
		*******************************/

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri1_bbubeat_msg(int sk, char *msg, int *num)
 * 功能描述：RRU在位检测处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_bbubeat_msg(int sk, char *msg, int *num)
{
	int ret = 0;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_RRUBEAT_MSG;
	msg_head.msg_size = MSG_HEADSIZE;

	//清零num
	*num = 0;

	ret = send(sk, &msg_head, msg_head.msg_size, 0);
	
	return ret;
}


/*
 * 函数名：int cpri1_lte_cfg(int sk, char *msg)
 * 功能描述：RRU小区配置处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri1_lte_cfg(int sk, char *msg)
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
int cpri1_write_str(char *msg, int i)
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


