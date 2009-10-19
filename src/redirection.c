/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "redirection.h"

const char* REDIRECT_STRS[] = { ">", ">&", ">>", ">>&", "<" };    

enum redirect_opcodes parse_redirection(char** command, char** file, 
        char* line){

    char* rd_stdout = strstr(line, ">");

    if(rd_stdout != NULL){
        int command_length = (int)rd_stdout - (int)line;
        *command = (char*)malloc(command_length + 1);
        memcpy(*command, line, command_length);
        (*command)[command_length - 1] = '\0';
        
        int file_length = strlen(line) - (int)rd_stdout + (int)line;
        char* ptr = strtok(line + file_length - 1, " >&<");
        *file = (char*)malloc(file_length); 
        memcpy(*file, ptr, strlen(ptr));

        return RD_STDOUT;
    }

    return RD_NONE;
}
