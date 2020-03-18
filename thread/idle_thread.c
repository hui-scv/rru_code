/*
 * 文件名：idle_thread.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：空闲任务处理线程，主要用于监控系统状态，记录错误信息。
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
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "struct.h"
#include "usr.h"

extern unsigned int cpri_ala_flag[8];


/*
 * 函数名：void get_stamp(char *time_stamp)
 * 功能描述：获取时间戳。
 * input：void
 * output：void
 */
void get_stamp(char *time_stamp)
{
	time_t t;
	struct tm area;

	//获取系统时间，并转换成struct tm结构体的格式
	t = time(NULL);
	localtime_r(&t, &area);

	//获取时间戳，包括年月日时分秒
	sprintf(time_stamp, "%d.%02d.%02d_%02d:%02d:%02d-", area.tm_year + 1900, area.tm_mon + 1, area.tm_mday, area.tm_hour, area.tm_min, area.tm_sec);
}

/*
 * 函数名：void idle_handle(void)
 * 功能描述：空闲任务函数，用于监控系统状态，记录错误信息。
 * input：void
 * output：void
 */
void idle_handle(void)
{
	char file_name[20], time_stamp[22], erro_msg[128];
	int cpri_num = 0, fd = -1, ret = -1;
	
	STATUS_S status;

	memset(file_name, 0, sizeof(file_name));

	while(1)
	{
		for(cpri_num = 0; cpri_num < 8; cpri_num++)
		{
			sleep(1);		//每隔8s检测一个通道的状态
			if(chlinkans[cpri_num].res == 1)	//如果通道未建立连接，则不检测此通道的状态
				continue;

			sprintf(file_name, "/cpri%d_error_log", cpri_num);
			fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if(fd < 0)
			{
				perror(file_name);
				continue;
			}

			/* 1、RRU输入电源监控 */
#ifdef PPC
			status_read(cpri_num/4, &status);
#endif
			if(status.vccin_voltage < 2.8 || status.vccin_voltage > 5.8)
			{
				cpri_ala_flag[cpri_num] |= (0x00000001 << 0);		//设置故障标志		
				//获取系统时间戳
				get_stamp(time_stamp);
				sprintf(erro_msg, "%sRRU输入电源告警\n", time_stamp);
				ret = write(fd, erro_msg, strlen(erro_msg));
				if(ret < 0)
					perror(erro_msg);
			}else
			{
				cpri_ala_flag[cpri_num] &= (~(0x00000001 << 0));		//清除故障标志
			}

			close(fd);
		}
	}
}
