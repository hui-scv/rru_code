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
 * 函数名：int cprix_write_info(char *msg, int i, int cpri_num)
 * 功能描述：写入cpri口不同内容的信息。
 * input：
 * 		1、msg，指向一段内存的指针，用于传递写入的消息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cprix_write_info(char *msg, int i, int cpri_num)
{
	int fd, ret;
	char path[30];
	
	sprintf(path, "./cpriinfo/cpri%d_info", cpri_num);
	fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open cprix_info");
		goto fd_error;
	}

	//定位到要写入rru系统信息中的某一项信息的开头位置，如果定位失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror("lseek cprix_info");
		goto error;
	}

	//写入rru系统信息中的某一项信息，如果写入失败就关闭文件并返回-1
	ret = write(fd, msg, 512);
	if(ret < 0)
	{
		perror("write cprix_info");
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
 * 函数名：int cpri_write_info(char *msg, int i)
 * 功能描述：读取RRU系统信息的接口，将上一次配置的RRU信息读取出来并采用。
 * input：
 * 		1、msg，指向一段内存的指针，用于传递写入的消息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cpri_write_info(char *msg, int i)
{
	int fd, ret;
	char path[30];
	
	sprintf(path, "./cpriinfo/cpri_info");
	fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("open cpri_info");
		goto fd_error;
	}

	//定位到要写入rru系统信息中的某一项信息的开头位置，如果定位失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror("lseek cpri_info");
		goto error;
	}

	//写入rru系统信息中的某一项信息，如果写入失败就关闭文件并返回-1
	ret = write(fd, msg, 512);
	if(ret < 0)
	{
		perror("write cpri_info");
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
 * 函数名：int cprix_read_info(char *msg, int i, int cpri_num)
 * 功能描述：读取RRU系统信息的接口，将上一次配置的RRU信息读取出来并采用。
 * input：
 * 		1、msg，指向一段内存的指针，用于存储读取出来的RRU系统信息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cprix_read_info(char *msg, int i, int cpri_num)
{
	int fd, ret;
	char path[30];

	sprintf(path, "./cpriinfo/cpri%d_info", cpri_num);
	//打开存储rru系统信息的文件，如果打开失败则返回-1
	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		perror(path);
		goto fd_error;
	}

	//定位到要读取rru系统信息中的某一项信息的开头位置，如果读取失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror(path);
		goto error;
	}

	//读取rru系统信息中的某一项信息，如果读取失败就关闭文件并返回-1
	ret = read(fd, msg, 512);
	if(ret < 0)
	{
		perror(path);
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
 * 函数名：int cpri_read_info(char *msg, int i)
 * 功能描述：读取RRU系统信息的接口，将上一次配置的RRU信息读取出来并采用。
 * input：
 * 		1、msg，指向一段内存的指针，用于存储读取出来的RRU系统信息；
 *		2、i， 用于指明是哪部份的系统信息。
 * output：
 * 		成功：ret，读出数据的size；
 * 		失败：-1
 */
int cpri_read_info(char *msg, int i)
{
	int fd, ret;
	char path[30];

	sprintf(path, "./cpriinfo/cpri_info");
	//打开存储rru系统信息的文件，如果打开失败则返回-1
	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		perror(path);
		goto fd_error;
	}

	//定位到要读取rru系统信息中的某一项信息的开头位置，如果读取失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 512, SEEK_SET);
	if(ret < 0)
	{
		perror(path);
		goto error;
	}

	//读取rru系统信息中的某一项信息，如果读取失败就关闭文件并返回-1
	ret = read(fd, msg, 512);
	if(ret < 0)
	{
		perror(path);
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
 * 函数名：void read_cpirx(void)
 * 功能描述：获取每个cpri口内容不同的IE信息。
 * input：void
 * output：void
 */
void read_cpirx(void)
{
	char msg[512];
	int i, j, ret;

	for(j = 0; j < 8; j++)
	{
		for(i = 1; i < 3; i++)
		{
			memset(msg, 0, 512);
			ret = cprix_read_info(msg, i, j);
			
			if(ret == -1)
				break;
			
			switch(i)
			{
				case 1:		//RRU频段能力IE
					if((*(unsigned short *)msg) == 7)
						memcpy((char *)(&rrufre[j]) + 4, (char *)msg + 4, sizeof(CL_RRUFRE) - 4);
					break;
				case 2:		//RRU射频通道能力IE
					if((*(unsigned short *)msg) == 8)
						memcpy((char *)(&rrurf[j]) + 4, (char *)msg + 4, sizeof(CL_RRURF) - 4);
					break;
			}
		}
	}
}

/*
 * 函数名：void read_cpir(void)
 * 功能描述：获取每个cpri口内容相同的IE信息。
 * input：void
 * output：void
 */
void read_cpir(void)
{
	char msg[512];
	int i, j, ret;

	for(i = 1; i < 9; i++)
	{
		memset(msg, 0, 512);
		ret = cpri_read_info(msg, i);

		if(ret == -1)
			break;
		
		for(j = 0; j < 8; j++)
		{
			switch(i)
			{
				case 1:		//RRU产品标识IE
					if((*(unsigned short *)msg) == 1)
						memcpy((char *)(&porid[j]) + 4, (char *)msg + 4, sizeof(CL_PROID) - 4);
					break;
				case 2:		//通道建立原因IE
					if((*(unsigned short *)msg) == 2)
						memcpy((char *)(&linktype[j]) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE) - 4);
					break;
				case 3:		//RRU能力IE
					if((*(unsigned short *)msg) == 3)
						memcpy((char *)(&rrucapa[j]) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA) - 4);
					break;
				case 4:		//RRU级数IE
					if((*(unsigned short *)msg) == 4)
						rrulv[j].rru_lv = ((CL_RRULV *)msg)->rru_lv;
					break;
				case 5:		//RRU硬件类型及版本信息IE
					if((*(unsigned short *)msg) == 5)
						memcpy((char *)(&rruinfo[j]) + 4, (char *)msg + 4, sizeof(CL_RRUINFO) - 4);
					break;
				case 6:		//RRU软件版本信息IE
					if((*(unsigned short *)msg) == 6)
						memcpy((char *)(&rrusoftinfo[j]) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO) - 4);
					break;
				case 7:		//载波能力组合IE
					if((*(unsigned short *)msg) == 9)
						memcpy((char *)(&rrucir[j]) + 4, (char *)msg + 4, sizeof(CL_RRUCIR) - 4);
				case 8:	//CPU占用率统计周期响应IE
					if((*(unsigned short *)msg) == 452)
						memcpy((char *)(&ratecycans[j]) + 4, (char *)msg + 4, sizeof(PQ_RATECYCANS) - 4);
					break;
			}
		}
	}
}

