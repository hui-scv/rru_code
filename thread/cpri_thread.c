/*
 * 文件名：cpri_thread.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：cpri接口的具体事务处理文件，cpri接口是作为客户端来使用。
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
#include "thread/cpri_handle.h"
#include "usr.h"

//服务器端口号
#define SERVER_PORT 33333

//定义告警标志位
unsigned int cpri_ala_flag[8] = {0};
//定义8个网口的设备名
char *eth_name[8] = {"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6", "eth7"};
//定义8个网口的套接字
int sock[8] = {-1};
BBU_HEAD cprians[8];

int cpri_creatsk(const int type, const int cpri_num);
int cpri_handle(char *msg, int sk, int *num, const int cpri_num);
int cpritobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num);
int cpri_tcpcon(const BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num);
int rec_timeout(int sk, const int sec);



/*
 * 函数名：void *cpri_thread(void)
 * 功能描述：cpri接口线程处理函数，向BBU获取接口的ip地址，并与BBU建立TCP/IP链接，
 * 		   随后进行时延补偿；完全建立链接后，采用select进行阻塞式接收，收到数据后
 * 		   完成相应功能。
 * input：void
 * output：void
 */
void *cpri_thread(void *cpri_n)
{
	unsigned short head, ie_id, ie_size;
	char *msg;
	int ret = 0, rec_num = 0, num = 0, cpri_num = 0;

	//定义消息头
	RRU_HEAD cpri_que;
	BBU_HEAD cpri_ans;
	//定义socket编程使用的socket地址相关信息结构体
	struct sockaddr_in cpri_addr;
	//定义cpri状态结构体
	CPRI_STATUS_S cpri_status;

	//为消息指针分配内存空间
	msg = (char *)malloc(sizeof(char) * 512);
CHLINK:
	//将各种结构体清零
	memset(&cpri_addr, 0, sizeof(struct sockaddr_in));
	memset(&cpri_que, 0, sizeof(RRU_HEAD));
	memset(&cpri_ans, 0, sizeof(BBU_HEAD));
	memset(msg, 0, sizeof(char) * 512);
	
	//指定socket套接字所采用的协议簇是IPv4
	cpri_addr.sin_family = AF_INET;
	//指定服务器端口号为33333
	cpri_addr.sin_port = htons(SERVER_PORT);
	//获取当前cpri的通道号
	cpri_num = *(int *)cpri_n;

#ifdef PPC
	/*****************
	等待F态，这里是揣测的state为0xF时，F态就绪
	*****************/
	cpri_status.state = 0xF;
	do{
		cpri_status_read(0, 0, &cpri_status);
		sleep(3);
	}while((((short)cpri_status.warning_report >> 0x01)&0xF) != 0xF);
#endif

	//RRU向BBU发送UDP广播，获取IP地址并设置
	cpritobbu_req(&cpri_que, &cpri_ans, &cpri_addr, cpri_num);
	memcpy(&cprians[cpri_num], &cpri_ans, sizeof(BBU_HEAD));

	//RRU向BBU发起TCP链接请求并完成链接
	cpri_addr.sin_port = htons(30000);
	sock[cpri_num] = cpri_tcpcon(cpri_ans, &cpri_addr, cpri_num);

	//RRU向BBU发起通信通道链路建立请求,完成RRU通信通道的配置以及返回配置响应
	cpri_comch_init(sock[cpri_num], msg, cpri_ans, cpri_num);

	memset(msg, 0, sizeof(char) * 512);

	while(1)
	{
		//接收BBU端发送的请求，等待3s，若没有接收到则超时
		ret = rec_timeout(sock[cpri_num], 3);
		if(ret < 0)
		{
			//接收超时
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
 		}else
 		{
			rec_num += recv(sock[cpri_num], msg + rec_num, 512, 0);
 		}

		//num用于记录多少次没有接收到心跳包，如果连续3次没有接收到心跳包，则次cpri接口重启
		num++;
		//用于判断一个消息体中的数据是否全部接收完毕
		if(rec_num >= ((MSG_HEAD *)msg)->msg_size && ((MSG_HEAD *)msg)->msg_size != 0)
		{
			//进入此函数，将对照不同的信息体类型进入到不同的处理函数中去
			cpri_handle(msg, sock[cpri_num], &num, cpri_num);
			rec_num = 0;
			memset(msg, 0, sizeof(char) * 512);
		}

		if(num == 3)
		{
			//心跳死亡，跳转到重新初始化cpri
			num = 0;
			shutdown(sock[cpri_num], SHUT_RDWR);
			goto CHLINK;
		}
	}
}

