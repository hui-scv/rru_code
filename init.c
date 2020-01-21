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

int cpri_read_str(char *msg, int i)
{
	int fd, ret;

	fd = open("./rru_info.dat", O_RDONLY);
	if(fd < 0)
		goto fd_error;

	ret = lseek(fd, (i - 1) * 510, SEEK_SET);
	if(ret < 0)
	{
		goto error;
	}

	ret = read(fd, msg, 510);
	if(ret < 0)
	{
		goto error;
	}
	
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	printf("read_str error!\n");
	return -1;
}

void init(void)
{
	char msg[512];
	int i, j;
	//pthread_rwlock_init(&chlinkque_rwlock, NULL);

	for(i = 1; i < 10; i++)
	{
		memset(msg, 0, 512);
		cpri_read_str(msg, i);
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
	
	driver_init();
	ad_da_init();
	time_init();
	rf_init();
}
