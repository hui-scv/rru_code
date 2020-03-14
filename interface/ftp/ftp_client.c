#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

#include "struct.h"

/*
 * FTP Retrun Code
 * 200 SUCCESS, 202 command not execute
 * 220 ready, 230 login
 * 250 file finish
 */

char gRevBuf[8][512];

// ftp cmd
static int ftpcmd(int gCmdSock, const char *s1, const char * s2, char cpri_num)
{
	int iRet;

	char cmdBuf[256] = {0};
	if(s1)
	{
		sprintf(cmdBuf, (s2?"%s %s\r\n":"%s\r\n"), s1, s2);
		send(gCmdSock, cmdBuf, strlen(cmdBuf), 0);
	}

	do{
		memset(gRevBuf[cpri_num], 0 , 512);
		strcpy(gRevBuf[cpri_num], "EOF");
		if(recv(gCmdSock, gRevBuf[cpri_num], sizeof(gRevBuf[cpri_num])-2, 0 ) < 0)
		{
			return 0;
		}
	}while(!isdigit(gRevBuf[cpri_num][0]) || gRevBuf[cpri_num][3] != ' ');
	gRevBuf[cpri_num][3] = '\0';
	iRet = atoi(gRevBuf[cpri_num]);
	gRevBuf[cpri_num][3] = ' ';

	return iRet;
}

