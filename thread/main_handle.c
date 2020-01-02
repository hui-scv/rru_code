#include <stdio.h>
#include <string.h>
#include "../include_pc/thread/main_handle.h"
#include "../include_pc/struct_pc.h"


int inftype_handle(char *msg, unsigned short ie_id)
{
	switch(ie_id)
	{
		case 1:
			memcpy(porid_pc.rru_manu, ((CL_PROID_PC *)msg)->rru_manu, 16);
			memcpy(porid_pc.rru_pub, ((CL_PROID_PC *)msg)->rru_pub, 16);
			memcpy(porid_pc.pro_num, ((CL_PROID_PC *)msg)->pro_num, 16);
			memcpy(porid_pc.birth_date, ((CL_PROID_PC *)msg)->birth_date, 16);
			memcpy(porid_pc.used_date, ((CL_PROID_PC *)msg)->used_date, 16);
			memcpy(porid_pc.addi_data, ((CL_PROID_PC *)msg)->addi_data, 16);
			printf("%s\n", porid_pc.rru_manu);
			break;
		case 2:

			break;
		case 3:

			break;
		case 4:

			break;
		case 5:

			break;
		case 6:

			break;
		case 7:

			break;
		case 8:

			break;
		case 9:

			break;
		case 11:

			break;
		case 13:

			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return 0;
}

int statype_handle(char *msg, unsigned short ie_id)
{


}

int alatype_handle(char *msg, unsigned short ie_id)
{


}

int testype_handle(char *msg, unsigned short ie_id)
{


}

