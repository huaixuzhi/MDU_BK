#
# c.cpp混合编译的makefile模板
#
#

BIN = ../bin/mdu
CC = gcc
CPP = g++
#这里只加入库头文件路径及库路径
INCS = $(shell pkg-config --cflags glib-2.0 gthread-2.0)
LIBS = $(shell pkg-config --libs glib-2.0 gthread-2.0)
LIBS += -L../lib -ltoplib
SUBDIRS =
#生成依赖信息时的搜索目录，比如到下列目录中搜索一个依赖文件(比如.h文件)
DEFINC = -I"../include"
#给INCS加上依赖搜索路径，分开写可能会产生不一致情况，而且繁琐
#
#
#maintest.c tree/rbtree.c  多了子目录，那就直接添加 目录/*.c即可   所有的源文件--  .c文件列表
CSRCS = $(wildcard  ./*.c)
CPPSRCS = $(wildcard ./*.cpp)
#
#
#所有的.o文件列表
COBJS := $(CSRCS:.c=.o)
CPPOBJS := $(CPPSRCS:.cpp=.o)

MAKEDEPEND = gcc -MM -MT
CFLAGS =
#CFLAGS += -Wall -ansi -DWIN32 -DNDEBUG -O2
CPPFLAGS =
#CPPFLAGS += -Wall -DWIN32 -DNDEBUG -O2
#-g 生成调试信息
#-pedantic参数与-ansi一起使用 会自动拒绝编译非ANSI程序
#-fomit-frame-pointer 去除函数框架
#-Wmissing-prototypes -Wstrict-prototypes 检查函数原型
#针对每个.c文件的.d依赖文件列表
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
#先删除依赖信息
#重新生成依赖信息
#这里出现了一个 $(MAKE) 没有定义的变量。这个变量是由 Make 自己定义的，它的值即为自己的位置，方便 Make 递归调用自己。
depend:
	-rm $(CDEF)
	-rm $(CPPDEF)
	$(MAKE) $(CDEF)
	$(MAKE) $(CPPDEF)
#$(OBJS):%.o :%.c  先用$(OBJS)中的一项，比如foo.o: %.o : %.c  含义为:试着用%.o匹配foo.o。如果成功%就等于foo。如果不成功，
# Make就会警告，然后。给foo.o添加依赖文件foo.c(用foo替换了%.c里的%)
# 也可以不要下面的这个生成规则，因为下面的 include $(DEF)  就隐含了。此处为了明了，易懂。故留着
$(COBJS) : %.o: %.c
	$(CC) -c $< -o $@ $(INCS) $(DEFINC) $(CFLAGS)
$(CPPOBJS) : %.o: %.cpp
	$(CPP) -c $< -o $@ $(INCS) $(DEFINC) $(CPPFLAGS)
# $@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。每次$< $@ 代表的值就是列表中的
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
#清除所有目标文件以及生成的最终目标文件
clean:
	-rm $(BIN) $(COBJS) $(CPPOBJS)
#rm *.d
cleanall:
	-rm $(BIN) $(COBJS) $(CPPOBJS)
