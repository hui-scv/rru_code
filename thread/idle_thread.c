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
extern int sock[8];
extern BBU_HEAD cprians[8];


/*
 * 函数名：void get_stamp(char *time_stamp)
 * 功能描述：获取时间戳。
 * input：void
 * output：void
 */
void get_stamp(char time_stamp[22])
{
	time_t t;
	struct tm area;

	//获取系统时间，并转换成struct tm结构体的格式
	t = time(NULL);
	localtime_r(&t, &area);

	//获取时间戳，包括年月日时分秒
	sprintf(time_stamp, "%d-%02d-%02d %02d:%02d:%02d", area.tm_year + 1900, area.tm_mon + 1, area.tm_mday, area.tm_hour, area.tm_min, area.tm_sec);
}

/*
 * 函数名：static send_err(int ala_code, int ala_subcode, int ala_flag, char *stamp, char *err_msg, int cpri_num)
 * 功能描述：上报告警信息。
 * input：void
 * output：void
 */
static send_err(int ala_code, int ala_subcode, int ala_flag, char *stamp, char *err_msg, int cpri_num)
{
	MSG_HEAD msg_head;
	char send_msg[160];

	alarep[cpri_num].ala_able = 0;
	alarep[cpri_num].ala_code = ala_code;
	alarep[cpri_num].ala_subcode = ala_subcode;
	alarep[cpri_num].ala_flag = ala_flag;
	memcpy(alarep[cpri_num].time_stamp, stamp, 20);
	memcpy(alarep[cpri_num].addi_data, err_msg, strlen(err_msg)-1);
	
	msg_head.msg_id = CPRI_ALAREP_QUE;
	msg_head.msg_size = MSG_HEADSIZE + alarep[cpri_num].ie_size;
	msg_head.rru_id = cprians[cpri_num].rru_id;
	msg_head.bbu_id = cprians[cpri_num].bbu_id;
	msg_head.ray_id = cprians[cpri_num].bbu_num;
	msg_head.inc_num = 1;
	
	memcpy(send_msg, (char *)&msg_head, MSG_HEADSIZE);
	memcpy(send_msg + MSG_HEADSIZE, (char *)(&alarep[cpri_num]), sizeof(AR_ALAREP));
	
	send(sock[cpri_num], send_msg, msg_head.msg_size, 0);
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
	memset(&status, 0, sizeof(STATUS_S));

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

#ifdef PPC
			status_read(cpri_num/4, &status);
#endif

			/* 1、RRU输入电源监控 */
			if(status.vccin_voltage < 2.8 || status.vccin_voltage > 5.8)
			{
				if(((cpri_ala_flag[cpri_num] >> 0) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 0);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s RRU输入电源监控告警,00001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(1, 0, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 0) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 0));		//清除故障标志
					send_err(1, 0, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 4、电源故障告警 */
			if(status.vccin_voltage < 2.8 || status.vccin_voltage > 5.8)
			{
				if(((cpri_ala_flag[cpri_num] >> 3) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 3);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 电源故障告警,30001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);
					
					send_err(30001, 0, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 3) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 3));		//清除故障标志
					send_err(30001, 0, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 5、功放过温告警 */
			if(status.fpga_tempatrue < temthrans[cpri_num].tem_low || status.fpga_tempatrue > temthrans[cpri_num].tem_max)
			{
				if(((cpri_ala_flag[cpri_num] >> 4) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 4);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 功放过温告警,40001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(40001, cpri_num, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 4) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 4));		//清除故障标志
					send_err(40001, cpri_num, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 6、本振失锁告警 */
			if(status.ad_clk_pll_state == 0 || status.da_clk_pll_state == 0)
			{
				if(((cpri_ala_flag[cpri_num] >> 5) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 5);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 本振失锁告警,50001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(50001, 0, 0, time_stamp, erro_msg, cpri_num);
#ifdef PPC
					ad_enable(cpri_num/4, 1, 0);			//关闭所有ad/da通道
					ad_enable(cpri_num/4, 2, 0);
					ad_enable(cpri_num/4, 3, 0);
					ad_enable(cpri_num/4, 4, 0);
					da_recall_enable(cpri_num/4, 0, 0, 0);
					da_recall_enable(cpri_num/4, 0, 1, 0);
					da_recall_enable(cpri_num/4, 1, 0, 0);
					da_recall_enable(cpri_num/4, 1, 1, 0);
#endif
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 5) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 5));		//清除故障标志
					send_err(50001, 0, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 7、上行通道故障告警 */
			if(status.ad_work_state == 0)
			{
				if(((cpri_ala_flag[cpri_num] >> 6) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 6);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 上行通道故障告警,60001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(60001, cpri_num, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 6) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 6));		//清除故障标志
					send_err(60001, cpri_num, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 8、下行通道故障告警 */
			if(status.da_work_state == 0)
			{
				if(((cpri_ala_flag[cpri_num] >> 7) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 7);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 下行通道故障告警,70001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(70001, cpri_num, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 7) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 7));		//清除故障标志
					send_err(70001, cpri_num, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 16、RRU处理器过载告警 */
			if(cpurateans[0].cpu_rate > 50)
			{
				if(((cpri_ala_flag[cpri_num] >> 15) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 15);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s RRU处理器过载告警,160001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(160001, 0, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 15) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 15));		//清除故障标志
					send_err(160001, 0, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 22、板卡过温告警 */
			if(status.fpga_tempatrue < temthrans[cpri_num].tem_low || status.fpga_tempatrue > temthrans[cpri_num].tem_max)
			{
				if(((cpri_ala_flag[cpri_num] >> 21) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 21);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 板卡过温告警,220001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(220001, cpri_num, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 21) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 21));		//清除故障标志
					send_err(220001, cpri_num, 1, time_stamp, erro_msg, cpri_num);
				}
			}

			/* 23、RRU内部时钟失锁告警 */
			if(status.sys_clk_pll_state == 0)
			{
				if(((cpri_ala_flag[cpri_num] >> 22) & 0x01) == 0)
				{
					cpri_ala_flag[cpri_num] |= (0x00000001 << 22);		//设置故障标志		
					//获取系统时间戳
					get_stamp(time_stamp);
					sprintf(erro_msg, "%s 板卡过温告警,230001\n", time_stamp);
					ret = write(fd, erro_msg, strlen(erro_msg));
					if(ret < 0)
						perror(erro_msg);

					send_err(230001, 0, 0, time_stamp, erro_msg, cpri_num);
				}
			}else
			{
				if((cpri_ala_flag[cpri_num] >> 22) & 0x01)
				{
					cpri_ala_flag[cpri_num] &= (~(0x00000001 << 22));		//清除故障标志
					send_err(230001, 0, 1, time_stamp, erro_msg, cpri_num);
				}
			}
			
			close(fd);
		}
	}
}
