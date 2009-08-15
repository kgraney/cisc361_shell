/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file alias.h
 * @brief Definition of alias functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#ifndef _ALIAS_INC
#define _ALIAS_INC

#include "types.h"

void add_alias(kgenv* env, char* name, int cmd_argc, char* cmd_argv[]);

bool remove_alias(kgenv* env, char* name);

aliasList* is_alias(kgenv* env, char* name);

#endif //_ALIAS_INC
