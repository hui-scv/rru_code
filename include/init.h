#ifndef ___init_h_
#define ___init_h_


void driver_init(void);
void ad_da_init(void);
void time_init(void);
void rf_init(void);
int cprix_read_info(char *msg, int i, int cpir_num);
int cpri_read_info(char *msg, int i);
void read_cpirx(void);
void read_cpir(void);
int cprix_write_info(char *msg, int i, int cpri_num);
int cpri_write_info(char *msg, int i);
void softinfo_write(int type, char name[16], char verinfo[40]);
void firminfo_write(int type, char name[16], char verinfo[40]);
void exit_fun(void);
void init(void);

#endif
