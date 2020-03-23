#ifndef _IDLE_THREAD__
#define _IDLE_THREAD__


void get_stamp(char time_stamp[22]);
static void send_err(int ala_code, int ala_subcode, int ala_flag, char *stamp, char *err_msg, int cpri_num);
void idle_handle(void);


#endif