/*
 * 函数名：int cpri_handle(char *msg, int acq, int *num)
 * 功能描述：cpri实际功能处理函数，此函数将消息体进行了分类，
 * 		   首先判断消息体的类型，然后根据类型再调用相应的
 * 		   处理函数来实现相应的功能。
 * input：
 * 		1、msg，cpri接收到的BBU端口发送的消息体；
 * 		2、acq，socket套接字；
 * 		3、num，心跳包计数值。
 *		4、cpri_num，cpri的接口号。
 * output：
 *		成功：0；
 *		失败：负数。
 */
int cpri_handle(char *msg, int sk, int *num, const int cpri_num)
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
			ret = cpri_comch_cfg(sk, msg, cpri_num);
			break;
		case CPRI_VERDOWN_QUE:					//版本下载请求
			ret = cpri_verdown_que(sk, msg, cpri_num);
			break;
		case CPRI_VERACT_IND:					//版本激活指示
			ret = cpri_veract_ind(sk, msg, cpri_num);
			break;
		case CPRI_STATE_QUE:					//状态查询请求
			ret = cpri_state_que(sk, msg, cpri_num);
			break;
		case CPRI_PARA_QUE:						//参数查询请求
			ret = cpri_para_que(sk, msg, cpri_num);
			break;
		case CPRI_PARACFG_QUE:					//参数配置请求
			ret = cpri_paracfg_que(sk, msg, cpri_num);
			break;
		case CPRI_DELAYMSE_QUE | CPRI_DELAYCFG_CMD:		//时延测试请求和时延配置命令
			ret = cpri_delaymse_que(sk, msg, cpri_num);
			break;
		case CPRI_ALA_QUE:						//告警上报请求
			ret = cpri_ala_que(sk, msg, cpri_num);
			break;
		case CPRI_LOGUP_QUE:					//日志上传请求
			ret = cpri_logup_que(sk, msg, cpri_num);
			break;
		case CPRI_RESET_IND:					//复位指示
			ret = cpri_reset_ind(sk, msg, cpri_num);
			break;
		case CPRI_BBUBEAT_MSG:					//BBU心跳包消息
			ret = cpri_bbubeat_msg(sk, msg, num, cpri_num);
			break;
		case CPRI_LTE_CFG:						//LTE小区配置
			ret = cpri_lte_cfg(sk, msg, cpri_num);
			break;
		default:
			printf("msg_id error!\n");
			return -2;
	}

	return ret;
}

/*
 * 函数名：int cpri_creatsk(const int type, const int cpri_num)
 * 功能描述：根据传入的参数，创建相对应的socket套接字。
 * input：
 * 		1、type，socket套接字的类型。
 *		2、cpri_num，cpri的接口号。
 * output：
 * 		成功：sk，socket套接字
 */
