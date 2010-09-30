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
  LIBPATH += /System/Library/Frameworks/OpenGL.framework/Libraries
  INCPATH += /sw/include /sw/include/SDL
  LIBRARIES += ObjC SDLmain SDL_ttf SDL_image GL
  FRAMEWORK += SDL SDL_ttf SDL_image Cocoa OpenGL
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

SRCS = util.c util.h m_util.c m_util.h gl_util.c gl_util.h igl.h viewport.c viewport.h geo.c geo.h

COMMON_OBJECTS = util.o m_util.o list.o gl_util.o viewport.o geo.o

libpiotrm.a: $(COMMON_OBJECTS)
	$(AR) -cr libpiotrm.a $(COMMON_OBJECTS)
	$(RANLIB) libpiotrm.a

.PHONY: clean run debug depend backup

clean:
	rm -Rf *.o *.a *~ .DS_Store .gdb_history

depend:
	mkdep ${CFLAGS} ${SRCS}

debug:
	make clean
	make
	gdb main

include .depend
