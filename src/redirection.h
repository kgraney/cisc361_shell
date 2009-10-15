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


enum redirect_opcodes {
    RD_STDOUT,          // The >   operator
    RD_ALL,             // The >&  operator
    RD_STDOUT_APPEND,   // The >>  operator
    RD_ALL_APPEND,      // The >>& operator
    RD_STDIN,           // The <   operator
    RD_NONE = -1        // No redirect operator
};

#endif //REDIRECTION_H
