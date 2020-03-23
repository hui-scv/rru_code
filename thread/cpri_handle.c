/*
 * 文件名：cpri_handle.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：cpri接口各个消息体的实际处理函数合集。
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>

#include "thread/cpri_handle.h"
#include "thread/idle_thread.h"
#include "struct.h"
#include "ftp_client.h"
#include "usr.h"



extern unsigned int cpri_ala_flag[8];
extern char *eth_name[8];

/*
 * 函数名：int cpri_comch_req(int sk, BBU_HEAD cpri_ans)
 * 功能描述：cpri向BBU端口发送所有关于通道建立请求的消息结构体。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、cpri_ans，BBU应答RRU的广播信息结构体。
 *		3、cpri_num，cpri的接口号。
 * output：
 *		ret：发送数据的大小
 */
int cpri_comch_req(int sk, const BBU_HEAD cpri_ans, const int cpri_num)
{
	char send_msg[512];
	int count = 0, ret = 0;
	MSG_HEAD msg_head;

	//初始化通道建立请求的消息头
	msg_head.msg_id = CPRI_CHLINK_QUE;
	msg_head.msg_size = 354;
	msg_head.bbu_id = cpri_ans.bbu_id;
	msg_head.rru_id = cpri_ans.rru_id;
	msg_head.ray_id = cpri_ans.bbu_num;
	msg_head.inc_num = 1;

	//将消息头和通道建立请求的所有消息体都拷贝到send_msg数组中
	memcpy(send_msg + count, &msg_head, sizeof(MSG_HEAD));
	count += sizeof(MSG_HEAD);
	memcpy(send_msg + count, &porid[cpri_num], sizeof(CL_PROID));
	count += sizeof(CL_PROID);
	memcpy(send_msg + count, &linktype[cpri_num], sizeof(CL_LINKTYPE));
	count += sizeof(CL_LINKTYPE);
	memcpy(send_msg + count, &rrucapa[cpri_num], sizeof(CL_RRUCAPA));
	count += sizeof(CL_RRUCAPA);
	memcpy(send_msg + count, &rrulv[cpri_num], sizeof(CL_RRULV));
	count += sizeof(CL_RRULV);
	memcpy(send_msg + count, &rruinfo[cpri_num], sizeof(CL_RRUINFO));
	count += sizeof(CL_RRUINFO);
	memcpy(send_msg + count, &rrusoftinfo[cpri_num], sizeof(CL_RRUSOFTINFO));
	count += sizeof(CL_RRUSOFTINFO);
	memcpy(send_msg + count, &rrufre[cpri_num], sizeof(CL_RRUFRE));
	count += sizeof(CL_RRUFRE);
	memcpy(send_msg + count, &rrurf[cpri_num], sizeof(CL_RRURF));
	count += sizeof(CL_RRURF);
	memcpy(send_msg + count, &rrucir[cpri_num], sizeof(CL_RRUCIR));
	count += sizeof(CL_RRUCIR);
	
	//发送给BBU端口
	ret = send(sk, send_msg, count, 0);
	if(ret < 0)
	{
		perror(eth_name[cpri_num]);
	}
	//printf("ret: %d, count: %d\n", ret, count);
	return ret;
}


/*
 * 函数名：int cpri_comch_cfg(int sk, char *msg)
 * 功能描述：cpri进行通道建立配置的操作。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，BBU端口应答cpri的通道建立配置消息体。
 *		3、cpri_num，cpri的接口号。
 * output：
 *		失败：-1；
 *		成功：0
 */
