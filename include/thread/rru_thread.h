#ifndef rru_thread___h
#define rru_thread___h


#define CPRI_INF_TYPE 1
#define CPRI_STA_TYPE 2
#define CPRI_ALA_TYPE 3
#define CPRI_TES_TYPE 4

void *cpri1_thread(void);
void *cpri2_thread(void);
void *cpri3_thread(void);
void *cpri4_thread(void);
void *cpri5_thread(void);
void *cpri6_thread(void);
void *cpri7_thread(void);
void *cpri8_thread(void);
void *maintain_thread(void);
void *ptran_thread(void);

#endif