/*
 * 函数名：void verinfo_write(int type, char name[16], char verinfo[40])
 * 功能描述：记录版本下载的信息，并将要激活的主程序路径写入到执行路径中。
 * input：
 * 		1、type，传入执行的类型，分为下载并激活，下载，激活；
 *		2、name，主程序文件名；
 *		3、verinfo，软件版本号。
 * output：void
 */
void softinfo_write(int type, char name[16], char verinfo[40])
{
	int cur_fd = -1, old_fd = -1, new_fd = -1;
	char buf[60];

	memset(buf, 0, 60);
	
	if(type == 0)										//下载并激活
	{
		new_fd = open("./verinfo/new_soft", O_RDWR | O_TRUNC);
		cur_fd = open("./verinfo/cur_soft", O_RDWR);
		old_fd = open("./verinfo/old_soft", O_RDWR);
		
		write(new_fd, name, strlen(name));
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, verinfo, strlen(verinfo));
	
		read(cur_fd, buf, 56);
		write(old_fd, buf, 56);
		close(old_fd);

		memset(buf, 0, 60);
		lseek(new_fd, 0, SEEK_SET);
		lseek(cur_fd, 0, SEEK_SET);
		
		read(new_fd, buf, 56);
		write(cur_fd, buf, 56);
		close(cur_fd);
		close(new_fd);
	}else if(type == 1)				//下载
	{
		new_fd = open("./verinfo/new_soft", O_RDWR | O_TRUNC);
		
		write(new_fd, name, strlen(name));
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, verinfo, strlen(verinfo));
		close(new_fd);
	}else												//激活
	{
		new_fd = open("./verinfo/new_soft", O_RDWR);
		cur_fd = open("./verinfo/cur_soft", O_RDWR);
		old_fd = open("./verinfo/old_soft", O_RDWR);
	
		read(cur_fd, buf, 56);
		write(old_fd, buf, 56);
		close(old_fd);

		memset(buf, 0, 60);
		lseek(cur_fd, 0, SEEK_SET);
		
		read(new_fd, buf, 56);
		write(cur_fd, buf, 56);
		close(cur_fd);
		close(new_fd);
	}
}