int cpri_comch_cfg(int sk, char *msg, const int cpri_num)
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
	msg_head.msg_id = CPRI_CHLINK_ANS;
	msg_head.msg_size = MSG_HEADSIZE;

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
				memcpy((char *)(&systime[cpri_num]) + 4, (char *)src_addr, sizeof(CL_SYSTIME) - 4);
				ret = cpri_set_systime((PC_SYSTIME *)&systime[0]);
				break;
			case 12:	//保存BBU侧的FTP服务器的地址信息
				memcpy((char *)(&linkaddr[cpri_num]) + 4, (char *)src_addr, sizeof(CL_LINKADDR) - 4);
				break;
			case 13:	//RRU操作模式设置，目前固定为正常模式
				memcpy((char *)(&rrumode[cpri_num]) + 4, (char *)src_addr, sizeof(CL_RRUMODE) - 4);
				if(rrumode[cpri_num].mode == 1)		//设置为测试模式
				{
					runans[cpri_num].sta = 1;
#ifdef PPC
					ret = da_recall_mode_set(cpri_num/4, (cpri_num - (cpri_num/4)*4)/2, 1);
#endif
				}
				break;
			case 14:	//软件版本核对结果，这个结果是由BBU得出的
				if(*(char *)src_addr == 0)	//将软件的版本核对结果赋值
					memcpy((char *)(&softchk[cpri_num]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				else						//将固件的版本核对结果赋值
					memcpy((char *)(&bioschk[cpri_num]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
				break;
			case 504:	//Ir口工作模式，目前固定为普通模式（Ir红外接口）
				//完成Ir口工作模式配置并应答BBU
				memcpy((char *)(&irmodecfg[cpri_num]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 
4);
				irmodeans[cpri_num].res = irmodecfg[cpri_num].ir_mode;		//这里将设置状态查询中ir口工作模式查询的内容
				irmodecfgans[cpri_num].res = 0;		//设置参数配置成功
				memcpy(send_msg + msg_head.msg_size, (char *)(&irmodecfgans[cpri_num]), sizeof(CL_IRMODECFGANS));
				msg_head.msg_size += irmodecfgans[cpri_num].ie_size;
				break;
			case 505:	//保存FTP的用户名和密码
				memcpy((char *)(&ftpinfo[cpri_num]) + 4, (char *)src_addr, sizeof(CL_FTPINFO) - 4);
				break;
			default:
				ret = -1;
				//return ret;
		}

		if(size == count || ret != 0)
			break;
	}

	//当消息头中的ie总大小和实际计算出的ie总大小相等并且每个IE都配置成功时，则通道建立配置成功
	if(size == count && ret == 0)
	{
		chlinkans[cpri_num].res = 0;
		memcpy(send_msg + msg_head.msg_size, (char *)(&chlinkans[cpri_num]), sizeof(CL_CHLINKANS));
		msg_head.msg_size += chlinkans[cpri_num].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		send(sk, send_msg, msg_head.msg_size, 0);
		return 0;
	}else		//通道建立配置失败
	{
		chlinkans[cpri_num].res = 1;
		memcpy(send_msg + msg_head.msg_size, (char *)(&chlinkans[cpri_num]), sizeof(CL_CHLINKANS));
		msg_head.msg_size += chlinkans[cpri_num].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		send(sk, send_msg, msg_head.msg_size, 0);
		return -1;
	}
}


/*
 * 函数名：void cpri_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans)
 * 功能描述：cpri将不断向BBU端口发送通信链路建立请求消息，直到BBU正确应答。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，用于接受BBU端口回复的信息；
 * 		3、msg_head，消息头结构体；
 * 		4、cpri_ans，BBU应答RRU的广播信息结构体。
 *		5、cpri_num，cpri的接口号。
 * output：void
 */
void cpri_comch_init(int sk, char *msg, const BBU_HEAD cpri_ans, const int cpri_num)
{
	char send_msg[512], server_file[220];
	int ret = 0, rec_num = 0;
	fd_set rdfds;
	struct timeval tv;
	MSG_HEAD msg_head;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memset(server_file, 0, sizeof(char)*220);
	//cpri发送通信链路建立的消息集合请求
	cpri_comch_req(sk, cpri_ans, cpri_num);

	while(1)
	{
		FD_ZERO(&rdfds);		//这个必须写到这里，重复调用select需要重新设置检测
		FD_SET(sk, &rdfds);		//的文件描述符的集合
		//接收BBU返回的通道建立配置信息，等待10s，若没有接收到则超时
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		ret = select(sk + 1, &rdfds, NULL, NULL, &tv);
		if(ret < 0)
			perror(eth_name[cpri_num]);
		else if(ret == 0)
		{
			//当select返回0时，说明等待超时，此时就重新发送通道建立请求
			printf("cpri%d chlink time out!\n", cpri_num+1);
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
			cpri_comch_req(sk, cpri_ans, cpri_num);
			continue;
 		}else
			rec_num += recv(sk, msg + rec_num, 512, 0);		//等到应答后，进行接收数据。数据有可能不是1包发完，所以需要进行+=，累积之前接收的数据

		//当接收的数据总大小和返回的消息头中指明数据的大小相等之后，说明数据接收完毕
		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			//然后再判断是否是通道建立配置消息体
			if(((MSG_HEAD *)msg)->msg_id == CPRI_CHLINK_CFG)
			{
				memcpy((char *)&msg_head, (char *)msg, MSG_HEADSIZE);
				//进行通信通道建立配置处理
				ret = cpri_comch_cfg(sk, msg, cpri_num);
				//返回配置响应
				if(ret >= 0)
				{
					if(softchk[cpri_num].res != 0)	//得出版本不一致，需要更新软件
					{
						while(1)
						{
							//strcat(server_file, softchk[cpri_num].file_path);
							//strcat(server_file, softchk[cpri_num].file_name);
							sprintf(server_file, "%s/%s", softchk[cpri_num].file_path, softchk[cpri_num].file_name);
							ret = ftp_down(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, softchk[cpri_num].file_name, server_file, cpri_num);
							verupdata[cpri_num].soft_type = softchk[cpri_num].soft_type;
							if(ret == 0)
								verupdata[cpri_num].res = 0;		//文件下载成功
							else if(ret == -5)
								verupdata[cpri_num].res = 1;		//文件不存在
							else if(ret == -8)
								verupdata[cpri_num].res = 2;		//文件下载超时
							else if(ret == -9)
								verupdata[cpri_num].res = 3;		//文件过大
							else
								verupdata[cpri_num].res = 4;		//其它原因

							msg_head.msg_id = CPRI_VERUP_IND;
							msg_head.msg_size = MSG_HEADSIZE + softchk[cpri_num].ie_size;
							memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
							memcpy(send_msg + MSG_HEADSIZE, (char *)(&softchk[cpri_num]), sizeof(CL_SOFTCHK));
							send(sk, send_msg, msg_head.msg_size, 0);
							if(verupdata[cpri_num].res == 0)
								break;
							else
								sleep(1);
						}
					}
					if(bioschk[cpri_num].res != 0)	//得出版本不一致，需要更新固件
					{
						while(1)
						{
							//strcat(server_file, bioschk[cpri_num].file_path);
							//strcat(server_file, bioschk[cpri_num].file_name);
							sprintf(server_file, "%s/%s", bioschk[cpri_num].file_path, bioschk[cpri_num].file_name);
							ret = ftp_down(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, bioschk[cpri_num].file_name, server_file, cpri_num);
							verupdata[cpri_num].soft_type = bioschk[cpri_num].soft_type;
							if(ret == 0)
								verupdata[cpri_num].res = 0;		//文件下载成功
							else if(ret == -5)
								verupdata[cpri_num].res = 1;		//文件不存在
							else if(ret == -8)
								verupdata[cpri_num].res = 2;		//文件下载超时
							else if(ret == -9)
								verupdata[cpri_num].res = 3;		//文件过大
							else
								verupdata[cpri_num].res = 4;		//其它原因

							msg_head.msg_id = CPRI_VERUP_IND;
							msg_head.msg_size = MSG_HEADSIZE + bioschk[cpri_num].ie_size;
							memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
							memcpy(send_msg + MSG_HEADSIZE, (char *)(&bioschk[cpri_num]), sizeof(CL_SOFTCHK));
							send(sk, send_msg, msg_head.msg_size, 0);
							if(verupdata[cpri_num].res == 0)
								break;
							else
								sleep(1);
						}
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

				//发送配置失败的响应，并重新请求建立
				msg_head.msg_id = CPRI_CHLINK_ANS;
				msg_head.msg_size = MSG_HEADSIZE + chlinkans[cpri_num].ie_size;
				chlinkans[cpri_num].res = 1;		//设置通道建立配置应答的结果为失败
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&chlinkans[cpri_num]), sizeof(CL_CHLINKANS));
				send(sk, send_msg, msg_head.msg_size, 0);
			}
			
			cpri_comch_req(sk, cpri_ans, cpri_num);
		}
	}
}


/*
 * 函数名：int cpri_verdown_que(int sk, char *msg)
 * 功能描述：RRU版本下载请求处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、软件版本一致，不需要下载；
 * 		1、软件版本不一致，需要下载；
 * 		-1、接收到的消息体有误。
 */
int cpri_verdown_que(int sk, char *msg, const int cpri_num)
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
		memcpy((char *)(&softchk[cpri_num]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		/*if(*(char *)src_addr == 0)	//将软件的版本核对结果赋值
			memcpy((char *)(&softchk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		else						//将固件的版本核对结果赋值
			memcpy((char *)(&bioschk[0]) + 4, (char *)src_addr, sizeof(CL_SOFTCHK) - 4);
		break;*/
		if(softchk[cpri_num].res == 0)		//判断接收到的版本下载请求ie，看软件版本是否一致
		{
			verdownans[cpri_num].soft_type = softchk[cpri_num].soft_type;
			verdownans[cpri_num].res = 1;		//拒绝版本下载，在这里给出拒绝原因，因为软件版本一致
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[cpri_num].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[cpri_num]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载应答消息体
			ret = 0;
		}else
		{
			verdownans[cpri_num].soft_type = softchk[cpri_num].soft_type;
			verdownans[cpri_num].res = 0;		//可以进行版本下载
			msg_head.msg_id = CPRI_VERDOWN_ANS;
			msg_head.msg_size = MSG_HEADSIZE + verdownans[cpri_num].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownans[cpri_num]), sizeof(VD_VERDOWNANS));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载应答消息体

			//进行ftp下载文件
			//strcat(server_file, softchk[cpri_num].file_path);
			//strcat(server_file, softchk[cpri_num].file_name);
			sprintf(server_file, "%s/%s", softchk[cpri_num].file_path, softchk[cpri_num].file_name);
			ret = ftp_down(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, softchk[cpri_num].file_name, server_file, 0);
			verdownres[cpri_num].soft_type = softchk[cpri_num].soft_type;
			if(ret == 0)
				verdownres[cpri_num].res = 0;		//文件下载成功
			else if(ret == -5)
				verdownres[cpri_num].res = 1;		//文件不存在
			else if(ret == -8)
				verdownres[cpri_num].res = 2;		//文件下载超时
			else if(ret == -9)
				verdownres[cpri_num].res = 3;		//文件过大
			else
				verdownres[cpri_num].res = 4;		//其它原因
								
			msg_head.msg_id = CPRI_VERDOWN_IND;
			msg_head.msg_size = MSG_HEADSIZE + verdownres[cpri_num].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&verdownres[cpri_num]), sizeof(VD_VERDOWNRES));
			send(sk, send_msg, msg_head.msg_size, 0);	//发送版本下载结果指示的消息体
			ret = 1;
		}
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri_veract_ind(int sk, char *msg)
 * 功能描述：RRU版本激活指示处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的激活操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_veract_ind(int sk, char *msg, const int cpri_num)
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
		memcpy((char *)(&rrusoftinfo[cpri_num]) + 4, (char *)src_addr, sizeof(CL_RRUSOFTINFO) - 4);
		/***********************************
		进行激活操作
		***********************************/
		msg_head.msg_id = CPRI_VERACT_ANS;
		msg_head.msg_size = MSG_HEADSIZE + 2 * rruveractans[cpri_num].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		rruveractans[cpri_num].soft_type = 0;	//设置激活应答的类型是软件
		rruveractans[cpri_num].res = flag[0];		//设置软件激活的结果
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&rruveractans[cpri_num]), sizeof(VA_RRUVERACTANS));
		rruveractans[cpri_num].soft_type = 1;	//设置激活应答的类型是固件
		rruveractans[cpri_num].res = flag[1];		//设置固件激活的结果
		memcpy(send_msg + MSG_HEADSIZE + rruveractans[cpri_num].ie_size, (char *)(&rruveractans[cpri_num]), sizeof(VA_RRUVERACTANS));
		send(sk, send_msg, msg_head.msg_size, 0);	//发送版本激活结果指示的消息体
		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri_state_que(int sk, char *msg)
 * 功能描述：RRU状态查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的状态查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_state_que(int sk, char *msg, const int cpri_num)
{
	char *src_addr, send_msg[512];
	unsigned short ie_id, ch, i;
	int size = 0, ret = 0, count, ad_val, da_val;
	MSG_HEAD msg_head;
	STATUS_S status;
	AD_ENABLE_S ad_en_sta;
	DA_ENABLE_S da_en_sta;

	memset(&msg_head, 0, sizeof(MSG_HEAD));
	count = *(unsigned int *)(msg + 4) - MSG_HEADSIZE;	//得到所有ie消息体的总大小
	memcpy(&msg_head, (MSG_HEAD *)msg, MSG_HEADSIZE);
	msg_head.msg_id = CPRI_STATE_ANS;
	msg_head.msg_size = MSG_HEADSIZE;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 302:		//射频通道状态查询
				memcpy((char *)(&rfchsta[cpri_num]) + 4, (char *)src_addr, sizeof(SQ_RFCHSTA) - 4);	//取得查询请求的消息体内容
#ifdef PPC
				status_read(cpri_num/4, &status);	//等待给出使能读取接口
				ad_enable_read(cpri_num/4, &ad_en_sta);
				da_enable_read(cpri_num/4, &da_en_sta);
				ad_val = (int)ad_en_sta.ch1_en;
				da_val = (int)da_en_sta.ch1_child1_en;
#endif
				if((ad_val & (0x01<<(cpri_num%4))) == 0)		//上行通道未使能
				{
					rfchans[cpri_num].ul_sta = 0;
				}
				else		//上行通道已使能
				{
					if(status.ad_work_state == 1)
						rfchans[cpri_num].ul_sta = 1;	//上行通道无故障
					else
						rfchans[cpri_num].ul_sta = 2;	//上行通道有故障
				}
				
				if((da_val & (0x01<<(cpri_num%4))) == 0)		//下行通道未使能
				{
					rfchans[cpri_num].dl_sta = 0;
				}
				else		//下行通道已使能
				{
					if(status.da_work_state == 1)
						rfchans[cpri_num].dl_sta = 1;	//下行通道无故障
					else
						rfchans[cpri_num].dl_sta = 2;	//下行通道有故障
				}
				rfchans[cpri_num].ch_num = rfchsta[cpri_num].ch_num;	//设置要查询的射频通道号
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&rfchans[cpri_num]), sizeof(SQ_RFCHANS));
				msg_head.msg_size += rfchans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送射频通道状态查询响应消息体
				break;
			case 303:		//载波状态查询
				cirans[cpri_num].ant_num = 1;		//设置天线组号，因为1个ip对应1个通道，所以设为1。
				cirans[cpri_num].cir_num = 1;		//设置载波号，因为1个ip对应1个通道，所以设为1。
				if(rfchans[cpri_num].dl_sta == 0)
					cirans[cpri_num].sta = 1;		//设置载波非使能，通过读取射频通道状态来获取。
				else
					cirans[cpri_num].sta = 0;		//设置载波使能，通过读取射频通道状态来获取。
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&cirans[cpri_num]), sizeof(SQ_CIRANS));
				msg_head.msg_size += cirans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送载波状态查询响应消息体
				break;
			case 304:		//本振状态查询
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
#ifdef PPC
				//对DA的本振状态查询
				status_read(cpri_num/4, &status);
				oscans[cpri_num].osc_fre = device_read(cpri_num/4, 0x06, 0);
