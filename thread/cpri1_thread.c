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

#include "../include/thread/rru_thread.h"
#include "../include_pc/struct_pc.h"
#include "../include/thread/cpri1_handle.h"


#define SERVER_PORT 33333

int cpri1_link(int type);
int cpri1_handle(char *msg, int acq);
short spot_id_cpri1(unsigned short type);


/***********************************************
功能：cpri1接口线程处理函数
***********************************************/
void *cpri1_thread(void)
{
	unsigned short head, ie_id, ie_size;
	char *msg;
	int sk, acq, ret, iOp = 1, rec_num = 0, num = 0;
	struct sockaddr_in cpri1_addr;

	do
	{
		sk = cpri1_link(SOCK_DGRAM);
	}while(sk == -1);
	//acq = cpri1_accept(sk);	

	if(setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &iOp, sizeof(int)) < 0)
	{
		printf("cpri1 setsockopt failed!\n");
	}

	memset(&cpri1_addr, 0, sizeof(struct sockaddr_in));
	
	cpri1_addr.sin_family = AF_INET;
	cpri1_addr.sin_port = htons(SERVER_PORT);
	cpri1_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	
	msg = (char *)malloc(sizeof(char) * 512);

	while(1)
	{
		memset(msg, 0, sizeof(char) * 512);
		
		memcpy(msg, "huizhongli", 10);
		num = sendto(sk, "huizhongli", 10, 0, (struct sockaddr*)&cpri1_addr, sizeof(struct sockaddr));
		printf("num :%d\n", num);
		sleep(1);
	}
}

int cpri1_handle(char *msg, int acq)
{
	unsigned short ie_id, ie_size;
	short type;
	int ret;

	ie_id = *(unsigned short *)(msg);
	ie_size = *(unsigned short *)(msg + 2);
	//ie_id = ntohs(ie_id);
	//ie_size = ntohs(ie_size);

	type = spot_id_cpri1(ie_id);
	
	printf("ie_id : %d; ie_size : %d\n", ie_id, ie_size);

	switch(type)
	{
		case CPRI_INF_TYPE:
			ret = cpri1_inftype_handle(msg, ie_id, acq);
			break;
		case CPRI_STA_TYPE:
			ret = cpri1_statype_handle(msg, ie_id, acq);
			break;
		case CPRI_ALA_TYPE:
			ret = cpri1_alatype_handle(msg, ie_id, acq);
			break;
		case CPRI_TES_TYPE:
			ret = cpri1_testype_handle(msg, ie_id, acq);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return ret;
}

short spot_id_cpri1(unsigned short type)
{
	short ret;

	if(0 < type && type <= 13)
		ret = CPRI_INF_TYPE;
	else if(302 <= type && type <= 458)
		ret = CPRI_STA_TYPE;
	else if(1001 <= type && type <= 1101)
		ret = CPRI_ALA_TYPE;
	else if(11000 <= type && type <= 11009)
		ret = CPRI_TES_TYPE;
	else
		ret = -1;

	return ret;
}

int cpri1_link(int type)
{
	int sk, ret = 0;
	struct sockaddr_in cpri1_client_addr;

	memset(&cpri1_client_addr, 0, sizeof(struct sockaddr_in));

	cpri1_client_addr.sin_family = AF_INET;
	cpri1_client_addr.sin_port = htons(33334);
	cpri1_client_addr.sin_addr.s_addr = inet_addr("192.168.100.33");

	sk = socket(AF_INET, type, 0);
	if(sk == -1)
	{
		printf("cpri1 socket error!\n");
		ret = -1;
		return ret;
	}
	printf("cpri1 socket success!\n");

	if(bind(sk, (struct sockaddr*)&cpri1_client_addr, sizeof(struct sockaddr)) == -1)
	{
		printf("cpri1 bind error!\n");
		ret = -1;
		close(sk);
		return ret;
	}
	printf("cpri1 bind success!\n");

	return sk;
}

