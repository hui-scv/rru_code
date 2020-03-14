/*
 * 文件名：init.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：此文件用于初始化RRU系统；初始化驱动、配置ADDA、配置时钟和射频；并提供了读取RRU系统信息的接口。
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "include/struct.h"
#include "include/init.h"

void driver_init(void)
{
	printf("driver is inited!\n");
}

void ad_da_init(void)
{
	printf("ad_da is inited!\n");
}

void time_init(void)
{
	printf("time is inited!\n");
}

void rf_init(void)
{
	printf("rf is inited!\n");
}

/*
 * 函数名：unsigned int get_val(char *s, int num)
 * 功能描述：从一行字符串中取得前后以空格隔开的字符。
 * input：
 		1、s，指向字符串的指针；
 		2、num，想要取得的第几个字符。
 * output：
 *		res：从字符转换成的数字
 */
unsigned int get_val(char *s, int num)
{
	char *c, *b, number[11];
	unsigned int i = 0, j = 0, k = 0, res = 0;

	memset(number, 0, 11);
	c = s;

	for(k = 0; k < 200; k++)
	{
		//当前字符为空格时，并且下一个字符不为空格时，可以取得此字符
		if(*c++ == ' ' && *c != ' ')
		{
			b = c;		//记录不为空格的字符所在的位置
			i++;
			if(i == num)
			{
				for(j = 0; j < 11; j++)
					if(*++c == ' ')
						break;
				memcpy(number, b, j+1);		//将此数字字符复制到数组中
				res = atoi(number);			//然后将它转化为整形数字
				//printf("%d\n", res);
				return res;
			}
		}
	}

	return 0;
}

/*
 * 函数名：void signalHandler(int signo)
 * 功能描述：当信号发生时的处理函数。
 * input：
 		1、signo，导致信号发生的信号类型
 * output：void
 */
void signalHandler(int signo)
{
	FILE *fd;
	char s[512];
	static unsigned int new_total = 0, old_total = 0, new_idle = 0, old_idle = 0, flag = 0;
	int i;
	float p;

	//打开/proc/stat文件，读取cpu使用的信息
	fd = fopen("/proc/stat", "r");

	//打开文件失败就返回
	if(fd < 0)
	{
		perror("read cpu rate");
		return;
	}
	
	//取出文件的第一行信息
	fgets(s, 512, fd);
	//printf("cpu:%s\n", s);

	if(flag == 0)		//当flag为0时，采用新的时间减去旧的时间来计算cpu使用率
	{
		new_total = 0;
		new_idle = get_val(s, 4);	//得到cpu的空闲时间
		for(i = 0; i < 8; i++)
			new_total += get_val(s, i + 1);		//得到cpu总共运行的时间
		
		p = ((float)(new_idle - old_idle)) / ((float)(new_total - old_total));	//使用空闲时间除以总共运行时间
		p = (1 - p) * 100;		//这就能得到cpu的占用率了
		printf("nCPU: %f\n", p);
	}else				//当flag为1时，采用旧的时间减去新的时间来计算cpu使用率
	{
		old_total = 0;
		old_idle = get_val(s, 4);	//得到cpu的空闲时间
		for(i = 0; i < 8; i++)
			old_total += get_val(s, i + 1);		//得到cpu总共运行的时间
		
		p = ((float)(old_idle - new_idle)) / ((float)(old_total - new_total));	//使用空闲时间除以总共运行时间
		p = (1 - p) * 100;		//这就能得到cpu的占用率了
		printf("oCPU: %f\n", p);
	}
	flag = ~flag;

	cpurateans[0].cpu_rate = (unsigned int)p;	//设置cpu的占用率，这个占用率设置在CPU占用响应IE，即参数查询时

	fclose(fd);
}


/*
 * 函数名：int cpri_write_str(const char *msg, int i)
 * 功能描述：读取RRU系统信息的接口，将上一次配置的RRU信息读取出来并采用。
 * input：
 * 		1、msg，指向一段内存的指针，用于传递写入的消息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cpri_write_str(const char *msg, int i)
{
	int fd, ret;

	//打开存储rru系统信息的文件，如果文件不存在，则创建一个新文件
	fd = open("./rru_info.dat", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open rru_info");
		goto fd_error;
	}

	//定位到要写入rru系统信息中的某一项信息的开头位置，如果定位失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror("lseek rru_info");
		goto error;
	}

	//写入rru系统信息中的某一项信息，如果写入失败就关闭文件并返回-1
	ret = write(fd, msg, 512);
	if(ret < 0)
	{
		perror("write rru_info");
		goto error;
	}
	
	//读取成功，关闭文件
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	return -1;
}


/*
 * 函数名：int cpri_read_str(char *msg, int i)
 * 功能描述：读取RRU系统信息的接口，将上一次配置的RRU信息读取出来并采用。
 * input：
 * 		1、msg，指向一段内存的指针，用于存储读取出来的RRU系统信息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cpri_read_str(char *msg, int i)
{
	int fd, ret;

	//打开存储rru系统信息的文件，如果打开失败则返回-1
	fd = open("./rru_info.dat", O_RDONLY);
	if(fd < 0)
	{
		perror("open rru_info");
		goto fd_error;
	}

	//定位到要读取rru系统信息中的某一项信息的开头位置，如果读取失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror("lseek rru_info");
		goto error;
	}

	//读取rru系统信息中的某一项信息，如果读取失败就关闭文件并返回-1
	ret = read(fd, msg, 512);
	if(ret < 0)
	{
		perror("read rru_info");
		goto error;
	}
	
	//读取成功，关闭文件
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	return -1;
}

/*
 * 函数名：void init(void)
 * 功能描述：获取RRU系统信息，将此信息写入到每个cpri接口，初始化驱动，以及配置各个模块。
 * input：void
 * output：void
 */
