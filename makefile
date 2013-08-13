CC=gcc
CFLAGS=-Wall
BIN=bin
INCLUDE=-include src/precompiled/includes.h
SYSTEM=-m64

ifeq ($(OS),Windows_NT)
    #Windows stuff
    LIBRARY=zRender.dll
    TESTS=tests/tests.exe
else
    #Linux stuff
    LIBRARY=zRender.so
    TESTS=tests/tests.o
endif

all: compile
	
compile:
	$(CC) -fPIC $(SYSTEM) -o $(BIN)/$(LIBRARY) -DTEMPLATE_DIR=\"tmpl/\" -shared src/*.c $(INCLUDE)

test:
	$(CC) -g3 -fPIC $(SYSTEM) -o $(TESTS) $(CFLAGS) -DTEMPLATE_DIR=\"tests/\" src/*.c tests/tests.c $(INCLUDE)
	./$(TESTS)

clean:
	rm bin/*.so
	rm tests/*.o
