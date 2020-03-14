#ifndef _FTP_CLIENT__
#define _FTP_CLIENT__

int ftp_down(char *net_name, char *server_ip, char *local_file, char *server_file, char cpri_num);
int ftp_up(char *net_name, char *server_ip, char *local_file, char *server_file, char cpri_num);

#endif