#endif
				oscans[cpri_num].sta = ~status.da_clk_pll_state;
				memcpy(send_msg + msg_head.msg_size, (char *)(&oscans[cpri_num]), sizeof(SQ_OSCANS));
				msg_head.msg_size += oscans[cpri_num].ie_size;
#ifdef PPC
				//对AD的本振状态查询
				status_read(cpri_num/4, &status);
				oscans[cpri_num].osc_fre = device_read(cpri_num/4, 0x04, 0);
#endif
				oscans[cpri_num].sta = ~status.ad_clk_pll_state;
				memcpy(send_msg + msg_head.msg_size, (char *)(&oscans[cpri_num]), sizeof(SQ_OSCANS));
				msg_head.msg_size += oscans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送本振状态查询响应消息体
				break;
			case 305:		//时钟状态查询
				//默认是失步状态，当设置参考时钟时，再改变它
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&rtcans[cpri_num]), sizeof(SQ_RTCANS));
				msg_head.msg_size += rtcans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送时钟状态查询响应消息体
				break;
			case 306:		//RRU运行状态查询
				//设置RRU运行状态，这里不需要设置，因为在通道建立配置和小区配置时会设置它。
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&runans[cpri_num]), sizeof(SQ_RUNANS));
				msg_head.msg_size += runans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//RRU运行状态查询响应消息体
				break;
			case 307:		//Ir口工作模式查询
				irmodeans[cpri_num].res = 1;	//Ir口工作模式目前固定为普通模式。
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&irmodeans[cpri_num]), sizeof(SQ_IRMODEANS));
				msg_head.msg_size += irmodeans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//Ir口工作模式查询响应消息体
				break;
			case 308:		//初始化校准结果查询
				initchkans[cpri_num].tr_num = 1;
				initchkans[cpri_num].rec_num = 1;
