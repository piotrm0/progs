#DEBUG = 1

ifndef OSTYPE
  OSTYPE = apple
  # assume osx
endif

INCPATH = /usr/local/include
LIBPATH = /usr/local/lib

ifeq ($(OSTYPE), apple)
  CFLAGS  += -fnested-functions
  LDFLAGS += $(patsubst %,-framework %,$(FRAMEWORK)) 
endif

AR      = ar
RANLIB  = ranlib
CC      = gcc
#CFLAGS  += -O3 -Wall $(patsubst %,-I%,$(INCPATH)) -D_THREAD_SAFE 
CFLAGS  += -O3 -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS += $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES)) 
FFLAGS  += 

ifdef DEBUG
  CFLAGS += -g -DDEBUG
endif

SRCS = main.c util.c list.c col.c

all: main

.PHONY: clean run debug depend backup

backup:
	cd ..; tar -cvf ccol.tar sg; gzip ccol.tar

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

COMMON_OBJECTS = util.o list.o

libcommon.a: $(COMMON_OBJECTS)
	$(AR) -cr libcommon.a $(COMMON_OBJECTS)
	$(RANLIB) libcommon.a

main: main.o col.o libcommon.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

include .depend