/*
 * 函数名：void verinfo_write(int type, char name[16], char verinfo[40])
 * 功能描述：记录版本下载的信息，并将要激活的主程序路径写入到执行路径中。
 * input：
 * 		1、type，传入执行的类型，分为下载并激活，下载，激活；
 *		2、name，主程序文件名；
 *		3、verinfo，软件版本号。
 * output：void
 */
void firminfo_write(int type, char name[16], char verinfo[40])
{
	int cur_fd = -1, old_fd = -1, new_fd = -1;
	char buf[60];

	memset(buf, 0, 60);
	
	if(type == 0)										//下载并激活
	{
		new_fd = open("./verinfo/new_firm", O_RDWR | O_TRUNC);
		cur_fd = open("./verinfo/cur_firm", O_RDWR);
		old_fd = open("./verinfo/old_firm", O_RDWR);
		
		write(new_fd, name, strlen(name));
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, verinfo, strlen(verinfo));
	
		read(cur_fd, buf, 56);
		write(old_fd, buf, 56);
		close(old_fd);

		memset(buf, 0, 60);
		lseek(new_fd, 0, SEEK_SET);
		lseek(cur_fd, 0, SEEK_SET);
		
		read(new_fd, buf, 56);
		write(cur_fd, buf, 56);
		close(cur_fd);
		close(new_fd);
	}else if(type == 1)				//下载
	{
		new_fd = open("./verinfo/new_firm", O_RDWR | O_TRUNC);
		
		write(new_fd, name, strlen(name));
		lseek(new_fd, 16, SEEK_SET);
		write(new_fd, verinfo, strlen(verinfo));
		close(new_fd);
	}else												//激活
	{
		new_fd = open("./verinfo/new_firm", O_RDWR);
		cur_fd = open("./verinfo/cur_firm", O_RDWR);
		old_fd = open("./verinfo/old_firm", O_RDWR);
	
		read(cur_fd, buf, 56);
		write(old_fd, buf, 56);
		close(old_fd);

		memset(buf, 0, 60);
		lseek(cur_fd, 0, SEEK_SET);
		
		read(new_fd, buf, 56);
		write(cur_fd, buf, 56);
		close(cur_fd);
		close(new_fd);
	}
}


/*
 * 函数名：void exit_fun(void)
 * 功能描述：退出进程前，执行的功能函数，主要执行复位fpga功能。
 * input：void
 * output：void
 */
void exit_fun(void)
{
#ifdef PPC
	eblc_write(0, 0, 0);	//复位FPGA
	eblc_write(1, 0, 0);
#endif
	sync();
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
	CL_LINKTYPE lt;
	char msg[512];
	int i, j, ret;

	atexit(exit_fun);

	//读取记录的cpri相关信息，包括通道建立请求的所有IE消息体，以及cpu占用率的统计周期
	read_cpir();
	read_cpirx();

	//在读取上一次通道建立原因后，再将原因重置为上电启动
	lt.ie_id = 2;
	lt.ie_size = 9;
	lt.link_type = 2;
	lt.reboot_code = 0;
	cpri_write_info((char *)&lt, 2);
	
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

	//根据cpu占用率统计周期是否设置，rate_cyc初始为10
	new_value.it_value.tv_sec = 1;
	new_value.it_value.tv_usec = 0;
	new_value.it_interval.tv_sec = ratecycans[0].rate_cyc;
	new_value.it_interval.tv_usec = 0;
	ret = setitimer(ITIMER_REAL, &new_value, NULL);

	if(ret == -1)
		perror("read cpu rate");
}
