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
endif

CC = gcc
#CC      = g++
CFLAGS  = -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = a.c #common.c util.c gl_util.c viewport.c

all: a

clean:
	rm -Rf *.o a
	rm -Rf *~

depend:
	mkdep ${CFLAGS} ${SRCS}

run: a
	./a

a: a.o util.o m_util.o #common.o util.o gl_util.o viewport.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)
##      <compiler> <complier flags> <source> <target> <linker flags> <framework>

# these typically come from your "make depends", but you're not using that:
include .depend

##  Another note, if you use pmake, you can use these substitutions:
#	$(.ALLSRC)	for	$^
#	$(.TARGET)	for	$@
##  which makes more sense

