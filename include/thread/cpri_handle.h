#ifndef ___main_handle_h__
#define ___main_handle_h__

#include "../struct.h"

int cpri_comch_req(const int sk, const BBU_HEAD cpri_ans, const int cpri_num);
int cpri_comch_cfg(const int sk, char *msg, const int cpri_num);
void cpri_comch_init(const int sk, char *msg, const BBU_HEAD cpri_ans, const int cpri_num);
int cpri_verdown_que(const int sk, char *msg, const int cpri_num);
int cpri_veract_ind(const int sk, char *msg, const int cpri_num);
int cpri_state_que(const int sk, char *msg, const int cpri_num);
int cpri_para_que(const int sk, char *msg, const int cpri_num);
int cpri_paracfg_que(const int sk, char *msg, const int cpri_num);
int cpri_delaymse_que(const int sk, char *msg, const int cpri_num);
int cpri_ala_que(const int sk, char *msg, const int cpri_num);
int cpri_logup_que(const int sk, char *msg, const int cpri_num);
int cpri_reset_ind(const int sk, char *msg, const int cpri_num);
int cpri_bbubeat_msg(const int sk, char *msg, int *num, const int cpri_num);
int cpri_lte_cfg(const int sk, char *msg, const int cpri_num);
int cpri_inftype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_statype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_alatype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_testype_handle(char *msg, unsigned short ie_id, int acq);
int cpri_set_systime(const PC_SYSTIME *cl_systime);


#endif
