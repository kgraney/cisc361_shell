/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

#include "redirection.h"

const char* REDIRECT_STRS[] = { ">", ">&", ">>", ">>&", "<" };    

enum redirect_opcodes parse_redirection(char** command, char** file, 
        char* line){

    char* rd_stdout = strstr(line, ">");

    if(rd_stdout != NULL){
        *command = line;
        *file = line;
        return RD_STDOUT;
    }

    return RD_NONE;
}
