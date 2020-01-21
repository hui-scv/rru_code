#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri6_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri6_comch_cfg(int sk, char *msg);
void cpri6_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri6_verdown_que(int sk, char *msg);
int cpri6_veract_ind(int sk, char *msg);
int cpri6_state_que(int sk, char *msg);
int cpri6_para_que(int sk, char *msg);
int cpri6_paracfg_que(int sk, char *msg);
int cpri6_delaymse_que(int sk, char *msg);
int cpri6_ala_que(int sk, char *msg);
int cpri6_logup_que(int sk, char *msg);
int cpri6_reset_ind(int sk, char *msg);
int cpri6_bbubeat_msg(int sk, char *msg, int *num);
int cpri6_lte_cfg(int sk, char *msg);
int cpri6_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri6_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri6_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri6_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
