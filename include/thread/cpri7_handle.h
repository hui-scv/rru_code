#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri7_comch_req(int sk, BBU_HEAD cpri_ans);
int cpri7_comch_cfg(int sk, char *msg);
void cpri7_comch_init(int sk, char *msg, MSG_HEAD *msg_head, BBU_HEAD cpri_ans);
int cpri7_verdown_que(int sk, char *msg);
int cpri7_veract_ind(int sk, char *msg);
int cpri7_state_que(int sk, char *msg);
int cpri7_para_que(int sk, char *msg);
int cpri7_paracfg_que(int sk, char *msg);
int cpri7_delaymse_que(int sk, char *msg);
int cpri7_ala_que(int sk, char *msg);
int cpri7_logup_que(int sk, char *msg);
int cpri7_reset_ind(int sk, char *msg);
int cpri7_bbubeat_msg(int sk, char *msg, int *num);
int cpri7_lte_cfg(int sk, char *msg);
int cpri7_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri7_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri7_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri7_testype_handle(char *msg, unsigned short ie_id, int acq);

#endif
