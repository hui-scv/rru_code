#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri4_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri4_comch_cfg(int sk, char *msg);
void cpri4_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri4_verdown_que(int sk, char *msg);
int cpri4_veract_ind(int sk, char *msg);
int cpri4_state_que(int sk, char *msg);
int cpri4_para_que(int sk, char *msg);
int cpri4_paracfg_que(int sk, char *msg);
int cpri4_delaymse_que(int sk, char *msg);
int cpri4_ala_que(int sk, char *msg);
int cpri4_logup_que(int sk, char *msg);
int cpri4_reset_ind(int sk, char *msg);
int cpri4_bbubeat_msg(int sk, char *msg, int *num);
int cpri4_lte_cfg(int sk, char *msg);
int cpri4_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri4_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri4_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri4_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
