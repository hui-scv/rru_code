#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri1_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri1_comch_cfg(int sk, char *msg);
void cpri1_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri1_verdown_que(int sk, char *msg);
int cpri1_veract_ind(int sk, char *msg);
int cpri1_state_que(int sk, char *msg);
int cpri1_para_que(int sk, char *msg);
int cpri1_paracfg_que(int sk, char *msg);
int cpri1_delaymse_que(int sk, char *msg);
int cpri1_ala_que(int sk, char *msg);
int cpri1_logup_que(int sk, char *msg);
int cpri1_reset_ind(int sk, char *msg);
int cpri1_bbubeat_msg(int sk, char *msg, int *num);
int cpri1_lte_cfg(int sk, char *msg);
int cpri1_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri1_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri1_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri1_testype_handle(char *msg, unsigned short ie_id, int acq);
int cpri1_set_systime(const PC_SYSTIME *cl_systime);


#endif
