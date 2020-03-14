#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri8_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri8_comch_cfg(int sk, char *msg);
void cpri8_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri8_verdown_que(int sk, char *msg);
int cpri8_veract_ind(int sk, char *msg);
int cpri8_state_que(int sk, char *msg);
int cpri8_para_que(int sk, char *msg);
int cpri8_paracfg_que(int sk, char *msg);
int cpri8_delaymse_que(int sk, char *msg);
int cpri8_ala_que(int sk, char *msg);
int cpri8_logup_que(int sk, char *msg);
int cpri8_reset_ind(int sk, char *msg);
int cpri8_bbubeat_msg(int sk, char *msg, int *num);
int cpri8_lte_cfg(int sk, char *msg);
int cpri8_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri8_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri8_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri8_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