#ifdef PPC
				//对接收发送通道PLL的锁定状态查询
				status_read(cpri_num/4, &status);
#endif			
				if(status.da_clk_pll_state && status.da_work_state)
					initchkans[cpri_num].trchk_sta = 0;
				else
					initchkans[cpri_num].trchk_sta = 1;	
				if(status.ad_clk_pll_state && status.ad_work_state)
					initchkans[cpri_num].recchk_sta = 0;
				else
					initchkans[cpri_num].recchk_sta = 1;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&initchkans[cpri_num]), sizeof(SQ_INITCHKANS));
				msg_head.msg_size += initchkans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//初始化校准结果查询响应消息体
				break;
			case 309:		//光口信息查询
							//没有找到哪里有对光口信息的设置，可能是默认初始化的，待确定
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&rayans[cpri_num]), sizeof(SQ_RAYANS));
				msg_head.msg_size += rayans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//光口信息查询响应消息体
				break;
			default:
				ret = -1;
		}
		
		if(size == count || ret != 0)
			break;
	}

	memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
	send(sk, send_msg, msg_head.msg_size, 0);
	
	if(size == count && ret == 0)
	{
		return 0;
	}else
	{
		return -1;
	}
}


/*
 * 函数名：void cpri_get_systime(CL_SYSTIME *cl_systime)
 * 功能描述：系统时间获取函数。
 * input：
 * 		1、cl_systime，系统时间结构体的指针。
 * output：void
 */
void cpri_get_systime(CL_SYSTIME *cl_systime)
{
	time_t t;
	struct tm area;

	//获取系统时间，并转换成struct tm结构体的格式
	t = time(NULL);
	localtime_r(&t, &area);

	//对系统时间结构体进行赋值
	cl_systime->year = area.tm_year + 1900;
	cl_systime->month = area.tm_mon + 1;
	cl_systime->day = area.tm_mday;
	cl_systime->hour = area.tm_hour;
	cl_systime->min = area.tm_min;
	cl_systime->sec = area.tm_sec;
}