int cpri_creatsk(const int type, const int cpri_num)
{
	char *device = eth_name[cpri_num], ip[20];
	int sk = 0;
	struct sockaddr_in cpri_client_addr;
	struct ifreq ifreq;

	memset(&cpri_client_addr, 0, sizeof(struct sockaddr_in));
	memset(&ifreq, 0, sizeof(struct ifreq));

	do
	{
		//首先根据类型，创建socket套接字
		sk = socket(AF_INET, type, 0);
		if(sk < 0)
		{
			perror(device);
			sleep(3);
			continue;
		}
		printf("cpri%d socket success!\n", cpri_num+1);

		//然后获得对应网口的ip地址，并设置到套接字地址信息设置的相关结构体中
		strcpy(ifreq.ifr_name, device);
		if(ioctl(sk, SIOCGIFADDR, &ifreq) < 0)
		{
			perror(device);
			shutdown(sk, SHUT_RDWR);
			sleep(3);
			continue;
		}
		strcpy(ip, inet_ntoa(((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr));
		printf("ip%d:%s\n", cpri_num+1, ip);
		cpri_client_addr.sin_family = AF_INET;
		if(type == SOCK_DGRAM)
			cpri_client_addr.sin_port = htons(33334);
		else
			cpri_client_addr.sin_port = htons(30000);
		cpri_client_addr.sin_addr.s_addr = inet_addr(ip);

		//使此端口可以在短时间内被多次bind
		int on = 1;
		if((setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
		{
			perror(device);
			shutdown(sk, SHUT_RDWR);
			sleep(3);
			continue;
		}
		//最后使用此网口的ip地址绑定套接字
		if(bind(sk, (struct sockaddr*)&cpri_client_addr, sizeof(struct sockaddr)) < 0)
		{
			perror(device);
			shutdown(sk, SHUT_RDWR);
			sleep(3);
			continue;
		}
		//将此套接字sk与对应的网口绑定起来
		if(setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq)) < 0)
		{
			perror(device);
			shutdown(sk, SHUT_RDWR);
			sleep(3);
			continue;
		}
		
		printf("cpri%d bind success!\n", cpri_num+1);
		break;
	}while(1);

	return sk;
}

/*
 * 函数名：int cpritobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num)
 * 功能描述：cpri将不断地向BBU端口发送UDP广播，直到获取RRU的ip地址。
 * 		   获取到ip地址后，cpri将自己的ip地址设置为此地址。
 * input：
 * 		1、cpri_que，RRU向BBU发送的UDP广播信息，在此函数中将自己初始化它；
 * 		2、cpri_ans，BBU向RRU答复的UDP广播信息，在此函数中将接收到的信息，并赋值给它；
 * 		3、cpri_addr，套接字使用的地址设置信息结构体。
 *		4、cpri_num，cpri的接口号。
 * output：
 * 		失败：不会返回，会一直发送UDP进行请求，直到有应答或程序关闭；
 * 		成功：0
 */
int cpritobbu_req(RRU_HEAD *cpri_que, BBU_HEAD *cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num)
{
	char *device = eth_name[cpri_num], ip[20], buf[16];
	int ret = -1, len = 0, iOp = 1, sk = -1;
	ssize_t ssize = -1;
	struct ifreq ifreq;

	//将ifreq结构体清零，并将想要设置ip地址的网口名称赋值给ifreq结构体
	memset(&ifreq, 0, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, device);
	
	len = sizeof(struct sockaddr);
	/*获取rru的id和bbu侧的光口号*/
#ifdef PPC
	vss_read(cpri_num/4, cpri_num%4, 0, buf);
#endif
	cpri_que->rru_id = buf[2];
	cpri_que->bbu_num = buf[1];

	//设置socket要通信的ip地址是255.255.255.255,
	//创建使用UDP协议的套接字，这样就表明采用UDP协议并全网广播数据
	(cpri_addr->sin_addr).s_addr = inet_addr("255.255.255.255");
	sk = cpri_creatsk(SOCK_DGRAM, cpri_num);

	do
	{
		//获取cpri接口使用的网口的mac地址
		ret = ioctl(sk, SIOCGIFHWADDR, &ifreq);
		if(ret < 0)
		{
			perror(device);
			sleep(3);
			continue;
		}
		memcpy(cpri_que->rru_mac, ifreq.ifr_hwaddr.sa_data, 6);

		//设置刚才创建的套接字采用广播方式通信
		ret = setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &iOp, sizeof(int));
		if(ret < 0)
		{
			perror(device);
			sleep(3);
			continue;
		}
		
		//发送RRU的广播请求信息
		ssize = sendto(sk, cpri_que, sizeof(RRU_HEAD), 0, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr));
		if(ssize < 0)
		{
			perror(device);
			sleep(3);
			continue;
		}
		
		//阻塞10s接受BBU应答回复的广播信息，如果没有收到，经过10s后将超时返回
		ret = rec_timeout(sk, 10);
		if(ret < 0)
		{
			continue;
		}
		ssize = recvfrom(sk, cpri_ans, sizeof(BBU_HEAD), 0, (struct sockaddr*)cpri_addr, &len);
		if(ssize > 0)
		{
			//收到应答后，比较rru的id是否相同。如果不同则说明不是想要收到的应答，然后重新开始请求。
			printf("cpri%d rru_id: 0x%x\n", cpri_num+1, cpri_ans->rru_id);
			if(cpri_que->rru_id != cpri_ans->rru_id)
			{
				printf("cpri%d rru_id error\n", cpri_num+1);
				sleep(3);
				continue;
			}
			else
			{
				//收到正确的应答后，设置此网口的ip地址并激活它
				sprintf(ip, "%d.%d.%d.%d", cpri_ans->rru_ip[0], cpri_ans->rru_ip[1], cpri_ans->rru_ip[2], cpri_ans->rru_ip[3]);
				((struct sockaddr_in*)&ifreq.ifr_addr)->sin_family = AF_INET;
				((struct sockaddr_in*)&ifreq.ifr_addr)->sin_addr.s_addr = inet_addr(ip);

				ret = ioctl(sk, SIOCSIFADDR, &ifreq);
				if(ret < 0)
				{
					perror(device);
					sleep(3);
					continue;
				}

				ifreq.ifr_flags = IFF_UP | IFF_RUNNING;
				ret = ioctl(sk, SIOCSIFFLAGS, &ifreq);
				if(ret < 0)
				{
					perror(device);
					sleep(3);
					continue;
				}

				break;
			}
		}
	}while(1);
	
	//关闭刚才创建使用UDP协议的套接字
	shutdown(sk, SHUT_RDWR);
	return 0;
}

