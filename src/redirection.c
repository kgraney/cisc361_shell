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

const char* REDIRECT_OPERATORS[] = { ">>&", ">>", ">&", ">", "<" };    
const int NUM_REDIRECT_OPERATORS = 5;

enum redirect_opcodes parse_redirection(char** command, char** file, 
        char* line){

    char* rd_stdout = NULL;
    enum redirect_opcodes redirect_code;

    for(int i=0; i < NUM_REDIRECT_OPERATORS && rd_stdout == NULL; i++){
        rd_stdout = strstr(line, REDIRECT_OPERATORS[i]);
        redirect_code = i;
    }

    if(rd_stdout == NULL){
        return RD_NONE;
    }

    int command_length = (int)rd_stdout - (int)line;
    *command = (char*)malloc(command_length + 1);
    memcpy(*command, line, command_length);
    (*command)[command_length - 1] = '\0';
    
    int file_length = strlen(line) - (int)rd_stdout + (int)line;
    char* ptr = strtok(line + command_length, " >&<");
    *file = (char*)malloc(file_length); 
    memcpy(*file, ptr, strlen(ptr));

    return redirect_code;
}
