#
# c.cpp��ϱ����makefileģ��
#
#

BIN = ../bin/mdu
CC = gcc
CPP = g++
#����ֻ�����ͷ�ļ�·������·��
INCS = $(shell pkg-config --cflags glib-2.0 gthread-2.0)
LIBS = $(shell pkg-config --libs glib-2.0 gthread-2.0)
LIBS += -L../lib -ltoplib
SUBDIRS =
#����������Ϣʱ������Ŀ¼�����絽����Ŀ¼������һ�������ļ�(����.h�ļ�)
DEFINC = -I"../include"
#��INCS������������·�����ֿ�д���ܻ������һ����������ҷ���
#
#
#maintest.c tree/rbtree.c  ������Ŀ¼���Ǿ�ֱ������ Ŀ¼/*.c����   ���е�Դ�ļ�--  .c�ļ��б�
CSRCS = $(wildcard  ./*.c)
CPPSRCS = $(wildcard ./*.cpp)
#
#
#���е�.o�ļ��б�
COBJS := $(CSRCS:.c=.o)
CPPOBJS := $(CPPSRCS:.cpp=.o)

MAKEDEPEND = gcc -MM -MT
CFLAGS =
#CFLAGS += -Wall -ansi -DWIN32 -DNDEBUG -O2
CPPFLAGS =
#CPPFLAGS += -Wall -DWIN32 -DNDEBUG -O2
#-g ���ɵ�����Ϣ
#-pedantic������-ansiһ��ʹ�� ���Զ��ܾ������ANSI����
#-fomit-frame-pointer ȥ���������
#-Wmissing-prototypes -Wstrict-prototypes ��麯��ԭ��
#���ÿ��.c�ļ���.d�����ļ��б�
CDEF = $(CSRCS:.c=.d)
CPPDEF = $(CPPSRCS:.cpp=.d)
PLATS = win32-debug win32-release linux-debug linux-release
none:linux-debug
#	@echo "Please choose a platform:"
#	@echo " $(PLATS)"
win32-debug:
	$(MAKE) all INCS=-I"c:/mingw/include" LIBS="-L"c:/mingw/lib" -L"./../../lib/lxnet" -llxnet -lws2_32 -L"./../../lib/tinyxml" -ltinyxml" CFLAGS="-Wall -DWIN32 -DDEBUG -g" CPPFLAGS="-Wall -DWIN32 -DDEBUG -g"
win32-release:
	$(MAKE) all INCS=-I"c:/mingw/include" LIBS="-L"c:/mingw/lib" -L"./../../lib/lxnet" -llxnet -lws2_32 -L"./../../lib/tinyxml" -ltinyxml" CFLAGS="-Wall -DWIN32 -DNDEBUG -O2" CPPFLAGS="-Wall -DWIN32 -DNDEBUG -O2"
linux-debug:
	$(MAKE) all CFLAGS="-Wall -DDEBUG -g -O0" CPPFLAGS="-Wall -DDEBUG -g"
linux-release:
	$(MAKE) all INCS=-I"/usr/include" LIBS="-L"/usr/lib" -L"./../../lib/lxnet" -llxnet -lpthread -L"./../../lib/tinyxml" -ltinyxml" CFLAGS="-Wall -DNDEBUG -O2" CPPFLAGS="-Wall -DNDEBUG -O2"
all:$(BIN)

$(CDEF) : %.d : %.c
	$(MAKEDEPEND) $(<:.c=.o) $< $(DEFINC) > $@
$(CPPDEF) : %.d : %.cpp
	$(MAKEDEPEND) $(<:.cpp=.o) $< $(DEFINC) > $@
#��ɾ��������Ϣ
#��������������Ϣ
#���������һ�� $(MAKE) û�ж���ı���������������� Make �Լ�����ģ�����ֵ��Ϊ�Լ���λ�ã����� Make �ݹ�����Լ���
depend:
	-rm $(CDEF)
	-rm $(CPPDEF)
	$(MAKE) $(CDEF)
	$(MAKE) $(CPPDEF)
#$(OBJS):%.o :%.c  ����$(OBJS)�е�һ�����foo.o: %.o : %.c  ����Ϊ:������%.oƥ��foo.o������ɹ�%�͵���foo��������ɹ���
# Make�ͻᾯ�棬Ȼ�󡣸�foo.o���������ļ�foo.c(��foo�滻��%.c���%)
# Ҳ���Բ�Ҫ�����������ɹ�����Ϊ����� include $(DEF)  �������ˡ��˴�Ϊ�����ˣ��׶���������
$(COBJS) : %.o: %.c
	$(CC) -c $< -o $@ $(INCS) $(DEFINC) $(CFLAGS)
$(CPPOBJS) : %.o: %.cpp
	$(CPP) -c $< -o $@ $(INCS) $(DEFINC) $(CPPFLAGS)
# $@--Ŀ���ļ���$^--���е������ļ���$<--��һ�������ļ���ÿ��$< $@ ������ֵ�����б��е�
#
$(BIN) : $(COBJS) $(CPPOBJS)
	$(CPP) -o $(BIN) $(COBJS) $(CPPOBJS) $(LIBS)
	-rm $(COBJS) $(CPPOBJS)

#tar
install:build
build:
	rm -f ../topmdu.tgz
	tar -czf ../topmdu.tgz ../bin ../lib ../conf ../log 


.PHONY:clean cleanall
#�������Ŀ���ļ��Լ����ɵ�����Ŀ���ļ�
clean:
	-rm $(BIN) $(COBJS) $(CPPOBJS)
#rm *.d
cleanall:
	-rm $(BIN) $(COBJS) $(CPPOBJS)