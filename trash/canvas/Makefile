DEBUG = 1

ifndef OSTYPE
OSTYPE = UNKNOWN
endif

INCPATH = /usr/local/include
LIBPATH = /usr/local/lib
LIBRARIES = GL GLU z m png avutil avcodec swscale xvidcore gd

ifeq ($(OSTYPE), FreeBSD)
LIBRARIES += glut
INCPATH += /usr/X11R6/include
LIBPATH += /usr/X11R6/lib
else
LIBPATH += /System/Library/Frameworks/OpenGL.framework/Libraries
LIBPATH += /sw/lib
INCPATH += /sw/include
LIBRARIES += objc stdc++
FRAMEWORK = -framework GLUT
FRAMEWORK += -framework OpenGL
endif

INCPATH += /Users/piotrm/progs/shared
LIBPATH += /Users/piotrm/progs/shared
LIBRARIES += piotrm

CC      = gcc
CFLAGS  = -O3 -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = main.c capture.c canvas.c

all: main

clean:
	rm -Rf *.o *~ main

depend:
	mkdep ${CFLAGS} ${SRCS}

run: main
	nice -n 20 ./main

main: main.o capture.o canvas.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)

include .depend
