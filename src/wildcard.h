
#ifndef _WILDCARD_H
#define _WILDCARD_H

#include <string.h>
#include <glob.h>
#include "types.h"

#define WILDCARD_CHARACTER	"*"
#define MAX_WILDCARDS		512

bool check_wildcard(char* pattern, char* string);

char* expand_wildcards(char* line, kgenv* env);

typedef struct {
    char* substring;
    int   type;
} wildcard_element;

enum {
    SINGLE,
    ALL
};


#endif //_WILDCARD_H
