ifndef OSTYPE
OSTYPE = UNKNOWN
endif

INCPATH = /usr/local/include
LIBPATH = /usr/local/lib
LIBRARIES = GL GLU m png avutil avcodec

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

CC      = gcc
CFLAGS  = -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG -O3
endif

SRCS = sim.c common.c util.c gl_util.c viewport.c m_util.c capture.c

all: sim

clean:
	rm -Rf *.o sim

depend:
	mkdep ${CFLAGS} ${SRCS}

run: sim
	./sim

sim: common.o util.o gl_util.o viewport.o sim.o m_util.o capture.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)

include .depend
