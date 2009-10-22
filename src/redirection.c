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

const char* REDIRECTION_STR[] = { "standard out and standard error, appending,",
    "standard out, appending,",
    "standard out and standard error",
    "standard out",
    "standard in" };


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

void perform_redirection(int* fid, char* redirect_file, 
        enum redirect_opcodes redirection_type){

    *fid = open(redirect_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    close(1);
    dup(*fid);
    close(*fid);


}

void reset_redirection(int* fid, enum redirect_opcodes redirection_type){
    if(redirection_type != RD_NONE && redirection_type != RD_STDIN){
        *fid = open("/dev/tty", O_WRONLY);
        close(1);
        dup(*fid);
        close(*fid);
    }
}
