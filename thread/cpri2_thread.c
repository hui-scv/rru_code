/*
 * 文件名：cpri2_thread.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：cpri2接口的具体事务处理文件，cpri接口是作为客户端来使用。
 */

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

#include "thread/rru_thread.h"
#include "struct.h"
#include "thread/cpri2_handle.h"
#include "usr.h"

//服务器端口号
#define SERVER_PORT 33333

//定义告警标志位
unsigned int cpri2_ala_flag = 0;
char cpri2_bbu_ip[20];

int cpri2_creatsk(int type);
int cpri2_handle(char *msg, int acq, int *num);
int cpri2tobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr);
int cpri2_tcpcon(BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr);

CPRI_STATUS_S cpri2_status;

/*
 * 函数名：void *cpri2_thread(void)
 * 功能描述：cpri2接口线程处理函数，向BBU获取接口的ip地址，并与BBU建立TCP/IP链接，
 * 		   随后进行时延补偿；完全建立链接后，采用select进行阻塞式接收，收到数据后
 * 		   完成相应功能。
 * input：void
 * output：void
 */
void *cpri2_thread(void)
{
	unsigned short head, ie_id, ie_size;
	char *msg;
	int sk, ret = 0, rec_num = 0, num = 0;

	//定义消息头
	RRU_HEAD cpri2_que;
	BBU_HEAD cpri2_ans;
	MSG_HEAD msg_head;
	//定义socket编程使用的socket地址相关信息结构体
	struct sockaddr_in cpri2_addr;

	fd_set rdfds;
	struct timeval tv;

	//为消息指针分配内存空间
	msg = (char *)malloc(sizeof(char) * 512);
	//将各种结构体清零
	memset(&cpri2_addr, 0, sizeof(struct sockaddr_in));
	memset(&cpri2_que, 0, sizeof(RRU_HEAD));
	memset(&cpri2_ans, 0, sizeof(BBU_HEAD));
	memset(&msg_head, 0, sizeof(MSG_HEAD));
	memset(msg, 0, sizeof(char) * 512);

	//指定socket套接字所采用的协议簇是IPv4
	cpri2_addr.sin_family = AF_INET;
	//指定服务器端口号为33333
	cpri2_addr.sin_port = htons(SERVER_PORT);


CHLINK:
	/*****************
	等待F态，这里是揣测的state为0x1时，F态就绪
	*****************/
	cpri2_status.state = 1;
	do{
		//cpri_status_read(0, 0, &cpri_status_1);
	}while((cpri2_status.state & 0x01) != 1);
	
	//RRU向BBU发送UDP广播，获取IP地址并设置
	cpri2tobbu_req(&cpri2_que, &cpri2_ans, &cpri2_addr);

	//RRU向BBU发起TCP链接请求并完成链接
	sk = cpri2_tcpcon(cpri2_ans, &cpri2_addr);

	//RRU向BBU发起通信通道链路建立请求,完成RRU通信通道的配置以及返回配置响应
	cpri2_comch_init(sk, msg, &msg_head, cpri2_ans);

	/*****************
	时延测量由BBU发起
	*****************/
	memset(msg, 0, sizeof(char) * 512);

	while(1)
	{
		//接收BBU端发送的请求，等待3s，若没有接收到则超时
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

		//num用于记录多少次没有接收到心跳包，如果连续3次没有接收到心跳包，则次cpri接口重启
		num++;
		//用于判断一个消息体中的数据是否全部接收完毕
		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			//进入此函数，将对照不同的信息体类型进入到不同的处理函数中去
			cpri2_handle(msg, sk, &num);
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
		}

		if(num == 3)
		{
			//心跳死亡，跳转到重新初始化cpri
			num = 0;
			close(sk);
			goto CHLINK;
		}
	}
}

/*
 * 函数名：int cpri2_handle(char *msg, int acq, int *num)
 * 功能描述：cpri实际功能处理函数，此函数将消息体进行了分类，
 * 		   首先判断消息体的类型，然后根据类型再调用相应的
 * 		   处理函数来实现相应的功能。
 * input：
 * 		1、msg，cpri接收到的BBU端口发送的消息体；
 * 		2、acq，socket套接字；
 * 		3、num，心跳包计数值。
 * output：
 *		成功：0；
 *		失败：负数。
 */