/*
 * 函数名：int cpri_para_que(int sk, char *msg)
 * 功能描述：RRU参数查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_para_que(int sk, char *msg, const cpri_num)
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
	msg_head.msg_size = MSG_HEADSIZE;

	for(i = 0; i < 9; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 401:		//系统时间查询
				cpri_get_systime(&systime[cpri_num]);	//获取本地系统时间到系统时间查询响应结构体
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&systime[cpri_num]), sizeof(CL_SYSTIME));
				msg_head.msg_size += systime[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//系统时间查询响应消息体
				break;
			case 402:		//CPU占用率查询
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);		//此功能已实现，开了一个定时器信号，每隔一段时间就运算出CPU占用率，并填写到cpurateans[0]结构体中
				memcpy(send_msg + msg_head.msg_size, (char *)(&cpurateans[0]), sizeof(PQ_CPURATEANS));
				msg_head.msg_size += cpurateans[0].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//CPU占用率查询响应消息体
				break;
			case 403:		//CPU占用率统计周期查询
				//ratecycans[0].rate_cyc += 0;		//设置CPU占用率统计周期，这里不需要设置，因为在参数配置时已经对它进行了设置。
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&ratecycans[0]), sizeof(PQ_RATECYCANS));
				msg_head.msg_size += ratecycans[0].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//CPU占用率统计周期查询响应消息体
				break;
			case 404:		//RRU温度查询
				rrutemans[cpri_num].tem_type = 0;		//设置温度查询类型为射频通道温度，因为只有这一个选项
#ifdef PPC
				//FPGA的温度读取，射频通道温度待定
				status_read(cpri_num/4, &status);
#endif
				rrutemans[cpri_num].rfch_num = 1;
				rrutemans[cpri_num].tem_val = status.fpga_tempatrue;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&rrutemans[cpri_num]), sizeof(PQ_RRUTEMANS));
				msg_head.msg_size += rrutemans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//RRU温度查询响应消息体
				break;
			case 405:		//驻波比状态查询
				memcpy((char *)(&swrsta[cpri_num]) + 4, (char *)src_addr, sizeof(PQ_SWRSTA) - 4);	//取得查询请求的消息体内容
				swrstaans[cpri_num].rfch_num = swrsta[cpri_num].rfch_num;
				swrstaans[cpri_num].swr_val = 15;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&swrstaans[cpri_num]), sizeof(PQ_SWRSTAANS));
				msg_head.msg_size += swrstaans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//驻波比状态查询响应消息体
				break;
			case 406:		//驻波比门限查询
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&swrthrans[cpri_num]), sizeof(PQ_SWRTHRANS));
				msg_head.msg_size += swrthrans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//驻波比门限查询响应消息体
				break;
			case 407:		//过温门限查询
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&temthrans[cpri_num]), sizeof(PQ_TEMTHRANS));
				msg_head.msg_size += temthrans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//过温门限查询响应消息体
				break;
			case 408:		//输出功率查询
				memcpy((char *)(&outpower[cpri_num]) + 4, (char *)src_addr, sizeof(PQ_OUTPOWER) - 4);	//取得查询请求的消息体内容
				outpowerans[cpri_num].rfch_num = outpower[cpri_num].rfch_num;
				//根据小区配置的输出功率来回复
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&outpowerans[cpri_num]), sizeof(PQ_OUTPOWERANS));
				msg_head.msg_size += outpowerans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//输出功率查询响应消息体
				break;
			case 409:		//状态机查询
#ifdef PPC
				//状态机查询应该就是读取cpri状态
				cpri_status_read(0, 0, &cpri_status);
#endif
				stamachans[cpri_num].state = cpri_status.state;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&stamachans[cpri_num]), sizeof(PQ_STAMACHANS));
				msg_head.msg_size += stamachans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//状态机查询响应消息体
				break;
			default:
				ret = -1;
		}
		
		if(size == count || ret != 0)
			break;
	}
	
	memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
	send(sk, send_msg, msg_head.msg_size, 0);
	
	if(size == count && ret == 0)
	{
		return 0;
	}else
	{
		return -1;
	}
}


/*
 * 函数名：int cpri_set_systime(PC_SYSTIME *cl_systime)
 * 功能描述：系统时间设置函数。
 * input：
 * 		1、cl_systime，系统时间结构体的指针。
 * output：
 * 		成功：0
 * 		失败：1
 */
