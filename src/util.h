/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file util.h
 * @brief Definitions of utility functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-08-02
 */
#ifndef _UTIL_INC
#define _UTIL_INC

#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>	// for waitpid and WEXITSTATUS
#include <sys/types.h>  // for readdir and opendir
#include <dirent.h>	// for readdir and opendir

#include <string.h>

#include "types.h"
#include "get_path.h"


#define  O_VERBOSE_EXE	// Enable the "Executing ..." messages

#define CWD_BUFFER_SIZE 	1024
#define LINE_BUFFER_SIZE 	1024
#define MAX_TOKENS_PER_LINE 	512
#define HISTORY_SIZE 		1024


/** 
 * @brief The external environment variable list from the calling shell.
 */
extern char** environ;

char* which(const char *command, pathList* pathlist);

void add_to_history(char* command, kgenv* env);

int exec_cmd(char* cmd, char** argv);

int process_command_in(char* line_in, kgenv* global_env);

int parse_line(int* argc, char*** argv, char* line);

void detokenize(char* str, int length);

void set_environment(kgenv* env, char* name, char* value);


#endif //_UTIL_INC
