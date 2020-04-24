ifeq ($(ARCH), ppc)
BUILD_TOOL := /home/farsight/freescale/gcc-4.1.78-eglibc-2.5.78-dp-1/powerpc-linux-gnuspe/bin/powerpc-linux-gnuspe-
else
BUILD_TOOL := 
endif

CC := $(BUILD_TOOL)gcc
LD := $(BUILD_TOOL)ld
#CC := gcc
#LD := ld

INCLUDEDIR :=-I$(shell pwd)/include
LIBDIR :=-L/home/huizl/work_path/rru_obj/usr/lib
CPPFLAGS :=$(INCLUDEDIR)

objs := main.o init.o thread/rru_thread.o thread/main_thread.o thread/main_handle.o 
objs += thread/cpri_thread.o thread/cpri_handle.o thread/idle_thread.o
objs += interface/ftp/ftp_client.o 
#objs += 5784api/lib5784api.a

rru : $(objs)
ifeq ($(ARCH), ppc)
	$(CC) $(objs) -DPPC $(CPPFLAGS) -lpthread -lm -l5784api -L5784api -o rru
else
	$(CC) $(objs) $(CPPFLAGS) -lpthread -lm -o rru
endif
	
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
