CC=gcc
CFLAGS=-Wall
BIN=bin
LIBRARY=zRender
INCLUDE=-include src/precompiled/includes.h

all:
	$(CC) -fPIC -o $(BIN)/$(LIBRARY).so -DTEMPLATE_DIR=\"tmpl/\" -shared src/*.c $(INCLUDE)

test:
	$(CC) -g3 -o tests/tests.o $(CFLAGS) -DTEMPLATE_DIR=\"tests/\" src/*.c tests/tests.c $(INCLUDE)
	./tests/tests.o

clean:
	rm bin/*.so
	rm tests/*.o
