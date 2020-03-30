#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri_comch_req(int sk, const BBU_HEAD cpri_ans, const int cpri_num);
int cpri_comch_cfg(int sk, char *msg, const int cpri_num);
int down_file(char local_file[26], char server_file[220], const int cpri_num);
void cpri_comch_init(int sk, char *msg, const BBU_HEAD cpri_ans, const int cpri_num);
int cpri_verdown_que(int sk, char *msg, const int cpri_num);
int cpri_veract_ind(int sk, char *msg, const int cpri_num);
int cpri_state_que(int sk, char *msg, const int cpri_num);
int cpri_para_que(int sk, char *msg, const int cpri_num);
int cpri_paracfg_que(int sk, char *msg, const int cpri_num);
int cpri_delaymse_que(int sk, char *msg, const int cpri_num);
int cpri_ala_que(int sk, char *msg, const int cpri_num);
int cpri_logup_que(int sk, char *msg, const int cpri_num);
int cpri_reset_ind(int sk, char *msg, const int cpri_num);
int cpri_bbubeat_msg(int sk, char *msg, int *num, const int cpri_num);
int cpri_lte_cfg(int sk, char *msg, const int cpri_num);
int cpri_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_testype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_set_systime(PC_SYSTIME *cl_systime);


#endif