int cpri_set_systime(PC_SYSTIME *cl_systime)
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
 * 函数名：int cpri_paracfg_que(int sk, char *msg)
 * 功能描述：RRU参数配置处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_paracfg_que(int sk, char *msg, const int cpri_num)
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
	msg_head.msg_size = MSG_HEADSIZE;

	for(i = 0; i < 8; i++)
	{
		ie_id = *(unsigned short *)(msg + size + MSG_HEADSIZE);
		src_addr = msg + size + 4 + MSG_HEADSIZE;
		size += *(unsigned short *)(msg + size + 2 + MSG_HEADSIZE);	//累计已经读取出的ie的长度

		switch(ie_id)
		{
			case 11:		//参数配置-系统时间
				memcpy((char *)(&systimecfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_SYSTIME) - 4);	//取得参数配置的消息体内容
				systimecfgans[cpri_num].res = cpri_set_systime(&systimecfg[cpri_num]);		//调用set_systime函数来配置系统时间，成功返回0，失败返回1
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&systimecfgans[cpri_num]), sizeof(PC_SYSTIMEANS));
				msg_head.msg_size += systimecfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 501:		//参数配置-IQ数据通道配置
				memcpy((char *)(&iqdatachcfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_IQDATACH) - 4);	//取得参数配置的消息体内容
				iqdatachcfgans[cpri_num].ray_num = iqdatachcfg[cpri_num].ray_num;
				iqdatachcfgans[cpri_num].cir_num = iqdatachcfg[cpri_num].cir_num;
				iqdatachcfgans[cpri_num].ant_num =  iqdatachcfg[cpri_num].ant_num;
				iqdatachcfgans[cpri_num].res = 0;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&iqdatachcfgans[cpri_num]), sizeof(PC_IQDATACHANS));
				msg_head.msg_size += iqdatachcfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 502:		//参数配置-CPU占用率统计周期配置
				memcpy((char *)(&ratecyccfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_RATECYC) - 4);	//取得参数配置的消息体内容
				//CPU占用率统计周期配置，采用ratecyccfg[0].rate_cyc这个值去设置。
				new_value.it_interval.tv_sec = ratecyccfg[0].rate_cyc;
				new_value.it_interval.tv_usec = 0;
				if(setitimer(ITIMER_REAL, &new_value, NULL) == -1)		//正式设置统计周期，每隔此时间段便计算一次
				{
					//设置参数配置失败
					perror(eth_name[cpri_num]);
					ratecyccfgans[cpri_num].res = 1;
				}else
				{
					ratecyccfgans[cpri_num].res = 0;
					ratecycans[0].rate_cyc = ratecyccfg[cpri_num].rate_cyc;		//这里将配置参数查询中CPU占用率统计周期的查询内容
					cpri_write_str(&ratecycans[0], 10);					//将CPU占用率统计周期参数写入到RRU的信息记录文件中
				}
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&ratecyccfgans[cpri_num]), sizeof(PC_RATECYCANS));
				msg_head.msg_size += ratecyccfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 503:		//参数配置-驻波比门限配置
				memcpy((char *)(&swrthrcfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_SWRTHR) - 4);	//取得参数配置的消息体内容
				swrthrans[cpri_num].no1_thr = swrthrcfg[cpri_num].no1_thr;		//这里将配置参数查询中驻波比门限的查询内容
				swrthrans[cpri_num].no2_thr = swrthrcfg[cpri_num].no2_thr;		//这里将配置参数查询中驻波比门限的查询内容
				swrthrcfgans[cpri_num].res = 0;		//设置参数配置成功
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&swrthrcfgans[cpri_num]), sizeof(PC_SWRTHRANS));
				msg_head.msg_size += swrthrcfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 504:		//参数配置-Ir口工作模式配置
				memcpy((char *)(&irmodecfg[cpri_num]) + 4, (char *)src_addr, sizeof(CL_IRMODECFG) - 4);	//取得参数配置的消息体内容
				irmodeans[cpri_num].res = irmodecfg[cpri_num].ir_mode;		//这里将设置状态查询中ir口工作模式查询的内容
				/************************************
				Ir口工作模式配置，目前固定配置为普通模式
				************************************/
				irmodecfgans[cpri_num].res = 0;		//设置参数配置成功
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&irmodecfgans[cpri_num]), sizeof(CL_IRMODECFGANS));
				msg_head.msg_size += irmodecfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 505:		//参数配置-过温门限配置
				memcpy((char *)(&temthrcfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_TEMTHR) - 4);	//取得参数配置的消息体内容
				temthrans[cpri_num].tem_max = temthrcfg[cpri_num].tem_max;		//这里将设置参数查询中过温门限查询的内容
				temthrans[cpri_num].tem_low = temthrcfg[cpri_num].tem_low;		//这里将设置参数查询中过温门限查询的内容
				temthrcfgans[cpri_num].tem_type = temthrcfg[cpri_num].tem_type;
				temthrcfgans[cpri_num].res = 0;		//设置参数配置成功
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&temthrcfgans[cpri_num]), sizeof(PC_TEMTHRANS));
				msg_head.msg_size += temthrcfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 302:		//参数配置-射频通道状态配置
				memcpy((char *)(&rfchans[cpri_num]) + 4, (char *)src_addr, sizeof(SQ_RFCHANS) - 4);	//取得参数配置的消息体内容
				//射频通道状态配置
				if(rfchans[cpri_num].dl_sta != 0)	//下行使能
				{	
#ifdef PPC
					//da_recall_mode_set(cpri_num/4, (cpri_num - (cpri_num/4)*4)/2, 0);
					rfchstacfgans[cpri_num].res = da_recall_enable(cpri_num/4, (cpri_num-(cpri_num/4)*4)/2, cpri_num%2, 1);
					if(rfchstacfgans[cpri_num].res == RET_OK)
						rfchstacfgans[cpri_num].res = 0;
					else
						rfchstacfgans[cpri_num].res = 1;
#endif
				}else											//下行通道失能
				{
#ifdef PPC
					//da_recall_mode_set(cpri_num/4, (cpri_num - (cpri_num/4)*4)/2, 0);
					rfchstacfgans[cpri_num].res = da_recall_enable(cpri_num/4, (cpri_num-(cpri_num/4)*4)/2, cpri_num%2, 0);
					if(rfchstacfgans[cpri_num].res == RET_OK)
						rfchstacfgans[cpri_num].res = 0;
					else
						rfchstacfgans[cpri_num].res = 1;
#endif
				}
				if(rfchans[cpri_num].ul_sta != 0)	//上行使能
				{	
#ifdef PPC
					if(ad_enable(cpri_num/4, cpri_num%4+1, 1) == RET_ERR)
						rfchstacfgans[cpri_num].res = 1;
#endif
				}else											//上行通道失能
				{
#ifdef PPC
					if(ad_enable(cpri_num/4, cpri_num%4+1, 0) == RET_ERR)
						rfchstacfgans[cpri_num].res = 1;
#endif
				}
				rfchstacfgans[cpri_num].ch_num = rfchans[cpri_num].ch_num;
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&rfchstacfgans[cpri_num]), sizeof(PC_RFCHSTAANS));
				msg_head.msg_size += rfchstacfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			case 507:		//参数配置-天线配置
				memcpy((char *)(&antcfg[cpri_num]) + 4, (char *)src_addr, sizeof(PC_ANTCFG) - 4);	//取得参数配置的消息体内容
				/************************************
				天线配置，这里也不知道怎么去配
				************************************/
				antcfgans[cpri_num].ant_num = antcfg[cpri_num].ant_num;
				antcfgans[cpri_num].res = 0;		//设置参数配置成功
				//memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + msg_head.msg_size, (char *)(&antcfgans[cpri_num]), sizeof(PC_ANTCFGANS));
				msg_head.msg_size += antcfgans[cpri_num].ie_size;
				//send(sk, send_msg, msg_head.msg_size, 0);	//发送配置结果
				break;
			default:
				ret = -1;
		}

		if(size == count || ret != 0)
			break;
	}
	
	memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
	send(sk, send_msg, msg_head.msg_size, 0);
	
	if(size == count && ret == 0)
	{
		return 0;
	}else
	{
		return -1;
	}
}


