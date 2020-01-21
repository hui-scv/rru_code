#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <linux/tcp.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "../include/thread/rru_thread.h"
#include "../include/struct.h"
#include "../include/thread/cpri7_handle.h"


#define SERVER_PORT 33333
#define ETH6 "eth6"

int cpri7_creatsk(int type);
int cpri7_handle(char *msg, int acq, int *num);
int cpri7tobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr);
int cpri7_tcpcon(BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr);


/***********************************************
功能：cpri7接口线程处理函数
***********************************************/
void *cpri7_thread(void)
{
	unsigned short head, ie_id, ie_size;
	char *msg;
	int sk, ret = 0, rec_num = 0, num = 0;

	RRU_HEAD cpri7_que;
	BBU_HEAD cpri7_ans;
	MSG_HEAD msg_head;
	struct sockaddr_in cpri7_addr;

	fd_set rdfds;
	struct timeval tv;

	msg = (char *)malloc(sizeof(char) * 512);
	memset(&cpri7_addr, 0, sizeof(struct sockaddr_in));
	memset(&cpri7_que, 0, sizeof(RRU_HEAD));
	memset(&cpri7_ans, 0, sizeof(BBU_HEAD));
	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memset(msg, 0, sizeof(char) * 512);

	cpri7_addr.sin_family = AF_INET;
	cpri7_addr.sin_port = htons(SERVER_PORT);


CHLINK:
	/*****************
	等待F态
	*****************/
	
	//RRU向BBU发起接入请求，获取IP地址并设置
	cpri7tobbu_req(&cpri7_que, &cpri7_ans, &cpri7_addr);

	//RRU向BBU发起TCP链接请求并完成链接
	sk = cpri7_tcpcon(cpri7_ans, &cpri7_addr);

	//RRU向BBU发起通信通道建立请求,完成RRU通信通道的配置以及返回配置响应
	cpri7_comch_init(sk, msg, &msg_head, cpri7_ans);

	/*****************
	时延测量由BBU发起
	*****************/
	memset(msg, 0, sizeof(char) * 512);

	while(1)
	{
		FD_ZERO(&rdfds);
		FD_SET(sk, &rdfds);
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		ret = select(sk + 1, &rdfds, NULL, NULL, &tv);
		if(ret < 0)
			printf("select erro!\n");
		else if(ret == 0)
		{
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
 		}else if(FD_ISSET(sk, &rdfds))
			rec_num += recv(sk, msg + rec_num, 512, 0);

		num++;
		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			cpri7_handle(msg, sk, &num);
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
		}

		if(num == 3)
		{
			num = 0;
			close(sk);
			goto CHLINK;
		}
	}
}

int cpri7_handle(char *msg, int acq, int *num)
{
	unsigned int msg_id, msg_size;
	int ret;

	msg_id = *(unsigned int *)(msg);
	msg_size = *(unsigned int *)(msg + 4);
	
	printf("msg_id : %d; msg_size : %d\n", msg_id, msg_size);

	switch(msg_id)
	{
		case CPRI_CHLINK_CFG:
			ret = cpri7_comch_cfg(acq, msg);
			break;
		case CPRI_VERDOWN_QUE:
			ret = cpri7_verdown_que(acq, msg);
			break;
		case CPRI_VERACT_IND:
			ret = cpri7_veract_ind(acq, msg);
			break;
		case CPRI_STATE_QUE:
			ret = cpri7_state_que(acq, msg);
			break;
		case CPRI_PARA_QUE:
			ret = cpri7_para_que(acq, msg);
			break;
		case CPRI_PARACFG_QUE:
			ret = cpri7_paracfg_que(acq, msg);
			break;
		case CPRI_DELAYMSE_QUE | CPRI_DELAYCFG_CMD:
			ret = cpri7_delaymse_que(acq, msg);
			break;
		case CPRI_ALA_QUE:
			ret = cpri7_ala_que(acq, msg);
			break;
		case CPRI_LOGUP_QUE:
			ret = cpri7_logup_que(acq, msg);
			break;
		case CPRI_RESET_IND:
			ret = cpri7_reset_ind(acq, msg);
			break;
		case CPRI_BBUBEAT_MSG:
			ret = cpri7_bbubeat_msg(acq, msg, num);
			break;
		case CPRI_LTE_CFG:
			ret = cpri7_lte_cfg(acq, msg);
			break;
		default:
			printf("msg_id error!\n");
			return -2;
	}

	return ret;
}

