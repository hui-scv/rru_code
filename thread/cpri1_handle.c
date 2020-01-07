#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include "../include/thread/cpri1_handle.h"
#include "../include_pc/struct_pc.h"
#include "../include/struct.h"



/********************************************RRU信息记录分割线***************************************/
int cpri1_write_str(char *msg, int i);

int cpri1_inftype_handle(char *msg, unsigned short ie_id, int acq)
{
	switch(ie_id)
	{
		case 1:
			memcpy((char *)(&porid_pc) + 4, (char *)msg + 4, sizeof(CL_PROID_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 2:
			memcpy((char *)(&linktype_pc) + 4, (char *)msg + 4, sizeof(CL_LINKTYPE_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 3:
			memcpy((char *)(&rrucapa_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCAPA_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 4:
			rrulv_pc.rru_lv = ((CL_RRULV_PC *)msg)->rru_lv;
			write_str(msg, ie_id);
			break;
		case 5:
			memcpy((char *)(&rruinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUINFO_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 6:
			memcpy((char *)(&rrusoftinfo_pc) + 4, (char *)msg + 4, sizeof(CL_RRUSOFTINFO_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 7:
			memcpy((char *)(&rrufre_pc) + 4, (char *)msg + 4, sizeof(CL_RRUFRE_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 8:
			memcpy((char *)(&rrurf_pc) + 4, (char *)msg + 4, sizeof(CL_RRURF_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 9:
			memcpy((char *)(&rrucir_pc) + 4, (char *)msg + 4, sizeof(CL_RRUCIR_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 11:
			memcpy((char *)(&systime_pc) + 4, (char *)msg + 4, sizeof(CL_SYSTIME_PC) - 4);
			write_str(msg, ie_id);
			break;
		case 13:
			memcpy((char *)(&rrumode_pc) + 4, (char *)msg + 4, sizeof(CL_RRUMODE_PC) - 4);
			write_str(msg, ie_id);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return 0;
}

int cpri1_write_str(char *msg, int i)
{
	int fd, ret;

	fd = open("./rru_info.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd < 0)
		goto fd_error;

	ret = lseek(fd, (i - 1) * 510, SEEK_SET);
	if(ret < 0)
	{
		goto error;
	}

	ret = write(fd, msg, i * 510);
	if(ret < 0)
	{
		goto error;
	}
	
	close(fd);
	return ret;

error:
	close(fd);
fd_error:
	printf("write_str error!\n");
	return -1;
}



/********************************************RRU状态响应分割线***************************************/
int cpri1_statype_handle(char *msg, unsigned short ie_id, int acq)
{
	char ch, send_msg[512] = {0x7e, 0xaa, 0, 0};
	
	switch(ie_id)
	{
		case 302:		//射频通道状态查询
			ch = ((SQ_RFCHSTA_PC *)msg)->ch_num;
			memcpy(&send_msg[2], (char *)&rfchans_pc[ch], sizeof(SQ_RFCHANS_PC));
			send(acq, send_msg, sizeof(SQ_RFCHANS_PC) + 2, 0);
			break;
		case 303:
			memcpy(&send_msg[2], (char *)&cirans_pc, sizeof(SQ_CIRANS_PC));
			send(acq, send_msg, sizeof(SQ_CIRANS_PC) + 2, 0);
			break;
		case 304:
			memcpy(&send_msg[2], (char *)&oscans_pc, sizeof(SQ_OSCANS_PC));
			send(acq, send_msg, sizeof(SQ_OSCANS_PC) + 2, 0);
			break;
		case 305:
			memcpy(&send_msg[2], (char *)&rtcans_pc, sizeof(SQ_RTCANS_PC));
			send(acq, send_msg, sizeof(SQ_RTCANS_PC) + 2, 0);
			break;
		case 306:
			memcpy(&send_msg[2], (char *)&runans_pc, sizeof(SQ_RUNANS_PC));
			send(acq, send_msg, sizeof(SQ_RUNANS_PC) + 2, 0);
			break;
		case 307:
			memcpy(&send_msg[2], (char *)&irmodeans_pc, sizeof(SQ_IRMODEANS_PC));
			send(acq, send_msg, sizeof(SQ_IRMODEANS_PC) + 2, 0);
			break;
		case 309:
			memcpy(&send_msg[2], (char *)&rayans_pc, sizeof(SQ_RAYANS_PC));
			send(acq, send_msg, sizeof(SQ_RAYANS_PC) + 2, 0);
			break;
		case 401:
			memcpy(&send_msg[2], (char *)(&systime), sizeof(CL_SYSTIME));
			((CL_SYSTIME *)&send_msg[2])->ie_id = 5000;
			send(acq, send_msg, sizeof(CL_SYSTIME) + 2, 0);
			break;
		case 402:
			memcpy(&send_msg[2], (char *)&cpurateans_pc, sizeof(PQ_CPURATEANS_PC));
			send(acq, send_msg, sizeof(PQ_CPURATEANS_PC) + 2, 0);
			break;
		case 403:
			memcpy(&send_msg[2], (char *)&ratecycans_pc, sizeof(PQ_RATECYCANS_PC));
			send(acq, send_msg, sizeof(PQ_RATECYCANS_PC) + 2, 0);
			break;
		case 404:
			memcpy(&send_msg[2], (char *)&rrutemans_pc, sizeof(PQ_RRUTEMANS_PC));
			send(acq, send_msg, sizeof(PQ_RRUTEMANS_PC) + 2, 0);
			break;
		case 407:
			memcpy(&send_msg[2], (char *)&temthrans_pc, sizeof(PQ_TEMTHRANS_PC));
			send(acq, send_msg, sizeof(PQ_TEMTHRANS_PC) + 2, 0);
			break;
		case 409:
			memcpy(&send_msg[2], (char *)&stamachans_pc, sizeof(PQ_STAMACHANS_PC));
			send(acq, send_msg, sizeof(PQ_STAMACHANS_PC) + 2, 0);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return 0;
}



/********************************************RRU报警查询分割线***************************************/
int cpri1_alatype_handle(char *msg, unsigned short ie_id, int acq)
{
	char send_msg[512] = {0x7e, 0xaa, 0, 0};
	
	switch(ie_id)
	{
		case 1101:		//报警查询请求
			memcpy(&send_msg[2], (char *)&alarep_pc, sizeof(AR_ALAREP_PC));
			send(acq, send_msg, sizeof(AR_ALAREP_PC) + 2, 0);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return 0;
}



/********************************************RRU射频通道测试分割线***************************************/
int cpri1_testype_handle(char *msg, unsigned short ie_id, int acq)
{
	char send_msg[512] = {0x7e, 0xaa, 0, 0};

	switch(ie_id)
	{
		case 11000:		//
			memcpy((char *)(&rftsen_pc) + 4, (char *)msg + 4, sizeof(TS_RFTSEN_PC) - 4);
			break;
		case 11001:
			memcpy((char *)(&senchset_pc) + 4, (char *)msg + 4, sizeof(TS_SENCHSET_PC) - 4);
			break;
		case 11002:
			memcpy((char *)(&recchset_pc) + 4, (char *)msg + 4, sizeof(TS_RECCHSET_PC) - 4);
			break;
		case 11003:
			memcpy((char *)(&sencirset_pc) + 4, (char *)msg + 4, sizeof(TS_SENCIRSET_PC) - 4);
			memcpy(&send_msg[2], (char *)&sencirans_pc, sizeof(TS_SENCIRANS_PC));
			send(acq, send_msg, sizeof(TS_SENCIRANS_PC) + 2, 0);
			break;
		case 11004:
			memcpy((char *)(&sendimset_pc) + 4, (char *)msg + 4, sizeof(TS_SENDIMSET_PC) - 4);
			break;
		case 11005:
			memcpy((char *)(&cirdataque_pc) + 4, (char *)msg + 4, sizeof(TS_CIRDATAQUE_PC) - 4);
			memcpy(&send_msg[2], (char *)&cirdataans_pc, sizeof(TS_CIRDATAANS_PC));
			send(acq, send_msg, sizeof(TS_CIRDATAANS_PC) + 2, 0);
			break;
		case 11006:
			memcpy((char *)(&reccirdata_pc) + 4, (char *)msg + 4, sizeof(TS_RECCIRDATA_PC) - 4);
			break;
		case 11007:
			memcpy((char *)(&recdimset_pc) + 4, (char *)msg + 4, sizeof(TS_RECDIMSET_PC) - 4);
			break;
		default:
			printf("ie_id error!\n");
			return -2;
	}

	return 0;
}

