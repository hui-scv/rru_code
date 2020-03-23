#ifndef _FTP_CLIENT__
#define _FTP_CLIENT__

int ftp_down(char *net_name, unsigned char server_ip[4], char *local_file, char *server_file, char cpri_num);
int ftp_up(char *net_name, unsigned char server_ip[4], char *local_file, char *server_file, char cpri_num);

#endif
