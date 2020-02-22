/*
 * 文件名：init.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：此文件用于初始化RRU系统；初始化驱动、配置ADDA、配置时钟和射频；并提供了读取RRU系统信息的接口。
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
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
		goto fd_error;

	//定位到要读取rru系统信息中的某一项信息的开头位置，如果读取失败就关闭文件并返回-1
	ret = lseek(fd, (i - 1) * 510, SEEK_SET);
	if(ret < 0)
	{
		goto error;
	}

	//读取rru系统信息中的某一项信息，如果读取失败就关闭文件并返回-1
	ret = read(fd, msg, 510);
	if(ret < 0)
	{
		goto error;
	}
	
	//读取成功，关闭文件
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	printf("read_str error!\n");
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
	char msg[512];
	int i, j, ret;
	//pthread_rwlock_init(&chlinkque_rwlock, NULL);

	//按照id号为1-10来读取RRU系统信息的结构体，然后将其存储到8个cpri接口中。
	//这里有个问题就是是否每个cpri的系统信息都相同？如果不同则不可以这样做！
	for(i = 1; i < 10; i++)
	{
		memset(msg, 0, 512);
		ret = cpri_read_str(msg, i);
		for(j = 0; j < 8; j++)
		{
			switch(i)
			{
				case 1:
					//memcpy((char *)(&porid_pc) + 4, (char *)msg + 4, sizeof(CL_PROID_PC) - 4);
					memcpy((char *)(&porid[j]) + 4, (char *)msg + 4, sizeof(CL_PROID) - 4);
					break;
				case 2:
					//memcpy((char *)(&linktype_pc) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE_PC) - 4);
					memcpy((char *)(&linktype[j]) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE) - 4);
					break;
				case 3:
					//memcpy((char *)(&rrucapa_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA_PC) - 4);
					memcpy((char *)(&rrucapa[j]) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA) - 4);
					break;
				case 4:
					//rrulv_pc.rru_lv = ((CL_RRULV_PC *)msg)->rru_lv;
					rrulv[j].rru_lv = ((CL_RRULV *)msg)->rru_lv;
					break;
				case 5:
					//memcpy((char *)(&rruinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUINFO_PC) - 4);
					memcpy((char *)(&rruinfo) + 4, (char *)msg + 4, sizeof(CL_RRUINFO) - 4);
					break;
				case 6:
					//memcpy((char *)(&rrusoftinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO_PC) - 4);
					memcpy((char *)(&rrusoftinfo) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO) - 4);
					break;
				case 7:
					//memcpy((char *)(&rrufre_pc) + 4, (char *)msg + 4, sizeof(CL_RRUFRE_PC) - 4);
					memcpy((char *)(&rrufre) + 4, (char *)msg + 4, sizeof(CL_RRUFRE) - 4);
					break;
				case 8:
					//memcpy((char *)(&rrurf_pc) + 4, (char *)msg + 4, sizeof(CL_RRURF_PC) - 4);
					memcpy((char *)(&rrurf) + 4, (char *)msg + 4, sizeof(CL_RRURF) - 4);
					break;
				case 9:
					//memcpy((char *)(&rrucir_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCIR_PC) - 4);
					memcpy((char *)(&rrucir) + 4, (char *)msg + 4, sizeof(CL_RRUCIR) - 4);
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
}
