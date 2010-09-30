#DEBUG = 1

INCPATH = /usr/local/include
LIBPATH = /usr/local/lib
LIBRARIES = 

CC      = gcc
CFLAGS  = -Wall $(patsubst %,-I%,$(INCPATH)) -fnested-functions
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = main.c parse.c list.c dist.c util.c

all: main

clean:
	rm -Rf *.o *~ main 

depend:
	mkdep ${CFLAGS} ${SRCS}

run: main
	./main

main: main.o parse.o list.o dist.o util.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)

include .depend
