#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri2_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri2_comch_cfg(int sk, char *msg);
void cpri2_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri2_verdown_que(int sk, char *msg);
int cpri2_veract_ind(int sk, char *msg);
int cpri2_state_que(int sk, char *msg);
int cpri2_para_que(int sk, char *msg);
int cpri2_paracfg_que(int sk, char *msg);
int cpri2_delaymse_que(int sk, char *msg);
int cpri2_ala_que(int sk, char *msg);
int cpri2_logup_que(int sk, char *msg);
int cpri2_reset_ind(int sk, char *msg);
int cpri2_bbubeat_msg(int sk, char *msg, int *num);
int cpri2_lte_cfg(int sk, char *msg);
int cpri2_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri2_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri2_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri2_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
