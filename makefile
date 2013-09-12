CC=gcc
CFLAGS=-m64 -std=c99 -fpic -Wall
LDFLAGS=
INCLUDE=-include src/precompiled/includes.h
SRC = Block.c Parameter.c Util.c zCore.c
OBJ = bin/Block.o bin/Parameter.o bin/Util.o bin/zCore.o

ifeq ($(OS),Windows_NT)
    #Windows stuff
    LIBRARY=zRender.dll
    TESTS=tests/tests.exe
else
    #Linux stuff
    LIBRARY=zRender.so
    TESTS=tests/tests.o
endif

all: $(LIBRARY)

$(LIBRARY): $(OBJ)
	$(CC) -o bin/$(LIBRARY) $(LDFLAGS) -shared $(OBJ)

bin/%.o:
	$(CC) -g3 -o bin/$*.o $(CFLAGS) $(INCLUDE) -DTEMPLATE_DIR=\"tmpl/\" -c src/$*.c

rebuild: clean all

test:
	$(CC) -g3 $(CFLAGS) -o $(TESTS) -DTEMPLATE_DIR=\"tests/\" src/*.c tests/tests.c $(INCLUDE)
	./$(TESTS)

clean:
	rm -rf bin/*.o