int download(char *net_name, int gCmdSock, char * local_path, char * server_path, char *server_ip, char cpri_num)
{
	int rd;
	char * buf_ptr;
	int port_num;
	int fd_local;
	int gDataSock;
	struct sockaddr_in ftpData_addr;
	struct ifreq ifreq;
	char buffer[4096] = {0};

	// Get the port of data tranfor
	if(ftpcmd(gCmdSock, "PASV", NULL, cpri_num) != 227)
	{
		fprintf(stderr, "Chang passive mode error\n");
		return -4;		//获取下载文件的端口号，如果获取失败则返回-4
	}
	printf("Enter passive mode\n");
	printf("%s\n", gRevBuf[cpri_num]);

	buf_ptr = strrchr(gRevBuf[cpri_num], ')');
	if( buf_ptr )
			*buf_ptr = '\0';

	buf_ptr = strrchr(gRevBuf[cpri_num], ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(gRevBuf[cpri_num], ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;
	printf("The data socket port is %d.\n", port_num);

	// connent the data socket
	memset(&ftpData_addr, 0, sizeof(ftpData_addr));
	ftpData_addr.sin_family = AF_INET;
	ftpData_addr.sin_addr.s_addr = inet_addr(server_ip);
	ftpData_addr.sin_port = htons(port_num);

	gDataSock = socket(AF_INET, SOCK_STREAM, 0 );
	if(gDataSock < 0)
	{
		fprintf(stderr, "Create data socket error.\n");
		return -99;
	}

	strncpy(ifreq.ifr_name, net_name, strlen(net_name));
	setsockopt(gCmdSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq));

	if(connect(gDataSock, (struct sockaddr *)&ftpData_addr, sizeof(ftpData_addr)) < 0)
	{
		fprintf(stderr, "Connect data socket error.\n");
		close(gDataSock);
		return -98;
	}

	if(ftpcmd(gCmdSock, "RETR", server_path, cpri_num) > 150)
	{
		fprintf(stderr, "RETR data error.\n");
		close(gDataSock);
		return -5;		//当获取文件失败时，包括ftp服务器端的文件不存在，返回-5
	}

	fd_local = open(local_path, (O_CREAT | O_TRUNC | O_WRONLY), S_IRUSR | S_IWUSR | S_IXUSR);
	if(fd_local < 0)
	{
		fprintf(stderr, "open local file error.\n");
		close(gDataSock);
		return -6;		//创建本地文件失败，返回-6
	}

	while(1)
	{
		rd = recv(gDataSock, buffer, 4096, 0);
		printf("rd=%d\n",rd);
		if( rd <= 0 )
				break;
		write(fd_local, buffer, rd);
		//sleep(1);
	}

	if (ftpcmd(gCmdSock, NULL, NULL, cpri_num) != 226) {
		fprintf(stderr, "Trans file error.\n");
		close(fd_local);
		close(gDataSock);
		return -7;		//下载文件失败，没有下载成功，返回-7
	}

	close(fd_local);
	close(gDataSock);
	return 0;
}

int upload(char *net_name, int gCmdSock, char * local_path, char * server_path, char *server_ip, char cpri_num)
{
	int rd;
	char * buf_ptr;
	int port_num;
	int fd_local;
	int gDataSock;
	struct sockaddr_in ftpData_addr;
	struct ifreq ifreq;
	char buffer[4096] = {0};

	// Get the port of data tranfor
	if(ftpcmd(gCmdSock, "PASV", NULL, cpri_num)!=227)
	{
		fprintf(stderr, "Chang passive mode error\n");
		return -4;		//获取下载文件的端口号，如果获取失败则返回-4
	}
	printf("Enter passive mode\n");
	printf("%s\n", gRevBuf[cpri_num]);

	buf_ptr = strrchr(gRevBuf[cpri_num], ')');
	if( buf_ptr )
		*buf_ptr = '\0';

	buf_ptr = strrchr(gRevBuf[cpri_num], ',');
	*buf_ptr = '\0';
	port_num = atoi(buf_ptr + 1);

	buf_ptr = strrchr(gRevBuf[cpri_num], ',');
	*buf_ptr = '\0';
	port_num += atoi(buf_ptr + 1) * 256;
	printf("The data socket port is %d.\n", port_num);

	// connent the data socket
	memset(&ftpData_addr, 0, sizeof(ftpData_addr));
	ftpData_addr.sin_family = AF_INET;
	ftpData_addr.sin_addr.s_addr = inet_addr(server_ip);
	ftpData_addr.sin_port = htons(port_num);

	gDataSock = socket(AF_INET, SOCK_STREAM, 0 );
	if(gDataSock < 0)
	{
		fprintf(stderr, "Create data socket error.\n");
		return -99;
	}

	strncpy(ifreq.ifr_name, net_name, strlen(net_name));
	setsockopt(gCmdSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq));

	if(connect(gDataSock, (struct sockaddr *)&ftpData_addr, sizeof(ftpData_addr)) < 0)
	{
		fprintf(stderr, "Connect data socket error.\n");
		close(gDataSock);
		return -98;
	}

	fd_local = open(local_path, (O_RDWR));
	if(fd_local < 0)
	{
		fprintf(stderr, "open local file error.\n");
		close(gDataSock);
		return -6;		//打开本地文件失败，返回-6
	}

	if(ftpcmd(gCmdSock, "STOR", server_path, cpri_num) > 150)
	{
		fprintf(stderr, "STOR data error.\n");
		close(fd_local);
		close(gDataSock);
		return -5;		//当上传文件失败时，包括本地文件不存在，返回-5
	}

	while(1)
	{
		rd = read(fd_local, buffer, 4096);
		send(gDataSock, buffer, rd, 0);
		printf("wr=%d\n",rd);
		if(rd < 4096)
			break;
		//sleep(1);
	}
	close(gDataSock);

	if (ftpcmd(gCmdSock, NULL, NULL, cpri_num) != 226) {
		fprintf(stderr, "up file error.\n");
		close(fd_local);
		return -7;		//上传文件失败，没有上传成功，返回-7
	}

	close(fd_local);
	return 0;
}

