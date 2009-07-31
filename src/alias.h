#ifndef _ALIAS_INC
#define _ALIAS_INC

#include "types.h"

void add_alias(kgenv* env, char* name, int cmd_argc, char* cmd_argv[]);

void remove_alias(kgenv* env, char* name);

aliasList* is_alias(kgenv* env, char* name);

#endif //_ALIAS_INC
