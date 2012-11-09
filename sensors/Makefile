CXX := g++
CC  := gcc
AR  := ar

INCPATH := include
LIBPATH := lib

LIBS := GLUI SOIL

CFLAGS  += -Wall $(patsubst %,-I%,$(INCPATH)) -g
LDFLAGS := $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBS)) -framework OpenGL -framework GLUT -framework CoreFoundation

SRCS := test.cpp common.cpp glcommon.cpp ui.cpp
OBJS := test.o common.o glcommon.o ui.o

test: ${OBJS}
	$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@

#common.o: common.cpp common.h
#glcommon.o: glcommon.cpp glcommon.h
#ui.o: ui.cpp ui.h
#test.o: test.cpp test.h

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

run: test
	./test

clean:
	rm -Rf .depend
	rm -Rf *.o a
	rm -Rf *~

#FREEGLUT_SRCS := $(shell ls include/freeglut/*.c)
#FREEGLUT_SRCS := include/freeglut/freeglut_main.c
#FREEGLUT_OBJS := $(FREEGLUT_SRCS:.c=.o)

#%.o: %.c
#	$(CC) -DFREEGLUT_STATIC -c $^ -o $@

#freeglut: $(FREEGLUT_OBJS)

depend: .depend

.depend: ${SRCS}
	mkdep ${SRCS} ${CFLAGS}

include .depend