int ftp_down(char *net_name, char *server_ip, char *local_file, char *server_file, char cpri_num)
{
	struct sockaddr_in ftpSrv_addr;
	struct ifreq ifreq;
	int gCmdSock, ret;

	memset(&ifreq, 0, sizeof(struct ifreq));

	// Create socket
	gCmdSock = socket(AF_INET, SOCK_STREAM, 0);
	if(gCmdSock < 0)
	{
		fprintf(stderr, "Create socket error\n");
		return -99;
	}
	
	strncpy(ifreq.ifr_name, net_name, strlen(net_name));
	setsockopt(gCmdSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq));

	// Connect to Ftp server
	memset(&ftpSrv_addr, 0, sizeof(ftpSrv_addr));
	ftpSrv_addr.sin_family = AF_INET;
	ftpSrv_addr.sin_addr.s_addr = inet_addr(server_ip);
	ftpSrv_addr.sin_port = htons(21);

	if(connect(gCmdSock, (struct sockaddr *)&ftpSrv_addr, sizeof(ftpSrv_addr)) < 0)
	{
		fprintf(stderr, "Connect socket error\n");
		close(gCmdSock);
		return -98;
	}

	// Check if connect OK
	if(ftpcmd(gCmdSock, NULL, NULL, cpri_num) != 220)
	{
		fprintf(stderr, "FTP server not ready error\n");
		close(gCmdSock);
		return -1;			//ftp服务器没有准备就绪，返回-1
	}
	printf("FTP READY!\n");
	// Login
	switch(ftpcmd(gCmdSock, "USER", ftpinfo[cpri_num].usr, cpri_num))
	{
		case 230:
			break;
		case 331:
			if(ftpcmd(gCmdSock, "PASS", ftpinfo[cpri_num].pwd, cpri_num) != 230)
			{
				fprintf(stderr, "Wrong password\n");
				close(gCmdSock);
				return -3;		//ftp登录时，密码错误，返回-3
			}
			break;
		default:
			fprintf(stderr, "Wrong user\n");
			close(gCmdSock);
			return -2;		//ftp登录时，用户名错误，返回-2
	}
	// Tansfor mode
	ftpcmd(gCmdSock, "TYPE I", NULL, cpri_num);
	printf("Login Success!\n");

	ret = download(net_name, gCmdSock, local_file, server_file, server_ip, cpri_num);

	ftpcmd(gCmdSock, "QUIT", NULL, cpri_num);
	close(gCmdSock);
	return ret;
}

int ftp_up(char *net_name, char *server_ip, char *local_file, char *server_file, char cpri_num)
{
	struct sockaddr_in ftpSrv_addr;
	struct ifreq ifreq;
	int gCmdSock, ret;

	memset(&ifreq, 0, sizeof(struct ifreq));

	// Create socket
	gCmdSock = socket(AF_INET, SOCK_STREAM, 0);
	if(gCmdSock < 0)
	{
		fprintf(stderr, "Create socket error\n");
		return -99;
	}

	strncpy(ifreq.ifr_name, net_name, strlen(net_name));
	setsockopt(gCmdSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifreq, sizeof(ifreq));

	// Connect to Ftp server
	memset(&ftpSrv_addr, 0, sizeof(ftpSrv_addr));
	ftpSrv_addr.sin_family = AF_INET;
	ftpSrv_addr.sin_addr.s_addr = inet_addr(server_ip);
	ftpSrv_addr.sin_port = htons(21);

	if(connect(gCmdSock, (struct sockaddr *)&ftpSrv_addr, sizeof(ftpSrv_addr)) < 0)
	{
		fprintf(stderr, "Connect socket error\n");
		close(gCmdSock);
		return -98;
	}

	// Check if connect OK
	if(ftpcmd(gCmdSock, NULL, NULL, cpri_num) != 220)
	{
		fprintf(stderr, "FTP server not ready error\n");
		close(gCmdSock);
		return -1;			//ftp服务器没有准备就绪，返回-1
	}
	printf("FTP READY!\n");
	// Login
	switch(ftpcmd(gCmdSock, "USER", ftpinfo[cpri_num].usr, cpri_num))
	{
		case 230:
			break;
		case 331:
			if(ftpcmd(gCmdSock, "PASS", ftpinfo[cpri_num].pwd, cpri_num) != 230)
			{
				fprintf(stderr, "Wrong password\n");
				close(gCmdSock);
				return -3;		//ftp登录时，密码错误，返回-3
			}
			break;
		default:
			fprintf(stderr, "Wrong user\n");
			close(gCmdSock);
			return -2;		//ftp登录时，用户名错误，返回-2
	}
	// Tansfor mode
	ftpcmd(gCmdSock, "TYPE I", NULL, cpri_num);
	printf("Login Success!\n");

	ret = upload(net_name, gCmdSock, local_file, server_file, server_ip, cpri_num);

	ftpcmd(gCmdSock, "QUIT", NULL, cpri_num);
	close(gCmdSock);
	return ret;
}