/*
 * 函数名：int cpri_delaymse_que(int sk, char *msg)
 * 功能描述：RRU时延测量处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_delaymse_que(int sk, char *msg, const int cpri_num)
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
				memcpy((char *)(&cycdelaycfg[cpri_num]) + 4, (char *)src_addr, sizeof(DM_CYCDELAY) - 4);
				/************************************
				时延测量，其中一些测量参数是PC端发送给我的
				************************************/
				msg_head.msg_id = CPRI_DELAYMSE_ANS;
				msg_head.msg_size = MSG_HEADSIZE + raydelayans[cpri_num].ie_size;
				raydelayans[cpri_num].ray_num = cycdelaycfg[cpri_num].ray_num;
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&raydelayans[cpri_num]), sizeof(DM_RAYDELAYANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 921:
				memcpy((char *)(&delaycfg[cpri_num]) + 4, (char *)src_addr, sizeof(DM_DELAYCFG) - 4);
				msg_head.msg_id = CPRI_DELAYCFG_ANS;
				msg_head.msg_size = MSG_HEADSIZE + delaycfgans[cpri_num].ie_size;
				delaycfgans[cpri_num].ray_num = delaycfg[cpri_num].ray_num;
#ifdef PPC
				if(upstream_delay_ctrl(cpri_num/4, 1, &delaycfg[cpri_num].ul_cal) == RET_OK)	//设置fpga的上行时延
					delaycfgans[cpri_num].res = 0;
				else
					delaycfgans[cpri_num].res = 1;
				if(downstream_delay_ctrl(cpri_num/4, 1, &delaycfg[cpri_num].dl_cal) == RET_ERR)	//设置fpga的下行时延
					delaycfgans[cpri_num].res = 1;
#endif
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&delaycfgans[cpri_num]), sizeof(DM_DELAYCFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			default:
				ret = -1;
		}

		if(size == count || ret != 0)
			break;
	}
	
	if(size == count && ret == 0)
	{
		return 0;
	}else
	{
		return -1;
	}
}


/*
 * 函数名：void cpri_get_ala(AR_ALAQUE *ala_que, AR_ALAREP *ala_rep)
 * 功能描述：针对查询指定的告警码是否被置位的功能函数。
 * input：
 * 		1、ala_que，告警查询请求消息体；
 * 		2、ala_rep，告警查询应答消息体；
 *		3、cpri_num，cpri的接口号。
 * output：void
 */
