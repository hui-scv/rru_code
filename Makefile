obj = rru.o init.o thread/rru_thread.o thread/maintain.o thread/main_handle.o thread/cpri1_thread.o thread/cpri1_handle.o
rru : $(obj)
	gcc $(obj) -static -lpthread -o rru

SUBDIR = $(shell ls ./ -R|grep /)
SUBDIRS = $(subst :,/,$(SUBDIR))
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.o))
BAK = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*~))
clean:
	rm -rf rru $(BAK) $(SOURCE)