DEBUG_LEVEL=0
ifeq ($(DEBUG_LEVEL), 0) #We don't want any debug flag here
	DEBUG_FLAG = 
else
	DEBUG_FLAG = -g$(DEBUG_LEVEL)
endif 
CC = gcc
PYTHON = python

CFLAGS = -m64 -std=c99 -fpic -Wall $(DEBUG_FLAG) -D DEBUG_LEVEL=$(DEBUG_LEVEL)
LDFLAGS=
INCLUDE = -include src/precompiled/includes.h
SRC = Block.c Parameter.c Util.c zCore.c Regex.c
OBJ = bin/Block.o bin/Parameter.o bin/Util.o bin/zCore.o bin/Regex.o

ifeq ($(OS),Windows_NT)
    #Windows stuff
    LIBRARY=zTemplate.dll
    TESTS=tests/tests.exe
else
    #Linux stuff
    LIBRARY=zTemplate.so
    TESTS=tests/tests.o
endif

all: $(LIBRARY)

$(LIBRARY): $(OBJ)
	$(CC) -o bin/$(LIBRARY) $(LDFLAGS) -shared $(OBJ)

bin/%.o:
	$(CC) -o bin/$*.o $(CFLAGS) $(INCLUDE) -DTEMPLATE_DIR=\"tmpl/\" -c src/$*.c

rebuild: clean all

test:
	$(CC) -g3 $(CFLAGS) -o $(TESTS) -DTEMPLATE_DIR=\"tests/\" src/*.c tests/tests.c $(INCLUDE)
	echo 'Running C tests:'
	./$(TESTS)
	echo 'Running Pythonic tests:'
	$(PYTHON) tests/tests.py

clean:
	rm -rf bin/*.o