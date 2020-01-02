#include <stdio.h>
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

void init(void)
{
	driver_init();
	ad_da_init();
	time_init();
	rf_init();
}
