DEBUG = 1

ifndef OSTYPE
OSTYPE = UNKNOWN
endif

INCPATH = /usr/local/include /usr/include/python2.3 /usr/bin
LIBPATH = /usr/local/lib
LIBRARIES = python

CC      = gcc
CFLAGS  = -O3 -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = main.c

all: main

clean:
	rm -Rf *.o *~ main

depend:
	mkdep ${CFLAGS} ${SRCS}

run: main
	nice -n 20 ./main

debug:
	make clean
	make
	gdb main

main: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sum: sum.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)