int cpri2_handle(char *msg, int acq, int *num)
{
	unsigned int msg_id, msg_size;
	int ret;

	msg_id = *(unsigned int *)(msg);
	msg_size = *(unsigned int *)(msg + 4);
	
	printf("msg_id : %d; msg_size : %d\n", msg_id, msg_size);

	//根据消息体的id来判断应该执行哪种cpri功能分支
	switch(msg_id)
	{
		case CPRI_CHLINK_CFG:					//通道连接配置
			ret = cpri2_comch_cfg(acq, msg);
			break;
		case CPRI_VERDOWN_QUE:					//版本下载请求
			ret = cpri2_verdown_que(acq, msg);
			break;
		case CPRI_VERACT_IND:					//版本激活指示
			ret = cpri2_veract_ind(acq, msg);
			break;
		case CPRI_STATE_QUE:					//状态查询请求
			ret = cpri2_state_que(acq, msg);
			break;
		case CPRI_PARA_QUE:						//参数查询请求
			ret = cpri2_para_que(acq, msg);
			break;
		case CPRI_PARACFG_QUE:					//参数配置请求
			ret = cpri2_paracfg_que(acq, msg);
			break;
		case CPRI_DELAYMSE_QUE | CPRI_DELAYCFG_CMD:		//时延测试请求和时延配置命令
			ret = cpri2_delaymse_que(acq, msg);
			break;
		case CPRI_ALA_QUE:						//告警上报请求
			ret = cpri2_ala_que(acq, msg);
			break;
		case CPRI_LOGUP_QUE:					//日志上传请求
			ret = cpri2_logup_que(acq, msg);
			break;
		case CPRI_RESET_IND:					//复位指示
			ret = cpri2_reset_ind(acq, msg);
			break;
		case CPRI_BBUBEAT_MSG:					//BBU心跳包消息
			ret = cpri2_bbubeat_msg(acq, msg, num);
			break;
		case CPRI_LTE_CFG:						//LTE小区配置
			ret = cpri2_lte_cfg(acq, msg);
			break;
		default:
			printf("msg_id error!\n");
			return -2;
	}

	return ret;
}

/*
 * 函数名：int cpri2_creatsk(int type)
 * 功能描述：根据传入的参数，创建相对应的socket套接字。
 * input：
 * 		1、type，socket套接字的类型。
 * output：
 * 		成功：sk，socket套接字
 */
int cpri2_creatsk(int type)
{
	char *device = ETH1, ip[20];
	int sk = 0;
	struct sockaddr_in cpri2_client_addr;
	struct ifreq ifreq;

	memset(&cpri2_client_addr, 0, sizeof(struct sockaddr_in));
	memset(&ifreq, 0, sizeof(struct ifreq));

	do
	{
		//首先根据类型，创建socket套接字
		sk = socket(AF_INET, type, 0);
		if(sk < 0)
		{
			printf("cpri2 socket error!\n");
			continue;
		}
		printf("cpri2 socket success!\n");

		//然后获得对应网口的ip地址，并设置到套接字地址信息设置的相关结构体中
		strcpy(ifreq.ifr_name, device);
		if(ioctl(sk, SIOCGIFADDR, &ifreq) < 0)
		{
			close(sk);
			sk = -1;
			sleep(3);
			continue;
		}
		strcpy(ip, inet_ntoa(((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr));
		printf("ip2:%s\n", ip);
		cpri2_client_addr.sin_family = AF_INET;
		cpri2_client_addr.sin_port = htons(33334);
		cpri2_client_addr.sin_addr.s_addr = inet_addr(ip);

		//最后使用此网口的ip地址绑定套接字
		if(bind(sk, (struct sockaddr*)&cpri2_client_addr, sizeof(struct sockaddr)) < 0)
		{
			printf("cpri2 bind error!\n");
			close(sk);
			sk = -1;
			sleep(3);
			continue;
		}
		printf("cpri2 bind success!\n");

		setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq));
	}while(sk < 0);

	return sk;
}

