/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file builtins.h
 * @brief Declarations of builtin functions and constant members.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#ifndef _BUILTINS_INC
#define _BUILTINS_INC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>	// for perror

#include <sys/types.h>  // for readdir and opendir
#include <dirent.h>	// for readdir and opendir

#include <signal.h>	// for sigsend

#include "util.h"
#include "types.h"

//-- Constants to define how many builtins we have
#ifdef DEBUG
#define NUM_BUILTINS 25		// Total number of commands, including debug
#else
#define NUM_BUILTINS 19		// Total number of commands, excluding debug
#endif //DEBUG

short int is_builtin(char* command);

void (*BUILT_IN_FUNCS[NUM_BUILTINS])(kgenv*, int, char**);

//------------------------------------------------------------------------------
//-- The following are functions that define each built in command.  Each
//-- function takes a pointer to the shell's environment structure followed by
//-- an argc and argv passed on from the shell. Functions are named such that a 
//-- function bic_foo is run when the foo command is issued. 
//------------------------------------------------------------------------------
void bic_exit(kgenv* env, int argc, char* argv[]);

void bic_which(kgenv* env, int argc, char* argv[]);

void bic_where(kgenv* env, int argc, char* argv[]);

void bic_cd(kgenv* env, int argc, char* argv[]);

void bic_pwd(kgenv* env, int argc, char* argv[]);

void bic_list(kgenv* env, int argc, char* argv[]);

void bic_pid(kgenv* env, int argc, char* argv[]);

void bic_kill(kgenv* env, int argc, char* argv[]);

void bic_prompt(kgenv* env, int argc, char* argv[]);

void bic_printenv(kgenv* env, int argc, char* argv[]);

void bic_alias(kgenv* env, int argc, char* argv[]);

void bic_unalias(kgenv* env, int argc, char* argv[]);

void bic_history(kgenv* env, int argc, char* argv[]);

void bic_setenv(kgenv* env, int argc, char* argv[]);

void bic_lsbuiltins(kgenv* env, int argc, char* argv[]);

void bic_watchmail(kgenv* env, int argc, char* argv[]);

void bic_noclobber(kgenv* env, int argc, char* argv[]);

void bic_vimode(kgenv* env, int argc, char* argv[]);

void bic_emacsmode(kgenv* env, int argc, char* argv[]);

//------------------------------------------------------------------------------
//-- The following are functions associated with debugging commands and are
//-- intended for development use only.  Compile with -DDEBUG for use.
//------------------------------------------------------------------------------
#ifdef DEBUG
void _db_tokenizer(kgenv* env, int argc, char* argv[]);

void _db_kgenv(kgenv* env, int argc, char* argv[]);

void _db_path(kgenv* env, int argc, char* argv[]);

void _db_history(kgenv* env, int argc, char* argv[]);

void _db_wc_contains(kgenv* env, int argc, char* argv[]);

void _db_wc_expand(kgenv* env, int argc, char* argv[]);
#endif //DEBUG

#endif //_BUILTINS_INC
