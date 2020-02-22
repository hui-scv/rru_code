CC := gcc

obj = rru.o init.o thread/rru_thread.o thread/maintain.o thread/main_handle.o thread/cpri1_thread.o thread/cpri1_handle.o thread/cpri2_thread.o thread/cpri2_handle.o thread/cpri3_thread.o thread/cpri3_handle.o thread/cpri4_thread.o thread/cpri4_handle.o thread/cpri5_thread.o thread/cpri5_handle.o thread/cpri6_thread.o thread/cpri6_handle.o thread/cpri7_thread.o thread/cpri7_handle.o thread/cpri8_thread.o thread/cpri8_handle.o

rru : $(obj)
	$(CC) $(obj) -static -lpthread -o rru

SUBDIR = $(shell ls ./ -R|grep /)
SUBDIRS = $(subst :,/,$(SUBDIR))
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.o))
BAK = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*~))

clean:
	rm -rf rru $(BAK) $(SOURCE)
