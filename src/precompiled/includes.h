#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define zString char*

//Tags
#define START_TAG "<<"
#define END_TAG ">>"

#define BLOCK_ENTRY "[["
#define BLOCK_EXIT "]]"
#define BLOCK_END "[[end]]"

#define DEBUG(level, fmt, ...) \
	if(DEBUG_LEVEL >= level) \
		fprintf(stderr, fmt, __VA_ARGS__)

