obj = rru.o init.o thread/rru_thread.o thread/maintain.o
rru : $(obj)
	gcc $(obj) -static -lpthread -o rru

SUBDIR = $(shell ls ./ -R|grep /)
SUBDIRS = $(subst :,/,$(SUBDIR))
SOURCE = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*.o))
BAK = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)*~))
clean:
	#rm -rf *.o rru *~ thread/*.o thread/*~
	rm -rf rru $(BAK) $(SOURCE)
