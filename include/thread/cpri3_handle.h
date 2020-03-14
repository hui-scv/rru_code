#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri3_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri3_comch_cfg(int sk, char *msg);
void cpri3_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri3_verdown_que(int sk, char *msg);
int cpri3_veract_ind(int sk, char *msg);
int cpri3_state_que(int sk, char *msg);
int cpri3_para_que(int sk, char *msg);
int cpri3_paracfg_que(int sk, char *msg);
int cpri3_delaymse_que(int sk, char *msg);
int cpri3_ala_que(int sk, char *msg);
int cpri3_logup_que(int sk, char *msg);
int cpri3_reset_ind(int sk, char *msg);
int cpri3_bbubeat_msg(int sk, char *msg, int *num);
int cpri3_lte_cfg(int sk, char *msg);
int cpri3_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri3_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri3_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri3_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