/*
 * 函数名：int cpri_tcpcon(const BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num)
 * 功能描述：cpri将创建使用TCP协议的套接字，然后与BBU端口进行TCP连接。
 * input：
 * 		1、cpri_ans，BBU向RRU答复的UDP广播信息；
 * 		2、cpri_addr，套接字使用的地址设置信息结构体。
 *		3、cpri_num，cpri的接口号。
 * output：
 * 		失败：不会返回，会一直发送TCP连接请求，直到成功或程序关闭；
 * 		成功：TCP连接成功的sk套接字。
 */
int cpri_tcpcon(const BBU_HEAD cpri_ans, struct sockaddr_in *cpri_addr, const int cpri_num)
{
	char bbu_ip[20];
	int sk = -1, ret = -1;

	//将BBU端口的ip地址作为服务器的ip地址来进行设置
	sprintf(bbu_ip, "%d.%d.%d.%d", cpri_ans.bbu_ip[0], cpri_ans.bbu_ip[1], cpri_ans.bbu_ip[2], cpri_ans.bbu_ip[3]);
	(cpri_addr->sin_addr).s_addr = inet_addr(bbu_ip);

	//创建使用TCP协议的套接字sk
	sk = cpri_creatsk(SOCK_STREAM, cpri_num);

	do
	{
		//连接TCP服务器，如果连接失败，则等待3s后，再次请求连接，直到成功
		ret = connect(sk, (struct sockaddr*)cpri_addr, sizeof(struct sockaddr));
		if(ret < 0)
		{
			perror(eth_name[cpri_num]);
			sleep(3);
		}
	}while(ret < 0);

	printf("cpri%d connect success!\n", cpri_num);
	//返回成功连接的套接字sk
	return sk;
}

/*
 * 函数名：int rec_timeout(int sk, const int sec)
 * 功能描述：超时返回网络接收函数，如果在规定时间内没有网络数据到来，则超时返回。
 * input：
 * 		1、sk，网络套接字；
 * 		2、sec，设置超时时间。
 * output：
 * 		失败：-1，调用函数失败；
 *		超时：-2，等待接收超时；
 * 		成功：0，在规定时间内，有数据到来。
 */
int rec_timeout(int sk, const int sec)
{
	fd_set rdfds;
	struct timeval tv;
	int ret;

	FD_ZERO(&rdfds);	//设置文件描述符的集合，并清除它
	FD_SET(sk, &rdfds);	//将sk加入此集合

	tv.tv_sec = sec;
	tv.tv_usec = 0;
	
	ret = select(sk + 1, &rdfds, NULL, NULL, &tv);	//等待文件描述符集合中文件有数据可读
	if(ret < 0)
	{
		perror("rec_timeout");
	}
	else if(ret == 0)
	{
		ret = -2;
	}else
	{
		ret = 0;
	}

	return ret;
}

