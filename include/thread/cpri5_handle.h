#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri5_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri5_comch_cfg(int sk, char *msg);
void cpri5_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri5_verdown_que(int sk, char *msg);
int cpri5_veract_ind(int sk, char *msg);
int cpri5_state_que(int sk, char *msg);
int cpri5_para_que(int sk, char *msg);
int cpri5_paracfg_que(int sk, char *msg);
int cpri5_delaymse_que(int sk, char *msg);
int cpri5_ala_que(int sk, char *msg);
int cpri5_logup_que(int sk, char *msg);
int cpri5_reset_ind(int sk, char *msg);
int cpri5_bbubeat_msg(int sk, char *msg, int *num);
int cpri5_lte_cfg(int sk, char *msg);
int cpri5_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri5_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri5_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri5_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