/*
 * 函数名：int cpri2tobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr)
 * 功能描述：cpri2将不断地向BBU端口发送UDP广播，直到获取RRU的ip地址。
 * 		   获取到ip地址后，cpri2将自己的ip地址设置为此地址。
 * input：
 * 		1、cpri_que，RRU向BBU发送的UDP广播信息，在此函数中将自己初始化它；
 * 		2、cpri_ans，BBU向RRU答复的UDP广播信息，在此函数中将接收到的信息，并赋值给它；
 * 		3、cpri_addr，套接字使用的地址设置信息结构体。
 * output：
 * 		失败：不会返回，会一直发送UDP进行请求，直到有应答或程序关闭；
 * 		成功：0
 */
int cpri2tobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr)
{
	char *device = ETH1, ip[20];
	int ret = 0, len = 0, iOp = 1, sk;
	struct ifreq ifreq;

	//将ifreq结构体清零，并将想要设置ip地址的网口名称赋值给ifreq结构体
	memset(&ifreq, 0, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, device);
	
	len = sizeof(struct sockaddr);
	/*****************
	获取rru的id
	*****************/
	cpri_que->rru_id = 1;

	//设置socket要通信的ip地址是255.255.255.255,
	//创建使用UDP协议的套接字，这样就表明采用UDP协议并全网广播数据
	(cpri_addr->sin_addr).s_addr = inet_addr("255.255.255.255");
	sk = cpri2_creatsk(SOCK_DGRAM);

	do
	{
		//获取cpri2接口使用的网口的mac地址
		if((ret = ioctl(sk, SIOCGIFHWADDR, &ifreq)) < 0)
			continue;
		memcpy(cpri_que->rru_mac, ifreq.ifr_hwaddr.sa_data, 6);

		//设置刚才创建的套接字采用广播方式通信
		if((ret = setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &iOp, sizeof(int))) < 0)
			continue;

		//发送RRU的广播请求信息
		ret = sendto(sk, cpri_que, sizeof(RRU_HEAD), 0, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr));
		if(ret < 0)
			continue;
		
		//阻塞接受BBU应答回复的广播信息，如果没有收到，则一直等待
		if((ret = recvfrom(sk, cpri_ans, sizeof(BBU_HEAD), 0, (struct sockaddr*)cpri_addr, &len)) > 0)
		{
			//收到应答后，比较rru的id是否相同。如果不同则说明不是想要收到的应答，然后重新开始请求。
			printf("rru_id2: %d\n", cpri_ans->rru_id);
			if(cpri_que->rru_id != cpri_ans->rru_id)
				ret = -1;
			else
			{
				//收到正确的应答后，设置此网口的ip地址并激活它
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
	
	//关闭刚才创建使用UDP协议的套接字
	close(sk);
	return 0;
}

/*
 * 函数名：int cpri2_tcpcon(BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr)
 * 功能描述：cpri2将创建使用TCP协议的套接字，然后与BBU端口进行TCP连接。
 * input：
 * 		1、cpri_ans，BBU向RRU答复的UDP广播信息；
 * 		2、cpri_addr，套接字使用的地址设置信息结构体。
 * output：
 * 		失败：不会返回，会一直发送TCP连接请求，直到成功或程序关闭；
 * 		成功：TCP连接成功的sk套接字。
 */
int cpri2_tcpcon(BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr)
{
	char bbu_ip[20];
	int sk, ret;

	//将BBU端口的ip地址作为服务器的ip地址来进行设置
	sprintf(cpri2_bbu_ip, "%d.%d.%d.%d", cpri_ans.bbu_ip[0], cpri_ans.bbu_ip[1], cpri_ans.bbu_ip[2], cpri_ans.bbu_ip[3]);
	(cpri_addr->sin_addr).s_addr = inet_addr(cpri2_bbu_ip);

	//创建使用TCP协议的套接字sk
	sk = cpri2_creatsk(SOCK_STREAM);

	do
	{
		//连接TCP服务器，如果连接失败，则等待3s后，再次请求连接，直到成功
		if((ret = connect(sk, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr))) < 0)
		{
			printf("cpri2 connect error!\n");
		}
		sleep(3);
	}while(ret < 0);

	printf("cpri2 connect success!\n");
	//返回成功连接的套接字sk
	return sk;
}