void init(void)
{
	struct itimerval new_value;
	char msg[512];
	int i, j, ret;
	//pthread_rwlock_init(&chlinkque_rwlock, NULL);

	//按照id号为1-10来读取RRU系统信息的结构体，然后将其存储到8个cpri接口中。
	//这里有个问题就是是否每个cpri的系统信息都相同？如果不同则不可以这样做！
	for(i = 1; i < 11; i++)
	{
		memset(msg, 0, 512);
		ret = cpri_read_str(msg, i);
		if(ret > 0)
			for(j = 0; j < 8; j++)
			{
				switch(i)
				{
					case 1:		//RRU产品标识IE
						//memcpy((char *)(&porid_pc) + 4, (char *)msg + 4, sizeof(CL_PROID_PC) - 4);
						memcpy((char *)(&porid[j]) + 4, (char *)msg + 4, sizeof(CL_PROID) - 4);
						break;
					case 2:		//通道建立原因IE
						//memcpy((char *)(&linktype_pc) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE_PC) - 4);
						memcpy((char *)(&linktype[j]) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE) - 4);
						break;
					case 3:		//RRU能力IE
						//memcpy((char *)(&rrucapa_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA_PC) - 4);
						memcpy((char *)(&rrucapa[j]) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA) - 4);
						break;
					case 4:		//RRU级数IE
						//rrulv_pc.rru_lv = ((CL_RRULV_PC *)msg)->rru_lv;
						rrulv[j].rru_lv = ((CL_RRULV *)msg)->rru_lv;
						break;
					case 5:		//RRU硬件类型及版本信息IE
						//memcpy((char *)(&rruinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUINFO_PC) - 4);
						memcpy((char *)(&rruinfo[j]) + 4, (char *)msg + 4, sizeof(CL_RRUINFO) - 4);
						break;
					case 6:		//RRU软件版本信息IE
						//memcpy((char *)(&rrusoftinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO_PC) - 4);
						memcpy((char *)(&rrusoftinfo[j]) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO) - 4);
						break;
					case 7:		//RRU频段能力IE
						//memcpy((char *)(&rrufre_pc) + 4, (char *)msg + 4, sizeof(CL_RRUFRE_PC) - 4);
						memcpy((char *)(&rrufre[j]) + 4, (char *)msg + 4, sizeof(CL_RRUFRE) - 4);
						break;
					case 8:		//RRU射频通道能力IE
						//memcpy((char *)(&rrurf_pc) + 4, (char *)msg + 4, sizeof(CL_RRURF_PC) - 4);
						memcpy((char *)(&rrurf[j]) + 4, (char *)msg + 4, sizeof(CL_RRURF) - 4);
						break;
					case 9:		//载波能力组合IE
						//memcpy((char *)(&rrucir_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCIR_PC) - 4);
						memcpy((char *)(&rrucir[j]) + 4, (char *)msg + 4, sizeof(CL_RRUCIR) - 4);
					case 10:	//CPU占用率统计周期响应IE
						memcpy((char *)(&ratecycans[j]) + 4, (char *)msg + 4, sizeof(PQ_RATECYCANS) - 4);
						break;
				}
			}
	}
	
	//外设驱动初始化
	driver_init();
	//AD、DA初始化配置
	ad_da_init();
	//时钟初始化配置
	time_init();
	//射频模块初始化配置
	rf_init();

	//绑定信号发生时的处理函数
	signal(SIGALRM, signalHandler);

	//根据cpu占用率统计周期是否设置
	if(ratecycans[0].rate_cyc <= 0)
	{
		new_value.it_value.tv_sec = 1;
		new_value.it_value.tv_usec = 0;
		new_value.it_interval.tv_sec = 10;
		new_value.it_interval.tv_usec = 0;
		ret = setitimer(ITIMER_REAL, &new_value, NULL);
	}else
	{
		new_value.it_value.tv_sec = 1;
		new_value.it_value.tv_usec = 0;
		new_value.it_interval.tv_sec = ratecycans[0].rate_cyc;
		new_value.it_interval.tv_usec = 0;
		ret = setitimer(ITIMER_REAL, &new_value, NULL);
	}

	if(ret == -1)
		perror("read cpu rate");
}
