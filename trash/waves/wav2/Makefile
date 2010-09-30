ifndef OSTYPE
OSTYPE = UNKNOWN
endif

INCPATH = /usr/local/include /usr/local/include/agar
LIBPATH = /usr/local/lib
LIBRARIES = GL GLU m png

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
CFLAGS  = -Wall $(patsubst %,-I%,$(INCPATH)) $(shell sdl-config --libs --cflags)
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = wav.c common.c util.c gl_util.c viewport.c m_util.c

all: wav

clean:
	rm -Rf *.o wav

depend:
	mkdep ${CFLAGS} ${SRCS}

run: wav
	./wav

wav: common.o util.o gl_util.o viewport.o wav.o m_util.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)

include .depend
