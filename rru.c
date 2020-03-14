/*
 * 文件名：rru.c
 * 版本描述：v1.0
 * 编写日期：2020.1.20
 * 文件描述：此文件是系统程序的入口文件，main是入口函数。
 */

#include <stdio.h>
#include <pthread.h>
#include "init.h"
#include "include_pc/init_struct_pc.h"
#include "init_struct.h"
#include "thread/rru_thread.h"

/*
 * 函数名：void main()
 * 功能描述：调用初始化函数init，然后创建10个线程，分别用于8个cpri的事务处理，
 * 		   1个维护口的事务处理和1个透传数据处理。
 * input：void
 * output：void
 */
int main()
{
	int ret = 0, arg[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	pthread_t thread_id[10];

	init();

	//创建维护任务线程
	//ret = pthread_create(&thread_id[0], NULL, (void *)maintain_thread, NULL);
	if(ret != 0)
	{
		printf("create mainten_thread error!\n");
		return;
	}

	//创建cpri接口1任务线程
	ret = pthread_create(&thread_id[1], NULL, (void *)cpri_thread, (void *)&arg[0]);
	if(ret != 0)
	{
		printf("create cpri1_thread error!\n");
		return;
	}
	
	//创建cpri接口2任务线程
	ret = pthread_create(&thread_id[2], NULL, (void *)cpri_thread, (void *)&arg[1]);
	if(ret != 0)
	{
		printf("create cpri2_thread error!\n");
		return;
	}

	//创建cpri接口3任务线程
	ret = pthread_create(&thread_id[3], NULL, (void *)cpri_thread, (void *)&arg[2]);
	if(ret != 0)
	{
		printf("create cpri3_thread error!\n");
		return;
	}

	//创建cpri接口4任务线程
	ret = pthread_create(&thread_id[4], NULL, (void *)cpri_thread, (void *)&arg[3]);
	if(ret != 0)
	{
		printf("create cpri4_thread error!\n");
		return;
	}

	//创建cpri接口5任务线程
	ret = pthread_create(&thread_id[5], NULL, (void *)cpri_thread, (void *)&arg[4]);
	if(ret != 0)
	{
		printf("create cpri5_thread error!\n");
		return;
	}

	//创建cpri接口6任务线程
	ret = pthread_create(&thread_id[6], NULL, (void *)cpri_thread, (void *)&arg[5]);
	if(ret != 0)
	{
		printf("create cpri6_thread error!\n");
		return;
	}

	//创建cpri接口7任务线程
	ret = pthread_create(&thread_id[7], NULL, (void *)cpri_thread, (void *)&arg[6]);
	if(ret != 0)
	{
		printf("create cpri7_thread error!\n");
		return;
	}

	//创建cpri接口8任务线程
	ret = pthread_create(&thread_id[8], NULL, (void *)cpri_thread, (void *)&arg[7]);
	if(ret != 0)
	{
		printf("create cpri7_thread error!\n");
		return;
	}

	//创建数据透传任务线程
	ret = pthread_create(&thread_id[9], NULL, (void *)ptran_thread, NULL);
	if(ret != 0)
	{
		printf("create ptran_thread error!\n");
		return;
	}

	pthread_join(thread_id[9], NULL);
}
