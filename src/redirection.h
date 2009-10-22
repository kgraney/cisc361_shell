/*
 * CISC361: Operating Systems (Fall 2009)
 * Instructor: Ben Miller
 *
 * Project 2
 * Kevin Graney
 */

#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char* REDIRECTION_STR[5];

enum redirect_opcodes {
    RD_ALL_APPEND,      // The >>& operator
    RD_STDOUT_APPEND,   // The >>  operator
    RD_ALL,             // The >&  operator
    RD_STDOUT,          // The >   operator
    RD_STDIN,           // The <   operator
    RD_NONE = -1        // No redirect operator
};

enum redirect_opcodes parse_redirection(char** command, char** file, 
        char* line);

void perform_redirection(int* fid, char* redirect_file, 
        enum redirect_opcodes redirection_type);

void reset_redirection(int* fid, enum redirect_opcodes redirection_type);

#endif //REDIRECTION_H
