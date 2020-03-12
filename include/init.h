#ifndef ___init_h_
#define ___init_h_


void driver_init(void);
void ad_da_init(void);
void time_init(void);
void rf_init(void);
int cpri_read_str(char *msg, int i);
int cpri_write_str(const char * msg, int i);
void init(void);

#endif
