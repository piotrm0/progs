DEBUG = 1

ifndef OSTYPE
  OSTYPE = apple
  # assume osx
endif

INCPATH = /usr/local/include
LIBPATH = /usr/local/lib

ifeq ($(OSTYPE), FreeBSD)
  INCPATH += /usr/X11R6/include
  LIBPATH += /usr/X11R6/lib
endif

ifeq ($(OSTYPE), apple)
  LIBPATH += /sw/lib
  LIBPATH += 
  INCPATH += /sw/include
  LIBRARIES += ObjC
  FRAMEWORK += Cocoa
  CFLAGS  += -fnested-functions -F/Library/Frameworks
  LDFLAGS += $(patsubst %,-framework %,$(FRAMEWORK)) 
endif

AR      = ar
RANLIB  = ranlib
CC      = gcc
CFLAGS  += -O3 -Wall $(patsubst %,-I%,$(INCPATH)) -D_THREAD_SAFE 
LDFLAGS += $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES)) 
FFLAGS  += 

ifdef DEBUG
  CFLAGS += -g -DDEBUG
endif

SRCS = main.c mroom.c util.c m_util.c serve.c

all: main

.PHONY: clean run debug depend backup

backup:
	cd ..; tar -cvf serve.tar serve; gzip serve.tar

clean:
	rm -Rf *.o *.a *~ main .DS_Store .gdb_history

depend:
	mkdep ${CFLAGS} ${SRCS}

run: main
	nice -n 20 ./main

debug:
	make clean
	make
	gdb main

COMMON_OBJECTS = util.o m_util.o list.o
SERVE_OBJECTS  = serve.o mroom.o

libcommon.a: $(COMMON_OBJECTS)
	$(AR) -cr libcommon.a $(COMMON_OBJECTS)
	$(RANLIB) libcommon.a

main: main.o $(SERVE_OBJECTS) libcommon.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

include .depend
