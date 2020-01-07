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
#include "../include/thread/main_handle.h"


#define RRU_INF_TYPE 1
#define RRU_STA_TYPE 2
#define RRU_ALA_TYPE 3
#define RRU_TES_TYPE 4

int main_link(void);
int main_accept(int sk);
int main_handle(char *msg, int acq);
short spot_id_pc(unsigned short type);


/***********************************************
功能：维护接口线程处理函数
***********************************************/
void *maintain_thread(void)
{
	unsigned short head, ie_id, ie_size;
	char *msg;
	int sk, acq, ret, rec_num = 0, num = 0;

	do
	{
		sk = main_link();
	}while(sk == -1);
	acq = main_accept(sk);	

	msg = (char *)malloc(sizeof(char) * 512);
	//memset(msg, 0, sizeof(char) * 512);

	while(1)
	{
		memset(msg, 0, sizeof(char) * 512);

		if((num = recv(acq, msg, 512, 0)) > 0)
		{
			rec_num += num;
			while(rec_num < 6)
			{
				num = recv(acq, msg + rec_num, 512, 0);
				rec_num += num;
			}

			//printf("rec_num: %d\n", rec_num);
			head = ((MSG_HEAD_PC *)msg)->msg_head;
			head = ntohs(head);
			//printf("head: 0x%x\n", head);

			if(head == head_pc.msg_head)
			{
				ie_id = *(unsigned short *)(msg + 2);
				ie_size = *(unsigned short *)(msg + 4);
				//ie_id = ntohs(ie_id);
				//ie_size = ntohs(ie_size);

				while(rec_num < (ie_size + 2))
				{
					num = recv(acq, msg + rec_num, 512, 0);
					rec_num += num;
				}

				ret = main_handle(msg + 2, acq);
			}
	
			rec_num = 0;
		}else
		{
			close(acq);
			acq = main_accept(sk);
		}
	}
}

int main_handle(char *msg, int acq)
{
	unsigned short ie_id, ie_size;
	short type;
	int ret;

	ie_id = *(unsigned short *)(msg);
	ie_size = *(unsigned short *)(msg + 2);
	//ie_id = ntohs(ie_id);
	//ie_size = ntohs(ie_size);

	type = spot_id_pc(ie_id);
	
	printf("ie_id : %d; ie_size : %d\n", ie_id, ie_size);

	switch(type)
	{
		case RRU_INF_TYPE:
			ret = inftype_handle(msg, ie_id, acq);
			break;
		case RRU_STA_TYPE:
			ret = statype_handle(msg, ie_id, acq);
			break;
		case RRU_ALA_TYPE:
			ret = alatype_handle(msg, ie_id, acq);
			break;
		case RRU_TES_TYPE:
			ret = testype_handle(msg, ie_id, acq);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return ret;
}

short spot_id_pc(unsigned short type)
{
	short ret;

	if(0 < type && type <= 13)
		ret = RRU_INF_TYPE;
	else if(302 <= type && type <= 458)
		ret = RRU_STA_TYPE;
	else if(1001 <= type && type <= 1101)
		ret = RRU_ALA_TYPE;
	else if(11000 <= type && type <= 11009)
		ret = RRU_TES_TYPE;
	else
		ret = -1;

	return ret;
}

int main_link(void)
{
	int sk, ret = 0;
	struct sockaddr_in main_addr;

	memset(&main_addr, 0, sizeof(struct sockaddr_in));

	main_addr.sin_family = AF_INET;
	main_addr.sin_port = htons(66666);
	main_addr.sin_addr.s_addr = inet_addr("192.168.100.33");

	sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk == -1)
	{
		printf("socket error!\n");
		ret = -1;
		return ret;
	}
	printf("socket success!\n");

	if(bind(sk, (struct sockaddr*)&main_addr, sizeof(struct sockaddr)) == -1)
	{
		printf("bind error!\n");
		ret = -1;
		close(sk);
		return ret;
	}
	printf("bind success!\n");

	if(listen(sk, 10) == -1)
	{
		printf("listen error!\n");
		ret = -1;
		close(sk);
		return ret;
	}
	printf("listen success!\n");

	return sk;
}

int main_accept(int sk)
{
	int acq, addrlen, ret = 0;
	struct sockaddr_in client_addr;

	addrlen = sizeof(struct sockaddr);
	memset(&client_addr, 0, sizeof(struct sockaddr_in));

	acq = accept(sk, (struct sockaddr*)&client_addr, &addrlen);
	if(acq == -1)
	{
		printf("accept error!\n");
		ret = -1;
		close(sk);
		return ret;
	}
	printf("%s %d success connect\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	
	return acq;
}



