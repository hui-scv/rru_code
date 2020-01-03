#include <stdio.h>
#include <pthread.h>
#include "include/init.h"
#include "include_pc/init_struct_pc.h"
#include "include/init_struct.h"
#include "include/thread/rru_thread.h"


void main()
{
	int ret = 0;
	pthread_t thread_id[10];

	init();

	ret = pthread_create(&thread_id[0], NULL, (void *)maintain_thread, NULL);
	if(ret != 0)
	{
		printf("create mainten_thread error!\n");
		return;
	}

	ret = pthread_create(&thread_id[9], NULL, (void *)ptran_thread, NULL);
	if(ret != 0)
	{
		printf("create ptran_thread error!\n");
		return;
	}

	pthread_join(thread_id[9], NULL);
}
