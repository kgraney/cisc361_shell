/** 
 * @file wildcard.h
 * @brief Declarations of wildcard functions and constant members.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#ifndef _WILDCARD_H
#define _WILDCARD_H

#include <string.h>
#include <glob.h>
#include "types.h"

#define MAX_WILDCARDS		512
#define WILDCARD_CHARS 		"*?"

bool contains_wildcards(char* line);

char* expand_wildcards(char* line);

char* expand_argument(char* argument);

#endif //_WILDCARD_H