int cpri7_creatsk(int type)
{
	char *device = ETH6, ip[20];
	int sk = 0;
	struct sockaddr_in cpri7_client_addr;
	struct ifreq ifreq;

	memset(&cpri7_client_addr, 0, sizeof(struct sockaddr_in));
	memset(&ifreq, 0, sizeof(struct ifreq));

	do
	{
		sk = socket(AF_INET, type, 0);
		if(sk < 0)
		{
			printf("cpri7 socket error!\n");
			continue;
		}
		printf("cpri7 socket success!\n");

		strcpy(ifreq.ifr_name, device);
		if(ioctl(sk, SIOCGIFADDR, &ifreq) < 0)
		{
			close(sk);
			sk = -1;
			sleep(3);
			continue;
		}
		strcpy(ip, inet_ntoa(((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr));
		printf("ip7:%s\n", ip);
		cpri7_client_addr.sin_family = AF_INET;
		cpri7_client_addr.sin_port = htons(33334);
		cpri7_client_addr.sin_addr.s_addr = inet_addr(ip);

		if(bind(sk, (struct sockaddr*)&cpri7_client_addr, sizeof(struct sockaddr)) < 0)
		{
			printf("cpri7 bind error!\n");
			close(sk);
			sk = -1;
			sleep(3);
			continue;
		}
		printf("cpri7 bind success!\n");
	}while(sk < 0);

	return sk;
}

int cpri7tobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr)
{
	char *device = ETH6, ip[20];
	int ret = 0, len = 0, iOp = 1, sk;
	struct ifreq ifreq;

	memset(&ifreq, 0, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, device);
	
	len = sizeof(struct sockaddr);
	/*****************
	获取rru的id
	*****************/
	cpri_que->rru_id = 1;
	(cpri_addr->sin_addr).s_addr = inet_addr("255.255.255.255");

	sk = cpri7_creatsk(SOCK_DGRAM);

	do
	{
		if((ret = ioctl(sk, SIOCGIFHWADDR, &ifreq)) < 0)
			continue;
		memcpy(cpri_que->rru_mac, ifreq.ifr_hwaddr.sa_data, 6);

		if((ret = setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &iOp, sizeof(int))) < 0)
			continue;

		ret = sendto(sk, cpri_que, sizeof(RRU_HEAD), 0, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr));
		if(ret < 0)
			continue;
		
		if((ret = recvfrom(sk, cpri_ans, sizeof(BBU_HEAD), 0, (struct sockaddr*)cpri_addr, &len)) > 0)
		{
			printf("rru_id7: %d\n", cpri_ans->rru_id);
			if(cpri_que->rru_id != cpri_ans->rru_id)
				ret = -1;
			else
			{
				sprintf(ip, "%d.%d.%d.%d", cpri_ans->rru_ip[0], cpri_ans->rru_ip[1], cpri_ans->rru_ip[2], cpri_ans->rru_ip[3]);
				((struct sockaddr_in*)&ifreq.ifr_addr)->sin_family = AF_INET;
				((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr.s_addr = inet_addr(ip);

				if((ret = ioctl(sk, SIOCSIFADDR, &ifreq)) < 0)
					continue;

				ifreq.ifr_flags = IFF_UP | IFF_RUNNING;
				if(ioctl(sk, SIOCSIFFLAGS, &ifreq) < 0)
					ret = -1;
			}
		}
	}while(ret < 0);
	
	close(sk);
	return 0;
}

int cpri7_tcpcon(BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr)
{
	char bbu_ip[20];
	int sk, ret;

	sprintf(bbu_ip, "%d.%d.%d.%d", cpri_ans.bbu_ip[0], cpri_ans.bbu_ip[1], cpri_ans.bbu_ip[2], cpri_ans.bbu_ip[3]);
	(cpri_addr->sin_addr).s_addr = inet_addr(bbu_ip);

	sk = cpri7_creatsk(SOCK_STREAM);

	do
	{
		if((ret = connect(sk, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr))) < 0)
		{
			printf("cpri7 connect error!\n");
		}
		sleep(3);
	}while(ret < 0);

	printf("cpri7 connect success!\n");
	return sk;
}

