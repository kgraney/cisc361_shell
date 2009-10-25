/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file ipc.c 
 * @brief Definitions of IPC functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-10-25
 */
#ifndef _IPC_INC
#define _IPC_INC

#include "types.h"
#include <string.h>

enum ipc_opcodes {
    IPC_ALL,         // The |&  operator
    IPC_STDOUT       // The | operator
};

bool contains_ipc(char* line);

enum ipc_opcodes parse_ipc_line(char** left, char** right, char* line);

void perform_ipc(char* left, char* right, enum ipc_opcodes ipc_type, 
        kgenv* global_env);

#endif //_IPC_INC