void cpri_get_ala(AR_ALAQUE *ala_que, AR_ALAREP *ala_rep, const int cpri_num)
{
	int i = 0;

	for(i = 0; i < 23; i++)
	{
		if(ala_que->ala_code == 0)
		{

		}else
		{
			//判断请求的告警码是否有效，即此告警码是否存在
			if(ala_que->ala_code == *(unsigned int *)((char *)(&alaexl[cpri_num]) + i*4))
			{
				ala_rep->ala_able = 0;		//告警码存在

				//告警码是否已经被置位
				if((cpri_ala_flag[cpri_num] & (1 << i)) != 0)
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
	get_stamp(ala_rep->time_stamp);
	ala_rep->ala_code = ala_que->ala_code;
	ala_rep->ala_subcode = ala_que->ala_subcode;
	//ala_rep->addi_data = "";
}


/*
 * 函数名：int cpri_ala_que(int sk, char *msg)
 * 功能描述：RRU报警查询处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_ala_que(int sk, char *msg, const int cpri_num)
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
		memcpy((char *)(&alaque[cpri_num]) + 4, (char *)src_addr, sizeof(AR_ALAQUE) - 4);	//取得参数配置的消息体内容
		cpri_get_ala(&alaque[cpri_num], &alarep[cpri_num], cpri_num);		//查询告警号是否存在，并且判断其是否被置位
		msg_head.msg_id = CPRI_ALA_ANS;
		msg_head.msg_size = MSG_HEADSIZE + alarep[cpri_num].ie_size;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&alarep[cpri_num]), sizeof(AR_ALAREP));
		send(sk, send_msg, msg_head.msg_size, 0);

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri_logup_que(int sk, char *msg)
 * 功能描述：RRU日志上传请求处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_logup_que(int sk, char *msg, const int cpri_num)
{
	char *src_addr, send_msg[512], server_file[220], err_file[20];
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
		memcpy((char *)(&upque[cpri_num]) + 4, (char *)src_addr, sizeof(LOG_UPQUE) - 4);	//取得参数配置的消息体内容
		msg_head.msg_id = CPRI_LOGUP_ANS;
		msg_head.msg_size = MSG_HEADSIZE + upans[cpri_num].ie_size;
		//日志上传请求，这里判断日志是否存在
		upans[cpri_num].log_type = upque[cpri_num].log_type;
		sprintf(err_file, "/cpri%d_error_log", cpri_num);
		if(upans[cpri_num].log_type == 0)
			upans[cpri_num].res = access(err_file, F_OK);
		else if(upque[cpri_num].log_type == 1)
			upans[cpri_num].res = access("usr.txt", F_OK);
		else
			upans[cpri_num].res = access("sys.txt", F_OK);
		if(upans[cpri_num].res == -1)
			upans[cpri_num].res = 1;
		memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
		memcpy(send_msg + MSG_HEADSIZE, (char *)(&upans[cpri_num]), sizeof(LOG_UPANS));
		send(sk, send_msg, msg_head.msg_size, 0);
		
		if(upans[cpri_num].res == 0)	//如果日志文件存在，开始上传
		{
			upres[cpri_num].log_type = upque[cpri_num].log_type;
			//进行ftp上传日志，然后记录上传结果
			//strcat(server_file, upque[cpri_num].bbu_path);
			//strcat(server_file, upque[cpri_num].bbu_file);
			sprintf(server_file, "%s/%s", upque[cpri_num].bbu_path, upque[cpri_num].bbu_file);
			if(upres[cpri_num].log_type == 0)
				ret = ftp_up(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, err_file, server_file, 0);
			else if(upque[cpri_num].log_type == 1)
				ret = ftp_up(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, "usr.txt", server_file, 0);
			else
				ret = ftp_up(eth_name[cpri_num], linkaddr[cpri_num].bbu_ip, "sys.txt", server_file, 0);

			if(ret == 0)
				upres[cpri_num].res = 0;		//文件下载成功
			else if(ret == -5)
				upres[cpri_num].res = 1;		//文件不存在
			else if(ret == -8)
				upres[cpri_num].res = 2;		//文件下载超时
			else if(ret == -9)
				upres[cpri_num].res = 3;		//文件过大
			else
				upres[cpri_num].res = 4;		//其它原因
			
			msg_head.msg_id = CPRI_LOGUP_IND;
			msg_head.msg_size = MSG_HEADSIZE + upres[cpri_num].ie_size;
			memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
			memcpy(send_msg + MSG_HEADSIZE, (char *)(&upres[cpri_num]), sizeof(LOG_UPRES));
			send(sk, send_msg, msg_head.msg_size, 0);
		}

		ret = 0;
	}else
		ret = -1;
	
	return ret;
}


/*
 * 函数名：int cpri_reset_ind(int sk, char *msg)
 * 功能描述：RRU复位指示处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_reset_ind(int sk, char *msg, const int cpri_num)
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
 * 函数名：int cpri_bbubeat_msg(int sk, char *msg, int *num)
 * 功能描述：RRU在位检测处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_bbubeat_msg(int sk, char *msg, int *num, const int cpri_num)
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
 * 函数名：int cpri_lte_cfg(int sk, char *msg)
 * 功能描述：RRU小区配置处理函数。
 * input：
 * 		1、sk，具有TCP连接的套接字；
 * 		2、msg，接收到的BBU端口回复的信息；
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		0、进行相应的参数查询操作；
 * 		-1、接收到的消息体有误。
 */
int cpri_lte_cfg(const int sk, char *msg, const int cpri_num)
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
				memcpy((char *)(&pltcfg[cpri_num]) + 4, (char *)src_addr, sizeof(PLT_CFG) - 4);	//取得参数配置的消息体内容
				msg_head.msg_id = CPRI_LTE_ANS;
				msg_head.msg_size = MSG_HEADSIZE + pltcfgans[cpri_num].ie_size;
				pltcfgans[cpri_num].plot_locflag = pltcfg[cpri_num].plot_locflag;
				if(pltcfg[cpri_num].plot_cfgflag == 0)		//建立
				{
#ifdef PPC			//设置da为正常模式，打开da/ad通道使能
					da_recall_mode_set(cpri_num/4, (cpri_num - (cpri_num/4)*4)/2, 0);
					if(da_recall_enable(cpri_num/4, (cpri_num-(cpri_num/4)*4)/2, cpri_num%2, 1) == RET_OK)
						pltcfgans[cpri_num].res = 0;
					else
						pltcfgans[cpri_num].res = 1;
					if(ad_enable(cpri_num/4, cpri_num%4+1, 1) == RET_ERR)
						pltcfgans[cpri_num].res = 1;

					device_write(0, cpri_num+8, 0, (pltcfg[cpri_num].plot_power-10)*2);
					outpowerans[cpri_num].value = pltcfg[cpri_num].plot_power;
#endif
					if(pltcfgans[cpri_num].res == 0)
						runans[cpri_num].sta = 2;
				}
				else if(pltcfg[cpri_num].plot_cfgflag == 1)	//重配
				{
#ifdef PPC
					da_recall_mode_set(cpri_num/4, (cpri_num - (cpri_num/4)*4)/2, 0);
					if(da_recall_enable(cpri_num/4, (cpri_num-(cpri_num/4)*4)/2, cpri_num%2, 1) == RET_OK)
						pltcfgans[cpri_num].res = 0;
					else
						pltcfgans[cpri_num].res = 1;
					if(ad_enable(cpri_num/4, cpri_num%4+1, 1) == RET_ERR)
						pltcfgans[cpri_num].res = 1;

					device_write(0, cpri_num+8, 0, (pltcfg[cpri_num].plot_power-10)*2);
					outpowerans[cpri_num].value = pltcfg[cpri_num].plot_power;
#endif
					if(pltcfgans[cpri_num].res == 0)
						runans[cpri_num].sta = 2;
				}
				else										//删除
				{
#ifdef PPC
					if(da_recall_enable(cpri_num/4, (cpri_num-(cpri_num/4)*4)/2, cpri_num%2, 0) == RET_OK)
						pltcfgans[cpri_num].res = 0;
					else
						pltcfgans[cpri_num].res = 1;
					if(ad_enable(cpri_num/4, cpri_num%4+1, 0) == RET_ERR)
						pltcfgans[cpri_num].res = 1;

					device_write(0, cpri_num+8, 0, (pltcfg[cpri_num].plot_power-10)*2);
					outpowerans[cpri_num].value = pltcfg[cpri_num].plot_power;
#endif
					if(pltcfgans[cpri_num].res == 0)
						runans[cpri_num].sta = 0;
				}
				memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
				memcpy(send_msg + MSG_HEADSIZE, (char *)(&pltcfgans[cpri_num]), sizeof(PLT_CFGANS));
				send(sk, send_msg, msg_head.msg_size, 0);
				break;
			case 1502:
				/*******************************
				先空置
				*******************************/
				break;
			default:
				ret = -1;
		}

		if(size == count || ret != 0)
			break;
	}
	
	if(size == count && ret == 0)
	{
		return 0;
	}else
	{
		return -1;
	}
}



