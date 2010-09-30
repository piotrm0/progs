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

SDL_CONF = $(shell sdl-config --cflags --libs)

AR      = ar
RANLIB  = ranlib
CC      = gcc
CFLAGS  += -O3 -Wall $(patsubst %,-I%,$(INCPATH)) -D_THREAD_SAFE $(SDL_CONF)
LDFLAGS += $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES)) 
FFLAGS  += 

ifdef DEBUG
  CFLAGS += -g -DDEBUG
endif

SRCS = main.c sg_main.c sg_timing.c sg_misc.c util.c m_util.c list.c \
 sg_element.c sg_element_root.c sg_element_window.c sg_element_label.c sg_element_pane.c sg_element_button.c \
 sg_event.c sg_mouse.c

all: main

.PHONY: clean run debug depend backup

backup:
	cd ..; tar -cvf sg.tar sg; gzip sg.tar

clean:
	rm -Rf *.o *.a *~ main .DS_Store .gdb_history fonts/.DS_Store fonts/work/.DS_Store fonts/work/*~

depend:
	mkdep ${CFLAGS} ${SRCS}

run: main
	nice -n 20 ./main

debug:
	make clean
	make
	gdb main

COMMON_OBJECTS = util.o m_util.o list.o
SG_OBJECTS     = sg_main.o sg_timing.o sg_type.o sg_image.o sg_misc.o \
		 sg_element.o sg_element_root.o sg_element_window.o \
	         sg_element_label.o sg_element_pane.o sg_element_button.o \
                 sg_event.o sg_mouse.o

libcommon.a: $(COMMON_OBJECTS)
	$(AR) -cr libcommon.a $(COMMON_OBJECTS)
	$(RANLIB) libcommon.a

libsg.a: $(SG_OBJECTS) util.o m_util.o list.o
	$(AR) -cr libsg.a $(SG_OBJECTS)
	$(RANLIB) libsg.a

.c:
	$(CC) $(CFLAGS) $^ -c 

main: main.o libsg.a libcommon.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

include .depend
