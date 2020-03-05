BUILD_TOOL := /home/farsight/freescale/gcc-4.1.78-eglibc-2.5.78-dp-1/powerpc-linux-gnuspe/bin/powerpc-linux-gnuspe-
#BUILD_TOOL := 

CC := $(BUILD_TOOL)gcc
LD := $(BUILD_TOOL)ld
#CC := gcc
#LD := ld

INCLUDEDIR :=-I$(shell pwd)/include
LIBDIR :=-L/home/huizl/work_path/rru_obj/usr/lib
CPPFLAGS :=$(INCLUDEDIR)

objs := rru.o init.o thread/rru_thread.o thread/maintain.o thread/main_handle.o 
objs += thread/cpri1_thread.o thread/cpri1_handle.o thread/cpri2_thread.o thread/cpri2_handle.o 
objs += thread/cpri3_thread.o thread/cpri3_handle.o thread/cpri4_thread.o thread/cpri4_handle.o
objs += thread/cpri5_thread.o thread/cpri5_handle.o thread/cpri6_thread.o thread/cpri6_handle.o
objs += thread/cpri7_thread.o thread/cpri7_handle.o thread/cpri8_thread.o thread/cpri8_handle.o
objs += interface/ftp/ftp_client.o 
#objs += 5784api/lib5784api.a

rru : $(objs)
	$(CC) $(objs) $(CPPFLAGS) -lpthread -lm 5784api/lib5784api.a -o rru
	
#%.o : %.c
#	${CC} $(CPPFLAGS) -c -o $@ $<
#thread/%.o : thread/%.c
#	${CC} $(CPPFLAGS) -c -o $@ $<

SUBDIR = $(shell ls ./ -R|grep /)
SUBDIRS = $(subst :,/,$(SUBDIR))
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.o))
BAK = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*~))

clean:
	rm -rf rru $(BAK) $(SOURCE)
