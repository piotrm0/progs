#DEBUG = 1

ifndef OSTYPE
OSTYPE = UNKNOWN
#default:
#	env
endif

INCPATH = /usr/local/include /usr/local/include/agar /usr/local/include/SDL /usr/X11/include/freetype2 /usr/X11/include
LIBPATH = /usr/local/lib /usr/X11/lib
LIBRARIES = GL GLU m ag_gui ag_core SDLmain SDL jpeg pthread z freetype

ifeq ($(OSTYPE), FreeBSD)
	LIBRARIES += glut
	INCPATH += /usr/X11R6/include
	LIBPATH += /usr/X11R6/lib
else
	LIBPATH += /System/Library/Frameworks/OpenGL.framework/Libraries
	LIBPATH += /sw/lib
	INCPATH += /sw/include
	LIBRARIES += objc stdc++
	FRAMEWORK = -framework GLUT -framework CoreServices -framework ApplicationServices -framework Cocoa
	FRAMEWORK += -framework OpenGL
	CFLAGS  += -fnested-functions

endif

INCPATH += /Users/piotrm/progs/shared
LIBPATH += /Users/piotrm/progs/shared
LIBRARIES += piotrm

CC = gcc
CFLAGS  += -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

CFLAGS += -DHAVE_OPENGL

CFLAGS += -O3

SRCS = a.c cell.c
OBJS = a.o cell.o

all: a

clean:
	rm -Rf *.o a
	rm -Rf *~

depend:
	mkdep ${CFLAGS} ${SRCS}

run: a
	./a

a: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)
##      <compiler> <complier flags> <source> <target> <linker flags> <framework>

# these typically come from your "make depends", but you're not using that:
include .depend

##  Another note, if you use pmake, you can use these substitutions:
#	$(.ALLSRC)	for	$^
#	$(.TARGET)	for	$@
##  which makes more sense

