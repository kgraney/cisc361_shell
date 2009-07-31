#ifndef _UTIL_INC
#define _UTIL_INC

#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>	// for waitpid and WEXITSTATUS
#include <sys/types.h>  // for readdir and opendir
#include <dirent.h>	// for readdir and opendir

#include "types.h"
#include "get_path.h"


#define  O_VERBOSE_EXE	// Enable the "Executing ..." messages

#define CWD_BUFFER_SIZE 	1024
#define LINE_BUFFER_SIZE 	1024
#define MAX_TOKENS_PER_LINE 	512
#define HISTORY_SIZE 		1024


// Envrionment variables
extern char** environ;

// Returns the full path to an executable by searching the path directories.
char* which(const char *command, pathList* pathlist);

// Adds a command to the history stack
void add_to_history(char* command, kgenv* env);

// Executes a command as a child process
int exec_cmd(char* cmd, char** argv);

int process_command_in(char* line_in, kgenv* global_env);

int parse_line(int* argc, char*** argv, char* line);

void detokenize(char* str, int length);

void set_environment(kgenv* env, char* name, char* value);


#pragma returns_new_memory(malloc)
#pragma returns_new_memory(calloc)

#endif //_UTIL_INC
