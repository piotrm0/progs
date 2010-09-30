ifndef OSTYPE
OSTYPE = UNKNOWN
#default:
#	env
endif

INCPATH = /usr/local/include
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

#CC = gcc
CC      = g++
CFLAGS  = -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBRARIES))

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

SRCS = t1.c common.c board.c util.c gl_util.c viewport.c
#SRCS = t1.c common.c board.c util.c gl_util.c viewport.c

all: t1

clean:
	rm -Rf *.o t1

depend:
	mkdep ${CFLAGS} ${SRCS}

run: t1
	./t1
#t1: common.o board.o util.o gl_util.o viewport.o t1.o
t1: common.o board.o util.o gl_util.o viewport.o t1.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(FRAMEWORK)
##      <compiler> <complier flags> <source> <target> <linker flags> <framework>

# these typically come from your "make depends", but you're not using that:
#common.c t1.c: common.h
include .depend

##  Another note, if you use pmake, you can use these substitutions:
#	$(.ALLSRC)	for	$^
#	$(.TARGET)	for	$@
##  which makes more sense

