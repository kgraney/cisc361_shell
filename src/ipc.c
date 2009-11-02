/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

/** 
 * @file ipc.c 
 * @brief Implementations of IPC functions.
 * @author Kevin Graney
 * @version v0.1
 * @date 2009-10-25
 */

#include "ipc.h"
#include "types.h"
#include "util.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char* IPC_OPERATORS[] = { "|&", "|" };    
const int NUM_IPC_OPERATORS = 5;

bool contains_ipc(char* line){
    return strstr(line, "|") || strstr(line, "|&");
}

enum ipc_opcodes parse_ipc_line(char** left, char** right, char* line){

    char* p = NULL;
    enum ipc_opcodes ipc_code;
    for(int i=0; i < NUM_IPC_OPERATORS && p == NULL; i++){
        p = strstr(line, IPC_OPERATORS[i]);
        ipc_code = i;
    }

    int left_length = (int)p - (int)line;
    *left = (char*)malloc(left_length + 1);
    memcpy(*left, line, left_length + 1);
    (*left)[left_length - 1] = '\0';

    char* ptr = strtok(line + left_length, "|&");
    int right_length = strlen(line) - (int)p + (int)line;
    *right = (char*)malloc(right_length + 1);
    memcpy(*right, ptr, right_length + 1);

    return ipc_code;
}

void perform_ipc(char* left, char* right, enum ipc_opcodes ipc_type, 
        kgenv* env){

    int fid;
    int filedes[2];     //<< Index 0 will be a dup of stdin and index 1 a dup of
                        //<< stdout/stderr

    if(pipe(filedes) == -1){
        perror("Error creating pipe");
        return;
    }

    // Redirect stdin
    close(0);
    dup(filedes[0]);
    close(filedes[0]);

    // Redirect stdout
    close(1);
    dup(filedes[1]);

    // Redirect stderr
    if(ipc_type == IPC_ALL){
        close(2);
        dup(filedes[1]);
    }

    close(filedes[1]);

    // Run the command on the left
    process_command_in(left, env, true, false);

    // Return stdout to terminal
    fid = open("/dev/tty", O_WRONLY);
    close(1);
    dup(fid);
    close(fid);

    // Return sterr to terminal
    fid = open("/dev/tty", O_WRONLY);
    close(2);
    dup(fid);
    close(fid);

    // Run the command on the right
    process_command_in(right, env, true, true);

    // Return stdin to terminal
    fid = open("/dev/tty", O_RDONLY);
    close(0);
    dup(fid);
    close(fid);
}
