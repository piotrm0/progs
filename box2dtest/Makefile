CXX = g++
AR  = ar

INCPATH = include
LIBPATH = lib

LIBS = Box2D GLUI

CFLAGS  += -Wall $(patsubst %,-I%,$(INCPATH))
LDFLAGS = $(patsubst %,-L%,$(LIBPATH)) $(patsubst %,-l%,$(LIBS)) -framework OpenGL -framework GLUT

SRCS = test.cpp Render.cpp
OBJS = test.o Render.o

test: ${OBJS}
	$(CXX) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@

run: test
	./test

clean:
	rm -Rf .depend
	rm -Rf *.o a
	rm -Rf *~

#depend: .depend

#.depend: ${SRCS}
#	mkdep ${CFLAGS} ${SRCS}

#include .depend
